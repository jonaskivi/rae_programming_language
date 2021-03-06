//This is currently just used as a mixin with a preprocessor include. :)
//We could make it a class, but currently we don't have a need for that, so
//let's just have it like this for a while, and see if there's any problems with
//it. 


//Validation

	vector<Element*> validLine;

	bool validate()
	{
		rlutil::setColor(rlutil::BROWN);
		cout<<"Validating: ";
		rlutil::setColor(rlutil::WHITE);
		cout<<moduleName()<<"\n";

		for( Element* module_elem : elements )
		{
			if( module_elem->token() != Token::MODULE )
			{
				#ifdef DEBUG_RAE_HUMAN
					cout<<"Validate Interesting: we have something in top hierarchy: "<<module_elem->tokenString()<<"\n";
				#endif
				continue; // skip it
			}

			validateElement(*module_elem);
		}		

		//return validateElement(*this);
		return true;
	}

	void iterateValidate( Element& set_elem )
	{
		for(uint i = 0; i < set_elem.elements.size(); ++i)
		{
			/*if(i+1 < set_elem.elements.size())
			{
				writer.nextElement( set_elem.elements[i+1] );
			}*/

			validateElement( *set_elem.elements[i] );
		}
	}

	//Does the_square_object fit the_hole:
	//in assignments:
	//something = other.call(args) + 52
	//in func_calls:
	//other.call( something.somewhere( got.here(1+3) + oh_no ), what_is_this, 45 )

	void validateStatement(Element& the_hole, Element& the_square_element_start)
	{

	}
