#include "LastFMClient.h"
#include "AudioFile.h"
#include "AudioDecoder.h"
#include "LastFMDialog.h"
#include "LastFMQueue.h"

#include <QDateTime>
#include <QTextStream>
#include <QDebug>

bool sortEntries(const QString &s1, const QString &s2) {
	if (s1.toInt() < s2.toInt())
		return true;
	return false;
}

LastFMClient::LastFMClient(const QString &inRecordFile) : mSettings(inRecordFile, QSettings::IniFormat, this) {
	mHandshakeDone = false;
	mInterval = 0;
	mHandshakeTries = 0;

	connect(&mHttpClient, SIGNAL(requestFinished(int, bool)), SLOT(httpRequestFinished(int, bool)));

	QStringList groups = mSettings.childGroups();
	qSort(groups.begin(), groups.end(), sortEntries);
	for (int i = 0; i < groups.size(); i++) {
		LastFMEntry *e = new LastFMEntry;
		if (e->load(mSettings, groups[i]))
			mQueue.append(e);
		else {
			mSettings.remove(groups[i]);
			delete e;
		}
	}

	mHandshakeRequest = -1;
	mProtocolError = eOk;

	mDialog = new LastFMDialog(this);
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
	mQueue.append(e);

	int nextSection = mSettings.value("nextSection", 1).toInt();
	while (mSettings.childGroups().contains(QString::number(nextSection)))
		nextSection++;
	mSettings.setValue("nextSection", nextSection + 1);
	e->save(mSettings, QString::number(nextSection));
	mSettings.sync();
}

void LastFMClient::showDialog() {
	mDialog->show();
}

LastFMQueue *LastFMClient::queue() {
	return &mQueue;
}

void LastFMClient::usernameAndPasswordHashUpdated(const QString &inUsername, const QString &inHash) {
	if (mUsername != inUsername) {
		mHandshakeDone = false;
		mHandshakeTries = 0;
	}

	setUsername(inUsername);
	setHashedPassword(inHash);
}

void LastFMClient::httpRequestFinished(int id, bool error) {
	if (id == mHandshakeRequest) {
		if (error) {
			mHandshakeRequest = false;
			mHandshakeTries++;
			mProtocolError = eNetworkError;
			if (mHandshakeTries > 3) {
				mInterval = 60*30;
			} else {
				mInterval = 60;
			}
			return;
		}
		mHandshakeTries = 0;

		QString s = mHttpClient.readAll();
		QStringList answer = s.split('\n');

		if (answer[0].startsWith("FAILED")) {
			// lets try again.. maybe notify the user that something failed
			if (answer.size() > 1 && answer[1].startsWith("INTERVAL ")) {
				setInterval(answer[1]);
			}

			mProtocolError = eError;
			mErrorMessage = answer[0].section(" ", 1);
		} else if (answer[0].startsWith("BADUSER")) {
			if (answer.size() > 1 && answer[1].startsWith("INTERVAL ")) {
				setInterval(answer[1]);
			}

			mProtocolError = eBadUser;
			// no idea what we should do in this case.
			// best thing would be to just wait until the user changes the username, but what if just the server has
			// a problem and says bad user?
			mInterval = qMax(mInterval, 60*5);
		} else if (answer[0].startsWith("UPTODATE")) {
			if (answer.size() < 4) {
				mProtocolError = eNetworkError;
				mErrorMessage = "Received broken answer from server.";
				mInterval = 60*5;
			} else {
				mMd5Challenge = answer[1];
				mSubmitUrl = answer[2];
				setInterval(answer[3]);
				mHandshakeDone = true;
			}
		}

		mHandshakeRequest = -1;
	}
}

void LastFMClient::trackStoppedPlaying(AudioFile *inAudioFile, quint32 inTimePlayed) {
	if (inAudioFile->decoder()->wasSeeking() == true)
		return;

	if (inAudioFile->timeTotal() <= 30)
		return;

	if (inTimePlayed > 240 || inTimePlayed >= (inAudioFile->timeTotal()/2))
		submitTrack(inAudioFile);
}

void LastFMClient::submitTracks() {
}

void LastFMClient::startHandshake() {
	mHttpClient.setHost("post.audioscrobbler.com");
	QString s = QString("/?hs=true&p=1.1&c=stm&v=0.1&u=%1").arg(mUsername);
	mHandshakeRequest = mHttpClient.get(s);
}

void LastFMClient::setInterval(const QString &inString) {
	mInterval = inString.section(" ", 1, 1).toInt();
	if (mInterval <= 0)
		mInterval = 30;
}
