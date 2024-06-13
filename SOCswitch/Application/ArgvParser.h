#pragma once

#include <map>
#include <list>
#include <vector>
#include <string>

namespace Application {

class ArgvParser
{
public:



    ArgvParser();
    ~ArgvParser();

    /** Attributes for options. */
    enum class OptionAttributes
    {
        NoOptionAttribute = 0x00,
        OptionRequiresValue = 0x01,
        OptionRequired = 0x02
    };
    /** Return values of the parser. */
    enum class ParserResults
    {
        NoParserError = 00,
        ParserUnknownOption = 1,
        ParserMissingValue = 2,
        ParserOptionAfterArgument = 4,
        ParserMalformedMultipleShortOption = 8,
        ParserRequiredOptionMissing = 16,
        ParserHelpRequested = 32,
        GeneralHelpRequested = 64,
        TopicHelpRequested = 128,
        ParserCommandNotFound = 256,
        NotACommand = 516,
        NoOptions = 1024,
        NoSubCommand = 2048
    };

    

    //typedef int OptionAttributes;
    //typedef int ParserResults;
    typedef std::map<std::string, unsigned int> String2KeyMap;
    typedef std::map<unsigned int, OptionAttributes> Key2AttributeMap;
    typedef std::map<unsigned int, std::string> Key2StringMap;
    typedef std::vector<std::string> ArgumentContainer;
    typedef std::vector<std::string> DefaultArgumentsContainer;

    /** Defines an option with optional attributes (required, ...) and an
    * additional (also optional) description. The description becomes part of the
    * generated usage help that can be requested by calling the usageDescription()
    * method.
    * \return Returns FALSE if there already is an option with this name
    * OR if a short option string (length == 1) is a digit. In that case no
    * action is peformed.
    */
    bool defineOption(const std::string& _name,
        const std::string& _description = std::string(),
        OptionAttributes _attributes = ArgvParser::OptionAttributes::NoOptionAttribute);
    /** Define an alternative name for an option that was previously defined by
    * defineOption().
    * \return Returns FALSE if there already is an option with the alternative
    * name or no option with the original name OR if a short option string
    * (length == 1) is a digit. In that case no action is performed.
    */
    bool defineOptionAlternative(const std::string& _original,
        const std::string& _alternative);

   


    /** Returns whether _name is a defined option. */
    bool isDefinedOption(const std::string& _name) const;
    /** Returns whether _name is an option that was found while parsing
    * the command line arguments with the parse() method. In other word: This
    * method returns true if the string is an option AND it was given on the
    * parsed command line.
    */
    bool foundOption(const std::string& _name) const;
    /** Define a help option. If this option is found a special error code is
    * returned by the parse method.
    * \attention If this method is called twice without an intermediate call
    * to the reset() method the previously set help option will remain a valid
    * option but is not detected as the special help option and will therefore
    * not cause the parse() method to return the special help error code.
    * \return Returns FALSE if there already is an option defined that equals
    * the short or long name.
    */
    bool setHelpOption(const std::string& _longname = "h",
        const std::string& _shortname = "help",
        const std::string& _descr = "");
    /** Returns the number of read arguments. Arguments are efined as beeing
    * neither options nor option values and are specified at the end of the
    * command line after all options and their values. */
    size_t arguments() const;
    /** Returns the Nth argument. See arguments().
    * \return Argument string or an empty string if there was no argument of
    * that id.
    */
    std::string argument(unsigned int _number) const;
    /** Get the complete argument vector. The order of the arguments in the
    * vector is the same as on the commandline.
    */
    const std::vector<std::string>& allArguments() const;
    /** Add an error code and its description to the command line parser.
    * This will do nothing more than adding an entry to the usage description.
    */

    const std::vector<std::string>& getUnNamedArguments() const {
        return unnamed_argument_container;
    }


    void addErrorCode(int _code, const std::string& _descr = "");
    /** Set some string as a general description, that will be printed before
    * the list of available options.
    */
    void setIntroductoryDescription(const std::string& _descr);
    /** Parse the command line arguments for all known options and arguments.
    * \return Error code with parsing result.
    * \retval NoParserError Everything went fine.
    * \retval ParserUnknownOption Unknown option was found.
    * \retval ParserMissingValue A value to a given option is missing.
    * \retval ParserOptionAfterArgument Option after an argument detected. All
    * options have to given before the first argument.
    * \retval ParserMalformedMultipleShortOption Malformed short option string.
    * \retval ParserRequiredOptionMissing Required option is missing.
    * \retval ParserHelpRequested Help option detected.
    */
    ParserResults parse(int _argc, char** _argv);



    DefaultArgumentsContainer& getDefaultArgumentsContainer() {
        return defaultArgumentsContainer;
    }

    /** Return the value of an option.
    * \return Value of a commandline options given by the name of the option or
    * an empty string if there was no such option or the option required no
    * value.
    */
    std::string optionValue(const std::string& _option) const;
    /** Reset the parser. Call this function if you want to parse another set of
    * command line arguments with the same parser object.
    */
    void reset();
    /** Returns the name of the option that was responsible for a parser error.
      * An empty string is returned if no error occured at all.
      */
    const std::string& errorOption() const;
    /** This method can be used to evaluate parser error codes and generate a
    * human-readable description. In case of a help request error code the
    * usage description as returned by usageDescription() is printed.
    */
    std::string parseErrorDescription(ParserResults _error_code) const;

