#include "AudioDecoderFlac.h"
#include "AudioFile.h"
#include "AudioBuffer.h"

#include <QSysInfo>

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
	} else if (metadata->type == FLAC__METADATA_TYPE_VORBIS_COMMENT) {
		const FLAC__StreamMetadata_VorbisComment *vc = &metadata->data.vorbis_comment;
		mAudioDecoderFlac->setVorbisComments(vc);
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

bool AudioDecoderFlac::openFile() {
	qDebug("AudioDecoderFlac::open(%s)", qPrintable(audioFile()->filePath()));

	mDecoder = FLAC__file_decoder_new();
	FLAC__file_decoder_set_client_data(mDecoder, this);
	FLAC__file_decoder_set_write_callback(mDecoder, write_callback);
	FLAC__file_decoder_set_metadata_callback(mDecoder, metadata_callback);
	FLAC__file_decoder_set_error_callback(mDecoder, error_callback);
	FLAC__file_decoder_set_metadata_respond(mDecoder, FLAC__METADATA_TYPE_VORBIS_COMMENT);

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

	if (QSysInfo::ByteOrder == QSysInfo::BigEndian)
		audioFormat().setIsBigEndian(true);
	else
		audioFormat().setIsBigEndian(false);

	audioFormat().setIsUnsigned(false);

	return true;
}

bool AudioDecoderFlac::closeFile() {
	qDebug("AudioDecoderFlac::close(%s)", qPrintable(audioFile()->filePath()));

	FLAC__file_decoder_finish(mDecoder);
	FLAC__file_decoder_delete(mDecoder);
	mDecoder = NULL;
	return true;
}


bool AudioDecoderFlac::seekToTimeInternal(quint32 inMilliSeconds) {
	return FLAC__file_decoder_seek_absolute(mDecoder, audioFormat().frequency() * (inMilliSeconds/1000.0));
}

bool AudioDecoderFlac::readInfo() {
	FLAC__FileDecoder *decoder;
	decoder = FLAC__file_decoder_new();
	FLAC__file_decoder_set_client_data(decoder, this);
	FLAC__file_decoder_set_write_callback(decoder, write_callback);
	FLAC__file_decoder_set_metadata_callback(decoder, metadata_callback);
	FLAC__file_decoder_set_error_callback(decoder, error_callback);
	FLAC__file_decoder_set_metadata_respond(decoder, FLAC__METADATA_TYPE_VORBIS_COMMENT);

	if (!FLAC__file_decoder_set_filename(decoder, qPrintable(audioFile()->filePath()))) {
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

	return true;
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
		if (FLAC__file_decoder_get_state(mDecoder) == FLAC__FILE_DECODER_END_OF_FILE) {
			status = eStop;
			break;
		}

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
	audioFile()->setTotalSamples(si->total_samples);
}

void AudioDecoderFlac::setVorbisComments(const FLAC__StreamMetadata_VorbisComment *vc) {
	QStringList list;
	for (unsigned int i=0; i < vc->num_comments; i++) {
		list += (const char *)vc->comments[i].entry;
	}

	audioFile()->metaData()->parseVorbisComments(list);
}

void AudioDecoderFlac::setCanDecode(bool inValue) {
	mCanDecode = inValue;
}

