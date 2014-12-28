//this file is automatically created from Rae programming language module:
///home/joonaz/Dropbox/jonas/2014/ohjelmointi/rae_programming_language/cpp/rae/examples/HelloWorld
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


	afloat = 248.52;//line: 51
	anotherNumber = 42;//line: 45
	no_init_datad = 0.0;//line: 44
	no_init_data = 0;//line: 42
	num = 5;//default initializers are copied to constructors.
	;//line: 66
	tester_not = nullptr;//need to specify that it is null, if you want it to be empty.
	;//line: 64
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
	rae::link<Tester> tester_lnk;//line: 84
	tester_lnk.linkTo(&hello.tester);//line: 85
	
	std::cout<<"LINK saying hello:"<<"\n";//line: 87
	tester_lnk.obj->logMe();//line: 88
	
	hello.sayHello();//line: 90
	
	std::cout<<"5 + 2 = ";//line: 92
	
	std::cout<<hello.count(hello.num, hello.tester_opt)<<"\n";//line: 94
	
	hello.tester.data = 3;//line: 96
	
	hello.array_test.push_back(9);//line: 98
	hello.array_test.push_back(7);//line: 99
	hello.array_test.push_back(4);//line: 100
	
	hello.tester_opts.push_back(hello.tester_opt);//line: 102
	
	hello.tester_links.emplace_back();//line: 104
	hello.tester_links.at(0).linkTo(&hello.tester);//line: 105
	std::cout<<"LINK in an array saying hello: "<<"\n";//line: 106
	hello.tester_links.at(0).obj->logMe();//line: 107
	std::cout<<"Ok. Said hello."<<"\n";//line: 108
	
	hello.array_test.at(0);//line: 110
	
	std::cout<<"arrays 9 + 2 = "<<hello.count(hello.array_test.at(0), hello.tester_opts.at(0))<<"\n";//line: 112
	
	return(0);
}
