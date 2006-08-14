#ifndef _AUDIO_FILE_H
#define _AUDIO_FILE_H

#include <QtGlobal>
#include <QString>
#include <QByteArray>

#include "AudioFormat.h"
#include "AudioConverter.h"

#include <SDL.h>

class AudioManager;

class AudioFile {
    public:
	AudioFile(AudioManager *inAudioManager);
	virtual ~AudioFile();

	virtual bool open(const QString &inFilename) = 0;
	virtual bool close() = 0;
	virtual bool seekToTime(quint32 inMilliSeconds) = 0;

	bool getAudioChunk(QByteArray &outArray);

	AudioFormat &audioFormat();

	quint32 totalSize();
	quint32 currentPosition();

	AudioManager *audioManager();

    private:
	virtual bool getDecodedChunk(QByteArray &inOutArray) = 0;

	AudioFormat mAudioFormat;
	SDL_AudioCVT mCVT;
	bool mBuiltCVT;
	AudioManager *mAudioManager;
	AudioConverter mConverter;

    protected:
	quint32 mTotalSize;
	quint32 mCurrentPosition;
};

#endif
