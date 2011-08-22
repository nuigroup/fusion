/////////////////////////////////////////////////////////////////////////////
// Name:        ccx/ccxDataGenericContainer.h
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

/***********************************************************************
 ** Some parts Copyright (C) 2010 Movid Authors.  All rights reserved.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **********************************************************************/


#ifndef CCX_DATASTRUCT_H
#define CCX_DATASTRUCT_H

#include "ccxProperty.h"

#include <string>
#include <map>
#include <vector>

class ccxProperty;

class ccxDataGenericContainer {
public:
	ccxDataGenericContainer();
	virtual ~ccxDataGenericContainer();

	bool hasProperty(const std::string &name);

	std::map<std::string, ccxProperty*> properties;

    ccxDataGenericContainer* clone();

};

typedef std::vector<ccxDataGenericContainer *> ccxDataGenericList;

#endif

