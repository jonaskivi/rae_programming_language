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

#define DEBUG_RAE_HUMAN
//#define DEBUG_RAE_PARSER
//#define DEBUG_RAE
//#define DEBUG_RAE2
//#define DEBUG_RAE_DESTRUCTORS
//#define DEBUG_RAE_RVALUE

template <typename T>
  string numberToString ( T number )
  {
     ostringstream ss;
     ss << number;
     return ss.str();
  }

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



