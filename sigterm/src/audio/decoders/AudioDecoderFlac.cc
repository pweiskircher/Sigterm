#include "AudioDecoderFlac.h"
#include "AudioFile.h"
#include "AudioBuffer.h"

#include <FLAC++/all.h>

static ::FLAC__StreamDecoderWriteStatus write_callback(const FLAC__FileDecoder *decoder, const ::FLAC__Frame *frame, const FLAC__int32 * const buffer[], void *userdata) {
    AudioDecoderFlac *mAudioDecoderFlac = (AudioDecoderFlac *)userdata;
    mAudioDecoderFlac->handleDecodedFlacFrame(frame, buffer);
    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

static void metadata_callback(const FLAC__FileDecoder *decoder, const ::FLAC__StreamMetadata *metadata, void *userdata) {
    AudioDecoderFlac *mAudioDecoderFlac = (AudioDecoderFlac *)userdata;
    if (metadata->type == FLAC__METADATA_TYPE_STREAMINFO) {
	const FLAC__StreamMetadata_StreamInfo *si = &metadata->data.stream_info;
	mAudioDecoderFlac->setAudioFormat(si);
    }
}

static void metadata_candecode_callback(const FLAC__FileDecoder *decoder, const ::FLAC__StreamMetadata *metadata, void *userdata) {
    AudioDecoderFlac *mAudioDecoderFlac = (AudioDecoderFlac *)userdata;
    if (metadata->type == FLAC__METADATA_TYPE_STREAMINFO) {
	mAudioDecoderFlac->setCanDecode(true);
    }
}

static void error_callback(const FLAC__FileDecoder *decoder, ::FLAC__StreamDecoderErrorStatus status, void *userdata) {
    qDebug("error callback: %d", status);
}


AudioDecoderFlac::AudioDecoderFlac(AudioFile *inAudioFile, AudioManager *inAudioManager) : AudioDecoder(inAudioFile, inAudioManager) {
}

AudioDecoderFlac::~AudioDecoderFlac() {
}

AudioDecoder *AudioDecoderFlac::createAudioDecoder(AudioFile *inAudioFile, AudioManager *inAudioManager) {
    return new AudioDecoderFlac(inAudioFile, inAudioManager);
}

bool AudioDecoderFlac::open() {
    qDebug("AudioDecoderFlac::open(%s)", qPrintable(audioFile()->filePath()));

    mDecoder = FLAC__file_decoder_new();
    FLAC__file_decoder_set_client_data(mDecoder, this);
    FLAC__file_decoder_set_write_callback(mDecoder, write_callback);
    FLAC__file_decoder_set_metadata_callback(mDecoder, metadata_callback);
    FLAC__file_decoder_set_error_callback(mDecoder, error_callback);

    if (!FLAC__file_decoder_set_filename(mDecoder, qPrintable(audioFile()->filePath()))) {
	qDebug("Couldn't set filename on flac decoder.");
	return false;
    }

    if (FLAC__file_decoder_init(mDecoder) != FLAC__FILE_DECODER_OK) {
	qDebug("Couldn't initialize flac decoder context.");
	return false;
    }

    if (!FLAC__file_decoder_process_until_end_of_metadata(mDecoder)) {
	qDebug("Couldn't process flac file to end of meta data.");
	return false;
    }

    audioFormat().setBitRate(0);
    audioFormat().setIsBigEndian(true);
    audioFormat().setIsUnsigned(false);

    setOpened(true);

    return true;
}

bool AudioDecoderFlac::close() {
    qDebug("AudioDecoderFlac::close(%s)", qPrintable(audioFile()->filePath()));

    setOpened(false);
    FLAC__file_decoder_finish(mDecoder);
    FLAC__file_decoder_delete(mDecoder);
    mDecoder = NULL;
    return true;
}


bool AudioDecoderFlac::seekToTime(quint32 inMilliSeconds) {
    return false;
}

AudioDecoder::DecodingStatus AudioDecoderFlac::getDecodedChunk(AudioBuffer *inOutAudioBuffer) {
    if (inOutAudioBuffer->state() != AudioBuffer::eEmpty) {
	qDebug("AudioDecoderFlac: AudioBuffer in wrong state!");
	return eContinue;
    }

    if (!inOutAudioBuffer->prepareForDecoding()) {
	qDebug("AudioBuffer::prepareForDecoding() failed.");
	return eContinue;
    }

    AudioDecoder::DecodingStatus status = eContinue;
    while (mStorage.needData(inOutAudioBuffer->requestedLength()) == false) {
	if (!FLAC__file_decoder_process_single(mDecoder)) {
	    // no idea how we could recover from that error. just stop.
	    status = eStop;
	    break;
	}
    }

    if (status == eContinue) {
	mStorage.get(inOutAudioBuffer->byteBuffer());
	inOutAudioBuffer->setDecodedChunkLength(inOutAudioBuffer->requestedLength());
    } else {
	inOutAudioBuffer->byteBuffer().resize(mStorage.bufferLength());
	mStorage.get(inOutAudioBuffer->byteBuffer());
	inOutAudioBuffer->setDecodedChunkLength(mStorage.bufferLength());
    }

    return status;
}

bool AudioDecoderFlac::canDecode(const QString &inFilePath) {
    FLAC__FileDecoder *decoder;
    decoder = FLAC__file_decoder_new();
    FLAC__file_decoder_set_client_data(decoder, this);
    FLAC__file_decoder_set_write_callback(decoder, write_callback);
    FLAC__file_decoder_set_metadata_callback(decoder, metadata_candecode_callback);
    FLAC__file_decoder_set_error_callback(decoder, error_callback);

    setCanDecode(false);

    if (!FLAC__file_decoder_set_filename(decoder, qPrintable(inFilePath))) {
	qDebug("Couldn't set filename on flac decoder.");
	return false;
    }

    if (FLAC__file_decoder_init(decoder) != FLAC__FILE_DECODER_OK) {
	qDebug("Couldn't initialize flac decoder context.");
	return false;
    }

    if (!FLAC__file_decoder_process_until_end_of_metadata(decoder)) {
	qDebug("Couldn't process flac file to end of meta data.");
	return false;
    }

    FLAC__file_decoder_finish(decoder);
    FLAC__file_decoder_delete(decoder);

    return mCanDecode;
}

void AudioDecoderFlac::handleDecodedFlacFrame(const ::FLAC__Frame *frame, const FLAC__int32 * const buffer[]) {
    unsigned int i, j;
    QByteArray array;

    array.resize(2 * frame->header.blocksize * frame->header.channels);
    quint16 *dst = (quint16*)array.data();

    // TODO: can we assume that we only get 16bit samples? SDL_sound does also handle 8bit case..

    for (i=0; i < frame->header.blocksize; i++) {
	for (j=0; j < frame->header.channels; j++) {
	    *dst++ = buffer[j][i];
	}
    }

    mStorage.add(array, array.size());
}

void AudioDecoderFlac::setAudioFormat(const FLAC__StreamMetadata_StreamInfo *si) {
    audioFormat().setChannels(si->channels);
    audioFormat().setBitsPerSample(si->bits_per_sample);
    audioFormat().setFrequency(si->sample_rate);
    mTotalSize = si->total_samples * si->channels * (si->bits_per_sample/8);
}

void AudioDecoderFlac::setCanDecode(bool inValue) {
    mCanDecode = inValue;
}

