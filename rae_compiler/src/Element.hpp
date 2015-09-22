#ifndef RAE_COMPILER_LANGELEMENT_HPP
#define RAE_COMPILER_LANGELEMENT_HPP

#include <assert.h>

#include "rae_helpers.hpp"

#include "ReportError.hpp"

namespace Rae
{

namespace ParseError
{
enum e
{
	UNDEFINED,
	SYNTAX_WARNING,
	SYNTAX_ERROR,
	COMPILER_ERROR
};	
}


namespace Role
{
enum e
{
	UNDEFINED,
	GLOBAL,
	INSIDE_CLASS,
	FUNC_RETURN,
	FUNC_PARAMETER,
	INSIDE_FUNCTION,
	TEMPLATE_PARAMETER
};	
}

namespace Kind
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
	VECTOR, //REMOVE
	TEMPLATE //REMOVE
};

string toString(Kind::e set);
Kind::e fromString(const string& set);
}

namespace ContainerType
{
enum e
{
	UNDEFINED, // = SINGLE
	ARRAY,
	STATIC_ARRAY//,
	//TEMPLATE,
	//DICTIONARY //MAP
};

string toString(ContainerType::e set);
}

namespace Token
{
enum e
{
	UNDEFINED,
	EMPTY, //intentionally empty
	PASSTHROUGH_HDR, //to c++ header .hpp
	PASSTHROUGH_SRC, //to c++ source .cpp
	RAE_ERROR,
	MODULE,
	MODULE_DIR,
	MODULE_NAME,
	CLOSE_MODULE,
	IMPORT,
	//MAYBE NOT TODO: we're probably using namespace list kind of thing instead. IMPORT_DIR, // import GL.glew, where GL is IMPORT_DIR and glew is IMPORT_NAME
	IMPORT_NAME,
	

	PROJECT, //project keyword defines a project usually in main
	TARGET_DIR,  //targetdir keyword for the executable
	// IMPORT_DIRS is a list within "project" and is defined with the keyword
	// importdirs = ["list/of/dirs", "other/dir"] 
	IMPORT_DIRS, // Waiting for a list of import paths
	
	NAMESPACE,
	USE_NAMESPACE,
	CLASS,
	// use together with Kind
	// to have e.g. CLASS Kind::TEMPLATE subtype that behaves differently.
	CLASS_NAME,
	// just something for expectinToken:
	CLASS_TEMPLATE_SECOND_TYPE, // class FirstType(SecondType)
	DEFINE_REFERENCE, // SomeClass aNewObject
	// use together with Kind to have different kind of DEFINE_REFERENCEs.

	DEFINE_REFERENCE_NAME,
	USE_REFERENCE, // aNewObject.callSomething //the first word here...
	// actually also the second word if it's not a func.
	// like in aNewObject.someNumber
	// Maybe we need to create more types like
	USE_MEMBER, // for someThing.someMember but not func. 
	
	//UNKNOWN, // generally unknown, we use UNKNOWN_USE_REFERENCE for this.
	//UNKNOWN_DEFINITION, // Tester tester
	//UNKNOWN_USE_REFERENCE, // tester
	//UNKNOWN_USE_MEMBER, // tester.useMember
	// an unknown use of member, which will be either a FUNC_CALL or USE_REFERENCE (NOT USED ATM:or USE_BUILT_IN_TYPE)

	ARRAY_VECTOR_STUFF, //SomeClass[] someVector is a vector, same as:
	VECTOR_STUFF, //vector!(SomeClass)
	VECTOR_NAME, //the name of the vector. vector!(type) name
	///////USE_VECTOR,

	TEMPLATE_STUFF, // the definition of using a template e.g. someTemplate!(typeName) objectName
	TEMPLATE_NAME,
	TEMPLATE_SECOND_TYPE, // firstType!(secondType) name

	//DEFINE_BUILT_IN_TYPE,
	//DEFINE_BUILT_IN_TYPE_IN_CLASS,
	DEFINE_BUILT_IN_TYPE_NAME,
	//USE_BUILT_IN_TYPE, // We are using USE_REFERENCE instead of this for now...

