#include "AudioFile.h"
#include "AudioManager.h"
#include "AudioDecoder.h"

#include <QDebug>
#include <SDL.h>

AudioFile::AudioFile(const QString &inFilePath, AudioManager *inAudioManager) : mMetaData(this) {
	mAudioManager = inAudioManager;
	mFilePath = inFilePath;
	mPlayedSamples = 0;
	mTotalSamples = 0;
	mBytesInAudioStorage = 0;

	mDecoder = inAudioManager->createAudioDecoder(this);
	if (mDecoder)
		mDecoder->readInfo();

	mIsPlaying = false;

	mAudioManager->audioLibrary()->addAudioFile(this);
}

AudioFile::~AudioFile() {
	mAudioManager->audioLibrary()->removeAudioFile(this);
	mAudioManager->playQueue()->removeAudioFile(this);
}

void AudioFile::addToQueue() {
	mAudioManager->playQueue()->addAudioFile(this);
}

void AudioFile::removeFromQueue() {
	mAudioManager->playQueue()->removeAudioFile(this);
}


QString &AudioFile::filePath() {
	return mFilePath;
}

AudioDecoder *AudioFile::decoder() {
	return mDecoder;
}

AudioMetaData *AudioFile::metaData() {
	return &mMetaData;
}

quint32 AudioFile::timeTotal() {
	if (!decoder())
		return 0;

	return mTotalSamples/decoder()->audioFormat().frequency();
}

quint32 AudioFile::timePlayed() {
	if (!decoder())
		return 0;

	return mPlayedSamples/decoder()->audioFormat().frequency();
}

bool AudioFile::seekToTime(quint32 inMilliSeconds) {
	SDL_LockAudio();
	bool paused = mAudioManager->paused();
	mAudioManager->setPause(true);
	mAudioManager->audioStorage()->clear();
	mPlayedSamples = (inMilliSeconds/1000) * decoder()->audioFormat().frequency();
	decoder()->seekToTime(inMilliSeconds);
	mAudioManager->playQueue()->setNextTrack(this);
	mAudioManager->setPause(paused);
	SDL_UnlockAudio();
	return true;
}

quint32 AudioFile::totalSamples() {
	return mTotalSamples;
}

quint32 AudioFile::playedSamples() {
	return mPlayedSamples;
}


void AudioFile::setTotalSamples(quint32 inTotalSamples) {
	mTotalSamples = inTotalSamples;
}

void AudioFile::setPlayedSamples(quint32 inPlayedSamples) {
	mPlayedSamples = inPlayedSamples;
}


bool AudioFile::isPlaying() {
	return mIsPlaying;
}

void AudioFile::setIsDecoding(bool inValue) {
	mIsDecoding = inValue;
}

void AudioFile::bytesAddedToAudioStorage(quint32 inSize) {
	mBytesInAudioStorage += inSize;
}

void AudioFile::bytesRemovedFromAudioStorage(quint32 inSize) {
	mBytesInAudioStorage -= inSize;

	if (mIsDecoding == true && mIsPlaying == false) {
		mIsPlaying = true;
		emit startedPlaying(this);
	} else if (mIsPlaying == true && mBytesInAudioStorage == 0) {
		// TODO: this isn't working right. its possible that we are two times in the storage.
		mIsPlaying = false;
		emit stoppedPlaying(this, timePlayed());
		mPlayedSamples = 0;
		return;
	}

	mPlayedSamples += inSize/mAudioManager->hardwareFormat()->channels()/(mAudioManager->hardwareFormat()->bitsPerSample()/8);

	if (mPlayedSamples >= mTotalSamples)
		mPlayedSamples = 0;
}

quint32 AudioFile::bytesInAudioStorage() {
	return mBytesInAudioStorage;
}
