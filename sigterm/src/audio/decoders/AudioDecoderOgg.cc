#include "AudioDecoderOgg.h"
#include "AudioFile.h"
#include "AudioBuffer.h"

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
}

AudioDecoder *AudioDecoderOgg::createAudioDecoder(AudioFile *inAudioFile, AudioManager *inAudioManager) {
    return new AudioDecoderOgg(inAudioFile, inAudioManager);
}

bool AudioDecoderOgg::openFile() {
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

    readVorbisInfo(&mOggVorbisFile);

    return true;
}

bool AudioDecoderOgg::closeFile() {
    if (opened()) {
	ov_clear(&mOggVorbisFile);
    }

    return true;
}


bool AudioDecoderOgg::seekToTime(quint32 inMilliSeconds) {
    if (ov_time_seek(&mOggVorbisFile, (double)(inMilliSeconds/1000.0)))
	return true;
    return false;
}

AudioDecoder::DecodingStatus AudioDecoderOgg::getDecodedChunk(AudioBuffer *inOutAudioBuffer) {
    if (inOutAudioBuffer->state() != AudioBuffer::eEmpty) {
	qDebug("AudioDecoderOgg: AudioBuffer in wrong state %d", inOutAudioBuffer->state());
	return eContinue;
    }

    if (!inOutAudioBuffer->prepareForDecoding()) {
	qDebug("AudioBuffer::prepareForDecoding() failed.");
	return eContinue;
    }

    quint32 bytesRead = 0;
    int currentSection;
    quint32 lenNeeded = inOutAudioBuffer->requestedLength();
    AudioDecoder::DecodingStatus status = eContinue;

    do {
	int r = ov_read(&mOggVorbisFile, inOutAudioBuffer->byteBuffer().data() + bytesRead,
		        lenNeeded - bytesRead, 0, audioFormat().bitsPerSample()/8, 1, &currentSection);
	if (r < 0) {
	    status = eContinue;
	    break;
	} else if (r == 0) {
	    status = eStop;
	    break;
	} else {
	    bytesRead += r;
	}
    } while (bytesRead < lenNeeded);

    inOutAudioBuffer->setDecodedChunkLength(bytesRead);
    //qDebug("decoder: read %d bytes", bytesRead);
    return status;
}

bool AudioDecoderOgg::canDecode(const QString &inFilePath) {
    bool result = false;

    FILE *f = fopen(qPrintable(inFilePath), "rb");
    if (f) {
	OggVorbis_File vf;
	if (ov_test_callbacks(f, &vf, NULL, 0, callbacks) == 0) {
	    result = true;
	    ov_clear(&vf);
	} else {
	    fclose(f);
	}
    }

    return result;
}

bool AudioDecoderOgg::readInfo() {
    bool result = false;
    FILE *f = fopen(qPrintable(audioFile()->filePath()), "rb");
    if (f) {
	OggVorbis_File vf;
	if (ov_open_callbacks(f, &vf, NULL, 0, callbacks) == 0) {
	    result = readVorbisInfo(&vf);
	    ov_clear(&vf);
	} else {
	    fclose(f);
	}
    }

    return result;
}

bool AudioDecoderOgg::readVorbisInfo(OggVorbis_File *inFile) {
    vorbis_info *info = ov_info(inFile, -1);
    if (!info)
	return false;

    audioFormat().setBitRate((int)(info->bitrate_nominal / 1000.0));
    audioFormat().setChannels(info->channels);
    audioFormat().setBitsPerSample(info->channels * 8);
    audioFormat().setFrequency(info->rate);
    audioFormat().setIsBigEndian(false);
    audioFormat().setIsUnsigned(false);

    audioFile()->setTotalSamples(ov_pcm_total(inFile, -1));

    return true;
}
