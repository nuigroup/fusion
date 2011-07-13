/////////////////////////////////////////////////////////////////////////////
// File:        containers/ccfMultimodalSyntaxTree.h
// Purpose:     Multimodal syntax tree data structure
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

#ifndef _CCF_MMST_H
#define _CCV_MMST_H

#include <iostream>
#include <string>
#include <vector>

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
    namespace qi = boost::qi;
    namespace ascii = boost::spirit::ascii;
    
    struct multimodalSyntaxTree;
    struct multimodalSyntaxTreeNode;
    
    typedef
    boost::variant<
    boost::recursive_wrapper<multimodalSyntaxTree>
    , multimodalLeaf
    , unimodalLeaf
    >
    multimodalSyntaxTreeNode;
    
    struct multimodalSyntaxTree
    {
        std::string type;
        std::string val;
        std::vector<multimodalSyntaxTreeNode> children;
    };
    
    struct multimodalLeaf
    {
        int seq;
        std::string type;
    };
    
    struct unimodalLeaf
    {
        std::string type;
        std::string val;
    };
}


BOOST_FUSION_ADAPT_STRUCT(
    client::multimodalSyntaxTree,
	(std::string, type),
	(std::string, val),
    (std::vector<client::multimodalSyntaxTreeNode>, children)
)