//This is currently just used as a mixin with a preprocessor include. :)
//We could make it a class, but currently we don't have a need for that, so
//let's just have it like this for a while, and see if there's any problems with
//it. 


//Writing
	
	void iterateWrite( StringFileWriter& writer, LangElement& set_elem )
	{
		for(uint i = 0; i < set_elem.langElements.size(); i++)
		{
			if(i+1 < set_elem.langElements.size())
			{
				//This misses some nextTokens, which are not children of
				//this LangElement, but maybe that doesn't matter that much,
				//they'll be members of different things anyway, and might not
				//be related.
				//writer.nextToken( langElements[i+1]->token() );
				writer.nextElement( set_elem.langElements[i+1] );
			}
			//set_elem.langElements[i]->write(writer);
			writeElement( writer, *set_elem.langElements[i]);
		}
	}

	void writeElement( StringFileWriter& writer, LangElement& set_elem )
	{
		//int count_elem = 0;

		if(set_elem.parseError() == ParseError::SYNTAX_ERROR)
		{
			//writer.writeString("/*ERROR*/");
			writer.writeString(" RAE ERROR ");
		}

		switch(set_elem.token())
		{
			default:
				//writer.writeIndents();
				writer.writeString( set_elem.name() );
				//writer.writeChar( '\n' );
				//foreach( LangElement* elem, langElements )
				//{
				//	elem->write(writer);
				//}
				{
					/*
					for(uint i = 0; i < set_elem.langElements.size(); i++)
					{
						if(i+1 < set_elem.langElements.size())
						{
							//This misses some nextTokens, which are not children of
							//this LangElement, but maybe that doesn't matter that much,
							//they'll be members of different things anyway, and might not
							//be related.
							//writer.nextToken( langElements[i+1]->token() );
							writer.nextElement( set_elem.langElements[i+1] );
						}
						langElements[i]->write(writer);
					}
					*/
					iterateWrite(writer, set_elem);
				}
			break;
			case Token::UNDEFINED:
			case Token::EMPTY: //more intentional than undefined... I guess they are the same thing...
				//we don't write undefined. it's like empty...
				iterateWrite(writer, set_elem);
			break;
			case Token::INFO_FUNC_PARAM:
				//don't write anything... Oh... let's write some info. Maybe we should have an option for this:
				
				//writer.writeString("/*");
				//writer.writeString(m_name);
				//writer.writeString(":*/");

			break;
			case Token::NEWLINE:

			{
				if( writer.lineNeedsSemicolon() == true
					&& writer.previousToken() != Token::SEMICOLON
					&& writer.previousToken() != Token::NEWLINE
					&& writer.previousToken() != Token::NEWLINE_BEFORE_SCOPE_END
					&& writer.previousToken() != Token::CONSTRUCTOR
					&& writer.previousToken() != Token::DESTRUCTOR
					&& writer.previousToken() != Token::FUNC
					&& writer.previousToken() != Token::MAIN
					/*&& writer.previousToken() != Token::SCOPE_BEGIN
					&& writer.previousToken() != Token::SCOPE_END
					&& writer.previousToken() != Token::FUNC
					&& writer.previousToken() != Token::DEFINE_FUNC_ARGUMENT
					&& writer.previousToken() != Token::DEFINE_FUNC_RETURN //not actually needed...
					&& writer.previousToken() != Token::CLASS
					&& writer.previousToken() != Token::COMMENT
					//&& writer.previousToken() != Token::STAR_COMMENT
					&& writer.previousToken() != Token::VISIBILITY_DEFAULT*/
				)
				{
					/*if( parentToken() == Token::CLASS || parentToken() == Token::ENUM )
					{
						//rae::log("NEWLINESTRANGE. This thing has parent CLASS and not func.", "\n");
						//rae::log("previousToken: ", Token::toString(writer.previousToken());
						//rae::log(" nextToken: ", Token::toString(writer.nextToken()), "\n");
					}*/

					writer.writeChar(';');

					if(debugWriteLineNumbers == true)
					{
						writer.writeString("//line: " + numberToString( set_elem.lineNumber().line) );
					}
				}
				writer.lineNeedsSemicolon(true);

				bool so_do_we_write_newline = true;

				if( writer.isHeader() == true )
				{
					if( writer.previousToken() == Token::PARENTHESIS_END_FUNC_PARAM_TYPES )
					{
						so_do_we_write_newline = false;
					}
					else if( writer.previous2ndToken() == Token::FUNC
						|| writer.previous2ndToken() == Token::CONSTRUCTOR
						|| writer.previous2ndToken() == Token::DESTRUCTOR
					)
					{
						//hpp after func.
						//no newline after func in hpp.
						//makes it prettier.
						//no newline here.
						so_do_we_write_newline = false;
					}
					//if( writer.previous3rdToken() == Token::PARENTHESIS_END_FUNC_PARAM_TYPES )
					//{

					//}
					else
					{
						//writer.writeChar( '\n' );
						so_do_we_write_newline = true;
					}
				}
				else
				{
					//writer.writeChar( '\n' );
					so_do_we_write_newline = true;
				}

				if( so_do_we_write_newline == true )
				{
					writer.writeChar( '\n' );

					//if( writer.nextToken() == Token::VISIBILITY_DEFAULT )
					if( writer.nextToken() == Token::VISIBILITY_DEFAULT )
					{
						writer.currentIndentMinus();
						writer.writeIndents();
						writer.currentIndentPlus();
					}
					else
					{
						writer.writeIndents();
					}
				}
			}
			break;
			case Token::NEWLINE_BEFORE_SCOPE_END:

				if( writer.lineNeedsSemicolon() == true
					&& writer.previousToken() != Token::SEMICOLON
					&& writer.previousToken() != Token::NEWLINE
					&& writer.previousToken() != Token::NEWLINE_BEFORE_SCOPE_END
					&& writer.previousToken() != Token::CONSTRUCTOR
					&& writer.previousToken() != Token::DESTRUCTOR
					&& writer.previousToken() != Token::FUNC
					&& writer.previousToken() != Token::MAIN
					/*&& writer.previousToken() != Token::SCOPE_BEGIN
					&& writer.previousToken() != Token::SCOPE_END
					&& writer.previousToken() != Token::FUNC
					&& writer.previousToken() != Token::DEFINE_FUNC_ARGUMENT
					&& writer.previousToken() != Token::DEFINE_FUNC_RETURN //not actually needed...
					&& writer.previousToken() != Token::CLASS
					&& writer.previousToken() != Token::COMMENT
					//&& writer.previousToken() != Token::STAR_COMMENT
					&& writer.previousToken() != Token::VISIBILITY_DEFAULT
					*/
				)
				{
					writer.writeChar(';');
				}
				writer.lineNeedsSemicolon(true);

				writer.writeChar( '\n' );
				//we need one less indent on SCOPE_END.
				writer.currentIndentMinus();
				
				if( writer.nextToken() == Token::VISIBILITY_DEFAULT )
				{
					writer.currentIndentMinus();
					writer.writeIndents();
					writer.currentIndentPlus();
				}
				else
				{
					writer.writeIndents();
				}
			break;
			case Token::OVERRIDE:
				writer.writeString("/*override:*/");
			break;
				/*
			case Token::DEFINE_BUILT_IN_TYPE_IN_CLASS:
				writeVisibilityForElement(writer, set_elem);

				writer.writeString( set_elem.builtInTypeStringCpp() );
				writer.writeChar(' ');
				writer.writeString(set_elem.name());
			break;
				 */
			
			case Token::INIT_DATA:
			
			{
				bool write_init_data = true;

				if( writer.isHeader() == true )//hpp
				{
					if( set_elem.previousToken() == Token::DEFINE_FUNC_ARGUMENT)
					{
						write_init_data = true;
					}
					else if( set_elem.previousToken() == Token::DEFINE_REFERENCE )
					{
						write_init_data = false;
					}
				}
				else// if( writer.isHeader() == false )//cpp
				{
					if( set_elem.previousToken() == Token::DEFINE_FUNC_ARGUMENT)
					{
						write_init_data = false;//here we don't want default initializers. c++ doesn't allow them...
					}
					else if( set_elem.previousToken() == Token::DEFINE_REFERENCE )
					{
						write_init_data = false;//Hmm. this could be set to true, but we are already writing this
						//in other places like below, so let's try not to bother.
					}
				}

				if( write_init_data == true )
				{
					writer.writeString(" = ");
					writer.writeString( set_elem.name() );
				}
			}
							
			break;
			case Token::AUTO_INIT:
				//case Token::BUILT_IN_TYPE_AUTO_INIT:
				if(set_elem.typeType() == TypeType::BUILT_IN_TYPE)
				{
					writer.writeString(set_elem.name());
					//TEMP:
					//writer.writeString(" = 0");
					if( set_elem.initData() )
					{
						writer.writeString(" = ");
						writer.writeString( set_elem.initData()->name() );
					}
					else //this should never happen. because initData is set every time a built in type is created.
					{
						reportError("writeElement: built_in_type auto_init had no initData.");
						writer.writeString(" = 0");
					}
				}
				//case Token::OBJECT_AUTO_INIT:
				else if(set_elem.typeType() == TypeType::REF)
				{
					//writer.writeString(m_type);
					//writer.writeString("* ");
					writer.writeString(set_elem.name());
					writer.writeString(" = new ");
					writer.writeString(set_elem.typeInCpp());
					writer.writeString("()");
				}
				//case Token::TEMPLATE_AUTO_INIT:
				else if(set_elem.typeType() == TypeType::TEMPLATE)
				{
					//writer.writeString(m_type);
					//writer.writeString("* ");
				
					writer.writeString(set_elem.name());
					writer.writeString(" = new ");
					writer.writeString( set_elem.templateTypeCombination() );

					//pointer to vector version:
					//writer.writeString(set_elem.name());
					//writer.writeString(" = new TODO TEMPLATE STUFF<");
					//writer.writeString( set_elem.typeInCpp() );
					//if( set_elem.isBuiltInType() == false )
					//{
						//if it is a builtin type, we store it as a value inside the vector.
						//if it is a user defined class, we store it as a pointer (or a ref in Rae.)
						writer.writeChar('*');
					//}
					//writer.writeString(">");
				}
				//case Token::VECTOR_AUTO_INIT:
				else if(set_elem.typeType() == TypeType::VECTOR)
				{
					//writer.writeString(m_type);
					//writer.writeString("* ");
				
					//pointer to vector version:
					writer.writeString(set_elem.name());
					writer.writeString(" = new std::vector<");
					writer.writeString( set_elem.templateSecondTypeStringInCpp() );
					if( set_elem.templateSecondType() && set_elem.templateSecondType()->isBuiltInType() == false )
					{
						//if it is a builtin type, we store it as a value inside the vector.
						//if it is a user defined class, we store it as a pointer (or a ref in Rae.)
						writer.writeChar('*');
					}
					writer.writeString(">");
				}
				//case Token::C_ARRAY_AUTO_INIT:
				
				/*else if(set_elem.typeType() == TypeType::C_ARRAY)
				{
					writer.writeString(set_elem.name());
					writer.writeString(" = new ");
					writer.writeString(set_elem.typeInCpp());
					writer.writeString("[10]");
				}*/
				else
				{
					reportError("writeElement: AUTO_INIT failed because typeType was invalid.");
				}
			break;
			case Token::FREE:
			case Token::AUTO_FREE:
			//case Token::OBJECT_AUTO_FREE:
			//case Token::TEMPLATE_AUTO_FREE:
			//case Token::VECTOR_AUTO_FREE://TODO do we need to clear the vector before delete?
					//writer.writeString(m_type);
					//writer.writeString("* ");
					
					//case Token::C_ARRAY_AUTO_FREE:
					/*if( set_elem.typeType() == TypeType::C_ARRAY)
					{
						writer.writeString("delete[] ");
						writer.writeString(set_elem.name());
					}
					else
					{*/
						writer.writeString("delete ");
						writer.writeString(set_elem.name());
					//}

			break;
			/*
			case Token::BOOL:
				writer.writeString( "bool " );
				writer.writeString(m_name);
			break;
			case Token::BYTE:
				writer.writeString( "int8_t " );
				writer.writeString(m_name);
			break;
			case Token::UBYTE:
				writer.writeString( "uint8_t " );
				writer.writeString(m_name);
			break;
			case Token::CHAR:
				writer.writeString( "char " );
				writer.writeString(m_name);
			break;
			case Token::WCHAR:
				writer.writeString( "wchar " );//TODO check if it works and is int32_t (or 16 bits better...)
				writer.writeString(m_name);
			break;
			case Token::DCHAR:
				writer.writeString( "int32_t " );
				writer.writeString(m_name);
			break;
			case Token::INT:
				writer.writeString( "int32_t " );
				writer.writeString(m_name);
			break;
			case Token::UINT:
				writer.writeString( "uint32_t " );
				writer.writeString(m_name);
			break;
			case Token::LONG:
				writer.writeString( "int64_t " );
				writer.writeString(m_name);
			break;
			case Token::ULONG:
				writer.writeString( "uint64_t " );
				writer.writeString(m_name);
			break;
			case Token::FLOAT:
				writer.writeString( "float " );
				writer.writeString(m_name);
			break;
			case Token::DOUBLE:
				writer.writeString( "double " );
				writer.writeString(m_name);
			break;
			//case Token::REAL:
			//	writer.writeString( "real " );
			//break;
			case Token::INT_IN_CLASS:
				writer.writeString( "int32_t " );
				writer.writeString(m_name);
			break;
			*/
			case Token::EQUALS:
				/*
				if( writer.previousToken() == Token::DEFINE_BUILT_IN_TYPE_IN_CLASS )
				{
					//sneaky. write comment slashes:
					writer.writeString("; //");
				}
				*/
				if( writer.previousElement()
					&& writer.previousElement()->isInClass()
					&& writer.previousToken() == Token::DEFINE_REFERENCE
					//&& writer.previousElement()->typeType() == TypeType::BUILT_IN_TYPE
				)
				{
					//sneaky. write comment slashes:
					writer.writeString("; //");
				}
				//else
				//{
					writer.writeChar( ' ' );
					writer.writeChar( '=' );
					writer.writeChar( ' ' );
				//}
			break;
			case Token::POINT_TO: //Umm. Point to in C++ is = and maybe = &.
				writer.writeChar( ' ' );
				writer.writeChar( '=' );
				writer.writeChar( ' ' );

				//if( writer.nextToken() == Token::USE_REFERENCE && writer.nextElement()->typeType() == TypeType::VAL )
				{
					LangElement* got_statementRValue = 0;
					if( writer.nextElement() != 0)
					{
						got_statementRValue = writer.nextElement()->statementRValue();

						if( got_statementRValue != 0 )
						{
							cout<<"GOT rvalue: "<<got_statementRValue->toString()<<"\n";

							//test for link to temp val, which is an error.
							if(writer.nextElement()->token() == Token::FUNC_CALL)
							{
								if( got_statementRValue->typeType() == TypeType::VAL
								|| got_statementRValue->typeType() == TypeType::BUILT_IN_TYPE )
								{
									writer.writeString( "ERROR /*point to temporary object.*/" );
									reportError("pointing a link to a temporary object returned by function call is not possible.", &set_elem);
								}
							}

							//This could be "else", but we'll write it anyway, even though the test is done twice in FUNC_CALL case.
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
					}
				}
			break;
			case Token::COMMA:
				writer.writeChar( ',' );
				writer.writeChar( ' ' );
			break;
			case Token::PLUS:
				if( writer.previousToken() == Token::PLUS || writer.nextToken() == Token::PLUS )
				{
					//if it's a ++ operator, then no spaces...
					writer.writeChar( '+' );
				}
				else
				{
					//otherwise with spaces please.
					writer.writeChar( ' ' );
					writer.writeChar( '+' );
					writer.writeChar( ' ' );
				}
			break;
			case Token::MINUS:
				if( writer.previousToken() == Token::MINUS || writer.nextToken() == Token::MINUS )
				{
					//if it's a -- operator, then no spaces...
					writer.writeChar( '-' );
				}
				else
				{
					//otherwise with spaces please.
					writer.writeChar( ' ' );
					writer.writeChar( '-' );
					writer.writeChar( ' ' );
				}
			break;
			case Token::SEMICOLON:
				if( writer.nextToken() == Token::NEWLINE || writer.nextToken() == Token::NEWLINE_BEFORE_SCOPE_END )
				{
					writer.writeChar( ';' );
				}
				else
				{
					//otherwise with a space please.
					writer.writeChar( ';' );
					writer.writeChar( ' ' );
				}
			break;
			case Token::DIVIDE:
				writer.writeChar( ' ' );
				writer.writeChar( '/' );
				writer.writeChar( ' ' );
			break;
			case Token::BIGGER_THAN:
				writer.writeChar( ' ' );
				writer.writeChar( '>' );
				writer.writeChar( ' ' );
			break;
			case Token::SMALLER_THAN:
				writer.writeChar( ' ' );
				writer.writeChar( '<' );
				writer.writeChar( ' ' );
			break;
			case Token::REFERENCE_DOT:
				if( writer.previousToken() == Token::NUMBER )
				{
					writer.writeChar( '.' );
				}
				else if(writer.previousElement() && writer.previousElement()->typeType() == TypeType::VAL )
				{
					writer.writeChar( '.' );
				}
				else
				{
					writer.writeChar( '-' );//we're using pointer dereferencing -> for now...
					writer.writeChar( '>' );
				}
			break;
			case Token::USE_MEMBER:
			case Token::USE_REFERENCE:
				if( set_elem.typeType() == TypeType::VECTOR )
				{
					if( writer.nextToken() == Token::BRACKET_BEGIN )
					{
						//Um, C++ is so weird. We need to dereference a pointer to vector to be able to use operator[]. But I guess it makes sense...
						//since operator[] is sort of like a function call, and otherwise it would think we're using a dynamic array of vectors...
						writer.writeString("(*");
						writer.writeString( set_elem.name() );
						writer.writeChar(')');
					}
					else
					{
						writer.writeString( set_elem.name() );
					}
				}
				else
				{
					if(set_elem.isUnknownType() == true)
					{
						writer.writeString("/*possible error, unknown token:*/");
						writer.writeString( set_elem.name() );
					}
					else
					{
						writer.writeString( set_elem.name() );
					}
				}
			break;
			//case Token::UNKNOWN_USE_REFERENCE:
			//	writer.writeString("/*possible error:*/");
			//	writer.writeString( set_elem.name() );
			//break;
			//case Token::USE_ARRAY:
			//	writer.writeString( set_elem.name() );
			//break;
			case Token::PARENTHESIS_BEGIN:
			case Token::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES:
				writer.writeChar( '(' );
			break;
			case Token::PARENTHESIS_BEGIN_FUNC_RETURN_TYPES:
				//no output.
			break;
			case Token::PARENTHESIS_END:
			case Token::PARENTHESIS_END_FUNC_PARAM_TYPES:
				writer.writeChar( ')' );
			break;
			case Token::PARENTHESIS_END_FUNC_RETURN_TYPES:
				//no output
			break;
			case Token::LOG_SEPARATOR:
			case Token::PARENTHESIS_BEGIN_LOG:
			case Token::PARENTHESIS_BEGIN_LOG_LN:
				writer.writeChar( '<' );
				writer.writeChar( '<' );
			break;
			case Token::PARENTHESIS_END_LOG:
				writer.writeString("<<\"\\n\"");//That's a little funky looking line, but it's supposed to look just like that. Hope your syntax highlighting can handle it.
				//it's just , "\n"
			break;
			case Token::PARENTHESIS_END_LOG_LN:
				/////writer.writeChar( ';' );
				
			break;
			case Token::BRACKET_BEGIN:
				//if( set_elem.previousToken() == Token::DEFINE_ARRAY || set_elem.previousToken() == Token::DEFINE_ARRAY_IN_CLASS )
				
				//if( set_elem.previousToken() == Token::DEFINE_REFERENCE && set_elem.previousElement()->typeType() == TypeType::C_ARRAY )
				//{
					//ignore them after definition.
				//}
				//else
				//{
					writer.writeString(set_elem.name());
				//}
			break;
			case Token::BRACKET_END:
				//if( set_elem.previousToken() == Token::BRACKET_BEGIN
					//NO: && (set_elem.previous2ndToken() == Token::DEFINE_ARRAY || set_elem.previous2ndToken() == Token::DEFINE_ARRAY_IN_CLASS)
				   //&& (set_elem.previous2ndToken() == Token::DEFINE_REFERENCE && set_elem.previous2ndElement()->typeType() == TypeType::C_ARRAY )
					//)
				//{
					//ignore them after definition.
				//}
				//else
				//{
					writer.writeString(set_elem.name());
				//}
			break;
			case Token::BRACKET_DEFINE_ARRAY_BEGIN:
				//writer.writeString("*");//a dynamic array is a pointer in C/C++
				//already written in NEW_ARRAY...
			break;
			case Token::BRACKET_DEFINE_ARRAY_END:
				//writer.writeString("*");//a dynamic array is a pointer in C/C++
				//so we write nothing here.
			break;
			
			/*case Token::DEFINE_REFERENCE_IN_CLASS:
				writeVisibilityForElement(writer, set_elem);

				//if( parentToken() == Token::CLASS )
				//{
					writer.writeString(set_elem.typeInCpp());
					writer.writeString("* ");
					writer.writeString(set_elem.name());
				//}	
			break;
			*/
			case Token::DEFINE_REFERENCE:
			
				#ifdef DEBUG_RAE_HUMAN
				cout<<"this is a reference. name: "<<set_elem.name()<<" type: "<<set_elem.type()<<"\n";
				#endif

				if( set_elem.typeType() == TypeType::VAL )
				{
					if( set_elem.isInClass() )
					{
						#ifdef DEBUG_RAE_HUMAN
						cout<<"it is inClass and a val.\n";
						#endif

						writeVisibilityForElement(writer, set_elem);

						writer.writeString(set_elem.typeInCpp());
						writer.writeChar(' ');
						writer.writeString(set_elem.name());
					}
					else
					{	
						writer.writeString(set_elem.typeInCpp());
						writer.writeChar(' ');
						if(set_elem.role() != Role::FUNC_RETURN )
						{
							writer.writeString(set_elem.name());
						}
					}
				}
				else if( set_elem.typeType() == TypeType::REF
					|| set_elem.typeType() == TypeType::OPT
					|| set_elem.typeType() == TypeType::LINK )
				{
					if( set_elem.isInClass() )
					{
						#ifdef DEBUG_RAE_HUMAN
						cout<<"it is inClass and a reference.\n";
						#endif

						writeVisibilityForElement(writer, set_elem);

						writer.writeString(set_elem.typeInCpp());
						writer.writeString("* ");
						if(set_elem.role() != Role::FUNC_RETURN )
						{
							writer.writeString(set_elem.name());
						}
					}
					else
					{	
						if( set_elem.typeType() == TypeType::REF )
						{
							writer.writeString(set_elem.typeInCpp());
							writer.writeString("* ");
							if(set_elem.role() != Role::FUNC_RETURN)
							{
								writer.writeString(set_elem.name());
							}
							else if(set_elem.role() != Role::FUNC_RETURN && set_elem.role() != Role::FUNC_PARAMETER)
							{
								writer.writeString(" = new ");
								writer.writeString(set_elem.typeInCpp());
								writer.writeString("()");
							}
						}
						else //opt or link
						{
							if(set_elem.nextToken() == Token::EQUALS )
							{
								writer.writeString(set_elem.typeInCpp());
								writer.writeString("* ");
								if(set_elem.role() != Role::FUNC_RETURN)
								{
									writer.writeString(set_elem.name());		
								}
							}
							else
							{
								writer.writeString(set_elem.typeInCpp());
								writer.writeString("* ");
								if(set_elem.role() != Role::FUNC_RETURN)
								{
									writer.writeString(set_elem.name());
								}
								else if(set_elem.role() != Role::FUNC_RETURN && set_elem.role() != Role::FUNC_PARAMETER)
								{
									writer.writeString(" = new ");
									writer.writeString(set_elem.typeInCpp());
									writer.writeString("()");
								}
							}
						}
					}
				}
				else if( set_elem.typeType() == TypeType::TEMPLATE )
				{
					if( set_elem.isInClass() )
					{
						#ifdef DEBUG_RAE_HUMAN
						cout<<"writing template.\n";
						#endif

						writeVisibilityForElement(writer, set_elem);

						//pointer to vector version:
						writer.writeString( set_elem.templateTypeCombination() );
						writer.writeString("* ");
						writer.writeString(set_elem.name());						
					}
					else //notInClass
					{
						writer.writeString(set_elem.templateTypeCombination());
						writer.writeString("* ");
						writer.writeString(set_elem.name());
						writer.writeString(" = new ");
						writer.writeString(set_elem.templateTypeCombination());
						writer.writeString("()");
					}
				}
			//break;
			//case Token::DEFINE_BUILT_IN_TYPE:
				else if( set_elem.typeType() == TypeType::BUILT_IN_TYPE )
				{
					#ifdef DEBUG_RAE_HUMAN
					cout<<"built_in_type.\n";
					#endif

					if( set_elem.isInClass() )
					{
						writeVisibilityForElement(writer, set_elem);
						
						writer.writeString( set_elem.builtInTypeStringCpp() );
						writer.writeChar(' ');
						writer.writeString(set_elem.name());
					}
					else
					{
						writer.writeString( set_elem.builtInTypeStringCpp() );
						writer.writeChar(' ');
						if(set_elem.role() != Role::FUNC_RETURN)
						{
							writer.writeString(set_elem.name());
						}
						else if(set_elem.role() != Role::FUNC_RETURN && set_elem.role() != Role::FUNC_PARAMETER)
						{
							if( writer.nextToken() == Token::EQUALS )
							{
								//don't write initdata. it should come...
							}
							else if( set_elem.initData() )
							{
								writer.writeString(" = ");
								writer.writeString( set_elem.initData()->name() );
							}
						}
					}
				}
				//break;
				//case Token::DEFINE_ARRAY_IN_CLASS:
				/*else if( set_elem.typeType() == TypeType::C_ARRAY )
				{
					if(set_elem.isInClass())
					{
						writeVisibilityForElement(writer, set_elem);

						//if( parentToken() == Token::CLASS )
						//{
						writer.writeString(set_elem.typeInCpp());
						writer.writeString("* ");
						writer.writeString(set_elem.name());
						//}
					}	
				//break;
				//case Token::DEFINE_ARRAY:
					else
					{
						writer.writeString(set_elem.typeInCpp());
						writer.writeString("* ");
						writer.writeString(set_elem.name());
						writer.writeString(" = new ");
						writer.writeString(set_elem.typeInCpp());
						writer.writeString("[10]");
					}
				}*/
				//break;
				//case Token::DEFINE_VECTOR_IN_CLASS:
				else if( set_elem.typeType() == TypeType::VECTOR )
				{
					if( set_elem.isInClass() )
					{
						#ifdef DEBUG_RAE_HUMAN
						cout<<"writing vector.\n";
						#endif

						writeVisibilityForElement(writer, set_elem);

						//pointer to vector version:
						writer.writeString("std::vector<");
						writer.writeString( set_elem.templateSecondTypeStringInCpp() );
						if( set_elem.templateSecondType() && set_elem.templateSecondType()->isBuiltInType() == false )
						{
							//if it is a builtin type, we store it as a value inside the vector.
							//if it is a user defined class, we store it as a pointer (or a ref in Rae.)
							writer.writeChar('*');
						}
						writer.writeString(">* ");
						writer.writeString(set_elem.name());
						
						/*
						//value vector version:
						writer.writeString("std::vector<");
						writer.writeString( type() );
						if( isBuiltInType() == false )
						{
							//if it is a builtin type, we store it as a value inside the vector.
							//if it is a user defined class, we store it as a pointer (or a ref in Rae.)
							writer.writeChar('*');
						}
						writer.writeString("> ");
						writer.writeString(m_name);
						*/
					}
				//break;
				//case Token::DEFINE_VECTOR:
					else //notInClass
					{
						//pointer to vector version:
						writer.writeString("std::vector<");
						//writer.writeString( set_elem.typeInCpp() );
						//if( set_elem.isBuiltInType() == false )
						writer.writeString( set_elem.templateSecondTypeStringInCpp() );
						if( set_elem.templateSecondType() && set_elem.templateSecondType()->isBuiltInType() == false )
						{
							//if it is a builtin type, we store it as a value inside the vector.
							//if it is a user defined class, we store it as a pointer (or a ref in Rae.)
							writer.writeChar('*');
						}
						writer.writeString(">* ");
						writer.writeString(set_elem.name());
						writer.writeString(" = new std::vector<");
						writer.writeString( set_elem.templateSecondTypeStringInCpp() );
						if( set_elem.templateSecondType() && set_elem.templateSecondType()->isBuiltInType() == false )
						{
							//if it is a builtin type, we store it as a value inside the vector.
							//if it is a user defined class, we store it as a pointer (or a ref in Rae.)
							writer.writeChar('*');
						}
						writer.writeString(">");
						

						//value vector version:
						/*
						writer.writeString("std::vector<");
						writer.writeString( type() );
						if( isBuiltInType() == false )
						{
							//if it is a builtin type, we store it as a value inside the vector.
							//if it is a user defined class, we store it as a pointer (or a ref in Rae.)
							writer.writeChar('*');
						}
						writer.writeString("> ");
						writer.writeString(m_name);
						*/
					}
				}
			break;
			case Token::TEMPLATE_SECOND_TYPE:
				//ignore. already written above.
			break;
			
			/*
			case Token::USE_VECTOR:
				if( writer.nextToken() == Token::BRACKET_BEGIN )
				{
					//Um, C++ is so weird. We need to dereference a pointer to vector to be able to use operator[]. But I guess it makes sense...
					//since operator[] is sort of like a function call, and otherwise it would think we're using a dynamic array of vectors...
					writer.writeString("(*");
					writer.writeString( set_elem.name() );
					writer.writeChar(')');
				}
				else
				{
					writer.writeString( set_elem.name() );
				}
			break;
			*/
			case Token::IF:
			case Token::FOR:
			case Token::FOREACH:
				writer.lineNeedsSemicolon(false);
				writer.writeString(set_elem.name());
			break;
			case Token::FUNC_CALL:
				writer.writeString(set_elem.name());
				if( writer.nextToken() != Token::PARENTHESIS_BEGIN)
					writer.writeString("()");
			break;
			case Token::SCOPE_BEGIN:
				//writer.writeIndents();
				writer.lineNeedsSemicolon(false);
				writer.writeString( set_elem.name() );
				///////writer.writeChar( '\n' );
				writer.currentIndentPlus();
				/*foreach( LangElement* elem, langElements )
				{
					elem->write(writer);
				}*/
				iterateWrite(writer, set_elem);
			break;
			case Token::SCOPE_END:
				//writer.currentIndentMinus();
				//writer.writeIndents();
				writer.lineNeedsSemicolon(false);
				writer.writeString( set_elem.name() );
				if( set_elem.parentToken() == Token::CLASS || set_elem.parentToken() == Token::ENUM )
				{
					writer.writeString( ";\n\n" );

					//Need to fix C++ to have public visibility by default:
					writer.currentDefaultVisibility("public");
					writer.nextNeedsDefaultVisibility(true);

					//rae::log("writing }; SCOPE_END for class or enum.");
				}
				else if( set_elem.parentToken() == Token::FUNC || set_elem.parentToken() == Token::CONSTRUCTOR || set_elem.parentToken() == Token::DESTRUCTOR )
				{
					////writer.writeChar( '\n' );//one line break after func
					////writer.writeChar( '\n' );
				}
				else
				{
					/////writer.writeChar( '\n' );
				}
				/*
				foreach( LangElement* elem, langElements )
				{
					elem->write(writer);
				}*/
				iterateWrite(writer, set_elem);
			break;
			case Token::QUOTE:
				//writer.writeChar('\"');
				writer.writeString(set_elem.name());
				//writer.writeChar('\"');
			break;
			case Token::COMMENT:
				if( writer.lineNeedsSemicolon() == true
					&& writer.previousToken() != Token::SEMICOLON
					&& writer.previousToken() != Token::NEWLINE
					&& writer.previousToken() != Token::NEWLINE_BEFORE_SCOPE_END
					/*&& writer.previousToken() != Token::SCOPE_BEGIN
					&& writer.previousToken() != Token::SCOPE_END
					&& writer.previousToken() != Token::FUNC
					&& writer.previousToken() != Token::DEFINE_FUNC_ARGUMENT
					&& writer.previousToken() != Token::DEFINE_FUNC_RETURN //not actually needed...
					&& writer.previousToken() != Token::CLASS
					&& writer.previousToken() != Token::COMMENT
					//&& writer.previousToken() != Token::STAR_COMMENT
					&& writer.previousToken() != Token::VISIBILITY_DEFAULT
					*/
				)
				{
					writer.writeChar(';');
				}

				writer.lineNeedsSemicolon(false);

				writer.writeString(set_elem.name());
			break;
			case Token::PLUS_COMMENT:
				//So if the plus comment is the only thing on this line
				//Then we don't need a semicolon...
				if(set_elem.previousToken() == Token::NEWLINE)
				{
					writer.lineNeedsSemicolon(false);
				}

				writer.writeString("/*");//comment it out in c++ syntax!
				writer.writeString(set_elem.name());
				writer.writeString("*/");
			break;
			case Token::STAR_COMMENT:
				//writer.writeChars("/*", 2);

				//So if the star comment is the only thing on this line
				//Then we don't need a semicolon...
				if(set_elem.previousToken() == Token::NEWLINE)
				{
					writer.lineNeedsSemicolon(false);
				}

				writer.writeString(set_elem.name());
				//writer.writeChar('/');
			break;
			case Token::LOG:
				writer.writeString("std::cout");
			break;
			case Token::LOG_LN:
				writer.writeString("std::cout");
			break;

			/*
			case Token::CONSTRUCTOR:
				writer.writeIndents();
				if( set_elem.parent() )
				{
					writer.writeString( set_elem.parent()->name() );//for a c++ constructor, the name of the class...
					//writer.writeString( "__new" + set_elem.parent()->name() );//for a struct "constructor"
				}
				foreach( LangElement* elem, set_elem.langElements )
				{
					elem->write(writer);
				}
			break;
			case Token::DESTRUCTOR:
				writer.writeIndents();
				if( set_elem.parent() )
				{
					writer.writeChar('~');
					writer.writeString( set_elem.parent()->name() );//these two lines for c++ destructor...
					//writer.writeString( "__delete" + set_elem.parent()->name() );//for a struct "constructor"
				}
				writer.writeChar( '\n' );
				foreach( LangElement* elem, set_elem.langElements )
				{
					elem->write(writer);
				}
			break;
			*/
			case Token::FUNC:
			case Token::CONSTRUCTOR:
			case Token::DESTRUCTOR:
			case Token::MAIN:
				//writer.writeIndents();
				
				{

				LangElement* first_return_elem;//we want to keep track of the first return type
				//so we don't write it twice.

				if( writer.isHeader() == false )//cpp
				{
					writer.lineNeedsSemicolon(false);
				}
				else if( set_elem.parent() && set_elem.parent()->token() == Token::CLASS )
				//hpp but no main... main needs no public... and so don't global funcs.
				{
					writer.lineNeedsSemicolon(true);

					//visibility means public, protected, private, library (which we need to not write here).
					/*
					LangElement* viselem = set_elem.hasVisibilityFuncChild();

					if( viselem )
					{
						if( LangElement::isVisibilityNameAllowedInCpp(viselem->name()) )
						{
							writer.writeString( viselem->name() );
							writer.writeChar( ':' );
							writer.writeChar( ' ' );
							
							if(viselem->name() != writer.currentDefaultVisibility() )
							{
								writer.nextNeedsDefaultVisibility(true);
								//if(writer.nextNeedsDefaultVisibility() == TripleOption::NO)
								//	writer.switchNextNeedsDefaultVisibility();
							}
						}
						else
						{
							writer.writeString("");
							writer.writeString( viselem->name() );
							writer.writeString(" ");
						}
					}
					else if(writer.nextNeedsDefaultVisibility() == true)//use default visibility
					{
						writer.nextNeedsDefaultVisibility(false);
						writer.writeString( writer.currentDefaultVisibility() );
						writer.writeChar( ':' );
						writer.writeChar( ' ' );
					}*/

					writeVisibilityForElement(writer, set_elem);

					////////////writer.writeIndents();
				}


					//main only in cpp. other func returns in both.
					if( set_elem.token() == Token::FUNC || (set_elem.token() == Token::MAIN && writer.isHeader() == false) )
					{
						//NOT: reuse myelem //<-- That might be a good keyword for Rae! Not really.
						//LangElement* myelem = set_elem.langElements.front();
						
						/*
						//THIS is how we used to do it. YAGNI.

						LangElement* myelem = set_elem.searchFirst(Token::DEFINE_FUNC_RETURN);

						if( myelem != 0 )
						{
							if( myelem->token() == Token::DEFINE_FUNC_RETURN )
							{
								//These three lines just document the return type name:
								writer.writeString( "//return type name: " );
								writer.writeString( myelem->name() );
								writer.writeChar( '\n' );
								writer.writeIndents();
								
								writer.writeString( myelem->typeInCpp() );
								
								writer.writeChar( ' ' );
							}
						}
						else //no DEFINE_FUNC_RETURN to be found. it is void.
						{
							////rae::log("No DEFINE_FUNC_RETURN: it was: ", myelem->toString();
							writer.writeString( "void " );
							//writer.writeChar( ' ' );
						}
						*/

						LangElement* myelem = set_elem.searchFirst(Token::PARENTHESIS_BEGIN_FUNC_RETURN_TYPES);

						if( myelem != 0 )
						{
							if( myelem->token() == Token::PARENTHESIS_BEGIN_FUNC_RETURN_TYPES )
							{
								first_return_elem = myelem->nextElement();

								if(first_return_elem != 0)
								{
									if( first_return_elem->token() == Token::PARENTHESIS_END_FUNC_RETURN_TYPES )
									{
										//Nothing between parentheses. Mark void return type.
										////rae::log("No DEFINE_FUNC_RETURN: it was: ", myelem->toString();
										writer.writeString( "void " );
										//writer.writeChar( ' ' );		
									}
									else
									{
										//we write the first return type here.
										writeElement(writer, *first_return_elem);
									}
									/*else if( first_return_elem->token() == Token::DEFINE_REFERENCE )
									{
										//These three lines just document the return type name:
										writer.writeString( "//return type name: " );
										writer.writeString( first_return_elem->name() );
										writer.writeChar( '\n' );
										writer.writeIndents();
										
										if( set_elem.typeType() == TypeType::VAL )
										{
											writer.writeString( first_return_elem->typeInCpp() );
										}
										else if( set_elem.typeType() == TypeType::REF
											|| set_elem.typeType() == TypeType::OPT
											|| set_elem.typeType() == TypeType::LINK )
										{
											writer.writeString(set_elem.typeInCpp());
											writer.writeString("* ");
										} 
										//TODO returning TypeType::TEMPLATE
										else if( set_elem.typeType() == TypeType::BUILT_IN_TYPE )
										{
											writer.writeString( set_elem.builtInTypeStringCpp() );
										}
										//TODO returning TypeType::VECTOR
										
										writer.writeChar( ' ' );	
									}
									*/
								}
								else
								{
									reportError("Compiler error in func return type: ", &set_elem);		
								}
							}
							else
							{
								reportError("No return_type parenthesis in func: ", &set_elem);
							}
						}

						if( writer.isHeader() == false )//cpp
						{
							if( set_elem.token() != Token::MAIN && set_elem.parent() )
							{
								//This one writes the class name in cpp func implementations.
								//Class::funcName(params)
								writer.writeString( set_elem.parent()->name() );
								writer.writeString( "::" );
							}
						}

						writer.writeString( set_elem.name() );
					}//if func
					else if( set_elem.token() == Token::CONSTRUCTOR )
					{
						if( set_elem.parent() )
						{
							if( writer.isHeader() == false )//cpp
							{
								writer.writeString( set_elem.parent()->name() );
								writer.writeString( "::" );
							}

							writer.writeString( set_elem.parent()->name() );//for a c++ constructor, the name of the class...
							//writer.writeString( "__new" + set_elem.parent()->name() );//for a struct "constructor"
						}
					}
					else if( set_elem.token() == Token::DESTRUCTOR )
					{
						if( set_elem.parent() )
						{
							if( writer.isHeader() == false )//cpp
							{
								writer.writeString( set_elem.parent()->name() );
								writer.writeString( "::" );
							}

							writer.writeChar('~');
							writer.writeString( set_elem.parent()->name() );//these two lines for c++ destructor...
							//writer.writeString( "__delete" + set_elem.parent()->name() );//for a struct "constructor"
						}
					}

				//WAS HERE BEFORE: }//just to make a nu LangElement*

				
				////////////////////////writer.writeChar( '(' );
				/////////writer.writeChar( ' ' );

				if( writer.isHeader() == false )//cpp
				{
					if( set_elem.token() == Token::MAIN )
					{
						writer.writeString( "(int argc, char* const argv[])" );
						/*
						foreach( LangElement* elem, set_elem.langElements )
						{
							if( elem->token() == Token::DEFINE_FUNC_RETURN )
							{
								//do nothing
							}
							else if( elem->token() == Token::VISIBILITY )
							{
								//do nothing
							}
							else if( elem->token() == Token::DEFINE_FUNC_ARGUMENT )
							{
								//DON'T WANT THESE FOR MAIN, FOR NOW...
								
								//writer.writeString( elem->type() );
								//writer.writeChar( ' ' );
								//writer.writeString( elem->name() );
								
							}
							else if( elem->token() == Token::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES )
							{
								//already written for main
							}
							else if( elem->token() == Token::PARENTHESIS_END_FUNC_PARAM_TYPES )
							{
								//already written for main...

								/////////writer.writeChar( ' ' );
								//writer.writeChar( ')' );
								//writer.writeChar( '\n' );
							}
							else
							{
								elem->write(writer);
							}
						}
						*/
						{
							bool got_first_scope = false;

							for(uint i = 0; i < set_elem.langElements.size(); i++)
							{
								if(i+1 < set_elem.langElements.size())
								{
									//This misses some nextTokens, which are not children of
									//this LangElement, but maybe that doesn't matter that much,
									//they'll be members of different things anyway, and might not
									//be related.
									//writer.nextToken( set_elem.langElements[i+1]->token() );
									writer.nextElement( set_elem.langElements[i+1] );
								}

								if( set_elem.langElements[i]->token() == Token::DEFINE_FUNC_RETURN )
								{
									//do nothing
								}
								else if( set_elem.langElements[i]->token() == Token::VISIBILITY )
								{
									//do nothing
								}
								else if( set_elem.langElements[i]->token() == Token::DEFINE_FUNC_ARGUMENT )
								{
									//DON'T WANT THESE FOR MAIN, FOR NOW...
									
									//writer.writeString( elem->type() );
									//writer.writeChar( ' ' );
									//writer.writeString( elem->name() );
									
								}
								else if( set_elem.langElements[i]->token() == Token::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES )
								{
									//already written for main
								}
								else if( set_elem.langElements[i]->token() == Token::PARENTHESIS_END_FUNC_PARAM_TYPES )
								{
									//already written for main...

									/////////writer.writeChar( ' ' );
									//writer.writeChar( ')' );
									//writer.writeChar( '\n' );
								}
								else if(set_elem.langElements[i]->token() == Token::DEFINE_REFERENCE
									&& (set_elem.langElements[i]->role() == Role::FUNC_RETURN || set_elem.langElements[i]->role() == Role::FUNC_PARAMETER))
								{
									//Don't want these for MAIN for now. Maybe later TODO.

									//just to check that the first return type doesn't get written twice.
									//TODO multiple return types handling.
									/*if( set_elem.langElements[i] == first_return_elem )
									{
										//don't write it.
									}
									else
									{
										writeElement(writer, *set_elem.langElements[i]);
										cout<<"TODO multiple return types handling.\n";
									}*/
								}
								else if( set_elem.langElements[i]->token() == Token::SCOPE_BEGIN )
								{
									got_first_scope = true;
									writer.writeChar('\n'); //one extra newline, because we missed it.
									writeElement(writer, *set_elem.langElements[i]);
								}
								else
								{
									//don't write anything until first scope.
									if(got_first_scope == true)
										writeElement(writer, *set_elem.langElements[i]);
								}
							}
						}
					}
				}//end isHeader cpp
				
					if( set_elem.token() == Token::MAIN )
					{
						//do nothing for main, it's already been written...
					}
					else //a normal FUNC
					{
						/*
						foreach( LangElement* elem, set_elem.langElements )
						{
							if( elem->token() == Token::DEFINE_FUNC_RETURN )
							{
								//do nothing
							}
							else if( elem->token() == Token::VISIBILITY )
							{
								//do nothing
							}
							else if( elem->token() == Token::DEFINE_FUNC_ARGUMENT )
							{
								writer.writeString( elem->type() );
								writer.writeChar( ' ' );
								writer.writeString( elem->name() );
								
							}
							//else if( elem->token() == Token::PARENTHESIS_END_FUNC_PARAM_TYPES )
							//{
								/////////writer.writeChar( ' ' );
							//	writer.writeChar( ')' );
								///////writer.writeChar( '\n' );
							//}
							else
							{
								elem->write(writer);
							}
						}
						*/
						{
							for(unsigned long i = 0; i < set_elem.langElements.size(); i++)
							{
								if(i+1 < set_elem.langElements.size())
								{
									//This misses some nextTokens, which are not children of
									//this LangElement, but maybe that doesn't matter that much,
									//they'll be members of different things anyway, and might not
									//be related.
									//writer.nextToken( set_elem.langElements[i+1]->token() );
									writer.nextElement( set_elem.langElements[i+1] );
								}

								if( set_elem.langElements[i]->token() == Token::DEFINE_FUNC_RETURN )
								{
									//do nothing, already written
								}
								else if( set_elem.langElements[i]->token() == Token::VISIBILITY )
								{
									//do nothing, already written
								}
								else if( set_elem.langElements[i]->token() == Token::DEFINE_FUNC_ARGUMENT )
								{
									cout<<"DEFINE_FUNC_ARGUMENT should be removed now We'll see.\n";
									assert(0);
									
									//if( set_elem.langElements[i]->isBuiltInType() )
									if( set_elem.langElements[i]->typeType() == TypeType::BUILT_IN_TYPE )
									{
										//cout<<"Why NOOOOOOOOOOOOOOOOOOOOOOOOOOOOT\n";
										writer.writeString( set_elem.langElements[i]->typeInCpp() );
									}
									else if( set_elem.langElements[i]->typeType() == TypeType::REF )
									{
										writer.writeString( set_elem.langElements[i]->typeInCpp() );
										writer.writeChar('*');	
									}
									/*else if( set_elem.langElements[i]->typeType() == TypeType::C_ARRAY )
									{
										writer.writeString( set_elem.langElements[i]->typeInCpp() );
										writer.writeChar('*');	
									}*/
									else if( set_elem.langElements[i]->typeType() == TypeType::VECTOR )
									{
										writer.writeString( "std::vector<" );
										//writer.writeString( set_elem.langElements[i]->typeInCpp() );
										writer.writeString( set_elem.langElements[i]->templateSecondTypeStringInCpp() );
										writer.writeString( "*>*" );	
									}
									writer.writeChar(' ');
									writer.writeString( set_elem.langElements[i]->name() );

									if( set_elem.initData() )
									{
										writer.writeString(" = ");
										writer.writeString( set_elem.initData()->name() );
									}
									
								}
								/*else if( set_elem.langElements[i]->token() == Token::PARENTHESIS_END_FUNC_PARAM_TYPES )
								{
									/////////writer.writeChar( ' ' );
									writer.writeChar( ')' );
									///////writer.writeChar( '\n' );
								}*/
								else if( set_elem.langElements[i]->token() == Token::NEWLINE)
								{
									//handle this separately because of isHeader.
									
									//set_elem.langElements[i]->write(writer);
									writeElement(writer, *set_elem.langElements[i]);

									if( writer.isHeader() == true )//hpp
									{
										//end loop. Write only one line to the header.
										i = set_elem.langElements.size();
									}

								}
								/*
								else if( set_elem.langElements[i]->token() == Token::PARENTHESIS_BEGIN
									|| set_elem.langElements[i]->token() == Token::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES
									|| set_elem.langElements[i]->token() == Token::PARENTHESIS_END
									|| set_elem.langElements[i]->token() == Token::PARENTHESIS_END_FUNC_PARAM_TYPES)
								{
									//handle this separately because of isHeader.
									set_elem.langElements[i]->write(writer);
								}*/
								else if(set_elem.langElements[i]->token() == Token::DEFINE_REFERENCE
									&& set_elem.langElements[i]->role() == Role::FUNC_RETURN)
								{
									//just to check that the first return type doesn't get written twice.
									//TODO multiple return types handling.
									if( set_elem.langElements[i] == first_return_elem )
									{
										//don't write it.
									}
									else
									{
										writeElement(writer, *set_elem.langElements[i]);
										cout<<"TODO multiple return types handling.\n";
									}
								}
								else
								{
									/*
									debug:
									if( token() == Token::CONSTRUCTOR )
									{
										//rae::log("const: elem: ", i, " is ", set_elem.langElements[i]->toString(), "\n");

										if( set_elem.langElements[i]->parentToken() == Token::CLASS || set_elem.langElements[i]->parentToken() == Token::ENUM )
										{
											//rae::log("STRANGE. This thing has set_elem.parent CLASS and not func.", "\n");
										}
									}
									*/

									//if( writer.isHeader() == false )//cpp
									//{
									//ok, we don't need this isHeader here, because
									//now we check for newline and end the loop.
										//set_elem.langElements[i]->write(writer);

									writeElement(writer, *set_elem.langElements[i]);
									//}
								}
							}
						}
					}
				}//just to make a nu LangElement*
				
/*
				uint count_elem = 0;
				foreach( LangElement* elem, set_elem.langElements )
				{
					if( count_elem == 0)
					{
						//the first element
						if( elem->token() == Token::RETURN)
						{
							//These three lines just do document the return type name:
							writer.writeString( "//return type name: " );
							writer.writeString( elem->name() );
							writer.writeChar( '\n' );
							
							writer.writeString( elem->type() );
							writer.writeChar( ' ' );
						}
					}
					//elem->write(writer);
					break;//can we break a foreach?
				}					
*/
			break;
			case Token::CLASS:
				//don't write the base template:
				if( set_elem.typeType() == TypeType::TEMPLATE )
				{
					//don't write anything.
					writer.writeString("This would be a template, but we are not writing it.\n");
					writer.writeString( set_elem.name() );
					writer.writeString( " and second_type: " );
					writer.writeString( set_elem.templateSecondTypeString() );
					writer.writeChar('\n');
				}
				else if( writer.isHeader() == true )//hpp
				{
					writer.lineNeedsSemicolon(false);
					writer.writeString( string("\nclass ") + set_elem.name() );
					//writer.writeString( string("struct ") + set_elem.name() );//I once had the idea to make them structs, for easier handling of constructors...
					/*writer.writeChar('\n');
					writer.writeChar('{');
					writer.writeChar('\n');*/
					//writer.writeChars("\n{\n", 3);
					//writer.writeChar('\n');

					/*foreach( LangElement* elem, set_elem.langElements )
					{
						elem->write(writer);
					}*/
					iterateWrite(writer, set_elem);
					/*writer.writeChar('}');
					writer.writeChar(';');
					writer.writeChar('\n');*/
					//writer.writeChars(");\n\n", 3);

				}
				else //isHeader == false //cpp
				{
					{
						writer.writeString( "\n//class " );
						writer.writeString( set_elem.name() );
						writer.writeString( "\n\n" );

						for(uint i = 0; i < set_elem.langElements.size(); i++)
						{
							//OK. This is important:
							//Only write funcs, destructors and constructors for
							//cpp. Add an extra newline between.
							if( set_elem.langElements[i]->token() == Token::FUNC
								|| set_elem.langElements[i]->token() == Token::CONSTRUCTOR
								|| set_elem.langElements[i]->token() == Token::DESTRUCTOR
							)
							{
								if(i+1 < set_elem.langElements.size())
								{
									//This misses some nextTokens, which are not children of
									//this LangElement, but maybe that doesn't matter that much,
									//they'll be members of different things anyway, and might not
									//be related.
									//writer.nextToken( set_elem.langElements[i+1]->token() );
									writer.nextElement( set_elem.langElements[i+1] );
								}
								//set_elem.langElements[i]->write(writer);
								writeElement(writer, *set_elem.langElements[i]);

								writer.writeString("\n\n");
							}
						}
					}
				}
			break;
			case Token::VISIBILITY:
				if( writer.isHeader() == true )
				{
					/*ON PURPOSE LEFT OUT: if( writer.currentDefaultVisibility() != set_elem.name() )
					{
						writer.nextNeedsDefaultVisibility(true);//tanne need to fix this maybe to an int 2,1,0
					}
					*/
					
					//writer.writeIndents(); //don't indent public: etc...
					if( writer.nextToken() == Token::NEWLINE )
					{
						writer.lineNeedsSemicolon(false);
					}
					/*
					if( LangElement::isVisibilityNameAllowedInCpp(set_elem.name()) )
					{
							writer.writeString( set_elem.name() );
							writer.writeChar( ':' );
							writer.writeChar( ' ' );
					}
					else
					{
						writer.writeString("");
						writer.writeString( set_elem.name() );
						writer.writeString(" ");
					}
					*/

					writer.writeString( LangElement::getVisibilityNameInCpp(set_elem.name()) );

					//writer.writeString( set_elem.name() );
					//writer.writeString( ": " );
					/////writer.writeChar( '\n' );
					//writer.writeChar( '\n' ); //let's put a line after public: ...
					/*foreach( LangElement* elem, set_elem.langElements )
					{
						elem->write(writer);
					}*/
				}
				else
				{
					//nothing
				}

				iterateWrite(writer, set_elem);
			break;
			case Token::VISIBILITY_DEFAULT:
				if( writer.isHeader() == true )
				{
					//if( LangElement::isVisibilityNameAllowedInCpp(set_elem.name()) )
					//{
						writer.currentDefaultVisibility(set_elem.name());
						writer.nextNeedsDefaultVisibility(false);

						//writer.writeIndents(); //don't indent public: etc...
						if( writer.nextToken() == Token::NEWLINE )
						{
							writer.lineNeedsSemicolon(false);
						}

						writer.writeString( LangElement::getVisibilityNameInCpp(set_elem.name()) );

						//writer.writeString( set_elem.name() );
						//writer.writeString( ": " );
					/*}
					else
					{
						if( writer.nextToken() == Token::NEWLINE )
						{
							writer.lineNeedsSemicolon(false);
						}

						writer.writeString("");
						writer.writeString( set_elem.name() );
						writer.writeString(" ");
					}*/

					
					/////writer.writeChar( '\n' );
					//writer.writeChar( '\n' ); //let's put a line after public: ...
					/*foreach( LangElement* elem, set_elem.langElements )
					{
						elem->write(writer);
					}*/
				}
				else
				{
					//nothing
				}

				iterateWrite(writer, set_elem);
			break;
			//ignore:
			case Token::IMPORT_NAME:
			case Token::MODULE_NAME:
				//just ignore this type...
			break;
			case Token::IMPORT:
				if( writer.isHeader() == true )
				{
					writer.lineNeedsSemicolon(false);
					writer.writeString( string("#include \"") );
					int not_on_first = 0;
					foreach( LangElement* elem, set_elem.langElements )
					{
						if( elem->token() == Token::IMPORT_NAME )
						{
							if(not_on_first > 0)
							{
								writer.writeChar('/');
							}
							else
							{
								not_on_first++;
							}
							writer.writeString( elem->name() );
						}
						else
						{
							break;//break the foreach... hope this works.
						}
					}
					writer.writeString( string(".hpp\"") );

					if( set_elem.nextElement() && set_elem.nextElement()->token() != Token::IMPORT )
					{
						writer.writeChar('\n');
					}
					
					iterateWrite(writer, set_elem);

					//writer.writeString( string("\n\n") );
				}
				else //isHeader == false //cpp
				{
					//TODO
					writer.lineNeedsSemicolon(false);

					//writer.writeString("#include \"helloworld.hpp\"\n");
					LangElement* last_module_name_element = set_elem.searchLast( Token::IMPORT_NAME );
					writer.writeString("#include \"");
					writer.writeString( last_module_name_element->name() );
					writer.writeString(".hpp\"");

					iterateWrite(writer, set_elem);
				}
			break;
			case Token::MODULE:
				if( writer.isHeader() == true )
				{
					writer.lineNeedsSemicolon(false);
					writer.writeString( string("#ifndef _") );
					foreach( LangElement* elem, set_elem.langElements )
					{
						if( elem->token() == Token::MODULE_NAME )
						{
							writer.writeString( elem->name() );
							writer.writeChar('_');
						}
						else
						{
							break;//break the foreach... hope this works.
						}
					}
					writer.writeString( string("hpp_\n") );
					
					writer.writeString( string("#define _") );
					foreach( LangElement* elem, set_elem.langElements )
					{
						if( elem->token() == Token::MODULE_NAME )
						{
							writer.writeString( elem->name() );
							writer.writeChar('_');
						}
						else
						{
							break;//break the foreach... hope this works.
						}
					}
					writer.writeString( string("hpp_\n\n") );
					
					writer.writeString("#include <stdint.h>\n");//for int32_t etc.
					writer.writeString("#include <iostream>\n");
					writer.writeString("#include <string>\n");
					writer.writeString("#include <vector>\n\n");
					//writer.writeString("using namespace std;"); //not this... I guess.
					
					/*foreach( LangElement* elem, set_elem.langElements )
					{
						elem->write(writer);
					}*/
					iterateWrite(writer, set_elem);

					//writer.writeString( string("\n\n") );
				}
				else //isHeader == false //cpp
				{
					//TODO
					//writer.writeString("#include \"helloworld.hpp\"\n");
					LangElement* last_module_name_element = set_elem.searchLast( Token::MODULE_NAME );
					writer.writeString("#include \"");
					writer.writeString( last_module_name_element->name() );
					writer.writeString(".hpp\"\n");

					//foreach( LangElement* elem, set_elem.langElements )
					//{
					//	elem->write(writer);
					//}
					iterateWrite(writer, set_elem);
				}
			break;
			case Token::CLOSE_MODULE:
				if( writer.isHeader() == true )
				{
					writer.lineNeedsSemicolon(false);
					writer.writeString( string("#endif // _") );
					
					/*foreach( LangElement* elem, set_elem.langElements )
					{
						if( elem->token() == Token::MODULE_NAME )
						{
							writer.writeString( elem->name() );
							writer.writeChar('_');
						}
					}*/

					//our set_elem.parent is the module now:
					if( set_elem.parent() && set_elem.parent()->token() == Token::MODULE)
					{
						foreach( LangElement* elem, set_elem.parent()->langElements )
						{
							if( elem->token() == Token::MODULE_NAME )
							{
								writer.writeString( elem->name() );
								writer.writeChar('_');
							}
							else
							{
								break;//break the foreach... hope this works.
							}
						}
					}
					else
					{
						writer.writeString("module_broken_");
					}					
					writer.writeString( string("hpp_\n\n") );
				}
				else
				{
					//cpp files end with newline...
					writer.writeString( "\n" );
				}
			break;
		}
		
		//writer.previousPreviousToken( writer.previousToken() );
		//writer.previousToken( token() );
		writer.previous2ndElement( writer.previousElement() );
		writer.previousElement( &set_elem );
	}

	void writeVisibilityForElement( StringFileWriter& writer, LangElement& set_elem )
	{
		LangElement* viselem = set_elem.hasVisibilityFuncChild();
		if( viselem )
		{
			/*
			if( LangElement::isVisibilityNameAllowedInCpp(viselem->name()) )
			{
				writer.writeString( viselem->name() );
				writer.writeChar( ':' );
				writer.writeChar( ' ' );
				
				if(viselem->name() != writer.currentDefaultVisibility() )
				{
					writer.nextNeedsDefaultVisibility(true);
					//if(writer.nextNeedsDefaultVisibility() == TripleOption::NO)
					//	writer.switchNextNeedsDefaultVisibility();
				}
			}
			else
			{
				writer.writeString("");
				writer.writeString( viselem->name() );
				writer.writeString(" ");
			}
			*/

			writer.writeString( LangElement::getVisibilityNameInCpp(viselem->name()) );
			//writer.writeChar( ':' );
			//writer.writeChar( ' ' );
			
			if(viselem->name() != writer.currentDefaultVisibility() )
			{
				writer.nextNeedsDefaultVisibility(true);
				//if(writer.nextNeedsDefaultVisibility() == TripleOption::NO)
				//	writer.switchNextNeedsDefaultVisibility();
			}
		}
		else if(writer.previousToken() == Token::VISIBILITY)
		{
			if(writer.previousElement()->name() != writer.currentDefaultVisibility() )
			{
				writer.nextNeedsDefaultVisibility(true);
			}	
		}
		else if(writer.nextNeedsDefaultVisibility() == true)//use default visibility
		{
			writer.nextNeedsDefaultVisibility(false);
			writer.writeString( LangElement::getVisibilityNameInCpp(writer.currentDefaultVisibility()) );
			//writer.writeChar( ':' );
			//writer.writeChar( ' ' );
		}
	}


