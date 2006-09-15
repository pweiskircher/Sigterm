#ifndef _AUDIO_FILE_MP3_H
#define _AUDIO_FILE_MP3_H

#include "AudioDecoder.h"
#include "AudioStorage.h"
#include <QFile>

#include <mad.h>

#define AUDIODECODER_MP3_READBUFFER_SIZE 40960

struct audio_dither {
	mad_fixed_t error[3];
	mad_fixed_t random;
};

class AudioDecoderMp3 : public AudioDecoder {
	public:
		AudioDecoderMp3(AudioFile *inAudioFile, AudioManager *inAudioManager);
		~AudioDecoderMp3();

		AudioDecoder *createAudioDecoder(AudioFile *inAudioFile, AudioManager *inAudioManager);
		bool canDecode(const QString &inFilePath);
		bool readInfo();

		QString audioFormatDescription();
		QStringList audioFormatFileExtensions();

	private:
		bool openFile();
		bool closeFile();
		AudioDecoder::DecodingStatus getDecodedChunk(AudioBuffer *inOutAudioBuffer);
		bool seekToTimeInternal(quint32 inMilliSeconds);
		bool decodeFirstFrame();
		int decodeNextFrameHeader();
		int decodeNextFrame();
		bool fillDecoderBuffer();

		QFile mInputFile;
		unsigned char mBufferRead[AUDIODECODER_MP3_READBUFFER_SIZE];

		struct mad_stream mMadStream;
		struct mad_frame mMadFrame;
		struct mad_synth mMadSynth;
		struct audio_dither mDither;
		mad_timer_t mMadTimer;
	/*	unsigned char readBuffer[READ_BUFFER_SIZE];
		char outputBuffer[MP3_DATA_OUTPUT_BUFFER_SIZE];
		char * outputPtr;
		char * outputBufferEnd;
		long * frameOffset;*/
		float mTimeTotal;
		float mTimeElapsed;
		int mMuteFrame;

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
