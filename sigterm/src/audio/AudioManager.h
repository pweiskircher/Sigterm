#ifndef _AUDIO_MANAGER_H
#define _AUDIO_MANAGER_H

#include <QString>
#include <QMutex>

#include <SDL.h>

class AudioDecoder;

class AudioManager {
    public:
	AudioManager();

	void init();

	void fillBuffer(Uint8 *stream, int len);

	SDL_AudioSpec *hardwareSpec();

    private:
	SDL_AudioSpec mHardwareAudioSpec, mDesiredAudioSpec;

	// this needs to go away - just testing!
	AudioDecoder *mAudioDecoder;

	QMutex mAudioMutex;
};

#endif
