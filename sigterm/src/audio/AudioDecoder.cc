#include "AudioDecoder.h"
#include "AudioManager.h"

AudioDecoder::AudioDecoder(AudioManager *inAudioManager) : mConverter(inAudioManager) {
    mBuiltCVT = false;
    mAudioManager = inAudioManager;
}

AudioDecoder::~AudioDecoder() {
}

AudioFormat &AudioDecoder::audioFormat() {
    return mAudioFormat;
}

quint32 AudioDecoder::totalSize() {
    return mTotalSize;
}

quint32 AudioDecoder::currentPosition() {
    return mCurrentPosition;
}

bool AudioDecoder::getAudioChunk(QByteArray &outArray) {
    if (!mBuiltCVT) {
	mConverter.setSourceFormat(&audioFormat());
	mBuiltCVT = true;
    }

    if (!getDecodedChunk(outArray)) {
	qDebug("Couldn't get decoded audio chunk!");
	return false;
    }
    if (!mConverter.convert(outArray)) {
	qDebug("Could not convert audio!");
	return false;
    }

    return true;
}

AudioManager *AudioDecoder::audioManager() {
    return mAudioManager;
}
