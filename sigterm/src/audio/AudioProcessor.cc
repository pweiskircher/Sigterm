#include "AudioProcessor.h"
#include "AudioManager.h"
#include "AudioFile.h"
#include "AudioDecoder.h"
#include "PlayList.h"
#include "AudioBuffer.h"

AudioProcessor::AudioProcessor(AudioManager *inAudioManager) {
    mAudioManager = inAudioManager;
    mPause = false;
    mSkipTrack = false;
    mQuit = false;
}

void AudioProcessor::run() {
    mMutex.lock();
    while (mAudioManager->audioProcessorWaitCondition()->wait(&mMutex)) {
	mMutex.unlock();
	while (1) {
	    mMutex.lock();
	    if (mQuit) {
		mMutex.unlock();
		return;
	    }
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

void AudioProcessor::quit() {
    QMutexLocker locker(&mMutex);
    mQuit = true;
}

void AudioProcessor::processFile(PlayList *inPlayList, AudioFile *inFile) {
    AudioDecoder::DecodingStatus status;
    AudioBuffer buffer(4096);

    bool done = false;
    emit startedPlaying(inFile);
    while (!done) {
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

	status = inFile->decoder()->getAudioChunk(&buffer);
	if (status == AudioDecoder::eError) {
	    qDebug("Error on decoding ...");
	    buffer.reset();
	    continue;
	} else if (status == AudioDecoder::eEOF) {
	    qDebug("Finished decoding ...");
	    inPlayList->finished(inFile);
	    done = true;
	}

	quint32 convertedLen;
	QByteArray *convertedChunk = buffer.convertedChunkBuffer(convertedLen);
	if (!convertedChunk) {
	    qDebug("no converted chunk buffer :(");
	    buffer.reset();
	    continue;
	}

	while (mAudioManager->audioStorage()->needSpace(convertedLen) == false) {
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
	mAudioManager->audioStorage()->add(&buffer);
	buffer.reset();
    }
}
