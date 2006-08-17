#include "AudioBuffer.h"

AudioBuffer::AudioBuffer(quint32 inRequestedLength) {
	mRequestedLength = inRequestedLength;
}

AudioBuffer::AudioBufferState AudioBuffer::state() {
	return mState;
}

QByteArray &AudioBuffer::byteBuffer() {
	return mByteBuffer;
}


bool AudioBuffer::prepareForConversion(SDL_AudioCVT *inOutCVT) {
	if (state() != eGotDecodedChunk) {
		qDebug("Wrong AudioBuffer state.");
		return false;
	}

	if (inOutCVT->needed) {
		inOutCVT->len = mDecodedChunkLength;
		mByteBuffer.resize(mDecodedChunkLength * inOutCVT->len_mult);
		inOutCVT->buf = (Uint8*)mByteBuffer.data();
	}

	return true;
}

bool AudioBuffer::prepareForDecoding() {
	mByteBuffer.resize(mRequestedLength);
	return true;
}


void AudioBuffer::setDecodedChunkLength(quint32 inDecodedChunkLength) {
	mDecodedChunkLength = inDecodedChunkLength;
	mState = eGotDecodedChunk;
}

void AudioBuffer::setConvertedChunkLength(quint32 inConvertedChunkLength) {
	mConvertedChunkLength = inConvertedChunkLength;
	mState = eGotConvertedChunk;
}


QByteArray *AudioBuffer::decodedChunkBuffer(quint32 &outLen) {
	if (state() == eGotDecodedChunk) {
		outLen = mDecodedChunkLength;
		return &mByteBuffer;
	}
	return NULL;
}

QByteArray *AudioBuffer::convertedChunkBuffer(quint32 &outLen) {
	if (state() == eGotConvertedChunk) {
		outLen = mConvertedChunkLength;
		return &mByteBuffer;
	}
	return NULL;
}


void AudioBuffer::reset() {
	mState = eEmpty;
	mDecodedChunkLength = 0;
	mConvertedChunkLength = 0;
}

quint32 AudioBuffer::requestedLength() {
	return mRequestedLength;
}

quint32 AudioBuffer::decodedChunkLength() {
	return mDecodedChunkLength;
}

quint32 AudioBuffer::convertedChunkLength() {
	return mConvertedChunkLength;
}

