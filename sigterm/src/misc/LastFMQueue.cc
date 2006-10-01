#include "LastFMQueue.h"
#include <QTreeView>
#include <QHeaderView>
#include <QDateTime>

bool LastFMEntry::load(QSettings &inSettings, const QString &inSection) {
	mArtist = inSettings.value(inSection + "/Artist").toString();
	mTitle = inSettings.value(inSection + "/Title").toString();
	mAlbum = inSettings.value(inSection + "/Album").toString();
	mMusicBrainzId = inSettings.value(inSection + "/MusicBrainzId").toString();
	mSeconds = inSettings.value(inSection + "/Seconds").toString();
	mDatePlayed = inSettings.value(inSection + "/DatePlayed").toString();

	if (mArtist.isEmpty() || mTitle.isEmpty() || mArtist.isEmpty() || mSeconds.isEmpty() || mDatePlayed.isEmpty())
		return false;

	return true;
}

bool LastFMEntry::save(QSettings &inSettings, const QString &inSection) {
	inSettings.setValue(inSection + "/Artist", mArtist);
	inSettings.setValue(inSection + "/Title", mTitle);
	inSettings.setValue(inSection + "/Album", mAlbum);
	inSettings.setValue(inSection + "/MusicBrainzId", mMusicBrainzId);
	inSettings.setValue(inSection + "/Seconds", mSeconds);
	inSettings.setValue(inSection + "/DatePlayed", mDatePlayed);

	return true;
}

QString LastFMEntry::toGetRequest(int count) {
	QString s;

	s += QString("&a[%1]=%2").arg(count).arg(mArtist);
	s += QString("&t[%1]=%2").arg(count).arg(mTitle);
	s += QString("&b[%1]=%2").arg(count).arg(mAlbum);
	s += QString("&m[%1]=%2").arg(count).arg(mMusicBrainzId);
	s += QString("&l[%1]=%2").arg(count).arg(mSeconds);
	s += QString("&i[%1]=%2").arg(count).arg(mDatePlayed);

	return s;
}


LastFMQueue::LastFMQueue() {
}

void LastFMQueue::append(LastFMEntry *inEntry) {
	beginInsertRows(QModelIndex(), mEntries.size(), mEntries.size());
	mEntries.append(inEntry);
	endInsertRows();
}

void LastFMQueue::remove(LastFMEntry *inEntry) {
	int index = mEntries.indexOf(inEntry);
	if (index != -1) {
		beginRemoveRows(QModelIndex(), index, index);
		mEntries.removeAt(index);
		endRemoveRows();
	}
}

void LastFMQueue::setupUi(QTreeView *inView) {
	inView->header()->resizeSection(eArtist, 100);
	inView->header()->resizeSection(eTitle, 150);
	inView->header()->setResizeMode(eTitle, QHeaderView::Stretch);
	inView->header()->resizeSection(eAlbum, 100);
	inView->header()->setSectionHidden(eMusicBrainzId, true);
	inView->header()->setSectionHidden(eLength, true);
	inView->header()->resizeSection(eDatePlayed, 100);
	inView->header()->setStretchLastSection(false);
}

QVariant LastFMQueue::headerData(int section, Qt::Orientation orientation, int role) const {
	if (role == Qt::DisplayRole) {
		switch ((Columns)section) {
			case eArtist:
				return "Artist";
			case eTitle:
				return "Title";
			case eAlbum:
				return "Album";
			case eMusicBrainzId:
				return "MusicBrainzId";
			case eLength:
				return "Length";
			case eDatePlayed:
				return "Date Played";
			case eLastElement:
				break;
		}
	}

	return QVariant();
}

int LastFMQueue::columnCount(const QModelIndex &parent) const {
	return eLastElement;
}

int LastFMQueue::rowCount(const QModelIndex &parent) const {
	if (!parent.isValid())
		return mEntries.size();
	return 0;
}

QVariant LastFMQueue::data(const QModelIndex &index, int role) const {
	if (index.isValid() == false)
		return QVariant();

	switch ((Columns)index.column()) {
		case eArtist:
			if (role == Qt::DisplayRole) {
				return mEntries[index.row()]->mArtist;
			}
			break;
		case eTitle:
			if (role == Qt::DisplayRole) {
				return mEntries[index.row()]->mTitle;
			}
			break;
		case eAlbum:
			if (role == Qt::DisplayRole) {
				return mEntries[index.row()]->mAlbum;
			}
			break;
		case eMusicBrainzId:
			if (role == Qt::DisplayRole) {
				return mEntries[index.row()]->mMusicBrainzId;
			}
			break;
		case eLength:
			if (role == Qt::DisplayRole) {
				return mEntries[index.row()]->mSeconds;
			}
			break;
		case eDatePlayed:
			if (role == Qt::DisplayRole) {
				QDateTime dt = QDateTime::fromString(mEntries[index.row()]->mDatePlayed, "yyyy-MM-dd hh:mm:ss");
				return dt;
			}
			break;
		case eLastElement:
			break;
	}

	return QVariant();
}

bool LastFMQueue::hasChildren(const QModelIndex &parent) const {
	if (parent.isValid() == false)
		return true;
	return false;
}

