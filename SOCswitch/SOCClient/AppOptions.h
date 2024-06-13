#pragma once

#include <string>

enum class AppCommand {
	VersionCMD,
	RunCMD,
	InvalidCMD
};

class SOCApp;

class AppOptions
{
	static AppOptions* m_this;
	static std::string m_homePath;
	static std::string m_configPath;
	static bool m_level;
	static bool m_quiet;
	static bool m_silent;
	static AppCommand m_cmd;
	friend class SOCApp;

public:

	static AppOptions& get();
	AppOptions() = default;
	virtual ~AppOptions() = default;


	const char* getHomePath() {
		return m_homePath.c_str();
	}

	bool quiet() {
		return m_quiet;
	}
	bool silent() {
		return m_silent;
	}

	void setHomePath(const char* homePath) {
		m_homePath = homePath;
	}

	void setConfigPath(const char* configPath) {
		m_configPath = configPath;
	}

	const char* getConfigPath() {
		return m_configPath.c_str();
	}

	void setCommand(AppCommand cmd) { m_cmd = cmd; };
	AppCommand getCommand() { return m_cmd; };
};

