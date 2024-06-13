#include "AppOptions.h"

//#include "ConfigReader.h"
//#include "AppConfig.h"
//#include "SAUtils.h"

//#include "Environment.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
//#define new DEBUG_NEW
#endif

AppCommand AppOptions::m_cmd;
	
std::string AppOptions::m_homePath;
std::string AppOptions::m_configPath;
bool AppOptions::m_quiet = false;
bool AppOptions::m_silent = false;
	

AppOptions& AppOptions::get() {
	static AppOptions _this;
	return _this;
}


	
	