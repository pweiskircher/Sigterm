#include "AudioDecoder.h"
#include "AudioManager.h"
#include "AudioBuffer.h"

AudioDecoder::AudioDecoder(AudioFile *inAudioFile, AudioManager *inAudioManager) : mConverter(inAudioManager) {
	mBuiltCVT = false;
	mAudioManager = inAudioManager;
	mAudioFile = inAudioFile;
	mOpened = false;
}

AudioDecoder::~AudioDecoder() {
	if (opened())
		close();
}

bool AudioDecoder::seekToTime(quint32 inMilliSeconds) {
	QMutexLocker locker(&mMutex);
	return seekToTimeInternal(inMilliSeconds);
}

bool AudioDecoder::open() {
	if (opened()) {
		qDebug("File already opened.");
		return false;
	}

	bool r = openFile();
	if (r)
		setOpened(true);
	return r;
}

bool AudioDecoder::close() {
	if (!opened()) {
		qDebug("File not open.");
		return false;
	}

	bool r = closeFile();
	if (r)
		setOpened(false);
	return r;
}

bool AudioDecoder::opened() {
	return mOpened;
}

AudioFormat &AudioDecoder::audioFormat() {
	return mAudioFormat;
}

AudioDecoder::DecodingStatus AudioDecoder::getAudioChunk(AudioBuffer *inOutAudioBuffer) {
	QMutexLocker locker(&mMutex);
	if (!opened()) {
		if (!open())
			return eStop;
	}

	if (!mBuiltCVT) {
		mConverter.setSourceFormat(&audioFormat());
		mBuiltCVT = true;
	}

	AudioDecoder::DecodingStatus status;
	status = getDecodedChunk(inOutAudioBuffer);
	if (status == eStop) {
		close();
	}

	if (inOutAudioBuffer->state() != AudioBuffer::eGotDecodedChunk) {
		return status;
	}

	if (!mConverter.convert(inOutAudioBuffer)) {
		qDebug("Could not convert audio!");
		close();
		return eStop;
	}

	// sanity check
	if (inOutAudioBuffer->state() != AudioBuffer::eGotConvertedChunk) {
		qDebug("AudioBuffer state wrong.");
		close();
		return eStop;
	}

	return status;
}

AudioManager *AudioDecoder::audioManager() {
	return mAudioManager;
}

AudioFile *AudioDecoder::audioFile() {
	return mAudioFile;
}

void AudioDecoder::setOpened(bool inValue) {
	mOpened = inValue;
}

qint64 AudioDecoder::fileId3V2TagSize(QFile& file) {

	qint64 id3TagSize = 0;
	qint64 startPos = file.pos();
	
	QByteArray startOfFile = file.read(5);
	/*
	 * wiiii, evil hack: skip id3v2 header if present. ideally this would be in the input
	 * stream class
	 */
	if (startOfFile.size() == 5 && 
			(char)startOfFile[0] == 'I' &&
			(char)startOfFile[1] == 'D' &&
			(char)startOfFile[2] == '3')
	{
		unsigned char id3VersionMajor = startOfFile[3];
		unsigned char id3VersionMinor = startOfFile[4];
		if (id3VersionMajor <= 3 && id3VersionMinor < 0xFF) {
			QByteArray flags_a = file.read(1);
			// unsigned char flags = flags_a[0];
			QByteArray id3size_a = file.read(4);
			if (id3size_a.size() == 4) {
				// TODO: figure out how we can do this in an xplatform-correct way and then memcpy() it
				id3TagSize = (id3size_a[0]<<21) | (id3size_a[1]<<14) | (id3size_a[2]<<7) | id3size_a[3];
				id3TagSize += 10;
			}
		}
	}
	file.seek(startPos);
	return id3TagSize;
}

