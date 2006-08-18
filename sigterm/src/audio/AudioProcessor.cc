#include "AudioProcessor.h"
#include "AudioManager.h"
#include "AudioFile.h"
#include "AudioDecoder.h"
#include "PlayQueue.h"
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

			PlayQueue *playQueue = mAudioManager->playQueue();
			AudioFile *file = playQueue->currentFile();
			if (!file) {
				emit paused();
				break;
			}

			file->setIsDecoding(true);
			processFile(playQueue, file);
			file->setIsDecoding(false);
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

void AudioProcessor::processFile(PlayQueue *inPlayQueue, AudioFile *inFile) {
	AudioDecoder::DecodingStatus status;
	AudioBuffer buffer(4096);
	buffer.setAudioFile(inFile);

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

		buffer.reset();
		if (!inFile->decoder()) {
			qDebug("Skipping track ...");
			inPlayQueue->finished(inFile);
			done = true;
			continue;
		}

		status = inFile->decoder()->getAudioChunk(&buffer);
		if (status == AudioDecoder::eStop) {
			qDebug("Skipping track ...");
			inPlayQueue->finished(inFile);
			done = true;
		}

		quint32 convertedLen;
		QByteArray *convertedChunk = buffer.convertedChunkBuffer(convertedLen);
		if (!convertedChunk) {
			qDebug("no converted chunk buffer :(");
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
	}
}
