#include "AudioManager.h"
#include "AudioFile.h"
#include "PlayList.h"

void fillBufferCallback(void *userdata, Uint8 *stream, int len) {
    AudioManager *mgr = (AudioManager *)userdata;
    mgr->fillBuffer(stream, len);
}

AudioManager::AudioManager() : mAudioProcessor(this) {
    mAudioProcessor.start();
}

void AudioManager::init() {
    mCurrentPlayList = new PlayList();
    mCurrentPlayList->add(new AudioFile("/tmp/a.ogg", this));

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

    setPause(false);
}

AudioBuffer *AudioManager::audioBuffer() {
    return &mAudioBuffer;
}

QWaitCondition *AudioManager::audioProcessorWaitCondition() {
    return &mAudioProcessorWaitCondition;
}

PlayList *AudioManager::currentPlayList() {
    return mCurrentPlayList;
}

void AudioManager::setPause(bool inPause) {
    SDL_PauseAudio(inPause);
    mAudioProcessorWaitCondition.wakeAll();
}

void AudioManager::fillBuffer(Uint8 *stream, int len) {
    QByteArray test;
    test.resize(len);
    if (audioBuffer()->get(test)) {
	memcpy(stream, test.data(), len);
    }
}

SDL_AudioSpec *AudioManager::hardwareSpec() {
    return &mHardwareAudioSpec;
}

