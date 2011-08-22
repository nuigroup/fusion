/////////////////////////////////////////////////////////////////////////////
// Name:        ccx/ccxDaemon.h
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

/***********************************************************************
 ** Some parts Copyright (C) 2010 Movid Authors.  All rights reserved.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **********************************************************************/


#ifndef CCX_DAEMON_H
#define CCX_DAEMON_H

class ccxDaemon {
public:
	static void init();
	static bool detach(std::string pidfilename);
	static void cleanup();
};

#endif
