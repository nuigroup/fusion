/////////////////////////////////////////////////////////////////////////////
// Name:        modules/ccxCCASpeechRecognitionModule.cpp
// Purpose:     CCA speech recognition output module
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


#ifndef MO_DUMMY_MODULE_H
#define MO_DUMMY_MODULE_H

#include "moImageFilterModule.h"

class moDUMMYModule : public moImageFilterModule{
public:
	moDUMMYModule();
	virtual ~moDUMMYModule();
	
protected:
	void applyFilter();

	MODULE_INTERNALS();
};

#endif

