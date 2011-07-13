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


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include "ccxProperty.h"
#include "ccxUtils.h"

#ifdef _WIN32
	#define snprintf _snprintf
#endif

#define CASTEDGET(x) x value = *(static_cast<x*>(val));

#define AUTOCONVERT(typein, value) \
	switch ( this->type ) { \
		case CCX_PROPERTY_BOOL: \
			*(static_cast<bool*>(this->val)) = convertToBool(typein, &value); \
			break; \
		case CCX_PROPERTY_STRING: \
			*(static_cast<std::string*>(this->val)) = convertToString(typein, &value); \
			break; \
		case CCX_PROPERTY_INTEGER: \
			*(static_cast<int*>(this->val)) = convertToInteger(typein, &value); \
			break; \
		case CCX_PROPERTY_DOUBLE: \
			*(static_cast<double*>(this->val)) = convertToDouble(typein, &value); \
			break; \
		case CCX_PROPERTY_POINTLIST: \
			*(static_cast<ccxPointList*>(this->val)) = convertToPointList(typein, &value); \
			break; \
		default:; \
	}


static bool convertToBool(ccxPropertyType type, void *val) {
	switch ( type ) {
		case CCX_PROPERTY_BOOL: {
			CASTEDGET(bool);
			return value;
		}

		case CCX_PROPERTY_STRING: {
			CASTEDGET(std::string);
			if ( value == "True" || value == "true" || value == "TRUE" || value == "1" )
				return true;
			return false;
		}

		case CCX_PROPERTY_INTEGER: {
			CASTEDGET(int);
			return value == 0 ? false : true;
		}

		case CCX_PROPERTY_DOUBLE: {
			CASTEDGET(double);
			return value == 0 ? false : true;
		}

		case CCX_PROPERTY_POINTLIST: {
			CASTEDGET(ccxPointList);
			return value.size() > 0;
		}

		default:
		assert(0);
	}

	assert(0);
	return false;
}

static std::string convertToString(ccxPropertyType type, void *val) {
	switch ( type ) {
		case CCX_PROPERTY_STRING: {
			CASTEDGET(std::string);
			return value;
		}

		case CCX_PROPERTY_BOOL: {
			CASTEDGET(bool);
			return value ? "true" : "false";
		}

		case CCX_PROPERTY_INTEGER: {
			char buffer[64];
			CASTEDGET(int);
			snprintf(buffer, sizeof(buffer), "%d", value);
			return buffer;
		}

		case CCX_PROPERTY_DOUBLE: {
			char buffer[64];
			CASTEDGET(double);
			snprintf(buffer, sizeof(buffer), "%f", value);
			return buffer;
		}

		case CCX_PROPERTY_POINTLIST: {
			std::ostringstream oss;
			ccxPointList::iterator it;
			CASTEDGET(ccxPointList);
			for ( it = value.begin(); it != value.end(); it++ )
				oss << (*it).x << "," << (*it).y << ";";
			return oss.str();
		}

		default:;
		assert(0);
	}

	assert(0);
	return "";
}

static double convertToDouble(ccxPropertyType type, void *val) {
	switch ( type ) {
		case CCX_PROPERTY_STRING: {
			CASTEDGET(std::string);
			return atof(value.c_str());
		}

		case CCX_PROPERTY_BOOL: {
			CASTEDGET(bool);
			return value ? 1.0 : 0.0;
		}

		case CCX_PROPERTY_INTEGER: {
			CASTEDGET(int);
			return (double)value;
		}

		case CCX_PROPERTY_DOUBLE: {
			CASTEDGET(double);
			return value;
		}

		default:;
		assert(0);
	}

	assert(0);
	return 0.0;
}

static int convertToInteger(ccxPropertyType type, void *val) {
	switch ( type ) {
		case CCX_PROPERTY_STRING: {
			CASTEDGET(std::string);
			return atoi(value.c_str());
		}

		case CCX_PROPERTY_BOOL: {
			CASTEDGET(bool);
			return value ? 1 : 0;
		}

		case CCX_PROPERTY_INTEGER: {
			CASTEDGET(int);
			return value;
		}

		case CCX_PROPERTY_DOUBLE: {
			CASTEDGET(double);
			return (int)value;
		}

		case CCX_PROPERTY_POINTLIST: {
			CASTEDGET(ccxPointList);
			return value.size();
		}

		default:;
		assert(0);
	}

	assert(0);
	return 0;
}

