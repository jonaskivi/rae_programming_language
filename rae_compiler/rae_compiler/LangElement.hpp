

namespace Role
{
enum e
{
	UNDEFINED,
	GLOBAL,
	INSIDE_CLASS,
	FUNC_RETURN,
	FUNC_PARAMETER,
	INSIDE_FUNCTION
};	
}

namespace TypeType
{
enum e
{
	UNDEFINED,
	VAL,
	REF,
	OPT,
	LINK,
	PTR,
	BUILT_IN_TYPE,
	//ARRAY,
	VECTOR,
	TEMPLATE
};

string toString(TypeType::e set)
{
	switch(set)
	{
		default:
		return "ERROR TypeType:: not recognized.";
		case TypeType::UNDEFINED:
		return "TypeType::UNDEFINED";
		case TypeType::VAL:
		return "TypeType::VAL";
		case TypeType::REF:
		return "TypeType::REF";
		case TypeType::OPT:
		return "TypeType::OPT";
		case TypeType::LINK:
		return "TypeType::LINK";
		case TypeType::PTR:
		return "TypeType::PTR";
		case TypeType::BUILT_IN_TYPE:
		return "TypeType::BUILT_IN_TYPE";
		//case TypeType::ARRAY:
		//return "TypeType::ARRAY";
		case TypeType::VECTOR:
		return "TypeType::VECTOR";
		case TypeType::TEMPLATE:
		return "TypeType::TEMPLATE";
	}
}
}

namespace Token
{
enum e
{
	UNDEFINED,
	EMPTY, //intentionally empty
	PASSTHROUGH,
	ERROR,
	MODULE,
	MODULE_NAME,
	CLOSE_MODULE,
	IMPORT,
	IMPORT_NAME,
	CLASS,
	//use together with TypeType
	//to have e.g. CLASS TypeType::TEMPLATE subtype that behaves differently.
	CLASS_NAME,
	//just something for expectinToken:
	CLASS_TEMPLATE_SECOND_TYPE, //class FirstType(SecondType)
	DEFINE_REFERENCE,//SomeClass aNewObject
	//use together with TypeType to have different kind of
	//DEFINE_REFERENCEs.

	//DEFINE_REFERENCE_IN_CLASS,//the same as above inside a class definition.
	DEFINE_REFERENCE_NAME,
	USE_REFERENCE,//aNewObject.callSomething //the first word here...
	//actually also the second word if it's not a func.
	//like in aNewObject.someNumber
	//Maybe we need to create more types like
	USE_MEMBER, //for someThing.someMember but not func. 
	
	//UNKNOWN, //generally unknown, we use UNKNOWN_USE_REFERENCE for this.
	//UNKNOWN_DEFINITION, //Tester tester
	//UNKNOWN_USE_REFERENCE,//tester
	//UNKNOWN_USE_MEMBER, //tester.useMember
	//an unknown use of member, which will be either a FUNC_CALL or USE_REFERENCE (NOT USED ATM:or USE_BUILT_IN_TYPE)

	//DEFINE_ARRAY, //SomeClass[] an_array
	//DEFINE_ARRAY_IN_CLASS,
	//////////DEFINE_C_ARRAY_NAME,
	//////USE_ARRAY,

	//DEFINE_VECTOR_IN_CLASS,
	//DEFINE_VECTOR,//vector!(typeName) -> vector<typeName>* name = new vector...
	ARRAY_VECTOR_STUFF, //SomeClass[] someVector is a vector, same as:
	VECTOR_STUFF, //vector!(SomeClass)
	VECTOR_NAME, //the name of the vector. vector!(type) name
	///////USE_VECTOR,

	TEMPLATE_STUFF,//the definition of using a template e.g. someTemplate!(typeName) objectName
	TEMPLATE_NAME,
	TEMPLATE_SECOND_TYPE,// firstType!(secondType) name

	//DEFINE_BUILT_IN_TYPE,
	//DEFINE_BUILT_IN_TYPE_IN_CLASS,
	DEFINE_BUILT_IN_TYPE_NAME,
	//USE_BUILT_IN_TYPE, //We are using USE_REFERENCE instead of this for now...

	//REAL_IN_CLASS, //biggest floating point type, maybe 80 bits or something...
	NUMBER,
	NUMBER_AFTER_DOT,

	REFERENCE_DOT,//the dot when using references and calling funcs and members.
	DOT,//do we need dot in other contexts? Yes, in floating point numbers!
	
	NEW,
	FREE,
	FREE_NAME,
	//OBJECT_
	AUTO_INIT,
	//OBJECT_
	AUTO_FREE,
	//ARRAY_AUTO_INIT,
	//ARRAY_AUTO_FREE,
	//VECTOR_AUTO_INIT,
	//VECTOR_AUTO_FREE,
	//TEMPLATE_AUTO_INIT,
	//TEMPLATE_AUTO_FREE,
	//BUILT_IN_TYPE_AUTO_INIT,
	INIT_DATA,

	VISIBILITY_DEFAULT,
	VISIBILITY,
	VISIBILITY_PARENT_CLASS,

	OVERRIDE, //for the override keyword, that let's you redefine stuff with a same name.

	FUNC_DEFINITION,
	FUNC,
	FUNC_NAME,
	DEFINE_FUNC_RETURN,
	FUNC_RETURN_TYPE,
	FUNC_RETURN_NAME,
	DEFINE_FUNC_ARGUMENT,//refactor... this is for built-in-types and class references...
	//so why do we need them separate when using DEFINE_REFERENCE and DEFINE_BUILT_IN_TYPE.
	//and then we also got DEFINE_ARRAY and DEFINE_VECTOR
	//sounds like we could use an enum like
	//TypeOfTheType
	//reference, built-in, vector, array, etc. new types...
	//that could replace the isBuiltInType thing in LangElement.
	//DEFINE_FUNC_ARGUMENT_ARRAY,//Uh, we should refactor... 
	FUNC_ARGUMENT_TYPE,
	FUNC_ARGUMENT_NAME,
	FUNC_CALL, //or should it be USE_FUNC for consistency?

	INFO_FUNC_PARAM,

	CONSTRUCTOR,
	DESTRUCTOR,
	MAIN,

	ENUM,//TODO
	COMMENT,//,
	STAR_COMMENT, /* a starcomment */
	PLUS_COMMENT, // /+ a pluscomment +/
	//PLUS_ONE_COMMENT, // /+1 a plusonecomment 1+/ //decided against these, but now plus comments can be inside each other.
	//COMMENT_LINE_END,
	QUOTE,

	LOG,
	LOG_LN,
	LOG_SEPARATOR,//a , in rae but a << in C++

	SCOPE_BEGIN,
	SCOPE_END,

	PARENTHESIS_BEGIN,
	PARENTHESIS_END,
	PARENTHESIS_BEGIN_LOG,
	PARENTHESIS_END_LOG,
	PARENTHESIS_BEGIN_LOG_LN,
	PARENTHESIS_END_LOG_LN,
	PARENTHESIS_BEGIN_FUNC_RETURN_TYPES,
	PARENTHESIS_END_FUNC_RETURN_TYPES,
	PARENTHESIS_BEGIN_FUNC_PARAM_TYPES,
	PARENTHESIS_END_FUNC_PARAM_TYPES,
	//PARENTHESIS_END_FUNC_DEFINITION,

	BRACKET_BEGIN,
	BRACKET_END,
	BRACKET_DEFINE_ARRAY_BEGIN, //Maybe remove these...
	BRACKET_DEFINE_ARRAY_END,

	RETURN,

	COMMA,//,
	SEMICOLON,//;
	PLUS,//+
	MINUS,//-
	EQUALS,//=
	DIVIDE,// /
	STAR,// *
	SMALLER_THAN,// <
	BIGGER_THAN,// >

	IF,
	FOR,
	FOREACH,

	NEWLINE,
	NEWLINE_BEFORE_SCOPE_END
};

