// This is currently just used as a mixin with a preprocessor include. :)
// We could make it a class, but currently we don't have a need for that, so
// let's just have it like this for a while, and see if there's any problems with
// it. 

// Write debug tree file:

	void writeDebugTree( StringFileWriter& writer, Element& set_elem )
	{
		writer.writeIndents();
		writer.writeString( set_elem.toSingleLineString() );

		writer.writeChar('\n');
		if (set_elem.initData())
		{
			writer.currentIndentPlus();
			writeDebugTree( writer, *set_elem.initData());	
			writer.currentIndentMinus();			
		}
		
		writer.currentIndentPlus();

		for(uint i = 0; i < set_elem.elements.size(); i++)
		{
			writeDebugTree( writer, *set_elem.elements[i]);
		}

		writer.currentIndentMinus();
	}

	void writeDebugTree2( StringFileWriter& writer, Element& set_elem )
	{
		writer.writeIndents();
		writer.writeString( set_elem.toSingleLineString() );
		writer.writeChar('\n');

		if( set_elem.nextElement() )
		{
			if(set_elem.nextElement()->parent() == &set_elem)
				writer.currentIndentPlus();

			writeDebugTree2( writer, *set_elem.nextElement() );

			if(set_elem.nextElement()->parent() == &set_elem)
				writer.currentIndentMinus();
		}
		else
		{
			//ReportError::reportError("writeDebugTree2 Chain ended at: ", &set_elem);
		}
	}


