#pragma once

#include <string>
#include <memory>
#include <vector>
#include <cerrno>
#include <filesystem>
#include <system_error>

namespace Application {

	enum class BoolOption
	{
		False = 0,
		True = 1,
		Invalid = -1
	};

	using FileList = std::vector<std::string>;
	using FileList_Ptr = std::unique_ptr<FileList>;

	class Utils
	{
		static std::error_code m_ec;
	public:
		Utils() = default;
		~Utils() = default;

		
		static FileList_Ptr getFiles(const char* dirpath);
		static bool FileExists(const char* p);
		static bool DirExists(const char* p);

		static std::string GetEnv(const std::string& value, bool all);
		static std::string GetPOSIXEnv(const std::string& key);
		static bool isEquals(const std::string& a, const std::string& b);
		static BoolOption isTrueFalse(std::string& s);
	};

}

