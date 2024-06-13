#include "Utils.h"

#include <windows.h>


namespace Application {

	std::error_code Utils::m_ec;

	FileList_Ptr Utils::getFiles(const char* dirpath)
	{
		FileList_Ptr fileList = std::make_unique<FileList>();
		for (auto const& dir_entry : std::filesystem::directory_iterator{ dirpath })
		{
			std::filesystem::path p = dir_entry.path();
			std::filesystem::path f = p.filename();
			std::string fileItem = f.string();
			std::string pathStr = p.string();
			fileList->emplace_back(fileItem);
		}
		return fileList;
	}

	bool Utils::DirExists(const char* p)
	{
		std::filesystem::path path = p;
		//error_code ec;
		if (std::filesystem::exists(path, m_ec) == true) {
			return std::filesystem::is_directory(path);
		}
		return false;
	}

	bool Utils::FileExists(const char* p)
	{
		std::filesystem::path path = p;
		if (std::filesystem::exists(path) == true) {
			return std::filesystem::is_regular_file(path);
		}
		return false;
	}

	std::string Utils::GetEnv(const std::string& value, bool all)
	{
		HKEY hKey = nullptr;
		char buf[MAX_PATH];
		DWORD dwType = 0;
		DWORD dwBufSize = MAX_PATH;
		std::string res;
		bool error = false;
		if (all)
		{
			const char* subkey = "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment";
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, subkey, 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
			{
				error = true;
			}
		}
		else
		{
			const char* subkey = "Environment";
			if (RegOpenKeyEx(HKEY_CURRENT_USER, subkey, 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
			{
				error = true;
			}
		}
		if (!error) {
			if (RegQueryValueEx(hKey, value.c_str(), nullptr, nullptr, (BYTE*)buf, &dwBufSize) != ERROR_SUCCESS)
				//if (RegQueryValueEx(hKey, "IMGARCHIVE_HOME", NULL, NULL, (BYTE*)buf, &dwBufSize) != ERROR_SUCCESS)
			{
				error = true;
			}
		}
		if (!error) {
			res = buf;
		}
		RegCloseKey(hKey);
		return res;

	}

	std::string Utils::GetPOSIXEnv(const std::string& key)
	{
		char* var = nullptr;

		size_t pReturnValue = 0;
		char buffer[2 * 1024];
		size_t numberOfElements = 2 * 1024;
		errno_t res = getenv_s(&pReturnValue, buffer, numberOfElements, key.c_str());
		if (res == 0)
		{
			var = buffer;
		}
		std::string retval;
		if (var != nullptr)
		{
			retval = var;
		}
		return retval;
	}


	bool Utils::isEquals(const std::string& a, const std::string& b)
	{
		return std::equal(a.begin(), a.end(),
			b.begin(), b.end(),
			[](char a, char b)
			{
				return tolower(a) == tolower(b);
			});
	}

	BoolOption Utils::isTrueFalse(std::string& s)
	{
		if (isEquals("enabled", s) || isEquals("true", s) || isEquals("on", s) || isEquals("yes", s))
		{
			s = "True";
			return BoolOption::True;
		}
		if (isEquals("disabled", s) || isEquals("false", s) || isEquals("off", s) || isEquals("no", s))
		{
			s = "False";
			return BoolOption::False;
		}
		s = "Invalid";
		return BoolOption::Invalid;
	}

}

