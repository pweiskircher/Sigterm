#ifndef _AUDIO_FILE_H
#define _AUDIO_FILE_H

#include <QString>

class AudioDecoder;
class AudioManager;

class AudioFile {
    public:
	AudioFile(const QString &inFilePath, AudioManager *inAudioManager);

	QString &filePath();
	AudioDecoder *decoder();

	quint32 timeTotal();
	quint32 timePlayed();

	// Total samples in stream. 'Samples' means inter-channel sample, i.e. one second
	// of 44.1Khz audio will have 44100 samples regardless of the number of channels
	quint32 totalSamples();
	quint32 playedSamples();
	void setTotalSamples(quint32 inTotalSamples);
	void setPlayedSamples(quint32 inPlayedSamples);

	bool isPlaying();
	void setIsPlaying(bool inValue);

    private:
	QString mFilePath;
	AudioDecoder *mDecoder;

	quint32 mTotalSamples;
	quint32 mPlayedSamples;

	bool mIsPlaying;
};

#endif
