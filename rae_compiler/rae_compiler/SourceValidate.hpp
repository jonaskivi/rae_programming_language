//This is currently just used as a mixin with a preprocessor include. :)
//We could make it a class, but currently we don't have a need for that, so
//let's just have it like this for a while, and see if there's any problems with
//it. 


//Validation

	vector<LangElement*> validLine;

	bool validate()
	{
		rlutil::setColor(rlutil::BROWN);
		cout<<"Validating: ";
		rlutil::setColor(rlutil::WHITE);
		cout<<moduleName()<<"\n";

		for( LangElement* module_elem : langElements )
		{
			if( module_elem->token() != Token::MODULE )
			{
				#ifdef DEBUG_RAE_HUMAN
				cout<<"Validate Interesting: we have something in top hierarchy: "<<module_elem->tokenString()<<"\n";
				//rae::log("Interesting: we have something in top hierarchy: ", module_elem->tokenString(), "\n");
				#endif
				continue; //skip it
			}

			validateElement(*module_elem);
		}		

		//return validateElement(*this);
		return true;
	}

	void iterateValidate( LangElement& set_elem )
	{
		for(uint i = 0; i < set_elem.langElements.size(); ++i)
		{
			/*if(i+1 < set_elem.langElements.size())
			{
				writer.nextElement( set_elem.langElements[i+1] );
			}*/

			validateElement( *set_elem.langElements[i] );
		}
	}

	//Does the_square_object fit the_hole:
	//in assignments:
	//something = other.call(args) + 52
	//in func_calls:
	//other.call( something.somewhere( got.here(1+3) + oh_no ), what_is_this, 45 )

	void validateStatement(LangElement& the_hole, LangElement& the_square_element_start)
	{

	}
