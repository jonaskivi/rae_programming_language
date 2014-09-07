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

	void validateFuncCall(LangElement& set_elem)
	{
		iterate and find arguments. validate them. add conversion & etc.
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


