
#include "Element.hpp"

namespace Rae
{

namespace Kind
{
string toString(Kind::e set)
{
	switch(set)
	{
		default:
			return "RAE_ERROR Kind:: not recognized.";
		case Kind::UNDEFINED:
			return "Kind::UNDEFINED";
		case Kind::VAL:
			return "Kind::VAL";
		case Kind::REF:
			return "Kind::REF";
		case Kind::OPT:
			return "Kind::OPT";
		case Kind::LINK:
			return "Kind::LINK";
		case Kind::PTR:
			return "Kind::PTR";
		case Kind::BUILT_IN_TYPE:
			return "Kind::BUILT_IN_TYPE";
		//case Kind::ARRAY:
			//return "Kind::ARRAY";
		case Kind::VECTOR:
			return "Kind::VECTOR";
		case Kind::TEMPLATE:
			return "Kind::TEMPLATE";
	}
}

Kind::e fromString(const string& set)
{
	if( set == "val" )
		return Kind::VAL;
	else if( set == "ref" )
		return Kind::REF;
	else if( set == "opt" )
		return Kind::OPT;
	else if( set == "link" )
		return Kind::LINK;
	else if( set == "ptr" )
		return Kind::PTR;
	else if( BuiltInType::isBuiltInType(set) )
		return Kind::BUILT_IN_TYPE;
	//else
	return Kind::UNDEFINED;
}
}

namespace ContainerType
{
string toString(ContainerType::e set)
{
	switch(set)
	{
		default:
			return "RAE_ERROR ContainerType:: not recognized.";
		case ContainerType::UNDEFINED:
			return "ContainerType::UNDEFINED";
		case ContainerType::ARRAY:
			return "ContainerType::ARRAY";
		case ContainerType::STATIC_ARRAY:
			return "ContainerType::STATIC_ARRAY";
	}
}
}

namespace Token
{
string toString(Token::e set)
	{
		switch(set)
		{
			default:
			return "RAE_ERROR Token:: not recognized.";
			case Token::UNDEFINED:
			return "Token::UNDEFINED";
			case Token::EMPTY:
			return "Token::EMPTY";
			case Token::PASSTHROUGH_HDR:
			return "Token::PASSTHROUGH_HDR";
			case Token::PASSTHROUGH_SRC:
			return "Token::PASSTHROUGH_SRC";
			case Token::RAE_ERROR:
			return "Token::RAE_ERROR";
			case Token::MODULE:
			return "Token::MODULE";
			case Token::MODULE_NAME:
			return "Token::MODULE_NAME";
			case Token::MODULE_DIR:
			return "Token::MODULE_DIR";
			case Token::CLOSE_MODULE:
			return "Token::CLOSE_MODULE";
			case Token::IMPORT:
			return "Token::IMPORT";
			case Token::IMPORT_NAME:
			return "Token::IMPORT_NAME";
			case Token::PROJECT:
			return "Token::PROJECT";
			case Token::TARGET_DIR:
			return "Token::TARGET_DIR";
			case Token::IMPORT_DIRS:
			return "Token::IMPORT_DIRS";
			case Token::NAMESPACE:
			return "Token::NAMESPACE";
			case Token::USE_NAMESPACE:
			return "Token::USE_NAMESPACE";
			case Token::CLASS:
			return "Token::CLASS";
			case Token::CLASS_NAME:
			return "Token::CLASS_NAME";
			case Token::CLASS_TEMPLATE_SECOND_TYPE:
			return "Token::CLASS_TEMPLATE_SECOND_TYPE";
			case Token::DEFINE_REFERENCE:
			return "Token::DEFINE_REFERENCE";
			case Token::DEFINE_REFERENCE_NAME:
			return "Token::DEFINE_REFERENCE_NAME";
			case Token::USE_REFERENCE:
			return "Token::USE_REFERENCE";
			case Token::USE_MEMBER:
			return "Token::USE_MEMBER";
			
			case Token::ARRAY_VECTOR_STUFF:
			return "Token::ARRAY_VECTOR_STUFF";
			case Token::VECTOR_STUFF:
			return "Token::VECTOR_STUFF";
			case Token::VECTOR_NAME:
			return "Token::VECTOR_NAME";
			
			case Token::TEMPLATE_STUFF:
			return "Token::TEMPLATE_STUFF";
			case Token::TEMPLATE_NAME:
			return "Token::TEMPLATE_NAME";
			case Token::TEMPLATE_SECOND_TYPE:// firstType!(secondType) name
			return "Token::TEMPLATE_SECOND_TYPE";

			case Token::DEFINE_BUILT_IN_TYPE_NAME:
			return "Token::DEFINE_BUILT_IN_TYPE_NAME";
			
			case Token::ALIAS:
			return "Token::ALIAS";

			case Token::NUMBER:
			return "Token::NUMBER";
			case Token::FLOAT_NUMBER:
			return "Token::FLOAT_NUMBER";
			case Token::DECIMAL_NUMBER_AFTER_DOT:
			return "Token::DECIMAL_NUMBER_AFTER_DOT";
			
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
			
			case Token::INIT_DATA:
			return "Token::INIT_DATA";
			case Token::ACTUAL_INIT_DATA:
			return "Token::ACTUAL_INIT_DATA";

			case Token::RAE_NULL:
			return "Token::RAE_NULL";
			
			case Token::VISIBILITY_DEFAULT:
			return "Token::VISIBILITY_DEFAULT";
			case Token::VISIBILITY:
			return "Token::VISIBILITY";
			case Token::VISIBILITY_PARENT_CLASS:
			return "Token::VISIBILITY_PARENT_CLASS";
			
			case Token::LET:
			return "Token::LET";
			case Token::MUT:
			return "Token::MUT";

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
			case Token::LOG_S:
			return "Token::LOG_S";
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
			case Token::PARENTHESIS_BEGIN_LOG_S:
			return "Token::PARENTHESIS_BEGIN_LOG_S";
			case Token::PARENTHESIS_END_LOG_S:
			return "Token::PARENTHESIS_END_LOG_S";
			case Token::PARENTHESIS_BEGIN_FUNC_RETURN_TYPES:
			return "Token::PARENTHESIS_BEGIN_FUNC_RETURN_TYPES";
			case Token::PARENTHESIS_END_FUNC_RETURN_TYPES:
			return "Token::PARENTHESIS_END_FUNC_RETURN_TYPES";
			case Token::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES:
			return "Token::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES";
			case Token::PARENTHESIS_END_FUNC_PARAM_TYPES:
			return "Token::PARENTHESIS_END_FUNC_PARAM_TYPES";
			
			case Token::BRACKET_BEGIN:
			return "Token::BRACKET_BEGIN";
			case Token::BRACKET_END:
			return "Token::BRACKET_END";
			case Token::BRACKET_DEFINE_ARRAY_BEGIN:
			return "Token::BRACKET_DEFINE_ARRAY_BEGIN";
			case Token::BRACKET_DEFINE_ARRAY_END:
			return "Token::BRACKET_DEFINE_ARRAY_END";
			case Token::BRACKET_DEFINE_STATIC_ARRAY_BEGIN:
			return "Token::BRACKET_DEFINE_STATIC_ARRAY_BEGIN";
			case Token::BRACKET_DEFINE_STATIC_ARRAY_END:
			return "Token::BRACKET_DEFINE_STATIC_ARRAY_END";
			case Token::BRACKET_INITIALIZER_LIST_BEGIN:
			return "Token::BRACKET_INITIALIZER_LIST_BEGIN";
			case Token::BRACKET_INITIALIZER_LIST_END:
			return "Token::BRACKET_INITIALIZER_LIST_END";
			case Token::BRACKET_CAST_BEGIN:
			return "Token::BRACKET_CAST_BEGIN";
			case Token::BRACKET_CAST_END:
			return "Token::BRACKET_CAST_END";

			case Token::RETURN:
			return "Token::RETURN";
			
			case Token::COMMA:
			return "Token::COMMA";
			case Token::SEMICOLON:
			return "Token::SEMICOLON";

			case Token::CAST:
			return "Token::CAST";

			case Token::ASSIGNMENT:
			return "Token::ASSIGNMENT";
			case Token::PLUS:
			return "Token::PLUS";
			case Token::MINUS:
			return "Token::MINUS";
			case Token::MULTIPLY:
			return "Token::MULTIPLY";
			case Token::DIVIDE:
			return "Token::DIVIDE";
			case Token::MODULO:
			return "Token::MODULO";
			case Token::OPERATOR_INCREMENT:
			return "Token::OPERATOR_INCREMENT";
			case Token::OPERATOR_DECREMENT:
			return "Token::OPERATOR_DECREMENT";
			case Token::PLUS_ASSIGN:
			return "Token::PLUS_ASSIGN";
			case Token::MINUS_ASSIGN:
			return "Token::MINUS_ASSIGN";
			case Token::MULTIPLY_ASSIGN:
			return "Token::MULTIPLY_ASSIGN";
			case Token::DIVIDE_ASSIGN:
			return "Token::DIVIDE_ASSIGN";
			case Token::MODULO_ASSIGN:
			return "Token::MODULO_ASSIGN";

			case Token::EQUALS:
			return "Token::EQUALS";
			case Token::NOT_EQUAL:
			return "Token::NOT_EQUAL";
			case Token::GREATER_THAN:
			return "Token::GREATER_THAN";
			case Token::LESS_THAN:
			return "Token::LESS_THAN";
			case Token::GREATER_THAN_OR_EQUAL:
			return "Token::GREATER_THAN_OR_EQUAL";
			case Token::LESS_THAN_OR_EQUAL:
			return "Token::LESS_THAN_OR_EQUAL";

			case Token::NOT:
			return "Token::NOT";
			case Token::AND:
			return "Token::AND";
			case Token::OR:
			return "Token::OR";

			case Token::BITWISE_OR:
			return "Token::BITWISE_OR";
			case Token::BITWISE_AND:
			return "Token::BITWISE_AND";
			case Token::BITWISE_XOR:
			return "Token::BITWISE_XOR";
			case Token::BITWISE_COMPLEMENT:
			return "Token::BITWISE_COMPLEMENT";

			case Token::POINT_TO:
			return "Token::POINT_TO";
			case Token::POINT_TO_END_PARENTHESIS:
			return "Token::POINT_TO_END_PARENTHESIS";
			case Token::IS:
			return "Token::IS";

			case Token::IF:
			return "Token::IF";
			case Token::ELSE:
			return "Token::ELSE";
			case Token::FOR:
			return "Token::FOR";
			case Token::FOR_EACH:
			return "Token::FOR_EACH";
			case Token::IN_TOKEN:
			return "Token::IN_TOKEN";

			case Token::TRUE_TRUE:
			return "Token::TRUE_TRUE";
			case Token::FALSE_FALSE:
			return "Token::FALSE_FALSE";
			
			case Token::SIZEOF:
			return "Token::SIZEOF";
			case Token::EXTERN:
			return "Token::EXTERN";

			case Token::NEWLINE:
			return "Token::NEWLINE";
			case Token::NEWLINE_BEFORE_SCOPE_END:
			return "Token::NEWLINE_BEFORE_SCOPE_END";

			case Token::PRAGMA_CPP:
			return "Token::PRAGMA_CPP";
			case Token::PRAGMA_CPP_HDR:
			return "Token::PRAGMA_CPP_HDR";
			case Token::PRAGMA_CPP_SRC:
			return "Token::PRAGMA_CPP_SRC";
			case Token::PRAGMA_CPP_END:
			return "Token::PRAGMA_CPP_END";
			case Token::PRAGMA_ASM:
			return "Token::PRAGMA_ASM";
			case Token::PRAGMA_ASM_END:
			return "Token::PRAGMA_ASM_END";
			case Token::PRAGMA_ECMA:
			return "Token::PRAGMA_ECMA";
			case Token::PRAGMA_ECMA_END:
			return "Token::PRAGMA_ECMA_END";
			
			case Token::EXPECTING_NAME:
			return "Token::EXPECTING_NAME";
			case Token::EXPECTING_TYPE:
			return "Token::EXPECTING_TYPE";

			// C++

			case Token::EXPECTING_CPP_PREPROCESSOR:
			return "Token::EXPECTING_CPP_PREPROCESSOR";
			case Token::CPP_PRE_DEFINE:
			return "Token::CPP_PRE_DEFINE";
			case Token::EXPECTING_CPP_PRE_DEFINE_VALUE:
			return "Token::EXPECTING_CPP_PRE_DEFINE_VALUE";

			case Token::CPP_UNSIGNED:
			return "Token::CPP_UNSIGNED";
			case Token::CPP_SIGNED:
			return "Token::CPP_SIGNED";
			case Token::CPP_TYPEDEF:
			return "Token::CPP_TYPEDEF";
			case Token::EXPECTING_CPP_TYPEDEF_TYPENAME:
			return "Token::EXPECTING_CPP_TYPEDEF_TYPENAME";
		}
	}

