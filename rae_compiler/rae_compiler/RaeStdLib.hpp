
void injectClassBoilerPlate( StringFileWriter& writer, LangElement& set_elem)
{
	//string linkHandlingBoilerplate;
	//linkHandlingBoilerplate = 

	string our_class_name = set_elem.parentClassName();

	writer.writeString(R"delimITER(
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

	void _rae_link(rae::link<)delimITER");

	writer.writeString(our_class_name);

	writer.writeString(R"delimITER(>& a_link)
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
			_rae_link_list = new rae::LinkList<)delimITER");

	writer.writeString(our_class_name);

	writer.writeString(R"delimITER(>(this, &a_link);
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

	rae::LinkList<)delimITER");

	writer.writeString(our_class_name);

	writer.writeString(R"delimITER(>* _rae_link_list;

	//-------------end Rae boilerplate code-------------

)delimITER");

}

void injectConstructorBoilerPlate( StringFileWriter& writer, LangElement& set_elem)
{
	//string our_class_name = set_elem.parentClassName();

	writer.writeString(R"delimITER(	
	//-------------Rae boilerplate code-------------
	_rae_link_list = nullptr;
	//-------------end Rae boilerplate code-------------

)delimITER");
}

void injectDestructorBoilerPlate( StringFileWriter& writer, LangElement& set_elem)
{
	//string our_class_name = set_elem.parentClassName();

	writer.writeString(R"delimITER(	
	//-------------Rae boilerplate code-------------
	_rae_unlink_all();
	//-------------end Rae boilerplate code-------------

)delimITER");
}

	void createRaeStdLib(string which_stdlib_class)
	{
		stringIndex = 0;
		isWriteToFile = false;
		isFileBased = false;

		//newClass("string");

		//init();

		//fileParsedOk = true;

		//static const char texture_fragment_shader_color[] 
		
		if(which_stdlib_class == "string")
		{
			#ifdef DEBUG_RAE_HUMAN
			cout<<"createRaeStdLib string!\n";
			#endif

			stringBasedSourceText =
			"module rae.std.string\n"
			"\n"
			"class string\n"
			"{\n"
				"\tfunc ()empty(){}\n"
				"\tfunc ()push_back(){}\n" //void push_back (const value_type& val);
				"\tfunc ()pop_back(){}\n"
				"\tfunc ()size(){}\n" //size_type size() const;

				"\tfunc ()append(){}\n"
				"\tfunc ()assign(){}\n"
				"\tfunc ()at(){}\n"
				"\tfunc ()back(){}\n"
				"\tfunc ()begin(){}\n"
				"\tfunc ()capacity(){}\n"
				"\tfunc ()cbegin(){}\n"
				"\tfunc ()cend(){}\n"
				"\tfunc ()clear(){}\n"
				"\tfunc ()compare(){}\n"
				"\tfunc ()copy(){}\n"
				"\tfunc ()crbegin(){}\n"
				"\tfunc ()crend(){}\n"
				"\tfunc ()c_str(){}\n"
				"\tfunc ()data(){}\n"
				//empty at the beginning!
				"\tfunc ()end(){}\n"
				"\tfunc ()erase(){}\n"
				"\tfunc ()find(){}\n"
				"\tfunc ()find_first_not_of(){}\n"
				"\tfunc ()find_first_of(){}\n"
				"\tfunc ()find_last_not_of(){}\n"
				"\tfunc ()find_last_of(){}\n"
				"\tfunc ()front(){}\n"
				"\tfunc ()get_allocator(){}\n"
				"\tfunc ()insert(){}\n"
				"\tfunc ()length(){}\n"
				"\tfunc ()max_size(){}\n"
				//operator+=
				//operator=
				//opeartor[]
				//pop_back yes at the beginning!
				//push_back
				"\tfunc ()rbegin(){}\n"
				"\tfunc ()rend(){}\n"
				"\tfunc ()replace(){}\n"
				"\tfunc ()reserve(){}\n"
				"\tfunc ()resize(){}\n"
				"\tfunc ()rfind(){}\n"
				"\tfunc ()shrink_to_fit(){}\n"
				//size
				"\tfunc ()substr(){}\n"
				"\tfunc ()swap(){}\n"
			"}\n"
			;
		}
		else if(which_stdlib_class == "array")
		{
			#ifdef DEBUG_RAE_HUMAN
			cout<<"createRaeStdLib array\n";
			#endif

			stringBasedSourceText =
			"module rae.std.array\n"
			"\n"
			"class array\n"
			"{\n"
				"\tfunc ()empty(){}\n"
				"\tfunc ()push_back(){}\n" //void push_back (const value_type& val);
				"\tfunc ()pop_back(){}\n"
				"\tfunc ()size(){}\n" //size_type size() const;

				"\tfunc ()assign(){}\n"
				"\tfunc ()at(){}\n"
				"\tfunc ()back(){}\n"
				"\tfunc ()begin(){}\n"
				"\tfunc ()capacity(){}\n"
				"\tfunc ()cbegin(){}\n"
				"\tfunc ()cend(){}\n"
				"\tfunc ()clear(){}\n"
				"\tfunc ()crbegin(){}\n"
				"\tfunc ()crend(){}\n"
				"\tfunc ()data(){}\n"
				"\tfunc ()emplace(){}\n"
				"\tfunc ()emplace_back(){}\n"
				//empty at the beginning!
				"\tfunc ()end(){}\n"
				"\tfunc ()erase(){}\n"
				"\tfunc ()front(){}\n"
				"\tfunc ()get_allocator(){}\n"
				"\tfunc ()insert(){}\n"
				"\tfunc ()max_size(){}\n"
				//operator=
				//operator[]
				//pop_back at the beginning!
				//push_back
				"\tfunc ()rbegin(){}\n"
				"\tfunc ()rend(){}\n"
				"\tfunc ()reserve(){}\n"
				"\tfunc ()resize(){}\n"
				"\tfunc ()shrink_to_fit(){}\n"

				"\tfunc ()swap(){}\n"
			"}\n"
			;
		}
		else if(which_stdlib_class == "link")
		{
			#ifdef DEBUG_RAE_HUMAN
			cout<<"createRaeStdLib link.\n";
			#endif

stringBasedSourceText = R"delimITER(
module rae.link

@c++

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

@end

)delimITER";
		}
	}


