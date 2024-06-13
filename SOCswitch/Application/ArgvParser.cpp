#include "ArgvParser.h"

#include <iostream>
#include <sstream>

using namespace std;

namespace Application {

	ArgvParser::ArgvParser()
		: max_key(1),
		current_command_id(-1),
		help_option(0) // must be smaller than max_key initially

	{
		// nothing
	}

	ArgvParser::~ArgvParser()
	{
		// nothing
	}

	void ArgvParser::reset()
	{
		max_key = 1;
		option2key.clear();
		option2attribute.clear();
		option2descr.clear();
		option2value.clear();
		errorcode2descr.clear();
		argument_container.clear();
		intro_description.clear();
		error_option.clear();
		help_option = 0;
	}

	void ArgvParser::printSyntax()
	{
		for (auto item : command2syntax) {
			auto opt = key2option.find(item.first);
			if (opt == key2option.end())
			{
				continue;
			}
			std::string optStr = opt->second;
			printf("itemid: %d string: %s, syntax: %s\n", item.first, optStr.c_str(), item.second.c_str());
		}
	}

	int ArgvParser::optionKey(const string& _name) const
	{
		const auto it = option2key.find(_name);

		// if not found
		if (it == option2key.end())
			return (-1);

		return (it->second);
	}

	bool ArgvParser::isDefinedOption(const string& _name) const
	{
		return (option2key.find(_name) != option2key.end());
	}

	bool ArgvParser::foundOption(const string& _name) const
	{
		int key = optionKey(_name);

		// not defined -> cannot by found
		if (key == -1)
			return (false);

		// return whether the key of the given option name is in the hash of the
		// parsed options.
		return (option2value.find(key) != option2value.end());
	}

	string ArgvParser::optionValue(const string& _option) const
	{
		int key = optionKey(_option);

		// not defined -> cannot by found
		if (key == -1)
		{
			cerr << "ArgvParser::optionValue(): Requested value of an option the parser did not find or does not know." <<
				endl;
			return ("");
		}
		if (option2value.find(key) == option2value.end())
		{
			cerr << "ArgvParser::optionValue(): Requested value of an option the parser did not find or does not know." <<
				endl;
			return std::string();
		}
		return (option2value.find(key)->second);
	}

