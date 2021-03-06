/////////////////////////////////////////////////////////////////////////////
// Name:        models/ballWorldGrammar.h
// Purpose:     BallWorld grammar for the Spirit parser module
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////

/***********************************************************************
 ** Some parts Copyright (C) 2010 Movid Authors.  All rights reserved.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **********************************************************************/

#include <stdio.h>

#ifdef WIN32
#include <Ws2tcpip.h>
#include <Wspiapi.h>
#include <winsock2.h>
#include <windows.h>
#include <winbase.h>
#include <Xgetopt.h>
#else
#include <getopt.h>
#endif


#include <sys/types.h>
#include <sys/stat.h>
#include <sys/queue.h>
#include <signal.h>

#ifndef WIN32
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#endif

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <algorithm>

// libevent
#include "event.h"
#include "evhttp.h"

// JSON
#include "cJSON.h"

// CCX
#include "ccx.h"
#include "ccfMultimodalSyntaxTree.h"

// assert
#include <assert.h>

#define CCX_DAEMON	"ccf"
#define CCX_GUIDIR	"gui/html"
#define CCX_VERSION	"0.1b"
#define CCX_SLOTMAX	10
#define CCX_SLOTDIR	"configs/slots/slot-"
#define CCX_SLOTEXT	".txt"

#ifdef WIN32
#define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else
#define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

#ifndef UNICODE
typedef std::string String;
#else
typedef std::wstring String;
#endif

LOG_DECLARE("App");

static ccxPipeline *pipeline = NULL;
static bool want_quit = false;
static struct event_base *base = NULL;
static bool config_detach = false;
static bool config_syslog = false;
static bool config_httpserver = true;
static bool test_mode = false;
static std::string config_pipelinefn = "configs/presets/fusion.txt";
static std::string config_guidir = CCX_GUIDIR;
static std::string config_pidfile = "/var/run/fusion.pid";
static struct evhttp *server = NULL;
int g_config_delay = 20;

bool want_quit_soon = false;
int want_quit_soon_count = 0;

char *strsep(char **stringp, const char *delim) {
	char *s = *stringp;
	char *e;
	if (!s)
		return NULL;
	e = strpbrk(s, delim);
	if (e)
		*e++ = '\0';
	*stringp = e;
	return s;
}

/*class otStreamModule : public ccxModule {
public:
	otStreamModule() : ccxModule(CCX_MODULE_INPUT) {
		this->is_internal = true;
		this->input = new ccxDataStream("stream");
		this->output_buffer = NULL;
		this->properties["id"] = new ccxProperty(ccxModule::createId("WebStream"));
		this->properties["scale"] = new ccxProperty(1);
		this->declareInput(0, &this->input, new ccxDataStreamInfo(
			"stream", "IplImage,IplImage8", "Image to stream on webbrowser"));
		// FIXME: silence error message that's printed when a module in the web gui
		//		  is clicked that doesn't output IplImages
	}

	void stop() {
		if ( this->output_buffer != NULL ) {
			cvReleaseImage(&this->output_buffer);
			this->output_buffer = NULL;
		}
		ccxModule::stop();
	}

	void notifyData(ccxDataStream *source) {
		IplImage* src = (IplImage*)(this->input->getData());
		if ( src == NULL )
			return;
		if ( this->output_buffer == NULL ||
			 this->output_buffer->nChannels != src->nChannels ) {
			if ( this->output_buffer != NULL )
				cvReleaseImage(&this->output_buffer);
			CvSize size = cvGetSize(src);
			size.width /= this->property("scale").asInteger();
			size.height /= this->property("scale").asInteger();
			this->output_buffer = cvCreateImage(size, src->depth, src->nChannels);
		}
		this->notifyUpdate();
	}

	bool copy() {
		IplImage *src;
		if ( this->output_buffer == NULL || this->input == NULL )
			return false;
		this->input->lock();
		src = (IplImage*)(this->input->getData());
		if ( src == NULL || src->imageData == NULL ) {
			this->input->unlock();
			return false;
		}
		if ( this->output_buffer->nChannels != src->nChannels ) {
			this->input->unlock();
			return false;
		}
		if ( this->property("scale").asInteger() == 1 )
			cvCopy(src, this->output_buffer);
		else
			cvResize(src, this->output_buffer);
		this->input->unlock();
		return true;
	}

	virtual void update() {}
	virtual std::string getName() { return "Stream"; }
	virtual std::string getDescription() { return ""; }
	virtual std::string getAuthor() { return ""; }

	ccxDataStream *input;
	IplImage* output_buffer;
};*/

/*struct chunk_req_state {
	struct evhttp_request *req;
	otStreamModule *stream;
	int i;
	bool closed;
	int delay;
};*/


static void signal_term(int signal) {
	want_quit = true;
}

//
// WEB CALLBACKS
//

void web_json(struct evhttp_request *req, cJSON *root) {
	struct evbuffer *evb = evbuffer_new();
	char *out;

	out = cJSON_Print(root);
	//cJSON_Delete(root);

	evbuffer_add(evb, out, strlen(out));
	evhttp_add_header(req->output_headers, "Content-Type", "application/json");
	evhttp_send_reply(req, HTTP_OK, "Everything is fine", evb);
	evbuffer_free(evb);

	free(out);
}

void web_message(struct evhttp_request *req, const char *message, int success=1) {
	cJSON *root = cJSON_CreateObject();
	cJSON_AddNumberToObject(root, "success", success);
	cJSON_AddStringToObject(root, "message", message);
	web_json(req, root);
}

void web_error(struct evhttp_request *req, const char* message) {
	return web_message(req, message, 0);
}

void web_status(struct evhttp_request *req, void *arg) {
	web_message(req, "ok");
}

/*static void web_pipeline_stream_close(struct evhttp_connection *conn, void *arg) {
	struct chunk_req_state *state = static_cast<chunk_req_state*>(arg);
	state->closed = true;
}*/

