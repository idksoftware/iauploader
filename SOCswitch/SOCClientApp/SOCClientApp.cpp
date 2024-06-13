
#define _CRT_SECURE_NO_WARNINGS

#include <csignal>
#include <thread>
#include <future>
#include <chrono>
#include <time.h>
#include "SOCClientApp.h"
#include "SOCClient/AppOptions.h"
#include "SOCClientArgvParser.h"
#include "SOCClient/AppConfig.h"
#include "Application/HomePaths.h"
#include "Application/ReturnCode.h"
#include "Application/Utils.h"
#include "Application/ConfigReader.h"
#include "Common/UDPInstance.h"
#include "Common/Logger.h"
#include "Common/SyncWorker.h"


#include "SOCClient/SOCClientLibary.h"



using namespace std::chrono_literals;

using namespace Common;

bool SOCApp::initaliseConfig()
{

/*
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
	*/
	if (SOCClientLibary::initalise() == false) {
		return false;
	}

	Logger& logger = Logger::getLogger();

	time_t ttime = time(0);
	tm* local_time = localtime(&ttime);

	std::stringstream s;
	s << std::setw(4) << std::setfill('0') << (local_time->tm_year + 1900) << '/';
	s << std::setw(2) << std::setfill('0') << local_time->tm_mon << '/';
	s << std::setw(2) << std::setfill('0') << local_time->tm_mday << ' ';
	s << std::setw(2) << std::setfill('0') << local_time->tm_hour << ':';
	s << std::setw(2) << std::setfill('0') << local_time->tm_min << ':';
	s << std::setw(2) << std::setfill('0') << local_time->tm_sec;

	logger.log(1234, AALTOLogNoticeType::SUMMARY, "Started application at %s", s.str().c_str());
	return true;
}

void SOCApp::AppEnded()
{
	Logger& logger = Logger::getLogger();

	time_t ttime = time(0);
	tm* local_time = localtime(&ttime);

	std::stringstream s;
	s << std::setw(4) << std::setfill('0') << (local_time->tm_year + 1900) << '/';
	s << std::setw(2) << std::setfill('0') << local_time->tm_mon << '/';
	s << std::setw(2) << std::setfill('0') << local_time->tm_mday << ' ';
	s << std::setw(2) << std::setfill('0') << local_time->tm_hour << ':';
	s << std::setw(2) << std::setfill('0') << local_time->tm_min << ':';
	s << std::setw(2) << std::setfill('0') << local_time->tm_sec;

	logger.log(1234, AALTOLogNoticeType::SUMMARY, "Application ended at %s", s.str().c_str());
}

bool SOCApp::initaliseArgs(int argc, char** argv)
{

	if (m_argvParser->initalise(argc, argv) == false)
	{
		setExitCode(ExitCode::Fatal);
		return false;
	}
	return true;
}

bool SOCApp::doRun()
{
	Logger& logger = Logger::getLogger();
	bool res = false;
	AppOptions& appOptions = AppOptions::get();
	
	if (SOCClientLibary::enableUDPEvents() == false) {
		return false;
	}

	switch (AppCommand::RunCMD) {
	case AppCommand::RunCMD:
		res = RunFTPSync();
		break;
	case AppCommand::VersionCMD:
		printf("Verion 0.0.0.1\n");
		res = true;
		break;
	default:
		res = false;
		logger.log(1234, AALTOLogNoticeType::FATAL, "No operation set");
	}
	
	if (res) {
		AppEnded();
		ReturnCodeObject::setReturn(Success, "Operation Successful");
		return true;
	}
	ReturnCodeObject::setReturn(FatalAppError, logger.getLastMessage());
	return false;
}

SOCApp::SOCApp() : AppBase("SOC App", std::make_shared<SOCArgvParser>())
{
};

bool gStop = false;

void signalHandler(int signum) {
	Logger& logger = Logger::getLogger();

	time_t ttime = time(0);
	tm* local_time = localtime(&ttime);

	std::stringstream s;
	s << std::setw(4) << std::setfill('0') << local_time->tm_year << '/';
	s << std::setw(2) << std::setfill('0') << local_time->tm_mon << '/';
	s << std::setw(2) << std::setfill('0') << local_time->tm_mday << ' ';
	s << std::setw(2) << std::setfill('0') << local_time->tm_hour << ':';
	s << std::setw(2) << std::setfill('0') << local_time->tm_min << ':';
	s << std::setw(2) << std::setfill('0') << local_time->tm_sec;

	logger.log(1234, AALTOLogNoticeType::SUMMARY, "Application sent a teminate signal at %s", s.str().c_str());
	gStop = true;
	//exit(signum);
}

bool SOCApp::RunFTPSync()
{
	Logger& logger = Logger::getLogger();
	AppConfig& config = AppConfig::get();

	if (SOCClientLibary::InitRunFTPSync(config.getFTPUser(), config.getFTPPassword(), config.getFTPIPAddress()) == false) {
		return false;
	}
	if (SOCClientLibary::FTPSyncFolder(config.getFTPRootFolder()) == false) {
		return false;
	}
	std::thread HeartBeat(runHeartBeat);
	std::thread SyncWorker(SOCClientLibary::startFTPSync);
	//if (SyncWorker::Start() == false) {
	//	logger.log(1234, AALTOLogNoticeType::FATAL, "FTP Sync not initalised");
	//	return false;
	//}

	
	while (1) {
		//logger.log(1234, AALTOLogNoticeType::SUMMARY, "Application running");
		std::this_thread::sleep_for((1000ms) * 2);
		if (gStop == true) {
			SOCClientLibary::stopFTPSync();
			break;
		}
	}
	
	/*
	for (int i = 0; i < 10; i++) {
		//logger.log(1234, AALTOLogNoticeType::SUMMARY, "Application running");
		std::this_thread::sleep_for((1000ms) * 2);
		if (gStop == true) {
			SOCClientLibary::stopFTPSync();
			break;
		}
	}
	*/
	
	SOCClientLibary::stopFTPSync();
	HeartBeat.join();
	SyncWorker.join();
	return true;
}

bool SOCApp::runHeartBeat()
{
	
	return true;
}



int main(int argc, char** argv)
{
	std::signal(SIGINT, signalHandler);
	
	WSADATA		wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCApp app;
	ExitCode exitCode = app.RunApp(argc, argv);
	return (static_cast<int>(exitCode));
	
}