// Writing
	
	void iterateWrite( StringFileWriter& writer, Element& set_elem )
	{
		for(uint i = 0; i < set_elem.elements.size(); i++)
		{
			if(i+1 < set_elem.elements.size())
			{
				//This misses some nextTokens, which are not children of
				//this Element, but maybe that doesn't matter that much,
				//they'll be members of different things anyway, and might not
				//be related.
				//writer.nextToken( elements[i+1]->token() );
				writer.nextElement( set_elem.elements[i+1] );
			}
			//set_elem.elements[i]->write(writer);
			writeElement( writer, *set_elem.elements[i]);
		}
	}

	void writeElement( StringFileWriter& writer, Element& set_elem )
	{
		//int count_elem = 0;

		if(set_elem.parseError() == ParseError::SYNTAX_ERROR)
		{
			writer.writeString(" RAE_ERROR: ");
		}
		else if(set_elem.parseError() == ParseError::COMPILER_ERROR)
		{
			writer.writeString(" RAE_COMPILER_ERROR: ");
		}
		else if(set_elem.parseError() == ParseError::SYNTAX_WARNING)
		{
			writer.writeString(" RAE_WARNING: ");
		}

		// Skipline thing:
		if( writer.isSkipLine() ) // cpp
		{
			if( set_elem.isNewline() )
			{
				writer.isSkipLine(false);
				writer.lineNeedsSemicolon(false);
			}
			return;
		}

		switch(set_elem.token())
		{
			default:
				//writer.writeIndents();
				writer.writeString( set_elem.name() );
				//writer.writeChar( '\n' );
				//for( Element* elem : elements )
				//{
				//	elem->write(writer);
				//}
				{
					/*
					for(uint i = 0; i < set_elem.elements.size(); i++)
					{
						if(i+1 < set_elem.elements.size())
						{
							//This misses some nextTokens, which are not children of
							//this Element, but maybe that doesn't matter that much,
							//they'll be members of different things anyway, and might not
							//be related.
							//writer.nextToken( elements[i+1]->token() );
							writer.nextElement( set_elem.elements[i+1] );
						}
						elements[i]->write(writer);
					}
					*/
					iterateWrite(writer, set_elem);
				}
			break;
			case Token::UNDEFINED:
				writer.writeString(" RAE_UNDEFINED: ");
				writer.writeString( set_elem.useNamespaceString() );
				writer.writeString( set_elem.name() );
				iterateWrite(writer, set_elem);
			break;
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
				writer.writeLineEnding();

				if( writer.writeSemicolon() )
				{
					if(debugWriteLineNumbers == true)
					{
						writer.writeString(" // line: " + numberToString( set_elem.lineNumber().line) );
					}	
				}

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
				writer.writeLineEnding();

				writer.writeSemicolon();

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
			case Token::PRAGMA_CPP:
				// figure out if we are inside func. then output to cpp file. otherwise to header.
				if( set_elem.parentFunc() == nullptr )
				{
					if( writer.isHeader() == true ) // hpp
					{
						writer.writeString("//");
						writer.writeString(set_elem.name());
						writer.writeChar('\n');
					}					
				}
				else // inside func:
				{
					if( writer.isHeader() == false ) // cpp
					{
						writer.writeString("//");
						writer.writeString(set_elem.name());
						writer.writeChar('\n');
					}					
				}
			break;
			case Token::PRAGMA_CPP_HDR:
				if( writer.isHeader() == true ) // hpp
				{
					writer.writeString("//");
					writer.writeString(set_elem.name());
					writer.writeChar('\n');
				}
			break;
			case Token::PRAGMA_CPP_SRC:
				if( writer.isHeader() == false ) // cpp
				{
					writer.writeString("//");
					writer.writeString(set_elem.name());
					writer.writeChar('\n');
				}
			break;
			case Token::PRAGMA_CPP_END:
				// TODO pragma end header source separation. now its both.
			case Token::PRAGMA_ASM:
			case Token::PRAGMA_ASM_END:
			case Token::PRAGMA_ECMA:
			case Token::PRAGMA_ECMA_END:
				writer.writeString("//");
				writer.writeString(set_elem.name());
				writer.writeChar('\n');
			break;
			case Token::PASSTHROUGH_HDR:
				if( writer.isHeader() == true ) // hpp
				{
					writer.writeString(set_elem.name());
				}
			break;
			case Token::PASSTHROUGH_SRC:
				if( writer.isHeader() == false ) // cpp
				{
					writer.writeString(set_elem.name());
				}
			break;
			case Token::OVERRIDE:
				writer.writeString("override");
			break;
			case Token::MUT:
				writer.writeString("mutable");
			break;
			case Token::LET:
				if( Token::isNewline(set_elem.previousToken()) == false )
					writer.writeChar(' ');
				writer.writeString("const");
				if( Token::isNewline(set_elem.nextToken()) == false )
					writer.writeChar(' ');
			break;
				/*
			case Token::DEFINE_BUILT_IN_TYPE_IN_CLASS:
				writeVisibilityForElement(writer, set_elem);

				writer.writeString( set_elem.builtInTypeStringCpp() );
				writer.writeChar(' ');
				writer.writeString(set_elem.name());
			break;
				 */
			case Token::RAE_NULL:
				writer.writeString("nullptr");
			break;
			case Token::INIT_DATA:
			
			{
				//DEBUG:writer.writeString("INDATA");

				bool write_init_data = true;

				if( writer.isHeader() == true )//hpp
				{
					//REMOVED: if( set_elem.previousToken() == Token::DEFINE_FUNC_ARGUMENT)
					//REMOVED: {
					//REMOVED: 	write_init_data = true;
					//REMOVED: }
					//REMOVED: else
					if( set_elem.previousToken() == Token::DEFINE_REFERENCE )
					{
						write_init_data = false;
					}
				}
				else// if( writer.isHeader() == false )//cpp
				{
					//REMOVED: if( set_elem.previousToken() == Token::DEFINE_FUNC_ARGUMENT)
					//REMOVED: {
						//REMOVED: write_init_data = false;//here we don't want default initializers. c++ doesn't allow them...
					//REMOVED: }
					//REMOVED: else 
					if( set_elem.previousToken() == Token::DEFINE_REFERENCE )
					{
						write_init_data = true;//Hmm. this could be set to true, but we are already writing this
						//in other places like below, so let's try not to bother.
					}
				}

				// Reverse writing of initdata when it's for a FUNC_PARAM because these are default initializers and need to go
				// to header and not cpp.
				if( set_elem.previousElement() && (set_elem.previousElement()->role() == Role::FUNC_PARAMETER || set_elem.previousElement()->role() == Role::FUNC_RETURN) )
					write_init_data = !write_init_data;

				if( write_init_data == true )
				{
					writer.writeString(" = ");
					iterateWrite(writer, set_elem);
				}
			}
							
			break;
			case Token::AUTO_INIT:

				writer.lineNeedsSemicolon(false); // We need commas, but those are inserted before the lines in FUNC and CONSTRUCTOR special handling.

				//case Token::BUILT_IN_TYPE_AUTO_INIT:
				if(set_elem.kind() == Kind::BUILT_IN_TYPE && set_elem.containerType() == ContainerType::UNDEFINED)
				{
					writer.writeString(set_elem.name());
					//TEMP:
					//writer.writeString(" = 0");
					if( set_elem.initData() )
					{
						writer.writeChar('(');
						//writer.writeString( set_elem.initData()->name() );
						iterateWrite(writer, *set_elem.initData());
						writer.writeChar(')');
					}
					else //NOPE: this should never happen. because initData is set every time a built in type is created.
					{
						cout << "this should never happen. because initData is set every time a built in type is created.\n";
						assert(0);
						//ReportError::reportError("writeElement: built_in_type auto_init had no initData.", &set_elem);
						//writer.writeString(" = 0");

						// Don't write anything for now. Maybe later we need to handle ContainerType::STATIC_ARRAY etc.
					}
				}
				//case Token::OBJECT_AUTO_INIT:
				else if(set_elem.kind() == Kind::REF)
				{
					//writer.writeString(m_type);
					//writer.writeString("* ");
					writer.writeString(set_elem.name());
					writer.writeString("( new ");
					if(set_elem.containerType() == ContainerType::ARRAY)
					{
						writer.writeString("std::vector<");
						iterateWrite(writer, set_elem);
						writer.writeString("> )");
					}
					else
					{
						writer.writeString(set_elem.typeInCpp());
						writer.writeString("() )");
					}
				}
				else if(set_elem.kind() == Kind::OPT)
				{
					writer.writeString(set_elem.name());

					if( set_elem.initData() )
					{
						writer.writeChar('(');
						//writer.writeString( set_elem.initData()->name() );
						iterateWrite(writer, *set_elem.initData());
						writer.writeChar(')');
					}
					else
					{
						//writer.writeString(m_type);
						//writer.writeString("* ");
						writer.writeString("( new ");
						if(set_elem.containerType() == ContainerType::ARRAY)
						{
							writer.writeString("std::vector<");
							iterateWrite(writer, set_elem);
							writer.writeString("> )");
						}
						else
						{
							writer.writeString(set_elem.typeInCpp());
							writer.writeString("() )");
						}
					}
				}
				else if(set_elem.kind() == Kind::VAL)
				{
					//writer.writeString(set_elem.name());

					if( set_elem.initData() )
					{
						ReportError::reportWarning("AUTO_INIT for VAL is TODO.", &set_elem);
						//writer.writeString(" = ");
						//writer.writeString( set_elem.initData()->name() );
						//iterateWrite(writer, *set_elem.initData());
					}
					else
					{
						//writer.writeString("SKIP VAL AUTO_INIT");
						//skipping val auto init.
					}
				}
				else if(set_elem.kind() == Kind::LINK)
				{
					//writer.writeString(set_elem.name());

					if( set_elem.initData() )
					{
						ReportError::reportWarning("AUTO_INIT for LINK is TODO.", &set_elem);
						//writer.writeString(" = ");
						//writer.writeString( set_elem.initData()->name() );
						//iterateWrite(writer, *set_elem.initData());
					}
					else
					{
						//writer.writeString("SKIP LINK AUTO_INIT");
						//skipping link auto init.
					}
				}
				//case Token::TEMPLATE_AUTO_INIT:
				else if(set_elem.kind() == Kind::TEMPLATE)
				{
					//REMOVE
					assert(0);

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
				else
				{
					ReportError::reportError("writeElement: AUTO_INIT failed because kind was invalid.", &set_elem);
				}
			break;
			case Token::FREE:
			case Token::AUTO_FREE:
				writer.lineNeedsSemicolon(false);
				writer.writeString("if(");
				writer.writeString(set_elem.name());
				writer.writeString(" != nullptr ) { ");
				writer.writeString("delete ");
				writer.writeString(set_elem.name());
				writer.writeString("; ");
				writer.writeString(set_elem.name());
				writer.writeString(" = nullptr; }");
			break;
			case Token::COMMA:
				writer.writeChar( ',' );
				writer.writeChar( ' ' );
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
			case Token::CAST:
				writer.writeString("static_cast");
			break;
			case Token::BRACKET_CAST_BEGIN:
				writer.writeString("<");
				iterateWrite(writer, set_elem);
			break;
			case Token::BRACKET_CAST_END:
				writer.writeString(">");
				iterateWrite(writer, set_elem);
			break;
			case Token::ASSIGNMENT:
				if( writer.previousElement()
					&& writer.previousElement()->isInClass()
					&& writer.previousToken() == Token::DEFINE_REFERENCE
					//&& writer.previousElement()->kind() == Kind::BUILT_IN_TYPE
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
			case Token::PLUS:
				writer.writeChar( ' ' );
				writer.writeChar( '+' );
				writer.writeChar( ' ' );
			break;
			case Token::MINUS:
				writer.writeChar( ' ' );
				writer.writeChar( '-' );
				writer.writeChar( ' ' );
			break;
			case Token::MULTIPLY:
				writer.writeChar( ' ' );
				writer.writeChar( '*' );
				writer.writeChar( ' ' );
			break;
			case Token::DIVIDE:
				writer.writeChar( ' ' );
				writer.writeChar( '/' );
				writer.writeChar( ' ' );
			break;
			case Token::MODULO:
				writer.writeChar( ' ' );
				writer.writeChar( '%' );
				writer.writeChar( ' ' );
			break;
			case Token::OPERATOR_INCREMENT:
				writer.writeChar( '+' );
				writer.writeChar( '+' );
			break;
			case Token::OPERATOR_DECREMENT:
				writer.writeChar( '-' );
				writer.writeChar( '-' );
			break;
			case Token::PLUS_ASSIGN:
				writer.writeChar( ' ' );
				writer.writeChar( '+' );
				writer.writeChar( '=' );
				writer.writeChar( ' ' );
			break;
			case Token::MINUS_ASSIGN:
				writer.writeChar( ' ' );
				writer.writeChar( '-' );
				writer.writeChar( '=' );
				writer.writeChar( ' ' );
			break;
			case Token::MULTIPLY_ASSIGN:
				writer.writeChar( ' ' );
				writer.writeChar( '*' );
				writer.writeChar( '=' );
				writer.writeChar( ' ' );
			break;
			case Token::DIVIDE_ASSIGN:
				writer.writeChar( ' ' );
				writer.writeChar( '/' );
				writer.writeChar( '=' );
				writer.writeChar( ' ' );
			break;
			case Token::MODULO_ASSIGN:
				writer.writeChar( ' ' );
				writer.writeChar( '/' );
				writer.writeChar( '=' );
				writer.writeChar( ' ' );
			break;
			case Token::EQUALS:
				writer.writeChar( ' ' );
				writer.writeChar( '=' );
				writer.writeChar( '=' );
				writer.writeChar( ' ' );
			break;
			case Token::NOT_EQUAL:
				writer.writeChar( ' ' );
				writer.writeChar( '!' );
				writer.writeChar( '=' );
				writer.writeChar( ' ' );
			break;
			case Token::GREATER_THAN:
				writer.writeChar( ' ' );
				writer.writeChar( '>' );
				writer.writeChar( ' ' );
			break;
			case Token::LESS_THAN:
				writer.writeChar( ' ' );
				writer.writeChar( '<' );
				writer.writeChar( ' ' );
			break;
			case Token::GREATER_THAN_OR_EQUAL:
				writer.writeChar( ' ' );
				writer.writeChar( '>' );
				writer.writeChar( '=' );
				writer.writeChar( ' ' );
			break;
			case Token::LESS_THAN_OR_EQUAL:
				writer.writeChar( ' ' );
				writer.writeChar( '<' );
				writer.writeChar( '=' );
				writer.writeChar( ' ' );
			break;
			case Token::NOT:
				writer.writeChar( '!' );
			break;
			case Token::AND:
				writer.writeChar( ' ' );
				writer.writeChar( '&' );
				writer.writeChar( '&' );
				writer.writeChar( ' ' );
			break;
			case Token::OR:
				writer.writeChar( ' ' );
				writer.writeChar( '|' );
				writer.writeChar( '|' );
				writer.writeChar( ' ' );
			break;
			case Token::BITWISE_OR:
				writer.writeChar( ' ' );
				writer.writeChar( '|' );
				writer.writeChar( ' ' );
			break;
			case Token::BITWISE_AND:
				writer.writeChar( ' ' );
				writer.writeChar( '&' );
				writer.writeChar( ' ' );
			break;
			case Token::BITWISE_XOR:
				writer.writeChar( ' ' );
				writer.writeChar( '^' );
				writer.writeChar( ' ' );
			break;
			case Token::BITWISE_COMPLEMENT:
				writer.writeChar( ' ' );
				writer.writeChar( '~' );
				writer.writeChar( ' ' );
			break;
			case Token::POINT_TO: //Umm. Point to in C++ is = and maybe = &.
			//but now we're using link, so it is usually a function call.
				
				if(set_elem.previousElement())
				{
					if( set_elem.previousToken() == Token::DEFINE_REFERENCE )
					{
						writer.writeChar( '(' );
						//writer.writeChar( ' ' );	
					}
					else if(set_elem.previousElement()->kind() == Kind::PTR)
					{
						writer.writeString(" = ");	
					}
					else if(set_elem.previousElement()->kind() == Kind::OPT)
					{
						writer.writeString(" -> ERROR can't reassign OPT.");
						ReportError::reportError("ERROR can't reassign OPT with the POINT_TO operator.", set_elem.previousElement() );
					}
					else if(set_elem.previousElement()->kind() == Kind::REF)
					{
						writer.writeString(" -> ERROR can't reassign REF.");
						ReportError::reportError("ERROR can't reassign REF with the POINT_TO operator.", set_elem.previousElement() );
					}
					else if( set_elem.previousToken() == Token::USE_REFERENCE || set_elem.previousToken() == Token::BRACKET_END )
					{
						writer.writeString(".linkTo(");
						writer.lineNeedsEnding(")"); // This is a method to end the "linkTo(" func call with a ")".
					}
					else
					{
						ReportError::reportError("Using a POINT_TO token with something that is not a DEFINE_REFERENCE or USE_REFERENCE.", set_elem.previousElement() );
					}
				}

				//COUT
				//ReportError::reportError("WE have a point to.", &set_elem);

				//if( writer.nextToken() == Token::USE_REFERENCE && writer.nextElement()->kind() == Kind::VAL )
				{
					Element* got_expressionRValue = 0;
					if( set_elem.nextElement() != 0)
					{
						#if defined(DEBUG_RAE_WRITER) || defined(DEBUG_RAE_POINT_TO)
							cout << "POINT_TO1: set_elem: " << set_elem.toSingleLineString() << "\n";
							cout << "POINT_TO1: set_elem.next: " << set_elem.nextElement()->toSingleLineString() << "\n";
							cout << "POINT_TO1: set_elem.next.next: " << set_elem.nextElement()->nextElement()->toSingleLineString() << "\n";
							cout << "POINT_TO1: set_elem.next.next.next: " << set_elem.nextElement()->nextElement()->nextElement()->toSingleLineString() << "\n";
						#endif

						got_expressionRValue = set_elem.nextElement()->expressionRValue();

						if( got_expressionRValue != 0 )
						{
							#ifdef DEBUG_RAE_RVALUE
							cout<<"GOT rvalue: "<<got_expressionRValue->toString()<<"\n";
							#endif

							//This error test was moved to validate.

							//test for link to temp val, which is an error.
							if(got_expressionRValue->role() == Role::FUNC_RETURN)
							{
								if( got_expressionRValue->kind() == Kind::VAL
								|| got_expressionRValue->kind() == Kind::BUILT_IN_TYPE )
								{
									writer.writeString( "RAE_ERROR /*point to temporary object.*/" );
									ReportError::reportError("pointing a link to a temporary object returned by function call is not possible.", &set_elem);
								}
							}
							
							//This could be "else", but we'll write it anyway, even though the test is done twice in FUNC_CALL case.
							if( got_expressionRValue->kind() == Kind::VAL
								|| got_expressionRValue->kind() == Kind::BUILT_IN_TYPE )
							{
								//COUT
								//ReportError::reportError("SHOULD_WRITE &", &set_elem);

								writer.writeChar( '&' );//TODO make this better...
								//.expressionRValue()
								//if( evaluate. == use_ref and writer.nextElement()->evaluateStatementReturnValue()->kind() == ...;
							}
							else if( got_expressionRValue->kind() == Kind::OPT
								|| got_expressionRValue->kind() == Kind::REF
								|| got_expressionRValue->kind() == Kind::LINK
								|| got_expressionRValue->kind() == Kind::PTR
							)
							{
								//ok.
								//COUT
								//writer.writeString("IT IS OPT REF LINK PTR.");
								//ReportError::reportError("SHOULD_NOT_WRITE &", &set_elem);
							}
							else
							{
								ReportError::reportError("TODO Compiler error. We should implement other kind of expressionRValues in ", &set_elem );
							}
						}
						#ifdef DEBUG_RAE_RVALUE
						else
						{	
							writer.writeString("NEVER GOT RVALUE.");
							ReportError::reportError("Never got rvalue for", set_elem.nextElement() );
						}
						#endif
					}
				}

			break;
			case Token::REFERENCE_DOT:

				//if (writer.previousElement() )
					//cout << "REFERENCE_DOT previousElement:::: " << writer.previousElement()->toSingleLineString() << "\n";

				if( writer.previousToken() == Token::USE_NAMESPACE )
				{
					assert(0); // This shouldn't happen. We skip dots when getting a namespace.
					writer.writeString( "::" );
				}
				else if( writer.previousToken() == Token::NUMBER )
				{
					ReportError::reportError("REFERENCE_DOT after NUMBER.", writer.previousElement() );
					assert(0); // This should not happen.
					writer.writeChar( '.' );
				}
				else if(writer.previousElement() && writer.previousElement()->kind() == Kind::VAL )
				{
					writer.writeChar( '.' );
				}
				else if(writer.previousElement() && writer.previousElement()->kind() == Kind::LINK )
				{
					writer.writeString( ".obj->" );
				}
				else if( writer.previousElement()
					&& writer.previousElement()->containerType() == ContainerType::ARRAY
					&& writer.previousElement()->kind() != Kind::VAL)
				{
					writer.writeChar( '.' ); // we use this strange thing for arrays: (*our_array_ptr).size()
				}
				else if( writer.previousElement()
					&& writer.previousElement()->token() == Token::BRACKET_BEGIN ) // NOTE it's actually BRACKET_BEGIN and not END here.
					// Because BRACKET_BEGIN is currentParentElement which has children.
				{
					// Using an array: some_array[5].something
					// A difficult case: some_array[Rae::GetSomeThing.whatIsIt(123 + 321)].something
					Element* array_ob = writer.previousElement()->previousElement();
					if(array_ob)
					{
						#if defined(DEBUG_RAE_WRITER) || defined(DEBUG_RAE_BRACKET)
							cout << "array_ob: " << array_ob->toSingleLineString() << "\n";
						#endif
						Element* type_element = array_ob->arrayContainedTypeElement();
						if(type_element)
						{
							if(type_element->kind() == Kind::VAL)
							{
								#if defined(DEBUG_RAE_WRITER) || defined(DEBUG_RAE_BRACKET)
									cout << "It's a VAL in an array. Writing dot.\n";
								#endif
								writer.writeChar( '.' );
							}
							else
							{
								#if defined(DEBUG_RAE_WRITER) || defined(DEBUG_RAE_BRACKET)
									cout << "It's something else. Writing -> for pointer stuff.\n";
								#endif
								writer.writeChar( '-' );
								writer.writeChar( '>' );
							}
						}
						else
						{
							cout << "Couldn't get type_element from an array.\n"; // TODO reportError
						}
					}
					else
					{
						#if defined(DEBUG_RAE_WRITER) || defined(DEBUG_RAE_BRACKET)
							cout << "OH NO. There's no array_ob. We'll just write a dot.\n";
						#endif
						writer.writeChar( '.' );
					}
				}
				else
				{
					writer.writeChar( '-' );//we're using pointer dereferencing -> for now...
					writer.writeChar( '>' );
				}
			break;
			case Token::USE_MEMBER:
			case Token::USE_REFERENCE:
			{
				bool we_need_array_to_ptr_conversion = false;

				if( (set_elem.type() == "array" && set_elem.typeConvertFrom() == Kind::VAL && set_elem.typeConvertTo() == Kind::PTR)
					)
				{
					we_need_array_to_ptr_conversion = true;
					writer.writeChar('&');
				}
				else if( (set_elem.typeConvertFrom() == Kind::VAL && set_elem.typeConvertTo() == Kind::REF)
					|| (set_elem.typeConvertFrom() == Kind::VAL && set_elem.typeConvertTo() == Kind::OPT)
					|| (set_elem.typeConvertFrom() == Kind::VAL && set_elem.typeConvertTo() == Kind::PTR)
					|| (set_elem.typeConvertFrom() == Kind::BUILT_IN_TYPE && set_elem.typeConvertTo() == Kind::PTR)
				)
				{
					// convert val to ref. (in C++ get the pointer of the value type.)
					writer.writeChar('&');
				}
				

				if( set_elem.definitionElement()
					&& set_elem.definitionElement()->containerType() == ContainerType::ARRAY
					&& set_elem.definitionElement()->kind() != Kind::VAL)
				{
					//Um, C++ is so weird. We need to dereference a pointer to vector to be able to use operator[]. But I guess it makes sense...
					//since operator[] is sort of like a function call, and otherwise it would think we're using a dynamic array of vectors...
					writer.writeString("(*");
					writer.writeString( set_elem.name() );
					writer.writeChar(')');
				}
				else
				{
					if(set_elem.isUnknownType() == true)
					{
						writer.writeString("/*possible error, unknown token:*/");
						writer.writeString( set_elem.useNamespaceString() );
						writer.writeString( set_elem.name() );
					}
					else
					{
						writer.writeString( set_elem.name() );
					}
				}

				if( (set_elem.typeConvertFrom() == Kind::LINK && set_elem.typeConvertTo() == Kind::REF)
					|| (set_elem.typeConvertFrom() == Kind::LINK && set_elem.typeConvertTo() == Kind::OPT)
				)
				{
					// convert link to ref.
					writer.writeString(".obj");
				}

				if(we_need_array_to_ptr_conversion)
				{
					writer.writeString("[0]");
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
				writer.addParenthesisCount();
				writer.writeChar( '(' );
				iterateWrite(writer, set_elem);
			break;
			case Token::PARENTHESIS_BEGIN_FUNC_RETURN_TYPES:
				//no output.
				writer.addParenthesisCount();
				iterateWrite(writer, set_elem);
			break;
			case Token::PARENTHESIS_END:
			case Token::PARENTHESIS_END_FUNC_PARAM_TYPES:
				writer.subParenthesisCount();
				writer.writeChar( ')' );
			break;
			case Token::PARENTHESIS_END_FUNC_RETURN_TYPES:
				//no output
				writer.subParenthesisCount();
			break;
			case Token::LOG_SEPARATOR:
			case Token::PARENTHESIS_BEGIN_LOG:
			case Token::PARENTHESIS_BEGIN_LOG_S:
				writer.writeChar( '<' );
				writer.writeChar( '<' );
				iterateWrite(writer, set_elem);
			break;
			case Token::PARENTHESIS_END_LOG:
				writer.writeString("<<\"\\n\"");//That's a little funky looking line, but it's supposed to look just like that. Hope your syntax highlighting can handle it.
				//it's just , "\n"
			break;
			case Token::PARENTHESIS_END_LOG_S:
				/////writer.writeChar( ';' );
				
			break;
			case Token::BRACKET_BEGIN:
				//if( set_elem.previousToken() == Token::DEFINE_ARRAY || set_elem.previousToken() == Token::DEFINE_ARRAY_IN_CLASS )
				
				//if( set_elem.previousToken() == Token::DEFINE_REFERENCE && set_elem.previousElement()->kind() == Kind::C_ARRAY )
				//{
					//ignore them after definition.
				//}
				//else
				//{
					

					// Non bounds checked with just a bracket:
					//writer.writeString(set_elem.name());
					// Bounds checked with helmet on:
					writer.writeString(".at(");
					iterateWrite(writer, set_elem);
				


				//}
			break;
			case Token::BRACKET_END:
				//if( set_elem.previousToken() == Token::BRACKET_BEGIN
					//NO: && (set_elem.previous2ndToken() == Token::DEFINE_ARRAY || set_elem.previous2ndToken() == Token::DEFINE_ARRAY_IN_CLASS)
				   //&& (set_elem.previous2ndToken() == Token::DEFINE_REFERENCE && set_elem.previous2ndElement()->kind() == Kind::C_ARRAY )
					//)
				//{
					//ignore them after definition.
				//}
				//else
				//{

				// Non bounds checked:
				//writer.writeString(set_elem.name());
				// Bounds checked:
				writer.writeString(")");

				if( set_elem.nextElement() && set_elem.nextElement()->token() != Token::POINT_TO ) // There are other cases where we want LINK instead of LINK.obj
				{
					//REMOVE: cout<<"interesting. ARRAYSTUFF:.\n";
					Element* prev_ref = set_elem.searchClosestPreviousUseReference();
					if(prev_ref)
					{
						//REMOVE: cout<<"interesting. ARRAYSTUFF:2.\n";
						if( prev_ref->definitionElement() )
						{
							//REMOVE: cout<<"interesting. ARRAYSTUFF:3.\n";
							//REMOVE: cout<<prev_ref->definitionElement()->toString();
							Element* temp_second_type = prev_ref->definitionElement()->templateSecondType();
							if(temp_second_type)
							{
								//REMOVE: cout<<"interesting. ARRAYSTUFF:4.\n";
								//REMOVE: cout<<temp_second_type->toString();
								/*REMOVE: if(temp_second_type->kind() == Kind::VAL)
								{
									writer.writeChar( '.' );
								}
								else
								*/
								if(temp_second_type->kind() == Kind::LINK)
								{
									writer.writeString( ".obj" );
								}
								//else writer.writeString( "->" );
							}
						}
					}
				}
			break;
			case Token::BRACKET_DEFINE_ARRAY_BEGIN:
				if( set_elem.isInClass() )
				{
					writeVisibilityForElement(writer, set_elem);
				}

				if(set_elem.kind() == Kind::LINK)
				{
					writer.writeString("rae::link<");
				}
				writer.writeString("std::vector<");
				iterateWrite(writer, set_elem);
				writer.writeString(">");
				if(set_elem.kind() == Kind::OPT
					or set_elem.kind() == Kind::REF
					or set_elem.kind() == Kind::PTR)
				{
					writer.writeString("* ");
				}
				else if(set_elem.kind() == Kind::LINK)
				{
					writer.writeString("> ");
				}
				else writer.writeChar(' '); //VAL
				writer.writeString( set_elem.name() );
			break;
			case Token::BRACKET_DEFINE_STATIC_ARRAY_BEGIN:
				if( set_elem.isInClass() )
				{
					writeVisibilityForElement(writer, set_elem);
				}

				writer.writeString("std::array<");
				iterateWrite(writer, set_elem);
				writer.writeString("> ");
				writer.writeString( set_elem.name() );
			break;

			case Token::BRACKET_DEFINE_ARRAY_END:
			case Token::BRACKET_DEFINE_STATIC_ARRAY_END:
				//writer.writeString("*");//a dynamic array is a pointer in C/C++
				//writer.writeString(">");
			break;
                
            case Token::BRACKET_INITIALIZER_LIST_BEGIN:
                writer.writeString("{");
                iterateWrite(writer, set_elem);
            break;
            case Token::BRACKET_INITIALIZER_LIST_END:
                writer.writeString("}");
                iterateWrite(writer, set_elem);
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

			case Token::EXTERN:
				writer.isSkipLine(true); // Skip the whole line because this was probably defined somewhere in C++ side.
			break;
			case Token::DEFINE_REFERENCE:
			
				#ifdef DEBUG_RAE_HUMAN
				cout<<"this is a reference. useNamespace: " << set_elem.useNamespaceString() << "name: "<<set_elem.name()<<" type: "<<set_elem.type()<<"\n";
				#endif

				if( writer.isHeader() == false && set_elem.parent() && set_elem.parent()->token() == Token::MODULE) // cpp
				{
					writer.isSkipLine(true); // Skip the whole line
					break;
				}

				if( set_elem.kind() == Kind::VAL )
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
				else if( set_elem.kind() == Kind::REF || set_elem.kind() == Kind::OPT || set_elem.kind() == Kind::PTR )
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
						if( set_elem.kind() == Kind::REF )
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
						else //opt and ptr differs from ref in that it can be initialized to null with = null.
						{
							if(set_elem.nextToken() == Token::ASSIGNMENT )
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
				else if( set_elem.kind() == Kind::LINK )
				{
					if( set_elem.isInClass() )
					{
						#ifdef DEBUG_RAE_HUMAN
						cout<<"it is inClass and a link.\n";
						#endif

						writeVisibilityForElement(writer, set_elem);

						writer.writeString("rae::link<");
						writer.writeString(set_elem.typeInCpp());
						writer.writeString("> ");
						if(set_elem.role() != Role::FUNC_RETURN )
						{
							writer.writeString(set_elem.name());
						}
					}
					else
					{	
						if(set_elem.nextToken() == Token::ASSIGNMENT )
						{
							writer.writeString("rae::link<");
							writer.writeString(set_elem.typeInCpp());
							writer.writeString("> ");
							if(set_elem.role() != Role::FUNC_RETURN)
							{
								writer.writeString(set_elem.name());		
							}
						}
						else
						{
							writer.writeString("rae::link<");
							writer.writeString(set_elem.typeInCpp());
							writer.writeString("> ");
							if(set_elem.role() != Role::FUNC_RETURN)
							{
								writer.writeString(set_elem.name());
							}
						}
					}
				}
				//IF WE EVER NEED TO USE SHARED_PTR AGAIN:
				/*
				else if( set_elem.kind() == Kind::OPT
					|| set_elem.kind() == Kind::LINK )
				{
					if( set_elem.isInClass() )
					{
						#ifdef DEBUG_RAE_HUMAN
						cout<<"it is inClass and an optional.\n";
						#endif

						writeVisibilityForElement(writer, set_elem);


						writer.writeString("std::shared_ptr<");
						writer.writeString(set_elem.typeInCpp());
						writer.writeString("> ");
						if(set_elem.role() != Role::FUNC_RETURN )
						{
							writer.writeString(set_elem.name());
						}
					}
					else
					{	
						if(set_elem.nextToken() == Token::ASSIGNMENT )
						{
							writer.writeString("std::shared_ptr<");
							writer.writeString(set_elem.typeInCpp());
							writer.writeString("> ");
							if(set_elem.role() != Role::FUNC_RETURN)
							{
								writer.writeString(set_elem.name());		
							}
						}
						else
						{
							writer.writeString("std::shared_ptr<");
							writer.writeString(set_elem.typeInCpp());
							writer.writeString("> ");
							if(set_elem.role() != Role::FUNC_RETURN)
							{
								writer.writeString(set_elem.name());
							}
							else if(set_elem.role() != Role::FUNC_RETURN && set_elem.role() != Role::FUNC_PARAMETER)
							{
								writer.writeString(" = new ");
								writer.writeString("std::shared_ptr<");
								writer.writeString(set_elem.typeInCpp());
								writer.writeString(">()");
							}
						}
						
					}
				}
				*/
				else if( set_elem.kind() == Kind::TEMPLATE )
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
				else if( set_elem.kind() == Kind::BUILT_IN_TYPE )
				{
					#ifdef DEBUG_RAE_HUMAN
					cout<<"built_in_type.\n";
					#endif

					if(set_elem.role() == Role::TEMPLATE_PARAMETER)
					{
						writer.writeString(set_elem.builtInTypeStringCpp());
					}
					else if( set_elem.isInClass() )
					{
						writeVisibilityForElement(writer, set_elem);
						writer.writeString( set_elem.builtInTypeStringCpp() );
						writer.writeChar(' ');
						writer.writeString(set_elem.name());
					}
					else // inFunc probably... or global...
					{
						writer.writeString( set_elem.builtInTypeStringCpp() );
						writer.writeChar(' ');
						
						if(set_elem.role() != Role::FUNC_RETURN)
						{
							writer.writeString(set_elem.name());
						}
						
						// JONDE REFACTOR: INIT_DATA handling has two overlapping ways. As a normal Element that is in the array,
						// or as an initData member for Element. Refactor to have only one way to store INIT_DATA.

						if(set_elem.role() != Role::FUNC_RETURN )// WHY WAS THIS HERE: && set_elem.role() != Role::FUNC_PARAMETER)
						{
							if( writer.nextToken() == Token::ASSIGNMENT || writer.nextToken() == Token::INIT_DATA)
							{
								//don't write initdata. it should come...
							}
							else if( set_elem.initData() )
							{
								//writer.writeString(" = ");
								//writer.writeString( set_elem.initData()->name() );
								//iterateWrite(writer, *set_elem.initData());
								writeElement(writer, *set_elem.initData());
							}
						}
					}
				}
			break;
			case Token::TEMPLATE_SECOND_TYPE:
				//ignore. already written above.
			break;
			
			case Token::IN_TOKEN:
				writer.writeChar(' ');
				writer.writeChar(':');
				writer.writeChar(' ');
			break;
			case Token::IS:
				writer.writeChar(' ');
				writer.writeChar('=');
				writer.writeChar('=');
				writer.writeChar(' ');
			break;
			case Token::RETURN:
				writer.writeString("return ");
			break;
			case Token::IF:
				writer.lineNeedsSemicolon(false);
				writer.writeString("if ");
				if( writer.nextToken() != Token::PARENTHESIS_BEGIN)
				{
					writer.writeString("(");
					writer.lineNeedsEnding(")");
				}
			break;
			case Token::ELSE:
				writer.writeString("else ");
				if( Token::isNewline(writer.nextToken())
					|| writer.nextToken() == Token::COMMENT
					|| writer.nextToken() == Token::SCOPE_BEGIN
				)
				{
					writer.lineNeedsSemicolon(false);
				}
			break;
			case Token::FOR:
			case Token::FOR_EACH:
				writer.lineNeedsSemicolon(false);
				writer.writeString("for ");
			break;
			case Token::TRUE_TRUE:
				writer.writeString("true");
			break;
			case Token::FALSE_FALSE:
				writer.writeString("false");
			break;
			case Token::FUNC_CALL:
				writer.writeString(set_elem.useNamespaceString());
				writer.writeString(set_elem.name());
				if( writer.nextToken() != Token::PARENTHESIS_BEGIN)
					writer.writeString("()");
			break;
			case Token::SCOPE_BEGIN:
				//writer.writeIndents();
				writer.lineNeedsSemicolon(false);
				writer.writeString( set_elem.name() );

				if(set_elem.nextToken() != Token::NEWLINE)
				{
					writer.writeChar(' '); // Extra space for oneliners. But I bet newline checking here is broken and it's done in ::FUNC
				}
				
				//if(writer.previousToken() == Token::NEWLINE) // Can't get this to work...  supposed to check for oneliners.
					writer.currentIndentPlus();
				
				if(set_elem.parent() && set_elem.parent()->token() == Token::CLASS)
				{
					//cout<<"IN_TOKEN SCOPE_BEGIN for class: "<<set_elem.parentClassName()<<"\n";
					//cout<<"parent: "<<set_elem.parent()->toString()<<"\n";
					
					//This is the scope that begins a class.
					//inject class boilerplate here.
					injectClassBoilerPlate(writer, set_elem);
				}
				else if(set_elem.parent() && set_elem.parent()->token() == Token::CONSTRUCTOR)
				{
					//cout<<"IN_TOKEN SCOPE_BEGIN for constructor: "<<set_elem.toString()<<"\n";
					//cout<<"parent: "<<set_elem.parent()->toString()<<"\n";
					
					injectConstructorBoilerPlate(writer, set_elem);
				}
				else if(set_elem.parent() && set_elem.parent()->token() == Token::DESTRUCTOR)
				{
					//cout<<"IN_TOKEN SCOPE_BEGIN for destructor: "<<set_elem.toString()<<"\n";
					//cout<<"parent: "<<set_elem.parent()->toString()<<"\n";
					
					injectDestructorBoilerPlate(writer, set_elem);
				}


				/*for( Element* elem : elements )
				{
					elem->write(writer);
				}*/
				iterateWrite(writer, set_elem);
			break;
			case Token::SCOPE_END:
				//writer.currentIndentMinus();
				//writer.writeIndents();
				writer.lineNeedsSemicolon(false);
				/* REMOVE this didn't seem to contribute too much:
				if(set_elem.previousToken() != Token::NEWLINE
					&& set_elem.previousToken() != Token::NEWLINE_BEFORE_SCOPE_END
					&& set_elem.previousToken() != Token::SEMICOLON)
				{
					writer.writeString("; "); // Extra semicolon and a space for oneliners.
				}*/

				writer.writeString( set_elem.name() );
				if( set_elem.parentToken() == Token::CLASS || set_elem.parentToken() == Token::ENUM )
				{
					writer.writeString( ";\n\n" );

					//Need to fix C++ to have public visibility by default:
					writer.currentDefaultVisibility("public");
					writer.nextNeedsDefaultVisibility(true);
				}
				else if( set_elem.parent() && set_elem.parent()->isFunc() )
				{
					////writer.writeChar( '\n' );//one line break after func
					////writer.writeChar( '\n' );
				}
				else
				{
					/////writer.writeChar( '\n' );
				}
				/*
				for( Element* elem : elements )
				{
					elem->write(writer);
				}*/
				iterateWrite(writer, set_elem);
			break;
			case Token::NUMBER:
			case Token::FLOAT_NUMBER:
				writer.writeString(set_elem.name());
			break;
			case Token::QUOTE:
				//writer.writeChar('\"');
				writer.writeString(set_elem.name());
				//writer.writeChar('\"');
			break;
			case Token::COMMENT:
				/*
				if( writer.lineNeedsSemicolon() == true
					&& writer.previousToken() != Token::SEMICOLON
					&& writer.previousToken() != Token::NEWLINE
					&& writer.previousToken() != Token::NEWLINE_BEFORE_SCOPE_END
					//&& writer.previousToken() != Token::SCOPE_BEGIN
					//&& writer.previousToken() != Token::SCOPE_END
					//&& writer.previousToken() != Token::FUNC
					//&& writer.previousToken() != Token::DEFINE_FUNC_ARGUMENT
					//&& writer.previousToken() != Token::DEFINE_FUNC_RETURN //not actually needed...
					//&& writer.previousToken() != Token::CLASS
					//&& writer.previousToken() != Token::COMMENT
					///////&& writer.previousToken() != Token::STAR_COMMENT
					//&& writer.previousToken() != Token::VISIBILITY_DEFAULT
				)
				{
					writer.writeChar(';');
					writer.writeChar(' ');
				}

				writer.lineNeedsSemicolon(false);
				*/

				writer.writeLineEnding();

				if( writer.writeSemicolon() )
					writer.writeChar(' ');

				writer.lineNeedsSemicolon(false);

				writer.writeString("//");
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

				writer.writeString("/*");
				writer.writeString(set_elem.name());
				writer.writeString("*/");
			break;
			case Token::LOG:
				writer.writeString("std::cout");
			break;
			case Token::LOG_S:
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
				for( Element* elem : set_elem.elements )
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
				for( Element* elem : set_elem.elements )
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

				Element* first_return_elem;//we want to keep track of the first return type
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
					Element* viselem = set_elem.hasVisibilityFuncChild();

					if( viselem )
					{
						if( Element::isVisibilityNameAllowedInCpp(viselem->name()) )
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
						//Element* myelem = set_elem.elements.front();
						
						/*
						//THIS is how we used to do it. YAGNI.

						Element* myelem = set_elem.searchFirst(Token::DEFINE_FUNC_RETURN);

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
							writer.writeString( "void " );
							//writer.writeChar( ' ' );
						}
						*/

						Element* myelem = set_elem.searchFirst(Token::PARENTHESIS_BEGIN_FUNC_RETURN_TYPES);

						if( myelem != 0 )
						{
							if( myelem->token() == Token::PARENTHESIS_BEGIN_FUNC_RETURN_TYPES )
							{
								first_return_elem = myelem->nextElement();

								if(first_return_elem != 0)
								{
									if( first_return_elem->token() == Token::PARENTHESIS_END_FUNC_RETURN_TYPES )
									{
										// Nothing between parentheses. Mark void return type.
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
										
										if( set_elem.kind() == Kind::VAL )
										{
											writer.writeString( first_return_elem->typeInCpp() );
										}
										else if( set_elem.kind() == Kind::REF
											|| set_elem.kind() == Kind::OPT
											|| set_elem.kind() == Kind::LINK )
										{
											writer.writeString(set_elem.typeInCpp());
											writer.writeString("* ");
										} 
										//TODO returning Kind::TEMPLATE
										else if( set_elem.kind() == Kind::BUILT_IN_TYPE )
										{
											writer.writeString( set_elem.builtInTypeStringCpp() );
										}
										//TODO returning Kind::VECTOR
										
										writer.writeChar( ' ' );	
									}
									*/
								}
								else
								{
									ReportError::reportError("Compiler error in func return type: ", &set_elem);		
								}
							}
							else
							{
								ReportError::reportError("No return_type parenthesis in func: ", &set_elem);
							}
						}
						else
						{
							// No return type parenthesis, so mark it as void.
							writer.writeString( "void " );
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

						// We write the name of the func here:
						writer.writeString( set_elem.name() );
					}//if func
					else if( set_elem.token() == Token::CONSTRUCTOR )
					{
						if( set_elem.parent() )
						{
							if( writer.isHeader() == false ) // cpp
							{
								writer.writeString( set_elem.parent()->name() );
								writer.writeString( "::" );
							}

							writer.writeString( set_elem.parent()->name() ); // for a c++ constructor, the name of the class...
							//writer.writeString( "__new" + set_elem.parent()->name() );//for a struct "constructor"
						}
					}
					else if( set_elem.token() == Token::DESTRUCTOR )
					{
						if( set_elem.parent() )
						{
							if( writer.isHeader() == false ) // cpp
							{
								writer.writeString( set_elem.parent()->name() );
								writer.writeString( "::" );
							}

							writer.writeChar('~');
							writer.writeString( set_elem.parent()->name() ); // these two lines for c++ destructor...
							//writer.writeString( "__delete" + set_elem.parent()->name() );//for a struct "constructor"
						}
					}

				//WAS HERE BEFORE: }//just to make a nu Element*

				
				////////////////////////writer.writeChar( '(' );
				/////////writer.writeChar( ' ' );

				if( writer.isHeader() == false )//cpp
				{
					if( set_elem.token() == Token::MAIN )
					{
						writer.writeString( "(int argc, char* const argv[])" );
						/*
						for( Element* elem : set_elem.elements )
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

							for(uint i = 0; i < set_elem.elements.size(); i++)
							{
								if(i+1 < set_elem.elements.size())
								{
									//This misses some nextTokens, which are not children of
									//this Element, but maybe that doesn't matter that much,
									//they'll be members of different things anyway, and might not
									//be related.
									//writer.nextToken( set_elem.elements[i+1]->token() );
									writer.nextElement( set_elem.elements[i+1] );
								}

								/*CAN REMOVE:
								if( set_elem.elements[i]->token() == Token::DEFINE_FUNC_RETURN )
								{
									//do nothing
								}
								else if( set_elem.elements[i]->token() == Token::VISIBILITY )
								{
									//do nothing
								}
								*/
								//REMOVED: else if( set_elem.elements[i]->token() == Token::DEFINE_FUNC_ARGUMENT )
								//REMOVED: {
									//DON'T WANT THESE FOR MAIN, FOR NOW...
									
									//writer.writeString( elem->type() );
									//writer.writeChar( ' ' );
									//writer.writeString( elem->name() );
									
								//REMOVED: }
								/*REMOVE
								else if( set_elem.elements[i]->token() == Token::PARENTHESIS_BEGIN_FUNC_RETURN_TYPES )
								{
									//already written for main
								}
								else if( set_elem.elements[i]->token() == Token::PARENTHESIS_END_FUNC_RETURN_TYPES )
								{
									//already written for main
								}
								else if( set_elem.elements[i]->token() == Token::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES )
								{
									//already written for main
								}
								else if( set_elem.elements[i]->token() == Token::PARENTHESIS_END_FUNC_PARAM_TYPES )
								{
									//already written for main...

									/////////writer.writeChar( ' ' );
									//writer.writeChar( ')' );
									//writer.writeChar( '\n' );
								}

								else if(set_elem.elements[i]->token() == Token::DEFINE_REFERENCE
									&& (set_elem.elements[i]->role() == Role::FUNC_RETURN || set_elem.elements[i]->role() == Role::FUNC_PARAMETER))
								{
									*/
									//Don't want these for MAIN for now. Maybe later TODO.

									//just to check that the first return type doesn't get written twice.
									//TODO multiple return types handling.
									/*if( set_elem.elements[i] == first_return_elem )
									{
										//don't write it.
									}
									else
									{
										writeElement(writer, *set_elem.elements[i]);
										cout<<"TODO multiple return types handling.\n";
									}*/
								//}
								//else

								//don't write anything until first scope.
								if(got_first_scope == false)
								{ 
									if( set_elem.elements[i]->token() == Token::SCOPE_BEGIN )
									{
										got_first_scope = true;
										writer.writeChar('\n'); //one extra newline, because we missed it.
										writeElement(writer, *set_elem.elements[i]);
									}
								}
								else
								{									
									writeElement(writer, *set_elem.elements[i]);
								}
							}
						}
					}
				}//end isHeader cpp
				
				bool is_a_oneliner = true; // We presume the worst case scenario... :)
				bool has_param_parentheses = true;
				bool is_first_auto_init = true; // Add a colon before first AUTO_INIT in Constructors.

					if( set_elem.token() == Token::MAIN )
					{
						//do nothing for main, it's already been written...
					}
					else //a normal FUNC
					{
						/*
						for( Element* elem : set_elem.elements )
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

						Element* myparam_start = set_elem.searchFirst(Token::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES, Token::SCOPE_BEGIN);

						if( myparam_start == nullptr || myparam_start->token() == Token::SCOPE_BEGIN)
						{
							// Write void param type for C++, if we find no parentheses here.
							writer.writeString("()");
							has_param_parentheses = false;
						}


						{
							bool stop_writing_until_func_return_ends = false;

							for(size_t i = 0; i < set_elem.elements.size(); i++)
							{
								if(i+1 < set_elem.elements.size())
								{
									//This misses some nextTokens, which are not children of
									//this Element, but maybe that doesn't matter that much,
									//they'll be members of different things anyway, and might not
									//be related.
									//writer.nextToken( set_elem.elements[i+1]->token() );
									writer.nextElement( set_elem.elements[i+1] );
								}

								if(stop_writing_until_func_return_ends)
								{
									//do nothing.
									if(set_elem.elements[i]->token() == Token::PARENTHESIS_END_FUNC_RETURN_TYPES)
										stop_writing_until_func_return_ends = false;
								}
								else if( set_elem.elements[i]->token() == Token::PARENTHESIS_BEGIN_FUNC_RETURN_TYPES )
								{
									//do nothing, already written
									stop_writing_until_func_return_ends = true;
								}
								else if( set_elem.elements[i]->token() == Token::DEFINE_FUNC_RETURN )
								{
									//do nothing, already written
								}
								else if( set_elem.elements[i]->token() == Token::VISIBILITY )
								{
									//do nothing, already written
								}
								else if( set_elem.elements[i]->token() == Token::AUTO_INIT )
								{
									// Special handling to write the IMHO strange C++ class initializers, known as AUTO_INIT here.
									if( writer.isHeader() == false ) // cpp
									{
										if( is_first_auto_init == true )
										{
											is_first_auto_init = false;
											writer.writeString( ": " );
										}
										else
										{
											// A not so pretty place for the comma, but I've seen this style of code written in production C++ code by humans!
											// So the comma will be before all auto inits here.
											writer.writeString( ", " );	
										}

										writeElement(writer, *set_elem.elements[i]);
									}
								}
								//REMOVED: else if( set_elem.elements[i]->token() == Token::DEFINE_FUNC_ARGUMENT )
								/*//REMOVED: {
									cout<<"DEFINE_FUNC_ARGUMENT should be removed now We'll see.\n";
									assert(0);
									
									//if( set_elem.elements[i]->isBuiltInType() )
									if( set_elem.elements[i]->kind() == Kind::BUILT_IN_TYPE )
									{
										//cout<<"Why NOOOOOOOOOOOOOOOOOOOOOOOOOOOOT\n";
										writer.writeString( set_elem.elements[i]->typeInCpp() );
									}
									else if( set_elem.elements[i]->kind() == Kind::REF )
									{
										writer.writeString( set_elem.elements[i]->typeInCpp() );
										writer.writeChar('*');	
									}
									else if( set_elem.elements[i]->kind() == Kind::VECTOR )
									{
										writer.writeString( "std::vector<" );
										//writer.writeString( set_elem.elements[i]->typeInCpp() );
										writer.writeString( set_elem.elements[i]->templateSecondTypeStringInCpp() );
										writer.writeString( "*>*" );	
									}
									writer.writeChar(' ');
									writer.writeString( set_elem.elements[i]->name() );

									if( set_elem.initData() )
									{
										writer.writeString(" = ");
										writer.writeString( set_elem.initData()->name() );
									}
									
								}
								*/
								/*else if( set_elem.elements[i]->token() == Token::PARENTHESIS_END_FUNC_PARAM_TYPES )
								{
									/////////writer.writeChar( ' ' );
									writer.writeChar( ')' );
									///////writer.writeChar( '\n' );
								}*/
								else if( set_elem.elements[i]->token() == Token::NEWLINE)
								{
									//handle this separately because of isHeader.

									is_a_oneliner = false; // Not a oneliner.
									
									if(writer.isHeader() == true && has_param_parentheses == false) //hpp
									{
										writer.writeChar(';'); // automatic semicolon handling case
									}

									//set_elem.elements[i]->write(writer);
									writeElement(writer, *set_elem.elements[i]);

									if( writer.isHeader() == true )//hpp
									{
										//end loop. Write only one line to the header.
										i = set_elem.elements.size();
									}

								}
								else if( set_elem.elements[i]->token() == Token::SCOPE_BEGIN)
								{
									//TODO check if this is called at all!!

									if( writer.isHeader() == true )//hpp
									{
										if(is_a_oneliner == true || has_param_parentheses == false) // If we are a oneliner the automatic semicolon won't happen, because there is no newline.
										{
											writer.writeChar(';');
										}
										//end loop. Write only one line to the header.
										i = set_elem.elements.size();
									}
									else //cpp
									{
										if(is_a_oneliner == true)
										{
											writer.writeChar(' '); // Some extra spaces for oneliners.
										}
										writeElement(writer, *set_elem.elements[i]);
									}
								}
								else if( set_elem.elements[i]->token() == Token::SCOPE_END)
								{
									// This will actually never happen at all, because SCOPE_ENDs are children of SCOPE_BEGIN,
									// and they will be handled in normal case.
									if( writer.isHeader() == true )//hpp
									{
										//This should never happen, because we've already bailed out in header.
										//end loop. Write only one line to the header.
										//////assert(0);
										writer.writeString("NotSupposedToHappen1");
										i = set_elem.elements.size();	
									}
									else //cpp
									{
										if(is_a_oneliner == true)
										{
											writer.writeChar(';');
											writer.writeChar(' ');
											writer.currentIndentMinus();
										}
										writeElement(writer, *set_elem.elements[i]);
									}
								}
								/*
								else if( set_elem.elements[i]->token() == Token::PARENTHESIS_BEGIN
									|| set_elem.elements[i]->token() == Token::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES)
								{
									has_param_parentheses = true;
									writeElement(writer, *set_elem.elements[i]);
								}
								*/
								else if(set_elem.elements[i]->token() == Token::DEFINE_REFERENCE
									&& set_elem.elements[i]->role() == Role::FUNC_RETURN)
								{
									//just to check that the first return type doesn't get written twice.
									//TODO multiple return types handling.
									if( set_elem.elements[i] == first_return_elem )
									{
										//don't write it.
									}
									else
									{
										writeElement(writer, *set_elem.elements[i]);
										cout<<"TODO multiple return types handling.\n";
									}
								}
								else
								{
									/*
									debug:
									if( token() == Token::CONSTRUCTOR )
									{
										if( set_elem.elements[i]->parentToken() == Token::CLASS || set_elem.elements[i]->parentToken() == Token::ENUM )
										{
											cout << "STRANGE. This thing has set_elem.parent CLASS and not func.\n";
										}
									}
									*/

									//if( writer.isHeader() == false )//cpp
									//{
										//ok, we don't need this isHeader here, because
										//now we check for newline and end the loop.
										//set_elem.elements[i]->write(writer);

										writeElement(writer, *set_elem.elements[i]);
									//}
								}
							}
						}
					}
				}//just to make a nu Element*
				
/*
				uint count_elem = 0;
				for( Element* elem : set_elem.elements )
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
					break;
				}					
*/
			break;
			case Token::CLASS:
				//don't write the base template:
				if( set_elem.kind() == Kind::TEMPLATE )
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

					/*for( Element* elem : set_elem.elements )
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

						for(uint i = 0; i < set_elem.elements.size(); i++)
						{
							//OK. This is important:
							//Only write funcs, destructors and constructors for
							//cpp. Add an extra newline between.
							if( set_elem.elements[i]->token() == Token::FUNC
								|| set_elem.elements[i]->token() == Token::CONSTRUCTOR
								|| set_elem.elements[i]->token() == Token::DESTRUCTOR
							)
							{
								if(i+1 < set_elem.elements.size())
								{
									//This misses some nextTokens, which are not children of
									//this Element, but maybe that doesn't matter that much,
									//they'll be members of different things anyway, and might not
									//be related.
									//writer.nextToken( set_elem.elements[i+1]->token() );
									writer.nextElement( set_elem.elements[i+1] );
								}
								//set_elem.elements[i]->write(writer);
								writeElement(writer, *set_elem.elements[i]);

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
					if( Element::isVisibilityNameAllowedInCpp(set_elem.name()) )
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

					writer.writeString( Element::getVisibilityNameInCpp(set_elem.name()) );

					//writer.writeString( set_elem.name() );
					//writer.writeString( ": " );
					/////writer.writeChar( '\n' );
					//writer.writeChar( '\n' ); //let's put a line after public: ...
					/*for( Element* elem : set_elem.elements )
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
					//if( Element::isVisibilityNameAllowedInCpp(set_elem.name()) )
					//{
						writer.currentDefaultVisibility(set_elem.name());
						writer.nextNeedsDefaultVisibility(false);

						//writer.writeIndents(); //don't indent public: etc...
						if( writer.nextToken() == Token::NEWLINE )
						{
							writer.lineNeedsSemicolon(false);
						}

						writer.writeString( Element::getVisibilityNameInCpp(set_elem.name()) );

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
					/*for( Element* elem : set_elem.elements )
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
			case Token::NAMESPACE:
				writer.lineNeedsSemicolon(false);
				writer.writeString("namespace ");
				writer.writeString(set_elem.name());
				iterateWrite(writer, set_elem);
				writer.writeString(" // end namespace ");
				writer.writeString( set_elem.name() );
			break;
			case Token::USE_NAMESPACE:
				writer.writeString(set_elem.name());
				writer.writeString("::");
				//iterateWrite(writer, set_elem);
			break;

			case Token::ALIAS:
				writer.lineNeedsSemicolon(false);
				writer.writeString("#define ");
				writer.writeString(set_elem.typedefNewType());
				writer.writeChar(' ');
				writer.writeString(set_elem.typedefOldType());
			break;

			case Token::PROJECT:
				writer.lineNeedsSemicolon(false);
				/*
				writer.writeString("#if 0\nproject ");
				writer.writeString(set_elem.name());
				writer.writeChar('\n');
				//iterateWrite(writer, set_elem);
				writer.writeString("#endif\n");
				*/
			break;

			//ignore:
			case Token::IMPORT_NAME:
			case Token::MODULE_NAME:
			case Token::MODULE_DIR:
				//just ignore this type...
			break;
			case Token::IMPORT:
				//if( writer.isHeader() == true )
				{
					writer.lineNeedsSemicolon(false);
					writer.writeString( string("#include \"") );
					int not_on_first = 0;
					for( Element* elem : set_elem.elements )
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
							break;
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
				/*WHY was this here???
				else //isHeader == false //cpp
				{
					//TODO
					writer.lineNeedsSemicolon(false);

					//writer.writeString("#include \"helloworld.hpp\"\n");
					Element* last_module_name_element = set_elem.searchLast( Token::IMPORT_NAME );
					writer.writeString("#include \"");
					writer.writeString( last_module_name_element->name() );
					writer.writeString(".hpp\"");

					iterateWrite(writer, set_elem);
				}*/
			break;
			case Token::MODULE:
				if( writer.isHeader() == true )
				{
					writer.lineNeedsSemicolon(false);
					writer.writeString( string("#ifndef _") );
					for( Element* elem : set_elem.elements )
					{
						if( elem->token() == Token::MODULE_DIR || elem->token() == Token::MODULE_NAME )
						{
							writer.writeString( elem->name() );
							writer.writeChar('_');
						}
						else
						{
							break;
						}
					}
					writer.writeString( string("hpp_\n") );
					
					writer.writeString( string("#define _") );
					for( Element* elem : set_elem.elements )
					{
						if( elem->token() == Token::MODULE_DIR || elem->token() == Token::MODULE_NAME )
						{
							writer.writeString( elem->name() );
							writer.writeChar('_');
						}
						else
						{
							break;
						}
					}
					writer.writeString( string("hpp_\n\n") );
					
					writer.writeString("#include <stdint.h>\n");//for int32_t etc.
					writer.writeString("#include <iostream>\n");
					writer.writeString("#include <string>\n");
					writer.writeString("#include <vector>\n");
					writer.writeString("#include <array>\n");
					writer.writeString("#include \"rae/link.hpp\"\n\n");
					//writer.writeString("using namespace std;"); //not this... I guess.
					
					/*for( Element* elem : set_elem.elements )
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
					Element* last_module_name_element = set_elem.searchLast( Token::MODULE_NAME );
					writer.writeString("#include \"");
					writer.writeString( last_module_name_element->name() );
					writer.writeString(".hpp\"\n");

					//for( Element* elem : set_elem.elements )
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
					
					/*for( Element* elem : set_elem.elements )
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
						for( Element* elem : set_elem.parent()->elements )
						{
							if( elem->token() == Token::MODULE_DIR || elem->token() == Token::MODULE_NAME )
							{
								writer.writeString( elem->name() );
								writer.writeChar('_');
							}
							else
							{
								break;
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
			// C++ specific
			case Token::CPP_PRE_DEFINE:
				if( writer.isHeader() == true )
				{
					writer.writeString( "#define " );
					writer.writeString( set_elem.name() ); // We use the name() for the definition name
					writer.writeString( " " );
					writer.writeString( set_elem.type() ); // and the type() for the value of that definition
				}
			break;
			case Token::CPP_TYPEDEF:
				if( writer.isHeader() == true )
				{
					writer.writeString( "typedef " );
					writer.writeString( set_elem.typedefOldTypeInCpp() );
					writer.writeString( " " );
					writer.writeString( set_elem.typedefNewType() );
				}
			break;
		}
		
		//writer.previousPreviousToken( writer.previousToken() );
		//writer.previousToken( token() );
		writer.previous2ndElement( writer.previousElement() );
		writer.previousElement( &set_elem );
	}

	void writeVisibilityForElement( StringFileWriter& writer, Element& set_elem )
	{
		Element* viselem = set_elem.hasVisibilityFuncChild();
		if( viselem )
		{
			/*
			if( Element::isVisibilityNameAllowedInCpp(viselem->name()) )
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

			writer.writeString( Element::getVisibilityNameInCpp(viselem->name()) );
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
			writer.writeString( Element::getVisibilityNameInCpp(writer.currentDefaultVisibility()) );
			//writer.writeChar( ':' );
			//writer.writeChar( ' ' );
		}
	}


