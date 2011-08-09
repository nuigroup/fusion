/////////////////////////////////////////////////////////////////////////////
// Name:        modules/ccxTemporalFusionModule.cpp
// Purpose:     Temporal fusion module
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

#include "ccxTemporalFusionModule.h"

MODULE_DECLARE(TemporalFusion, "native", "Temporal fusion module");

ccxTemporalFusionModule::ccxTemporalFusionModule() : ccxModule(CCX_MODULE_INPUT | CCX_MODULE_OUTPUT) {

	MODULE_INIT();

	this->input = new ccxDataStream("mAST");
    this->output = new ccxDataStream("mAST");
            
    this->declareInput(0, &this->input, new ccxDataStreamInfo("data", "mAST", "Multimodal abstract syntax tree"));
    this->declareOutput(0, &this->output, new ccxDataStreamInfo("data", "mAST", "Multimodal abstract syntax tree (fused)"));

	// declare properties her, e.g:
	// this->properties["size"] = new moProperty(1.);
}

ccxTemporalFusionModule::~ccxTemporalFusionModule() {
}

void ccxTemporalFusionModule::notifyData(ccxDataStream *input) {
    assert(input != NULL);
    assert(input == this->input);
    this->notifyUpdate();
}

void ccxTemporalFusionModule::update() {

}

void ccxTemporalFusionModule::start() {
	
	ccxModule::start();
}

void ccxTemporalFusionModule::stop() {

	ccxModule::stop();
}