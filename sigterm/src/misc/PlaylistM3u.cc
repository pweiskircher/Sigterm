#include <QString>
#include "PlaylistM3u.h"
#include "AudioFile.h"
#include "AudioManager.h"

PlaylistM3u::PlaylistM3u(const QString& inFileName, AudioManager* inAudioManager) {
	mAudioManager = inAudioManager;
	setFileName(inFileName);
}

PlaylistM3u::~PlaylistM3u() {
}

void PlaylistM3u::setFileName(const QString& fileName) {
	mFile.setFileName(fileName);
}

bool PlaylistM3u::openFile(QIODevice::OpenMode mode) {
	
	if (!closeFile())
		return false;

	return (mFile.open(mode | QFile::Text) == true);
}

bool PlaylistM3u::closeFile() {
	mFile.close();
	return true;
}

bool PlaylistM3u::load(QList<AudioFile *> &audioFileList) {
	
	if (!openFile(QFile::ReadOnly))
		return false;
	
	QTextStream playlist(&mFile);
	QString itemFileName;
	while(!playlist.atEnd()) {
		
		itemFileName = playlist.readLine();

		if (itemFileName.isNull())
			break;
		
		if (itemFileName.startsWith("#"))
			continue;
	
		AudioFile *af = new AudioFile(itemFileName, mAudioManager);
		audioFileList.append(af);
	}

	closeFile();

	return true;
}

bool PlaylistM3u::save(QList<AudioFile *> &audioFileList) {
	
	if (!openFile(QFile::WriteOnly | QFile::Truncate))
		return false;
	
	QTextStream playlist(&mFile);
	playlist << "#EXTM3U" << endl;
	
	QListIterator<AudioFile *> afit(audioFileList);
	while (afit.hasNext()) {
		AudioFile *af = afit.next();
		playlist << "#EXTINF:";
		playlist << af->timeTotal() << ",";
		playlist << af->metaData()->artist() << " - " << af->metaData()->title() << endl;
		playlist << af->filePath() << endl;
	}
	playlist.flush();

	closeFile();

	return true;
}


