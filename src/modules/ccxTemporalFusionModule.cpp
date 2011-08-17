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

	this->audioInput = new ccxDataStream("mAST");
    this->tactileInput = new ccxDataStream("vector<unimodalLeaf>");
    this->output = new ccxDataStream("mAST");
            
    this->declareInput(0, &this->audioInput, new ccxDataStreamInfo("audio", "mAST", "Multimodal abstract syntax tree (from audio utterance)"));
    this->declareInput(1, &this->tactileInput, new ccxDataStreamInfo("tactile", "vector<unimodalLeaf>", "Series of unimodal actions from the GUI"));
    this->declareOutput(0, &this->output, new ccxDataStreamInfo("fused", "mAST", "Multimodal abstract syntax tree (fused)"));

	// declare properties her, e.g:
	// this->properties["size"] = new moProperty(1.);
}

ccxTemporalFusionModule::~ccxTemporalFusionModule() {
}

void ccxTemporalFusionModule::notifyData(ccxDataStream *audioInput) {
    assert(audioInput != NULL);
    assert(tactileInput != NULL);
    //assert(audioInput == this->audioInput);
    //assert(tactileInput == this->tactileInput);
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
    client::multimodalSyntaxTree* audioTree = (client::multimodalSyntaxTree *)this->audioInput->getData();
    std::vector<client::unimodalLeafNode>* tactileTree = (std::vector<client::unimodalLeafNode> *)this->tactileInput->getData();
    if(audioTree != NULL && tactileTree != NULL) {
        if(audioTree->type == "interaction" && tactileTree->size() > 0) {
            int childcount = 0;
            std::vector<client::unimodalLeafNode>::iterator gesIter = tactileTree->begin();
            BOOST_FOREACH(client::node const& nod, audioTree->children)
            {
                if(boost::apply_visitor(multimodal_node_finder(), nod) && gesIter != tactileTree->end()) {
                    client::multimodalLeafNode node = boost::get<client::multimodalLeafNode>(nod);
                    LOG(CCX_INFO, node.type << " needed");
                    if(node.type == gesIter->type) {
                        LOG(CCX_INFO, "success!!!!!!!!");
                        client::node newNode = *gesIter;
                        audioTree->children.at(childcount).swap(newNode);
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
        if(good) {
            this->output->push(audioTree);
        }
        else {
            this->output->clear();
            this->setError("fusion error");
        }
    }
    else {
        this->output->clear();
    }
}

void ccxTemporalFusionModule::start() {
	
	ccxModule::start();
}

void ccxTemporalFusionModule::stop() {

	ccxModule::stop();
}