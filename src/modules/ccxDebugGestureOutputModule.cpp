/////////////////////////////////////////////////////////////////////////////
// Name:        modules/ccxCCASpeechRecognitionModule.cpp
// Purpose:     CCA speech recognition output module
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


#include "ccxDebugGestureOutputModule.h"

MODULE_DECLARE(DebugGestureOutput, "native", "Pushes a vector<unimodalLeaf> onto the stream for tactile GUI events");

ccxDebugGestureOutputModule::ccxDebugGestureOutputModule() : ccxModule(CCX_MODULE_OUTPUT) {
    MODULE_INIT();
    
    this->stream = new ccxDataStream("vector<unimodalLeaf>");
	this->declareOutput(0, &this->stream, new ccxDataStreamInfo(
			"tactiles", "vector<unimodalLeaf>", "Vector of tactile GUI events"));
    this->alreadySent = false;
    
}

ccxDebugGestureOutputModule::~ccxDebugGestureOutputModule() {
}

void ccxDebugGestureOutputModule::update() {
    LOG(CCX_INFO, "updating");
    std::vector<client::unimodalLeafNode> *gestureTree = new std::vector<client::unimodalLeafNode>();
    client::unimodalLeafNode* debugNode = new client::unimodalLeafNode;
    debugNode->type = "ball";
    debugNode->val = "0";
    gestureTree->push_back(*debugNode);
    debugNode = new client::unimodalLeafNode;
    debugNode->type = "point";
    debugNode->val = "(0,22)";
    gestureTree->push_back(*debugNode);
    this->stream->push(gestureTree);
}

void ccxDebugGestureOutputModule::start() {
    this->alreadySent = false;
    ccxModule::start();
}

void ccxDebugGestureOutputModule::poll() {
    if(!this->alreadySent) {
        this->notifyUpdate();
        this->alreadySent = true;
    }
    ccxModule::poll();
}

void ccxDebugGestureOutputModule::notifyData(ccxDataStream *stream) {
}

