/////////////////////////////////////////////////////////////////////////////
// Name:        modules/ccxCCASpeechRecognitionModule.cpp
// Purpose:     CCA speech recognition output module
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


#ifndef CCX_DEBUG_OUTPUT_MODULE_H
#define CCX_DEBUG_OUTPUT_MODULE_H

#include "ccxModule.h"

class ccxDebugOutputModule : public ccxModule {
public:
	ccxDebugOutputModule(); 
	virtual ~ccxDebugOutputModule();

	virtual void notifyData(ccxDataStream *stream);
    virtual void poll();
	void update();
    void start();

private:
	ccxDataStream *stream;
    bool alreadySent;
	MODULE_INTERNALS();
};


#endif

