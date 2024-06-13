#include "stdio.h"
#include <iostream>
#include "AppBase.h"
#include "ReturnCode.h"

namespace Application {
	
	ReturnCode AppBase::m_returnCode;
	std::string AppBase::m_appName;
	std::string AppBase::m_returnString;
	ExitCode AppBase::m_exitCode;

	AppBase::~AppBase()
	{
	}

	bool AppBase::initalise(int argc, char** argv) {
		bool ret = true;
		if (initaliseConfig() == false) {
			ret = false;
		}
		if (ret) {
			if (initaliseArgs(argc, argv) == false) {
				ret = false;
			}
		}
		std::string  retStr = ReturnCodeObject::getReturnString();
		if (!retStr.empty()) {
			m_returnString = retStr;
			m_returnCode = ReturnCodeObject::getReturnCode();
		}
		if (!ret) {
			m_exitCode = ExitCode::Fatal;
		}
		return ret;
	};

	bool AppBase::Run()
	{
		bool ret = doRun();
		std::string  retStr = ReturnCodeObject::getReturnString();

		if (!retStr.empty()) {
			m_returnString = retStr;
			m_returnCode = ReturnCodeObject::getReturnCode();
		}
		
		if (ret == false) {
			
			m_exitCode = ExitCode::Fatal;
		}
		return ret;
	}

	ExitCode AppBase::RunApp(int argc, char **argv) {
		bool error = false;
		if (initalise(argc, argv) == false) {

			error = true;
		}
		else {
			if (Run() == false) {
				error = true;
			}
		}
		if (error) {
			int code = AppBase::getError();
			std::cout << AppBase::getFullErrorString();
			
		}
		else {
			// this is invalid in xml,json or html
			//std::cout << "0000:Success";
		}
		return m_exitCode;
	}
	
}