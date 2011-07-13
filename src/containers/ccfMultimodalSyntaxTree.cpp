/////////////////////////////////////////////////////////////////////////////
// Name:        containers/ccfMultimodalSyntaxTree.cpp
// Purpose:     Multimodal syntax tree implementation
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

#include "ccfMultimodalSyntaxTree.h"

namespace client {
    template <typename Iterator>
    struct multimodalSyntaxTree_grammar() : qi::grammar<Iterator, syntaxTree(), ascii::space_type>
    {
        multimodalSyntaxTree_grammar : multimodalSyntaxTree_grammar::base_type(interaction)
        {
            
            interaction %= moveInteraction | colorInteraction | destroyInteraction | createInteraction;
            
            moveInteraction = moveCommand > (ballSelector >> point);
            colorInteraction = colorCommand > (ballSelector >> color); // build an interaction object, 
            
            
            moveCommand = lit('move') | lit('put') [_val = 'move']; // val should be tuple<string type, string command>
            colorCommand %= string('color');
            destroyCommand = lit('delete') | lit('remove') | lit('destroy') [_val = 'destroy'];
            createCommand = lit('make') | lit('create') | lit('put') [_val = 'create'];
            
            ball = ballSelectorUnimodal | ballSelectorMultimodal; // pass on the ball object
            
            point = pointSelectorMultimodal; // build a point object
            
            color = colorSelectorUnimodal; // build a color object (or its data), name of the rule is the type attribute and val is the returned attribute
            
            colorSelectorUnimodal %= string('red') | string('green') | string('blue') | string('yellow') | string('orange') | string('white') | string('purple'); // return a domain type, this is terminal
            
            newBall %= (lit('an') | lit('a')) >> -lit('new') >> color >> (lit('ball') | lit('one'));
            
            
        }
        
        qi::rule<Iterator, multimodalSyntaxTree(), ascii::space_type> interaction;
        
        qi::rule<Iterator, multimodalSyntaxTree(), ascii::space_type> moveInteraction;
        qi::rule<Iterator, multimodalSyntaxTree(), ascii::space_type> colorInteraction;
        qi::rule<Iterator, multimodalSyntaxTree(), ascii::space_type> createInteraction;
        qi::rule<Iterator, multimodalSyntaxTree(), ascii::space_type> destroyInteraction;
        
		
        qi::rule<Interator, std::string(), , ascii::space_type> moveCommand;
        qi::rule<Iterator, std::string(), ascii::space_type> colorCommand;
        qi::rule<Interator, std::string(), ascii::space_type> createCommand;
        qi::rule<Iterator, std::string(), ascii::space_type> destroyCommand;
        
        qi::rule<Iterator, multimodalSyntaxTreeNode(), ascii::space_type> ball;
        
        qi::rule<Iterator, multimodalLeaf(), ascii::space_type> ballSelectorMultimodal;
        qi::rule<Iterator, unimodalLeaf(), ascii::space_type> ballSelectorUnimodal;
        
        qi::rule<Iterator, multimodalLeaf(), ascii::space_type> pointSelectorMultimodal;
        
        qi::rule<Iterator, unimodalLeaf(), ascii::space_type> newBall;
        
        qi::rule<Iterator, unimodalLeaf(), ascii::space_type> color;
        
        
    };
}