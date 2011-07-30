/////////////////////////////////////////////////////////////////////////////
// File:        containers/ccfMultimodalSyntaxTree.h
// Purpose:     Multimodal syntax tree data structure
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

#ifndef _CCF_MMST_H
#define _CCV_MMST_H

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

namespace client
{
    namespace fusion = boost::fusion;
    namespace phoenix = boost::phoenix;
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    
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

BOOST_FUSION_ADAPT_STRUCT(
                          client::multimodalSyntaxTree,
                          (std::string, val)
                          (std::string, type)
                          (std::vector<client::node>, children)
                          )

BOOST_FUSION_ADAPT_STRUCT(
                          client::multimodalLeafNode,
                          (std::string, type)
                          )

BOOST_FUSION_ADAPT_STRUCT(
                          client::unimodalLeafNode,
                          (std::string, val)
                          (std::string, type)
                          )

#endif