	string toString(Token::e set)
	{
		switch(set)
		{
			default:
			return "ERROR Token:: not recognized.";
			case Token::UNDEFINED:
			return "Token::UNDEFINED";
			case Token::EMPTY:
			return "Token::EMPTY";
			case Token::MODULE:
			return "Token::MODULE";
			case Token::MODULE_NAME:
			return "Token::MODULE_NAME";
			case Token::CLOSE_MODULE:
			return "Token::CLOSE_MODULE";
			case Token::IMPORT:
			return "Token::IMPORT";
			case Token::IMPORT_NAME:
			return "Token::IMPORT_NAME";
			case Token::CLASS:
			return "Token::CLASS";
			case Token::CLASS_NAME:
			return "Token::CLASS_NAME";
			case Token::CLASS_TEMPLATE_SECOND_TYPE:
			return "Token::CLASS_TEMPLATE_SECOND_TYPE";
			case Token::DEFINE_REFERENCE:
			return "Token::DEFINE_REFERENCE";
			//case Token::DEFINE_REFERENCE_IN_CLASS:
			//return "Token::DEFINE_REFERENCE_IN_CLASS";
			case Token::DEFINE_REFERENCE_NAME:
			return "Token::DEFINE_REFERENCE_NAME";
			case Token::USE_REFERENCE:
			return "Token::USE_REFERENCE";
			case Token::USE_MEMBER:
			return "Token::USE_MEMBER";
			
			/*case Token::UNKNOWN_DEFINITION:
			return "Token::UNKNOWN_DEFINITION";
			case Token::UNKNOWN_USE_REFERENCE:
			return "Token::UNKNOWN_USE_REFERENCE";
			case Token::UNKNOWN_USE_MEMBER:
			return "Token::UNKNOWN_USE_MEMBER";*/

			//case Token::DEFINE_ARRAY:
			//return "Token::DEFINE_ARRAY";
			//case Token::DEFINE_ARRAY_IN_CLASS:
			//return "Token::DEFINE_ARRAY_IN_CLASS";
			//case Token::USE_ARRAY:
			//return "Token::USE_ARRAY";

			//case Token::DEFINE_VECTOR_IN_CLASS:
			//return "Token::DEFINE_VECTOR_IN_CLASS";
			//case Token::DEFINE_VECTOR:
			//return "Token::DEFINE_VECTOR";
			case Token::ARRAY_VECTOR_STUFF:
			return "Token::ARRAY_VECTOR_STUFF";
			case Token::VECTOR_STUFF:
			return "Token::VECTOR_STUFF";
			case Token::VECTOR_NAME:
			return "Token::VECTOR_NAME";
			//case Token::USE_VECTOR:
			//return "Token::USE_VECTOR";

			case Token::TEMPLATE_STUFF:
			return "Token::TEMPLATE_STUFF";
			case Token::TEMPLATE_NAME:
			return "Token::TEMPLATE_NAME";
			case Token::TEMPLATE_SECOND_TYPE:// firstType!(secondType) name
			return "Token::TEMPLATE_SECOND_TYPE";

			//case Token::DEFINE_BUILT_IN_TYPE:
			//return "Token::DEFINE_BUILT_IN_TYPE";
			//case Token::DEFINE_BUILT_IN_TYPE_IN_CLASS:
			//return "Token::DEFINE_BUILT_IN_TYPE_IN_CLASS";
			case Token::DEFINE_BUILT_IN_TYPE_NAME:
			return "Token::DEFINE_BUILT_IN_TYPE_NAME";
			//case Token::USE_BUILT_IN_TYPE:
			//return "Token::USE_BUILT_IN_TYPE";

			case Token::NUMBER:
			return "Token::NUMBER";
			case Token::NUMBER_AFTER_DOT:
			return "Token::NUMBER_AFTER_DOT";
			
			case Token::REFERENCE_DOT:
			return "Token::REFERENCE_DOT";
			case Token::DOT:
			return "Token::DOT";
			
			case Token::NEW:
			return "Token::NEW";
			case Token::FREE:
			return "Token::FREE";
			case Token::FREE_NAME:
			return "Token::FREE_NAME";
			
			case Token::AUTO_INIT:
			return "Token::AUTO_INIT";
			case Token::AUTO_FREE:
			return "Token::AUTO_FREE";
			/*case Token::OBJECT_AUTO_INIT:
			return "Token::OBJECT_AUTO_INIT";
			case Token::OBJECT_AUTO_FREE:
			return "Token::OBJECT_AUTO_FREE";
			case Token::ARRAY_AUTO_INIT:
			return "Token::ARRAY_AUTO_INIT";
			case Token::ARRAY_AUTO_FREE:
			return "Token::ARRAY_AUTO_FREE";
			case Token::VECTOR_AUTO_INIT:
			return "Token::VECTOR_AUTO_INIT";
			case Token::VECTOR_AUTO_FREE:
			return "Token::VECTOR_AUTO_FREE";
			case Token::TEMPLATE_AUTO_INIT:
			return "Token::TEMPLATE_AUTO_INIT";
			case Token::TEMPLATE_AUTO_FREE:
			return "Token::TEMPLATE_AUTO_FREE";
			case Token::BUILT_IN_TYPE_AUTO_INIT:
			return "Token::BUILT_IN_TYPE_AUTO_INIT";
			*/
			case Token::INIT_DATA:
			return "Token::INIT_DATA";
			
			case Token::VISIBILITY_DEFAULT:
			return "Token::VISIBILITY_DEFAULT";
			case Token::VISIBILITY:
			return "Token::VISIBILITY";
			case Token::VISIBILITY_PARENT_CLASS:
			return "Token::VISIBILITY_PARENT_CLASS";
			
			case Token::OVERRIDE:
			return "Token::OVERRIDE";

			case Token::FUNC:
			return "Token::FUNC";
			case Token::FUNC_DEFINITION:
			return "Token::FUNC_DEFINITION";
			case Token::FUNC_NAME:
			return "Token::FUNC_NAME";
			case Token::DEFINE_FUNC_RETURN:
			return "Token::DEFINE_FUNC_RETURN";
			case Token::FUNC_RETURN_TYPE:
			return "Token::FUNC_RETURN_TYPE";
			case Token::FUNC_RETURN_NAME:
			return "Token::FUNC_RETURN_NAME";
			case Token::DEFINE_FUNC_ARGUMENT:
			return "Token::DEFINE_FUNC_ARGUMENT";
			case Token::FUNC_ARGUMENT_TYPE:
			return "Token::FUNC_ARGUMENT_TYPE";
			case Token::FUNC_ARGUMENT_NAME:
			return "Token::FUNC_ARGUMENT_NAME";
			case Token::FUNC_CALL:
			return "Token::FUNC_CALL";
			
			case Token::INFO_FUNC_PARAM:
			return "Token::INFO_FUNC_PARAM";
			
			case Token::CONSTRUCTOR:
			return "Token::CONSTRUCTOR";
			case Token::DESTRUCTOR:
			return "Token::DESTRUCTOR";
			case Token::MAIN:
			return "Token::MAIN";

			case Token::ENUM:
			return "Token::ENUM";
			case Token::COMMENT:
			return "Token::COMMENT";
			case Token::STAR_COMMENT:
			return "Token::STAR_COMMENT";
			case Token::PLUS_COMMENT:
			return "Token::PLUS_COMMENT";
			case Token::QUOTE:
			return "Token::QUOTE";
			
			case Token::LOG:
			return "Token::LOG";
			case Token::LOG_LN:
			return "Token::LOG_LN";
			case Token::LOG_SEPARATOR:
			return "Token::LOG_SEPARATOR";
			
			case Token::SCOPE_BEGIN:
			return "Token::SCOPE_BEGIN";
			case Token::SCOPE_END:
			return "Token::SCOPE_END";
			
			case Token::PARENTHESIS_BEGIN:
			return "Token::PARENTHESIS_BEGIN";
			case Token::PARENTHESIS_END:
			return "Token::PARENTHESIS_END";
			case Token::PARENTHESIS_BEGIN_LOG:
			return "Token::PARENTHESIS_BEGIN_LOG";
			case Token::PARENTHESIS_END_LOG:
			return "Token::PARENTHESIS_END_LOG";
			case Token::PARENTHESIS_BEGIN_LOG_LN:
			return "Token::PARENTHESIS_BEGIN_LOG_LN";
			case Token::PARENTHESIS_END_LOG_LN:
			return "Token::PARENTHESIS_END_LOG_LN";
			case Token::PARENTHESIS_BEGIN_FUNC_RETURN_TYPES:
			return "Token::PARENTHESIS_BEGIN_FUNC_RETURN_TYPES";
			case Token::PARENTHESIS_END_FUNC_RETURN_TYPES:
			return "Token::PARENTHESIS_END_FUNC_RETURN_TYPES";
			case Token::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES:
			return "Token::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES";
			case Token::PARENTHESIS_END_FUNC_PARAM_TYPES:
			return "Token::PARENTHESIS_END_FUNC_PARAM_TYPES";
			//case Token::PARENTHESIS_END_FUNC_DEFINITION:
			//return "Token::PARENTHESIS_END_FUNC_DEFINITION";
			
			case Token::BRACKET_BEGIN:
			return "Token::BRACKET_BEGIN";
			case Token::BRACKET_END:
			return "Token::BRACKET_END";
			case Token::BRACKET_DEFINE_ARRAY_BEGIN:
			return "Token::BRACKET_DEFINE_ARRAY_BEGIN";
			case Token::BRACKET_DEFINE_ARRAY_END:
			return "Token::BRACKET_DEFINE_ARRAY_END";

			case Token::RETURN:
			return "Token::RETURN";
			
			case Token::COMMA:
			return "Token::COMMA";
			case Token::SEMICOLON:
			return "Token::SEMICOLON";
			case Token::PLUS:
			return "Token::PLUS";
			case Token::MINUS:
			return "Token::MINUS";
			case Token::EQUALS:
			return "Token::EQUALS";
			case Token::DIVIDE:
			return "Token::DIVIDE";
			case Token::STAR:
			return "Token::STAR";
			case Token::SMALLER_THAN:
			return "Token::SMALLER_THAN";
			case Token::BIGGER_THAN:
			return "Token::BIGGER_THAN";
			
			case Token::IF:
			return "Token::IF";
			case Token::FOR:
			return "Token::FOR";
			case Token::FOREACH:
			return "Token::FOREACH";
			
			case Token::NEWLINE:
			return "Token::NEWLINE";
			case Token::NEWLINE_BEFORE_SCOPE_END:
			return "Token::NEWLINE_BEFORE_SCOPE_END";
			/*
			case Token::BOOL:
			return "Token::BOOL";
			case Token::BYTE:
			return "Token::BYTE";
			case Token::UBYTE:
			return "Token::UBYTE";
			case Token::CHAR:
			return "Token::CHAR";
			case Token::WCHAR:
			return "Token::WCHAR";
			case Token::DCHAR:
			return "Token::DCHAR";
			case Token::INT:
			return "Token::INT";
			case Token::UINT:
			return "Token::UINT";
			case Token::LONG:
			return "Token::LONG";
			case Token::ULONG:
			return "Token::ULONG";
			case Token::FLOAT:
			return "Token::FLOAT";
			case Token::DOUBLE:
			return "Token::DOUBLE";
			//case Token::REAL:
			//return "Token::REAL";
			case Token::BOOL_IN_CLASS:
			return "Token::BOOL_IN_CLASS";
			case Token::BYTE_IN_CLASS:
			return "Token::BYTE_IN_CLASS";
			case Token::UBYTE_IN_CLASS:
			return "Token::UBYTE_IN_CLASS";
			case Token::CHAR_IN_CLASS:
			return "Token::CHAR_IN_CLASS";
			case Token::WCHAR_IN_CLASS:
			return "Token::WCHAR_IN_CLASS";
			case Token::DCHAR_IN_CLASS:
			return "Token::DCHAR_IN_CLASS";
			case Token::INT_IN_CLASS:
			return "Token::INT_IN_CLASS";
			case Token::UINT_IN_CLASS:
			return "Token::UINT_IN_CLASS";
			case Token::LONG_IN_CLASS:
			return "Token::LONG_IN_CLASS";
			case Token::ULONG_IN_CLASS:
			return "Token::ULONG_IN_CLASS";
			case Token::FLOAT_IN_CLASS:
			return "Token::FLOAT_IN_CLASS";
			case Token::DOUBLE_IN_CLASS:
			return "Token::DOUBLE_IN_CLASS";
			//case Token::REAL_IN_CLASS:
			//return "Token::REAL_IN_CLASS";
			*/
			
		}
	}

