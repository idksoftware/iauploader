#include "SOCClientLibary.h"
#include <csignal>
#include <thread>
#include <future>
#include <chrono>
#include <time.h>

#include "SOCClient/AppOptions.h"
#include "SOCClient/AppConfig.h"
#include "Application/HomePaths.h"
#include "Application/ReturnCode.h"
#include "Application/Utils.h"
#include "Application/ConfigReader.h"
#include "Common/UDPInstance.h"
#include "Common/Logger.h"
#include "Common/UDPOut.h"
#include "Common/SyncWorker.h"
#include "Common/UDPInstance.h"

using namespace Common;
using namespace std::chrono_literals;

std::string  SOCClientLibary::m_configPath;
std::string  SOCClientLibary::m_homePath;

bool SOCClientLibary::initalise()
{
	AppOptions& appOptions = AppOptions::get();
	SOCConfiguration config;


	SOCHomePath& SOCHomePath = SOCHomePath::getObject();
	std::string homePath = SOCHomePath::getSOCHomePath().c_str();

	if (SOCHomePath.isValid() == false)
	{

		if (homePath.length() == 0) {
			ReturnCodeObject::setReturn(4004, "SOCSWITCH_HOME location not set.");
		}
		else {
			ReturnCodeObject::setReturn(4005, "SOCSWITCH_HOME not found at location: %s.\n", homePath.c_str());
		}
		return false;
	}

	std::string configfile = homePath + "/config/" + "config.dat";
	std::string configPath = homePath + "/config";
	if (Utils::DirExists(configPath.c_str()) == false)
	{
		ReturnCodeObject::setReturn(4004, "Error configuration path not found: \"%s\".", configPath.c_str());
		return false;
	}
	if (Utils::FileExists(configfile.c_str()) == false)
	{
		ReturnCodeObject::setReturn(4004, "Error configuration file not found: \"%s\".", configfile.c_str());
		return false;
	}

	setConfigPath(configPath.c_str());
	AppConfigReader configReader;
	configReader.setNoLogging();
	if (configReader.read(configfile.c_str(), config) == false)
	{
		ReturnCodeObject::setReturn(4004, "Error found at line %d in the configuration file.",
			configReader.getCurrentLineNumber());
		return false;
	}
	config.fileBasedValues(homePath.c_str());
	std::string logPath = homePath + "/logs";
	Logger& logger = Logger::getLogger();
	Logger::setAppName("SocClient");
	AppConfig appConfig = AppConfig::get();

	Logger::setLogLevel(appConfig.getLogLevel());
	Logger::setConsoleLevel(appConfig.getConsoleLevel());
	Logger::setNetworkLevel(appConfig.getConsoleLevel());
	Logger::setSilent(appConfig.isSilent());
	Logger::setQuiet(appConfig.isQuiet());

	Logger::setLogPath(logPath.c_str());
	try {
		Logger::startLogging();
	}
	catch (std::exception e) {
		logger.log(1234, AALTOLogNoticeType::FATAL, "Failed to start logging");
		return false;


	}
	WSADATA		wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
    return true;
}

bool SOCClientLibary::InitRunFTPSync(const char* u, const char* pw, const char* ip)
{
	Logger& logger = Logger::getLogger();
	AppConfig& config = AppConfig::get();

	std::string user = config.getFTPUser();
	std::string password = config.getFTPPassword();
	std::string ipAddress = config.getFTPIPAddress();
	std::string rootFolder = config.getFTPRootFolder();
	int periodSeconds = 2;

	if (u != nullptr) {
		user = u;
	}
	if (pw != nullptr) {
		password = pw;
	}
	if (ip != nullptr) {
		ipAddress = ip;
	}
	
	SyncWorker::init(user.c_str(), password.c_str(), ipAddress.c_str());

	return true;
}

bool SOCClientLibary::FTPSyncFolder(const char* folder)
{
	return SyncWorker::addFolder(folder);
}

bool SOCClientLibary::startFTPSync()
{
	
	return SyncWorker::Start();
}

bool SOCClientLibary::stopFTPSync()
{
	SyncWorker::Stop();
	return true;
}

bool SOCClientLibary::enableUDPEvents()
{
	return UDPOut::enableUDPOutput(560);
}

bool SOCClientLibary::shutdown()
{
	WSACleanup();
	return true;
}
