//this file is automatically created from Rae programming language module:
///home/joonaz/Dropbox/jonas/2014/ohjelmointi/rae_programming_language/cpp/rae/examples/HelloWorld
#ifndef _rae_examples_HelloWorld_hpp_
#define _rae_examples_HelloWorld_hpp_

#include <stdint.h>
#include <iostream>
#include <string>
#include <vector>

//@c++

namespace rae
{

using namespace std;

//write c++ code here...
template<typename T> class LinkList;

template<typename T> class link
{
public:

	link(T* link_me)
	: obj(nullptr)
	{
		#ifdef RAE_DEBUG
			cout<<"link.new().\n";
		#endif

		linkTo(link_me);
	}

	link()
	: obj(nullptr)
	{
		#ifdef RAE_DEBUG
			cout<<"link.new().\n";
		#endif
	}

	~link()
	{
		#ifdef RAE_DEBUG
			cout<<"link.free().\n";
		#endif

		unlink();
	}

	bool isValid()
	{
		if(obj != nullptr)
			return true;
		//else
		return false;
	}

	void linkTo( T* link_me )//, LinkList<T>* set_link_list )
	{
		//This is set later by T._rae_link()
		//obj = &link_me;
		
		link_me->_rae_link(*this);
	}
	
	void unlink()
	{
		#ifdef RAE_DEBUG
			cout<<"link._rae_unlink() just one:";
			if(obj) cout<<*obj<<"\n"; else cout<<"nullptr\n";
		#endif
		
		if(obj)
		{
			if(obj->_rae_link_list)
			{
				#ifdef RAE_DEBUG
					cout<<"link._rae_unlink() Should call link_list.unlinkJustOneLink().\n";
				#endif
				obj->_rae_link_list->unlinkJustOneLink(this);
			}
			#ifdef RAE_DEBUG
			else
			{
				cout<<"link._rae_unlink() Didn't call link_list.unlinkJustOneLink() because _rae_link_list is nullptr.\n";
			}
			#endif
			obj = nullptr;
		}
		#ifdef RAE_DEBUG
		else
		{
			cout<<"link._rae_unlink() we were already unlinked since obj is nullptr.\n";
		}
		#endif
	}

	void unlinkByValue()
	{
		#ifdef RAE_DEBUG
			cout<<"link.unlinkByValue()\n";
		#endif

		obj = nullptr;
	}

	T* obj;
};

template<typename T> class LinkList
{
public:

	LinkList(T* set_val, link<T>* set_first_link)
	{
		#ifdef RAE_DEBUG
			cout<<"LinkList.new() linklist POINTER: "<<this<<"\nobj POINTER: "<<set_val<<"\n";
		#endif
		obj = set_val;
		addLink(set_first_link);
	}

	~LinkList()
	{
		#ifdef RAE_DEBUG
			cout<<"LinkList.free():";
			if(obj) cout<<*obj<<"\n"; else cout<<"already freed: nullptr\n";
			cout<<"also the size of links is: "<<links.size()<<"\n";
		#endif
	}

	//use this just to update the objects address that
	//was passed when creating this LinkList.
	void updateObject( T* set_object )
	{
		obj = set_object;
		for(auto* a_link : links)
		{
			//we'll take the risk of not checking for null here:
			a_link->obj = set_object;
		}
	}
	
	void addLink( link<T>* link_me )
	{
		#ifdef RAE_DEBUG
			cout<<"LinkList.addLink():";
			if(obj) cout<<*obj<<"\n"; else cout<<"nullptr\n";

			assert(obj != nullptr);
		#endif

		link_me->obj = obj;

		links.push_back(link_me);
	}

	void unlinkAllByValue()
	{
		#ifdef RAE_DEBUG
			cout<<"LinkList.unlinkByValue():";
			if(obj) cout<<*obj<<"\n"; else cout<<"nullptr\n";
		#endif
		for(link<T>* a_link : links)
		{
			a_link->unlinkByValue();
		}
		links.clear();
		obj = nullptr;
	}

	void unlinkJustOneLink(link<T>* rem)
	{
		#ifdef RAE_DEBUG
			cout<<"LinkList.unlinkJustOneLink():";
			if(obj) cout<<*obj<<"\n"; else cout<<"nullptr\n";
		#endif

		if(links.empty())
		{
			#ifdef RAE_DEBUG
			cout<<"ERROR: LinkList.unlinkJustOneLink() links was already empty. Cancelled removing it.\n";
			#endif
			return;
		}
		
		/*
		//Too C++11 for my linux install:
		auto it = std::find(links.begin(), links.end(), rem);
		if(it != links.end())
		{
			links.erase(it);
		}
		*/
		for(int i = 0; i < links.size(); i++)
		{
			if(links[i] == rem)
			{
				links.erase( links.begin() + i );
				break;
			}
		}
	}
	
	T* obj;
	vector<link<T>*> links;
};

}//end namespace rae

//@end
class Tester
{
	public: Tester();//line: 287
	~Tester();//line: 287
public: 
	
	//@c++
	
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
			//RectangleVal_rae_manager.link_lists.emplace_back(this, &a_link);
			//RectangleVal_rae_manager.createFirstLink(*this, a_link);
			//_rae_create_first_link(a_link);

			//C++14:
			//_rae_link_list = std::make_unique<LinkList<RectangleVal>>(this, &a_link);
			//C++11:
			//_rae_link_list.reset( new LinkList<RectangleVal>(this, &a_link) );
			//C++98: it has to be a raw pointer, and we must delete it later:
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
			
			//RectangleVal_rae_manager.removeLinkList(_rae_link_list);

			//C++11:
			//_rae_link_list.reset();//destroy link list
			//C++98:
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

	//@end;//line: 279
	
	
	void logMe();//line: 282
	int32_t data;
};


class HelloWorld
{
public: 
	
	//constructor:
	HelloWorld();//line: 296
	//destructor:
	~HelloWorld();//line: 301
	public: void sayHello();//line: 307
	public: int32_t count(int32_t param1, Tester* param2);//line: 316
	int32_t num;//line: 321
	protected: int64_t anotherNumber;//line: 322
	public: float afloat;//line: 323
	
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
	
	Tester tester;//line: 363
	
	Tester* tester_opt;//= new Tester
	
	rae::link<Tester> tester_link;
};

#endif // _rae_examples_HelloWorld_hpp_