/*static void web_pipeline_stream_trickle(int fd, short events, void *arg) {
	struct evbuffer *evb = NULL;
	struct chunk_req_state *state = static_cast<chunk_req_state*>(arg);
	struct timeval when = { 0, 0 };
	long unsigned int outlen;
	std::vector<uchar>outbuf;
	std::vector<int> params;
	IplImage* img;
	bool convert = false;

	when.tv_usec = state->delay * 1000;

	// stream is closed, clean the state.
	if ( state->closed ) {
		state->stream->setInput(NULL);
		delete state->stream;
		free(state);
		return;
	}

	// nothing to copy, schedule the next event
	if ( !state->stream->copy() )
		goto stream_trickle_end;

	// convert the image from BRG to RGB
	img = state->stream->output_buffer;
	if ( img == NULL ) {
		LOG(CCX_ERROR, "stream: output_buffer is null !");
		goto stream_trickle_end;
	}

	if ( img->nChannels == 3 )
		cvCvtColor(img, img, CV_BGR2RGB);

	// if the depth is not a 8, create a temporary image, and convert to 8 bytes depth
	if ( img->depth != 8 ) {
		convert = true;
		img = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U, img->nChannels);
		if ( img == NULL ) {
			LOG(CCX_ERROR, "stream: unable to create a new image");
			goto stream_trickle_end;
		}
		cvConvertScale(state->stream->output_buffer, img, 255, 0);
	}

	// convert the image to JPEG
	params.push_back(CV_IMWRITE_JPEG_QUALITY);
	params.push_back(100);
	cv::imencode(".jpg", img, outbuf, params);
	outlen = outbuf.size();

	// release temporary image if created
	if ( convert )
		cvReleaseImage(&img);

	// create and send the buffer
	evb = evbuffer_new();
	if ( evb == NULL ) {
		LOG(CCX_ERROR, "stream: unable to create a libevent buffer");
		goto stream_trickle_end;
	}
	evbuffer_add_printf(evb, "--mjpegstream\r\n");
	evbuffer_add_printf(evb, "Content-Type: image/jpeg\r\n");
	evbuffer_add_printf(evb, "Content-Length: %lu\r\n\r\n", outlen);
	evbuffer_add(evb, &outbuf[0], outlen);
	evhttp_send_reply_chunk(state->req, evb);
	evbuffer_free(evb);

	outbuf.clear();
	params.clear();

stream_trickle_end:;
	event_once(-1, EV_TIMEOUT, web_pipeline_stream_trickle, state, &when);
}*/

/*void web_pipeline_stream(struct evhttp_request *req, void *arg) {
	const char *uri;
	int	idx = 0;
	ccxModule *module = NULL;
	struct chunk_req_state *state = NULL;
	struct evkeyvalq headers;
	struct timeval when = { 0, 20 };

	uri = evhttp_request_uri(req);
	if ( uri == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "unable to retreive uri");
	}

	evhttp_parse_query(uri, &headers);

	if ( evhttp_find_header(&headers, "objectname") == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "missing objectname");
	}

	module = pipeline->getModuleById(evhttp_find_header(&headers, "objectname"));
	if ( module == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "object not found");
	}

	if ( evhttp_find_header(&headers, "index") != NULL )
		idx = atoi(evhttp_find_header(&headers, "index"));

	if ( idx < 0 || idx >= module->getOutputCount() ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "invalid index");
	}

	state = (struct chunk_req_state*)malloc(sizeof(struct chunk_req_state));
	if ( state == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "unable to allocate state");
	}

	memset(state, 0, sizeof(struct chunk_req_state));
	state->req		= req;
	state->closed	= false;
	state->delay	= 100;
	state->stream	= new otStreamModule();
	if ( state->stream == NULL ) {
		evhttp_clear_headers(&headers);
		free(state);
		return web_error(req, "unable to allocate stream module");
	}

	if ( evhttp_find_header(&headers, "scale") != NULL )
		state->stream->property("scale").set(evhttp_find_header(&headers, "scale"));

	if ( evhttp_find_header(&headers, "delay") != NULL )
		state->delay = atoi(evhttp_find_header(&headers, "delay"));

	state->stream->setInput(module->getOutput(idx));

	// prepare connection
	evhttp_add_header(req->output_headers, "Content-Type", "multipart/x-mixed-replace; boundary=mjpegstream");
	evhttp_send_reply_start(req, HTTP_OK, "Everything is fine");
	evhttp_connection_set_closecb(req->evcon, web_pipeline_stream_close, state);

	// reschedule a future event for next call
	event_once(-1, EV_TIMEOUT, web_pipeline_stream_trickle, state, &when);
}*/

void web_pipeline_create(struct evhttp_request *req, void *arg) {
	ccxModule *module;
	struct evkeyvalq headers;
	const char *uri;

	uri = evhttp_request_uri(req);
	if ( uri == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "unable to retreive uri");
	}

	evhttp_parse_query(uri, &headers);

	if ( evhttp_find_header(&headers, "objectname") == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "missing objectname");
	}

	module = ccxFactory::getInstance()->create(evhttp_find_header(&headers, "objectname"));
	if ( module == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "invalid objectname");
	}

	pipeline->addElement(module);

	evhttp_clear_headers(&headers);
	web_message(req, module->property("id").asString().c_str());
}

