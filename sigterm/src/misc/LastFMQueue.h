#ifndef _LAST_FM_QUEUE_H
#define _LAST_FM_QUEUE_H

#include <QAbstractItemModel>
#include <QSettings>

class LastFMEntry {
	public:
	bool load(QSettings &inSettings, const QString &inSection);
	bool save(QSettings &inSettings, const QString &inSection);
	QString toGetRequest(int count);

	QString mArtist;
	QString mTitle;
	QString mAlbum;
	QString mMusicBrainzId;
	QString mSeconds;
	QString mDatePlayed;
};

class QTreeView;

class LastFMQueue : public QAbstractTableModel {
	public:
		LastFMQueue();

		void append(LastFMEntry *inEntry);
		void remove(LastFMEntry *inEntry);

		void setupUi(QTreeView *inView);

		typedef enum {
			eArtist,
			eTitle,
			eAlbum,
			eMusicBrainzId,
			eLength,
			eDatePlayed,
			eLastElement
		} Columns;

		QVariant headerData(int section, Qt::Orientation orientation, int role) const;
		int columnCount(const QModelIndex &parent) const;
		int rowCount(const QModelIndex &parent) const;
		QVariant data(const QModelIndex &index, int role) const;
		bool hasChildren(const QModelIndex &parent) const;

	private:
		QList<LastFMEntry *> mEntries;
};

#endif
