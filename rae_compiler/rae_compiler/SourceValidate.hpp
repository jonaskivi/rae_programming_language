//This is currently just used as a mixin with a preprocessor include. :)
//We could make it a class, but currently we don't have a need for that, so
//let's just have it like this for a while, and see if there's any problems with
//it. 


//Validation

	bool validate()
	{
		rlutil::setColor(rlutil::BROWN);
		cout<<"Validating.\n";
		rlutil::setColor(rlutil::WHITE);

		foreach( LangElement* module_elem, langElements )
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
		for(uint i = 0; i < set_elem.langElements.size(); i++)
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

	void validateFuncCall(LangElement& set_elem)
	{
		//TODO iterate and find arguments. validate them. add conversion & etc.
		if( set_elem.isFunc() )
		{
			ReportError::compilerError("Trying to validateFuncCall an element which is not a FUNC_CALL.", &set_elem);
			return;
		}
		
		//In Rae I'd like to just write:
		//alias func_definition -> set_elem.definitionElement
		//instead of creating a temporary in these situations, we're just doing this for clarity:
		LangElement* func_definition = set_elem.definitionElement();

		if(func_definition == nullptr)
		{
			ReportError::compilerError("Trying to validateFuncCall but the function was not found and set to definitionElement yet.", &set_elem);
			return;
		}

		//cout<<"Validate function ok.\n";
		
		vector<LangElement*> func_params = func_definition->funcParameterList();

		//cout<<"Params for function: "<<func_definition->name()<<"\n";

		/*
		//nice debugging of funcParameterList:
		for(LangElement* elem: func_params)
		{
			cout<<"param: "<<elem->toSingleLineString()<<"\n";
			if(elem->definitionElement())
			{
				cout<<"TYPE definitionElement: "<<elem->definitionElement()->toSingleLineString()<<"\n";
			}
			else cout<<"types DON'T have definitionElements.\n"; //this mostly happens only for built_in_types.
			//and I'm happy to tell you that we seem to have definitionElements for user defined types!
		}
		*/

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

	bool validateElement(LangElement& set_elem)
	{

		switch(set_elem.token())
		{
			default:
			break;
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
								writer.writeString( "ERROR point to temporary object." );
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


