/////////////////////////////////////////////////////////////////////////////
// Name:        ccx/ccxFactory.h
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


/***********************************************************************
 ** Some parts Copyright (C) 2010 Movid Authors.  All rights reserved.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **********************************************************************/


#ifndef CCX_FACTORY_H
#define CCX_FACTORY_H

#include <vector>
#include <map>
#include <string>

#include "ccxLog.h"
#include "ccxModule.h"

typedef ccxModule *(*ccxFactoryCreateCallback)();

class ccxFactory {
public:
	~ccxFactory();

	static ccxFactory *getInstance();
	static void init();
	static void cleanup();

	void registerModule(const std::string &name, ccxFactoryCreateCallback callback);

	ccxModule *create(const std::string &name);
	std::vector<std::string> list();

protected:
	ccxFactory();

	std::map<std::string, ccxFactoryCreateCallback> database;
};

#endif

