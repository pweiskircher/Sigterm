#include "PlayList.h"
#include "AudioFile.h"

#include <QFileInfo>
#include <QPixmap>
#include <QIcon>

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

QVariant PlayList::headerData(int section, Qt::Orientation orientation, int role) const {
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

int PlayList::columnCount(const QModelIndex &parent) const {
    return 3;
}

int PlayList::rowCount(const QModelIndex &parent) const {
    return mAudioFileList.size();
}

QVariant PlayList::data(const QModelIndex &index, int role) const {
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
