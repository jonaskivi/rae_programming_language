//this file is automatically created from Rae programming language module:
//c:\Users\jonaz\Dropbox\jonas\2015\ohjelmointi\rae_programming_language/cpp/rae/examples/HelloWorld
#include "HelloWorld.hpp"

//class Tester

Tester::Tester()
: data(2)
{ //-------------Rae boilerplate code-------------
	_rae_link_list = nullptr;
	//-------------end Rae boilerplate code-------------


}

Tester::~Tester()
{ //-------------Rae boilerplate code-------------
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
{ //-------------Rae boilerplate code-------------
	_rae_link_list = nullptr;
	//-------------end Rae boilerplate code-------------


}

HelloWorld::~HelloWorld()
{ //-------------Rae boilerplate code-------------
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
	
	//TODO NEXT log_int_array(my_numbers)
	
	for (int32_t a_num : my_numbers)
	{
		std::cout<<a_num<<"\n";
	}
	
	/*
		loop( uint i = 0, i < my_numbers.size, ++i )
		{
			log(i, "= ", my_numbers[i])
		}
		*/
	
	/*possible error, unknown token:*/tester->/*possible error, unknown token:*/log_me; // line: 72
	std::cout<<"World!"<<"\n";
}

int32_t HelloWorld::int_count(const int32_t param1, const int32_t param2)
{
	return param1 + param2;
}

int32_t HelloWorld::tester_count( const Tester* param1,  const Tester* param2)
{
	return param1->data + param2->data;
}

int32_t HelloWorld::count(const int32_t param1,  const Tester* param2)
{
	if (param2 == nullptr)//TODO oneliner if handling:
	return 0; // line: 89
	
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

int32_t HelloWorld::number() { return /*possible error, unknown token:*/m_number; }

void HelloWorld::number(int32_t set) { /*possible error, unknown token:*/m_number = set; }

int32_t main(int argc, char* const argv[])
{
	std::string hello_rae = "What? Hello Rae World"; // line: 144
	std::cout<<hello_rae<<"\n"; // line: 145
	
	HelloWorld hello; //semicolons are allowed, but not required.
	
	HelloWorld hello2; //val is default
	
	std::cout<<hello2.name<<"\n"; // line: 151
	
	//Rae does not use = for pointing to an object. Instead use -> to point.
	//= operator will copy by value, so that the behaviour is consistent,
	//whether you're using pointers or values.
	rae::link<Tester> tester_lnk; // line: 156
	tester_lnk.linkTo(&hello.tester); // line: 157
	
	std::cout<<"LINK saying hello:"<<"\n"; // line: 159
	tester_lnk.obj->log_me(); // line: 160
	
	hello.say_hello(); // line: 162
	
	std::cout<<"5 + 2 = "; // line: 164
	
	std::cout<<hello.count(hello.number(), hello.tester_opt)<<"\n"; // opt to ref
	std::cout<<hello.count(hello.number(), hello.tester)<<"\n"; // val to ref
	std::cout<<hello.count(hello.number(), tester_lnk)<<"\n"; // link to ref
	
	// Complex validation
	std::cout<<hello.count(hello.int_count(hello.number(), (42 - 14) *  * 3), hello.tester_count(hello.tester, tester_lnk))<<"\n"; // line: 171
	// End complex validation
	
	hello.tester.data = 3; // line: 174
	
	hello.tester_opts.push_back(hello.tester_opt); // line: 176
	
	hello.tester_links.emplace_back(); // line: 178
	hello.tester_links.at(0).linkTo(&hello.tester); // line: 179
	std::cout<<"LINK in an array saying hello: "<<"\n"; // line: 180
	hello.tester_links.at(0).obj->log_me(); // line: 181
	std::cout<<"Ok. Said hello."<<"\n"; // line: 182
	hello.tester_links.at(0).obj->data = 8; // line: 183
	std::cout<<"an eight = "<<hello.tester_links.at(0).obj->data<<"\n";
	
	if (true == 1)// parentheses also allowed.
	{
		std::cout<<"true"<<"\n";
	}
	
	bool is_what = false; // line: 191
	bool is_other = true; // line: 192
	
	if (is_what == true || is_other == true)
	{
		std::cout<<"false"<<"\n";
	}
	
	//hello.array_test[0]
	//log("arrays 9 + 2 = ", hello.count( hello.array_test[0], hello.tester_opts[0] ))
	
	return 0;
}
