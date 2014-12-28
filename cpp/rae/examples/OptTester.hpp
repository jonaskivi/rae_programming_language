//this file is automatically created from Rae programming language module:
///home/joonaz/Dropbox/jonas/2014/ohjelmointi/rae_programming_language/cpp/rae/examples/OptTester
#ifndef _rae_examples_OptTester_hpp_
#define _rae_examples_OptTester_hpp_

#include <stdint.h>
#include <iostream>
#include <string>
#include <vector>
#include "rae/link.hpp"


class Inner
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

	void _rae_link(rae::link<Inner>& a_link)
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
			_rae_link_list = new rae::LinkList<Inner>(this, &a_link);
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

	rae::LinkList<Inner>* _rae_link_list;

	//-------------end Rae boilerplate code-------------


	public: Inner();//line: 12
	~Inner();//line: 12
public: 
	void logMe();//line: 7
	int32_t data;
};


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


	public: Tester();//line: 31
	~Tester();//line: 31
public: 
	void logMe();//line: 19
	int32_t data;//line: 24
	
	
	rae::link<Inner> inner();//line: 27
	protected: Inner* m_inner;
};


class OptTester
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

	void _rae_link(rae::link<OptTester>& a_link)
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
			_rae_link_list = new rae::LinkList<OptTester>(this, &a_link);
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

	rae::LinkList<OptTester>* _rae_link_list;

	//-------------end Rae boilerplate code-------------


	public: OptTester();//line: 73
public: 
	
	void new();//line: 39
	~OptTester();//line: 43
	public: void sayHello();//line: 48
	public: int32_t count(int32_t param1, int32_t param2);//line: 56
	int32_t num;//line: 61
	uint32_t num25;//line: 62
	protected: int64_t anotherNumber;//line: 63
	public: float afloat;//line: 64
	double initMeToo;//line: 65
	int32_t azero;//line: 66
	
	Tester tester;//line: 68
	
	Tester* tester_ref;//line: 70
	
	Tester* tester_opt;//line: 72
	
};

#endif // _rae_examples_OptTester_hpp_

