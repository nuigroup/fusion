/////////////////////////////////////////////////////////////////////////////
// Name:        ccx/ccxModule.h
// Author:      Scott Halstvedt
// Copyright:   (c) 2011 NUI Group
/////////////////////////////////////////////////////////////////////////////


/***********************************************************************
 ** Some parts Copyright (C) 2010 Movid Authors.  All rights reserved.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **********************************************************************/


#ifndef CCX_MODULE_H
#define CCX_MODULE_H

#include <string>
#include <map>

#include "ccxProperty.h"
#include "pasync.h"

class ccxThread;
class ccxDataStream;
class ccxDataStreamInfo;
class ccxPipeline;

typedef struct {
	double average_fps;
	double average_process_time;
	double average_wait_time;
	double total_process_time;
	double total_wait_time;
	unsigned long long total_process_frame;

	// used for calculation of fps and update time
	unsigned long long _process_frame;
	double _process_time;
	double _wait_time;
	double _last_time;
} ccx_module_stats_t;


/*! \brief Declare a new module (create all needed informations) + Log declaration
 *
 * \param name Name of the module
 * \param author Author of the module (native mean not external)
 * \param description Description of the module
 */
#define MODULE_DECLARE(name, author, description) \
	LOG_DECLARE(#name); \
	MODULE_DECLARE_EX(name, Module, author, description);

/*! \brief Declare a new module (create all needed informations)
 *
 * \param name Name of the module
 * \param module Postfix of the classname module
 * \param author Author of the module (native mean not external)
 * \param description Description of the module
 */
#define MODULE_DECLARE_EX(name, module, author, description) \
	static std::string module_name = #name; \
	static std::string module_author = author; \
	static std::string module_description = description; \
	std::string ccx##name##module::getName() { return module_name; } \
	std::string ccx##name##module::getDescription() { return module_description; } \
	std::string ccx##name##module::getAuthor() { return module_author; } \
	ccxModule *factory_create_ccx##name() { return new ccx##name##module(); }


/*! \brief Call initialization of the module (must be inserted in constructor)
 */
#define MODULE_INIT() \
	this->properties["id"] = new ccxProperty(ccxModule::createId(module_name)); \
	LOG(CCX_DEBUG, "create object <" << module_name << "> with id <" \
			   << this->property("id").asString() << ">");

/*! \brief Declare needed functions for a module (must be inserted at end of a module)
 */
#define MODULE_INTERNALS() 					\
	public:									\
	virtual std::string getName(); 			\
	virtual std::string getDescription(); 	\
	virtual std::string getAuthor();

enum {
	CCX_MODULE_NONE		= 0x00000000,		/*< Module have no input/output */
	CCX_MODULE_INPUT		= 0x00000001,		/*< Module have inputs */
	CCX_MODULE_OUTPUT	= 0x00000002,		/*< Module have outputs */
	CCX_MODULE_GUI       = 0x00000004,		/*< Module have a GUI */
};

/*! \brief Base class for all modules
 */
class ccxModule {
public:	

	/*! \brief Instance a module
	 *
	 * \param capabilities combine flags from CCX_MODULE_*
	 *
	 * \return an instance of ccxModule()
	 */
	ccxModule(unsigned int capabilities);

	/*! \brief Free the module
	 */
	virtual ~ccxModule();
    
    /*! \brief Trigger the module to the start of an utterance
     */
    virtual void trigger();

	/*! \brief Declare an input (only at init time.)
	 */
	void declareInput(int n, ccxDataStream **storage, ccxDataStreamInfo *info);

	/*! \brief Declare an output (only at init time.)
	 */
	void declareOutput(int n, ccxDataStream **storage, ccxDataStreamInfo *info);
	
	/*! \brief Connect an input from an existing ccxDataStream
	 *
	 * \param stream instance of a ccxDataStream()
	 * \param n index of the input to connect
	 */
	virtual void setInput(ccxDataStream* stream, int n=0);

	/*! \brief Get a input data stream
	 *
	 * \param n index of the input to get
	 *
	 * \return the selected input.
	 */
	virtual ccxDataStream *getInput(int n=0);

	/*! \brief Get a output data stream
	 *
	 * \param n index of the output to get
	 *
	 * \return the selected output.
	 */
	virtual ccxDataStream *getOutput(int n=0);
	
	/*! \brief Get the input index at which the data stream is attached
	 *
	 * \param ds datastream which is connected to the module
	 *
	 * \return the input index at which ds is connected, -1 if it is not connected
	 */
	virtual int getInputIndex(ccxDataStream* stream);

	/*! \brief Get the output index at which the data stream is attached
	 *
	 * \param ds datastream which is connected to the module
	 *
	 * \return the output index at which ds is connected, -1 if it is not connected
	 */
	virtual int getOutputIndex(ccxDataStream* stream);

	/*! \brief Get number of input stream
	 */
	virtual int getInputCount();

	/*! \brief Get number of output stream
	 */
	virtual int getOutputCount();

	/*! \brief Update the type of an output
	 */
	virtual void setInputType(int n, const std::string& type);

	/*! \brief Update the type of an output
	 */
	virtual void setOutputType(int n, const std::string& type);

	/*! \brief Get informations about input stream
	 *
	 * \param n index of the input to get
	 *
	 * \return an instance of ccxDataStreamInfo()
	 */
	virtual ccxDataStreamInfo *getInputInfos(int n=0);

	/*! \brief Get informations about output stream
	 *
	 * \param n index of the output to get
	 *
	 * \return an instance of ccxDataStreamInfo()
	 */
	virtual ccxDataStreamInfo *getOutputInfos(int n=0);

	/*! \brief Start the module
	 */
	virtual void start();

	/*! \brief Stop the module
	 */
	virtual void stop();

	/*! \brief Update the module
	 */
	virtual void update() = 0;

	/*! \brief Poll the module (usually, update() is called if it's not threaded.
	 */
	virtual void poll();
    
	/*! \brief Lock access to the module data
	 */
	virtual void lock();

	/*! \brief Unlock access to the module data
	 */
	virtual void unlock();

	/*! \brief Check if the module is started
	 *
	 * \return true if the module is started, otherwise false
	 */
	virtual bool isStarted();

	/*! \brief Check if the module is an internal module
	 *
	 * \return true if the module is an internal one.
	 */
	virtual bool isInternal();

	/*! \brief Get a property from the module
	 * If the property not exist, it will be automaticly created
	 *
	 * \param name name of the property to lookup
	 *
	 * \return return the reference of the property
	 */
	ccxProperty &property(std::string name);

	/*! \brief Get all properties
	 */
	std::map<std::string, ccxProperty*> &getProperties();

	/*! \brief Get capabilities flags
	 */
	unsigned int getCapabilities();

	/*! \brief Get the name of the module
	 */
	virtual std::string getName() = 0;

	/*! \brief Get the description of the module
	 */
	virtual std::string getDescription() = 0;

	/*! \brief Get the author of the module
	 */
	virtual std::string getAuthor() = 0;

	/*! \brief Show on console all information on the module
	 */
	void describe();

	/*! \brief Get last error message, once called, it will reset the error state
	 */
	virtual std::string getLastError();

	/*! \brief Indicate if the module have an error
	 */
	virtual bool haveError();

	/*! \brief Check if the module need to be updated
	 */
	bool needUpdate(bool lock=false);

	/*! \brief Export setup of the module to a file
	 */
	virtual bool serializeCreation(std::ostringstream &oss, bool do_data);

	/*! \brief Export connections of the module to a file
	 */
	virtual bool serializeConnections(std::ostringstream &oss);

	/*! \brief Receive some feedback on the configuration ui
	 */
	virtual void guiFeedback(const std::string &type, double x, double y);

	/*! \brief Build the gui
	 */
	virtual void guiBuild();

	/*! \brief Get instruction about how to draw the GUI
	 */
	std::vector<std::string> &getGui();

	/*! \brief Notify gui to rebuild itself
	 */
	void notifyGui();

	/*! \brief Module statistics
	 */
	ccx_module_stats_t stats;
	
private:
	/*! \brief Capabilities flags
	 */
	unsigned int capabilities;

	/*! \brief Boolean to store if the module is started or not
	 */
	bool is_started;

	/*! \brief Indicate if error exist on module
	 */
	bool is_error;

	/*! \brief Store the last error message
	 */
	std::string error_msg;

	/*! \brief Store the thread instance if it's used
	 */
	ccxThread *thread;

	/*! \brief Boolean to indicate if we use thread or not
	 */
	bool use_thread;

	/*! \brief Boolean to known if we need to call update or not
	 */
	bool need_update;

	/*! \brief Boolean to known if the gui must be rebuild
	 */
	bool need_gui_build;

	/*! \brief Mutex to protect part of the module
	 */
	pt::mutex *mtx;

	/*! \brief Trigger to awake the thread
	 */
	pt::trigger *thread_trigger;

protected:

	/*! \brief Pipeline that own the module
	 */
	ccxModule *owner;

	/*! \brief Indicate if the module is internal to fusion
	 *         and must not be exported.
	 */
	bool is_internal;

	/*! \brief Call it if you want to notify to call update()
	 */
	virtual void notifyUpdate();

	/*! \brief Input mapping
	 */
	std::map<int, ccxDataStream **> input_map;

	/*! \brief Output mapping
	 */
	std::map<int, ccxDataStream **> output_map;

	/*! \brief Input informations
	 */
	std::map<int, ccxDataStreamInfo*> input_infos;

	/*! \brief Output informations
	 */
	std::map<int, ccxDataStreamInfo*> output_infos;

	/*! \brief Properties storage
	 */
	std::map<std::string, ccxProperty*> properties;

	/*! \brief Indicate if the module is a pipeline
	 *
	 * \return true if it's a pipeline, otherwise false
	 */
	virtual bool isPipeline();

	/*! \brief Notify the module that an input data stream have been updated
	 *
	 * \param source instance of the updated data stream
	 */
	virtual void notifyData(ccxDataStream *source);

	/*! \brief Set the error state on the class
	 */
	void setError(const std::string &msg);

	/*! \brief Create a uniq id for the instance of the class
	 *
	 * \param base name of the class to use it
	 */
	static std::string createId(std::string base);

	/*! \brief Storage for GUI instruction
	 */
	std::vector<std::string> gui;

	friend class ccxDataStream;
	friend class ccxPipeline;
};

#endif

