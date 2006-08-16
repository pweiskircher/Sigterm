#include "PlayQueue.h"
#include "AudioFile.h"

#include <QFileInfo>
#include <QPixmap>
#include <QIcon>

PlayQueue::PlayQueue() {
    mCurrentAudioFileIndex = 0;
}

void PlayQueue::addAudioFile(AudioFile *inAudioFile) {
    beginInsertRows(QModelIndex(), mAudioFileList.size(), mAudioFileList.size());
    mAudioFileList.append(inAudioFile);
    endInsertRows();

    connect(inAudioFile, SIGNAL(startedPlaying(AudioFile *)), SLOT(audioFileStartedPlaying(AudioFile *)));
    connect(inAudioFile, SIGNAL(stoppedPlaying(AudioFile *)), SLOT(audioFileStoppedPlaying(AudioFile *)));
}

void PlayQueue::removeAudioFile(AudioFile *inAudioFile) {
    int index = mAudioFileList.indexOf(inAudioFile);
    if (index != -1) {
	beginRemoveRows(QModelIndex(), index, index);
	mAudioFileList.removeAt(index);
	endRemoveRows();
    }
}

AudioFile *PlayQueue::currentFile() {
    if (mAudioFileList.size() == 0)
	return NULL;

    if (mCurrentAudioFileIndex >= mAudioFileList.size())
	mCurrentAudioFileIndex = 0;

    return mAudioFileList[mCurrentAudioFileIndex];
}

void PlayQueue::setNextTrack(int inIndex) {
    mCurrentAudioFileIndex = inIndex;
}

void PlayQueue::finished(AudioFile *inAudioFile) {
    mCurrentAudioFileIndex++;
}

QVariant PlayQueue::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole) {
	switch (section) {
	    case eIsPlaying:
		return "";
	    case eTitle:
		return "Title";
	    case eTotalTime:
		return "seconds";
	}
    }

    return QVariant();
}

int PlayQueue::columnCount(const QModelIndex &parent) const {
    return 3;
}

int PlayQueue::rowCount(const QModelIndex &parent) const {
    return mAudioFileList.size();
}

QVariant PlayQueue::data(const QModelIndex &index, int role) const {
    if (index.isValid() == false)
	return QVariant();

    switch (index.column()) {
	case eIsPlaying:
	    if (role == Qt::DecorationRole) {
		if (mAudioFileList[index.row()]->isPlaying()) {
		    QPixmap p(8,8);
		    p.fill(Qt::blue);
		    return QIcon(p);
		}
	    }
	    break;

	case eTitle:
	    if (role == Qt::DisplayRole) {
		QFileInfo info(mAudioFileList[index.row()]->filePath());
		return info.baseName();
	    }
	    break;

	case eTotalTime:
	    if (role == Qt::DisplayRole) {
		QString help;
		quint32 length = mAudioFileList[index.row()]->timeTotal();
		help.sprintf("%d:%02d", length/60, length%60);
		return help;
	    } else if (role == Qt::TextAlignmentRole) {
		return Qt::AlignCenter;
	    }
	    break;
    }

    return QVariant();
}

bool PlayQueue::hasChildren(const QModelIndex &parent) const {
    if (parent.isValid() == false)
	return true;
    return false;
}

void PlayQueue::audioFileStartedPlaying(AudioFile *inAudioFile) {
    int index = mAudioFileList.indexOf(inAudioFile);
    if (index != -1)
	emit dataChanged(createIndex(index, 0), createIndex(index, 0));
}

void PlayQueue::audioFileStoppedPlaying(AudioFile *inAudioFile) {
    int index = mAudioFileList.indexOf(inAudioFile);
    if (index != -1)
	emit dataChanged(createIndex(index, 0), createIndex(index, 0));
}

