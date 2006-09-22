#include "LastFMClient.h"

bool LastFMEntry::load(QSettings &inSettings, const QString &inSection) {
	mArtist = inSettings.value(inSection + "/Artist").toString();
	mTrack = inSettings.value(inSection + "/Track").toString();
	mAlbum = inSettings.value(inSection + "/Album").toString();
	mMusicBrainzId = inSettings.value(inSection + "/MusicBrainzId").toString();
	mSeconds = inSettings.value(inSection + "/Seconds").toString();
	mDatePlayed = inSettings.value(inSection + "/DatePlayed").toString();
}

bool LastFMEntry::save(QSettings &inSettings, const QString &inSection) {
	inSettings.setValue(inSection + "/Artist", mArtist);
	inSettings.setValue(inSection + "/Track", mTrack);
	inSettings.setValue(inSection + "/Album", mAlbum);
	inSettings.setValue(inSection + "/MusicBrainzId", mMusicBrainzId);
	inSettings.setValue(inSection + "/Seconds", mSeconds);
	inSettings.setValue(inSection + "/DatePlayed", mDatePlayed);
}

QString LastFMEntry::toGetRequest(int count) {
	QString s;

	s += QString("&a[%1]=%2").arg(count).arg(mArtist);
	s += QString("&t[%1]=%2").arg(count).arg(mTrack);
	s += QString("&b[%1]=%2").arg(count).arg(mAlbum);
	s += QString("&m[%1]=%2").arg(count).arg(mMusicBrainzId);
	s += QString("&l[%1]=%2").arg(count).arg(mSeconds);
	s += QString("&i[%1]=%2").arg(count).arg(mDatePlayed);

	return s;
}


LastFMClient::LastFMClient(const QString &inRecordFile) : mSettings(inRecordFile, QSettings::IniFormat, this) {
	mHandshakeDone = false;
	mBadErrors = 0;
	mInterval = 0;
}

void LastFMClient::setUsername(const QString &inUsername) {
}
void LastFMClient::setPassword(const QString &inPassword) {
}

void LastFMClient::submitTrack(AudioFile *inAudioFile) {
}

void LastFMClient::httpRequestFinished(int id, bool error) {
}

void LastFMClient::submitTracks() {
}


