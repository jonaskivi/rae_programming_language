#ifndef RAE_COMPILER_RAE_HELPERS_HPP
#define RAE_COMPILER_RAE_HELPERS_HPP


#include <iostream>
#include "rlutil.h"//for colours in console output
#include <sstream> //for ostringstream
#include <string>
#include <vector>
#if __cplusplus >= 201103L //c++11
	#include <thread>
#endif

	using namespace std;

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#include <boost/filesystem.hpp>

#include <boost/signals2.hpp>
#include <boost/bind.hpp>
#include <boost/range/algorithm/remove_if.hpp>
	using namespace boost;

#include <stdio.h>//for FILE

typedef unsigned int uint;


//////////////////////

//#include "rae_log.hpp"

//#define DEBUG_RAE_HUMAN
//#define DEBUG_RAE_PARSER
//#define DEBUG_RAE
//#define DEBUG_RAE2
//#define DEBUG_RAE_DESTRUCTORS
//#define DEBUG_RAE_RVALUE
//#define DEBUG_FILEWRITER
#define DEBUG_RAE_DEBUGTREE
//#define DEBUG_RAE_BRACKET

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
/*
bool isWhiteSpace(const char& set_char)
{
  if( set_char == ' ' || set_char == '\n' || set_char == '\t' )
    return true;
  //else
  return false;
}

bool isWhiteSpace(const string& set)
{
  if(set.size() == 0)
    return false;

  if( set[0] == ' ' || set[0] == '\n' || set[0] == '\t' )
    return true;
  //else
  return false;
}
*/
namespace PathType
{
enum e
{
	DOES_NOT_EXIST,
	FILE,
	DIRECTORY
};
}




#endif



