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
class AudioFormat;

class AudioManager : public QObject {
	Q_OBJECT

	public:
		AudioManager();
		~AudioManager();

		void init();
		void setPause(bool inPause);
		void togglePause();
		void skipTrack();
		void quit();
		bool paused();
		void fillBuffer(Uint8 *stream, int len);

		void nextTrack();
		void prevTrack();

		SDL_AudioSpec *hardwareSpec();
		AudioFormat *hardwareFormat();
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
		AudioFormat *mHardwareAudioFormat;

		QMutex mAudioMutex;
		QWaitCondition mAudioProcessorWaitCondition;
		PlayQueue mPlayQueue;
		bool mPaused;
		QList<AudioDecoder *> mAudioDecoderList;

		QByteArray mSDLBuffer;
};

#endif
