#include "AudioConverter.h"
#include "AudioManager.h"
#include "AudioFormat.h"
#include "AudioBuffer.h"

AudioConverter::AudioConverter(AudioManager *inAudioManager) {
    mAudioManager = inAudioManager;
}

bool AudioConverter::setSourceFormat(AudioFormat *inFormat) {
    if (SDL_BuildAudioCVT(&mCVT, inFormat->sdlFormat(), inFormat->channels(), inFormat->frequency(),
		mAudioManager->hardwareSpec()->format, mAudioManager->hardwareSpec()->channels,
		mAudioManager->hardwareSpec()->freq) < 0) {
	qDebug("Error building AudioCVT");
	return false;
    }

    return true;
}

bool AudioConverter::convert(AudioBuffer *inOutAudioBuffer) {
    // no conversion needed!
    if (mCVT.needed == 0) {
	inOutAudioBuffer->setConvertedChunkLength(inOutAudioBuffer->decodedChunkLength());
	return true;
    }

    if (!inOutAudioBuffer->prepareForConversion(&mCVT)) {
	qDebug("Error on AudioBuffer::prepareForConversion");
	return false;
    }

    if (SDL_ConvertAudio(&mCVT)) {
	qDebug("Error converting audio ...");
	return false;
    }

    //qDebug("converter: converted to %d (%d * %f) bytes", (int)(mCVT.len * mCVT.len_ratio), mCVT.len, mCVT.len_ratio);
    inOutAudioBuffer->setConvertedChunkLength(mCVT.len * mCVT.len_ratio);

    return true;
}


