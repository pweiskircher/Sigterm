#ifndef _AUDIO_MANAGER_H
#define _AUDIO_MANAGER_H

#include <QString>
#include <QMutex>
#include <QThread>

#include <SDL.h>

class AudioFile;

class AudioManager : public QThread {
    public:
	AudioManager();

	void run();

	void fillBuffer(Uint8 *stream, int len);

	SDL_AudioSpec *hardwareSpec();

    private:
	SDL_AudioSpec mHardwareAudioSpec, mDesiredAudioSpec;

	// this needs to go away - just testing!
	AudioFile *mAudioFile;

	QMutex mAudioMutex;
};

#endif
