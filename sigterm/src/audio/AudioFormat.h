#ifndef _AUDIO_FORMAT_H
#define _AUDIO_FORMAT_H

#include <QtGlobal>

class AudioFormat {
    public:
	AudioFormat();
	AudioFormat(quint16 inBitsPerSample, quint16 inChannels, quint32 inFrequency, quint32 inBitRate, bool inIsBigEndian,
		    bool inIsUnsigned);
	~AudioFormat();

	bool isValid();

	quint16 sdlFormat();

	quint16 bitsPerSample();
	quint16 channels();
	quint32 frequency();
	quint32 bitRate();
	bool isBigEndian();
	bool isUnsigned();

	void setBitsPerSample(quint16 inValue);
	void setChannels(quint16 inValue);
	void setFrequency(quint32 inValue);
	void setBitRate(quint32 inValue);
	void setIsBigEndian(bool inIsBigEndian);
	void setIsUnsigned(bool inIsUnsigned);

    private:
	bool mIsValid;

	quint16 mBitsPerSample;
	quint16 mChannels;
	quint32 mFrequency;
	quint32 mBitRate;
	bool mIsBigEndian;
	bool mIsUnsigned;
};

#endif
