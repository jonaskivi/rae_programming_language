//this file is automatically created from Rae programming language module:
///home/joonaz/Dropbox/jonas/2014/ohjelmointi/rae_programming_language/cpp/rae/link
#ifndef _rae_link_hpp_
#define _rae_link_hpp_

#include <stdint.h>
#include <iostream>
#include <string>
#include <vector>
#include "rae/link.hpp"

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
#endif // _rae_link_hpp_

