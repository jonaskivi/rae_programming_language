#include "Compiler.hpp"

namespace Rae
{

Compiler::Compiler(string working_directory)
{
	force_one_thread = true;

	if (working_directory == "")
	{
		char cCurrentPath[FILENAME_MAX];
		if( !GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)) )
		{
			//return errno;
		}

		cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */

		//cout << "The current working directory is " << cCurrentPath <<"\n";

		workingPath = cCurrentPath;
	}
	else workingPath = working_directory;

	addModuleSearchPath(workingPath);
	//cout << "Added current directory to module search paths: " << moduleSearchPaths.back() << "\n";
}

Compiler::~Compiler()
{
	#ifdef DEBUG_RAE_DESTRUCTORS
		cout<<"~Compiler() START."<<"\n";
	#endif
	/*
	foreach(SourceParser* pars, sourceParsers)
	{
		#ifdef DEBUG_RAE_DESTRUCTORS
			if(pars)
				cout<<"~Compiler() going to delete pars."<<"\n";
			else cout<<"~Compiler() already deleted pars.\n";
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
		cout<<"~Compiler() END."<<"\n";
	#endif
}

void Compiler::createRaeStdLib()
{
	createRaeStdLibModule("string");
	createRaeStdLibModule("array");
	createRaeStdLibCppSupport("link");
}

void Compiler::createRaeStdLibModule(string which_stdlib_module)
{
	SourceParser* a_parser = new SourceParser();
	a_parser->createRaeStdLib(which_stdlib_module);
	stdLibModules.push_back(a_parser);
	a_parser->parseString();
}

void Compiler::createRaeStdLibCppSupport(string which_stdlib_module)
{
	SourceParser* a_parser = new SourceParser();
	a_parser->createRaeStdLib(which_stdlib_module);
	sourceParsers.push_back(a_parser);
	a_parser->parseString();
}

void Compiler::addModuleSearchPath(string set_path)
{
	if( set_path.back() != '/' )
		set_path.append("/");
	moduleSearchPaths.push_back(set_path);	
}

void Compiler::addSourceFile(string set_filename)
{
	sourceFiles.push_back(set_filename);
}

// param: e.g. "rae/examples/Tester.rae"
string Compiler::findModuleFileInSearchPaths(string set)
{
	for(string a_path : moduleSearchPaths)
	{
		string another_copy_path = a_path;
		another_copy_path += set;
		
		// Extension needs to be added already: another_copy_path += ".rae";
		
		if( checkPathType(another_copy_path) == PathType::FILE )
		{
			return another_copy_path; // this is our path, and it exists. At least it was the first one...
		}

	}
	
	return "";	
}

// This must return a copy. Otherwise our pointer owning system will crash on destructors.
// Every SourceParser must own their Elements.
Element* Compiler::searchElementInOtherModules(SourceParser* set_parser, Element* set_elem)
{
	if(set_parser == 0 || set_elem == 0)
	{
		cout << "RAE_ERROR in Compiler::searchElementInOtherModules. set_parser or set_elem is null.\n";
		return 0;
	}

	#ifdef DEBUG_RAE_PARSER
	cout << "searchElementInOtherModules START.\n";
	cout << set_parser->moduleName() << " is looking for " << set_elem->toString() << "\n";
	#endif

	Element* result;
	
	//first search stdlib
	for(uint i = 0; i < stdLibModules.size(); i++)
	{
		if(stdLibModules[i] != set_parser )
		{
			result = stdLibModules[i]->searchElementAndCheckIfValidLocal(set_elem);
			if(result)
			{
				#ifdef DEBUG_RAE_PARSER
				cout << "searchElementInOtherModules FOUND from stdlib: " << result->toString() << " from module: " << stdLibModules[i]->moduleName() << "\n";;
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

void Compiler::addSourceFileAsImport(string set_import_name)
{
	#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_IMPORT)
		cout<<"Adding import name: "<<set_import_name<<"\n";
	#endif

	string set_import_path = findModuleFileInSearchPaths(set_import_name + ".rae");

	if(set_import_path != "")
	{
		#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_IMPORT)
			cout << "Found imported module: " << set_import_path << "\n";
		#endif
	}
	else
	{
		cout << "Didn't find imported module: " << set_import_name << "\n";
		assert(0);
	}
	
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
		sourceParsers.push_back(a_parser);

		// TODO threads
		//thread* t1 = new thread( &SourceParser::parse, a_parser);
		//parserThreads.push_back(t1);
		
		a_parser->parse();
	}
}

void Compiler::addCppHeaderAsImport(string set_import_name)
{
	#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_IMPORT)
		cout<<"Adding C++ import name: "<<set_import_name<<"\n";
	#endif
	
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
	{
		#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_IMPORT)
			cout << "Found imported C++ header: " << set_import_path << "\n";
		#endif
	}
	else
	{
		cout << "Didn't find imported C++ header: " << set_import_name << "\n";
		assert(0);
	}
	
	/////////////WHAT DOES THIS DO???: addSourceFile( set_import_path );
	
	SourceParser* a_parser = new SourceParser( set_import_path, /*do_parse:*/false);
	a_parser->parserType(ParserType::CPP);

	string module_name = replaceCharInString(set_import_name, "/\\", '.'); // replace / or \ with a dot
	#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_IMPORT)
		cout << "Adding C++ module name: " << module_name << "\n";
	#endif
	a_parser->createModule(module_name);

	sourceParsers.push_back(a_parser);
	a_parser->parse();
}

bool Compiler::parse()
{

	#if __cplusplus >= 201103L //c++11
		force_one_thread = true; //it's not yet ready.
	#else //older c++, we don't use threads...
		force_one_thread = true;
	#endif

	if(force_one_thread == false)
	{

	#if __cplusplus >= 201103L //c++11

        cout<<"We have C++11, but this threading code is just bollocks. Rewrite this.\n";
        
		for(uint i = 0; i < sourceFiles.size(); i++)
		{
			//thread* t1 = new thread(task, "Hello " + numberToString(i));
            
			SourceParser* a_parser = new SourceParser(sourceFiles[i], false);//false is do_parse:
			//a_parser->newImportSignal.connect( bind(&Compiler::addSourceFileAsImport, this, _1) );
			//a_parser->searchElementInOtherModulesSignal.connect( bind(&Compiler::searchElementInOtherModules, this, _1, _2) );
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
			#endif

			SourceParser* a_parser = new SourceParser(sourceFiles[i], /*do_parse:*/false);
			sourceParsers.push_back(a_parser);

			//thread* t1 = new thread( &SourceParser::parse, a_parser);
			//parserThreads.push_back(t1);
			
			a_parser->parse();
		}
	}

	return true;
}

bool Compiler::validate()
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

// Write to default path. Which is workingPath + "/cpp/"
bool Compiler::write()
{
	#if defined(DEBUG_RAE_PARSER)
		cout << "working path is: " << workingPath << "\n";
	#endif
	return write(workingPath + "/cpp/");
}

bool Compiler::write(string folder_path_to_write_to)
{
	if(sourceParsers.size() == 0)
	{
		cout<<"No parsed sources.\n";
		return false;
	}

	for(SourceParser* a_parser : sourceParsers)
	{
		#if defined(DEBUG_RAE_PARSER)
			cout<<"Going to write: " << a_parser->moduleName() << "\n";
		#endif
		a_parser->write(folder_path_to_write_to);
	}

	return true;
}

} // end namespace Rae

