#pragma once

#include <string>

#include "Application/ArgvParser.h"

using namespace Application;



class SOCArgvParser : public ArgvParser
{
public:

	SOCArgvParser() = default;
	virtual ~SOCArgvParser() = default;

	uint32_t parseErrorNumber(ParserResults _error_code) const;

protected:
	virtual bool doInitalise(int argc, char** argv);

	virtual void defineOptions();
	std::string usageDescriptionHeader(unsigned int _width) const;

	

	virtual std::string generalHelp(unsigned int _width) const;
};
