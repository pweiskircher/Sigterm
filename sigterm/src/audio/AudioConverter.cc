#include "AudioConverter.h"
#include "AudioManager.h"
#include "AudioFormat.h"

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

bool AudioConverter::convert(QByteArray &inOutDataArray) {
    // we should .. be a bit .. smarter about that.
    mCVT.len = inOutDataArray.size()*4;

    inOutDataArray.resize(mCVT.len * mCVT.len_mult);
    mCVT.buf = (Uint8*)inOutDataArray.data();

    if (SDL_ConvertAudio(&mCVT)) {
	qDebug("Error converting audio ...");
	return false;
    }

    return true;
}


