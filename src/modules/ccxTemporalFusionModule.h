/////////////////////////////////////////////////////////////////////////////
// Name:        modules/ccxTemporalFusionModule.cpp
// Purpose:     Temporal fusion module
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


#ifndef CCX_TEMPORAL_FUSION_MODULE_H
#define CCX_TEMPORAL_FUSION_MODULE_H

#include "ccfMultimodalSyntaxTree.h"
#include "ccx.h"
#include <assert.h>

class ccxTemporalFusionModule : public ccxModule {
public:
	ccxTemporalFusionModule();
	virtual ~ccxTemporalFusionModule();

	void start();
    void update();
    void stop();

	void notifyData(ccxDataStream* data);
	
protected:
	ccxDataStream* output;
	ccxDataStream* input;

	MODULE_INTERNALS();
};

#endif

