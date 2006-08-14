#include "AudioFile.h"
#include "AudioManager.h"

AudioFile::AudioFile(AudioManager *inAudioManager) : mConverter(inAudioManager) {
    mBuiltCVT = false;
    mAudioManager = inAudioManager;
}

AudioFile::~AudioFile() {
}

AudioFormat &AudioFile::audioFormat() {
    return mAudioFormat;
}

quint32 AudioFile::totalSize() {
    return mTotalSize;
}

quint32 AudioFile::currentPosition() {
    return mCurrentPosition;
}

bool AudioFile::getAudioChunk(QByteArray &outArray) {
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

AudioManager *AudioFile::audioManager() {
    return mAudioManager;
}
