

//namespace Rae
//{

	/*
	#include "rae_inject01.hpp"
	#include "rae_inject02.hpp"
	#include "rae_inject03.hpp"
	#include "rae_inject04.hpp"
	#include "rae_inject05.hpp"
	#include "rae_inject06.hpp"
	#include "rae_inject07.hpp"
	*/


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


	void injectClassBoilerPlate(StringFileWriter& writer, LangElement& set_elem)
	{
		//string linkHandlingBoilerplate;
		//linkHandlingBoilerplate = 

		string our_class_name = set_elem.parentClassName();

		writer.writeString(
			"public:\n"
			"\t\n"
			"\t//-------------Rae boilerplate code-------------\n"
			"\t\t\n"
			"\t//link management:\n"
			"\n"
			"\tvoid _rae_link(rae::link<");

		writer.writeString(our_class_name);

		writer.writeString(">& a_link)\n"
			"\t{\n"
			"\t\t#ifdef RAE_DEBUG\n"
			"\t\t\tstd::cout<<\"Tester._rae_link()\\n\";\n"
			"\t\t#endif\n"
			"\n"
			"\t\tif(a_link.obj != nullptr)\n"
			"\t\t{\n"
			"\t\t\tstd::cout<<\"link Error. The link is already in use.\\n\";\n"
			"\t\t}\n"
			"\n"
			"\t\tif(_rae_link_list)// != nullptr)\n"
			"\t\t{\n"
			"\t\t\t//we already have a link_list (and have been linked)\n"
			"\t\t\t_rae_link_list->addLink(&a_link);\n"
			"\t\t}\n"
			"\t\telse\n"
			"\t\t{\n"
			"\t\t\t_rae_link_list = new rae::LinkList<");

		writer.writeString(our_class_name);

		writer.writeString(">(this, &a_link);\n"
			"\t\t}\n"
			"\t}\n"
			"\n"
			"\tvoid _rae_unlink_all()\n"
			"\t{\n"
			"\t\t#ifdef RAE_DEBUG\n"
			"\t\t\tstd::cout<<\"Tester._rae_unlink() by POINTER: \"<<this<<\" VALUE: \"<<*this<<\"\\n\";\n"
			"\t\t#endif\n"
			"\n"
			"\t\tif(_rae_link_list)\n"
			"\t\t{\n"
			"\t\t\t#ifdef RAE_DEBUG\n"
			"\t\t\t\tstd::cout<<\"val.unlink calling _rae_link_list->unlinkByValue.\"<<\"\\n\";\n"
			"\t\t\t#endif\n"
			"\n"
			"\t\t\t_rae_link_list->unlinkAllByValue();\n"
			"\t\t\t\n"
			"\t\t\tdelete _rae_link_list;\n"
			"\t\t\t_rae_link_list = nullptr;\n"
			"\t\t}\n"
			"\t\t#ifdef RAE_DEBUG\n"
			"\t\telse\n"
			"\t\t{\n"
			"\t\t\tstd::cout<<\"Tester is already unlinked since _rae_link_list is nullptr.\\n\";\n"
			"\t\t}\n"
			"\t\t#endif\n"
			"\t}\n"
			"\n"
			"\trae::LinkList<");

		writer.writeString(our_class_name);

		writer.writeString(
			">* _rae_link_list;\n"
			"\n"
			"\t//-------------end Rae boilerplate code-------------\n"
			"\n"
			"");

	}

	void injectConstructorBoilerPlate(StringFileWriter& writer, LangElement& set_elem)
	{
		//string our_class_name = set_elem.parentClassName();

		writer.writeString(
			"//-------------Rae boilerplate code-------------\n"
			"\t_rae_link_list = nullptr;\n"
			"\t//-------------end Rae boilerplate code-------------\n"
			"\n"
			"");
	}

	void injectDestructorBoilerPlate(StringFileWriter& writer, LangElement& set_elem)
	{
		//string our_class_name = set_elem.parentClassName();

		writer.writeString(
			"//-------------Rae boilerplate code-------------\n"
			"\t_rae_unlink_all();\n"
			"\t//-------------end Rae boilerplate code-------------\n"
			"\n"
			"");
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

		if (which_stdlib_class == "string")
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
		else if (which_stdlib_class == "array")
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
		else if (which_stdlib_class == "link")
		{
#ifdef DEBUG_RAE_HUMAN
			cout<<"createRaeStdLib link.\n";
#endif

			stringBasedSourceText = "module rae.link\n"
				"\n"
				"@c++\n"
				"\n"
				"namespace rae\n"
				"{\n"
				"\n"
				"using namespace std;\n"
				"\n"
				"//write c++ code here...\n"
				"template<typename T> class LinkList;\n"
				"\n"
				"template<typename T> class link\n"
				"{\n"
				"public:\n"
				"\n"
				"\tlink(T* link_me)\n"
				"\t: obj(nullptr)\n"
				"\t{\n"
				"\t\t#ifdef RAE_DEBUG\n"
				"\t\t\tcout<<\"link.new().\\n\";\n"
				"\t\t#endif\n"
				"\n"
				"\t\tlinkTo(link_me);\n"
				"\t}\n"
				"\n"
				"\tlink()\n"
				"\t: obj(nullptr)\n"
				"\t{\n"
				"\t\t#ifdef RAE_DEBUG\n"
				"\t\t\tcout<<\"link.new().\\n\";\n"
				"\t\t#endif\n"
				"\t}\n"
				"\n"
				"\t~link()\n"
				"\t{\n"
				"\t\t#ifdef RAE_DEBUG\n"
				"\t\t\tcout<<\"link.free().\\n\";\n"
				"\t\t#endif\n"
				"\n"
				"\t\tunlink();\n"
				"\t}\n"
				"\n"
				"\tbool isValid()\n"
				"\t{\n"
				"\t\tif(obj != nullptr)\n"
				"\t\t\treturn true;\n"
				"\t\t//else\n"
				"\t\treturn false;\n"
				"\t}\n"
				"\n"
				"\tvoid linkTo( T* link_me )//, LinkList<T>* set_link_list )\n"
				"\t{\n"
				"\t\t//This is set later by T._rae_link()\n"
				"\t\t//obj = &link_me;\n"
				"\t\t\n"
				"\t\tlink_me->_rae_link(*this);\n"
				"\t}\n"
				"\t\n"
				"\tvoid unlink()\n"
				"\t{\n"
				"\t\t#ifdef RAE_DEBUG\n"
				"\t\t\tcout<<\"link._rae_unlink() just one:\";\n"
				"\t\t\tif(obj) cout<<*obj<<\"\\n\"; else cout<<\"nullptr\\n\";\n"
				"\t\t#endif\n"
				"\t\t\n"
				"\t\tif(obj)\n"
				"\t\t{\n"
				"\t\t\tif(obj->_rae_link_list)\n"
				"\t\t\t{\n"
				"\t\t\t\t#ifdef RAE_DEBUG\n"
				"\t\t\t\t\tcout<<\"link._rae_unlink() Should call link_list.unlinkJustOneLink().\\n\";\n"
				"\t\t\t\t#endif\n"
				"\t\t\t\tobj->_rae_link_list->unlinkJustOneLink(this);\n"
				"\t\t\t}\n"
				"\t\t\t#ifdef RAE_DEBUG\n"
				"\t\t\telse\n"
				"\t\t\t{\n"
				"\t\t\t\tcout<<\"link._rae_unlink() Didn\'t call link_list.unlinkJustOneLink() because _rae_link_list is nullptr.\\n\";\n"
				"\t\t\t}\n"
				"\t\t\t#endif\n"
				"\t\t\tobj = nullptr;\n"
				"\t\t}\n"
				"\t\t#ifdef RAE_DEBUG\n"
				"\t\telse\n"
				"\t\t{\n"
				"\t\t\tcout<<\"link._rae_unlink() we were already unlinked since obj is nullptr.\\n\";\n"
				"\t\t}\n"
				"\t\t#endif\n"
				"\t}\n"
				"\n"
				"\tvoid unlinkByValue()\n"
				"\t{\n"
				"\t\t#ifdef RAE_DEBUG\n"
				"\t\t\tcout<<\"link.unlinkByValue()\\n\";\n"
				"\t\t#endif\n"
				"\n"
				"\t\tobj = nullptr;\n"
				"\t}\n"
				"\n"
				"\tT* obj;\n"
				"};\n"
				"\n"
				"template<typename T> class LinkList\n"
				"{\n"
				"public:\n"
				"\n"
				"\tLinkList(T* set_val, link<T>* set_first_link)\n"
				"\t{\n"
				"\t\t#ifdef RAE_DEBUG\n"
				"\t\t\tcout<<\"LinkList.new() linklist POINTER: \"<<this<<\"\\nobj POINTER: \"<<set_val<<\"\\n\";\n"
				"\t\t#endif\n"
				"\t\tobj = set_val;\n"
				"\t\taddLink(set_first_link);\n"
				"\t}\n"
				"\n"
				"\t~LinkList()\n"
				"\t{\n"
				"\t\t#ifdef RAE_DEBUG\n"
				"\t\t\tcout<<\"LinkList.free():\";\n"
				"\t\t\tif(obj) cout<<*obj<<\"\\n\"; else cout<<\"already freed: nullptr\\n\";\n"
				"\t\t\tcout<<\"also the size of links is: \"<<links.size()<<\"\\n\";\n"
				"\t\t#endif\n"
				"\t}\n"
				"\n"
				"\t//use this just to update the objects address that\n"
				"\t//was passed when creating this LinkList.\n"
				"\tvoid updateObject( T* set_object )\n"
				"\t{\n"
				"\t\tobj = set_object;\n"
				"\t\tfor(auto* a_link : links)\n"
				"\t\t{\n"
				"\t\t\t//we\'ll take the risk of not checking for null here:\n"
				"\t\t\ta_link->obj = set_object;\n"
				"\t\t}\n"
				"\t}\n"
				"\t\n"
				"\tvoid addLink( link<T>* link_me )\n"
				"\t{\n"
				"\t\t#ifdef RAE_DEBUG\n"
				"\t\t\tcout<<\"LinkList.addLink():\";\n"
				"\t\t\tif(obj) cout<<*obj<<\"\\n\"; else cout<<\"nullptr\\n\";\n"
				"\n"
				"\t\t\tassert(obj != nullptr);\n"
				"\t\t#endif\n"
				"\n"
				"\t\tlink_me->obj = obj;\n"
				"\n"
				"\t\tlinks.push_back(link_me);\n"
				"\t}\n"
				"\n"
				"\tvoid unlinkAllByValue()\n"
				"\t{\n"
				"\t\t#ifdef RAE_DEBUG\n"
				"\t\t\tcout<<\"LinkList.unlinkByValue():\";\n"
				"\t\t\tif(obj) cout<<*obj<<\"\\n\"; else cout<<\"nullptr\\n\";\n"
				"\t\t#endif\n"
				"\t\tfor(link<T>* a_link : links)\n"
				"\t\t{\n"
				"\t\t\ta_link->unlinkByValue();\n"
				"\t\t}\n"
				"\t\tlinks.clear();\n"
				"\t\tobj = nullptr;\n"
				"\t}\n"
				"\n"
				"\tvoid unlinkJustOneLink(link<T>* rem)\n"
				"\t{\n"
				"\t\t#ifdef RAE_DEBUG\n"
				"\t\t\tcout<<\"LinkList.unlinkJustOneLink():\";\n"
				"\t\t\tif(obj) cout<<*obj<<\"\\n\"; else cout<<\"nullptr\\n\";\n"
				"\t\t#endif\n"
				"\n"
				"\t\tif(links.empty())\n"
				"\t\t{\n"
				"\t\t\t#ifdef RAE_DEBUG\n"
				"\t\t\tcout<<\"RAE_ERROR: LinkList.unlinkJustOneLink() links was already empty. Cancelled removing it.\\n\";\n"
				"\t\t\t#endif\n"
				"\t\t\treturn;\n"
				"\t\t}\n"
				"\t\t\n"
				"\t\t/*\n"
				"\t\t//Too C++11 for my linux install:\n"
				"\t\tauto it = std::find(links.begin(), links.end(), rem);\n"
				"\t\tif(it != links.end())\n"
				"\t\t{\n"
				"\t\t\tlinks.erase(it);\n"
				"\t\t}\n"
				"\t\t*/\n"
				"\t\tfor(int i = 0; i < links.size(); i++)\n"
				"\t\t{\n"
				"\t\t\tif(links[i] == rem)\n"
				"\t\t\t{\n"
				"\t\t\t\tlinks.erase( links.begin() + i );\n"
				"\t\t\t\tbreak;\n"
				"\t\t\t}\n"
				"\t\t}\n"
				"\t}\n"
				"\t\n"
				"\tT* obj;\n"
				"\tvector<link<T>*> links;\n"
				"};\n"
				"\n"
				"}//end namespace rae\n"
				"\n"
				"@end\n"
				"\n"
				"";
		}
	}

//}
