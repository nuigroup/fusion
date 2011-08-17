/////////////////////////////////////////////////////////////////////////////
// Name:        modules/ccxCCASpeechRecognitionModule.cpp
// Purpose:     CCA speech recognition output module
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


#ifndef CCX_DEBUG_GESTURE_OUTPUT_MODULE_H
#define CCX_DEBUG_GESTURE_OUTPUT_MODULE_H

#include "ccxModule.h"
#include "ccxDataStream.h"
#include "ccxDataGenericContainer.h"
#include "ccxLog.h"
#include "ccfMultimodalSyntaxTree.h"

class ccxDebugGestureOutputModule : public ccxModule {
public:
	ccxDebugGestureOutputModule(); 
	virtual ~ccxDebugGestureOutputModule();

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

