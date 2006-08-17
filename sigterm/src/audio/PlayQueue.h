#ifndef _PLAY_LIST_H
#define _PLAY_LIST_H

#include <QAbstractItemModel>

class AudioFile;

class PlayQueue : public QAbstractTableModel {
    Q_OBJECT

    public:
	PlayQueue();

	enum {
	    eIsPlaying = 0,
	    eTitle,
	    eTotalTime
	};

	AudioFile *currentFile();

	void setNextTrack(int inIndex);

	void finished(AudioFile *inAudioFile);

	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	int columnCount(const QModelIndex &parent ) const;
	int rowCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role) const;
	bool hasChildren(const QModelIndex &parent) const;

	void nextTrack();
	void prevTrack();

	void removeTracks(QModelIndexList &inIndexes);

    private slots:
	void audioFileStartedPlaying(AudioFile *inAudioFile);
        void audioFileStoppedPlaying(AudioFile *inAudioFile);

    private:
	friend class AudioFile;
	void addAudioFile(AudioFile *inAudioFile);
	void removeAudioFile(AudioFile *inAudioFile);

	QList<AudioFile *> mAudioFileList;
	int mCurrentAudioFileIndex;
};

#endif