void web_pipeline_stats(struct evhttp_request *req, void *arg) {
	ccxModule *module;
	cJSON *root, *data, *mod;

	root = cJSON_CreateObject();
	cJSON_AddNumberToObject(root, "success", 1);
	cJSON_AddStringToObject(root, "message", "ok");
	cJSON_AddItemToObject(root, "stats", data=cJSON_CreateObject());

	for ( unsigned int i = 0; i < pipeline->size(); i++ ) {
		module = pipeline->getModule(i);

		cJSON_AddItemToObject(data,
			module->property("id").asString().c_str(),
			mod=cJSON_CreateObject());

		cJSON_AddNumberToObject(mod, "average_fps", module->stats.average_fps);
		cJSON_AddNumberToObject(mod, "average_process_time", module->stats.average_process_time);
		cJSON_AddNumberToObject(mod, "average_wait_time", module->stats.average_wait_time);
		cJSON_AddNumberToObject(mod, "total_process_frame", module->stats.total_process_frame);
		cJSON_AddNumberToObject(mod, "total_process_time", module->stats.total_process_time);
		cJSON_AddNumberToObject(mod, "total_wait_time", module->stats.total_wait_time);
	}

	web_json(req, root);
}

void web_pipeline_status(struct evhttp_request *req, void *arg) {
	std::map<std::string, ccxProperty*>::iterator it;
	unsigned int i, j;
	int k;
	char buffer[64];
	cJSON *root, *data, *modules, *mod, *properties, *io, *observers, *array, *property;
	ccxDataStream *ds;

	root = cJSON_CreateObject();
	cJSON_AddNumberToObject(root, "success", 1);
	cJSON_AddStringToObject(root, "message", "ok");
	cJSON_AddItemToObject(root, "status", data=cJSON_CreateObject());
	cJSON_AddStringToObject(data, "version", CCX_VERSION);
	cJSON_AddNumberToObject(data, "size", pipeline->size());
	cJSON_AddNumberToObject(data, "running", pipeline->isStarted() ? 1 : 0);
	cJSON_AddItemToObject(data, "modules", modules=cJSON_CreateObject());

	for ( i = 0; i < pipeline->size(); i++ ) {
		ccxModule *module = pipeline->getModule(i);
		assert( module != NULL );

		cJSON_AddItemToObject(modules,
			module->property("id").asString().c_str(),
			mod=cJSON_CreateObject());

		cJSON_AddStringToObject(mod, "name", module->getName().c_str());
		cJSON_AddStringToObject(mod, "description", module->getDescription().c_str());
		cJSON_AddStringToObject(mod, "author", module->getAuthor().c_str());
		cJSON_AddNumberToObject(mod, "running", module->isStarted() ? 1 : 0);
		cJSON_AddNumberToObject(mod, "gui", module->getCapabilities() & CCX_MODULE_GUI ? 1 : 0);
		cJSON_AddItemToObject(mod, "properties", properties=cJSON_CreateObject());

		for ( it = module->getProperties().begin(); it != module->getProperties().end(); it++ ) {
			cJSON_AddStringToObject(properties, it->first.c_str(),
					it->second->asString().c_str());
		}

		cJSON_AddItemToObject(mod, "propertiesInfos", properties=cJSON_CreateObject());

		for ( it = module->getProperties().begin(); it != module->getProperties().end(); it++ ) {
			ccxProperty *p = it->second;
			cJSON_AddItemToObject(properties, it->first.c_str(), property=cJSON_CreateObject());
			cJSON_AddStringToObject(property, "type", ccxProperty::getPropertyTypeName(p->getType()).c_str());
			cJSON_AddNumberToObject(property, "readonly", p->isReadOnly() ? 1 : 0);
			if ( p->haveMax() )
				cJSON_AddNumberToObject(property, "max", p->getMax());
			if ( p->haveMin() )
				cJSON_AddNumberToObject(property, "min", p->getMin());
			if ( p->haveChoices() )
				cJSON_AddStringToObject(property, "choices", p->getChoices().c_str());
		}

		if ( module->getInputCount() ) {
			cJSON_AddItemToObject(mod, "inputs", array=cJSON_CreateArray());
			for ( k = 0; k < module->getInputCount(); k++ ) {
				ds = module->getInput(k);
				cJSON_AddItemToArray(array, io=cJSON_CreateObject());
				cJSON_AddNumberToObject(io, "index", k);
				cJSON_AddStringToObject(io, "name", module->getInputInfos(k)->getName().c_str());
				cJSON_AddStringToObject(io, "type", module->getInputInfos(k)->getType().c_str());
				cJSON_AddNumberToObject(io, "used", ds == NULL ? 0 : 1);
			}
		}

		if ( module->getOutputCount() ) {
			cJSON_AddItemToObject(mod, "outputs", array=cJSON_CreateArray());
			for ( k = 0; k < module->getOutputCount(); k++ ) {
				ds = module->getOutput(k);
				cJSON_AddItemToArray(array, io=cJSON_CreateObject());
				cJSON_AddNumberToObject(io, "index", k);
				cJSON_AddStringToObject(io, "name", module->getOutputInfos(k)->getName().c_str());
				cJSON_AddStringToObject(io, "type", module->getOutputInfos(k)->getType().c_str());
				cJSON_AddNumberToObject(io, "used", ds == NULL ? 0 : 1);
				cJSON_AddItemToObject(io, "observers", observers=cJSON_CreateObject());
				if ( ds != NULL ) {
					for ( j = 0; j < ds->getObserverCount(); j++ ) {
						snprintf(buffer, sizeof(buffer), "%d", j);
						cJSON_AddStringToObject(observers, buffer,
							ds->getObserver(j)->property("id").asString().c_str());
					}
				}
			}
		}
	}

	web_json(req, root);
}

