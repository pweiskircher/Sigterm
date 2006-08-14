#include "AudioProcessor.h"
#include "AudioManager.h"
#include "AudioFile.h"
#include "AudioDecoder.h"
#include "PlayList.h"

AudioProcessor::AudioProcessor(AudioManager *inAudioManager) {
    mAudioManager = inAudioManager;
}

void AudioProcessor::run() {
    mMutex.lock();
    while (mAudioManager->audioProcessorWaitCondition()->wait(&mMutex)) {
	PlayList *playList = mAudioManager->currentPlayList();
	AudioFile *file = playList->currentFile();

	AudioDecoder::DecodingStatus status;
	QByteArray audioData;
	audioData.resize(4096);
	while (1) {
	    status = file->decoder()->getAudioChunk(audioData);
	    if (status == AudioDecoder::eError) {
		qDebug("Error on decoding ...");
		exit(EXIT_FAILURE);
	    } else if (status == AudioDecoder::eEOF) {
		qDebug("Finished decoding ...");
		break;
	    }

	    mAudioManager->audioBuffer()->add(audioData);
	}
    }
    mMutex.unlock();
}
