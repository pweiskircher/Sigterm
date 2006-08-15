#ifndef _PLAY_LIST_H
#define _PLAY_LIST_H

#include <QAbstractListModel>

class AudioFile;

class PlayList : public QAbstractListModel {
    public:
	PlayList();

	void add(AudioFile *inAudioFile);
	AudioFile *currentFile();

	void setNextTrack(int inIndex);

	void finished(AudioFile *inAudioFile);

	int rowCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role) const;

    private:
	QList<AudioFile *> mAudioFileList;
	int mCurrentAudioFileIndex;
};

#endif
