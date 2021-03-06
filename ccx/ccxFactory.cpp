/////////////////////////////////////////////////////////////////////////////
// Name:        ccx/ccxFactory.cpp
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


/***********************************************************************
 ** Some parts Copyright (C) 2010 Movid Authors.  All rights reserved.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **********************************************************************/


#include "ccxFactory.h"

#define REGISTER_MODULE(name) \
	extern ccxModule* factory_create_ccx##name(); \
	LOG(CCX_DEBUG, "register <" #name "> module"); \
	ccxFactory::getInstance()->registerModule(#name, factory_create_ccx##name);

LOG_DECLARE("Factory");

static ccxFactory *instance = NULL;

void ccxFactory::init() {
	// Keep the alphabetical order when adding new modules!
    REGISTER_MODULE(AudioOutput);
    REGISTER_MODULE(DebugGesture);
    REGISTER_MODULE(DebugGestureOutput);
    REGISTER_MODULE(DebugInput);
    REGISTER_MODULE(DebugOutput);
    REGISTER_MODULE(JSONInteraction);
    REGISTER_MODULE(SphinxASR);
    REGISTER_MODULE(SpiritParser);
	REGISTER_MODULE(TemporalFusion);
    //DoNotRemoveThisComment
	LOG(CCX_INFO, "registered " << ccxFactory::getInstance()->list().size() << " modules");
}

void ccxFactory::cleanup() {
	delete instance;
}

ccxFactory::ccxFactory() {
}

ccxFactory::~ccxFactory() {
}

ccxFactory *ccxFactory::getInstance() {
	if ( instance == NULL )
		instance = new ccxFactory();
	return instance;
}

void ccxFactory::registerModule(const std::string &name, ccxFactoryCreateCallback callback) {
	this->database[name] = callback;
}

ccxModule *ccxFactory::create(const std::string &name) {
	std::string sname = std::string(name);

	// for easier creation, remove the ot at start
	if ( sname.size() > 2 && sname.substr(0, 2) == "ccx" )
		sname =	sname.substr(2, sname.length() - 2);
	if ( sname.size() > 6 && sname.substr(sname.length() - 6, 6) == "Module" )
		sname = sname.substr(0, sname.length() - 6);

	std::map<std::string, ccxFactoryCreateCallback>::iterator it;
	it = this->database.find(sname);
	if ( it == this->database.end() )
		return NULL;
	return (it->second)();
}

std::vector<std::string> ccxFactory::list() {
	std::vector<std::string> l;
	std::map<std::string, ccxFactoryCreateCallback>::iterator it;
	for ( it = this->database.begin(); it != this->database.end(); it++ )
		l.push_back(it->first);
	return l;
}

