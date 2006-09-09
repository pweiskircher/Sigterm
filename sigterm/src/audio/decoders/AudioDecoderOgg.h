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

		QString audioFormatDescription();
		QStringList audioFormatFileExtensions();

	private:
		virtual bool openFile();
		virtual bool closeFile();
		virtual AudioDecoder::DecodingStatus getDecodedChunk(AudioBuffer *inOutAudioBuffer);
		virtual bool seekToTimeInternal(quint32 inMilliSeconds);

		bool readVorbisInfo(OggVorbis_File *inFile);

		OggVorbis_File mOggVorbisFile;
};

#endif
