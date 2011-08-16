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

#include "ccxDataStream.h"
#include "ccxModule.h"
#include "ccxUtils.h"

bool ccxDataStreamInfo::isStreamValid(ccxDataStream *stream) {
	if ( stream == NULL )
		return false;
	if ( this->type == "*" )
		return true;
	return ccxUtils::inList(stream->getFormat(), this->type, ",");
}

ccxDataStream::ccxDataStream(std::string format) {
	this->format = format;
	this->data	 = NULL;
	this->mtx	 = new pt::mutex();
}

ccxDataStream::~ccxDataStream() {
	delete this->mtx;
}

std::string ccxDataStream::getFormat() {
	return this->format;
}

void ccxDataStream::setFormat(const std::string &format) {
	this->format = format;
}

void ccxDataStream::lock() {
	this->mtx->lock();
}

void ccxDataStream::unlock() {
	this->mtx->unlock();
}

void ccxDataStream::push(void *data) {
	this->lock();
	this->data = data;
	this->unlock();

	this->notifyObservers();
}

void ccxDataStream::clear() {
    this->lock();
    this->data = NULL;
    this->unlock();
}

void ccxDataStream::addObserver(ccxModule *module) {
	this->observers.push_back(module);
}

void ccxDataStream::removeObserver(ccxModule *module) {
	std::vector<ccxModule *>::iterator it;
	// FIXME lock needed ?
	for ( it = this->observers.begin(); it != this->observers.end(); it++ ) {
		if ( *it != module )
			continue;
		this->observers.erase(it);
		return;
	}
}

void ccxDataStream::notifyObservers() {
	std::vector<ccxModule *>::iterator it;
	for ( it = this->observers.begin(); it != this->observers.end(); it++ ) {
		(*it)->lock();
		(*it)->notifyData(this);
		(*it)->unlock();
	}
}

void *ccxDataStream::getData() {
	return this->data;
}

unsigned int ccxDataStream::getObserverCount() {
	return this->observers.size();
}

ccxModule *ccxDataStream::getObserver(unsigned int index) {
	assert( index >= 0 && index < this->observers.size() );
	return this->observers[index];
}

void ccxDataStream::removeObservers() {
	std::vector<ccxModule *>::iterator it;

restartremove:;
	for ( it = this->observers.begin(); it != this->observers.end(); it++ ) {
		for ( int i = 0; i < (*it)->getInputCount(); i++ ) {
			if ( (*it)->getInput(i) == this ) {
				(*it)->setInput(NULL, i);
				// the list have been altered, must restart from scratch
				goto restartremove;
			}
		}
	}

	this->observers.clear();
}
