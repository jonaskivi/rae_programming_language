//this file is automatically created from Rae programming language module:
//c:\Users\jonaz\Dropbox\jonas\2015\ohjelmointi\rae_programming_language/cpp/rae/examples/HelloWorld
#ifndef _rae_examples_HelloWorld_hpp_
#define _rae_examples_HelloWorld_hpp_

#include <stdint.h>
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include "rae/link.hpp"


class Tester
{public:
	
	//-------------Rae boilerplate code-------------
		
	//link management:

	void _rae_link(rae::link<Tester>& a_link)
	{
		#ifdef RAE_DEBUG
			std::cout<<"Tester._rae_link()\n";
		#endif

		if(a_link.obj != nullptr)
		{
			std::cout<<"link Error. The link is already in use.\n";
		}

		if(_rae_link_list)// != nullptr)
		{
			//we already have a link_list (and have been linked)
			_rae_link_list->addLink(&a_link);
		}
		else
		{
			_rae_link_list = new rae::LinkList<Tester>(this, &a_link);
		}
	}

	void _rae_unlink_all()
	{
		#ifdef RAE_DEBUG
			std::cout<<"Tester._rae_unlink() by POINTER: "<<this<<" VALUE: "<<*this<<"\n";
		#endif

		if(_rae_link_list)
		{
			#ifdef RAE_DEBUG
				std::cout<<"val.unlink calling _rae_link_list->unlinkByValue."<<"\n";
			#endif

			_rae_link_list->unlinkAllByValue();
			
			delete _rae_link_list;
			_rae_link_list = nullptr;
		}
		#ifdef RAE_DEBUG
		else
		{
			std::cout<<"Tester is already unlinked since _rae_link_list is nullptr.\n";
		}
		#endif
	}

	rae::LinkList<Tester>* _rae_link_list;

	//-------------end Rae boilerplate code-------------


	Tester(); // line: 12
	~Tester(); // line: 13
public: 
	
	void log_me();
	
	int32_t data;
};


class HelloWorld
{public:
	
	//-------------Rae boilerplate code-------------
		
	//link management:

	void _rae_link(rae::link<HelloWorld>& a_link)
	{
		#ifdef RAE_DEBUG
			std::cout<<"Tester._rae_link()\n";
		#endif

		if(a_link.obj != nullptr)
		{
			std::cout<<"link Error. The link is already in use.\n";
		}

		if(_rae_link_list)// != nullptr)
		{
			//we already have a link_list (and have been linked)
			_rae_link_list->addLink(&a_link);
		}
		else
		{
			_rae_link_list = new rae::LinkList<HelloWorld>(this, &a_link);
		}
	}

	void _rae_unlink_all()
	{
		#ifdef RAE_DEBUG
			std::cout<<"Tester._rae_unlink() by POINTER: "<<this<<" VALUE: "<<*this<<"\n";
		#endif

		if(_rae_link_list)
		{
			#ifdef RAE_DEBUG
				std::cout<<"val.unlink calling _rae_link_list->unlinkByValue."<<"\n";
			#endif

			_rae_link_list->unlinkAllByValue();
			
			delete _rae_link_list;
			_rae_link_list = nullptr;
		}
		#ifdef RAE_DEBUG
		else
		{
			std::cout<<"Tester is already unlinked since _rae_link_list is nullptr.\n";
		}
		#endif
	}

	rae::LinkList<HelloWorld>* _rae_link_list;

	//-------------end Rae boilerplate code-------------


public: 
	
	//constructor:
	HelloWorld();
	
	//destructor:
	~HelloWorld();
	
	void log_int_array(std::vector<int32_t>* someints) const; // line: 30
	
	public: void say_hello(); // line: 46
	public: int32_t int_count(const int32_t param1, const int32_t param2); // line: 76
	public: int32_t tester_count( const Tester* param1,  const Tester* param2); // line: 81
	public: int32_t count(const int32_t param1,  const Tester* param2); // line: 86
	public: int32_t number();
	public: void number(int32_t set);
	protected: int32_t m_number; // default initializers are copied to constructors.
	
	
	public: const int32_t let_init_data; // line: 109
	int32_t no_init_data; // line: 110
	double no_init_datad; // line: 111
	
	protected: int32_t anotherNumber; // line: 113
	public: float afloat; // line: 114
	
	Tester tester; // val is the default type
	Tester* tester_opt; // opt is the optional type. This one is allocated automatically.
	Tester* tester_not; // need to specify that it is null, if you want it to be empty.
	rae::link<Tester> tester_link; // link is a pointer which can not allocate or free memory.
	// a non-owning pointer which is also automatically set to null 
	// when the pointed object is destroyed.
	Tester* tester_ptr; // Don't use raw pointers unless interfacing with C.
	
	std::string name; // line: 124
	
	std::vector<std::string> names; // line: 126
	
	std::vector<int32_t>* opt_array_test; // line: 128
	std::vector<int32_t>* ref_array_test; // line: 129
	std::vector<int32_t>* ptr_array_test; // line: 130
	//TODO link[int] link_array_test
	std::vector<int32_t> array_test; //std::vector<int>	
	std::array<int32_t, 5> static_array_test; //std::array<int, 5>
	std::vector<Tester > testers; // line: 134
	std::vector<Tester > defaultTesters; // line: 135
	std::vector<rae::link<Tester> > tester_links; // line: 136
	std::vector<Tester* > tester_opts; // line: 137
	std::vector<Tester* > tester_refs; // line: 138
	std::vector<Tester* > tester_ptrs;
};

#endif // _rae_examples_HelloWorld_hpp_

