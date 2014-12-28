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


	name = "Hello Rae World";//line: 46
	afloat = 248.52;//line: 45
	anotherNumber = 42;//line: 43
	no_init_datad = 0.0;//line: 42
	no_init_data = 0;//line: 41
	num = 5;//default initializers are copied to constructors.
	tester_not = nullptr;//need to specify that it is null, if you want it to be empty.
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
	tester.logMe();//line: 33
	std::cout<<"World!"<<"\n";
}

int32_t HelloWorld::count(int32_t param1, Tester* param2)
{
	return(param1 + param2->data);
}

int32_t main(int argc, char* const argv[])
{
	int32_t eger;//line: 74
	std::cout<<eger<<"\n";//line: 75
	
	std::string helloRae;//line: 77
	std::cout<<helloRae<<"\n";//line: 78
	
	HelloWorld hello;//semicolons are allowed, but not required.
	
	HelloWorld hello2;//val is default
	
	//Rae does not use = for pointing to an object. Instead use -> to point.
	//= operator will copy by value, so that the behaviour is consistent,
	//whether you're using pointers or values.
	rae::link<Tester> tester_lnk;//line: 87
	tester_lnk.linkTo(&hello.tester);//line: 88
	
	std::cout<<"LINK saying hello:"<<"\n";//line: 90
	tester_lnk.obj->logMe();//line: 91
	
	hello.sayHello();//line: 93
	
	std::cout<<"5 + 2 = ";//line: 95
	
	std::cout<<hello.count(hello.num, hello.tester_opt)<<"\n";//line: 97
	
	hello.tester.data = 3;//line: 99
	
	hello.array_test.push_back(9);//line: 101
	hello.array_test.push_back(7);//line: 102
	hello.array_test.push_back(4);//line: 103
	
	hello.tester_opts.push_back(hello.tester_opt);//line: 105
	
	hello.tester_links.emplace_back();//line: 107
	hello.tester_links.at(0).linkTo(&hello.tester);//line: 108
	std::cout<<"LINK in an array saying hello: "<<"\n";//line: 109
	hello.tester_links.at(0).obj->logMe();//line: 110
	std::cout<<"Ok. Said hello."<<"\n";//line: 111
	hello.tester_links.at(0).obj->data = 8;//line: 112
	std::cout<<"an eight = "<<hello.tester_links.at(0).obj->data<<"\n";
	
	hello.array_test.at(0);//line: 115
	
	std::cout<<"arrays 9 + 2 = "<<hello.count(hello.array_test.at(0), hello.tester_opts.at(0))<<"\n";//line: 117
	
	return(0);
}
