#include "AudioBuffer.h"
#include <unistd.h>

// synchronize using a QSemaphore!

AudioBuffer::AudioBuffer() {
    mBuffer.resize(1024*128);
    mBufferLength = 0;
}

bool AudioBuffer::add(QByteArray &inArray) {
    int i = 0;
    while (needSpace(inArray.size()) == false) {
	usleep(100);

	// timeout ...
	if (i++ == 2000) {
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

