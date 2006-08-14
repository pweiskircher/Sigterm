#ifndef _AUDIO_PROCESSOR_H
#define _AUDIO_PROCESSOR_H

#include <QThread>
#include <QMutex>

class AudioManager;
class AudioFile;
class PlayList;

class AudioProcessor : public QThread {
    public:
	AudioProcessor(AudioManager *inAudioManager);

	void run();

	void pause();
	void skipTrack();

    private:
	QMutex mMutex;
	AudioManager *mAudioManager;

	void processFile(PlayList *inPlayList, AudioFile *inFile);

	bool mPause;
	bool mSkipTrack;
};

#endif