/*
	vector<LangElement*> primalTypes(LangElement* elem)
	{
		// Search for the primalType between two commas? End at NEWLINE or SEMICOLON?

		vector<LangElement*> ret;

		do some kind of iterate thing here:
		if( elem->findNext([](LangElement* test_elem){ if( isEndsStatement(test_elem) ) return test_elem; return nullptr; }) )
	}
*/

	/*
	// Complex validation example line:

	log(hello.count( hello.int_count( hello.number, (42 - 14) * 3), hello.tester_count(hello.tester, tester_lnk) ))
        FUNC_CALL  ( int   FUNC_CALL(  int        , number        , FUNC_CALL int     (val tester  , link Tester) ))

	Example debugtree:

		NEWLINE
		log                      Token::LOG line: 170
		(                        Token::PARENTHESIS_BEGIN_LOG line: 170
		HelloWorld hello         Token::USE_REFERENCE typetype: TypeType::VAL line: 170
		.                        Token::REFERENCE_DOT line: 170
		count                    Token::FUNC_CALL line: 170
		(                        Token::PARENTHESIS_BEGIN line: 170
		HelloWorld hello         Token::USE_REFERENCE typetype: TypeType::VAL line: 170
		.                        Token::REFERENCE_DOT line: 170
		int_count                Token::FUNC_CALL line: 170
		(                        Token::PARENTHESIS_BEGIN line: 170
		HelloWorld hello         Token::USE_REFERENCE typetype: TypeType::VAL line: 170
		.                        Token::REFERENCE_DOT line: 170
		number                   Token::FUNC_CALL line: 170
		,                        Token::COMMA line: 170
		(                        Token::PARENTHESIS_BEGIN line: 170
		42                       Token::NUMBER line: 170
		-                        Token::MINUS line: 170
		14                       Token::NUMBER line: 170
		)                        Token::PARENTHESIS_END line: 170
		*                        Token::MULTIPLY line: 170
		*                        Token::MULTIPLY line: 170
		3                        Token::NUMBER line: 170
		)                        Token::PARENTHESIS_END line: 170
		,                        Token::COMMA line: 170
		HelloWorld hello         Token::USE_REFERENCE typetype: TypeType::VAL line: 170
		.                        Token::REFERENCE_DOT line: 170
		tester_count             Token::FUNC_CALL line: 170
		(                        Token::PARENTHESIS_BEGIN line: 170
		HelloWorld hello         Token::USE_REFERENCE typetype: TypeType::VAL line: 170
		.                        Token::REFERENCE_DOT line: 170
		Tester tester            Token::USE_REFERENCE typetype: TypeType::VAL line: 170
		,                        Token::COMMA line: 170
		Tester tester_lnk        Token::USE_REFERENCE typetype: TypeType::LINK line: 170
		)                        Token::PARENTHESIS_END line: 170
		)                        Token::PARENTHESIS_END line: 170
		)                        Token::PARENTHESIS_END_LOG line: 170
		NEWLINE

	// Complex validation example line:

	log(hello.count( hello.int_count( hello.number, (42 - 14) * 3), hello.tester_count(hello.tester, tester_lnk) ))
        FUNC_CALL  ( int   FUNC_CALL(  int        , number        , FUNC_CALL int     (val tester  , link Tester) ))

	// What if it was like this:
		NEWLINE
		log                      Token::LOG line: 170
		(                        Token::PARENTHESIS_BEGIN_LOG line: 170
			HelloWorld hello         Token::USE_REFERENCE typetype: TypeType::VAL line: 170
			.                        Token::REFERENCE_DOT line: 170
			count                    Token::FUNC_CALL line: 170
			(                        Token::PARENTHESIS_BEGIN line: 170
				HelloWorld hello         Token::USE_REFERENCE typetype: TypeType::VAL line: 170
				.                        Token::REFERENCE_DOT line: 170
				int_count                Token::FUNC_CALL line: 170
				(                        Token::PARENTHESIS_BEGIN line: 170
					HelloWorld hello         Token::USE_REFERENCE typetype: TypeType::VAL line: 170
					.                        Token::REFERENCE_DOT line: 170
					number                   Token::FUNC_CALL line: 170
					,                        Token::COMMA line: 170
					(                        Token::PARENTHESIS_BEGIN line: 170
						42                       Token::NUMBER line: 170
						-                        Token::MINUS line: 170
						14                       Token::NUMBER line: 170
					)                        Token::PARENTHESIS_END line: 170
					*                        Token::MULTIPLY line: 170
					*                        Token::MULTIPLY line: 170
					3                        Token::NUMBER line: 170
				)                        Token::PARENTHESIS_END line: 170
				,                        Token::COMMA line: 170
				HelloWorld hello         Token::USE_REFERENCE typetype: TypeType::VAL line: 170
				.                        Token::REFERENCE_DOT line: 170
				tester_count             Token::FUNC_CALL line: 170
				(                        Token::PARENTHESIS_BEGIN line: 170
					HelloWorld hello         Token::USE_REFERENCE typetype: TypeType::VAL line: 170
					.                        Token::REFERENCE_DOT line: 170
					Tester tester            Token::USE_REFERENCE typetype: TypeType::VAL line: 170
					,                        Token::COMMA line: 170
					Tester tester_lnk        Token::USE_REFERENCE typetype: TypeType::LINK line: 170
				)                        Token::PARENTHESIS_END line: 170
			)                        Token::PARENTHESIS_END line: 170
		)                        Token::PARENTHESIS_END_LOG line: 170
		NEWLINE
	*/

	// other parenthesis except the ones in the start and the end
	bool hasListOtherParenthesis(vector<LangElement*>& list)
	{
		// note, we skip first and last index.
		for(uint i = 1; i < list.size()-1; ++i)
		{
			if( list[i]->isParenthesis() )
				return true;
		}
		return false;
	}

	// Returns expressionRValue of the FUNC_CALL
	//LangElement*
	void validateFuncCall(LangElement& set_elem)
	{
		//TODO iterate and find arguments. validate them. add conversion & etc.
		if( set_elem.isFunc() )
		{
			ReportError::compilerError("Trying to validateFuncCall an element which is not a FUNC_CALL.", &set_elem);
			return;
		}
		
		//DEBUG
		cout<<"\nValidate Func: "<<set_elem.name()<<"\n";
		//DEBUG

		//In Rae I'd like to just write:
		//alias func_definition -> set_elem.definitionElement
		//instead of creating a temporary in these situations, we're just doing this for clarity:
		LangElement* func_definition = set_elem.definitionElement();

		if( func_definition == nullptr )
		{
			ReportError::compilerError("Trying to validateFuncCall but the function was not found and set to definitionElement yet.", &set_elem);
			return;
		}

		//cout<<"Validate function ok.\n";
		
		vector<LangElement*> func_params = func_definition->funcParameterList();

		if(func_definition->parentClassName() == "array")
		{
			//cout<<"skip array for now.\n";
			return;
		}

		if( func_params.size() == 0 && set_elem.nextElement() && not set_elem.nextElement()->isParenthesis() )
		{
			cout<<"Function validated ok. No parenthesis and 0 params.\n";
			return;
		}

		cout<<"Params for function: "<<func_definition->parentClassName()<<"."<<func_definition->name()<<" : params size: "<<func_params.size()<<"\n";

		//nice debugging of funcParameterList:
		for(LangElement* elem: func_params)
		{
			cout<<"\tparam: "<<elem->toSingleLineString()<<"\n";
			if(elem->definitionElement())
			{
				cout<<"\t\tTYPE definitionElement: "<<elem->definitionElement()->toSingleLineString()<<"\n";
			}
			//else cout<<"types DON'T have definitionElements.\n"; //this mostly happens only for built_in_types.
			//and I'm happy to tell you that we seem to have definitionElements for user defined types!
		}
		
		if( set_elem.nextElement() && set_elem.nextElement()->isParenthesis() )
		{
			vector<LangElement*> element_list;
			set_elem.nextElement()->elementListUntilPair(element_list);

			if(hasListOtherParenthesis(element_list))
			{
				cout<<"Gonna get em pairs.\n";

				/*for(LangElement* elem: element_list)
				{
					cout<<"\tFUNC_ELEM_LIST: "<<elem->toSingleLineString()<<"\n";
				}
				
				//THIS IS BOLLOCKS!:
				uint k = 0;
				vector<vector<LangElement*> > arg_stuff;
				for(uint i = 0; i < func_params.size(); ++i)
				{
					if(k >= element_list.size())
					{
						ReportError::reportError("Too few arguments to function.", {&set_elem, element_list[k-1]});
						return;
					}
					LangElement* arg = element_list[k];
					LangElement* param = func_params[i];
					if(arg->token() == Token::PARENTHESIS_BEGIN || arg->token() == Token::PARENTHESIS_END)
					{
						++k;
						continue;
					}
				}
				*/
			}
		}
		else
		{
			ReportError::reportError("Trying to validateFuncCall but nextElement was not a parenthesis.", { &set_elem, set_elem.nextElement() } );
		}


		/*
		vector<LangElement*> func_call_args = set_elem.funcCallArgumentList();		
		for(LangElement* elem: func_call_args)
		{
			cout<<"func_call_args: "<<elem->toSingleLineString()<<"\n";
		}
		*/

		/*
		if(func_call_args.size() > func_params.size() )
		{
			//Hmm. TODO function parameter overloading:
			ReportError::reportError("Too many arguments to function in: ", &set_elem, func_definition);
		}
		
		int ind = 0;
		for(LangElement* a_param: func_params)
		{
			if(ind >= func_call_args.size() )
			{
				//Too few arguments, so we check if there are default initializers...
				LangElement* default_init = a_param.getFuncParamDefaultInitializer();
				that func could also be just getInitData()
				if(default_init)
				{
					insert default init into where and how?
				}
				else
				{
					ReportError::reportError("Too few arguments to function in: ", &set_elem, func_definition);
				}
				return;
			}
			if(a_param.type() != func_call_args[ind]->type())
			{
				//Check can we cast or convert the type:
				//Or should we use definitionElements to compare, but how would that work with
				//built_in types?? Or do we do them separately...
				if( isTypeConvertable(a_param.type(), func_call_args[ind]->type()) )
				{
					//No worries then.
				}
				else
				{
					//Worries.
				}
				
				if( a_param.typeType() != func_call_args[ind]->typeType() )
				{
					//So how can we convert VAL, OPT, LINK, REF, PTR, arrays...
					//most common case should be:
					//val to ref //pass a pointer, which must not leave the func.
					//opt to ref //it's a pointer, but require checking the opt
					//link to ref //tester.obj, require checking the link

					//val to val //copy
					//val to link //create a new link object. That you can pass on.
					//val to ptr //unsafe but ok inside unsafe{}
					//val to array //needs a converter. We create a temp array with one link?
					
					
				}
				
			}
			
			ind++;
		}
		*/
		
	}

	void coutIndent(int tab_level)
	{
		for( int i = 0; i < tab_level; i++ )
		{
			cout<<"\t";
		}
	}

	void validateLine(vector<LangElement*>& line, int tab_level = 0)
	{
		//DEBUG
		coutIndent(tab_level);
		cout<<"START validateLine: \n";
		for(uint i = 0; i < line.size(); ++i)
		{
			coutIndent(tab_level);
			cout<<i<<" : "<<line[i]->toSingleLineString()<<"\n";
		}
		//DEBUG
		
		if(hasListOtherParenthesis(line))
		{
			// Parenthesis splitting:

			for(uint i = 0; i < line.size(); ++i)
			{
				// Ignore first and last elements, find parenthesis.
				if( i > 0 && i < line.size()-1 && line[i]->isParenthesis() )
				{
					vector<LangElement*> fragment;
					// starting from parenthesis, put all elements into fragment, until parenthesis pair is found.
					for(uint k = i; k < line.size(); ++k)
					{
						fragment.push_back( line[k] );
						if( line[k] == line[i]->pairElement() )
						{
							// pair is found, validate stuff between these parenthesis.
							validateLine(fragment);
							break;
						}
					}
				}
			}
			return;
		}

		// These are the parenthesis after a FUNC_CALL
		if( line.size() > 0 && line[0]->previousElement() && line[0]->previousElement()->token() == Token::FUNC_CALL )// && line[0]->isParenthesis() )
		{
			LangElement* set_elem = line[0]->previousElement();
			
			LangElement* func_definition = set_elem->definitionElement();

			if( func_definition == nullptr )
			{
				ReportError::compilerError("Trying to validateFuncCall but the function was not found and set to definitionElement yet.", set_elem);
				return;
			}

			//cout<<"Validate function ok.\n";
			
			vector<LangElement*> func_params = func_definition->funcParameterList();

			//if(func_definition->parentClassName() == "array")
			//{
				//cout<<"skip array for now.\n";
			//	return;
			//}

			if( func_params.size() == 0 && set_elem->nextElement() && not set_elem->nextElement()->isParenthesis() )
			{
				cout<<"Function validated ok. No parenthesis and 0 params.\n";
				return;
			}

			cout<<"Params for function: "<<func_definition->parentClassName()<<"."<<func_definition->name()<<" : params size: "<<func_params.size()<<"\n";

			uint k = 1;

			//nice debugging of funcParameterList:
			for(uint i = 0; i < func_params.size() && k < line.size(); ++i)
			{
				cout<<"\tparam: "<<func_params[i]->toSingleLineString()<<"\n";
				if(func_params[i]->definitionElement())
				{
					cout<<"\t\tTYPE definitionElement: "<<func_params[i]->definitionElement()->toSingleLineString()<<"\n";
				}
				//else cout<<"types DON'T have definitionElements.\n"; //this mostly happens only for built_in_types.
				//and I'm happy to tell you that we seem to have definitionElements for user defined types!

				if( line[k]->definitionElement() && func_params[i]->typeType() != line[k]->definitionElement()->typeType() )
				{
					line[k]->typeConvert( func_params[i]->typeType() );
				}

				++k;
			}	
		}

		bool hasMathOperator = false;
		bool hasInts = false;
		bool hasFloats = false;

		for(LangElement* elem: line)
		{
			if(elem->isMathOperator())
				hasMathOperator = true;
			else if(elem->token() == Token::NUMBER || elem->builtInType() == BuiltInType::INT )
			{
				hasInts = true;
			}
			else if(elem->token() == Token::FLOAT_NUMBER || elem->builtInType() == BuiltInType::FLOAT ) //TODO isFloat built in type
			{
				hasFloats = true;
			}
		}

		if(hasFloats && hasInts && line.size() > 0)
		{
			ReportError::reportError("Ints and floats mixed.", line[0]);
			assert(0);
		}

		coutIndent(tab_level);
		cout<<"END validateLine\n";
		
	}

	bool validateElement(LangElement& set_elem)
	{
		if( Token::isNewline(set_elem.token()) )
		{
			validateLine(validLine);
			validLine.clear();
		}
		else
		{
			validLine.push_back(&set_elem);
		}
		iterateValidate(set_elem);
		return true;
	}

	bool validateElementUnused(LangElement& set_elem)
	{

		switch(set_elem.token())
		{
			default:
			break;
			/*
			// We can't validate this currently, because assignment can be used in for loops e.g.
			case Token::ASSIGNMENT:
				if( set_elem.previous2ndElement() )
				{
					if(set_elem.previous2ndElement()->token() == Token::NEWLINE
					|| set_elem.previous2ndElement()->token() == Token::SCOPE_BEGIN)
					{
						// Ok.
					}
					else
					{
						ReportError::reportError("Assignment operator = is not allowed inside other statements. You can't put it inside a while, if or function call.", { &set_elem, set_elem.previous2ndElement() } );
					}
				}
			break;
			*/
			case Token::FUNC_CALL:
				validateFuncCall(set_elem);
			break;
			case Token::POINT_TO:
				if( set_elem.previousElement() )
				{
					if( set_elem.previousElement()->typeType() == TypeType::VAL
						|| set_elem.previousElement()->typeType() == TypeType::BUILT_IN_TYPE )
					{
						ReportError::reportError("Using value types (val and built in types) with -> point to operator is not possible.", &set_elem );
					}
					else if( set_elem.previousElement()->typeType() == TypeType::OPT
						|| set_elem.previousElement()->typeType() == TypeType::REF
						|| set_elem.previousElement()->typeType() == TypeType::LINK
						|| set_elem.previousElement()->typeType() == TypeType::PTR
						|| set_elem.previousElement()->token() == Token::BRACKET_END
					)
					{
						//ok.
					}
					else
					{
						ReportError::reportError("Point to operator -> after strange element: ", set_elem.previousElement() );
					}
				}
				/*
				if( set_elem.nextElement() )
				{
					LangElement* got_statementRValue = 0;
					
					got_statementRValue = set_elem.nextElement()->statementRValue();

					if( got_statementRValue != 0 )
					{

						//This test is currently done in writing, because we wanted to be able
						//to write the error to the line... We should make that available as a mechanism
						//probably in LangElement again. An error string stored in each LangElement...

						//test for link to temp val, which is an error.
						if(got_statementRValue->role() == Role::FUNC_RETURN)
						{
							if( got_statementRValue->typeType() == TypeType::VAL
							|| got_statementRValue->typeType() == TypeType::BUILT_IN_TYPE )
							{
								writer.writeString( "RAE_ERROR point to temporary object." );
								reportError("pointing a link to a temporary object returned by function call is not possible.", &set_elem);
							}
						}

						//This is C++ writing that should not be in validate anyway:
						if( got_statementRValue->typeType() == TypeType::VAL
							|| got_statementRValue->typeType() == TypeType::BUILT_IN_TYPE )
						{
							writer.writeChar( '&' );//TODO make this better...
							//.statementRValue()
							//if( evaluate. == use_ref and writer.nextElement()->evaluateStatementReturnValue()->typeType() == ...;
						}
						else if( got_statementRValue->typeType() == TypeType::OPT
							|| got_statementRValue->typeType() == TypeType::REF
							|| got_statementRValue->typeType() == TypeType::LINK
							|| got_statementRValue->typeType() == TypeType::PTR
						)
						{
							//ok.
						}
						else
						{
							reportError("TODO Compiler error. We should implement other kind of statementRValues in ", &set_elem );
						}
					}
				}*/
			break;
		}

		iterateValidate(set_elem);

		return true;
	}


