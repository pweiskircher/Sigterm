#ifndef _AUDIO_MANAGER_H
#define _AUDIO_MANAGER_H

#include <QObject>
#include <QString>
#include <QMutex>
#include <QWaitCondition>

#include <SDL.h>

#include "AudioBuffer.h"
#include "AudioProcessor.h"

class AudioDecoder;
class PlayList;

class AudioManager : public QObject {
    Q_OBJECT

    public:
	AudioManager();

	void init();
	void setPause(bool inPause);
	void togglePause();

	void fillBuffer(Uint8 *stream, int len);

	SDL_AudioSpec *hardwareSpec();

	AudioBuffer *audioBuffer();
	QWaitCondition *audioProcessorWaitCondition();

	PlayList *currentPlayList();

    signals:
	void audioPaused(bool inPaused);

    private slots:
	void audioProcessorPaused();

    private:
	SDL_AudioSpec mHardwareAudioSpec, mDesiredAudioSpec;

	QMutex mAudioMutex;

	QWaitCondition mAudioProcessorWaitCondition;
	AudioProcessor mAudioProcessor;

	AudioBuffer mAudioBuffer;
	PlayList *mCurrentPlayList;

	bool mPaused;
};

#endif
