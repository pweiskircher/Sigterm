#include "AudioDecoderOgg.h"
#include "AudioFile.h"

AudioDecoderOgg::AudioDecoderOgg(AudioFile *inAudioFile, AudioManager *inAudioManager) : AudioDecoder(inAudioFile, inAudioManager) {
    mOpened = false;
}

AudioDecoderOgg::~AudioDecoderOgg() {
    if (mOpened)
	ov_clear(&mOggVorbisFile);
}


bool AudioDecoderOgg::open() {
    FILE *f = fopen(qPrintable(audioFile()->filePath()), "r");
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

bool AudioDecoderOgg::close() {
    if (mOpened) {
	ov_clear(&mOggVorbisFile);
	mOpened = false;
    }

    return true;
}


bool AudioDecoderOgg::seekToTime(quint32 inMilliSeconds) {
    if (ov_time_seek(&mOggVorbisFile, (double)(inMilliSeconds/1000.0)))
	return true;
    return false;
}

AudioDecoder::DecodingStatus AudioDecoderOgg::getDecodedChunk(QByteArray &inOutArray) {
    quint32 bytesRead = 0;
    int currentSection;
    quint32 lenNeeded = inOutArray.size();

    do {
	int r = ov_read(&mOggVorbisFile, inOutArray.data() + bytesRead, lenNeeded - bytesRead, 0, audioFormat().bitsPerSample()/8, 1,
		        &currentSection);
	if (r < 0) {
	    return eError;
	} else if (r == 0) {
	    inOutArray.resize(bytesRead);
	    return eEOF;
	} else {
	    bytesRead += r;
	}
    } while (bytesRead < lenNeeded);

    return eSuccess;
}

bool AudioDecoderOgg::canDecode(const QString &inFilePath) {
    bool result = false;

    FILE *f = fopen(qPrintable(inFilePath), "r");
    if (f) {
	OggVorbis_File vf;
	if (ov_test(f, &vf, NULL, 0) == 0)
	    result = true;
	ov_clear(&vf);
    }

    return result;
}

