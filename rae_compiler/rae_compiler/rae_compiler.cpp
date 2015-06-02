#include "rae_helpers.hpp"
#include "ReportError.hpp"
#include "LangCompiler.hpp"

namespace Rae
{
	LangCompiler* g_compiler;
}

int main (int argc, char * const argv[])
{
	Rae::ReportError::countWarnings(0);
	Rae::ReportError::countErrors(0);

	rlutil::setColor(rlutil::GREEN);
	cout<<"Rae Compiler version 0.0.1\n";
    //rae::log("Rae Compiler version 0.0.1\n");
	rlutil::setColor(rlutil::WHITE);

	if(argc <= 1)
	{
		rlutil::setColor(rlutil::RED);
		cout<<"No source files added.\n";
		rlutil::setColor(rlutil::WHITE);
		return -1;
	}

	Rae::g_compiler = new Rae::LangCompiler();

	Rae::g_compiler->createRaeStdLib();

	for(int i = 1; i < argc; ++i)
	{
		cout<<"Adding source file: "<<argv[i]<<"\n";
		Rae::g_compiler->addSourceFile(argv[i]);
	}

	Rae::g_compiler->parse();
	//TODO Rae::g_compiler->validate();
	cout << "Validate is TODO.\n";
	Rae::g_compiler->write();

	/*
	Rae::LangCompiler langCompiler;

	langCompiler.createRaeStdLib();

	for(uint i = 1; i < argc; i++)
	{
		cout<<"Adding source file: "<<argv[i]<<"\n";
		langCompiler.addSourceFile(argv[i]);
	}

	langCompiler.parse();
	langCompiler.validate();
	langCompiler.write();
	*/
	rlutil::setColor(rlutil::GREEN);
	cout<<"Rae Finished.\n";
	rlutil::setColor(rlutil::WHITE);	

	return 0;
}



/*
int main(int argc, char *argv[])
{    
	//rae::log("wahat\n");
    Foo* f = new Foo();
    f->startThread();
    f->joinIt();
    return 0;
}
*/

