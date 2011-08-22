/////////////////////////////////////////////////////////////////////////////
// Name:        modules/ccxParserModule.h
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

#ifndef CCX_PARSER_MODULE_H
#define CCX_PARSER_MODULE_H

#include "ccx.h"

class ccxParserGrammar {
    
}

class ccxParserModule : public ccxModule {
    
public:
    virtual ccxParserModule();
    virtual ~ccxParserModule();
    
    virtual void notifyData();
    virtual void update();
    virtual void stop();
    
protected:
    
    
};

#endif