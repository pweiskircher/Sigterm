#ifndef _PLAY_LIST_H
#define _PLAY_LIST_H

#include <QAbstractItemModel>

class AudioFile;

class PlayList : public QAbstractTableModel {
    public:
	PlayList();

	enum {
	    eIsPlaying = 0,
	    eTitle,
	    eTotalTime
	};

	void add(AudioFile *inAudioFile);
	AudioFile *currentFile();

	void setNextTrack(int inIndex);

	void finished(AudioFile *inAudioFile);

	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	int columnCount(const QModelIndex &parent ) const;
	int rowCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role) const;

    private:
	QList<AudioFile *> mAudioFileList;
	int mCurrentAudioFileIndex;
};

#endif
