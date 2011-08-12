/////////////////////////////////////////////////////////////////////////////
// Name:        modules/ccxCCASpeechRecognitionModule.cpp
// Purpose:     CCA speech recognition output module
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


#include "ccxDebugOutputModule.h"
#include "ccxDataStream.h"
#include "ccxDataGenericContainer.h"
#include "ccxLog.h"
#include "ccxModule.h"
#include "ccfMultimodalSyntaxTree.h"

MODULE_DECLARE(DebugOutput, "native", "Print streams/structures to the debug console in text format");

ccxDebugOutputModule::ccxDebugOutputModule() : ccxModule(CCX_MODULE_OUTPUT) {
    MODULE_INIT();
    
    this->stream = new ccxDataStream("CCAHypothesis");
	this->declareOutput(0, &this->stream, new ccxDataStreamInfo(
			"data", "CCAHypothesis", "Create simple text for debugging"));
    this->alreadySent = false;

	this->properties["sentence"] = new ccxProperty("delete the yellow ball", "The sentence to feed");
    
}

ccxDebugOutputModule::~ccxDebugOutputModule() {
}

void ccxDebugOutputModule::update() {
    LOG(CCX_INFO, "updating");
	std::string text = this->properties["sentence"]->asString();
    char* cstr;
    cstr = new char [text.size()+1];
    strcpy (cstr, text.c_str());
    this->stream->push(cstr);
}

void ccxDebugOutputModule::start() {
    this->alreadySent = false;
    ccxModule::start();
}

void ccxDebugOutputModule::poll() {
    if(!this->alreadySent) {
        this->notifyUpdate();
        this->alreadySent = true;
    }
    ccxModule::poll();
}

void ccxDebugOutputModule::notifyData(ccxDataStream *stream) {
}

