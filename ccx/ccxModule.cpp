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


#include <stdlib.h>
#include <assert.h>
#include <sstream>
#include <iostream>
#include <errno.h>
#include <string.h>

#include "pasync.h"

#include "ccxModule.h"
#include "ccxPipeline.h"
#include "ccxDataStream.h"
#include "ccxLog.h"
#include "ccxThread.h"
#include "ccxUtils.h"

LOG_DECLARE("Module");

static unsigned int idcount = 0;

// callback called when the property gui_feedback is changed.
static void module_gui_feedback_cb(ccxProperty *property, void *userdata) {
	assert( property != NULL );
	assert( userdata != NULL );
	ccxModule *module = NULL;
	std::vector<std::string> tokens;

	// split the value in 3 (type, x, y)
	std::string s = property->asString();
	if ( s == "" )
		return;

	tokens = ccxUtils::tokenize(s, ";");
	if ( tokens.size() != 3 )
		return;

	module = static_cast<ccxModule *>(userdata);
	module->guiFeedback(
		tokens[0],
		atof(tokens[1].c_str()),
		atof(tokens[2].c_str())
	);
}

static void stats_init(ccx_module_stats_t *s) {
	s->_last_time = ccxUtils::time();
}

static void stats_wait(ccx_module_stats_t *s) {
	double curtime = ccxUtils::time();
	s->_wait_time += curtime - s->_last_time;
	s->_last_time = curtime;
}

static void stats_process(ccx_module_stats_t *s) {
	double curtime = ccxUtils::time();
	s->_process_time += curtime - s->_last_time;
	s->_last_time = curtime;
	s->_process_frame ++;

	// calculate average fps every 1s
	if ( (s->_process_time + s->_wait_time) > 1. )
	{
		// average is on the last period
		s->average_process_time	= s->_process_time / (double)s->_process_frame;
		s->average_wait_time	= s->_wait_time / (double)s->_process_frame;

		// calculate fps
		s->average_fps			= (double)s->_process_frame / (
				s->_process_time + s->_wait_time);

		// update totals
		s->total_wait_time		+= s->_wait_time;
		s->total_process_time	+= s->_process_time;
		s->total_process_frame	+= s->_process_frame;

		// reset period
		s->_process_frame		= 0;
		s->_process_time		= 0;
		s->_wait_time			= 0;
	}
}

ccxModule::ccxModule(unsigned int capabilities) {
	this->capabilities	= capabilities;
	this->is_started	= false;
	this->is_internal	= false;
	this->owner			= NULL;
	this->is_error		= false;
	this->error_msg		= "";
	this->thread		= NULL;
	this->use_thread	= false;
	this->need_update	= false;
	this->thread_trigger = NULL;
	this->need_gui_build = false;
	this->mtx			= new pt::mutex();

	memset(&this->stats,0,sizeof(ccx_module_stats_t));

	this->properties["use_thread"] = new ccxProperty(false);

	// create the default properties used for gui
	// the gui_feedback will be formatted as [down|move|up];x;y
	this->properties["gui_feedback"] = new ccxProperty("");
	this->properties["gui_feedback"]->addCallback(module_gui_feedback_cb, this);
}

ccxModule::~ccxModule() {
	this->stop();

	if ( this->output_infos.size() > 0 ) {
		std::map<int, ccxDataStreamInfo*>::iterator it;
		for ( it = this->output_infos.begin(); it != this->output_infos.end(); it++ )
			delete it->second;
	}

	if ( this->input_infos.size() > 0 ) {
		std::map<int, ccxDataStreamInfo*>::iterator it;
		for ( it = this->input_infos.begin(); it != this->input_infos.end(); it++ )
			delete it->second;
	}

	if ( this->properties.size() > 0 ) {
		std::map<std::string, ccxProperty*>::iterator it;
		for ( it = this->properties.begin(); it != this->properties.end(); it++ ) {
			delete (*it).second;
			(*it).second = NULL;
		}
	}

	if ( this->thread_trigger != NULL )
		delete this->thread_trigger;
	delete this->mtx;
}

std::string ccxModule::createId(std::string base) {
	std::ostringstream oss;
	oss << base << (idcount++);
	return oss.str();
}

