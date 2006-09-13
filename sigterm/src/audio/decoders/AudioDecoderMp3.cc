#include "AudioDecoderMp3.h"
#include "AudioFile.h"
#include "AudioBuffer.h"

#include <QFile>
#include <QDebug>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/* xing stuff stolen from alsaplayer */
# define XING_MAGIC	(('X' << 24) | ('i' << 16) | ('n' << 8) | 'g')

struct xing {
  	long flags;			/* valid fields (see below) */
  	unsigned long frames;		/* total number of frames */
  	unsigned long bytes;		/* total number of bytes */
  	unsigned char toc[100];		/* 100-point seek table */
  	long scale;			/* ?? */
};

enum {
  	XING_FRAMES = 0x00000001L,
  	XING_BYTES  = 0x00000002L,
  	XING_TOC    = 0x00000004L,
  	XING_SCALE  = 0x00000008L
};

enum {
	DECODE_BREAK,
	DECODE_SKIP,
	DECODE_STOP,
	DECODE_CONT,
	DECODE_OK
};

static int parse_xing(struct xing *xing, struct mad_bitptr ptr, unsigned int bitlen) {
	
	if (bitlen < 64 || mad_bit_read(&ptr, 32) != XING_MAGIC) goto fail;

	xing->flags = mad_bit_read(&ptr, 32);
	bitlen -= 64;

	if (xing->flags & XING_FRAMES) {
		if (bitlen < 32) goto fail;
		xing->frames = mad_bit_read(&ptr, 32);
		bitlen -= 32;
	}

	if (xing->flags & XING_BYTES) {
		if (bitlen < 32) goto fail;
		xing->bytes = mad_bit_read(&ptr, 32);
		bitlen -= 32;
	}

	if (xing->flags & XING_TOC) {
		int i;
		if (bitlen < 800) goto fail;
		for (i = 0; i < 100; ++i) xing->toc[i] = mad_bit_read(&ptr, 8);
		bitlen -= 800;
	}

	if (xing->flags & XING_SCALE) {
		if (bitlen < 32) goto fail;
		xing->scale = mad_bit_read(&ptr, 32);
		bitlen -= 32;
	}

	return 1;

fail:
	xing->flags = 0;
	return 0;
}

/* this is stolen from mpg321! */
static unsigned long prng(unsigned long state) {
	return (state * 0x0019660dL + 0x3c6ef35fL) & 0xffffffffL;
}

static signed long audio_linear_dither(unsigned int bits, mad_fixed_t sample, struct audio_dither *dither) {
	unsigned int scalebits;
	mad_fixed_t output, mask, random;

	enum {
		MIN = -MAD_F_ONE,
		MAX =  MAD_F_ONE - 1
	};

	sample += dither->error[0] - dither->error[1] + dither->error[2];

	dither->error[2] = dither->error[1];
	dither->error[1] = dither->error[0] / 2;

	output = sample + (1L << (MAD_F_FRACBITS + 1 - bits - 1));

	scalebits = MAD_F_FRACBITS + 1 - bits;
	mask = (1L << scalebits) - 1;

	random  = prng(dither->random);
	output += (random & mask) - (dither->random & mask);

	dither->random = random;

	if (output > MAX) {
		output = MAX;

		if (sample > MAX)
			sample = MAX;
	}
	else if (output < MIN) {
	        output = MIN;

		if (sample < MIN)
			sample = MIN;
	}

	output &= ~mask;

	dither->error[0] = sample - output;

	return output >> scalebits;
}
/* end of stolen stuff from mpg321 */



AudioDecoderMp3::AudioDecoderMp3(AudioFile *inAudioFile, AudioManager *inAudioManager) : AudioDecoder(inAudioFile, inAudioManager) {
}

AudioDecoderMp3::~AudioDecoderMp3() {
}

AudioDecoder *AudioDecoderMp3::createAudioDecoder(AudioFile *inAudioFile, AudioManager *inAudioManager) {
	return new AudioDecoderMp3(inAudioFile, inAudioManager);
}

bool AudioDecoderMp3::openFile() {
	mInputFile = fopen(qPrintable(audioFile()->filePath()), "rb");
	if (!mInputFile) {
		qDebug("AudioDecoderMp3::openFile: Couldn't open file");
		return false;
	}

	memset(&mDither, 0, sizeof(struct audio_dither));
	
	mad_stream_init(&mMadStream);
	mMadStream.options |= MAD_OPTION_IGNORECRC;
	mad_frame_init(&mMadFrame);
	mad_synth_init(&mMadSynth);
	mad_timer_reset(&mMadTimer);

	audioFile()->setTotalSamples(0);
	audioFormat().setIsBigEndian(false);
	audioFormat().setIsUnsigned(false);
	audioFormat().setBitsPerSample(16);

	decodeFirstFrame();
	
	qWarning("samplerate: %d\n", mMadFrame.header.samplerate);
	
	audioFormat().setFrequency(mMadFrame.header.samplerate);
	audioFormat().setChannels(MAD_NCHANNELS(&mMadFrame.header));

	return true;
}

