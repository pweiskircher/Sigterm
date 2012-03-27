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
	if (roundedLen < inBufferSize) roundedLen += CHUNK_SIZE;

	mBuffer = new quint8[roundedLen];

	mBufferSize = roundedLen;

	for (unsigned int i=0; i < roundedLen; i += CHUNK_SIZE) {
		mFreeBufferList.enqueue(mBuffer + i);
	}

	mPartialBuffer = NULL;
	mPartialBufferLength = 0;

	mGetPartialBuffer = NULL;
	mGetPartialBufferLength = 0;

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
	int j = 0;
	while (needSpace(inLen) == false) {
		usleep(100);

		// timeout ...
		if (j++ == 2000) {
			qDebug("Buffer full.");
			return false;
		}
	}

	mMutex.lock();
	unsigned int i=0;
	while (i < inLen) {
		quint8 *fb;
		quint32 len;
		bool enqueue = true;

		if (mPartialBuffer) {
			len = CHUNK_SIZE - mPartialBufferLength;

			enqueue = false;

			fb = mPartialBuffer + mPartialBufferLength;

			if (i + len > inLen) {
				len = inLen - i;
				mPartialBufferLength += len;
			} else {
				mUsedBufferList.enqueue(mPartialBuffer);
				mPartialBuffer = NULL;
				mPartialBufferLength = 0;
			}
		} else {
			len = CHUNK_SIZE;
			fb = mFreeBufferList.dequeue();

			if (i + len > inLen) {
				len = inLen - i;
				mPartialBuffer = fb;
				mPartialBufferLength = len;
				enqueue = false;
			}
		}

		memcpy(fb, inArray.data() + i, len);

		if (enqueue)
			mUsedBufferList.enqueue(fb);

		i += len;
	}
	mMutex.unlock();
	return true;

}

bool AudioStorage::get(QByteArray &outArray) {
	int j=0;
	if (needData(outArray.size()) == false) {
		qWarning("Buffer empty .. waiting ..");

		while (needData(outArray.size()) == false) {
			usleep(100);

			// timeout ...
			if (j++ == 2000) {
				qDebug("Buffer empty.");
				return false;
			}
		}
	}

	mMutex.lock();
	int i = 0;
	while (i < outArray.size()) {
		quint8 *fb;
		quint32 len;
		bool isGetPartial = false;
		bool enqueue = true;

		if (mPartialBuffer && mUsedBufferList.size() == 0) {
			len = mPartialBufferLength;
			fb = mPartialBuffer;

			if (len + i > (quint32)outArray.size()) {
				qDebug("NOT TESTED YET - IF YOU SEE THIS MESSAGE AND AUDIO SOUNDS WRONG, TELL ME!");
				len = outArray.size() - i;
				mPartialBufferLength += len;
				enqueue = false;
			} else {
				mPartialBuffer = NULL;
				mPartialBufferLength = 0;
			}
		} else if (mGetPartialBuffer) {
			fb = mGetPartialBuffer;
			len = mGetPartialBufferLength;
			mGetPartialBuffer = NULL;
			mGetPartialBufferLength = 0;

			if (len + i > (quint32)outArray.size()) {
				quint32 tmp = len;
				len = outArray.size() - i;
				mGetPartialBuffer = fb;
				mGetPartialBufferLength = tmp - len;
				isGetPartial = true;
			}
		} else {
			len = CHUNK_SIZE;
			fb = mUsedBufferList.dequeue();

			if (len + i > (quint32)outArray.size()) {
				len = outArray.size() - i;

				mGetPartialBuffer = fb;
				mGetPartialBufferLength = CHUNK_SIZE - len;
				isGetPartial = true;
			}
		}

		memcpy(outArray.data() + i, fb, len);

		if (!isGetPartial) {
			if (enqueue) {
				mFreeBufferList.enqueue(fb);
			}
		} else {
			memmove(fb, fb + len, CHUNK_SIZE - len);
		}

		i += len;
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
	QMutexLocker locker(&mMutex);
	quint32 len;

	if (mUsedBufferList.size() == 0)
		return 0;

	len = CHUNK_SIZE * mUsedBufferList.size();
	len += mPartialBufferLength;
	len += mGetPartialBufferLength;

	return len;
}

bool AudioStorage::needSpace(quint32 inSpace) {
	quint32 len;

	len = mFreeBufferList.size() * CHUNK_SIZE;
	if (mPartialBufferLength)
		len += CHUNK_SIZE - mPartialBufferLength;

	if (inSpace > len)
		return false;
	return true;
}

bool AudioStorage::needData(quint32 inData) {
	if (bufferLength() < inData)
		return false;
	return true;
}

void AudioStorage::wakeOnBufferGet(QWaitCondition *inCondition) {
	QMutexLocker locker(&mMutex);
	mBufferGetCondition = inCondition;
}

void AudioStorage::dump() {
	for (unsigned int i = 0; i < mBufferSize; i++)
		printf("%02x", mBuffer[i]);
	printf("\n");
}
