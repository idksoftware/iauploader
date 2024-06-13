#pragma once
#include <string>
#include "ExitCode.h"
using ReturnCode = uint32_t;

constexpr ReturnCode Success = 0;
constexpr ReturnCode ParserUnknownOption = 4001;
constexpr ReturnCode ParserMissingValue = 4002;
constexpr ReturnCode ParserOptionAfterArgument = 4003;
constexpr ReturnCode ParserMalformedMultipleShortOption = 4004;
constexpr ReturnCode ParserRequiredOptionMissing = 4005;
constexpr ReturnCode ParserCommandNotFound = 4006;
constexpr ReturnCode NoArgument = 4006;
constexpr ReturnCode UnKnownError = 4007;

constexpr ReturnCode InvalidArgument = 4008;
constexpr ReturnCode NoArgumentForSubCommand = 4008;
constexpr ReturnCode SubCommandRequired = 4009;
constexpr ReturnCode FatalAppError = 4010;

class ReturnCodeObject
{

	static std::string m_errorstring;
	static ReturnCode m_returnCode;
	
public:

	static void setReturnString(const char* format, ...);
	static void setReturn(ReturnCode returnCode, const char* format, ...);
	static std::string& getReturnString() { return m_errorstring; };
	static ReturnCode getReturnCode() { return m_returnCode; };
	
};

