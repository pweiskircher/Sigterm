#ifndef _AUDIO_STORAGE_H
#define _AUDIO_STORAGE_H

#include <QByteArray>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>

class AudioBuffer;
class AudioFile;

class AudioStorage {
	public:
		AudioStorage(quint32 inBufferSize=128*1024);
		~AudioStorage();

		bool add(AudioBuffer *inAudioBuffer);
		bool add(QByteArray &inArray, quint32 inLen);
		bool get(QByteArray &outArray);
		void clear();

		quint32 bufferLength();

		bool needSpace(quint32 inSpace);
		bool needData(quint32 inData);

		void wakeOnBufferGet(QWaitCondition *inCondition);
		void dump();

	private:
		QMutex mMutex;
		quint32 mBufferSize;
		quint8 *mBuffer;

		QQueue<quint8 *> mUsedBufferList;
		QQueue<quint8 *> mFreeBufferList;
		quint8 *mPartialBuffer;
		quint32 mPartialBufferLength;

		quint8 *mGetPartialBuffer;
		quint32 mGetPartialBufferLength;

		QWaitCondition *mBufferGetCondition;

		QQueue<AudioFile *> mAudioFileQueue;
};

#endif