void web_factory_list(struct evhttp_request *req, void *arg) {
	std::vector<std::string>::iterator it;
	std::vector<std::string> list = ccxFactory::getInstance()->list();
	int i;
	cJSON *root, *data, *mod, *io, *array, *details;
	ccxModule *module;
	ccxDataStream *ds;

	root = cJSON_CreateObject();
	cJSON_AddNumberToObject(root, "success", 1);
	cJSON_AddStringToObject(root, "message", "ok");
	cJSON_AddItemToObject(root, "list", data=cJSON_CreateArray());

	for ( it = list.begin(); it != list.end(); it++ )
		cJSON_AddItemToArray(data, cJSON_CreateString(it->c_str()));

	cJSON_AddItemToObject(root, "details", details=cJSON_CreateObject());
	for ( it = list.begin(); it != list.end(); it++ ) {
		module = ccxFactory::getInstance()->create(*it);

		cJSON_AddItemToObject(details, module->getName().c_str(), mod=cJSON_CreateObject());
		cJSON_AddStringToObject(mod, "name", module->getName().c_str());
		cJSON_AddStringToObject(mod, "description", module->getDescription().c_str());
		cJSON_AddStringToObject(mod, "author", module->getAuthor().c_str());
		cJSON_AddNumberToObject(mod, "gui", (module->getCapabilities() & CCX_MODULE_GUI) ? 1 : 0);

		if ( module->getInputCount() ) {
			cJSON_AddItemToObject(mod, "inputs", array=cJSON_CreateArray());
			for ( i = 0; i < module->getInputCount(); i++ ) {
				ds = module->getInput(i);
				cJSON_AddItemToArray(array, io=cJSON_CreateObject());
				cJSON_AddNumberToObject(io, "index", i);
				cJSON_AddStringToObject(io, "name", module->getInputInfos(i)->getName().c_str());
				cJSON_AddStringToObject(io, "type", module->getInputInfos(i)->getType().c_str());
			}
		}

		if ( module->getOutputCount() ) {
			cJSON_AddItemToObject(mod, "outputs", array=cJSON_CreateArray());
			for ( i = 0; i < module->getOutputCount(); i++ ) {
				ds = module->getOutput(i);
				cJSON_AddItemToArray(array, io=cJSON_CreateObject());
				cJSON_AddNumberToObject(io, "index", i);
				cJSON_AddStringToObject(io, "name", module->getOutputInfos(i)->getName().c_str());
				cJSON_AddStringToObject(io, "type", module->getOutputInfos(i)->getType().c_str());
			}
		}

		delete module;
	}

	web_json(req, root);
}

void web_factory_describe(struct evhttp_request *req, void *arg) {
	std::map<std::string, ccxProperty*>::iterator it;
	cJSON *root, *mod, *properties, *io, *array;
	ccxDataStream *ds;
	ccxModule *module;
	struct evkeyvalq headers;
	const char *uri;
	int i;

	uri = evhttp_request_uri(req);
	if ( uri == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "unable to retreive uri");
	}

	evhttp_parse_query(uri, &headers);

	if ( evhttp_find_header(&headers, "name") == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "missing name");
	}

	module = ccxFactory::getInstance()->create(evhttp_find_header(&headers, "name"));
	if ( module == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "invalid name");
	}

	root = cJSON_CreateObject();
	cJSON_AddNumberToObject(root, "success", 1);
	cJSON_AddStringToObject(root, "message", "ok");
	cJSON_AddItemToObject(root, "describe", mod=cJSON_CreateObject());

	cJSON_AddStringToObject(mod, "name", module->getName().c_str());
	cJSON_AddStringToObject(mod, "description", module->getDescription().c_str());
	cJSON_AddStringToObject(mod, "author", module->getAuthor().c_str());
	cJSON_AddNumberToObject(mod, "running", module->isStarted() ? 1 : 0);
	cJSON_AddNumberToObject(mod, "gui", (module->getCapabilities() & CCX_MODULE_GUI) ? 1 : 0);
	cJSON_AddItemToObject(mod, "properties", properties=cJSON_CreateObject());

	for ( it = module->getProperties().begin(); it != module->getProperties().end(); it++ ) {
		cJSON_AddStringToObject(properties, it->first.c_str(),
				it->second->asString().c_str());
	}

	if ( module->getInputCount() ) {
		cJSON_AddItemToObject(mod, "inputs", array=cJSON_CreateArray());
		for ( i = 0; i < module->getInputCount(); i++ ) {
			ds = module->getInput(i);
			cJSON_AddItemToArray(array, io=cJSON_CreateObject());
			cJSON_AddNumberToObject(io, "index", i);
			cJSON_AddStringToObject(io, "name", module->getInputInfos(i)->getName().c_str());
			cJSON_AddStringToObject(io, "type", module->getInputInfos(i)->getType().c_str());
		}
	}

	if ( module->getOutputCount() ) {
		cJSON_AddItemToObject(mod, "outputs", array=cJSON_CreateArray());
		for ( i = 0; i < module->getOutputCount(); i++ ) {
			ds = module->getOutput(i);
			cJSON_AddItemToArray(array, io=cJSON_CreateObject());
			cJSON_AddNumberToObject(io, "index", i);
			cJSON_AddStringToObject(io, "name", module->getOutputInfos(i)->getName().c_str());
			cJSON_AddStringToObject(io, "type", module->getOutputInfos(i)->getType().c_str());
		}
	}

	delete module;

	evhttp_clear_headers(&headers);
	web_json(req, root);
}

void web_pipeline_connect(struct evhttp_request *req, void *arg) {
	ccxModule *in, *out;
	int inidx = 0, outidx = 0;
	struct evkeyvalq headers;
	const char *uri;

	uri = evhttp_request_uri(req);
	if ( uri == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "unable to retreive uri");
	}

	evhttp_parse_query(uri, &headers);

	if ( evhttp_find_header(&headers, "out") == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "missing out");
	}

	if ( evhttp_find_header(&headers, "in") == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "missing in");
	}

	if ( evhttp_find_header(&headers, "outidx") != NULL )
		outidx = atoi(evhttp_find_header(&headers, "outidx"));
	if ( evhttp_find_header(&headers, "inidx") != NULL )
		inidx = atoi(evhttp_find_header(&headers, "inidx"));

	in = pipeline->getModuleById(evhttp_find_header(&headers, "in"));
	out = pipeline->getModuleById(evhttp_find_header(&headers, "out"));

	if ( in == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "in object not found");
	}

	if ( out == NULL && strcmp(evhttp_find_header(&headers, "out"), "NULL") != 0 ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "out object not found");
	}

	if ( strcmp(evhttp_find_header(&headers, "out"), "NULL") == 0 )
		in->setInput(NULL, inidx);
	else
		in->setInput(out->getOutput(outidx), inidx);

	evhttp_clear_headers(&headers);
	web_message(req, "ok");
}

