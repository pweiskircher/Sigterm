#include "AudioBuffer.h"
#include <QWaitCondition>
#include <QThread>

// synchronize using a QSemaphore!

AudioBuffer::AudioBuffer() {
    mBuffer.resize(1024*128);
    mBufferLength = 0;
    mBufferGetCondition = NULL;
}

bool AudioBuffer::add(QByteArray &inArray) {
    int i = 0;
    while (needSpace(inArray.size()) == false) {
	uglyHackSleep(10);

	// timeout ...
	if (i++ == 200) {
	    qDebug("Buffer full.");
	    return false;
	}
    }

    mMutex.lock();
    memcpy(mBuffer.data() + mBufferLength, inArray.data(), inArray.size());
    mBufferLength += inArray.size();
    mMutex.unlock();
}

bool AudioBuffer::get(QByteArray &outArray) {
    int i=0;
    if (needData(outArray.size()) == false) {
	qWarning("Buffer empty .. waiting ..");

	while (needData(outArray.size()) == false) {
	    uglyHackSleep(10);

	    // timeout ...
	    if (i++ == 200) {
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
}

void AudioBuffer::clear() {
    QMutexLocker locker(&mMutex);

    mBufferLength = 0;
}

bool AudioBuffer::needSpace(quint32 inSpace) {
    QMutexLocker locker(&mMutex);
    if (mBufferLength + inSpace > mBuffer.size())
	return false;
    return true;
}

bool AudioBuffer::needData(quint32 inData) {
    QMutexLocker locker(&mMutex);
    if (mBufferLength < inData)
	return false;
    return true;
}

void AudioBuffer::wakeOnBufferGet(QWaitCondition *inCondition) {
    QMutexLocker locker(&mMutex);
    mBufferGetCondition = inCondition;
}

void AudioBuffer::uglyHackSleep(int inMilliSeconds) {
    mUglyHackMutex.lock();
    mUglyHackWaitCondition.wait(&mUglyHackMutex, inMilliSeconds);
    mUglyHackMutex.unlock();
}

