#include "AudioManager.h"

#include "decoders/AudioFileOgg.h"

void fillBufferCallback(void *userdata, Uint8 *stream, int len) {
    AudioManager *mgr = (AudioManager *)userdata;
    mgr->fillBuffer(stream, len);
}

AudioManager::AudioManager() {
}

void AudioManager::run() {
    mAudioFile = new AudioFileOgg(this);
    mAudioFile->open("/tmp/a.ogg");

    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
	qDebug("Could not initialize SDL audio.");
	exit(EXIT_FAILURE);
    }

    mDesiredAudioSpec.freq = 44100;
    mDesiredAudioSpec.format = AUDIO_S16LSB;
    mDesiredAudioSpec.channels = 2;
    mDesiredAudioSpec.samples = 4096;
    mDesiredAudioSpec.userdata = this;
    mDesiredAudioSpec.callback = fillBufferCallback;

    if (SDL_OpenAudio(&mDesiredAudioSpec, &mHardwareAudioSpec) < 0) {
	qDebug("Could not open the audio device.");
	exit(EXIT_FAILURE);
    }

    SDL_PauseAudio(0);

    while (1) {
	SDL_Delay(10);
    }

    SDL_Quit();
}

void AudioManager::fillBuffer(Uint8 *stream, int len) {
#if 0
    if (!mBuiltCVT) {
	if (SDL_BuildAudioCVT(&mCVT, mAudioFile->audioFormat().sdlFormat(), mAudioFile->audioFormat().channels(),
		              mAudioFile->audioFormat()->frequency(), mHardwareAudioSpec.format, mHardwareAudioSpec.channels,
			      mHardwareAudioSpec.freq) != 0) {
	    qDebug("Error building AudioCVT");
	    exit(EXIT_FAILURE);
	}

	mCVT.len = 4096;
	mCVT.buf = malloc(mCVT.len * mCVT.len_mult);
    }
#endif

    if (mAudioFile->currentPosition() < mAudioFile->totalSize()) {
	mAudioFile->getAudioChunk(stream, len);
    } else {
	exit(1);
    }
}

SDL_AudioSpec *AudioManager::hardwareSpec() {
    return &mHardwareAudioSpec;
}