	ALIAS, // alias newName = oldName // just string replacement. Doesn't create a new type. Similar to #define newName oldName

	//REAL_IN_CLASS, // biggest floating point type, maybe 80 bits or something...
	NUMBER, // integer
	FLOAT_NUMBER,
	DECIMAL_NUMBER_AFTER_DOT,

	REFERENCE_DOT, // the dot when using references and calling funcs and members.
	DOT, // do we need dot in other contexts? Yes, in floating point numbers!
	
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
	INIT_DATA, // maybe rename to INIT_DATA_BEGIN or something...
	ACTUAL_INIT_DATA, // maybe rename to just INIT_DATA...

	RAE_NULL,

	VISIBILITY_DEFAULT,
	VISIBILITY,
	VISIBILITY_PARENT_CLASS,

	LET, // C++ const
	MUT, // mutable, not used very often...

	OVERRIDE, // for the override keyword, that let's you redefine stuff with a same name.

	FUNC_DEFINITION,
	FUNC,
	FUNC_NAME,
	DEFINE_FUNC_RETURN,
	FUNC_RETURN_TYPE,
	FUNC_RETURN_NAME,
	//REMOVED: DEFINE_FUNC_ARGUMENT,//refactor... this is for built-in-types and class references...
	//so why do we need them separate when using DEFINE_REFERENCE and DEFINE_BUILT_IN_TYPE.
	//and then we also got DEFINE_ARRAY and DEFINE_VECTOR
	//sounds like we could use an enum like
	//TypeOfTheType
	//reference, built-in, vector, array, etc. new types...
	//that could replace the isBuiltInType thing in Element.
	//DEFINE_FUNC_ARGUMENT_ARRAY,//Uh, we should refactor... 
	//REMOVED: FUNC_ARGUMENT_TYPE,
	//REMOVED: FUNC_ARGUMENT_NAME,
	FUNC_CALL, // or should it be USE_FUNC for consistency?

	INFO_FUNC_PARAM,

	CONSTRUCTOR,
	DESTRUCTOR,
	MAIN,

	ENUM, // TODO
	COMMENT, // # for comments
	STAR_COMMENT, /* a starcomment */ // Currently works also with #* *# but should probably start using #{  #}
	PLUS_COMMENT, // /+ a pluscomment +/
	//PLUS_ONE_COMMENT, // /+1 a plusonecomment 1+/ //decided against these, but now plus comments can be inside each other.
	//COMMENT_LINE_END,
	QUOTE,

	LOG,
	LOG_S,
	LOG_SEPARATOR,//a , in rae but a << in C++

	SCOPE_BEGIN,
	SCOPE_END,

	PARENTHESIS_BEGIN,
	PARENTHESIS_END,
	PARENTHESIS_BEGIN_LOG,
	PARENTHESIS_END_LOG,
	PARENTHESIS_BEGIN_LOG_S,
	PARENTHESIS_END_LOG_S,
	PARENTHESIS_BEGIN_FUNC_RETURN_TYPES,
	PARENTHESIS_END_FUNC_RETURN_TYPES,
	PARENTHESIS_BEGIN_FUNC_PARAM_TYPES,
	PARENTHESIS_END_FUNC_PARAM_TYPES,
	//PARENTHESIS_END_FUNC_DEFINITION,

	BRACKET_BEGIN,
	BRACKET_END,
	BRACKET_DEFINE_ARRAY_BEGIN,
	BRACKET_DEFINE_ARRAY_END,
	BRACKET_DEFINE_STATIC_ARRAY_BEGIN,
	BRACKET_DEFINE_STATIC_ARRAY_END,
	BRACKET_INITIALIZER_LIST_BEGIN,
	BRACKET_INITIALIZER_LIST_END,
	BRACKET_CAST_BEGIN,
	BRACKET_CAST_END,

	RETURN,

	COMMA,//,
	SEMICOLON,//;