void web_pipeline_get(struct evhttp_request *req, void *arg) {
	ccxModule *module;
	struct evkeyvalq headers;
	const char *uri;

	uri = evhttp_request_uri(req);
	if ( uri == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "unable to retreive uri");
	}

	evhttp_parse_query(uri, &headers);

	if ( evhttp_find_header(&headers, "objectname") == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "missing objectname");
	}

	if ( evhttp_find_header(&headers, "name") == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "missing name");
	}

	module = pipeline->getModuleById(evhttp_find_header(&headers, "objectname"));
	if ( module == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "object not found");
	}

	web_message(req, module->property(evhttp_find_header(&headers, "name")).asString().c_str());
	evhttp_clear_headers(&headers);
}

void web_pipeline_set(struct evhttp_request *req, void *arg) {
	ccxModule *module;
	struct evkeyvalq headers;
	const char *uri;

	uri = evhttp_request_uri(req);
	if ( uri == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "unable to retreive uri");
	}

	evhttp_parse_query(uri, &headers);

	if ( evhttp_find_header(&headers, "objectname") == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "missing objectname");
	}

	module = pipeline->getModuleById(evhttp_find_header(&headers, "objectname"));
	if ( module == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "object not found");
	}

	while ( true ) {

		if ( evhttp_find_header(&headers, "name") == NULL )
			break;

		if ( evhttp_find_header(&headers, "value") == NULL )
			break;

		module->property(evhttp_find_header(&headers, "name")).set(
			(const std::string &)evhttp_find_header(&headers, "value"));

		evhttp_remove_header(&headers, "name");
		evhttp_remove_header(&headers, "value");
	}

	evhttp_clear_headers(&headers);
	web_message(req, "ok");
}

void web_pipeline_remove(struct evhttp_request *req, void *arg) {
	ccxModule *module;
	ccxDataStream *ds;
	struct evkeyvalq headers;
	const char *uri;

	uri = evhttp_request_uri(req);
	if ( uri == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "unable to retreive uri");
	}

	evhttp_parse_query(uri, &headers);

	if ( evhttp_find_header(&headers, "objectname") == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "missing objectname");
	}

	module = pipeline->getModuleById(evhttp_find_header(&headers, "objectname"));
	if ( module == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "object not found");
	}

	pipeline->stop();
	module->stop();

	// disconnect inputs
	if ( module->getInputCount() ) {
		for ( int i = 0; i < module->getInputCount(); i++ ) {
			ds = module->getInput(i);
			if ( ds == NULL )
				continue;
			ds->removeObserver(module);
		}
	}

	// disconnect output
	if ( module->getOutputCount() ) {
		for ( int i = 0; i < module->getOutputCount(); i++ ) {
			ds = module->getOutput(i);
			if ( ds == NULL )
				continue;
			ds->removeObservers();
		}
	}

	// remove element from pipeline
	pipeline->removeElement(module);

	delete module;

	web_message(req, "ok");
	evhttp_clear_headers(&headers);
}

void web_pipeline_gui(struct evhttp_request *req, void *arg) {
	ccxModule *module;
	struct evkeyvalq headers;
	const char *uri;

	uri = evhttp_request_uri(req);
	evhttp_parse_query(uri, &headers);

	if ( evhttp_find_header(&headers, "objectname") == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "missing objectname");
	}

	module = pipeline->getModuleById(evhttp_find_header(&headers, "objectname"));
	if ( module == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "object not found");
	}

	evhttp_clear_headers(&headers);

	struct evbuffer *evb = evbuffer_new();
	std::vector<std::string> gui = module->getGui();
	std::vector<std::string>::iterator it;
	for ( it = gui.begin(); it != gui.end(); it++ )
		evbuffer_add_printf(evb, "%s\n", (*it).c_str());
	evhttp_add_header(req->output_headers, "Content-Type", "text/plain");
	evhttp_send_reply(req, HTTP_OK, "Everything is fine", evb);
	evbuffer_free(evb);
}

void web_pipeline_start(struct evhttp_request *req, void *arg) {
	pipeline->start();
	web_message(req, "ok");
}

void web_pipeline_stop(struct evhttp_request *req, void *arg) {
	pipeline->stop();
	web_message(req, "ok");
}

void web_pipeline_slot_refresh(struct evhttp_request *req, void *arg) {
	std::ostringstream oss;
	cJSON *root, *slots, *slot;
	struct stat st;

	root = cJSON_CreateObject();
	cJSON_AddNumberToObject(root, "success", 1);
	cJSON_AddStringToObject(root, "message", "ok");
	cJSON_AddItemToObject(root, "slots", slots=cJSON_CreateArray());

	for ( int i = 1; i <= CCX_SLOTMAX; i++ ) {
		oss.str("");
		oss << CCX_SLOTDIR << i << CCX_SLOTEXT;
		cJSON_AddItemToArray(slots, slot=cJSON_CreateObject());
		if ( stat(oss.str().c_str(), &st) == 0 ) {
			cJSON_AddNumberToObject(slot, "lastmod", st.st_mtime);
		} else {
			cJSON_AddNumberToObject(slot, "lastmod", 0);
		}
	}

	web_json(req, root);
}

