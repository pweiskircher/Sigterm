#ifndef _AUDIO_FILE_OGG_H
#define _AUDIO_FILE_OGG_H

#include "AudioDecoder.h"
#include <vorbis/vorbisfile.h>

class AudioDecoderOgg : public AudioDecoder {
    public:
	AudioDecoderOgg(AudioManager *inAudioManager);
	~AudioDecoderOgg();

	bool open(const QString &inFilename);
	bool close();

	bool seekToTime(quint32 inMilliSeconds);

    private:
	bool getDecodedChunk(QByteArray &inOutArray);

	OggVorbis_File mOggVorbisFile;
	bool mOpened;
};

#endif
