//this file is automatically created from Rae programming language module:
///Users/joonaz/Dropbox/jonas/2014/ohjelmointi/rae_programming_language/cpp/rae/examples/HelloWorld
#include "HelloWorld.hpp"

//class Tester

Tester::Tester()
{	
	//-------------Rae boilerplate code-------------
	_rae_link_list = nullptr;
	//-------------end Rae boilerplate code-------------


	data = 2;
}

Tester::~Tester()
{	
	//-------------Rae boilerplate code-------------
	_rae_unlink_all();
	//-------------end Rae boilerplate code-------------


}

void Tester::logMe()
{
	std::cout<<"Hello ";
}


//class HelloWorld

HelloWorld::HelloWorld()
{	
	//-------------Rae boilerplate code-------------
	_rae_link_list = nullptr;
	//-------------end Rae boilerplate code-------------


	what_if_im_removed = 1;//line: 45
	afloat = 248.52;//line: 44
	anotherNumber = 42;//line: 42
	num = 5;//default initializers are copied to constructors.
	;//line: 51
	tester_not = nullptr;//need to specify that it is null, if you want it to be empty.
	;//line: 49
	tester_opt = new Tester();
}

HelloWorld::~HelloWorld()
{	
	//-------------Rae boilerplate code-------------
	_rae_unlink_all();
	//-------------end Rae boilerplate code-------------


	if(tester_not != nullptr ) { delete tester_not; tester_not = nullptr; }
	if(tester_opt != nullptr ) { delete tester_opt; tester_opt = nullptr; }
}

void HelloWorld::sayHello()
{
	tester.logMe();//line: 34
	std::cout<<"World!"<<"\n";
}

int32_t HelloWorld::count(int32_t param1, Tester* param2)
{
	return(param1 + param2->data);
}

int32_t main(int argc, char* const argv[])
{
	HelloWorld hello;//semicolons are allowed, but not required.
	
	//Rae does not use = for pointing to an object. Instead use -> to point.
	//= operator will copy by value, so that the behaviour is consistent,
	//whether you're using pointers or values.
	rae::link<Tester> tester_lnk;//line: 69
	tester_lnk.linkTo(&hello.tester);//line: 70
	
	std::cout<<"LINK saying hello:"<<"\n";//line: 72
	tester_lnk.obj->logMe();//line: 73
	
	hello.sayHello();//line: 75
	
	std::cout<<"5 + 2 = ";//line: 77
	
	std::cout<<hello.count(hello.num, hello.tester_opt)<<"\n";//line: 79
	
	hello.tester.data = 3;//line: 81
	
	return(0);
}
