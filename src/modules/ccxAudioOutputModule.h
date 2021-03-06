/////////////////////////////////////////////////////////////////////////////
// Name:        modules/ccxAudioOutputModule.h
// Purpose:     Uses RtAudio library to collect audio input
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

#ifndef CCX_AUDIO_OUTPUT_MODULE_H
#define CCX_AUDIO_OUTPUT_MODULE_H

#include "ccx.h"

#include "RtAudio.h"
#include "libresample.h"

#if defined(WIN32)
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

#define AUDIO_HARDWARE_SAMPLE_RATE_F 44100.0f
#define AUDIO_HARDWARE_SAMPLE_RATE_I 44100

#define AUDIO_TARGET_SAMPLE_RATE_F 8000.0f
#define AUDIO_TARGET_SAMPLE_RATE_I 8000

#ifdef WIN32
typedef short AUDIO_TYPE;
#define FORMAT RTAUDIO_SINT16
#else
typedef float  AUDIO_TYPE;
#define FORMAT RTAUDIO_FLOAT32
#endif

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
    unsigned int bufferSize;
};

class ccxAudioOutputModule : public ccxModule {
public:
    ccxAudioOutputModule();
    ~ccxAudioOutputModule();
    void start();
    void update();
    void stop();
	static bool isRecording();
	static void setRecording(bool rec);
    
protected:
    void *              resample_handle;
    float               resample_factor;
    
    RtAudio recorder;
    
    static bool                bRecording;
    
    float*             audioBuf;
    unsigned int        audioBufSize;

	//int gotAudioInput(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *data);
    
    AudioData data;
    
	RtAudio::StreamOptions options;
    
    ccxDataStream* output;
    ccxDataStream* input;
    
    void recordAudioStart(int max_time);
    AudioDataStream* recordAudioEnd();
    
    MODULE_INTERNALS();
};

int gotAudioInput( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *data );
int gotAudioOutput( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *data );

#endif
