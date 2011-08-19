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
#include "ccfMultimodalSyntaxTree.h"

MODULE_DECLARE(DebugInput, "native", "Print streams/structures to the debug console in text format");

ccxDebugInputModule::ccxDebugInputModule() : ccxModule(CCX_MODULE_INPUT) {
    MODULE_INIT();
    
	this->stream = NULL;
	this->declareInput(0, &this->stream, new ccxDataStreamInfo(
			"data", "mAST", "Show any stream that conforms to GenericList in text format"));
}

ccxDebugInputModule::~ccxDebugInputModule() {
}

void ccxDebugInputModule::update() {
    LOG(CCX_INFO, "stream<" << stream << ">, type=" << stream->getFormat() << ", observers=" << stream->getObserverCount());
    
	if ( stream->getFormat() == "generic" ) {
		ccxDataGenericList *list = static_cast<ccxDataGenericList*>(stream->getData());
		LOG(CCX_INFO, " `- " << stream->getFormat() << " size=" << list->size());
	}
    
    if( stream->getFormat() == "mAST" ) {
        LOG(CCX_INFO, mast_to_string(static_cast<client::multimodalSyntaxTree*>(stream->getData())));
    }
}

void ccxDebugInputModule::notifyData(ccxDataStream *stream) {
    this->notifyUpdate();
}

