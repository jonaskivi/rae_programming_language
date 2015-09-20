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

#define DEBUG_RAE_DEBUGTREE
//#define DEBUG_RAE_DEBUGTREE2
#define DEBUG_DEBUGNAME
const std::string g_debugName("NONONONONO");
const std::string g_debugModuleName("rae.examples.small");

//#define DEBUG_RAE_VALIDATE
//#define DEBUG_RAE_HUMAN

//#define DEBUG_RAE_PARSER
//#define DEBUG_RAE
//#define DEBUG_RAE2
//#define DEBUG_RAE_DESTRUCTORS
//#define DEBUG_RAE_RVALUE
//#define DEBUG_FILEWRITER
//#define DEBUG_RAE_INDENTS
//#define DEBUG_RAE_BRACKET
#define DEBUG_COMMENTS

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



