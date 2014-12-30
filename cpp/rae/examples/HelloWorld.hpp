//this file is automatically created from Rae programming language module:
///home/joonaz/Dropbox/jonas/2014/ohjelmointi/rae_programming_language/cpp/rae/examples/HelloWorld
#ifndef _rae_examples_HelloWorld_hpp_
#define _rae_examples_HelloWorld_hpp_

#include <stdint.h>
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include "rae/link.hpp"


class Tester
{
public:
	
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


	public: Tester();//line: 13
	~Tester();//line: 13
public: 
	
	void logMe();//line: 8
	int32_t data;
};


class HelloWorld
{
public:
	
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
	HelloWorld();//line: 21
	//destructor:
	~HelloWorld();//line: 26
	void doesnotwork(std::vector<int32_t> someints);//line: 30
	public: void sayHello();//line: 37
	public: int32_t count(int32_t param1, Tester* param2);//line: 59
	int32_t num;//line: 64
	int32_t no_init_data;//line: 65
	double no_init_datad;//line: 66
	
	protected: int32_t anotherNumber;//line: 68
	public: float afloat;//line: 69
	
	Tester tester;// val is the default type
	Tester* tester_opt;// opt is the optional type. This one is allocated automatically.
	Tester* tester_not;//line: 73
	rae::link<Tester> tester_link;// link is a pointer which can not allocate or free memory.
	// a non-owning pointer which is also automatically set to null 
	// when the pointed object is destroyed.
	Tester* tester_ptr;// Don't use raw pointers unless interfacing with C.
	
	std::string name;//line: 79
	
	std::vector<std::string> names;//line: 81
	
	std::vector<int32_t> array_test;//std::vector<int>	
	std::array<int32_t, 5> static_array_test;//std::array<int, 5>
	std::vector<Tester > testers;//line: 85
	std::vector<Tester > defaultTesters;//line: 86
	std::vector<rae::link<Tester> > tester_links;//line: 87
	std::vector<Tester* > tester_opts;//line: 88
	std::vector<Tester* > tester_refs;//line: 89
	std::vector<Tester* > tester_ptrs;
};

#endif // _rae_examples_HelloWorld_hpp_