	Token::e matchParenthesisEnd(Token::e parenthesis_begin_type)
	{
		switch(parenthesis_begin_type)
		{
			default:
			return Token::ERROR;
			case Token::PARENTHESIS_BEGIN:
			//return Token::PARENTHESIS_BEGIN;
			//case Token::PARENTHESIS_END:
			return Token::PARENTHESIS_END;
			case Token::PARENTHESIS_BEGIN_LOG:
			//return Token::PARENTHESIS_BEGIN_LOG;
			//case Token::PARENTHESIS_END_LOG:
			return Token::PARENTHESIS_END_LOG;
			case Token::PARENTHESIS_BEGIN_LOG_LN:
			//return Token::PARENTHESIS_BEGIN_LOG_LN;
			//case Token::PARENTHESIS_END_LOG_LN:
			return Token::PARENTHESIS_END_LOG_LN;
			case Token::PARENTHESIS_BEGIN_FUNC_RETURN_TYPES:
			//return Token::PARENTHESIS_BEGIN_FUNC_RETURN_TYPES;
			//case Token::PARENTHESIS_END_FUNC_RETURN_TYPES:
			return Token::PARENTHESIS_END_FUNC_RETURN_TYPES;
			case Token::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES:
			//return Token::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES;
			//case Token::PARENTHESIS_END_FUNC_PARAM_TYPES:
			return Token::PARENTHESIS_END_FUNC_PARAM_TYPES;
		}

	}

	Token::e matchBracketEnd(Token::e begin_type)
	{
		switch(begin_type)
		{
			default:
			return Token::ERROR;
			case Token::BRACKET_BEGIN:
			//return Token::BRACKET_BEGIN;
			//case Token::BRACKET_END:
			return Token::BRACKET_END;
			case Token::BRACKET_DEFINE_ARRAY_BEGIN:
			//return Token::BRACKET_DEFINE_ARRAY_BEGIN;
			//case Token::BRACKET_DEFINE_ARRAY_END:
			return Token::BRACKET_DEFINE_ARRAY_END;
			
		}

	}
/*
	Token::e matchBuiltIntTypesToInClass(Token::e lang_token_type)
	{
		switch(lang_token_type)
		{
			default:
			return Token::ERROR;
			case Token::BOOL:
			return Token::BOOL_IN_CLASS;
			case Token::BYTE:
			return Token::BYTE_IN_CLASS;
			case Token::UBYTE:
			return Token::UBYTE_IN_CLASS;
			case Token::CHAR:
			return Token::CHAR_IN_CLASS;
			case Token::WCHAR:
			return Token::WCHAR_IN_CLASS;
			case Token::DCHAR:
			return Token::DCHAR_IN_CLASS;
			case Token::INT:
			return Token::INT_IN_CLASS;
			case Token::UINT:
			return Token::UINT_IN_CLASS;
			case Token::LONG:
			return Token::LONG_IN_CLASS;
			case Token::ULONG:
			return Token::ULONG_IN_CLASS;
			case Token::FLOAT:
			return Token::FLOAT_IN_CLASS;
			case Token::DOUBLE:
			return Token::DOUBLE_IN_CLASS;
			//case Token::REAL:
			//return Token::REAL_IN_CLASS;
		}
	}
	*/
}


namespace BuiltInType
{
enum e
{
	UNDEFINED,
	BOOL, //bool
	BYTE, //int8_t
	UBYTE, //uint8_t
	CHAR, //for use with UTF-8
	WCHAR, //UTF-16
	DCHAR, //for UTF-32
	INT, //int32_t
	UINT, //uint32_t
	LONG, //NOT long, but int64_t (long long in C/C++)
	ULONG, //uint64_t
	//what's that 16 bit float type... half? sharp???
	FLOAT, //float (should be 32 bits)
	DOUBLE, //double (should be 64 bits)
	//REAL, //biggest floating point type, maybe 80 bits or something...
	STRING
	/*
	BOOL_IN_CLASS, //bool
	BYTE_IN_CLASS, //int8_t
	UBYTE_IN_CLASS, //uint8_t
	CHAR_IN_CLASS, //for use with UTF-8
	WCHAR_IN_CLASS, //UTF-16
	DCHAR_IN_CLASS, //for UTF-32
	INT_IN_CLASS, //int32_t
	UINT_IN_CLASS, //uint32_t
	LONG_IN_CLASS, //NOT long, but int64_t (long long in C/C++)
	ULONG_IN_CLASS, //uint64_t
	//what's that 16 bit float type... half? sharp???
	FLOAT_IN_CLASS, //float (should be 32 bits)
	DOUBLE_IN_CLASS, //double (should be 64 bits)
	*/
};

	string toString(BuiltInType::e set_type)
	{
		switch(set_type)
		{
			default:
			return "nosuchtype";
			case BuiltInType::BOOL:
			return "bool";
			case BuiltInType::BYTE:
			return "byte";
			case BuiltInType::UBYTE:
			return "ubyte";
			case BuiltInType::CHAR:
			return "char";
			case BuiltInType::WCHAR:
			return "wchar";//TODO check if it works and is int32_t (or 16 bits better...)
			case BuiltInType::DCHAR:
			return "dchar";
			case BuiltInType::INT:
			return "int";
			case BuiltInType::UINT:
			return "uint";
			case BuiltInType::LONG:
			return "long";
			case BuiltInType::ULONG:
			return "ulong";
			case BuiltInType::FLOAT:
			return "float";
			case BuiltInType::DOUBLE:
			return "double";
			//case BuiltInType::REAL:
			//return "real";
			case BuiltInType::STRING:
			return "string";
		}
	}

	string toCppString(BuiltInType::e set_type)
	{
		switch(set_type)
		{
			default:
			return "nosuchtype";
			case BuiltInType::BOOL:
			return "bool";
			case BuiltInType::BYTE:
			return "int8_t";
			case BuiltInType::UBYTE:
			return "uint8_t";
			case BuiltInType::CHAR:
			return "char";
			case BuiltInType::WCHAR:
			return "wchar";//TODO check if it works and is int32_t (or 16 bits better...)
			case BuiltInType::DCHAR:
			return "int32_t";
			case BuiltInType::INT:
			return "int32_t";
			case BuiltInType::UINT:
			return "uint32_t";
			case BuiltInType::LONG:
			return "int64_t";
			case BuiltInType::ULONG:
			return "uint64_t";
			case BuiltInType::FLOAT:
			return "float";
			case BuiltInType::DOUBLE:
			return "double";
			//case BuiltInType::REAL:
			//return "real";
			case BuiltInType::STRING:
			return "std::string";
		}
	}

	BuiltInType::e stringToBuiltInType(string set_token)
	{
		if( set_token == "bool" )
		{
			return BuiltInType::BOOL;
		}
		else if( set_token == "byte" )
		{
			return BuiltInType::BYTE;
		}
		else if( set_token == "ubyte" )
		{
			return BuiltInType::UBYTE;
		}
		else if( set_token == "char" )
		{
			return BuiltInType::CHAR;
		}
		else if( set_token == "wchar" )
		{
			return BuiltInType::WCHAR;
		}
		else if( set_token == "dchar" )
		{
			return BuiltInType::DCHAR;
		}
		else if( set_token == "int" )
		{
			return BuiltInType::INT;
		}
		else if( set_token == "uint" )
		{
			return BuiltInType::UINT;
		}
		else if( set_token == "long" )
		{
			return BuiltInType::LONG;
		}
		else if( set_token == "ulong" )
		{
			return BuiltInType::ULONG;
		}
		else if( set_token == "float" )
		{
			return BuiltInType::FLOAT;
		}
		else if( set_token == "double" )
		{
			return BuiltInType::DOUBLE;
		}
		/*else if( set_token == "real" )
		{
			return BuiltInType::REAL;
		}*/
		else if( set_token == "string" )
		{
			return BuiltInType::STRING;
		}

		return BuiltInType::UNDEFINED;
	}

	bool isBuiltInType(string set)
	{
		BuiltInType::e test_built_in_type = BuiltInType::stringToBuiltInType(set);
		if( test_built_in_type != BuiltInType::UNDEFINED )
		{
			return true;
		}
		//else
		return false;
	}

	bool isBuiltInType(BuiltInType::e set)
	{
		if( set != BuiltInType::UNDEFINED )
		{
			return true;
		}
		//else
		return false;
	}

}

bool isNumericChar( int letter )
{
	switch(letter)
	{
		default:
		return false;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		return true;
	}

	return false;

}

bool isAlphabetic( int letter )
{
	switch(letter)
	{
		default:
		return false;
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'I':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'U':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
		return true;
	}

	return false;
}

struct LineNumber
{
	//TODO filename in here as well! Is that wise? maybe too much to have a big string for each letter...
	int totalCharNumber;
	int line;
	int charPosInLine;

