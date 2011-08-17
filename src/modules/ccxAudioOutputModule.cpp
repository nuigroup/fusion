//
//  ccxAudioOutputModule.cpp
//  fusion
//
//  Created by Scott Halstvedt on 8/8/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "ccxAudioOutputModule.h"

MODULE_DECLARE(AudioOutput, "native", "Get audio from RtAudio input (Core Audio or DirectSound)");

ccxAudioOutputModule::ccxAudioOutputModule() : ccxModule(CCX_MODULE_INPUT||CCX_MODULE_OUTPUT) {
    
    MODULE_INIT();
    
    resample_handle = NULL;
    
    resample_factor = AUDIO_TARGET_SAMPLE_RATE_F / AUDIO_HARDWARE_SAMPLE_RATE_F;
    resample_handle = resample_open(1, resample_factor, resample_factor);
    
    bRecording = false;
    
    audioBuf = NULL;
    audioBufSize = 0;
    
    this->output = new ccxDataStream("audiodatastream_8000");
    this->declareOutput(0, &this->output, new ccxDataStreamInfo("AudioDataStream", "audiodatastream_8000", "AudioDataStream (samplerate: 8000, format: 16-bit/short)"));
    
    this->input = new ccxDataStream("audio_triggers");
    this->declareInput(0, &this->input, new ccxDataStreamInfo("AudioTriggers", "AudioTriggers", "Audio triggers (basically just a bool for recording)"));
    
    this->properties["use_thread"] = new ccxProperty(true);
}

ccxAudioOutputModule::~ccxAudioOutputModule() {
    
}

void ccxAudioOutputModule::stop() {
    ccxModule::stop();
}

void ccxAudioOutputModule::start() {
    ccxModule::start();
}

AudioDataStream* ccxAudioOutputModule::recordAudioEnd() {
    unsigned int channels = 1, fs = AUDIO_HARDWARE_SAMPLE_RATE_I, bufferFrames, offset = 0;
    if(bRecording) {
        LOG(CCX_INFO, "done recording");
        recorder.stopStream();
        LOG(CCX_INFO, "stopped stream");
        recorder.closeStream();
        LOG(CCX_INFO, "closed input stream");
        
        bRecording = false;
        
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
        
        audioBufSize = (unsigned int)data.frameCounter * (unsigned int)data.channels;
        
        int newBufSize = audioBufSize;
        
        data.frameCounter = 0;
        
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
                
        return outputStream;
    } else {
        return NULL;
    }
    
}

void ccxAudioOutputModule::recordAudioStart(int max_time = 10) {
    unsigned int channels = 1, fs = AUDIO_HARDWARE_SAMPLE_RATE_I, bufferFrames, offset = 0;
    bRecording = true;
    this->output->lock();
    
    if ( recorder.getDeviceCount() < 1 ) {
        LOG(CCX_ERROR, "No audio devices found!");
        bRecording = false;
        this->output->lock();
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
    
    data.buffer = 0;
        
    try {
        recorder.openStream(NULL, &iParams, FORMAT, fs, &bufferFrames, &gotAudioInput, (void *) &data );
    }
    catch(RtError& e) {
        LOG(CCX_ERROR, e.getMessage());
        bRecording = false;
        this->output->unlock();
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
        this->output->unlock();
        bRecording = false;
    }
    
    try {
        recorder.startStream();
    } catch (RtError& e) {
        LOG(CCX_ERROR, e.getMessage());
        bRecording = false;
        this->output->unlock();
    }
    
    
	if(bRecording) LOG(CCX_INFO, "started recording");
    
    
}

void ccxAudioOutputModule::update() {
    
    LOG(CCX_INFO, "got update call");
    
    if(this->input->getData() != NULL) {
    
        if(((ccxDataGenericContainer *)this->input->getData())->hasProperty("recording") && !bRecording) {
            this->output->clear();
            recordAudioStart(10);
        }
        else if(bRecording) {
            AudioDataStream* audio = recordAudioEnd();
            if(audio != NULL) {
                this->output->push(audio);
            } else {
                this->output->clear();
            }
        }
        else {
            this->output->clear();
        }
        
    }
    else {
        this->output->clear();
    }
    
    this->output->unlock();
    
    
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

