#ifndef _AUDIO_LIBRARY_H
#define _AUDIO_LIBRARY_H

#include <QList>

class AudioManager;
class AudioFile;

class AudioLibrary {
	public:
		AudioLibrary(AudioManager *inAudioManager);

		void addAudioFile(AudioFile *inAudioFile);
		void removeAudioFile(AudioFile *inAudioFile);

	private:
		QList<AudioFile *> mAudioFileList;
		AudioManager *mAudioManager;
};

#endif
