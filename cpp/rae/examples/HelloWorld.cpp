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
	afloat = 248.52;//line: 44
	anotherNumber = 42;//line: 43
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
	//NOT YET: tester_opt?.logMe()
	tester_opt->logMe();
}

int32_t HelloWorld::count(int32_t param1, Tester* param2)
{
	return(param1 + param2->data);
}

int32_t main(int argc, char* const argv[])
{
	HelloWorld hello; //semicolons are allowed, but not required.
	
	//NOT YET: link Tester tester_lnk = hello.tester
	
	hello.sayHello();//line: 60
	
	std::cout<<"5 + 2 = ";//line: 62
	
	//the following line will not run if tester_lnk is null.
	//NOT YET: log(hello.count(int param1: hello.num, ref Tester param2: tester_lnk?))
	
	//NOT_YET: log(hello.count( hello.num, hello.tester ))
	
	hello.tester.data = 3;//line: 69
	
	return(0);
}
