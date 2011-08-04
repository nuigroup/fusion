/////////////////////////////////////////////////////////////////////////////
// Name:        modules/ccxSpiritParserModule.h
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

#include "ccfMultimodalSyntaxTree.cpp"

#ifndef CCX_SPIRIT_PARSER_MODULE_H
#define CCX_SPIRIT_PARSER_MODULE_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/foreach.hpp>

#include "ccx.h"

//class ccxSpiritParserModule : public ccxModule {
class ccxSpiritParserModule {
public:
    ccxSpiritParserModule();
    virtual ~ccxSpiritParserModule();
    
    //void notifyData(ccxDataStream*);
    //void update();
    //void stop();
    //void start();
    
    bool initializeFromString(std::string input);
    
protected:
    ccxDataStream* input;
    ccxDataStream* output;
    
    std::string storage;

    client::multimodalSyntaxTree *mast;
    client::multimodalSyntaxTree_grammar<std::string::iterator> grammar;
    
    client::mast_printer printer;

    //MODULE_INTERNALS();
};

#endif