	CAST, // cast[float] someInt

	// Arithmetic operators
	ASSIGNMENT, // =
	PLUS, // + addition
	MINUS, // - subtraction
	MULTIPLY, // * multiplication
	DIVIDE, // / division
	MODULO, // % 
	OPERATOR_INCREMENT, // ++
	OPERATOR_DECREMENT, // --
	PLUS_ASSIGN, // +=
	MINUS_ASSIGN, // -=
	MULTIPLY_ASSIGN, // *=
	DIVIDE_ASSIGN, // /=
	MODULO_ASSIGN, // %=

	// Comparison operators
	EQUALS, // == equals keyword
	NOT_EQUAL, // != not equals keyword pair
	GREATER_THAN, // >
	LESS_THAN, // <
	GREATER_THAN_OR_EQUAL, // >=
	LESS_THAN_OR_EQUAL, // <=

	NOT, // not, !
	AND, // and, &&
	OR, // or, ||

	//The tokens and, and_eq, bitand, bitor, compl, not, not_eq, or, or_eq, xor, xor_eq, <%, %>, <:, and :>
	//can be used instead of the symbols &&, &=, &, |, ~, !, !=, ||, |=, ^, ^=, {, }, [, and ].

	BITWISE_OR, // bitor, |
	BITWISE_AND, // bitand, &
	BITWISE_XOR, // xor, ^
	BITWISE_COMPLEMENT, //compl, ~ operator

	POINT_TO,// -> used instead of = to point references to objects etc.
	POINT_TO_END_PARENTHESIS, //we use the link<Object> a_link(arguments); to create links, so we need the closing parenthesis in C++.
	//these are injected in SourceParser::newPointToElement() and SourceParser::newLine().
	IS, // is pointing to
	
	IF,
	ELSE,
	FOR,
	FOR_EACH,
	IN_TOKEN,

	TRUE_TRUE,
	FALSE_FALSE,

	SIZEOF,
	EXTERN,

	NEWLINE,
	NEWLINE_BEFORE_SCOPE_END,
	PRAGMA_CPP, //@c++ automatically try to figure out where to put this raw c++ code. Usually it goes to header, inside funcs it goes to cpp files.
	PRAGMA_CPP_HDR, //@c++hpp for raw C++ code to be put in the header .hpp
	PRAGMA_CPP_SRC, //@c++cpp for raw C++ code to be put in the source .cpp
	PRAGMA_CPP_END,
	PRAGMA_ASM, //@asm for raw assembler code.
	PRAGMA_ASM_END,
	PRAGMA_ECMA, //@ecma for raw ecmascript/javascript code.
	PRAGMA_ECMA_END,

	EXPECTING_NAME, // Used together with the expectingNameFor mechanism that automates simple "keyword name" pairs.
	EXPECTING_TYPE, // The same for type

	// -----------------------------------------
	// C++ specific tokens
	// -----------------------------------------

	EXPECTING_CPP_PREPROCESSOR, // C++ preprocessor #define #ifdef etc.
	CPP_PRE_DEFINE, // #define
	// Notes about CPP_PRE_DEFINE elements: We use the name() for the definition name
	// and the type() for the value of that definition
	// That's kind of like the same as in typedef, but C/C++ has got it's typedefs defined backwards.
	EXPECTING_CPP_PRE_DEFINE_VALUE, // #define SOME_NAME value <- the value in that line.
	//CPP_PRE_IFDEF, // #ifdef 
	//CPP_PRE_ELSE, // #else
	//CPP_PRE_ENDIF, // #endif
	CPP_UNSIGNED,
	CPP_SIGNED,
	CPP_TYPEDEF,
	EXPECTING_CPP_TYPEDEF_TYPENAME

};

	string toString(Token::e set);
	Token::e matchParenthesisEnd(Token::e parenthesis_begin_type);
	Token::e matchBracketEnd(Token::e begin_type);

	bool isNewline(Token::e set);
}


