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
	//free tester_opt
}

void HelloWorld::sayHello()
{
	tester.logMe();//line: 32
	std::cout<<"World!"<<"\n";//line: 33
	std::cout<<"tester_opt: "<<"\n";//line: 34
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
	Tester* tester_lnk2 = hello.tester_opt;//line: 96
	Tester* tester_lnk = &hello.tester;//line: 97
	//Should give us: Tester** tester_lnk = &hello.tester;
	
	Tester* tester_lnk3 = hello.testerLink();//line: 100
	//ERROR: can't point to with a value type: val Tester tester_val4 -> hello.testerVal
	Tester tester_val4 = hello.testerVal();//line: 102
	//A link to a return value is not allowed, because val as return type is temporary:
	//link Tester tester_lnk5 -> hello.testerVal
	std::cout<<"link to a return value: tester_val4: ";//line: 105
	tester_val4.logMe();//line: 106
	
	//val Tester tester_val5 -> hello.testerVal2
	
	hello.sayHello();//line: 110
	
	std::cout<<"5 + 2 = ";//line: 112
	
	//the following line will not run if tester_lnk is null.
	//NOT YET: log(hello.count(int param1: hello.num, ref Tester param2: tester_lnk?))
	
	//NOT_YET: log(hello.count( hello.num, hello.tester ))
	
	hello.tester.data = 3;//line: 119
	
	return(0);//line: 121
	delete tester_lnk2;//line: 90
	delete tester_lnk;//line: 90
	delete tester_lnk3;
}
