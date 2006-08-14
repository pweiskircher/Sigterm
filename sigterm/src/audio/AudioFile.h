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

    private:
	QString mFilePath;
	AudioDecoder *mDecoder;
};

#endif
