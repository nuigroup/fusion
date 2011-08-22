/////////////////////////////////////////////////////////////////////////////
// Name:        modules/ccxJSONGestureModule.h
// Purpose:     Generates gestures from JSON stream input
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


#ifndef CCX_JSON_GESTURE_MODULE_H
#define CCX_JSON_GESTURE_MODULE_H

#include "ccxModule.h"
#include "ccxDataStream.h"
#include "ccxDataGenericContainer.h"
#include "ccxLog.h"
#include "ccfMultimodalSyntaxTree.h"

class ccxDebugGestureOutputModule : public ccxModule {
public:
	ccxDebugGestureOutputModule(); 
	virtual ~ccxDebugGestureOutputModule();

	void update();
    void trigger();
    void start();

private:
	ccxDataStream *stream;
    bool alreadySent;
	MODULE_INTERNALS();
};


#endif

