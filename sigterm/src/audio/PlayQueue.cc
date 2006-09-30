#include "PlayQueue.h"
#include "AudioFile.h"
#include "AudioManager.h"
#include "PlaylistM3u.h"

#include <QFileInfo>
#include <QPixmap>
#include <QIcon>
#include <QTextStream>
#include <QMimeData>
#include <QtDebug>
#include <QUrl>

PlayQueue::PlayQueue(AudioManager *inAudioManager) {
	mCurrentAudioFileIndex = 0;
	mPlayingTrack = NULL;
	mAudioManager = inAudioManager;

	connect(mAudioManager, SIGNAL(audioPaused(bool)), SLOT(audioPaused(bool)));
}

void PlayQueue::addAudioFile(AudioFile *inAudioFile) {
	beginInsertRows(QModelIndex(), mAudioFileList.size(), mAudioFileList.size());
	mAudioFileList.append(inAudioFile);
	endInsertRows();

	connect(inAudioFile, SIGNAL(startedPlaying(AudioFile *)), SLOT(audioFileStartedPlaying(AudioFile *)));
	connect(inAudioFile, SIGNAL(stoppedPlaying(AudioFile *, quint32)), SLOT(audioFileStoppedPlaying(AudioFile *, quint32)));
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

AudioFile *PlayQueue::currentFile() const {
	QMutexLocker locker(&mMutex);
	if (mAudioFileList.size() == 0)
		return NULL;

	Q_ASSERT(mCurrentAudioFileIndex >= 0);
	Q_ASSERT(mCurrentAudioFileIndex < mAudioFileList.size());

	return mAudioFileList[mCurrentAudioFileIndex];
}

int PlayQueue::currentFileId() {
	return mCurrentAudioFileIndex;
}

AudioFile *PlayQueue::playingTrack() {
	return mPlayingTrack;
}

void PlayQueue::setNextTrack(int inIndex) {
	if (mAudioFileList.size() < inIndex || inIndex < 0)
		return;
	mCurrentAudioFileIndex = inIndex;
}

void PlayQueue::setNextTrack(AudioFile *inAudioFile) {
	int index = mAudioFileList.indexOf(inAudioFile);
	if (index != -1)
		setNextTrack(index);
}

void PlayQueue::setStartTime(quint32 inMilliseconds) {
	AudioFile *af = currentFile();

	if (!af)
		return;

	af->seekToTime(inMilliseconds);
}

void PlayQueue::finished(AudioFile *inAudioFile) {
	QMutexLocker locker(&mMutex);
	nextTrack();
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
				if (mAudioFileList[index.row()]->isPlaying() && mAudioManager->paused() == false) {
					QPixmap p(8,8);
					p.fill(Qt::blue);
					return QIcon(p);
				} else if (mAudioFileList[index.row()] == currentFile()) {
					QPixmap p(8,8);
					p.fill(Qt::gray);
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
	mCurrentAudioFileIndex++;

	if (mCurrentAudioFileIndex >= mAudioFileList.size())
		mCurrentAudioFileIndex = 0;
}

void PlayQueue::prevTrack() {
	mCurrentAudioFileIndex--;

	if (mCurrentAudioFileIndex < 0)
		mCurrentAudioFileIndex = mAudioFileList.size()-1;
}

void PlayQueue::audioFileStartedPlaying(AudioFile *inAudioFile) {
	int index = mAudioFileList.indexOf(inAudioFile);
	if (index != -1)
		emit dataChanged(createIndex(index, 0), createIndex(index, 0));
	mPlayingTrack = inAudioFile;

	emit audioFileStarted(inAudioFile);
}

void PlayQueue::audioFileStoppedPlaying(AudioFile *inAudioFile, quint32 inTimePlayed) {
	int index = mAudioFileList.indexOf(inAudioFile);
	if (index != -1)
		emit dataChanged(createIndex(index, 0), createIndex(index, 0));

	if (mPlayingTrack == inAudioFile)
		mPlayingTrack = NULL;

	emit audioFileStopped(inAudioFile, inTimePlayed);
}

void PlayQueue::audioPaused(bool inPauseStatus) {
	int index = mAudioFileList.indexOf(currentFile());
	emit dataChanged(createIndex(index, 0), createIndex(index, 0));
}

bool PlayQueue::removeTracks(QList<AudioFile*> &inList) {
	QListIterator<AudioFile *> afit(inList);
	while (afit.hasNext()) {
		AudioFile *af = afit.next();
		// TODO: we should delete the item if its not in our library
		af->removeFromQueue();
	}
	return true;
}

bool PlayQueue::removeTracks(QModelIndexList &inIndexes) {
	QModelIndex index;
	QListIterator<QModelIndex> it(inIndexes);

	QList<AudioFile *> list;

	while (it.hasNext()) {
		index = it.next();
		if (index.column() != 0) continue;

		list.append(mAudioFileList[index.row()]);
	}
	return removeTracks(list);
}

bool PlayQueue::removeRows(int rowStart, int count, const QModelIndex & parent) {
	qDebug("::removeRows called: %d, %d", rowStart, count);
	
	QList<AudioFile *> list;
	for(int row=rowStart; row<(rowStart+count); row++) {
		list.append(mAudioFileList[row]);
	}
	return removeTracks(list);
}
bool PlayQueue::insertRows(int rowStart, int count, const QModelIndex & parent) {
	qDebug("::insertRows called: %d, %d", rowStart, count);
	return true;
}

void PlayQueue::clear() {
	mAudioManager->setPause(true);
	removeTracks(mAudioFileList);
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

Qt::DropActions PlayQueue::supportedDropActions() const {
	return Qt::CopyAction | Qt::MoveAction;
}

Qt::ItemFlags PlayQueue::flags(const QModelIndex &index) const {
	Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);
	if (index.isValid())
		return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
	else
		return Qt::ItemIsDropEnabled | defaultFlags;
}

QStringList PlayQueue::mimeTypes() const {
	QStringList types;
	types << "text/plain";
	types << "text/uri-list";
	return types;
}

QMimeData* PlayQueue::mimeData(const QModelIndexList &indexes) const {
	QMimeData *mimeData = new QMimeData();
	QByteArray encodedData;

	QTextStream stream(&encodedData, QIODevice::WriteOnly);
	
	foreach (QModelIndex index, indexes) {
		if (index.column() != 0)
			continue;

		if (index.isValid()) {
			QUrl url = QUrl::fromLocalFile(mAudioFileList[index.row()]->filePath());
			QString text = url.toEncoded();

			stream << text;
			qDebug() << text;
		}
	}
	qDebug() << "::mimeData: " << encodedData;
	mimeData->setData("text/uri-list", encodedData);
	return mimeData;
}

bool PlayQueue::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {
	if (action == Qt::IgnoreAction)
		return true;

	qDebug() << "::dropMimeData " << data->formats().join(" ");

	if (!data->hasFormat("text/plain") && !data->hasFormat("text/uri-list"))
		return false;

	QByteArray encodedData;
	
	if (data->hasFormat("text/plain"))
		encodedData = data->data("text/plain");

	if (data->hasFormat("text/uri-list"))
		encodedData = data->data("text/uri-list");
	
	qDebug() << encodedData;
	
	QTextStream stream(&encodedData, QIODevice::ReadOnly);
	while(!stream.atEnd()) {
		QString itemFileName;
		stream >> itemFileName;
		qDebug() << itemFileName;
		if (itemFileName.startsWith("file://")) {
			QUrl url(itemFileName);
			itemFileName = url.toLocalFile();
		}
		
		QFileInfo fi(itemFileName);
		if (!fi.isRelative() && fi.exists()) {
			AudioFile *af = new AudioFile(fi.filePath(), mAudioManager);
			if (af->decoder())
				af->addToQueue();
			else
				delete af;
		}
	}
	
	return true;
}


