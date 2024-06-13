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

#define _CRT_SECURE_NO_WARNINGS


#include <iostream>
#include <filesystem>
#include <sstream>
#include <cstdarg>
#include <iostream>

#include "Logger.h"
#include "UDPOut.h"

using FileList = std::vector<std::string>;
using FileList_Ptr = std::unique_ptr<FileList>;

namespace Common

{

	std::string Logger::m_filename = "Log.txt";
	std::ofstream Logger::m_logfile;
	AALTOLogNoticeType Logger::m_logLevel = AALTOLogNoticeType::SUMMARY;
	AALTOLogNoticeType Logger::m_consoleLevel = AALTOLogNoticeType::SUMMARY;
	AALTOLogNoticeType Logger::m_networkLevel = AALTOLogNoticeType::SUMMARY;
	AALTOLogNoticeType Logger::m_lastLevel = AALTOLogNoticeType::UNKNOWN;

	std::string Logger::m_logpath;
	int Logger::m_size = 10000;
	int Logger::m_cursize = 0;
	bool Logger::m_isQuiet = false;
	bool Logger::m_isSilent = false;
	bool Logger::m_isOpen = false;
	int Logger::m_lastCode;
	std::string Logger::m_lastMessage;
	std::string Logger::m_currentMessage;
	std::string Logger::m_appName;

	class LogName {
		static std::string fileTemplate(const char* preName);
		int checkLogSize(const char* logPath, uint64_t maxSize);
		std::string m_filename;
	public:
		/// Constructor
		LogName() = default;
		/// Destructor
		virtual ~LogName() = default;
		/// @brief Makes the log file name base on size and date of previous log files in the directory
		/// passed.
		/// @param logPath - path to look for previous logs.
		/// @param preName - Prefixed name given to the log file.
		/// @param ext - Extension given to the log file.
		/// @param maxSize - max size of current log file before new log name is created.
		std::string makeName(const char* logPath, const char* preName, const char* ext, int maxSize);
		static std::string mkfilename(const char* folder, const char* preName, const char* ext);
		/// Date string from filename
		/// Make date string
		//static std::string dateString(CDate& date);
		/// Make todays date string
		static std::string dateStringToday();
		std::string& getFilename() {
			return m_filename;
		}
		static const int ALWAYS_CREATE = -1;
	};



