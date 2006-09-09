#include "AudioManager.h"
#include "AudioFile.h"
#include "PlayQueue.h"

#include "decoders/AudioDecoderOgg.h"
#include "decoders/AudioDecoderFlac.h"
#include "decoders/AudioDecoderMp4.h"

void fillBufferCallback(void *userdata, Uint8 *stream, int len) {
	AudioManager *mgr = (AudioManager *)userdata;
	mgr->fillBuffer(stream, len);
}

AudioManager::AudioManager() : mAudioProcessor(this), mAudioLibrary(this), mPlayQueue(this) {
	connect(&mAudioProcessor, SIGNAL(paused()), SLOT(audioProcessorPaused()));

	mPaused = true;

	mAudioDecoderList.append(new AudioDecoderOgg(NULL, this));
	mAudioDecoderList.append(new AudioDecoderFlac(NULL, this));
	mAudioDecoderList.append(new AudioDecoderMp4(NULL, this));

	mHardwareAudioFormat = NULL;
}

AudioManager::~AudioManager() {
	delete mHardwareAudioFormat;
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

	mHardwareAudioFormat = new AudioFormat(&mHardwareAudioSpec);
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

QStringList AudioManager::supportedFileFilter() {
	QListIterator<AudioDecoder *> it(mAudioDecoderList);
	QStringList list;
	QString allSupported = "All Supported File Format (";
	bool firstAllSupported = true;

	while (it.hasNext()) {
		AudioDecoder *d = it.next();

		QString s;
		bool firstSingleSupported = true;
		s = d->audioFormatDescription() + " (";
		QStringList extensions = d->audioFormatFileExtensions();
		for (int i = 0; i < extensions.size(); i++) {
			if (firstAllSupported)
				firstAllSupported = false;
			else
				allSupported += " ";
			allSupported += extensions[i];

			if (firstSingleSupported)
				firstSingleSupported = false;
			else
				s += " ";
			s += extensions[i];
		}

		s += ")";
		list += s;
	}
	allSupported += ")";
	list += allSupported;

	return list;
}

void AudioManager::audioProcessorPaused() {
	SDL_PauseAudio(true);
	mPaused = true;

	emit audioPaused(true);
}

void AudioManager::setPause(bool inPause) {
	mPaused = inPause;

	SDL_PauseAudio(inPause);

	if (inPause == true) {
		mAudioProcessor.pause();
		mAudioProcessorWaitCondition.wakeAll();

		mAudioProcessor.wait();
		qDebug("thread finished.");
	} else {
		mAudioProcessor.start();
		qDebug("thread started.");
	}

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
	setPause(true);
	mAudioProcessor.wait();
}

bool AudioManager::paused() {
	return mPaused;
}

void AudioManager::fillBuffer(Uint8 *stream, int len) {
	mSDLBuffer.resize(len);
	if (audioStorage()->get(mSDLBuffer)) {
		memcpy(stream, mSDLBuffer.data(), len);
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

AudioFormat *AudioManager::hardwareFormat() {
	return mHardwareAudioFormat;
}

