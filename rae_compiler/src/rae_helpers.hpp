#ifndef RAE_COMPILER_RAE_HELPERS_HPP
#define RAE_COMPILER_RAE_HELPERS_HPP

#include <iostream>
#include "rlutil.h"//for colours in console output
#include <sstream> //for ostringstream
#include <string>
#include <algorithm>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef _WIN32
	#include <direct.h>
#endif
#if __cplusplus >= 201103L //c++11
	#include <thread>
#endif

	using namespace std;

/* REMOVE BOOST
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#include <boost/filesystem.hpp>

#include <boost/signals2.hpp>
#include <boost/bind.hpp>
#include <boost/range/algorithm/remove_if.hpp>
	using namespace boost;
*/

#include <stdio.h>//for FILE

typedef unsigned int uint;


//////////////////////

//#include "rae_log.hpp"

// The Debugtree is the most useful way to debug the parser!
//#define DEBUG_RAE_DEBUGTREE
// Debugtree2 is not so useful...
//#define DEBUG_RAE_DEBUGTREE2
//#define DEBUG_DEBUGNAME
//const std::string g_debugName("NONONONONO");
//#define DEBUG_DEBUGMODULENAME
//const std::string g_debugModuleName("rae.examples.small");

//#define DEBUG_RAE_VALIDATE
//#define DEBUG_RAE_HUMAN

// Bigger picture debugs:
//#define DEBUG_RAE_PARSER
//#define DEBUG_RAE_TOKENIZER
//#define DEBUG_RAE_WRITER
//#define DEBUG_RAE
//#define DEBUG_RAE2
//#define DEBUG_RAE_DESTRUCTORS
//#define DEBUG_RAE_RVALUE
//#define DEBUG_FILEWRITER
//#define DEBUG_RAE_INDENTS
//#define DEBUG_RAE_BRACKET
//#define DEBUG_COMMENTS

//#define DEBUG_RAE_NEWLINE
//#define DEBUG_RAE_CLASS
//#define DEBUG_RAE_FUNCS
//#define DEBUG_RAE_REFERENCES
//#define DEBUG_RAE_NUMBERS
//#define DEBUG_RAE_INIT_DATA
//#define DEBUG_RAE_QUOTES
//#define DEBUG_RAE_UNKNOWN
//#define DEBUG_RAE_ALIAS
//#define DEBUG_RAE_IMPORT
//#define DEBUG_RAE_MODULE
//#define DEBUG_RAE_NAMESPACE
//#define DEBUG_RAE_POINT_TO

// Debugs for small stuff:
//#define DEBUG_PARENT_ELEMENT
//#define DEBUG_RAE_EXPECTING_TOKEN
//#define DEBUG_RAE_EXPECTING_NAME
//#define DEBUG_RAE_EXPECTING_TYPE

// Cpp stuff:
//#define DEBUG_CPP_PARSER
//#define DEBUG_CPP_TYPEDEF
//#define DEBUG_RAE_CPP_PREPROCESSOR
//#define DEBUG_CPP_NEWLINE
//#define DEBUG_CPP_INIT_DATA
//#define DEBUG_CPP_PARENTHESIS




template <typename T>
	string numberToString ( T number )
	{
		ostringstream ss;
		ss << number;
		return ss.str();
	}

template <typename T>
	T stringToNumber( const string& set_text )
	{
		istringstream ss(set_text);
		T result;
		return ss >> result ? result : 0;
	}

bool isWhiteSpace(const char& set_char);
bool isWhiteSpace(const string& set);
string removeFromString(const string& set, const char rem);
string replaceCharInString(string str, const string& replace, char ch);
// Basically adds ./ to begin and / to end of a directory string in some circumstances.
string makeDirPathBetter(const string& set_path);


namespace PathType
{
enum e
{
	DOES_NOT_EXIST,
	FILE,
	DIRECTORY
};
}

PathType::e checkPathType(const string& currentFilenamePath);
int createPathIfNotExist(const string& set_path);
int createPath(const string& s);
string parentPath(const string& original_path);
string getFilenameFromPath(const string& original_path);
string replaceExtension(const string& original_path, const string& new_extension);

#endif