void web_pipeline_slot_load(struct evhttp_request *req, void *arg) {
	std::ostringstream oss;
	struct evkeyvalq headers;
	const char *uri, *s_idx;
	int slotidx;

	uri = evhttp_request_uri(req);
	if ( uri == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "unable to retreive uri");
	}

	evhttp_parse_query(uri, &headers);

	if ( evhttp_find_header(&headers, "idx") == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "missing idx");
	}

	s_idx = evhttp_find_header(&headers, "idx");
	slotidx = atoi(s_idx);
	evhttp_clear_headers(&headers);

	if ( slotidx < 1 || slotidx > CCX_SLOTMAX )
		return web_error(req, "invalid idx");

	oss << CCX_SLOTDIR << slotidx << CCX_SLOTEXT;

	pipeline->clear();
	if ( pipeline->parse(oss.str()) == false )
		return web_error(req, "unable to parse slot");

	evhttp_clear_headers(&headers);
	web_message(req, "ok");
}

void web_pipeline_slot_save(struct evhttp_request *req, void *arg) {
	std::string sout = pipeline->serializeCreation();
	std::ostringstream oss;
	struct evkeyvalq headers;
	const char *uri, *s_idx;
	int slotidx;
	//struct stat st;
	FILE *fd;

	uri = evhttp_request_uri(req);
	if ( uri == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "unable to retreive uri");
	}

	evhttp_parse_query(uri, &headers);

	if ( evhttp_find_header(&headers, "idx") == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "missing idx");
	}

	s_idx = evhttp_find_header(&headers, "idx");
	slotidx = atoi(s_idx);
	evhttp_clear_headers(&headers);

	if ( slotidx < 1 || slotidx > CCX_SLOTMAX )
		return web_error(req, "invalid idx");

	oss << CCX_SLOTDIR << slotidx << CCX_SLOTEXT;

	fd = fopen(oss.str().c_str(), "w");
	if ( fd == NULL )
		return web_error(req, "unable to open slot for writing");

	fwrite(sout.c_str(), sout.length(), 1, fd);
	fclose(fd);

	web_message(req, "ok");
}

void web_pipeline_dump(struct evhttp_request *req, void *arg) {
	struct evbuffer *evb = evbuffer_new();
	std::string sout = pipeline->serializeCreation();
	const char *out = sout.c_str();
	struct evkeyvalq headers;
	const char *uri;

	uri = evhttp_request_uri(req);
	if ( uri == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "unable to retreive uri");
	}

	evhttp_parse_query(uri, &headers);

	if ( evhttp_find_header(&headers, "download") != NULL ) {
		evhttp_add_header(req->output_headers, "Content-Type", "application/force-download; name=\"fusion-pipeline.txt\"");
		evhttp_add_header(req->output_headers, "Content-Disposition", "attachment; filename=\"fusion-pipeline.txt\"");
	} else
		evhttp_add_header(req->output_headers, "Content-Type", "text/plain");

	evbuffer_add(evb, out, strlen(out));
	evhttp_send_reply(req, HTTP_OK, "OK", evb);
	evbuffer_free(evb);

	evhttp_clear_headers(&headers);
}

void web_pipeline_quit(struct evhttp_request *req, void *arg) {
	web_message(req, "bye");
	want_quit_soon = true;
}

void web_index(struct evhttp_request *req, void *arg) {
	evhttp_add_header(req->output_headers, "Location", "/gui/index.html");
	evhttp_send_reply(req, HTTP_MOVETEMP, "Everything is fine", NULL);
}

void web_file(struct evhttp_request *req, void *arg) {
	FILE *fd;
	int readidx = 0, ret;
	long filesize = 0;
	struct evbuffer *evb;
	char filename[256],
		 *buf, *uri, *baseuri;

	/* web_file accept only file from gui
	 */
	if ( strstr(req->uri, "/gui/") != req->uri ) {
		evhttp_send_error(req, 404, "Not found");
		return;
	}

	if ( strstr(req->uri, "..") != NULL ) {
		evhttp_send_error(req, 403, "Security error");
		return;
	}

	uri = strdup(req->uri);
	if ( uri == NULL ) {
		LOG(CCX_ERROR, "unable to duplicate uri, memory missing ?");
		evhttp_send_error(req, 500, "Memory error");
		return;
	}

	baseuri = strsep(&uri, "?");

	snprintf(filename, sizeof(filename), "%s/%s",
		config_guidir.c_str(), baseuri + sizeof("/gui/") - 1);

	free(baseuri);

	LOG(CCX_DEBUG, "web: GET " << filename);
	fd = fopen(filename, "rb");
	if ( fd == NULL ) {
		evhttp_send_error(req, 404, "Not found");
		return;
	}

	fseek(fd, 0, SEEK_END);
	filesize = ftell(fd);
	fseek(fd, 0, SEEK_SET);

	buf = (char*)malloc(filesize);
	if ( buf == NULL ) {
		fclose(fd);
		web_error(req, "memory error");
		return;
	}

	while ( readidx < filesize ) {
		ret = fread(&buf[readidx], 1, filesize - readidx, fd);
		if ( ret <= 0 ) {
			perror("guifile");
			return;
		}
		readidx += ret;
	}
	fclose(fd);

	if ( strncmp(filename + strlen(filename) - 2, "js", 2) == 0 )
		evhttp_add_header(req->output_headers, "Content-Type", "application/javascript");
	else if ( strncmp(filename + strlen(filename) - 3, "css", 3) == 0 )
		evhttp_add_header(req->output_headers, "Content-Type", "text/css");
	else if ( strncmp(filename + strlen(filename) - 3, "png", 3) == 0 )
		evhttp_add_header(req->output_headers, "Content-Type", "image/png");
	else if ( strncmp(filename + strlen(filename) - 3, "swf", 3) == 0 )
		evhttp_add_header(req->output_headers, "Content-Type", "application/x-shockwave-flash");
	else
		evhttp_add_header(req->output_headers, "Content-Type", "text/html");

	evb = evbuffer_new();
	evbuffer_add(evb, buf, filesize);

	evhttp_send_reply(req, HTTP_OK, "Everything is fine", evb);
	evbuffer_free(evb);
	free(buf);
}