unsigned int ccxModule::getCapabilities() {
	return this->capabilities;
}

int ccxModule::getInputCount() {
	return this->input_infos.size();
}

int ccxModule::getOutputCount() {
	return this->output_infos.size();
}

int ccxModule::getInputIndex(ccxDataStream *ds) {
	for(int i=0; i < this->getInputCount(); i++){
		if (ds == this->getInput(i))
			return i;
	}
	return -1;
}

int ccxModule::getOutputIndex(ccxDataStream *ds){
	for(int i=0; i < this->getOutputCount(); i++){
		if (ds == this->getOutput(i))
			return i;
	}
	return -1;
}

void ccxModule::notifyData(ccxDataStream *source) {
}

void _thread_process(ccxThread *thread) {
	ccxModule *module = (ccxModule *)thread->getUserData();
	stats_init(&module->stats);
	while ( !thread->wantQuit() ) {
		if ( !module->needUpdate(true) )
			continue;

		stats_wait(&module->stats);
		module->update();
		stats_process(&module->stats);
	}
}

void ccxModule::start() {
	this->use_thread = this->property("use_thread").asBool();
	stats_init(&this->stats);
	if ( this->use_thread ) {
		if ( this->thread_trigger == NULL ) {
			LOGM(CCX_TRACE, "create trigger");
			this->thread_trigger = new pt::trigger(true, false);
		}

		LOGM(CCX_TRACE, "start thread");
		this->thread = new ccxThread(_thread_process, this);
		if ( this->thread == NULL ) {
			LOGM(CCX_ERROR, "unable to create thread");
			this->setError("Error while creating thread");
			this->use_thread = false;
		} else {
			this->thread->start();
		}
	}

	this->is_started = true;
	LOGM(CCX_DEBUG, "start");
}

void ccxModule::stop() {
	if ( this->use_thread &&  this->thread != NULL ) {
		this->thread->stop();
		this->thread_trigger->post();
		this->thread->waitfor();
		delete this->thread;
		this->thread = NULL;
		this->use_thread = false;
	}

	this->need_update = false;
	this->is_started = false;
	LOG(CCX_DEBUG, "stop <" << this->property("id").asString() << ">");
}

void ccxModule::lock() {
	this->mtx->lock();
}

void ccxModule::unlock() {
	this->mtx->unlock();
}

bool ccxModule::isStarted() {
	return this->is_started;
}

bool ccxModule::isInternal() {
	return this->is_internal;
}

ccxDataStreamInfo *ccxModule::getInputInfos(int n) {
	std::map<int, ccxDataStreamInfo*>::iterator it;
	it = this->input_infos.find(n);
	if ( it == this->input_infos.end() )
		return NULL;
	return it->second;
}

ccxDataStreamInfo *ccxModule::getOutputInfos(int n) {
	std::map<int, ccxDataStreamInfo*>::iterator it;
	it = this->output_infos.find(n);
	if ( it == this->output_infos.end() )
		return NULL;
	return it->second;
}

ccxProperty &ccxModule::property(std::string str) {
	std::map<std::string, ccxProperty*>::iterator it;
	it = this->properties.find(str);
	if ( it == this->properties.end() ) {
		this->properties[str] = new ccxProperty("", "?? auto created ??");
		return *(this->properties[str]);
	}
	return *it->second;
}

std::map<std::string, ccxProperty*> &ccxModule::getProperties() {
	return this->properties;
}

