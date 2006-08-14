#include "AudioBuffer.h"
#include <unistd.h>

// synchronize using a QSemaphore!

AudioBuffer::AudioBuffer() {
    mBuffer.resize(1024*128);
    mBufferLength = 0;
}

bool AudioBuffer::add(QByteArray &inArray) {
    mMutex.lock();

    int i = 0;
    while (mBufferLength + inArray.size() > mBuffer.size()) {
	mMutex.unlock();
	usleep(100);

	// timeout ...
	if (i++ == 20000) {
	    qDebug("Buffer full.");
	    return false;
	}

	mMutex.lock();
    }

    memcpy(mBuffer.data() + mBufferLength, inArray.data(), inArray.size());
    mBufferLength += inArray.size();
    mMutex.unlock();
}

bool AudioBuffer::get(QByteArray &outArray) {
    mMutex.lock();

    int i=0;
    if (mBufferLength < outArray.size()) {
	qWarning("Buffer empty .. waiting ..");

	while ((((mBuffer.size()/100)*mBufferLength) < 30) || mBufferLength < outArray.size()) {
	    mMutex.unlock();
	    usleep(100);

	    // timeout ...
	    if (i++ == 2000) {
		qDebug("Buffer empty.");
		return false;
	    }

	    mMutex.lock();
	}
    }

    outArray = mBuffer.left(outArray.size());
    memmove(mBuffer.data(), mBuffer.data() + outArray.size(), mBufferLength - outArray.size());
    mBufferLength -= outArray.size();

    mMutex.unlock();
}

void AudioBuffer::clear() {
    QMutexLocker locker(&mMutex);

    mBufferLength = 0;
}