namespace BuiltInType
{
enum e
{
	UNDEFINED,
	A_VOID,   // void
	BOOL,   // bool
	BYTE,   // int8_t
	UBYTE,  // uint8_t
	CHAR,   // for use with UTF-8
	WCHAR,  // UTF-16
	DCHAR,  // for UTF-32
	SHORT,  // int16_t
	USHORT, // uint16_t
	INT,    // int32_t
	UINT,   // uint32_t
	LONG,   // NOT long, but int64_t (long long in C/C++)
	ULONG,  // uint64_t
	//TODO CENT,   // int128_t
	//TODO UCENT,  // uint128_t
	// what's that 16 bit float type... half? sharp???
	FLOAT,  // float (should be 32 bits)
	DOUBLE, // double (should be 64 bits)
	//REAL, // TODO biggest floating point type, maybe 80 bits or something...
	STRING
	//WSTRING, // TODO
	//DSTRING, // TODO
};

	string toString(BuiltInType::e set_type);
	string toCppString(BuiltInType::e set_type);
	BuiltInType::e stringToBuiltInType(string set_token);
	BuiltInType::e cppStringToBuiltInType(string set_token);
	bool isBuiltInType(string set);
	bool isBuiltInType(BuiltInType::e set);

}

bool isNumericChar( int letter );
bool isAlphabetic( int letter );

struct LineNumber
{
	// TODO filename in here as well! Is that wise? maybe too much to have a big string for each letter...
	int totalCharNumber;
	int line;
	int column;

	void copyFrom(LineNumber& set);

	string toString()
	{
		return std::to_string(line) + ":" + std::to_string(column);
	}

	void printOut();
};

class Element
{
public:

	Element()
	:
		m_token(Token::UNDEFINED),
		m_name(""),
		m_type(""),
		m_kind(Kind::UNDEFINED),
		m_typeConvertFrom(Kind::UNDEFINED),
		m_typeConvertTo(Kind::UNDEFINED),
		m_builtInType(BuiltInType::UNDEFINED),
		m_containerType(ContainerType::UNDEFINED),
		m_role(Role::UNDEFINED),
		m_parseError(ParseError::UNDEFINED),
		m_currentElement(nullptr),
		m_parent(nullptr),
		m_initData(nullptr),
		m_namespaceElement(nullptr),
		m_useNamespace(nullptr),
		m_definitionElement(nullptr),
		m_pairElement(nullptr),
		m_previousElement(nullptr),
		m_nextElement(nullptr),
		m_isUnknownType(false),
		m_isLet(false)
	{
	}
	
	Element(LineNumber& set_line_number, Token::e set_lang_token_type, Kind::e set_type_type, string set_name = "", string set_type = "")
	:
		m_token(set_lang_token_type),
		m_name(set_name),	
		m_kind(set_type_type),
		m_typeConvertFrom(Kind::UNDEFINED),
		m_typeConvertTo(Kind::UNDEFINED),
		m_builtInType(BuiltInType::UNDEFINED),
		m_containerType(ContainerType::UNDEFINED),
		m_role(Role::UNDEFINED),
		m_parseError(ParseError::UNDEFINED),
		m_currentElement(nullptr),
		m_parent(nullptr),
		m_initData(nullptr),
		m_namespaceElement(nullptr),
		m_useNamespace(nullptr),
		m_definitionElement(nullptr),
		m_pairElement(nullptr),
		m_previousElement(nullptr),
		m_nextElement(nullptr),
		m_isUnknownType(false),
		m_isLet(false)
	{
		lineNumber(set_line_number);
		
		type(set_type); // must be initialized after m_builtInType.
	}

	//copying
	/*
	Element(Element& copy_from)
	{
		m_token = copy_from.m_token;
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

		for(Element* an_elem : copy_from.elements)
		{
			if(an_elem)
				elements.push_back( (an_elem->copy()) );
		}
	}
	*/