	void copyFrom(LineNumber& set)
	{
		totalCharNumber = set.totalCharNumber;
		line = set.line;
		charPosInLine = set.charPosInLine;
	}

	/*string toString()
	{
		string ret = 
		return ret;
	}*/

	void printOut()
	{
		cout<<"line: "<<line<<" charpos: "<<charPosInLine;
		////rae::log("line: ", line, " charpos: ", charPosInLine);
	}
};

class LangElement
{
public:
	
	LangElement()
	{
		m_langTokenType = Token::UNDEFINED;
		m_typeType = TypeType::UNDEFINED;
		m_builtInType = BuiltInType::UNDEFINED;
		m_role = Role::UNDEFINED;
		m_currentElement = 0;
		m_parent = 0;
		m_initData = 0;
		m_definitionElement = 0;
		m_previousElement = 0;
		m_nextElement = 0;
		m_isUnknownType = false;
	}
	
	LangElement(LineNumber& set_line_number, Token::e set_lang_token_type, TypeType::e set_type_type, string set_name = "", string set_type = "")
	{
		lineNumber(set_line_number);
		m_langTokenType = set_lang_token_type;
		m_currentElement = 0;
		m_parent = 0;
		m_initData = 0;
		m_definitionElement = 0;
		m_previousElement = 0;
		m_nextElement = 0;

		m_name = set_name;

		m_isUnknownType = false;
		m_typeType = set_type_type;//TypeType::UNDEFINED;
		m_builtInType = BuiltInType::UNDEFINED;//must NOT be initialized after the next call to type().
		type(set_type); //automatically tests if it is built_in_type.
	}

	//copying
	/*
	LangElement(LangElement& copy_from)
	{
		m_langTokenType = copy_from.m_langTokenType;
		m_lineNumber.copyFrom( copy_from.m_lineNumber );
		m_currentElement = copy_from.m_currentElement;
		m_parent = copy_from.m_parent;
		if(m_initData)
			m_initData = copy_from.m_initData->copy();
		m_previousElement = copy_from.m_previousElement;
		m_nextElement = copy_from.m_nextElement;

		m_name = copy_from.m_name;

		m_isUnknownType = copy_from.m_isUnknownType;
		m_builtInType = copy_from.m_builtInType;//must NOT be initialized after the next call to type().
		m_type = copy_from.m_type; //automatically tests if it is built_in_type.

		foreach(LangElement* an_elem, copy_from.langElements)
		{
			if(an_elem)
				langElements.push_back( (an_elem->copy()) );
		}
	}
	*/

	//returns a hopefully complete newly allocated copy of this LangElement.
	/*LangElement* copy()
	{
		return new LangElement(*this);
	}*/

	LangElement* copy()
	{
		LangElement* res = new LangElement();

		res->m_langTokenType = m_langTokenType;
		res->m_lineNumber.copyFrom( m_lineNumber );
		res->m_currentElement = m_currentElement;
		res->m_parent = m_parent;
		if(m_initData)
			res->m_initData = m_initData->copy();
		res->m_definitionElement = m_definitionElement;
		res->m_previousElement = m_previousElement;
		res->m_nextElement = m_nextElement;

		res->m_name = m_name;

		res->m_isUnknownType = m_isUnknownType;
		res->m_builtInType = m_builtInType;//must NOT be initialized after the next call to type().
		res->m_typeType = m_typeType;
		res->m_role = m_role;
		res->m_type = m_type; //automatically tests if it is built_in_type.

		foreach(LangElement* an_elem, langElements)
		{
			if(an_elem)
				res->langElements.push_back( (an_elem->copy()) );
		}
        return res;
	}

	
	~LangElement()
	{
		#ifdef DEBUG_RAE_DESTRUCTORS
			cout<<"~LangElement() START. "<<toString()<<"\n";
		#endif

		//all LangElements should be owned by vector langElements
		//other vectors only have borrowed ones... I hope.
		/*
		foreach(LangElement* elem, langElements)
		{
			#ifdef DEBUG_RAE_DESTRUCTORS
				if(elem)
				{
					cout<<"~LangElement() Going to delete: "<<elem->toString()<<"\n";
				}
				else
				{
					cout<<"~LangElement() already deleted.\n";
				}
			#endif

			if(elem)
				delete elem;
		}
		*/
		for(uint i = 0; i < langElements.size(); i++)
		{
			//Hmm. Only destroy objects which have parent set to this.
			if(langElements[i] && langElements[i]->parent() == this)
				delete langElements[i];
		}
		langElements.clear();

		#ifdef DEBUG_RAE_DESTRUCTORS
			cout<<"~LangElement() END."<<toString()<<"\n";
		#endif
	}
	
	string toString()
	{
		string ret = langTokenTypeString() + " typetype: " + typeTypeString() + " name: " + name() + " type: " + type() + "<. line: " + numberToString(lineNumber().line);
		return ret;
	}

	//a debugging help:
	void printOutListOfFunctions()
	{
		cout<<"Functions in "<<toString()<<"\n-------------------------\n";
		foreach(LangElement* elem, langElements)
		{
			if( elem->langTokenType() == Token::FUNC )
			{
				cout<<elem->toString()<<"\n";
			}
		}
		cout<<"-------------------------\n";
	}

	void printOutListOfReferences()
	{
		cout<<"Defined references in "<<toString()<<"\n-------------------------\n";
		foreach(LangElement* elem, langElements)
		{
			if( elem->langTokenType() == Token::DEFINE_REFERENCE )
			{
				cout<<elem->toString()<<"\n";
			}
		}
		cout<<"-------------------------\n";
	}

	//a specialized func for use with Token::IMPORT
	string importName(string separator_char_str = ".")
	{
		if(langTokenType() != Token::IMPORT)
		{
			cout<<"Error: this is not an import. Can't get importName(): "<<toString()<<"\n";
			return "Not an import.";
		}

		string a_module_name = "";

		int not_on_first = 0;

		foreach(LangElement* elem, langElements)
		{
			if( elem->langTokenType() == Token::IMPORT_NAME )
			{
				if(not_on_first > 0)
				{
					a_module_name += separator_char_str; //add a . between the module_name components
				}
				else
				{
					not_on_first++;
				}
				a_module_name += elem->name();
			}
			else
			{
				break;//break the foreach... hope this works.
			}
		}

		return a_module_name;
	}


	public: Token::e langTokenType() { return m_langTokenType; }
	public: void langTokenType(Token::e set) { m_langTokenType = set; }
	protected: Token::e m_langTokenType;

	public: string langTokenTypeString() { return Token::toString(m_langTokenType); }

	public: string typeTypeString() { return TypeType::toString(m_typeType); }
	
	public: bool isDefinition()
	{
		if( langTokenType() == Token::DEFINE_REFERENCE
			//|| langTokenType() == Token::DEFINE_REFERENCE_IN_CLASS
			//|| langTokenType() == Token::DEFINE_ARRAY
			//|| langTokenType() == Token::DEFINE_ARRAY_IN_CLASS
			//|| langTokenType() == Token::DEFINE_VECTOR
			//|| langTokenType() == Token::DEFINE_VECTOR_IN_CLASS
			//|| langTokenType() == Token::DEFINE_BUILT_IN_TYPE
			//|| langTokenType() == Token::DEFINE_BUILT_IN_TYPE_IN_CLASS
			|| langTokenType() == Token::CLASS
			|| langTokenType() == Token::ENUM
			|| langTokenType() == Token::FUNC
			|| langTokenType() == Token::CONSTRUCTOR
			|| langTokenType() == Token::DESTRUCTOR
			|| langTokenType() == Token::MAIN
			|| langTokenType() == Token::DEFINE_FUNC_ARGUMENT
			|| langTokenType() == Token::DEFINE_FUNC_RETURN
		)
		{
			return true;
		}
		//else
		return false;
	}

	public: bool isFunc()
	{
		if( langTokenType() == Token::FUNC
			|| langTokenType() == Token::CONSTRUCTOR
			|| langTokenType() == Token::DESTRUCTOR
			|| langTokenType() == Token::MAIN
		)
		{
			return true;
		}
		//else
		return false;
	}

	public: bool isUseReference()
	{
		if( langTokenType() == Token::USE_REFERENCE
			//|| langTokenType() == Token::USE_ARRAY
			//|| langTokenType() == Token::USE_VECTOR
			//|| langTokenType() == Token::USE_BUILT_IN_TYPE
			|| langTokenType() == Token::USE_MEMBER
			|| langTokenType() == Token::FUNC_CALL
		)
		{
			return true;
		}
		//else
		return false;
	}

	public: bool isUserDefinableToken()
	{
		//We need to add other user defined types here later...
		if( langTokenType() == Token::CLASS
			|| langTokenType() == Token::ENUM )
  		{
  			return true;
  		}
  		//else
  		return false;
  	}

	//the actual name...
	public: string& name() { return m_name; }
	public: void name(string set) { m_name = set; }
	protected: string m_name;
	
	//type is a user defined Class name like Gradient... etc...
	//You might e.g. have your Token::DEFINE_FUNC_RETURN or Token::DEFINE_FUNC_ARGUMENT
	//and the type might be some class name.
	
	public: bool isBuiltInType()
	{
		if( builtInType() == BuiltInType::UNDEFINED )
		{
			return false;
		}
		//else
		return true;
	}

