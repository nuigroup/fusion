/////////////////////////////////////////////////////////////////////////////
// Name:        ccx/ccxProperty.h
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


/***********************************************************************
 ** Some parts Copyright (C) 2010 Movid Authors.  All rights reserved.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **********************************************************************/


#ifndef CCX_PROPERTY_H
#define CCX_PROPERTY_H

#include <ostream>
#include <string>
#include <vector>
#include <map>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

#include "ccxUtils.h"

typedef struct _ccxPoint {
	double x;
	double y;
} ccxPoint;

typedef std::vector<ccxPoint> ccxPointList;

typedef enum _ccxPropertyType {
	CCX_PROPERTY_NONE,
	CCX_PROPERTY_BOOL, 
	CCX_PROPERTY_STRING, 
	CCX_PROPERTY_INTEGER,
	CCX_PROPERTY_DOUBLE,
	CCX_PROPERTY_POINTLIST,
} ccxPropertyType;

class ccxProperty;

typedef void (*ccxPropertyCallback)(ccxProperty *property, void *userdata);

class ccxProperty {
public:
	ccxProperty(bool value, const std::string &description = "");
	ccxProperty(const char *value, const std::string &description = "");
	ccxProperty(std::string value, const std::string &description = "");
	ccxProperty(int value, const std::string &description = "");
	ccxProperty(double value, const std::string &description = "");
	ccxProperty(ccxPointList, const std::string &description = "");
	~ccxProperty();
	
	ccxPropertyType getType();

	void setDescription(const std::string& desc);
	std::string getDescription();

	void setReadOnly(bool);
	bool isReadOnly();
	
	bool asBool();
	std::string asString();
	double asDouble();
	int asInteger();
	ccxPointList asPointList();

	void set(bool value);
	void set(std::string value);
	void set(const char* value);
	void set(int value);
	void set(double value);
	void set(ccxPointList value);

	bool haveMin();
	bool haveMax();
	bool haveChoices();
	int getMin();
	int getMax();
	std::string getChoices();
	void setMin(int val);
	void setMax(int val);
	void setChoices(const std::string &val);

	void setText(bool is_text);
	bool isText();

	void addCallback(ccxPropertyCallback callback, void *userdata);
	void removeCallback(ccxPropertyCallback callback);
	
	friend std::ostream& operator<< (std::ostream& o, const ccxProperty& f);

	static std::string getPropertyTypeName(ccxPropertyType type);
	
private:
	ccxProperty(const ccxProperty& property);
	ccxPropertyType type;
	std::map<ccxPropertyCallback, void*> callbacks;
	std::string description;
	void* val;
	bool readonly;
	bool have_min;
	bool have_max;
	bool have_choices;
	bool is_text;
	int val_min;
	int val_max;
	std::string val_choices;
	
	void free();
	void init(const std::string& description);
	void fireCallback();
};

#endif

