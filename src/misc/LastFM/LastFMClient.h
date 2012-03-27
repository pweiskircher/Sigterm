#ifndef _LASTFM_CLIENT_H
#define _LASTFM_CLIENT_H

#include <QHttp>
#include <QSettings>

class AudioFile;
class LastFMDialog;
class LastFMEntry;

#include "LastFMQueue.h"

class LastFMClient : public QObject {
	Q_OBJECT

	public:
		LastFMClient(const QString &inRecordFile);

		void setUsername(const QString &inUsername);
		void setHashedPassword(const QString &inPassword);
		void submitTrack(AudioFile *inAudioFile);
		void showDialog();

		LastFMQueue *queue();

	public slots:
		void usernameAndPasswordHashUpdated(const QString &inUsername, const QString &inHash);

	private slots:
		void httpRequestFinished(int id, bool error);
		void trackStoppedPlaying(AudioFile *inAudioFile, quint32 inTimePlayed);

	private:
		void submitTracks();
		void startHandshake();

		void setInterval(const QString &inString);

		QString mUsername;
		QString mHashedPassword;
		QHttp mHttpClient;

		QString mMd5Challenge;
		QString mSubmitUrl;

		typedef enum {
			eOk,			// everyhings ok
			eError,			// server told us that the request failed. show error message to user
			eBadUser,		// tell user that username is wrong and that he needs to change it
			eNetworkError,	// no network or sever is down.. try it again in 5 minutes
			eFatalError		// no point in continuning..
		} ProtocolErrorType;
		ProtocolErrorType mProtocolError;
		QString mErrorMessage;

		bool mHandshakeDone;
		int mHandshakeRequest;
		int mHandshakeTries;

		int mInterval;

		LastFMQueue mQueue;
		QSettings mSettings;

		LastFMDialog *mDialog;
};

#endif