	Element* copy()
	{
		Element* res = new Element();

		res->m_token = m_token;
		res->m_name = m_name;
		res->m_type = m_type;
		res->m_kind = m_kind;
		res->m_typeConvertFrom = m_typeConvertFrom;
		res->m_typeConvertTo = m_typeConvertTo;
		res->m_builtInType = m_builtInType;
		res->m_containerType = m_containerType;
		res->m_role = m_role;
		res->m_parseError = m_parseError;
		res->m_currentElement = m_currentElement;
		res->m_parent = m_parent;
		if(m_initData)
			res->m_initData = m_initData->copy();
		else res->m_initData = nullptr;
		res->m_namespaceElement = m_namespaceElement;
		res->m_useNamespace = m_useNamespace;
		res->m_definitionElement = m_definitionElement;
		res->m_pairElement = m_pairElement;
		res->m_previousElement = m_previousElement;
		res->m_nextElement = m_nextElement;
		res->m_isUnknownType = m_isUnknownType;
		res->m_lineNumber.copyFrom( m_lineNumber );

		for(Element* an_elem : elements)
		{
			if(an_elem)
				res->elements.push_back( (an_elem->copy()) );
		}
        return res;
	}

	~Element()
	{
		#ifdef DEBUG_RAE_DESTRUCTORS
			cout<<"~Element() START. "<<toString()<<"\n";
		#endif

		for(uint i = 0; i < elements.size(); i++)
		{
			//Hmm. Only destroy objects which have parent set to this.
			if(elements[i] && elements[i]->parent() == this)
				delete elements[i];
		}
		elements.clear();

		#ifdef DEBUG_RAE_DESTRUCTORS
			cout<<"~Element() END."<<toString()<<"\n";
		#endif
	}
	
    bool isWhiteSpace();

	string toString();
	string toSingleLineString();
	
	// debugging helps:
	void printOutElements();
	void printOutListOfFunctions();
	void printOutListOfReferences();

	// a specialized func for use with Token::IMPORT
	string importName(string separator_char_str = ".");

	// You probably want to use parentModuleString instead.
	string moduleString(string separator_char_str = ".");
	// This just shows in which module we are at the moment.
	string parentModuleString();

	// property token
	public: Token::e token() { return m_token; }
	public: void token(Token::e set) { m_token = set; }
	protected: Token::e m_token;

public:
	string tokenString() { return Token::toString(m_token); }
	string kindString() { return Kind::toString(m_kind); }
	string containerTypeString() { return ContainerType::toString(m_containerType); }
	
	bool isOperator();
	bool isMathOperator();
	bool isEndsExpression();
	bool isDefinition();
	bool isBeginsScope();
	bool isParenthesis();
	bool isNewline();
	bool isFunc();
	bool isUseReference();
	bool isUserDefinableToken();
	
	// The expressionRValue. This is important.
	Element* expressionRValue();
	Element* funcReturnType();

	// other parenthesis except the ones in the start and the end
	static bool hasListOtherParenthesis(vector<Element*>& list);

	void elementListUntil(Token::e set_token, vector<Element*>& return_list);
	void elementListUntilPair(vector<Element*>& return_list);
	void elementListUntilPair(Element* pair, vector<Element*>& return_list);
	
	//void listBetweenParenthesis(Token::e set_token, vector<Element*>& return_list);
	
	vector<Element*> funcParameterList();
	//vector<Element*> funcCallArgumentList();

	// property name
	public:    string& name()           { return m_name; }
	public:    void    name(string set) { m_name = set;  }
	protected: string  m_name;

public:	
	// type is a user defined Class name like Gradient... etc...
	// You might e.g. have your Token::DEFINE_FUNC_RETURN or Token::DEFINE_FUNC_ARGUMENT
	// and the type might be some class name.
	bool isBuiltInType();

	// Now, if we have a builtin type, it will be set and get from builtInType(),
	// if we don't, it'll be get and set from m_type which is just a string,
	// and can contain all the user defined types.
	string type();
	string typeInCpp();
	void   type(string set);
	protected: string m_type;
	
public:
	string typedefOldType();
	string typedefOldTypeInCpp();
	string typedefNewType();

