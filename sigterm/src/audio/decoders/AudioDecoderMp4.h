#ifndef _AUDIO_FILE_MP4_H
#define _AUDIO_FILE_MP4_H

#include "AudioDecoder.h"
#include "AudioStorage.h"

#include <neaacdec.h>
#include "mp4ff.h"

class AudioDecoderMp4 : public AudioDecoder {
	public:
		AudioDecoderMp4(AudioFile *inAudioFile, AudioManager *inAudioManager);
		~AudioDecoderMp4();

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

		FILE *mAacFile;
		NeAACDecHandle mAacHandle;
		NeAACDecFrameInfo mAacFrameInfo;

		mp4ff_callback_t mMp4Callbacks;
		mp4ff_t *mMp4File;
		quint32 mMp4Track;
		quint32 mSampleId;

		AudioStorage mAudioStorage;
		float mF;
};

#endif