void ccxModule::describe() {
	std::cout << "Module: " << this->getName() << std::endl;
	std::cout << "Author: " << this->getAuthor() << std::endl;
	std::cout << "Description: " << this->getDescription() << std::endl;

	std::cout << "Capabilities: ";
	if ( this->getCapabilities() & CCX_MODULE_INPUT )
		std::cout << "input,";
	if ( this->getCapabilities() & CCX_MODULE_OUTPUT )
		std::cout << "output,";
	if ( this->getCapabilities() & CCX_MODULE_GUI )
		std::cout << "gui,";
	std::cout << std::endl;

	if ( this->properties.size() > 0 ) {
		std::cout << std::endl;
		std::cout << "Properties: " << std::endl;

		std::map<std::string, ccxProperty*>::iterator it;
		for ( it = this->properties.begin(); it != this->properties.end(); it++ ) {
			std::cout << " " << (*it).first << ": " \
				<< "type=" << ccxProperty::getPropertyTypeName((*it).second->getType()) << ", "\
				<< "default=" << (*it).second->asString() \
				<< std::endl;
		}
	}

	if ( this->getCapabilities() & CCX_MODULE_INPUT ) {
		std::cout << std::endl;
		std::cout << "Input :" << std::endl;
		for ( int i = 0; i < this->getInputCount(); i++ ) {
			std::cout << " " << i << ": name=" \
				<< this->getInputInfos(i)->getName() << ", type=" \
				<< this->getInputInfos(i)->getType() << ", desc=" \
				<< this->getInputInfos(i)->getDescription() << std::endl;
		}
	}

	if ( this->getCapabilities() & CCX_MODULE_OUTPUT ) {
		std::cout << std::endl;
		std::cout << "Output :" << std::endl;
		for ( int i = 0; i < this->getOutputCount(); i++ ) {
			std::cout << " " << i << ": name=" \
				<< this->getOutputInfos(i)->getName() << ", type=" \
				<< this->getOutputInfos(i)->getType() << ", desc=" \
				<< this->getOutputInfos(i)->getDescription() << std::endl;
		}
	}

	std::cout << std::endl;
}

bool ccxModule::isPipeline() {
	return false;
}

bool ccxModule::haveError() {
	return this->is_error;
}

void ccxModule::setError(const std::string& msg) {
	this->error_msg = msg;
	this->is_error = true;
}

std::string ccxModule::getLastError() {
	std::ostringstream oss;
	this->is_error = false;
	oss << "<" << this->property("id").asString() << "> " << this->error_msg;
	return oss.str();
}

void ccxModule::poll() {
	if ( this->use_thread )
		return;
	if ( this->needUpdate() ) {
		stats_wait(&this->stats);
		this->update();
		stats_process(&this->stats);
	}
}

void ccxModule::notifyUpdate() {
	// If a module was added AFTER the pipeline was started,
	// start it manually.
	if (!this->is_started)
		// XXX Is this the right place?
		this->start();
	this->need_update = true;
	if ( this->use_thread )
		this->thread_trigger->post();
}

void ccxModule::trigger() {
    this->notifyUpdate();
}

bool ccxModule::needUpdate(bool lock) {
	if ( this->need_update ) {
		this->need_update = false;
		return true;
	} else if ( lock == false )
		return false;

	// call from a thread
	if ( lock ) {
		assert(this->thread != NULL);
		this->thread_trigger->wait();
	}

	if ( this->need_update ) {
		this->need_update = false;
		return true;
	}

	return false;
}

bool ccxModule::serializeCreation(std::ostringstream &oss, bool do_data) {
	std::string id = this->property("id").asString();
	std::map<std::string, ccxProperty*>::iterator it;

	if ( do_data == false ) {

		oss << "pipeline create " << this->getName() << " " << id << std::endl;

		if ( this->properties.size() > 0 ) {
			for ( it = this->properties.begin(); it != this->properties.end(); it++ ) {
				if ( ((*it).second)->isText() ) {
					oss << "pipeline settext " << id << " "
						<< (*it).first << " " << id << "_"
						<< (*it).first << std::endl;
				} else {
					oss << "pipeline set " << id << " "
						<< (*it).first << " "
						<< ((*it).second)->asString() << std::endl;
				}
			}
		}

		oss << "" << std::endl;

	} else {

		if ( this->properties.size() <= 0 )
			return true;

		bool have_text = false;
		for ( it = this->properties.begin(); it != this->properties.end(); it++ ) {
			if ( !((*it).second)->isText() )
				continue;
			have_text = true;
			oss << PIPELINE_BOUNDARY << " " << id << "_"
				<< (*it).first << std::endl
				<< ((*it).second)->asString() << std::endl;
		}
		if ( have_text )
			oss << PIPELINE_BOUNDARY << std::endl;
	}

	return true;
}


