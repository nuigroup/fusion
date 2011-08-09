//
//  ccxAudioOutputModule.cpp
//  fusion
//
//  Created by Scott Halstvedt on 8/8/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "ccxAudioOutputModule.h"

MODULE_DECLARE(AudioOutput, "native", "Get audio from RtAudio input (Core Audio or DirectSound)");

ccxAudioOutputModule::ccxAudioOutputModule() : ccxModule(CCX_MODULE_OUTPUT) {
    
    MODULE_INIT();
    
    resample_handle = NULL;
    
    resample_factor = AUDIO_TARGET_SAMPLE_RATE_F / AUDIO_HARDWARE_SAMPLE_RATE_F;
    resample_handle = resample_open(1, resample_factor, resample_factor);
    
    bRecording = false;
    
    audioBuf = NULL;
    audioBufSize = 0;
    
    this->output = new ccxDataStream("audiodatastream_8000");
    this->declareOutput(0, &this->output, new ccxDataStreamInfo("AudioDataStream", "audiodatastream_8000", "AudioDataStream (samplerate: 8000, format: 16-bit/short)"));

}

ccxAudioOutputModule::~ccxAudioOutputModule() {
    
}

void ccxAudioOutputModule::stop() {
    ccxModule::stop();
}

void ccxAudioOutputModule::start() {
    
    ccxModule::start();
    
    unsigned int channels = 1, fs = AUDIO_HARDWARE_SAMPLE_RATE_I, bufferFrames, device = 0, offset = 0;
    double max_time = 5.0;

    if ( recorder.getDeviceCount() < 1 ) {
        LOG(CCX_ERROR, "No audio devices found!");
    }

    recorder.showWarnings( true );
    bufferFrames = 512;
    RtAudio::StreamParameters iParams;
    RtAudio::StreamParameters oParams;
	iParams.deviceId = recorder.getDefaultInputDevice();
    iParams.nChannels = channels;
    iParams.firstChannel = offset;
	oParams.deviceId = recorder.getDefaultOutputDevice();
    oParams.nChannels = channels;
    oParams.firstChannel = offset;
    
    AudioData data;
    data.buffer = 0;
    
    SLEEP(1000);
    
    try {
        recorder.openStream(NULL, &iParams, FORMAT, fs, &bufferFrames, &gotAudioInput, (void *) &data );
    }
    catch(RtError& e) {
        LOG(CCX_ERROR, e.getMessage());
    }
    
    data.bufferBytes = bufferFrames * channels * sizeof( AUDIO_TYPE );
    data.totalFrames = (unsigned long) (fs * max_time);
    data.frameCounter = 0;
    data.channels = channels;
    
    unsigned long totalBytes;
    totalBytes = data.totalFrames * channels * sizeof( AUDIO_TYPE );
    
    data.buffer = (AUDIO_TYPE*) malloc(totalBytes);
    
    if(data.buffer == 0) {
        LOG(CCX_ERROR, "malloc problem!");
    }
    
    try {
        recorder.startStream();
    } catch (RtError& e) {
        LOG(CCX_ERROR, e.getMessage());
    }

	LOG(CCX_INFO, "started recording");
    
    for(int tm = 0; tm < max_time; tm++) {
        SLEEP(500);
        LOG(CCX_INFO, tm);
    }
	LOG(CCX_INFO, "done recording");

    recorder.stopStream();
	LOG(CCX_INFO, "stopped stream");

    recorder.closeStream();
    LOG(CCX_INFO, "closed input stream");
    
    data.frameCounter = 0;
     
	/*

    try {
        recorder.openStream( &oParams, NULL, FORMAT, fs, &bufferFrames, &gotAudioOutput, (void *)&data );
        recorder.startStream();
    }
    catch(RtError& e) {
        LOG(CCX_ERROR, e.getMessage());
    }
    
    while ( 1 ) {
        SLEEP( 100 ); // wake every 100 ms to check if we're done
        if ( recorder.isStreamRunning() == false ) break;
    }
     
    recorder.closeStream();
    
    LOG(CCX_INFO, "done playing");
    
	*/

    int srcused = 0;
    
    audioBufSize = fs * max_time * data.channels;
    int newBufSize = audioBufSize;
    
    if (fs != AUDIO_TARGET_SAMPLE_RATE_I) {
        newBufSize = int(audioBufSize * resample_factor);
        audioBuf = new float[newBufSize];
        resample_process(resample_handle, resample_factor, data.buffer, audioBufSize, 1, &srcused, audioBuf, newBufSize);
    }
    
    short* buf_16 = new short[newBufSize];
    for (int i=0; i<newBufSize; i++) {
        buf_16[i] = AUDIO_FLOAT2SHORT(audioBuf[i]);
    }
    
    AudioDataStream* outputStream = new AudioDataStream;
    outputStream->buffer = buf_16;
    outputStream->sampleRate = AUDIO_TARGET_SAMPLE_RATE_I;
    outputStream->bufferSize = newBufSize;
    
    this->output->push(outputStream);
    
}

void ccxAudioOutputModule::update() {
    
}

int gotAudioInput( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                  double streamTime, RtAudioStreamStatus status, void *data )
{
    AudioData *iData = (AudioData *) data;
    
    // Simply copy the data to our allocated buffer.
    unsigned int frames = nBufferFrames;
    if ( iData->frameCounter + nBufferFrames > iData->totalFrames ) {
        frames = iData->totalFrames - iData->frameCounter;
        iData->bufferBytes = frames * iData->channels * sizeof( AUDIO_TYPE );
    }
    
    unsigned long offset = iData->frameCounter * iData->channels;
    memcpy( iData->buffer+offset, inputBuffer, iData->bufferBytes );
    iData->frameCounter += frames;
    
    if ( iData->frameCounter >= iData->totalFrames ) return 2;
    return 0;
}

// Interleaved buffers
int gotAudioOutput( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                   double streamTime, RtAudioStreamStatus status, void *data )
{
    AudioData *aData = (AudioData*) data;
    unsigned int frames = nBufferFrames;
    if ( aData->frameCounter + nBufferFrames > aData->totalFrames ) {
        frames = aData->totalFrames - aData->frameCounter;
        aData->bufferBytes = frames * aData->channels * sizeof( AUDIO_TYPE );
    }
    unsigned long offset = aData->frameCounter * aData->channels;
    memcpy(outputBuffer, aData->buffer+offset, aData->bufferBytes);
    aData->frameCounter += frames;
    if ( aData->frameCounter >= aData->totalFrames ) return 1;
    return 0;
}

