#ifndef _AUDIO_BUFFER_H
#define _AUDIO_BUFFER_H

#include <QtGlobal>
#include <QByteArray>
#include <SDL.h>

class AudioFile;

class AudioBuffer {
	public:
		AudioBuffer(quint32 inRequestedLength);

		typedef enum {
			eEmpty,
			eGotDecodedChunk,
			eGotConvertedChunk
		} AudioBufferState;
		AudioBufferState state();

		QByteArray &byteBuffer();

		bool prepareForConversion(SDL_AudioCVT *inOutCVT);
		bool prepareForDecoding();

		void setDecodedChunkLength(quint32 inDecodedChunkLength);
		void setConvertedChunkLength(quint32 inConvertedChunkLength);

		QByteArray *decodedChunkBuffer(quint32 &outLen);
		QByteArray *convertedChunkBuffer(quint32 &outLen);

		void reset();

		quint32 requestedLength();
		quint32 decodedChunkLength();
		quint32 convertedChunkLength();

		void setAudioFile(AudioFile *inAudioFile);
		AudioFile *audioFile();

	private:
		quint32 mRequestedLength;
		quint32 mDecodedChunkLength;
		quint32 mConvertedChunkLength;

		QByteArray mByteBuffer;

		AudioBufferState mState;

		AudioFile *mAudioFile;
};

#endif
