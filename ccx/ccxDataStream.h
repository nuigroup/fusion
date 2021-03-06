/////////////////////////////////////////////////////////////////////////////
// Name:        ccx/ccxDataStream.h
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


/***********************************************************************
 ** Some parts Copyright (C) 2010 Movid Authors.  All rights reserved.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **********************************************************************/


#ifndef CCX_DATA_STREAM_H
#define CCX_DATA_STREAM_H

#include <string>
#include <vector>

#include "pasync.h"

class ccxModule;
class ccxDataStream;

class ccxDataStreamInfo {
public:
	ccxDataStreamInfo(const std::string &name,
					 const std::string &type,
					 const std::string &description = "") {
		this->name = name;
		this->type = type;
		this->description = description;
	};

	std::string getName() {
		return this->name;
	}

	std::string getType() {
		return this->type;
	}

	std::string getDescription() {
		return this->description;
	}

	void setType(const std::string &type) {
		this->type = type;
	}

	bool isStreamValid(ccxDataStream *stream);

protected:
	std::string name;
	std::string type;
	std::string description;
};

class ccxDataStream {
	
public:
	ccxDataStream(std::string format); 
	virtual ~ccxDataStream();
	
	void addObserver(ccxModule *module);
	void removeObserver(ccxModule *module);
	void removeObservers();
	unsigned int getObserverCount();
	ccxModule *getObserver(unsigned int index);

	void push(void *data);
	void *getData();
    
    void clear();

	void lock();
	void unlock();

	std::string getFormat();
	void setFormat(const std::string& format);
	
protected:
	std::string format;
	void *data;
	std::vector<ccxModule*> observers;
	pt::mutex *mtx;

	void notifyObservers();
};

#endif

