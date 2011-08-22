/////////////////////////////////////////////////////////////////////////////
// Name:        modules/ccxJSONInteractionModule.cpp
// Purpose:     Generates JSON from a mAST
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


#include "ccxJSONInteractionModule.h"
#include "ccxDataStream.h"
#include "ccxDataGenericContainer.h"
#include "ccxLog.h"
#include "ccxModule.h"
#include "ccfMultimodalSyntaxTree.h"

MODULE_DECLARE(JSONInteraction, "native", "Converts mAST to JSON for output");

ccxJSONInteractionModule::ccxJSONInteractionModule() : ccxModule(CCX_MODULE_INPUT) {
    MODULE_INIT();
    
	this->input = new ccxDataStream("mAST");
	this->declareInput(0, &this->input, new ccxDataStreamInfo(
			"data", "mAST", "Multimodal abstract syntax tree"));
    this->output = new ccxDataStream("JSON");
    this->declareOutput(0, &this->output, new ccxDataStreamInfo("cJSON", "JSON", "JSON interaction"));
}

ccxJSONInteractionModule::~ccxJSONInteractionModule() {
}

void ccxJSONInteractionModule::update() {
    cJSON* outputJSON;
    if(this->input->getData() != NULL) outputJSON = mast_to_json((client::multimodalSyntaxTree *)this->input->getData());
    if(outputJSON != NULL) this->output->push(outputJSON);
    else this->output->clear();
}

void ccxJSONInteractionModule::notifyData(ccxDataStream *stream) {
    this->notifyUpdate();
}

