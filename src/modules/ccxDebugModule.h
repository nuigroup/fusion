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


#ifndef CCX_DEBUG_MODULE_H
#define CCX_DEBUG_MODULE_H

#include "ccxModule.h"

class ccxDataStream;

class ccxDebugModule : public ccxModule {
public:
	ccxDebugModule(); 
	virtual ~ccxDebugModule();

	virtual void notifyData(ccxDataStream *stream);
	void update();

private:
	ccxDataStream *stream;
	MODULE_INTERNALS();
};


#endif

