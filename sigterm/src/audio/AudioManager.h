#ifndef _AUDIO_MANAGER_H
#define _AUDIO_MANAGER_H

#include <QObject>
#include <QString>
#include <QMutex>
#include <QWaitCondition>

#include <SDL.h>

#include "AudioStorage.h"
#include "AudioLibrary.h"
#include "AudioProcessor.h"
#include "PlayQueue.h"

class AudioDecoder;

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
	AudioLibrary *audioLibrary();
	QWaitCondition *audioProcessorWaitCondition();
	PlayQueue *playQueue();

	AudioDecoder *createAudioDecoder(AudioFile *inAudioFile);

    signals:
	void audioPaused(bool inPaused);

    private slots:
	void audioProcessorPaused();

    private:
	AudioProcessor mAudioProcessor;
	AudioStorage mAudioStorage;
	AudioLibrary mAudioLibrary;

	SDL_AudioSpec mHardwareAudioSpec, mDesiredAudioSpec;

	QMutex mAudioMutex;
	QWaitCondition mAudioProcessorWaitCondition;
	PlayQueue mPlayQueue;
	bool mPaused;
	QList<AudioDecoder *> mAudioDecoderList;
};

#endif
