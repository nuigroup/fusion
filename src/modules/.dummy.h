/////////////////////////////////////////////////////////////////////////////
// Name:        modules/.dummy.cpp
// Purpose:     Module template
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


#ifndef CCX_DUMMY_MODULE_H
#define CCX_DUMMY_MODULE_H

#include <assert.h>
#include "ccx.h"

class ccxDUMMYModule : public ccxModule {
public:
	ccxDUMMYModule();
	virtual ~ccxDUMMYModule();
    void update();
protected:
	MODULE_INTERNALS();
};

#endif

