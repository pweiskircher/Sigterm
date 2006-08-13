#include "AudioFile.h"
#include "AudioManager.h"

AudioFile::AudioFile(AudioManager *inAudioManager) {
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

bool AudioFile::getAudioChunk(quint8 *inBuffer, int len) {
    if (!mBuiltCVT) {
	if (SDL_BuildAudioCVT(&mCVT, audioFormat().sdlFormat(), audioFormat().channels(), audioFormat().frequency(),
		                     audioManager()->hardwareSpec()->format, audioManager()->hardwareSpec()->channels,
				     audioManager()->hardwareSpec()->freq) < 0) {
	    qDebug("Error building AudioCVT");
	    exit(EXIT_FAILURE);
	}

	mCVT.len = 4096*4;
	mCVT.buf = (Uint8*)malloc(mCVT.len * mCVT.len_mult);

	mBuiltCVT = true;
    }

    quint32 decodedLen = len;
    getDecodedChunk((char *)mCVT.buf, decodedLen);
    SDL_ConvertAudio(&mCVT);
    memcpy(inBuffer, mCVT.buf, len);
    return true;
}

AudioManager *AudioFile::audioManager() {
    return mAudioManager;
}
