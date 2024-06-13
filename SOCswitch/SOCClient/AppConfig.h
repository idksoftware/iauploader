#pragma once

#include <string>
#include <memory>

#include "Application/ConfigReader.h"


using namespace Application;


#define CONFIG_PATH						"/config"
#define GENERAL_LABEL			"general"
#define LOGGING_LABEL			"logging"
#define NETWORK_LABEL			"network"


#define QUIET_LABEL						"Quiet"
#define SILENT_LABEL					"Silent"
#define LOG_LEVEL_LABEL					"LogLevel"
#define CONSOLE_LEVEL_LABEL				"ConsoleLevel"
#define USER_LABEL						"User"
#define PASSWORD_LABEL					"Password"
#define FTP_IPADDESS_LABEL				"IPAddress"
#define FTP_ROOT_FOLDER_LABEL			"RootFolder"

#define STRICMP _stricmp

	/**
	* @brief This is the main configuration object. It is used for the
	* primary configuration options.
	*
	*/
	class AppConfig
	{

	private:
		friend class AppOptions;
		friend class SOCConfiguration;
		//friend class SIAARCConfig;

		static std::unique_ptr<AppConfig> m_this;

		static bool m_verbose;					//< -v --Verbose
		static bool m_quiet;					//
		static bool m_silent;					// 
		
		static std::string m_logLevel;			// Logging Level
		static std::string m_consoleLevel;		// Logging level to console
		

		/// Log file path
		static std::string m_logPath;
		/// home path. This is the root path all default paths are made. 
		static std::string m_homePath;
		/// System Path
		static std::string m_systemPath;
			
		static std::string m_configPath;

		static std::string m_FTPUser;
		static std::string m_FTPPassword;
		static std::string m_FTPIPAddress;
		static std::string m_FTPRootFolder;
		
		void fileBasedValues(const char* home, const char* tmpPath);
		std::string toString();
		static bool locations(const char* path);

	public:
		AppConfig() noexcept {}
		~AppConfig() {}
		void settup();

		static bool setDefaultLocations();

		static AppConfig& get() {
			static AppConfig    instance; // Guaranteed to be destroyed.
			// Instantiated on first use.
			return instance;
		}
		/// @brief Gets log file path
		const char* getLogPath();
		/// System path
		const char* getSystemPath();
		
		const char* getConfigPath();
		/// @brief Gets home path. This is the root path all default paths are made.
		const char* getHomePath();
		void setHomePath(const char* homePath);
		
		const char* getLogLevel();
		const char* getConsoleLevel();

		const char* getFTPUser();
		const char* getFTPPassword();
		const char* getFTPIPAddress();
		const char* getFTPRootFolder();

	
		void setLogLevel(const char* logLevel);
		void setQuiet(bool quiet);
		void setSilent(bool silent);
		void setVerbose(bool verbose);

		bool isSilent() const;
		bool isQuiet() const;
		bool isVerbose() const;

	};

	class SOCConfiguration : public AppConfigBase {

	public:
		SOCConfiguration() = default;
		~SOCConfiguration() = default;

		bool setFTPSync(const char* name, std::string& value, std::string& defaultValue) {
			return setConfigBlock(name, value, defaultValue, FTPSYNC_BLOCK);
		}

		bool setGeneral(const char* name, std::string& value, std::string& defaultValue) {
			return setConfigBlock(name, value, defaultValue, GENERAL_BLOCK);
		}

		bool setLogging(const char* name, std::string& value, std::string& defaultValue) {
			return setConfigBlock(name, value, defaultValue, LOGGING_BLOCK);
		}
		void fileBasedValues(const char* home);

		void setHomePath(const char* homePath);
	};