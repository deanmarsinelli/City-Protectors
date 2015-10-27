/*
	Logger.cpp

	Inspired by Game Coding Complete 4th ed.
	by Mike McShaffry and David Graham
*/

#include <list>
#include <unordered_map>

#include "Logger.h"

// log file name
static const char* ERRORLOG_FILENAME = "error.log";

// default display flags
#ifdef _DEBUG
const unsigned char ERRORFLAG_DEFAULT =		LOGFLAG_WRITE_TO_DEBUGGER;
const unsigned char WARNINGFLAG_DEFAULT =	LOGFLAG_WRITE_TO_DEBUGGER;
const unsigned char LOGFLAG_DEFAULT =		LOGFLAG_WRITE_TO_DEBUGGER;

#elif NDEBUG
const unsigned char ERRORFLAG_DEFAULT =		(LOGFLAG_WRITE_TO_DEBUGGER | LOGFLAG_WRITE_TO_LOG_FILE);
const unsigned char WARNINGFLAG_DEFAULT =	(LOGFLAG_WRITE_TO_DEBUGGER | LOGFLAG_WRITE_TO_LOG_FILE);
const unsigned char LOGFLAG_DEFAULT =		(LOGFLAG_WRITE_TO_DEBUGGER | LOGFLAG_WRITE_TO_LOG_FILE);

#else
const unsigned char ERRORFLAG_DEFAULT = 0;
const unsigned char WARNINGFLAG_DEFAULT = 0;
const unsigned char LOGFLAG_DEFAULT = 0;

#endif


// singleton log manager
class LogManager;
static LogManager* s_pLogMgr = nullptr;


//====================================================
//	LogManager class - private class to write to log
//====================================================
class LogManager
{
public:
	// enum and typedefs
	enum ErrorDialogResult
	{
		LOGMGR_ERROR_ABORT,
		LOGMGR_ERROR_RETRY,
		LOGMGR_ERROR_IGNORE
	};

	typedef std::unordered_map<std::string, unsigned char> Tags;
	typedef std::list<Logger::ErrorMessenger*> ErrorMessengerList;

public:
	LogManager();
	~LogManager();
	void Init(const char* loggingConfigFilename);

	// logging
	void Log(const std::string& tag, const std::string& message, const char* funcName, const char* sourceFile, unsigned int lineNum);
	void SetDisplayFlags(const std::string& tag, unsigned char flags);

	// errors
	void AddErrorMessenger(Logger::ErrorMessenger* pMessenger);
	LogManager::ErrorDialogResult Error(const std::string& errorMessage, bool isFatal, const char* funcName, const char* sourceFile, unsigned int lineNum);

private:
	// log helpers
	void OuputFinalBufferToLogs(const std::string& finalBuffer, unsigned char flags);
	void WriteToLogFile(const std::string& data);
	void GetOuputBuffer(std::string& outOuputBuffer, const std::string& tag, const std::string& message, const char* funcName, const char* sourceFile, unsigned int lineNum);

public:
	// map of tags
	Tags m_Tags;
	ErrorMessengerList m_ErrorMessengers;

	// thread safety
	CriticalSection m_TagCriticalSection;
	CriticalSection m_MessengerCriticalSection;
};


// constructor
LogManager::LogManager()
{
	// set default log tags
	SetDisplayFlags("ERROR", ERRORFLAG_DEFAULT);
	SetDisplayFlags("WARNING", WARNINGFLAG_DEFAULT);
	SetDisplayFlags("INFO", LOGFLAG_DEFAULT);
}


// destructor
LogManager::~LogManager()
{
	m_MessengerCriticalSection.Lock();
	// delete all error messengers
	for (auto it = m_ErrorMessengers.begin(); it != m_ErrorMessengers.end(); ++it)
	{
		auto pMessenger = (*it);
		delete pMessenger;
	}
	
	m_ErrorMessengers.clear();
	m_MessengerCriticalSection.Unlock();
}


// initialize the logger
void LogManager::Init(const char* loggingConfigFilename)
{
	// if a file is given
	if (loggingConfigFilename)
	{
		TiXmlDocument configFile(loggingConfigFilename);

		if (configFile.LoadFile())
		{
			TiXmlElement* pRoot = configFile.RootElement();
			if (!pRoot)
			{
				// no root element
				return;
			}

			// loop through each element and set display flags
			for (TiXmlElement* pNode = pRoot->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement())
			{
				unsigned char flags = 0;
				std::string tag(pNode->Attribute("tag"));

				// display logs to debugger?
				int debugger = 0;
				pNode->Attribute("debugger", &debugger);
				if (debugger)
					flags |= LOGFLAG_WRITE_TO_DEBUGGER;

				// display logs to file?
				int logfile = 0;
				pNode->Attribute("file", &logfile);
				if (logfile)
					flags |= LOGFLAG_WRITE_TO_LOG_FILE;

				SetDisplayFlags(tag, flags);
			}
		}
	}
}


// build up the log string and output it based on the display flags
void LogManager::Log(const std::string& tag, const std::string& message, const char* funcName, const char* sourceFile, unsigned int lineNum)
{
	m_TagCriticalSection.Lock();
	auto it = m_Tags.find(tag);
	if (it != m_Tags.end())
	{
		m_TagCriticalSection.Unlock();

		// if the tag is found, log a message for it
		std::string buffer;
		GetOuputBuffer(buffer, tag, message, funcName, sourceFile, lineNum);
		OuputFinalBufferToLogs(buffer, it->second);
	}
	else
	{
		// tag not found
		m_TagCriticalSection.Unlock();
	}
}


