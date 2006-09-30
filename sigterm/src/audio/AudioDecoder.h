#ifndef _AUDIO_DECODER_H
#define _AUDIO_DECODER_H

#include <QtGlobal>
#include <QString>
#include <QByteArray>
#include <QMutex>
#include <QFile>

#include "AudioFormat.h"
#include "AudioConverter.h"
#include "AudioFile.h"

#include <SDL.h>

class AudioManager;
class AudioFile;
class AudioBuffer;

class AudioDecoder {
	public:
		AudioDecoder(AudioFile *inAudioFile, AudioManager *inAudioManager);
		virtual ~AudioDecoder();

		virtual AudioDecoder *createAudioDecoder(AudioFile *inAudioFile, AudioManager *inAudioManager) = 0;
		virtual bool canDecode(const QString &inFilePath) = 0;

		virtual bool readInfo() = 0;
		bool seekToTime(quint32 inMilliSeconds);

		bool open();
		bool close();

		bool opened();

		typedef enum {
			eContinue,
			eStop
		} DecodingStatus;
		DecodingStatus getAudioChunk(AudioBuffer *inOutAudioBuffer);

		AudioFormat &audioFormat();

		AudioManager *audioManager();
		AudioFile *audioFile();

		virtual QString audioFormatDescription() = 0;
		virtual QStringList audioFormatFileExtensions() = 0;

		bool wasSeeking();

	protected:
		qint64 fileId3V2TagSize(QFile& file);

	private:
		virtual DecodingStatus getDecodedChunk(AudioBuffer *inOutAudioBuffer) = 0;
		virtual bool openFile() = 0;
		virtual bool closeFile() = 0;
		virtual bool seekToTimeInternal(quint32 inMilliSeconds) = 0;

		void setOpened(bool inValue);

		AudioFormat mAudioFormat;
		SDL_AudioCVT mCVT;
		bool mBuiltCVT;
		AudioManager *mAudioManager;
		AudioConverter mConverter;
		AudioFile *mAudioFile;
		bool mSeeked;
		quint32 mStartTime;

		QMutex mMutex;

		bool mOpened;
};

#endif
