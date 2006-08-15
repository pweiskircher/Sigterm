#ifndef _AUDIO_MANAGER_H
#define _AUDIO_MANAGER_H

#include <QObject>
#include <QString>
#include <QMutex>
#include <QWaitCondition>

#include <SDL.h>

#include "AudioStorage.h"
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
	void skipTrack();
	void quit();

	bool paused();

	void fillBuffer(Uint8 *stream, int len);

	SDL_AudioSpec *hardwareSpec();

	AudioStorage *audioStorage();
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

	AudioStorage mAudioStorage;
	PlayList *mCurrentPlayList;

	bool mPaused;
};

#endif
