#ifndef _AUDIO_FILE_MP3_H
#define _AUDIO_FILE_MP3_H

#include "AudioDecoder.h"
#include "AudioStorage.h"

#include <mad.h>

struct audio_dither {
	mad_fixed_t error[3];
	mad_fixed_t random;
};

//#define MP3_DATA_OUTPUT_BUFFER_SIZE 4096

class AudioDecoderMp3 : public AudioDecoder {
	public:
		AudioDecoderMp3(AudioFile *inAudioFile, AudioManager *inAudioManager);
		~AudioDecoderMp3();

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
		virtual int decodeFirstFrame();
		virtual int decodeNextFrameHeader();
		virtual int decodeNextFrame();
		virtual bool fillDecoderBuffer();

		FILE *mInputFile;

		struct mad_stream mMadStream;
		struct mad_frame mMadFrame;
		struct mad_synth mMadSynth;
		struct audio_dither mDither;
		mad_timer_t mMadTimer;
	/*	unsigned char readBuffer[READ_BUFFER_SIZE];
		char outputBuffer[MP3_DATA_OUTPUT_BUFFER_SIZE];
		char * outputPtr;
		char * outputBufferEnd;
		float totalTime;
		float elapsedTime;
		int muteFrame;
		long * frameOffset;*/
		mad_timer_t * mMadTimes;
/*		long highestFrame;
		long maxFrames;
		long currentFrame;
		int flush;
		unsigned long bitRate;
		struct audio_dither dither;

		NeAACDecHandle mAacHandle;
		NeAACDecFrameInfo mAacFrameInfo;

		mp4ff_callback_t mMp4Callbacks;
		mp4ff_t *mMp4File;
		quint32 mMp4Track;
		quint32 mSampleId;*/

		AudioStorage mAudioStorage;
		size_t mReadBytes;
//		float mF;
};

#endif
