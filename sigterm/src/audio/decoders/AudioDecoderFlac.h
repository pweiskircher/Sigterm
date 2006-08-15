#ifndef _AUDIO_FILE_FLAC_H
#define _AUDIO_FILE_FLAC_H

#include "AudioDecoder.h"
#include "AudioStorage.h"
#include <FLAC/all.h>

class AudioDecoderFlac;

class AudioDecoderFlac : public AudioDecoder {
    public:
	AudioDecoderFlac(AudioFile *inAudioFile, AudioManager *inAudioManager);
	~AudioDecoderFlac();

	AudioDecoder *createAudioDecoder(AudioFile *inAudioFile, AudioManager *inAudioManager);
	bool canDecode(const QString &inFilePath);

	bool open();
	bool close();

	bool seekToTime(quint32 inMilliSeconds);
	void handleDecodedFlacFrame(const ::FLAC__Frame *frame, const FLAC__int32 * const buffer[]);
	void setAudioFormat(const FLAC__StreamMetadata_StreamInfo *si);

	void setCanDecode(bool inValue);

    private:
	friend class FlacDecoder;

	AudioDecoder::DecodingStatus getDecodedChunk(AudioBuffer *inOutAudioBuffer);

	AudioStorage mStorage;

	bool mCanDecode;
	FLAC__FileDecoder *mDecoder;
};

#endif
