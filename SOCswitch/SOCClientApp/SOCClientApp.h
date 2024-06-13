#pragma once

#include <string>
#include "Application/AppBase.h"
#include "SOCClient/AppOptions.h"

using namespace Application;




class SOCApp : public AppBase {
private:
	AppCommand m_cmd{ AppCommand::InvalidCMD };
	std::string m_HomePath;
protected:
	virtual bool initaliseConfig();
	virtual bool initaliseArgs(int argc, char** argv);
	virtual bool doRun();
	void AppEnded();
	bool runFTPSync();
	bool RunFTPSync();
	static bool runHeartBeat();
public:
	SOCApp();

	void setCommand(AppCommand cmd) { m_cmd = cmd; };
	AppCommand getCommand() { return m_cmd; };
};

