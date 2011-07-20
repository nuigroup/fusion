/////////////////////////////////////////////////////////////////////////////
// Name:        modules/ccxCCASpeechRecognitionModule.cpp
// Purpose:     CCA speech recognition output module
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


#ifndef CCX_CCA_ASR_MODULE_H
#define CCX_CCA_ASR_MODULE_H

#include "ccxModule.h"

class ccxCCASpeechRecognitionModule : public ccxModule(CCX_MODULE_OUTPUT|CCX_MODULE_INPUT){
public:
	ccxCCASpeechRecognitionModule();
	virtual ~ccxCCASpeechRecognitionModule();
	
protected:

	MODULE_INTERNALS();
};

#endif

