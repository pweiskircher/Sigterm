#include "AudioStorage.h"
#include "AudioBuffer.h"
#include "AudioFile.h"

#include <QWaitCondition>
#include <QThread>
#include <QDebug>

#ifdef WIN32
#include <windows.h>
static void usleep(int usecs) {
	Sleep((usecs/1000) + 1);
}
#else
#include <unistd.h>
#endif

// synchronize using a QSemaphore!

#define CHUNK_SIZE 4096

AudioStorage::AudioStorage(quint32 inBufferSize) {
	quint32 roundedLen = (inBufferSize / CHUNK_SIZE) * CHUNK_SIZE;
	mBuffer = new quint8[roundedLen];

	mBufferSize = roundedLen;

	mPartialUsedBuffer = NULL;
	mPartialFreeBuffer = NULL;

	for (unsigned int i=0; i < roundedLen; i += CHUNK_SIZE) {
		mFreeBufferList.enqueue(mBuffer + i);
	}

	mBufferGetCondition = NULL;
}

AudioStorage::~AudioStorage() {
	delete mBuffer;
}

bool AudioStorage::add(AudioBuffer *inAudioBuffer) {
	quint32 len;
	QByteArray *buffer = inAudioBuffer->convertedChunkBuffer(len);
	if (!add(*buffer, len))
		return false;

	QMutexLocker locker(&mMutex);
	inAudioBuffer->audioFile()->bytesAddedToAudioStorage(len);
	if (mAudioFileQueue.empty())
		mAudioFileQueue.enqueue(inAudioBuffer->audioFile());
	else {
		if (mAudioFileQueue.last() != inAudioBuffer->audioFile())
			mAudioFileQueue.enqueue(inAudioBuffer->audioFile());
	}

	return true;
}

bool AudioStorage::add(QByteArray &inArray, quint32 inLen) {
	int i = 0;
	while (needSpace(inLen) == false) {
		usleep(100);

		// timeout ...
		if (i++ == 2000) {
			qDebug("Buffer full.");
			return false;
		}
	}

	mMutex.lock();
	for (unsigned int i=0; i < inLen; i += CHUNK_SIZE) {
		quint8 *fb = mFreeBufferList.head();
		quint32 len;

		if (mPartialFreeBuffer) {
			len = mPartialFreeBuffer - fb;
			fb = mPartialFreeBuffer;
			mPartialFreeBuffer = NULL;
		} else {
			len = CHUNK_SIZE;
		}

		if (len + i > inLen) {
			len = inLen - i;
			mPartialFreeBuffer = fb + len;
		} else {
			mUsedBufferList.enqueue(mFreeBufferList.dequeue());
		}

		memcpy(fb, inArray.data() + i, len);
	}
	mMutex.unlock();
	return true;

}

bool AudioStorage::get(QByteArray &outArray) {
	int i=0;
	if (needData(outArray.size()) == false) {
		qWarning("Buffer empty .. waiting ..");

		while (needData(outArray.size()) == false) {
			usleep(100);

			// timeout ...
			if (i++ == 2000) {
				qDebug("Buffer empty.");
				return false;
			}
		}
	}

	mMutex.lock();
	for (int i = 0; i < outArray.size(); i += CHUNK_SIZE) {
		quint8 *fb = mUsedBufferList.head();
		quint32 len;

		if (mPartialUsedBuffer) {
			len = mPartialUsedBuffer - fb;
			fb = mPartialUsedBuffer;
			mPartialUsedBuffer = NULL;
		} else {
			len = CHUNK_SIZE;
		}

		if (i + len > (quint32)outArray.size()) {
			len = outArray.size() - i;
			mPartialUsedBuffer = fb + len;
		} else {
			mFreeBufferList.enqueue(mUsedBufferList.dequeue());
		}

		memcpy(outArray.data() + i, fb, len);
	}

	if (mBufferGetCondition) {
		mBufferGetCondition->wakeAll();
		mBufferGetCondition = NULL;
	}

	quint32 bytesRemoved = outArray.size();
	while (mAudioFileQueue.empty() == false) {
		quint32 bytes = mAudioFileQueue.head()->bytesInAudioStorage();
		if (bytes <= bytesRemoved) {
			mAudioFileQueue.head()->bytesRemovedFromAudioStorage(bytes);
			mAudioFileQueue.dequeue();
			bytesRemoved -= bytes;
		} else {
			mAudioFileQueue.head()->bytesRemovedFromAudioStorage(bytesRemoved);
			break;
		}
	}

	mMutex.unlock();
	return true;
}

void AudioStorage::clear() {
	QMutexLocker locker(&mMutex);

	mFreeBufferList += mUsedBufferList;
	mUsedBufferList.clear();

	while (mAudioFileQueue.empty() == false) {
		quint32 bytes = mAudioFileQueue.head()->bytesInAudioStorage();
		mAudioFileQueue.head()->bytesRemovedFromAudioStorage(bytes);
		mAudioFileQueue.dequeue();
	}

	if (mBufferGetCondition) {
		mBufferGetCondition->wakeAll();
		mBufferGetCondition = NULL;
	}
}

quint32 AudioStorage::bufferLength() {
	quint32 len = 0;

	if (mPartialUsedBuffer)
		len += mPartialUsedBuffer - mUsedBufferList.head();
	else
		len += CHUNK_SIZE;

	len += CHUNK_SIZE * (mUsedBufferList.size() - 1);

	return len;
}

bool AudioStorage::needSpace(quint32 inSpace) {
	QMutexLocker locker(&mMutex);
	if (inSpace > (mBufferSize - bufferLength()))
		return false;
	return true;
}

bool AudioStorage::needData(quint32 inData) {
	QMutexLocker locker(&mMutex);
	if (bufferLength() < inData)
		return false;
	return true;
}

void AudioStorage::wakeOnBufferGet(QWaitCondition *inCondition) {
	QMutexLocker locker(&mMutex);
	mBufferGetCondition = inCondition;
}