bool AudioDecoderMp3::closeFile() {
	fclose(mInputFile);
	
	mad_synth_finish(&mMadSynth);
	mad_frame_finish(&mMadFrame);
	mad_stream_finish(&mMadStream);

	return true;
}

bool AudioDecoderMp3::fillDecoderBuffer() {
	unsigned char buffer[4096];
	size_t l = 4095;
	clearerr(mInputFile);
	l = fread(buffer, sizeof(unsigned char), l, mInputFile);
	mReadBytes += l;
	qWarning("mad: refilling buffer %d/%d bytes",l,mReadBytes);
	if (ferror(mInputFile) || feof(mInputFile)) {
		return false;
	}
	mad_stream_buffer(&mMadStream, buffer, l);
	mMadStream.error = MAD_ERROR_NONE;
	return true;
}

int AudioDecoderMp3::decodeNextFrameHeader() {

	if(mMadStream.buffer==NULL || mMadStream.error==MAD_ERROR_BUFLEN) {
		if (!fillDecoderBuffer()) {
			return DECODE_BREAK;
		}
	}
	if(mad_header_decode(&mMadFrame.header,&mMadStream)) {
#ifdef HAVE_ID3TAG
		if(mMadStream.error==MAD_ERROR_LOSTSYNC && 
				mMadStream.this_frame) 
		{
			signed long tagsize = id3_tag_query(
					mMadStream.this_frame,
					mMadStream.bufend-
					mMadStream.this_frame);

			if(tagsize>0) {
				if(tag && !(*tag)) {
					*tag = mp3_parseId3Tag(data, tagsize);
					
				}
				else {
					mad_stream_skip(&mMadStream,
							tagsize);
				}
				return DECODE_CONT;
			}
		}
#endif
		if (MAD_RECOVERABLE(mMadStream.error)) {
			return DECODE_SKIP;
		} else {
			if (mMadStream.error == MAD_ERROR_BUFLEN)
				return DECODE_CONT;
			else
			{
				qWarning("unrecoverable frame level error "
					"(%s).\n",
					mad_stream_errorstr(&mMadStream));
				return DECODE_BREAK;
			}
		}
	}
	if (mMadFrame.header.layer != MAD_LAYER_III) {
		return DECODE_SKIP;
	}

	return DECODE_OK;
}

int AudioDecoderMp3::decodeNextFrame() {

	if (mMadStream.buffer==NULL || mMadStream.error==MAD_ERROR_BUFLEN) {
		if (!fillDecoderBuffer()) {
			return DECODE_BREAK;
		}
	}
	
	if (mad_frame_decode(&mMadFrame, &mMadStream)) {
#ifdef HAVE_ID3TAG
		if (mMadStream.error==MAD_ERROR_LOSTSYNC) {
			signed long tagsize = id3_tag_query(
					(data->stream).this_frame,
					(data->stream).bufend-
					(data->stream).this_frame);
			if(tagsize>0) {
				mad_stream_skip(&(data->stream),tagsize);
				return DECODE_CONT;
			}
		}
#endif
		if (MAD_RECOVERABLE(mMadStream.error)) {
			return DECODE_SKIP;
		}
		else {
			if (mMadStream.error == MAD_ERROR_BUFLEN)
				return DECODE_CONT;
			else {
				qWarning("unrecoverable frame level error "
					"(%s).\n",
					mad_stream_errorstr(&mMadStream));
//				data->flush = 0;
				return DECODE_BREAK;
			}
		}
	}

	return DECODE_OK;
}

