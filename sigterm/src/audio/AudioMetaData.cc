#include "AudioMetaData.h"
#include "AudioFile.h"

#include <QFileInfo>

#include <id3tag.h>

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

void AudioMetaData::parseId3Tags(struct id3_tag *inTag) {
	QString s;

	s = getID3Info(inTag, ID3_FRAME_ARTIST);
	if (s.length()) setArtist(s);

	s = getID3Info(inTag, ID3_FRAME_TITLE);
	if (s.length()) setTitle(s);

	s = getID3Info(inTag, ID3_FRAME_ALBUM);
	if (s.length()) setAlbum(s);

	s = getID3Info(inTag, ID3_FRAME_TRACK);
	if (s.length()) setTrackNumber(s.toInt());

	s = getID3Info(inTag, ID3_FRAME_YEAR);
	if (s.length()) setDate(s);
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

QString AudioMetaData::getID3Info(struct id3_tag *tag, char *id) {
	struct id3_frame const *frame;
	id3_ucs4_t const *ucs4;
	id3_utf8_t *utf8;
	union id3_field const *field;
	unsigned int nstrings;

	frame = id3_tag_findframe(tag, id, 0);
	if (!frame)
		return QString();

	field = &frame->fields[1];
	nstrings = id3_field_getnstrings(field);
	if (nstrings < 1) return NULL;

	ucs4 = id3_field_getstrings(field, 0);
	utf8 = id3_ucs4_utf8duplicate(ucs4);
	if (!utf8) return NULL;

	QString ret = QString::fromUtf8((const char *)utf8);
	free(utf8);

	return ret;
}
