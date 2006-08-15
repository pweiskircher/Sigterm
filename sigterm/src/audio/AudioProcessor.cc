#include "AudioProcessor.h"
#include "AudioManager.h"
#include "AudioFile.h"
#include "AudioDecoder.h"
#include "PlayList.h"

AudioProcessor::AudioProcessor(AudioManager *inAudioManager) {
    mAudioManager = inAudioManager;
    mPause = false;
    mSkipTrack = false;
}

void AudioProcessor::run() {
    mMutex.lock();
    while (mAudioManager->audioProcessorWaitCondition()->wait(&mMutex)) {
	mMutex.unlock();
	while (1) {
	    mMutex.lock();
	    if (mPause) {
		mPause = false;
		emit paused();
		mMutex.unlock();
		break;
	    }
	    mMutex.unlock();

	    PlayList *playList = mAudioManager->currentPlayList();
	    AudioFile *file = playList->currentFile();
	    if (!file) {
		emit paused();
		break;
	    }

	    processFile(playList, file);
	}
	mMutex.lock();
    }
}

void AudioProcessor::pause() {
    QMutexLocker locker(&mMutex);
    mPause = true;
}

void AudioProcessor::skipTrack() {
    QMutexLocker locker(&mMutex);
    mSkipTrack = true;
}

void AudioProcessor::processFile(PlayList *inPlayList, AudioFile *inFile) {
    AudioDecoder::DecodingStatus status;
    QByteArray audioData;
    audioData.resize(4096);

    emit startedPlaying(inFile);
    while (1) {
	mMutex.lock();
	if (mSkipTrack) {
	    mSkipTrack = false;
	    mAudioManager->audioStorage()->clear();
	    mMutex.unlock();
	    return;
	} else if (mPause) {
	    mMutex.unlock();
	    return;
	}
	mMutex.unlock();

	status = inFile->decoder()->getAudioChunk(audioData);
	if (status == AudioDecoder::eError) {
	    qDebug("Error on decoding ...");
	    continue;
	} else if (status == AudioDecoder::eEOF) {
	    qDebug("Finished decoding ...");
	    inPlayList->finished(inFile);
	    break;
	}

	while (mAudioManager->audioStorage()->needSpace(audioData.size()) == false) {
	    mMutex.lock();
	    mAudioManager->audioStorage()->wakeOnBufferGet(mAudioManager->audioProcessorWaitCondition());
	    mAudioManager->audioProcessorWaitCondition()->wait(&mMutex);

	    if (mSkipTrack) {
		mSkipTrack = false;
		mAudioManager->audioStorage()->clear();
		inFile->decoder()->close();
		mMutex.unlock();
		return;
	    } else if (mPause) {
		mMutex.unlock();
		return;
	    }
	    mMutex.unlock();
	}
	mAudioManager->audioStorage()->add(audioData);
    }
}
