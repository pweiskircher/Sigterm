#ifndef _AUDIO_PROCESSOR_H
#define _AUDIO_PROCESSOR_H

#include <QThread>
#include <QMutex>

class AudioManager;
class AudioFile;
class PlayQueue;

class AudioProcessor : public QThread {
    Q_OBJECT

    public:
	AudioProcessor(AudioManager *inAudioManager);

	void run();

	void pause();
	void skipTrack();
	void quit();

    signals:
	void startedPlaying(AudioFile *inFile);
	void paused();

    private:
	QMutex mMutex;
	AudioManager *mAudioManager;

	void processFile(PlayQueue *inPlayQueue, AudioFile *inFile);

	bool mPause;
	bool mSkipTrack;
	bool mQuit;
};

#endif
