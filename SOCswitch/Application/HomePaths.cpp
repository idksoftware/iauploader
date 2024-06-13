#include "HomePaths.h"
#include <string>
#include <vector>
#include <filesystem>
#include <system_error>
#include "Utils.h"

constexpr auto SOCSWITCH_HOME = "SOCSWITCH_HOME";
constexpr auto DEFAULT_ALLUSER_HOME_PATH = "/AALTO/SOCSwitch";
constexpr auto DEFAULT_LOCAL_HOME_PATH = "/SOCSwitch";

namespace Application {


	bool HomePathsBase::m_first = true;
	std::string HomePathsBase::m_allUsersHomeEnvironmentPath;
	std::string HomePathsBase::m_myselfHomeEnvironmentPath;
	std::string HomePathsBase::m_allusersHomeDefaultPath;
	std::string HomePathsBase::m_myselfHomeDefaultPath;
	std::string HomePathsBase::m_homePathEnv;
	std::string HomePathsBase::m_relativeRoot = "\\SOCSwitch";


	std::vector<HomePathsBase*> SOCHomePath::m_list;
	std::string SOCHomePath::m_socHomePath;


	void SOCHomePath::initHomePaths() {

		/*
		HomePathsBase* path = &(MasterPath::getObject());
		m_list.push_back(path);
		path = &(MasterBackupOnePath::getObject());
		m_list.push_back(path);
		path = &(MasterBackupTwoPath::getObject());
		m_list.push_back(path);
		path = &(DerivativePath::getObject());
		m_list.push_back(path);
		path = &(DerivativeBackupOnePath::getObject());
		m_list.push_back(path);
		path = &(DerivativeBackupTwoPath::getObject());
		m_list.push_back(path);
		path = &(UserspacePath::getObject());
		m_list.push_back(path);
		path = &(WorkspacePath::getObject());
		m_list.push_back(path);
		path = &(PicturePath::getObject());
		m_list.push_back(path);
		path = &(WWWImagePath::getObject());
		m_list.push_back(path);
		path = &(UserMetadataPath::getObject());
		m_list.push_back(path);

		m_primaryPath = m_imgArchiveHome;
		m_primaryPath += "\\pi";
		*/
	}



	const std::string& SOCHomePath::getSOCHomePath() {
		return m_socHomePath;
	}




	bool SOCHomePath::init() {

		bool res = true;
		for (auto i = m_list.begin(); i != m_list.end(); i++) {
			auto hp = *i;
			if (hp->init() == false) {
				m_error = HPError::initaliseError;
				res = false;
			}
		}

		return res;
	}

	bool SOCHomePath::checkAndMakePaths() {

		bool res = true;
		for (auto i = m_list.begin(); i != m_list.end(); i++) {
			auto hp = *i;
			if (hp->checkAndMakePath() == false) {
				m_error = HPError::initaliseError;
				res = false;
			}
		}
		return res;
	}




	bool SOCHomePath::setSOCHomePath()
	{
		// Set Windows Defaults (they can be overridden later)
		std::string allUsersHomeEnvironmentPath = Utils::GetEnv(SOCSWITCH_HOME, true);
		std::string myselfHomeEnvironmentPath = Utils::GetEnv(SOCSWITCH_HOME, false);
		// All Users

		std::string allusersHomeDefaultPath = Utils::GetPOSIXEnv("ProgramData");
		allusersHomeDefaultPath += DEFAULT_ALLUSER_HOME_PATH;
		std::string myselfHomeDefaultPath = Utils::GetPOSIXEnv("LOCALAPPDATA");
		myselfHomeDefaultPath += DEFAULT_LOCAL_HOME_PATH;

		// Looking the HKEY_LOCAL_MACHINE first
		if (allUsersHomeEnvironmentPath.empty() == false) {
			m_type = HomePathType::SystemEnv;	// System Environment set
			m_found = true;
			m_socHomePath = allUsersHomeEnvironmentPath;
		}
		else if (myselfHomeEnvironmentPath.empty() == false) {
			m_type = HomePathType::LocalEnv;
			m_found = true;
			m_socHomePath = myselfHomeEnvironmentPath;
		}
		else if (Utils::DirExists(allusersHomeDefaultPath.c_str()) == true) {
			m_socHomePath = allusersHomeDefaultPath;
			m_type = HomePathType::AllUsers;
			m_found = true;

		}
		else if (Utils::DirExists(myselfHomeDefaultPath.c_str()) == true) {
			m_socHomePath = myselfHomeDefaultPath;
			m_type = HomePathType::UserOnly;
			m_found = true;

		}
		else {
			m_error = HPError::NotFound;
			return false;
		}

		if (m_type == HomePathType::SystemEnv) {
			if (Utils::DirExists(m_socHomePath.c_str()) == false) {
				m_error = HPError::CannotLocatePath;
				return false;
			}

		}
		else if (m_type == HomePathType::LocalEnv) {

			if (Utils::DirExists(m_socHomePath.c_str()) == false) {
				m_error = HPError::CannotLocatePath;
				return false;
			}

		}
		m_valid = true;
		return true;
	}

