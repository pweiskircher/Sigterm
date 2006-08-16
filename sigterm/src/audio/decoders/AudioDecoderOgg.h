#ifndef _AUDIO_FILE_OGG_H
#define _AUDIO_FILE_OGG_H

#include "AudioDecoder.h"
#include <vorbis/vorbisfile.h>

class AudioDecoderOgg : public AudioDecoder {
    public:
	AudioDecoderOgg(AudioFile *inAudioFile, AudioManager *inAudioManager);
	~AudioDecoderOgg();

	virtual AudioDecoder *createAudioDecoder(AudioFile *inAudioFile, AudioManager *inAudioManager);
	virtual bool canDecode(const QString &inFilePath);
	virtual bool readInfo();
	virtual bool seekToTime(quint32 inMilliSeconds);

    private:
	virtual bool openFile();
	virtual bool closeFile();
	virtual AudioDecoder::DecodingStatus getDecodedChunk(AudioBuffer *inOutAudioBuffer);

	bool readVorbisInfo(OggVorbis_File *inFile);

	OggVorbis_File mOggVorbisFile;
};

#endif