	Element* arrayContainedTypeElement();

	// Templates are hugely TODO:
	// stuff for templates: first we have a second type for the template
	// like in:
	// class FirstType(SecondType)
	// or when using it:
	// FirstType!(SecondType) name

	Element* templateSecondType();
	//Not up to date, probably obsolete:
	void createTemplateSecondType(string set_type);
	string templateSecondTypeString();

	// this is usefull when constructing the template class
	// in c++. It will be a normal c++ class:
	// rae template definition:
	// class FirstType(T)
	// rae template use definition:
	// FirstType!(SecondType) name
	// in C++ we construct artificial classes from templates:
	// This is what templateTypeCombination gives:
	// class _FirstTypeSecondType_
	// and use them normally:
	// FirstTypeSecondType* name = new FirstTypeSecondType();

	string templateSecondTypeStringInCpp();
	// We don't use the C++ type here. We use Rae types.
	string templateTypeCombination();

	
	// property kind
	public: Kind::e kind() { return m_kind; }
	public: void kind(Kind::e set) { m_kind = set; }
	protected: Kind::e m_kind;

	// Convert from e.g. val to ref
	public: void typeConvert(Kind::e from, Kind::e to)
	{
		m_typeConvertFrom = from;
		m_typeConvertTo = to;
	}

	public: Kind::e typeConvertFrom() { return m_typeConvertFrom; }
	public: void typeConvertFrom(Kind::e set) { m_typeConvertFrom = set; }
	protected: Kind::e m_typeConvertFrom;

	public: Kind::e typeConvertTo() { return m_typeConvertTo; }
	public: void typeConvertTo(Kind::e set) { m_typeConvertTo = set; }
	protected: Kind::e m_typeConvertTo;

	// ContainerType is an addition to Kind and DefineReference.
	// It tells if we're just single or an array, or something else.
	public: ContainerType::e containerType() { return m_containerType; }
	public: void containerType(ContainerType::e set) { m_containerType = set; }
	protected: ContainerType::e m_containerType;

	// Only used with static arrays:
	public: int staticArraySize() { return m_staticArraySize; }
	public: void staticArraySize(int set) { m_staticArraySize = set; }
	protected: int m_staticArraySize;

	public: Role::e role() { return m_role; }
	public: void role(Role::e set) { m_role = set; }
	protected: Role::e m_role;

	public: ParseError::e parseError() { return m_parseError; }
	public: void parseError(ParseError::e set) { m_parseError = set; }
	protected: ParseError::e m_parseError;

	public: bool isLet() { return m_isLet; }
	public: void isLet(bool set) { m_isLet = set; }
	protected: bool m_isLet;

	public: bool isExtern()
	{
		if(previousToken() == Token::EXTERN )
			return true;
		//else
		return false;
	}

	//the type is here, if it's a built_in_type, NOW:
	//The reason for this separate builtIntype thing is that
	//we want to be able to change from e.g rae-type long to c++-type
	//long long. I guess, we would be able to do that with just putting
	//the type in the type string, but maybe, maybe, it's easier this way...
	//or not.
	public: BuiltInType::e builtInType() { return m_builtInType; }
	public: void builtInType(BuiltInType::e set) { m_builtInType = set; }
	protected: BuiltInType::e m_builtInType;

	public: string builtInTypeString() { return BuiltInType::toString(m_builtInType); }
	public: string builtInTypeStringCpp();

	// We've moved the UNKNOWN stuff into this property:
	public: bool isUnknownType() { return m_isUnknownType; }
	public: void isUnknownType(bool set)
	{
		#ifdef DEBUG_DEBUGNAME
			if (g_debugName == name())
				cout << "isUnknownType set from: " << (m_isUnknownType ? "true" : "false") << " to: " << (set ? "true" : "false")<< " " << toSingleLineString() << "\n";
		#endif
		if(set == false) // Self healed from the unknown reference, so we set that there was no error either.
			parseError(ParseError::UNDEFINED);
		m_isUnknownType = set;
	}
	protected: bool m_isUnknownType;

