/////////////////////////////////////////////////////////////////////////////
// Name:        modules/ccxCCASpeechRecognitionModule.cpp
// Purpose:     CCA speech recognition output module
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include "ccxCCASpeechRecognitionModule.h"
#include "moLog.h"

MODULE_DECLARE(CCASpeechRecognition, "native", "Fetch CCA speech recognition; output module");

ccxCCASpeechRecognitionModule::ccxCCASpeechRecognitionModule() : ccxModule(CCX_MODULE_OUTPUT){

	MODULE_INIT();

    this->output = new ccxDataStream("mmAST");
    
    this->declareOutput(0, &this->output, new ccxDataStreamInfo("AST", "mmAST", "Multimodal abstract syntax tree"));
    
    
    // from NCore
    /*
    audioBuf = NULL;
    audioBufSize = 0;
    bRecording = false;
    asrEngine = NULL;
    resample_handle = NULL;
     */
    
    // ASR Engine: commandpicking
    asrEngine = new ofxSphinxASR;
    ofAsrEngineArgs *engineArgs = new ofAsrEngineArgs;
    engineArgs->sphinxmodel_am = ".";
    engineArgs->sphinxmodel_lm = "voxforge_en_sphinx.lm.DMP";
    engineArgs->sphinxmodel_dict = "cmudict.0.7a";
    engineArgs->sphinxmodel_fdict = "noisedict";
    engineArgs->sphinx_mode = 2;
    engineArgs->samplerate = 16000;
    if (asrEngine->engineInit(engineArgs) != OFXASR_SUCCESS) {
        printf("Initial ASR Engine Failed!"); // change to log
    }
    
    delete engineArgs;

    
	// declare properties her, e.g:
	// this->properties["size"] = new moProperty(1.);
}

ccxCCASpeechRecognitionModule::~ccxCCASpeechRecognitionModule() {
}

void ccxCCASpeechRecognitionModule::start() {
    
}

void ccxCCASpeechRecognitionModule::stop() {
    
}

void ccxCCASpeechRecognitionModule::notifyData() {
    
}

