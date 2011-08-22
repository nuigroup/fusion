/////////////////////////////////////////////////////////////////////////////
// Name:        ccx/ccxDataGenericContainer.cpp
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

/***********************************************************************
 ** Some parts Copyright (C) 2010 Movid Authors.  All rights reserved.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **********************************************************************/


#include "ccxDataGenericContainer.h"

ccxDataGenericContainer::ccxDataGenericContainer() {
}

ccxDataGenericContainer::~ccxDataGenericContainer() {
	std::map<std::string, ccxProperty*>::iterator it;
	for ( it = this->properties.begin(); it != this->properties.end(); it++ )
		delete (it->second);
	this->properties.clear();
}


ccxDataGenericContainer* ccxDataGenericContainer::clone(){
	std::map<std::string, ccxProperty*>::iterator it;
    ccxDataGenericContainer* clone = new ccxDataGenericContainer();
    for ( it = this->properties.begin(); it != this->properties.end(); it++ )
		clone->properties[it->first] = new ccxProperty(it->second->asString());
    return clone;
}

bool ccxDataGenericContainer::hasProperty(const std::string &name) {
	std::map<std::string, ccxProperty*>::iterator it;
	it = this->properties.find(name);
	return it == this->properties.end() ? false : true;
}

