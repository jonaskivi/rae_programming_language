//this file is automatically created from Rae programming language module:
///Users/joonaz/Dropbox/jonas/2014/ohjelmointi/rae_programming_language/cpp/rae/examples/HelloWorld
#include "HelloWorld.hpp"

//class Tester

Tester::Tester()
{
	data = 2;
}

Tester::~Tester()
{
}

void Tester::logMe()
{
	std::cout<<"Hello ";
}


//class HelloWorld

HelloWorld::HelloWorld()
{
	afloat = 248.52;//line: 43
	anotherNumber = 42;//line: 42
	num = 5;
}

HelloWorld::~HelloWorld()
{
}

void HelloWorld::sayHello()
{
	tester.logMe();//line: 31
	std::cout<<"World!"<<"\n";//line: 32
	
	std::cout<<"tester_opt: "<<"\n";//line: 34
	/*possible error, unknown token:*/tester_opt?->/*possible error, unknown token:*/logMe();
}

//return type name: result
int32_t HelloWorld::count(int32_t param1,  param2)
{
	return(param1 + param2->/*possible error, unknown token:*/data);
}

//return type name: 
int32_t main(int argc, char* const argv[])//semicolons are allowed, but not required.
"5 + 2 = ";//the following line will not run if tester_lnk is null.

