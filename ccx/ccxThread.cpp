/////////////////////////////////////////////////////////////////////////////
// Name:        ccx/ccxThread.cpp
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


#include "ccxThread.h"

ccxThread::ccxThread(thread_process_t _process, void *_userdata) : pt::thread(false) {
	this->process = _process;
	this->userdata = _userdata;
	this->want_quit = false;
}


ccxThread::~ccxThread() {
}

void ccxThread::execute() {
	this->process(this);
}

void *ccxThread::getUserData() {
	return this->userdata;
}

void ccxThread::stop() {
	this->want_quit = true;
}

bool ccxThread::wantQuit() {
	return this->want_quit;
}

bool ccxThread::relax(int m) {
	return pt::thread::relax(m);
}
