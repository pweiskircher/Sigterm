#include "AudioManager.h"

#include "decoders/AudioFileOgg.h"

void fillBufferCallback(void *userdata, Uint8 *stream, int len) {
    AudioManager *mgr = (AudioManager *)userdata;
    mgr->fillBuffer(stream, len);
}

AudioManager::AudioManager() {
}

void AudioManager::init() {
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
}

void AudioManager::fillBuffer(Uint8 *stream, int len) {
    QByteArray test;
    test.resize(len);
    if (mAudioFile->getAudioChunk(test)) {
	memcpy(stream, test.data(), len);
    }
}

SDL_AudioSpec *AudioManager::hardwareSpec() {
    return &mHardwareAudioSpec;
}
