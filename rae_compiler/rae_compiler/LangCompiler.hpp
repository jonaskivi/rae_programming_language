#ifndef RAE_COMPILER_LANGCOMPILER_HPP
#define RAE_COMPILER_LANGCOMPILER_HPP

#include <stdio.h>  /* defines FILENAME_MAX */
#ifdef _WIN32
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
 #endif

#include "LangElement.hpp"
#include "StringFileWriter.hpp"
#include "SourceParser.hpp"
#include "ReportError.hpp"

namespace Rae
{

//test code:
/*
void task(string msg)
{
    //rae::log("task says: ", msg, "\n");
}

class Foo
{
public:
    void threadFunc()
    {
    	//rae::log("Hello\n");

    }
    void startThread()
    {
        _th = std::thread(&Foo::threadFunc, this);
    }
    void joinIt()
    {
    	_th.join();
    }
protected:

	vector<LangElement*> langElements;

    std::thread _th;
};
*/



class LangCompiler
{
public:
	LangCompiler()
	{
		force_one_thread = true;

		//workingPath = boost::filesystem::current_path();

		char cCurrentPath[FILENAME_MAX];
		if( !GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)) )
		{
			//return errno;
		}

		cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */

		//cout << "The current working directory is " << cCurrentPath <<"\n";

		workingPath = cCurrentPath;

