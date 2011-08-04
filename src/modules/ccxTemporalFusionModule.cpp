/////////////////////////////////////////////////////////////////////////////
// Name:        modules/ccxCCASpeechRecognitionModule.cpp
// Purpose:     CCA speech recognition output module
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


#include <assert.h>
#include "moDUMMYModule.h"
#include "../moLog.h"
#include "cv.h"

MODULE_DECLARE(DUMMY, "native", "DUMMY Description");

moDUMMYModule::moDUMMYModule() : moImageFilterModule(){

	MODULE_INIT();

	// declare properties her, e.g:
	// this->properties["size"] = new moProperty(1.);
}

moDUMMYModule::~moDUMMYModule() {
}

void moDUMMYModule::applyFilter() {
	// Add your filter code here
}