	Token::e matchParenthesisEnd(Token::e parenthesis_begin_type)
	{
		switch(parenthesis_begin_type)
		{
			default:
			return Token::RAE_ERROR;
			case Token::PARENTHESIS_BEGIN:
			//return Token::PARENTHESIS_BEGIN;
			//case Token::PARENTHESIS_END:
			return Token::PARENTHESIS_END;
			case Token::PARENTHESIS_BEGIN_LOG:
			//return Token::PARENTHESIS_BEGIN_LOG;
			//case Token::PARENTHESIS_END_LOG:
			return Token::PARENTHESIS_END_LOG;
			case Token::PARENTHESIS_BEGIN_LOG_S:
			//return Token::PARENTHESIS_BEGIN_LOG_S;
			//case Token::PARENTHESIS_END_LOG_S:
			return Token::PARENTHESIS_END_LOG_S;
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
			//return Token::RAE_ERROR; // We might think this is an error,
			return Token::BRACKET_END; //but we don't.
			case Token::BRACKET_BEGIN:
			//return Token::BRACKET_BEGIN;
			//case Token::BRACKET_END:
			return Token::BRACKET_END;
			case Token::BRACKET_DEFINE_ARRAY_BEGIN:
			//return Token::BRACKET_DEFINE_ARRAY_BEGIN;
			//case Token::BRACKET_DEFINE_ARRAY_END:
			return Token::BRACKET_DEFINE_ARRAY_END;
			case Token::BRACKET_DEFINE_STATIC_ARRAY_BEGIN:
			return Token::BRACKET_DEFINE_STATIC_ARRAY_END;
			case Token::BRACKET_INITIALIZER_LIST_BEGIN:
			return Token::BRACKET_INITIALIZER_LIST_END;
			case Token::BRACKET_CAST_BEGIN:
			return Token::BRACKET_CAST_END;
		}

	}

	bool isNewline(Token::e set)
	{
		if( set == Token::NEWLINE
			|| set == Token::NEWLINE_BEFORE_SCOPE_END)
			return true;
		return false;
	}
}


namespace BuiltInType
{
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
			return "wchar"; // TODO check if it works and is int32_t (or 16 bits better...)
			case BuiltInType::DCHAR:
			return "dchar";
			case BuiltInType::SHORT:
			return "short";
			case BuiltInType::USHORT:
			return "ushort";
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
			case BuiltInType::A_VOID:
			return "void";
		}
	}