		addModuleSearchPath(workingPath);
		//cout << "Added current directory to module search paths: " << moduleSearchPaths.back() << "\n";
	}

	~LangCompiler()
	{
		#ifdef DEBUG_RAE_DESTRUCTORS
			cout<<"~LangCompiler() START."<<"\n";
		#endif
		/*
		foreach(SourceParser* pars, sourceParsers)
		{
			#ifdef DEBUG_RAE_DESTRUCTORS
				if(pars)
					cout<<"~LangCompiler() going to delete pars."<<"\n";
				else cout<<"~LangCompiler() already deleted pars.\n";
			#endif
			
			if(pars)
				delete pars;
		}
		*/
		for(uint i = 0; i < sourceParsers.size(); i++)
		{
			if(sourceParsers[i])
				delete sourceParsers[i];
		}
		sourceParsers.clear();

		for(uint i = 0; i < stdLibModules.size(); i++)
		{
			if(stdLibModules[i])
				delete stdLibModules[i];
		}
		stdLibModules.clear();

		#ifdef DEBUG_RAE_DESTRUCTORS
			cout<<"~LangCompiler() END."<<"\n";
		#endif
	}

	bool force_one_thread;

	//REMOVE BOOST: boost::filesystem::path workingPath;
	//REMOVE BOOST: vector<boost::filesystem::path> moduleSearchPaths;
	string         workingPath;
	vector<string> moduleSearchPaths;
	

	void createRaeStdLib()
	{
		createRaeStdLibModule("string");
		createRaeStdLibModule("array");
		createRaeStdLibCppSupport("link");
	}

	void createRaeStdLibModule(string which_stdlib_module)
	{
		SourceParser* a_parser = new SourceParser();
		a_parser->createRaeStdLib(which_stdlib_module);
		//REMOVE remember to connect all signals:
		//REMOVE a_parser->newImportSignal.connect( bind(&LangCompiler::addSourceFileAsImport, this, _1) );
		//REMOVE a_parser->searchElementInOtherModulesSignal.connect( bind(&LangCompiler::searchElementInOtherModules, this, _1, _2) );
		//REMOVE cout << "Not connecting signals in LangCompiler::createRaeStdLibModule.\n";
		stdLibModules.push_back(a_parser);
		a_parser->parseString();
	}

	void createRaeStdLibCppSupport(string which_stdlib_module)
	{
		SourceParser* a_parser = new SourceParser();
		a_parser->createRaeStdLib(which_stdlib_module);
		//REMOVE remember to connect all signals:
		//////REMOVE a_parser->newImportSignal.connect( bind(&LangCompiler::addSourceFileAsImport, this, _1) );
		//////REMOVE a_parser->searchElementInOtherModulesSignal.connect( bind(&LangCompiler::searchElementInOtherModules, this, _1, _2) );
		//////REMOVE stdLibModules.push_back(a_parser);

		sourceParsers.push_back(a_parser);
		
		a_parser->parseString();


	}

	void addModuleSearchPath(string set_path)
	{
		if( set_path.back() != '/' )
			set_path.append("/");
		moduleSearchPaths.push_back(set_path);	
	}

	void addSourceFile(string set_filename)
	{
		//REMOVE BOOST: boost::filesystem::path currentFilenamePath = set_filename;
		sourceFiles.push_back(set_filename);
	}

	//REMOVE BOOST: boost::filesystem::path findModuleFileInSearchPaths(boost::filesystem::path set)//param: e.g. "rae/examples/Tester"
	string findModuleFileInSearchPaths(string set)//param: e.g. "rae/examples/Tester.rae"
	{
		for(string a_path : moduleSearchPaths)
		{
			//boost::filesystem::path another_copy_path = a_path;
			//boost 1.55 or later required:
			//another_copy_path += set;
			//another_copy_path += ".rae";

			//for older boost:
			//string a_string_path = a_path.string() + set.string() + ".rae";
			//boost::filesystem::path another_copy_path = a_string_path;

			string another_copy_path = a_path;
			another_copy_path += set;
			// Extension needs to be added already: another_copy_path += ".rae";
			
			if( checkPathType(another_copy_path) == PathType::FILE )
			{
				return another_copy_path;//this is our path, and it exists. Atleast it was the first one...
			}

		}
		
		return "";	
	}

	//This must return a copy. Otherwise our pointer owning system will crash on destructors.
	//Every SourceParser must own their LangElements.
	LangElement* searchElementInOtherModules(SourceParser* set_parser, LangElement* set_elem)
	{
		if(set_parser == 0 || set_elem == 0)
		{
			cout<<"RAE_ERROR in LangCompiler::searchElementInOtherModules. set_parser or set_elem is null.\n";
			return 0;
		}

		#ifdef DEBUG_RAE_PARSER
		cout<<"searchElementInOtherModules START.\n";
		cout<<set_parser->moduleName()<<" is looking for "<<set_elem->toString()<<"\n";
		#endif

		LangElement* result;
		
		//first search stdlib
		for(uint i = 0; i < stdLibModules.size(); i++)
		{
			if(stdLibModules[i] != set_parser )
			{
				result = stdLibModules[i]->searchElementAndCheckIfValidLocal(set_elem);
				if(result)
				{
					#ifdef DEBUG_RAE_PARSER
					cout<<"searchElementInOtherModules FOUND from stdlib: "<<result->toString()<<" from module: "<<stdLibModules[i]->moduleName()<<"\n";;
					#endif
					return result->copy();
				}
			}
		}

		//then search user defined modules:
		for(uint i = 0; i < sourceParsers.size(); i++)
		{
			//it has to be on the list of imports in set_parser.
			if(sourceParsers[i] != set_parser && set_parser->isModuleOnListOfImports(sourceParsers[i]->moduleName()) )
			{
				result = sourceParsers[i]->searchElementAndCheckIfValidLocal(set_elem);
				if(result)
				{
					#ifdef DEBUG_RAE_PARSER
					cout<<"searchElementInOtherModules FOUND : "<<result->toString()<<" from module: "<<sourceParsers[i]->moduleName()<<"\n";;
					#endif
					return result->copy();
				}
			}
		}

		return 0;
	}

	void addSourceFileAsImport(string set_import_name)
	{
		#ifdef DEBUG_RAE_PARSER
		cout<<"Adding import name: "<<set_import_name<<"\n";
		#endif

		//REMOVE BOOST: boost::filesystem::path set_import_path = findModuleFileInSearchPaths(set_import_name);
		string set_import_path = findModuleFileInSearchPaths(set_import_name + ".rae");

		if(set_import_path != "")
		{
			cout << "Found imported module: " << set_import_path << "\n";
		}
		else cout << "Didn't find imported module: " << set_import_name << "\n";
		
		// JONDE CHECK Do we need this line:
		//addSourceFile( set_import_path );

		if(force_one_thread == false)
		{

			#if __cplusplus >= 201103L //c++11

				//thread code here
			#else
				cout<<"No multithreading because we don't have C++11. You must use force_one_thread = true.\n";
			#endif
		}
		else
		{
			SourceParser* a_parser = new SourceParser( set_import_path, /*do_parse:*/false);
			//REMOVE remember to connect all signals:
			//REMOVE a_parser->newImportSignal.connect( bind(&LangCompiler::addSourceFileAsImport, this, _1) );
			//REMOVE a_parser->searchElementInOtherModulesSignal.connect( bind(&LangCompiler::searchElementInOtherModules, this, _1, _2) );
			//REMOVE cout << "Not connecting signals. addSourceFileAsImport.\n";
			sourceParsers.push_back(a_parser);

			//thread* t1 = new thread( &SourceParser::parse, a_parser);
			//parserThreads.push_back(t1);
			
			a_parser->parse();
		}
	}

	void addCppHeaderAsImport(string set_import_name)
	{
//#ifdef DEBUG_RAE_PARSER
		cout<<"Adding C++ import name: "<<set_import_name<<"\n";
//#endif
		
		bool did_we_find_the_file = false;
		
		// Check .h
		string set_import_path = findModuleFileInSearchPaths(set_import_name + ".h");
		if(set_import_path != "")
		{
			did_we_find_the_file = true;
		}
		else // Check .hpp
		{
			set_import_path = findModuleFileInSearchPaths(set_import_name + ".h");
			if(set_import_path != "")
			{
				did_we_find_the_file = true;
			}
			else // Check .hxx
			{
				set_import_path = findModuleFileInSearchPaths(set_import_name + ".hxx");
				if(set_import_path != "")
				{
					did_we_find_the_file = true;
				}
			}
		}
		
		if(did_we_find_the_file == true)
			cout << "Found imported C++ header: " << set_import_path << "\n";
		else cout << "Didn't find imported C++ header: " << set_import_name << "\n";
		
		/////////////WHAT DOES THIS DO???: addSourceFile( set_import_path );
		
		cout << "TODO parse C++!\n";
		SourceParser* a_parser = new SourceParser( set_import_path, /*do_parse:*/false);
		a_parser->parserType(ParserType::CPP);

		string module_name = replaceCharInString(set_import_name, "/\\", '.'); // replace / or \ with a dot
		cout << "Adding C++ module name: " << module_name << "\n";
		a_parser->newModule(module_name);

		sourceParsers.push_back(a_parser);
		a_parser->parse();
	}
	
	bool parse()
	{

		#if __cplusplus >= 201103L //c++11
			force_one_thread = true; //it's not yet ready.
		#else //older c++, we don't use threads...
			force_one_thread = true;
		#endif

		if(force_one_thread == false)
		{

		#if __cplusplus >= 201103L //c++11

			//rae::log("We have sourceFiles: ", sourceFiles.size(), "\n");;

            cout<<"We have C++11, but this code is just bollocks. Rewrite this.\n";
            
			for(uint i = 0; i < sourceFiles.size(); i++)
			{
				//thread* t1 = new thread(task, "Hello " + numberToString(i));

				//rae::log("sourcefile: ", sourceFiles[i], "\n");
                
				SourceParser* a_parser = new SourceParser(sourceFiles[i], false);//false is do_parse:
				//a_parser->newImportSignal.connect( bind(&LangCompiler::addSourceFileAsImport, this, _1) );
				//a_parser->searchElementInOtherModulesSignal.connect( bind(&LangCompiler::searchElementInOtherModules, this, _1, _2) );
				sourceParsers.push_back(a_parser);

				//std::thread t1(&SomeClass::threadFunction, *this, arg1, arg2);

				thread* t1 = new thread( &SourceParser::parse, a_parser);
				parserThreads.push_back(t1);
				
				//simple threading test:
				//Foo* f = new Foo();
				//foos.push_back(f);

				//thread* t1 = new thread( &Foo::threadFunc, f);
				//parserThreads.push_back(t1);
								
			}
            /*
			foreach(thread* th, parserThreads)
			{
				//rae::log("Going to join() a thread.\n");
				if(th->joinable() )
					th->join();
			}
            */
		#else
			cout<<"No multithreading because we don't have C++11. You must use force_one_thread = true.\n";
		#endif

		}
		else //if force_one_thread
		{
			//rae::log("We have sourceFiles: ", sourceFiles.size(), "\n");;

			if(sourceFiles.size() == 0)
			{
				cout<<"No source files added.\n";
				return false;
			}
		
			for(uint i = 0; i < sourceFiles.size(); i++)
			{
				//thread* t1 = new thread(task, "Hello " + numberToString(i));

				#ifdef DEBUG_RAE_HUMAN
				cout<<"sourcefile: "<<sourceFiles[i]<<"\n";
				//rae::log("sourcefile: ", sourceFiles[i], "\n");
				#endif

				SourceParser* a_parser = new SourceParser(sourceFiles[i], /*do_parse:*/false);
				//REMOVE a_parser->newImportSignal.connect( bind(&LangCompiler::addSourceFileAsImport, this, _1) );
				//REMOVE a_parser->searchElementInOtherModulesSignal.connect( bind(&LangCompiler::searchElementInOtherModules, this, _1, _2) );
				//REMOVE cout << "Not connecting signals. parse()\n";
				sourceParsers.push_back(a_parser);

				//thread* t1 = new thread( &SourceParser::parse, a_parser);
				//parserThreads.push_back(t1);
				
				a_parser->parse();
			}
		}

		return true;
	}

	bool validate()
	{
		if(sourceParsers.size() == 0)
		{
			cout<<"No parsed sources.\n";
			return false;
		}

		bool ret = true;

		for(SourceParser* a_parser : sourceParsers)
		{
			if( a_parser->validate() == false )
			{
				ret = false;
			}
			
		}

		return ret;
	}

	//Write to default path. Which is workingPath + "/cpp/"
	bool write()
	{
		return write(workingPath + "/cpp/");
	}

	bool write(string folder_path_to_write_to)
	{
		if(sourceParsers.size() == 0)
		{
			cout<<"No parsed sources.\n";
			return false;
		}

		for(SourceParser* a_parser : sourceParsers)
		{
			a_parser->write(folder_path_to_write_to);
		}

		return true;
	}

	//we need a modulesList here...
	//and put only a single langElement in SourceParser
	
	//A list of source files that we have already parsed:
	vector<string> sourceFiles;
	//REMOVE BOOST: vector<boost::filesystem::path> sourceFiles;

	vector<SourceParser*> sourceParsers;

	//stdlib modules:
	vector<SourceParser*> stdLibModules;

#if __cplusplus >= 201103L //c++11
	vector<thread*> parserThreads;
#endif

	//vector<Foo*> foos;
};

} // end namespace Rae

#endif

