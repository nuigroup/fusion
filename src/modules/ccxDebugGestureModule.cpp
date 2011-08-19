/////////////////////////////////////////////////////////////////////////////
// Name:        modules/ccxDebugGestureModule.cpp
// Purpose:     Debug gestures
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


#include "ccxDebugGestureModule.h"

MODULE_DECLARE(DebugGesture, "native", "Passes through vector<unimodalLeaf> onto the stream for tactile GUI events");

ccxDebugGestureModule::ccxDebugGestureModule() : ccxModule(CCX_MODULE_INPUT || CCX_MODULE_OUTPUT) {
    MODULE_INIT();
    
    this->output = new ccxDataStream("vector<unimodalLeaf>");
	this->declareOutput(0, &this->output, new ccxDataStreamInfo(
			"tactiles", "vector<unimodalLeaf>", "Vector of tactile GUI events"));
    this->input = new ccxDataStream("vector<unimodalLeaf");
    this->declareInput(0, &this->input, new ccxDataStreamInfo("debugtactiles", "vector<unimodalLeaf>", "Vector of tactile GUI events"));
    
}

ccxDebugGestureModule::~ccxDebugGestureModule() {
}

void ccxDebugGestureModule::update() {
    LOG(CCX_INFO, "passing through");
    std::vector<client::unimodalLeafNode> *gestureTree = NULL;
    if(this->input != NULL) gestureTree = (std::vector<client::unimodalLeafNode> *)this->input->getData();
    if(gestureTree != NULL) this->output->push(gestureTree);
    else this->output->clear();
}

void ccxDebugGestureModule::start() {
    ccxModule::start();
}