	// trim from start (in place)
	static inline void ltrim(std::string& s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
			return !std::isspace(ch);
			}));
	}

	// trim from end (in place)
	static inline void rtrim(std::string& s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
			return !std::isspace(ch);
			}).base(), s.end());
	}

	// trim from both ends (in place)
	static inline void trim(std::string& s) {
		ltrim(s);
		rtrim(s);
	}

	// trim from start (copying)
	static inline std::string ltrim_copy(std::string s) {
		ltrim(s);
		return s;
	}

	// trim from end (copying)
	static inline std::string rtrim_copy(std::string s) {
		rtrim(s);
		return s;
	}

	// trim from both ends (copying)
	static inline std::string trim_copy(std::string s) {
		trim(s);
		return s;
	}

	bool AALTOLogEntry::setMsgType(AALTOLogNoticeType noticeType)
	{
		//if (noticeType < AALTOLogNoticeType::LOG_FATAL) return (false);
		//if (noticeType > AALTOLogNoticeType::LOG_INFO) return (false);

		m_noticeType = noticeType;
		return (true);
	}

	AALTOLogNoticeType AALTOLogEntry::getMsgType() const
	{
		return (m_noticeType);
	}

	bool AALTOLogEntry::setMsg(const std::string msg)
	{
		if (msg.empty()) return (false);
		m_message = msg;
		return (true);
	}

	const std::string& AALTOLogEntry::getMsg() const
	{
		return (m_message);
	}



	const bool Logger::enableConsole(const bool sendToConsole)
	{
		m_outputOnConsole = sendToConsole;
		return (true);
	}



	/*static*/Logger& Logger::getLogger()
	{
		static Logger invalid;
		invalid.enableConsole(true);
		return invalid;
	}


	void Logger::makeFile(const std::string& appName) {
		LogName logName;
		if (m_isOpen == false) {
			return;
		}
		m_filename = logName.makeName(m_logpath.c_str(), appName.c_str(), "log", 256);
		std::string fullpath = m_logpath;
		fullpath += '/';
		fullpath += m_filename;
		m_logfile.open(fullpath.c_str(), std::ios::out | std::ios::app);
		if (m_logfile.is_open() == false) {  // changed to true for testing
			//printf("cannot open log file %s", fullpath.c_str());
			throw std::exception("Cannot open log file");
		}
		//printf("Opened log file %s", fullpath.c_str());
	}

	AALTOLogNoticeType Logger::messageLevel(std::string message) {
		size_t spos = message.find("[");
		size_t epos = message.find("]", spos + 1);
		std::string levelStr = message.substr(spos + 1, (epos - spos) - 1);
		trim(levelStr);
		return toLevel(levelStr);

	}

	void Logger::setLogPath(const char* logpath) {
		m_logpath = logpath;
	}

	void Logger::startLogging() {
		m_isOpen = true;
		makeFile(m_appName.c_str());
	}

	inline bool Logger::IsLogOut(AALTOLogNoticeType level) {
		if (level >= m_logLevel) {
			return true;
		}
		return false;
	}

	inline bool Logger::IsConsoleOut(AALTOLogNoticeType level) {
		if (level >= m_consoleLevel) {
			return true;
		}
		return false;
	}

	inline bool Logger::IsNetworkOut(AALTOLogNoticeType level) {
		if (level >= m_consoleLevel) {
			return true;
		}
		return false;
	}

	const char* Logger::toString(AALTOLogNoticeType level) {
		switch (level) {
		case AALTOLogNoticeType::TRACE: return    "TRACE";
		case AALTOLogNoticeType::FINE: return     "FINE";
		case AALTOLogNoticeType::INFO: return     "INFO";
		case AALTOLogNoticeType::STATUS: return   "STATUS";
		case AALTOLogNoticeType::SUMMARY: return  "SUMMARY";
		case AALTOLogNoticeType::WARNING: return  "WARNING";
		case AALTOLogNoticeType::ERR: return      "ERROR";
		case AALTOLogNoticeType::FATAL: return    "FATAL";
		case AALTOLogNoticeType::UNKNOWN: return  "FATAL";
		}
		return "FATAL";
	}

	const char* Logger::levelStr(AALTOLogNoticeType level) {
		switch (level) {
		case AALTOLogNoticeType::TRACE: return    "      TRACE";
		case AALTOLogNoticeType::FINE: return     "      FINE ";
		case AALTOLogNoticeType::INFO: return     "     INFO  ";
		case AALTOLogNoticeType::STATUS: return   "    STATUS ";
		case AALTOLogNoticeType::SUMMARY: return  "   SUMMARY ";
		case AALTOLogNoticeType::WARNING: return  "  WARNING  ";
		case AALTOLogNoticeType::ERR: return      " ERROR     ";
		case AALTOLogNoticeType::FATAL: return    "FATAL      ";
		case AALTOLogNoticeType::UNKNOWN: return  "FATAL      ";
		}
		return "FATAL";
	}

	bool Logger::setLevel(AALTOLogNoticeType& level, const std::string& s)
	{
		level = toLevel(s);
		return true;
	}

	AALTOLogNoticeType Logger::toLevel(const std::string& s) {
		AALTOLogNoticeType level;
		if (s.compare("TRACE") == 0) {
			level = AALTOLogNoticeType::TRACE;
		}
		else if (s.compare("FINE") == 0) {
			level = AALTOLogNoticeType::FINE;
		}
		else if (s.compare("INFO") == 0) {
			level = AALTOLogNoticeType::INFO;
		}
		else if (s.compare("STATUS") == 0) {
			level = AALTOLogNoticeType::STATUS;
		}
		else if (s.compare("SUMMARY") == 0) {
			level = AALTOLogNoticeType::SUMMARY;
		}
		else if (s.compare("WARNING") == 0) {
			level = AALTOLogNoticeType::WARNING;
		}
		else if (s.compare("ERROR") == 0) {
			level = AALTOLogNoticeType::ERR;
		}
		else if (s.compare("FATAL") == 0) {
			level = AALTOLogNoticeType::FATAL;
		}
		else {
			level = AALTOLogNoticeType::UNKNOWN;
		}
		return level;
	}

	void Logger::log(int code, AALTOLogNoticeType type, const char* format, ...)
	{
		std::string message;
		try {
			size_t final_n, n = (strlen(format) * 2); // Reserve two times as much as the length of the fmt_str //
			std::string str;
			std::unique_ptr<char[]> formatted;
			va_list ap;
			while (1) {
				//formatted.reset(new char[n]); // Wrap the plain char array into the unique_ptr
				auto formatted = std::make_unique<char[]>(n);
				//strcpy(formatted.get(), format);
				va_start(ap, format);
				final_n = vsnprintf(formatted.get(), n, format, ap);
				va_end(ap);
				if (final_n < 0 || final_n >= n) {
					n += abs((long)(final_n - n + 1));
				}
				else {
					message = formatted.get();
					break;
				}
			}

		}
		catch (const std::exception e) {
			printf("logger crashed parsing message");
			exit(-1);
		}

		log(code, type, message);
	}

	int count = 0;
	time_t last;

	std::string Logger::toLogString() {
		time_t ttime = time(0);
		tm* local_time = localtime(&ttime);

		std::stringstream s;
		s << std::setw(4) << std::setfill('0') << local_time->tm_year << '.';
		s << std::setw(2) << std::setfill('0') << local_time->tm_mon << '.';
		s << std::setw(2) << std::setfill('0') << local_time->tm_mday << ' ';
		s << std::setw(2) << std::setfill('0') << local_time->tm_hour << '.';
		s << std::setw(2) << std::setfill('0') << local_time->tm_min << '.';
		s << std::setw(2) << std::setfill('0') << local_time->tm_sec;
		return std::string(s.str());

	}

	void Logger::log(int code, AALTOLogNoticeType type, const std::string& message)
	{
		if (m_size < m_cursize) {
			m_logfile.close();
			makeFile(m_appName);
			m_cursize = 0;
		}

		time_t ttime = time(0);
		tm* date = localtime(&ttime);

		//ExifDateTime date;
		//date.now();
		if (last == ttime) {
			count++;
		}
		else {
			count = 1;
		}
		last = ttime;
		//std::string message;

		try {

			if (IsLogOut(type)) {
				m_lastMessage = message;
				m_lastCode = code;
				std::stringstream logstr;
				logstr << "\n" << std::setfill('0') << std::setw(6) << code << ": " << toLogString() << '.' << count << "\t";
				logstr << '[' << levelStr(type) << "]\t";
				logstr << message;
				if (m_isOpen) {
					m_logfile << logstr.str();
				}
				
				m_cursize++;
			}
			if (m_isSilent == false) {
				if (IsConsoleOut(type)) {
					if (!m_isQuiet) { // if isQuiet is false then send out the log message
						std::cout << '[' << levelStr(type) << "]\t";
						std::cout << message << std::endl;

					}
					
				}
			}

			if (UDPOut::isEnabled()) {
				if (IsNetworkOut(type)) {
					std::stringstream strudp;
					strudp << std::setfill('0') << std::setw(6) << code << ":" << message;
					const std::string udpMessage = strudp.str();
					UDPOut::out(udpMessage.c_str());
				}
			}

		}
		catch (const std::exception e) {
			printf("logger crashed");
			exit(-1);
		}

		if (type == AALTOLogNoticeType::FATAL) {
			m_isFatal = true;
		}
	}

	/*
	LogMessage& LogMessage::operator<<(bool value) {
		std::string s = (value)?"true":"false";
		setMessage(s);
		return *this;
	}
	*/
	LogMessage& LogMessage::operator<<(const char *value) {
		std::string s = value;
		setMessage(s);
		return *this;
	}

	LogMessage& LogMessage::operator<<(std::string& value)
	{
		setMessage(value);
		return *this;
	}

	LogMessage& LogMessage::operator<<(long value) {
		std::string s = std::to_string(value);
		setMessage(s);
		return *this;
	}

	LogMessage& LogMessage::operator<<(unsigned long value) {
		std::string s = std::to_string(value);
		setMessage(s);
		return *this;
	}

	LogMessage& LogMessage::operator<<(long long value) {
		std::string s = std::to_string(value);
		setMessage(s);
		return *this;
	}

	LogMessage& LogMessage::operator<<(unsigned long long value) {
		std::string s = std::to_string(value);
		setMessage(s);
		return *this;
	}

	LogMessage& LogMessage::operator<<(double value) {
		std::string s = std::to_string(value);
		setMessage(s);
		return *this;
	}

	LogMessage& LogMessage::operator<<(long double value) {
		std::string s = std::to_string(value);
		setMessage(s);
		return *this;
	}

	LogMessage& LogMessage::operator<<(short value) {
		std::string s = std::to_string(value);
		setMessage(s);
		return *this;
	}

	LogMessage& LogMessage::operator<<(int value) {
		std::string s = std::to_string(value);
		setMessage(s);
		return *this;
	}

	LogMessage& LogMessage::operator<<(unsigned short value) {
		std::string s = std::to_string(value);
		setMessage(s);
		return *this;
	}

	LogMessage& LogMessage::operator<<(unsigned int value) {
		std::string s = std::to_string(value);
		setMessage(s);
		return *this;
	}

	LogMessage& LogMessage::operator<<(float value) {
		std::string s = std::to_string(value);
		setMessage(s);
		return *this;
	}



	LogMessage& Logger::log(int code, AALTOLogNoticeType type)
	{
		m_logMessage.m_code = code;
		m_logMessage.m_type = type;
		return m_logMessage;
	}


	Logger& Logger::operator << (LogMessage& message)
	{
		if (!message.message.empty()) {
			log(message.m_code, message.m_type, message.message);
			message.clear();
		}
		/*
		if (message.m_type != AALTOLogNoticeType::UNKNOWN) {
			log(message.m_code, message.m_type, message.message);
			message.m_type = AALTOLogNoticeType::UNKNOWN;
			message.clear();
		}
		*/
		return *this;
	}

	/// <summary>
	/// LogName
	/// </summary>
	/// 


	int LogName::checkLogSize(const char* logPath, uint64_t maxSize) {
		uintmax_t size = 0;
		std::error_code ec;
		size = std::filesystem::file_size(logPath, ec);
		if (!ec) {
			return -1;
		}
		if (size > maxSize) {
			return 0;
		}
		return 1;
	}
	std::string LogName::fileTemplate(const char* preName) {
		//CDate& date = CDate::timeNow();
		time_t ttime = time(0);
		tm* local_time = localtime(&ttime);

		std::string fileMatch;
		if (preName != 0) {
			fileMatch += preName;
		}
		std::stringstream s;
		s << local_time->tm_year << std::setw(2) << std::setfill('0') << local_time->tm_mon
			<< std::setw(2) << std::setfill('0') << local_time->tm_mday;
		fileMatch += s.str();
		return fileMatch;
	}
	std::string LogName::makeName(const char* logPath, const char* preName, const char* ext, int maxSize) {

		std::string fileMatch = fileTemplate(preName);
		bool found = false;
		int version = 0;
		std::string filenameItem;

		FileList_Ptr fileList = std::make_unique<FileList>();
		for (auto const& dir_entry : std::filesystem::directory_iterator{ logPath })
		{
			std::filesystem::path p = dir_entry.path();
			std::filesystem::path f = p.filename();
			std::string fileItem = f.string();
			fileList->emplace_back(fileItem);
		}
		if (fileList == nullptr || fileList->empty() == false) {

			for (std::vector<std::string>::iterator i = fileList->begin(); i != fileList->end(); i++) {
				filenameItem = *i;	
				std::filesystem::path p = filenameItem;
				std::string tmpExt = p.filename().string();
			
				if (tmpExt.compare(ext) != 0) {
					continue;
				}

				p = filenameItem;
				std::filesystem::path f = p.filename();
				std::string tmpFileName = f.replace_extension().string();
				std::string nameDate = tmpFileName.substr(0, fileMatch.length());

				if (nameDate.compare(fileMatch) != 0) {
					continue;
				}
				std::string fileNumber = tmpFileName.substr(fileMatch.length(), tmpFileName.length());
				fileNumber = fileNumber.substr(1, fileNumber.length());
				int thisVer = atoi(fileNumber.c_str());
				if (thisVer > version) {
					version = thisVer;
				}
				found = true;
			}
		}

		std::string result;
		std::stringstream s;
		if (found == false) {

			s << fileMatch << "_0001." << ext;

		}
		else {

			s << fileMatch << '_' << std::setw(4) << std::setfill('0') << version << '.' << ext;
			std::string fullpath = logPath;
			fullpath += '/';
			fullpath += s.str().c_str();

			if (maxSize != ALWAYS_CREATE) { // always create new filename.
				int res = checkLogSize(fullpath.c_str(), maxSize * 1024);
				if (res == 1) {
					// less than max size
					result = s.str();
					return result;
				}
			}
			s.str(std::string());
			version++;
			s << fileMatch << '_' << std::setw(4) << std::setfill('0') << version << '.' << ext;
		}
		result = s.str();
		return result;
	}

	// todays date string
	std::string LogName::dateStringToday() {
		time_t ttime = time(0);
		tm* local_time = localtime(&ttime);

		std::stringstream s;
		s << local_time->tm_year << std::setw(2) << std::setfill('0') << local_time->tm_mon
			<< std::setw(2) << std::setfill('0') << local_time->tm_mday;

		return s.str();
	}

	std::string LogName::mkfilename(const char* folder, const char* preName, const char* ext)
	{
		std::string fileMatch = fileTemplate(preName);
		bool found = false;
		int version = 0;
		std::string filenameItem;
		//FileList_Ptr filelist = SAUtils::getFiles(folder);

		FileList_Ptr fileList = std::make_unique<FileList>();
		for (auto const& dir_entry : std::filesystem::directory_iterator{ folder })
		{
			std::filesystem::path p = dir_entry.path();
			std::filesystem::path f = p.filename();
			std::string fileItem = f.string();
			fileList->emplace_back(fileItem);
		}

		if (fileList == nullptr || fileList->empty() == false) {

			for (std::vector<std::string>::iterator i = fileList->begin(); i != fileList->end(); i++) {
				filenameItem = *i;
			
				std::filesystem::path p = filenameItem;
				std::string tmpExt = p.filename().string();
				
				if (tmpExt.compare(ext) != 0) {
					continue;
				}

				p = filenameItem;
				std::filesystem::path f = p.filename();
				std::string tmpFileName = f.replace_extension().string();
				std::string nameDate = tmpFileName.substr(0, fileMatch.length());
				//printf("nameDate \"%s\"\n", nameDate.c_str());
				if (nameDate.compare(fileMatch) != 0) {
					continue;
				}
				std::string fileNumber = tmpFileName.substr(fileMatch.length(), tmpFileName.length());
				fileNumber = fileNumber.substr(1, fileNumber.length());
				int thisVer = atoi(fileNumber.c_str());
				if (thisVer > version) {
					version = thisVer;
				}

				//printf("fileNumber \"%s\"\n", fileNumber.c_str());

				found = true;
			}
		}
		std::string result;
		std::stringstream s;
		if (found == false) {

			s << fileMatch << "_0001." << ext;

		}
		else {

			s << fileMatch << '_' << std::setw(4) << std::setfill('0') << version << '.' << ext;
			std::string fullpath = folder;
			fullpath += '/';
			fullpath += s.str().c_str();
			s.str(std::string());
			version++;
			s << fileMatch << '_' << std::setw(4) << std::setfill('0') << version << '.' << ext;
		}
		result = s.str();
		return result;
	}


} //namespace aaltoLog

