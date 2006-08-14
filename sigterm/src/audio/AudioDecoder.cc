#include "AudioDecoder.h"
#include "AudioManager.h"

AudioDecoder::AudioDecoder(AudioFile *inAudioFile, AudioManager *inAudioManager) : mConverter(inAudioManager) {
    mBuiltCVT = false;
    mAudioManager = inAudioManager;
    mAudioFile = inAudioFile;
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

AudioDecoder::DecodingStatus AudioDecoder::getAudioChunk(QByteArray &outArray) {
    if (!mBuiltCVT) {
	mConverter.setSourceFormat(&audioFormat());
	mBuiltCVT = true;
    }

    DecodingStatus status = getDecodedChunk(outArray);
    if (status == eError) {
	qDebug("Couldn't get decoded audio chunk!");
	return eError;
    }
    if (!mConverter.convert(outArray)) {
	qDebug("Could not convert audio!");
	return eError;
    }

    return status;
}

AudioManager *AudioDecoder::audioManager() {
    return mAudioManager;
}

AudioFile *AudioDecoder::audioFile() {
    return mAudioFile;
}