/*
// From D programming language we have taken the following built in types:
// In the middle there's the default initializer, but in Rae we've changed
// D's float nan's to 0.0fs.
void	-	no type
bool	false	boolean value
byte	0	signed 8 bits
ubyte	0	unsigned 8 bits
short	0	signed 16 bits
ushort	0	unsigned 16 bits
int	0	signed 32 bits
uint	0	unsigned 32 bits
long	0L	signed 64 bits
ulong	0L	unsigned 64 bits
cent	0	signed 128 bits (reserved for future use)
ucent	0	unsigned 128 bits (reserved for future use)
float	float.nan	32 bit floating point
double	double.nan	64 bit floating point
real	real.nan	largest FP size implemented in hardwareImplementation Note: 80 bits for x86 CPUs or double size, whichever is larger
char	0xFF	unsigned 8 bit (UTF-8 code unit)
wchar	0xFFFF	unsigned 16 bit (UTF-16 code unit)
dchar	0x0000FFFF	unsigned 32 bit (UTF-32 code unit)
*/

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
			case BuiltInType::SHORT:
			return "int16_t";
			case BuiltInType::USHORT:
			return "uint16_t";
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
			case BuiltInType::A_VOID:
			return "void";
		}
	}

	BuiltInType::e stringToBuiltInType(string set_token)
	{
		if( set_token == "bool" )
			return BuiltInType::BOOL;
		else if( set_token == "byte" )
			return BuiltInType::BYTE;
		else if( set_token == "ubyte" )
			return BuiltInType::UBYTE;
		else if( set_token == "char" )
			return BuiltInType::CHAR;
		else if( set_token == "wchar" )
			return BuiltInType::WCHAR;
		else if( set_token == "dchar" )
			return BuiltInType::DCHAR;
		else if( set_token == "short" )
			return BuiltInType::SHORT;
		else if( set_token == "ushort" )
			return BuiltInType::USHORT;
		else if( set_token == "int" )
			return BuiltInType::INT;
		else if( set_token == "uint" )
			return BuiltInType::UINT;
		else if( set_token == "long" )
			return BuiltInType::LONG;
		else if( set_token == "ulong" )
			return BuiltInType::ULONG;
		else if( set_token == "float" )
			return BuiltInType::FLOAT;
		else if( set_token == "double" )
			return BuiltInType::DOUBLE;
		/*else if( set_token == "real" )
			return BuiltInType::REAL;*/
		else if( set_token == "string" )
			return BuiltInType::STRING;
		else if( set_token == "void" )
			return BuiltInType::A_VOID;

		return BuiltInType::UNDEFINED;
	}

	BuiltInType::e cppStringToBuiltInType(string set_token)
	{
		if( set_token == "bool" )
			return BuiltInType::BOOL;
		//Byte?
		else if( set_token == "char" ) // C's char -> Rae char
			return BuiltInType::CHAR;
		else if( set_token == "int" || set_token == "int32_t" )
			return BuiltInType::INT; // CHECK should this actually be LONG as in 64 bits
		else if( set_token == "uint" || set_token == "uint32_t" )
			return BuiltInType::UINT; // CHECK should this actually be ULONG as in 64 bits
		else if( set_token == "long" )
			return BuiltInType::INT; // NOTE! in C++ long is equal to int. Use long long instead.
		else if( set_token == "ulong" )
			return BuiltInType::UINT;
		else if( set_token == "int64_t" || set_token == "long long" )
			return BuiltInType::LONG;
		else if( set_token == "uint64_t" || set_token == "unsigned long long" )
			return BuiltInType::ULONG;
		else if( set_token == "float" )
			return BuiltInType::FLOAT;
		else if( set_token == "double" )
			return BuiltInType::DOUBLE;
		/*else if( set_token == "real" )
			return BuiltInType::REAL;*/
		else if( set_token == "string" )
			return BuiltInType::STRING;
		else if( set_token == "void" )
			return BuiltInType::A_VOID;
		// I'm moving these less common types to the bottom as a premature micro-optimization (PMO):
		else if( set_token == "short" || set_token == "int16_t" )
			return BuiltInType::SHORT;
		else if( set_token == "unsigned short" || set_token == "uint16_t" || set_token == "ushort" )
			return BuiltInType::USHORT;
		else if( set_token == "uchar" || set_token == "unsigned char" || set_token == "uint8_t") // C's unsigned char -> Rae ubyte
			return BuiltInType::UBYTE;
		else if( set_token == "signed char" || set_token == "int8_t")
			return BuiltInType::BYTE;

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

void LineNumber::copyFrom(LineNumber& set)
{
	totalCharNumber = set.totalCharNumber;
	line = set.line;
	column = set.column;
}

void LineNumber::printOut()
{
	cout<<"line: "<<line<<" charpos: "<<column;
	////rae::log("line: ", line, " charpos: ", column);
}



// class Element ---------------------------------------------------------------------------------

bool Element::isWhiteSpace()
{
	if( ::isWhiteSpace(name()) )
		return true;
	return false;
}

string Element::toString()
{
	string ret = "\tname: >" + name() + "<\n\t" + tokenString() + "\n\t" + "typetype: " + kindString() + "\n\t" + "type: " + type() + "containerType: " + ContainerType::toString(containerType()) + "\n\t" + "line: " + numberToString(lineNumber().line) + "\n";
	if( ::isWhiteSpace(name()) )
	{
		ret += "The name is whitespace.\n";
		ret += toSingleLineString() + "\n";
	}
	return ret;
}

string Element::toSingleLineString()
{
	if( m_name.size() == 1 )
	{
		if( m_name[0] == ' ' )
			return "SPACE";
		else if( m_name[0] == '\n' && (token() == Token::NEWLINE || token() == Token::NEWLINE_BEFORE_SCOPE_END) )
			return "NEWLINE";
		else if( m_name[0] == '\n' )
			return "RETURN WHICH IS NOT NEWLINE.";
		else if( m_name[0] == '\t' )
			return "TAB";
	}
	//else
	//string ret = "name: " + name() + " " + tokenString() + " typetype: " + kindString() + " type: " + type() + " line: " + numberToString(lineNumber().line);
	string ret;

	if(token() == Token::CPP_TYPEDEF) // Not a very good thing to have this here just for this one Token type. But as long as this is just for debugging it's maybe ok.
	{
		ret += "typedef " + typedefNewType() + " = " + typedefOldType();
	}
	else
	{
		if(type() != "")
			ret += "type:<" + type() + "> ";
		else ret += "type:empty ";
		
		if(name() != "")
			ret += "name:<" + name() + "> ";
		else ret += "name:empty ";
	}

	// Add spaces to align next tokenString at 25 chars
	for(size_t i = ret.length(); i < 25; ++i)
	{
		ret += " ";
	}

	ret += tokenString();

	if(kind() != Kind::UNDEFINED)
		ret += " typetype: " + kindString();
	if(containerType() != ContainerType::UNDEFINED)
		ret += " containerType: " + containerTypeString();
	if(isUnknownType() == true)
		ret += " isUnknownType.";

	if (useNamespace() != nullptr)
		ret += " usenamespace: " + useNamespaceString();
	
	ret += " line: " + numberToString(lineNumber().line);

	ret += " " + namespaceString();

	return ret;
}

// debugging helps:

void Element::printOutElements()
{
	cout<<"printOutElements():\n";
	uint i = 0;
	for( Element* elem : elements )
	{
		cout<<i<<": "<<elem->toString()<<"\n";
		i++;
	}
	cout<<"\n";
}

void Element::printOutListOfFunctions()
{
	cout<<"Functions in "<<toString()<<"\n-------------------------\n";
	for(Element* elem : elements)
	{
		if( elem->token() == Token::FUNC )
		{
			cout<<elem->toString()<<"\n";
		}
	}
	cout<<"-------------------------\n";
}

void Element::printOutListOfReferences()
{
	cout<<"Defined references in "<<toString()<<"\n-------------------------\n";
	for(Element* elem : elements)
	{
		if( elem->token() == Token::DEFINE_REFERENCE )
		{
			cout<<elem->toString()<<"\n";
		}
	}
	cout<<"-------------------------\n";
}

// a specialized func for use with Token::IMPORT
string Element::importName(string separator_char_str)
{
	if(token() != Token::IMPORT)
	{
		cout<<"Error: this is not an import. Can't get importName(): "<<toString()<<"\n";
		return "Not an import.";
	}

	string a_module_name = "";

	int not_on_first = 0;

	for(Element* elem : elements)
	{
		if( elem->token() == Token::IMPORT_NAME )
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
			break;
		}
	}

	return a_module_name;
}

// You probably want to use parentModuleString instead.
string Element::moduleString(string separator_char_str)
{
	if(token() != Token::MODULE)
	{
		cout<<"Error: this is not a module. Can't get moduleName(): "<<toString()<<"\n";
		return "Not a module.";
	}

	string a_module_name = "";

	int not_on_first = 0;

	for(Element* elem : elements)
	{
		if( elem->token() == Token::MODULE_NAME )
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
			break;
		}
	}

	return a_module_name;
}

// This just shows in which module we are at the moment.
string Element::parentModuleString()
{
	Element* our_module = parentModule();
	if(our_module)
		return our_module->moduleString();
	return "No module";
}

bool Element::isOperator()
{
	// This doesn't return ASSIGNMENT or INCREMENT and DECREMENT, because
	// those have different rules in validate.
	if( token() == Token::PLUS
		|| token() == Token::MINUS
		|| token() == Token::MULTIPLY
		|| token() == Token::DIVIDE
		|| token() == Token::MODULO
		|| token() == Token::ASSIGNMENT
		|| token() == Token::OPERATOR_INCREMENT
		|| token() == Token::OPERATOR_DECREMENT
		|| token() == Token::EQUALS
		|| token() == Token::NOT_EQUAL
		|| token() == Token::GREATER_THAN
		|| token() == Token::LESS_THAN
		|| token() == Token::GREATER_THAN_OR_EQUAL
		|| token() == Token::LESS_THAN_OR_EQUAL
		//|| token() == Token::NOT
		//|| token() == Token::AND
		//|| token() == Token::OR
		)
		return true;
	//else
	return false;
}

