#include "AudioDecoder.h"
#include "AudioManager.h"
#include "AudioBuffer.h"

AudioDecoder::AudioDecoder(AudioFile *inAudioFile, AudioManager *inAudioManager) : mConverter(inAudioManager) {
    mBuiltCVT = false;
    mAudioManager = inAudioManager;
    mAudioFile = inAudioFile;
    mOpened = false;
}

AudioDecoder::~AudioDecoder() {
}

bool AudioDecoder::opened() {
    return mOpened;
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

AudioDecoder::DecodingStatus AudioDecoder::getAudioChunk(AudioBuffer *inOutAudioBuffer) {
    if (!opened()) {
	if (!open())
	    return eError;
    }

    if (!mBuiltCVT) {
	mConverter.setSourceFormat(&audioFormat());
	mBuiltCVT = true;
    }

    DecodingStatus status = getDecodedChunk(inOutAudioBuffer);
    if (status == eError) {
	qDebug("Couldn't get decoded audio chunk!");
	return eError;
    } else if (status == eEOF) {
	close();
    }

    // sanity check
    if (inOutAudioBuffer->state() != AudioBuffer::eGotDecodedChunk) {
	qDebug("AudioBuffer state wrong.");
	return eError;
    }

    if (!mConverter.convert(inOutAudioBuffer)) {
	qDebug("Could not convert audio!");
	return eError;
    }

    // sanity check
    if (inOutAudioBuffer->state() != AudioBuffer::eGotConvertedChunk) {
	qDebug("AudioBuffer state wrong.");
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

void AudioDecoder::setOpened(bool inValue) {
    mOpened = inValue;
}
