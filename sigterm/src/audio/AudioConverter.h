#ifndef _AUDIO_CONVERTER_H
#define _AUDIO_CONVERTER_H

#include <QByteArray>

#include <SDL.h>

class AudioFormat;
class AudioManager;

class AudioConverter {
    public:
	AudioConverter(AudioManager *inAudioManager);

	bool setSourceFormat(AudioFormat *inFormat);
	bool convert(QByteArray &inOutDataArray);

    private:
	AudioManager *mAudioManager;
	SDL_AudioCVT mCVT;
};

#endif
