#ifndef _AUDIO_FILE_H
#define _AUDIO_FILE_H

#include <QtGlobal>
#include <QString>

#include "AudioFormat.h"

#include <SDL.h>

class AudioManager;

class AudioFile {
    public:
	AudioFile(AudioManager *inAudioManager);
	virtual ~AudioFile();

	virtual bool open(const QString &inFilename) = 0;
	virtual bool close() = 0;

	virtual bool seekToTime(quint32 inMilliSeconds) = 0;
	virtual bool getDecodedChunk(char *inOutBuffer, quint32 &inOutLen) = 0;

	bool getAudioChunk(quint8 *inBuffer, int len);

	AudioFormat &audioFormat();

	quint32 totalSize();
	quint32 currentPosition();

	AudioManager *audioManager();

    private:
	AudioFormat mAudioFormat;
	SDL_AudioCVT mCVT;
	bool mBuiltCVT;
	AudioManager *mAudioManager;

    protected:
	quint32 mTotalSize;
	quint32 mCurrentPosition;
};

#endif
