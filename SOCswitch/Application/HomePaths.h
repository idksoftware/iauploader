#pragma once

#include <string>
#include <vector>
#include <memory>

namespace Application {


	enum class HPSource {

	};

	enum class HomePathType {
		Default,	// Default value
		LocalEnv,	// Local Environment set
		SystemEnv,	// System Environment set
		UserOnly,	// user only archive
		AllUsers,	// all users archive
		Unknown
	};

	enum class HPError {
		Ok,					// OK
		initaliseError,
		CannotLocatePath,	// Cannot locate path at default or HOME if set
		NotFound,			// Path not at default and HOME not set 
		Unknown				// Unknown state
	};

	enum class HomePathID {
		HomePath,
		InvalidPath
	};

	class InitailiseHomePaths;

	class HomePathsBase {


	protected:

		bool m_found{ false };	// string found
		bool m_valid{ false };	// in file system
		bool m_changed{ false };	// in file system
		HomePathType m_type{ HomePathType::Unknown };
		HPError m_error{ HPError::Unknown };
		std::string m_path;

		static bool m_first;

		static std::string m_allUsersHomeEnvironmentPath;
		static std::string m_myselfHomeEnvironmentPath;
		static std::string m_allusersHomeDefaultPath;
		static std::string m_myselfHomeDefaultPath;

		static std::string m_homePathEnv;
		static std::string m_relativeRoot;

		bool baseSysInit(std::string& path, const char* relativePath, const char* enviromentPath);
		bool baseUserInit(std::string& path, const char* relativePath, const char* enviromentPath);
		void baseEnableInit(bool& option, bool defaultOption, const char* enviromentOption);

	public:
		HomePathsBase() = default;
		virtual ~HomePathsBase() {};

		HPError error() noexcept {
			return m_error;
		}
		HomePathType type() noexcept {
			return m_type;
		}
		bool isFound() noexcept {	// string found
			return m_found;
		}
		bool isValid() noexcept {	// in file system
			return m_valid;
		}
		bool isChanged() noexcept { // changed by enviroment option 
			return m_changed;
		}

		void setPath(const char* p);
		const std::string& getPath();

		std::string errorStr();

		virtual bool init() = 0;
		virtual bool checkAndMakePath() { return true; }
		virtual HomePathID getID() const = 0;


		static bool loadEnv();

		static std::string getAllUsersHomeEnvironmentPath() { return m_allUsersHomeEnvironmentPath; };
		static std::string getMyselfHomeEnvironmentPath() { return m_myselfHomeEnvironmentPath; };
		static std::string getAllusersHomeDefaultPath() { return m_allusersHomeDefaultPath; };
		static std::string getMyselfHomeDefaultPath() { return m_myselfHomeDefaultPath; };

		static std::string getHomePathEnv() { return m_homePathEnv; };
		static std::string getRelativeRoot() { return m_relativeRoot; };
	};

	class SOCHomePath {
		static std::vector<HomePathsBase*> m_list;

		bool m_found{ false };	// string found
		bool m_valid{ false };	// in file system
		HomePathType m_type{ HomePathType::Unknown };
		HPError m_error{ HPError::Unknown };
		static std::string m_socHomePath;
		void initHomePaths();

		bool setSOCHomePath();

	protected:

		SOCHomePath() noexcept {
			if (setSOCHomePath() == false) {
				return; // m_error set
			}
			HomePathsBase::loadEnv();
			if (m_valid == true) {
				initHomePaths();
				init();
			}
		}

	public:

		~SOCHomePath() = default;

		bool init();
		bool checkAndMakePaths();
		
		HPError error() noexcept {
			return m_error;
		}

		std::string errorStr();

		HomePathType type() noexcept {
			return m_type;
		}
		bool isFound() noexcept {	// string found
			return m_found;
		}
		bool isValid() noexcept {	// in file system
			return m_valid;
		}

		static const std::string& getSOCHomePath();
		
		static SOCHomePath& getObject() {
			static SOCHomePath socHomePath;
			return socHomePath;
		}


	};



	

};