int AudioDecoderMp3::decodeFirstFrame() {
	struct xing xing;
	int ret;
	int skip;

	memset(&xing,0,sizeof(struct xing));
	xing.flags = 0;
	mReadBytes = 0;

	qWarning("decodeFirstFrame()");

	while(1) {
		skip = 0;
		while((ret = decodeNextFrameHeader())==DECODE_CONT);
		if(ret==DECODE_SKIP) skip = 1;
		else if(ret==DECODE_BREAK) return -1;
		while((ret = decodeNextFrame())==DECODE_CONT);
		if(ret==DECODE_BREAK) return -1;
		if(!skip && ret==DECODE_OK) break;
	}

	if (parse_xing(&xing,mMadStream.anc_ptr,mMadStream.anc_bitlen)) {
		if (xing.flags & XING_FRAMES) {
			mad_timer_t duration = mMadFrame.header.duration;
			mad_timer_multiply(&duration, xing.frames);

			audioFile()->setTotalSamples(xing.frames);
/*			data->muteFrame = MUTEFRAME_SKIP;
			data->totalTime = ((float)mad_timer_count(duration,
						MAD_UNITS_MILLISECONDS))/1000;
			data->maxFrames = xing.frames;*/
		}
	}
	else {
		size_t offset = 0;
		mad_timer_t duration = mMadFrame.header.duration;
		float frameTime = ((float)mad_timer_count(duration,
					MAD_UNITS_MILLISECONDS))/1000;
		if(mMadStream.this_frame!=NULL) {
			offset = mMadStream.bufend-mMadStream.this_frame;
		}
		else {
			offset-= mMadStream.bufend-mMadStream.buffer;
		}
		if(1 /*instream->Size >= offset*/) {
/*			data->totalTime = ((data->inStream->size-offset)*8.0)/
					(data->frame).header.bitrate;
			data->maxFrames = 
				data->totalTime/frameTime+FRAMES_CUSHION;*/
		}
		else {
/*			data->maxFrames = FRAMES_CUSHION;
			data->totalTime = 0;*/
		}
	}

/*	data->frameOffset = malloc(sizeof(long)*data->maxFrames);
	data->times = malloc(sizeof(mad_timer_t)*data->maxFrames);*/

	return 0;
}



bool AudioDecoderMp3::seekToTimeInternal(quint32 inMilliSeconds) {
//	mSampleId = (audioFormat().frequency() * (inMilliSeconds/1000.0)) / mF;
	return true;
}


AudioDecoder::DecodingStatus AudioDecoderMp3::getDecodedChunk(AudioBuffer *inOutAudioBuffer) {

	
	if (inOutAudioBuffer->state() != AudioBuffer::eEmpty) {
		qDebug("AudioDecoderMp3: AudioBuffer in wrong state!");
		return eContinue;
	}

	if (!inOutAudioBuffer->prepareForDecoding()) {
		qDebug("AudioBuffer::prepareForDecoding() failed.");
		return eContinue;
	}

	AudioDecoder::DecodingStatus status = eContinue;
	while (mAudioStorage.needData(inOutAudioBuffer->requestedLength()) == false) {

		mad_synth_frame(&mMadSynth,&mMadFrame);
		qWarning("decode made %d samples\n", mMadSynth.pcm.length);
		
		QByteArray a;
		quint32 len = mMadSynth.pcm.length * (audioFormat().channels()) * 2; /* FIXME: replace 2 with bits/8 */
		a.resize(len);
		char * ptr;
		ptr = a.data();
		
		for (int i=0; i < mMadSynth.pcm.length; i++) {
			signed long* sample;
			
			sample = (signed long*)ptr;
			*sample = (signed long)audio_linear_dither(16,
					mMadSynth.pcm.samples[0][i],
					&mDither);

			ptr+=2;
			
			if (audioFormat().channels()==2) {
				sample = (signed long*)ptr;
				*sample = (signed long)audio_linear_dither(16,
						mMadSynth.pcm.samples[1][i],
						&mDither);
				ptr+=2;
			}
		}
	
		qWarning("array is %d bytes\n", a.length());
		
		mAudioStorage.add(a, a.length());

		int skip, ret;
		while(1) {
			skip=0;
			while((ret = decodeNextFrameHeader())==DECODE_CONT);
			if(ret==DECODE_BREAK) break;
			else if(ret==DECODE_SKIP) skip = 1;
			while((ret = decodeNextFrame())==DECODE_CONT);
			if(ret==DECODE_BREAK) break;
			if(!skip && ret==DECODE_OK) break;
		}
		
	}

	if (status == eContinue) {
		mAudioStorage.get(inOutAudioBuffer->byteBuffer());
		inOutAudioBuffer->setDecodedChunkLength(inOutAudioBuffer->requestedLength());
	} else {
		inOutAudioBuffer->byteBuffer().resize(mAudioStorage.bufferLength());
		mAudioStorage.get(inOutAudioBuffer->byteBuffer());
		inOutAudioBuffer->setDecodedChunkLength(mAudioStorage.bufferLength());
	}

	return status;
}

bool AudioDecoderMp3::canDecode(const QString &inFilePath) {
	QFile file(inFilePath);
	if (!file.open(QIODevice::ReadOnly)) {
		return false;
	}

	/*
	 * there are no magic bytes, so we just have to pretend
	 * that we could play the file. sorry.
	 */
	return true;
}


bool AudioDecoderMp3::readInfo() {
	qWarning("entered readInfo()\n");

	if (!openFile())
		return false;

	closeFile();

	return true;
}

QString AudioDecoderMp3::audioFormatDescription() {
	return "MPEG 2 Audio Layer III";
}

QStringList AudioDecoderMp3::audioFormatFileExtensions() {
	return QStringList() << "*.mp3";
}

