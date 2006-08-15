#include "AudioStorage.h"
#include "AudioBuffer.h"
#include <QWaitCondition>
#include <QThread>

#ifdef WIN32
#include <windows.h>
static void usleep(int usecs) {
    Sleep((usecs/1000) + 1);
}
#else
#include <unistd.h>
#endif

// synchronize using a QSemaphore!

AudioStorage::AudioStorage() {
    mBuffer.resize(1024*128);
    mBufferLength = 0;
    mBufferGetCondition = NULL;
}

bool AudioStorage::add(AudioBuffer *inAudioBuffer) {
    quint32 len;
    QByteArray *buffer = inAudioBuffer->convertedChunkBuffer(len);
    return add(*buffer, len);
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
    memcpy(mBuffer.data() + mBufferLength, inArray.data(), inLen);
    mBufferLength += inLen;
    //qDebug("storage: added %d bytes", len);
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
    outArray = mBuffer.left(outArray.size());
    memmove(mBuffer.data(), mBuffer.data() + outArray.size(), mBufferLength - outArray.size());
    mBufferLength -= outArray.size();

    if (mBufferGetCondition) {
	mBufferGetCondition->wakeAll();
	mBufferGetCondition = NULL;
    }
    mMutex.unlock();
    return true;
}

void AudioStorage::clear() {
    QMutexLocker locker(&mMutex);

    mBufferLength = 0;
}

quint32 AudioStorage::bufferLength() {
    return mBufferLength;
}

bool AudioStorage::needSpace(quint32 inSpace) {
    QMutexLocker locker(&mMutex);
    if (mBufferLength + inSpace > (quint32)mBuffer.size())
	return false;
    return true;
}

bool AudioStorage::needData(quint32 inData) {
    QMutexLocker locker(&mMutex);
    if (mBufferLength < inData)
	return false;
    return true;
}

void AudioStorage::wakeOnBufferGet(QWaitCondition *inCondition) {
    QMutexLocker locker(&mMutex);
    mBufferGetCondition = inCondition;
}

