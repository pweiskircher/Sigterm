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

	if (mCurrentAudioFileIndex < 0)
		mCurrentAudioFileIndex = mAudioFileList.size()-1;

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
		switch ((Columns)section) {
			case eIsPlaying:
				return "";
			case eTitle:
				return "Title";
			case eTotalTime:
				return "Total Time";
			case eTrackNumber:
				return "#";
			case eArtist:
				return "Artist";
			case eAlbum:
				return "Album";

			case eLastElement:
				break;
		}
	}

	return QVariant();
}

int PlayQueue::columnCount(const QModelIndex &parent) const {
	return eLastElement;
}

	int PlayQueue::rowCount(const QModelIndex &parent) const {
		if (!parent.isValid())
			return mAudioFileList.size();
		return 0;
	}

	QVariant PlayQueue::data(const QModelIndex &index, int role) const {
		if (index.isValid() == false)
			return QVariant();

		switch ((Columns)index.column()) {
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
					return mAudioFileList[index.row()]->metaData()->title();
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

			case eTrackNumber:
				if (role == Qt::DisplayRole) {
					return mAudioFileList[index.row()]->metaData()->trackNumber();
				}
				break;

			case eArtist:
				if (role == Qt::DisplayRole) {
					return mAudioFileList[index.row()]->metaData()->artist();
				}
				break;

			case eAlbum:
				if (role == Qt::DisplayRole) {
					return mAudioFileList[index.row()]->metaData()->album();
				}
				break;

			case eLastElement:
				break;
		}

		return QVariant();
	}

	bool PlayQueue::hasChildren(const QModelIndex &parent) const {
		if (parent.isValid() == false)
			return true;
		return false;
	}

void PlayQueue::nextTrack() {
	mCurrentAudioFileIndex++;
}

void PlayQueue::prevTrack() {
	mCurrentAudioFileIndex--;
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

void PlayQueue::removeTracks(QModelIndexList &inIndexes) {
	QModelIndex index;
	QListIterator<QModelIndex> it(inIndexes);

	QList<AudioFile *> list;

	while (it.hasNext()) {
		index = it.next();
		if (index.column() != 0) continue;

		list.append(mAudioFileList[index.row()]);
	}

	QListIterator<AudioFile *> afit(list);
	while (afit.hasNext()) {
		AudioFile *af = afit.next();
		// TODO: we should delete the item if its not in our library
		af->removeFromQueue();
	}
}

