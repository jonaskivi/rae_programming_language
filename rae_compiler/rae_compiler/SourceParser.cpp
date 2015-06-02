#include "SourceParser.hpp"

#include "LangCompiler.hpp"

namespace Rae
{

LangElement* SourceParser::newDefineVector(string set_template_second_type, string set_name)
{
	#ifdef DEBUG_RAE_HUMAN
	cout<<"newDefineVector() : set_template_second_type: "<<set_template_second_type<<" set_name: "<<set_name<<" line: "<<lineNumber.line<<"\n";
	#endif

	//LangElement* lang_elem = newLangElement(Token::DEFINE_VECTOR, set_name, set_type);
	LangElement* lang_elem = newLangElement(Token::DEFINE_REFERENCE, TypeType::VECTOR, set_name, "vector");
	//lang_elem->typeType(TypeType::VECTOR);
	currentTemplate = lang_elem;//Oh, remove this.
	currentReference = lang_elem;

	//Hmm. This is a bit silly and slow, but we'll try to find the vector from our stdlib modules:
	//LangElement* stdlib_vector_element = *searchElementInOtherModulesSignal(this, lang_elem);
	LangElement* stdlib_vector_element = g_compiler->searchElementInOtherModules(this, lang_elem);
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

LangElement* SourceParser::searchElementAndCheckIfValid(LangElement* set_elem)
{
	LangElement* result = searchElementAndCheckIfValidLocal(set_elem);

	if (result)
		return result;

	//Then search in other modules outside this SourceParser.
	//result = *searchElementInOtherModulesSignal(this, set_elem);
	result = g_compiler->searchElementInOtherModules(this, set_elem);

	return result;
}

void SourceParser::handleToken(string set_token)
{
	if (set_token == "")//ignore empty tokens...
	{
		return;
	}

	//We catch all tokens when doing PRAGMA_CPP until we get a website address
	//such as: forsaken_woods@endisnear.com
	//Oh. @Pragmas need to end with space or enter, so that might handle that case.
	if (expectingToken() == Token::PRAGMA_CPP)
	{
		if (set_token == "@end")
		{
			isPassthroughMode = false;
			newLangElement(Token::PRAGMA_CPP_END, TypeType::UNDEFINED, set_token);
			doReturnToExpectToken();
		}
		else
		{
			newLangElement(Token::PASSTHROUGH_HDR, TypeType::UNDEFINED, set_token);
		}
		return;
	}
	else if (expectingToken() == Token::PRAGMA_CPP_SRC)
	{
		if (set_token == "@end")
		{
			isPassthroughSourceMode = false;
			newLangElement(Token::PRAGMA_CPP_END, TypeType::UNDEFINED, set_token);
			doReturnToExpectToken();
		}
		else
		{
			newLangElement(Token::PASSTHROUGH_SRC, TypeType::UNDEFINED, set_token);
		}
		return;
	}

	if (set_token == "\t")
	{
		return;
	}
	else if (set_token == " ")
	{
		return;
	}
	/*else if( set_token == "\n" )
	{
	return;
	}*/

	if (set_token == "+/")
	{
		if (isPlusCommentReady == true)
		{
			newPlusComment(currentPlusComment);
			isPlusCommentReady = false;
			return;
		}
	}

	if (set_token == "*/")
	{
		if (isStarCommentReady == true)
		{
			newStarComment(currentStarComment);
			isStarCommentReady = false;
			return;
		}
	}

	if (set_token == "//")
	{
		//expectingToken = Token::COMMENT_LINE_END;
		isSingleLineComment = true;
		currentLine = "//";
		//cout<<"Waiting for comment line to end.\n";
		return;
	}

	//

	if (set_token == "\"")
	{
		if (isQuoteReady == true)
		{
			isQuoteReady = false;

			if (expectingToken() == Token::INIT_DATA)
			{
				if (currentReference)
				{
#ifdef DEBUG_RAE_HUMAN
					cout << "string init_data: " << currentQuote << " Special handling for newQuote. INIT_DATA.\n";
#endif
					//special handling for quote i.e. string initData.
					LangElement* in_dat = newLangElement(Token::INIT_DATA, TypeType::UNDEFINED, currentQuote);
					currentReference->initData(in_dat);
					doReturnToExpectToken();
					return;
				}
			}
			else
			{
				newQuote(currentQuote);
				return;
			}
		}
	}

	if (expectingToken() == Token::INIT_DATA)
	{
		if (set_token == "=" || set_token == "->") //TODO better checking for point to only pointing to pointer types like null...
		{
			LangElement* prev_elem = previousElement();

			//create a init_data element and continue waiting for the actual data!
			LangElement* in_dat = newLangElement(Token::INIT_DATA, TypeType::UNDEFINED, set_token);
			//currentReference->initData(in_dat);
			if (prev_elem)
			{
				prev_elem->initData(in_dat);
#ifdef DEBUG_RAE_HUMAN
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
		else if (set_token == "\n")
		{
			//No initdata. Set to default init_data
			//currentReference->addDefaultInitData(); //OK. Don't do this. Adding auto-init data is dependant on not having this.
			if (currentParentElement() && currentParentElement()->token() == Token::INIT_DATA)
			{
				endInitData();//This is important. This ends our init_data being the currentParentElement (which receives the init_data.)
			}
			else if (currentReference)
			{
				currentReference->addDefaultInitData();
			}
			doReturnToExpectToken();
			newLine();
		}
		else if (set_token == ")")
		{
			if (expectingRole() == Role::FUNC_RETURN)
			{
#ifdef DEBUG_RAE_PARSER
				cout << "Got ending ) for FUNC_RETURNs (in INIT_DATA). Going back to FUNC_DEFINITION.\n";
#endif

				expectingRole(Role::UNDEFINED);//could also be Role::FUNC_DEFINITION, but we don't need that, so.
				//TODO: check parenthesisStack:
				newParenthesisEnd(Token::PARENTHESIS_END_FUNC_RETURN_TYPES, ")");
				expectingToken(Token::FUNC_DEFINITION);
			}
			else if (expectingRole() == Role::FUNC_PARAMETER)
			{
#ifdef DEBUG_RAE_PARSER
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

				newLangElement(Token::COMMA, TypeType::UNDEFINED, ",");
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

		if (set_token == ".")
		{
			if (previousElement() && previousToken() == Token::NUMBER)
			{
				//newLangElement(Token::DOT, set_token);
				previousElement()->token(Token::FLOAT_NUMBER);
				previousElement()->name(previousElement()->name() + set_token);
				expectingToken(Token::DECIMAL_NUMBER_AFTER_DOT);
				//expectingRole(Role::INIT_DATA);

			}
			else
			{
				ReportError::reportError("A strange dot while waiting for INIT_DATA. Floating point numbers are written: 0.0", previousElement());
				newLangElement(Token::REFERENCE_DOT, TypeType::UNDEFINED, set_token);
			}
		}
		else if (set_token == ";")
		{
			if (currentParentElement() && currentParentElement()->token() == Token::INIT_DATA)
			{
				endInitData();//This is important. This ends our init_data being the currentParentElement (which receives the init_data.)
			}
			else
			{
				ReportError::reportError("Waiting for INIT_DATA: Got SEMICOLON but there was no INIT_DATA element.", previousElement());
				//TODO ReportError::additionalInfo("currentParentElement: ", currentParentElement() );
				cout << "currentParentElement: ";
				if (currentParentElement()) cout << currentParentElement()->toString() << "\n"; else cout << "nullptr" << "\n";

				doReturnToExpectToken();
			}

			newLangElement(Token::SEMICOLON, TypeType::UNDEFINED, set_token);
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
				endInitData();//This is important. This ends our init_data being the currentParentElement (which receives the init_data.)
			}
			else
			{
				ReportError::reportError("Waiting for INIT_DATA: Got NEWLINE but there was no INIT_DATA element.", previousElement());
				//TODO ReportError::additionalInfo("currentParentElement: ", currentParentElement() );
				cout << "currentParentElement: ";
				if (currentParentElement()) cout << currentParentElement()->toString() << "\n"; else cout << "nullptr" << "\n";

				doReturnToExpectToken();
			}

			newLine();
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
				//WHY does this set the token of an already existing langElement?????????? TODO
				currentReference->token(Token::INFO_FUNC_PARAM);
			}
			//expectingToken = Token::UNDEFINED;
			doReturnToExpectToken();
		}
		else if (set_token == "new")
		{
#ifdef DEBUG_RAE_HUMAN
			cout << "TODO new() INIT_DATA.\n";
			//rae::log("TODO new() INIT_DATA.\n");
#endif
			//TODO error on returnToExpectToken FUNC_ARGUMENT_NAME, no new here.
			//currentReference->initData(set_token);
			//expectingToken = Token::UNDEFINED; //TEMP
			doReturnToExpectToken();
		}
		else if (set_token == "null")
		{
#ifdef DEBUG_RAE_HUMAN
			cout << "ACTUAL_INIT_DATA: Got null initdata.\n";
#endif

			if (currentParentElement() && currentParentElement()->token() == Token::INIT_DATA)
			{
				//cout<<"And created null initdata for element: "<<dang->toString()<<"\n";
				LangElement* in_dat = newLangElement(Token::RAE_NULL, TypeType::UNDEFINED, set_token);
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
					//don't do anything. We've done it already!
				}
				else
				{
					//we should have a handleInitData() func!
					//newLangElement(Token::STRING, TypeType::UNDEFINED, set_token); //Hmm STRINGs like "a string" are Token::QUOTES.
					cout << "TODO better handling of init_data in initialization. " << set_token << "\n";
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
			LangElement* in_dat = newLangElement(Token::INIT_DATA, TypeType::UNDEFINED, set_token);
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
			LangElement* in_dat = previousElement()->copy();
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
	else if (expectingToken() == Token::IMPORT_NAME)
	{
		if (set_token[0] == '.')
		{
			expectingToken(Token::IMPORT_NAME);
		}
		else if (set_token == "\n")
		{
#ifdef DEBUG_RAE
			cout << "Line change. End of IMPORT_NAME.\n";
			//rae::log("Line change. End of IMPORT_NAME.\n");
#endif
			if (currentTempElement)
			{
				//currentModuleHeaderFileName = currentTempElement->searchLast(Token::IMPORT_NAME)->name();
#ifdef DEBUG_RAE
				//	cout<<"last module name was: "<<currentModuleHeaderFileName;
#endif

				/*
				string a_import_whole_name = "";

				int not_on_first = 0;

				for(LangElement* elem : currentTempElement->langElements)
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
				break;//break the for... hope this works.
				}
				}
				*/

				string a_import_whole_name = currentTempElement->importName("/");//a bit of a hack...
				//and the func that it is calling replace . with /

#ifdef DEBUG_RAE_HUMAN
				cout << "newImport: ";
				//rae::log("newImport: ");
				cout << a_import_whole_name;
				cout << "\n";
				//rae::log("\n");
#endif

				listOfImports.push_back(currentTempElement);

				//newImportSignal(a_import_whole_name);
				g_compiler->addSourceFileAsImport(a_import_whole_name);

			}

			newLine();
			//expectingToken = Token::UNDEFINED;
			doReturnToExpectToken();
			currentTempElement = 0;
		}
		else
		{
			if (currentTempElement)
				currentTempElement->newLangElement(lineNumber, Token::IMPORT_NAME, TypeType::UNDEFINED, set_token);

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
#ifdef DEBUG_RAE
			cout << "Line change. End of MODULE_NAME.\n";
			//rae::log("Line change. End of MODULE_NAME.\n");
#endif
			if (currentModule)
			{
				currentModuleHeaderFileName = currentModule->searchLast(Token::MODULE_NAME)->name();
#ifdef DEBUG_RAE
				cout << "last module name was: " << currentModuleHeaderFileName;
				//rae::log("last module name was: ", currentModuleHeaderFileName);
#endif

#ifdef DEBUG_RAE_HUMAN
				cout << "\n\nnewModule: ";
				//rae::log("newModule: ");
				for (LangElement* elem : currentModule->langElements)
				{
					cout << elem->name() << ".";
					//rae::log(elem->name(), ".");
				}
				cout << "\n";
				//rae::log("\n");
#endif

				currentModule->name(moduleName());//Hmm. Using a very strange function we put the module name also
				//to the name variable.

			}

			newLine();
			//expectingToken(Token::UNDEFINED);
			doReturnToExpectToken();
		}
		else
		{
			if (currentModule)
				currentModule->newLangElement(lineNumber, Token::MODULE_NAME, TypeType::UNDEFINED, set_token);

			//expectingToken(Token::MODULE_NAME);
		}
	}
	/*
	else if( set_token == "\n" )
	{
	//How about this: expectingToken = Token::UNDEFINED;
	if( currentParentElement() )
	{
	currentParentElement()->newLangElement(Token::NEWLINE, set_token);
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
	else if (expectingToken() == Token::DECIMAL_NUMBER_AFTER_DOT)
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

			if (isReceivingInitData == true)
			{
				expectingToken(Token::ACTUAL_INIT_DATA);
			}
			else
			{
				doReturnToExpectToken();
			}
		}
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
	previousElement()->typeType() == TypeType::C_ARRAY
	//|| previousElement()->token() == TypeType::DEFINE_ARRAY_IN_CLASS)
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



					//previousElement()->typeType(TypeType::VECTOR);
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
				//currentClass->newLangElement(lineNumber, Token::TEMPLATE_SECOND_TYPE, TypeType::TEMPLATE/*are you sure this is not confusing...*/, /*string set_name:*/ "undefined", /*string set_type:*/ set_token );
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
				//currentReference->newLangElement(lineNumber, Token::TEMPLATE_SECOND_TYPE, TypeType::TEMPLATE/*are you sure this is not confusing...*/, /*string set_name:*/ "undefined", /*string set_type:*/ set_token );
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
	else if (expectingToken() == Token::EXPECTING_NAME)
	{
		setNameForExpectingName(set_token);
	}
	else if (expectingToken() == Token::UNDEFINED)
	{
		//reserved words:


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


		if (set_token == "\n")
		{
			newLine();
		}
		else if (set_token == ";")
		{
			newLangElement(Token::SEMICOLON, TypeType::UNDEFINED, set_token);
		}
		else if (set_token == "{")
		{
			newScopeBegin();
		}
		else if (set_token == "}")
		{
			newScopeEnd();
		}
		else if (set_token == "func")
		{
#ifdef DEBUG_RAE
			cout << "Got func. Waiting func_definition.\n";
			//rae::log("Got func. Waiting func_definition.\n");
#endif
			expectingToken(Token::FUNC_DEFINITION);
			newFunc();
		}
		else if (set_token == ".")
		{
			if (previousElement() && previousToken() == Token::NUMBER)
			{
				//newLangElement(Token::DOT, set_token);
				previousElement()->token(Token::FLOAT_NUMBER);
				previousElement()->name(previousElement()->name() + set_token);
				expectingToken(Token::DECIMAL_NUMBER_AFTER_DOT);
			}
			else
			{
				newLangElement(Token::REFERENCE_DOT, TypeType::UNDEFINED, set_token);
			}
		}
		else if (set_token == ",")
		{
			/*
			LangElement* stack_elem;
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
				LangElement* our_array_definition = bracketStack.back();
				if (our_array_definition->token() == Token::BRACKET_DEFINE_ARRAY_BEGIN)
				{
					our_array_definition->token(Token::BRACKET_DEFINE_STATIC_ARRAY_BEGIN);
				}
				newLangElement(Token::COMMA, TypeType::UNDEFINED, ",");
			}
			else if (expectingRole() == Role::FUNC_RETURN)
			{
				//TODO: Hmm. These are the same as normal case. So we could just remove the expectingRoles here...
				newLangElement(Token::COMMA, TypeType::UNDEFINED, ",");
			}
			else if (expectingRole() == Role::FUNC_PARAMETER || expectingRole() == Role::TEMPLATE_PARAMETER)
			{
				newLangElement(Token::COMMA, TypeType::UNDEFINED, ",");
			}
			else //Role::UNDEFINED or something. Normal function body etc.
			{
				Token::e parenthesis_type = parenthesisStackTokenType();

				if (parenthesis_type == Token::PARENTHESIS_BEGIN_LOG || parenthesis_type == Token::PARENTHESIS_BEGIN_LOG_S)
				{
					newLangElement(Token::LOG_SEPARATOR, TypeType::UNDEFINED, set_token);//it is still a comma "," but we write it out as << for C++
				}
				else
				{
					newLangElement(Token::COMMA, TypeType::UNDEFINED, set_token);
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
				LangElement* our_ref = newDefineReference(set_token, expectingRole());
				unfinishedElement(our_ref);
			}

		}
		else if (set_token == "bool"
			|| set_token == "byte"
			|| set_token == "ubyte"
			|| set_token == "char"
			|| set_token == "wchar"
			|| set_token == "dchar"
			|| set_token == "int"
			|| set_token == "uint"
			|| set_token == "long"
			|| set_token == "ulong"
			|| set_token == "float"
			|| set_token == "double"
			//|| set_token == "real"
			|| set_token == "string"
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

			LangElement* our_ref;
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
			expectingToken(Token::VECTOR_STUFF);
			newDefineVector("unknown", "unknown");
		}
		else if (set_token == "(")
		{
			if (previousToken() == Token::CLASS)
			{
#ifdef DEBUG_RAE_HUMAN
				cout << "( means template class.\n";
#endif
				//a simple class template
				previousElement()->typeType(TypeType::TEMPLATE);
				expectingToken(Token::CLASS_TEMPLATE_SECOND_TYPE);
			}
			else if (expectingRole() == Role::FUNC_PARAMETER)
			{
				//TODO: check parenthesisStack:
				newParenthesisBegin(Token::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES, "(");
			}
			else //normal (
			{
				//cout<<"normal (\n";
				//if(previousElement())
				//	cout<<"previousToken: "<<previousElement()->toString();
				//else cout<<"no previousElement!\n";
				newParenthesisBegin(Token::PARENTHESIS_BEGIN, set_token);
			}
		}
		else if (set_token == ")")
		{
			if (expectingRole() == Role::FUNC_RETURN)
			{
#ifdef DEBUG_RAE_PARSER
				cout << "Got ending ) for FUNC_RETURNs in UNDEFINED. Going back to FUNC_DEFINITION.\n";
#endif				

				expectingRole(Role::UNDEFINED);//could also be Role::FUNC_DEFINITION, but we don't need that, so.
				//TODO: check parenthesisStack:
				newParenthesisEnd(Token::PARENTHESIS_END_FUNC_RETURN_TYPES, ")");
				expectingToken(Token::FUNC_DEFINITION);
			}
			else if (expectingRole() == Role::FUNC_PARAMETER)
			{
#ifdef DEBUG_RAE_PARSER
				cout << "Got ending ) for FUNC_PARAMETERs in UNDEFINED. Going back to UNDEFINED.\n";
#endif

				expectingRole(Role::UNDEFINED);//could also be Role::FUNC_DEFINITION, but we don't need that, so.
				//TODO: check parenthesisStack:
				newParenthesisEnd(Token::PARENTHESIS_END_FUNC_PARAM_TYPES, ")");
				expectingToken(Token::UNDEFINED);
			}
			else
			{
				//cout<<"normal )\n";
				newParenthesisEnd(Token::PARENTHESIS_END, set_token);
			}
		}
		else if (set_token == "[")
		{
			//cout<<"BRACKET we got a bracket in the normal place and it is strange!\n";

			//cout<<"Wise people say, this should never get called. SourceParser line 4398.\n";
			//assert(0);

			if (previousElement() && (previousElement()->isUnknownType() || previousElement()->token() == Token::USE_REFERENCE))
			{
				newBracketBegin(Token::BRACKET_BEGIN, set_token);
			}
			else if (previousElement() && previousElement()->token() == Token::DEFINE_REFERENCE)
			{
				//convert to array:
				previousElement()->token(Token::BRACKET_DEFINE_ARRAY_BEGIN);
				currentParentElement(previousElement());
				previousElement()->containerType(ContainerType::ARRAY);
				previousElement()->type("array");
				bracketStack.push_back(previousElement());
			}
			else
			{
				newDefineArray();
			}



			//expectingContainer( ContainerType::ARRAY );
			//tanne not a good idea: we need to handle opt, val, bool, int etc. which are all handled in undefined.//expectingToken(Token::ARRAY_VECTOR_STUFF);

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
			previousElement()->typeType(TypeType::C_ARRAY);
			expectingToken(Token::DEFINE_C_ARRAY_NAME);
			*/
			//cout<<"A bracket but we didn't have a DEFINE_REFERENCE? Maybe it is an error.\n";
			/*not up to date, as it won't be called...
			previousElement()->typeType(TypeType::VECTOR);
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
#ifdef DEBUG_RAE_PARSER
			cout << "normal newBracketEnd.";
#endif
			LangElement* our_array_definition;
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
			//newLangElement(Token::POINT_TO, TypeType::UNDEFINED, set_token);
			newPointToElement();
		}
		//operators:
		else if (set_token == "=")
		{
			if (previousElement() && previousElement()->name() == "=")
				previousElement()->token(Token::EQUALS);
			else if (previousElement() && previousElement()->name() == "!")
				previousElement()->token(Token::NOT_EQUAL);
			else if (previousElement() && previousElement()->name() == ">")
				previousElement()->token(Token::GREATER_THAN_OR_EQUAL);
			else if (previousElement() && previousElement()->name() == "<")
				previousElement()->token(Token::LESS_THAN_OR_EQUAL);
			else
				newLangElement(Token::ASSIGNMENT, TypeType::UNDEFINED, set_token);
		}
		else if (set_token == "!" || set_token == "not")
		{
			newLangElement(Token::NOT, TypeType::UNDEFINED, set_token);
		}
		else if (set_token == "&&" || set_token == "and")
		{
			newLangElement(Token::AND, TypeType::UNDEFINED, set_token);
		}
		else if (set_token == "||" || set_token == "or")
		{
			newLangElement(Token::OR, TypeType::UNDEFINED, set_token);
		}
		else if (set_token == "+")
		{
			if (previousElement() && previousElement()->name() == "+")
				previousElement()->token(Token::OPERATOR_INCREMENT);
			else
				newLangElement(Token::PLUS, TypeType::UNDEFINED, set_token);
		}
		else if (set_token == "-")
		{
			if (previousElement() && previousElement()->name() == "-")
				previousElement()->token(Token::OPERATOR_DECREMENT);
			else
				newLangElement(Token::MINUS, TypeType::UNDEFINED, set_token);
		}
		else if (set_token == "*")
		{
			newLangElement(Token::MULTIPLY, TypeType::UNDEFINED, set_token);
		}
		else if (set_token == "/")
		{
			newLangElement(Token::DIVIDE, TypeType::UNDEFINED, set_token);
		}
		else if (set_token == "%")
		{
			newLangElement(Token::MODULO, TypeType::UNDEFINED, set_token);
		}
		else if (set_token == "<")
		{
			newLangElement(Token::LESS_THAN, TypeType::UNDEFINED, set_token);
		}
		else if (set_token == ">")
		{
			newLangElement(Token::GREATER_THAN, TypeType::UNDEFINED, set_token);
		}
		else if (set_token == "\"")
		{
			//shouldn't happen.
			//newLangElement(Token::MULTIPLY, set_token);
		}
		else if (set_token == "*/")
		{
			//newLangElement(Token::DIVIDE, set_token);
		}
		else if (set_token == "/*")
		{
			//newLangElement(Token::DIVIDE, set_token);
		}
		else if (set_token == "if")
		{
			newLangElement(Token::IF, TypeType::UNDEFINED, set_token);
		}
		else if (set_token == "loop")
		{
			newLangElement(Token::FOR, TypeType::UNDEFINED, set_token);
		}
		else if (set_token == "for")
		{
			cout << "RAE_ERROR for is deprecated!\n";
		}
		/*
		else if( set_token == "foreach" )
		{
		newLangElement(Token::FOR_EACH, TypeType::UNDEFINED, set_token);
		}*/
		else if (set_token == "in")
		{
			newLangElement(Token::IN_TOKEN, TypeType::UNDEFINED, set_token);
		}
		else if (set_token == "log_s")
		{
			//isInsideLogStatement = true;
			newLangElement(Token::LOG_S, TypeType::UNDEFINED, set_token);
			expectingToken(Token::PARENTHESIS_BEGIN_LOG_S);

		}
		else if (set_token == "log")
		{
			//isInsideLogStatement = true;
			newLangElement(Token::LOG, TypeType::UNDEFINED, set_token);
			expectingToken(Token::PARENTHESIS_BEGIN_LOG);
		}
		else if (set_token == "true")
		{
			newLangElement(Token::TRUE_TRUE, TypeType::UNDEFINED, set_token);
		}
		else if (set_token == "false")
		{
			newLangElement(Token::FALSE_FALSE, TypeType::UNDEFINED, set_token);
		}
		else if (set_token == "null")
		{
			newLangElement(Token::RAE_NULL, TypeType::UNDEFINED, set_token);
		}
		else if (set_token == "is")
		{
			newLangElement(Token::IS, TypeType::UNDEFINED, set_token);
		}
		/*
		//These don't work anymore as : is now it's own token. We must wait for it...
		else if( set_token == "public:" )
		{
		newLangElement(Token::VISIBILITY_DEFAULT, set_token);
		}
		else if( set_token == "protected:" )
		{
		newLangElement(Token::VISIBILITY_DEFAULT, set_token);
		}
		else if( set_token == "library:" )
		{
		newLangElement(Token::VISIBILITY_DEFAULT, set_token);
		}
		else if( set_token == "private:" )//what's the real use of private, anyway?
		{
		newLangElement(Token::VISIBILITY_DEFAULT, set_token);
		}
		*/
		/*//TODO errors for these old LONG visibility keywords:
		else if( set_token == "public" )
		{
		newLangElement(Token::VISIBILITY, TypeType::UNDEFINED, set_token );
		}
		else if( set_token == "protected" )
		{
		newLangElement(Token::VISIBILITY, TypeType::UNDEFINED, set_token);
		}
		else if( set_token == "library" )
		{
		//TODO
		newLangElement(Token::VISIBILITY, TypeType::UNDEFINED, set_token);
		}
		else if( set_token == "hidden" )
		{
		//TODO
		newLangElement(Token::VISIBILITY, TypeType::UNDEFINED, set_token);
		}
		else if( set_token == "private" )//what's the real use of private, anyway?
		{
		newLangElement(Token::VISIBILITY, TypeType::UNDEFINED, set_token);
		}
		*/
		else if (set_token == "let")
		{
			newLangElement(Token::LET, TypeType::UNDEFINED, set_token);
		}
		else if (set_token == "mut")
		{
			newLangElement(Token::MUT, TypeType::UNDEFINED, set_token);
		}
		else if (set_token == "pub")
		{
			newLangElement(Token::VISIBILITY, TypeType::UNDEFINED, set_token);
		}
		else if (set_token == "lib")
		{
			//TODO
			newLangElement(Token::VISIBILITY, TypeType::UNDEFINED, set_token);
		}
		else if (set_token == "priv") // -> protected
		{
			newLangElement(Token::VISIBILITY, TypeType::UNDEFINED, set_token);
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
		previousElement()->typeType(TypeType::TEMPLATE);
		//}
		}
		expectingToken(Token::TEMPLATE_STUFF);
		}
		*/
		else if (set_token == "return")
		{
#ifdef DEBUG_RAE
			cout << "Got return.\n";
			//rae::log("Got return.\n");
#endif
			//expectingToken = Token::PARENTHESIS_BEGIN_RETURN;
			newLangElement(Token::RETURN, TypeType::UNDEFINED, set_token);
		}
		else if (set_token == "new")
		{
			cout << "TODO Got new. Waiting new_class.\n";
			//rae::log("TODO Got new. Waiting new_class.\n");
			//expectingToken = Token::FUNC_DEFINITION;
			//newFunc();
		}
		else if (set_token == "free")
		{
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
		else if (set_token == "override")
		{
			newLangElement(Token::OVERRIDE, TypeType::UNDEFINED, set_token);
		}
		else if (set_token == "class")
		{
			////rae::log("found WORD class:>", set_token, "<\n");
			expectingToken(Token::CLASS_NAME);
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
		//try to automatically figure out where to put this c++ code. Usually it goes to headers. Inside funcs it goes to cpp files.
		else if (set_token == "@c++")
		{
			isPassthroughMode = true;
			//cout<<"To PASSTHROUGH_HDR mode.\n";
			newLangElement(Token::PRAGMA_CPP, TypeType::UNDEFINED, set_token);
			expectingToken(Token::PRAGMA_CPP);
		}
		//copy to c++ header .hpp:
		else if (set_token == "@c++hpp")
		{
			isPassthroughMode = true;
			//cout<<"To PASSTHROUGH_HDR mode.\n";
			newLangElement(Token::PRAGMA_CPP_HDR, TypeType::UNDEFINED, set_token);
			expectingToken(Token::PRAGMA_CPP_HDR);
		}
		//copy to c++ source file .cpp:
		else if (set_token == "@c++cpp")
		{
			isPassthroughSourceMode = true;
			//cout<<"To PASSTHROUGH_HDR mode.\n";
			newLangElement(Token::PRAGMA_CPP_SRC, TypeType::UNDEFINED, set_token);
			expectingToken(Token::PRAGMA_CPP_SRC);
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
		newLangElement(Token::SEMICOLON, set_token);
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
		newLangElement(Token::REFERENCE_DOT, set_token);
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
		newLangElement(Token::INT, set_token);
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
		newLangElement(Token::PLUS, set_token);
		}
		else if( set_token == "-" )
		{
		newLangElement(Token::MINUS, set_token);
		}
		else if( set_token == "=" )
		{
		newLangElement(Token::ASSIGNMENT, set_token);
		}
		else if( set_token == "/" )
		{
		newLangElement(Token::DIVIDE, set_token);
		}
		else if( set_token == "if" )
		{
		newLangElement(Token::IF, set_token);
		}
		else if( set_token == "log_s" )
		{
		newLangElement(Token::LOG_S, set_token);
		expectingToken = Token::PARENTHESIS_BEGIN_LOG_S;
		}
		else if( set_token == "log" )
		{
		newLangElement(Token::LOG, set_token);
		expectingToken = Token::PARENTHESIS_BEGIN_LOG;
		}
		else if( set_token == "public:" )
		{
		newLangElement(Token::VISIBILITY_DEFAULT, set_token);
		}
		else if( set_token == "protected:" )
		{
		newLangElement(Token::VISIBILITY_DEFAULT, set_token);
		}
		else if( set_token == "library:" )
		{
		newLangElement(Token::VISIBILITY_DEFAULT, set_token);
		}
		else if( set_token == "private:" )//what's the real use of private, anyway?
		{
		newLangElement(Token::VISIBILITY_DEFAULT, set_token);
		}
		else if( set_token == "public" )
		{
		newLangElement(Token::VISIBILITY, set_token);
		}
		else if( set_token == "protected" )
		{
		newLangElement(Token::VISIBILITY, set_token);
		}
		else if( set_token == "library" )
		{
		newLangElement(Token::VISIBILITY, set_token);
		}
		else if( set_token == "private" )//what's the real use of private, anyway?
		{
		newLangElement(Token::VISIBILITY, set_token);
		}
		else if( set_token == "return" )
		{
		#ifdef DEBUG_RAE
		//rae::log("Got return.\n");
		#endif
		//expectingToken = Token::PARENTHESIS_BEGIN_RETURN;
		newLangElement(Token::RETURN, set_token);
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
		newLangElement(Token::FREE, TypeType::UNDEFINED, set_token);
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
				else if (currentClass != nullptr && currentFunc == nullptr && currentEnum == nullptr)//INSIDE_CLASS doesn't work currently: if(expectingRole() == Role::INSIDE_CLASS)
				{
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
				}
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
		if (set_token == ")")
		{
			if (expectingRole() == Role::FUNC_RETURN)
			{
#ifdef DEBUG_RAE_PARSER
				cout << "Got ending ) for FUNC_RETURNs in DEFINE_REFERENCE_NAME. Going back to FUNC_DEFINITION.\n";
#endif

				expectingRole(Role::UNDEFINED);//could also be Role::FUNC_DEFINITION, but we don't need that, so.
				//TODO: check parenthesisStack:
				newParenthesisEnd(Token::PARENTHESIS_END_FUNC_RETURN_TYPES, ")");
				expectingToken(Token::FUNC_DEFINITION);
			}
			else
			{
				ReportError::reportError(") parenthesis_end - can't be a name for a reference.\n", previousElement());
			}
		}
		else if (set_token == "[")
		{
#ifdef DEBUG_RAE_PARSER
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
		else if (set_token == ",")
		{
			//REMOVE old.
			assert(0);
			//setUnfinishedElementToStaticArray();
		}
		else if (set_token == "*")
		{
			//TODO pointer type...
			ReportError::reportError("TODO pointer type.", previousElement());
			newLangElement(Token::MULTIPLY, TypeType::UNDEFINED, set_token);
		}
		else if (set_token == "?")
		{
			//A huge TODO.
			ReportError::reportError("Huge todo on line 4535. handle null pointers: ????.", previousElement());
			//newLangElement(Token::MULTIPLY, set_token);
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
		previousElement()->typeType(TypeType::TEMPLATE);
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
	}
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
			newParenthesisBegin(Token::PARENTHESIS_BEGIN_FUNC_RETURN_TYPES, "(");
			expectingRole(Role::FUNC_RETURN);

			//doReturnToExpectToken();//How about this change?
			expectingToken(Token::UNDEFINED);

			//expectingToken(Token::FUNC_RETURN_TYPE);
		}
		else
		{
#ifdef DEBUG_RAE
			//rae::log("Got func_name. The name could be this token:>", set_token, "< Waiting params.\n");
#endif
			//record func name
			if (currentFunc)
			{
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
	//currentFunc->newLangElement(lineNumber, Token::PARENTHESIS_END_FUNC_DEFINITION, ")");
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
	//currentFunc->newLangElement(lineNumber, Token::COMMA, ",");
	//}
	newLangElement(Token::COMMA, TypeType::UNDEFINED, ",");
	expectingToken(Token::FUNC_ARGUMENT_TYPE);
	}
	else if(set_token == "override")
	{
	//if(currentFunc)
	//{
	//currentFunc->newLangElement(lineNumber, Token::OVERRIDE);
	//}
	newLangElement(Token::OVERRIDE, TypeType::UNDEFINED, set_token);
	}
	else if( set_token == "vector" )//Ooh, it's a vector
	{
	#ifdef DEBUG_RAE
	//rae::log("Got comma between param types.>", set_token, "< Waiting rest FUNC_ARGUMENT_TYPE.\n");
	#endif
	//if(currentReference)
	//{
	//currentReference->typeType(TypeType::C_ARRAY);
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

	//currentFunc->newLangElement(lineNumber, Token::DEFINE_FUNC_ARGUMENT);
	//currentFunc->addTypeToCurrentElement(set_token);

	//					if( BuiltInType::isBuiltInType(set_token) )
	//				{
	//				//ok.
	//		}
	//	else
	//{
	//LangElement* found_elem = searchToken(set_token);
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
	//	currentFunc->newLangElement(lineNumber, Token::PARENTHESIS_END_FUNC_DEFINITION, ")");
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
	newLangElement(Token::COMMA, TypeType::UNDEFINED, ",");
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
	//currentReference->typeType(TypeType::C_ARRAY);
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
	//currentReference->typeType(TypeType::C_ARRAY);
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
}

} // end namespace Rae