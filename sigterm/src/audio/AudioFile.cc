#include "AudioFile.h"

#include "decoders/AudioDecoderOgg.h"

AudioFile::AudioFile(const QString &inFilePath, AudioManager *inAudioManager) {
    mFilePath = inFilePath;
    mDecoder = new AudioDecoderOgg(this, inAudioManager);
    mDecoder->open();
}

QString &AudioFile::filePath() {
    return mFilePath;
}

AudioDecoder *AudioFile::decoder() {
    return mDecoder;
}

