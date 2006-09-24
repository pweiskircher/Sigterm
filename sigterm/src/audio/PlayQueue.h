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

		AudioFile *currentFile();
		AudioFile *playingTrack();

		void setNextTrack(int inIndex);
		void setNextTrack(AudioFile *inAudioFile);

		void finished(AudioFile *inAudioFile);

		QVariant headerData(int section, Qt::Orientation orientation, int role) const;
		int columnCount(const QModelIndex &parent ) const;
		int rowCount(const QModelIndex &parent) const;
		QVariant data(const QModelIndex &index, int role) const;
		bool hasChildren(const QModelIndex &parent) const;

		void nextTrack();
		void prevTrack();

		void removeTracks(QModelIndexList &inIndexes);

		bool appendFromFile(const QString fileName);
		bool loadFromFile(const QString fileName);
		bool saveToFile(const QString fileName);
		bool clear();

	private slots:
		void audioFileStartedPlaying(AudioFile *inAudioFile);
		void audioFileStoppedPlaying(AudioFile *inAudioFile);

	private:
		friend class AudioFile;
		void addAudioFile(AudioFile *inAudioFile);
		void removeAudioFile(AudioFile *inAudioFile);

		QList<AudioFile *> mAudioFileList;
		int mCurrentAudioFileIndex;

		AudioFile *mPlayingTrack;
		QMutex mMutex;
		AudioManager *mAudioManager;
};

#endif
