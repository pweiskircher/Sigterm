#ifndef _PLAY_LIST_H
#define _PLAY_LIST_H

#include <QAbstractItemModel>
#include <QMutex>

class AudioFile;
class AudioManager;

class PlayQueue : public QAbstractTableModel {
	Q_OBJECT

	public:
		PlayQueue(AudioManager *inAudioManager);

		typedef enum {
			eIsPlaying = 0,
			eTrackNumber,
			eArtist,
			eAlbum,
			eTitle,
			eTotalTime,
			eLastElement
		} Columns;

		AudioFile *currentFile() const;
		int currentFileId();
		AudioFile *playingTrack();

		void setNextTrack(int inIndex);
		void setNextTrack(AudioFile *inAudioFile);
		void setStartTime(quint32 inMilliseconds);

		void finished(AudioFile *inAudioFile);

		QVariant headerData(int section, Qt::Orientation orientation, int role) const;
		int columnCount(const QModelIndex &parent ) const;
		int rowCount(const QModelIndex &parent) const;
		QVariant data(const QModelIndex &index, int role) const;
		bool hasChildren(const QModelIndex &parent) const;
		Qt::DropActions supportedDropActions() const;
		Qt::ItemFlags flags(const QModelIndex &index) const;
		bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex());
		bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());
		QStringList mimeTypes() const;
		QMimeData* mimeData(const QModelIndexList &indexes) const;
		bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
		

		void nextTrack();
		void prevTrack();

		bool removeTracks(QModelIndexList &inIndexes);
		bool removeTracks(QList<AudioFile*> &inList);

		bool appendFromFile(const QString fileName);
		bool loadFromFile(const QString fileName);
		bool saveToFile(const QString fileName);
		void clear();

	signals:
		void audioFileStarted(AudioFile *inAudioFile);
		void audioFileStopped(AudioFile *inAudioFile, quint32 inTimePlayed);

	private slots:
		void audioFileStartedPlaying(AudioFile *inAudioFile);
		void audioFileStoppedPlaying(AudioFile *inAudioFile, quint32 inTimePlayed);
		void audioPaused(bool inPauseStatus);

	private:
		friend class AudioFile;
		void addAudioFile(AudioFile *inAudioFile);
		void removeAudioFile(AudioFile *inAudioFile);

		QList<AudioFile *> mAudioFileList;
		int mCurrentAudioFileIndex;

		AudioFile *mPlayingTrack;
		mutable QMutex mMutex;
		AudioManager *mAudioManager;
};

#endif
