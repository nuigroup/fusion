/////////////////////////////////////////////////////////////////////////////
// Name:        modules/ccxSpiritParserModule.cpp
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

#include "ccxSpiritParserModule.h"

MODULE_DECLARE(SpiritParser, "native", "Parses attributes out of ASR text to fill in the semantic tree");

ccxSpiritParserModule::ccxSpiritParserModule() : ccxModule(CCX_MODULE_INPUT | CCX_MODULE_OUTPUT) {
    
    MODULE_INIT();
    
    this->input = new ccxDataStream("CCAHypothesis");
    this->output = new ccxDataStream("mAST");
            
    this->declareInput(0, &this->input, new ccxDataStreamInfo("data", "CCAHypothesis", "CCA recognition hypothesis"));
    this->declareOutput(0, &this->output, new ccxDataStreamInfo("data", "mAST", "Multimodal abstract syntax tree"));
    
    this->properties["grammar"] = new ccxProperty("ballworld", "The grammar to use");
                                                              
}


ccxSpiritParserModule::~ccxSpiritParserModule() {
    if(this->mast != NULL) delete(this->mast);
}


void ccxSpiritParserModule::start() {
    // initialize things    
    this->mast = new client::multimodalSyntaxTree;
    ccxModule::start();
    LOG(CCX_INFO, "started!");
}

void ccxSpiritParserModule::stop() {
    LOG(CCX_INFO, "stopped!");
    ccxModule::stop();
}

void ccxSpiritParserModule::notifyData(ccxDataStream *input) {
    assert(input != NULL);
    assert(input == this->input);
    this->notifyUpdate();
}

void ccxSpiritParserModule::update() {
    this->input->lock();
    if(this->initializeFromString(std::string((char*)this->input->getData()))) {
        this->output->push(mast);
    }
    else {
        this->output->clear();
    }
    this->input->unlock();
}



bool ccxSpiritParserModule::initializeFromString(std::string input) {
        
    storage = input; // We will manipulate the contents here.
    
    delete(mast);
    mast = new client::multimodalSyntaxTree;
    
    std::string::iterator iter = storage.begin();
    std::string::iterator end = storage.end();
        
    bool r = boost::spirit::qi::phrase_parse(iter, end, grammar, boost::spirit::ascii::space, *mast);
    
    if(r && iter == end) {
        LOG(CCX_INFO, "success! sentence: \"" << storage << "\"");
        return(true);
    }
    else {
        LOG(CCX_INFO, "failure! sentence: \"" << storage << "\"");
        return(false);
    }
}