	public: Element* namespaceElement() { return m_namespaceElement; }
	public: void namespaceElement(Element* set) { m_namespaceElement = set; }
	protected: Element* m_namespaceElement;

	public: string useNamespaceString()
	{
		if (m_useNamespace)
			return m_useNamespace->name() + "::";
		//else
		return "";
	}
	public: Element* useNamespace() { return m_useNamespace; }
	public: void useNamespace(Element* set) { m_useNamespace = set; }
	protected: Element* m_useNamespace;

	// An important addition: a link to the element where this element is defined:
	// For DEFINE_REFERENCE (and DEFINE_VECTOR and DEFINE_ARRAY) this is the class.
	// For USE_REFERENCE this is the DEFINE_REFERENCE
	// And do we also do: For FUNC_CALL this is the func that is being called?
	public: Element* definitionElement() { return m_definitionElement; }
	public: void definitionElement(Element* set) { m_definitionElement = set; }
	protected: Element* m_definitionElement;

	// A link to the matching parenthesis pair. Currently for parenthesis (), but
	// could be used with {} and []
	public: Element* pairElement() { return m_pairElement; }
	public: void pairElement(Element* set) { m_pairElement = set; }
	protected: Element* m_pairElement;

	public: Element* previousElement() { return m_previousElement; }
	public: void previousElement(Element* set) { m_previousElement = set; }
	protected: Element* m_previousElement;

	public: Element* previous2ndElement()
	{
		if(m_previousElement && m_previousElement->m_previousElement)
			return m_previousElement->m_previousElement;
		//else
		return nullptr;
	}

	public: Element* nextElement() { return m_nextElement; }
	public: void nextElement(Element* set) { m_nextElement = set; }
	protected: Element* m_nextElement;

	//And here's some token shortcuts for those elements.
	public: Token::e previousToken()
	{
		if(m_previousElement == 0)
			return Token::UNDEFINED;
		return m_previousElement->token();
	}
	public: Token::e previous2ndToken()
	{
		if(m_previousElement && m_previousElement->m_previousElement)
			return m_previousElement->m_previousElement->token();
		//else
		return Token::UNDEFINED;
	}
	public: Token::e nextToken()
	{
		if(m_nextElement == 0)
			return Token::UNDEFINED;
		return m_nextElement->token();
	}

	//
	// Oh well, I added another pointer to an element here. 
	// This one is data about initialization... example:
	// int someInt = 5
	// so, initData would be that "="" and contain new Element 5, which is of the type ::NUMBER.
	// SomeClass someOb = new SomeOb(float param1: 256.0, OtherClass param2: ob);
	// Well, that's a bit TODO.
	//

	// TODO REFACTOR: INIT_DATA handling has two overlapping ways. As a normal Element that is in the array,
	// or as an initData member for Element. Refactor to have only one way to store INIT_DATA.

	public: Element* initData() { return m_initData; }
	public: void initData(Element* set) { m_initData = set; }
	protected: Element* m_initData;

	public: Element* addDefaultInitData();

	//
	
	public: LineNumber& lineNumber() { return m_lineNumber; }
	public: void lineNumber(LineNumber& set) { m_lineNumber.copyFrom(set); }
	protected: LineNumber m_lineNumber;
		
public:
	bool isInClass();
	bool isInFunc();

	Token::e parentToken() { return (m_parent ? m_parent->token() : Token::UNDEFINED); }
	Element* parent() { return m_parent; }
	void parent(Element* set) { m_parent = set; }
	protected: Element* m_parent;

public:
	string parentClassName();
	Element* searchClosestParentToken(Token::e set);

	//then some strange checking from the parent, the class and func and stuff:
	Element* scope(); // could be renamed to parentScope
	Element* parentModule() { return searchClosestParentToken(Token::MODULE); }
	Element* parentClass() { return searchClosestParentToken(Token::CLASS); }
	Element* parentFunc();

