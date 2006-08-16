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
    if (opened())
	close();
}

bool AudioDecoder::open() {
    if (opened()) {
	qDebug("File already opened.");
	return false;
    }

    bool r = openFile();
    if (r)
	setOpened(true);
    return r;
}

bool AudioDecoder::close() {
    if (!opened()) {
	qDebug("File not open.");
	return false;
    }

    bool r = closeFile();
    if (r)
	setOpened(false);
    return r;
}

bool AudioDecoder::opened() {
    return mOpened;
}

AudioFormat &AudioDecoder::audioFormat() {
    return mAudioFormat;
}

AudioDecoder::DecodingStatus AudioDecoder::getAudioChunk(AudioBuffer *inOutAudioBuffer) {
    if (!opened()) {
	if (!open())
	    return eStop;
    }

    if (!mBuiltCVT) {
	mConverter.setSourceFormat(&audioFormat());
	mBuiltCVT = true;
    }

    AudioDecoder::DecodingStatus status;
    status = getDecodedChunk(inOutAudioBuffer);
    if (status == eStop) {
	close();
    }

    if (inOutAudioBuffer->state() != AudioBuffer::eGotDecodedChunk) {
	return status;
    }

    if (!mConverter.convert(inOutAudioBuffer)) {
	qDebug("Could not convert audio!");
	close();
	return eStop;
    }

    // sanity check
    if (inOutAudioBuffer->state() != AudioBuffer::eGotConvertedChunk) {
	qDebug("AudioBuffer state wrong.");
	close();
	return eStop;
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
