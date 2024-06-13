#include "SOCClientArgvParser.h"
#include "SOCClient/AppOptions.h"
#include "Application/ReturnCode.h"




bool SOCArgvParser::doInitalise(int argc, char** argv)
{
	AppOptions& appOptions = AppOptions::get();
	ArgvParser::ParserResults res = parse(argc, argv);

	std::string errStr;

	switch (res) {
	case ArgvParser::ParserResults::NoParserError:
		break;
	case ArgvParser::ParserResults::ParserUnknownOption:
	case ArgvParser::ParserResults::ParserMissingValue:
	case ArgvParser::ParserResults::ParserOptionAfterArgument:
	case ArgvParser::ParserResults::ParserMalformedMultipleShortOption:
	case ArgvParser::ParserResults::ParserRequiredOptionMissing:
	case ArgvParser::ParserResults::ParserHelpRequested:
		errStr = parseErrorDescription(res);
		ReturnCodeObject::setReturn(parseErrorNumber(res), "%s", errStr.c_str());
		return false;
	case ArgvParser::ParserResults::NoOptions:
		appOptions.setCommand(AppCommand::RunCMD);
		return true; // Just do the run command
	case ArgvParser::ParserResults::NotACommand:
		errStr = parseErrorDescription(res);
		ReturnCodeObject::setReturn(parseErrorNumber(res), "%s", errStr.c_str());
		return false;
	default:
		ReturnCodeObject::setReturn(UnKnownError, "Invalid parser operation: %s");
		return false;
	}



	if (foundOption("version") == true)
	{
		
		appOptions.setCommand(AppCommand::VersionCMD);
		return true;
	}
	

	return false;
}

void SOCArgvParser::defineOptions()
{

	addErrorCode(0, "Success");
	addErrorCode(1, "Warnings");
	addErrorCode(2, "Errors");
	addErrorCode(3, "Fatal");

	setIntroductoryDescription("socapp - Tool provides a routing application that enable the management of network routes.");
	setHelpOption();

		setHeader("usage: socapp [options] [args]\n\n"
			"SOC Switch app, version 1.0.0.1\n"
			"Type 'socapp help' for help on the network routes management application.\n\n"
			"socapp is the primary command-line interface to SOC network routes management."
			"\n");
	
		defineOption("version", "prints the version information", ArgvParser::OptionAttributes::NoOptionAttribute);


}

std::string SOCArgvParser::usageDescriptionHeader(unsigned int _width) const
{
	return std::string();
}

std::string SOCArgvParser::generalHelp(unsigned int _width) const
{
	return std::string();
}

uint32_t SOCArgvParser::parseErrorNumber(ParserResults _error_code) const
{


	switch (_error_code)
	{
	case ParserResults::ParserHelpRequested:
	case ParserResults::GeneralHelpRequested:
	case ParserResults::TopicHelpRequested:
	case ParserResults::NoParserError:
		return Success;
	case ParserResults::ParserUnknownOption:
		return ParserUnknownOption;
	case ParserResults::ParserMissingValue:
		return ParserMissingValue;
	case ParserResults::ParserOptionAfterArgument:
		return ParserOptionAfterArgument;
	case ParserResults::ParserMalformedMultipleShortOption:
		return ParserMalformedMultipleShortOption;
	case ParserResults::ParserRequiredOptionMissing:
		return ParserRequiredOptionMissing;
	case ParserResults::ParserCommandNotFound:
		return ParserUnknownOption;
	case ParserResults::NotACommand:
		return ParserUnknownOption;
	}
	return UnKnownError;
}