	//Now, if we have a builtin type, it will be set and get from builtInType(),
	//if we don't, it'll be get and set from m_type which is just a string,
	//and can contain all the user defined types.
	public: string type()
	{
		if( builtInType() == BuiltInType::UNDEFINED )
		{
			return m_type;
		}
		//else
		return builtInTypeString();
	}
	public: string typeInCpp()
	{
		if( builtInType() == BuiltInType::UNDEFINED )
		{
			return m_type;
		}
		//else
		return builtInTypeStringCpp();
	}
	public: void type(string set)
	{
		BuiltInType::e test_built_in_type = BuiltInType::stringToBuiltInType(set);
		if( test_built_in_type != BuiltInType::UNDEFINED )
		{			
			////////NOOOOO: typeType(TypeType::BUILT_IN_TYPE);
			builtInType(test_built_in_type);
			m_type = set;
			//cout<<"SEEEEEEEEEEEEET type to BUILT_IN_TYPE. "<<m_type<<" name: "<<m_name<<" line: "<<lineNumber().line<<"\n";
			return;
		}
		//else //it's some other type...
		//////////NOOOOO don't do this: typeType(TypeType::REF);//We'll just mark it as reference for now.
		m_type = set;
	}
	protected: string m_type;
	
	//stuff for templates: first we have a second type for the template
	//like in:
	//class FirstType(SecondType)
	//or when using it:
	//FirstType!(SecondType) name

	public: LangElement* templateSecondType()
	{
		return searchFirst(Token::TEMPLATE_SECOND_TYPE);
	}

	public: void createTemplateSecondType(string set_type)
	{
		if( searchFirst(Token::TEMPLATE_SECOND_TYPE) != 0 )
		{
			cout<<"ERROR in createTemplateSecondType() We already had a second type for the template: "<<templateSecondTypeString()<<" new second type: "<<set_type<<"\n";
		}
		else
		{
			newLangElement(lineNumber(), Token::TEMPLATE_SECOND_TYPE, TypeType::TEMPLATE/*are you sure this is not confusing...*/, /*string set_name:*/ "undefined", /*string set_type:*/ set_type );
		}
	}

	public: string templateSecondTypeString()
	{
		//returns the type of the TEMPLATE_SECOND_TYPE
		//if it has one. there should be only one child.
		foreach(LangElement* set_elem, langElements)
		{
			if(set_elem->langTokenType() == Token::TEMPLATE_SECOND_TYPE)
				return set_elem->type();
		}
        //else
		return "templateError";
	}

	//this is usefull when constructing the template class
	//in c++. It will be a normal c++ class:
	//rae template definition:
	//class FirstType(T)
	//rae template use definition:
	//FirstType!(SecondType) name
	//in C++ we construct artificial classes from templates:
	//This is what templateTypeCombination gives:
	//class _FirstTypeSecondType_
	//and use them normally:
	//FirstTypeSecondType* name = new FirstTypeSecondType();

	public: string templateSecondTypeStringInCpp()
	{
		//returns the type of the TEMPLATE_SECOND_TYPE
		//if it has one. there should be only one child.
		foreach(LangElement* set_elem, langElements)
		{
			if(set_elem->langTokenType() == Token::TEMPLATE_SECOND_TYPE)
				return set_elem->typeInCpp();
		}
		//else
		return "templateError";
	}

	//We don't use the C++ type here. We use Rae types.
	public: string templateTypeCombination()
	{
		return "_" + type() + "_" + templateSecondTypeString() + "_";
	}

	//TypeType doesn't have anything to do with templates anymore. But it is very important
	//secondary type for e.g. DEFINE_REFERENCE.

	public: TypeType::e typeType() { return m_typeType; }
	public: void typeType(TypeType::e set) { m_typeType = set; }
	protected: TypeType::e m_typeType;

	public: Role::e role() { return m_role; }
	public: void role(Role::e set) { m_role = set; }
	protected: Role::e m_role;

	//the type is here, if it's a built_in_type, NOW:
	//The reason for this separate builtIntype thing is that
	//we want to be able to change from e.g rae-type long to c++-type
	//long long. I guess, we would be able to do that with just putting
	//the type in the type string, but maybe, maybe, it's easier this way...
	//or not.
	public: BuiltInType::e builtInType() { return m_builtInType; }
	public: void builtInType(BuiltInType::e set)
	{
		m_builtInType = set;
		////////NOOOOOO: typeType(TypeType::BUILT_IN_TYPE);
	}
	protected: BuiltInType::e m_builtInType;

	public: string builtInTypeString() { return BuiltInType::toString(m_builtInType); }
	public: string builtInTypeStringCpp() { return BuiltInType::toCppString(m_builtInType); }

	//We've moved the UNKNOWN stuff into this property:
	public: bool isUnknownType() { return m_isUnknownType; }
	public: void isUnknownType(bool set) { m_isUnknownType = set; }
	protected: bool m_isUnknownType;

	//An important addition: a link to the element where this element is defined:
	//For DEFINE_REFERENCE (and DEFINE_VECTOR and DEFINE_ARRAY) this is the class.
	//For USE_REFERENCE this is the DEFINE_REFERENCE
	//And do we also do: For FUNC_CALL this is the func that is being called?
	public: LangElement* definitionElement() { return m_definitionElement; }
	public: void definitionElement(LangElement* set) { m_definitionElement = set; }
	protected: LangElement* m_definitionElement;

	public: LangElement* previousElement() { return m_previousElement; }
	public: void previousElement(LangElement* set) { m_previousElement = set; }
	protected: LangElement* m_previousElement;

	public: LangElement* previous2ndElement()
	{
		if(m_previousElement && m_previousElement->m_previousElement)
			return m_previousElement->m_previousElement;
		//else
		return 0;//null
	}

	public: LangElement* nextElement() { return m_nextElement; }
	public: void nextElement(LangElement* set) { m_nextElement = set; }
	protected: LangElement* m_nextElement;

	//And here's some token shortcuts for those elements.
	public: Token::e previousToken()
	{
		if(m_previousElement == 0)
			return Token::UNDEFINED;
		return m_previousElement->langTokenType();
	}
	public: Token::e previous2ndToken()
	{
		if(m_previousElement && m_previousElement->m_previousElement)
			return m_previousElement->m_previousElement->langTokenType();
		//else
		return Token::UNDEFINED;
	}
	public: Token::e nextToken()
	{
		if(m_nextElement == 0)
			return Token::UNDEFINED;
		return m_nextElement->langTokenType();
	}

	//
	//Oh well, I added another pointer to a langElement here. 
	//This one is data about initialization... example:
	//int someInt = 5
	//so, initData would be "5"
	//SomeClass someOb = new SomeOb(float param1: 256.0, OtherClass param2: ob);
	//Well, that's a bit TODO.
	//Maybe it could be a linked list! So it links to next until = 0!!!!

	public: LangElement* initData() { return m_initData; }
	public: void initData(LangElement* set) { m_initData = set; }
	protected: LangElement* m_initData;

	public: LangElement* addDefaultInitData()
	{
		LangElement* lang_elem;

		string init_string;

		switch(builtInType())
		{
			default:
			case BuiltInType::UNDEFINED:
				init_string = ""; //empty
			break;
			case BuiltInType::BYTE:
			case BuiltInType::UBYTE:
			case BuiltInType::CHAR:
			case BuiltInType::WCHAR:
			case BuiltInType::DCHAR:
			case BuiltInType::INT:
			case BuiltInType::UINT:
				init_string = "0";
			break;
			case BuiltInType::LONG:
			case BuiltInType::ULONG:
				init_string = "0L"; //might be "wrong" as L stands for long, and it should be long long.
				//check it. it's no big deal. It will work anyway.
			break;
			case BuiltInType::FLOAT:
				init_string = "0.0f";
			break;
			case BuiltInType::DOUBLE:
			//case BuiltInType::REAL:
				init_string = "0.0";
			break;
			case BuiltInType::BOOL:
				init_string = "false";
			break;
			case BuiltInType::STRING:
				init_string = "\"\"";//an empty string.
			break;

		}

		if( builtInType() == BuiltInType::UNDEFINED )
		{
			if( m_type != "" )
			{
				//handle SomeClass.ptr = 0
				init_string = "0";
			}
			else
			{
				//no type! just return and ignore this strange request.
				return 0;
			}
		}

		lang_elem = new LangElement(lineNumber(), Token::INIT_DATA, TypeType::UNDEFINED, init_string);
		//lang_elem = new LangElement(lineNumber(), Token::UNDEFINED, init_string);
		initData(lang_elem);
		
		return lang_elem;
	}

	//
	
	public: LineNumber& lineNumber() { return m_lineNumber; }
	public: void lineNumber(LineNumber& set)
	{
		m_lineNumber.copyFrom(set);
	}
	protected: LineNumber m_lineNumber;
	
	
	public:

	bool isInClass()
	{
		if(parent() && parent()->langTokenType() == Token::CLASS)
		{
			return true;
		}
		//else
		return false;
	}

	Token::e parentToken()
	{
		if( m_parent )
			return m_parent->langTokenType();
		//else
		return Token::UNDEFINED;
	}
	LangElement* parent() { return m_parent; }
	void parent(LangElement* set) { m_parent = set; }
	protected: LangElement* m_parent;

public:
	//then some strange checking from the parent, the class and func and stuff:
	LangElement* parentClass()
	{
		return searchClosestParentToken(Token::CLASS);

		//return null if not found.
		/*LangElement* res = parent();
		while( res )
		{
			if( res->langTokenType() == Token::CLASS )
			{
				return res;
			}
			res = res->parent();//move up in hierarchy.
		}
		return res;//if not found this will be zero in the end, because that's what ends the while loop.
		*/
	}

	LangElement* searchClosestParentToken(Token::e set)
	{
		//return null if not found.
		LangElement* res = parent();
		while( res )
		{
			if( res->langTokenType() == set )
			{
				return res;
			}
			res = res->parent();//move up in hierarchy.
		}
		return res;//if not found this will be zero in the end, because that's what ends the while loop.
	}

