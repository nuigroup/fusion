/////////////////////////////////////////////////////////////////////////////
// Name:        modules/ccxDebugGestureModule.h
// Purpose:     Generate debug gestures
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

#ifndef CCX_DEBUG_GESTURE_MODULE_H
#define CCX_DEBUG_GESTURE_MODULE_H

#include "ccxModule.h"
#include "ccxDataStream.h"
#include "ccxDataGenericContainer.h"
#include "ccxLog.h"
#include "ccfMultimodalSyntaxTree.h"

class ccxDebugGestureModule : public ccxModule {
public:
	ccxDebugGestureModule(); 
	virtual ~ccxDebugGestureModule();

	void update();
    void start();

private:
	ccxDataStream *input;
    ccxDataStream *output;
	MODULE_INTERNALS();
};


#endif