bool ccxModule::serializeConnections(std::ostringstream &oss) {
	std::string id = this->property("id").asString();

	// for every Output Connection that we have
	for ( int i = 0; i < this->getOutputCount(); i++ ) {
		ccxDataStream* ds = this->getOutput(i);
		if ( ds == NULL )
			continue;

		for ( unsigned int j=0; j < ds->getObserverCount(); j++ ) {
			ccxModule* observer = ds->getObserver(j);
			if ( observer->isInternal() )
				continue;
			oss << "pipeline connect " << id << " " << i  << " "
				<< observer->property("id").asString() << " "
				<< observer->getInputIndex(ds)<< " " << std::endl;
		}
	}

	return true;
}

//
// Feedback part between GUI and Module
//
// Gui is transmit is information with mouse (down/move/up + position)
// with the property gui_feedback.
//
// Module with CCX_MODULE_GUI capability are able to send instruction on the GUI
// Theses instructions are taken with the new /pipeline/gui command
//
// Here is a list of possible instructions:
//   viewport w h;
//   color r g b;
//   text x y label;
//   style <filled|stroke>;
//   rect x y w h;
//   circle x y r;
//   line x1 y1 x2 y2;
//
//
// Theses instructions are very basic, but can be understood by any UI.
// Do not add your instruction without having the approbation of the core team.
//

void ccxModule::guiFeedback(const std::string& type, double x, double y) {
}

void ccxModule::guiBuild() {
}

void ccxModule::notifyGui() {
	this->need_gui_build = true;
}

std::vector<std::string> &ccxModule::getGui(void) {
	assert(this->getCapabilities() & CCX_MODULE_GUI);
	if ( this->need_gui_build ) {
		this->need_gui_build = false;
		this->guiBuild();
	}
	return this->gui;
}

void ccxModule::declareInput(int n, ccxDataStream **storage, ccxDataStreamInfo *info) {
	// FIXME ensure we are not leaking anything before reassign
	assert(info != NULL);
	assert(storage != NULL);
	this->input_map[n] = storage;
	this->input_infos[n] = info;
}

void ccxModule::declareOutput(int n, ccxDataStream **storage, ccxDataStreamInfo *info) {
	// FIXME ensure we are not leaking anything before reassign
	assert(info != NULL);
	assert(storage != NULL);
	this->output_map[n] = storage;
	this->output_infos[n] = info;
}

ccxDataStream *ccxModule::getInput(int n) {
	std::map<int, ccxDataStream**>::iterator it;
	it = this->input_map.find(n);
	if ( it == this->input_map.end() )
		return NULL;
	return *it->second;
}

ccxDataStream *ccxModule::getOutput(int n) {
	std::map<int, ccxDataStream**>::iterator it;
	it = this->output_map.find(n);
	if ( it == this->output_map.end() )
		return NULL;
	return *it->second;
}

void ccxModule::setOutputType(int n, const std::string& type) {
	ccxDataStream *stream = this->getOutput(n);
	ccxDataStreamInfo *info = this->getOutputInfos(n);
	if ( stream )
		stream->setFormat(type);
	if ( info )
		info->setType(type);
}

void ccxModule::setInputType(int n, const std::string& type) {
	ccxDataStream *stream = this->getInput(n);
	ccxDataStreamInfo *info = this->getInputInfos(n);
	if ( stream )
		stream->setFormat(type);
	if ( info )
		info->setType(type);
}

void ccxModule::setInput(ccxDataStream *stream, int n) {
	std::ostringstream oss;
	std::map<int, ccxDataStream**>::iterator it;
	ccxDataStream **input = NULL;
	ccxDataStreamInfo *info = NULL;

	// search if the index is mapped to an input storage
	it = this->input_map.find(n);
	if ( it == this->input_map.end() ) {
		this->setError("Invalid input index");
		return;
	}

	// deference this input storage
	input = it->second;
	if ( input == NULL ) {
		this->setError("Invalid input mapping");
		return;
	}

	// get stream info for this storage
	info = this->input_infos[n];

	if ( *input != NULL )
		(*input)->removeObserver(this);
	*input = stream;
	if ( stream != NULL ) {
		if ( !info->isStreamValid(stream) ) {
			oss << "Input " << n << " accepts <" << info->getType() << \
				"> but got <" << stream->getFormat() << ">";
			LOG(CCX_ERROR, oss.str());
			this->setError(oss.str());
			*input = NULL;
			return;
		}
	}
	if ( *input != NULL )
		(*input)->addObserver(this);
}
