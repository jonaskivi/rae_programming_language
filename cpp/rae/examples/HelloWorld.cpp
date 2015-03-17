//this file is automatically created from Rae programming language module:
///Users/joonaz/Dropbox/jonas/2014/ohjelmointi/rae_programming_language/cpp/rae/examples/HelloWorld
#include "HelloWorld.hpp"

//class Tester

Tester::Tester()
: data(2)
{ 	
	//-------------Rae boilerplate code-------------
	_rae_link_list = nullptr;
	//-------------end Rae boilerplate code-------------


}

Tester::~Tester()
{ 	
	//-------------Rae boilerplate code-------------
	_rae_unlink_all();
	//-------------end Rae boilerplate code-------------


}

void Tester::log_me()
{
	std::cout<<"Hello ";
}


//class HelloWorld

HelloWorld::HelloWorld()
: tester_opt( new Tester() )
, tester_not(nullptr)
, opt_array_test( new std::vector<int32_t> )
, ref_array_test( new std::vector<int32_t> )
, m_number(5)
, let_init_data(42)
, no_init_data(0)
, no_init_datad(0.0)
, anotherNumber(42)
, afloat(248.52)
, name("Hello Rae World")
{ 	
	//-------------Rae boilerplate code-------------
	_rae_link_list = nullptr;
	//-------------end Rae boilerplate code-------------


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

void HelloWorld::log_int_array(std::vector<int32_t>* someints) const
{
	// foreach loop
	for (int32_t a_num : (*someints))
	{
		std::cout<<"foreach: "<<a_num<<"\n";
	}
	
	// for loop
	for (uint32_t i = 0; i < (*someints).size(); ++i)
	{
		std::cout<<"for: "<<(*someints).at(i)<<"\n";
	}
}

void HelloWorld::say_hello()
{
	std::vector<int32_t> my_numbers; // line: 48
	my_numbers.emplace_back(1); // TODO rename emplace_back(1) to create(1)
	my_numbers.emplace_back(2); // line: 50
	my_numbers.emplace_back(3); // line: 51
	my_numbers.emplace_back(4); // line: 52
	my_numbers.emplace_back(5); // line: 53
	my_numbers.emplace_back(6); // line: 54
	my_numbers.emplace_back(7); // line: 55
	my_numbers.emplace_back(129874234985); // line: 56
	
	//NEXT TODO log_int_array(my_numbers)
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
	
	tester.log_me(); // line: 72
	std::cout<<"World!"<<"\n";
}

int32_t HelloWorld::count(const int32_t param1,  const Tester* param2)
{
	if (param2 == nullptr)//TODO oneliner if handling:
	return 0; // line: 79
	
	if (param1 > 0)// TODO is not: and param2 is not null)
	{
		std::cout<<"whoa."<<"\n";
	}
	
	if (param1 > 0)// TODO no parentheses
	{
		std::cout<<"whoa."<<"\n";
	}
	
	return param1 + param2->data;
}

int32_t HelloWorld::number() { return m_number; }

void HelloWorld::number(int32_t set) { m_number = set; }

int32_t main(int argc, char* const argv[])
{
	std::string hello_rae = "What? Hello Rae World"; // line: 134
	std::cout<<hello_rae<<"\n"; // line: 135
	
	HelloWorld hello; //semicolons are allowed, but not required.
	
	HelloWorld hello2; //val is default
	
	std::cout<<hello2.name<<"\n"; // line: 141
	
	//Rae does not use = for pointing to an object. Instead use -> to point.
	//= operator will copy by value, so that the behaviour is consistent,
	//whether you're using pointers or values.
	rae::link<Tester> tester_lnk; // line: 146
	tester_lnk.linkTo(&hello.tester); // line: 147
	
	std::cout<<"LINK saying hello:"<<"\n"; // line: 149
	tester_lnk.obj->log_me(); // line: 150
	
	hello.say_hello(); // line: 152
	
	std::cout<<"5 + 2 = "; // line: 154
	
	std::cout<<hello.count(hello.number(), hello.tester_opt)<<"\n"; // line: 156
	
	hello.tester.data = 3; // line: 158
	
	hello.tester_opts.push_back(hello.tester_opt); // line: 160
	
	hello.tester_links.emplace_back(); // line: 162
	hello.tester_links.at(0).linkTo(&hello.tester); // line: 163
	std::cout<<"LINK in an array saying hello: "<<"\n"; // line: 164
	hello.tester_links.at(0).obj->log_me(); // line: 165
	std::cout<<"Ok. Said hello."<<"\n"; // line: 166
	hello.tester_links.at(0).obj->data = 8; // line: 167
	std::cout<<"an eight = "<<hello.tester_links.at(0).obj->data<<"\n";
	
	if (true == 1)// parentheses also allowed.
	{
		std::cout<<"true"<<"\n";
	}
	
	bool is_what = false; // line: 175
	bool is_other = true; // line: 176
	
	if (is_what == true || is_other == true)
	{
		std::cout<<"false"<<"\n";
	}
	
	//hello.array_test[0]
	//log("arrays 9 + 2 = ", hello.count( hello.array_test[0], hello.tester_opts[0] ))
	
	return 0;
}
