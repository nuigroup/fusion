/////////////////////////////////////////////////////////////////////////////
// Name:        modules/ccxSpiritParserModule.cpp
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

#include "ccxSpiritParserModule.h"

//MODULE_DECLARE(SpiritParser, "native", "Parses attributes out of ASR text to fill in the semantic tree");

//ccxSpiritParserModule::ccxSpiritParserModule() : ccxModule(CCX_MODULE_INPUT | CCX_MODULE_OUTPUT) {
ccxSpiritParserModule::ccxSpiritParserModule() {
    
    //MODULE_INIT();
    
    this->input = NULL;
    //this->output = new ccxDataStream("mAST");
    this-> output = NULL;  
    
    //this->declareInput(0, &this->input, new ccxDataStreamInfo("data", "CCAHypothesis", "CCA recognition hypothesis"));
    //this->declareOutput(0, &this->output, new ccxDataStreamInfo("data", "mAST", "Multimodal abstract syntax tree"));
    
    //this->properties["grammar"] = new ccxProperty("ballworld", "The grammar to use");
                                                              
}


ccxSpiritParserModule::~ccxSpiritParserModule() {
    // delete things
}

/*
void ccxSpiritParserModule::start() {
    // initialize things
    // grammar = the right grammar;
    
    
}

void ccxSpiritParserModule::stop() {
    // stop things
}

void ccxSpiritParserModule::notifyData(ccxDataStream *input) {
    assert(input != NULL);
    assert(input == this->input);
    this->notifyUpdate();
}

void ccxSpiritParserModule::update() {
    this->input->lock();
    if(this->initializeFromString(std::string(this->input->getData()))) {
        this->input->unlock();
        this->output->push(mast);
    }
    else {
        // log failure
    }
    // if input is valid and initializeFromString is good, then set the output to the current mast

}*/



bool ccxSpiritParserModule::initializeFromString(std::string input) {
        
    storage = input; // We will manipulate the contents here.
    
    free(mast);
    mast = new client::multimodalSyntaxTree;
    
    std::string::iterator iter = storage.begin();
    std::string::iterator end = storage.end();
        
    bool r = boost::spirit::qi::phrase_parse(iter, end, grammar, boost::spirit::ascii::space, *mast);
    
    if(r && iter == end) {
        std::cout << "success! sentence: \"" << storage << "\"\n";
        printer(*mast);
        return(true);
    }
    else {
        std::cout << "failure\n";
        return(false);
    }
}

/*
 *
 * Testing framework 
 *
 */

int main(int argc, char *argv[]) {
    
    std::string storage2;
    
    ccxSpiritParserModule* parser = new ccxSpiritParserModule();
        
    if(argc > 2 && strcmp(argv[1], "c") == 0) {
        if(parser->initializeFromString(argv[2])) {
            return(0); 
        } else {
            return(-1); 
        }
    } else if(argc > 2 && strcmp(argv[1], "f") == 0) {
        char const* filename = argv[2];
        
        std::ifstream in(filename, std::ios_base::in);
        
        if (!in)
        {
            std::cerr << "Error: Could not open input file: "
            << filename << std::endl;
            return 1;
        }
        
        in.unsetf(std::ios::skipws); // No white space skipping!
        std::copy(
                  std::istream_iterator<char>(in),
                  std::istream_iterator<char>(),
                  std::back_inserter(storage2));
        char *cstr, *lines;
        
        cstr = new char [storage2.size()+1];
        strcpy (cstr, storage2.c_str());
        
        bool success = true;
        
        lines = strtok(cstr, "/");
        
        while (lines != NULL) {
            success = success && parser->initializeFromString(lines);
            lines = strtok(NULL, "/");
            std::cout << std::endl;
        }
        
        if(success) {
            return(0);
        }
        else 
        {
            return(-1);
        }
        
    } else {
        std::cout << "either provide command line input with c flag or a filename with f flag\n";
    }
}