void usage(void) {
	printf("Usage: %s [options...]                                              \n" \
		   "                                                                \n" \
		   "  -t  --test                  Test mode, stop on the first error\n" \
		   "  -i  --info <modulename>     Show infos on a module            \n" \
		   "  -s  --syslog                Send loggings to syslog           \n" \
		   "  -d  --detach                Detach from console               \n" \
		   "  -p  --pidfile <filename>    Write PID into this file          \n" \
		   "  -n  --no_http               No webserver                      \n" \
		   "  -g  --guidir <filename>     Directory for GUI                 \n" \
		   "  -l  --pipeline <filename>   Read a pipeline from filename     \n",
		   CCX_DAEMON
	);
}

void describe(const char *name) {
	ccxModule *module;
	module = ccxFactory::getInstance()->create(name);
	if ( module == NULL ) {
		LOG(CCX_ERROR, "error: unable to found object named <" << name << ">");
		return;
	}
	module->describe();
	delete module;
}

int parse_options(int *argc, char ***argv) {
	int ch;
#ifndef WIN32
	static struct option options[] = {
		{"info", 1, 0, 'i'},
		{"pipeline", 1, 0, 'l'},
		{"syslog", 0, 0, 's'},
		{"detach", 0, 0, 'd'},
		{"pidfile", 1, 0, 'p'},
		{"guidir", 1, 0, 'g'},
		{"no_http", 0, 0, 'n'},
		{"test", 0, 0, 't'},
		{"help", 0, 0, 'h'},
		{0, 0, 0, 0}
	};
#endif
	while (1) {
		int option_index = 0;
#ifndef WIN32
		ch = getopt_long(*argc, *argv, "hp:g:l:sdni:t", options, &option_index);
#else
		//ch = getopt(*argc, *argv, "hp:g:l:sdni:t"); // fix this, http://www.wincli.com/?p=72
		ch = -1;
#endif
		if (ch == -1)
			break;
		switch ( ch ) {
			case 's':
				config_syslog = true;
				ccxLog::init(config_syslog);
				break;
			case 'd':
				config_detach = true;
				break;
			case 'p':
				//config_pidfile = std::string(optarg);
				break;
			case 'g':
				//config_guidir = std::string(optarg);
				break;
			case 'n':
				config_httpserver = false;
				break;
			case 'l':
				//config_pipelinefn = std::string(optarg);
				break;
			case 'i':
				ccxDaemon::init();
				//describe(optarg);
				return 0; // leave properly
			case 't':
				test_mode = true;
				break;
			case 'h':
			case '?':
			default:
				usage();
				return 0;
		}
	}

	(*argc) -= optind;
	(*argv) -= optind;

	return -1; // no error
}

void web_utterance_begin(struct evhttp_request *req, void *arg) {
    LOG(CCX_INFO, "Utterance started! ");
    web_message(req, "utterance start ok");
    pipeline->clearStreams();
}

void web_utterance_speech_begin(struct evhttp_request *req, void *arg) {
    // send a property container that controls recording; these property changes are properly notified for threaded operation.
    web_message(req, "speech begin ok");
    ccxModule* triggerModule = pipeline->getModuleById("Audio");
    LOG(CCX_DEBUG, "on: " << triggerModule->getName());
    // do a notifyUpdate() on the audio module, no need for input data as this is a trigger
    ccxDataGenericContainer* triggerContainer = new ccxDataGenericContainer();
    triggerContainer->properties["recording"] = new ccxProperty(true);
    triggerModule->getInput()->push(triggerContainer);
    triggerModule->trigger();
}

void web_utterance_speech_end(struct evhttp_request *req, void *arg) {
    // same as above, property container
    web_message(req, "speech end ok");
    ccxModule* triggerModule = pipeline->getModuleById("Audio");
    LOG(CCX_DEBUG, "off: " << triggerModule->getName());
    ccxDataGenericContainer* triggerContainer = new ccxDataGenericContainer();
    triggerModule->getInput()->push(triggerContainer);
    triggerModule->trigger();
}

void web_utterance_gesture_data(struct evhttp_request *req, void *arg) {
    // instead of a property container we feed JSON, (but for now just trigger())
    LOG(CCX_DEBUG, "Gesture data added to the utterance...");
    struct evbuffer* buf = req->input_buffer;
    char* input = (char *)malloc(sizeof(char) * 16384);
    evbuffer_remove(buf, input, 16384);
    cJSON *parsedInput = cJSON_Parse(input);
    LOG(CCX_DEBUG, input);
    std::vector<client::unimodalLeafNode> *gestureTree = new std::vector<client::unimodalLeafNode>();
    for(int index = 0; index < cJSON_GetArraySize(parsedInput); index++) {
        cJSON *item = cJSON_GetArrayItem(parsedInput, index);
        std::string typeString = std::string(cJSON_PrintUnformatted(cJSON_GetObjectItem(item, "type")));
        std::string valString = std::string(cJSON_PrintUnformatted(cJSON_GetObjectItem(item, "val")));
        typeString.erase(
                         remove( typeString.begin(), typeString.end(), '\"' ),
                         typeString.end()
                         );
        valString.erase(
                        remove( valString.begin(), valString.end(), '\"' ),
                        valString.end()
                        );
        client::unimodalLeafNode *debugNode = new client::unimodalLeafNode;
        debugNode->type = typeString;
        debugNode->val = valString;
        gestureTree->push_back(*debugNode);

    }
    // do a notifyUpdate() on the gesture JSON module, and feed in the special gesture stream (pushing to the stream would have it call this automatically)
    web_message(req, "utterance data ok");
    ccxModule* triggerModule = pipeline->getModuleById("Gesture");
    LOG(CCX_DEBUG, "gd: " << triggerModule->getName());
    triggerModule->getInput()->push(gestureTree);
    triggerModule->trigger();
}

void web_utterance_end(struct evhttp_request *req, void *arg) {
    LOG(CCX_INFO, "Utterance stop!");
    web_message(req, "Utterance stop.");
}

