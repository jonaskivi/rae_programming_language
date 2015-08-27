// this file is automatically created from Rae programming language module:
///Users/joonaz/Dropbox/jonas/2015/ohjelmointi/rae_programming_language/cpp/rae/examples/HelloWorld
#ifndef _rae_examples_HelloWorld_hpp_
#define _rae_examples_HelloWorld_hpp_

#include <stdint.h>
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include "rae/link.hpp"



namespace Rae
{ 
	
	
class Tester
	{ public:
	
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


		~Tester(); // line: 21
	public: 
		Tester(int32_t set_data = 5); // line: 20
		
		void log_me();
		
		int32_t data;
	};


	
} // end namespace Rae// end namespace Rae


class HelloWorld
{ public:
	
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
	
	void log_int_array(std::vector<int32_t>* someints) const ; // line: 40
	
	public: void say_hello(); // line: 56
	public: int32_t int_count(const int32_t param1, const int32_t param2); // line: 86
	public: Rae::Tester tester_count( const Rae::Tester* param1,  const Rae::Tester* param2); // line: 91
	public: int32_t count(const int32_t param1,  const Rae::Tester* param2); // line: 98
	public: int32_t number();
	public: void number(int32_t set);
	protected: int32_t m_number; // default initializers are copied to constructors.
	
	
	public: const int32_t let_init_data; // line: 121
	int32_t no_init_data; // line: 122
	double no_init_datad; // line: 123
	
	protected: int32_t anotherNumber; // line: 125
	public: float afloat; // line: 126
	
	Rae::Tester tester; // val is the default type
	Rae::Tester* tester_opt; // opt is the optional type. This one is allocated automatically.
	Rae::Tester* tester_not; // need to specify that it is null, if you want it to be empty.
	rae::link<Rae::Tester> tester_link; // link is a pointer which can not allocate or free memory.
	// a non-owning pointer which is also automatically set to null 
	// when the pointed object is destroyed.
	Rae::Tester* tester_ptr; // Don't use raw pointers unless interfacing with C.
	
	std::string name; // line: 136
	
	std::vector<std::string> names; // line: 138
	
	//syntaksi testailua:
	//const Rae::Tester* tester;
	//def tester{let opt Rae.Tester}
	//[let opt Rae.Tester tester]
	
	
	std::vector<int32_t>* opt_array_test; // line: 146
	std::vector<int32_t>* ref_array_test; // line: 147
	std::vector<int32_t>* ptr_array_test; // line: 148
	//TODO link[int] link_array_test
	std::vector<int32_t> array_test; //std::vector<int>	
	std::array<int32_t, 5> static_array_test; //std::array<int, 5>
	std::vector<Rae::Tester > testers; // line: 152
	std::vector<Rae::Tester > default_testers; // line: 153
	std::vector<rae::link<Rae::Tester> > tester_links; // line: 154
	std::vector<Rae::Tester* > tester_opts; // line: 155
	std::vector<Rae::Tester* > tester_refs; // line: 156
	std::vector<Rae::Tester* > tester_ptrs;
};



#endif // _rae_examples_HelloWorld_hpp_

