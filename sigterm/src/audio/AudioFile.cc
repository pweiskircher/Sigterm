#include "AudioFile.h"
#include "AudioManager.h"
#include "AudioDecoder.h"

AudioFile::AudioFile(const QString &inFilePath, AudioManager *inAudioManager) {
    mAudioManager = inAudioManager;
    mFilePath = inFilePath;
    mDecoder = inAudioManager->createAudioDecoder(this);
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

quint32 AudioFile::timeTotal() {
    return mTotalSamples/decoder()->audioFormat().frequency();
}

quint32 AudioFile::timePlayed() {
    return mPlayedSamples/decoder()->audioFormat().frequency();
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

void AudioFile::setIsPlaying(bool inValue) {
    mIsPlaying = inValue;

    if (mIsPlaying == true) {
	emit startedPlaying(this);
    } else {
	emit stoppedPlaying(this);
    }
}

