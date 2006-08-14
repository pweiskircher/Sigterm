#ifndef _AUDIO_PROCESSOR_H
#define _AUDIO_PROCESSOR_H

#include <QThread>
#include <QMutex>

class AudioManager;

class AudioProcessor : public QThread {
    public:
	AudioProcessor(AudioManager *inAudioManager);

	void run();

    private:
	QMutex mMutex;
	AudioManager *mAudioManager;
};

#endif
