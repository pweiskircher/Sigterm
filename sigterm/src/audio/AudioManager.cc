#include "AudioManager.h"
#include "AudioFile.h"
#include "PlayList.h"

void fillBufferCallback(void *userdata, Uint8 *stream, int len) {
    AudioManager *mgr = (AudioManager *)userdata;
    mgr->fillBuffer(stream, len);
}

AudioManager::AudioManager() : mAudioProcessor(this) {
    connect(&mAudioProcessor, SIGNAL(paused()), SLOT(audioProcessorPaused()));

    mAudioProcessor.start();
    mPaused = true;
    mCurrentPlayList = new PlayList();
}

void AudioManager::init() {
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
}

AudioStorage *AudioManager::audioStorage() {
    return &mAudioStorage;
}

QWaitCondition *AudioManager::audioProcessorWaitCondition() {
    return &mAudioProcessorWaitCondition;
}

PlayList *AudioManager::currentPlayList() {
    return mCurrentPlayList;
}

void AudioManager::audioProcessorPaused() {
    SDL_PauseAudio(true);
    mPaused = true;

    emit audioPaused(true);
}

void AudioManager::setPause(bool inPause) {
    mPaused = inPause;

    SDL_PauseAudio(inPause);

    if (inPause == true)
	mAudioProcessor.pause();

    mAudioProcessorWaitCondition.wakeAll();

    emit audioPaused(mPaused);
}

void AudioManager::togglePause() {
    setPause(!mPaused);
}

void AudioManager::skipTrack() {
    mAudioProcessor.skipTrack();
    mAudioProcessorWaitCondition.wakeAll();
}

bool AudioManager::paused() {
    return mPaused;
}

void AudioManager::fillBuffer(Uint8 *stream, int len) {
    QByteArray test;
    test.resize(len);
    if (audioStorage()->get(test)) {
	memcpy(stream, test.data(), len);
    }
}

SDL_AudioSpec *AudioManager::hardwareSpec() {
    return &mHardwareAudioSpec;
}

