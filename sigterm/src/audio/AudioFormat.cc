#include "AudioFormat.h"
#include <SDL.h>

AudioFormat::AudioFormat() {
	mIsValid = false;

	mBitsPerSample = 0;
	mChannels = 0;
	mFrequency = 0;
	mBitRate = 0;
	mIsBigEndian = false;
	mIsUnsigned = false;
}

AudioFormat::AudioFormat(quint16 inBitsPerSample, quint16 inChannels, quint32 inFrequency, quint32 inBitRate,
		bool inIsBigEndian, bool inIsUnsigned) {
	setBitsPerSample(inBitsPerSample);
	setChannels(inChannels);
	setFrequency(inFrequency);
	setBitRate(inBitRate);
	setIsBigEndian(inIsBigEndian);
	setIsUnsigned(inIsUnsigned);
}

AudioFormat::AudioFormat(SDL_AudioSpec *inAudioSpec) {
	switch(inAudioSpec->format) {
		case AUDIO_U8:
			setBitsPerSample(8);
			setIsUnsigned(true);
			setIsBigEndian(false);
			break;

		case AUDIO_S8:
			setBitsPerSample(8);
			setIsUnsigned(false);
			setIsBigEndian(false);
			break;

		case AUDIO_U16LSB:
			setBitsPerSample(16);
			setIsUnsigned(true);
			setIsBigEndian(false);
			break;

		case AUDIO_S16LSB:
			setBitsPerSample(16);
			setIsUnsigned(false);
			setIsBigEndian(false);
			break;

		case AUDIO_U16MSB:
			setBitsPerSample(16);
			setIsUnsigned(true);
			setIsBigEndian(true);
			break;

		case AUDIO_S16MSB:
			setBitsPerSample(16);
			setIsUnsigned(false);
			setIsBigEndian(true);
			break;
	}

	setChannels(inAudioSpec->channels);
	setFrequency(inAudioSpec->freq);
	setBitRate(0);
}

AudioFormat::~AudioFormat() {
}


bool AudioFormat::isValid() {
	return mIsValid;
}

quint16 AudioFormat::sdlFormat() {
	if (isUnsigned() == true) {
		if (isBigEndian() == true) {
			if (bitsPerSample() == 8)
				return AUDIO_U8;
			else
				return AUDIO_U16MSB;
		} else {
			if (bitsPerSample() == 8)
				return AUDIO_U8;
			else
				return AUDIO_U16LSB;
		}
	} else {
		if (isBigEndian() == true) {
			if (bitsPerSample() == 8)
				return AUDIO_S8;
			else
				return AUDIO_S16MSB;
		} else {
			if (bitsPerSample() == 8)
				return AUDIO_S8;
			else
				return AUDIO_S16LSB;
		}
	}
}

quint16 AudioFormat::bitsPerSample() {
	return mBitsPerSample;
}

quint16 AudioFormat::channels() {
	return mChannels;
}

quint32 AudioFormat::frequency() {
	return mFrequency;
}

quint32 AudioFormat::bitRate() {
	return mBitRate;
}

bool AudioFormat::isBigEndian() {
	return mIsBigEndian;
}

bool AudioFormat::isUnsigned() {
	return mIsUnsigned;
}


void AudioFormat::setBitsPerSample(quint16 inValue) {
	mIsValid = true;
	mBitsPerSample = inValue;
}

void AudioFormat::setChannels(quint16 inValue) {
	mIsValid = true;
	mChannels = inValue;
}

void AudioFormat::setFrequency(quint32 inValue) {
	mIsValid = true;
	mFrequency = inValue;
}

void AudioFormat::setBitRate(quint32 inValue) {
	mIsValid = true;
	mBitRate = inValue;
}

void AudioFormat::setIsBigEndian(bool inIsBigEndian) {
	mIsValid = true;
	mIsBigEndian = inIsBigEndian;
}

void AudioFormat::setIsUnsigned(bool inIsUnsigned) {
	mIsValid = true;
	mIsUnsigned = inIsUnsigned;
}

