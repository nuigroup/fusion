/////////////////////////////////////////////////////////////////////////////
// Name:        modules/ccxCCASpeechRecognitionModule.cpp
// Purpose:     CCA speech recognition output module
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


#include "ccxDebugInputModule.h"
#include "ccxDataStream.h"
#include "ccxDataGenericContainer.h"
#include "ccxLog.h"
#include "ccxModule.h"

MODULE_DECLARE(DebugInput, "native", "Print streams/structures to the debug console in text format");

ccxDebugModule::ccxDebugInputModule() : ccxModule(CCX_MODULE_INPUT) {
	this->stream = NULL;
	this->declareInput(0, &this->stream, new ccxDataStreamInfo(
			"data", "*", "Show any stream that conforms to GenericList in text format"));
}

ccxDebugModule::~ccxDebugInputModule() {
}

void ccxDebugInputModule::update() {
}

void ccxDebugInputModule::notifyData(ccxDataStream *stream) {
	LOG(CCX_INFO, "stream<" << stream << ">, type=" << stream->getFormat() << ", observers=" << stream->getObserverCount());

	if ( stream->getFormat() == "generic" ) {
		ccxDataGenericList *list = static_cast<ccxDataGenericList*>(stream->getData());
		LOG(CCX_INFO, " `- " << stream->getFormat() << " size=" << list->size());
	}
    
    if( stream->getFormat() == "multimodalSyntaxTree" ) {
        // iterate over the tree and log it
    }
}

