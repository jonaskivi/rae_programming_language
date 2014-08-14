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
	afloat = 248.52;//line: 45
	anotherNumber = 42;//line: 44
	num = 5;
}

HelloWorld::~HelloWorld()
{
	//free tester_opt
}

void HelloWorld::sayHello()
{
	tester.logMe();//line: 32
	std::cout<<"World!"<<"\n";//line: 33
	
	std::cout<<"tester_opt: "<<"\n";//line: 35
	//NOT YET: tester_opt?.logMe()
	tester_opt->logMe();
}

int32_t HelloWorld::count(int32_t param1, Tester* param2)
{
	return(param1 + param2->data);
}

Tester HelloWorld::testerVal()
{
	return(tester);
}

Tester* HelloWorld::testerLink()
{
	return(tester_opt);
}

int32_t main(int argc, char* const argv[])
{
	HelloWorld hello; //semicolons are allowed, but not required.
	
	//Rae does not use = for pointing to an object. Instead use -> to point.
	Tester* tester_lnk2 = hello.tester_opt;//line: 79
	Tester* tester_lnk = &hello.tester;//line: 80
	//Should give us: Tester** tester_lnk = &hello.tester;
	
	Tester* tester_lnk3 = hello.testerLink();//line: 83
	Tester tester_val4 = hello.testerVal();//line: 84
	//val Tester tester_val5 -> hello.testerVal2
	
	hello.sayHello();//line: 87
	
	std::cout<<"5 + 2 = ";//line: 89
	
	//the following line will not run if tester_lnk is null.
	//NOT YET: log(hello.count(int param1: hello.num, ref Tester param2: tester_lnk?))
	
	//NOT_YET: log(hello.count( hello.num, hello.tester ))
	
	hello.tester.data = 3;//line: 96
	
	return(0);//line: 98
	delete tester_lnk2;//line: 73
	delete tester_lnk;//line: 73
	delete tester_lnk3;
}