	std::string HomePathsBase::errorStr()
	{
		std::string errStr;
		switch (m_error) {
		case HPError::Ok:					// OK
			return "Ok no error";
		case HPError::initaliseError:
			return "Initalise error";
		case HPError::CannotLocatePath:	// Cannot locate path at default or HOME if set
			return "Cannot locate path at default or HOME if set";
		case HPError::NotFound:			// Path not at default and HOME not set 
			return "Path not at default and SOCSWITCH_HOME not set";
		case HPError::Unknown:				// Unknown state
		default:
			break;
		}
		return "Unknown error";
	}



	void HomePathsBase::setPath(const char* p)
	{
		if (Utils::DirExists(p) == false) {
			m_valid = false;
		}
		else {
			m_valid = true;
		}
		m_path = p;

	}

	const std::string& HomePathsBase::getPath()
	{
		return m_path;
	}

	bool HomePathsBase::loadEnv() {

		HomePathsBase::m_allUsersHomeEnvironmentPath = Utils::GetEnv(SOCSWITCH_HOME, true);
		HomePathsBase::m_myselfHomeEnvironmentPath = Utils::GetEnv(SOCSWITCH_HOME, false);
		// All Users
		HomePathsBase::m_allusersHomeDefaultPath = Utils::GetPOSIXEnv("ProgramData");
		HomePathsBase::m_myselfHomeDefaultPath = Utils::GetPOSIXEnv("LOCALAPPDATA");


		HomePathsBase::m_homePathEnv = Utils::GetEnv("USERPROFILE", true);

		return true;
	}

	bool HomePathsBase::baseSysInit(std::string& path, const char* relativePath, const char* enviromentPath)
	{

		path = SOCHomePath::getSOCHomePath();
		path += relativePath;
		m_type = HomePathType::Default;
		// Set Windows Defaults (they can be overridden later)
		std::string allUsersHomeEnvironmentPath = Utils::GetEnv(enviromentPath, true);
		std::string myselfHomeEnvironmentPath = Utils::GetEnv(enviromentPath, false);
		// Looking the HKEY_LOCAL_MACHINE first
		if (allUsersHomeEnvironmentPath.empty() == false) {
			m_type = HomePathType::SystemEnv;	// System Environment set
			path = allUsersHomeEnvironmentPath;

		}
		else if (myselfHomeEnvironmentPath.empty() == false) {
			m_type = HomePathType::LocalEnv;
			path = myselfHomeEnvironmentPath;
		}
		m_found = true;
		if (Utils::DirExists(path.c_str()) == false) {
			m_error = HPError::CannotLocatePath;
			return false;

		}

		m_valid = true;
		return true;
	}

	bool HomePathsBase::baseUserInit(std::string& path, const char* relativePath, const char* enviromentPath)
	{


		std::string homePath = m_homePathEnv;

		m_found = true;
		// Set Windows Defaults (they can be overridden later)
		std::string allUsersHomeEnvironmentPath = Utils::GetEnv(enviromentPath, true);
		std::string myselfHomeEnvironmentPath = Utils::GetEnv(enviromentPath, false);

		path = homePath + m_relativeRoot + relativePath;
		m_type = HomePathType::Default;
		// Looking the HKEY_LOCAL_MACHINE first
		if (allUsersHomeEnvironmentPath.empty() == false) {
			m_type = HomePathType::SystemEnv;	// System Environment set
			path = allUsersHomeEnvironmentPath;
			m_changed = true;
		}
		else if (myselfHomeEnvironmentPath.empty() == false) {
			m_type = HomePathType::LocalEnv;
			path = myselfHomeEnvironmentPath;
			m_changed = true;
		}
		/*
		else if (SAUtils::DirExists(path.c_str()) == false) {
			m_error = HPError::CannotLocatePath;
			return false;
		}
		*/
		m_valid = true;
		return true;
	}

	void HomePathsBase::baseEnableInit(bool& option, bool defaultOption, const char* enviromentOption)
	{

		// Set Windows Defaults (they can be overridden later)
		std::string allUsersHomeEnvironmentOption = Utils::GetEnv(enviromentOption, true);
		std::string myselfHomeEnvironmentOption = Utils::GetEnv(enviromentOption, false);

		option = defaultOption;

		// Looking the HKEY_LOCAL_MACHINE first
		if (allUsersHomeEnvironmentOption.empty() == false) {

			BoolOption opt = Utils::isTrueFalse(allUsersHomeEnvironmentOption);
			option = (BoolOption::True == opt);
		}
		else if (myselfHomeEnvironmentOption.empty() == false) {

			BoolOption opt = Utils::isTrueFalse(myselfHomeEnvironmentOption);
			option = (BoolOption::True == opt);
		}
	}

}