	LangElement* parentFunc()
	{
		LangElement* res = searchClosestParentToken(Token::FUNC);
		if(res)
			return res;
		//else
		res = searchClosestParentToken(Token::CONSTRUCTOR);
		if(res)
			return res;
		//else
		res = searchClosestParentToken(Token::DESTRUCTOR);
		if(res)
			return res;
		//else
		res = searchClosestParentToken(Token::MAIN);
		//return res anyway, even if it is null.
		return res;
	}

	LangElement* searchClosestPreviousUseReferenceOrUseVector()
	{
		//for stuff like:
		//someOb.callSomeFunc //when called from callSomeFunc it will return someOb.
		//someVector[doSomething.extremely(difficult, and_strange)].callSomeFunc //when called from callSomeFunc it will return someVector.

		//return null if not found.

		//first check for reference dot. We have to have that.
		if( previousToken() != Token::REFERENCE_DOT )
		{
			return 0;
		}

		//ok we have reference_dot.

		LangElement* res = previous2ndElement();

		int found_a_bracket = 0;

		while( res )
		{
			if( res->langTokenType() == Token::SCOPE_BEGIN )
			{
				//if at any case we would find a scope begin, then we'll just return 0.
				return 0;
			}

			if(found_a_bracket <= 0)//Well if this gets negative it's an error... too many brackets... but it could theoretically happen if we're inside two brackets.
			//so let's just leave it like this, and hope it's correct.
			{
				if( res->langTokenType() == Token::USE_REFERENCE
					//|| res->langTokenType() == Token::USE_VECTOR
					//|| res->langTokenType() == Token::USE_ARRAY
				)
				{
					return res;
				}
				else if( res->langTokenType() == Token::BRACKET_END || res->langTokenType() == Token::BRACKET_DEFINE_ARRAY_END )
				{
					found_a_bracket = true;
				}
			}
			else //there's some brackets there...
			{
				if( res->langTokenType() == Token::BRACKET_BEGIN || res->langTokenType() == Token::BRACKET_DEFINE_ARRAY_BEGIN )
				{
					found_a_bracket--;
				}	
			}
			res = res->previousElement();//move backwards.
		}
		return res;//if not found this will be zero in the end, because that's what ends the while loop.
	}	
	
public:

	//is visibility name allowed in c++
	/*
	static bool isVisibilityNameAllowedInCpp(string set)
	{
		if(set == "public")
			return true;
		else if(set == "protected")
			return true;
		else if(set == "private")
			return true;
		else if(set == "library")
			return false;
		else if(set == "hidden")
			return false;
		//else
		return false;
	}
	*/

	static string getVisibilityNameInCpp(string set)
	{
		if(set == "public")
			return "public: ";
		else if(set == "protected")
			return "protected: ";
		else if(set == "private")
			return "private: ";
		else if(set == "library")
			return "/*library*/public: ";
		else if(set == "hidden")
			return "/*hidden*/protected: ";
		//else
		return "";
	}

	//Use this with FUNCs
	//returns 0 if nothing found.
	LangElement* hasVisibilityFuncChild()
	{
		//Will stop at SCOPE_BEGIN
		for( uint i = 0; i < langElements.size(); i++ )
		{
			if( langElements[i]->langTokenType() == Token::VISIBILITY )
			{
				//return true;
				return langElements[i];
			}
			else if( langElements[i]->langTokenType() == Token::SCOPE_BEGIN )
			{
				//return false;
				return 0;
			}
		}
		//else
		//return false;
		return 0;
	}

	//For scopeElement and class:

	//These are the elements that are most probably of the type
	//DEFINE_REFERENCE and usually allocate memory with new.
	//So, if not otherwise said, we will deallocate/delete them
	//at the end of this scope/class. Unless their ownership is claimed by
	//some other func.
	//Umm. Something like:
	//func ()push_back(own T set)
	//and then the compiler should also guarantee that the push_back
	//places T set onto some container, or ref T, or ref T?.
	protected: vector<LangElement*> ownedElements;

public:
	void own(LangElement* set)
	{
		ownedElements.push_back(set);
	}

	void freeOwned()
	{
		if( langTokenType() != Token::CLASS )
		{
			foreach(LangElement* init_ob, ownedElements)
			{
					LangElement* auto_init_elem = init_ob->copy();
					auto_init_elem->definitionElement(init_ob);//our init_ob can be found through the definitionElement.
					auto_init_elem->langTokenType(Token::AUTO_FREE);
					addElement(auto_init_elem);
					addNewLine();
			}
		}
	}

	//For class:

	void createObjectAutoInitAndFree(LangElement* init_ob)
	{
		createObjectAutoInitInConstructors(init_ob);
		createObjectAutoFreeInDestructors(init_ob);
	}

	void createObjectAutoInitInConstructors(LangElement* init_ob)
	{
		if( init_ob == 0)
		{
			return;
		}

		foreach( LangElement* elem, langElements )
		{
			if(elem->langTokenType() == Token::CONSTRUCTOR)
			{	
			/*		
				if( init_ob->langTokenType() == Token::DEFINE_VECTOR_IN_CLASS )
				{
					elem->newLangElementToTopWithNewline( init_ob->lineNumber(), Token::VECTOR_AUTO_INIT, init_ob->name(), init_ob->type() );
				}
				else if( init_ob->langTokenType() == Token::DEFINE_ARRAY_IN_CLASS )
				{
					elem->newLangElementToTopWithNewline( init_ob->lineNumber(), Token::ARRAY_AUTO_INIT, init_ob->name(), init_ob->type() );
				}
				else if( init_ob->langTokenType() == Token::DEFINE_REFERENCE_IN_CLASS )
				{	
					elem->newLangElementToTopWithNewline( init_ob->lineNumber(), Token::OBJECT_AUTO_INIT, init_ob->name(), init_ob->type() );
				}
				*/
				/*if( init_ob->typeType() == TypeType::VECTOR )//TODO maybe get rid of these and just use typeType and AUTO_INIT.
				{
					elem->newLangElementToTopWithNewline( init_ob->lineNumber(), Token::VECTOR_AUTO_INIT, TypeType::VECTOR, init_ob->name(), init_ob->type() );
				}
				else if( init_ob->typeType() == TypeType::TEMPLATE )
				{
					elem->newLangElementToTopWithNewline( init_ob->lineNumber(), Token::TEMPLATE_AUTO_INIT, TypeType::TEMPLATE, init_ob->name(), init_ob->type() );
				}
				else if( init_ob->typeType() == TypeType::ARRAY )
				{
					elem->newLangElementToTopWithNewline( init_ob->lineNumber(), Token::ARRAY_AUTO_INIT, TypeType::ARRAY, init_ob->name(), init_ob->type() );
				}
				else if( init_ob->typeType() == TypeType::REF )
				{	
					elem->newLangElementToTopWithNewline( init_ob->lineNumber(), Token::OBJECT_AUTO_INIT, TypeType::REF, init_ob->name(), init_ob->type() );
				}
				else if( init_ob->typeType() == TypeType::BUILT_IN_TYPE )
				{	
					elem->newLangElementToTopWithNewline( init_ob->lineNumber(), Token::BUILT_IN_TYPE_AUTO_INIT, TypeType::BUILT_IN_TYPE, init_ob->name(), init_ob->type() );
				}
				else
				{
					cout<<"Error: wrong kind of init_ob in createObjectAutoInitInConstructors.\n";
				}
				*/
				
				//elem->newLangElementToTopWithNewline( init_ob->lineNumber(), Token::AUTO_INIT, init_ob->typeType(), init_ob->name(), init_ob->type() );				
			
				LangElement* auto_init_elem = init_ob->copy();
				auto_init_elem->definitionElement(init_ob);//our init_ob can be found through the definitionElement.
				auto_init_elem->langTokenType(Token::AUTO_INIT);
				elem->addElementToTopOfFunc(auto_init_elem);
			}
		}		
	}

	void createObjectAutoFreeInDestructors(LangElement* init_ob)
	{
		if( init_ob == 0)
		{
			return;
		}

		foreach( LangElement* elem, langElements )
		{
			if(elem->langTokenType() == Token::DESTRUCTOR)
			{		
			/*	
				if( init_ob->langTokenType() == Token::DEFINE_VECTOR_IN_CLASS )
				{
					elem->newLangElementToTopWithNewline( init_ob->lineNumber(), Token::VECTOR_AUTO_FREE, init_ob->name(), init_ob->type() );
				}
				else if( init_ob->langTokenType() == Token::DEFINE_ARRAY_IN_CLASS )
				{
					elem->newLangElementToTopWithNewline( init_ob->lineNumber(), Token::ARRAY_AUTO_FREE, init_ob->name(), init_ob->type() );
				}
				else if( init_ob->langTokenType() == Token::DEFINE_REFERENCE_IN_CLASS )
				{	
					elem->newLangElementToTopWithNewline( init_ob->lineNumber(), Token::OBJECT_AUTO_FREE, init_ob->name(), init_ob->type() );
				}
				*/
				/*
				if( init_ob->typeType() == TypeType::VECTOR )
				{
					elem->newLangElementToTopWithNewline( init_ob->lineNumber(), Token::VECTOR_AUTO_FREE, TypeType::VECTOR, init_ob->name(), init_ob->type() );
				}
				else if( init_ob->typeType() == TypeType::TEMPLATE )
				{
					elem->newLangElementToTopWithNewline( init_ob->lineNumber(), Token::TEMPLATE_AUTO_FREE, TypeType::TEMPLATE, init_ob->name(), init_ob->type() );
				}
				else if( init_ob->typeType() == TypeType::ARRAY )
				{
					elem->newLangElementToTopWithNewline( init_ob->lineNumber(), Token::ARRAY_AUTO_FREE, TypeType::ARRAY, init_ob->name(), init_ob->type() );
				}
				else if( init_ob->typeType() == TypeType::REF )
				{	
					elem->newLangElementToTopWithNewline( init_ob->lineNumber(), Token::OBJECT_AUTO_FREE, TypeType::REF, init_ob->name(), init_ob->type() );
				}
				//No need to free built ins: else if( init_ob->typeType() == TypeType::BUILT_IN_TYPE )
				//{	
				//	elem->newLangElementToTopWithNewline( init_ob->lineNumber(), Token::BUILT_IN_TYPE_AUTO_FREE, init_ob->name(), init_ob->type() );
				//}
				else
				{
					cout<<"Error: wrong kind of init_ob in createObjectAutoInitInConstructors.\n";
				}
				*/

				LangElement* auto_init_elem = init_ob->copy();
				auto_init_elem->definitionElement(init_ob);//our init_ob can be found through the definitionElement.
				auto_init_elem->langTokenType(Token::AUTO_FREE);
				elem->addElementToTopOfFunc(auto_init_elem);
			}
		}		
	}

