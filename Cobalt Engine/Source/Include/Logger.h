/*
	Logger.h

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#pragma once

#include <assert.h>
#include <string>
#include "EngineStd.h"

/*
	Call CB_ASSERT() just like a regular assert()
		ex: CB_ASSERT(ptr != nullptr);

	CALL CB_ERROR() to send out an error message
		ex: CB_ERROR("Something went wrong");


	LOGGING:
	
	Call CB_LOG("tag" "message") to print to the log:
		[tag] Message

	To enable logging, manually call Logger::SetDisplayFlags() or use a logging.xml config
	file with the following format:
	<Logger>
		<Log tag="Object" debugger="1" file="0"/>
	</Logger>


	This will cause all logs with the Object tag to be displayed in the debugger instead of 
	to a file. 
*/


// display flags
const unsigned char LOGFLAG_WRITE_TO_LOG_FILE = 1 << 0;
const unsigned char LOGFLAG_WRITE_TO_DEBUGGER = 1 << 1;

/**
	Public Logger interface. Call Init() before any logging takes place and Destroy() 
	when you're done to clean it up. Call SetDisplayFlags() to set the display flags for
	a specific tag. They are all off by default. Use CB_LOG() to print to the logger.
*/
namespace Logger
{
	/**
		This class is used by the debug macros and should not be accessed externally.
	*/
	class ErrorMessenger
	{
	public:
		/// Default constructor
		ErrorMessenger();

		/// Print an error message
		void Show(const std::string& message, bool isFatal, const char* funcName, const char* sourceFile, unsigned int lineNum);

	private:
		/// Is the error messenger enabled or not
		bool m_Enabled;
	};

	/// Initialize the logger with a config file
	void Init(const char* loggingConfigFilename = nullptr);

	/// Shutdown the logger
	void Destroy();

	/// Log a message
	void Log(const std::string& tag, const std::string& message, const char* funcName, const char* sourceFile, unsigned int lineNum);

	/// Set the display flags for a tag (0 or 1)
	void SetDisplayFlags(const std::string& tag, unsigned char flags);
}


//====================================================
//	Debug Macros
//====================================================

// log a tag and message
#define CB_LOG(tag, str) \
do \
{ \
	std::string s((str)); \
	Logger::Log(tag, s, nullptr, nullptr, 0); \
} \
while (0) \


// errors are potentially fatal. they are presented as a dialog with Abort, Retry, Ignore.
// Abort will break into the debugger, Retry will continue the game, and Ignore will ignore every 
// subsequent call to this specific error.  They are ignored completely in release mode. 
#define CB_ERROR(str) \
do \
{ \
	static Logger::ErrorMessenger* pErrorMessenger = CB_NEW Logger::ErrorMessenger; \
	std::string s((str)); \
	pErrorMessenger->Show(s, false, __FUNCTION__, __FILE__, __LINE__); \
} \
while (0) \


// warnings are recoverable, they are logs with the [WARNING] tag
#define CB_WARNING(str) \
do \
{ \
	std::string s((str)); \
	Logger::Log("WARNING", s, __FUNCTION__, __FILE__, __LINE__); \
} \
while (0) \


// similar to a regular assert()
#define CB_ASSERT(x) \
do \
{ \
if (!(x)) \
{ \
	static Logger::ErrorMessenger* pErrorMessenger = CB_NEW Logger::ErrorMessenger; \
	pErrorMessenger->Show(#x, false, __FUNCTION__, __FILE__, __LINE__); \
} \
} \
while (0) \


// fatal errors presented to the user
#define CB_FATAL(str) \
do \
{ \
	static Logger::ErrorMessenger* pErrorMessenger = CB_NEW Logger::ErrorMessenger; \
	std::string s((str)); \
	pErrorMessenger->Show(s, true, __FUNCTION__, __FILE__, __LINE__); \
} \
while (0) \


#define EXIT_ASSERT CB_ASSERT(0)
