/////////////////////////////////////////////////////////////////////////////
// Name:        modules/ccxSpinxASRModule.cpp
// Purpose:     CCA speech recognition output module
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


#ifndef CCX_SPHINX_ASR_MODULE_H
#define CCX_SPHINX_ASR_MODULE_H

#include "ccx.h"

#include "s3_decode.h"
#include "fsg_model.h"
#include "jsgf.h"

#include "RtAudio.h"

#include "libresample.h"

#define AUDIO_SEGBUF_SIZE 256

typedef float  AUDIO_TYPE;
#define FORMAT RTAUDIO_FLOAT32
#define SCALE  1.0;

#if defined( __WINDOWS_ASIO__ ) || defined( __WINDOWS_DS__ )
#include <windows.h>
#define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else // Unix variants
#include <unistd.h>
#define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif


// The return codes
#define SPHINXASR_SUCCESS                   0
#define SPHINXASR_FAIL_UNKNOWN             -1
#define SPHINXASR_FAIL_READ_FILES          -2
#define SPHINXASR_HAVE_NOT_INIT            -3
#define SPHINXASR_FAIL_WRITE_CONFIG        -4
#define SPHINXASR_INVALID_JSGF_GRAMMAR     -5
#define SPHINXASR_INVALID_CONFIG           -6
#define SPHINXASR_FAIL_INIT_FRONTEND       -7
#define SPHINXASR_FAIL_STARTENGINE         -8
#define SPHINXASR_FAIL_INIT_DECODER        -9
#define SPHINXASR_INVALID_AUDIO_FORMAT     -10
#define SPHINXASR_HAVE_NOT_START           -11

struct ccaSphinxASREngineArgs
{    
    int samplerate;
    
    // Only for sphinx
    std::string sphinxmodel_am;
    std::string sphinxmodel_lm;
    std::string sphinxmodel_dict;
    std::string sphinxmodel_fdict;
    int sphinx_mode;
        
    ccaSphinxASREngineArgs()
    {
        samplerate = 16000;
        sphinx_mode = 0;
    }
};

int gotAudioInput( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *data );
int gotAudioOutput( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *data );

struct AudioData {
    AUDIO_TYPE* buffer;
    unsigned long bufferBytes;
    unsigned long totalFrames;
    unsigned long frameCounter;
    unsigned int channels;
};


class ccaSphinxASREngine {

public:
    ccaSphinxASREngine();
    ~ccaSphinxASREngine();
    
    int engineInit(ccaSphinxASREngineArgs *e);
    int engineExit();
    int engineOpen();
    int engineClose();
    int engineSentAudio(short *audioBuf, int audioSize);
    bool isEngineOpened();
    char * engineGetText();
    
private:
    // The Decoder
    s3_decode_t *decoder;
    fe_t *fe;
    
    // States
    bool bEngineInitialed;
    bool bEngineOpened;
    
    
    // Grammar
    fsg_model_t *get_fsg(jsgf_t *grammar, const char *name);
    
    // Utterance number
    int uttnum;
};


class ccxSphinxASRModule : public ccxModule {

public:
	ccxSphinxASRModule();
    ~ccxSphinxASRModule();
    
    void notifyData(ccxDataStream*);
    void update();
    void stop();
    void start();
    
protected:

    ccxDataStream* output;
    
    RtAudio recorder;
    
	MODULE_INTERNALS();
    
private:
    // The ASR Engine
    ccaSphinxASREngine *engine;
    int                 model_sampleRate;
    void *              resample_handle;
    float               resample_factor;
    
    bool                bRecording;
    
    
    int                 SampleRate;
    float *             audioBuf;
    int                 audioBufSize;
    

};

#endif

