/////////////////////////////////////////////////////////////////////////////
// Name:        modules/ccxSphinxASRModule.cpp
// Purpose:     CCA speech recognition output module
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include "ccxSphinxASRModule.h"
#include "ccxLog.h"

#include <iostream>
#include <fstream>

#define AUDIO_FLOAT2SHORT(P) ( short(P * 32767.0f) )
#define AUDIO_SHORT2FLOAT(P) ( (float(P)) / 32767.0f )

MODULE_DECLARE(SphinxASR, "native", "Fetch CCA speech recognition; output module");

ccxSphinxASRModule::ccxSphinxASRModule() : ccxModule(CCX_MODULE_OUTPUT){

	MODULE_INIT();

    this->output = new ccxDataStream("mAST");
    
    this->declareOutput(0, &this->output, new ccxDataStreamInfo("AST", "mmAST", "Multimodal abstract syntax tree"));
    
    
    // from NCore
    audioBuf = NULL;
    audioBufSize = 0;
    engine = NULL;
    resample_handle = NULL;
    bRecording = false;
    model_sampleRate = 8000;
    
    // ASR Engine: commandpicking
    engine = new ccaSphinxASREngine();
    ccaSphinxASREngineArgs *engineArgs = new ccaSphinxASREngineArgs;
    engineArgs->sphinxmodel_am = "/Users/shalstvedt/Code/ccf/configs/ballworld/sphinxmodel/.";
    engineArgs->sphinxmodel_lm = "/Users/shalstvedt/Code/ccf/configs/ballworld/sphinxmodel/voxforge_en_sphinx.lm.DMP";
    engineArgs->sphinxmodel_dict = "/Users/shalstvedt/Code/ccf/configs/ballworld/sphinxmodel/cmudict.0.7a";
    engineArgs->sphinxmodel_fdict = "/Users/shalstvedt/Code/ccf/configs/ballworld/sphinxmodel/noisedict";
    engineArgs->sphinx_mode = 2;
    engineArgs->samplerate = model_sampleRate;
    int asrstatus = engine->engineInit(engineArgs);
    if (asrstatus != SPHINXASR_SUCCESS) {
        LOG(CCX_INFO, "Initialization of ASR Engine failed!");
        LOG(CCX_INFO, asrstatus);
    }
    
    delete engineArgs;
    
    resample_factor = (float)model_sampleRate / 96000.0f;
    resample_handle = resample_open(1, resample_factor, resample_factor);
    
	// declare properties here, e.g:
	// this->properties["size"] = new moProperty(1.);
}

ccxSphinxASRModule::~ccxSphinxASRModule() {
    delete engine;
}

void ccxSphinxASRModule::start() {
    unsigned int channels = 1, fs = 96000, bufferFrames, device = 0, offset = 0;
    double max_time = 5.0;
    if ( recorder.getDeviceCount() < 1 ) {
        LOG(CCX_INFO, "No audio devices found!");
    }
    else {
        LOG(CCX_INFO, "Audio good!");
    }
    recorder.showWarnings( true );
    bufferFrames = 512;
    RtAudio::StreamParameters iParams;
    RtAudio::StreamParameters oParams;
    iParams.deviceId = device;
    iParams.nChannels = channels;
    iParams.firstChannel = offset;
    oParams.deviceId = 2;
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
    
    for(int tm = 0; tm < max_time; tm++) {
        SLEEP(500);
        LOG(CCX_INFO, tm);
    }    
    recorder.stopStream();
    recorder.closeStream();
    
    LOG(CCX_INFO, "done recording");
    
    data.frameCounter = 0;
    
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
    
    LOG(CCX_INFO, "done playing");
    
    recorder.closeStream();
    
    int srcused = 0;
    
    audioBufSize = fs * max_time * data.channels;
    int newBufSize = audioBufSize;

    if (fs != model_sampleRate) {
        newBufSize = int(audioBufSize * resample_factor);
        audioBuf = new float[newBufSize];
        resample_process(resample_handle, resample_factor, data.buffer, audioBufSize, 1, &srcused, audioBuf, newBufSize);
    }
    
    short* buf_16 = new short[newBufSize];
    for (int i=0; i<newBufSize; i++) {
        buf_16[i] = AUDIO_FLOAT2SHORT(audioBuf[i]);
    }
    
    engine->engineOpen();
    
    int ret = engine->engineSentAudio(buf_16, newBufSize);
        
    if(ret != SPHINXASR_SUCCESS) {
        LOG(CCX_ERROR, "recognize failed");
    } else {
    
        LOG(CCX_INFO, engine->engineGetText());
    }
    
    engine->engineClose();

    
}

void ccxSphinxASRModule::stop() {
    
}

void ccxSphinxASRModule::notifyData(ccxDataStream* data) {
    
}

