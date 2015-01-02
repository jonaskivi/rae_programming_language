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


	name = "Hello Rae World";//line: 78
	afloat = 248.52;//line: 77
	anotherNumber = 42;//line: 75
	no_init_datad = 0.0;//line: 74
	no_init_data = 0;//line: 73
	num = 5;//default initializers are copied to constructors.
	ref_array_test = new std::vector<int32_t>;//line: 92
	opt_array_test = new std::vector<int32_t>;//line: 82
	tester_not = nullptr;// need to specify that it is null, if you want it to be empty.
	tester_opt = new Tester();
}

HelloWorld::~HelloWorld()
{	
	//-------------Rae boilerplate code-------------
	_rae_unlink_all();
	//-------------end Rae boilerplate code-------------


	if(ref_array_test != nullptr ) { delete ref_array_test; ref_array_test = nullptr; }
	if(opt_array_test != nullptr ) { delete opt_array_test; opt_array_test = nullptr; }
	if(tester_not != nullptr ) { delete tester_not; tester_not = nullptr; }
	if(tester_opt != nullptr ) { delete tester_opt; tester_opt = nullptr; }
}

void HelloWorld::logIntArray(std::vector<int32_t>* someints)
{
	for(int32_t a_num : (*someints))
	{
		std::cout<<a_num<<"\n";
	}
}

void HelloWorld::sayHello()
{
	std::vector<int32_t> my_numbers;//line: 41
	my_numbers.emplace_back(1);//line: 42
	my_numbers.emplace_back(2);//line: 43
	my_numbers.emplace_back(3);//line: 44
	my_numbers.emplace_back(4);//line: 45
	my_numbers.emplace_back(5);//line: 46
	my_numbers.emplace_back(6);//line: 47
	my_numbers.emplace_back(7);//line: 48
	my_numbers.emplace_back(129874234985);//line: 49
	
	logIntArray(my_numbers);//line: 51
	/*
		for(int a_num in my_numbers)
		{
			log(a_num)
		}
		*/
	/*
		for(uint i = 0; i < my_numbers.size; i++)
		{
			log(i, ": ", my_numbers[i])
		}
		*/
	
	tester.logMe();//line: 65
	std::cout<<"World!"<<"\n";
}

int32_t HelloWorld::count(int32_t param1, Tester* param2)
{
	return(param1 + param2->data);
}

int32_t main(int argc, char* const argv[])
{
	std::string helloRae = "What? Hello Rae World";//line: 109
	std::cout<<helloRae<<"\n";//line: 110
	
	HelloWorld hello; //semicolons are allowed, but not required.
	
	HelloWorld hello2;//val is default
	
	std::cout<<hello2.name<<"\n";//line: 116
	
	//Rae does not use = for pointing to an object. Instead use -> to point.
	//= operator will copy by value, so that the behaviour is consistent,
	//whether you're using pointers or values.
	rae::link<Tester> tester_lnk;//line: 121
	tester_lnk.linkTo(&hello.tester);//line: 122
	
	std::cout<<"LINK saying hello:"<<"\n";//line: 124
	tester_lnk.obj->logMe();//line: 125
	
	hello.sayHello();//line: 127
	
	std::cout<<"5 + 2 = ";//line: 129
	
	std::cout<<hello.count(hello.num, hello.tester)<<"\n";//line: 131
	
	hello.tester.data = 3;//line: 133
	
	hello.array_test.push_back(9);//line: 135
	hello.array_test.push_back(7);//line: 136
	hello.array_test.push_back(4);//line: 137
	
	hello.tester_opts.push_back(hello.tester_opt);//line: 139
	
	hello.tester_links.emplace_back();//line: 141
	hello.tester_links.at(0).linkTo(&hello.tester);//line: 142
	std::cout<<"LINK in an array saying hello: "<<"\n";//line: 143
	hello.tester_links.at(0).obj->logMe();//line: 144
	std::cout<<"Ok. Said hello."<<"\n";//line: 145
	hello.tester_links.at(0).obj->data = 8;//line: 146
	std::cout<<"an eight = "<<hello.tester_links.at(0).obj->data<<"\n";
	
	hello.array_test.at(0);//line: 149
	
	std::cout<<"arrays 9 + 2 = "<<hello.count(hello.array_test.at(0), hello.tester_opts.at(0))<<"\n";//line: 151
	
	return(0);
}
