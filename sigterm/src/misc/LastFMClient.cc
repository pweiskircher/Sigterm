#include "LastFMClient.h"
#include "AudioFile.h"
#include "AudioDecoder.h"

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


LastFMClient::LastFMClient(const QString &inRecordFile) : mSettings(inRecordFile, QSettings::IniFormat, this) {
	mHandshakeDone = false;
	mBadErrors = 0;
	mInterval = 0;

	QStringList groups = mSettings.childGroups();
	for (int i = 0; i < groups.size(); i++) {
		LastFMEntry *e = new LastFMEntry;
		if (e->load(mSettings, groups[i]))
			mEntryList.append(e);
		else {
			mSettings.remove(groups[i]);
			delete e;
		}
	}
}

void LastFMClient::setUsername(const QString &inUsername) {
	mUsername = inUsername;
}
void LastFMClient::setHashedPassword(const QString &inPassword) {
	mHashedPassword = inPassword;
}

void LastFMClient::submitTrack(AudioFile *inAudioFile) {
	if (mUsername.isEmpty() || mHashedPassword.isEmpty())
		return;

	AudioMetaData *meta = inAudioFile->metaData();
	if (meta->artist().isEmpty() || meta->title().isEmpty() || meta->album().isEmpty())
		return;

	LastFMEntry *e = new LastFMEntry;
	e->mArtist = meta->artist();
	e->mTitle = meta->title();
	e->mAlbum = meta->album();
	e->mMusicBrainzId = "";
	e->mSeconds = QString::number(inAudioFile->timeTotal());
	e->mDatePlayed = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
	mEntryList.append(e);

	int nextSection = mSettings.value("nextSection", 1).toInt();
	while (mSettings.childGroups().contains(QString::number(nextSection)))
		nextSection++;
	mSettings.setValue("nextSection", nextSection + 1);
	e->save(mSettings, QString::number(nextSection));
	mSettings.sync();
}

void LastFMClient::usernameAndPasswordHashUpdated(const QString &inUsername, const QString &inHash) {
	setUsername(inUsername);
	setHashedPassword(inHash);
}

void LastFMClient::httpRequestFinished(int id, bool error) {
}

void LastFMClient::trackStoppedPlaying(AudioFile *inAudioFile, quint32 inTimePlayed) {
	if (inAudioFile->decoder()->wasSeeking() == true)
		return;

	if (inAudioFile->timeTotal() <= 30)
		return;

	qDebug("timePlayed: %d timeTotal: %d", inTimePlayed, inAudioFile->timeTotal());
	if (inTimePlayed > 240 || inTimePlayed >= (inAudioFile->timeTotal()/2))
		submitTrack(inAudioFile);
}

void LastFMClient::submitTracks() {
}


