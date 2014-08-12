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

HelloWorld::publicint32_t /*possible error, unknown token:*/count(int32_t Tester* )
{
	return(param1 + param2->data);
}

rae.examples.HelloWorld::int32_t  = main(std::string [/*possible error, unknown token:*/args)
{
	HelloWorld hello; //semicolons are allowed, but not required.
	
	Tester* tester_lnk = hello.tester;//line: 57
	
	hello.sayHello();//line: 59
	
	std::cout<<"5 + 2 = ";//line: 61
	
	//the following line will not run if tester_lnk is null.
	std::cout<<hello./*possible error, unknown token:*/count(hello.num, Tester* param2/*possible error, unknown token:*/tester_lnk?)<<"\n";//line: 64
	
	hello.tester.data = 3;//line: 66
	
	return(0);//line: 68
	delete tester_lnk;//line: 52
	delete param2;
}
