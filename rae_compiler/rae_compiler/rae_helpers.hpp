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

PathType::e checkPathType(boost::filesystem::path currentFilenamePath)
{	
	PathType::e path_type;
	path_type = PathType::DOES_NOT_EXIST; //we're a bit pessimistic here.

	try
	{
		if (boost::filesystem::exists(currentFilenamePath))    // does currentFilenamePath actually exist?
		{
			if (boost::filesystem::is_regular_file(currentFilenamePath))        // is currentFilenamePath a regular file?
			{
				#ifdef DEBUG_RAE_HUMAN
				cout << currentFilenamePath << " size is " << boost::filesystem::file_size(currentFilenamePath) << "\n";
				//rae::log(currentFilenamePath.string(), " size is ", boost::filesystem::file_size(currentFilenamePath), "\n");
				#endif

				path_type = PathType::FILE;
			}
			else if( boost::filesystem::is_directory(currentFilenamePath) )      // is currentFilenamePath a directory?
			{
				path_type = PathType::DIRECTORY;

				cout << currentFilenamePath << " is a directory containing:\n";
				//rae::log(currentFilenamePath.string(), "is a directory containing:\n");


				typedef vector<boost::filesystem::path> vec;             // store paths,
				vec v;                                // so we can sort them later

				copy(boost::filesystem::directory_iterator(currentFilenamePath), boost::filesystem::directory_iterator(), back_inserter(v));

				sort(v.begin(), v.end());             // sort, since directory iteration
													  // is not ordered on some file systems

				for (vec::const_iterator it (v.begin()); it != v.end(); ++it)
				{
				  cout << "   " << *it << "\n";
					//rae::log("   ", *it, "\n");
				}
			}
			else
			{
				//Hmm, we are not handling this in PathType... oh well. What is it really? A link? A ufo?

				cout << currentFilenamePath << " exists, but is neither a regular file nor a directory\n";
				//rae::log(currentFilenamePath, " exists, but is neither a regular file nor a directory\n");
			}
		}
		else
		{
			cout << currentFilenamePath << " does not exist\n";
			//rae::log(currentFilenamePath, " does not exist\n");

			path_type = PathType::DOES_NOT_EXIST;
		}
	}
	catch (const boost::filesystem::filesystem_error& ex)
	{
		cout << ex.what() << "\n";
		//rae::log(ex.what(), "\n");
	}

	return path_type;
}


