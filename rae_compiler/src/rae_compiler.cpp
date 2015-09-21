#include "rae_helpers.hpp"
#include "ReportError.hpp"
#include "Compiler.hpp"

namespace Rae
{
	Compiler* g_compiler;
}

int main (int argc, char * const argv[])
{
	Rae::ReportError::countWarnings(0);
	Rae::ReportError::countErrors(0);

	rlutil::setColor(rlutil::GREEN);
	cout<<"Rae Compiler version 0.0.4\n";
	rlutil::setColor(rlutil::WHITE);

	string debug_filename = "";
	string debug_working_dir = "";
	//string debug_filename = "/Users/joonaz/Dropbox/jonas/2015/ohjelmointi/rae_programming_language/rae/examples/test1.rae";
	//string debug_working_dir = "/Users/joonaz/Dropbox/jonas/2015/ohjelmointi/rae_programming_language/";

	if(argc <= 1 && debug_filename == "")
	{
		rlutil::setColor(rlutil::RED);
		cout<<"No source files added.\n";
		rlutil::setColor(rlutil::WHITE);
		return -1;
	}

	Rae::g_compiler = new Rae::Compiler(debug_working_dir);

	Rae::g_compiler->createRaeStdLib();

	if (debug_filename == "")
	{
		for(int i = 1; i < argc; ++i)
		{
			cout<<"Adding source file: "<<argv[i]<<"\n";
			Rae::g_compiler->addSourceFile(argv[i]);
		}
	}
	else
	{
		cout<<"Adding DEBUG source file: "<<debug_filename<<"\n";
		Rae::g_compiler->addSourceFile(debug_filename);
	}

	Rae::g_compiler->parse();
	Rae::g_compiler->validate();
	//cout << "Validate is TODO.\n";
	Rae::g_compiler->write();

	rlutil::setColor(rlutil::GREEN);
	cout<<"Rae Finished.\n";
	rlutil::setColor(rlutil::WHITE);	

	return 0;
}