void ccxSphinxASRModule::update() {
    
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



// ******** the engine ********

ccaSphinxASREngine::ccaSphinxASREngine() {
    
    bEngineInitialed = false;
    bEngineOpened = false;
    decoder = NULL;
    fe = NULL;
    uttnum = 0;
    
}

ccaSphinxASREngine::~ccaSphinxASREngine() {
    // do cleanup
}

int ccaSphinxASREngine::engineInit(ccaSphinxASREngineArgs *args) {
#if defined TARGET_WIN32
    char cfg_filename[] = "sphinx.cfg";
    char grammarJSGF_filename[] = "grammar.jsgf";
    char grammarFSG_filename[] = "grammar.fsg";
#else
	char cfg_filename[] = "/tmp/sphinx.cfg";
    char grammarFSG_filename[] = "/Users/shalstvedt/Code/ccf/configs/ballworld/ballworld.fsg";
#endif	
    FILE *cfg_fp = fopen(cfg_filename, "wt");
    if (cfg_fp==NULL)
        return SPHINXASR_FAIL_WRITE_CONFIG;
    if ( access(args->sphinxmodel_am.c_str(), 0) != 0 )
		return SPHINXASR_FAIL_READ_FILES;
	if ( access(args->sphinxmodel_lm.c_str(), 0) != 0 )
		return SPHINXASR_FAIL_READ_FILES;
	if ( access(args->sphinxmodel_dict.c_str(), 0) != 0 )
		return SPHINXASR_FAIL_READ_FILES;
	if ( access(args->sphinxmodel_fdict.c_str(), 0) != 0 )
		return SPHINXASR_FAIL_READ_FILES;
	
	/*char cur_path[1024];
	getcwd(cur_path, 1024);*/
    
    fprintf(cfg_fp, "-samprate %d\n", args->samplerate);
    fprintf(cfg_fp, "-hmm %s\n", args->sphinxmodel_am.c_str());
    fprintf(cfg_fp, "-dict %s\n", args->sphinxmodel_dict.c_str());
    fprintf(cfg_fp, "-fdict %s\n", args->sphinxmodel_fdict.c_str());
    fprintf(cfg_fp, "-lm %s\n", args->sphinxmodel_lm.c_str());
    if (args->sphinx_mode == 2) {
        fprintf(cfg_fp, "-fsg %s\n", grammarFSG_filename);
    }
    fprintf(cfg_fp, "-op_mode %d\n", args->sphinx_mode);
    fclose(cfg_fp);
    err_set_logfp(NULL); // disable logs
    cmd_ln_t *config = NULL;
    config = cmd_ln_parse_file_r(config, S3_DECODE_ARG_DEFS, cfg_filename, TRUE);
    if (config == NULL) {
        return SPHINXASR_INVALID_CONFIG;
    }
    decoder = new s3_decode_t;
    if (s3_decode_init(decoder, config) != S3_DECODE_SUCCESS) {
        return SPHINXASR_FAIL_INIT_DECODER;
    }
    fe = fe_init_auto_r(config);
    if (fe == NULL) {
        return SPHINXASR_FAIL_INIT_FRONTEND;
    }
    bEngineInitialed = true;
    return SPHINXASR_SUCCESS;

}

int ccaSphinxASREngine::engineExit() {
    s3_decode_close(decoder);
    if (decoder != NULL) {
        delete decoder;
        decoder = NULL;
    }
    if (fe != NULL) {
        fe_free(fe);
        fe = NULL;
    }
    return SPHINXASR_SUCCESS;

}

int ccaSphinxASREngine::engineOpen()
{
    if (! bEngineInitialed)
        return SPHINXASR_HAVE_NOT_INIT;
    char uttid[8];
    sprintf(uttid, "%d", uttnum);
    if (s3_decode_begin_utt(decoder, uttid) != S3_DECODE_SUCCESS)
    {
        return SPHINXASR_FAIL_STARTENGINE;
    }
    else {
        bEngineOpened = true;
        return SPHINXASR_SUCCESS;
    }
}

int ccaSphinxASREngine::engineClose()
{
    bEngineOpened = false;
    if (! bEngineInitialed)
        return SPHINXASR_SUCCESS;
    s3_decode_end_utt(decoder);
    return SPHINXASR_SUCCESS;
}

int ccaSphinxASREngine::engineSentAudio(short *audioBuf, int audioSize)
{
    float32 **frames;
    int n_frames;
    if (!bEngineInitialed)
        return SPHINXASR_HAVE_NOT_INIT;
    if (!bEngineOpened)
        return SPHINXASR_HAVE_NOT_START;
    fe_process_utt(fe, audioBuf, audioSize, &frames, &n_frames);
    if (frames != NULL) {
        s3_decode_process(decoder, frames, n_frames);
        ckd_free_2d((void **)frames);
    }
    return SPHINXASR_SUCCESS;
}

char * ccaSphinxASREngine::engineGetText()
{
    char * hypstr;
    hyp_t **hypsegs;
    if (s3_decode_hypothesis(decoder, NULL, &hypstr, &hypsegs)
        == S3_DECODE_SUCCESS && decoder->phypdump) {
        for (; *hypsegs; hypsegs++) {
            printf("Word-segment id: %i; start-frame: %i; string: %s; score: %i\n", (*hypsegs)->id, (*hypsegs)->sf, (*hypsegs)->word, (*hypsegs)->ascr);
        }
        return hypstr;
    }
    else {
        return NULL;
    }
}

bool ccaSphinxASREngine::isEngineOpened()
{
    return bEngineInitialed * bEngineOpened;
}