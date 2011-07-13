/***********************************************************************
 ** Copyright (C) 2010 Movid Authors.  All rights reserved.
 **
 ** This file is part of the Movid Software.
 **
 ** This file may be distributed under the terms of the Q Public License
 ** as defined by Trolltech AS of Norway and appearing in the file
 ** LICENSE included in the packaging of this file.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** Contact info@movid.org if any conditions of this licensing are
 ** not clear to you.
 **
 **********************************************************************/


#ifndef CCX_FACTORY_H
#define CCX_FACTORY_H

#include <vector>
#include <map>
#include <string>

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

