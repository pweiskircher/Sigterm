#include "AudioFile.h"
#include "AudioManager.h"

AudioFile::AudioFile(const QString &inFilePath, AudioManager *inAudioManager) {
    mFilePath = inFilePath;
    mDecoder = inAudioManager->createAudioDecoder(this);
}

QString &AudioFile::filePath() {
    return mFilePath;
}

AudioDecoder *AudioFile::decoder() {
    return mDecoder;
}