bool Element::isMathOperator()
{
	// This doesn't return ASSIGNMENT or INCREMENT and DECREMENT, because
	// those have different rules in validate.
	if( token() == Token::PLUS
		|| token() == Token::MINUS
		|| token() == Token::MULTIPLY
		|| token() == Token::DIVIDE
		|| token() == Token::MODULO
		)
		return true;
	//else
	return false;
}

bool Element::isEndsExpression()
{
	if(    token() == Token::PARENTHESIS_END
		|| token() == Token::PARENTHESIS_END_FUNC_PARAM_TYPES
		|| token() == Token::POINT_TO_END_PARENTHESIS
		|| token() == Token::PARENTHESIS_BEGIN
		|| token() == Token::COMMA
		|| token() == Token::SEMICOLON
		|| token() == Token::COMMENT // yes. single line comments also end expressions.
		|| token() == Token::NEWLINE
		|| token() == Token::NEWLINE_BEFORE_SCOPE_END
	)
	{
		return true;
	}
	//else
	return false;
}

bool Element::isDefinition()
{
	if( token() == Token::DEFINE_REFERENCE
		//|| token() == Token::DEFINE_REFERENCE_IN_CLASS
		//|| token() == Token::DEFINE_ARRAY
		//|| token() == Token::DEFINE_ARRAY_IN_CLASS
		//|| token() == Token::DEFINE_VECTOR
		//|| token() == Token::DEFINE_VECTOR_IN_CLASS
		//|| token() == Token::DEFINE_BUILT_IN_TYPE
		//|| token() == Token::DEFINE_BUILT_IN_TYPE_IN_CLASS
		|| token() == Token::BRACKET_DEFINE_ARRAY_BEGIN
		|| token() == Token::BRACKET_DEFINE_STATIC_ARRAY_BEGIN
		|| token() == Token::CLASS
		|| token() == Token::ENUM
		|| token() == Token::FUNC
		|| token() == Token::CONSTRUCTOR
		|| token() == Token::DESTRUCTOR
		|| token() == Token::MAIN
		//REMOVED: || token() == Token::DEFINE_FUNC_ARGUMENT
		|| token() == Token::DEFINE_FUNC_RETURN
		|| token() == Token::NAMESPACE
		|| token() == Token::MODULE_NAME
		|| token() == Token::CPP_TYPEDEF
		|| token() == Token::CPP_PRE_DEFINE
		|| token() == Token::ALIAS
	)
	{
		return true;
	}
	//else
	return false;
}

bool Element::isBeginsScope()
{
	if( isFunc()
		|| token() == Token::SCOPE_BEGIN
		|| token() == Token::CLASS
		|| token() == Token::ENUM
	)
	{
		return true;
	}
	//else
	return false;
}

bool Element::isParenthesis()
{
	if( token() == Token::PARENTHESIS_BEGIN
		|| token() == Token::PARENTHESIS_END
		|| token() == Token::PARENTHESIS_BEGIN_LOG
		|| token() == Token::PARENTHESIS_END_LOG
		|| token() == Token::PARENTHESIS_BEGIN_LOG_S
		|| token() == Token::PARENTHESIS_END_LOG_S
		|| token() == Token::PARENTHESIS_BEGIN_FUNC_RETURN_TYPES
		|| token() == Token::PARENTHESIS_END_FUNC_RETURN_TYPES
		|| token() == Token::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES
		|| token() == Token::PARENTHESIS_END_FUNC_PARAM_TYPES
	)
	{
		return true;
	}
	//else
	return false;
}

bool Element::isNewline()
{
	if( token() == Token::NEWLINE
		|| token() == Token::NEWLINE_BEFORE_SCOPE_END)
		return true;
	return false;
}

bool Element::isFunc()
{
	if( token() == Token::FUNC
		|| token() == Token::CONSTRUCTOR
		|| token() == Token::DESTRUCTOR
		|| token() == Token::MAIN
	)
	{
		return true;
	}
	//else
	return false;
}

bool Element::isUseReference()
{
	if( token() == Token::USE_REFERENCE
		//|| token() == Token::USE_ARRAY
		//|| token() == Token::USE_VECTOR
		//|| token() == Token::USE_BUILT_IN_TYPE
		|| token() == Token::USE_MEMBER
		|| token() == Token::FUNC_CALL
	)
	{
		return true;
	}
	//else
	return false;
}

bool Element::isUserDefinableToken()
{
	//We need to add other user defined types here later...
	if( token() == Token::CLASS
		|| token() == Token::ENUM )
		{
			return true;
		}
		//else
		return false;
	}

//The expressionRValue. This is important.
Element* Element::expressionRValue()
{
	#ifdef DEBUG_RAE_RVALUE
	cout<<"Element::expressionRValue() START. "<<toString()<<"\n";
	#endif

	//TODO handle () handle ?. etc.

	if( token() == Token::FUNC_CALL )
	{
		#ifdef DEBUG_RAE_RVALUE
		cout<<"Element::expressionRValue() it is a FUNC_CALL.\n";
		#endif

		Element* ret_type = funcReturnType();
		if(ret_type)
		{
			#ifdef DEBUG_RAE_RVALUE
			cout<<"Element::expressionRValue() and it has a funcReturnType().\n";
			#endif
			//return ret_type->expressionRValue();
			return ret_type;
		}
		else
		{
			#ifdef DEBUG_RAE_RVALUE
			cout<<"Element::expressionRValue() bohoo. no funcReturnType() while it is a FUNC_CALL. Strange. Odd. Probably an error: "<<toString()<<"\n";
			#endif
			ReportError::reportError(std::string("Compiler asked for statement return value, but this function call doesn't return anything."), this);
			return nullptr; //if a func has no return type then there's no expressionRValue.
		}
	}
	else if( token() == Token::USE_REFERENCE
				|| token() == Token::USE_MEMBER
	)
	{
		#ifdef DEBUG_RAE_RVALUE
		cout<<"Element::expressionRValue() it is a USE_REFERENCE or USE_MEMBER." << toSingleLineString() << "\n";
		#endif

		if( nextElement() == nullptr
		|| nextElement()->isEndsExpression()
		)
		{
			#ifdef DEBUG_RAE_RVALUE
			cout << "IS ENDS EXPRESSION.";
			#endif
			return this;
		}
		else
		{
			#ifdef DEBUG_RAE_RVALUE
			cout << "IS NOT ENDS EXPRESSION: " << nextElement()->toSingleLineString() << "\n";
			#endif
			return nextElement()->expressionRValue();
		}
	}
	else if( token() == Token::REFERENCE_DOT )
	{
		#ifdef DEBUG_RAE_RVALUE
		cout<<"Element::expressionRValue() got a REFERENCE_DOT.\n";
		#endif

		if( nextElement() == 0
		|| nextElement()->isEndsExpression()
		)
		{
			//TODO errors in element with reportError?
			ReportError::reportError("Error. No USE_REFERENCE or FUNC_CALL after REFERENCE_DOT.", this);
			return nullptr;
		}
		else
		{
			return nextElement()->expressionRValue();
		}	
	}
	else if( token() == Token::QUOTE || token() == Token::NUMBER )
	{
		// This function does not have any sense!
		return this;
	}
	else
	{
		#ifdef DEBUG_RAE_RVALUE
		cout<<"Element::expressionRValue() no expressionRValue.\n";
		#endif
		//no expressionRValue.
		return nullptr;
	}

	cout<<"shouldn't get here.\n";
	//assert(0);
	return nullptr;
}

