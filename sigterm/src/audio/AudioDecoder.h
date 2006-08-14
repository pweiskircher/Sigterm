#ifndef _AUDIO_DECODER_H
#define _AUDIO_DECODER_H

#include <QtGlobal>
#include <QString>
#include <QByteArray>

#include "AudioFormat.h"
#include "AudioConverter.h"

#include <SDL.h>

class AudioManager;
class AudioFile;

class AudioDecoder {
    public:
	AudioDecoder(AudioFile *inAudioFile, AudioManager *inAudioManager);
	virtual ~AudioDecoder();

	virtual bool open() = 0;
	virtual bool close() = 0;
	virtual bool seekToTime(quint32 inMilliSeconds) = 0;

	typedef enum {
	    eSuccess,
	    eEOF,
	    eError
	} DecodingStatus;
	DecodingStatus getAudioChunk(QByteArray &outArray);

	AudioFormat &audioFormat();

	quint32 totalSize();
	quint32 currentPosition();

	AudioManager *audioManager();
	AudioFile *audioFile();

    private:
	virtual DecodingStatus getDecodedChunk(QByteArray &inOutArray) = 0;

	virtual bool canDecode(const QString &inFilePath) = 0;

	AudioFormat mAudioFormat;
	SDL_AudioCVT mCVT;
	bool mBuiltCVT;
	AudioManager *mAudioManager;
	AudioConverter mConverter;
	AudioFile *mAudioFile;

    protected:
	quint32 mTotalSize;
	quint32 mCurrentPosition;
};

#endif
