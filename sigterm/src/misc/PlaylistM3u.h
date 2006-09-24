#ifndef _MISC_PLAYLISTM3U_H
#define _MISC_PLAYLISTM3U_H

#include <QFile>
#include <QTextStream>

class AudioFile;
class AudioManager;

class PlaylistM3u {

	public:
		PlaylistM3u(const QString& inFileName, AudioManager *inAudioManager);
		~PlaylistM3u();

		void setFileName(const QString& fileName);

		bool load(QList<AudioFile *> &audioFileList);
		bool save(QList<AudioFile *> &audioFileList);

	protected:
		bool openFile(QIODevice::OpenMode mode);
		bool closeFile();

		QFile mFile;
		AudioManager* mAudioManager;
};

#endif //!_MISC_PLAYLISTM3U_H
