#include "SourceParser.hpp"

#include "Compiler.hpp"

namespace Rae
{

Element* SourceParser::newDefineVector(string set_template_second_type, string set_name)
{
	#ifdef DEBUG_RAE_HUMAN
	cout<<"newDefineVector() : set_template_second_type: "<<set_template_second_type<<" set_name: "<<set_name<<" line: "<<lineNumber.line<<"\n";
	#endif

	//Element* lang_elem = newElement(Token::DEFINE_VECTOR, set_name, set_type);
	Element* lang_elem = newElement(Token::DEFINE_REFERENCE, Kind::VECTOR, set_name, "vector");
	//lang_elem->kind(Kind::VECTOR);
	currentTemplate = lang_elem;//Oh, remove this.
	currentReference = lang_elem;

	//Hmm. This is a bit silly and slow, but we'll try to find the vector from our stdlib modules:
	//Element* stdlib_vector_element = *searchElementInOtherModulesSignal(this, lang_elem);
	Element* stdlib_vector_element = g_compiler->searchElementInOtherModules(this, lang_elem);
	if(stdlib_vector_element)
	{
		#ifdef DEBUG_RAE_HUMAN
		cout<<"Hurray! Found stdlib_vector_element.\n";
		#endif
		lang_elem->definitionElement(stdlib_vector_element);
	}
	#ifdef DEBUG_RAE_HUMAN
	else
	{
		cout<<"Buhuuh. No stdlib_vector_element found.\n";
	}
	#endif

	if( lang_elem->parentToken() == Token::CLASS )
	{
		//we are inside a class definition, not a func:
		///////lang_elem->token(Token::DEFINE_VECTOR_IN_CLASS);

		listOfAutoInitObjects.push_back(lang_elem);
	}

	addToUserDefinedTokens(lang_elem);

	#ifdef DEBUG_RAE_HUMAN
		cout<<"newDefineVector: "<<lang_elem->toString()<<"\n";
		//rae::log("newDefineVector: ",set_name,"\n");
	#endif

	return lang_elem;
}

Element* SourceParser::searchElementAndCheckIfValid(Element* set_elem)
{
	Element* result = searchElementAndCheckIfValidLocal(set_elem);

	if (result)
		return result;

	//Then search in other modules outside this SourceParser.
	//result = *searchElementInOtherModulesSignal(this, set_elem);
	result = g_compiler->searchElementInOtherModules(this, set_elem);

	return result;
}

// Token handling that is common with Rae and C++
bool SourceParser::handleTokenRaeCppCommon(string set_token)
{
	//cout << "handleTokenRaeCppCommon();\n";

	if (set_token == "")//ignore empty tokens...
		return true;
	else if (set_token == "\t")
		return true;
	else if (set_token == " ")
		return true;
    else if (set_token == "\"") // Ignore quotes as they are handled later.
		return false; // But return false, as we want it handled later.
	/*else if( set_token == "\n" )
		return;*/
	// Handle comments:
	else if (set_token == "*/")
	{
		if (isStarCommentReady == true)
		{
			newStarComment(currentStarComment);
			isStarCommentReady = false;
			return true;
		}
	}
	else if (set_token == "//")
	{
		//expectingToken = Token::COMMENT_LINE_END;
		isSingleLineComment = true;
		currentLine = "";
		//cout<<"Waiting for comment line to end.\n";
		return true;
	}
	
	// Different expectingTokens:

	if (expectingToken() == Token::DECIMAL_NUMBER_AFTER_DOT)
	{
		if (previousElement() && previousToken() == Token::FLOAT_NUMBER)//the previousElement() check is needed because we use it next.
		{
			if (isNumericChar(set_token[0]))
			{
				previousElement()->name(previousElement()->name() + set_token);
				//expectingToken = Token::UNDEFINED;
				//doReturnToExpectToken();
			}
			else
			{
				//cout<<"RAE_ERROR: "<<lineNumber.line<<" float number messed up, after dot.";
				ReportError::reportError(" float number messed up, after dot.", previousElement());
			}

			// This is Rae specific, but let's leave it here anyway:
			if (isReceivingInitData == true)
			{
				expectingToken(Token::ACTUAL_INIT_DATA);
			}
			else
			{
				doReturnToExpectToken();
			}
		}
		return true;
	}
	else if (expectingToken() == Token::EXPECTING_NAME)
	{
		if (m_expectingRole == Role::FUNC_RETURN)
		{
			if (set_token == ")")
			{
				#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_EXPECTING_NAME)
					cout << "No name for FUNC_RETURN. Got ). Re-handling it.\n";
				#endif
				doReturnToExpectToken();
				return handleToken(set_token);
			}
		}

		#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_EXPECTING_NAME)
			cout << "EXPECTING_NAME got name: " << set_token << "\n";
		#endif
		setNameForExpectingName(set_token);
		return true;
	}
	else if (expectingToken() == Token::EXPECTING_TYPE)
	{
		#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_EXPECTING_TYPE)
			cout << "EXPECTING_TYPE got type: " << set_token << "\n";
		#endif
		setTypeForExpectingType(set_token);
		return true;
	}
	else if (expectingToken() == Token::CLASS_NAME)
	{
		/*if( set_token == "" )
		{
		}
		else
		if( set_token == "\t" )
		{
		}
		else if( set_token == " " )
		{
		}
		else if( set_token == "\n" )
		{
		}
		else
		{*/
		checkForPreviousDefinitions(set_token);
		newClass(set_token);
		//expectingToken = Token::UNDEFINED;
		doReturnToExpectToken();
		//}
		return true;
	}
	else if (expectingToken() == Token::UNDEFINED)
	{
		// Rae and C++ reserved words:

		if (set_token == "\n")
		{
			//cout << "GOT newline\n";
			newLine();
			return true;
		}
		else if (set_token == ";")
		{
			Element* lang_elem = newElement(Token::SEMICOLON, Kind::UNDEFINED, set_token);
			if( lang_elem->previousToken() == Token::PARENTHESIS_END_FUNC_PARAM_TYPES
				||  lang_elem->isFunc() )
			{
				// End func. Header func definition.
				#ifdef DEBUG_RAE_PARSER
					rlutil::setColor(rlutil::RED);
						cout<<"TODO. end of function. with a semicolon. This code is now broken. We should separate between header definitions and USE_FUNCs. TODO.\n\n";
					rlutil::setColor(rlutil::WHITE);
				#endif
				currentFunc = nullptr;
				scopeElementStackPop();
			}
			return true;
		}
		else if (set_token == "{")
		{
			// This Role might be still on, if there we're no parameters to the func.
			if(expectingRole() == Role::FUNC_PARAMETER)
				expectingRole(Role::UNDEFINED);
			newScopeBegin();
			return true;
		}
		else if (set_token == "}")
		{
			newScopeEnd();
			return true;
		}
		else if (set_token == ".")
		{
			if (isWaitingForNamespaceDot)
			{
				// Ignore.
				//cout << "Got namespace dot.\n";
				//isWaitingForNamespaceDot = false;
				//newElement(Token::REFERENCE_DOT, Kind::UNDEFINED, "temp_got_namespace_dot");	
				if(set_token == ".")
				{
					#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_NAMESPACE)
						cout << "Got namespace dot. isWaitingForNamespaceDot set to false now.\n";
					#endif
					isWaitingForNamespaceDot = false;
				}
				else if(set_token == ":")
				{
					ReportError::reportError("Don't use :: for namespace separation. Use dot .", previousElement());
					assert(0);
				}
				else
				{
					ReportError::reportError("Waiting for namespace dot, but got " + set_token, previousElement());
					assert(0);
				}
				return true;
			}
			else if (previousElement() && previousToken() == Token::NUMBER)
			{
				//newElement(Token::DOT, set_token);
				previousElement()->token(Token::FLOAT_NUMBER);
				previousElement()->name(previousElement()->name() + set_token);
				expectingToken(Token::DECIMAL_NUMBER_AFTER_DOT);
			}
			else
			{
				newElement(Token::REFERENCE_DOT, Kind::UNDEFINED, set_token);
			}
			return true;
		}
		//operators:
		else if (set_token == "=")
		{
			currentReference = nullptr; // this is important to null again.

			if (previousElement() && previousElement()->name() == "=")
			{
				previousElement()->token(Token::EQUALS);
				previousElement()->name("==");
			}
			else if (previousElement() && previousElement()->name() == "!")
			{
				previousElement()->token(Token::NOT_EQUAL);
				previousElement()->name("!=");
			}
			else if (previousElement() && previousElement()->name() == ">")
			{
				previousElement()->token(Token::GREATER_THAN_OR_EQUAL);
				previousElement()->name(">=");
			}
			else if (previousElement() && previousElement()->name() == "<")
			{
				previousElement()->token(Token::LESS_THAN_OR_EQUAL);
				previousElement()->name("<=");
			}
			else if (previousElement() && previousElement()->name() == "+")
			{
				previousElement()->token(Token::PLUS_ASSIGN);
				previousElement()->name("+=");
			}
			else if (previousElement() && previousElement()->name() == "-")
			{
				previousElement()->token(Token::MINUS_ASSIGN);
				previousElement()->name("-=");
			}
			else if (previousElement() && previousElement()->name() == "*")
			{
				previousElement()->token(Token::MULTIPLY_ASSIGN);
				previousElement()->name("*=");
			}
			else if (previousElement() && previousElement()->name() == "/")
			{
				previousElement()->token(Token::DIVIDE_ASSIGN);
				previousElement()->name("/=");
			}
			else if (previousElement() && previousElement()->name() == "%")
			{
				previousElement()->token(Token::MODULO_ASSIGN);
				previousElement()->name("%=");
			}
			else
			{
				newElement(Token::ASSIGNMENT, Kind::UNDEFINED, set_token);
			}

			return true;
		}
		else if (set_token == "not" || set_token == "!")
		{
			newElement(Token::NOT, Kind::UNDEFINED, set_token);
			return true;
		}
		else if (set_token == "and" || set_token == "&&")
		{
			newElement(Token::AND, Kind::UNDEFINED, set_token);
			return true;
		}
		else if (set_token == "or" || set_token == "||")
		{
			newElement(Token::OR, Kind::UNDEFINED, set_token);
			return true;
		}
		else if (set_token == "+")
		{
			if (previousElement() && previousElement()->name() == "+")
				previousElement()->token(Token::OPERATOR_INCREMENT);
			else
				newElement(Token::PLUS, Kind::UNDEFINED, set_token);

			return true;
		}
		else if (set_token == "-")
		{
			#if defined(DEBUG_DEBUGMODULENAME)
				if (moduleName() == g_debugModuleName)
					cout << "Yey. Creating new MINUS element.\n";
			#endif

			if (previousElement() && previousElement()->name() == "-")
				previousElement()->token(Token::OPERATOR_DECREMENT);
			else
				newElement(Token::MINUS, Kind::UNDEFINED, set_token);

			return true;
		}
		else if (set_token == "*")
		{
			newElement(Token::MULTIPLY, Kind::UNDEFINED, set_token);
			return true;
		}
		else if (set_token == "/")
		{
			newElement(Token::DIVIDE, Kind::UNDEFINED, set_token);
			return true;
		}
		else if (set_token == "%")
		{
			newElement(Token::MODULO, Kind::UNDEFINED, set_token);
			return true;
		}
		else if (set_token == "<")
		{
			newElement(Token::LESS_THAN, Kind::UNDEFINED, set_token);
			return true;
		}
		else if (set_token == ">")
		{
			newElement(Token::GREATER_THAN, Kind::UNDEFINED, set_token);
			return true;
		}
		else if (set_token == "\"")
		{
			//shouldn't happen.
			//newElement(Token::MULTIPLY, set_token);
		}
		else if (set_token == "*/")
		{
			//newElement(Token::DIVIDE, set_token);
		}
		else if (set_token == "/*")
		{
			//newElement(Token::DIVIDE, set_token);
		}
		else if (set_token == "if")
		{
			newElement(Token::IF, Kind::UNDEFINED, set_token);
			return true;
		}
		else if (set_token == "else")
		{
			newElement(Token::ELSE, Kind::UNDEFINED, set_token);
			return true;
		}
		else if (set_token == "true")
		{
			newElement(Token::TRUE_TRUE, Kind::UNDEFINED, set_token);
			return true;
		}
		else if (set_token == "false")
		{
			newElement(Token::FALSE_FALSE, Kind::UNDEFINED, set_token);
			return true;
		}
		else if (set_token == "return")
		{
#ifdef DEBUG_RAE
			cout << "Got return.\n";
			//rae::log("Got return.\n");
#endif
			//expectingToken = Token::PARENTHESIS_BEGIN_RETURN;
			newElement(Token::RETURN, Kind::UNDEFINED, set_token);
			return true;
		}
		else if (set_token == "new")
		{
			cout << "TODO Got new. Waiting new_class.\n";
			//rae::log("TODO Got new. Waiting new_class.\n");
			//expectingToken = Token::FUNC_DEFINITION;
			//newFunc();
			return true;
		}
		else if (set_token == "override")
		{
			newElement(Token::OVERRIDE, Kind::UNDEFINED, set_token);
			return true;
		}
		else if (set_token == "class" || set_token == "struct") // Do we really want to support struct too? Probably not!
		{
			////rae::log("found WORD class:>", set_token, "<\n");
			expectingToken(Token::CLASS_NAME);
			return true;
		}
		else if (set_token == "namespace")
		{
			//cout << "Got namespace. Waiting for name.\n";
			Element* lang_elem = newNamespace(set_token);
			expectingNameFor(lang_elem);
			return true;
		}
		else if (set_token == "sizeof")
		{
			newElement(Token::SIZEOF, Kind::UNDEFINED, set_token);
			return true;
		}
		else if (set_token == "bitor" || set_token == "|") // bit_or etc. are not really common with C++...
		{
			newElement(Token::BITWISE_OR, Kind::UNDEFINED, set_token);
			return true;
		}
		else if (set_token == "bitand" || set_token == "&")
		{
			newElement(Token::BITWISE_AND, Kind::UNDEFINED, set_token);
			return true;
		}
		else if (set_token == "xor" || set_token == "^")
		{
			newElement(Token::BITWISE_XOR, Kind::UNDEFINED, set_token);
			return true;
		}
		else if (set_token == "compl" || set_token == "~" )
		{
			newElement(Token::BITWISE_AND, Kind::UNDEFINED, set_token);
			return true;
		}
		else if (set_token == "extern")
		{
			newElement(Token::EXTERN, Kind::UNDEFINED, set_token);
			return true;
		}
	}

	// Could not handle the token.
	return false;
}

bool SourceParser::handleTokenCpp(string set_token)
{
	//cout << "C++ handleToken();\n";

    if(set_token == "\r")
        cout << "R problem.\n";
    
	if(isCppBindingsParseMode)
	{
		if(set_token == "@end")
		{
			isCppBindingsParseMode = false;
			return true;
		}

	}

	if (handleTokenRaeCppCommon(set_token) == true)
		return true;

	if (set_token == "\"")
	{
		if (isQuoteReady == true)
		{
			isQuoteReady = false;

			newQuote(currentQuote);
			return true;
		}
	}

	#if defined(DEBUG_CPP_PARSER) || defined(DEBUG_RAE_EXPECTING_TOKEN)
		cout << "expectingToken is: " << Token::toString(expectingToken()) << " set_token: " << set_token << "\n";
	#endif

	if (expectingToken() == Token::CPP_UNSIGNED)
	{
		if (   set_token == "char"
			|| set_token == "short"
			|| set_token == "int"
			|| set_token == "long"
			)
		{
			doReturnToExpectToken();
			return handleTokenCpp("u" + set_token);
		}

		// These would be errors here:
			/*
			|| set_token == "bool"
			|| set_token == "uint"
			|| set_token == "ulong"
			|| set_token == "float"
			|| set_token == "double"
			|| set_token == "real"
			|| set_token == "string"
			*/
	}
	else if (expectingToken() == Token::CPP_SIGNED)
	{
		if (set_token == "char")
		{
			doReturnToExpectToken();
			return handleTokenCpp("signed " + set_token);
		}
		else if (  set_token == "short"
				|| set_token == "int"
				|| set_token == "long"
				)
		{
			doReturnToExpectToken();
			return handleTokenCpp(set_token);
		}

		// These would be errors here:
			/*
			|| set_token == "bool"
			|| set_token == "uint"
			|| set_token == "ulong"
			|| set_token == "float"
			|| set_token == "double"
			|| set_token == "real"
			|| set_token == "string"
			*/
	}
	else if (expectingToken() == Token::EXPECTING_CPP_PREPROCESSOR)
	{
		#if defined(DEBUG_CPP_PARSER) || defined(DEBUG_CPP_PREPROCESSOR)
        	cout << "Doing EXPECTING_CPP_PREPROCESSOR: " << set_token << "\n";
        #endif
        
		if (set_token == "define")
		{
			#if defined(DEBUG_CPP_PARSER) || defined(DEBUG_CPP_PREPROCESSOR)
				cout << "Got define in the right place.\n";
			#endif
			pushExpectingToken(Token::EXPECTING_CPP_PRE_DEFINE_VALUE); // A funny hack to put this in line first, and then
			// wait for the name in between. expectingName will then doReturnToExpectToken() which will then expect this one.
			Element* lang_elem = newElement(Token::CPP_PRE_DEFINE, Kind::UNDEFINED, set_token);
			expectingNameFor(lang_elem);
			addToUserDefinedTokens(lang_elem);
		}
		else if (set_token == "if")
			cout << "#" << set_token << " is TODO.\n";
		else if (set_token == "ifdef")
			cout << "#" << set_token << " is TODO.\n";
		else if (set_token == "else")
			cout << "#" << set_token << " is TODO.\n";
		else if (set_token == "endif")
			cout << "#" << set_token << " is TODO.\n";
		else if (set_token == "\n")
		{
			#if defined(DEBUG_CPP_PARSER) || defined(DEBUG_CPP_PREPROCESSOR) || defined(DEBUG_CPP_NEWLINE)
				cout << "It's a newline. Returning.\n";
			#endif
			newLine();
			doReturnToExpectToken();
		}
		return true;
	}
	else if (expectingToken() == Token::EXPECTING_CPP_PRE_DEFINE_VALUE)
	{
		if (previousElement()->token() == Token::CPP_PRE_DEFINE)
		{
			previousElement()->type(set_token);
			doReturnToExpectToken();
		}
		else
		{
			ReportError::reportError("#define error", previousElement());
		}
		return true;
	}
	else if (expectingToken() == Token::EXPECTING_CPP_TYPEDEF_TYPENAME)
	{
		// e.g. GLfloat is saved to the type() and
		// float is put in builtInType for typedefs
		//previousElement()->type(set_token);
		//NOPE: not anymore. Now in name.
		previousElement()->name(set_token);
		doReturnToExpectToken();
		return true;
	}
	/*
	else if (expectingToken() == Token::FUNC_DEFINITION)
	{
		if (set_token[0] == '\n')
		{
#ifdef DEBUG_RAE_HUMAN
			cout << "Got NEWLINE. Ignore it because func.\n";
			//rae::log("Got NEWLINE. Ignore it because func.\n");
#endif
			//expectingToken = Token::FUNC_RETURN_TYPE;
		}
		else if (set_token[0] == '(')
		{
#ifdef DEBUG_RAE
			//rae::log("Got func_def (. Waiting return_types.\n");
#endif
			newParenthesisBegin(Token::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES, "(");
			expectingRole(Role::FUNC_PARAMETER);
			expectingToken(Token::UNDEFINED);
		}
	}
	*/
	else if (expectingToken() == Token::DEFINE_BUILT_IN_TYPE_NAME)
	{
		if (unfinishedElement())
		{
			if (isNumericChar(set_token[0]))
			{
				if (unfinishedElement()->containerType() != ContainerType::STATIC_ARRAY)
				{
					ReportError::reportError("Waiting for BUILT_IN_TYPE_NAME. Got a number, but this is not a static array.", unfinishedElement());
				}
				else
				{
					unfinishedElement()->staticArraySize(stringToNumber<int>(set_token));
				}
			}
			else
			{
				setNameAndCheckForPreviousDefinitions(unfinishedElement(), set_token);

				//if (currentClass != nullptr || currentFunc != nullptr || currentEnum != nullptr)//INSIDE_CLASS doesn't work currently: if(expectingRole() == Role::INSIDE_CLASS)
				//{
					//FIRST of this text!! line 547. Maybe this is the C++ version??
					if (unfinishedElement()->containerType() == ContainerType::UNDEFINED)
					{
						//in the normal function body case (which is still Role::UNDEFINED
						//because we haven't yet needed e.g. Role::FUNCTION_BODY for anything)
						//we wait for initdata if there is some:
						expectingToken(Token::INIT_DATA);
					}
					else
					{
						expectingToken(Token::UNDEFINED);
						unfinishedElement(nullptr);
					}
				//}
				//else
				//{
					/*
#ifdef DEBUG_RAE_PARSER
					if (currentFunc == nullptr)
						cout << "currentFunc is null.\n";
					else ReportError::reportInfo("currentFunc: ", currentFunc);
					if (currentEnum == nullptr)
						cout << "currentEnum is null.\n";
					if (currentClass == nullptr)
						cout << "currentClass is null.\n";

					ReportError::reportInfo("No initdata in not inside class: ", unfinishedElement());
#endif
					expectingToken(Token::UNDEFINED);
					unfinishedElement(nullptr);
				}*/
			}
		}
		else
		{
			ReportError::reportError("Waiting for BUILT_IN_TYPE_NAME. No unfinishedElement.", previousElement());
		}
		return true;
	}
    else if (expectingToken() == Token::DEFINE_REFERENCE_NAME)
	{
		if (set_token == "*")
		{
			//TODO pointer type...
			//ReportError::reportError("TODO C++ pointer type.", previousElement());
			//newElement(Token::MULTIPLY, Kind::UNDEFINED, set_token);
            if (currentReference)
            {
                currentReference->kind(Kind::PTR);
            }
            
			//assert(0);
            return true;
		}
		else if (set_token == "<")
		{
			ReportError::reportError("TODO C++ templates", previousElement());
            return true;
        }
		else if (currentReference)
		{
			#if defined(DEBUG_CPP_PARSER) || defined(DEBUG_CPP_REFERENCES)
            	cout << "Setting the name for currentRef: " << currentReference->toSingleLineString() << " as " << set_token << "\n";
            #endif

			//currentReference->name(set_token);
			setNameAndCheckForPreviousDefinitions(currentReference, set_token);
            
			if (expectingRole() == Role::FUNC_RETURN)
			{
				//Back to waiting for UNDEFINED as that handles the coming ")" the best.
				expectingToken(Token::UNDEFINED);
			}
			else if (currentClass != nullptr && currentFunc == nullptr && currentEnum == nullptr)//INSIDE_CLASS doesn't work currently: if(expectingRole() == Role::INSIDE_CLASS)
			{
				expectingToken(Token::INIT_DATA);
				//expectingToken = Token::UNDEFINED;
				//doReturnToExpectToken();
			}
			else
			{
				expectingToken(Token::UNDEFINED);
			}
            
            return true;
		}
	}
	else if (expectingToken() == Token::INIT_DATA)
	{
		#if defined(DEBUG_CPP_PARSER) || defined(DEBUG_CPP_INIT_DATA)
			cout << "INIT_DATA for C++ is TODO.\n";
		#endif
		doReturnToExpectToken();

		return handleToken(set_token);
	}
	else if (expectingToken() == Token::UNDEFINED)
	{
		if(currentClass && currentClass->name() == set_token)
		{
			#if defined(DEBUG_CPP_PARSER)
				cout << "Got the CLASS_NAME as set_token. This is probably a C++ CONSTRUCTOR.\n";
			#endif
			Element* our_ref = newFunc();
			our_ref->token(Token::CONSTRUCTOR);
			expectingRole(Role::FUNC_PARAMETER);
			return true;
		}
		// C++ reserved words:
		else if (set_token == ",")
		{
			newElement(Token::COMMA, Kind::UNDEFINED, set_token);
			currentReference = nullptr;
			return true;
		}
		else if (set_token == ":")
		{
			cout << "C++ got : we'll need to handle this later.\n";
			assert(0);
			return true;
		}
		/*JONDE DOEsn't work because it's already handled in common.
		else if (set_token == ";") // This is a stupid hack ATM. only support headers in C++ side for now.
		{
			cout << "C++ semicolon handling.\n";
			newElement(Token::SEMICOLON, Kind::UNDEFINED, set_token);
			currentReference = nullptr;
			Element* scope_elem = scopeElementStack.back();
			if( scope_elem->isFunc() )
				currentFunc = nullptr;
			scopeElementStackPop();

			return true;
		}
		*/
		else if( set_token == "void" )
		{
			if (previousToken() == Token::CPP_TYPEDEF)
			{
				previousElement()->builtInType(BuiltInType::cppStringToBuiltInType(set_token));
				// Typedefs name will also hold a copy of the built in type, because that's
				// what will happen to non-built in typedefs too. The new type will be in type() and the
				// old type in name() (and/or builtInType())
				//NOPE: just reversed it. newType in name and old type in type.
				//previousElement()->name(BuiltInType::toString(BuiltInType::cppStringToBuiltInType(set_token)));
				previousElement()->type(BuiltInType::toString(BuiltInType::cppStringToBuiltInType(set_token)));
				expectingToken(Token::EXPECTING_CPP_TYPEDEF_TYPENAME);
				return true;
			}
			//else

			Element* our_ref = newFunc();
			//pushExpectingToken(Token::FUNC_DEFINITION);
			expectingRole(Role::FUNC_PARAMETER);
			expectingNameFor(our_ref);
			//unfinishedElement(our_ref);
			return true;
		}
		else if (set_token == "unsigned")
		{
			// Note! Doesn't support using unsigned as a synonym for unsigned int, without the int specified. TODO
			expectingToken(Token::CPP_UNSIGNED);
			return true;
		}
		else if (set_token == "signed")
		{
			expectingToken(Token::CPP_SIGNED);
			return true;
		}
		else if (set_token == "long")
		{
			cout << "TODO C++ unsigned and long long.\n";
			ReportError::reportError("TODO C++ unsigned and long long", previousElement() );
			assert(0);
			return true;
		}
		//C++
		else if (set_token == "bool"
			|| set_token == "char"
			|| set_token == "uchar"
			|| set_token == "signed char"
			|| set_token == "short"
			|| set_token == "ushort"
			|| set_token == "int"
			|| set_token == "uint"
			|| set_token == "ulong"
			|| set_token == "float"
			|| set_token == "double"
			//|| set_token == "real"
			|| set_token == "string"
			|| set_token == "int64_t"
			|| set_token == "uint64_t"
			|| set_token == "int32_t"
			|| set_token == "uint32_t"
			|| set_token == "int16_t"
			|| set_token == "uint16_t"
			|| set_token == "int8_t"
			|| set_token == "uint8_t"
			//|| set_token == "int128_t"
			//|| set_token == "uint128_t"
			)
		{
			// Note !! We don't yet support parsing "long long" for C++, TODO it could be parsed a bit similar to unsigned and signed...

			//JONDE REMOVE cout << "Got " << set_token << "\n";

			bool is_let = false; // in C++ it's const, but we'll call it let here.

			if (previousToken() == Token::LET)
			{
				is_let = true;
				previousElement()->token(Token::EMPTY);
			}
			else if (previousToken() == Token::CPP_TYPEDEF)
			{
				previousElement()->builtInType(BuiltInType::cppStringToBuiltInType(set_token));
				// Typedefs name will also hold a copy of the built in type, because that's
				// what will happen to non-built in typedefs too. The new type will be in type() and the
				// old type in name() (and/or builtInType())
				//NOPE just reversed it. new type in name, and old type in type.
				//previousElement()->name(BuiltInType::toString(BuiltInType::cppStringToBuiltInType(set_token)));
				previousElement()->type(BuiltInType::toString(BuiltInType::cppStringToBuiltInType(set_token)));
				expectingToken(Token::EXPECTING_CPP_TYPEDEF_TYPENAME);
				return true;
			}
			else if (previousToken() == Token::SIZEOF || (previousToken() == Token::PARENTHESIS_BEGIN && previous2ndToken() == Token::SIZEOF))
			{
				newDefineBuiltInType(BuiltInType::cppStringToBuiltInType(set_token), expectingRole(), set_token);
				return true;
			}

			Element* our_ref;
			/*Rae specific, so commented out:
			if (not bracketStack.empty())
			{
				our_ref = newDefineBuiltInType(BuiltInType::cppStringToBuiltInType(set_token), Role::TEMPLATE_PARAMETER, set_token);
			}
			else
			{*/
				expectingToken(Token::DEFINE_BUILT_IN_TYPE_NAME);
				our_ref = newDefineBuiltInType(BuiltInType::cppStringToBuiltInType(set_token), expectingRole(), set_token);
				unfinishedElement(our_ref);
			//}

			our_ref->isLet(is_let);

			return true;
		}
		else if (set_token == "#")
		{
			pushExpectingToken(Token::EXPECTING_CPP_PREPROCESSOR);
			return true;
		}
		else if (set_token == "(")
		{
			#if defined(DEBUG_CPP_PARSER) || defined(DEBUG_CPP_PARENTHESIS)
				cout << "Got C++ (\n";
			#endif

			if (previousToken() == Token::DEFINE_REFERENCE)
			{
				#if defined(DEBUG_CPP_PARSER) || defined(DEBUG_CPP_PARENTHESIS) || defined(DEBUG_CPP_FUNC)
					cout << "Maybe it's a C++ func returning something.\n";
				#endif
				// Maybe it's a C++ func returning something.
				currentReference = nullptr;
				previousElement()->token(Token::FUNC);
				Kind::e ret_typetype = previousElement()->kind();
				previousElement()->kind(Kind::UNDEFINED);
				previousElement()->definitionElement(nullptr);
				currentParentElement(previousElement());
				addToUserDefinedTokens(previousElement()); // This is important. Otherwise it won't be found.
				
				expectingRole(Role::FUNC_RETURN);
				string ret_type = previousElement()->type();
				#if defined(DEBUG_CPP_PARSER) || defined(DEBUG_CPP_PARENTHESIS) || defined(DEBUG_CPP_FUNC)
					cout << "The ret_type for the func is: " << ret_type << "\n";;
				#endif
				previousElement()->type("");
				newParenthesisBegin(Token::PARENTHESIS_BEGIN_FUNC_RETURN_TYPES, "(");
				//newDefineReference(ret_type, expectingRole());
				//currentReference = nullptr;
				Element* lang_elem = newElement(Token::DEFINE_REFERENCE, ret_typetype, "", ret_type);
				lang_elem->role(Role::FUNC_RETURN);
				//TODO addTouserDefinedTokens???

				newParenthesisEnd(Token::PARENTHESIS_END_FUNC_RETURN_TYPES, ")");
				
				expectingRole(Role::FUNC_PARAMETER);
			}

			//ReportError::reportError("TODO C++ (", previousElement() );
			//assert(0);
			if( expectingRole() == Role::FUNC_PARAMETER )
			{
				#if defined(DEBUG_CPP_PARSER) || defined(DEBUG_CPP_PARENTHESIS) || defined(DEBUG_CPP_FUNC)
                	cout << "And made it FUNC_PARAMETER PARENTHESIS.\n";
                #endif
				newParenthesisBegin(Token::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES, set_token);
			}
			else
			{
				#if defined(DEBUG_CPP_PARSER) || defined(DEBUG_CPP_PARENTHESIS)
                	cout << "Made it just regular PARENTHESIS_BEGIN.\n";
                #endif
				newParenthesisBegin(Token::PARENTHESIS_BEGIN, set_token);
			}

			return true;
		}
		else if (set_token == ")")
		{
			if (expectingRole() == Role::FUNC_PARAMETER)
			{
				#if defined(DEBUG_CPP_PARSER) || defined(DEBUG_CPP_PARENTHESIS)
					cout << "Got ending ) for FUNC_PARAMETERs in UNDEFINED. Going back to UNDEFINED.\n";
				#endif

				expectingRole(Role::UNDEFINED);
				newParenthesisEnd(Token::PARENTHESIS_END_FUNC_PARAM_TYPES, ")");
				expectingToken(Token::UNDEFINED);
			}
			else
			{
				newParenthesisEnd(Token::PARENTHESIS_END, set_token);
			}
            return true;
		}
		else if (set_token == "->")
		{
			// We don't separate . and -> while parsing... even with C++.
			newElement(Token::REFERENCE_DOT, Kind::UNDEFINED, set_token);
			return true;
		}
		else if (set_token == "for")
		{
			newElement(Token::FOR, Kind::UNDEFINED, set_token);
			return true;
		}
		else if (set_token == "NULL" || set_token == "nullptr")
		{
			newElement(Token::RAE_NULL, Kind::UNDEFINED, set_token);
			return true;
		}
		else if (set_token == "const")
		{
			newElement(Token::LET, Kind::UNDEFINED, set_token);
			return true;
		}
		else if (set_token == "mutable")
		{
			newElement(Token::MUT, Kind::UNDEFINED, set_token);
			return true;
		}
		else if (set_token == "typedef")
		{
			Element* lang_elem = newElement(Token::CPP_TYPEDEF, Kind::UNDEFINED, set_token);
			currentReference = lang_elem; // TODO this currentReference mechanism is stupid. We should remove it, and maybe just use previousElement() and rename it to currentElement, or something.
			addToUserDefinedTokens(lang_elem);
			return true;
		}
		else if (set_token == "public")
		{
			newElement(Token::VISIBILITY, Kind::UNDEFINED, set_token);
			return true;
		}
		else if (set_token == "protected")
		{
			newElement(Token::VISIBILITY, Kind::UNDEFINED, set_token);
			return true;
		}
		else if (set_token == "private")
		{
			newElement(Token::VISIBILITY, Kind::UNDEFINED, set_token);
			return true;
		}
		else if (set_token == ":")
		{
			if (previousElement())
			{
				if (previousElement()->token() == Token::VISIBILITY)
				{
					//change it.
					previousElement()->token(Token::VISIBILITY_DEFAULT);
					////Now we don't add, as we already added before.
					//add :
					//previousElement()->name( previousElement->name() + ":" );
				}
			}
			return true;
		}
		else if (set_token == "delete")
		{
			cout << "TODO Got C++ delete.\n";
			return true;
		}
		else if (previousToken() == Token::CPP_TYPEDEF)
		{
			// The new type will be in type() and the
			// old type in name() (and/or builtInType())
			//NOPE reversed now.
			//previousElement()->name(set_token); //JONDE TODO check typedef old type. Does it exist. Validate it does.
			previousElement()->type(set_token);
			#if defined(DEBUG_CPP_PARSER) || defined(DEBUG_CPP_TYPEDEF)
				cout << "Created a typedef for an unknown old type: " << set_token << "\n";
			#endif
			expectingToken(Token::EXPECTING_CPP_TYPEDEF_TYPENAME);
			return true;
		}
		else
		{
			// TODO Generic handle new names here...
			handleUserDefinedToken(set_token);
			//newUnknownUseReference2(set_token);
			return true;
		}
	}

	/////
    ReportError::reportError("C++ parser couldn't handle token: " + set_token, previousElement() );

	// Could not handle the token.
	return false;
}

bool SourceParser::handleToken(string set_token)
{
	//cout << "RAE handleToken();\n";

	if (set_token == "")//ignore empty tokens...
		return true;
	else if (set_token == "\t")
		return true;
	else if (set_token == " ")
		return true;
	/*else if( set_token == "\n" )
		return true;*/
	
	//We catch all tokens when doing PRAGMA_CPP until we get a website address
	//such as: forsaken_woods@endisnear.com
	//Oh. @Pragmas need to end with space or enter, so that might handle that case.
	if (expectingToken() == Token::PRAGMA_CPP)
	{
		if (set_token == "@end")
		{
			isPassthroughMode = false;
			newElement(Token::PRAGMA_CPP_END, Kind::UNDEFINED, set_token);
			doReturnToExpectToken();
		}
		else
		{
			// If we're inside a func, then it goes to cpp. Otherwise to header.
			if( currentParentElement() != nullptr && currentParentElement()->isFunc() )
			{
				newElement(Token::PASSTHROUGH_SRC, Kind::UNDEFINED, set_token);
			}
			else
			{
				newElement(Token::PASSTHROUGH_HDR, Kind::UNDEFINED, set_token);
			}
		}

		return true;
	}
	else if (expectingToken() == Token::PRAGMA_CPP_SRC)
	{
		if (set_token == "@end")
		{
			isPassthroughSourceMode = false;
			newElement(Token::PRAGMA_CPP_END, Kind::UNDEFINED, set_token);
			doReturnToExpectToken();
		}
		else
		{
			newElement(Token::PASSTHROUGH_SRC, Kind::UNDEFINED, set_token);
		}

		return true;
	}

	// Handle comments:

	// Rae plus comments
	if (set_token == "+/" || set_token == "+#")
	{
		if (isPlusCommentReady == true)
		{
			newPlusComment(currentPlusComment);
			isPlusCommentReady = false;

			return true; // CHECK if it makes any difference for this return to be in the parent scope...?
		}
	}

	if( isCppBindingsParseMode == true )
	{
		return handleTokenCpp(set_token);
	}
	else if (set_token == "*#")
	{
		if (isStarCommentReady == true)
		{
			newStarComment(currentStarComment);
			isStarCommentReady = false;
			return true;
		}
	}
	else if (set_token == "#")
	{
		isSingleLineComment = true;
		//currentLine = "#";
		//cout<<"Waiting for comment line to end.\n";
		return true;
	}

	if (handleTokenRaeCppCommon(set_token) == true )
		return true;

	// Rae quote handling. Cpp quote handling is in hanldeTokenCpp, but it's just a shorter version of this.
	if (set_token == "\"")
	{
		if (isQuoteReady == true)
		{
			isQuoteReady = false;

			if (expectingToken() == Token::INIT_DATA)
			{
				if (currentReference)
				{
//#ifdef DEBUG_RAE_HUMAN
					cout << "string init_data: " << currentQuote << " Special handling for newQuote. INIT_DATA.\n";
//#endif
					//special handling for quote i.e. string initData.
					Element* in_dat = newElement(Token::INIT_DATA, Kind::UNDEFINED, currentQuote);
					currentReference->initData(in_dat);
					doReturnToExpectToken();
					return true;
				}
			}
			else if (expectingToken() == Token::IMPORT_DIRS)
			{
				string removed_quotes = removeFromString( currentQuote, '\"' );
				removed_quotes = makeDirPathBetter(removed_quotes);
				cout << "Should add to import dirs: " << removed_quotes << "\n";
				g_compiler->addModuleSearchPath(removed_quotes);
			}
			else
			{
				newQuote(currentQuote);
				return true;
			}

			return true;
		}
        
        return true; // We handled the quote, even it's not yet ready.
	}
	
	// TODO from here on we don't properly return true...

	if (expectingToken() == Token::INIT_DATA)
	{
		#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_INIT_DATA)
			cout << "Waiting for INIT_DATA for " << previousElement()->name() << "\n";
		#endif

		if (set_token == "=" || set_token == "->") //TODO better checking for point_to only pointing to pointer types like null...
		{
			Element* prev_elem = previousElement();

			//create a init_data element and continue waiting for the actual data!
			Element* in_dat = newElement(Token::INIT_DATA, Kind::UNDEFINED, set_token);
			//currentReference->initData(in_dat);
			if (prev_elem)
			{
				prev_elem->initData(in_dat);
				#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_INIT_DATA)
					cout << "= starting to receive init_data for " << prev_elem->type() << " " << prev_elem->namespaceString() << "\n";
				#endif
			}
			else
			{
				ReportError::compilerError("There's no previous element to take hold of the coming initdata.");
			}

			isReceivingInitData = true;

			expectingToken(Token::ACTUAL_INIT_DATA);
		}
		else if (set_token == "\n" || set_token == ";")
		{
			#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_INIT_DATA)
				cout << "No init_data. Set to default init_data next please.\n";
			#endif
			//No initdata. Set to default init_data
			//currentReference->addDefaultInitData(); //OK. Don't do this. Adding auto-init data is dependant on not having this.
			if (currentParentElement() && currentParentElement()->token() == Token::INIT_DATA)
			{
				endInitData();//This is important. This ends our init_data being the currentParentElement (which receives the init_data.)
			}
			else if (currentReference)
			{
				#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_INIT_DATA)
					cout << "Next, please addDefaultInitData.\n";
				#endif
				currentReference->addDefaultInitData();
			}
			doReturnToExpectToken();
			if (set_token == "\n")
				newLine();
		}
		else if (set_token == ")")
		{
			//DUPLICATE1 of ::INIT_DATA parenthesis handling.
			if (expectingRole() == Role::FUNC_RETURN)
			{
				#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_INIT_DATA)
					cout << "Got ending ) for FUNC_RETURNs (in INIT_DATA). Going back to FUNC_DEFINITION.\n";
				#endif

				expectingRole(Role::UNDEFINED);//could also be Role::FUNC_DEFINITION, but we don't need that, so.
				//TODO: check parenthesisStack:
				newParenthesisEnd(Token::PARENTHESIS_END_FUNC_RETURN_TYPES, ")");
				expectingToken(Token::FUNC_DEFINITION);
			}
			else if (expectingRole() == Role::FUNC_PARAMETER)
			{
				#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_INIT_DATA)
					cout << "Got ending ) for FUNC_PARAMETERs in INIT_DATA. Going back to UNDEFINED.\n";
				#endif

				expectingRole(Role::UNDEFINED);//could also be Role::FUNC_DEFINITION, but we don't need that, so.
				//TODO: check parenthesisStack:
				newParenthesisEnd(Token::PARENTHESIS_END_FUNC_PARAM_TYPES, ")");
				expectingToken(Token::UNDEFINED);
			}
			else
			{
				ReportError::reportError(") parenthesis_end - can't be initdata for built in type.\n", previousElement());
			}
		}
		else if (set_token == ",")
		{
			if (expectingRole() == Role::FUNC_RETURN || expectingRole() == Role::FUNC_PARAMETER)
			{
				//No initdata.
				if (currentParentElement() && currentParentElement()->token() == Token::INIT_DATA)
				{
					endInitData();//This is important. This ends our init_data being the currentParentElement (which receives the init_data.)
				}

				newElement(Token::COMMA, Kind::UNDEFINED, ",");
				//expectingToken(Token::UNDEFINED);
				doReturnToExpectToken();
			}
			else
			{
				ReportError::reportError("unexpected , COMMA while waiting for initdata for built in type.\n", previousElement());
			}
		}
		else
		{
			//No initdata.
			expectingToken(Token::UNDEFINED);
			handleToken(set_token); // Oh, we're heading to the recursive territory here... re-handleToken.
		}
	}
	else if (expectingToken() == Token::ACTUAL_INIT_DATA)
	{
		//we should make a expectingToken() == Token::ACTUAL_INIT_DATA that we wait after we get 
		//the =. And also to handle //comments /**/ as not being init_data, or can they be initDATA? or passed with it?

		#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_INIT_DATA)
			cout << "ACTUAL_INIT_DATA handling.\n";
		#endif

		if (isWaitingForNamespaceDot)
		{
			// Ignore.
			if(set_token == ".")
			{
				cout << "Got namespace dot. isWaitingForNamespaceDot set to false now.\n";
				isWaitingForNamespaceDot = false;
			}
			else if(set_token == ":")
			{
				ReportError::reportError("Don't use :: for namespace separation. Use dot .", previousElement());
				assert(0);
			}
			else
			{
				ReportError::reportError("Waiting for namespace dot, but got " + set_token, previousElement());
				assert(0);
			}
			return true;
		}
		else if (set_token == ")")
		{
			//DUPLICATE2 of ::INIT_DATA parenthesis handling.
			if (currentParentElement() && currentParentElement()->token() == Token::INIT_DATA)
			{
				endInitData();//This is important. This ends our init_data being the currentParentElement (which receives the init_data.)
			}

			if (expectingRole() == Role::FUNC_RETURN)
			{
				#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_INIT_DATA)
					cout << "Got ending ) for FUNC_RETURNs (in INIT_DATA). Going back to FUNC_DEFINITION.\n";
				#endif

				expectingRole(Role::UNDEFINED);//could also be Role::FUNC_DEFINITION, but we don't need that, so.
				//TODO: check parenthesisStack:
				newParenthesisEnd(Token::PARENTHESIS_END_FUNC_RETURN_TYPES, ")");
				expectingToken(Token::FUNC_DEFINITION);
			}
			else if (expectingRole() == Role::FUNC_PARAMETER)
			{
				#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_INIT_DATA)
					cout << "Got ending ) for FUNC_PARAMETERs in INIT_DATA. Going back to UNDEFINED.\n";
				#endif

				expectingRole(Role::UNDEFINED);//could also be Role::FUNC_DEFINITION, but we don't need that, so.
				//TODO: check parenthesisStack:
				newParenthesisEnd(Token::PARENTHESIS_END_FUNC_PARAM_TYPES, ")");
				expectingToken(Token::UNDEFINED);
			}
			else
			{
				ReportError::reportError(") parenthesis_end - can't be initdata for built in type.\n", previousElement());
			}
		}
		else if (set_token == ".")
		{

			if (previousElement() && previousToken() == Token::NUMBER)
			{
				//newElement(Token::DOT, set_token);
				previousElement()->token(Token::FLOAT_NUMBER);
				previousElement()->name(previousElement()->name() + set_token);
				expectingToken(Token::DECIMAL_NUMBER_AFTER_DOT);
				//expectingRole(Role::INIT_DATA);

			}
			else
			{
				ReportError::reportError("A strange dot while waiting for INIT_DATA. Floating point numbers are written: 0.0", previousElement());
				newElement(Token::REFERENCE_DOT, Kind::UNDEFINED, set_token);
			}
		}
		else if (set_token == ";")
		{
			if (currentParentElement() && currentParentElement()->token() == Token::INIT_DATA)
			{
				#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_INIT_DATA)
					cout << "TODO. Check this. Got semicolon in for loop INIT_DATA.\n";
				#endif
				endInitData();//This is important. This ends our init_data being the currentParentElement (which receives the init_data.)
			}
			else
			{
				ReportError::reportError("Waiting for INIT_DATA: Got SEMICOLON but there was no INIT_DATA element.", previousElement());
				//TODO ReportError::additionalInfo("currentParentElement: ", currentParentElement() );
				#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_INIT_DATA)
					cout << "currentParentElement: ";
					if (currentParentElement()) cout << currentParentElement()->toString() << "\n"; else cout << "nullptr" << "\n";
				#endif

				if (isReceivingInitData == true)
					assert(0);
				doReturnToExpectToken();
			}

			newElement(Token::SEMICOLON, Kind::UNDEFINED, set_token);
		}
		else if (set_token == "\n")
		{
			/*if( currentReference )
			{
			currentReference->addDefaultInitData();

			//TODO check for shadowing other name definitions!!!!!!!!!!!!!!!!
			//and error if we're returnToExpectToken FUNC_ARGUMENT_NAME

			#ifdef DEBUG_RAE_HUMAN
			cout<<"newDefineBuiltInType: "<<currentReference->type()<<" "<<currentReference->namespaceString()<<"\n";
			//rae::log("newDefineBuiltInType: ", currentReference->type(), " ", currentReference->name(), "\n");
			#endif
			}*/
			//expectingToken = Token::UNDEFINED;

			if (currentParentElement() && currentParentElement()->token() == Token::INIT_DATA)
			{
				#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_INIT_DATA)
                	cout << "Going to endInitData. Here1.\n";
                #endif
				endInitData();//This is important. This ends our init_data being the currentParentElement (which receives the init_data.)
			}
			else
			{
				ReportError::reportError("Waiting for INIT_DATA: Got NEWLINE but there was no INIT_DATA element.", previousElement());
				//TODO ReportError::additionalInfo("currentParentElement: ", currentParentElement() );
				#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_INIT_DATA)
					cout << "currentParentElement: ";
					if (currentParentElement()) cout << currentParentElement()->toString() << "\n"; else cout << "nullptr" << "\n";
				#endif

				if (isReceivingInitData == true)
					assert(0);

				doReturnToExpectToken();
			}

			newLine();
		}
		else if (set_token == ",")
		{
			if (expectingRole() == Role::FUNC_RETURN || expectingRole() == Role::FUNC_PARAMETER)
			{
				//No initdata.
				if (currentParentElement() && currentParentElement()->token() == Token::INIT_DATA)
				{
					endInitData();//This is important. This ends our init_data being the currentParentElement (which receives the init_data.)
				}

				newElement(Token::COMMA, Kind::UNDEFINED, ",");
				//expectingToken(Token::UNDEFINED);
				doReturnToExpectToken();
			}
			else
			{
				ReportError::reportError("unexpected , COMMA while waiting for actual initdata for built in type.\n", previousElement());
			}
		}
		else if (set_token == "\"")
		{
			//let it pass.
		}
		else if (set_token == ":")
		{
			//We're actually inside a function call, so we want to mark this as
			//INFO_FUNC_PARAM, which will not be printed.
			if (currentReference)
			{
				//WHY does this set the token of an already existing element?????????? TODO
				currentReference->token(Token::INFO_FUNC_PARAM);
			}
			//expectingToken = Token::UNDEFINED;
			doReturnToExpectToken();
		}
		else if (set_token == "new")
		{
			#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_INIT_DATA)
				cout << "TODO new() INIT_DATA.\n";
			#endif
			//TODO error on returnToExpectToken FUNC_ARGUMENT_NAME, no new here.
			//currentReference->initData(set_token);
			//expectingToken = Token::UNDEFINED; //TEMP
			doReturnToExpectToken();
		}
		else if (set_token == "null")
		{
			#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_INIT_DATA)
				cout << "ACTUAL_INIT_DATA: Got null initdata.\n";
			#endif

			if (currentParentElement() && currentParentElement()->token() == Token::INIT_DATA)
			{
				//cout<<"And created null initdata for element: "<<dang->toString()<<"\n";
				Element* in_dat = newElement(Token::RAE_NULL, Kind::UNDEFINED, set_token);
			}
			else
			{
				ReportError::reportError("Got null as init_data, but there was no = before the init_data.", previousElement());
			}

			//#ifdef DEBUG_RAE_HUMAN
			//	cout<<"Set null as init_data for "<<dang->type()<<" "<<dang->namespaceString()<<"\n";
			//#endif
		}
		else
		{
			if (currentParentElement() && currentParentElement()->token() == Token::INIT_DATA)
			{
				if (handleNumber(set_token))
				{
					#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_INIT_DATA)
						cout << "INIT_DATA got a number.\n";
					#endif
					//don't do anything. We've done it already!
				}
				else
				{
					//we should have a handleInitData() func!
					//newElement(Token::STRING, Kind::UNDEFINED, set_token); //Hmm STRINGs like "a string" are Token::QUOTES.
					cout << "TODO better handling of init_data in initialization. " << set_token << "\n";

					//if (isReceivingInitData == true)
						//assert(0);

					expectingToken(Token::UNDEFINED);
					handleToken(set_token); // Oh, we're heading to the recursive territory here... re-handleToken.
					expectingToken(Token::ACTUAL_INIT_DATA);
				}
			}
			else
			{
				ReportError::reportError("Got null as init_data, but there was no = before the init_data.", previousElement());
			}



			/*if( handleInitData(set_token) )
			{
			//got all init data.
			expectingToken = Token::UNDEFINED;
			}*/
			//if( isNumericChar(set_token[0]) &&
			/*
			if( currentReference && previousElement()->token() != Token::QUOTE )
			{
			Element* in_dat = newElement(Token::INIT_DATA, Kind::UNDEFINED, set_token);
			currentReference->initData(in_dat);

			#ifdef DEBUG_RAE_HUMAN
			cout<<"initData: "<<set_token<<" is new initData for: "<<currentReference->type()<<" "<<currentReference->namespaceString()<<"\n";
			//rae::log("newDefineBuiltInType: ", currentReference->type(), " ", currentReference->name(), "\n");
			#endif
			}
			*/
			/*
			//This didn't work. see newQuote above for the new version.
			else if( currentReference && previousElement()->token() == Token::QUOTE )
			{
			//special handling for quote i.e. string initData.
			Element* in_dat = previousElement()->copy();
			in_dat->token(Token::INIT_DATA);
			currentReference->initData( in_dat );//copy the quote to initData.
			previousElement()->token(Token::EMPTY);//hide the quote.
			}
			*/
			/*else
			{
			//cout<<"RAE_ERROR: "<<lineNumber.line<<" was expecting an INIT_DATA. It should be a number or new.\n";
			ReportError::reportError("expecting an INIT_DATA. It should be a number or new.", previousElement() );
			}*/
			//expectingToken = Token::UNDEFINED;

			//doReturnToExpectToken();
		}
	}
	else if (expectingToken() == Token::IMPORT_DIRS) //TODO move this lower in priority, as this only happens once per project.
	{
		if (set_token == "\n")
		{
			//ignore
		}
		else if (set_token == ",")
		{
			//ignore
		}
		else if (set_token == "[")
		{
			//ignore
		}
		else if (set_token == "]")
		{
			doReturnToExpectToken();
		}
		else if (set_token == "=")
		{
			//ignore
		}
		else if (set_token == "\"")
		{
			cout << "Got quotation mark:<" << set_token << ">\n";
		}
		else
		{
			cout << "Should add import path:<" << set_token << ">\n";
		}
	}
	else if (expectingToken() == Token::IMPORT_NAME)
	{
		if (set_token[0] == '.')
		{
			#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_IMPORT)
				cout << "Got IMPORT_NAME dot.\n";
			#endif
			expectingToken(Token::IMPORT_NAME);
		}
		else if (set_token == "\n")
		{
			#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_IMPORT) || defined(DEBUG_RAE_NEWLINE)
				cout << "Line change. End of IMPORT_NAME.\n";
			#endif
			if (currentTempElement)
			{
				//currentModuleHeaderFileName = currentTempElement->searchLast(Token::IMPORT_NAME)->name();
				#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_IMPORT)
					cout << "last module name was: " << currentModuleHeaderFileName;
				#endif

				/*
				string a_import_whole_name = "";

				int not_on_first = 0;

				for(Element* elem : currentTempElement->elements)
				{
				if( elem->token() == Token::IMPORT_NAME )
				{
				if(not_on_first > 0)
				{
				a_import_whole_name += "/";
				}
				else
				{
				not_on_first++;
				}
				a_import_whole_name += elem->name();
				}
				else
				{
				break;
				}
				}
				*/

				string a_import_whole_name = currentTempElement->importName("/");//a bit of a hack...
				//and the func that it is calling replace . with /

				#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_IMPORT)
					cout << "Importing: ";
					cout << a_import_whole_name;
					cout << " from module: " << moduleName();
					cout << "\n";
				#endif

				bool it_is_cpp_file = false;
				if(a_import_whole_name.size() > 4)
				{
					if(a_import_whole_name[0] == 'c'
						 && a_import_whole_name[1] == 'p'
						 && a_import_whole_name[2] == 'p'
						 && a_import_whole_name[3] == '/')
					{
						it_is_cpp_file = true;
						g_compiler->addCppHeaderAsImport(a_import_whole_name.substr(4));
					}
				}

				if(it_is_cpp_file == false)
				{
					listOfImports.push_back(currentTempElement);

					//newImportSignal(a_import_whole_name);
					g_compiler->addSourceFileAsImport(a_import_whole_name);
				}
			}

			newLine();
			//expectingToken = Token::UNDEFINED;
			doReturnToExpectToken();
			currentTempElement = 0;
		}
		else
		{
			if (currentTempElement)
			{
				#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_IMPORT)
					cout << "Import DEBUG got: " << set_token << "\n";
				#endif
				currentTempElement->newElement(lineNumber, Token::IMPORT_NAME, Kind::UNDEFINED, set_token);
			}

			expectingToken(Token::IMPORT_NAME);
		}
	}
	else if (expectingToken() == Token::MODULE_NAME)
	{
		if (set_token[0] == '.')
		{
			//cout<<"Got func_def (. Waiting return_types.\n";

			//expectingToken(Token::MODULE_NAME);
		}
		else if (set_token == "\n")
		{
			#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_NEWLINE)
				cout << "Line change. End of MODULE_NAME.\n";
			#endif

			if (currentModule)
			{
				Element* last_module_name = currentModule->searchLast(Token::MODULE_DIR);
				last_module_name->token(Token::MODULE_NAME); // Set the last MODULE_DIR to be the actual MODULE_NAME.
				currentModuleHeaderFileName = last_module_name->name();
				
				#ifdef DEBUG_RAE_PARSER
					cout << "last module name was: " << currentModuleHeaderFileName;
				
					cout << "\n\nnewModule: ";
				
					for (Element* elem : currentModule->elements)
					{
						cout << elem->name() << ".";
						//rae::log(elem->name(), ".");
					}
					cout << "\n";
				#endif

				currentModule->name(moduleName());//Hmm. Using a very strange function we put the combined module name also
				//to the name variable of the current MODULE token.

			}

			newLine();
			//expectingToken(Token::UNDEFINED);
			doReturnToExpectToken();
		}
		else
		{
			if (currentModule)
			{
				Element* lang_elem = currentModule->newElement(lineNumber, Token::MODULE_DIR, Kind::UNDEFINED, set_token);
				//addToUserDefinedTokens(lang_elem); // Module names are NOT in userDefinedTokens
			}

			//expectingToken(Token::MODULE_NAME);
		}
	}
	/*
	else if( set_token == "\n" )
	{
	//How about this: expectingToken = Token::UNDEFINED;
	if( currentParentElement() )
	{
	currentParentElement()->newElement(Token::NEWLINE, set_token);
	}

	return;
	}*/
	else if (expectingToken() == Token::PARENTHESIS_BEGIN_LOG)
	{
		if (set_token == "(")
		{
			newParenthesisBegin(Token::PARENTHESIS_BEGIN_LOG, set_token);
		}
		else
		{
			//cout<<"RAE_ERROR: ";
			//rae::log("RAE_ERROR: ");
			//lineNumber.printOut();
			//cout<<" calling log(). No parenthesis after log.";
			//rae::log(" calling log(). No parenthesis after log.");

			ReportError::reportError("calling log(). No parenthesis after log.", previousElement());
		}

		//expectingToken = Token::UNDEFINED;
		doReturnToExpectToken();
	}
	else if (expectingToken() == Token::PARENTHESIS_BEGIN_LOG_S)
	{
		if (set_token == "(")
		{
			newParenthesisBegin(Token::PARENTHESIS_BEGIN_LOG_S, set_token);
		}
		else
		{
			//cout<<"RAE_ERROR: ";
			//"RAE_ERROR: "
			//lineNumber.printOut();
			//cout<<" calling log_s(). No parenthesis after log_s.";

			ReportError::reportError(" calling log_s(). No parenthesis after log_s.", previousElement());
		}

		//expectingToken = Token::UNDEFINED;
		doReturnToExpectToken();
	}
	
	/*else if( expectingToken() == Token::DEFINE_C_ARRAY_NAME )
	{
	if( set_token == "[" )
	{
	//This shouldn't happen...
	}
	else if( set_token == "]" )
	{
	//ignore this.
	#ifdef DEBUG_RAE_PARSER
	cout<<"expectingToken: DEFINE_ARRAY_NAME. Got ].\n";
	#endif
	}
	else //a name hopefully...
	{
	if( previousElement() &&
	previousElement()->kind() == Kind::C_ARRAY
	//|| previousElement()->token() == Kind::DEFINE_ARRAY_IN_CLASS)
	)
	{
	#ifdef DEBUG_RAE_PARSER
	cout<<"expectingToken: DEFINE_ARRAY_NAME. Got a name: "<<set_token<<"\n";
	#endif
	previousElement()->name( set_token );
	}
	else
	{
	#ifdef DEBUG_RAE_PARSER
	cout<<"the array name was NOT set because the previousElement was not a DEFINE_ARRAY or DEFINE_ARRAY_IN_CLASS.\n";
	#endif
	}
	//expectingToken = Token::UNDEFINED;
	doReturnToExpectToken();
	}
	}*/
	else if (expectingToken() == Token::ARRAY_VECTOR_STUFF)
	{
		//Probably REMOVE all of this...
		assert(0);
		if (set_token == "[")
		{
			//Shouldn't happen.
			ReportError::reportError("Duplicate [. Compiler TODO.", previousElement());
			//expectingToken(Token::VECTOR_STUFF);
		}
		else if (set_token == "]")
		{
			if (previousElement())
			{
				if (previousElement()->token() == Token::DEFINE_REFERENCE)
				{
#ifdef DEBUG_RAE_PARSER
					cout << "making the previous DEFINE_REFERENCE_IN_CLASS an array.\n";
#endif



					//previousElement()->kind(Kind::VECTOR);
					//previousElement()->createTemplateSecondType( previousElement()->type() );
					//previousElement()->type("vector");
					//expectingToken(Token::VECTOR_NAME);
				}
				else
				{
					ReportError::reportError("An ending array bracket ] in a strange place. The element before is not a type. It is: " + previousElement()->toString(), previousElement());
				}
			}

		}
		else if (set_token == ",")
		{
			assert(0);
			// A comma usually means a static array.
			//if(previousPreviousToken() == Token::BRACKET_DEFINE_ARRAY_BEGIN || previousPreviousToken() == Token::BRACKET_BEGIN)
			//{
			//A static array. TODO.
			ReportError::reportError("Got comma after something and a [. Maybe a static array. TODO.", previousElement());
			//}
		}
		else if (isNumericChar(set_token[0]))
		{
			//An array subscript.
		}
		else
		{

		}
	}
	else if (expectingToken() == Token::VECTOR_STUFF)
	{
		if (set_token == "!")
		{
			expectingToken(Token::VECTOR_STUFF);
		}
		else if (set_token == "(")
		{
			expectingToken(Token::VECTOR_STUFF);
		}
		else if (set_token == ")")
		{
			expectingToken(Token::VECTOR_NAME);
		}
		else
		{
			//if(currentTemplate)
			//{
			//	currentTemplate->type(set_token);
			//}
			if (currentReference)
			{
#ifdef DEBUG_RAE_HUMAN
				cout << "set vector second type. to: " << set_token << " for: " << currentReference->toString() << "\n";
#endif
				//currentReference->type(set_token);
				currentReference->createTemplateSecondType(set_token);
			}
			expectingToken(Token::VECTOR_STUFF);
		}
	}
	else if (expectingToken() == Token::VECTOR_NAME)
	{
		//if(currentTemplate)
		//{
		//	currentTemplate->name(set_token);
		//}
		if (currentReference)
		{
#ifdef DEBUG_RAE_HUMAN
			cout << "set vector name. to: " << set_token << " for: " << currentReference->toString() << "\n";
#endif
			currentReference->name(set_token);
		}
		//expectingToken = Token::UNDEFINED;
		doReturnToExpectToken();
	}
	else if (expectingToken() == Token::CLASS_TEMPLATE_SECOND_TYPE)
	{
		//if( set_token == "!" )//No, we won't get this on templates... We already got it.
		//{
		//expectingToken(Token::TEMPLATE_STUFF);
		//}
		//else
		/*if( set_token == "(" )//and we don't get this either...
		{
		//just eat this token.
		//expectingToken(Token::TEMPLATE_STUFF);
		}
		else */
		if (set_token == ")")
		{
			//end this simple class template.
			doReturnToExpectToken();
		}
		else //the template type inside !(type) aka TEMPLATE_SECOND_TYPE
		{
			if (currentClass)
			{
#ifdef DEBUG_RAE_HUMAN
				cout << "Add template second type to simple template class: " << set_token << " for: " << currentClass->toString() << "\n";
#endif
				//currentClass->newElement(lineNumber, Token::TEMPLATE_SECOND_TYPE, Kind::TEMPLATE/*are you sure this is not confusing...*/, /*string set_name:*/ "undefined", /*string set_type:*/ set_token );
				currentClass->createTemplateSecondType(set_token);
			}
			//expectingToken(Token::TEMPLATE_STUFF);
		}
	}
	else if (expectingToken() == Token::TEMPLATE_STUFF)
	{
		//if( set_token == "!" )//No, we won't get this on templates... We already got it.
		//{
		//expectingToken(Token::TEMPLATE_STUFF);
		//}
		//else
		if (set_token == "(")
		{
			//just eat this token.
			//expectingToken(Token::TEMPLATE_STUFF);
		}
		else if (set_token == ")")
		{
			expectingToken(Token::TEMPLATE_NAME);
		}
		else //the template type inside !(type)
		{
			//if(currentTemplate)
			//{
			//	currentTemplate->type(set_token);
			//}
			if (currentReference)
			{
#ifdef DEBUG_RAE_HUMAN
				cout << "Add subtype template definition type. to: " << set_token << " for: " << currentReference->toString() << "\n";
#endif
				//currentReference->type(set_token);
				//currentReference->newElement(lineNumber, Token::TEMPLATE_SECOND_TYPE, Kind::TEMPLATE/*are you sure this is not confusing...*/, /*string set_name:*/ "undefined", /*string set_type:*/ set_token );
				currentReference->createTemplateSecondType(set_token);
			}
			//expectingToken(Token::TEMPLATE_STUFF);
		}
	}
	else if (expectingToken() == Token::TEMPLATE_NAME)
	{
		//if(currentTemplate)
		//{
		//	currentTemplate->name(set_token);
		//}
		if (currentReference)
		{
#ifdef DEBUG_RAE_HUMAN
			cout << "set template definition name. to: " << set_token << " for: " << currentReference->toString() << "\n";
#endif
			currentReference->name(set_token);
		}
		//expectingToken = Token::UNDEFINED;
		doReturnToExpectToken();
	}
	else if (expectingToken() == Token::ALIAS)
	{
		// We already got the new type (set in to type property) for the alias with the expectingTypeFor mechanism.
		if (set_token == "=")
		{
			//Ignore.
			return true;
		}
		else // The oldName part will be set to name property of the element.
		{
			//TODO check for BUILT_IN_TYPE aliases: alias newName = int
			//if (isBuiltInType(set_token)) ... do something...
			Element* old_name = searchToken(set_token);
			if (old_name)
			{
				if(previousToken() == Token::ALIAS)
				{
					#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_ALIAS)
						if(old_name->type() == set_token)
						{
							cout << "ALIAS old_name <" << set_token << "> was type: " << old_name->toSingleLineString() << "\n";
						}
						else if(old_name->name() == set_token)
						{
							cout << "ALIAS old_name <" << set_token << "> was name: " << old_name->toSingleLineString() << "\n";
						}
					#endif
					previousElement()->type(set_token);
					previousElement()->definitionElement(old_name);
				}
				else
				{
					ReportError::compilerError("Couldn't find ALIAS element from where it should have been. 1.", previousElement() );
				}
			}
			else if (previousToken() == Token::ALIAS)
			{
				previousElement()->type(set_token);
				// TODO check this: how isUnknownType handled for ALIASes? No code written yet, so will it work?
				previousElement()->isUnknownType(true);
			}
			else
			{
				ReportError::compilerError("Couldn't find ALIAS element from where it should have been. 2.", previousElement() );
			}
			doReturnToExpectToken();
		}
	}
	else if (expectingToken() == Token::UNDEFINED)
	{
		// Rae reserved words:


		/*Role is kind of like context.
		UNDEFINED,
		GLOBAL,
		INSIDE_CLASS,
		FUNC_RETURN,
		FUNC_PARAMETER,
		INSIDE_FUNCTION
		*/

		/*if( expectingRole() == Role::UNDEFINED
		|| expectingRole() == Role::GLOBAL
		|| expectingRole() == Role::INSIDE_CLASS
		)
		{
		}

		if( expectingRole() == Role::FUNC_RETURN )
		{

		}*/

		/*
		if( expectingRole() != Role::FUNC_RETURN
		&& expectingRole() != Role::FUNC_PARAMETER
		)
		{
		//These things are not allowed with FUNC_RETURN or FUNC_PARAMETER:
		}
		*/


		
		if (set_token == "func")
		{
#ifdef DEBUG_RAE
			cout << "Got func. Waiting func_definition.\n";
			//rae::log("Got func. Waiting func_definition.\n");
#endif
			expectingToken(Token::FUNC_DEFINITION);
			newFunc();
		}
		else if (set_token == "prop")
		{
			cout << "TODO prop keyword for property\n";
		}
		else if (set_token == ",")
		{
			/*
			Element* stack_elem;
			Token::e parenthesis_type;

			//if( isInsideLogStatement )
			//{
			stack_elem = parenthesisStack.back();
			//}

			if( stack_elem )
			{
			//We just do matching: is it good enough:
			parenthesis_type = Token::matchParenthesisEnd(stack_elem->token());
			}
			*/

			if (not bracketStack.empty())
			{
				//doing some kind of template type definition with brackets...
				Element* our_array_definition = bracketStack.back();
				if (our_array_definition->token() == Token::BRACKET_DEFINE_ARRAY_BEGIN)
				{
					our_array_definition->token(Token::BRACKET_DEFINE_STATIC_ARRAY_BEGIN);
				}
				newElement(Token::COMMA, Kind::UNDEFINED, set_token);
			}
			else if (expectingRole() == Role::FUNC_RETURN)
			{
				//TODO: Hmm. These are the same as normal case. So we could just remove the expectingRoles here...
				newElement(Token::COMMA, Kind::UNDEFINED, set_token);
			}
			else if (expectingRole() == Role::FUNC_PARAMETER || expectingRole() == Role::TEMPLATE_PARAMETER)
			{
				newElement(Token::COMMA, Kind::UNDEFINED, set_token);
			}
			else //Role::UNDEFINED or something. Normal function body etc.
			{
				Token::e parenthesis_type = parenthesisStackTokenType();

				if (parenthesis_type == Token::PARENTHESIS_BEGIN_LOG || parenthesis_type == Token::PARENTHESIS_BEGIN_LOG_S)
				{
					newElement(Token::LOG_SEPARATOR, Kind::UNDEFINED, set_token);//it is still a comma "," but we write it out as << for C++
				}
				else
				{
					newElement(Token::COMMA, Kind::UNDEFINED, set_token);
				}
			}
		}
		else if (set_token == "val" || set_token == "ref" || set_token == "opt" || set_token == "link" || set_token == "ptr")
		{
			if (!bracketStack.empty())
			{
				newDefineReference(set_token, Role::TEMPLATE_PARAMETER);
			}
			else
			{
				Element* our_ref = newDefineReference(set_token, expectingRole());
				unfinishedElement(our_ref);
			}

		}
		//Rae
		else if (set_token == "bool"
			|| set_token == "byte"
			|| set_token == "ubyte"
			|| set_token == "char"
			|| set_token == "wchar"
			|| set_token == "dchar"
			|| set_token == "short"
			|| set_token == "ushort"
			|| set_token == "int"
			|| set_token == "uint"
			|| set_token == "long"
			|| set_token == "ulong"
			|| set_token == "float"
			|| set_token == "double"
			//|| set_token == "real"
			|| set_token == "string"
			|| set_token == "void"
			)
		{
			//checkPreviousBracketArrayDefineEtc(Token::DEFINE_BUILT_IN_TYPE_NAME);

			//if( currentParentElement() && currentParentElement()->token() == Token::BRACKET_DEFINE_ARRAY_BEGIN )

			bool is_let = false;

			if (previousToken() == Token::LET)
			{
				is_let = true;
				previousElement()->token(Token::EMPTY);
			}
			else if (previousToken() == Token::SIZEOF || (previousToken() == Token::PARENTHESIS_BEGIN && previous2ndToken() == Token::SIZEOF))
			{
				newDefineBuiltInType(BuiltInType::stringToBuiltInType(set_token), expectingRole(), set_token);
				return true;
			}

			Element* our_ref;
			if (not bracketStack.empty())
			{
				our_ref = newDefineBuiltInType(BuiltInType::stringToBuiltInType(set_token), Role::TEMPLATE_PARAMETER, set_token);
			}
			else
			{
				expectingToken(Token::DEFINE_BUILT_IN_TYPE_NAME);
				our_ref = newDefineBuiltInType(BuiltInType::stringToBuiltInType(set_token), expectingRole(), set_token);
				unfinishedElement(our_ref);
			}

			our_ref->isLet(is_let);

			/*
			if( !bracketStack.empty() )
			{
			if(previous2ndElement() && previous2ndElement()->token() == Token::BRACKET_BEGIN)
			{
			previous2ndElement()->token(Token::BRACKET_DEFINE_ARRAY_BEGIN);

			our_ref->containerType( ContainerType::ARRAY );
			}
			}*/
			//if(previous2ndElement() && previous2ndElement()->token() == Token::BRACKET_BEGIN)
			//if( currentParentElement() && currentParentElement()->token() == Token::BRACKET_BEGIN )
			//{
			//previous2ndElement()->token(Token::BRACKET_DEFINE_ARRAY_BEGIN);
			//currentParentElement()->token(Token::BRACKET_DEFINE_ARRAY_BEGIN);
			//our_ref->containerType( ContainerType::ARRAY );
			//}
		}
		/*
		else if( set_token == "bool" )
		{
		expectingToken(Token::DEFINE_BUILT_IN_TYPE_NAME);
		newDefineBuiltInType(BuiltInType::BOOL, expectingRole(), set_token);
		}
		else if( set_token == "byte" )
		{
		expectingToken(Token::DEFINE_BUILT_IN_TYPE_NAME);
		newDefineBuiltInType(BuiltInType::BYTE, expectingRole(), set_token);
		}
		else if( set_token == "ubyte" )
		{
		expectingToken(Token::DEFINE_BUILT_IN_TYPE_NAME);
		newDefineBuiltInType(BuiltInType::UBYTE, expectingRole(), set_token);
		}
		else if( set_token == "char" )
		{
		expectingToken(Token::DEFINE_BUILT_IN_TYPE_NAME);
		newDefineBuiltInType(BuiltInType::CHAR, expectingRole(), set_token);
		}
		else if( set_token == "wchar" )
		{
		expectingToken(Token::DEFINE_BUILT_IN_TYPE_NAME);
		newDefineBuiltInType(BuiltInType::WCHAR, expectingRole(), set_token);
		}
		else if( set_token == "dchar" )
		{
		expectingToken(Token::DEFINE_BUILT_IN_TYPE_NAME);
		newDefineBuiltInType(BuiltInType::DCHAR, expectingRole(), set_token);
		}
		else if( set_token == "int" )
		{
		expectingToken(Token::DEFINE_BUILT_IN_TYPE_NAME);
		newDefineBuiltInType(BuiltInType::INT, expectingRole(), set_token);
		}
		else if( set_token == "uint" )
		{
		expectingToken(Token::DEFINE_BUILT_IN_TYPE_NAME);
		newDefineBuiltInType(BuiltInType::UINT, expectingRole(), set_token);
		}
		else if( set_token == "long" )
		{
		expectingToken(Token::DEFINE_BUILT_IN_TYPE_NAME);
		newDefineBuiltInType(BuiltInType::LONG, expectingRole(), set_token);
		}
		else if( set_token == "ulong" )
		{
		expectingToken(Token::DEFINE_BUILT_IN_TYPE_NAME);
		newDefineBuiltInType(BuiltInType::ULONG, expectingRole(), set_token);
		}
		else if( set_token == "float" )
		{
		expectingToken(Token::DEFINE_BUILT_IN_TYPE_NAME);
		newDefineBuiltInType(BuiltInType::FLOAT, expectingRole(), set_token);
		}
		else if( set_token == "double" )
		{
		expectingToken(Token::DEFINE_BUILT_IN_TYPE_NAME);
		newDefineBuiltInType(BuiltInType::DOUBLE, expectingRole(), set_token);
		}
		*/
		/*else if( set_token == "real" )
		{
		expectingToken = Token::DEFINE_BUILT_IN_TYPE_NAME;
		newDefineBuiltInType(BuiltInType::REAL, set_token);
		}*/
		/*
		else if( set_token == "string" )
		{
		expectingToken(Token::DEFINE_BUILT_IN_TYPE_NAME);
		newDefineBuiltInType(BuiltInType::STRING, expectingRole(), set_token);
		}
		*/
		else if (set_token == "vector")
		{
			assert(0); // No more vectors. JONDE REMOVE this stuff.
			expectingToken(Token::VECTOR_STUFF);
			newDefineVector("unknown", "unknown");
			return true;
		}
		else if (set_token == "(")
		{
			if (previousToken() == Token::CLASS)
			{
				#ifdef DEBUG_RAE_HUMAN
					cout << "( means template class.\n";
				#endif
				//a simple class template
				previousElement()->kind(Kind::TEMPLATE);
				expectingToken(Token::CLASS_TEMPLATE_SECOND_TYPE);
				return true;
			}
			else if (expectingRole() == Role::FUNC_PARAMETER)
			{
				//TODO: check parenthesisStack:
				newParenthesisBegin(Token::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES, "(");
				return true;
			}
			else //normal (
			{
				if(currentReference != nullptr)
				{
					#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_PARENTHESIS)
						cout << "We really have some dangling currentReference. Got a (. nameless stuff. Ending currentReference.\n";
					#endif
					currentReference = nullptr;
				}
			
				//cout<<"normal (\n";
				//if(previousElement())
				//	cout<<"previousToken: "<<previousElement()->toString();
				//else cout<<"no previousElement!\n";
				newParenthesisBegin(Token::PARENTHESIS_BEGIN, set_token);
				return true;
			}
		}
		else if (set_token == ")")
		{
			if (expectingRole() == Role::FUNC_RETURN)
			{
				#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_PARENTHESIS)
					cout << "Got ending ) for FUNC_RETURNs in UNDEFINED. Going back to FUNC_DEFINITION.\n";
				#endif				

				expectingRole(Role::UNDEFINED);//could also be Role::FUNC_DEFINITION, but we don't need that, so.
				//TODO: check parenthesisStack:
				newParenthesisEnd(Token::PARENTHESIS_END_FUNC_RETURN_TYPES, ")");
				expectingToken(Token::FUNC_DEFINITION);
				return true;
			}
			else if (expectingRole() == Role::FUNC_PARAMETER)
			{
				#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_PARENTHESIS)
					cout << "Got ending ) for FUNC_PARAMETERs in UNDEFINED. Going back to UNDEFINED.\n";
				#endif

				expectingRole(Role::UNDEFINED);//could also be Role::FUNC_DEFINITION, but we don't need that, so.
				//TODO: check parenthesisStack:
				newParenthesisEnd(Token::PARENTHESIS_END_FUNC_PARAM_TYPES, ")");
				expectingToken(Token::UNDEFINED);
				return true;
			}
			else
			{
				//cout<<"normal )\n";
				newParenthesisEnd(Token::PARENTHESIS_END, set_token);
				return true;
			}
		}
		else if (set_token == "cast")
		{
			newElement(Token::CAST, Kind::UNDEFINED, set_token);
			return true;
		}
		else if (set_token == "[")
		{
			//cout<<"BRACKET we got a bracket in the normal place and it is strange!\n";

			//cout<<"Wise people say, this should never get called. SourceParser line 4398.\n";
			//assert(0);

			if (previousElement() && (previousElement()->isUnknownType() || previousElement()->token() == Token::USE_REFERENCE))
			{
				#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_BRACKET)
					cout << "Bracket begin simple.\n";
				#endif
				newBracketBegin(Token::BRACKET_BEGIN, set_token);
			}
			else if (previousElement() && previousElement()->token() == Token::DEFINE_REFERENCE)
			{
				#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_BRACKET)
                	cout << "Bracket define array begin.\n";
                #endif
				//convert to array:
				currentReference = nullptr; // leave currentReference to nullptr. This is important.
				previousElement()->token(Token::BRACKET_DEFINE_ARRAY_BEGIN);
				currentParentElement(previousElement());
				previousElement()->containerType(ContainerType::ARRAY);
				previousElement()->type("array");
				bracketStack.push_back(previousElement());
			}
            else if (previousElement() && previousElement()->token() == Token::ASSIGNMENT)
			{
				#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_BRACKET)
                	cout << "Bracket initializer list begin!\n";
                #endif
                // Bracket initializer list
                newBracketBegin(Token::BRACKET_INITIALIZER_LIST_BEGIN, set_token);
            }
            else if (previousElement() && previousElement()->token() == Token::CAST)
			{
				#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_BRACKET)
					cout << "Bracket begin CAST.\n";
				#endif
				newBracketBegin(Token::BRACKET_CAST_BEGIN, set_token);
			}
			else
			{
				#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_BRACKET)
                	cout << "Bracket newDefineArray.\n";
                	if(previousElement())
                    	cout << "previousElement: " << previousElement()->toSingleLineString() << "\n";
                #endif
				newDefineArray();
			}



			//expectingContainer( ContainerType::ARRAY );
			//not a good idea: we need to handle opt, val, bool, int etc. which are all handled in undefined.//expectingToken(Token::ARRAY_VECTOR_STUFF);

			/*
			if(previousElement())
			{
			if( previousElement()->token() == Token::DEFINE_REFERENCE )
			{*/
			/*
			!!!!!!!!!!!!!!!!!!!OK
			This never gets called. Look in expecting NAME thing...
			*/


			/*
			#ifdef DEBUG_RAE_PARSER
			cout<<"bracket, make previous definition a DEFINE_VECTOR.\n";
			#endif*/
			/*
			//SomeClass[] someVector
			//----here->------------
			//OLD behaviour turned it into a C_ARRAY, but now it's the preferred way to use vectors.
			previousElement()->kind(Kind::C_ARRAY);
			expectingToken(Token::DEFINE_C_ARRAY_NAME);
			*/
			//cout<<"A bracket but we didn't have a DEFINE_REFERENCE? Maybe it is an error.\n";
			/*not up to date, as it won't be called...
			previousElement()->kind(Kind::VECTOR);
			previousElement()->createTemplateSecondType( previousElement()->type() );
			previousElement()->type("vector");
			expectingToken(Token::VECTOR_NAME);
			*/
			/*}
			else
			{
			#ifdef DEBUG_RAE_PARSER
			cout<<"normal newBracketBegin.";
			#endif
			//newBracketBegin(Token::BRACKET_BEGIN, set_token);
			}
			}
			else ReportError::reportError("a bracket, but no previousElement. This is a compiler error. We'll need to fix it.", previousElement());
			*/
		}
		else if (set_token == "]")
		{
			/*if( previousElement() && previousElement()->token() == Token::BRACKET_BEGIN )
			{
			//previousElement()->token(Token::BRACKET_DEFINE_ARRAY_BEGIN);
			newBracketEnd(Token::BRACKET_END, set_token);
			}
			else if( previousElement() && previousElement()->token() == Token::DEFINE_ARRAY )
			{

			}
			else
			{*/
			#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_BRACKET)
				cout << "normal newBracketEnd.";
			#endif

			Element* our_array_definition;
			if (not bracketStack.empty())
			{
				our_array_definition = bracketStack.back();
			}

			newBracketEnd(Token::BRACKET_END, set_token);
			if (our_array_definition && (our_array_definition->token() == Token::BRACKET_DEFINE_ARRAY_BEGIN || our_array_definition->token() == Token::BRACKET_DEFINE_STATIC_ARRAY_BEGIN))
			{
				expectingNameFor(our_array_definition);
			}
			//}
		}
		else if (set_token == "->")
		{
			//newElement(Token::POINT_TO, Kind::UNDEFINED, set_token);
			newPointToElement();
		}
		else if (set_token == "loop")
		{
			newElement(Token::FOR, Kind::UNDEFINED, set_token);
		}
		else if (set_token == "for")
		{
			ReportError::reportError("RAE_ERROR for is deprecated!\n", previousElement());
		}
		/*
		else if( set_token == "foreach" )
		{
		newElement(Token::FOR_EACH, Kind::UNDEFINED, set_token);
		}*/
		else if (set_token == "in")
		{
			newElement(Token::IN_TOKEN, Kind::UNDEFINED, set_token);
		}
		else if (set_token == "alias")
		{
			#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_ALIAS)
				cout << "Got alias keyword. Going to alias mode.\n";
			#endif
			Element* lang_elem = newElement(Token::ALIAS, Kind::UNDEFINED, set_token);
			addToUserDefinedTokens(lang_elem);
			pushExpectingToken(Token::ALIAS);
			expectingNameFor(lang_elem);
			return true;
		}
		else if (set_token == "log_s")
		{
			//isInsideLogStatement = true;
			newElement(Token::LOG_S, Kind::UNDEFINED, set_token);
			expectingToken(Token::PARENTHESIS_BEGIN_LOG_S);

		}
		else if (set_token == "log")
		{
			//isInsideLogStatement = true;
			newElement(Token::LOG, Kind::UNDEFINED, set_token);
			expectingToken(Token::PARENTHESIS_BEGIN_LOG);
		}
		else if (set_token == "null")
		{
			newElement(Token::RAE_NULL, Kind::UNDEFINED, set_token);
		}
		else if (set_token == "is")
		{
			newElement(Token::IS, Kind::UNDEFINED, set_token);
		}
		/*
		//These don't work anymore as : is now it's own token. We must wait for it...
		else if( set_token == "public:" )
		{
		newElement(Token::VISIBILITY_DEFAULT, set_token);
		}
		else if( set_token == "protected:" )
		{
		newElement(Token::VISIBILITY_DEFAULT, set_token);
		}
		else if( set_token == "library:" )
		{
		newElement(Token::VISIBILITY_DEFAULT, set_token);
		}
		else if( set_token == "private:" )//what's the real use of private, anyway?
		{
		newElement(Token::VISIBILITY_DEFAULT, set_token);
		}
		*/
		/*//TODO errors for these old LONG visibility keywords:
		else if( set_token == "public" )
		{
		newElement(Token::VISIBILITY, Kind::UNDEFINED, set_token );
		}
		else if( set_token == "protected" )
		{
		newElement(Token::VISIBILITY, Kind::UNDEFINED, set_token);
		}
		else if( set_token == "library" )
		{
		//TODO
		newElement(Token::VISIBILITY, Kind::UNDEFINED, set_token);
		}
		else if( set_token == "hidden" )
		{
		//TODO
		newElement(Token::VISIBILITY, Kind::UNDEFINED, set_token);
		}
		else if( set_token == "private" )//what's the real use of private, anyway?
		{
		newElement(Token::VISIBILITY, Kind::UNDEFINED, set_token);
		}
		*/
		else if (set_token == "let")
		{
			newElement(Token::LET, Kind::UNDEFINED, set_token);
		}
		else if (set_token == "mut")
		{
			newElement(Token::MUT, Kind::UNDEFINED, set_token);
		}
		else if (set_token == "pub")
		{
			newElement(Token::VISIBILITY, Kind::UNDEFINED, set_token);
		}
		else if (set_token == "lib")
		{
			//TODO
			newElement(Token::VISIBILITY, Kind::UNDEFINED, set_token);
		}
		else if (set_token == "priv") // -> protected
		{
			newElement(Token::VISIBILITY, Kind::UNDEFINED, set_token);
		}
		else if (set_token == ":")
		{
			if (previousElement())
			{
				if (previousElement()->token() == Token::VISIBILITY)
				{
					//change it.
					previousElement()->token(Token::VISIBILITY_DEFAULT);
					////Now we don't add, as we already added before.
					//add :
					//previousElement()->name( previousElement->name() + ":" );
				}
			}

			if (isWaitingForNamespaceDot)
			{
				cout << "Got : \n";
				isWaitingForNamespaceDot = false;
				ReportError::reportError("Don't use :: for namespace separation. Use dot .", previousElement());
				assert(0);
			}
		}
		/*
		else if( set_token == "!" )
		{
		cout<<"Got template!\n";

		if( previousElement() )
		{
		cout<<"Got previousElement:"<< previousElement()->toString()<<"\n";
		//TODO maybe we really need to check this:
		//if( previousElement()->token() == Token::DEFINE_REFERENCE )
		//{

		//change the typetype.
		previousElement()->kind(Kind::TEMPLATE);
		//}
		}
		expectingToken(Token::TEMPLATE_STUFF);
		}
		*/
		else if (set_token == "free")
		{
			//Duplicate free. We'll have to sort this out later. TODO.

			assert(0);
			//cout<<"TODO Got free. Waiting free_class.\n";
			////rae::log("TODO Got free. Waiting free_class.\n");
			expectingToken(Token::FREE_NAME);
			//newFunc();
			if (previousElement())
			{
				if (previousElement()->token() == Token::REFERENCE_DOT)
				{
					if (previous2ndElement() && (previous2ndElement()->token() == Token::USE_REFERENCE)
						//|| previous2ndElement->token() == Token::UNKNOWN_USE_REFERENCE)
						)
					{
						//hide the REFERENCE_DOT from something.free
						previousElement()->token(Token::UNDEFINED);
						previousElement()->name("");

						previous2ndElement()->token(Token::FREE);

						//expectingToken(Token::UNDEFINED);
						doReturnToExpectToken();
					}
				}
			}
		}
		else if (set_token == "delete")
		{
			cout << "TODO Got delete. there's no delete keyword in Rae. Use free instead.\n";
			//rae::log("TODO Got delete. Waiting delete_class.\n");
			//expectingToken = Token::FUNC_DEFINITION;
			//newFunc();
		}
		else if (set_token == "import")
		{
			newImport(set_token);
		}
		else if (set_token == "module")
		{
			////rae::log("found WORD module:>", set_token, "<\n");
			newModule(set_token);
		}
		else if (set_token == "project")
		{
			//cout << "TODO project keyword.\n";
			// TODO save the project to m_projectName variable
			Element* lang_elem = newElement(Token::PROJECT, Kind::UNDEFINED, set_token);
			expectingNameFor(lang_elem);
		}
		else if (set_token == "targetdir")
		{
			cout << "TODO targetdir keyword.\n";
		}
		else if (set_token == "importdirs")
		{
			//cout << "TODO importdirs keyword inside project.\n";
			expectingToken(Token::IMPORT_DIRS);
		}
		/*
		else if (set_token == "targetdir")
		{
		 cout << "TODO targetdir keyword inside project.\n";
		}
		else if (set_token == "defines")
		{
			cout << "TODO defines keyword inside project.\n";
		}
		else if (set_token == "configuration")
		{
			cout << "TODO configuration keyword inside project.\n";
		}
		else if (set_token == "flags")
		{
			cout << "TODO flags keyword inside project.\n";
		}
		else if (set_token == "links")
		{
			cout << "TODO defines keyword inside project.\n";
		}
		*/
		//try to automatically figure out where to put this c++ code. Usually it goes to headers. Inside funcs it goes to cpp files.
		else if (set_token == "@cpp")
		{
			isPassthroughMode = true;
			//cout<<"To PASSTHROUGH_HDR mode.\n";
			newElement(Token::PRAGMA_CPP, Kind::UNDEFINED, set_token);
			expectingToken(Token::PRAGMA_CPP);
		}
		//copy to c++ header .hpp:
		else if (set_token == "@cpp_hdr")
		{
			isPassthroughMode = true;
			//cout<<"To PASSTHROUGH_HDR mode.\n";
			newElement(Token::PRAGMA_CPP_HDR, Kind::UNDEFINED, set_token);
			expectingToken(Token::PRAGMA_CPP_HDR);
		}
		//copy to c++ source file .cpp:
		else if (set_token == "@cpp_src")
		{
			isPassthroughSourceMode = true;
			//cout<<"To PASSTHROUGH_HDR mode.\n";
			newElement(Token::PRAGMA_CPP_SRC, Kind::UNDEFINED, set_token);
			expectingToken(Token::PRAGMA_CPP_SRC);
		}
		else if (set_token == "@dont_generate_code")
		{
			isWriteToFile = false; // Don't generate code for this module/SourceParser object
		}
		else if (set_token == "@cpp_bindings")
		{
			isWriteToFile = false; // Don't generate code for this module/SourceParser object
			isCppBindingsParseMode = true;
		}
		else if (set_token == "@asm")
		{
			ReportError::reportError("@asm is reserved for assembler, but it is not yet implemented.", currentLineNumber(), namespaceString());
		}
		else if (set_token == "@ecma")
		{
			ReportError::reportError("@ecma is reserved for ecmascript, but it is not yet implemented.", currentLineNumber(), namespaceString());
		}
		else if (set_token == "@javascript")
		{
			ReportError::reportError("@javascript is reserved, but use @ecma instead, but it is not yet implemented.", currentLineNumber(), namespaceString());
		}
		//Ok, stop the press! It's not a reserved word. We really have to do something with this info:
		//Maybe we'll first check if it's some user defined type.
		else
		{
			handleUserDefinedToken(set_token);
		}

		/*
		if( set_token == "\n" )
		{
		newLine();
		}
		else if( set_token == ";" )
		{
		newElement(Token::SEMICOLON, set_token);
		}
		else if( set_token == "{" )
		{
		newScopeBegin();
		}
		else if( set_token == "}" )
		{
		newScopeEnd();
		}
		else if( set_token == "." )
		{
		newElement(Token::REFERENCE_DOT, set_token);
		}
		else if( set_token == "func" )
		{
		#ifdef DEBUG_RAE
		//rae::log("Got func. Waiting func_definition.\n");
		#endif
		expectingToken = Token::FUNC_DEFINITION;
		newFunc();
		}
		else if( set_token == "int" )
		{
		newElement(Token::INT, set_token);
		}
		else if( set_token == "(" )
		{
		newParenthesisBegin(Token::PARENTHESIS_BEGIN, set_token);
		}
		else if( set_token == ")" )
		{
		newParenthesisEnd(Token::PARENTHESIS_END, set_token);
		}
		else if( set_token == "+" )
		{
		newElement(Token::PLUS, set_token);
		}
		else if( set_token == "-" )
		{
		newElement(Token::MINUS, set_token);
		}
		else if( set_token == "=" )
		{
		newElement(Token::ASSIGNMENT, set_token);
		}
		else if( set_token == "/" )
		{
		newElement(Token::DIVIDE, set_token);
		}
		else if( set_token == "if" )
		{
		newElement(Token::IF, set_token);
		}
		else if( set_token == "log_s" )
		{
		newElement(Token::LOG_S, set_token);
		expectingToken = Token::PARENTHESIS_BEGIN_LOG_S;
		}
		else if( set_token == "log" )
		{
		newElement(Token::LOG, set_token);
		expectingToken = Token::PARENTHESIS_BEGIN_LOG;
		}
		else if( set_token == "public:" )
		{
		newElement(Token::VISIBILITY_DEFAULT, set_token);
		}
		else if( set_token == "protected:" )
		{
		newElement(Token::VISIBILITY_DEFAULT, set_token);
		}
		else if( set_token == "library:" )
		{
		newElement(Token::VISIBILITY_DEFAULT, set_token);
		}
		else if( set_token == "private:" )//what's the real use of private, anyway?
		{
		newElement(Token::VISIBILITY_DEFAULT, set_token);
		}
		else if( set_token == "public" )
		{
		newElement(Token::VISIBILITY, set_token);
		}
		else if( set_token == "protected" )
		{
		newElement(Token::VISIBILITY, set_token);
		}
		else if( set_token == "library" )
		{
		newElement(Token::VISIBILITY, set_token);
		}
		else if( set_token == "private" )//what's the real use of private, anyway?
		{
		newElement(Token::VISIBILITY, set_token);
		}
		else if( set_token == "return" )
		{
		#ifdef DEBUG_RAE
		//rae::log("Got return.\n");
		#endif
		//expectingToken = Token::PARENTHESIS_BEGIN_RETURN;
		newElement(Token::RETURN, set_token);
		}
		else if( set_token == "new" )
		{
		//rae::log("TODO Got new. Waiting new_class.\n");
		//expectingToken = Token::FUNC_DEFINITION;
		//newFunc();
		}
		else if( set_token == "delete" )
		{
		//rae::log("TODO Got delete. Waiting delete_class.\n");
		//expectingToken = Token::FUNC_DEFINITION;
		//newFunc();
		}
		else if( set_token == "class" )
		{
		////rae::log("found WORD class:>", set_token, "<\n");
		expectingToken = Token::CLASS_NAME;
		}
		else if( set_token == "module" )
		{
		////rae::log("found WORD module:>", set_token, "<\n");
		newModule(set_token);
		}
		//Ok, stop the press! It's not a reserved word. We really have to do something with this info:
		//Maybe we'll first check if it's some user defined type.
		else
		{
		handleUserDefinedToken(set_token);
		}
		*/

	}
	else if (expectingToken() == Token::FREE_NAME)
	{
		//TODO check if name is a valid name...
		newElement(Token::FREE, Kind::UNDEFINED, set_token);
		//expectingToken = Token::UNDEFINED;
		doReturnToExpectToken();
	}
	else if (expectingToken() == Token::DEFINE_BUILT_IN_TYPE_NAME)
	{
		if (set_token == ")")
		{
			if (expectingRole() == Role::FUNC_RETURN)
			{
#ifdef DEBUG_RAE_PARSER
				cout << "Got ending ) for FUNC_RETURNs in DEFINE_BUILT_IN_TYPE_NAME. Going back to FUNC_DEFINITION.\n";
#endif

				expectingRole(Role::UNDEFINED);//could also be Role::FUNC_DEFINITION, but we don't need that, so.
				//TODO: check parenthesisStack:
				newParenthesisEnd(Token::PARENTHESIS_END_FUNC_RETURN_TYPES, ")");
				expectingToken(Token::FUNC_DEFINITION);
			}
			else
			{
				newParenthesisEnd(Token::PARENTHESIS_END, ")");
				ReportError::reportError(") parenthesis_end - can't be a name for built in type.\n", previousElement());
			}
		}
		else if (set_token == "]")
		{
			newBracketEnd(Token::BRACKET_DEFINE_ARRAY_END, set_token);
		}
		else if (set_token == ",")
		{
			//REMOVE this strange case... it's old.
			assert(0);
			//setUnfinishedElementToStaticArray();
		}
		else if (unfinishedElement())
		{
			if (isNumericChar(set_token[0]))
			{
				if (unfinishedElement()->containerType() != ContainerType::STATIC_ARRAY)
				{
					ReportError::reportError("Waiting for BUILT_IN_TYPE_NAME. Got a number, but this is not a static array.", unfinishedElement());
				}
				else
				{
					unfinishedElement()->staticArraySize(stringToNumber<int>(set_token));
				}
			}
			else
			{
				setNameAndCheckForPreviousDefinitions(unfinishedElement(), set_token);

				/*if( checkIfNewDefinedNameIsValid(currentReference, set_token) )
				{
				currentReference->name(set_token);
				}
				else
				{
				ReportError::reportError("redefinition of name: " + set_token);
				}*/

				//And now we'll see what to do next:
				if (expectingRole() == Role::FUNC_RETURN)
				{
					//Back to waiting for UNDEFINED as that handles the coming ")" the best.
					expectingToken(Token::UNDEFINED);
				}
				else
				//else if (currentClass != nullptr && currentFunc == nullptr && currentEnum == nullptr)//INSIDE_CLASS doesn't work currently: if(expectingRole() == Role::INSIDE_CLASS)
				{
					//SECOND of this text! line 2659
					if (unfinishedElement()->containerType() == ContainerType::UNDEFINED)
					{
						//in the normal function body case (which is still Role::UNDEFINED
						//because we haven't yet needed e.g. Role::FUNCTION_BODY for anything)
						//we wait for initdata if there is some:
						expectingToken(Token::INIT_DATA);
					}
					else
					{
						expectingToken(Token::UNDEFINED);
						unfinishedElement(nullptr);
					}
				}
				/*
				else
				{
#ifdef DEBUG_RAE_PARSER
					if (currentFunc == nullptr)
						cout << "currentFunc is null.\n";
					else ReportError::reportInfo("currentFunc: ", currentFunc);
					if (currentEnum == nullptr)
						cout << "currentEnum is null.\n";
					if (currentClass == nullptr)
						cout << "currentClass is null.\n";

					ReportError::reportInfo("No initdata in not inside class: ", unfinishedElement());
#endif
					expectingToken(Token::UNDEFINED);
					unfinishedElement(nullptr);
				}*/
			}
		}
		else
		{
			ReportError::reportError("Waiting for BUILT_IN_TYPE_NAME. No unfinishedElement.", previousElement());
		}
	}
	else if (expectingToken() == Token::DEFINE_REFERENCE)
	{
		handleUserDefinedToken(set_token);
	}
	else if (expectingToken() == Token::DEFINE_REFERENCE_NAME)
	{
		if (set_token == "(" || set_token == ",")
		{
			#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_REFERENCES) || defined(DEBUG_RAE_PARENTHESIS)
				cout << "Got ( or a , which can't be a DEFINE_REFERENCE_NAME. This is probably a nameless temporary object. We currently support them only as value types.\n";
			#endif
			doReturnToExpectToken();
			return handleToken(set_token); //rehandle set_token.
		}
		else if (set_token == ")")
		{
			if (expectingRole() == Role::FUNC_RETURN)
			{
				#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_REFERENCES) || defined(DEBUG_RAE_PARENTHESIS) || defined(DEBUG_RAE_FUNC)
					cout << "Got ending ) for FUNC_RETURNs in DEFINE_REFERENCE_NAME. Going back to FUNC_DEFINITION.\n";
				#endif

				expectingRole(Role::UNDEFINED);//could also be Role::FUNC_DEFINITION, but we don't need that, so.
				//TODO: check parenthesisStack:
				newParenthesisEnd(Token::PARENTHESIS_END_FUNC_RETURN_TYPES, ")");
				expectingToken(Token::FUNC_DEFINITION);
			}
			else
			{
				// A nameless define.
				newParenthesisEnd(Token::PARENTHESIS_END, ")");
				//ReportError::reportError(") parenthesis_end - can't be a name for a reference.\n", previousElement());
				doReturnToExpectToken();
				return true;
			}
		}
		else if (set_token == "[")
		{
			#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_REFERENCES) || defined(DEBUG_RAE_BRACKET)
				cout << "BRACKET we got a bracket in DEFINE_REFERENCE_NAME!\n";
			#endif

			if (previousElement())
			{
				if (previousElement()->token() == Token::DEFINE_REFERENCE)
				{
					expectingToken(Token::ARRAY_VECTOR_STUFF);
				}
				else
				{
					ReportError::reportError("A starting array bracket [ in a strange place. The element before is not a type. It is: " + previousElement()->toString(), previousElement());
				}
			}
		}
		else if (set_token == "]")
		{
			newBracketEnd(Token::BRACKET_DEFINE_ARRAY_END, set_token);
		}
		else if (set_token == "*")
		{
			//TODO pointer type...
			ReportError::reportError("TODO pointer type.", previousElement());
			newElement(Token::MULTIPLY, Kind::UNDEFINED, set_token);

			assert(0);
		}
		else if (set_token == "?")
		{
			//A huge TODO.
			ReportError::reportError("Huge todo on line 4535. handle null pointers: ????.", previousElement());
			//newElement(Token::MULTIPLY, set_token);
		}
		/*REMOVE
		else if( set_token == "!" )
		{
		cout<<"Got template!\n";

		if( previousElement() )
		{
		cout<<"Got previousElement:"<< previousElement()->toString()<<"\n";
		//TODO maybe we really need to check this:
		//if( previousElement()->token() == Token::DEFINE_REFERENCE )
		//{

		//change the typetype.
		previousElement()->kind(Kind::TEMPLATE);
		//}
		}
		expectingToken(Token::TEMPLATE_STUFF);
		}
		*/
		else if (currentReference)
		{
			//currentReference->name(set_token);
			setNameAndCheckForPreviousDefinitions(currentReference, set_token);

			if (expectingRole() == Role::FUNC_RETURN)
			{
				//Back to waiting for UNDEFINED as that handles the coming ")" the best.
				expectingToken(Token::UNDEFINED);
			}
			else if (currentClass != nullptr && currentFunc == nullptr && currentEnum == nullptr)//INSIDE_CLASS doesn't work currently: if(expectingRole() == Role::INSIDE_CLASS)
			{
				//THIRD of this text!
				expectingToken(Token::INIT_DATA);
				//expectingToken = Token::UNDEFINED;
				//doReturnToExpectToken();
			}
			else
			{
				expectingToken(Token::UNDEFINED);
			}
		}

	}
	else if (expectingToken() == Token::FUNC_DEFINITION)
	{
		#ifdef DEBUG_RAE_PARSER
			cout << "FUNC_DEFINITION handling START.\n";
		#endif

		if (set_token[0] == '\n')
		{
			#ifdef DEBUG_RAE_HUMAN
				cout << "Got NEWLINE. Ignore it because func.\n";
			#endif
			//expectingToken = Token::FUNC_RETURN_TYPE;
		}
		else if (set_token[0] == '(')
		{
			#ifdef DEBUG_RAE_PARSER
				cout << "Got func_def (. Waiting return_types.\n";
			#endif
			newParenthesisBegin(Token::PARENTHESIS_BEGIN_FUNC_RETURN_TYPES, "(");
			expectingRole(Role::FUNC_RETURN);

			//doReturnToExpectToken();//How about this change?
			expectingToken(Token::UNDEFINED);

			//expectingToken(Token::FUNC_RETURN_TYPE);
		}
		else if (set_token[0] == ')')
		{
			cout << ") in FUNC_DEFINITION is not supposed to happen.\n";
			assert(0);
		}
		else
		{
#ifdef DEBUG_RAE
			cout << "Maybe got func name: " << set_token << "\n";
#endif
			//record func name
			if (currentFunc)
			{
				#ifdef DEBUG_RAE_PARSER
					cout << "Got currentFunc name: " << set_token << "\n";
				#endif

				currentFunc->name(set_token);
				//setNameAndCheckForPreviousDefinitions(currentReference, set_token);

				/*if( set_token == "this" )
				{
				currentFunc->token( Token::CONSTRUCTOR );
				}
				else if( set_token == "~this" )
				{
				currentFunc->token( Token::DESTRUCTOR );
				}*/

				//if a funcs name is init, then it is a constructor.
				//Used to be new: if( set_token == "new" )
				if (set_token == "init")
				{
#ifdef DEBUG_RAE
					//rae::log("Got new. It's a CONSTRUCTOR.\n");
#endif
					currentFunc->token(Token::CONSTRUCTOR);
					listOfConstructors.push_back(currentFunc);
				}
				else if (set_token == "free")
				{
#ifdef DEBUG_RAE
					//rae::log("Got free. It's a DESTRUCTOR.\n");
#endif
					currentFunc->token(Token::DESTRUCTOR);
					listOfDestructors.push_back(currentFunc);
				}
				else if (set_token == "delete")
				{
					//#ifdef DEBUG_RAE
					ReportError::reportError("\"delete\" keyword is not used in the Rae programming language. Please use \"free\" instead.", previousElement());
					//rae::log("delete is deprecated. please use free instead.\n");
					//#endif
					currentFunc->token(Token::DESTRUCTOR);
					listOfDestructors.push_back(currentFunc);
				}
				else if (set_token == "main")
				{
#ifdef DEBUG_RAE_HUMAN
					cout << ("Got main. It's a MAIN.\n");
#endif
					currentFunc->token(Token::MAIN);
				}
			}
			else
				cout << "No currentFunc. Returning back to UNDEFINED and Role::FUNC_PARAMETER. set_token was: " << set_token <<"\n";
			//expectingToken(Token::FUNC_ARGUMENT_TYPE);
			expectingRole(Role::FUNC_PARAMETER);

			//doReturnToExpectToken();//How about this change?
			expectingToken(Token::UNDEFINED);
		}
	}
	/*
	else if( expectingToken() == Token::FUNC_RETURN_TYPE )
	{
	if( set_token[0] == ')' )
	{
	#ifdef DEBUG_RAE
	//rae::log("Got end func return types.>", set_token, "< Waiting rest of definition.\n");
	#endif
	//TODO: check parenthesisStack:
	newParenthesisEnd(Token::PARENTHESIS_END_FUNC_RETURN_TYPES, ")");
	expectingToken(Token::FUNC_DEFINITION);
	}
	else if( set_token[0] == ',' )//a comma separates the list of return types...
	{
	#ifdef DEBUG_RAE
	//rae::log("Got comma between return types.>", set_token, "< Waiting rest func_return_type.\n");
	#endif
	expectingToken(Token::FUNC_RETURN_TYPE);
	}
	else
	{
	#ifdef DEBUG_RAE
	//rae::log("Got func_return_type. Could be:>", set_token, "<.\n");
	#endif
	if(currentFunc)
	{
	newDefineFuncReturn(set_token);
	}
	expectingToken(Token::FUNC_RETURN_NAME);
	//record return type1, 2 , 3 etc.
	}
	}
	else if( expectingToken() == Token::FUNC_RETURN_NAME )
	{
	if( set_token[0] == ')' )
	{
	#ifdef DEBUG_RAE
	//rae::log("Got end func returns.>", set_token, "< Waiting rest of definition.\n");
	#endif
	//TODO: check parenthesisStack:
	newParenthesisEnd(Token::PARENTHESIS_END_FUNC_RETURN_TYPES, ")");
	expectingToken(Token::FUNC_DEFINITION);
	}
	else
	{
	#ifdef DEBUG_RAE
	//rae::log("Got func_return_name. Could be:>", set_token, "<.\n");
	#endif
	if(currentReference)
	{
	///////////////checkForPreviousDefinitions(set_token);
	//currentFunc->addNameToCurrentElement(set_token);
	currentReference->name(set_token);
	currentReference->isUnknownType(true);//rename to isUnknown? because the name is unknown here.
	addToCheckForPreviousDefinitionsList(currentReference);
	}
	expectingToken(Token::FUNC_RETURN_TYPE);
	//record return type1, 2 , 3 etc.
	}
	}
	else if( expectingToken() == Token::FUNC_ARGUMENT_TYPE )
	{
	if( set_token[0] == '(' )
	{
	#ifdef DEBUG_RAE
	//rae::log("Got param start (: >", set_token, "< Still waiting for params.\n");
	#endif
	//TODO: check parenthesisStack:
	newParenthesisBegin(Token::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES, "(");

	//finalize func
	//expectingToken = Token::FUNCTION_BODY;
	}
	else if( set_token[0] == ')' )
	{
	#ifdef DEBUG_RAE
	//rae::log("Got final ): >", set_token, "< Going for the func BODY.\n");
	#endif
	//finalize func
	//if( currentFunc )
	//{
	//currentFunc->newElement(lineNumber, Token::PARENTHESIS_END_FUNC_DEFINITION, ")");
	//}
	//TODO: check parenthesisStack:
	newParenthesisEnd(Token::PARENTHESIS_END_FUNC_PARAM_TYPES, ")");

	//expectingToken = Token::FUNCTION_BODY;
	//expectingToken = Token::UNDEFINED;
	doReturnToExpectToken();

	#ifdef DEBUG_RAE_HUMAN
	if( currentFunc )
	{
	cout<<"\nnewFunction: "<<currentFunc->name()<<"\n";
	//rae::log("newFunction: ", currentFunc->name(), "\n");
	}
	#endif
	}
	else if( set_token[0] == ',' )//a comma separates the list of param types...
	{
	#ifdef DEBUG_RAE
	//rae::log("Got comma between param types.>", set_token, "< Waiting rest FUNC_ARGUMENT_TYPE.\n");
	#endif
	//if( currentFunc )
	//{
	//currentFunc->newElement(lineNumber, Token::COMMA, ",");
	//}
	newElement(Token::COMMA, Kind::UNDEFINED, ",");
	expectingToken(Token::FUNC_ARGUMENT_TYPE);
	}
	else if(set_token == "override")
	{
	//if(currentFunc)
	//{
	//currentFunc->newElement(lineNumber, Token::OVERRIDE);
	//}
	newElement(Token::OVERRIDE, Kind::UNDEFINED, set_token);
	}
	else if( set_token == "vector" )//Ooh, it's a vector
	{
	#ifdef DEBUG_RAE
	//rae::log("Got comma between param types.>", set_token, "< Waiting rest FUNC_ARGUMENT_TYPE.\n");
	#endif
	//if(currentReference)
	//{
	//currentReference->kind(Kind::C_ARRAY);
	//}
	cout<<"TODO enable vector FUNC_ARGUMENTs.\n";
	//returnToExpect( Token::FUNC_ARGUMENT_NAME );
	//expectingToken( Token::VECTOR_STUFF );
	}
	else if( set_token == "!" )//Ooh, it's a vector
	{
	//ignore
	//Hmm, !() is going to get totally wrong. It's going to end func_def and all that.
	cout<<"Ok. Got !. !() is going to go so bad.\n";
	}
	else
	{
	#ifdef DEBUG_RAE
	//rae::log("Got func param type: >", set_token, "<.\n");
	#endif
	if(currentFunc)
	{
	//replace these with a new func:
	newDefineFuncArgument(set_token);

	//currentFunc->newElement(lineNumber, Token::DEFINE_FUNC_ARGUMENT);
	//currentFunc->addTypeToCurrentElement(set_token);

	//					if( BuiltInType::isBuiltInType(set_token) )
	//				{
	//				//ok.
	//		}
	//	else
	//{
	//Element* found_elem = searchToken(set_token);
	//if( found_elem )
	//{
	//
	//}
	//else
	//{
	//	currentFunc->currentElement()->isUnknownType(true);
	//	addToUnknownDefinitions( currentFunc->currentElement() );
	//}
	//}

	}
	expectingToken(Token::FUNC_ARGUMENT_NAME);
	//record return type1, 2 , 3 etc.
	}
	}
	else if( expectingToken() == Token::FUNC_ARGUMENT_NAME )
	{
	cout<<"Remove this FUNC_ARGUMENT thing, and just use parent() which is scope, to know if it's inside a func def. Use DEFINE_REFERENCE for them instead.\n";
	assert(0);

	if( set_token[0] == '(' )
	{
	//rae::log("RAE_ERROR: ");
	lineNumber.printOut();
	//rae::log(" Got param start, while expecting FUNC_ARGUMENT_NAME (: >", set_token, "< Still waiting for params.\n");

	//TODO: check parenthesisStack:
	newParenthesisBegin(Token::PARENTHESIS_BEGIN, "(");

	//finalize func
	//expectingToken = Token::FUNCTION_BODY;
	}
	else if( set_token[0] == ')' )
	{
	#ifdef DEBUG_RAE
	//rae::log("Got final ), func_param not named: >", set_token, "< Going for the func BODY.\n");
	#endif
	//finalize func
	//if( currentFunc )
	//{
	//	currentFunc->newElement(lineNumber, Token::PARENTHESIS_END_FUNC_DEFINITION, ")");
	//}

	//TODO: check parenthesisStack:
	newParenthesisEnd(Token::PARENTHESIS_END_FUNC_PARAM_TYPES, ")");

	//expectingToken = Token::FUNCTION_BODY;
	//expectingToken = Token::UNDEFINED;
	doReturnToExpectToken();
	}
	else if( set_token[0] == ',' )//a comma separates the list of param types...
	{
	#ifdef DEBUG_RAE
	//rae::log("Got comma between param types.>", set_token, "< Waiting rest FUNC_ARGUMENT_TYPE.\n");
	#endif
	newElement(Token::COMMA, Kind::UNDEFINED, ",");
	expectingToken(Token::FUNC_ARGUMENT_TYPE);
	}
	else if( set_token[0] == '[' )//Ooh, it's an array...
	{
	#ifdef DEBUG_RAE
	//rae::log("Got comma between param types.>", set_token, "< Waiting rest FUNC_ARGUMENT_TYPE.\n");
	#endif
	if(currentReference)
	{
	expectingToken(Token::ARRAY_VECTOR_STUFF);
	//cout<<"Got a bracket. TODO something with it. VECTOR\n";
	//assert(0);
	//currentReference->kind(Kind::C_ARRAY);
	}
	}
	else if( set_token[0] == ']' )//just ignore...
	{
	#ifdef DEBUG_RAE
	//rae::log("Got comma between param types.>", set_token, "< Waiting rest FUNC_ARGUMENT_TYPE.\n");
	#endif
	//ignore
	ReportError::reportError("An ending array bracket ] in a strange place. Element before: " + previousElement()->toString() );
	}
	else if( set_token[0] == '=' )//ooh, we are getting init_data...
	{
	#ifdef DEBUG_RAE
	//rae::log("init data.>", set_token, ".\n");
	cout<<"GETTING INIT_DATA!!!";
	#endif
	if(currentReference)
	{
	//currentReference->kind(Kind::C_ARRAY);
	}


	addReturnToExpectToken( Token::FUNC_ARGUMENT_NAME );
	expectingToken( Token::INIT_DATA );
	}
	else
	{
	#ifdef DEBUG_RAE
	//rae::log("Got func param name: >", set_token, "<.\n");
	#endif
	if(currentReference)
	{
	//We'll have to check these after the class is done.
	//to check for need for override keyword.

	/////////checkForPreviousDefinitions(set_token);
	//currentFunc->addNameToCurrentElement(set_token);
	currentReference->name(set_token);
	currentReference->isUnknownType(true);//rename to isUnknown? because the name is unknown here.
	addToCheckForPreviousDefinitionsList(currentReference);
	}
	//expectingToken = Token::FUNC_ARGUMENT_TYPE;
	//record return type1, 2 , 3 etc.
	}

	}
	else
	{
	//rae::log("RAE_ERROR: ");
	lineNumber.printOut();
	//rae::log(" token: ", set_token, " expecting: ", Token::toString(expectingToken), " lineText: ", currentLine, "\n");
	//expectingToken = Token::UNDEFINED;
	doReturnToExpectToken();
	}
	*/

	return false;
}

} // end namespace Rae