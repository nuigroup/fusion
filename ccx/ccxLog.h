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


#ifndef CCX_LOG_H
#define CCX_LOG_H

#include <sstream>
#include <iostream>
#include <string>

#ifdef NO_LOG

#define LOG_DECLARE(x)
#define LOG(level, x)
#define LOGX(level, x)
#define LOGM(level, x)

#else // NO_LOG

#define LOG_DECLARE(x) \
	static char log_name[] = x;

#define LOG(level, x) do { \
	if ( level <= g_loglevel ) \
		ccxLogMessage(log_name, __FILE__, __LINE__, level) << x; \
} while (0);

#define LOGX(level, x) do { \
	if ( level <= g_loglevel ) \
		ccxLogMessage(this->getName(), __FILE__, __LINE__, level) << x; \
} while (0);

#define LOGM(level, x) do { \
	if ( level <= g_loglevel ) \
		ccxLogMessage(log_name, __FILE__, __LINE__, level) \
		<< "<" << this->property("id").asString() << "> " << x; \
} while(0);

#endif // NO_LOG

#define _LOG_FUNC { \
	if ( this->level <= g_loglevel ) \
		this->os << __n; \
	return *this; \
}

extern int g_loglevel;

enum {
	CCX_CRITICAL		= 0,
	CCX_ERROR		= 1,
	CCX_WARNING		= 2,
	CCX_INFO			= 3,
	CCX_DEBUG		= 4,
	CCX_TRACE		= 5,
};

class ccxLog {
public:
	static void init(bool use_syslog);
	static void cleanup();
	static void setLogLevel(int n);
	static int getLogLevel();
	static int getSysLogLevel(int n);
	static std::string getLogLevelName(int n);
};

class ccxLogMessage {
public:
	ccxLogMessage(std::string name, std::string filename, int line, int level);
	~ccxLogMessage();

    ccxLogMessage &operator<<(bool __n) _LOG_FUNC;
	ccxLogMessage &operator<<(char __n) _LOG_FUNC;
    ccxLogMessage &operator<<(short __n) _LOG_FUNC;
    ccxLogMessage &operator<<(int __n) _LOG_FUNC;
    ccxLogMessage &operator<<(long __n) _LOG_FUNC;
	ccxLogMessage &operator<<(unsigned char __n) _LOG_FUNC;
    ccxLogMessage &operator<<(unsigned short __n) _LOG_FUNC;
    ccxLogMessage &operator<<(unsigned int __n) _LOG_FUNC;
    ccxLogMessage &operator<<(unsigned long __n) _LOG_FUNC;
    ccxLogMessage &operator<<(float __n) _LOG_FUNC;
    ccxLogMessage &operator<<(double __n) _LOG_FUNC;
	ccxLogMessage &operator<<(std::string __n) _LOG_FUNC;
	ccxLogMessage &operator<<(const char *__n) _LOG_FUNC;

private:
	std::ostringstream os;
	int level;
};

#endif

