/////////////////////////////////////////////////////////////////////////////
// Name:        containers/ccfMultimodalSyntaxTree.cpp
// Purpose:     Multimodal syntax tree implementation
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

#include "ccfMultimodalSyntaxTree.h"
  
int const tabsize = 4;

void tab(int indent)
{
    for (int i = 0; i < indent; ++i) {
        std::cout << ' ';
    }
}

struct mast_printer
{
    mast_printer(int indent = 0) : indent(indent) {}
    
    void operator()(client::multimodalSyntaxTree const& mast) const;
    
    int indent;
};
    
struct mast_node_printer : boost::static_visitor<>
{
    mast_node_printer(int indent = 0) : indent(indent) {}
    
    void operator()(client::multimodalSyntaxTree const& mast) const {
        mast_printer(indent+tabsize)(mast);
    }
    
    void operator()(client::multimodalLeafNode const& mmNode) const {
        tab(indent+tabsize);
        std::cout << "|multimodal| type: \"" << mmNode.type << "\" |\n";
    }
    
    void operator()(client::unimodalLeafNode const& umNode) const {
        tab(indent+tabsize);
        std::cout << "|unimodal| type: \"" << umNode.type << "\", val: \"" << umNode.val << "\" |\n";
    }
    
    int indent;
};



void mast_printer::operator()(client::multimodalSyntaxTree const& mast) const
{
    tab(indent);
    std::cout << "|abstract| type: \"" << mast.type << "\", val: \"" << mast.val << "\", children.size(): " << mast.children.size() << " |\n";
    tab(indent);
    std::cout << '{' << std::endl;
    BOOST_FOREACH(client::node const& nod, mast.children)
    {
        boost::apply_visitor(mast_node_printer(indent), nod);
    }
    tab(indent);
    std::cout << '}' << std::endl;
}

std::string mast_to_string(client::multimodalSyntaxTree *mast) {
    mast_printer printer;
    printer(*mast);
    return "";
}

class mast_iterator {
    ;
    
public:
    ;
};
