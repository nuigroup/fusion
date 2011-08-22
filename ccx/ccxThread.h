/////////////////////////////////////////////////////////////////////////////
// Name:        ccx/ccxThread.h
// Author:      Scott Halstvedt
// Purpose:     Basic abstraction of thread/semaphore for WIN32 and Posix
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


/***********************************************************************
 ** Some parts Copyright (C) 2010 Movid Authors.  All rights reserved.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **********************************************************************/


#ifndef CCX_THREAD_H
#define CCX_THREAD_H

#include "pasync.h"

class ccxThread;

typedef void (*thread_process_t)(ccxThread *thread);

class ccxThread : public pt::thread {
public:
	ccxThread(thread_process_t _process, void *_userdata);
	virtual ~ccxThread();
	void *getUserData();
	void execute();
	void stop();
	bool wantQuit();
	bool relax(int);

private:
	thread_process_t process;
	void *userdata;
	bool want_quit;
};

#endif
