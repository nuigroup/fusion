/////////////////////////////////////////////////////////////////////////////
// Name:        modules/ccxCCASpeechRecognitionModule.cpp
// Purpose:     CCA speech recognition output module
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


#ifndef CCX_JSON_INTERACTION_MODULE_H
#define CCX_JSON_INTERACTION_MODULE_H

#include "ccxModule.h"
#include "ccfMultimodalSyntaxTree.h"

class ccxJSONInteractionModule : public ccxModule {
public:
	ccxJSONInteractionModule(); 
	virtual ~ccxJSONInteractionModule();

	virtual void notifyData(ccxDataStream *stream);
	void update();

private:
	ccxDataStream *input;
    ccxDataStream *output;
	MODULE_INTERNALS();
};


#endif

