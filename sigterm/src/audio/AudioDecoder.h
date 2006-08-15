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
class AudioBuffer;

class AudioDecoder {
    public:
	AudioDecoder(AudioFile *inAudioFile, AudioManager *inAudioManager);
	virtual ~AudioDecoder();

	virtual AudioDecoder *createAudioDecoder(AudioFile *inAudioFile, AudioManager *inAudioManager) = 0;
	virtual bool canDecode(const QString &inFilePath) = 0;

	virtual bool open() = 0;
	virtual bool close() = 0;
	virtual bool seekToTime(quint32 inMilliSeconds) = 0;

	bool opened();

	typedef enum {
	    eContinue,
	    eStop
	} DecodingStatus;
	DecodingStatus getAudioChunk(AudioBuffer *inOutAudioBuffer);

	AudioFormat &audioFormat();

	quint32 totalSize();
	quint32 currentPosition();

	AudioManager *audioManager();
	AudioFile *audioFile();

    private:
	virtual DecodingStatus getDecodedChunk(AudioBuffer *inOutAudioBuffer) = 0;

	AudioFormat mAudioFormat;
	SDL_AudioCVT mCVT;
	bool mBuiltCVT;
	AudioManager *mAudioManager;
	AudioConverter mConverter;
	AudioFile *mAudioFile;

	bool mOpened;

    protected:
	void setOpened(bool inValue);

	quint32 mTotalSize;
	quint32 mCurrentPosition;
};

#endif
