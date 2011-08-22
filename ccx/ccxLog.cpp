/////////////////////////////////////////////////////////////////////////////
// Name:        ccx/ccxLog.cpp
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


/***********************************************************************
 ** Some parts Copyright (C) 2010 Movid Authors.  All rights reserved.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **********************************************************************/


#include "ccxLog.h"

static pt::mutex(logmtx);

int g_loglevel = CCX_INFO;
bool g_use_syslog = false;

ccxLogMessage::ccxLogMessage(std::string name, std::string filename,
						   int line, int level) {
	char buffer[64];
	time_t t;
	struct tm *tmp;

	if (! g_use_syslog) {
		t = time(NULL);
		tmp = localtime(&t);
		strftime(buffer, sizeof(buffer), "%H:%M:%S", tmp);

		this->os << buffer << " | ";
	}
	this->os << ccxLog::getLogLevelName(level) << " | ";
	this->os << (const char *)name.c_str() << " | ";
	this->level = level;
}

ccxLogMessage::~ccxLogMessage() {
	if (this->level <= g_loglevel) {
		logmtx.lock();
#ifndef WIN32
		if (g_use_syslog) {
			syslog(LOG_USER | ccxLog::getSysLogLevel(this->level), "%s", this->os.str().c_str());
		} else {
			std::cout << this->os.str() << std::endl;
		}
#else
		std::cout << this->os.str() << std::endl;
#endif
		logmtx.unlock();
	}
}


void ccxLog::init(bool use_syslog) {
#ifndef WIN32
	g_use_syslog = use_syslog;
#endif
	g_loglevel = CCX_INFO;
	if (getenv("CCX_DEBUG"))
		g_loglevel = CCX_DEBUG;
	if (getenv("CCX_TRACE"))
		g_loglevel = CCX_TRACE;
}

void ccxLog::cleanup() {
}

int ccxLog::getLogLevel() {
	return g_loglevel;
}

void ccxLog::setLogLevel(int n) {
	g_loglevel = n;
}

int ccxLog::getSysLogLevel(int n) {
#ifndef WIN32
	switch (n) {
		case CCX_CRITICAL:	return LOG_CRIT;
		case CCX_ERROR:		return LOG_ERR;
		case CCX_WARNING:	return LOG_WARNING;
		case CCX_INFO:		return LOG_INFO;
		case CCX_DEBUG:		return LOG_DEBUG;
		default:		return 0;
	}
#endif
	// TODO implement for other platform
	return 0;
}

std::string ccxLog::getLogLevelName(int n) {
	switch (n) {
		case CCX_CRITICAL:	return "Critical";
		case CCX_ERROR:		return "Error";
		case CCX_WARNING:	return "Warning";
		case CCX_INFO:		return "Info";
		case CCX_DEBUG:		return "Debug";
		case CCX_TRACE:		return "Trace";
	}
	return "Unknown";
}
