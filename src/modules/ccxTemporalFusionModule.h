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
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/foreach.hpp>

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
	ccxDataStream* audioInput;
    ccxDataStream* tactileInput;

	MODULE_INTERNALS();
};

#endif

