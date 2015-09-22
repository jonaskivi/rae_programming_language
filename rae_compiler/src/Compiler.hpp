#ifndef RAE_COMPILER_LANGCOMPILER_HPP
#define RAE_COMPILER_LANGCOMPILER_HPP

#include <stdio.h> // for FILENAME_MAX
#ifdef _WIN32
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
 #endif

#include "Element.hpp"
#include "StringFileWriter.hpp"
#include "SourceParser.hpp"
#include "ReportError.hpp"

namespace Rae
{

class Compiler
{
public:
	Compiler(string working_directory = "");
	~Compiler();

	void createRaeStdLib();
	void createRaeStdLibModule(string which_stdlib_module);
	void createRaeStdLibCppSupport(string which_stdlib_module);
	
	void addModuleSearchPath(string set_path);
	void addSourceFile(string set_filename);

	string findModuleFileInSearchPaths(string set);

	// This must return a copy. Otherwise our pointer owning system will crash on destructors.
	// Every SourceParser must own their Elements.
	Element* searchElementInOtherModules(SourceParser* set_parser, Element* set_elem);
	
	void addSourceFileAsImport(string set_import_name);
	void addCppHeaderAsImport(string set_import_name);
	
	bool parse();

	bool validate();

	// Write to default path. Which is workingPath + "/cpp/"
	bool write();
	bool write(string folder_path_to_write_to);

protected:
	string         workingPath;
	vector<string> moduleSearchPaths;

	//we need a modulesList here...
	
	//A list of source files that we have already parsed:
	vector<string> sourceFiles;

	vector<SourceParser*> sourceParsers;

	//stdlib modules:
	vector<SourceParser*> stdLibModules;

	bool force_one_thread;
#if __cplusplus >= 201103L //c++11
	vector<thread*> parserThreads; // TODO unused for now.
#endif
};

} // end namespace Rae

#endif

