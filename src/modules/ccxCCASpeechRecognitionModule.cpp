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

	// declare properties her, e.g:
	// this->properties["size"] = new moProperty(1.);
}

ccxCCASpeechRecognitionModule::~ccxCCASpeechRecognitionModule() {
}

void ccxCCASpeechRecognitionModule::start() {
    
}

void ccxCCASpeechRecognitionModule::stop() {
    
}

void ccxCCASpeechRecognitionModule::applyFilter() {
	// Add your filter code here
}

void ccxCCASpeechRecognitionModule::notifyData() {
    
}

