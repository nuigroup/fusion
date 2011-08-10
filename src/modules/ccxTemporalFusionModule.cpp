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

struct multimodal_node_finder : boost::static_visitor<bool>
{
    bool operator()(client::multimodalLeafNode const& mmNode) const {
        return true;
    }
    bool operator()(client::unimodalLeafNode const& umNode) const {
        return false;
    }
    bool operator()(client::multimodalSyntaxTree const& mast) const {
        return false;
    }
};

void ccxTemporalFusionModule::update() {
    bool good = true;
    client::multimodalSyntaxTree* inputTree = (client::multimodalSyntaxTree *)this->input->getData();
    std::vector<client::unimodalLeafNode> gestureTree;
    client::unimodalLeafNode* debugNode = new client::unimodalLeafNode;
    debugNode->type = "ball";
    debugNode->val = "01";
    //gestureTree.push_back(*debugNode);
    debugNode = new client::unimodalLeafNode;
    debugNode->type = "point";
    debugNode->val = "(0,22)";
    gestureTree.push_back(*debugNode);
    if(inputTree->type == "interaction") {
        //mast_to_string(inputTree);
        int childcount = 0;
        std::vector<client::unimodalLeafNode>::iterator gesIter = gestureTree.begin();
        BOOST_FOREACH(client::node const& nod, inputTree->children)
        {
            if(boost::apply_visitor(multimodal_node_finder(), nod) && gesIter != gestureTree.end()) {
                client::multimodalLeafNode node = boost::get<client::multimodalLeafNode>(nod);
                LOG(CCX_INFO, node.type << " needed");
                if(node.type == gesIter->type) {
                    LOG(CCX_INFO, "success!!!!!!!!");
                    inputTree->children.at(childcount) = *gesIter;
                }
                else {
                    LOG(CCX_INFO, "mismatch");
                    good = false;
                }
                gesIter++;
            }
            childcount++;
        }
    }
    if(good) this->output->push(inputTree);
}

void ccxTemporalFusionModule::start() {
	
	ccxModule::start();
}

void ccxTemporalFusionModule::stop() {

	ccxModule::stop();
}