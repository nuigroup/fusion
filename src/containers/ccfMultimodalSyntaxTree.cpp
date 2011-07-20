/////////////////////////////////////////////////////////////////////////////
// Name:        containers/ccfMultimodalSyntaxTree.cpp
// Purpose:     Multimodal syntax tree implementation
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

#include "ccfMultimodalSyntaxTree.h"

namespace client {
    template <typename Iterator> struct multimodalSyntaxTree_grammar : qi::grammar<Iterator, multimodalSyntaxTree(), ascii::space_type>
	{
        multimodalSyntaxTree_grammar() : multimodalSyntaxTree_grammar::base_type(interaction)
        {
	
			using qi::lit;
			using qi::attr;
			
			using ascii::string;
			using namespace qi::labels;
			
			using phoenix::at_c;
			using phoenix::push_back;
			
            
            interaction %= moveInteraction | colorInteraction | destroyInteraction | createInteraction; // type is name of the rule, val is string, and children is children
            
            moveInteraction = moveCommand [at_c<1>(_val) = _1]
				> (ball [push_back(at_c<2>(_val), _1)]
				> point [push_back(at_c<2>(_val), _1)]
				) [at_c<0>(_val) = "interaction"];
            colorInteraction = colorCommand [at_c<1>(_val) = _1]
 				> (ball [push_back(at_c<2>(_val), _1)]
				> color [push_back(at_c<2>(_val), _1)]
				) [at_c<0>(_val) = "interaction"];
            createInteraction = createCommand [at_c<1>(_val) = _1]
				> (newBall [push_back(at_c<2>(_val), _1)]
				> point [push_back(at_c<2>(_val), _1)]
				) [at_c<0>(_val) = "interaction"];
            destroyInteraction = destroyCommand [at_c<1>(_val) = _1]
				> (ball [push_back(at_c<2>(_val), _1)]
				) [at_c<0>(_val) = "interaction"];
            
            
            moveCommand = lit("move") | lit("put") >> attr("move"); // val should be tuple<string type, string command>
            colorCommand %= string("color");
            destroyCommand = lit("delete") | lit("remove") | lit("destroy") >> attr("destroy");
            createCommand = lit("make") | lit("create") | lit("put") >> attr("create");
            
            ball %= attr("ball") > attr("NULL") > (ballSelectorUnimodal | ballSelectorMultimodal); // pass on the ball node
            ballSelectorUnimodal %= attr("color") > (lit("the") >> color >> (lit("ball") | lit("one")));
            ballSelectorMultimodal %= lit("this") | lit("that") >> -lit("ball");
            
            point = pointSelectorMultimodal; // build a point object
            pointSelectorMultimodal = lit("here") | lit("there");
            
            color = colorSelectorUnimodal; // build a color object (or its data), name of the rule is the type attribute and val is the returned attribute
            
            colorSelectorUnimodal %= (string("red") | string("green") | string("blue") | string("yellow") | string("orange") | string("white") | string("purple")); // return a domain value, this is terminal
            
            newBall %= (lit("an") | lit("a")) >> -lit("new") >> color >> (lit("ball") | lit("one"));
            
            
            
        }
        
        qi::rule<Iterator, multimodalSyntaxTree(), ascii::space_type> interaction;
        
        qi::rule<Iterator, multimodalSyntaxTree(), ascii::space_type> moveInteraction;
        qi::rule<Iterator, multimodalSyntaxTree(), ascii::space_type> colorInteraction;
        qi::rule<Iterator, multimodalSyntaxTree(), ascii::space_type> createInteraction;
        qi::rule<Iterator, multimodalSyntaxTree(), ascii::space_type> destroyInteraction;
		
        qi::rule<Iterator, std::string(), ascii::space_type> moveCommand;
        qi::rule<Iterator, std::string(), ascii::space_type> colorCommand;
        qi::rule<Iterator, std::string(), ascii::space_type> createCommand;
        qi::rule<Iterator, std::string(), ascii::space_type> destroyCommand;
        
        qi::rule<Iterator, multimodalSyntaxTreeNode(), ascii::space_type> ball;
        qi::rule<Iterator, multimodalLeaf(), ascii::space_type> ballSelectorMultimodal;
        qi::rule<Iterator, unimodalLeaf(), ascii::space_type> ballSelectorUnimodal;

        qi::rule<Iterator, multimodalSyntaxTreeNode(), ascii::space_type> newBall;

		qi::rule<Iterator, multimodalSyntaxTreeNode(), ascii::space_type> point;
        qi::rule<Iterator, multimodalLeaf(), ascii::space_type> pointSelectorMultimodal;
                
        qi::rule<Iterator, multimodalSyntaxTreeNode(), ascii::space_type> color;
        qi::rule<Iterator, std::string(), ascii::space_type> colorSelectorUnimodal;
        
        
    };
}