	Element* searchClosestPreviousUseReferenceOrUseVector();

public:
	static string getVisibilityNameInCpp(string set);

	// Use this with FUNCs
	// returns nullptr if nothing found.
	Element* hasVisibilityFuncChild();

	// For scopeElement and class:

	// These are the elements that are most probably of the type
	// DEFINE_REFERENCE and usually allocate memory with new.
	// So, if not otherwise said, we will deallocate/delete them
	// at the end of this scope/class. Unless their ownership is claimed by
	// some other func.
	// Umm. Something like:
	// func ()push_back(own T set)
	// and then the compiler should also guarantee that the push_back
	// places T set onto some container, or ref T, or ref T?.
	protected: vector<Element*> ownedElements; // TODO This is probably obsolete!

public:
	void own(Element* set) // TODO obsolete?
	{
		ownedElements.push_back(set);
	}

	void freeOwned(); // TODO obsolete?

	//For class:
	void createObjectAutoInitAndFree(Element* init_ob)
	{
		createObjectAutoInitInConstructors(init_ob);
		createObjectAutoFreeInDestructors(init_ob);
	}

	void createObjectAutoInitInConstructors(Element* init_ob);
	void createObjectAutoFreeInDestructors(Element* init_ob);

	//

//TODO protected: ?
	vector<Element*> elements;

	public: Element* currentElement() { return m_currentElement; }
	public: void currentElement(Element* set) { m_currentElement = set; }
	protected: Element* m_currentElement;
	
	// includes the name of this element.
	// e.g. rae.examples.RaeTester(.SomeNamespace).RaeTester.sayHello.notrees
	//      module----------------(>namespace)---->class---->func---->built_in_type
public:
	string namespaceString();
	
	Element* newElement(LineNumber& set_line_number, Token::e set_lang_token_type, Kind::e set_type_type = Kind::UNDEFINED, string set_name = "", string set_type = "" );
	Element* newElementToTop( LineNumber& set_line_number, Token::e set_lang_token_type, Kind::e set_type_type, string set_name = "", string set_type = "" );
	Element* newElementToTopAfterNewlineWithNewline( LineNumber& set_line_number, Token::e set_lang_token_type, Kind::e set_type_type, string set_name = "", string set_type = "", BuiltInType::e set_built_in_type = BuiltInType::UNDEFINED );
	Element* newElementToTopOfFunc( LineNumber& set_line_number, Token::e set_lang_token_type, Kind::e set_type_type, string set_name = "", string set_type = "", BuiltInType::e set_built_in_type = BuiltInType::UNDEFINED );
	Element* newElementToTopOfClass( LineNumber& set_line_number, Token::e set_lang_token_type, Kind::e set_type_type, string set_name = "", string set_type = "", BuiltInType::e set_built_in_type = BuiltInType::UNDEFINED );
	
	void addElement(Element* set);
	void addNewLine();
	void addElementToTop(Element* set);
	
	void addAutoInitElementToFunc(Element* set);
	
	//Use this with a func element:
	//void addElementToTopAfterScopeWithNewline(Element* set)
	void addElementToTopOfFunc(Element* set);
	//This is a helper for the above. Use it with a scope element or class or whatever.
	void addElementToTopAfterNewlineWithNewline(Element* set);
	void addElementToTopOfClass(Element* set);
	void addNameToCurrentElement( string set_name );
	void addTypeToCurrentElement( string set_type );
	void copyChildElementsFrom( Element& from );

	// TODO clean up hasFunctionOrMember and searchName.
	// They are the same currently. Are they the same also conceptually,
	// if so then merge as hasFunctionOrMember which is more descriptive.
	Element* hasFunctionOrMember(string set_name);
	Element* searchName(string set_name);	
	Element* searchFirst(Token::e set_lang_token_type);
	Element* searchFirst(Token::e set_lang_token_type, Token::e set_lang_token_type2);
	Element* searchLast(Token::e set_lang_token_type);
};

} // end namespace Rae

#endif //RAE_COMPILER_LANGELEMENT_HPP

