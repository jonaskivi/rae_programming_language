//this file is automatically created from Rae programming language module:
///home/joonaz/Dropbox/jonas/2014/ohjelmointi/rae_programming_language/cpp/rae/examples/HelloWorld
#include "HelloWorld.hpp"
//@end
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
	afloat = 248.52;//line: 321
	anotherNumber = 42;//line: 320
	num = 5;
}

HelloWorld::~HelloWorld()
{
	//free tester_opt
}

void HelloWorld::sayHello()
{
	tester.logMe();//line: 309
	std::cout<<"World!"<<"\n";//line: 310
	std::cout<<"tester_opt: "<<"\n";//line: 311
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
	
	//Rae does not use = for pointing to an object. Instead use -> to point.
	///////////////link Tester tester_lnk2 -> hello.tester_opt
	//DOES NOT WORK ATM: 
	rae::link<Tester> tester_lnk;//line: 378
	tester_lnk.linkTo(&hello.tester);//line: 379
	
	std::cout<<"LINK saying hello:"<<"\n";//line: 381
	///////////TODO NEXT: tester_lnk.sayHello()
	
	//Should give us: tester_lnk.linkTo(&hello.tester);
	
	////////////////////////////////////link Tester tester_lnk3 -> hello.testerLink
	//ERROR: can't point to with a value type: val Tester tester_val4 -> hello.testerVal
	///////////////////////////val Tester tester_val4 = hello.testerVal
	//A link to a return value is not allowed, because val as return type is temporary:
	//link Tester tester_lnk5 -> hello.testerVal
	//log_s("link to a return value: tester_val4: ")
	//////////////////////////////tester_val4.logMe
	
	//val Tester tester_val5 -> hello.testerVal2
	
	hello.sayHello();//line: 396
	
	std::cout<<"5 + 2 = ";//line: 398
	
	//the following line will not run if tester_lnk is null.
	//NOT YET: log(hello.count(int param1: hello.num, ref Tester param2: tester_lnk?))
	
	//NOT_YET: log(hello.count( hello.num, hello.tester ))
	
	hello.tester.data = 3;//line: 405
	
	return(0);
}