// set the display flags for a specific tag
void LogManager::SetDisplayFlags(const std::string& tag, unsigned char flags)
{
	m_TagCriticalSection.Lock();
	if (flags != 0)
	{
		auto it = m_Tags.find(tag);
		if (it != m_Tags.end())
		{
			// update the flags
			it->second = flags;
		}
		else
		{
			// tag doesn't exist yet, insert a new tag/flags pair
			m_Tags.insert(std::make_pair(tag, flags));
		}
	}
	else
	{
		// remove the tag is flags set to 0
		m_Tags.erase(tag);
	}
	m_TagCriticalSection.Unlock();
}


// add an error messenger to the list of messengers
void LogManager::AddErrorMessenger(Logger::ErrorMessenger* pMessenger)
{
	m_MessengerCriticalSection.Lock();
	m_ErrorMessengers.push_back(pMessenger);
	m_MessengerCriticalSection.Unlock();
}


// helper method for error messenger
LogManager::ErrorDialogResult LogManager::Error(const std::string& errorMessage, bool isFatal, const char* funcName, const char* sourceFile, unsigned int lineNum)
{
	std::string tag = ((isFatal) ? ("FATAL") : ("ERROR"));

	// buffer for the final output string
	std::string buffer;
	GetOuputBuffer(buffer, tag, errorMessage, funcName, sourceFile, lineNum);

	// write final buffer to various logs
	m_TagCriticalSection.Lock();
	auto it = m_Tags.find(tag);
	if (it != m_Tags.end())
	{
		OuputFinalBufferToLogs(buffer, it->second);
	}
	m_TagCriticalSection.Unlock();

	// show dialog box
	int result = ::MessageBoxA(NULL, buffer.c_str(), tag.c_str(), MB_ABORTRETRYIGNORE | MB_ICONERROR | MB_DEFBUTTON3);

	// respond to the choice
	switch (result)
	{
	case IDIGNORE: return LogManager::LOGMGR_ERROR_IGNORE;
	case IDABORT: __debugbreak(); return LogManager::LOGMGR_ERROR_RETRY;  // assembly language instruction to break into the debugger
	case IDRETRY:	return LogManager::LOGMGR_ERROR_RETRY;
	default:       return LogManager::LOGMGR_ERROR_RETRY;
	}
}

// check all the display flags and output the final buffer to the appropriate places
// make sure this method is called from somewhere wrapped in a critical section to be thread safe
void LogManager::OuputFinalBufferToLogs(const std::string& finalBuffer, unsigned char flags)
{
	// log file
	if ((flags & LOGFLAG_WRITE_TO_LOG_FILE) > 0)
	{
		WriteToLogFile(finalBuffer);
	}

	// debugger output window
	if (flags & LOGFLAG_WRITE_TO_DEBUGGER > 0)
	{
		::OutputDebugStringA(finalBuffer.c_str());
	}
}


// write the data string to the log file
void LogManager::WriteToLogFile(const std::string& data)
{
	// open the log file for appending
	FILE* pLogFile = nullptr;
	fopen_s(&pLogFile, ERRORLOG_FILENAME, "a+");
	if (!pLogFile)
	{
		// error, can't write to the file
		return;
	}

	fprintf_s(pLogFile, data.c_str());

	fclose(pLogFile);
}


// fills the outOutputBuffer with the correct error string
void LogManager::GetOuputBuffer(std::string& outOuputBuffer, const std::string& tag, const std::string& message, const char* funcName, const char* sourceFile, unsigned int lineNum)
{
	// tag and message
	if (!tag.empty())
	{
		outOuputBuffer = "[" + tag + "] " + message;
	}
	else
	{
		// no tag
		outOuputBuffer = message;
	}

	// function name
	if (funcName != nullptr)
	{
		outOuputBuffer += "\nFunction: ";
		outOuputBuffer += funcName;
	}

	// source file
	if (sourceFile != nullptr)
	{
		outOuputBuffer += "\n";
		outOuputBuffer += funcName;
	}

	// line number
	if (lineNum != 0)
	{
		outOuputBuffer += "\nLine: ";
		char lineNumBuffer[11];
		ZeroMemory(lineNumBuffer, sizeof(char));
		outOuputBuffer += _itoa(lineNum, lineNumBuffer, 10);
	}

	outOuputBuffer += "\n";
}


//====================================================
//	ErrorMessenger definitions
//====================================================
Logger::ErrorMessenger::ErrorMessenger()
{
	// add this error messenger to the singleton log manager
	s_pLogMgr->AddErrorMessenger(this);
	m_Enabled = true;
}

void Logger::ErrorMessenger::Show(const std::string& message, bool isFatal, const char* funcName, const char* sourceFile, unsigned int lineNum)
{
	if (m_Enabled)
	{
		if (s_pLogMgr->Error(message, isFatal, funcName, sourceFile, lineNum) == LogManager::LOGMGR_ERROR_IGNORE)
		{
			// disable the error messenger if the user hits ignore
			m_Enabled = false;
		}
	}
}


//====================================================
//	C free functions
//====================================================
namespace Logger
{

void Init(const char* loggingConfigFilename)
{
	if (!s_pLogMgr)
	{
		s_pLogMgr = CB_NEW LogManager;
		s_pLogMgr->Init(loggingConfigFilename);
	}
}

void Destroy()
{
	CB_SAFE_DELETE(s_pLogMgr);
}

void Log(const std::string& tag, const std::string& message, const char* funcName, const char* sourceFile, unsigned int lineNum)
{
	CB_ASSERT(s_pLogMgr);
	s_pLogMgr->Log(tag, message, funcName, sourceFile, lineNum);
}

void SetDisplayFlags(const std::string& tag, unsigned char flags)
{
	CB_ASSERT(s_pLogMgr);
	s_pLogMgr->SetDisplayFlags(tag, flags);
}

}