    virtual std::string usageDescriptionHeader(unsigned int _width) const;

   

    virtual std::string usageDescription(unsigned int _width = 80) const;

    virtual std::string generalHelp(unsigned int _width) const;


    void setHeader(const std::string& _option);



    std::string getCommandLine() {
        return commandLine;
    }

    void printSyntax();

    bool initalise(int argc, char** argv) {
        defineOptions();
        return doInitalise(argc, argv);
    }

    virtual void defineOptions() {};
    virtual bool doInitalise(int argc, char** argv) { return true; };

protected:
    /** Returns the key of a defined option with name _name or -1 if such option
     * is not defined. */
    int optionKey(const std::string& _name) const;
    /** Returns a list of option names that are all alternative names associated
     * with a single key value.
     */
    std::list<std::string> getAllOptionAlternatives(unsigned int _key) const;

    std::string getSyntax(const std::string& _command) const;

    /** The current maximum key value for an option. */
    unsigned int max_key;
    /** Map option names to a numeric key. */
    String2KeyMap option2key;

    /** Map numeric key to option. The reverse of the above */
    Key2StringMap key2option;

    /** Map option key to option attributes. */
    Key2AttributeMap option2attribute;

    /** Map option key to option description. */
    Key2StringMap option2descr;

    /** Map option key to option value. */
    Key2StringMap option2value;

    /** Map error code to its description. */
    std::map<int, std::string> errorcode2descr;

    /** Vector of command line arguments. */
    ArgumentContainer argument_container;

    ArgumentContainer unnamed_argument_container;

    /** May of command syntax strings */
    Key2StringMap command2syntax;

    /** General description to be returned as first part of the generated help page. */
    std::string intro_description;

    /** Command header. */
    std::string command_header;

    /** General description to be returned as first part of the generated help page. */
    std::string masteroption_description;

    // Command line
    std::string commandLine;

    

    unsigned int current_command_id;

    std::string current_command;

    /** Holds file or directory names*/
    DefaultArgumentsContainer defaultArgumentsContainer;
    /** Holds the key for the help option. */
    unsigned int help_option;

    /** Holds the name of the option that was responsible for a parser error.
    */
    std::string error_option;

    void makeCommandline(int _argc, char** _argv);
}; // class ArgvParser


// Auxillary functions

/** Returns whether the given string is a valid (correct syntax) option string.
 * It has to fullfill the following criteria:
 *  1. minimum length is 2 characters
 *  2. Start with '-'
 *  3. if if minimal length -> must not be '--'
 *  4. first short option character must not be a digit (to distinguish negative numbers)
 */
bool isValidOptionString(const std::string& _string);

/** Returns whether the given string is a valid (correct syntax) long option string.
 * It has to fullfill the following criteria:
 *  1. minimum length is 4 characters
 *  2. Start with '--'
 */
bool isValidLongOptionString(const std::string& _string);

bool isValidCommandString(const std::string& _string);
/** Splits option and value string if they are given in the form 'option=value'.
* \return Returns TRUE if a value was found.
*/
bool splitOptionAndValue(const std::string& _string, std::string& _option,
    std::string& _value);

/** String tokenizer using standard C++ functions. Taken from here:
 * http://gcc.gnu.org/onlinedocs/libstdc++/21_strings/howto.html#3
 * Splits the string _in by _delimiters and store the tokens in _container.
 */
template <typename Container>
void splitString(Container& _container, const std::string& _in,
    const char* const _delimiters = " \t\n")
{
    const std::string::size_type len = _in.length();
    std::string::size_type i = 0;

    while (i < len)
    {
        // eat leading whitespace
        i = _in.find_first_not_of(_delimiters, i);
        if (i == std::string::npos)
            return;   // nothing left but white space

        // find the end of the token
        std::string::size_type j = _in.find_first_of(_delimiters, i);

        // push token
        if (j == std::string::npos)
        {
            _container.push_back(_in.substr(i));
            return;
        }
        else
            _container.push_back(_in.substr(i, j - i));

        // set up for next loop
        i = j + 1;
    }
}

/** Returns true if the character is a digit (what else?). */
bool isDigit(const char& _char);

/** Build a vector of integers from a string of the form:
* '1,3-5,14,25-20'. This string will be expanded to a list of positive
* integers with the following elements: 1,3,4,5,14,25,24,23,22,21,20.
* All of the expanded elements will be added to the provided list.
* \return Returns FALSE if there was any syntax error in the given string
* In that case the function stops at the point where the error occured.
* Only elements processed up to that point will be added to the expanded
* list.
* \attention This function can only handle unsigned integers!
*/
bool expandRangeStringToUInt(const std::string& _string,
    std::vector<unsigned int>& _expanded);
/** Returns a copy of _str with whitespace removed from front and back. */
std::string trimmedString(const std::string& _str);

/** Formats a string of an arbitrary length to fit a terminal of width
* _width and to be indented by _indent columns.
*/
std::string formatString(const std::string& _string,
    unsigned int _width,
    unsigned int _indent = 0,
    unsigned int _endPadding = 0,
    unsigned int _padplusone = 0);

std::string formatLine(const std::string& _string,
    unsigned int _width,
    unsigned int _indent = 0,
    unsigned int _endPadding = 0);

}; // namespace CommandLineProcessing


