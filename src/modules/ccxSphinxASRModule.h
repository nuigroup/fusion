/////////////////////////////////////////////////////////////////////////////
// Name:        modules/ccxCCASpeechRecognitionModule.cpp
// Purpose:     CCA speech recognition output module
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


#ifndef CCX_CCA_ASR_MODULE_H
#define CCX_CCA_ASR_MODULE_H

#include "ccxModule.h"

#include "ofxBaseASR.h"
#include "s3_decode.h"
#include "fsg_model.h"
#include "jsgf.h"

#define AUDIO_SEGBUF_SIZE 256

struct ofAsrEngineArgs
{    
    int samplerate;
    
    // Only for sphinx
    string sphinxmodel_am;
    string sphinxmodel_lm;
    string sphinxmodel_dict;
    string sphinxmodel_fdict;
    int sphinx_mode;
        
    ofAsrEngineArgs()
    {
        samplerate = 16000;
        sphinx_mode = 0;
    }
};


class ccxCCASpeechRecognitionModule : public ccxModule(CCX_MODULE_OUTPUT|CCX_MODULE_INPUT){
public:
	ccxCCASpeechRecognitionModule();
    ~ccxCCASpeechRecognitionModule();
	
protected:

    ccxDataStream output;
    
	MODULE_INTERNALS();
    
private:
    // The ASR Engine
    ofxSphinxASR *            asrEngine;
};

#endif