	//

	vector<LangElement*> langElements;


	public: LangElement* currentElement() { return m_currentElement; }
	public: void currentElement(LangElement* set) { m_currentElement = set; }
	protected: LangElement* m_currentElement;
	
	//includes the name of this element.
	//e.g. rae.examples.RaeTester.RaeTester.sayHello.notrees
	//     module---------------->class---->func---->built_in_type
	public: string namespaceString()
	{
		string res = name();
		LangElement* kind_of_like_recursive = parent();
		while( kind_of_like_recursive )
		{
			res = kind_of_like_recursive->name() + "." + res;
			kind_of_like_recursive = kind_of_like_recursive->parent();
		}
		return res;
	}

public:	
	LangElement* newLangElement(LineNumber& set_line_number, Token::e set_lang_token_type, TypeType::e set_type_type = TypeType::UNDEFINED, string set_name = "", string set_type = "" )
	{
		LangElement* lang_elem = new LangElement(set_line_number, set_lang_token_type, set_type_type, set_name, set_type);
		//lang_elem->lineNumber( set_line_number );
		addElement(lang_elem);
		m_currentElement = lang_elem;//Should this be default for addElement too?
		
		#ifdef DEBUG_RAE
			cout<<"this.name: "<<name()<<" LangElement.newLangElement: "<<Token::toString(set_lang_token_type)<<" name:>"<<set_name<<"< type: "<<set_type<<"\n";
			//rae::log("this.name: ",name()," LangElement.newLangElement: ",Token::toString(set_lang_token_type)," name:>",set_name,"< type: ",set_type,"\n");
		#endif

		return lang_elem;
	}

	LangElement* newLangElementToTop( LineNumber& set_line_number, Token::e set_lang_token_type, TypeType::e set_type_type, string set_name = "", string set_type = "" )
	{
		LangElement* lang_elem = new LangElement(set_line_number, set_lang_token_type, set_type_type, set_name, set_type);
		addElementToTop(lang_elem);
		m_currentElement = lang_elem;//Should this be default for addElement too?
		
		#ifdef DEBUG_RAE
			cout<<"this.name: "<<name()<<" LangElement.newLangElementToTop: "<<Token::toString(set_lang_token_type)<<" name:>"<<set_name<<"< type: "<<set_type<<"\n";
			//rae::log("this.name: ",name()," LangElement.newLangElementToTop: ",Token::toString(set_lang_token_type)," name:>",set_name,"< type: ",set_type,"\n");
		#endif

		return lang_elem;	
	}

	LangElement* newLangElementToTopAfterNewlineWithNewline( LineNumber& set_line_number, Token::e set_lang_token_type, TypeType::e set_type_type, string set_name = "", string set_type = "", BuiltInType::e set_built_in_type = BuiltInType::UNDEFINED )
	{
		LangElement* lang_elem = new LangElement(set_line_number, set_lang_token_type, set_type_type, set_name, set_type);
		lang_elem->builtInType(set_built_in_type);
		addElementToTopAfterNewlineWithNewline(lang_elem);
		m_currentElement = lang_elem;//Should this be default for addElement too?
		
		#ifdef DEBUG_RAE
			cout<<"this.name: "<<name()<<" LangElement.newLangElementToTop: "<<Token::toString(set_lang_token_type)<<" name:>"<<set_name<<"< type: "<<set_type<<"\n";
			//rae::log("this.name: ",name()," LangElement.newLangElementToTop: ",Token::toString(set_lang_token_type)," name:>",set_name,"< type: ",set_type,"\n");
		#endif

		return lang_elem;	
	}

	LangElement* newLangElementToTopOfFunc( LineNumber& set_line_number, Token::e set_lang_token_type, TypeType::e set_type_type, string set_name = "", string set_type = "", BuiltInType::e set_built_in_type = BuiltInType::UNDEFINED )
	{
		LangElement* lang_elem = new LangElement(set_line_number, set_lang_token_type, set_type_type, set_name, set_type);
		lang_elem->builtInType(set_built_in_type);
		addElementToTopOfFunc(lang_elem);
		m_currentElement = lang_elem;//Should this be default for addElement too?
		
		#ifdef DEBUG_RAE
			cout<<"this.name: "<<name()<<" LangElement.newLangElementToTopOfFunc: "<<Token::toString(set_lang_token_type)<<" name:>"<<set_name<<"< type: "<<set_type<<"\n";
			//rae::log("this.name: ",name()," LangElement.newLangElementToTop: ",Token::toString(set_lang_token_type)," name:>",set_name,"< type: ",set_type,"\n");
		#endif

		return lang_elem;	
	}

	LangElement* newLangElementToTopOfClass( LineNumber& set_line_number, Token::e set_lang_token_type, TypeType::e set_type_type, string set_name = "", string set_type = "", BuiltInType::e set_built_in_type = BuiltInType::UNDEFINED )
	{
		LangElement* lang_elem = new LangElement(set_line_number, set_lang_token_type, set_type_type, set_name, set_type);
		lang_elem->builtInType(set_built_in_type);
		addElementToTopOfClass(lang_elem);
		m_currentElement = lang_elem;//Should this be default for addElement too?
		
		#ifdef DEBUG_RAE
			cout<<"this.name: "<<name()<<" LangElement.newLangElementToTopOfClass: "<<Token::toString(set_lang_token_type)<<" name:>"<<set_name<<"< type: "<<set_type<<"\n";
			//rae::log("this.name: ",name()," LangElement.newLangElementToTop: ",Token::toString(set_lang_token_type)," name:>",set_name,"< type: ",set_type,"\n");
		#endif

		return lang_elem;	
	}

	

	void addElement(LangElement* set)
	{
		set->parent(this);
		langElements.push_back(set);
	}

	void addNewLine()
	{
		LangElement* elem_newline = new LangElement( lineNumber(), Token::NEWLINE, TypeType::UNDEFINED, "\n" );
		langElements.push_back(elem_newline);
		m_currentElement = elem_newline;
	}

	void addElementToTop(LangElement* set)
	{
		set->parent(this);
		
		if( langElements.empty() == true )
		{
			langElements.push_back(set);
			return;
		}
		/*for( uint i < 0; i < langElements.size(); i++ )
		{
			if() 

		}*/

		vector<LangElement*>::iterator my_it;

		for( my_it = langElements.begin(); my_it < langElements.end(); my_it++ )
		{
			if( (*my_it) && (*my_it)->langTokenType() == Token::SCOPE_BEGIN )
			{
				//do nothing
			}
			else
			{
				//we found it! the element after the first scope_begin..
				break; //Umm this looks like it shouldn't work. This will get called all the time.
			}
		}

		if( my_it < langElements.end() )
			langElements.insert(my_it, set);
	}
/*
	void addElementToTopWithNewline(LangElement* set)
	{
		//empty
		cout<<"DANGDANG: "<<langElements.size()<<"\n");
	}
*/

	//Use this with a func element:
	//void addElementToTopAfterScopeWithNewline(LangElement* set)
	void addElementToTopOfFunc(LangElement* set)
	{
		vector<LangElement*>::iterator my_it;

		/*if( isFunc() == false )
		{
			cout<<"addElementToTopAfterScopeWithNewline used without a func.\n";
		}*/

		bool to_debug = false;
		if( langTokenType() == Token::CLASS )
		{
			cout<<"CLASS addElementToTopOfFunc: "<<toString()<<"\n";
			to_debug = true;
			cout<<"we are adding: "<<set->toString();
		}

		for( my_it = langElements.begin(); my_it < langElements.end(); my_it++ )
		{
			#ifdef DEBUG_RAE2
				cout<<"addElementToTopOfFunc elem: "<<(*my_it)->toString();
				//rae::log("elem: ",(*my_it)->toString());
			#endif

			if( (*my_it) && (*my_it)->langTokenType() == Token::SCOPE_BEGIN )
			{
				if(to_debug)
					cout<<"CLASS found Scope begin on class.\n";
				
				#ifdef DEBUG_RAE2
				cout<<"found SCOPE_BEGIN.\n";
				//rae::log("found SCOPE_BEGIN.\n");
				#endif

				(*my_it)->addElementToTopAfterNewlineWithNewline(set);

				if(to_debug) cout<<"shouldve called addElementToTopAfterNewlineWithNewline.\n";
				
				break;//We break here.
			}
			else
			{
				if(to_debug)
					cout<<"CLASS not a scope_begin: "<<(*my_it)->toString()<<".\n";
			}
		}
	}