void web_utterance_get(struct evhttp_request *req, void *arg) {
    LOG(CCX_INFO, "Utterance get!");
    cJSON* outputJSON = (cJSON *)pipeline->getModuleById("Interaction")->getOutput()->getData();
    if(outputJSON != NULL) web_json(req, outputJSON);
    else web_error(req, "no valid utterance");
}

int main(int argc, char **argv) {
	int ret, exit_ret = 0;

	// initialize all signals
#ifndef _WIN32
	signal(SIGPIPE, SIG_IGN);
#endif
	signal(SIGTERM, signal_term);
	signal(SIGINT, signal_term);

	// initialize log
	ccxLog::init(config_syslog);

	// parse options
	ret = parse_options(&argc, &argv);
	if ( ret >= 0 ) {
		ccxDaemon::cleanup();
		return ret;
	}

	// initialize daemon (factory, network...)
	ccxDaemon::init();

	// detach from console
	if (config_detach)
		if (! ccxDaemon::detach(config_pidfile))
			return exit_ret;

	// parse pipeline passed in parameters
	if ( config_pipelinefn != "" ) {
		pipeline = new ccxPipeline();
		if ( pipeline == NULL )
			goto exit_critical;
		if ( pipeline->parse(config_pipelinefn) == false )
			goto exit_critical;
		//pipeline->start();
	} else if ( config_httpserver == false ) {
		LOG(CCX_CRITICAL, "no pipeline or webserver to start !");
		goto exit_critical;
	}

	// no default pipeline ? create one !
	if ( pipeline == NULL )
		pipeline = new ccxPipeline();
	if ( pipeline == NULL ) {
		LOG(CCX_CRITICAL, "unable to create default pipeline");
		goto exit_critical;
	}

	// if an http server is asked, start it !
	if ( config_httpserver ) {

		base = event_init();
		server = evhttp_new(NULL);

		if ( server == NULL ) {
			LOG(CCX_CRITICAL, "unable to create http server");
			goto exit_critical;
		}

		do {
			ret = evhttp_bind_socket(server, "127.0.0.1", 7500);
			if ( ret == -1 ) {
				perror("HTTP server");
				LOG(CCX_ERROR, "unable to open socket for 127.0.0.1:7500... retry in 3s");
				SLEEP(3000);
			}
		} while ( ret == -1 );

		LOG(CCX_INFO, "Http server running at http://127.0.0.1:7500/");

		evhttp_set_cb(server, "/", web_index, NULL);
		evhttp_set_cb(server, "/factory/list", web_factory_list, NULL);
		evhttp_set_cb(server, "/factory/describe", web_factory_describe, NULL);
		evhttp_set_cb(server, "/pipeline/create", web_pipeline_create, NULL);
		evhttp_set_cb(server, "/pipeline/remove", web_pipeline_remove, NULL);
		evhttp_set_cb(server, "/pipeline/status", web_pipeline_status, NULL);
		evhttp_set_cb(server, "/pipeline/connect", web_pipeline_connect, NULL);
		evhttp_set_cb(server, "/pipeline/set", web_pipeline_set, NULL);
		evhttp_set_cb(server, "/pipeline/get", web_pipeline_get, NULL);
		evhttp_set_cb(server, "/pipeline/gui", web_pipeline_gui, NULL);
		//evhttp_set_cb(server, "/pipeline/stream", web_pipeline_stream, NULL);  //[OpenCV]
		evhttp_set_cb(server, "/pipeline/start", web_pipeline_start, NULL);
		evhttp_set_cb(server, "/pipeline/stop", web_pipeline_stop, NULL);
		evhttp_set_cb(server, "/pipeline/quit", web_pipeline_quit, NULL);
		evhttp_set_cb(server, "/pipeline/dump", web_pipeline_dump, NULL);
		evhttp_set_cb(server, "/pipeline/stats", web_pipeline_stats, NULL);
		evhttp_set_cb(server, "/pipeline/slot/refresh", web_pipeline_slot_refresh, NULL);
		evhttp_set_cb(server, "/pipeline/slot/load", web_pipeline_slot_load, NULL);
		evhttp_set_cb(server, "/pipeline/slot/save", web_pipeline_slot_save, NULL);
        
        evhttp_set_cb(server, "/utterance/begin", web_utterance_begin, NULL);
        evhttp_set_cb(server, "/utterance/end", web_utterance_end, NULL);
        evhttp_set_cb(server, "/utterance/speech/begin", web_utterance_speech_begin, NULL);
        evhttp_set_cb(server, "/utterance/speech/end", web_utterance_speech_end, NULL);
        evhttp_set_cb(server, "/utterance/gesture/data", web_utterance_gesture_data, NULL);
        evhttp_set_cb(server, "/utterance/get", web_utterance_get, NULL);
        
		evhttp_set_gencb(server, web_file, NULL);
	}

	// main loop
	while ( want_quit == false && want_quit_soon_count < 5) {
		SLEEP(g_config_delay);

		// update pipeline
		if ( pipeline->isStarted() ) {
			pipeline->poll();

			// check for error in pipeline
			while ( pipeline->haveError() ) {
				LOG(CCX_ERROR, "Pipeline error: " << pipeline->getLastError());
				if ( test_mode )
					want_quit = true;
			}
		}

		// if we're running the server, allow the event loop to have control for a while
		if ( server != NULL )
			event_base_loop(base, EVLOOP_ONCE|EVLOOP_NONBLOCK);
            //event_base_dispatch(base);
        
        if(want_quit_soon == true) {
            want_quit_soon_count++;
        }
	}

exit_standard:
	if ( server != NULL )
		evhttp_free(server);
	if ( base != NULL )
		event_base_free(base);

	if ( pipeline != NULL )
		delete pipeline;
	ccxDaemon::cleanup();

	return 0;

exit_critical:
	exit_ret = 1;
	goto exit_standard;
}
