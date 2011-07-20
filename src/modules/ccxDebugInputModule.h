/////////////////////////////////////////////////////////////////////////////
// Name:        modules/ccxCCASpeechRecognitionModule.cpp
// Purpose:     CCA speech recognition output module
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


#ifndef CCX_DEBUG_INPUT_MODULE_H
#define CCX_DEBUG_INPUT_MODULE_H

#include "ccxModule.h"

class ccxDataStream;

class ccxDebugInputModule : public ccxModule {
public:
	ccxDebugInputModule(); 
	virtual ~ccxDebugInputModule();

	virtual void notifyData(ccxDataStream *stream);
	void update();

private:
	ccxDataStream *stream;
	MODULE_INTERNALS();
};


#endif