	//This is a helper for the above. Use it with a scope element or class or whatever.
	void addElementToTopAfterNewlineWithNewline(LangElement* set)
	{
		set->parent(this);

		#ifdef DEBUG_RAE2
		cout<<"addElementToTopAfterNewlineWithNewline() START. "<<toString()<<"\n";
		cout<<"DANGDANG: "<<langElements.size()<<"\n";
		//rae::log("addElementToTopWithNewline() START. ",toString(),"\n");
		//rae::log("langElements.size(): ",langElements.size(),"\n");
		#endif

		//LangElement* elem_newline = newLangElement( set->lineNumber(), Token::NEWLINE, "\n" );
		LangElement* elem_newline = new LangElement( set->lineNumber(), Token::NEWLINE, TypeType::UNDEFINED, "\n" );

		if( langElements.empty() == true )
		{
			#ifdef DEBUG_RAE2
			cout<<"it was ALL EMPTY. so we just put it in there.\n";
			//rae::log("it was ALL EMPTY. so we just put it in there.\n");
			#endif
			langElements.push_back(elem_newline);
			langElements.push_back(set);
			langElements.push_back(elem_newline);
			return;
		}

		vector<LangElement*>::iterator my_it;

		bool found_newline_after_scope = false;

		for( my_it = langElements.begin(); my_it < langElements.end(); my_it++ )
		{
			#ifdef DEBUG_RAE2
				cout<<"elem: "<<(*my_it)->toString();
				//rae::log("elem: ",(*my_it)->toString());
			#endif

			if( (*my_it) && ((*my_it)->langTokenType() == Token::NEWLINE || (*my_it)->langTokenType() == Token::NEWLINE_BEFORE_SCOPE_END) )
			{	
				found_newline_after_scope = true;
				#ifdef DEBUG_RAE2
				cout<<"found NEWLINE after scope_begin.\n";
				//rae::log("found NEWLINE after scope_begin.\n");
				#endif
				break;//We break here so we get the correct iterator position where to insert.
			}	
			
		}

		//if( my_it < langElements.end() )
		{
			#ifdef DEBUG_RAE2
			cout<<"We found it! iterator and stuff worked. ITERATOR!!!\n";
			//rae::log("We found it! iterator and stuff worked. ITERATOR!!!\n");
			#endif
			langElements.insert(my_it, set);
			//langElements.insert(my_it, elem_newline );
		}

		//I believe our iterator is not valid anymore...


		//vector<LangElement*>::const_iterator my_it2;

		//Then we'll search again for our set, because the iterators were messed up by our last insert.
		for( my_it = langElements.begin(); my_it < langElements.end(); my_it++ )
		{
			#ifdef DEBUG_RAE2
			cout<<"elem2: "<<(*my_it)->toString();
			//rae::log("elem2: ",(*my_it)->toString());
			#endif

			if( (*my_it) && (*my_it) == set )
			{
				break;
			}
		}

		//And insert our newline.
		langElements.insert(my_it, elem_newline );

		#ifdef DEBUG_RAE2
		cout<<"FONGFONG: "<<langElements.size()<<"\n";
		//rae::log("FONGFONG: ",langElements.size(),"\n");
		#endif
	}
	
	void addElementToTopOfClass(LangElement* set)
	{
		set->parent(this);

		#ifdef DEBUG_RAE2
		cout<<"addElementToTopOfClass() START. "<<toString()<<"\n";
		cout<<"DANGDANG: "<<langElements.size()<<"\n";
		//rae::log("addElementToTopWithNewline() START. ",toString(),"\n");
		//rae::log("langElements.size(): ",langElements.size(),"\n");
		#endif

		//LangElement* elem_newline = newLangElement( set->lineNumber(), Token::NEWLINE, "\n" );
		LangElement* elem_newline = new LangElement( set->lineNumber(), Token::NEWLINE, TypeType::UNDEFINED, "\n" );

		if( langElements.empty() == true )
		{
			#ifdef DEBUG_RAE2
			cout<<"it was ALL EMPTY. so we just put it in there.\n";
			//rae::log("it was ALL EMPTY. so we just put it in there.\n");
			#endif
			langElements.push_back(elem_newline);
			langElements.push_back(set);
			langElements.push_back(elem_newline);
			return;
		}
		/*for( uint i < 0; i < langElements.size(); i++ )
		{
			if() 

		}*/

		vector<LangElement*>::iterator my_it;

		bool found_scope_begin = false;
		bool found_newline_after_scope = false;

		for( my_it = langElements.begin(); my_it < langElements.end(); my_it++ )
		{
			#ifdef DEBUG_RAE2
				cout<<"elem: "<<(*my_it)->toString();
				//rae::log("elem: ",(*my_it)->toString());
			#endif

			if( found_scope_begin == false )
			{
				if( (*my_it) && (*my_it)->langTokenType() == Token::SCOPE_BEGIN )
				{
					#ifdef DEBUG_RAE2
					cout<<"found SCOPE_BEGIN.\n";
					//rae::log("found SCOPE_BEGIN.\n");
					#endif
					found_scope_begin = true;
					//break;
				}
				else
				{	
					//else
					#ifdef DEBUG_RAE2
					cout<<"not a SCOPE_BEGIN.\n";
					//rae::log("not a SCOPE_BEGIN.\n");
					#endif
				}
			}
			else
			{
				if( (*my_it) && ((*my_it)->langTokenType() == Token::NEWLINE || (*my_it)->langTokenType() == Token::NEWLINE_BEFORE_SCOPE_END) )
				{	
					found_newline_after_scope = true;
					#ifdef DEBUG_RAE2
					cout<<"found NEWLINE after scope_begin.\n";
					//rae::log("found NEWLINE after scope_begin.\n");
					#endif
					break;
				}	
			}
		}

		//if( my_it < langElements.end() )
		{
			#ifdef DEBUG_RAE2
			cout<<"We found it! iterator and stuff worked. ITERATOR!!!\n";
			//rae::log("We found it! iterator and stuff worked. ITERATOR!!!\n");
			#endif
			langElements.insert(my_it, set);
			//langElements.insert(my_it, elem_newline );
		}

		//I believe our iterator is not valid anymore...


		//vector<LangElement*>::const_iterator my_it2;

		//Then we'll search again for our set, because the iterators were messed up by our last insert.
		for( my_it = langElements.begin(); my_it < langElements.end(); my_it++ )
		{
			#ifdef DEBUG_RAE2
			cout<<"elem2: "<<(*my_it)->toString();
			//rae::log("elem2: ",(*my_it)->toString());
			#endif

			if( (*my_it) && (*my_it) == set )
			{
				break;
			}
		}

		//And insert our newline.
		langElements.insert(my_it, elem_newline );

		#ifdef DEBUG_RAE2
		cout<<"FONGFONG: "<<langElements.size()<<"\n";
		//rae::log("FONGFONG: ",langElements.size(),"\n");
		#endif
	}

	void addNameToCurrentElement( string set_name )
	{
		if(m_currentElement)
		{
			/////////cout<<"LangElement.addName: "<<Token::toString(m_currentElement->langTokenType())<<" name:>"<<set_name<<"\n");
			m_currentElement->name( set_name );
		}
		else cout<<"ERROR: LangElement::addNameToCurrentElement() : No m_currentElement.\n";
			//rae::log("ERROR: LangElement::addNameToCurrentElement() : No m_currentElement.\n");
	}
	
	void addTypeToCurrentElement( string set_type )
	{
		if(m_currentElement)
		{
			//////////cout<<"LangElement.addType: "<<Token::toString(m_currentElement->langTokenType())<<" type:>"<<set_type<<"\n");
			m_currentElement->type( set_type );
		}
		else cout<<"ERROR: LangElement::addTypeToCurrentElement() : No m_currentElement.\n";
			//rae::log("ERROR: LangElement::addTypeToCurrentElement() : No m_currentElement.\n");
	}
	
	void copyChildElementsFrom( LangElement& from )
	{
		/////////cout<<"from: "<<from.name()<<"<\n");
		foreach( LangElement* elem, from.langElements )
		{
			////////////cout<<"Copy elem: \n"<<elem->name();
			//Hmm. We copy just the pointers... Ouch.
			addElement(elem);
		}
	}
	
	/*LangElement* newLangElementWithType(Token::e set_lang_token_type, string set_type )
	{
		LangElement* lang_elem = new LangElement(set_lang_token_type, set_name);
		langElements.push_back( lang_elem );
		cout<<"LangElement.newLangElement: "<<Token::toString(set_lang_token_type)<<" name:>"<<set_name<<"<\n");
		return lang_elem;
	}*/
	
	LangElement* searchFunctionParams(string set_name)
	{
		foreach( LangElement* elem, langElements )
		{
			if( elem->name() == set_name )
			{	
				if( elem->langTokenType() == Token::DEFINE_FUNC_ARGUMENT )
				{
					return elem;
				}
				else if( elem->langTokenType() == Token::DEFINE_FUNC_RETURN )
				{
					return elem;
				}
			}
		}
		//nothing found:
		return 0;
	}

	//TODO clean up hasFunctionOrMember and searchName.
	//They are the same currently. Are they the same also conceptually,
	//if so then merge as hasFunctionOrMember which is more descriptive.
	LangElement* hasFunctionOrMember(string set_name)
	{
		foreach( LangElement* elem, langElements )
		{
			if( elem->name() == set_name )
			{
				return elem;
			}
		}
		//nothing found:
		return 0;
	}

	LangElement* searchName(string set_name)
	{
		foreach( LangElement* elem, langElements )
		{
			if( elem->name() == set_name )
			{
				return elem;
			}
		}
		//nothing found:
		return 0;
	}	

	LangElement* searchFirst(Token::e set_lang_token_type)
	{
		foreach( LangElement* elem, langElements )
		{
			if( elem->langTokenType() == set_lang_token_type )
			{
				return elem;
			}
		}
		//nothing found:
		return 0;
	}

	LangElement* searchLast(Token::e set_lang_token_type)
	{
		//foreach( LangElement* elem, langElements )
		for( long i = langElements.size()-1l; i >= 0l; i-- )
		{
			if( langElements[i]->langTokenType() == set_lang_token_type )
			{
				return langElements[i];
			}
		}
		//nothing found:
		return 0;
	}
};