Element* Element::funcReturnType()
{
	if( !isFunc() && (token() != Token::FUNC_CALL))
	{
		ReportError::reportError("Compiler error: Trying to get funcReturnType, but this is not a FUNC or a FUNC_CALL.", this);
		assert(0);
	}

	if( token() == Token::FUNC_CALL )
	{
		if(definitionElement() != 0)
		{
			return definitionElement()->funcReturnType();
		}
		else
		{
			ReportError::reportError("Couldn't get funcReturnType for FUNC_CALL, because it's definition is not (yet) found.", this);
			/////////assert(0);
		}
	}
	//else it's a FUNC:
			
	Element* first_return_elem;
	Element* myelem = searchFirst(Token::PARENTHESIS_BEGIN_FUNC_RETURN_TYPES);

	if( myelem != 0 )
	{
		#ifdef DEBUG_RAE_RVALUE
		cout<<"Element::funcReturnType() found PARENTHESIS_BEGIN_FUNC_RETURN_TYPES. ok.\n";
		#endif

		if( myelem->token() == Token::PARENTHESIS_BEGIN_FUNC_RETURN_TYPES )
		{
			first_return_elem = myelem->nextElement();

			if(first_return_elem != 0)
			{
				if( first_return_elem->token() == Token::PARENTHESIS_END_FUNC_RETURN_TYPES )
				{
					#ifdef DEBUG_RAE_RVALUE
					cout<<"Element::funcReturnType() nothing between parentheses. void return type.\n";
					#endif
					//Nothing between parentheses. void return type. no expressionRValue, by the way.
					return 0;
				}
				else
				{
					#ifdef DEBUG_RAE_RVALUE
					cout<<"Element::funcReturnType() first return element is: "<<first_return_elem->toString()<<"\n";
					#endif
					return first_return_elem;
				}
			}
		}
	}

	#ifdef DEBUG_RAE_RVALUE
	cout<<"Element::funcReturnType() couldn't find PARENTHESIS_BEGIN_FUNC_RETURN_TYPES from func. Error."<<toString()<<"\n";
	
	myelem = searchFirst(Token::PARENTHESIS_BEGIN);

	if( myelem != 0 )
	{
		cout<<"but found PARENTHESIS_BEGIN.\n";
		first_return_elem = myelem->nextElement();
		if(first_return_elem != 0)
		{
			cout<<"Followed by: "<<first_return_elem->toString()<<"\n";
		}
	}
	else
	{
		printOutElements();
	}

	#endif



	// no return type.
	return 0;
}

// other parenthesis except the ones in the start and the end
bool Element::hasListOtherParenthesis(vector<Element*>& list)
{
	// note, we skip first and last index.
	for(uint i = 1; i < list.size()-1; ++i)
	{
		if( list[i]->isParenthesis() )
			return true;
	}
	return false;
}

void Element::elementListUntil(Token::e set_token, vector<Element*>& return_list)
{
	return_list.push_back(this);
	if( token() == set_token )
		return;
	//else
	if(nextElement())
		nextElement()->elementListUntil(set_token, return_list);
}

void Element::elementListUntilPair(vector<Element*>& return_list)
{
	if(pairElement() == nullptr)
	{
		ReportError::reportError("Asked elementListUntilPair, but there's no pair", this);
		return;
	}
	elementListUntilPair(pairElement(), return_list);
}

void Element::elementListUntilPair(Element* pair, vector<Element*>& return_list)
{
	return_list.push_back(this);
	if( pair == this )
		return;
	//else
	if(nextElement())
		nextElement()->elementListUntilPair(pair, return_list);
}

/*
void Element::listBetweenParenthesis(Token::e set_token, vector<Element*>& return_list)
{
	return_list.push_back(this);
	if( token() == set_token )
		return;
	//else
	elementListUntil(set_token, return_list);
}
*/

vector<Element*> Element::funcParameterList()
{
	if( !isFunc() && (token() != Token::FUNC_CALL))
	{
		ReportError::compilerError("Trying to get funcParameterList, but this is not a FUNC or a FUNC_CALL.", this);
		assert(0);
	}

	if( token() == Token::FUNC_CALL )
	{
		if(definitionElement() != 0)
		{
			return definitionElement()->funcParameterList();
		}
		else
		{
			ReportError::reportError("Couldn't get funcParameterList for FUNC_CALL, because it's definition is not (yet) found.", this);
			assert(0);
		}
	}
	// else it's a FUNC:

	vector<Element*> ret;

	bool found_begin_func_param_types = false;

	for(Element* elem: elements)
	{
		if(found_begin_func_param_types == false)
		{
			if( elem->token() == Token::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES )
			{
				found_begin_func_param_types = true;
			}
		}
		else // already found it.
		{
			if( elem->token() == Token::PARENTHESIS_END_FUNC_PARAM_TYPES )
			{
				break; // break the for when the param_types end.
			}

			//if( elem->token() == Token::DEFINE_REFERENCE )
			if( elem->isDefinition() )
			{
				ret.push_back(elem);
			}
		}
	}

	return ret;
}

/*
vector<Element*> Element::funcCallArgumentList()
{
	if( token() != Token::FUNC_CALL )
	{
		ReportError::compilerError("Trying to get funcCallArgumentList, but this is not a FUNC_CALL.", this);
		assert(0);
	}

	vector<Element*> ret;

	bool found_parenthesis_begin = false;

	Element* elem = this;
	while( (elem = elem->nextElement()) )//loop with an assignment
	{
		//ignore everything before the parenthesis
		if(found_parenthesis_begin == false)
		{
			if( elem->token() == Token::PARENTHESIS_BEGIN )
			{
				found_parenthesis_begin = true;
			}
			else if( elem->token() == Token::NEWLINE || elem->token() == Token::NEWLINE_BEFORE_SCOPE_END )
			{
				cout<<"should break newline\n";
				break;//break the loop. didn't find parenthesis.
			}
		}
		else //already found it.
		{
			if( elem->token() == Token::PARENTHESIS_END )
			{
				cout<<"should break parenthesis end.\n";
				break;//break the for when the arguments end.
			}
			else if( elem->token() == Token::NEWLINE || elem->token() == Token::NEWLINE_BEFORE_SCOPE_END )
			{
				ReportError::reportError("funcCallArgumentList: No ending parenthesis for a function call.", this);
				break;//break the loop. didn't find parenthesis. This is an error.
			}
			else if( elem->token() == Token::COMMA )
			{
				cout<<"skip the comma.\n";
				//skip it.
			}
			else
			{
				UNFINISHED IN_TOKEN HERE: because we need to think about how to represent an argument which is a statement like:
				func_call( arg + 1, arg2 * 55 + "an_error" + hello.callSomething(and_this_here_wont_affect_it) )

				cout<<"added elem: "<<elem->toSingleLineString()<<"\n";
				//an argument hopefully.
				if(elem)
					ret.push_back( elem );
			}
		}
	}

	return ret;
}
*/

//type is a user defined Class name like Gradient... etc...
//You might e.g. have your Token::DEFINE_FUNC_RETURN or Token::DEFINE_FUNC_ARGUMENT
//and the type might be some class name.
bool Element::isBuiltInType()
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
string Element::type()
{
	#ifdef DEBUG_RAE_VALIDATE
		// Validate:
		if( m_type == "array" && m_containerType != ContainerType::ARRAY && namespaceString() != "rae.std.array.array" )
		{	
			ReportError::reportError("Array and ContainerType::ARRAY mismatch in debug mode." + m_name + " " + namespaceString(), nullptr);
			assert(0);
		}
	#endif

	if( builtInType() == BuiltInType::UNDEFINED )
	{
		return m_type;
	}
	//else
	return builtInTypeString();
}

string Element::typeInCpp()
{
	if( builtInType() == BuiltInType::UNDEFINED )
	{
		return useNamespaceString() + m_type;
	}
	//else
	return builtInTypeStringCpp();
}

void Element::type(string set)
{
	BuiltInType::e test_built_in_type = BuiltInType::stringToBuiltInType(set);
	if( test_built_in_type != BuiltInType::UNDEFINED )
	{			
		builtInType(test_built_in_type);
		m_type = set;
		return;
	}
	m_type = set;
}

string Element::typedefOldType()
{
	if( builtInType() == BuiltInType::UNDEFINED )
		return m_type; // else
	return builtInTypeString();
}

string Element::typedefOldTypeInCpp()
{
	if( builtInType() == BuiltInType::UNDEFINED )
		return m_type; // else
	return builtInTypeStringCpp();
}

string Element::typedefNewType()
{
	return m_name;
}

Element* Element::arrayContainedTypeElement()
{
	if(m_token == Token::BRACKET_DEFINE_ARRAY_BEGIN && elements.size() > 0)
	{
		return elements[0];
	}
	else if( m_token != Token::USE_REFERENCE)
	{
		ReportError::compilerError("arrayContainedTypeElement. Tried to get it for something that is not an array.", this);
	}
	else if( definitionElement() )
	{
		return definitionElement()->arrayContainedTypeElement();
	}

	return nullptr;
	//return searchFirst(Token::TEMPLATE_SECOND_TYPE);
}

// Template stuff is hugely TODO:
// stuff for templates: first we have a second type for the template
// like in:
// class FirstType(SecondType)
// or when using it:
// FirstType!(SecondType) name

Element* Element::templateSecondType()
{
	if(elements.size() > 0)
	{
		//if( elements[0]->definitionElement() ) // No, we don't want the definitionElement, because that won't tell us if we're val or opt or link.
		//	return elements[0]->definitionElement();
		//else
		return elements[0];
	}
	return nullptr;
	//return searchFirst(Token::TEMPLATE_SECOND_TYPE);
}

