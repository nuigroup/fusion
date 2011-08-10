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
#include <algorithm>

MODULE_DECLARE(SphinxASR, "native", "Fetch CCA speech recognition");

ccxSphinxASRModule::ccxSphinxASRModule() : ccxModule(CCX_MODULE_INPUT||CCX_MODULE_OUTPUT){

	MODULE_INIT();

    this->output = new ccxDataStream("CCAHypothesis");
    this->declareOutput(0, &this->output, new ccxDataStreamInfo("CCAHypothesis", "CCAHypothesis", "CCA recognition hypothesis"));
    
    this->input = new ccxDataStream("audiodatastream_8000");
    this->declareInput(0, &this->input, new ccxDataStreamInfo("AudioDataStream", "audiodatastream_8000", "AudioDataStream (samplerate: 8000, format: 16-bit/short)"));
    
    engine = NULL;
    model_sampleRate = 8000;
    
    // ASR Engine: commandpicking
    engine = new ccaSphinxASREngine();
    ccaSphinxASREngineArgs *engineArgs = new ccaSphinxASREngineArgs;
#ifndef WIN32
    engineArgs->sphinxmodel_am = "configs/ballworld/sphinxmodel/.";
    engineArgs->sphinxmodel_lm = "configs/ballworld/sphinxmodel/voxforge_en_sphinx.lm.DMP";
    engineArgs->sphinxmodel_dict = "configs/ballworld/sphinxmodel/cmudict.0.7a";
    engineArgs->sphinxmodel_fdict = "configs/ballworld/sphinxmodel/noisedict";
#else
    engineArgs->sphinxmodel_am = "C:\\Users\\shalstvedt\\Code\\ccf\\configs\\ballworld\\sphinxmodel";
    engineArgs->sphinxmodel_lm = "C:\\Users\\shalstvedt\\Code\\ccf\\configs\\ballworld\\sphinxmodel\\voxforge_en_sphinx.lm.DMP";
    engineArgs->sphinxmodel_dict = "C:\\Users\\shalstvedt\\Code\\ccf\\configs\\ballworld\\sphinxmodel\\cmudict.0.7a";
    engineArgs->sphinxmodel_fdict = "C:\\Users\\shalstvedt\\Code\\ccf\\configs\\ballworld\\sphinxmodel\\noisedict";

#endif

    engineArgs->sphinx_mode = 2;
    engineArgs->samplerate = model_sampleRate;
    int asrstatus = engine->engineInit(engineArgs);
    if (asrstatus != SPHINXASR_SUCCESS) {
        LOG(CCX_INFO, "Initialization of ASR Engine failed!");
        LOG(CCX_INFO, asrstatus);
    }
    
    delete engineArgs;
        
	// declare properties here, e.g:
	// this->properties["size"] = new moProperty(1.);
}

ccxSphinxASRModule::~ccxSphinxASRModule() {
    delete engine;
}

void ccxSphinxASRModule::start() {
    
    ccxModule::start();
    
}

void ccxSphinxASRModule::stop() {
    ccxModule::stop();
}

void ccxSphinxASRModule::notifyData(ccxDataStream* data) {
    assert(data != NULL);
    assert(data == this->input);
    this->notifyUpdate();
}

void ccxSphinxASRModule::update() {    
    engine->engineOpen();
    
    this->input->lock();
    
    AudioDataStream* ad = (AudioDataStream*)this->input->getData();
    
    this->input->unlock();
    
    if(engine->engineSentAudio(ad->buffer, ad->bufferSize)) {
        LOG(CCX_ERROR, "recognize failed");
        engine->engineClose();
    } else {
        LOG(CCX_INFO, "recogize succeeded!");
        std::string hypothesis(engine->engineGetText());
        engine->engineClose();
        std::transform(hypothesis.begin(), hypothesis.end(), hypothesis.begin(), ::tolower);
		char* chyp = new char[hypothesis.size()+1];
		strcpy (chyp, hypothesis.c_str());
        this->output->push((void*)chyp);
        
    }
    
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
#if defined WIN32
    char cfg_filename[] = "sphinx.cfg";
    char grammarFSG_filename[] = "configs\\ballworld\\ballworld.fsg";
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
        /*for (; *hypsegs; hypsegs++) {
            printf("Word-segment id: %i; start-frame: %i; string: %s; score: %i\n", (*hypsegs)->id, (*hypsegs)->sf, (*hypsegs)->word, (*hypsegs)->ascr);
        }*/
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