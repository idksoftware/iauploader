//
//
//
// 
//
//
//
// 
// frank.watson@aaltohaps.com
/////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <fstream>
#include <string>
#include <vector>
//#include "CommonUtilsLib_defs.h"

#ifndef FILECODE
#define FILECODE 01
#endif

#define LOG_OK ((FILECODE * 10000) + __LINE__)


namespace Common
{
	enum class AALTOLogNoticeType
	{
		TRACE,		// Trace debugging
		FINE,		// Fine debugging
		INFO,		// General information
		STATUS,		// Status information
		SUMMARY,	// Summary information
		WARNING,	// Warning of incomplete operations
		ERR,		// Failed operations
		FATAL,		// Application cannot complete
		UNKNOWN = -1

	};

	class AALTOLogger;

	class LogMessage {
		std::string message;
		int m_code;
		AALTOLogNoticeType m_type;
		
		friend class Logger;
	public:
		LogMessage() = default;
		~LogMessage() = default;

		//LogMessage& operator<<(bool value);

		LogMessage& operator<<(const char* value);

		LogMessage& operator<<(std::string& value);
	
		LogMessage& operator<<(long value);
		
		LogMessage& operator<<(unsigned long value);
		
		LogMessage& operator<<(long long value);
		
		LogMessage& operator<<(unsigned long long value);
		
		LogMessage& operator<<(double value);
		
		LogMessage& operator<<(long double value);
		
		LogMessage& operator<<(short value);
		
		LogMessage& operator<<(int value);
		
		LogMessage& operator<<(unsigned short value);
		
		LogMessage& operator<<(unsigned int value);
	
		LogMessage& operator<<(float value);
		
		void setMessage(const char* value) {
			std::string v = value;
			setMessage(v);
		}

		void setMessage(std::string& value)
		{
			if (message.empty()) {
				message = value;
			}
			else {
				message += value;
			}

			
		}

		void clear() { message.clear(); };
	};


	//! Used to contain a single log msg.
	class AALTOLogEntry
	{
	public:

		AALTOLogEntry() = default;
		AALTOLogEntry(const AALTOLogEntry& orig) = delete;

		AALTOLogEntry& operator= (const AALTOLogEntry& orig) = delete;
		virtual ~AALTOLogEntry() = default;

		virtual bool setMsgType(AALTOLogNoticeType noticeType);
		virtual AALTOLogNoticeType getMsgType() const;

		virtual bool setMsg(const std::string msg);
		virtual const std::string& getMsg() const;

	protected:

		AALTOLogNoticeType m_noticeType;

		std::string m_message;
	};


	//! Used to contain general status notification message or warnings.
	class Logger
	{
		LogMessage m_logMessage;
		bool m_isFatal{ false };
		//int m_lastCode;
		//std::string m_lastMessage;

		public:

			Logger() = default;
			Logger(const Logger& orig) = delete;
			Logger& operator= (const AALTOLogger& orig) = delete;

			virtual ~Logger() = default;

			const bool enableConsole(const bool sendToConsole);

			

			int returnCode();

			void log(int code, AALTOLogNoticeType type, const std::string& message);
			void log(int code, AALTOLogNoticeType type, const char* format, ...);

			LogMessage& log(int code, AALTOLogNoticeType type);

			Logger& operator << (const std::string& message);

			Logger& operator << (LogMessage& message);

			static Logger& getLogger();
			static const int getLastCode() {
				/*
				if (m_lastCode == 0) {
					// check logger for errors
					if (CLogger::getLastCode() != 0) {
						m_lastCode = CLogger::getLastCode();
					}
				}
				*/
				return m_lastCode;
			}
			static const char* getLastMessage() {
				/*
				if (CLogger::getLastCode() != 0) {
					m_lastCode = CLogger::
				}
				*/
				return m_lastMessage.c_str();
			}

			static AALTOLogNoticeType getHighestLevel()
			{
				return m_lastLevel;
			}

			static void setMaxSize(int size) {
				m_size = size;
			}

			static void setSilent(bool b = false) {
				m_isSilent = b;
			}

			static void setQuiet(bool b = false) {
				m_isQuiet = b;
			}

			static void setNetworkLevel(AALTOLogNoticeType level) {
				m_consoleLevel = level;
			}

			static bool setNetworkLevel(const std::string& level) {
				return setLevel(m_consoleLevel, level);
			}

			static void setConsoleLevel(AALTOLogNoticeType level) {
				m_consoleLevel = level;
			}

			static bool setConsoleLevel(const std::string& level) {
				return setLevel(m_consoleLevel, level);
			}

			static void setLogLevel(AALTOLogNoticeType level) {
				m_logLevel = level;
			}

			static bool setLogLevel(const std::string& level) {
				return setLevel(m_logLevel, level);
			}

			static void setAppName(const std::string& appName) {
				m_appName = appName;
			}

			static void setLogPath(const char* logpath);
			static void startLogging();

			static void Close() {
				Logger& logger = getLogger();
				logger.m_logfile.close();
			}

			

		protected:
			bool m_outputOnConsole = true;

			static bool IsConsoleOut(AALTOLogNoticeType level);
			static bool IsNetworkOut(AALTOLogNoticeType level);
			static void makeFile(const std::string& appName);
			const char* levelStr(AALTOLogNoticeType level);
			static bool m_isQuiet;
			static bool m_isSilent;
			static bool m_isOpen;

			static std::string m_filename;
			static const std::string m_Path;
			static std::ofstream m_logfile;
			static AALTOLogNoticeType m_logLevel;
			static AALTOLogNoticeType m_consoleLevel;
			static AALTOLogNoticeType m_networkLevel;

			static std::string m_logpath;
			static int m_size;
			static int m_cursize;
			static int m_lastCode;
			static AALTOLogNoticeType m_lastLevel;
			static std::string m_lastMessage;
			static std::string m_currentMessage;
			static std::string m_appName;

			static bool setLevel(AALTOLogNoticeType& level, const std::string& s);
			static AALTOLogNoticeType toLevel(const std::string& s);
			inline bool IsLogOut(AALTOLogNoticeType level);
			static const char* toString(AALTOLogNoticeType level);
			static AALTOLogNoticeType messageLevel(std::string message);
			bool setHighestLevel(AALTOLogNoticeType level);
			std::string toLogString();
			bool IsFatal() { return m_isFatal; };
			int m_fatalCode;
			std::string m_fatalMessage;
	};

} // namespace aaltoLog



