#include "AudioFileOgg.h"

AudioFileOgg::AudioFileOgg(AudioManager *inAudioManager) : AudioFile(inAudioManager) {
    mOpened = false;
}

AudioFileOgg::~AudioFileOgg() {
}


bool AudioFileOgg::open(const QString &inFilename) {
    FILE *f = fopen(qPrintable(inFilename), "r");
    if (!f) {
	// TODO: error reporting
	return false;
    }

    if (ov_open(f, &mOggVorbisFile, NULL, 0)) {
	fclose(f);
	// TODO: error reporting
	return false;
    }

    mOpened = true;

    vorbis_info *info = ov_info(&mOggVorbisFile, -1);
    audioFormat().setBitRate((int)(info->bitrate_nominal / 1000.0));
    audioFormat().setChannels(info->channels);
    audioFormat().setBitsPerSample(info->channels * 8);
    audioFormat().setFrequency(info->rate);
    audioFormat().setIsBigEndian(false);
    audioFormat().setIsUnsigned(false);

    mTotalSize = ov_pcm_total(&mOggVorbisFile, -1) * audioFormat().channels() * audioFormat().bitsPerSample()/8;
    mCurrentPosition = 0;

    return true;
}

bool AudioFileOgg::close() {
    if (mOpened) {
	ov_clear(&mOggVorbisFile);
	mOpened = false;
    }

    return true;
}


bool AudioFileOgg::seekToTime(quint32 inMilliSeconds) {
    if (ov_time_seek(&mOggVorbisFile, (double)(inMilliSeconds/1000.0)))
	return true;
    return false;
}

bool AudioFileOgg::getDecodedChunk(char *inOutBuffer, quint32 &inOutLen) {
    quint32 bytesRead = 0;
    int currentSection;

    do {
	int r = ov_read(&mOggVorbisFile, inOutBuffer + bytesRead, inOutLen - bytesRead, 0, audioFormat().bitsPerSample()/8, 1,
		        &currentSection);
	if (r < 0) {
	    inOutLen = 0;
	    return false;
	} else if (r == 0) {
	    inOutLen = bytesRead;
	    return true;
	} else {
	    bytesRead += r;
	}
    } while (bytesRead < inOutLen);

    inOutLen = bytesRead;
    return true;
}

