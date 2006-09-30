#ifndef _LASTFM_CLIENT_H
#define _LASTFM_CLIENT_H

#include <QHttp>
#include <QSettings>

class AudioFile;

class LastFMEntry {
	public:
	bool load(QSettings &inSettings, const QString &inSection);
	bool save(QSettings &inSettings, const QString &inSection);
	QString toGetRequest(int count);

	QString mArtist;
	QString mTitle;
	QString mAlbum;
	QString mMusicBrainzId;
	QString mSeconds;
	QString mDatePlayed;
};

class LastFMClient : public QObject {
	Q_OBJECT

	public:
		LastFMClient(const QString &inRecordFile);

		void setUsername(const QString &inUsername);
		void setHashedPassword(const QString &inPassword);

		void submitTrack(AudioFile *inAudioFile);

	public slots:
		void usernameAndPasswordHashUpdated(const QString &inUsername, const QString &inHash);

	private slots:
		void httpRequestFinished(int id, bool error);
		void trackStoppedPlaying(AudioFile *inAudioFile, quint32 inTimePlayed);

	private:
		void submitTracks();

		QString mUsername;
		QString mHashedPassword;
		QHttp mHttpClient;

		QString mMd5Challenge;
		QString mSubmitUrl;

		bool mHandshakeDone;
		int mBadErrors;
		int mInterval;

		QList<LastFMEntry *> mEntryList;
		QSettings mSettings;
};

#endif
