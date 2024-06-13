#include "AppConfig.h"
#include <sstream>
#include <iomanip>




	std::unique_ptr<AppConfig> m_this = nullptr;

	bool AppConfig::m_verbose = false;
	bool AppConfig::m_quiet = true;
	bool AppConfig::m_silent = false;
	std::string AppConfig::m_logLevel = "SUMMARY";
	std::string AppConfig::m_consoleLevel;
	std::string AppConfig::m_configPath;
	std::string AppConfig::m_logPath;
	std::string AppConfig::m_homePath;
	std::string AppConfig::m_systemPath;
	std::string AppConfig::m_FTPUser;
	std::string AppConfig::m_FTPPassword;
	std::string AppConfig::m_FTPIPAddress;
	std::string AppConfig::m_FTPRootFolder;
	
	/*
		Default paths based on UserDrive and UserHome

	*/


	void AppConfig::settup() {
		
		getLogPath();
		getConfigPath();
		getHomePath();
		isQuiet();
		isVerbose();
		getLogLevel();
	}

	void SOCConfiguration::setHomePath(const char* homePath) {
		AppConfig::m_homePath = homePath;
		
		AppConfig::m_configPath = homePath;
		AppConfig::m_configPath += CONFIG_PATH;
	}


	void SOCConfiguration::fileBasedValues(const char* home) {
		
		// Home Path (The path to this file will be based on the home path)
		std::string homePath = home;
		setHomePath(home);
		
		// General	
		std::string quiet = "false";
		setGeneral(QUIET_LABEL, quiet, quiet);
		AppConfig::m_quiet = (STRICMP(quiet.c_str(), "true") == 0);

		std::string silent = "false";
		setGeneral(SILENT_LABEL, silent, silent);
		AppConfig::m_silent = (STRICMP(silent.c_str(), "true") == 0);

		std::string logLevel = "STATUS";
		setLogging(LOG_LEVEL_LABEL, logLevel, logLevel);
		AppConfig::m_logLevel = logLevel;

		std::string consoleLevel = "STATUS";
		setLogging(CONSOLE_LEVEL_LABEL, consoleLevel, consoleLevel);
		AppConfig::m_consoleLevel = consoleLevel;
	
		std::string user = "iain";
		setLogging(USER_LABEL, user, user);
		AppConfig::m_FTPUser = user;

		std::string password = "qwerty";
		setLogging(PASSWORD_LABEL, password, password);
		AppConfig::m_FTPPassword = password;

		std::string ftpIPAddress = "127.0.0.1";
		setLogging(FTP_IPADDESS_LABEL, ftpIPAddress, ftpIPAddress);
		AppConfig::m_FTPIPAddress = ftpIPAddress;

		std::string ftpRootFolder = "c:\\Temp";
		setLogging(FTP_ROOT_FOLDER_LABEL, ftpRootFolder, ftpRootFolder);
		AppConfig::m_FTPRootFolder = ftpRootFolder;


	}


	void AppConfig::setHomePath(const char* homePath) {
		AppConfig::m_homePath = homePath;
		//setPathToHome(AppConfig::m_homePath);
		AppConfig::m_configPath = homePath;
		//AppConfig::m_configPath += CONFIG_PATH;
	}

	const char* AppConfig::getHomePath() {

		return m_homePath.c_str();

	}

	
	void AppConfig::setLogLevel(const char* logLevel) {
		AppConfig::m_logLevel = logLevel;
	}

	const char* AppConfig::getFTPIPAddress()
	{
		return m_FTPIPAddress.c_str();
	}

	

	void AppConfig::setQuiet(bool quiet) {
		AppConfig::m_quiet = quiet;
	}



	void AppConfig::setSilent(bool silent) {
		AppConfig::m_silent = silent;
	}
	void AppConfig::setVerbose(bool verbose) {
		AppConfig::m_verbose = verbose;
	}

	

	const char* AppConfig::getLogPath() {
		return m_logPath.c_str();
	}

	const char* AppConfig::getLogLevel() {
		return m_logLevel.c_str();
	}

	const char* AppConfig::getConsoleLevel() {
		return m_consoleLevel.c_str();
	}

	const char* AppConfig::getFTPUser()
	{
		return m_FTPUser.c_str();
	}

	const char* AppConfig::getFTPPassword()
	{
		return m_FTPPassword.c_str();
	}

	const char* AppConfig::getFTPRootFolder()
	{
		return m_FTPRootFolder.c_str();
	}

	const char* AppConfig::getConfigPath() {

		return 	m_configPath.c_str();
	}

	


	bool AppConfig::isQuiet() const {
		return m_quiet;
	}

	bool AppConfig::isSilent() const {
		return m_silent;
	}

	bool AppConfig::isVerbose() const {
		return m_verbose;
	}



	


	std::string AppConfig::toString() {
		/// @brief Gets the source path.
		///const char *getSourcePath();

		/// @brief Gets the archive path.
		/// user definable
		std::stringstream str;
		str << "Configuration" << '\n';
		str << "=============" << '\n';
		
		/// @brief Gets log file path
		/*
		str << "Log path:                " << getLogPath() << '\n';
		
		}
		str << "Is quiet:                " << (isQuiet() ? "true" : "false") << '\n';
		str << "Is verbose:              " << (isVerbose() ? "true" : "false") << '\n';
		str << "Log level:               " << getLogLevel() << '\n';
		*/
		return str.str();
	}



	bool AppConfig::setDefaultLocations() {

		//return locations(ImgArchiveHome::getImgArchiveHome().c_str());
		return true;
	}

	bool AppConfig::locations(const char* home) {

		std::string homePath = home;
		
		return false;
	}



