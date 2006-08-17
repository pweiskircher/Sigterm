#include "AudioLibrary.h"

AudioLibrary::AudioLibrary(AudioManager *inAudioManager) {
	mAudioManager = inAudioManager;
}

void AudioLibrary::addAudioFile(AudioFile *inAudioFile) {
	mAudioFileList.append(inAudioFile);
}

void AudioLibrary::removeAudioFile(AudioFile *inAudioFile) {
	int index = mAudioFileList.indexOf(inAudioFile);
	if (index != -1)
		mAudioFileList.removeAt(index);
}
