#include "PlayList.h"

PlayList::PlayList() {
    mCurrentAudioFileIndex = 0;
}

void PlayList::add(AudioFile *inAudioFile) {
    mAudioFileList.append(inAudioFile);
}

AudioFile *PlayList::currentFile() {
    if (mAudioFileList.size() == 0)
	return NULL;

    if (mCurrentAudioFileIndex >= mAudioFileList.size())
	mCurrentAudioFileIndex = 0;

    return mAudioFileList[mCurrentAudioFileIndex];
}

void PlayList::finished(AudioFile *inAudioFile) {
    mCurrentAudioFileIndex++;
}

int PlayList::rowCount(const QModelIndex &parent) const {
    return 0;
}

QVariant PlayList::data(const QModelIndex &index, int role) const {
    return QVariant();
}
