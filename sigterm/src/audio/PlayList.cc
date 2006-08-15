#include "PlayList.h"
#include "AudioFile.h"

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

void PlayList::setNextTrack(int inIndex) {
    mCurrentAudioFileIndex = inIndex;
}

void PlayList::finished(AudioFile *inAudioFile) {
    mCurrentAudioFileIndex++;
}

int PlayList::rowCount(const QModelIndex &parent) const {
    return mAudioFileList.size();
}

QVariant PlayList::data(const QModelIndex &index, int role) const {
    if (index.isValid() == false)
	return QVariant();

    if (role == Qt::DisplayRole) {
	return mAudioFileList[index.row()]->filePath();
    }

    return QVariant();
}
