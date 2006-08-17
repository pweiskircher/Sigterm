#include "AudioManager.h"
#include "AudioFile.h"
#include "PlayQueue.h"

#include "decoders/AudioDecoderOgg.h"
#include "decoders/AudioDecoderFlac.h"

void fillBufferCallback(void *userdata, Uint8 *stream, int len) {
    AudioManager *mgr = (AudioManager *)userdata;
    mgr->fillBuffer(stream, len);
}

AudioManager::AudioManager() : mAudioProcessor(this), mAudioLibrary(this) {
    connect(&mAudioProcessor, SIGNAL(paused()), SLOT(audioProcessorPaused()));

    mAudioProcessor.start();
    mPaused = true;

    mAudioDecoderList.append(new AudioDecoderOgg(NULL, this));
    mAudioDecoderList.append(new AudioDecoderFlac(NULL, this));
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

AudioLibrary *AudioManager::audioLibrary() {
    return &mAudioLibrary;
}

QWaitCondition *AudioManager::audioProcessorWaitCondition() {
    return &mAudioProcessorWaitCondition;
}

PlayQueue *AudioManager::playQueue() {
    return &mPlayQueue;
}

AudioDecoder *AudioManager::createAudioDecoder(AudioFile *inAudioFile) {
    QListIterator<AudioDecoder *> it(mAudioDecoderList);
    while (it.hasNext()) {
	AudioDecoder *d = it.next();
	if (d->canDecode(inAudioFile->filePath()))
	    return d->createAudioDecoder(inAudioFile, this);
    }

    return NULL;
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

void AudioManager::quit() {
    if (!paused())
	setPause(true);
    mAudioProcessor.quit();
    mAudioProcessorWaitCondition.wakeAll();
    mAudioProcessor.wait();
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

void AudioManager::nextTrack() {
    mPlayQueue.nextTrack();
    skipTrack();
}

void AudioManager::prevTrack() {
    mPlayQueue.prevTrack();
    skipTrack();
}


SDL_AudioSpec *AudioManager::hardwareSpec() {
    return &mHardwareAudioSpec;
}