	ArgvParser::ParserResults
	ArgvParser::parse(int _argc, char** _argv)
	{
		bool finished_options = false; // flag whether an argument was found (options are passed)
		unsigned int key = -1;
		makeCommandline(_argc, _argv);
		// loop over all command line arguments
		int i = 1; // argument counter

		if (_argc <= 1)
		{
			// No command found.
			return ParserResults::NoOptions;
		}
	

		while (i < _argc)
		{
			string argument = _argv[i];

			string option; // option name
			string value; // option value

			// if argument is an option
			if (!isValidOptionString(argument))
			{
				// string is a real argument since values are processed elsewhere
				finished_options = true;
				argument_container.push_back(argument);
				unnamed_argument_container.push_back(argument);
			}
			else // can be a long or multiple short options at this point
			{
				// check whether we already found an argument
				if (finished_options)
				{
					error_option = argument;
					return (ParserResults::ParserOptionAfterArgument); // return error code
				}

				unsigned int key = 0;
				// check for long options
				if (isValidLongOptionString(argument))
				{
					// handle long options

					// remove trailing '--'
					argument = argument.substr(2);
					// check for option value assignment 'option=value'
					splitOptionAndValue(argument, option, value);

					if (!isDefinedOption(option)) // is this a known option
					{
						error_option = option; // store the option that caused the error
						return (ParserResults::ParserUnknownOption); // return error code if not
					}

					// get the key of this option - now that we know that it is defined
					key = option2key.find(option)->second;
					if (key == help_option) // if help is requested return error code
						return (ParserResults::ParserHelpRequested);

					// do we need to extract a value
					// AND a value is not already assigned from the previous step
					if ((option2attribute.find(key)->second == OptionAttributes::OptionRequiresValue) && value.empty())
					{
						if (i + 1 >= _argc) // are there arguments left?
						{
							error_option = option; // store the option that caused the error
							return (ParserResults::ParserMissingValue); // the was no argument left although we need a value
						}

						string temp = _argv[i + 1]; // get the next element
						++i; // increase counter now that we moved forward

						if (isValidOptionString(temp))
						{
							error_option = option; // store the option that caused the error
							return (ParserResults::ParserMissingValue); // missing option value
						}
						value = temp; // assign value
					}
					// add option-value map entry
					option2value[key] = value;
				}
				else // handle short options
				{
					argument = argument.substr(1); // remove trailing '-'

					// check for option value assignment 'option=value'
					if (splitOptionAndValue(argument, option, value))
					{
						// there was an option <- value assignment
						if (option.length() > 1)
						{
							error_option = option; // store the option that caused the error
							return (ParserResults::ParserMalformedMultipleShortOption);
							// return error code if option has more than one character
						}

						if (!isDefinedOption(option)) // is this a known option
						{
							error_option = option; // store the option that caused the error
							return (ParserResults::ParserUnknownOption); // return error code if not
						}
						key = option2key.find(option)->second; // get the key for the extracted option name

						if (key == help_option) // if help is requested return error code
							return (ParserResults::ParserHelpRequested);

						// if value is still empty for some reason: we have an error
						if ((option2attribute.find(key)->second == OptionAttributes::OptionRequiresValue) && value.empty())
						{
							error_option = option; // store the option that caused the error
							return (ParserResults::ParserMissingValue); // missing option value
						}
						// add option-value map entry
						option2value[key] = value;
					}
					else // no '=' assignment: can be either multiple short options or
						// something like '-s 4'
					{
						// handle short options with value like '-s 4'
						option.clear();
						value.clear();

						if (argument.length() == 1) // if a single short option
						{
							if (!isDefinedOption(argument)) // is this a known option
							{
								error_option = argument; // store the option that caused the error
								return (ParserResults::ParserUnknownOption); // return error code if not
							}
							key = option2key.find(argument)->second; // get the key for the extracted option name

							if (key == help_option) // if help is requested return error code
								return (ParserResults::ParserHelpRequested);

							// check if option needs a value and next arg is not an option
							if ((option2attribute.find(key)->second == OptionAttributes::OptionRequiresValue))
							{
								if (i + 1 >= _argc) // are there arguments left?
								{
									error_option = argument; // store the option that caused the error
									return (ParserResults::ParserMissingValue);
									// the was no argument left although we need a value
								}
								string temp = _argv[i + 1]; // get the next element
								++i; // increase counter now that we moved forward

								if (isValidOptionString(temp))
								{
									error_option = argument; // store the option that caused the error
									return (ParserResults::ParserMissingValue); // missing option value
								}
								// add option-value map entry
								option2value[key] = temp;
							}
							else // no value needed
							{
								option2value[key] = ""; // assign value
							}
						}
						else // handle multiple short option like '-svxgh'
						{
							unsigned int short_option_counter = 0; // position in the multiple short option string
							while (short_option_counter < argument.length()) // parse the whole string
							{
								option = argument[short_option_counter]; // get the option character

								if (!isDefinedOption(option)) // is this a known option
								{
									error_option = option; // store the option that caused the error
									return (ParserResults::ParserUnknownOption); // return error code if not
								}
								key = option2key.find(option)->second; // get the key for the extracted option name

								if (key == help_option) // if help is requested return error code
									return (ParserResults::ParserHelpRequested);

								option2value[key] = value;

								++short_option_counter; // advance one character forward
							}
						}
					}
				}
			}
			if (option.length() == 0)
			{
				// Is a file
				defaultArgumentsContainer.push_back(argument);
			}
		
			++i;
		}


		for (auto it = option2attribute.begin(); it != option2attribute.end(); ++it)
		{
		
			// if the current option is required look if we got it
			if (it->second == OptionAttributes::OptionRequired)
			{
				// is the object missing
				if (option2value.find(it->first) == option2value.end())
				{
					// get the list of alternative names for this option
					list<string> alternatives = getAllOptionAlternatives(it->first);

					unsigned int count = 0;
					for (list<string>::const_iterator alt = alternatives.begin();
						alt != alternatives.end();
						++alt)
					{
						++count;
						// additional '-' for long options
						if (alt->length() > 1)
							error_option += "-";

						error_option += "-" + *alt;

						// alternatives to come?
						if (count < alternatives.size())
							error_option += ", "; // add separator
					}
					return (ParserResults::ParserRequiredOptionMissing);
				}
			}
		}

		return (ParserResults::NoParserError); // everthing went fine -> sucess
	}


