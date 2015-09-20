// this file is automatically created from Rae programming language module:
//c:\Users\Joonas\rae_programming_language/cpp/rae/examples/HelloWorld
#include "HelloWorld.hpp"
; // line: 2

namespace Rae
{ 
	
	
//class Tester

Tester::~Tester()
	{ //-------------Rae boilerplate code-------------
	_rae_unlink_all();
	//-------------end Rae boilerplate code-------------


	}

Tester::Tester(int32_t set_data)
	: data(2)
	{ //-------------Rae boilerplate code-------------
	_rae_link_list = nullptr;
	//-------------end Rae boilerplate code-------------


		data = set_data;
	}

void Tester::log_me()
	{ 
		std::cout<<"Hello ";
	}


	
} // end namespace Rae// end namespace Rae


//class HelloWorld

HelloWorld::HelloWorld()
: tester_opt( new Rae::Tester() )
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
	std::vector<int32_t> my_numbers; // line: 60
	my_numbers.emplace_back(1); // line: 61
	my_numbers.emplace_back(2); // line: 62
	my_numbers.emplace_back(3); // line: 63
	my_numbers.emplace_back(4); // line: 64
	my_numbers.emplace_back(5); // line: 65
	my_numbers.emplace_back(6); // line: 66
	my_numbers.emplace_back(7); // line: 67
	my_numbers.emplace_back(129874234985); // line: 68
	
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
		*/; // line: 82
	
	tester.log_me(); // line: 84
	std::cout<<"World!"<<"\n";
}

int32_t HelloWorld::int_count(const int32_t param1, const int32_t param2)
{ 
	return param1 + param2;
}

Rae::Tester HelloWorld::tester_count( const Rae::Tester* param1,  const Rae::Tester* param2)
{ 
	//return param1.data + param2.data
	//return param1 # TODO convert let ref to val by making a copy!
	return tester;
}

int32_t HelloWorld::count(const int32_t param1,  const Rae::Tester* param2)
{ 
	if (param2 == nullptr)//TODO oneliner if handling:
	return 0; // line: 103
	
	if (param1 > 0)// TODO is not: and param2 is not null)
	{ 
		//log("whoa.")
	}
	
	if (param1 > 0)// TODO no parentheses
	{ 
		//log("whoa.")
	}
	
	return param1 + param2->data;
}

int32_t HelloWorld::number() { return m_number; }

void HelloWorld::number(int32_t set) { m_number = set; }



int32_t main(int argc, char* const argv[])
{ 
	std::string hello_rae = "What? Hello Rae World"; // line: 164
	std::cout<<hello_rae<<"\n"; // line: 165
	
	HelloWorld hello; //semicolons are allowed, but not required.
	
	HelloWorld hello2; //val is default
	
	std::cout<<hello2.name<<"\n"; // line: 171
	
	//Rae does not use = for pointing to an object. Instead use -> to point.
	//= operator will copy by value, so that the behaviour is consistent,
	//whether you're using pointers or values.
	rae::link<Rae::Tester> tester_lnk; // line: 176
	tester_lnk.linkTo(&hello.tester); // line: 177
	
	std::cout<<"LINK saying hello:"<<"\n"; // line: 179
	tester_lnk.obj->log_me(); // line: 180
	
	std::cout<<"LINK saying hello with an alias:"<<"\n"; // line: 182
	tester_lnk.obj->log_me(); // line: 183
	
	hello.say_hello(); // line: 185
	
	std::cout<<"5 + 2 = "; // line: 187
	
	std::cout<<hello.count(hello.number(), hello.tester_opt)<<"\n"; //opt to ref
	std::cout<<hello.count(hello.number(), &hello.tester)<<"\n"; //val to ref
	std::cout<<hello.count(hello.number(), tester_lnk.obj)<<"\n"; //link to ref
	
	// Complex validation
	//NEXT: log(hello.count( hello.int_count( hello.number, (42 - 14) * 3), hello.tester_count(hello.tester, tester_lnk) ))
	// End complex validation
	
	hello.tester.data = 3; // line: 197
	
	hello.tester_opts.push_back(hello.tester_opt); // line: 199
	
	hello.tester_links.emplace_back(); // line: 201
	hello.tester_links.at(0).linkTo(&hello.tester); // line: 202
	std::cout<<"LINK in an array saying hello: "<<"\n"; // line: 203
	hello.tester_links.at(0).obj->log_me(); // line: 204
	std::cout<<"Ok. Said hello."<<"\n"; // line: 205
	hello.tester_links.at(0).obj->data = 8; // line: 206
	std::cout<<"an eight = "<<hello.tester_links.at(0).obj->data<<"\n";
	
	if (true == 1)// parentheses also allowed.
	{ 
		std::cout<<"true"<<"\n";
	}
	
	bool is_what = false; // line: 214
	bool is_other = true; // line: 215
	
	if (is_what == true || is_other == false)
	{ 
		std::cout<<"false"<<"\n";
	}
	
	; //hello.array_test[0]
	//log("arrays 9 + 2 = ", hello.count( hello.array_test[0], hello.tester_opts[0] ))
	
	/*
	#This only works with C++11 so disabling now
	[Rae.Tester] tester_stuff = [Rae.Tester(8), Rae.Tester(1)]
	*/; // line: 228
	
	std::vector<Rae::Tester > tester_stuff; // line: 230
	tester_stuff.emplace_back(8); // line: 231
	tester_stuff.emplace_back(1); // line: 232
	
	for (int32_t i = 0; i < tester_stuff.size(); ++i)
	std::cout<<i<<": "<<tester_stuff.at(i).data<<"\n"; // line: 235
	
	return 0;
}


