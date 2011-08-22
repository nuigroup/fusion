/////////////////////////////////////////////////////////////////////////////
// Name:        modules/.dummy.cpp
// Purpose:     Module template
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

#include "ccxDUMMYModule.h"

MODULE_DECLARE(DUMMY, "native", "DUMMY Description");

ccxDUMMYModule::ccxDUMMYModule() : ccxModule(CCX_MODULE_INPUT|CCX_MODULE_OUTPUT) {
	MODULE_INIT();

	// declare properties here, e.g:
	// this->properties["size"] = new ccxProperty(1.);
}

ccxDUMMYModule::~moDUMMYModule() {
}

void ccxDUMMYModule::update() {
	// Update code goes here
}


