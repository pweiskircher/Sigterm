#ifndef _AUDIO_BUFFER_H
#define _AUDIO_BUFFER_H

#include <QByteArray>
#include <QMutex>
#include <QWaitCondition>

class AudioBuffer {
    public:
	AudioBuffer();

	bool add(QByteArray &inArray);
	bool get(QByteArray &outArray);
	void clear();

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
