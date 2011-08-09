//
//  ccxAudioOutputModule.h
//  fusion
//
//  Created by Scott Halstvedt on 8/8/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef CCX_AUDIO_INPUT_MODULE_H
#define CCX_AUDIO_INPUT_MODULE_H

#include "ccx.h"

#include "RtAudio.h"
#include "libresample.h"

#if defined( __WINDOWS_ASIO__ ) || defined( __WINDOWS_DS__ ) || defined(WIN32)
#include <windows.h>
#include <io.h>
#include <direct.h>
#define getcwd _getcwd
#define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else // Unix variants
#include <unistd.h>
#define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

#define AUDIO_FLOAT2SHORT(P) ( short(P * 32767.0f) )
#define AUDIO_SHORT2FLOAT(P) ( (float(P)) / 32767.0f )

#define AUDIO_SEGBUF_SIZE 256

#define AUDIO_HARDWARE_SAMPLE_RATE_F 96000.0f
#define AUDIO_HARDWARE_SAMPLE_RATE_I 96000

#define AUDIO_TARGET_SAMPLE_RATE_F 8000.0f
#define AUDIO_TARGET_SAMPLE_RATE_I 8000

typedef float  AUDIO_TYPE;
#define FORMAT RTAUDIO_FLOAT32
#define SCALE  1.0

struct AudioData {
    AUDIO_TYPE* buffer;
    unsigned long bufferBytes;
    unsigned long totalFrames;
    unsigned long frameCounter;
    unsigned int channels;
};

struct AudioDataStream {
    short* buffer;
    int sampleRate;
    int bufferSize;
};

class ccxAudioOutputModule : public ccxModule {
public:
    ccxAudioOutputModule();
    ~ccxAudioOutputModule();
    void start();
    void update();
    void stop();
    
protected:
    void *              resample_handle;
    float               resample_factor;
    
    RtAudio recorder;
    
    bool                bRecording;
    
    float *             audioBuf;
    int                 audioBufSize;
    
    ccxDataStream* output;
    
    MODULE_INTERNALS();
};

int gotAudioInput( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *data );
int gotAudioOutput( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *data );

#endif
