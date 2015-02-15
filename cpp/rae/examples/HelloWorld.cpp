//this file is automatically created from Rae programming language module:
///Users/joonaz/Dropbox/jonas/2014/ohjelmointi/rae_programming_language/cpp/rae/examples/HelloWorld
#include "HelloWorld.hpp"

//class Tester

Tester::Tester()
{ 	
	//-------------Rae boilerplate code-------------
	_rae_link_list = nullptr;
	//-------------end Rae boilerplate code-------------


	data = 2;//line: 12
	;
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


	name = "Hello Rae World";//line: 89
	afloat = 248.52;//line: 88
	anotherNumber = 42;//line: 86
	no_init_datad = 0.0;//line: 85
	no_init_data = 0;//line: 83
	m_number = 5;// default initializers are copied to constructors.
	ref_array_test = new std::vector<int32_t>;//line: 103
	opt_array_test = new std::vector<int32_t>;//line: 93
	tester_not = nullptr;// need to specify that it is null, if you want it to be empty.
	tester_opt = new Tester();//line: 92
	;
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
	;
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
	
	//logIntArray(my_numbers)
	/*
		for(int a_num in my_numbers)
		{
			log(a_num)
		}
		*/
	/*
		for(uint i = 0, i < my_numbers.size, i++)
		{
			log(i, "= ", my_numbers[i])
		}
		*/
	
	tester.logMe();//line: 65
	std::cout<<"World!"<<"\n";
}

int32_t HelloWorld::count(int32_t param1, Tester* param2)
{
	if(param2 == nullptr)//TODO oneliner if handling:
	return(0);//line: 72
	
	if(param1 > 0)// TODO is not: and param2 is not null)
	{
		std::cout<<"whoa."<<"\n";
	}
	
	return(param1 + param2->data);
}

int32_t HelloWorld::number() { return(m_number); }

void HelloWorld::number(int32_t set) { m_number = set; }

int32_t main(int argc, char* const argv[])
{
	std::string helloRae = "What? Hello Rae World";//line: 120
	std::cout<<helloRae<<"\n";//line: 121
	
	HelloWorld hello; //semicolons are allowed, but not required.
	
	HelloWorld hello2;//val is default
	
	std::cout<<hello2.name<<"\n";//line: 127
	
	//Rae does not use = for pointing to an object. Instead use -> to point.
	//= operator will copy by value, so that the behaviour is consistent,
	//whether you're using pointers or values.
	rae::link<Tester> tester_lnk;//line: 132
	tester_lnk.linkTo(&hello.tester);//line: 133
	
	std::cout<<"LINK saying hello:"<<"\n";//line: 135
	tester_lnk.obj->logMe();//line: 136
	
	hello.sayHello();//line: 138
	
	std::cout<<"5 + 2 = ";//line: 140
	
	std::cout<<hello.count(hello.number(), hello.tester_opt)<<"\n";//line: 142
	
	hello.tester.data = 3;//line: 144
	
	hello.tester_opts.push_back(hello.tester_opt);//line: 146
	
	hello.tester_links.emplace_back();//line: 148
	hello.tester_links.at(0).linkTo(&hello.tester);//line: 149
	std::cout<<"LINK in an array saying hello: "<<"\n";//line: 150
	hello.tester_links.at(0).obj->logMe();//line: 151
	std::cout<<"Ok. Said hello."<<"\n";//line: 152
	hello.tester_links.at(0).obj->data = 8;//line: 153
	std::cout<<"an eight = "<<hello.tester_links.at(0).obj->data<<"\n";
	
	if(true == 1)
	{
		std::cout<<"true"<<"\n";
	}
	
	bool isWhat = false;//line: 161
	bool isOther = true;//line: 162
	
	if(isWhat == true || isOther == true)
	{
		std::cout<<"false"<<"\n";
	}
	
	//hello.array_test[0]
	//log("arrays 9 + 2 = ", hello.count( hello.array_test[0], hello.tester_opts[0] ))
	
	return(0);
}