static ccxPointList convertToPointList(ccxPropertyType type, void *val) {
	ccxPointList output = ccxPointList();
	switch ( type ) {
		case CCX_PROPERTY_STRING: {
			CASTEDGET(std::string);
			std::vector<std::string> points = ccxUtils::tokenize(value, ";");
			std::vector<std::string>::iterator it;
			for ( it = points.begin(); it != points.end(); it++ ) {
				std::vector<std::string> point = ccxUtils::tokenize((*it), ",");
				ccxPoint p;

				// it's an error, not 2 points. just forget it.
				if ( point.size() != 2 )
					continue;

				// push the point into the list
				p.x = atof(point[0].c_str());
				p.y = atof(point[1].c_str());
				output.push_back(p);
			}

			return output;
		}

		case CCX_PROPERTY_POINTLIST: {
			CASTEDGET(ccxPointList);
			return value;
		}

		// we can't do anything for other type.
		default:
			return output;
	}

	return output;
}



ccxProperty::ccxProperty(bool value, const std::string &description) {
	this->init(description);
	this->type = CCX_PROPERTY_BOOL;
	this->val = new bool();
	this->set(value);
}

ccxProperty::ccxProperty(const char *value, const std::string &description) {
	this->init(description);
	this->type = CCX_PROPERTY_STRING;
	this->val = new std::string();
	this->set(value);
}

ccxProperty::ccxProperty(std::string value, const std::string &description) {
	this->init(description);
	this->type = CCX_PROPERTY_STRING;
	this->val = new std::string();
	this->set(value);
}

ccxProperty::ccxProperty(int value, const std::string &description) {
	this->init(description);
	this->type = CCX_PROPERTY_INTEGER;
	this->val = new int();
	this->set(value);
}

ccxProperty::ccxProperty(double value, const std::string &description) {
	this->init(description);
	this->type = CCX_PROPERTY_DOUBLE;
	this->val = new double();
	this->set(value);
}

ccxProperty::ccxProperty(ccxPointList value, const std::string &description) {
	this->init(description);
	this->type = CCX_PROPERTY_POINTLIST;
	this->val = new ccxPointList();
	this->set(value);
}

void ccxProperty::init(const std::string &description) {
	this->is_text = false;
	this->readonly = false;
	this->val = NULL;
	this->val_min = 0;
	this->val_max = 0;
	this->val_choices = "";
	this->have_min = false;
	this->have_max = false;
	this->have_choices = false;
	this->callbacks = std::map<ccxPropertyCallback,void*>();
	this->setDescription(description);
}

void ccxProperty::set(bool value) {
	if ( this->isReadOnly() )
		return;
	AUTOCONVERT(CCX_PROPERTY_BOOL, value);
	this->fireCallback();
}

void ccxProperty::set(std::string value) {
	if ( this->isReadOnly() )
		return;
	AUTOCONVERT(CCX_PROPERTY_STRING, value);
	this->fireCallback();
}

void ccxProperty::set(const char *value) {
	if ( this->isReadOnly() )
		return;
	std::string s = std::string(value);
	AUTOCONVERT(CCX_PROPERTY_STRING, s);
	this->fireCallback();
}

void ccxProperty::set(int value) {
	if ( this->isReadOnly() )
		return;
	AUTOCONVERT(CCX_PROPERTY_INTEGER, value);
	this->fireCallback();
}

void ccxProperty::set(double value) {
	if ( this->isReadOnly() )
		return;
	AUTOCONVERT(CCX_PROPERTY_DOUBLE, value);
	this->fireCallback();
}

void ccxProperty::set(ccxPointList value) {
	if ( this->isReadOnly() )
		return;
	AUTOCONVERT(CCX_PROPERTY_POINTLIST, value);
	this->fireCallback();
}

ccxProperty::~ccxProperty() {
	this->free();
}

ccxPropertyType ccxProperty::getType() {
	return this->type;
}

