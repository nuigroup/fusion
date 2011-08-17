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


#ifndef CCX_PIPELINE_H
#define CCX_PIPELINE_H

#include <vector>
#include <string>
#include "ccxModule.h"

#define PIPELINE_BOUNDARY "=============================="

class ccxPipeline : public ccxModule {
public:

	ccxPipeline();
	virtual ~ccxPipeline();

	virtual void setInput(ccxDataStream* stream, int n=0);
	virtual ccxDataStream *getInput(int n=0);
	virtual ccxDataStream *getOutput(int n=0);
    
    void clearInputs();

	virtual void addElement(ccxModule *module);
	virtual void removeElement(ccxModule *module);

	virtual int getInputCount();
	virtual int getOutputCount();
	virtual ccxDataStreamInfo *getInputInfos(int n=0);
	virtual ccxDataStreamInfo *getOutputInfos(int n=0);

	virtual void start();
	virtual void stop();
	virtual void update();
	virtual void poll();
	virtual void clear();

	ccxModule *firstModule();
	ccxModule *lastModule();
	ccxModule *getModule(unsigned int index);
	ccxModule *getModuleById(const std::string &id);
	unsigned int size();

	virtual void setGroup(bool group=true);
	virtual bool isGroup();
	virtual bool isPipeline();

	/*! \brief Get last error message, once called, it will reset the error state
	 */
	virtual std::string getLastError();

	/*! \brief Indicate if the module have an error
	 */
	virtual bool haveError();

	/*! \brief Dump the pipeline into a file
	 */
	virtual std::string serializeCreation();

	/*! \brief Parse a file, and inject to the current pipeline
	 */
	bool parse(const std::string& filename);

private:
	std::vector<ccxModule *> modules;
	bool is_group;
	std::string last_internal_error;

	MODULE_INTERNALS();
};

#endif

