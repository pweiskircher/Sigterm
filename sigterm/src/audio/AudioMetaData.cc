#include "AudioMetaData.h"
#include "AudioFile.h"

#include <QFileInfo>

AudioMetaData::AudioMetaData(AudioFile *inAudioFile) {
	mAudioFile = inAudioFile;
	mTrackNumber = 0;
	mTotalTracks = 0;
}

AudioMetaData::~AudioMetaData() {
}

void AudioMetaData::parseVorbisComments(QStringList &inList) {
	QStringListIterator it(inList);
	while (it.hasNext()) {
		QString comment = it.next();
		if (comment.toUpper().startsWith("ARTIST=")) {
			setArtist(comment.mid(comment.indexOf("=")+1));
		} else if (comment.toUpper().startsWith("TITLE=")) {
			setTitle(comment.mid(comment.indexOf("=")+1));
		} else if (comment.toUpper().startsWith("ALBUM=")) {
			setAlbum(comment.mid(comment.indexOf("=")+1));
		} else if (comment.toUpper().startsWith("DATE=")) {
			setDate(comment.mid(comment.indexOf("=")+1));
		} else if (comment.toUpper().startsWith("TRACKNUMBER=")) {
			setTrackNumber(comment.mid(comment.indexOf("=")+1).toInt());
		} else if (comment.toUpper().startsWith("TOTALTRACKS=")) {
			setTotalTracks(comment.mid(comment.indexOf("=")+1).toInt());
		}
	}
}

QString &AudioMetaData::artist() {
	return mArtist;
}

QString &AudioMetaData::title() {
	if (mTitle.length() == 0) {
		QFileInfo fi(mAudioFile->filePath());
		mTitle = fi.baseName();
	}
	return mTitle;
}

QString &AudioMetaData::album() {
	return mAlbum;
}

quint16 AudioMetaData::trackNumber() {
	return mTrackNumber;
}

quint16 AudioMetaData::totalTracks() {
	return mTotalTracks;
}

QString &AudioMetaData::date() {
	return mDate;
}


void AudioMetaData::setArtist(const QString &inArtist) {
	mArtist = inArtist;
}

void AudioMetaData::setTitle(const QString &inTitle) {
	mTitle = inTitle;
}

void AudioMetaData::setAlbum(const QString &inAlbum) {
	mAlbum = inAlbum;
}

void AudioMetaData::setTrackNumber(quint32 inTrackNumber) {
	mTrackNumber = inTrackNumber;
}

void AudioMetaData::setTotalTracks(quint32 inTotalTracks) {
	mTotalTracks = inTotalTracks;
}

void AudioMetaData::setDate(const QString &inDate) {
	mDate = inDate;
}