bool ccxProperty::asBool() {
	return convertToBool(this->type, this->val);
}

std::string ccxProperty::asString() {
	return convertToString(this->type, this->val);
}

double ccxProperty::asDouble() {
	return convertToDouble(this->type, this->val);
}

int ccxProperty::asInteger() {
	return convertToInteger(this->type, this->val);
}

ccxPointList ccxProperty::asPointList() {
	return convertToPointList(this->type, this->val);
}

void ccxProperty::free() {
	if ( this->val == NULL )
		return;

	switch ( this->type ) {
		case CCX_PROPERTY_STRING:
			delete (std::string *)(this->val);
			break;
		case CCX_PROPERTY_BOOL:
			delete static_cast<bool *>(this->val);
			break;
		case CCX_PROPERTY_INTEGER:
			delete static_cast<int *>(this->val);
			break;
		case CCX_PROPERTY_DOUBLE:
			delete static_cast<double *>(this->val);
			break;
		default:;
	}

	this->val = NULL;
}

std::string ccxProperty::getPropertyTypeName(ccxPropertyType type) {
	switch ( type ) {
		case CCX_PROPERTY_DOUBLE: return "double";
		case CCX_PROPERTY_INTEGER: return "integer";
		case CCX_PROPERTY_STRING: return "string";
		case CCX_PROPERTY_BOOL: return "bool";
		case CCX_PROPERTY_POINTLIST: return "pointlist";
		default:;
	}

	return "unknown";
}

std::ostream& operator<< (std::ostream& o, const ccxProperty& p) {

	// Bad bad ... :'(
	ccxProperty *f = (ccxProperty *)&p;

	switch ( f->getType() ) {
		case CCX_PROPERTY_STRING:	return o << f->asString();
		case CCX_PROPERTY_BOOL:		return o << f->asBool();
		case CCX_PROPERTY_INTEGER:	return o << f->asInteger();
		case CCX_PROPERTY_DOUBLE:	return o << f->asDouble();
		case CCX_PROPERTY_POINTLIST: return o << f->asPointList();
		default:;
	}

	return o;
}

std::string ccxProperty::getDescription() {
	return this->description;
}

void ccxProperty::setDescription(const std::string& description) {
	this->description = description;
}

bool ccxProperty::isReadOnly() {
	return this->readonly;
}

void ccxProperty::setReadOnly(bool ro) {
	this->readonly = ro;
}

bool ccxProperty::haveMin() {
	return this->have_min;
}

bool ccxProperty::haveMax() {
	return this->have_max;
}

bool ccxProperty::haveChoices() {
	return this->have_choices;
}

int ccxProperty::getMin() {
	return this->val_min;
}

int ccxProperty::getMax() {
	return this->val_max;
}

std::string ccxProperty::getChoices() {
	return this->val_choices;
}

void ccxProperty::setMin(int val) {
	this->val_min = val;
	this->have_min = true;
}

void ccxProperty::setMax(int val) {
	this->val_max = val;
	this->have_max = true;
}

void ccxProperty::setChoices(const std::string &val) {
	this->val_choices = val;
	this->have_choices = true;
}

void ccxProperty::addCallback(ccxPropertyCallback callback, void *userdata) {
	std::map<ccxPropertyCallback, void*>::iterator it;
	// ensure no callback already exist for this property
	assert( this->callbacks.find(callback) == this->callbacks.end() );
	this->callbacks[callback] = userdata;
}

void ccxProperty::removeCallback(ccxPropertyCallback callback) {
	std::map<ccxPropertyCallback, void*>::iterator it;
	for ( it = this->callbacks.begin(); it != this->callbacks.end(); it++ ) {
		if ( it->first == callback ) {
			this->callbacks.erase(it);
			return;
		}
	}
}

void ccxProperty::fireCallback() {
	std::map<ccxPropertyCallback, void*>::iterator it;
	for ( it = this->callbacks.begin(); it != this->callbacks.end(); it++ )
		it->first(this, it->second);
}

void ccxProperty::setText(bool is_text) {
	this->is_text = is_text;
}

bool ccxProperty::isText() {
	return this->is_text;
}