	size_t ArgvParser::arguments() const
	{
		return (argument_container.size());
	}

	string ArgvParser::argument(unsigned int _id) const
	{
		if (_id >= arguments())
		{
			cerr << "ArgvParser::argument(): Request for non-existing argument." << endl;
			return ("");
		}
		return (argument_container[_id]);
	}

	const vector<string>& ArgvParser::allArguments() const
	{
		return (argument_container);
	}



	std::string ArgvParser::usageDescription(unsigned int _width) const
	{
		std::string usage; // the usage description text

	

		usage += '\n';

		usage += usageDescriptionHeader(_width);

		usage += '\n';

		usage += formatString(command_header, _width) + "\n";
	
		usage += ":\n\n";

		for (auto it = option2attribute.begin(); it != option2attribute.end(); ++it)
		{
			std::string _os; // temp string for the option
		
			std::string _longOpt;
			std::string _shortOpt;
			std::list<std::string> alternatives = getAllOptionAlternatives(it->first);
		

			if (!_longOpt.empty())
			{
				_os += ' ';
				_os += _longOpt;
			}
			if (!_shortOpt.empty())
			{
				_os += " (";
				_os += _shortOpt;
				_os += ')';
			}
			//_os += " - ";
			usage += formatLine(_os, _width, 0, 20);
			_os.clear();
			_longOpt.clear();
			_shortOpt.clear();
			if (option2descr.find(it->first) != option2descr.end())
				//usage += formatString(option2descr.find(it->first)->second, _width, _os.length() + 2) + "\n";
				usage += formatString(option2descr.find(it->first)->second, _width) + "\n";
			else
				usage += formatString("(no description)", _width, 4) + "\n";
		}


		if (!errorcode2descr.size()) // if have no errorcodes
			return (usage);

		usage += "\n";
		usage += formatString("Return codes:\n", _width) + "\n";

		//   map<int, string>::const_iterator eit;
		for (auto alt = errorcode2descr.begin();
			alt != errorcode2descr.end();
			++alt)
		{
			std::ostringstream code;
			code << alt->first;
			std::string label = formatString(code.str(), _width, 4);
			std::string descr = formatString(alt->second, _width, 10);
			usage += label + descr.substr(label.length()) + "\n";
		}
	
		usage += "\n\n";
		return (usage);
	}

	std::string ArgvParser::generalHelp(unsigned int _width) const
	{
		return std::string();
	}


	void ArgvParser::makeCommandline(int _argc, char** _argv)
	{
		commandLine = _argv[0];
		for (int i = 1; i < _argc; i++)
		{
			commandLine += ' ';
			commandLine += _argv[i];
		}
	}


	/*
	bool ArgvParser::matchOption(unsigned int topic, const char* option) const
	{
		string _longOpt;
		string _shortOpt;

		auto topicObj = command_set.find(topic);
		if (topicObj == command_set.end())
		{
			return false;
		}
		ArgumentContainer ac = topicObj->second;

		for (vector<string>::const_iterator opt = ac.begin(); opt != ac.end(); ++opt)
		{
			//printf("%s\n", opt->c_str());
			if (option2key.find(opt->c_str()) == option2key.end())
			{
				continue;
			}
			unsigned int key = option2key.find(opt->c_str())->second;
			list<string> alternatives = getAllOptionAlternatives(key);
			for (list<string>::const_iterator alt = alternatives.begin();
				alt != alternatives.end();
				++alt)
			{
				//int option = option2attribute.find(key)->second;

				if (alt->length() > 1)
				{
					_longOpt = *alt;
				}
				else
				{
					_shortOpt = *alt;
				}
			}

			if (!_longOpt.empty())
			{
				if (_longOpt.compare(option) == 0)
				{
					return true;
				}
			}
			if (!_shortOpt.empty())
			{
				if (_shortOpt.compare(option) == 0)
				{
					return true;
				}
			}
			_longOpt.clear();
			_shortOpt.clear();
		}
		return false;
	}
	*/

