
#include "LangElement.hpp"

namespace Rae
{

namespace TypeType
{
string toString(TypeType::e set)
{
	switch(set)
	{
		default:
			return "RAE_ERROR TypeType:: not recognized.";
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

TypeType::e fromString(const string& set)
{
	if( set == "val" )
		return TypeType::VAL;
	else if( set == "ref" )
		return TypeType::REF;
	else if( set == "opt" )
		return TypeType::OPT;
	else if( set == "link" )
		return TypeType::LINK;
	else if( set == "ptr" )
		return TypeType::PTR;
	else if( BuiltInType::isBuiltInType(set) )
		return TypeType::BUILT_IN_TYPE;
	//else
	return TypeType::UNDEFINED;
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
			//REMOVED: case Token::DEFINE_FUNC_ARGUMENT:
			//REMOVED: return "Token::DEFINE_FUNC_ARGUMENT";
			//REMOVED: case Token::FUNC_ARGUMENT_TYPE:
			//REMOVED: return "Token::FUNC_ARGUMENT_TYPE";
			//REMOVED: case Token::FUNC_ARGUMENT_NAME:
			//REMOVED: return "Token::FUNC_ARGUMENT_NAME";
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
/*
	Token::e matchBuiltIntTypesToInClass(Token::e lang_token_type)
	{
		switch(lang_token_type)
		{
			default:
			return Token::RAE_ERROR;
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

}