/*
	vector<Element*> primalTypes(Element* elem)
	{
		// Search for the primalType between two commas? End at NEWLINE or SEMICOLON?

		vector<Element*> ret;

		do some kind of iterate thing here:
		if( elem->findNext([](Element* test_elem){ if( isEndsStatement(test_elem) ) return test_elem; return nullptr; }) )
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
		HelloWorld hello         Token::USE_REFERENCE typetype: Kind::VAL line: 170
		.                        Token::REFERENCE_DOT line: 170
		count                    Token::FUNC_CALL line: 170
		(                        Token::PARENTHESIS_BEGIN line: 170
		HelloWorld hello         Token::USE_REFERENCE typetype: Kind::VAL line: 170
		.                        Token::REFERENCE_DOT line: 170
		int_count                Token::FUNC_CALL line: 170
		(                        Token::PARENTHESIS_BEGIN line: 170
		HelloWorld hello         Token::USE_REFERENCE typetype: Kind::VAL line: 170
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
		HelloWorld hello         Token::USE_REFERENCE typetype: Kind::VAL line: 170
		.                        Token::REFERENCE_DOT line: 170
		tester_count             Token::FUNC_CALL line: 170
		(                        Token::PARENTHESIS_BEGIN line: 170
		HelloWorld hello         Token::USE_REFERENCE typetype: Kind::VAL line: 170
		.                        Token::REFERENCE_DOT line: 170
		Tester tester            Token::USE_REFERENCE typetype: Kind::VAL line: 170
		,                        Token::COMMA line: 170
		Tester tester_lnk        Token::USE_REFERENCE typetype: Kind::LINK line: 170
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
			HelloWorld hello         Token::USE_REFERENCE typetype: Kind::VAL line: 170
			.                        Token::REFERENCE_DOT line: 170
			count                    Token::FUNC_CALL line: 170
			(                        Token::PARENTHESIS_BEGIN line: 170
				HelloWorld hello         Token::USE_REFERENCE typetype: Kind::VAL line: 170
				.                        Token::REFERENCE_DOT line: 170
				int_count                Token::FUNC_CALL line: 170
				(                        Token::PARENTHESIS_BEGIN line: 170
					HelloWorld hello         Token::USE_REFERENCE typetype: Kind::VAL line: 170
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
				HelloWorld hello         Token::USE_REFERENCE typetype: Kind::VAL line: 170
				.                        Token::REFERENCE_DOT line: 170
				tester_count             Token::FUNC_CALL line: 170
				(                        Token::PARENTHESIS_BEGIN line: 170
					HelloWorld hello         Token::USE_REFERENCE typetype: Kind::VAL line: 170
					.                        Token::REFERENCE_DOT line: 170
					Tester tester            Token::USE_REFERENCE typetype: Kind::VAL line: 170
					,                        Token::COMMA line: 170
					Tester tester_lnk        Token::USE_REFERENCE typetype: Kind::LINK line: 170
				)                        Token::PARENTHESIS_END line: 170
			)                        Token::PARENTHESIS_END line: 170
		)                        Token::PARENTHESIS_END_LOG line: 170
		NEWLINE
	*/

	// other parenthesis except the ones in the start and the end
	bool hasListOtherParenthesis(vector<Element*>& list)
	{
		if( list.size() == 0 )
			return false;

		// note, we skip first and last index.
		for(uint i = 1; i < list.size()-1; ++i)
		{
			if( list[i]->isParenthesis() )
				return true;
		}
		return false;
	}

	// Returns expressionRValue of the FUNC_CALL
	//Element*
	void validateFuncCall(Element& set_elem)
	{
		//TODO iterate and find arguments. validate them. add conversion & etc.
		if( set_elem.isFunc() )
		{
			ReportError::compilerError("Trying to validateFuncCall an element which is not a FUNC_CALL.", &set_elem);
			return;
		}
		
		#ifdef DEBUG_RAE_VALIDATE
		cout<<"\nValidate Func: "<<set_elem.name()<<"\n";
		#endif

		//In Rae I'd like to just write:
		//alias func_definition -> set_elem.definitionElement
		//instead of creating a temporary in these situations, we're just doing this for clarity:
		Element* func_definition = set_elem.definitionElement();

		if( func_definition == nullptr )
		{
			ReportError::compilerError("Trying to validateFuncCall but the function was not found and set to definitionElement yet.", &set_elem);
			return;
		}

		//cout<<"Validate function ok.\n";
		
		vector<Element*> func_params = func_definition->funcParameterList();

		if(func_definition->parentClassName() == "array")
		{
			//cout<<"skip array for now.\n";
			return;
		}

		if( func_params.size() == 0 && set_elem.nextElement() && not set_elem.nextElement()->isParenthesis() )
		{
			#ifdef DEBUG_RAE_VALIDATE
			cout<<"Function validated ok. No parenthesis and 0 params.\n";
			#endif
			return;
		}

		#ifdef DEBUG_RAE_VALIDATE
		cout<<"Params for function: "<<func_definition->parentClassName()<<"."<<func_definition->name()<<" : params size: "<<func_params.size()<<"\n";
		
		//nice debugging of funcParameterList:
		for(Element* elem: func_params)
		{
			cout<<"\tparam: "<<elem->toSingleLineString()<<"\n";
			if(elem->definitionElement())
			{
				cout<<"\t\tTYPE definitionElement: "<<elem->definitionElement()->toSingleLineString()<<"\n";
			}
			//else cout<<"types DON'T have definitionElements.\n"; //this mostly happens only for built_in_types.
			//and I'm happy to tell you that we seem to have definitionElements for user defined types!
		}
		#endif
		
		if( set_elem.nextElement() && set_elem.nextElement()->isParenthesis() )
		{
			vector<Element*> element_list;
			set_elem.nextElement()->elementListUntilPair(element_list);

			if(hasListOtherParenthesis(element_list))
			{
				#ifdef DEBUG_RAE_VALIDATE
				cout<<"Gonna get em pairs.\n";
				#endif

				/*for(Element* elem: element_list)
				{
					cout<<"\tFUNC_ELEM_LIST: "<<elem->toSingleLineString()<<"\n";
				}
				
				//THIS IS BOLLOCKS!:
				uint k = 0;
				vector<vector<Element*> > arg_stuff;
				for(uint i = 0; i < func_params.size(); ++i)
				{
					if(k >= element_list.size())
					{
						ReportError::reportError("Too few arguments to function.", {&set_elem, element_list[k-1]});
						return;
					}
					Element* arg = element_list[k];
					Element* param = func_params[i];
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
			#if (_MSC_VER == 1700) // Visual Studio 2012
				ReportError::reportError("Trying to validateFuncCall but nextElement was not a parenthesis.", set_elem.nextElement() );
			#else
				ReportError::reportError("Trying to validateFuncCall but nextElement was not a parenthesis.", { &set_elem, set_elem.nextElement() } );
			#endif
		}


		/*
		vector<Element*> func_call_args = set_elem.funcCallArgumentList();		
		for(Element* elem: func_call_args)
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
		for(Element* a_param: func_params)
		{
			if(ind >= func_call_args.size() )
			{
				//Too few arguments, so we check if there are default initializers...
				Element* default_init = a_param.getFuncParamDefaultInitializer();
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
				
				if( a_param.kind() != func_call_args[ind]->kind() )
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

	void validateLine(vector<Element*>& line, int tab_level = 0)
	{
		#ifdef DEBUG_RAE_VALIDATE
		string current_module_name;
		if (!line.empty())
			current_module_name = line[0]->parentModuleString();

		if (g_debugModuleName == current_module_name)
		{
			coutIndent(tab_level);
			cout<<"START validateLine: \n";
			for(uint i = 0; i < line.size(); ++i)
			{
				coutIndent(tab_level);
				cout<<i<<" : "<<line[i]->toSingleLineString()<<"\n";
			}
		}
		#endif
		
		
		if(hasListOtherParenthesis(line))
		{
			// Parenthesis splitting:
			#ifdef DEBUG_RAE_VALIDATE
			if (g_debugModuleName == current_module_name)
			{
				cout<<"We still got parens.\n";
			}
			#endif

			for(uint i = 0; i < line.size(); ++i)
			{
				// Ignore first and last elements, find parenthesis.
				if( i > 0 && i < line.size()-1 && line[i]->isParenthesis() )
				{
					vector<Element*> fragment;
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
			//return;
		}

		#ifdef DEBUG_RAE_VALIDATE
		if (g_debugModuleName == current_module_name)
		{
			cout<<"Validate. Continue line with no parens.\n";
		}
		#endif

		// These are the parenthesis after a FUNC_CALL
		if( line.size() > 0 && line[0]->previousElement() && line[0]->previousElement()->token() == Token::FUNC_CALL )// && line[0]->isParenthesis() )
		{
			#ifdef DEBUG_RAE_VALIDATE
			if (g_debugModuleName == current_module_name)
			{
				cout<<"Validate function call parenthesis.\n";
			}
			#endif

			Element* set_elem = line[0]->previousElement();
			
			Element* func_definition = set_elem->definitionElement();

			if( func_definition == nullptr )
			{
				ReportError::compilerError("Trying to validateFuncCall but the function was not found and set to definitionElement yet.", set_elem);
				return;
			}

			//cout<<"Validate function ok.\n";
			
			vector<Element*> func_params = func_definition->funcParameterList();

			//if(func_definition->parentClassName() == "array")
			//{
				//cout<<"skip array for now.\n";
			//	return;
			//}

			if( func_params.size() == 0 && set_elem->nextElement() && not set_elem->nextElement()->isParenthesis() )
			{
				#ifdef DEBUG_RAE_VALIDATE
				if (g_debugModuleName == current_module_name)
				{
					cout<<"Function validated ok. No parenthesis and 0 params.\n";
				}
				#endif
				return;
			}

			uint k = 1;

			#ifdef DEBUG_RAE_VALIDATE
			if (g_debugModuleName == current_module_name)
			{
				cout<<"Params for function: "<<func_definition->parentClassName()<<"."<<func_definition->name()<<" : params size: "<<func_params.size()<<"\n";
			
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

					/*if( line[k]->definitionElement() && func_params[i]->kind() != line[k]->definitionElement()->kind() )
					{
						cout << "Setting typeConvert in: " << line[k]->toSingleLineString() << " to: " << Kind::toString( func_params[i]->kind() ) << "\n";
						line[k]->typeConvert( func_params[i]->kind() );
					}*/

					++k;
				}
			}
			#endif

			// Find commas and split
			uint nroFragments = 0;
			uint start_frag = 1;
			for(k = 1; k < line.size(); ++k)
			{
				if( k > start_frag && line[k]->token() == Token::COMMA )
				{
					#if defined(DEBUG_RAE_VALIDATE) || defined(DEBUG_DEBUGMODULENAME)
						if (g_debugModuleName == current_module_name)
							cout << "Found comma at: " << k << " so fragment is: " << start_frag << " - " << k-1 << "\n";
					#endif
					vector<Element*> fragment;
					for(uint j = start_frag; j < k; ++j)
					{
						fragment.push_back( line[j] );
					}
					
					if( func_params.size() > nroFragments )
					{
						validateParameterLine(*func_params[nroFragments], fragment);
					}
					else
					{
						#if defined(DEBUG_RAE_VALIDATE)
							cout << "TODO ERROR: too many parameters to a function.\n";
						#endif
					}
					
					start_frag = k + 1;
					++nroFragments;
				}

				if( k == line.size()-1 && k > start_frag)
				{
					#if defined(DEBUG_RAE_VALIDATE) || defined(DEBUG_DEBUGMODULENAME)
						if (g_debugModuleName == current_module_name)
							cout << "Last fragment: " << start_frag << " - " << k << "\n";
					#endif
					vector<Element*> fragment;
					for(uint j = start_frag; j < k; ++j)
					{
						fragment.push_back( line[j] );
					}
					
					if( func_params.size() > nroFragments )
					{
						validateParameterLine(*func_params[nroFragments], fragment);
					}
					else
					{
						#if defined(DEBUG_RAE_VALIDATE)
							cout << "TODO ERROR: too many parameters to a function.\n";
						#endif
					}
					
					start_frag = k + 1;
					++nroFragments;
				}
			}

			#ifdef DEBUG_RAE_VALIDATE
				if (g_debugModuleName == current_module_name)
				{
					if( func_params.size() > nroFragments )
						cout << "TODO ERROR: too FEW parameters to a function. Check default args.\n";
					else if(nroFragments > 0)
						cout << "Just the right amount of parameters to a function. " << func_params.size() << " and " << nroFragments << "\n";
				}
			#endif
		}

		bool hasMathOperator = false;
		bool hasInts = false;
		bool hasFloats = false;

		for(Element* elem: line)
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
			ReportError::reportError("Ints and floats mixed in line.", line[0], line.back());
			//assert(0);
		}

		#ifdef DEBUG_RAE_VALIDATE
			if (g_debugModuleName == current_module_name)
			{
				coutIndent(tab_level);
				cout<<"END validateLine\n";
			}
		#endif
		
	}

	void validateParameterLine(Element& param, vector<Element*>& line, int tab_level = 0)
	{
		assert(line.size() > 0); // I bet this assert will cause trouble...

		#ifdef DEBUG_RAE_VALIDATE
			string current_module_name;
			if (!line.empty())
				current_module_name = line[0]->parentModuleString();
		#endif

		if(hasListOtherParenthesis(line))
		{
			#if defined(DEBUG_RAE_VALIDATE) || defined(DEBUG_DEBUGMODULENAME)
				if (g_debugModuleName == current_module_name)
					cout << "PROBLEM: there's parenthesis inside this function call.\n";
			#endif
			////assert(0);
		}

		if(line.size() == 1)
		{
			#if defined(DEBUG_RAE_VALIDATE) || defined(DEBUG_DEBUGMODULENAME)
				if (g_debugModuleName == current_module_name)
					cout << "The only thing here is: " << line[0]->toSingleLineString() << "\n";
			#endif
			validateParameter(param, *line[0]);
		}
		else
		{
			validateParameter(param, *line[0]);

			/*
			for(int i = 0; i < line.size(); ++i)
			{

				
				if( line[i]->token() == Token::USE_REFERENCE && i+1 < line.size() && line[i+1]->token() == Token::REFERENCE_DOT)
				{

				}
			}	
			*/		
		}
	}

	void validateParameter(Element& param, Element& line)
	{
		#if defined(DEBUG_RAE_VALIDATE) || defined(DEBUG_DEBUGMODULENAME)
			string current_module_name = line.parentModuleString();
			if (g_debugModuleName == current_module_name)
				cout << "validateParameter START.\n";
		#endif

		Element* ret_value = line.expressionRValue();

		if(ret_value == nullptr)
		{
			//cout << "TODO ERROR in validateParameter(): expressionRValue is null.\n";
			ReportError::reportError("TODO ERROR in validateParameter(): expressionRValue is null.", &line);
			return;
			//assert(0);
		}

		//if( ret_value->definitionElement() && param.kind() != ret_value->definitionElement()->kind() )
		if( param.kind() != ret_value->kind() )
		{
			#if defined(DEBUG_RAE_VALIDATE) || defined(DEBUG_DEBUGMODULENAME)
				if (g_debugModuleName == current_module_name)
					cout << "Setting typeConvert in: " << line.toSingleLineString() << " to: " << Kind::toString( param.kind() ) << "\n";
			#endif
			line.typeConvert( ret_value->kind(), param.kind() );
		}
		else
		{
			#if defined(DEBUG_RAE_VALIDATE) || defined(DEBUG_DEBUGMODULENAME)
				if (g_debugModuleName == current_module_name)
				{
					cout << "TODO validateParameter() Unhandled typeConvert.\n";
					cout << "types are: ret_value: " << ret_value->toSingleLineString() << " param: " << param.toSingleLineString() << " paramKind: " << Kind::toString( param.kind() ) << "\n";
				}
			#endif
		}

		/*
		if( line.token() == Token::FUNC_CALL )
		{
			if( line.returnType() )
		}
		else if( line.token() == Token::USE_REFERENCE )
		{
			if( line.definitionElement() && param.kind() != line.definitionElement()->kind() )
			{
				cout << "Setting typeConvert in: " << line.toSingleLineString() << " to: " << Kind::toString( param.kind() ) << "\n";
				line.typeConvert( param.kind() );
			}
		}
		*/

		#if defined(DEBUG_RAE_VALIDATE) || defined(DEBUG_DEBUGMODULENAME)
			if (g_debugModuleName == current_module_name)
				cout << "validateParameter END.\n";
		#endif
	}




	// Probably some old stuff:

	bool validateElement(Element& set_elem)
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

	bool validateElementUnused(Element& set_elem)
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
					if( set_elem.previousElement()->kind() == Kind::VAL
						|| set_elem.previousElement()->kind() == Kind::BUILT_IN_TYPE )
					{
						ReportError::reportError("Using value types (val and built in types) with -> point to operator is not possible.", &set_elem );
					}
					else if( set_elem.previousElement()->kind() == Kind::OPT
						|| set_elem.previousElement()->kind() == Kind::REF)
					{
						ReportError::reportError("Using ref or opt owning pointers with -> point to operator is not possible.", &set_elem );	
					}
					else if( 
						set_elem.previousElement()->kind() == Kind::LINK
						|| set_elem.previousElement()->kind() == Kind::PTR
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
					Element* got_statementRValue = 0;
					
					got_statementRValue = set_elem.nextElement()->statementRValue();

					if( got_statementRValue != 0 )
					{

						//This test is currently done in writing, because we wanted to be able
						//to write the error to the line... We should make that available as a mechanism
						//probably in Element again. An error string stored in each Element...

						//test for link to temp val, which is an error.
						if(got_statementRValue->role() == Role::FUNC_RETURN)
						{
							if( got_statementRValue->kind() == Kind::VAL
							|| got_statementRValue->kind() == Kind::BUILT_IN_TYPE )
							{
								writer.writeString( "RAE_ERROR point to temporary object." );
								reportError("pointing a link to a temporary object returned by function call is not possible.", &set_elem);
							}
						}

						//This is C++ writing that should not be in validate anyway:
						if( got_statementRValue->kind() == Kind::VAL
							|| got_statementRValue->kind() == Kind::BUILT_IN_TYPE )
						{
							writer.writeChar( '&' );//TODO make this better...
							//.statementRValue()
							//if( evaluate. == use_ref and writer.nextElement()->evaluateStatementReturnValue()->kind() == ...;
						}
						else if( got_statementRValue->kind() == Kind::OPT
							|| got_statementRValue->kind() == Kind::REF
							|| got_statementRValue->kind() == Kind::LINK
							|| got_statementRValue->kind() == Kind::PTR
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


