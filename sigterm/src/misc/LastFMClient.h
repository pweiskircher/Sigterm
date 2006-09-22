#ifndef _LASTFM_CLIENT_H
#define _LASTFM_CLIENT_H

#include <QHttp>
#include <QSettings>

class AudioFile;

class LastFMEntry {
	bool load(QSettings &inSettings, const QString &inSection);
	bool save(QSettings &inSettings, const QString &inSection);
	QString toGetRequest(int count);

	QString mArtist;
	QString mTrack;
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
		void setPassword(const QString &inPassword);

		void submitTrack(AudioFile *inAudioFile);

	private slots:
		void httpRequestFinished(int id, bool error);

	private:
		void submitTracks();

		QString mUsername;
		QString mPassword;
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
