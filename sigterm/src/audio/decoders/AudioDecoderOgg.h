#ifndef _AUDIO_FILE_OGG_H
#define _AUDIO_FILE_OGG_H

#include "AudioDecoder.h"
#include <vorbis/vorbisfile.h>

class AudioDecoderOgg : public AudioDecoder {
    public:
	AudioDecoderOgg(AudioFile *inAudioFile, AudioManager *inAudioManager);
	~AudioDecoderOgg();

	bool open();
	bool close();

	bool seekToTime(quint32 inMilliSeconds);

    private:
	AudioDecoder::DecodingStatus getDecodedChunk(QByteArray &inOutArray);
	bool canDecode(const QString &inFilePath);

	OggVorbis_File mOggVorbisFile;
	bool mOpened;
};

#endif
