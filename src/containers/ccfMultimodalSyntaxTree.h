/////////////////////////////////////////////////////////////////////////////
// File:        containers/ccfMultimodalSyntaxTree.h
// Purpose:     Multimodal syntax tree data structure
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

#ifndef _CCF_MMST_H
#define _CCF_MMST_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#include <boost/config/warning_disable.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/get.hpp>
#include <boost/foreach.hpp>

#include "cJSON.h"

namespace client
{    
    struct multimodalSyntaxTree;
	struct unimodalLeafNode;
	struct multimodalLeafNode;
    
    typedef
        boost::variant<
            boost::recursive_wrapper<multimodalSyntaxTree>
            , multimodalLeafNode
            , unimodalLeafNode
        >
    node;
    
    struct multimodalSyntaxTree
    {
        std::string val;
        std::string type;
        std::vector<node> children;
    };
    
    struct multimodalLeafNode
    {
       	std::string type;
    };
    
    struct unimodalLeafNode
    {
        std::string val;
        std::string type;
    };
    
}

std::string mast_to_string(client::multimodalSyntaxTree *mast);

cJSON* mast_to_json(client::multimodalSyntaxTree *mast);

#endif