//Not up to date:
void Element::createTemplateSecondType(string set_type)
{
	if( searchFirst(Token::TEMPLATE_SECOND_TYPE) != 0 )
	{
		ReportError::reportError("RAE_ERROR in createTemplateSecondType() We already had a second type for the template: " + templateSecondTypeString() + " new second type: " + set_type, this);
	}
	else
	{
		newElement(lineNumber(), Token::TEMPLATE_SECOND_TYPE, Kind::TEMPLATE/*are you sure this is not confusing...*/, /*string set_name:*/ "undefined", /*string set_type:*/ set_type );
	}
}

string Element::templateSecondTypeString()
{
	//returns the type of the TEMPLATE_SECOND_TYPE
	//if it has one. there should be only one child.
	for(Element* set_elem : elements)
	{
		if(set_elem->token() == Token::TEMPLATE_SECOND_TYPE)
			return set_elem->type();
	}
    //else
	return "templateError";
}

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

string Element::templateSecondTypeStringInCpp()
{
	// returns the type of the TEMPLATE_SECOND_TYPE
	// if it has one. there should be only one child.
	for(Element* set_elem : elements)
	{
		if(set_elem->token() == Token::TEMPLATE_SECOND_TYPE)
			return set_elem->typeInCpp();
	}
	//else
	return "templateError";
}

// We don't use the C++ type here. We use Rae types.
string Element::templateTypeCombination()
{
	return "_" + type() + "_" + templateSecondTypeString() + "_";
}

string Element::builtInTypeStringCpp()
{
	string is_let = "";
	if(isLet())
		is_let = "const ";

	if(containerType() == ContainerType::ARRAY )
	{
		return "std::vector<" + is_let + useNamespaceString() + BuiltInType::toCppString(m_builtInType) + ">";
	}
	else if(containerType() == ContainerType::STATIC_ARRAY )
	{
		return "std::array<" + is_let + useNamespaceString() + BuiltInType::toCppString(m_builtInType) + ", " + numberToString(staticArraySize()) + ">";
	}
	//else
	return is_let + useNamespaceString() + BuiltInType::toCppString(m_builtInType);
}

Element* Element::addDefaultInitData()
{
	#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_INIT_DATA)
		cout << "addDefaultInitData START.\n";
	#endif

	// Currently only adds default data to built in types.
	if(isBuiltInType() == false || containerType() != ContainerType::UNDEFINED)
	{
		#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_INIT_DATA)
			cout << "addDefaultInitData: not BUILT_IN_TYPE. Not setting INIT_DATA.\n";
		#endif
		return nullptr;
	}

	Element* lang_elem;

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
	
	lang_elem = new Element(lineNumber(), Token::INIT_DATA, Kind::UNDEFINED, "=");

	lang_elem->newElement(lineNumber(), Token::NUMBER, Kind::UNDEFINED, init_string);

	#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_INIT_DATA)
		cout << "addDefaultInitData: init_string: " << init_string << "\n";

		if (isInClass() )
			cout << "YES isInClass.\n";
		else cout << "Not in a class.\n";

		if (isInFunc() )
			cout << "YES in func.\n";
		else cout << "Not in a func.\n";
	#endif

	//lang_elem = new Element(lineNumber(), Token::UNDEFINED, init_string);
	initData(lang_elem);
	
	return lang_elem;
}

bool Element::isInClass()
{
	if(parent() && parent()->token() == Token::CLASS)
	{
		return true;
	}
	//else
	return false;
}

bool Element::isInFunc()
{
	if(parent() && parent()->isFunc())
	{
		return true;
	}
	//else
	return false;
}

string Element::parentClassName()
{
	Element* par_clas = parentClass();

	if(par_clas)
	{
		return par_clas->name();
	}
	//else
	return "ERRORnotAClass";
}

Element* Element::searchClosestParentToken(Token::e set)
{
	//return null if not found.
	Element* res = parent();
	while( res )
	{
		if( res->token() == set )
		{
			return res;
		}
		res = res->parent();//move up in hierarchy.
	}
	return res;//if not found this will be zero in the end, because that's what ends the while loop.
}

//then some strange checking from the parent, the class and func and stuff:
Element* Element::scope() // could be renamed to parentScope
{
	Element* res = parent();
	while( res )
	{
		if( res->isBeginsScope() )
		{
			return res;
		}
		res = res->parent();//move up in hierarchy.
	}
	return res;	
}