	const string& ArgvParser::errorOption() const
	{
		return (error_option);
	}

	std::string ArgvParser::parseErrorDescription(ParserResults _error_code) const
	{
		string descr;

		switch (_error_code)
		{
		case ParserResults::NoParserError:
			// no error -> nothing to do
			break;
		case ParserResults::ParserUnknownOption:
			descr = "Unknown option: '" + errorOption() + "'";
			break;
		case ParserResults::NoSubCommand:
			descr = "No Sub-command";
			break;
		case ParserResults::ParserMissingValue:
			descr = "Missing required value for option: '" + errorOption() + "'";
			break;
		case ParserResults::ParserOptionAfterArgument:
			descr = "Misplaced option '" + errorOption() + "' detected. All option have to be BEFORE the first argument";
			break;
		case ParserResults::ParserMalformedMultipleShortOption:
			descr = "Malformed short-options: '" + errorOption() + "'";
			break;
		case ParserResults::ParserRequiredOptionMissing:
			descr = "Required option missing: '" + errorOption() + "'";
			break;
		case ParserResults::ParserHelpRequested: // help
			descr = usageDescription();
			break;
		case ParserResults::NotACommand:
			descr = "Invalid sub-command: '" + errorOption() + "'";
			break;
			break;
		case ParserResults::NoOptions:
			descr = "Required sub-command missing" + usageDescription();
			break;
		default:
			cerr << "ArgvParser::ParserResults::documentParserErrors(): Unknown error code" << endl;
		}

		return (descr);
	}

	std::string ArgvParser::usageDescriptionHeader(unsigned int _width) const
	{
		return std::string();
	}

	bool ArgvParser::defineOption(const string& _name,
		const string& _descr,
		OptionAttributes _attrs)
	{
		// do nothing if there already is an option of this name
		if (isDefinedOption(_name))
		{
			cerr << "ArgvParser::defineOption(): The option label: \"" << _name << "\" equals an already defined option." <<
				endl;
			return (false);
		}

		// no digits as short options allowed
		if (_name.length() == 1 && isDigit(_name[0]))
		{
			cerr << "ArgvParser::defineOption(): Digits as short option labels are not allowd." << endl;
			return (false);
		}

		option2key[_name] = max_key; // give the option a unique key

		key2option[max_key] = _name;

		// store the option attributes
		option2attribute[max_key] = _attrs;

		// store the option description if there is one
		if (_descr.length())
			option2descr[max_key] = _descr;

		// inc the key counter
		++max_key;

		return (true);
	}


	bool ArgvParser::defineOptionAlternative(const string& _original,
		const string& _alternative)
	{
		// do nothing if there already is no option of this name
		if (!isDefinedOption(_original))
		{
			cerr << "ArgvParser::defineOptionAlternative(): Original option label: \"" << _original <<
				"\" is not a defined option." << endl;
			return (false);
		}

		// AND no digits as short options allowed
		if (_alternative.length() == 1 && isDigit(_alternative[0]))
		{
			cerr << "ArgvParser::defineOptionAlternative(): Digits as short option labels are not allowd." << endl;
			return (false);
		}

		// AND do nothing if there already is an option with the alternativ name
		if (isDefinedOption(_alternative))
		{
			cerr << "ArgvParser::defineOptionAlternative(): The alternative option label equals an already defined option."
				<< endl;
			return (false);
		}

		option2key[_alternative] = optionKey(_original);

		return (true);
	}

	


	bool ArgvParser::setHelpOption(const string& _shortname,
		const string& _longname,
		const string& _descr)
	{
		// do nothing if any name is already in use
		if (isDefinedOption(_shortname) || isDefinedOption(_longname))
		{
			cerr << "ArgvParser::setHelpOption(): Short or long help option label equals an already defined option." <<
				endl;
			return (false);
		}

		// define the help option's short name and the alternative
		// longname
		defineOption(_shortname, _descr, OptionAttributes::NoOptionAttribute);
		defineOptionAlternative(_shortname, _longname);

		help_option = max_key - 1; // store the key in a special member

		return (true);
	}

