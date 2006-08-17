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

	virtual AudioDecoder *createAudioDecoder(AudioFile *inAudioFile, AudioManager *inAudioManager);
	virtual bool canDecode(const QString &inFilePath);
	virtual bool seekToTime(quint32 inMilliSeconds);
	virtual bool readInfo();

	void handleDecodedFlacFrame(const ::FLAC__Frame *frame, const FLAC__int32 * const buffer[]);
	void setAudioFormat(const FLAC__StreamMetadata_StreamInfo *si);
	void setVorbisComments(const FLAC__StreamMetadata_VorbisComment *vc);

	void setCanDecode(bool inValue);

    private:
	virtual bool openFile();
	virtual bool closeFile();
	virtual AudioDecoder::DecodingStatus getDecodedChunk(AudioBuffer *inOutAudioBuffer);

	AudioStorage mStorage;

	bool mCanDecode;
	FLAC__FileDecoder *mDecoder;
};

#endif
