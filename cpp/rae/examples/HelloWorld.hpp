//this file is automatically created from Rae programming language module:
///Users/joonaz/Dropbox/jonas/2014/ohjelmointi/rae_programming_language/cpp/rae/examples/HelloWorld
#ifndef _rae_examples_HelloWorld_hpp_
#define _rae_examples_HelloWorld_hpp_

#include <stdint.h>
#include <iostream>
#include <string>
#include <vector>
#include "rae/link.hpp"


class Tester
{
public:
	
	//-------------Rae boilerplate code-------------
	/*
	//locking
	bool _rae_lock()
	{
		_m_rae_lock = true;
	}
	void _rae_unlock()
	{
		_m_rae_lock = false;
	}
	protected: bool _m_rae_lock;
	*/
public:
	
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
	int32_t dataINDATA;
};


class HelloWorld
{
public:
	
	//-------------Rae boilerplate code-------------
	/*
	//locking
	bool _rae_lock()
	{
		_m_rae_lock = true;
	}
	void _rae_unlock()
	{
		_m_rae_lock = false;
	}
	protected: bool _m_rae_lock;
	*/
public:
	
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
	HelloWorld();//line: 22
	//destructor:
	~HelloWorld();//line: 27
	public: void sayHello();//line: 33
	public: int32_t count(int32_t param1, Tester* param2);//line: 42
	int32_t numINDATA;//line: 47
	protected: int64_t anotherNumberINDATA;//line: 48
	public: float afloatINDATA;//line: 49
	
	//REMOVE FROM HELLO
	/*
RETURN THIS RIGHT AFTER IT COMPILES:
	func (val Tester)testerVal()
	{
		return(tester)
	}

	func (link Tester)testerLink()
	{
		return(tester_opt)
	}
*/
	/*func (val Tester)testerVal2()
	{
		return(tester_opt)
	}*/
	
	//
	/*
	[Tester] testers //Swift

	//A related note: template syntax with brackets?
	vector[Tester] testers
	signal[void, int, float] mouseClicked

	link [val Tester] linkToArray

	//Signals syntax uses the func syntax:
	signal (bool result, int other)mouseClicked(ref Tester a_tester, int a_value)

	//Am I wrong if I think that signals can be used in all cases where you'd use a deledate?
	//They work differently, but a signal is just more flexible.
	delegate (bool result, int other)mouseClicked(ref Tester a_tester, int a_value)
*/
	
	//REMOVE
	
	Tester tester;//line: 89
	
	Tester* tester_opt;//= new Tester
	
	Tester* tester_notINDATA;//line: 93
	
	rae::link<Tester> tester_link;
};

#endif // _rae_examples_HelloWorld_hpp_