	void ArgvParser::addErrorCode(int _code, const string& _descr)
	{
		errorcode2descr[_code] = _descr;
	}

	void ArgvParser::setIntroductoryDescription(const string& _descr)
	{
		intro_description = _descr;
	}

	void ArgvParser::setHeader(const std::string& _option)
	{
		command_header = _option;
	}

	list<string> ArgvParser::getAllOptionAlternatives(unsigned int _key) const
	{
		// keys go here
		list<string> keys;
		// for all container elements
		for (map<string, unsigned int>::const_iterator it = option2key.begin();
			it != option2key.end();
			++it)
		{
			if (it->second == _key)
				keys.push_back(it->first);
		}
		return (keys);
	}

	std::string ArgvParser::getSyntax(const std::string& _command) const
	{
		if (option2key.find(_command) == option2key.end())
		{
			// application error
			return std::string();
		}
		int key = option2key.find(_command)->second;
		if (command2syntax.find(key) == command2syntax.end())
		{
			std::string ret;
			return ret;
		}
		return command2syntax.find(key)->second;
	}

	bool Application::isDigit(const char& _char)
	{
		if (_char == '0' || _char == '1' || _char == '2' || _char == '3'
			|| _char == '4' || _char == '5' || _char == '6' || _char == '7'
			|| _char == '8' || _char == '9')
			return (true);
		return (false);
	}

	bool Application::isValidOptionString(const string& _string)
	{
		// minimal short option length is 2
		if (_string.length() < 2)
			return (false);

		// is it an option (check for '-' as first character)
		if (_string.compare(0, 1, "-"))
			return (false);

		// not an option if just '--'
		if (_string.length() == 2 && _string == "--")
			return (false);

		// it might still be a negative number
		// (but not if there is no digit afterwards)
		if (isDigit(_string[1]))
			return (false);

		// let's consider this an option
		return (true);
	}

	bool Application::isValidCommandString(const string& _string)
	{
		//unsigned int key = 0;
		// is it an option (check for '--')
		if (!_string.compare(0, 2, "--"))
		{
			return false;
		}
		if (!_string.compare(0, 1, "-"))
		{
			return false;
		}
		return true;
	}

	bool Application::isValidLongOptionString(const string& _string)
	{
		if (_string.length() < 4) // must be at least '--??'
			return (false);

		// is it an option (check for '--')
		if (_string.compare(0, 2, "--"))
			return (false);
		return (true);
	}

	bool Application::splitOptionAndValue(const string& _string,
		string& _option, string& _value)
	{
		// string token container
		std::vector<string> tokens;

		// split string by '=' delimiter
		splitString(tokens, _string, "=");

		// check for option value assignment 'option=value'
		if (tokens.size() < 2)
		{
			_option = _string; // the option is the whole string
			return (false);
		}

		// separate option and value
		_option = tokens[0];

		// concat all remaining tokens to the value string
		for (unsigned int i = 1; i < tokens.size(); ++i)
		{
			_value.append(tokens[i]);
		}

		return (true);
	}

	string Application::trimmedString(const std::string& _str)
	{
		// no string no work
		if (_str.length() == 0)
			return _str;

		string::size_type start_pos = _str.find_first_not_of(" \a\b\f\n\r\t\v");
		string::size_type end_pos = _str.find_last_not_of(" \a\b\f\n\r\t\v");

		// check whether there was any non-whitespace
		if (start_pos == string::npos)
			return ("");

		return string(_str, start_pos, end_pos - start_pos + 1);
	}

