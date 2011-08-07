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


#include <string>

#include "ccxLog.h"
#include "ccxFactory.h"

#define REGISTER_MODULE(name) \
	extern ccxModule* factory_create_ccx##name(); \
	LOG(CCX_DEBUG, "register <" #name "> module"); \
	ccxFactory::getInstance()->registerModule(#name, factory_create_ccx##name);

LOG_DECLARE("Factory");

static ccxFactory *instance = NULL;

void ccxFactory::init() {
	// Keep the alphabetical order when adding new modules!
	/*REGISTER_MODULE(Amplify);
	REGISTER_MODULE(BackgroundSubtract);
	REGISTER_MODULE(BinaryMask);
	REGISTER_MODULE(BlobFinder)
	REGISTER_MODULE(Calibration);
	REGISTER_MODULE(Camera);
	REGISTER_MODULE(Canny);
	REGISTER_MODULE(Combine);
	REGISTER_MODULE(Dilate);
	REGISTER_MODULE(DistanceTransform);
	REGISTER_MODULE(Dump);
	REGISTER_MODULE(Erode);
	REGISTER_MODULE(EqualizeHist);
	REGISTER_MODULE(FiducialFinder);
	REGISTER_MODULE(FingerTipFinder);
	REGISTER_MODULE(GrayScale);
	REGISTER_MODULE(GreedyBlobTracker);
	REGISTER_MODULE(Highpass);
	REGISTER_MODULE(Hsv);
	REGISTER_MODULE(HuObjectFinder);
	REGISTER_MODULE(Image);
	REGISTER_MODULE(ImageDisplay);
	REGISTER_MODULE(Invert);
	REGISTER_MODULE(Justify);
	REGISTER_MODULE(Mask);
	REGISTER_MODULE(MirrorImage);
	REGISTER_MODULE(MunkresBlobTracker);
	REGISTER_MODULE(PeakFinder);
	REGISTER_MODULE(Roi);
	REGISTER_MODULE(Smooth);
	REGISTER_MODULE(Threshold);
	REGISTER_MODULE(Tuio);
	REGISTER_MODULE(Tuio2);
	REGISTER_MODULE(Video);
	REGISTER_MODULE(YCrCbThreshold);*/
    REGISTER_MODULE(DebugInput);
    REGISTER_MODULE(DebugOutput);
    REGISTER_MODULE(SpiritParser);
    REGISTER_MODULE(SphinxASR);
    //DoNotRemoveThisComment
	LOG(CCX_INFO, "registered " << ccxFactory::getInstance()->list().size() << " modules");
}

void ccxFactory::cleanup() {
	delete instance;
}

ccxFactory::ccxFactory() {
}

ccxFactory::~ccxFactory() {
}

ccxFactory *ccxFactory::getInstance() {
	if ( instance == NULL )
		instance = new ccxFactory();
	return instance;
}

void ccxFactory::registerModule(const std::string &name, ccxFactoryCreateCallback callback) {
	this->database[name] = callback;
}

ccxModule *ccxFactory::create(const std::string &name) {
	std::string sname = std::string(name);

	// for easier creation, remove the ot at start
	if ( sname.size() > 2 && sname.substr(0, 2) == "ccx" )
		sname =	sname.substr(2, sname.length() - 2);
	if ( sname.size() > 6 && sname.substr(sname.length() - 6, 6) == "Module" )
		sname = sname.substr(0, sname.length() - 6);

	std::map<std::string, ccxFactoryCreateCallback>::iterator it;
	it = this->database.find(sname);
	if ( it == this->database.end() )
		return NULL;
	return (it->second)();
}

std::vector<std::string> ccxFactory::list() {
	std::vector<std::string> l;
	std::map<std::string, ccxFactoryCreateCallback>::iterator it;
	for ( it = this->database.begin(); it != this->database.end(); it++ )
		l.push_back(it->first);
	return l;
}

