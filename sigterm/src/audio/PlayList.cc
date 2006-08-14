#include "PlayList.h"

PlayList::PlayList() {
}

void PlayList::add(AudioFile *inAudioFile) {
    mAudioFileList.append(inAudioFile);
}

AudioFile *PlayList::currentFile() {
    return mAudioFileList.first();
}

int PlayList::rowCount(const QModelIndex &parent) const {
    return 0;
}

QVariant PlayList::data(const QModelIndex &index, int role) const {
    return QVariant();
}