Element* Element::parentFunc()
{
	Element* res = searchClosestParentToken(Token::FUNC);
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

Element* Element::searchClosestPreviousUseReferenceOrUseVector()
{
	//for stuff like:
	//someOb.callSomeFunc //when called from callSomeFunc it will return someOb.
	//someVector[doSomething.extremely(difficult, and_strange)].callSomeFunc //when called from callSomeFunc it will return someVector.

	//return null if not found.

	//first check for reference dot. We have to have that.
	/*if( previousToken() != Token::REFERENCE_DOT )
	{
		return 0;
	}*/

	//ok we have reference_dot.

	//Element* res = previous2ndElement();

	Element* res = previousElement();

	int found_a_bracket = 0;

	while( res )
	{
		if( res->token() == Token::SCOPE_BEGIN )
		{
			//if at any case we would find a scope begin, then we'll just return 0.
			return 0;
		}

		if(found_a_bracket <= 0)//Well if this gets negative it's an error... too many brackets... but it could theoretically happen if we're inside two brackets.
		//so let's just leave it like this, and hope it's correct.
		{
			if( res->token() == Token::USE_REFERENCE
				//|| res->token() == Token::USE_VECTOR
				//|| res->token() == Token::USE_ARRAY
			)
			{
				return res;
			}
			else if( res->token() == Token::BRACKET_END || res->token() == Token::BRACKET_DEFINE_ARRAY_END )
			{
				found_a_bracket++;
			}
		}
		else //there's some brackets there... so skip everything between brackets.
		{
            // TODO if( res->isBracket() )
			if( res->token() == Token::BRACKET_BEGIN || res->token() == Token::BRACKET_DEFINE_ARRAY_BEGIN )
			{
				found_a_bracket--;
			}	
		}
		res = res->previousElement();//move backwards.
	}
	return res;//if not found this will be zero in the end, because that's what ends the while loop.
}	

string Element::getVisibilityNameInCpp(string set)
{
	if(set == "pub")
		return "public: ";
	//REMOVED else if(set == "protected")
		//return "protected: ";
	else if(set == "priv")
		return "protected: "; // priv -> protected !!!!!!!!!
	else if(set == "lib")
		return "/*library*/public: ";
	//TODO or REMOVED else if(set == "hidden")
	//	return "/*hidden*/protected: ";
	//else
	return "";
}

// Use this with FUNCs
// returns nullptr if nothing found.
Element* Element::hasVisibilityFuncChild()
{
	// Will stop at SCOPE_BEGIN
	for( uint i = 0; i < elements.size(); i++ )
	{
		if( elements[i]->token() == Token::VISIBILITY )
		{
			return elements[i];
		}
		else if( elements[i]->token() == Token::SCOPE_BEGIN )
		{
			return nullptr;
		}
	}
	return nullptr;
}

void Element::freeOwned() // TODO obsolete?
{
	if( token() != Token::CLASS )
	{
		for(Element* init_ob : ownedElements)
		{
				Element* auto_init_elem = init_ob->copy();
				auto_init_elem->definitionElement(init_ob);//our init_ob can be found through the definitionElement.
				auto_init_elem->token(Token::AUTO_FREE);
				addElement(auto_init_elem);
				addNewLine();
		}
	}
}

//For class:

void Element::createObjectAutoInitInConstructors(Element* init_ob)
{
	if( init_ob == 0)
	{
		return;
	}

	for( Element* elem : elements )
	{
		if(elem->token() == Token::CONSTRUCTOR)
		{	
			Element* auto_init_elem = init_ob->copy();
			auto_init_elem->definitionElement(init_ob);//our init_ob can be found through the definitionElement.
			auto_init_elem->token(Token::AUTO_INIT);
			elem->addAutoInitElementToFunc(auto_init_elem);
		}
	}		
}

void Element::createObjectAutoFreeInDestructors(Element* init_ob)
{
	if( init_ob == 0)
	{
		return;
	}

	for( Element* elem : elements )
	{
		if(elem->token() == Token::DESTRUCTOR)
		{
			Element* auto_init_elem = init_ob->copy();
			auto_init_elem->definitionElement(init_ob);//our init_ob can be found through the definitionElement.
			auto_init_elem->token(Token::AUTO_FREE);
			elem->addElementToTopOfFunc(auto_init_elem);
		}
	}		
}

//includes the name of this element.
//e.g. rae.examples.RaeTester(.SomeNamespace).RaeTester.sayHello.notrees
//     module----------------(>namespace)---->class---->func---->built_in_type
string Element::namespaceString()
{
	string res = name();
	Element* kind_of_like_recursive = parent();
	while( kind_of_like_recursive )
	{
		res = kind_of_like_recursive->name() + "." + res;
		kind_of_like_recursive = kind_of_like_recursive->parent();
	}
	return res;
}

Element* Element::newElement(LineNumber& set_line_number, Token::e set_lang_token_type, Kind::e set_type_type, string set_name, string set_type )
{
	Element* lang_elem = new Element(set_line_number, set_lang_token_type, set_type_type, set_name, set_type);
	//lang_elem->lineNumber( set_line_number );
	addElement(lang_elem);
	m_currentElement = lang_elem; // Should this be default for addElement too?
	
	#ifdef DEBUG_RAE
		cout<<"this.name: "<<name()<<" Element.newElement: "<<Token::toString(set_lang_token_type)<<" name:>"<<set_name<<"< type: "<<set_type<<"\n";
	#endif

	return lang_elem;
}

Element* Element::newElementToTop( LineNumber& set_line_number, Token::e set_lang_token_type, Kind::e set_type_type, string set_name, string set_type )
{
	Element* lang_elem = new Element(set_line_number, set_lang_token_type, set_type_type, set_name, set_type);
	addElementToTop(lang_elem);
	m_currentElement = lang_elem; // Should this be default for addElement too?
	
	#ifdef DEBUG_RAE
		cout<<"this.name: "<<name()<<" Element.newElementToTop: "<<Token::toString(set_lang_token_type)<<" name:>"<<set_name<<"< type: "<<set_type<<"\n";
	#endif

	return lang_elem;	
}

Element* Element::newElementToTopAfterNewlineWithNewline( LineNumber& set_line_number, Token::e set_lang_token_type, Kind::e set_type_type, string set_name, string set_type, BuiltInType::e set_built_in_type )
{
	Element* lang_elem = new Element(set_line_number, set_lang_token_type, set_type_type, set_name, set_type);
	lang_elem->builtInType(set_built_in_type);
	addElementToTopAfterNewlineWithNewline(lang_elem);
	m_currentElement = lang_elem; // Should this be default for addElement too?
	
	#ifdef DEBUG_RAE
		cout<<"this.name: "<<name()<<" Element.newElementToTop: "<<Token::toString(set_lang_token_type)<<" name:>"<<set_name<<"< type: "<<set_type<<"\n";
	#endif

	return lang_elem;	
}

Element* Element::newElementToTopOfFunc( LineNumber& set_line_number, Token::e set_lang_token_type, Kind::e set_type_type, string set_name, string set_type, BuiltInType::e set_built_in_type )
{
	Element* lang_elem = new Element(set_line_number, set_lang_token_type, set_type_type, set_name, set_type);
	lang_elem->builtInType(set_built_in_type);
	addElementToTopOfFunc(lang_elem);
	m_currentElement = lang_elem; // Should this be default for addElement too?
	
	#ifdef DEBUG_RAE
		cout<<"this.name: "<<name()<<" Element.newElementToTopOfFunc: "<<Token::toString(set_lang_token_type)<<" name:>"<<set_name<<"< type: "<<set_type<<"\n";
	#endif

	return lang_elem;	
}

Element* Element::newElementToTopOfClass( LineNumber& set_line_number, Token::e set_lang_token_type, Kind::e set_type_type, string set_name, string set_type, BuiltInType::e set_built_in_type )
{
	Element* lang_elem = new Element(set_line_number, set_lang_token_type, set_type_type, set_name, set_type);
	lang_elem->builtInType(set_built_in_type);
	addElementToTopOfClass(lang_elem);
	m_currentElement = lang_elem; // Should this be default for addElement too?
	
	#ifdef DEBUG_RAE
		cout<<"this.name: "<<name()<<" Element.newElementToTopOfClass: "<<Token::toString(set_lang_token_type)<<" name:>"<<set_name<<"< type: "<<set_type<<"\n";
	#endif

	return lang_elem;	
}

void Element::addElement(Element* set)
{
	set->parent(this);
	elements.push_back(set);
}

void Element::addNewLine()
{
	Element* elem_newline = new Element( lineNumber(), Token::NEWLINE, Kind::UNDEFINED, "\n" );
	elements.push_back(elem_newline);
	m_currentElement = elem_newline;
}

void Element::addElementToTop(Element* set)
{
	set->parent(this);
	
	if( elements.empty() == true )
	{
		elements.push_back(set);
		return;
	}
	
	vector<Element*>::iterator my_it;

	for( my_it = elements.begin(); my_it < elements.end(); my_it++ )
	{
		if( (*my_it) && (*my_it)->token() == Token::SCOPE_BEGIN )
		{
			//do nothing
		}
		else
		{
			//we found it! the element after the first scope_begin..
			break; //Umm this looks like it shouldn't work. This will get called all the time.
		}
	}

	if( my_it < elements.end() )
		elements.insert(my_it, set);
}

void Element::addAutoInitElementToFunc(Element* set)
{
	bool to_debug = false;
	if( token() == Token::CLASS )
	{
		cout<<"CLASS addAutoInitElementToFunc: "<<toString()<<"\n";
		to_debug = true;
		cout<<"we are adding: "<<set->toString();
	}

	Element* elem_newline = new Element( set->lineNumber(), Token::NEWLINE, Kind::UNDEFINED, "\n" );

	for( uint i = 0; i < elements.size(); ++i )
	{
		if( elements[i]->token() == Token::NEWLINE )
		{
			if( i+1 < elements.size() )
			{
				if( elements[i+1]->token() == Token::SCOPE_BEGIN )
				{
					elements.insert( elements.begin() + i, set);
					elements.insert( elements.begin() + i, elem_newline);
					return;
				}
			}
		}
	}

	/*
	vector<Element*>::iterator my_it;

	for( my_it = elements.begin(); my_it < elements.end(); my_it++ )
	{
		#ifdef DEBUG_RAE2
			cout<<"addAutoInitElementToFunc elem: "<<(*my_it)->toString();
			//rae::log("elem: ",(*my_it)->toString());
		#endif

		if( (*my_it) && (*my_it)->token() == Token::NEWLINE_BEFORE_SCOPE_BEGIN )
		{
			if(to_debug)
				cout<<"CLASS found newline on class.\n";
			
			#ifdef DEBUG_RAE2
			cout<<"found newline.\n";
			//rae::log("found SCOPE_BEGIN.\n");
			#endif

			(*my_it)->token(Token::NEWLINE); // it is not before a scope anymore.
			Element* elem_newline = new Element( set->lineNumber(), Token::NEWLINE_BEFORE_SCOPE_BEGIN, Kind::UNDEFINED, "\n" );

			//if( my_it < elements.end() )
			{
				#ifdef DEBUG_RAE2
				cout<<"We found it! iterator and stuff worked. ITERATOR!!!\n";
				//rae::log("We found it! iterator and stuff worked. ITERATOR!!!\n");
				#endif
				elements.insert(my_it, set);
				//elements.insert(my_it, elem_newline );
			}

			//I believe our iterator is not valid anymore...


			//vector<Element*>::const_iterator my_it2;

			//Then we'll search again for our set, because the iterators were messed up by our last insert.
			for( my_it2 = elements.begin(); my_it < elements.end(); my_it2++ )
			{
				#ifdef DEBUG_RAE2
				cout<<"elem2: "<<(*my_it2)->toString();
				//rae::log("elem2: ",(*my_it)->toString());
				#endif

				if( (*my_it2) && (*my_it2) == set )
				{
					break;
				}
			}

			//And insert our newline.
			elements.insert(my_it2, elem_newline );
			
			break;//We break here.
		}
		else
		{
			if(to_debug)
				cout<<"CLASS not a scope_begin: "<<(*my_it)->toString()<<".\n";
		}
	}
	*/
}

// Use this with a func element:
// void addElementToTopAfterScopeWithNewline(Element* set)
void Element::addElementToTopOfFunc(Element* set)
{
	vector<Element*>::iterator my_it;

	/*if( isFunc() == false )
	{
		cout<<"addElementToTopAfterScopeWithNewline used without a func.\n";
	}*/

	bool to_debug = false;
	if( token() == Token::CLASS )
	{
		cout<<"CLASS addElementToTopOfFunc: "<<toString()<<"\n";
		to_debug = true;
		cout<<"we are adding: "<<set->toString();
	}

	for( my_it = elements.begin(); my_it < elements.end(); my_it++ )
	{
		#ifdef DEBUG_RAE2
			cout<<"addElementToTopOfFunc elem: "<<(*my_it)->toString();
			//rae::log("elem: ",(*my_it)->toString());
		#endif

		if( (*my_it) && (*my_it)->token() == Token::SCOPE_BEGIN )
		{
			if(to_debug)
				cout<<"CLASS found Scope begin on class.\n";
			
			#ifdef DEBUG_RAE2
			cout<<"found SCOPE_BEGIN.\n";
			//rae::log("found SCOPE_BEGIN.\n");
			#endif

			(*my_it)->addElementToTopAfterNewlineWithNewline(set);

			if(to_debug) cout<<"should've called addElementToTopAfterNewlineWithNewline.\n";
			
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
void Element::addElementToTopAfterNewlineWithNewline(Element* set)
{
	set->parent(this);

	#ifdef DEBUG_RAE2
		cout<<"addElementToTopAfterNewlineWithNewline() START. "<<toString()<<"\n";
		cout<<"addElementToTopAfterNewlineWithNewline elements size: "<<elements.size()<<"\n";
	#endif

	Element* elem_newline = new Element( set->lineNumber(), Token::NEWLINE, Kind::UNDEFINED, "\n" );

	if( elements.empty() == true )
	{
		#ifdef DEBUG_RAE2
			cout<<"it was ALL EMPTY. so we just put it in there.\n";
		#endif
		elements.push_back(elem_newline);
		elements.push_back(set);
		elements.push_back(elem_newline);
		return;
	}

	vector<Element*>::iterator my_it;

	bool found_newline_after_scope = false;

	for( my_it = elements.begin(); my_it < elements.end(); my_it++ )
	{
		#ifdef DEBUG_RAE2
			cout<<"elem: "<<(*my_it)->toString();
		#endif

		if( (*my_it) && ((*my_it)->token() == Token::NEWLINE || (*my_it)->token() == Token::NEWLINE_BEFORE_SCOPE_END) )
		{	
			found_newline_after_scope = true;
			#ifdef DEBUG_RAE2
			cout<<"found NEWLINE after scope_begin.\n";
			#endif
			break;//We break here so we get the correct iterator position where to insert.
		}
	}

	//if( my_it < elements.end() )
	{
		#ifdef DEBUG_RAE2
		cout<<"We found it! iterator and stuff worked. ITERATOR!!!\n";
		#endif
		elements.insert(my_it, set);
		//elements.insert(my_it, elem_newline );
	}

	//I believe our iterator is not valid anymore...

	//vector<Element*>::const_iterator my_it2;

	//Then we'll search again for our set, because the iterators were messed up by our last insert.
	for( my_it = elements.begin(); my_it < elements.end(); my_it++ )
	{
		#ifdef DEBUG_RAE2
			cout<<"elem2: "<<(*my_it)->toString();
		#endif

		if( (*my_it) && (*my_it) == set )
		{
			break;
		}
	}

	//And insert our newline.
	elements.insert(my_it, elem_newline );

	#ifdef DEBUG_RAE2
		cout<<"addElementToTopAfterNewlineWithNewline elements size end: "<<elements.size()<<"\n";
	#endif
}

void Element::addElementToTopOfClass(Element* set)
{
	set->parent(this);

	#ifdef DEBUG_RAE2
		cout << "addElementToTopOfClass() START. " << toString() << "\n";
		cout << "addElementToTopOfClass elements size: "<< elements.size()<<"\n";
	#endif

	Element* elem_newline = new Element( set->lineNumber(), Token::NEWLINE, Kind::UNDEFINED, "\n" );

	if( elements.empty() == true )
	{
		#ifdef DEBUG_RAE2
			cout<<"it was ALL EMPTY. so we just put it in there.\n";
		#endif
		elements.push_back(elem_newline);
		elements.push_back(set);
		elements.push_back(elem_newline);
		return;
	}
	
	vector<Element*>::iterator my_it;

	bool found_scope_begin = false;
	bool found_newline_after_scope = false;

	for( my_it = elements.begin(); my_it < elements.end(); my_it++ )
	{
		#ifdef DEBUG_RAE2
			cout<<"elem: "<<(*my_it)->toString();
		#endif

		if( found_scope_begin == false )
		{
			if( (*my_it) && (*my_it)->token() == Token::SCOPE_BEGIN )
			{
				#ifdef DEBUG_RAE2
					cout<<"found SCOPE_BEGIN.\n";
				#endif
				found_scope_begin = true;
				//break;
			}
			else
			{	
				//else
				#ifdef DEBUG_RAE2
					cout<<"not a SCOPE_BEGIN.\n";
				#endif
			}
		}
		else
		{
			if( (*my_it) && ((*my_it)->token() == Token::NEWLINE || (*my_it)->token() == Token::NEWLINE_BEFORE_SCOPE_END) )
			{	
				found_newline_after_scope = true;
				#ifdef DEBUG_RAE2
					cout<<"found NEWLINE after scope_begin.\n";
				#endif
				break;
			}	
		}
	}

	//if( my_it < elements.end() )
	{
		#ifdef DEBUG_RAE2
			cout << "We found it! iterator and stuff worked. ITERATOR!!!\n";
		#endif
		elements.insert(my_it, set);
		//elements.insert(my_it, elem_newline );
	}

	//I believe our iterator is not valid anymore...


	//vector<Element*>::const_iterator my_it2;

	//Then we'll search again for our set, because the iterators were messed up by our last insert.
	for( my_it = elements.begin(); my_it < elements.end(); my_it++ )
	{
		#ifdef DEBUG_RAE2
			cout<<"elem2: "<<(*my_it)->toString();
		#endif

		if( (*my_it) && (*my_it) == set )
		{
			break;
		}
	}

	//And insert our newline.
	elements.insert(my_it, elem_newline );

	#ifdef DEBUG_RAE2
		cout << "addElementToTopOfClass elements size end: " << elements.size() << "\n";
	#endif
}

void Element::addNameToCurrentElement( string set_name )
{
	if(m_currentElement)
	{
		/////////cout<<"Element.addName: "<<Token::toString(m_currentElement->token())<<" name:>"<<set_name<<"\n");
		m_currentElement->name( set_name );
	}
	else cout<<"RAE_ERROR: Element::addNameToCurrentElement() : No m_currentElement.\n";
}

void Element::addTypeToCurrentElement( string set_type )
{
	if(m_currentElement)
	{
		//////////cout<<"Element.addType: "<<Token::toString(m_currentElement->token())<<" type:>"<<set_type<<"\n");
		m_currentElement->type( set_type );
	}
	else cout<<"RAE_ERROR: Element::addTypeToCurrentElement() : No m_currentElement.\n";
}

void Element::copyChildElementsFrom( Element& from )
{
	/////////cout<<"from: "<<from.name()<<"<\n");
	for( Element* elem : from.elements )
	{
		////////////cout<<"Copy elem: \n"<<elem->name();
		//Hmm. We copy just the pointers... Ouch.
		addElement(elem);
	}
}

// TODO clean up hasFunctionOrMember and searchName.
// They are the same currently. Are they the same also conceptually,
// if so then merge as hasFunctionOrMember which is more descriptive.
Element* Element::hasFunctionOrMember(string set_name)
{
	for( Element* elem : elements )
	{
		if( elem->name() == set_name )
		{
			return elem;
		}
	}
	// nothing found:
	return nullptr;
}

Element* Element::searchName(string set_name)
{
	for( Element* elem : elements )
	{
		if( elem->name() == set_name )
		{
			return elem;
		}
	}
	// nothing found:
	return nullptr;
}	

Element* Element::searchFirst(Token::e set_lang_token_type)
{
	for( Element* elem : elements )
	{
		if( elem->token() == set_lang_token_type )
		{
			return elem;
		}
	}
	// nothing found:
	return nullptr;
}

Element* Element::searchFirst(Token::e set_lang_token_type, Token::e set_lang_token_type2)
{
	for( Element* elem : elements )
	{
		if( elem->token() == set_lang_token_type )
		{
			return elem;
		}
		else if( elem->token() == set_lang_token_type2 )
		{
			return elem;
		}
	}
	// nothing found:
	return nullptr;
}

Element* Element::searchLast(Token::e set_lang_token_type)
{
	//for( Element* elem : elements )
	for( int i = (int)elements.size()-1; i >= 0; i-- )
	{
		if( elements[i]->token() == set_lang_token_type )
		{
			return elements[i];
		}
	}
	// nothing found:
	return nullptr;
}

} // end namespace Rae

