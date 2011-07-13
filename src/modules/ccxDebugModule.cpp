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


#include "ccxDebugModule.h"
#include "ccxDataStream.h"
#include "ccxDataGenericContainer.h"
#include "ccxLog.h"
#include "ccxModule.h"

MODULE_DECLARE(Debug, "native", "Print streams/structures to the debug console in text format");

ccxDebugModule::ccxDebugModule() : ccxModule(CCX_MODULE_INPUT) {
	this->stream = NULL;
	this->declareInput(0, &this->stream, new ccxDataStreamInfo(
			"data", "*", "Show any stream that conforms to GenericList in text format"));
}

ccxDebugModule::~ccxDebugModule() {
}

void ccxDebugModule::update() {
}

void ccxDebugModule::notifyData(ccxDataStream *stream) {
	LOG(CCX_INFO, "stream<" << stream << ">, type=" << stream->getFormat() << ", observers=" << stream->getObserverCount());

	if ( stream->getFormat() == "mst" ||
		 stream->getFormat() == "json" ) {
		ccxDataGenericList *list = static_cast<ccxDataGenericList*>(stream->getData());
		LOG(CCX_INFO, " `- " << stream->getFormat() << " size=" << list->size());
	}
}

