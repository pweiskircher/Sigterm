#ifndef _AUDIO_STORAGE_H
#define _AUDIO_STORAGE_H

#include <QByteArray>
#include <QMutex>
#include <QWaitCondition>

class AudioBuffer;

class AudioStorage {
    public:
	AudioStorage();

	bool add(AudioBuffer *inAudioBuffer);
	bool add(QByteArray &inArray, quint32 inLen);
	bool get(QByteArray &outArray);
	void clear();

	quint32 bufferLength();

	bool needSpace(quint32 inSpace);
	bool needData(quint32 inData);

	void wakeOnBufferGet(QWaitCondition *inCondition);

    private:
	QMutex mMutex;
	QByteArray mBuffer;
	quint32 mBufferLength;

	QWaitCondition *mBufferGetCondition;
};

#endif
