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


	name = "Hello Rae World";//line: 61
	afloat = 248.52;//line: 60
	anotherNumber = 42;//line: 58
	no_init_datad = 0.0;//line: 57
	no_init_data = 0;//line: 56
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
	std::vector<int32_t> my_numbers;//line: 33
	my_numbers.emplace_back(5);//line: 34
	my_numbers.emplace_back(6);//line: 35
	my_numbers.emplace_back(7);//line: 36
	
	for(int32_t a_num : my_numbers)
	{
		std::cout<<a_num<<"\n";
	}
	
	for(uint32_t i = 0; i < my_numbers.size(); i++)
	{
		std::cout<<i<<": "<<my_numbers.at(i)<<"\n";
	}
	
	tester.logMe();//line: 48
	std::cout<<"World!"<<"\n";
}

int32_t HelloWorld::count(int32_t param1, Tester* param2)
{
	return(param1 + param2->data);
}

int32_t main(int argc, char* const argv[])
{
	std::string helloRae = "What? Hello Rae World";//line: 89
	std::cout<<helloRae<<"\n";//line: 90
	
	HelloWorld hello; //semicolons are allowed, but not required.
	
	HelloWorld hello2;//val is default
	
	std::cout<<hello2.name<<"\n";//line: 96
	
	//Rae does not use = for pointing to an object. Instead use -> to point.
	//= operator will copy by value, so that the behaviour is consistent,
	//whether you're using pointers or values.
	rae::link<Tester> tester_lnk;//line: 101
	tester_lnk.linkTo(&hello.tester);//line: 102
	
	std::cout<<"LINK saying hello:"<<"\n";//line: 104
	tester_lnk.obj->logMe();//line: 105
	
	hello.sayHello();//line: 107
	
	std::cout<<"5 + 2 = ";//line: 109
	
	std::cout<<hello.count(hello.num, hello.tester_opt)<<"\n";//line: 111
	
	hello.tester.data = 3;//line: 113
	
	hello.array_test.push_back(9);//line: 115
	hello.array_test.push_back(7);//line: 116
	hello.array_test.push_back(4);//line: 117
	
	hello.tester_opts.push_back(hello.tester_opt);//line: 119
	
	hello.tester_links.emplace_back();//line: 121
	hello.tester_links.at(0).linkTo(&hello.tester);//line: 122
	std::cout<<"LINK in an array saying hello: "<<"\n";//line: 123
	hello.tester_links.at(0).obj->logMe();//line: 124
	std::cout<<"Ok. Said hello."<<"\n";//line: 125
	hello.tester_links.at(0).obj->data = 8;//line: 126
	std::cout<<"an eight = "<<hello.tester_links.at(0).obj->data<<"\n";
	
	hello.array_test.at(0);//line: 129
	
	std::cout<<"arrays 9 + 2 = "<<hello.count(hello.array_test.at(0), hello.tester_opts.at(0))<<"\n";//line: 131
	
	return(0);
}
