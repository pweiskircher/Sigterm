#include "AudioDecoderOgg.h"
#include "AudioFile.h"

static int _fseek64_wrap(FILE *f,ogg_int64_t off,int whence){
    if(f==NULL)return(-1);
    return fseek(f,off,whence);
}

ov_callbacks callbacks = {
    (size_t (*)(void *, size_t, size_t, void *))  fread,
    (int (*)(void *, ogg_int64_t, int))           _fseek64_wrap,
    (int (*)(void *))                             fclose,
    (long (*)(void *))                            ftell
};

AudioDecoderOgg::AudioDecoderOgg(AudioFile *inAudioFile, AudioManager *inAudioManager) : AudioDecoder(inAudioFile, inAudioManager) {
}

AudioDecoderOgg::~AudioDecoderOgg() {
    if (opened())
	close();
}


bool AudioDecoderOgg::open() {
    FILE *f = fopen(qPrintable(audioFile()->filePath()), "rb");
    if (!f) {
	// TODO: error reporting
	return false;
    }

    if (ov_open_callbacks(f, &mOggVorbisFile, NULL, 0, callbacks)) {
	fclose(f);
	// TODO: error reporting
	return false;
    }

    setOpened(true);

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
    if (opened()) {
	ov_clear(&mOggVorbisFile);
	setOpened(false);
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

