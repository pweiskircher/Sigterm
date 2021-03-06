#ifndef _AUDIO_FILE_H
#define _AUDIO_FILE_H

#include <QString>
#include <QObject>

#include "AudioMetaData.h"

class AudioDecoder;
class AudioManager;

class AudioFile : public QObject {
	Q_OBJECT

	public:
		AudioFile(const QString &inFilePath, AudioManager *inAudioManager);
		~AudioFile();

		void addToQueue();
		void removeFromQueue();

		QString &filePath();
		AudioDecoder *decoder();
		AudioMetaData *metaData();

		quint32 timeTotal();
		quint32 timePlayed();

		bool seekToTime(quint32 inMilliSeconds);

		// Total samples in stream. 'Samples' means inter-channel sample, i.e. one second
		// of 44.1Khz audio will have 44100 samples regardless of the number of channels
		quint32 totalSamples();
		quint32 playedSamples();
		void setTotalSamples(quint32 inTotalSamples);
		void setPlayedSamples(quint32 inPlayedSamples);

		bool isPlaying();

		void setIsDecoding(bool inValue);

		void bytesAddedToAudioStorage(quint32 inSize);
		void bytesRemovedFromAudioStorage(quint32 inSize);
		quint32 bytesInAudioStorage();

	signals:
		void startedPlaying(AudioFile *inAudioFile);
		void stoppedPlaying(AudioFile *inAudioFile, quint32 inTimePlayed);

	private:
		QString mFilePath;
		AudioDecoder *mDecoder;
		AudioMetaData mMetaData;

		quint32 mTotalSamples;
		quint32 mPlayedSamples;

		bool mIsPlaying;
		bool mIsDecoding;

		AudioManager *mAudioManager;

		quint32 mBytesInAudioStorage;
};

#endif