	bool Application::expandRangeStringToUInt(const std::string& _string,
		std::vector<unsigned int>& _expanded)
	{
		list<string> tokens;
		// split string by delimiter
		splitString(tokens, _string, ",");

		// loop over all entries
		for (list<string>::const_iterator it = tokens.begin(); it != tokens.end(); ++it)
		{
			const string& entry = *it; // convenience reference

	#ifdef ARGVPARSER_FINE

			cout << "TOKEN: " << entry << endl;
	#endif

			// if range was given
			if (entry.find("-") != string::npos)
			{
				// split into upper and lower border
				list<string> range_borders;
				splitString(range_borders, entry, "-");

				// fail if insane range spec
				if (range_borders.size() != 2)
					return (false);

				int first = atoi(range_borders.begin()->c_str());
				int second = atoi((++range_borders.begin())->c_str());

				// write id in increasing order
				if (first <= second)

				{
					for (int j = first; j <= second; ++j)
					{
						_expanded.push_back(j);
					}
				}
				else // write id in decreasing order
				{
					for (int k = first; k >= second; k--)
					{
						_expanded.push_back(k);
					}
				}
			}
			else // single number was given
				_expanded.push_back(atoi(entry.c_str())); // store id
		}

		return (true);
	}

	std::string Application::formatString(const std::string& _string,
		unsigned int _width,
		unsigned int _indent,
		unsigned int _padend,
		unsigned int _padplusone)
	{
		// if insane parameters do nothing
		if (_indent >= _width)
			return (_string);

		// list of lines of the formated string
		list<string> lines;

		// current position in the string
		size_t pos = 0;

		// till the end of the string
		while (pos < _string.length())
		{
			// get the next line of the string
			string line = _string.substr(pos, _width - _indent);


			// check for newlines in the line and break line at first occurence (if any)
			string::size_type first_newline = line.find_first_of("\n");
			if (first_newline != string::npos)
			{
				line = line.substr(0, first_newline);
			}

			// we need to check for possible breaks within words only if the extracted
			// line spans the whole allowed width
			bool check_truncation = true;
			if (line.length() < _width - _indent)
				check_truncation = false;

			// remove unecessary whitespace at front and back
			line = trimmedString(line);


			// only perform truncation if there was enough data for a full line
			if (!check_truncation)
				pos += line.length() + 1;
			else
			{
				// look for the last whitespace character
				string::size_type last_white_space = line.find_last_of(" \a\b\f\n\r\t\v");

				if (last_white_space != string::npos) // whitespace found!
				{
					// truncated the line at the last whitespace
					line = string(line, 0, last_white_space);
					pos += last_white_space + 1;
				}
				else // no whitespace found
					// rude break! we can leave the line in its current state
					pos += _width - _indent;
			}

			if (!line.empty())
			{
				if (_indent)
					line.insert(0, _indent, ' ');


				lines.push_back(line);
			}
		}

		// concat the formated string
		string formated;
		bool first = true;
		// for all lines
		auto it0 = lines.begin();
		for (; it0 != lines.end(); ++it0)
		{
			// add _indentplusone
			if (!first)
			{
				if (_padplusone)
				{
					it0->insert(0, _padplusone, ' ');
				}
			}
			else
			{
				first = false;
			}
		}
		first = true;
		auto it = lines.begin();
		for (; it != lines.end(); ++it)
		{
			// prefix with newline if not first
			if (!first)
			{
				formated += "\n";
			}
			else
			{
				first = false;
			}
			formated += *it;
		}

		if (_padend != 0)
		{
			if (it->length() < _padend)
			{
				size_t psize = _padend - it->length();
				for (size_t i = 0; i < psize; i++)
				{
					formated += ' ';
				}
			}
		}

		return (formated);
	}

	std::string Application::formatLine(const std::string& _string,
		unsigned int _width,
		unsigned int _indent,
		unsigned int _padend)
	{
		// if insane parameters do nothing
		if (_indent >= _width)
			return (_string);


		// current position in the string
		//unsigned int pos = 0;

		// till the end of the string

		std::string line = _string;

		// remove unecessary whitespace at front and back
		line = trimmedString(line);

		if (!line.empty())
		{
			if (_indent)
				line.insert(0, _indent, ' ');
		}

		if (_padend != 0)
		{
			if (line.length() < _padend)
			{
				size_t psize = _padend - line.length();
				for (size_t i = 0; i < psize; i++)
				{
					line += ' ';
				}
			}
		}

		return line;
	}
} 