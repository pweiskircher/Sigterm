#include "PlayQueue.h"
#include "AudioFile.h"
#include "AudioManager.h"
#include "PlaylistM3u.h"

#include <QFileInfo>
#include <QPixmap>
#include <QIcon>
#include <QTextStream>

PlayQueue::PlayQueue(AudioManager *inAudioManager) {
	mCurrentAudioFileIndex = 0;
	mPlayingTrack = NULL;
	mAudioManager = inAudioManager;
}

void PlayQueue::addAudioFile(AudioFile *inAudioFile) {
	beginInsertRows(QModelIndex(), mAudioFileList.size(), mAudioFileList.size());
	mAudioFileList.append(inAudioFile);
	endInsertRows();

	connect(inAudioFile, SIGNAL(startedPlaying(AudioFile *)), SLOT(audioFileStartedPlaying(AudioFile *)));
	connect(inAudioFile, SIGNAL(stoppedPlaying(AudioFile *)), SLOT(audioFileStoppedPlaying(AudioFile *)));
}

void PlayQueue::removeAudioFile(AudioFile *inAudioFile) {
	bool needToSkip = false;
	if (currentFile() == inAudioFile)
		needToSkip = true;

	int index = mAudioFileList.indexOf(inAudioFile);
	if (index != -1) {
		beginRemoveRows(QModelIndex(), index, index);
		mAudioFileList.removeAt(index);
		endRemoveRows();
	}

	if (mAudioFileList.size() == 0)
		mAudioManager->setPause(true);
	else if (needToSkip)
		mAudioManager->skipTrack();
}

AudioFile *PlayQueue::currentFile() {
	QMutexLocker locker(&mMutex);
	if (mAudioFileList.size() == 0)
		return NULL;

	if (mCurrentAudioFileIndex >= mAudioFileList.size())
		mCurrentAudioFileIndex = 0;

	if (mCurrentAudioFileIndex < 0)
		mCurrentAudioFileIndex = mAudioFileList.size()-1;

	return mAudioFileList[mCurrentAudioFileIndex];
}

AudioFile *PlayQueue::playingTrack() {
	QMutexLocker locker(&mMutex);
	return mPlayingTrack;
}

void PlayQueue::setNextTrack(int inIndex) {
	QMutexLocker locker(&mMutex);
	mCurrentAudioFileIndex = inIndex;
}

void PlayQueue::setNextTrack(AudioFile *inAudioFile) {
	mMutex.lock();
	int index = mAudioFileList.indexOf(inAudioFile);
	mMutex.unlock();
	if (index != -1)
		setNextTrack(index);
}

void PlayQueue::finished(AudioFile *inAudioFile) {
	QMutexLocker locker(&mMutex);
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
				return Qt::AlignRight;
			}
			break;

		case eTrackNumber:
			if (role == Qt::DisplayRole) {
				return mAudioFileList[index.row()]->metaData()->trackNumber();
			} else if (role == Qt::TextAlignmentRole) {
				return Qt::AlignRight;
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
	QMutexLocker locker(&mMutex);
	mCurrentAudioFileIndex++;
}

void PlayQueue::prevTrack() {
	QMutexLocker locker(&mMutex);
	mCurrentAudioFileIndex--;
}


void PlayQueue::audioFileStartedPlaying(AudioFile *inAudioFile) {
	QMutexLocker locker(&mMutex);

	int index = mAudioFileList.indexOf(inAudioFile);
	if (index != -1)
		emit dataChanged(createIndex(index, 0), createIndex(index, 0));
	mPlayingTrack = inAudioFile;
}

void PlayQueue::audioFileStoppedPlaying(AudioFile *inAudioFile) {
	QMutexLocker locker(&mMutex);

	int index = mAudioFileList.indexOf(inAudioFile);
	if (index != -1)
		emit dataChanged(createIndex(index, 0), createIndex(index, 0));

	if (mPlayingTrack == inAudioFile)
		mPlayingTrack = NULL;
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

bool PlayQueue::clear() {
	mAudioManager->setPause(true);
	QListIterator<AudioFile *> afit(mAudioFileList);
	while (afit.hasNext()) {
		AudioFile *af = afit.next();
		// TODO: we should delete the item if its not in our library
		af->removeFromQueue();
	}
}

bool PlayQueue::saveToFile(QString fileName) {
	PlaylistM3u playlist(fileName, mAudioManager);
	playlist.save(mAudioFileList);
	return true;
}

bool PlayQueue::loadFromFile(QString fileName) {
	clear();
	return appendFromFile(fileName);
}

bool PlayQueue::appendFromFile(QString fileName) {
	QList<AudioFile *> myList;
	PlaylistM3u playlist(fileName, mAudioManager);
	playlist.load(myList);
	
	QListIterator<AudioFile *> it(myList);
	while(it.hasNext()) {
		AudioFile *af = it.next();
		af->addToQueue();
	}
	return true;
}

