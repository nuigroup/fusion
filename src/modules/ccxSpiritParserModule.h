/////////////////////////////////////////////////////////////////////////////
// Name:        modules/ccxSpiritParserModule.h
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

#ifndef CCX_SPIRIT_PARSER_MODULE_H
#define CCX_SPIRIT_PARSER_MODULE_H

#include "ccfMultimodalSyntaxTree.h"

#include "ballWorldGrammar.cpp" // do this more elegantly later

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <boost/config/warning_disable.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/foreach.hpp>

#include "ccx.h"

class ccxSpiritParserModule : public ccxModule {
public:
    ccxSpiritParserModule();
    virtual ~ccxSpiritParserModule();
    
    void notifyData(ccxDataStream*);
    void update();
    void stop();
    void start();
    
    bool initializeFromString(std::string input);
        
protected:
    
    ccxDataStream* input;
    ccxDataStream* output;
    
    std::string storage;

    client::multimodalSyntaxTree *mast;
    client::spiritGrammar<std::string::iterator> grammar;

    MODULE_INTERNALS();
};

#endif