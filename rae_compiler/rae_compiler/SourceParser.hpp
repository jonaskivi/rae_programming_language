
/*
Rae TODO
-? nullable
-ref is more like value. It can not be null, you can not free it. Auto free on scope end.
-easy boost signals.
-some sense into raw arrays. Do we want size? Maybe.
Then maybe it should be a struct in C++
*/


/*
template<class T>
struct _rae_array
{
	void allocate(uint set_size)
	{
		size = set_size;
		ptr = new T[size];
		init();
	}

	void init()
	{
		for(uint i = 0; i < size; i++)
		{
			ptr[i] = T();
		}
	}

	void free()
	{
		delete[] ptr;
		size = 0;
	}

	void printOut()
	{
		for(uint i = 0; i < size; i++)
		{
			std::cout<<"i: "<<i<<" : "<<ptr[i]<<"\n";
		}
	}

	T* ptr;
	uint size;
};

//using arrays in C++:
	_rae_array<float> an_array;
	_rae_array<int> int_array;
	_rae_array<Tester*> test_array;


	an_array.allocate(14);
	an_array.printOut();
	an_array.free();
	an_array.printOut();

	int_array.allocate(5);
	int_array.printOut();
	int_array.free();
	int_array.printOut();

	test_array.allocate(4);
	for(uint i = 0; i < test_array.size; i++)
	{
		std::cout<<"tester: i: "<<i<<" ";
		test_array.ptr[i]->logMe();
		std::cout<<"\n";
	}
*/


/*
TODO
currently it seems that for unknown tokens
we first look for them when they are found.
Most unknown tokens will not be found at this stage,
because most likely they have not yet been defined.
e.g. in a class function (method) when you use a member
ref and call a function in that other class.
And only later in your current class you define what that
member is:

func()someFunc()
{
	unknownMember.someOtherFunc
}

SomeOtherClass unknownMember

//

So it might be wise to just parse everything first, and
only check for unknown stuff in the end, in handleUnknownTokens??

//

TODO
for the case above it would also be optimised to:
Make a new pointer in class LangElement that points
to the definition of this object. It could be a class
or a member definition. But this way we would only need
to find it once!

E.g. in the definition of unknownMember above
we would have a pointer to the class SomeOtherClass in addition
to the (now temporary) string type = "SomeOtherClass" that would only
be used to store it until it is found. Once it is found we would then
use the pointer to the definition. But would the definition have to be
complete? Is it possible that it might still have some unknown tokens?
Anyway it would be a good system.

Then when searching for unknownMember in someFunc:
find the definition.
check if the next token is a dot, and if so, then check if the
next token is an unknown member or function call!
And also check the .someOtherFunc function call at the same time,
as we have just found the definition of unknownMember, which now
contains the link/pointer to the class definition of SomeOtherClass.
So check if SomeOtherClass contains a func or member called someOtherFunc.
That should make it all faster!!! And easier too.

This test is currently made in about line 5085.
and by using a method called classHasFunctionOrValue()


*/

class SourceParser
{
public:
//constructor:
	SourceParser()
	{
		init();
	}
	
	/*
	SourceParser(int dummy_for_stdlib)
	{
		dummy_for_stdlib = 0;//to use the dummy.
		createRaeStdLib();
	}
	*/

	SourceParser(string set_filename, bool do_parse = true)
	{
		/*if(set_filename == "rae_std_lib")//magic string...
		{
			createRaeStdLib();
		}
		else
		{
		*/

			//rae::log("creating SourceParser.\n");
			read(set_filename);
			/*
			note: we can't parse yet, because the signals are not yet connected...???

			if( do_parse == true )
			{
				parse();
			}*/
		//}
	}

	~SourceParser()
	{
		#ifdef DEBUG_RAE_DESTRUCTORS
			cout<<"~SourceParser() START: module: "<<moduleName()<<"\n";
		#endif

		//all LangElements should be owned by vector langElements
		//other vectors only have borrowed ones... I hope.
		/*foreach(LangElement* elem, langElements)
		{
			#ifdef DEBUG_RAE_DESTRUCTORS
				if(elem)
					cout<<"~SourceParser() going to delete elem: "<<elem->toString()<<"\n";
				else cout<<"~SourceParser() already deleted elem.\n";
			#endif
			if(elem)
				delete elem;
		}*/
		for(uint i = 0; i < langElements.size(); i++)
		{
			if(langElements[i] && langElements[i]->parent() == 0)
			{
				#ifdef DEBUG_RAE_DESTRUCTORS
					cout<<"~SourceParser() going to delete elem: "<<langElements[i]->toString()<<"\n";
				#endif
				delete langElements[i];
			}
			else
			{
				#ifdef DEBUG_RAE_DESTRUCTORS
					cout<<"~SourceParser() DID NOT delete object."<<"\n";
				#endif
			}
		}	
		langElements.clear();

		//here's all vectors I can remember. But I guess we wouldn't need to clear them.
		unknownDefinitions.clear();
		unknownUseReferences.clear();
		unknownUseMembers.clear();

		#ifdef DEBUG_RAE_DESTRUCTORS
			cout<<"~SourceParser() END: module: "<<moduleName()<<"\n";
		#endif
	}

	void init()
	{
		lineNumber.line = 1;//linenumbers start from 1, but we keep it default to 0 to see if it's really been set.

		debugWriteLineNumbers = true;//false;

		stringIndex = 0;
		isWriteToFile = true;
		isFileBased = true;

		isDone = false;
		isWholeToken = false;
		isWholeToken2 = false;
		isEndOfLine = false;
		isSingleLineComment = false;
		//m_returnToExpectToken = LangTokenType::UNDEFINED;
		m_expectingToken = LangTokenType::UNDEFINED;
		m_expectingTypeType = TypeType::UNDEFINED;
		expectingChar = 'T';
		foundToken = LangTokenType::UNDEFINED;

		//isInsideLogStatement = false;

		isQuoteReady = false;
		isWaitingForQuoteEnd = false;
		
		isStarCommentReady = false;
		isWaitingForStarCommentEnd = false;
		
		isPlusCommentReady = false;
		nroWaitingForPlusCommentEnd = 0;


		m_currentParentElement = 0;
		currentModule = 0;
		currentClass = 0;
		currentFunc = 0;
		currentEnum = 0;
		currentTemplate = 0;
		currentReference = 0;
		currentTempElement = 0;

		countWarnings = 0;
		countErrors = 0;
	}

	bool debugWriteLineNumbers;//= false
	
	public: LangTokenType::e doReturnToExpectToken()
	{
		if( returnToExpectTokenStack.empty() )
		{
			m_expectingToken = LangTokenType::UNDEFINED;
			return m_expectingToken;
		}

		//else
		m_expectingToken = returnToExpectTokenStack.back();
		returnToExpectTokenStack.pop_back();
		return m_expectingToken;
		//return m_returnToExpectToken;
	}
	public: void addReturnToExpectToken(LangTokenType::e set)
	{
		returnToExpectTokenStack.push_back(set);
		//m_returnToExpectToken = set;
	}
	public: LangTokenType::e returnToExpectToken()//don't use this... just for if you need it to check what it is.
	{
		if( returnToExpectTokenStack.empty() )
			return LangTokenType::UNDEFINED;

		//else
		return returnToExpectTokenStack.back();
		//return m_returnToExpectToken;
	}
	//protected: LangTokenType::e m_returnToExpectToken;
	protected: vector<LangTokenType::e> returnToExpectTokenStack;
	public: LangTokenType::e expectingToken(){ return m_expectingToken; }
	public: void expectingToken(LangTokenType::e set){ m_expectingToken = set; }
	protected: LangTokenType::e m_expectingToken;// = EDLTokenType::TITLE;

	//secondary expecting
	public: TypeType::e expectingTypeType(){ return m_expectingTypeType; }
	public: void expectingTypeType(TypeType::e set){ m_expectingTypeType = set; }
	protected: TypeType::e m_expectingTypeType;

	public:
	int expectingChar;// = 'T';
	LangTokenType::e foundToken;// = EDLTokenType::UNDEFINED;
	
	bool isSingleLineComment;// = false;
	
	//for handling quotes.
	bool isQuoteReady;
	bool isWaitingForQuoteEnd;

	string currentQuote;

	//a system to help with LOG commas... LOG_SEPARATORs //replaced this with a call to matchParenthesis and some checking in ","
	//bool isInsideLogStatement;

	/*for handling star comments like this one.*/
	bool isStarCommentReady;
	bool isWaitingForStarCommentEnd;
	string currentStarComment;

	/* plus comments are quite like star comments, but they override them.
	And you need a matching number to end them, so you can have them inside each other.*/
	bool isPlusCommentReady;
	int nroWaitingForPlusCommentEnd;
	string currentPlusComment;

	string currentModuleHeaderFileName;//e.g. HelloWorld.hpp, we pass this into the cpp writer.

	//low level parser:

	int currentChar;
	
	//two systems doing basically the same thing. TODO for refactoring:
	int beforeChar;
	string handleSlash;
	


	bool isWholeToken;
	string wholeToken;
	string currentWord;
	
	bool isWholeToken2;
	string wholeToken2;

	bool isEndOfLine;
	string endOfLine;
	string currentLine;

	string currentVisibility;
	
	bool isDone;
	
	bool fileParsedOk;

	LangElement* newLangElement(LangTokenType::e set_lang_token_type, TypeType::e set_type_type, string set_name = "", string set_type = "")
	{
		LangElement* lang_elem;
		
		if( currentParentElement() )
		{
			lang_elem = currentParentElement()->newLangElement(lineNumber, set_lang_token_type, set_type_type, set_name, set_type);
		
		//else if( currentModule )
		//{
		
			//lang_elem = currentModule->newLangElement(lineNumber, set_lang_token_type, set_type_type, set_name, set_type);
			if( set_lang_token_type == LangTokenType::MODULE
				|| set_lang_token_type == LangTokenType::CLASS
				|| set_lang_token_type == LangTokenType::FUNC
				)
			{
				currentParentElement(lang_elem);
			}
			//langElements.push_back( lang_elem );
			//#ifdef DEBUG_RAE
			//	cout<<"BASE Add langElement: "<<LangTokenType::toString(set_lang_token_type)<<" name:>"<<set_name<<"< type:"<<set_type<<"\n");
			//#endif
		}
		else
		{
			reportError("newLangElement() No current parent element found. Do you have a module? Compilers fault. Something went wrong.");
			cout<<"tried to create:"<<LangTokenType::toString(set_lang_token_type)<<" name: "<<set_name<<" type: "<<set_type<<"\n";
		}

		if(lang_elem)
		{
			lang_elem->previousElement( previousElement() );
			previousElement()->nextElement(lang_elem);
		
		//previous2ndElement = previousElement;
		
			previousElement(lang_elem);//this is where we set previousElement.
		
		//Hmm. Now that I think of it. previousElement and currentParentElement are the same! We'll there might be some differences because of those ifs above...

		}

		return lang_elem;

		/*
		LangElement* lang_elem = new LangElement(lineNumber, set_lang_token_type, set_name, set_type);
		currentParentElement() = lang_elem;
		langElements.push_back( lang_elem );
		cout<<"BASE Add langElement: "<<LangTokenType::toString(set_lang_token_type)<<" name:>"<<set_name<<"< type:"<<set_type<<"\n");
		return lang_elem;
		*/
	}

	void newScopeBegin()
	{
		/*if( currentParentElement() )
		{
			currentParentElement()->newLangElement(LangTokenType::SCOPE_BEGIN, "{");
			cout<<"Begin scope for: "<<LangTokenType::toString( currentParentElement()->langTokenType() );
		}
		else
		{
			newLangElement(LangTokenType::SCOPE_BEGIN, "{");
		}
		*/

		/*

		class <- put to scopeElementsStack, become a currentParentElement
		{
			func <- put to scopeElementsStack, become a currentParentElement
			{
				something

				{ <- put to scopeElementsStack, become a currentParentElement
					inside a limiting scope, we have our own sub-namespace.
				}
			}
		}

		*/


		LangElement* our_scope_elem = newLangElement(LangTokenType::SCOPE_BEGIN, TypeType::UNDEFINED, "{");
		
		//If we're already inside a func. Then we create a so called limiting scope, which will act as parent for
		//it's insides.
		//This only works if the parent is a func. Otherwise it wouldn't make any sense, or would it?
		//if( currentParentElement() && currentParentElement()->langTokenType() == LangTokenType::FUNC )
		if( scopeElementStack.empty() == false )
		{
			//if( scopeElementStack.back()->langTokenType() == LangTokenType::FUNC )
			//{
				currentParentElement(our_scope_elem);
			//}
		}

		//put the scope or class or func object to scope stack.
		scopeElementStack.push_back( currentParentElement() ); //NOT true anymore: but we put the class or func (or limiting scope) in the stack. Not the actual scope object (unless it is a limiting scope.)
		
	}

	void newScopeEnd()
	{
		LangElement* scope_elem = scopeElementStack.back();
		
		if( scope_elem )
		{
			scope_elem->freeOwned();
			//newLine();
		}

		if( currentParentElement() && currentParentElement()->currentElement() && currentParentElement()->currentElement()->langTokenType() == LangTokenType::NEWLINE )
		{
			//REVIEW: This code (currentParentElement()->currentElement()) looks very strange. I wonder why it works!

			//we had a newline. mark it.
			currentParentElement()->currentElement()->langTokenType(LangTokenType::NEWLINE_BEFORE_SCOPE_END);
		}
		
		if( scope_elem )
		{
			LangElement* new_lang_elem = scope_elem->newLangElement(lineNumber, LangTokenType::SCOPE_END, TypeType::UNDEFINED, "}" );

			if(new_lang_elem)
			{
				new_lang_elem->previousElement(previousElement());
				previousElement()->nextElement(new_lang_elem);
			}

			if( scope_elem->langTokenType() == LangTokenType::CLASS )
			{
				#ifdef DEBUG_RAE_HUMAN
				cout<<"end of class.\n\n";
				#endif

				//class ends, do stuff:

				//if there are no constructors then add one.
				if( listOfDestructors.empty() == true )
				{
					
					LangElement* a_con = scope_elem->newLangElementToTopOfClass(lineNumber, LangTokenType::DESTRUCTOR, TypeType::UNDEFINED, "free" );
					listOfDestructors.push_back(a_con);
					a_con->newLangElement(lineNumber, LangTokenType::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES);
					a_con->newLangElement(lineNumber, LangTokenType::PARENTHESIS_END_FUNC_PARAM_TYPES);
					a_con->newLangElement(lineNumber, LangTokenType::NEWLINE);
					LangElement* a_scop = a_con->newLangElement(lineNumber, LangTokenType::SCOPE_BEGIN, TypeType::UNDEFINED, "{");
					a_scop->newLangElement(lineNumber, LangTokenType::NEWLINE_BEFORE_SCOPE_END);
					a_scop->newLangElement(lineNumber, LangTokenType::SCOPE_END, TypeType::UNDEFINED, "}");
					//a_con->newLangElement(lineNumber, LangTokenType::NEWLINE);
					//a_con->newLangElement(lineNumber, LangTokenType::NEWLINE);
				
				}

				if( listOfConstructors.empty() == true )
				{
					#ifdef DEBUG_RAE_HUMAN
					cout<<"Creating a constructor.\n";
					#endif

					LangElement* a_con = scope_elem->newLangElementToTopOfClass(lineNumber, LangTokenType::CONSTRUCTOR, TypeType::UNDEFINED, "new" );
					listOfConstructors.push_back(a_con);
					a_con->newLangElement(lineNumber, LangTokenType::PARENTHESIS_BEGIN_FUNC_RETURN_TYPES);
					a_con->newLangElement(lineNumber, LangTokenType::PARENTHESIS_END_FUNC_RETURN_TYPES);
					a_con->newLangElement(lineNumber, LangTokenType::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES);
					a_con->newLangElement(lineNumber, LangTokenType::PARENTHESIS_END_FUNC_PARAM_TYPES);
					a_con->newLangElement(lineNumber, LangTokenType::NEWLINE);
					LangElement* a_scop = a_con->newLangElement(lineNumber, LangTokenType::SCOPE_BEGIN, TypeType::UNDEFINED, "{");
					a_scop->newLangElement(lineNumber, LangTokenType::NEWLINE_BEFORE_SCOPE_END);
					a_scop->newLangElement(lineNumber, LangTokenType::SCOPE_END, TypeType::UNDEFINED, "}");
					//a_con->newLangElement(lineNumber, LangTokenType::NEWLINE);
					//a_con->newLangElement(lineNumber, LangTokenType::NEWLINE);
					
				}

				if( listOfConstructors.empty() == false )
				{
					if( listOfAutoInitObjects.empty() == false )
					{
						foreach( LangElement* elem, listOfConstructors )
						{
							foreach( LangElement* init_ob, listOfAutoInitObjects )
							{
								if( init_ob )
								{
									/*
									if( init_ob->langTokenType() == LangTokenType::DEFINE_VECTOR_IN_CLASS )
									{
										elem->newLangElementToTopWithNewline( elem->lineNumber(), LangTokenType::VECTOR_AUTO_INIT, init_ob->name(), init_ob->type() );
									}
									else if( init_ob->langTokenType() == LangTokenType::DEFINE_ARRAY_IN_CLASS )
									{
										elem->newLangElementToTopWithNewline( elem->lineNumber(), LangTokenType::C_ARRAY_AUTO_INIT, init_ob->name(), init_ob->type() );
									}
									*/
									/*
									if( init_ob->typeType() == TypeType::VECTOR )
									{
										elem->newLangElementToTopWithNewline( elem->lineNumber(), LangTokenType::VECTOR_AUTO_INIT, TypeType::VECTOR, init_ob->name(), init_ob->type() );
									}
									else if( init_ob->typeType() == TypeType::TEMPLATE )
									{
										elem->newLangElementToTopWithNewline( elem->lineNumber(), LangTokenType::TEMPLATE_AUTO_INIT, TypeType::TEMPLATE, init_ob->name(), init_ob->type() );
									}
									else if( init_ob->typeType() == TypeType::C_ARRAY )
									{
										elem->newLangElementToTopWithNewline( elem->lineNumber(), LangTokenType::C_ARRAY_AUTO_INIT, TypeType::C_ARRAY, init_ob->name(), init_ob->type() );
									}
									else
									{	
										elem->newLangElementToTopWithNewline( elem->lineNumber(), LangTokenType::OBJECT_AUTO_INIT, TypeType::REF, init_ob->name(), init_ob->type() );
									}*/
									LangElement* auto_init_elem = init_ob->copy();
									auto_init_elem->langTokenType(LangTokenType::AUTO_INIT);
									elem->addElementToTopOfFunc(auto_init_elem);
								}
							}
						}
					}

					if( listOfBuiltInTypesInit.empty() == false )
					{
						foreach( LangElement* elem, listOfConstructors )
						{
							foreach( LangElement* init_ob, listOfBuiltInTypesInit )
							{		
								//LangElement* bui_le = elem->newLangElementToTopWithNewline( elem->lineNumber(), LangTokenType::BUILT_IN_TYPE_AUTO_INIT, TypeType::BUILT_IN_TYPE, init_ob->name(), init_ob->type(), init_ob->builtInType() );//TODO value...
								
								LangElement* auto_init_elem = init_ob->copy();
								auto_init_elem->langTokenType(LangTokenType::AUTO_INIT);
								elem->addElementToTopOfFunc(auto_init_elem);
								auto_init_elem->initData( init_ob->initData() );//Copy initdata!!
								
								//elem->addElementToTopWithNewline(init_ob);//add the raw pointer, beware of cyclic loops with this stuff.
							}
						}
					}
				}
				

				if( listOfDestructors.empty() == false )
				{
					if( listOfAutoInitObjects.empty() == false )
					{
						foreach( LangElement* elem, listOfDestructors )
						{
							foreach( LangElement* init_ob, listOfAutoInitObjects )
							{
								if( init_ob )
								{
									/*
									if( init_ob->langTokenType() == LangTokenType::DEFINE_VECTOR_IN_CLASS )
									{
										elem->newLangElementToTopWithNewline( elem->lineNumber(), LangTokenType::VECTOR_AUTO_FREE, init_ob->name(), init_ob->type() );
									}
									else if( init_ob->langTokenType() == LangTokenType::DEFINE_ARRAY_IN_CLASS )
									{
										elem->newLangElementToTopWithNewline( elem->lineNumber(), LangTokenType::C_ARRAY_AUTO_FREE, init_ob->name(), init_ob->type() );
									}
									*/
									/*
									if( init_ob->typeType() == TypeType::VECTOR )
									{
										elem->newLangElementToTopWithNewline( elem->lineNumber(), LangTokenType::VECTOR_AUTO_FREE, TypeType::VECTOR, init_ob->name(), init_ob->type() );
									}
									else if( init_ob->typeType() == TypeType::TEMPLATE )
									{
										elem->newLangElementToTopWithNewline( elem->lineNumber(), LangTokenType::TEMPLATE_AUTO_FREE, TypeType::TEMPLATE, init_ob->name(), init_ob->type() );
									}
									else if( init_ob->typeType() == TypeType::C_ARRAY )
									{
										elem->newLangElementToTopWithNewline( elem->lineNumber(), LangTokenType::C_ARRAY_AUTO_FREE, TypeType::C_ARRAY, init_ob->name(), init_ob->type() );
									}
									else
									{
										elem->newLangElementToTopWithNewline( elem->lineNumber(), LangTokenType::OBJECT_AUTO_FREE, TypeType::REF, init_ob->name(), init_ob->type() );
									}*/
									LangElement* auto_init_elem = init_ob->copy();
									auto_init_elem->langTokenType(LangTokenType::AUTO_FREE);
									elem->addElementToTopOfFunc(auto_init_elem);
								}
							}
						}
					}
				}

				//Clear all temporary lists related to this class:
				listOfConstructors.clear();
				listOfDestructors.clear();
				listOfAutoInitObjects.clear();
				listOfBuiltInTypesInit.clear();

				//class ends with this }. So we mark that we are not inside a class definition now.
				//Ok, but we might have classes inside classes so we check later when we pop, if it's another class...
				currentClass = 0;
			}
			else if( scope_elem->langTokenType() == LangTokenType::FUNC )
			{
				#ifdef DEBUG_RAE_HUMAN
				cout<<"end of function.\n\n";
				#endif
				currentFunc = 0;
			}
			else if( scope_elem->langTokenType() == LangTokenType::ENUM )
			{
				currentEnum = 0;
			}

			#ifdef DEBUG_RAE
			cout<<"End scope for: "<<LangTokenType::toString( scope_elem->langTokenType() );
			//rae::log("End scope for: ",LangTokenType::toString( scope_elem->langTokenType() ));
			#endif
		}
		else
		{
			newLangElement(LangTokenType::SCOPE_END, TypeType::UNDEFINED, "}");
 		}
	 	
 		scopeElementStack.pop_back();
		if( scopeElementStack.empty() == false )
		{
			currentParentElement( scopeElementStack.back() );
			if( currentParentElement() )
			{
				if( currentParentElement()->langTokenType() == LangTokenType::CLASS )
				{
					//...so if the current scope is another class, we mark it again.
					currentClass = currentParentElement();
				}
				else if( currentParentElement()->langTokenType() == LangTokenType::FUNC )
				{
					currentFunc = currentParentElement();
				}
				else if( currentParentElement()->langTokenType() == LangTokenType::ENUM )
				{
					currentEnum = currentParentElement();
				}
			}
		}
		else if( currentModule )
		{
			//now also the module can be the parent element.
			currentParentElement(currentModule);
		}
		else
		{
			//currentParentElement(0);
			cout<<"error in scopeEnd. no currentModule!!!!!!\n";
		}

	}

	LangElement* newParenthesisBegin(LangTokenType::e set_lang_token_type, string set_token)
	{
		LangElement* lang_elem = newLangElement(set_lang_token_type, TypeType::UNDEFINED, set_token);
		//parenthesisStack.push_back(currentParentElement());
		parenthesisStack.push_back(lang_elem);

		//NOTE: we push the parenthesis_begin elements to the stack!
		//not the currentParentElements like in the scopeElementsStack...!
		return lang_elem;
	}

	//a small helper, just so we can get the token easily...
	LangTokenType::e parenthesisStackTokenType()
	{
		LangElement* stack_elem = parenthesisStack.back();

		if(stack_elem)
		{
			return stack_elem->langTokenType();
		}
		//else
		return LangTokenType::UNDEFINED;
	}

	LangElement* newParenthesisEnd(LangTokenType::e set_lang_token_type, string set_token)
	{
		LangElement* stack_elem = parenthesisStack.back();

		LangElement* lang_elem;

		if( stack_elem )
		{
			//We just do matching: is it good enough:

			lang_elem = newLangElement(LangTokenType::matchParenthesisEnd(stack_elem->langTokenType()), TypeType::UNDEFINED, set_token);

			#ifdef DEBUG_RAE
			/*cout<<"MATCH parenthesis: from: "<<LangTokenType::toString(stack_elem->langTokenType())
				<<" to: "<<LangTokenType::toString(LangTokenType::matchParenthesisEnd(stack_elem->langTokenType()))
				<<" caller: "<<LangTokenType::toString(set_lang_token_type)<<"\n");*/
			//rae::log("match parenthesis...\n");
			#endif

			/*
			if( stack_elem->langTokenType() == LangTokenType::PARENTHESIS_BEGIN_LOG )
			{
				stack_elem->newLangElement(lineNumber, set_lang_token_type, set_token );
			}
			else
			{
				stack_elem->newLangElement(lineNumber, set_lang_token_type, set_token );
			}
			*/
			
			//cout<<"End parenthesis for: "<<LangTokenType::toString( scope_elem->langTokenType() );
		}
		else
		{
			lang_elem = newLangElement(set_lang_token_type, TypeType::UNDEFINED, set_token);
 		}
	 	
 		parenthesisStack.pop_back();

 		return lang_elem;
	}

	LangElement* newBracketBegin(LangTokenType::e set_lang_token_type, string set_token)
	{
		LangElement* lang_elem = newLangElement(set_lang_token_type, TypeType::UNDEFINED, set_token);
		//bracketStack.push_back(currentParentElement());
		bracketStack.push_back(lang_elem);

		//cout<<"bracketStack.push_back() called.\n");

		//NOTE: we push the bracket_begin elements to the stack!
		//not the currentParentElements like in the scopeElementsStack...!
		return lang_elem;
	}

	LangElement* newBracketEnd(LangTokenType::e set_lang_token_type, string set_token)
	{
		LangElement* stack_elem = bracketStack.back();

		if( bracketStack.empty() == true )
		{
			stack_elem = 0;//null

			reportError("No matching starting [ for ]. Missing bracket.");
		}

		LangElement* lang_elem;

		if( stack_elem )
		{
			//cout<<"we has stack_elem in newBracketEnd.\n");
			//We just do matching: is it good enough:

			lang_elem = newLangElement(LangTokenType::matchBracketEnd(stack_elem->langTokenType()), TypeType::UNDEFINED, set_token);

			//cout<<"I bet we've crashed.\n");


			#ifdef DEBUG_RAE
			cout<<"MATCH bracket: from: "<<LangTokenType::toString(stack_elem->langTokenType())
				<<" to: "<<LangTokenType::toString(LangTokenType::matchBracketEnd(stack_elem->langTokenType()))
				<<" caller: "<<LangTokenType::toString(set_lang_token_type)<<"\n";
			//rae::log("match bracket.\n");
			#endif

			/*
			if( stack_elem->langTokenType() == LangTokenType::bracket_BEGIN_LOG )
			{
				stack_elem->newLangElement(lineNumber, set_lang_token_type, set_token );
			}
			else
			{
				stack_elem->newLangElement(lineNumber, set_lang_token_type, set_token );
			}
			*/
			
			//cout<<"End bracket for: "<<LangTokenType::toString( scope_elem->langTokenType() );
		}
		else
		{
			lang_elem = newLangElement(set_lang_token_type, TypeType::UNDEFINED, set_token);
 		}
	 	
 		bracketStack.pop_back();

 		return lang_elem;
	}
/*
	LangElement* newImport(string set_name)
	{
		LangElement* lang_elem;
		
		if( currentModule )
		{
			lang_elem = currentModule->newLangElement(lineNumber, LangTokenType::IMPORT, TypeType::UNDEFINED, set_name);
			currentParentElement(lang_elem);
			
			langElements.push_back( lang_elem );
			#ifdef DEBUG_RAE
				cout<<"newImport: "<<LangTokenType::toString(LangTokenType::IMPORT)<<" name:>"<<set_name<<"\n";
				//rae::log("newImport: ",LangTokenType::toString(LangTokenType::IMPORT)," name:>",set_name<<"\n");
			#endif

			//previous2ndElement = previousElement;
			previousElement(lang_elem);

			//currentModule = lang_elem;
			//currentParentElement() = lang_elem;
			currentTempElement = lang_elem;

			expectingToken(LangTokenType::IMPORT_NAME);

		}
		else 
		{
			reportError("newImport() No current module found. Compilers fault. Something went wrong.");
		}

		return lang_elem;
	}
*/

	LangElement* newImport(string set_name)
	{
		LangElement* lang_elem;
		
		lang_elem = newLangElement(LangTokenType::IMPORT, TypeType::UNDEFINED, set_name);
			
			#ifdef DEBUG_RAE
				cout<<"newImport: "<<LangTokenType::toString(LangTokenType::IMPORT)<<" name:>"<<set_name<<"\n";
				//rae::log("newImport: ",LangTokenType::toString(LangTokenType::IMPORT)," name:>",set_name<<"\n");
			#endif

		currentTempElement = lang_elem;

		expectingToken(LangTokenType::IMPORT_NAME);

		return lang_elem;
	}

	LangElement* newModule(string set_name)
	{
		//LangElement* lang_elem = newLangElement(LangTokenType::MODULE, set_name);

		LangElement* lang_elem;
		
		lang_elem = new LangElement(lineNumber, LangTokenType::MODULE, TypeType::UNDEFINED, set_name);
		currentParentElement(lang_elem);
			
			langElements.push_back( lang_elem );
			#ifdef DEBUG_RAE
				cout<<"BASE Add module langElement: "<<LangTokenType::toString(LangTokenType::MODULE)<<" name:>"<<set_name<<"\n";
				//rae::log("BASE Add module langElement: ",LangTokenType::toString(LangTokenType::MODULE)," name:>",set_name<<"\n");
			#endif

		//previous2ndElement = previousElement;
		previousElement(lang_elem);

		currentModule = lang_elem;
		//currentParentElement() = lang_elem;

		expectingToken(LangTokenType::MODULE_NAME);

		return lang_elem;
	}

	/*
	//old version put the closemodule to top level in hierarchy.
	LangElement* closeModule()
	{
		LangElement* lang_elem = newLangElement(LangTokenType::CLOSE_MODULE);
		if( currentModule && lang_elem )
		{
			#ifdef DEBUG_RAE
			cout<<"COPYING currentModule to closeModule.\n");
			#endif
			lang_elem->copyChildElementsFrom(*currentModule);
		}
		//currentModule = lang_elem;
		//currentParentElement() = lang_elem;
		return lang_elem;
	}
	*/

	LangElement* closeModule()
	{
        LangElement* lang_elem;
		//LangElement* lang_elem = newLangElement(LangTokenType::CLOSE_MODULE);
		//if( currentModule && lang_elem )
		if( currentModule )
		{
			//let's put the closeModule to be the last element in module...
			
			lang_elem = currentModule->newLangElement(lineNumber, LangTokenType::CLOSE_MODULE, TypeType::UNDEFINED);

			//#ifdef DEBUG_RAE
			//cout<<"COPYING currentModule to closeModule.\n");
			//#endif
			///////lang_elem->copyChildElementsFrom(*currentModule);
		}
		//currentModule = lang_elem;
		//currentParentElement() = lang_elem;
		return lang_elem;
	}
/*
//Old newClass which puts the classes to the same level as modules in the
//hierarchy...
	LangElement* newClass(string set_name)
	{
		LangElement* lang_elem = newLangElement(LangTokenType::CLASS, set_name);
		currentClass = lang_elem;
		addToUserDefinedTokens(lang_elem);
		//currentParentElement() = lang_elem; //already did this inside newLangElement...
		return lang_elem;
	}
*/	
/*
	LangElement* newClass(string set_name)
	{
		LangElement* lang_elem;
		if( currentModule )
		{
			//Hmm, we put the name of the class into the name AND type params!!!
			lang_elem = currentModule->newLangElement(lineNumber, LangTokenType::CLASS, TypeType::REF, set_name, set_name);
			currentParentElement(lang_elem);//Hmm... we should make this easier...
			currentClass = lang_elem;

			addToClasses(lang_elem);
			addToUserDefinedTokens(lang_elem);
			
			#ifdef DEBUG_RAE_HUMAN
				cout<<"\n\nnewClass: "<<set_name<<"\n";
				//rae::log("newClass: ",set_name,"\n");
			#endif
		}
		else //no class, so we just make a global class... Um... this shouldn't happen. We have to have a module.
			//This is an error.
		{
			//lang_elem = newLangElement(LangTokenType::CLASS, set_name);
			//currentFunc = lang_elem;
			reportError("newClass() No current module found. Compilers fault. Something went wrong.");
		}

		return lang_elem;
	}
*/
	LangElement* newClass(string set_name)
	{
		LangElement* lang_elem;
		//Hmm, we put the name of the class into the name AND type params!!!
		lang_elem = newLangElement(LangTokenType::CLASS, TypeType::REF, set_name, set_name);
		currentClass = lang_elem;

		addToClasses(lang_elem);
		addToUserDefinedTokens(lang_elem);
		
		#ifdef DEBUG_RAE_HUMAN
			cout<<"\n\nnewClass: "<<set_name<<"\n";
			//rae::log("newClass: ",set_name,"\n");
		#endif
		
		return lang_elem;
	}
/*
	LangElement* newFunc(string set_name = "")
	{
		LangElement* lang_elem;
		if( currentClass )
		{
			lang_elem = currentClass->newLangElement(lineNumber, LangTokenType::FUNC, TypeType::UNDEFINED, set_name);
			currentParentElement(lang_elem);//Hmm... we should make this easier...
			currentFunc = lang_elem;
			//currentParentElement() = lang_elem;
		}
		else //no class, so we just make a global func...
		{
			lang_elem = newLangElement(LangTokenType::FUNC, TypeType::UNDEFINED, set_name);
			currentFunc = lang_elem;
		}
		addToUserDefinedTokens(lang_elem);

		return lang_elem;
	}
*/

	LangElement* newFunc(string set_name = "")
	{
		LangElement* lang_elem;
		lang_elem = newLangElement(LangTokenType::FUNC, TypeType::UNDEFINED, set_name);
		currentFunc = lang_elem;
		
		addToUserDefinedTokens(lang_elem);

		return lang_elem;
	}

	LangElement* newQuote(string set_name = "")
	{
		/*LangElement* lang_elem;
		if( currentParentElement() )
		{
			lang_elem = currentParentElement()->newLangElement(LangTokenType::QUOTE, set_name);
			//currentParentElement() = lang_elem;//Hmm... we should make this easier...
			//currentFunc = lang_elem;
			//currentParentElement() = lang_elem;
		}
		else //no current element, so we just make a comment...
		{
			lang_elem = newLangElement(LangTokenType::QUOTE, set_name);
		}
		return lang_elem;*/

		#ifdef DEBUG_RAE_HUMAN
			cout<<"newQuote: "<<set_name<<"\n";
			//rae::log("newQuote: ",set_name,"\n");
		#endif
		
		return newLangElement(LangTokenType::QUOTE, TypeType::UNDEFINED, set_name);		
	}
	
	LangElement* newPlusComment(string set_name = "")
	{
		#ifdef DEBUG_RAE_HUMAN
			cout<<"newPlusComment: "<<set_name<<"\n";
			//rae::log("newPlusComment: ",set_name,"\n");
		#endif

		return newLangElement(LangTokenType::PLUS_COMMENT, TypeType::UNDEFINED, set_name);		
	}

	LangElement* newStarComment(string set_name = "")
	{
		#ifdef DEBUG_RAE_HUMAN
			cout<<"newStarComment: "<<set_name<<"\n";
			//rae::log("newStarComment: ",set_name,"\n");
		#endif

		return newLangElement(LangTokenType::STAR_COMMENT, TypeType::UNDEFINED, set_name);		
	}	

	LangElement* newComment(string set_name)
	{
		/*LangElement* lang_elem;
		if( currentParentElement() )
		{
			lang_elem = currentParentElement()->newLangElement(LangTokenType::COMMENT, set_name);
			//currentParentElement() = lang_elem;//Hmm... we should make this easier...
			currentFunc = lang_elem;
			//currentParentElement() = lang_elem;
		}
		else //no current element, so we just make a comment...
		{
			lang_elem = newLangElement(LangTokenType::COMMENT, set_name);
		}
		return lang_elem;
		*/
	
		#ifdef DEBUG_RAE_HUMAN
			cout<<"newComment: "<<set_name<<"\n";
			//rae::log("newComment: ",set_name,"\n");
		#endif

		return newLangElement(LangTokenType::COMMENT, TypeType::UNDEFINED, set_name);
	}

	void newLine()
	{
		/*if( currentParentElement() )
		{
			currentParentElement()->newLangElement(LangTokenType::NEWLINE, "\n");
			//cout<<"Begin scope for: "<<LangTokenType::toString( currentParentElement()->langTokenType() );
		}
		else
		{
			cout<<"newLine() AGAIN added to the BASE level.\n");
			newLangElement(LangTokenType::NEWLINE, "\n");
		}*/
		
		//if()
		if(currentParentElement() && currentParentElement()->langTokenType() != LangTokenType::MODULE )
		{
			newLangElement(LangTokenType::NEWLINE, TypeType::UNDEFINED, "\n");
		}
		else if( previousToken() == LangTokenType::COMMENT )
		{
			//if it's a comment, then do the newline anyway...
			newLangElement(LangTokenType::NEWLINE, TypeType::UNDEFINED, "\n");	
		}
	}


	LangElement* newDefineBuiltInType(BuiltInType::e set_built_in_type, string set_type, string set_name = "")
	{
		//LangElement* lang_elem = newLangElement(LangTokenType::DEFINE_BUILT_IN_TYPE, set_name, set_type);
		LangElement* lang_elem = newLangElement(LangTokenType::DEFINE_REFERENCE, TypeType::BUILT_IN_TYPE, set_name, set_type);
		lang_elem->builtInType(set_built_in_type);
		//lang_elem->typeType(TypeType::BUILT_IN_TYPE);
		
		currentReference = lang_elem;

		if( lang_elem->parentLangTokenType() == LangTokenType::CLASS )
		{
			//we are inside a class definition, not a func:
			//lang_elem->langTokenType( LangTokenType::matchBuiltIntTypesToInClass(set_lang_token_type) );
			/////lang_elem->langTokenType( LangTokenType::DEFINE_BUILT_IN_TYPE_IN_CLASS );

			listOfBuiltInTypesInit.push_back(lang_elem);			
		}

		addToUserDefinedTokens(lang_elem);

		return lang_elem;
	}

//using newUseReference for the time being.

	LangElement* newUseBuiltInType(LangElement* set_elem)
	{
		if( set_elem == 0 )
			return 0;

		//cout<<"newUseReference(set_elem) "<<set_elem->toString()<<"\n");
		
		//LangElement* lang_elem = newLangElement(LangTokenType::USE_BUILT_IN_TYPE, set_elem->name(), set_elem->type() );
		LangElement* lang_elem = newLangElement(LangTokenType::USE_REFERENCE, TypeType::BUILT_IN_TYPE, set_elem->name(), set_elem->type() );
		
		#ifdef DEBUG_RAE_HUMAN
			cout<<"newUseBuiltInType: "<<set_elem->toString()<<"\n";
			//rae::log("newUseReference: ",set_elem->toString(),"\n");
		#endif

		return lang_elem;
	}


	LangElement* newDefineVector(string set_template_second_type, string set_name = "")
	{
		#ifdef DEBUG_RAE_HUMAN
		cout<<"newDefineVector() : set_template_second_type: "<<set_template_second_type<<" set_name: "<<set_name<<" line: "<<lineNumber.line<<"\n";
		#endif

		//LangElement* lang_elem = newLangElement(LangTokenType::DEFINE_VECTOR, set_name, set_type);
		LangElement* lang_elem = newLangElement(LangTokenType::DEFINE_REFERENCE, TypeType::VECTOR, set_name, "vector");
		//lang_elem->typeType(TypeType::VECTOR);
		currentTemplate = lang_elem;//Oh, remove this.
		currentReference = lang_elem;

		//Hmm. This is a bit silly and slow, but we'll try to find the vector from our stdlib modules:
		LangElement* stdlib_vector_element = *searchElementInOtherModulesSignal(this, lang_elem);
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

		if( lang_elem->parentLangTokenType() == LangTokenType::CLASS )
		{
			//we are inside a class definition, not a func:
			///////lang_elem->langTokenType(LangTokenType::DEFINE_VECTOR_IN_CLASS);

			listOfAutoInitObjects.push_back(lang_elem);
		}

		addToUserDefinedTokens(lang_elem);

		#ifdef DEBUG_RAE_HUMAN
			cout<<"newDefineVector: "<<lang_elem->toString()<<"\n";
			//rae::log("newDefineVector: ",set_name,"\n");
		#endif

		return lang_elem;
	}

	LangElement* newDefineFuncReturn(string set_type, string set_name = "")
	{
		LangElement* lang_elem = newLangElement(LangTokenType::DEFINE_FUNC_RETURN, TypeType::UNDEFINED, set_name, set_type);
		currentReference = lang_elem;

		if( BuiltInType::isBuiltInType(set_type) )
		{
			//ok.
			lang_elem->typeType(TypeType::BUILT_IN_TYPE);
			addToUserDefinedTokens(lang_elem);
		}
		else
		{
			LangElement* found_elem = searchToken(set_type);
			if( found_elem )
			{
				//ok.
				lang_elem->typeType( found_elem->typeType() );//is it ok to just copy the type from here...???
				addToUserDefinedTokens(lang_elem);
			}
			else
			{
				lang_elem->isUnknownType(true);
				addToUnknownDefinitions( lang_elem );
			}
		}

		#ifdef DEBUG_RAE_HUMAN
			cout<<"newDefineFuncReturn: type: "<<set_type<<" name: "<<lang_elem->namespaceString()<<"<\n";
			//rae::log("newDefineFuncReturn: ",set_type," ",set_name,"\n");
		#endif

		return lang_elem;
	}

	LangElement* newDefineFuncArgument(string set_type, string set_name = "")
	{

		cout<<"Remove this FUNC_ARGUMENT thing, and just use parent() which is scope, to know if it's inside a func def. Use DEFINE_REFERENCE for them instead.\n";
		assert(0);

		LangElement* lang_elem = newLangElement(LangTokenType::DEFINE_FUNC_ARGUMENT, TypeType::UNDEFINED, set_name, set_type);
		currentReference = lang_elem;

		if( BuiltInType::isBuiltInType(set_type) )
		{
			//ok.
			lang_elem->typeType(TypeType::BUILT_IN_TYPE);
			addToUserDefinedTokens(lang_elem);
		}
		else
		{
			LangElement* found_elem = searchToken(set_type);
			if( found_elem )
			{
				//ok.
				lang_elem->typeType(found_elem->typeType());//is it ok to just copy the typetype here?
				addToUserDefinedTokens(lang_elem);
			}
			else
			{
				lang_elem->isUnknownType(true);
				addToUnknownDefinitions( lang_elem );
			}
		}

		#ifdef DEBUG_RAE_HUMAN
			cout<<"newDefineFuncArgument: type: "<<set_type<<" name: "<<lang_elem->namespaceString()<<"<\n";
			//rae::log("newDefineFuncArgument: ",set_type," ",set_name,"\n");
		#endif

		return lang_elem;
	}

	LangElement* newDefineReference(TypeType::e set_type_type, LangElement* maybe_found_class, string set_type, string set_name = "")
	{
		LangElement* lang_elem = newLangElement(LangTokenType::DEFINE_REFERENCE, set_type_type, set_name, set_type);
		//lang_elem->typeType(TypeType::REF);
		currentReference = lang_elem;

		if(maybe_found_class)
		{
			lang_elem->definitionElement(maybe_found_class);
		}

		if( lang_elem->parent() )
		{
			if( set_type_type != TypeType::VAL )
			{
				lang_elem->parent()->own(lang_elem);
			}
		}

		if( lang_elem->parentLangTokenType() == LangTokenType::CLASS )
		{
			//we are inside a class definition, not a func:
			/////////lang_elem->langTokenType(LangTokenType::DEFINE_REFERENCE_IN_CLASS);

			if( set_type_type == TypeType::REF )
			{
				listOfAutoInitObjects.push_back(lang_elem);
			}
		}

		addToUserDefinedTokens(lang_elem);

		#ifdef DEBUG_RAE_HUMAN
			cout<<"newDefineReference: type: "<<set_type<<" name: "<<lang_elem->namespaceString()<<"<\n";
			//rae::log("newDefineReference: ",set_type," ",set_name,"\n");
		#endif

		return lang_elem;
	}

	LangElement* newUseReference(LangElement* set_elem)
	{
		if( set_elem == 0 )
			return 0;

		LangElement* lang_elem = newLangElement(LangTokenType::USE_REFERENCE, set_elem->typeType(), set_elem->name(), set_elem->type() );
		
		lang_elem->definitionElement(set_elem);

		#ifdef DEBUG_RAE_HUMAN
			cout<<"newUseReference: "<<set_elem->toString()<<"\n";
			//rae::log("newUseReference: ",set_elem->toString(),"\n");
		#endif

		return lang_elem;
	}
/*
	LangElement* newUnknown(string set_token)
	{
		LangElement* lang_elem = newLangElement(LangTokenType::UNKNOWN, set_token );
		addToUnknowns(lang_elem);

		#ifdef DEBUG_RAE_HUMAN
			cout<<"newUnknown: "<<set_token<<"\n";
			//rae::log("newUnknown: ",set_token,"\n");
		#endif

		return lang_elem;
	}
*/
	LangElement* newUnknownDefinition(string set_token)
	{
		//LangElement* lang_elem = newLangElement(LangTokenType::UNKNOWN_DEFINITION, set_token );
		LangElement* lang_elem = newLangElement(LangTokenType::DEFINE_REFERENCE, TypeType::UNDEFINED, set_token );
		lang_elem->isUnknownType(true);
		addToUnknownDefinitions(lang_elem);

		#ifdef DEBUG_RAE_HUMAN
			cout<<"newUnknownDefinition: "<<set_token<<"\n";
			//rae::log("newUnknownUseReference: ",set_token,"\n");
		#endif

		return lang_elem;
	}

	LangElement* newUnknownUseReference2(string set_token)
	{
		//LangElement* lang_elem = newLangElement(LangTokenType::UNKNOWN_USE_REFERENCE, set_token );
		LangElement* lang_elem = newLangElement(LangTokenType::USE_REFERENCE, TypeType::UNDEFINED, set_token );
		lang_elem->isUnknownType(true);
		addToUnknownUseReferences(lang_elem);

		#ifdef DEBUG_RAE_HUMAN
			cout<<"newUnknownUseReference: "<<set_token<<"\n";
			//rae::log("newUnknownUseReference: ",set_token,"\n");
		#endif

		return lang_elem;
	}

	LangElement* newUnknownUseMember(string set_token)
	{
		//LangElement* lang_elem = newLangElement(LangTokenType::UNKNOWN_USE_MEMBER, set_token );
		LangElement* lang_elem = newLangElement(LangTokenType::USE_MEMBER, TypeType::UNDEFINED, set_token );
		lang_elem->isUnknownType(true);
		addToUnknownUseMembers(lang_elem);

		#ifdef DEBUG_RAE_HUMAN
			cout<<"newUnknownUseMember: "<<set_token<<"\n";
			//rae::log("newUnknownUseReference: ",set_token,"\n");
		#endif

		return lang_elem;
	}
/*
	LangElement* newUseArray(LangElement* set_elem)
	{
		if( set_elem == 0 )
			return 0;
		//LangElement* lang_elem = newLangElement(LangTokenType::USE_ARRAY, set_elem->name(), set_elem->type() );
		LangElement* lang_elem = newLangElement(LangTokenType::USE_REFERENCE, TypeType::C_ARRAY, set_elem->name(), set_elem->type() );
		//lang_elem->typeType(TypeType::C_ARRAY);

		lang_elem->definitionElement(set_elem);
		
		reportError("Don't use C_ARRAYs with this syntax at the moment.");

		#ifdef DEBUG_RAE_HUMAN
			cout<<"DON'T use at the moment. newUseArray: "<<set_elem->toString()<<"\n";
			//rae::log("newUseArray: ",set_elem->toString(),"\n");
		#endif
		return lang_elem;
	}
*/
	LangElement* newUseVector(LangElement* set_elem)
	{
		if( set_elem == 0 )
			return 0;
		//LangElement* lang_elem = newLangElement(LangTokenType::USE_VECTOR, set_elem->name(), set_elem->type() );
		LangElement* lang_elem = newLangElement(LangTokenType::USE_REFERENCE, TypeType::VECTOR, set_elem->name(), set_elem->type() );
		//lang_elem->typeType(TypeType::VECTOR);
		
		lang_elem->definitionElement(set_elem);

		#ifdef DEBUG_RAE_HUMAN
			cout<<"newUseVector: "<<set_elem->toString()<<"\n";
			//rae::log("newUseVector: ",set_elem->toString(),"\n");
		#endif
		return lang_elem;
	}

	LangElement* newNumber(string set_name)
	{
		#ifdef DEBUG_RAE_HUMAN
			cout<<"newNumber: "<<set_name<<"\n";
			//rae::log("newNumber: ",set_name,"\n");
		#endif
		return newLangElement(LangTokenType::NUMBER, TypeType::UNDEFINED, set_name);	
	}

	
	boost::signals2::signal<void (string)> newImportSignal;

	vector<LangElement*> listOfImports;//a list of imports used in this module.

	vector<LangElement*> classes;

	//Do we need this: and it probably should be a map or something with fast text search.
	vector<LangElement*> userDefinedTokens;

	//We'll list them separately now, because we'll have to handle them in this order:
	vector<LangElement*> unknownDefinitions;//->both of them: Tester tester
	vector<LangElement*> unknownUseReferences;//tester or tester.doSomething() //without a class. and the func is not usereference.
	//unless it's a global func. 
	vector<LangElement*> unknownUseMembers;//tester.unknownUseMember() or tester.unknownUseMember //unknown after a reference dot.
	
	//mostly func arguments:
	vector<LangElement*> checkForPreviousDefinitionsList;

	//This is the three of all our langElements.
	vector<LangElement*> langElements;
	//The three is something like this, even though this LangCompiler class
	//handles almost all things in the tree.
	/*
	Module1
	----Class 1
	--------Func 1
	--------Func 2
	----Class 2
	--------Func 3
	Module2
	----Class 3
	--------Func 4
	--------Func 5
	etc.

	//So in the top level, we only have modules/files!
	//So, we could just rename that "langElements" to be "modules"...
	//But maybe we'll just call it langElements, as it's just a big tree anyway.
	*/

	LangElement* currentModule;
	LangElement* currentClass;
	LangElement* currentFunc;
	LangElement* currentEnum;
	LangElement* currentTemplate;

	LangElement* currentTempElement;

	LangElement* currentReference;

	public: LangElement* previousElement() { return m_previousElement; }
	public: void previousElement(LangElement* set) { m_previousElement = set; }
	protected: LangElement* m_previousElement;

	public: LangElement* previous2ndElement()
	{
		if(m_previousElement && m_previousElement->previousElement())
			return m_previousElement->previousElement();
		//else
		return 0;//null
	}

	//And here's some token shortcuts for those elements.
	public: LangTokenType::e previousToken()
	{
		if(m_previousElement == 0)
			return LangTokenType::UNDEFINED;
		return m_previousElement->langTokenType();
	}
	public: LangTokenType::e previous2ndToken()
	{
		if(m_previousElement && m_previousElement->previousElement())
			return m_previousElement->previousElement()->langTokenType();
		//else
		return LangTokenType::UNDEFINED;
	}
	//LangElement* previousElement;
	//LangElement* previous2ndElement;

	//These lists are temporary lists that are only used while parsing the current class.
	//After the class ends, these lists are cleared.
	vector<LangElement*> listOfConstructors;//we need these lists because we have to add stuff there later...
	vector<LangElement*> listOfDestructors;
	vector<LangElement*> listOfAutoInitObjects;
	vector<LangElement*> listOfBuiltInTypesInit;

	//This is either a module, class, func or an empty scope
	public: LangElement* currentParentElement() { return m_currentParentElement; }
	public: void currentParentElement(LangElement* set)
	{
		#ifdef DEBUG_RAE_PARSER
		if(set)
		{
			cout<<"currentParentElement set to: "<<set->toString()<<"\n";
		}
		else cout<<"currentParentElement set to null.\n";
		#endif
		m_currentParentElement = set;
	}
	protected: LangElement* m_currentParentElement;
	
	vector<LangElement*> scopeElementStack;

	vector<LangElement*> parenthesisStack;

	vector<LangElement*> bracketStack;


public:
	
	bool isWriteToFile;
	bool isFileBased;

	FILE* currentFile;
	string currentFilename;//
	boost::filesystem::path currentFilenamePath;//this is the whole filename and the path...

	string moduleName(string separator_char_str = ".")
	{
		if(currentModule == 0)
		{
			return "no module";
		}

		string a_module_name = "";

		int not_on_first = 0;

		foreach(LangElement* elem, currentModule->langElements)
		{
			if( elem->langTokenType() == LangTokenType::MODULE_NAME )
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
				break;//break the foreach... hope this works.
			}
		}

		return a_module_name;
	}

	//compares them as strings: "rae.examples.Tester"
	bool isModuleOnListOfImports(string set_module)
	{
		#ifdef DEBUG_RAE_PARSER
		cout<<"isModuleOnListOfImports: module: "<<set_module<<"\n";
		#endif
		foreach(LangElement* an_import, listOfImports)
		{
			if(an_import->importName() == set_module)
			{
				#ifdef DEBUG_RAE_PARSER
				cout<<"YES. it is on the list.\n";
				#endif
				return true;
			}
			else
			{
				#ifdef DEBUG_RAE_PARSER
				cout<<"No it is not: "<<an_import->importName()<<"\n";
				#endif
			}
		}
		//else
		return false;
	}
	

	void createRaeStdLib(string which_stdlib_class)
	{
		stringIndex = 0;
		isWriteToFile = false;
		isFileBased = false;

		//newClass("string");

		//init();

		//fileParsedOk = true;

		//static const char texture_fragment_shader_color[] 
		
		if(which_stdlib_class == "string")
		{
			#ifdef DEBUG_RAE_HUMAN
			cout<<"createRaeStdLib string!\n";
			#endif

			stringBasedSourceText =
			"module rae.std.string\n"
			"\n"
			"class string\n"
			"{\n"
				"\tfunc ()empty(){}\n"
				"\tfunc ()push_back(){}\n" //void push_back (const value_type& val);
				"\tfunc ()pop_back(){}\n"
				"\tfunc ()size(){}\n" //size_type size() const;

				"\tfunc ()append(){}\n"
				"\tfunc ()assign(){}\n"
				"\tfunc ()at(){}\n"
				"\tfunc ()back(){}\n"
				"\tfunc ()begin(){}\n"
				"\tfunc ()capacity(){}\n"
				"\tfunc ()cbegin(){}\n"
				"\tfunc ()cend(){}\n"
				"\tfunc ()clear(){}\n"
				"\tfunc ()compare(){}\n"
				"\tfunc ()copy(){}\n"
				"\tfunc ()crbegin(){}\n"
				"\tfunc ()crend(){}\n"
				"\tfunc ()c_str(){}\n"
				"\tfunc ()data(){}\n"
				//empty at the beginning!
				"\tfunc ()end(){}\n"
				"\tfunc ()erase(){}\n"
				"\tfunc ()find(){}\n"
				"\tfunc ()find_first_not_of(){}\n"
				"\tfunc ()find_first_of(){}\n"
				"\tfunc ()find_last_not_of(){}\n"
				"\tfunc ()find_last_of(){}\n"
				"\tfunc ()front(){}\n"
				"\tfunc ()get_allocator(){}\n"
				"\tfunc ()insert(){}\n"
				"\tfunc ()length(){}\n"
				"\tfunc ()max_size(){}\n"
				//operator+=
				//operator=
				//opeartor[]
				//pop_back yes at the beginning!
				//push_back
				"\tfunc ()rbegin(){}\n"
				"\tfunc ()rend(){}\n"
				"\tfunc ()replace(){}\n"
				"\tfunc ()reserve(){}\n"
				"\tfunc ()resize(){}\n"
				"\tfunc ()rfind(){}\n"
				"\tfunc ()shrink_to_fit(){}\n"
				//size
				"\tfunc ()substr(){}\n"
				"\tfunc ()swap(){}\n"
			"}\n"
			;
		}
		else if(which_stdlib_class == "vector")
		{
			#ifdef DEBUG_RAE_HUMAN
			cout<<"createRaeStdLib vector!\n";
			#endif

			stringBasedSourceText =
			"module rae.std.vector\n"
			"\n"
			"class vector\n"
			"{\n"
				"\tfunc ()empty(){}\n"
				"\tfunc ()push_back(){}\n" //void push_back (const value_type& val);
				"\tfunc ()pop_back(){}\n"
				"\tfunc ()size(){}\n" //size_type size() const;

				"\tfunc ()assign(){}\n"
				"\tfunc ()at(){}\n"
				"\tfunc ()back(){}\n"
				"\tfunc ()begin(){}\n"
				"\tfunc ()capacity(){}\n"
				"\tfunc ()cbegin(){}\n"
				"\tfunc ()cend(){}\n"
				"\tfunc ()clear(){}\n"
				"\tfunc ()crbegin(){}\n"
				"\tfunc ()crend(){}\n"
				"\tfunc ()data(){}\n"
				"\tfunc ()emplace(){}\n"
				"\tfunc ()emplace_back(){}\n"
				//empty at the beginning!
				"\tfunc ()end(){}\n"
				"\tfunc ()erase(){}\n"
				"\tfunc ()front(){}\n"
				"\tfunc ()get_allocator(){}\n"
				"\tfunc ()insert(){}\n"
				"\tfunc ()max_size(){}\n"
				//operator=
				//operator[]
				//pop_back at the beginning!
				//push_back
				"\tfunc ()rbegin(){}\n"
				"\tfunc ()rend(){}\n"
				"\tfunc ()reserve(){}\n"
				"\tfunc ()resize(){}\n"
				"\tfunc ()shrink_to_fit(){}\n"

				"\tfunc ()swap(){}\n"
			"}\n"
			;
		}
	}

	
	void parseString()
	{
		while( isDone == false )
		{
			parseIdleString();
		}
		
		//close module:
		closeModule();
		
		handleUnknownTokens();

		fileParsedOk = true;

		if( countWarnings > 0)
		cout<<"\nNumber of warnings from stdlib: "<<countWarnings<<"\n\n";

		if( countErrors > 0)
			cout<<"\nNumber of errors from stdlib: "<<countErrors<<"\n\n";
	}

	
	string stringBasedSourceText;
	

	void read( string a_filename )
	{
		init();

		fileParsedOk = false;
	
		currentFilename = a_filename;
		
		//boost::filesystem::path p( currentFilename );
		currentFilenamePath = currentFilename;

		//rae::log("SourceParser::read END", a_filename, " dat.\n" );

		//rae::log("SourceParser::read END", currentFilename, "dat.\n" );
		//rae::log("SourceParser::read END", currentFilenamePath.string(), "Ag.\n" );

	}
	
	void parse()
	{
		//rae::log("SourceParser::parse START.", currentFilenamePath.string(), "\n" );
		cout<<"\nParsing: "<<currentFilenamePath.string()<<"\n";

		if(currentFilenamePath.string() == "")
			return;

		//rae::log("SourceParser::parse we are actually going to parse.\n");

		//bool file_is_ok = false;
		
		if( checkPathType(currentFilenamePath) == PathType::FILE )
		{
			currentFile = fopen(currentFilename.c_str(), "r");
			//currentFile = fopen("sauna.edl", "r");
			
			
			if( currentFile != NULL )
			{	
				while( isDone == false )
				{
					parseIdle();
				}
				
				//close module:
				closeModule();
				
				fclose(currentFile);

				handleUnknownTokens();

				fileParsedOk = true;
				
			}			
		}

		if( countWarnings > 0)
		cout<<"\nNumber of warnings: "<<countWarnings<<"\n\n";

		if( countErrors > 0)
			cout<<"\nNumber of errors: "<<countErrors<<"\n\n";
	}
	
	void write(string folder_path_to_write_to)
	{
		/*
		//if( fileParsedOk == false )
		if( sourceFiles.empty() )
		{
			//cout<<
			reportError("write() None of the source files were parsed correctly. Can't write anything.");
			return;
		}
		
		//else

		foreach(boost::filesystem::path set_path, sourceFiles)
		{
			writeCppFile(set_path);
		}
		*/
	
		/*
		foreach( LangElement* elem, langElements )
		{
			cout<<elem->name()<<"\n");
		}
		*/

		foreach( LangElement* module_elem, langElements )
		{
			if( module_elem->langTokenType() != LangTokenType::MODULE )
			{
				#ifdef DEBUG_RAE_HUMAN
				cout<<"Interesting: we have something in top hierarchy: "<<module_elem->langTokenTypeString()<<"\n";
				//rae::log("Interesting: we have something in top hierarchy: ", module_elem->langTokenTypeString(), "\n");
				#endif
				continue; //skip it
			}


			boost::filesystem::path module_filename;

			string module_name; 

			foreach( LangElement* elem, module_elem->langElements )
			{
				if( elem->langTokenType() == LangTokenType::MODULE_NAME )
				{
					module_name += elem->name();
					module_name += "/"; //a directory separator
				}
				else
				{
					break;//break the foreach... hope this works.
				}
			}

			//remove last /
			#if __cplusplus >= 201103L //c++11
				if( module_name.back() == '/' )
				{
					module_name.pop_back();
				}
			#else //older c++
				if( module_name.at(module_name.size()-1) == '/' )
				{
					std::string::iterator the_last_letter = module_name.end();
					the_last_letter--;
					module_name.erase( the_last_letter );
				}
			#endif

			module_filename = folder_path_to_write_to + module_name;// + ".rae");
			
			boost::filesystem::path outputHeaderPath = module_filename;
			//outputHeaderPath.replace_extension(".hpp");
			outputHeaderPath.replace_extension(".hpp"); //TEMPTEMPTEMPTEMPTEMPTE
		
			boost::filesystem::path outputFilePath = module_filename;
			outputFilePath.replace_extension(".cpp");
			
			//#ifdef DEBUG_RAE_HUMAN
			//cout<<"\n\nWriting C++:\n\n";
			cout<<"Writing C++: "<<module_filename.string()<<" .hpp and .cpp\n";
			//cout<<"outputHeaderPath: "<<outputHeaderPath.string()<<"\n";
			//cout<<"outputFilePath: "<<outputFilePath.string()<<"\n";
			//#endif

			//rae::log("\n\nWriting C++:\n\n");
			//rae::log("module_filename: ", module_filename.string(), "\n");
			//rae::log("outputHeaderPath: ", outputHeaderPath.string(), "\n");
			//rae::log("outputFilePath: ", outputFilePath.string(), "\n");
			
			if(boost::filesystem::exists(outputHeaderPath.parent_path()) == false)
			{
				if(boost::filesystem::create_directories( outputHeaderPath.parent_path() ) )
				{
					cout<<"Created folder: "<<outputHeaderPath.parent_path()<<"\n";
					//rae::log("Created folder: ", outputHeaderPath.parent_path(), "\n");
				}
				else
				{
					cout<<"Failed to create folder: "<<outputHeaderPath.parent_path()<<"\n";
					//rae::log("Failed to create folder: ", outputHeaderPath.parent_path(), "\n");
				}
			}

			if(boost::filesystem::exists(outputFilePath.parent_path()) == false)
			{
				if(boost::filesystem::create_directories( outputFilePath.parent_path() ) )
				{
					cout<<"Created folder: "<<outputFilePath.parent_path()<<"\n";
					//rae::log("Created folder: ", outputFilePath.parent_path(), "\n");
				}
				else
				{
					cout<<"Failed to create folder: "<<outputFilePath.parent_path()<<"\n";
					//rae::log("Failed to create folder: ", outputFilePath.parent_path(), "\n");
				}
			}
			

			StringFileWriter writer;
			writer.create( outputHeaderPath.string(), /*isHeader*/true );

			writer.writeString("//this file is automatically created from Rae programming language module:\n//");
			writer.writeString( module_filename.string() );
			writer.writeString("\n");
			
			
			writeElement(writer, *module_elem);
				//writer.writeString( elem->name() );
				//fwrite(&record.name,sizeof(char),record.nameLength,fp);
			
			//fwrite( buffer, sizeof(char), sizeof(buffer), pFile);
				
			writer.close();

			StringFileWriter writer2;
			writer2.create( outputFilePath.string(), /*isHeader*/false );

			writer2.writeString("//this file is automatically created from Rae programming language module:\n//");
			writer2.writeString( module_filename.string() );
			writer.writeString("\n");

				
			//module_elem->write(writer2);
			writeElement(writer2, *module_elem);
					//writer.writeString( elem->name() );
					//fwrite(&record.name,sizeof(char),record.nameLength,fp);
				
				//fwrite( buffer, sizeof(char), sizeof(buffer), pFile);
				
			writer2.close();
			
			
			
			//cout<<"\n\nheader: "<<outputHeaderPath<<"\n";
			//cout<<"file: "<<outputFilePath<<"\n";
			//rae::log("\n\nheader: ", outputHeaderPath, "\n");
			//rae::log("file: ", outputFilePath, "\n");
		}//end foreach modules langElements...
	}

	bool isWhiteSpace(char set_char)
	{
		if( set_char == ' ' || set_char == '\n' || set_char == '\t' )
			return true;
		//else
		return false;
	}

	LineNumber lineNumber;
	/*
	bool readRawChar()
	{
		if(isFileBased == true)
		{
			if( EOF != (currentChar = getc(currentFile)) )
			{
				return true;
			}
			//
			return false;
		}
		else
		{

		}
	}*/

	bool readChar()
	{
		lineNumber.totalCharNumber++;//Ok, 1 is the first element.
		lineNumber.charPosInLine++;//1 is the first char in line. (unlike in C arrays...)

		handleSlash = "";
		handleSlash += currentChar;//put the char we had in the last "frame" here. So it's the char before the currentChar
		//that we read in the next line.
		beforeChar = currentChar;//the same but with an int.
	
		if( EOF != (currentChar = getc(currentFile)) )
		{
			return handleChar();
		}
		//else
			isDone = true;//Quit parsing
			return false;
	}

	bool readCharFromString()
	{
		lineNumber.totalCharNumber++;//Ok, 1 is the first element.
		lineNumber.charPosInLine++;//1 is the first char in line. (unlike in C arrays...)

		handleSlash = "";
		handleSlash += currentChar;//put the char we had in the last "frame" here. So it's the char before the currentChar
		//that we read in the next line.
		beforeChar = currentChar;//the same but with an int.
	
		if( stringIndex < stringBasedSourceText.size() )
		{
			currentChar = stringBasedSourceText.at(stringIndex);
			stringIndex++;
			return handleChar();
		}
		//else
			isDone = true;//Quit parsing
			return false;
	}

	int stringIndex;

	//return false if EOF or other error...
	//puts result in currentChar.
	bool handleChar()
	{
		
		//if( readRawChar() )
		//{
			/*if( isWhiteSpace(currentChar) || currentChar == ';' )//semicolon needs more work, like in empty if statements...
			{
				wholeToken = currentWord;
				isWholeToken = true;
				currentWord = "");
				cout<<"\nreset currentWord.\n");
				
				//special check for end of line...
				if( currentChar == '\n' )
				{
					endOfLine = currentLine;
					isEndOfLine = true;
					currentLine = "");
					cout<<"\nreset currentLine.\n");
				}
				else //add other whitespace to currentLine.
				{
					currentLine += currentChar;
				}
			}*/
			
			if( currentChar == '\n')
			{
				//just handling the linenumbers here, so that none get lost.
				lineNumber.line++;
			}

			//list of separators:

			//handle quotes:

			if( nroWaitingForPlusCommentEnd == 0 && isWaitingForStarCommentEnd == false )//we don't want quotes to mess with comments.
			{
				if( currentChar == '\"')
				{
					if( isWaitingForQuoteEnd == false )
					{
						if( handleSlash == "\\")
						{
							wholeToken = "\"";//put a quote to token 1.
							isWholeToken = true;
							currentWord = "";
						}
						else
						{
							//first quote

							wholeToken = currentWord;
							isWholeToken = true;
							
							wholeToken2 = "\"";
							isWholeToken2 = true;
							currentWord = "";
							
							currentLine += currentChar;

							currentQuote = "";
							isWaitingForQuoteEnd = true;
						}
					}
					else
					{
						if( handleSlash == "\\")
						{
							//ignore it here, and the next if will add it to the quote...

							//wholeToken = "\"";//put a quote to token 1.
							//isWholeToken = true;
							//currentWord = "";		
						}
						else
						{
							//second quote closes the quote.

							wholeToken = currentWord;
							isWholeToken = true;
							
							wholeToken2 = "\"";
							isWholeToken2 = true;
							currentWord = "";
							
							currentLine += currentChar;

							currentQuote += currentChar;

							isWaitingForQuoteEnd = false;
							isQuoteReady = true;
						}	
					}
				}

				if( isWaitingForQuoteEnd == true )
				{
					currentQuote += currentChar;
					return true;
				}
			}

			//pluscomments:
			if( nroWaitingForPlusCommentEnd > 0 )
			{
				if( currentChar == '/' )
				{
					if( handleSlash == "+" )
					{
						//one ending plus and slash.
						nroWaitingForPlusCommentEnd--;

						//we need to have a matching number of plus comments to end commenting.
						if( nroWaitingForPlusCommentEnd == 0 )
						{
							wholeToken = "+/";
							isWholeToken = true;
							currentWord = "";
							
							//currentLine += currentChar;

							currentPlusComment += currentChar;
							isPlusCommentReady = true;	
						}
					}
					//ends a star comment, but we don't want to do that.
					else if( handleSlash == "*" )
					{
						currentPlusComment += 'o';//an extra letter between star and slash. /o* like this *o/
						//currentPlusComment += currentChar;
						//return true;		
					}
				}
				else if( currentChar == '+' )
				{
					//start a plus comment
					if( handleSlash == "/" )
					{
						nroWaitingForPlusCommentEnd++;
					}
				}
				else if( currentChar == '*' )
				{
					//start a star comment, but we don't want to do that.
					if( handleSlash == "/" )
					{
						currentPlusComment += 'o';//an extra letter between slash and star. /o* like this *o/
						//currentPlusComment += currentChar;
						//return true;		
					}
				}

				if( nroWaitingForPlusCommentEnd > 0 )
				{
					currentPlusComment += currentChar;
				}
				return true;
			}

			//starcomments:
			if( isWaitingForStarCommentEnd == true )
			{
				if( currentChar == '/' )
				{
					if( handleSlash == "*" )
					{
						//if( isWaitingForStarCommentEnd == false )
						//{
						//	cout<<"Got starcomment end, but not waiting for it... */\n";

						//	wholeToken = "*/";
						//	isWholeToken = true;
						//	currentWord = "";
						//}
						//else
						//{
							//close starcomment

							//wholeToken = currentWord;
							//isWholeToken = true;
							
							wholeToken = "*/";
							isWholeToken = true;
							currentWord = "";
							
							//currentLine += currentChar;

							isWaitingForStarCommentEnd = false;
							currentStarComment += currentChar;
							isStarCommentReady = true;	
						//}

						
						//Extended comment ends.
						
						///////wholeToken = "*/";
						///////isWholeToken = true;
						///////currentWord = "";
						//handleSlash = "";
						
					}
					///////currentLine += currentChar;
				}

				if( isWaitingForStarCommentEnd == true )
				{
					currentStarComment += currentChar;
				}
				return true;
			}

			//handle slashes:

			if( currentChar == '+' )
			{
				if( handleSlash == "/" )
				{
					if( nroWaitingForPlusCommentEnd == 0 )
					{
						//start plus comment
						#ifdef DEBUG_RAE
							cout<<"Start plus_comment HandleSlash /+\n";
							//rae::log("HandleSlash /*\n");
						#endif

						////////Extended comment.
						///////wholeToken = "/*";
						///////isWholeToken = true;
						//////currentWord = "";
						//handleSlash = "";
						
						currentPlusComment = "/+";
						nroWaitingForPlusCommentEnd++;
						return true;
					}
				}
				else
				{				
					wholeToken = currentWord;
					isWholeToken = true;
					
					wholeToken2 = "+";
					isWholeToken2 = true;
					currentWord = "";
					
					currentLine += currentChar;
					return true;
				}
			}			
			else if( currentChar == '*' )
			{
				if( handleSlash == "/" )
				{
					if( isWaitingForStarCommentEnd == false )
					{
						//start comment
						#ifdef DEBUG_RAE
							cout<<"HandleSlash /*\n";
							//rae::log("HandleSlash /*\n");
						#endif

						////////Extended comment.
						///////wholeToken = "/*";
						///////isWholeToken = true;
						//////currentWord = "";
						//handleSlash = "";
						
						currentStarComment = "/*";
						isWaitingForStarCommentEnd = true;
						return true;
					}
				}
				else
				{				
					wholeToken = currentWord;
					isWholeToken = true;
					
					wholeToken2 = "*";
					isWholeToken2 = true;
					currentWord = "";
					
					currentLine += currentChar;
				}
			}

//handle simple one line comments:

			if( currentChar == '/' )
			{
				/*if( handleSlash == "" )
				{
					wholeToken = currentWord;
					isWholeToken = true;
					currentWord = "/";
					//handleSlash = "/";
				}
				else */
				if( handleSlash == "/" )
				{
					//Comment time.
					/*
					wholeToken = currentWord;
					isWholeToken = true;//end token 1.
					wholeToken2 = "//";//put // to token 2.
					isWholeToken2 = true;
					currentWord = "";
					*/
					//handleSlash = "";

					wholeToken = "//";//put // to token 1.
					isWholeToken = true;
					currentWord = "";
				}
			}

			//handle backslashes:
			if( currentChar == '\\' )//a backslash
			{
				/*if( handleSlash == "" )
				{
					wholeToken = currentWord;
					isWholeToken = true;
					currentWord = "/";
					//handleSlash = "/";
				}
				else */
				/*if( handleSlash == "\\" )
				{
					wholeToken = "\\";//put // to token 1.
					isWholeToken = true;
					currentWord = "";
				}*/

				wholeToken = "\\";//put // to token 1.
				isWholeToken = true;
				currentWord = "";
			}

			//handle other stuff:

			if( currentChar == '.' )
			{
				wholeToken = currentWord;
				isWholeToken = true;
				
				wholeToken2 = ".";
				isWholeToken2 = true;
				currentWord = "";
				
				currentLine += currentChar;
			}
			else if( currentChar == ',' )
			{
				wholeToken = currentWord;
				isWholeToken = true;
				
				wholeToken2 = ",";
				isWholeToken2 = true;
				currentWord = "";
				
				currentLine += currentChar;
			}
			else if( currentChar == '!' )
			{
				wholeToken = currentWord;
				isWholeToken = true;
				
				wholeToken2 = "!";
				isWholeToken2 = true;
				currentWord = "";
				
				currentLine += currentChar;
			}
			//math
			/*else if( currentChar == '+' )
			{
				//cout<<"early We got +\n";
				
				if( handleSlash == "/" )
				{
					cout<<"HandleSlash /+\n";
					//rae::log("HandleSlash /+\n");
					//Extended+ comment.
					wholeToken = "/+";
					isWholeToken = true;
					currentWord = "";
					//handleSlash = "";
				}
				else
				{				
					wholeToken = currentWord;
					isWholeToken = true;
					
					wholeToken2 = "+";
					isWholeToken2 = true;
					currentWord = "";
					
					currentLine += currentChar;
				}
			}*/
			else if( currentChar == '-' )
			{
				//cout<<"early We got -\n";
				wholeToken = currentWord;
				isWholeToken = true;
				
				wholeToken2 = "-";
				isWholeToken2 = true;
				currentWord = "";
				
				currentLine += currentChar;
			}
			else if( currentChar == '=' )
			{
				//cout<<"early We got =\n";
				wholeToken = currentWord;
				isWholeToken = true;
				
				wholeToken2 = "=";
				isWholeToken2 = true;
				currentWord = "";
				
				currentLine += currentChar;
			}
			else if( currentChar == '<' )
			{
				//cout<<"early We got =\n";
				wholeToken = currentWord;
				isWholeToken = true;
				
				wholeToken2 = "<";
				isWholeToken2 = true;
				currentWord = "";
				
				currentLine += currentChar;
			}
			else if( currentChar == '>' )
			{
				//cout<<"early We got =\n";
				wholeToken = currentWord;
				isWholeToken = true;
				
				wholeToken2 = ">";
				isWholeToken2 = true;
				currentWord = "";
				
				currentLine += currentChar;
			}
			//chars that end last token and begin a nu one:
			else if( currentChar == '(' )
			{
				wholeToken = currentWord;
				isWholeToken = true;
				//currentWord = "(";
				//currentWord = "";
				
				wholeToken2 = "(";
				isWholeToken2 = true;
				currentWord = "";
				
				currentLine += currentChar;
			}
			else if( currentChar == '[' )
			{
				wholeToken = currentWord;
				isWholeToken = true;
				//currentWord = "(";
				//currentWord = "";
				
				wholeToken2 = "[";
				isWholeToken2 = true;
				currentWord = "";
				
				currentLine += currentChar;
			}
			else if( currentChar == '{' )
			{
				wholeToken = currentWord;
				isWholeToken = true;
				//currentWord = "{";
				//currentWord = "";
				
				wholeToken2 = "{";
				isWholeToken2 = true;
				currentWord = "";
				
				currentLine += currentChar;
			}
			//chars that end last token and are part of it: NOPE
			else if( currentChar == ')' )
			{
				//currentWord += currentChar;
				wholeToken = currentWord;
				isWholeToken = true;
				//currentWord = "";
				
				wholeToken2 = ")";
				isWholeToken2 = true;
				currentWord = "";
				
				currentLine += currentChar;
			}
			else if( currentChar == ']' )
			{
				//currentWord += currentChar;
				wholeToken = currentWord;
				isWholeToken = true;
				//currentWord = "";
				
				wholeToken2 = "]";
				isWholeToken2 = true;
				currentWord = "";
				
				currentLine += currentChar;
			}
			//chars that end last token but are not part of it, mostly whitespace:
			else if( currentChar == '\n' )
			{
				//Moved the linenumber adder to more beginning in this func.

				wholeToken = currentWord;
				isWholeToken = true;
				//currentWord = "";
				
				wholeToken2 = "\n";
				isWholeToken2 = true;
				currentWord = "";
				
				endOfLine = currentLine;
				isEndOfLine = true;
				currentLine = "";
			}
			else if( currentChar == ' ' )
			{
				wholeToken = currentWord;
				isWholeToken = true;
				//currentWord = "";
				
				wholeToken2 = " ";
				isWholeToken2 = true;
				currentWord = "";
				
				currentLine += currentChar;
			}
			else if( currentChar == '\t' )
			{
				wholeToken = currentWord;
				isWholeToken = true;
				//currentWord = "";
				
				wholeToken2 = "\t";
				isWholeToken2 = true;
				currentWord = "";
				
				currentLine += currentChar;
			}
			else if( currentChar == '}' )
			{
				wholeToken = currentWord;
				isWholeToken = true;
				//currentWord = "}";
				
				wholeToken2 = "}";
				isWholeToken2 = true;
				currentWord = "";
				
				currentLine += currentChar;
			}
			else if( currentChar == ';' )
			{
				wholeToken = currentWord;
				isWholeToken = true;
				//currentWord = ";";
		
				wholeToken2 = ";";
				isWholeToken2 = true;
				currentWord = "";
		
				currentLine += currentChar;
			}
			else if( currentChar == ':' )
			{
				wholeToken = currentWord;
				isWholeToken = true;
				//currentWord = ";";
		
				wholeToken2 = ":";
				isWholeToken2 = true;
				currentWord = "";
		
				currentLine += currentChar;
			}
			/*else if(
				(currentChar == '0'
				|| currentChar == '1'
				|| currentChar == '2'
				|| currentChar == '3'
				|| currentChar == '4'
				|| currentChar == '5'
				|| currentChar == '6'
				|| currentChar == '7'
				|| currentChar == '8'
				|| currentChar == '9')
				//&& isAlphabetic(beforeChar) == false
				)
			{
				isWholeToken = false;
				isWholeToken2 = false;
				currentWord = "";
				wholeToken = "";
				wholeToken2 = "";

				//currentLine += currentChar;	
			
				//exceptionally handled already here:
				string num = "";
				num += currentChar;
				newNumber(num);
			}*/
			else
			{
				currentWord += currentChar;
				//cout<<"currentWord:"<<currentWord<<"\n";
				currentLine += currentChar;
				//cout<<"currentLine:"<<currentLine<<"\n";
			}
			return true;
		//}
	}

	int countWarnings;
	int countErrors;

	void reportWarning(string set)
	{
		cout<<"Warning: "<<set<<" / line: "<<lineNumber.line<<" / Module: "<<moduleName()<<"\n";
		//rae::log("Warning: ", lineNumber.line, " ", set, "\n");
		countWarnings++;
	}

	void reportError(string set)
	{
		cout<<"ERROR: "<<set<<" / line: "<<lineNumber.line<<" / Module: "<<moduleName()<<"\n";
		//rae::log("ERROR: ", lineNumber.line, " ", set, "\n");
		countErrors++;
	}

	void reportError(string set, LangElement* set_elem)
	{
		if(set_elem)
		{
			cout<<"ERROR: "<<set<<" / line: "<<set_elem->lineNumber().line<<" / Module: "<<moduleName()<<"\n";
			//rae::log("ERROR: ", lineNumber.line, " ", set, "\n");	
		}
		else
		{
			cout<<"Error reporting failed. Oh no. This isn't supposed to happen. Email the developer of the compiler.";
		}
		countErrors++;		
	}

	void parseIdle()
	{				
		//while( EOF != (currentChar = getc(currentFile)) )
		//if( expectingToken() == LangTokenType::UNDEFINED )
		//{
			if( readChar() )
			{
				if( isWholeToken )
				{
					if( wholeToken != "" )//ignore empty tokens...
					{
						//cout<<"TOKEN:>"<<wholeToken<<"<\n";
						isWholeToken = false;
						//cout<<"calling handleToken on wholeToken1\n";
						handleToken(wholeToken);
					}
					else isWholeToken = false;
				}
				if( isWholeToken2 )
				{
					if( wholeToken2 != "" )//ignore empty tokens...
					{
						//cout<<"TOKEN2:>"<<wholeToken2<<"<\n";
						isWholeToken2 = false;
						//cout<<"calling handleToken on wholeToken2!!!!\n";
						handleToken(wholeToken2);
					}
					else isWholeToken2 = false;
				}
				/*
				if( isEndOfLine )
				{
					cout<<"Found line:>"<<endOfLine<<"<\n";
					isEndOfLine = false;
				}
				*/
			}
		
			/*
			if( readChar() == true )
			{
			
				switch(currentChar)
				{
					case 'c':
						if( checkToken("lass ") )
						{
							cout<<"Got class .";
							//edlTitle = getUntilEndOfLine();
							//cout<<"The title is: "<<edlTitle<<":\n";
							expectingToken = LangTokenType::CLASS_NAME;
							isWholeToken = false;//important to reset this, otherwise you get the word class
						}
						else
						{
							cout<<"it's not class: "<<currentWord;
						}
					break;
					case 'f':
						if( checkToken("unc ") )
						{
							cout<<"Got func .";
							//edlTitle = getUntilEndOfLine();
							//cout<<"The title is: "<<edlTitle<<":\n";
							expectingToken = LangTokenType::FUNC_DEFINITION;
							isWholeToken = false;//important to reset this, otherwise you get the word class
						}
						else
						{
							cout<<"it's not func: "<<currentWord;
						}
					break;
					case 'p':
						if( checkToken("ublic") )
						{
							cout<<"Got public.";
							//edlTitle = getUntilEndOfLine();
							//cout<<"The title is: "<<edlTitle<<":\n";
							//expectingToken = EDLTokenType::UNDEFINED;
							currentVisibility = "public";
						}
						else
						{
							cout<<"it's not public: "<<currentWord;
						}
					break;
					case 't':
						if( checkToken("his") )
						{
							cout<<"Got this.";
							//edlTitle = getUntilEndOfLine();
							//cout<<"The title is: "<<edlTitle<<":\n";
							//expectingToken = EDLTokenType::UNDEFINED;
						}
						else
						{
							cout<<"it's not this: "<<currentWord;
						}
					break;
					case '/':
						if( checkToken("/") )
						{
							cout<<"Got // comment.";
							//edlTitle = getUntilEndOfLine();
							//cout<<"The title is: "<<edlTitle<<":\n";
							expectingToken = LangTokenType::COMMENT;
							isEndOfLine = false;//hmm, important to release this... otherwise you get the line before this.
							currentLine = "//";
						}
						else
						{
							cout<<"it's not // comment:>"<<currentWord<<"<\n";
							if( isWholeToken )
							{
								cout<<"wholeToken:>"<<wholeToken<<"<\n";
							}
						}
					break;
					case '0':
						//cout<<" zero.";
					break;
					case ';':
						
						//line.push_back(token);
						//token.clear();
						//cout<<" asterisk.";
						break;
					case '\n':
						cout<<" end line.";
						//lines.push_back(line);
						//line.clear();
						//token.clear();
						break;
					default:
						/////////cout<<(char)currentChar;
						//token.push_back(currentChar);
					break;
				}
				
			}//if readchar
			*/
		//}
		/*
		else if( expectingToken() == LangTokenType::CLASS_NAME )
		{
			if( readChar() && isWholeToken )
			{
				isWholeToken = false;
				newClass(wholeToken);
				//wholeToken = "";
				expectingToken = LangTokenType::UNDEFINED;
			}
		}
		else if( expectingToken() == LangTokenType::COMMENT )
		{
			if( readChar() && isEndOfLine )
			{
				isEndOfLine = false;
				newComment(endOfLine);
				expectingToken = LangTokenType::UNDEFINED;
			}
		}
		else if( expectingToken() == LangTokenType::FUNC_DEFINITION )
		{
			if( readChar() && isEndOfLine )
			{
				isEndOfLine = false;
				
				if( endOfLine != "" )
				{
					cout<<"nu09817240918274091782 func: "<<endOfLine<<"\n";
					//newClass(wholeToken);
					//wholeToken = "";
					expectingToken = LangTokenType::UNDEFINED;
				}
				else cout<<"waiting real func on next line.\n";
	*/		
				/*if( wholeToken[0] != '(' )
				{
					cout<<"nu12938719238719273 func: "<<wholeToken<<"\n";
					//newClass(wholeToken);
					//wholeToken = "";
					expectingToken = LangTokenType::UNDEFINED;
				}*/
		/*	}
		}
		else
		{
			cout<<"Unhandled expecting LangTokenType:"<<LangTokenType::toString(expectingToken)<<"\n";
			expectingToken = LangTokenType::UNDEFINED;
		}
		*/
			/*
			else if( expectingToken() == EDLTokenType::TITLE )
			{
				switch(currentChar)
				{
					case 'T':
						if( checkToken("ITLE:") )
						{
							cout<<"Got whole TITLE.";
							edlTitle = getUntilEndOfLine();
							cout<<"The title is: "<<edlTitle<<":\n";
							expectingToken = EDLTokenType::UNDEFINED;
						}
						else
						{
							cout<<"ERROR: NON STANDARD CMX3600 EDL file. Stopping parser. Sorry. Add a TITLE line to the top.";
							isDone = true;
						}
							
					break;
					default:
						cout<<"ERROR: NON STANDARD CMX3600 EDL file. Stopping parser. Sorry. Add a TITLE line to the top.";
						//token.push_back(currentChar);
						isDone = true;
					//return 0;
					break;
				}
			}*/
		
	}

	void parseIdleString()
	{				
		if( readCharFromString() )
		{
			if( isWholeToken )
			{
				if( wholeToken != "" )//ignore empty tokens...
				{
					//cout<<"TOKEN:>"<<wholeToken<<"<\n";
					isWholeToken = false;
					//cout<<"calling handleToken on wholeToken1\n";
					handleToken(wholeToken);
				}
				else isWholeToken = false;
			}
			if( isWholeToken2 )
			{
				if( wholeToken2 != "" )//ignore empty tokens...
				{
					//cout<<"TOKEN2:>"<<wholeToken2<<"<\n";
					isWholeToken2 = false;
					//cout<<"calling handleToken on wholeToken2!!!!\n";
					handleToken(wholeToken2);
				}
				else isWholeToken2 = false;
			}
		}
	}

	void handleToken(string set_token)
	{
		if( set_token == "" )//ignore empty tokens...
		{
			return;
		}
		else if( set_token == "\t" )
		{
			return;
		}
		else if( set_token == " " )
		{
			return;
		}
		/*else if( set_token == "\n" )
		{
			return;
		}*/

		if( set_token == "+/")
		{
			if( isPlusCommentReady == true )
			{
				newPlusComment(currentPlusComment);
				isPlusCommentReady = false;
				return;
			}
		}

		if( set_token == "*/")
		{
			if( isStarCommentReady == true )
			{
				newStarComment(currentStarComment);
				isStarCommentReady = false;
				return;
			}
		}
	
		if( set_token == "//" )
		{
			//expectingToken = LangTokenType::COMMENT_LINE_END;
			isSingleLineComment = true;
			currentLine = "//";
			//cout<<"Waiting for comment line to end.\n";
		}
	
		//if( expectingToken() == LangTokenType::COMMENT_LINE_END )
		if( isSingleLineComment == true )
		{
			//cout<<"Waiting comment end...\n";
		
			if( set_token == "\n" )
			{
				//cout<<"...hurray! Comment finally ended.\n";
			
				/*if( isEndOfLine )
				{
					cout<<"We has isEndOfLine...:>"<<endOfLine<<"<\n";
				}
				else cout<<"We don't have isEndOfLine...:>"<<endOfLine<<"<\n";
				*/
				newComment(endOfLine);
				newLine();
				//expectingToken = LangTokenType::UNDEFINED;
				isSingleLineComment = false;
				isEndOfLine = false;
			}
			
			return;
		}

		if( set_token == "\"")
		{
			if( isQuoteReady == true )
			{
				isQuoteReady = false;

				if( expectingToken() == LangTokenType::INIT_DATA )
				{
					if( currentReference )
					{
						#ifdef DEBUG_RAE_HUMAN
						cout<<"string init_data: "<<currentQuote<<" Special handling for newQuote. INIT_DATA.\n";
						#endif
						//special handling for quote i.e. string initData.
						LangElement* in_dat = newLangElement(LangTokenType::INIT_DATA, TypeType::UNDEFINED, currentQuote);
						currentReference->initData( in_dat );
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
		
		if( expectingToken() == LangTokenType::INIT_DATA )
		{
			if( set_token == "\n" )
			{
				if( currentReference )
				{
					currentReference->addDefaultInitData();

					//TODO check for shadowing other name definitions!!!!!!!!!!!!!!!!
					//and error if we're returnToExpectToken FUNC_ARGUMENT_NAME

					#ifdef DEBUG_RAE_HUMAN
						cout<<"newDefineBuiltInType: "<<currentReference->type()<<" "<<currentReference->namespaceString()<<"\n";
						//rae::log("newDefineBuiltInType: ", currentReference->type(), " ", currentReference->name(), "\n");
					#endif
				}
				//expectingToken = LangTokenType::UNDEFINED;
				doReturnToExpectToken();
				newLine();				
			}
			else if( set_token == "=" )
			{
				//let it pass. and continue waiting for the actual data!
				//newLangElement(LangTokenType::EQUALS, TypeType::UNDEFINED, set_token);
				//expectingToken is still INIT_DATA!
			}
			else if( set_token == "\"" )
			{
				//let it pass.
			}
			else if( set_token == ":" )
			{
				//We're actually inside a function call, so we want to mark this as
				//INFO_FUNC_PARAM, which will not be printed.
				if( currentReference )
				{
					currentReference->langTokenType(LangTokenType::INFO_FUNC_PARAM);
				}
				//expectingToken = LangTokenType::UNDEFINED;
				doReturnToExpectToken();
			}
			else if( set_token == "new")
			{
				#ifdef DEBUG_RAE_HUMAN
					cout<<"TODO new() INIT_DATA.\n";
					//rae::log("TODO new() INIT_DATA.\n");
				#endif
				//TODO error on returnToExpectToken FUNC_ARGUMENT_NAME, no new here.
				//currentReference->initData(set_token);
				//expectingToken = LangTokenType::UNDEFINED; //TEMP
				doReturnToExpectToken();
			}
			else
			{
				/*if( handleInitData(set_token) )
				{
					//got all init data.
					expectingToken = LangTokenType::UNDEFINED;
				}*/
				//if( isNumericChar(set_token[0]) &&
				if( currentReference && previousElement()->langTokenType() != LangTokenType::QUOTE )
				{
					//LangElement* in_dat = newLangElement(LangTokenType::NUMBER, TypeType::UNDEFINED, set_token);
					LangElement* in_dat = newLangElement(LangTokenType::INIT_DATA, TypeType::UNDEFINED, set_token);
					//in_dat->parent(currentReference);
					currentReference->initData(in_dat);


					//tanne check for shadowing other name definitions!!!!!!!!!!!!!!!!


					#ifdef DEBUG_RAE_HUMAN
						cout<<"initData: "<<set_token<<" is new initData for: "<<currentReference->type()<<" "<<currentReference->namespaceString()<<"\n";
						//rae::log("newDefineBuiltInType: ", currentReference->type(), " ", currentReference->name(), "\n");
					#endif
				}
				/*
				//This didn't work. see newQuote above for the new version.
				else if( currentReference && previousElement()->langTokenType() == LangTokenType::QUOTE )
				{
					//special handling for quote i.e. string initData.
					LangElement* in_dat = previousElement()->copy();
					in_dat->langTokenType(LangTokenType::INIT_DATA);
					currentReference->initData( in_dat );//copy the quote to initData.
					previousElement()->langTokenType(LangTokenType::EMPTY);//hide the quote.
				}
				*/
				else
				{
					//cout<<"ERROR: "<<lineNumber.line<<" was expecting an INIT_DATA. It should be a number or new.\n";
					reportError("expecting an INIT_DATA. It should be a number or new.");
				}
				//expectingToken = LangTokenType::UNDEFINED;
				doReturnToExpectToken();
			}
		}
		else if( expectingToken() == LangTokenType::IMPORT_NAME )
		{
			if( set_token[0] == '.' )
			{
				expectingToken( LangTokenType::IMPORT_NAME );
			}
			else if( set_token == "\n" )
			{
				#ifdef DEBUG_RAE
					cout<<"Line change. End of IMPORT_NAME.\n";
					//rae::log("Line change. End of IMPORT_NAME.\n");
				#endif
				if( currentTempElement )
				{
					//currentModuleHeaderFileName = currentTempElement->searchLast(LangTokenType::IMPORT_NAME)->name();
					#ifdef DEBUG_RAE
					//	cout<<"last module name was: "<<currentModuleHeaderFileName;
					#endif
					
					/*
					string a_import_whole_name = "";

					int not_on_first = 0;

					foreach(LangElement* elem, currentTempElement->langElements)
					{
						if( elem->langTokenType() == LangTokenType::IMPORT_NAME )
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
							break;//break the foreach... hope this works.
						}
					}
					*/

					string a_import_whole_name = currentTempElement->importName("/");//a bit of a hack...
					//and the func that it is calling replace . with /

					#ifdef DEBUG_RAE_HUMAN
						cout<<"newImport: ";
						//rae::log("newImport: ");
						cout<<a_import_whole_name;
						cout<<"\n";
						//rae::log("\n");
					#endif

					listOfImports.push_back(currentTempElement);

					newImportSignal(a_import_whole_name);

				}

				newLine();
				//expectingToken = LangTokenType::UNDEFINED;
				doReturnToExpectToken();
				currentTempElement = 0;
			}
			else
			{
				if( currentTempElement )
					currentTempElement->newLangElement(lineNumber, LangTokenType::IMPORT_NAME, TypeType::UNDEFINED, set_token);

				expectingToken(LangTokenType::IMPORT_NAME);
			}
		}
		else if( expectingToken() == LangTokenType::MODULE_NAME )
		{
			if( set_token[0] == '.' )
			{
				//cout<<"Got func_def (. Waiting return_types.\n";
				
				expectingToken(LangTokenType::MODULE_NAME);
			}
			else if( set_token == "\n" )
			{
				#ifdef DEBUG_RAE
					cout<<"Line change. End of MODULE_NAME.\n";
					//rae::log("Line change. End of MODULE_NAME.\n");
				#endif
				if( currentModule )
				{
					currentModuleHeaderFileName = currentModule->searchLast(LangTokenType::MODULE_NAME)->name();
					#ifdef DEBUG_RAE
						cout<<"last module name was: "<<currentModuleHeaderFileName;
						//rae::log("last module name was: ", currentModuleHeaderFileName);
					#endif

					#ifdef DEBUG_RAE_HUMAN
						cout<<"\n\nnewModule: ";
						//rae::log("newModule: ");
						foreach(LangElement* elem, currentModule->langElements)
						{
							cout<<elem->name()<<".";
							//rae::log(elem->name(), ".");
						}
						cout<<"\n";
						//rae::log("\n");
					#endif

					currentModule->name( moduleName() );//Hmm. Using a very strange function we put the module name also
					//to the name variable.

				}

				newLine();
				//expectingToken = LangTokenType::UNDEFINED;
				doReturnToExpectToken();
			}
			else
			{
				if( currentModule )
					currentModule->newLangElement(lineNumber, LangTokenType::MODULE_NAME, TypeType::UNDEFINED, set_token);

				expectingToken(LangTokenType::MODULE_NAME);
			}
		}
		/*
		else if( set_token == "\n" )
		{
				//How about this: expectingToken = LangTokenType::UNDEFINED;
				if( currentParentElement() )
				{
					currentParentElement()->newLangElement(LangTokenType::NEWLINE, set_token);
				}

			return;
		}*/
		else if( expectingToken() == LangTokenType::PARENTHESIS_BEGIN_LOG )
		{
			if( set_token == "(" )
			{
				newParenthesisBegin(LangTokenType::PARENTHESIS_BEGIN_LOG, set_token);
			}
			else
			{
				//cout<<"ERROR: ";
				//rae::log("ERROR: ");
				//lineNumber.printOut();
				//cout<<" calling log(). No parenthesis after log.";
				//rae::log(" calling log(). No parenthesis after log.");

				reportError("calling log(). No parenthesis after log.");
			}

			//expectingToken = LangTokenType::UNDEFINED;
			doReturnToExpectToken();
		}
		else if( expectingToken() == LangTokenType::PARENTHESIS_BEGIN_LOG_LN )
		{
			if( set_token == "(" )
			{
				newParenthesisBegin(LangTokenType::PARENTHESIS_BEGIN_LOG_LN, set_token);
			}
			else
			{
				//cout<<"ERROR: ";
				//"ERROR: "
				//lineNumber.printOut();
				//cout<<" calling log_ln(). No parenthesis after log_ln.";
				
				reportError(" calling log_ln(). No parenthesis after log_ln.");
			}

			//expectingToken = LangTokenType::UNDEFINED;
			doReturnToExpectToken();
		}
		else if( expectingToken() == LangTokenType::NUMBER_AFTER_DOT )
		{
			if( previousElement() && (previousToken() == LangTokenType::NUMBER || previousToken() == LangTokenType::INIT_DATA) )//the previousElement() check is needed because we use it next.
			{
				if( isNumericChar(set_token[0]) )
				{	
					previousElement()->name( previousElement()->name() + set_token );
					//expectingToken = LangTokenType::UNDEFINED;
					doReturnToExpectToken();
				}
				else
				{
					//cout<<"ERROR: "<<lineNumber.line<<" float number messed up, after dot.";
					reportError(" float number messed up, after dot.");
				}
			}
		}
		/*else if( expectingToken() == LangTokenType::DEFINE_C_ARRAY_NAME )
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
					//|| previousElement()->langTokenType() == TypeType::DEFINE_ARRAY_IN_CLASS)
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
				//expectingToken = LangTokenType::UNDEFINED;
				doReturnToExpectToken();
			}
		}*/
		else if( expectingToken() == LangTokenType::ARRAY_VECTOR_STUFF )
		{
			if( set_token == "[" )
			{
				//Shouldn't happen.
				reportError("Duplicate [. Compiler TODO.");
				//expectingToken(LangTokenType::VECTOR_STUFF);
			}
			else if( set_token == "]" )
			{
				if(previousElement())
				{
					if( previousElement()->langTokenType() == LangTokenType::DEFINE_REFERENCE )
					{
						#ifdef DEBUG_RAE_PARSER
						cout<<"making the previous DEFINE_REFERENCE_IN_CLASS an array.\n";
						#endif
						/*
						//SomeClass[] someVector
						//----here->------------
						//OLD behaviour turned it into a C_ARRAY, but now it's the preferred way to use vectors.
						previousElement()->typeType(TypeType::C_ARRAY);
						expectingToken(LangTokenType::DEFINE_C_ARRAY_NAME);
						*/
						previousElement()->typeType(TypeType::VECTOR);
						previousElement()->createTemplateSecondType( previousElement()->type() );
						previousElement()->type("vector");
						expectingToken(LangTokenType::VECTOR_NAME);
					}
					else
					{
						reportError("An ending array bracket ] in a strange place. The element before is not a type. It is: " + previousElement()->toString() );
					}
				}
			}
			else
			{
				//A static array. TODO.
				reportError("Maybe a static array. TODO.");
			}
		}
		else if( expectingToken() == LangTokenType::VECTOR_STUFF )
		{
			if( set_token == "!" )
			{
				expectingToken(LangTokenType::VECTOR_STUFF);
			}
			else if( set_token == "(" )
			{
				expectingToken(LangTokenType::VECTOR_STUFF);
			}
			else if( set_token == ")" )
			{
				expectingToken(LangTokenType::VECTOR_NAME);
			}
			else
			{
				//if(currentTemplate)
				//{
				//	currentTemplate->type(set_token);
				//}
				if(currentReference)
				{
					#ifdef DEBUG_RAE_HUMAN
						cout<<"set vector second type. to: "<<set_token<<" for: "<<currentReference->toString()<<"\n";
					#endif
					//currentReference->type(set_token);
					currentReference->createTemplateSecondType(set_token);
				}
				expectingToken(LangTokenType::VECTOR_STUFF);
			}
		}
		else if( expectingToken() == LangTokenType::VECTOR_NAME )
		{		
			//if(currentTemplate)
			//{
			//	currentTemplate->name(set_token);
			//}
			if(currentReference)
			{
				#ifdef DEBUG_RAE_HUMAN
					cout<<"set vector name. to: "<<set_token<<" for: "<<currentReference->toString()<<"\n";
				#endif
				currentReference->name(set_token);
			}
			//expectingToken = LangTokenType::UNDEFINED;
			doReturnToExpectToken();			
		}
		else if( expectingToken() == LangTokenType::CLASS_TEMPLATE_SECOND_TYPE )
		{
			//if( set_token == "!" )//No, we won't get this on templates... We already got it.
			//{
				//expectingToken(LangTokenType::TEMPLATE_STUFF);
			//}
			//else
			/*if( set_token == "(" )//and we don't get this either...
			{
				//just eat this token.
				//expectingToken(LangTokenType::TEMPLATE_STUFF);
			}
			else */
			if( set_token == ")" )
			{
				//end this simple class template.
				doReturnToExpectToken();
			}
			else //the template type inside !(type) aka TEMPLATE_SECOND_TYPE
			{
				if(currentClass)
				{
					#ifdef DEBUG_RAE_HUMAN
						cout<<"Add template second type to simple template class: "<<set_token<<" for: "<<currentClass->toString()<<"\n";
					#endif
					//currentClass->newLangElement(lineNumber, LangTokenType::TEMPLATE_SECOND_TYPE, TypeType::TEMPLATE/*are you sure this is not confusing...*/, /*string set_name:*/ "undefined", /*string set_type:*/ set_token );
					currentClass->createTemplateSecondType(set_token);
				}
				//expectingToken(LangTokenType::TEMPLATE_STUFF);
			}
		}
		else if( expectingToken() == LangTokenType::TEMPLATE_STUFF )
		{
			//if( set_token == "!" )//No, we won't get this on templates... We already got it.
			//{
				//expectingToken(LangTokenType::TEMPLATE_STUFF);
			//}
			//else
			if( set_token == "(" )
			{
				//just eat this token.
				//expectingToken(LangTokenType::TEMPLATE_STUFF);
			}
			else if( set_token == ")" )
			{
				expectingToken(LangTokenType::TEMPLATE_NAME);
			}
			else //the template type inside !(type)
			{
				//if(currentTemplate)
				//{
				//	currentTemplate->type(set_token);
				//}
				if(currentReference)
				{
					#ifdef DEBUG_RAE_HUMAN
						cout<<"Add subtype template definition type. to: "<<set_token<<" for: "<<currentReference->toString()<<"\n";
					#endif
					//currentReference->type(set_token);
					//currentReference->newLangElement(lineNumber, LangTokenType::TEMPLATE_SECOND_TYPE, TypeType::TEMPLATE/*are you sure this is not confusing...*/, /*string set_name:*/ "undefined", /*string set_type:*/ set_token );
						currentReference->createTemplateSecondType( set_token );
				}
				//expectingToken(LangTokenType::TEMPLATE_STUFF);
			}
		}
		else if( expectingToken() == LangTokenType::TEMPLATE_NAME )
		{		
			//if(currentTemplate)
			//{
			//	currentTemplate->name(set_token);
			//}
			if(currentReference)
			{
				#ifdef DEBUG_RAE_HUMAN
					cout<<"set template definition name. to: "<<set_token<<" for: "<<currentReference->toString()<<"\n";
				#endif
				currentReference->name(set_token);
			}
			//expectingToken = LangTokenType::UNDEFINED;
			doReturnToExpectToken();			
		}
		else if( expectingToken() == LangTokenType::UNDEFINED )
		{
			//reserved words:

			if( set_token == "\n" )
			{
					newLine();
			}
			else if( set_token == ";" )
			{
				newLangElement(LangTokenType::SEMICOLON, TypeType::UNDEFINED, set_token);
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
				if( previousElement() && (previousToken() == LangTokenType::NUMBER || previousToken() == LangTokenType::INIT_DATA) )
				{
					//newLangElement(LangTokenType::DOT, set_token);
					previousElement()->name( previousElement()->name() + set_token );
					expectingToken(LangTokenType::NUMBER_AFTER_DOT);
				}
				else
				{
					newLangElement(LangTokenType::REFERENCE_DOT, TypeType::UNDEFINED, set_token);
				}
			}
			else if( set_token == "," )
			{
				/*
				LangElement* stack_elem;
				LangTokenType::e parenthesis_type;

				//if( isInsideLogStatement )
				//{
					stack_elem = parenthesisStack.back();
				//}
				
				if( stack_elem )
				{
					//We just do matching: is it good enough:
					parenthesis_type = LangTokenType::matchParenthesisEnd(stack_elem->langTokenType());
				}
				*/
				
				LangTokenType::e parenthesis_type = parenthesisStackTokenType();

				if( parenthesis_type == LangTokenType::PARENTHESIS_BEGIN_LOG || parenthesis_type == LangTokenType::PARENTHESIS_BEGIN_LOG_LN )
				{
					newLangElement(LangTokenType::LOG_SEPARATOR, TypeType::UNDEFINED, set_token);//it is still a comma "," but we write it out as << for C++
				}
				else
				{
					newLangElement(LangTokenType::COMMA, TypeType::UNDEFINED, set_token);
				}
			}
			else if( set_token == "val" )
			{
				expectingToken(LangTokenType::DEFINE_REFERENCE);
				expectingTypeType(TypeType::VAL);
			}
			else if( set_token == "ref" )
			{
				expectingToken(LangTokenType::DEFINE_REFERENCE);
				expectingTypeType(TypeType::REF);
			}
			else if( set_token == "opt" )
			{
				expectingToken(LangTokenType::DEFINE_REFERENCE);
				expectingTypeType(TypeType::OPT);
			}
			else if( set_token == "link" )
			{
				expectingToken(LangTokenType::DEFINE_REFERENCE);
				expectingTypeType(TypeType::LINK);
			}
			else if( set_token == "func" )
			{
				#ifdef DEBUG_RAE
				cout<<"Got func. Waiting func_definition.\n";
				//rae::log("Got func. Waiting func_definition.\n");
				#endif
				expectingToken(LangTokenType::FUNC_DEFINITION);
				newFunc();
			}
			else if( set_token == "bool" )
			{
				expectingToken(LangTokenType::DEFINE_BUILT_IN_TYPE_NAME);
				newDefineBuiltInType(BuiltInType::BOOL, set_token);
			}
			else if( set_token == "byte" )
			{
				expectingToken(LangTokenType::DEFINE_BUILT_IN_TYPE_NAME);
				newDefineBuiltInType(BuiltInType::BYTE, set_token);
			}
			else if( set_token == "ubyte" )
			{
				expectingToken(LangTokenType::DEFINE_BUILT_IN_TYPE_NAME);
				newDefineBuiltInType(BuiltInType::UBYTE, set_token);
			}
			else if( set_token == "char" )
			{
				expectingToken(LangTokenType::DEFINE_BUILT_IN_TYPE_NAME);
				newDefineBuiltInType(BuiltInType::CHAR, set_token);
			}
			else if( set_token == "wchar" )
			{
				expectingToken(LangTokenType::DEFINE_BUILT_IN_TYPE_NAME);
				newDefineBuiltInType(BuiltInType::WCHAR, set_token);
			}
			else if( set_token == "dchar" )
			{
				expectingToken(LangTokenType::DEFINE_BUILT_IN_TYPE_NAME);
				newDefineBuiltInType(BuiltInType::DCHAR, set_token);
			}
			else if( set_token == "int" )
			{
				expectingToken(LangTokenType::DEFINE_BUILT_IN_TYPE_NAME);
				newDefineBuiltInType(BuiltInType::INT, set_token);
			}
			else if( set_token == "uint" )
			{
				expectingToken(LangTokenType::DEFINE_BUILT_IN_TYPE_NAME);
				newDefineBuiltInType(BuiltInType::UINT, set_token);
			}
			else if( set_token == "long" )
			{
				expectingToken(LangTokenType::DEFINE_BUILT_IN_TYPE_NAME);
				newDefineBuiltInType(BuiltInType::LONG, set_token);
			}
			else if( set_token == "ulong" )
			{
				expectingToken(LangTokenType::DEFINE_BUILT_IN_TYPE_NAME);
				newDefineBuiltInType(BuiltInType::ULONG, set_token);
			}
			else if( set_token == "float" )
			{
				expectingToken(LangTokenType::DEFINE_BUILT_IN_TYPE_NAME);
				newDefineBuiltInType(BuiltInType::FLOAT, set_token);
			}
			else if( set_token == "double" )
			{
				expectingToken(LangTokenType::DEFINE_BUILT_IN_TYPE_NAME);
				newDefineBuiltInType(BuiltInType::DOUBLE, set_token);
			}
			/*else if( set_token == "real" )
			{
				expectingToken = LangTokenType::DEFINE_BUILT_IN_TYPE_NAME;
				newDefineBuiltInType(BuiltInType::REAL, set_token);
			}*/
			else if( set_token == "string" )
			{
				expectingToken(LangTokenType::DEFINE_BUILT_IN_TYPE_NAME);
				newDefineBuiltInType(BuiltInType::STRING, set_token);
			}
			else if( set_token == "vector" )
			{
				expectingToken(LangTokenType::VECTOR_STUFF);
				newDefineVector("unknown", "unknown");
			}
			else if( set_token == "(" )
			{
				if( previousToken() == LangTokenType::CLASS )
				{
					#ifdef DEBUG_RAE_HUMAN
					cout<<"( means template class.\n";
					#endif
					//a simple class template
					previousElement()->typeType(TypeType::TEMPLATE);
					expectingToken(LangTokenType::CLASS_TEMPLATE_SECOND_TYPE);
				}
				else //normal (
				{
					//cout<<"normal (\n";
					//if(previousElement())
					//	cout<<"previousToken: "<<previousElement()->toString();
					//else cout<<"no previousElement!\n";
					newParenthesisBegin(LangTokenType::PARENTHESIS_BEGIN, set_token);
				}
			}
			else if( set_token == ")" )
			{
				//cout<<"normal )\n";
				newParenthesisEnd(LangTokenType::PARENTHESIS_END, set_token);
			}
			else if( set_token == "[" )
			{
				//cout<<"BRACKET we got a bracket in the normal place and it is strange!\n";

				cout<<"Wise people say, this should never get called. SourceParser line 3806.\n";
				assert(0);

				if(previousElement())
				{
					if( previousElement()->langTokenType() == LangTokenType::DEFINE_REFERENCE )
					{
						/*
!!!!!!!!!!!!!!!!!!!OK
This never gets called. Look in expecting NAME thing...
						*/

						

						#ifdef DEBUG_RAE_PARSER
						cout<<"bracket, make previous definition a DEFINE_VECTOR.\n";
						#endif
						/*
						//SomeClass[] someVector
						//----here->------------
						//OLD behaviour turned it into a C_ARRAY, but now it's the preferred way to use vectors.
						previousElement()->typeType(TypeType::C_ARRAY);
						expectingToken(LangTokenType::DEFINE_C_ARRAY_NAME);
						*/
						cout<<"A bracket but we didn't have a DEFINE_REFERENCE? Maybe it is an error.\n";
						/*not up to date, as it won't be called...
						previousElement()->typeType(TypeType::VECTOR);
						previousElement()->createTemplateSecondType( previousElement()->type() );
						previousElement()->type("vector");
						expectingToken(LangTokenType::VECTOR_NAME);
						*/
					}
					else
					{
						#ifdef DEBUG_RAE_PARSER
						cout<<"normal newBracketBegin.";
						#endif
						newBracketBegin(LangTokenType::BRACKET_BEGIN, set_token);
					}
				}
				else reportError("a bracket, but no previousElement. This is a compiler error. We'll need to fix it.");
				
			}
			else if( set_token == "]" )
			{
				/*if( previousElement() && previousElement()->langTokenType() == LangTokenType::BRACKET_BEGIN )
				{
					//previousElement()->langTokenType(LangTokenType::BRACKET_DEFINE_ARRAY_BEGIN);
					newBracketEnd(LangTokenType::BRACKET_END, set_token);
				}
				else if( previousElement() && previousElement()->langTokenType() == LangTokenType::DEFINE_ARRAY )
				{

				}
				else
				{*/
					#ifdef DEBUG_RAE_PARSER
					cout<<"normal newBracketEnd.";
					#endif
					newBracketEnd(LangTokenType::BRACKET_END, set_token);
				//}
			}
			else if( set_token == "+" )
			{
				newLangElement(LangTokenType::PLUS, TypeType::UNDEFINED, set_token);
			}
			else if( set_token == "-" )
			{
				newLangElement(LangTokenType::MINUS, TypeType::UNDEFINED, set_token);
			}
			else if( set_token == "=" )
			{
				newLangElement(LangTokenType::EQUALS, TypeType::UNDEFINED, set_token);
			}
			else if( set_token == "*" )
			{
				newLangElement(LangTokenType::STAR, TypeType::UNDEFINED, set_token);
			}
			else if( set_token == "/" )
			{
				newLangElement(LangTokenType::DIVIDE, TypeType::UNDEFINED, set_token);
			}
			else if( set_token == "<" )
			{
				newLangElement(LangTokenType::SMALLER_THAN, TypeType::UNDEFINED, set_token);
			}
			else if( set_token == ">" )
			{
				newLangElement(LangTokenType::BIGGER_THAN, TypeType::UNDEFINED, set_token);
			}
			else if( set_token == "\"" )
			{
				//shouldn't happen.
				//newLangElement(LangTokenType::STAR, set_token);
			}
			else if( set_token == "*/" )
			{
				//newLangElement(LangTokenType::DIVIDE, set_token);
			}
			else if( set_token == "/*" )
			{
				//newLangElement(LangTokenType::DIVIDE, set_token);
			}
			else if( set_token == "if" )
			{
				newLangElement(LangTokenType::IF, TypeType::UNDEFINED, set_token);
			}
			else if( set_token == "for" )
			{
				newLangElement(LangTokenType::FOR, TypeType::UNDEFINED, set_token);
			}
			else if( set_token == "foreach" )
			{
				newLangElement(LangTokenType::FOREACH, TypeType::UNDEFINED, set_token);
			}
			else if( set_token == "log_ln" )
			{
				//isInsideLogStatement = true;
				newLangElement(LangTokenType::LOG_LN, TypeType::UNDEFINED, set_token);
				expectingToken(LangTokenType::PARENTHESIS_BEGIN_LOG_LN);

			}
			else if( set_token == "log" )
			{
				//isInsideLogStatement = true;
				newLangElement(LangTokenType::LOG, TypeType::UNDEFINED, set_token);
				expectingToken(LangTokenType::PARENTHESIS_BEGIN_LOG);
			}
			/*
			//These don't work anymore as : is now it's own token. We must wait for it...
			else if( set_token == "public:" )
			{
				newLangElement(LangTokenType::VISIBILITY_DEFAULT, set_token);
			}
			else if( set_token == "protected:" )
			{
				newLangElement(LangTokenType::VISIBILITY_DEFAULT, set_token);
			}
			else if( set_token == "library:" )
			{
				newLangElement(LangTokenType::VISIBILITY_DEFAULT, set_token);
			}
			else if( set_token == "private:" )//what's the real use of private, anyway?
			{
				newLangElement(LangTokenType::VISIBILITY_DEFAULT, set_token);
			}
			*/
			else if( set_token == "public" )
			{
				newLangElement(LangTokenType::VISIBILITY, TypeType::UNDEFINED, set_token );
			}
			else if( set_token == "protected" )
			{
				newLangElement(LangTokenType::VISIBILITY, TypeType::UNDEFINED, set_token);
			}
			else if( set_token == "library" )
			{
				//TODO
				newLangElement(LangTokenType::VISIBILITY, TypeType::UNDEFINED, set_token);
			}
			else if( set_token == "hidden" )
			{
				//TODO
				newLangElement(LangTokenType::VISIBILITY, TypeType::UNDEFINED, set_token);
			}
			else if( set_token == "private" )//what's the real use of private, anyway?
			{
				newLangElement(LangTokenType::VISIBILITY, TypeType::UNDEFINED, set_token);
			}
			else if( set_token == ":" )
			{
				if( previousElement() )
				{
					if( previousElement()->langTokenType() == LangTokenType::VISIBILITY )
					{
						//change it.
						previousElement()->langTokenType(LangTokenType::VISIBILITY_DEFAULT);
						////Now we don't add, as we already added before.
						//add :
						//previousElement()->name( previousElement->name() + ":" );
					}
				}
			}
			else if( set_token == "!" )
			{
				cout<<"Got template!\n";

				if( previousElement() )
				{
					cout<<"Got previousElement:"<< previousElement()->toString()<<"\n";
					//TODO maybe we really need to check this:
					//if( previousElement()->langTokenType() == LangTokenType::DEFINE_REFERENCE )
					//{
					
						//change the typetype.
						previousElement()->typeType(TypeType::TEMPLATE);
					//}
				}
				expectingToken(LangTokenType::TEMPLATE_STUFF);
			}
			else if( set_token == "return" )
			{
				#ifdef DEBUG_RAE
				cout<<"Got return.\n";
				//rae::log("Got return.\n");
				#endif
				//expectingToken = LangTokenType::PARENTHESIS_BEGIN_RETURN;
				newLangElement(LangTokenType::RETURN, TypeType::UNDEFINED, set_token);
			}
			else if( set_token == "new" )
			{
				cout<<"TODO Got new. Waiting new_class.\n";
				//rae::log("TODO Got new. Waiting new_class.\n");
			//expectingToken = LangTokenType::FUNC_DEFINITION;
			//newFunc();
			}
			else if( set_token == "free" )
			{
				//cout<<"TODO Got free. Waiting free_class.\n";
				////rae::log("TODO Got free. Waiting free_class.\n");
				expectingToken(LangTokenType::FREE_NAME);
				//newFunc();
				if( previousElement() )
				{
					if( previousElement()->langTokenType() == LangTokenType::REFERENCE_DOT )
					{
						if( previous2ndElement() && (previous2ndElement()->langTokenType() == LangTokenType::USE_REFERENCE )
							//|| previous2ndElement->langTokenType() == LangTokenType::UNKNOWN_USE_REFERENCE)
						   )
						{
							//hide the REFERENCE_DOT from something.free
							previousElement()->langTokenType(LangTokenType::UNDEFINED);
							previousElement()->name( "" );

							previous2ndElement()->langTokenType(LangTokenType::FREE);

							//expectingToken(LangTokenType::UNDEFINED);
							doReturnToExpectToken();
						}
					}	
				}
			}
			else if( set_token == "delete" )
			{
				cout<<"TODO Got delete. Waiting delete_class.\n";
				//rae::log("TODO Got delete. Waiting delete_class.\n");
			//expectingToken = LangTokenType::FUNC_DEFINITION;
			//newFunc();
			}
			else if( set_token == "override" )
			{
				newLangElement(LangTokenType::OVERRIDE, TypeType::UNDEFINED, set_token);
			}
			else if( set_token == "class" )
			{
			////rae::log("found WORD class:>", set_token, "<\n");
				expectingToken(LangTokenType::CLASS_NAME);
			}
			else if( set_token == "import" )
			{
				newImport(set_token);	
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

			/*
			if( set_token == "\n" )
			{
					newLine();
			}
			else if( set_token == ";" )
			{
				newLangElement(LangTokenType::SEMICOLON, set_token);
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
				newLangElement(LangTokenType::REFERENCE_DOT, set_token);
			}
			else if( set_token == "func" )
			{
				#ifdef DEBUG_RAE
				//rae::log("Got func. Waiting func_definition.\n");
				#endif
				expectingToken = LangTokenType::FUNC_DEFINITION;
				newFunc();
			}
			else if( set_token == "int" )
			{
				newLangElement(LangTokenType::INT, set_token);
			}
			else if( set_token == "(" )
			{
				newParenthesisBegin(LangTokenType::PARENTHESIS_BEGIN, set_token);
			}
			else if( set_token == ")" )
			{
				newParenthesisEnd(LangTokenType::PARENTHESIS_END, set_token);
			}
			else if( set_token == "+" )
			{
				newLangElement(LangTokenType::PLUS, set_token);
			}
			else if( set_token == "-" )
			{
				newLangElement(LangTokenType::MINUS, set_token);
			}
			else if( set_token == "=" )
			{
				newLangElement(LangTokenType::EQUALS, set_token);
			}
			else if( set_token == "/" )
			{
				newLangElement(LangTokenType::DIVIDE, set_token);
			}
			else if( set_token == "if" )
			{
				newLangElement(LangTokenType::IF, set_token);
			}
			else if( set_token == "log_ln" )
			{
				newLangElement(LangTokenType::LOG_LN, set_token);
				expectingToken = LangTokenType::PARENTHESIS_BEGIN_LOG_LN;
			}
			else if( set_token == "log" )
			{
				newLangElement(LangTokenType::LOG, set_token);
				expectingToken = LangTokenType::PARENTHESIS_BEGIN_LOG;
			}
			else if( set_token == "public:" )
			{
				newLangElement(LangTokenType::VISIBILITY_DEFAULT, set_token);
			}
			else if( set_token == "protected:" )
			{
				newLangElement(LangTokenType::VISIBILITY_DEFAULT, set_token);
			}
			else if( set_token == "library:" )
			{
				newLangElement(LangTokenType::VISIBILITY_DEFAULT, set_token);
			}
			else if( set_token == "private:" )//what's the real use of private, anyway?
			{
				newLangElement(LangTokenType::VISIBILITY_DEFAULT, set_token);
			}
			else if( set_token == "public" )
			{
				newLangElement(LangTokenType::VISIBILITY, set_token);
			}
			else if( set_token == "protected" )
			{
				newLangElement(LangTokenType::VISIBILITY, set_token);
			}
			else if( set_token == "library" )
			{
				newLangElement(LangTokenType::VISIBILITY, set_token);
			}
			else if( set_token == "private" )//what's the real use of private, anyway?
			{
				newLangElement(LangTokenType::VISIBILITY, set_token);
			}
			else if( set_token == "return" )
			{
				#ifdef DEBUG_RAE
				//rae::log("Got return.\n");
				#endif
				//expectingToken = LangTokenType::PARENTHESIS_BEGIN_RETURN;
				newLangElement(LangTokenType::RETURN, set_token);
			}
			else if( set_token == "new" )
			{
				//rae::log("TODO Got new. Waiting new_class.\n");
			//expectingToken = LangTokenType::FUNC_DEFINITION;
			//newFunc();
			}
			else if( set_token == "delete" )
			{
				//rae::log("TODO Got delete. Waiting delete_class.\n");
			//expectingToken = LangTokenType::FUNC_DEFINITION;
			//newFunc();
			}
			else if( set_token == "class" )
			{
			////rae::log("found WORD class:>", set_token, "<\n");
				expectingToken = LangTokenType::CLASS_NAME;
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
		else if( expectingToken() == LangTokenType::FREE_NAME )
		{
			//TODO check if name is a valid name...
			newLangElement(LangTokenType::FREE, TypeType::UNDEFINED, set_token);
			//expectingToken = LangTokenType::UNDEFINED;
			doReturnToExpectToken();
		}
		else if( expectingToken() == LangTokenType::DEFINE_BUILT_IN_TYPE_NAME )
		{
			if( currentReference )
			{
				setNameAndCheckForPreviousDefinitions(currentReference, set_token);

				/*if( checkIfNewDefinedNameIsValid(currentReference, set_token) )
				{
					currentReference->name(set_token);
				}
				else
				{
					reportError("redefinition of name: " + set_token);
				}*/
				
			}

			//if( currentReference->parentLangTokenType() == LangTokenType::CLASS )
			//{
				//We have to get initData, if there is.
				expectingToken(LangTokenType::INIT_DATA);
			//}
			/*else
			{
				expectingToken = LangTokenType::UNDEFINED;
			}*/
		}
		else if( expectingToken() == LangTokenType::DEFINE_REFERENCE )
		{
			handleUserDefinedToken(set_token);
		}
		else if( expectingToken() == LangTokenType::DEFINE_REFERENCE_NAME )
		{
			if( set_token == "[")
			{
				#ifdef DEBUG_RAE_PARSER
				cout<<"BRACKET we got a bracket in DEFINE_REFERENCE_NAME!\n";
				#endif

				if(previousElement())
				{
					if( previousElement()->langTokenType() == LangTokenType::DEFINE_REFERENCE )
					{
						expectingToken(LangTokenType::ARRAY_VECTOR_STUFF);
					}
					else
					{
						reportError("A starting array bracket [ in a strange place. The element before is not a type. It is: " + previousElement()->toString() );
					}
				}
			}
			else if( set_token == "]")
			{
				//newBracketEnd(LangTokenType::BRACKET_DEFINE_ARRAY_END, set_token);
			}
			else if( set_token == "*" )
			{
				//TODO pointer type...
				reportError("TODO pointer type.");
				newLangElement(LangTokenType::STAR, TypeType::UNDEFINED, set_token);
			}
			else if( set_token == "?" )
			{
				//A huge TODO.
				reportError("Huge todo on line 4535. handle null pointers: ????.");
				//newLangElement(LangTokenType::STAR, set_token);
			}
			else if( set_token == "!" )
			{
				cout<<"Got template!\n";

				if( previousElement() )
				{
					cout<<"Got previousElement:"<< previousElement()->toString()<<"\n";
					//TODO maybe we really need to check this:
					//if( previousElement()->langTokenType() == LangTokenType::DEFINE_REFERENCE )
					//{
					
						//change the typetype.
						previousElement()->typeType(TypeType::TEMPLATE);
					//}
				}
				expectingToken(LangTokenType::TEMPLATE_STUFF);
			}
			else if( currentReference )
			{
				//currentReference->name(set_token);
				setNameAndCheckForPreviousDefinitions(currentReference, set_token);
			
				//TODO expectingToken = LangTokenType::INIT_DATA;
				//expectingToken = LangTokenType::UNDEFINED;
				doReturnToExpectToken();
			}
			
		}
		else if( expectingToken() == LangTokenType::CLASS_NAME )
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
				//expectingToken = LangTokenType::UNDEFINED;
				doReturnToExpectToken();
			//}
		}
		else if( expectingToken() == LangTokenType::FUNC_DEFINITION )
		{
			if( set_token[0] == '\n' )
			{
				#ifdef DEBUG_RAE
					cout<<"Got NEWLINE. Ignore it because func.\n";
					//rae::log("Got NEWLINE. Ignore it because func.\n");
				#endif
				//expectingToken = LangTokenType::FUNC_RETURN_TYPE;
			}
			else if( set_token[0] == '(' )
			{
				#ifdef DEBUG_RAE
					//rae::log("Got func_def (. Waiting return_types.\n");
				#endif
				newParenthesisBegin(LangTokenType::PARENTHESIS_BEGIN_FUNC_RETURN_TYPES, "(");
				expectingRole(Role::FUNC_RETURN);
				//expectingToken(LangTokenType::FUNC_RETURN_TYPE);
			}
			else
			{
				#ifdef DEBUG_RAE
					//rae::log("Got func_name. The name could be this token:>", set_token, "< Waiting params.\n");
				#endif
				//record func name
				if( currentFunc )
				{
					currentFunc->name(set_token);
					//setNameAndCheckForPreviousDefinitions(currentReference, set_token);
					
					/*if( set_token == "this" )
					{
						currentFunc->langTokenType( LangTokenType::CONSTRUCTOR );
					}
					else if( set_token == "~this" )
					{
						currentFunc->langTokenType( LangTokenType::DESTRUCTOR );
					}*/

					//if a funcs name is new, then it is a constructor.
					if( set_token == "new" )
					{
						#ifdef DEBUG_RAE
						//rae::log("Got new. It's a CONSTRUCTOR.\n");
						#endif
						currentFunc->langTokenType( LangTokenType::CONSTRUCTOR );
						listOfConstructors.push_back(currentFunc);
					}
					else if( set_token == "free" )
					{
						#ifdef DEBUG_RAE
						//rae::log("Got free. It's a DESTRUCTOR.\n");
						#endif
						currentFunc->langTokenType( LangTokenType::DESTRUCTOR );
						listOfDestructors.push_back(currentFunc);
					}
					else if( set_token == "delete" )
					{
						//#ifdef DEBUG_RAE
						reportError("\"delete\" keyword is not used in the Rae programming language. Please use \"free\" instead.");
						//rae::log("delete is deprecated. please use free instead.\n");
						//#endif
						currentFunc->langTokenType( LangTokenType::DESTRUCTOR );
						listOfDestructors.push_back(currentFunc);
					}
					else if( set_token == "main" )
					{
						#ifdef DEBUG_RAE
						//rae::log("Got main. It's a MAIN.\n");
						#endif
						currentFunc->langTokenType( LangTokenType::MAIN );
					}
				}
				expectingToken(LangTokenType::FUNC_ARGUMENT_TYPE);
			}
		}
		else if( expectingToken() == LangTokenType::FUNC_RETURN_TYPE )
		{
			if( set_token[0] == ')' )
			{
				#ifdef DEBUG_RAE
				//rae::log("Got end func return types.>", set_token, "< Waiting rest of definition.\n");
				#endif
				//TODO: check parenthesisStack:
				newParenthesisEnd(LangTokenType::PARENTHESIS_END_FUNC_RETURN_TYPES, ")");
				expectingToken(LangTokenType::FUNC_DEFINITION);
			}
			else if( set_token[0] == ',' )//a comma separates the list of return types...
			{
				#ifdef DEBUG_RAE
				//rae::log("Got comma between return types.>", set_token, "< Waiting rest func_return_type.\n");
				#endif
				expectingToken(LangTokenType::FUNC_RETURN_TYPE);
			}
			else
			{
				#ifdef DEBUG_RAE
				//rae::log("Got func_return_type. Could be:>", set_token, "<.\n");
				#endif
				if(currentFunc)
				{
					newDefineFuncReturn(set_token);
					
					/*
					LangElement* lang_el = currentFunc->newLangElement(lineNumber, LangTokenType::DEFINE_FUNC_RETURN);
					//currentFunc->addTypeToCurrentElement(set_token);
					lang_el->type(set_token);//TODO here we'd need to check the
					//userDefinedTypes if we have this, and mark it unknown if it's not...

					if( BuiltInType::isBuiltInType(set_token) )
					{
						//ok.
					}
					else
					{
						LangElement* found_elem = searchToken(set_token);
						if( found_elem )
						{
							
						}
						else
						{
							lang_el->isUnknownType(true);
							addToUnknownDefinitions( lang_el );
						}
					}
					*/
				}
				expectingToken(LangTokenType::FUNC_RETURN_NAME);
				//record return type1, 2 , 3 etc.
			}
		}
		else if( expectingToken() == LangTokenType::FUNC_RETURN_NAME )
		{
			if( set_token[0] == ')' )
			{
				#ifdef DEBUG_RAE
				//rae::log("Got end func returns.>", set_token, "< Waiting rest of definition.\n");
				#endif
				//TODO: check parenthesisStack:
				newParenthesisEnd(LangTokenType::PARENTHESIS_END_FUNC_RETURN_TYPES, ")");
				expectingToken(LangTokenType::FUNC_DEFINITION);
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
				expectingToken(LangTokenType::FUNC_RETURN_TYPE);
				//record return type1, 2 , 3 etc.
			}
		}
		else if( expectingToken() == LangTokenType::FUNC_ARGUMENT_TYPE )
		{
			if( set_token[0] == '(' )
			{
				#ifdef DEBUG_RAE
				//rae::log("Got param start (: >", set_token, "< Still waiting for params.\n");
				#endif
				//TODO: check parenthesisStack:
				newParenthesisBegin(LangTokenType::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES, "(");
				
				//finalize func
				//expectingToken = LangTokenType::FUNCTION_BODY;
			}
			else if( set_token[0] == ')' )
			{
				#ifdef DEBUG_RAE
				//rae::log("Got final ): >", set_token, "< Going for the func BODY.\n");
				#endif
				//finalize func
				/*if( currentFunc )
				{
					currentFunc->newLangElement(lineNumber, LangTokenType::PARENTHESIS_END_FUNC_DEFINITION, ")");
				}*/
				//TODO: check parenthesisStack:
				newParenthesisEnd(LangTokenType::PARENTHESIS_END_FUNC_PARAM_TYPES, ")");

				//expectingToken = LangTokenType::FUNCTION_BODY;
				//expectingToken = LangTokenType::UNDEFINED;
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
				/*if( currentFunc )
				{
					currentFunc->newLangElement(lineNumber, LangTokenType::COMMA, ",");
				}*/
				newLangElement(LangTokenType::COMMA, TypeType::UNDEFINED, ",");
				expectingToken(LangTokenType::FUNC_ARGUMENT_TYPE);
			}
			else if(set_token == "override")
			{
				/*if(currentFunc)
				{
					currentFunc->newLangElement(lineNumber, LangTokenType::OVERRIDE);
				}*/
				newLangElement(LangTokenType::OVERRIDE, TypeType::UNDEFINED, set_token);
			}
			else if( set_token == "vector" )//Ooh, it's a vector
			{
				#ifdef DEBUG_RAE
				//rae::log("Got comma between param types.>", set_token, "< Waiting rest FUNC_ARGUMENT_TYPE.\n");
				#endif
				/*if(currentReference)
				{
					currentReference->typeType(TypeType::C_ARRAY);
				}*/
				cout<<"TODO enable vector FUNC_ARGUMENTs.\n";
				//returnToExpect( LangTokenType::FUNC_ARGUMENT_NAME );
				//expectingToken( LangTokenType::VECTOR_STUFF );
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
					/*
					currentFunc->newLangElement(lineNumber, LangTokenType::DEFINE_FUNC_ARGUMENT);
					currentFunc->addTypeToCurrentElement(set_token);

					if( BuiltInType::isBuiltInType(set_token) )
					{
						//ok.
					}
					else
					{
						LangElement* found_elem = searchToken(set_token);
						if( found_elem )
						{
							
						}
						else
						{
							currentFunc->currentElement()->isUnknownType(true);
							addToUnknownDefinitions( currentFunc->currentElement() );
						}
					}
					*/
				}
				expectingToken(LangTokenType::FUNC_ARGUMENT_NAME);
				//record return type1, 2 , 3 etc.
			}
		}
		else if( expectingToken() == LangTokenType::FUNC_ARGUMENT_NAME )
		{
			cout<<"Remove this FUNC_ARGUMENT thing, and just use parent() which is scope, to know if it's inside a func def. Use DEFINE_REFERENCE for them instead.\n";
			assert(0);

			if( set_token[0] == '(' )
			{
				//rae::log("ERROR: ");
				lineNumber.printOut();
				//rae::log(" Got param start, while expecting FUNC_ARGUMENT_NAME (: >", set_token, "< Still waiting for params.\n");

				//TODO: check parenthesisStack:
				newParenthesisBegin(LangTokenType::PARENTHESIS_BEGIN, "(");

				//finalize func
				//expectingToken = LangTokenType::FUNCTION_BODY;
			}
			else if( set_token[0] == ')' )
			{
				#ifdef DEBUG_RAE
				//rae::log("Got final ), func_param not named: >", set_token, "< Going for the func BODY.\n");
				#endif
				//finalize func
				/*if( currentFunc )
				{
					currentFunc->newLangElement(lineNumber, LangTokenType::PARENTHESIS_END_FUNC_DEFINITION, ")");
				}*/
				
				//TODO: check parenthesisStack:
				newParenthesisEnd(LangTokenType::PARENTHESIS_END_FUNC_PARAM_TYPES, ")");
				
				//expectingToken = LangTokenType::FUNCTION_BODY;
				//expectingToken = LangTokenType::UNDEFINED;
				doReturnToExpectToken();
			}
			else if( set_token[0] == ',' )//a comma separates the list of param types...
			{
				#ifdef DEBUG_RAE
				//rae::log("Got comma between param types.>", set_token, "< Waiting rest FUNC_ARGUMENT_TYPE.\n");
				#endif
				newLangElement(LangTokenType::COMMA, TypeType::UNDEFINED, ",");
				expectingToken(LangTokenType::FUNC_ARGUMENT_TYPE);
			}
			else if( set_token[0] == '[' )//Ooh, it's an array...
			{
				#ifdef DEBUG_RAE
				//rae::log("Got comma between param types.>", set_token, "< Waiting rest FUNC_ARGUMENT_TYPE.\n");
				#endif
				if(currentReference)
				{
					expectingToken(LangTokenType::ARRAY_VECTOR_STUFF);
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
				reportError("An ending array bracket ] in a strange place. Element before: " + previousElement()->toString() );
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


				addReturnToExpectToken( LangTokenType::FUNC_ARGUMENT_NAME );
				expectingToken( LangTokenType::INIT_DATA );
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
				//expectingToken = LangTokenType::FUNC_ARGUMENT_TYPE;
				//record return type1, 2 , 3 etc.
			}
		}
		else
		{
			//rae::log("ERROR: ");
			lineNumber.printOut();
			//rae::log(" token: ", set_token, " expecting: ", LangTokenType::toString(expectingToken), " lineText: ", currentLine, "\n");
			//expectingToken = LangTokenType::UNDEFINED;
			doReturnToExpectToken();
		}
	
	}

	

	//return true if it was a number...
	bool handleNumber(string set_token)
	{
		if(set_token.size() == 0)
			return false;

		if( isNumericChar(set_token[0]) )
		{
			newLangElement(LangTokenType::NUMBER, TypeType::UNDEFINED, set_token);

			#ifdef DEBUG_RAE_HUMAN
				cout<<"newNumber: "<<set_token<<"\n";
				//rae::log("newNumber: ", set_token, "\n");
			#endif

			return true;
		}

		return false;
	}

	void addToClasses(LangElement* set_elem)
	{
		classes.push_back(set_elem);
	}

	void addToUserDefinedTokens(LangElement* set_elem)
	{
		userDefinedTokens.push_back(set_elem);
	}

	void addToCheckForPreviousDefinitionsList(LangElement* set_elem)
	{
		checkForPreviousDefinitionsList.push_back(set_elem);
	}

	void addToUnknownDefinitions(LangElement* set_elem)
	{
		unknownDefinitions.push_back(set_elem);
	}

	//this also add to unknownUseReferences, but we don't really know what it is... might be helpful later. or not.
	void addToUnknowns(LangElement* set_elem)
	{
		unknownUseReferences.push_back(set_elem);
	}

	void addToUnknownUseReferences(LangElement* set_elem)
	{
		unknownUseReferences.push_back(set_elem);
	}

	void addToUnknownUseMembers(LangElement* set_elem)
	{
		unknownUseMembers.push_back(set_elem);
	}

	void cleanUpUnknownTokens(vector<LangElement*>& unknown_tokens)
	{
		#ifdef DEBUG_RAE_PARSER
		cout<<"unknown_tokens.size before remove: "<<unknown_tokens.size()<<"\n";
		#endif
		//remove unknown tokens that are now known:
		//boost::remove_if( unknownDefinitions, bind(&LangElement::langTokenType, _1) != LangTokenType::UNKNOWN_DEFINITION );
		//boost::remove_if( unknownDefinitions, bind(&LangElement::isUnknownType, _1) != true );

		unknown_tokens.erase(std::remove_if(unknown_tokens.begin(),
                         unknown_tokens.end(),
                         boost::bind(&LangElement::isUnknownType, _1) != true),
          unknown_tokens.end());

		#ifdef DEBUG_RAE_PARSER
		cout<<"unknown_tokens.size after remove: "<<unknown_tokens.size()<<"\n";
		#endif
	}

	void cleanUpUnknownDefinitions()
	{
		#ifdef DEBUG_RAE_PARSER
		cout<<"unknownDefinitions.size before remove: "<<unknownDefinitions.size()<<"\n";
		#endif
		//remove unknown tokens that are now known:
		//boost::remove_if( unknownDefinitions, bind(&LangElement::langTokenType, _1) != LangTokenType::UNKNOWN_DEFINITION );
		//boost::remove_if( unknownDefinitions, bind(&LangElement::isUnknownType, _1) != true );

		unknownDefinitions.erase(std::remove_if(unknownDefinitions.begin(),
                         unknownDefinitions.end(),
                         boost::bind(&LangElement::isUnknownType, _1) != true),
          unknownDefinitions.end());

		#ifdef DEBUG_RAE_PARSER
		cout<<"unknownDefinitions.size after remove: "<<unknownDefinitions.size()<<"\n";
		#endif
	}

	void cleanUpUnknownUseReferences()
	{
		#ifdef DEBUG_RAE_PARSER
		cout<<"unknownUseReferences.size before remove: "<<unknownUseReferences.size()<<"\n";
		#endif
		//remove unknown tokens that are now known:
		//boost::remove_if( unknownUseReferences, bind(&LangElement::langTokenType, _1) != LangTokenType::UNKNOWN_USE_REFERENCE );
		//boost::remove_if( unknownUseReferences, bind(&LangElement::isUnknownType, _1) != true );

		unknownUseReferences.erase(std::remove_if(unknownUseReferences.begin(),
                         unknownUseReferences.end(),
                         boost::bind(&LangElement::isUnknownType, _1) != true),
          unknownUseReferences.end());

		#ifdef DEBUG_RAE_PARSER
		cout<<"unknownUseReferences.size after remove: "<<unknownUseReferences.size()<<"\n";
		#endif
	}

	void cleanUpUnknownUseMembers()
	{
		#ifdef DEBUG_RAE_PARSER
		cout<<"unknownUseMembers.size before remove: "<<unknownUseMembers.size()<<"\n";
		#endif
		//remove unknown tokens that are now known:
		//boost::remove_if( unknownUseMembers, bind(&LangElement::langTokenType, _1) != LangTokenType::UNKNOWN_USE_MEMBER );
		//boost::remove_if( unknownUseMembers, bind(&LangElement::isUnknownType, _1) != true );

		unknownUseMembers.erase(std::remove_if(unknownUseMembers.begin(),
                         unknownUseMembers.end(),
                         boost::bind(&LangElement::isUnknownType, _1) != true),
          unknownUseMembers.end());

		#ifdef DEBUG_RAE_PARSER
		cout<<"unknownUseMembers.size after remove: "<<unknownUseMembers.size()<<"\n";
		#endif
	}

	/*
	void addToUnknownUserTokens(LangElement* set_elem)
	{
		unknownUserTokens.push_back(set_elem);
	}
	*/

	/*
	LangElement* searchUnknownUserTokens(string set_token)
	{
		//TODO this is very inefficient... just a draft. use a map... or something.
		foreach(LangElement* elem, unknownUserTokens)
		{
			if( elem->name() == set_token )
			{
				return elem;
			}
		}

		//else
		return 0;
	}
	*/

	//Doesn't check for validity
	LangElement* searchUserDefinedTokens(string set_token)
	{
		//TODO this is very inefficient... just a draft. use a map... or something.
		foreach(LangElement* elem, userDefinedTokens)
		{
			if( elem->name() == set_token )
			{
				//checkIfTokenIsValidInCurrentContext(); we can't do this yet...
				return elem;
			}
		}

		//else
		return 0;
	}

	LangElement* classHasFunctionOrValue(string set_class, string set_name)
	{
		LangElement* found_class = 0;

		#ifdef DEBUG_RAE_PARSER
		cout<<"looking for class: "<<set_class<<" val or func: "<<set_name<<"\n";
		#endif

		foreach(LangElement* elem, userDefinedTokens)
		{
			if( elem->name() == set_class )
			{
				#ifdef DEBUG_RAE_PARSER
				cout<<"found name of class.\n";
				#endif
				if(elem->langTokenType() == LangTokenType::CLASS)
				{
					#ifdef DEBUG_RAE_PARSER
					cout<<"it is of type class.\n";
					#endif
					found_class = elem;
					break;
				}
				else
				{
					#ifdef DEBUG_RAE_PARSER
					cout<<"not a class type... DANG.\n";
					#endif
				}
			}
		}

		if( found_class )
		{
			LangElement* found_func_or_val = found_class->searchName(set_name);
			if( found_func_or_val )
			{
				#ifdef DEBUG_RAE_PARSER
				cout<<"found func or val too.\n";
				#endif
				return found_func_or_val;
			}
			else
			{
				#ifdef DEBUG_RAE_PARSER
				cout<<"didn't find func or val though.\n";
				#endif
			}
		}

		/*foreach(LangElement* elem, classes)
		{
			if( elem->name() == set_class )
			{
				//Hurray. We have that class.
				if( elem->searchName(set_name) )
				{
					//and it has that name.
					return true;
				}
			}
		}*/

		return 0;
	}

	LangElement* searchElementDefinitionInScope(LangElement* set_elem, LangElement* search_from_scope)
	{
		if(set_elem == 0 || search_from_scope == 0)
			return 0;

		foreach(LangElement* elem, search_from_scope->langElements)
		{
			if(set_elem == elem)
			{
				continue;
			}

			//If it's a definition. Then we are only interested in classes.

			//if( (set_elem->langTokenType() == LangTokenType::DEFINE_REFERENCE || set_elem->langTokenType() == LangTokenType::DEFINE_ARRAY)
			if( set_elem->isDefinition() && elem->langTokenType() == LangTokenType::CLASS )
			{
				if( elem->type() == set_elem->type() )
				{
					#ifdef DEBUG_RAE_PARSER
					cout<<"searchScope: found class: "<<elem->type()<<" : "<<elem->toString()<<"\n";
					#endif
					if( checkIfTokenIsValidInCurrentContext(set_elem, elem) )
					{
						return elem;
					}
					else
					{
						#ifdef DEBUG_RAE_PARSER
						cout<<"BUT IT*S NOT VALID!!!!\n";
						#endif
					}
				}
				//TEMP
				else
				{
					//TEMP
					//if(moduleName() == "rae.examples.Tester")
					//{
						#ifdef DEBUG_RAE_PARSER
						cout<<"type it is not: "<<elem->toString()<<"\n";
						#endif
					//}
				}
			}
			//We are only interested in definitions.
			else if( elem->isDefinition() ) //ok so we don't check for: set_elem->isUseReference()
			{
				if( elem->name() == set_elem->name() )
				{
					#ifdef DEBUG_RAE_PARSER
					cout<<"searchScope: found definition: "<<elem->name()<<" : "<<elem->toString()<<"\n";
					#endif
					if( checkIfTokenIsValidInCurrentContext(set_elem, elem) )
					{
						return elem;
					}
					else
					{
						#ifdef DEBUG_RAE_PARSER
							cout<<"BUT it wasn't valid in this context. searchScope: found definition: "<<elem->name()<<" : "<<elem->toString()<<"\n";
						#endif
					}
				}
			}
			//TEMP
			else
			{
				//TEMP
				//if(moduleName() == "rae.examples.Tester")
				//{
					#ifdef DEBUG_RAE_PARSER
					cout<<"it is not: "<<elem->toString()<<"\n";
					#endif
				//}
			}
		}

		return 0;
	}

	//return true if found previous defs.
	//false is ok.
	bool checkForPreviousDefinitions(string set_token)
	{
		LangElement* prev_def = searchToken(set_token);
		if(prev_def)
		{
			if(previous2ndToken() == LangTokenType::OVERRIDE 
				|| previousToken() == LangTokenType::OVERRIDE )
			{
				//no errors.
			}
			else
			{
				//if( previousElement() )
				//	cout<<"PREVIOUSTOKEN WAS: "<<previousElement()->toString();

				reportError("redefinition of name: " + set_token + "\nUse \"override\" to use the same name again." );
				reportError("previous definition is here: " + prev_def->toString() );
			}
			
			//elem_to_set->name(set_token);
			return true;
		}
		//else
		//{
			//elem_to_set->name(set_token);
		//}
		return false;
	}

	//return true if found previous defs.
	//false is ok. //null if ok.

	LangElement* checkForPreviousDefinitions(LangElement* set_elem)
	{
		LangElement* prev_def = searchElementAndCheckIfValid(set_elem);
		if(prev_def)
		{
			if(set_elem->previous2ndToken() == LangTokenType::OVERRIDE 
				|| set_elem->previousToken() == LangTokenType::OVERRIDE )
			{
				//no errors.
			}
			else
			{
				//if( previousElement() )
				//	cout<<"PREVIOUSTOKEN WAS: "<<previousElement()->toString();

				reportError("redefinition of name: " + set_elem->toString() + "\nUse \"override\" to use the same name again." );
				reportError("previous definition is here: " + prev_def->toString() );
			}
			
			//elem_to_set->name(set_token);
			//return true;
			return prev_def;
		}
		//else
		//{
			//elem_to_set->name(set_token);
		//}
		//return false;
		return prev_def;//null if ok.
	}	
	

	//This a func to set a name and to check if the name was used before.
	//The source file should then contain "override" so that no errors come.
	void setNameAndCheckForPreviousDefinitions(LangElement* elem_to_set, string set_token)
	{
		if(elem_to_set == 0)
		{
			reportError("Compiler error. setNameAndCheckForPreviousDefinitions. elem_to_set is null.");
			return;
		}

		LangElement* prev_def = searchToken(set_token);
		if(prev_def)
		{
			if(elem_to_set->previous2ndToken() == LangTokenType::OVERRIDE
				|| elem_to_set->previousToken() == LangTokenType::OVERRIDE )
			{
				//no errors.
			}
			else
			{
				reportError("redefinition of name: " + set_token );
				reportError("previous definition is here: " + prev_def->toString() );
			}
			
			elem_to_set->name(set_token);
		}
		else
		{
			elem_to_set->name(set_token);
		}
	}
	
	LangElement* searchElementDefinitionFromParentScopes(LangElement* set_elem, LangElement* search_from_scope)
	{
		while(search_from_scope != 0)
		{
			LangElement* res = searchElementDefinitionInScope(set_elem, search_from_scope);
			if(res)
			{
				return res;
			}
			//continue searching:
			search_from_scope = search_from_scope->parent();
		}
		return 0;
	}

	//This should be good...
	LangElement* searchElementAndCheckIfValidLocal(LangElement* set_elem)
	{
		#ifdef DEBUG_RAE_PARSER
		//TEMP:
		//if(moduleName() == "rae.examples.Tester")
		//{
			cout<<"searching for: "<<set_elem->toString()<<"\n";
		//}
		#endif

		//It seems were first searching in the current func... is that good??
		/*
		if(currentFunc)
		{
			LangElement* found_elem = currentFunc->searchName( set_elem->name() );
			if(found_elem && checkIfTokenIsValidInCurrentContext(set_elem, found_elem) )
			{
				return found_elem;
			}
		}
		*/

		//WHAAAT was I thinking: if( scopeElementStack.empty() == false )//we don't have to check for null here: && scopeElementStack.back() )
		//that was the completely wrong scope.
		//By now, we're already at the end of the source file...
		
		if( set_elem->parent() )
		{
			LangElement* res = searchElementDefinitionFromParentScopes(set_elem, set_elem->parent() );//scopeElementStack.back() );
			if(res)
			{
				#ifdef DEBUG_RAE_PARSER
				cout<<"Super. ScopeFinder found it.\n";
				#endif
				return res;
			}
		}

		//return searchUserDefinedTokens(set_token);

		//Then search in current module.
		foreach(LangElement* elem, userDefinedTokens)
		{
			//if( elem->langTokenType() == LangTokenType::DEFINE_REFERENCE || elem->langTokenType() == LangTokenType::DEFINE_REFERENCE_IN_CLASS )
			//if( set_elem->langTokenType() == LangTokenType::UNKNOWN_DEFINITION && elem->langTokenType() == LangTokenType::CLASS )
			//if( (set_elem->langTokenType() == LangTokenType::DEFINE_REFERENCE || set_elem->langTokenType() == LangTokenType::DEFINE_ARRAY)
				//&& set_elem->isUnknownType() == true //how is this needed? Who says we can't check on stuff that we already now?
			//	&& elem->langTokenType() == LangTokenType::CLASS )
			
			if(set_elem == elem)
			{
				continue;
			}

			if( set_elem->isDefinition() && elem->langTokenType() == LangTokenType::CLASS )
			{
				if( elem->type() == set_elem->type() )
				{
					#ifdef DEBUG_RAE_PARSER
					cout<<"JJJJJJEEEEEEEESSSS: found: "<<elem->type()<<" : "<<elem->toString()<<"\n";
					#endif
					if( checkIfTokenIsValidInCurrentContext(set_elem, elem) )
					{
						return elem;
					}
					else
					{
						#ifdef DEBUG_RAE_PARSER
						cout<<"BUT IT*S NOT VALID!!!!\n";
						#endif
					}
				}
				//TEMP
				else
				{
					//TEMP
					//if(moduleName() == "rae.examples.Tester")
					//{
						#ifdef DEBUG_RAE_PARSER
						cout<<"type it is not: "<<elem->toString()<<"\n";
						#endif
					//}
				}
			}
			//We are only interested in definitions.
			else if( elem->isDefinition() ) //ok so we don't check for: set_elem->isUseReference()
			{
				if( elem->name() == set_elem->name() )
				{
					#ifdef DEBUG_RAE_PARSER
					cout<<"HORDUM: found definition: "<<elem->name()<<" : "<<elem->toString()<<"\n";
					#endif
					if( checkIfTokenIsValidInCurrentContext(set_elem, elem) )
					{
						return elem;
					}
				}
			}
			//TEMP
			else
			{
				//TEMP
				//if(moduleName() == "rae.examples.Tester")
				//{
					#ifdef DEBUG_RAE_PARSER
					cout<<"it is not: "<<elem->toString()<<"\n";
					#endif
				//}
			}
		}

		//Then search in other modules outside this SourceParser.


		return 0;
	}

	LangElement* searchElementAndCheckIfValid(LangElement* set_elem)
	{
		LangElement* result = searchElementAndCheckIfValidLocal(set_elem);

		if(result)
			return result;

		//Then search in other modules outside this SourceParser.
		result = *searchElementInOtherModulesSignal(this, set_elem);

		return result;
	}
/*
	bool checkIfNewDefinedNameIsValid(LangTokenType::e set_token_type, string set_token)
	{
		//Hmm.. let's build a temp object and stuff...

		LangTokenType::e use_type_to_use = LangTokenType::UNDEFINED;

		//OK. this should be a func called something like: matchUseTypeToDefineType(LangTokenType::e set_type):
		if( set_token_type == LangTokenType::DEFINE_BUILT_IN_TYPE
			|| set_token_type == LangTokenType::DEFINE_BUILT_IN_TYPE_IN_CLASS
			|| set_token_type == LangTokenType::DEFINE_REFERENCE
			|| set_token_type == LangTokenType::DEFINE_REFERENCE_IN_CLASS
		)
		{
			use_type_to_use = LangTokenType::USE_REFERENCE;
		}
		else if( set_token_type == LangTokenType::DEFINE_VECTOR
			|| set_token_type == LangTokenType::DEFINE_VECTOR_IN_CLASS
		)
		{
			use_type_to_use = LangTokenType::USE_VECTOR;
		}
		else if( set_token_type == LangTokenType::DEFINE_ARRAY
			|| set_token_type == LangTokenType::DEFINE_ARRAY_IN_CLASS
		)
		{
			use_type_to_use = LangTokenType::USE_ARRAY;
		}

		LangElement* temp_elem = new LangElement(lineNumber, use_type_to_use, set_token, set_token );
		temp_elem->isUnknownType(true);
		temp_elem->parent( currentParentElement() );
		temp_elem->previousElement( previousElement() );

		LangElement* res = searchElementAndCheckIfValid(temp_elem);

		delete temp_elem;

		if(res)
		{
			return false; //if we got an object, then the name is NOT valid.
		}
		//else
		return true; //there's no other object with the same name...
	}
*/
	boost::signals2::signal<LangElement* (SourceParser*, LangElement*)> searchElementInOtherModulesSignal;

	//A general search that will use other smaller searches to check everywhere.
	LangElement* searchToken(string set_token)
	{
		/*if(currentFunc)
		{
			LangElement* found_elem = currentFunc->searchFunctionParams(set_token);
			if(found_elem)
			{
				return found_elem;
			}
		}

		return searchUserDefinedTokens(set_token);
		*/

		//now we do a temporary LangElement* object so we can use the normal search tools. But for funny effect, we destroy the
		//temp element afterwards!

		//and then handleUserDefinedTokens creates another one with the calls that add it to the langElements in the right places.

		LangElement* temp_elem = new LangElement(lineNumber, LangTokenType::USE_REFERENCE, TypeType::UNDEFINED, set_token, set_token );
		temp_elem->isUnknownType(true);
		temp_elem->parent( currentParentElement() );
		temp_elem->previousElement( previousElement() );

		LangElement* res = searchElementAndCheckIfValid(temp_elem);

		delete temp_elem;

		return res;
	}

	bool checkIfTokenIsValidInCurrentContext( LangElement* current_context_use, LangElement* found_definition )
	{
		#ifdef DEBUG_RAE_PARSER
		//TEMP
		string debug_string_con = ""; //testers
		if(current_context_use->name() == debug_string_con)
		{
			cout<<"checkIfTokenIsValid.\n";
		}
		#endif

		if( current_context_use && found_definition )
		{
			//if it's a definition, then it's valid.
			/*if(current_context_use->langTokenType() == LangTokenType::DEFINE_REFERENCE
				|| current_context_use->langTokenType() == LangTokenType::DEFINE_REFERENCE_IN_CLASS
				|| current_context_use->langTokenType() == LangTokenType::DEFINE_BUILT_IN_TYPE
				|| current_context_use->langTokenType() == LangTokenType::DEFINE_BUILT_IN_TYPE_IN_CLASS
				|| current_context_use->langTokenType() == LangTokenType::DEFINE_ARRAY
				|| current_context_use->langTokenType() == LangTokenType::DEFINE_ARRAY_IN_CLASS
				|| current_context_use->langTokenType() == LangTokenType::DEFINE_VECTOR
				|| current_context_use->langTokenType() == LangTokenType::DEFINE_VECTOR_IN_CLASS
				)
				*/
			if( current_context_use->isDefinition() )
			{
				#ifdef DEBUG_RAE_PARSER
				if(current_context_use->name() == debug_string_con)
			  	{
			  		cout<<"definitions are always valid.\n";
			  	}
			  	#endif

				return true;
			}

			//also if found_elem is a class or enum. We can use those always. 
			if( found_definition->isUserDefinableLangTokenType() )
			{
				#ifdef DEBUG_RAE_PARSER
				if(current_context_use->name() == debug_string_con)
		  	{
		  		cout<<"valid because the found_definition is a class or enum or other user definable type.\n";
		  	}
		  	#endif
				return true;
			}

			if( current_context_use->previousToken() == LangTokenType::REFERENCE_DOT && current_context_use->previous2ndToken() == LangTokenType::USE_REFERENCE )
			{
				//this is something like someObject.aFuncOrValue
				//We should check if SomeClass has aFuncOrValue.

				//we know that previous2ndElement is not null, because there was a token that was not UNDEFINED...

				#ifdef DEBUG_RAE_PARSER
				cout<<"current_context_use->previous2ndElement(): "<<current_context_use->previous2ndElement()->toString();
				#endif

				if( classHasFunctionOrValue( current_context_use->previous2ndElement()->type(), current_context_use->name() ) )
				{
					//It's ok.
					#ifdef DEBUG_RAE_PARSER
					cout<<"HAROP2: it's ok. it's valid. someclass.somefunc\n";
					#endif
					return true;
				}
				else
				{
					#ifdef DEBUG_RAE_PARSER
					cout<<"HAROP3: not ok. no such symbol in class.\n";
					#endif
					return false;//now we know that the class didn't have that symbol. or it isn't yet defined.
				}
			}
			else if( current_context_use->previousToken() == LangTokenType::REFERENCE_DOT )
			{
				//This is the array case, but we could use this for the case up there too. This is more recent anyway...
				//But I guess the case up there is a bit simpler so we might just leave it there too.

				LangElement* prev_ref = current_context_use->searchClosestPreviousUseReferenceOrUseVector();
				if(prev_ref)
				{
					//useVector[i].logMe
					if( prev_ref->typeType() == TypeType::VECTOR || prev_ref->typeType() == TypeType::TEMPLATE )
					{
						if( prev_ref->definitionElement() )
						{
							//useVectors definitionElement would be
							//vector!(Tester) useVector
							//and its templateSecondType is Tester, if it is found already at this point in parsing.
							LangElement* temp_second_type = prev_ref->definitionElement()->templateSecondType();
							if(temp_second_type)
							{
								//TODO we still could find if the templateSecondTypeString has another definitionElement, which would be Tester class.
								//now we just look for the string of that class and it is slow.
								//OLD TODO this is still really inefficient. Maybe first check if we have templateSecondType()
								//and then call something like: templateSecondType()->hasFunctionOrMember(). Would be faster.
								
								if( classHasFunctionOrValue( prev_ref->definitionElement()->templateSecondTypeString(), current_context_use->name() ) )
								{
									//It's ok.
									#ifdef DEBUG_RAE_PARSER
									cout<<"VECTOR reference thing: it's ok. it's valid. somevector[i].somefunc\n";
									#endif
									return true;
								}
								else
								{
									#ifdef DEBUG_RAE_PARSER
									cout<<"Vector call HAROP5: not ok. no such symbol in class.\n";
									#endif
									return false;//now we know that the class didn't have that symbol. or it isn't yet defined.
								}		
							}
							else
							{
								#ifdef DEBUG_RAE_PARSER
									cout<<"Vector call HAROP6: no templateSecondType in definitionElement, in useVector.\n";
									cout<<"definitionElement was: "<<prev_ref->definitionElement()->toString()<<"\n";
									cout<<"prev_ref was: "<<prev_ref->toString()<<"\n";
								#endif
								return false;
							}
						}
						else
						{
							#ifdef DEBUG_RAE_PARSER
								cout<<"Vector call HAROP7: no definitionElement in useVector.\n";
								cout<<"prev_ref was: "<<prev_ref->toString()<<"\n";
							#endif
							return false;
						}
						/*REMOVE
						if( classHasFunctionOrValue( prev_ref->templateSecondTypeString(), current_context_use->name() ) )
						{
							//It's ok.
							#ifdef DEBUG_RAE_PARSER
							cout<<"VECTOR reference thing: it's ok. it's valid. somevector[i].somefunc\n";
							#endif
							return true;
						}
						else
						{
							#ifdef DEBUG_RAE_PARSER
							cout<<"Vector call HAROP5: not ok. no such symbol in class.\n";
							#endif
							return false;//now we know that the class didn't have that symbol. or it isn't yet defined.
						}
						*/
					}
					else
					{
						if( classHasFunctionOrValue( prev_ref->type(), current_context_use->name() ) )
						{
							//It's ok.
							#ifdef DEBUG_RAE_PARSER
							cout<<"VECTOR reference thing: it's ok. it's valid. somevector[i].somefunc\n";
							#endif
							return true;
						}
						else
						{
							#ifdef DEBUG_RAE_PARSER
							cout<<"Vector call HAROP6: not ok. no such symbol in class.\n";
							#endif
							return false;//now we know that the class didn't have that symbol. or it isn't yet defined.
						}
					}		
				}
			}
			else if( current_context_use->previousElement() )
			{
				#ifdef DEBUG_RAE_PARSER
				if(current_context_use->name() == debug_string_con)
				{
					cout<<"Do we have REFERENCE_DOT:.\n";
					if(current_context_use->previousToken() == LangTokenType::REFERENCE_DOT)
						cout<<"YES a REFERENCE_DOT.\n";
					else cout<<"NO REFERENCE_DOT.\n";

					if(current_context_use->previous2ndToken() == LangTokenType::USE_REFERENCE)
						cout<<"YES a USE_REFERENCE before that.\n";
					else cout<<"NO USE_REFERENCE before that.\n";

					cout<<"previous: "<<current_context_use->previousElement()->toString()<<"\n";
					cout<<"previous 2nd: "<<current_context_use->previous2ndElement()->toString()<<"\n";
				}	
				#endif
			}
			else
			{
				reportError("checkIfTokenIsValidInCurrentContext() current_context_use doesn't have a previousElement. Compiler error.");
			}

			//In the same class or func, or global:
			if( current_context_use->parent() == found_definition->parent() )
			{
				#ifdef DEBUG_RAE_PARSER
				if(current_context_use->name() == debug_string_con)
				{
					cout<<"valid in SAME PARENT.\n";
				}
				#endif

				return true;
			}
			else if( current_context_use->parent() && found_definition->parent() )
			{
				//note to self: here we are most probably only dealing with USE_REFERENCEs, USE_MEMBERs and FUNC_CALLs.
				//all definitions we're most likely already checked.

				#ifdef DEBUG_RAE_PARSER
				if(current_context_use->name() == debug_string_con)
				{
					cout<<"getting desperate here. not in same parent. no reference dot type use and all that... but both have parents.\n";
					cout<<"current_context_use.parent is: "<<current_context_use->parent()->toString()<<"\n";
					cout<<"found_definition.parent is: "<<found_definition->parent()->toString()<<"\n";
				}
				#endif

				if( current_context_use->parentFunc() != 0 && found_definition->parentFunc() != 0 )//both are in funcs.
				{
					if( current_context_use->parentFunc() == found_definition->parentFunc() )//the same func.
					{
						//the same func. hmm, we check the position (which element was defined before?)
						//from the linenumber... I don't know if that really works. We'll see.
						if( found_definition->lineNumber().totalCharNumber < current_context_use->lineNumber().totalCharNumber )
						{
							#ifdef DEBUG_RAE_PARSER
							if(current_context_use->name() == debug_string_con)
							{
								cout<<"valid. same func. and definition is before use.\n";
							}
							#endif
							return true;
						}
						else
						{
							#ifdef DEBUG_RAE_PARSER
							if(current_context_use->name() == debug_string_con)
							{
								cout<<"NOT valid. same func, but definition is after use.\n";
							}
							#endif
							return false;
						}
					}
					else if( current_context_use->parentFunc() != found_definition->parentFunc() )//different funcs.
					{
						#ifdef DEBUG_RAE_PARSER
						if(current_context_use->name() == debug_string_con)
						{
							cout<<"NOT valid. different funcs.\n";
						}
						#endif
						return false;
					}
				}
				//else both of them are not in funcs.

				if( current_context_use->parentClass() == found_definition->parentClass() )
				{
					//They are in the same class
					if( current_context_use->parentFunc() != 0 && found_definition->parentFunc() == 0)
					{
						//use is in a func. definition is probably in the class body, because it is not in a func.

						//TODO here's where the code to check if something is hidden goes... so TODO an element must know it's own visibility.
						//so maybe we'll add the visibility as a param to LangElement, and only VISIBILITY_DEFAULT will be put on the list of
						//langelements...
						#ifdef DEBUG_RAE_PARSER
						if(current_context_use->name() == debug_string_con)
						{
							cout<<"valid. same class. definition in class (not in func) and use in func.\n";
						}
						#endif
						return true;					
					}
					else
					{
						#ifdef DEBUG_RAE_PARSER
						if(current_context_use->name() == debug_string_con)
						{
							cout<<"NOT valid because this case is Unhandled2.\n";
							cout<<"context_use: "<<current_context_use->toString()<<"\nfound_stuff: "<<found_definition->toString();
							if(current_context_use->parentFunc() )
							{
								cout<<"current_context_use->parentFunc: "<<current_context_use->parentFunc()->toString()<<"\n";
							}
							else cout<<"current_context_use has no parentFunc.";

							if( found_definition->parentFunc() )
							{
								cout<<"found_definition->parentFunc: "<<found_definition->parentFunc()->toString()<<"\n";
							}
							else cout<<"found_definition has no parentFunc.";
							
							cout<<"\nand their parents:\ncontext_use: "<<current_context_use->parent()->toString()<<"\nfound_stuff: "<<found_definition->parent()->toString();
							
						}
						//rae::log("checkIfValid. An unhandled case. Check this thing...\n");
						#endif
					}
				}
				else
				{
					#ifdef DEBUG_RAE_PARSER
					if(current_context_use->name() == debug_string_con)
					{
						cout<<"NOT valid because this case is Unhandled1.\n";
						cout<<"context_use: "<<current_context_use->toString()<<"\nfound_stuff: "<<found_definition->toString();
						cout<<"\nand their parents:\ncontext_use: "<<current_context_use->parent()->toString()<<"\nfound_stuff: "<<found_definition->parent()->toString();
					}
					//rae::log("checkIfValid. An unhandled case. Check this thing...\n");
					#endif
				}

				/*
				//They actually have parents.
				if( current_context_use->parent()->parent() == found_definition->parent()->parent() )
				{
					//in the same class, but perhaps in different funcs...
					if(current_context_use->name() == debug_string_con)
					{
						cout<<"NOT valid in SAME class but perhaps different funcs.\n";
					}
					return false;
				}
				//else if( current_context_use->parent()->parent() == found_definition->parent() )
				else if( current_context_use->parentClass() == found_definition->parentClass() )
				{
					//current_contexts is in a func, so parent->parent is class
					//found_definition is inside a class. the same class...
					if(current_context_use->name() == debug_string_con)
					{
						cout<<"valid in context in func. same class.\n";
					}
					return true;
				}
				else if( current_context_use->parent() == found_definition->parent()->parent() )
				{
					//current_contexts is in a class, so parent is class (it could be func, but it would never be equal, maybe.)
					//found_definition is inside a func which is in a class.
					//in this case, the context is wrong, because found_elem is definition,
					//and current_context is using it.
					if(current_context_use->name() == debug_string_con)
					{
						cout<<"NOT valid context is wrong.\n";
					}
					return false;
				}
				else
				{
					if(current_context_use->name() == debug_string_con)
					{
						cout<<"NOT valid because this case is Unhandled1.\n";
						cout<<"context_use: "<<current_context_use->toString()<<"\nfound_stuff: "<<found_definition->toString();
						cout<<"\nand their parents:\ncontext_use: "<<current_context_use->parent()->toString()<<"\nfound_stuff: "<<found_definition->parent()->toString();
					}
					//rae::log("checkIfValid. An unhandled case. Check this thing...\n");
				}
				*/
			}
			else if( found_definition->parent() == 0 )
			{
				#ifdef DEBUG_RAE_PARSER
				//maybe it's a global definition, because it doesn't have a parent?
				if(current_context_use->name() == debug_string_con)
				{
					cout<<"Umm. valid in maybe its a global thing..\n";
				}
				#endif
				return true;
			}
			else if( current_context_use->parent() == 0 )
			{
				#ifdef DEBUG_RAE_PARSER
				if(current_context_use->name() == debug_string_con)
				{
					cout<<"NOT valid in current use is global and def in class or func.\n";
				}
				#endif
				//the current use is global, but the definition is inside some class or func...
				return false;
				//TODO really, check for previousElement() if it has some kind of class usage
				//that makes the context ok...
			}
			else
			{
				#ifdef DEBUG_RAE_PARSER
				if(current_context_use->name() == debug_string_con)
				{
					cout<<"NOT valid because this case is Unhandled2.\n";
				}
				//rae::log("checkIfValid. An unhandled case. Check this thing...\n");
				#endif
			}
		}

		#ifdef DEBUG_RAE_PARSER
		if(current_context_use->name() == debug_string_con)
		{
			cout<<"valid END.\n";
		}
		#endif
		return false;


		//used to default to true... Hmm? is this a good default... What if we don't have current_context, so we're global...
		//That's a case for true... but if found_elem is null, then that's just an error...
	}

	void handleUserDefinedToken(string set_token)
	{
		////rae::log("Looking for: ", set_token, "\n");

		if( handleNumber(set_token) )
			return;

		#ifdef DEBUG_RAE_HUMAN
			cout<<"trying to handle: "<<set_token<<" in line: "<<lineNumber.line<<"\n";
			//rae::log("trying to handle: ", set_token, "\n");
		#endif

		//if we have some typetype that we're waiting, then just jump over these checks.
		if( expectingTypeType() != TypeType::UNDEFINED && previousElement() && previous2ndElement() )
		{
			//This is a fast check for unknown things before us.
			//like: unknown.set_token
			//so set_token should be unknown too.

			//use members: tester.useMember()
			//if(previousElement()->langTokenType() == LangTokenType::REFERENCE_DOT && previous2ndElement()->langTokenType() == LangTokenType::UNKNOWN_USE_REFERENCE)
			if(previousElement()->langTokenType() == LangTokenType::REFERENCE_DOT && previous2ndElement()->langTokenType() == LangTokenType::USE_REFERENCE && previous2ndElement()->isUnknownType() == true )
			{
				#ifdef DEBUG_RAE_HUMAN
					cout<<"because previousElement was unknown and there was a REFERENCE_DOT we set this to UNKNOWN_USE_MEMBER: "<<set_token<<"\n";
					//rae::log("because previousElement was unknown we set this to unknown too: ", set_token, "\n");
				#endif
				newUnknownUseMember(set_token);
				return;
			}
			//definitions: Tester tester <-- both are definitions... NO! We'll fix the first one and set the second token to be empty.
			//else if(previousElement()->langTokenType() == LangTokenType::UNKNOWN_USE_REFERENCE && previous2ndElement()->langTokenType() != LangTokenType::UNKNOWN_USE_REFERENCE)
			else if(previousElement()->langTokenType() == LangTokenType::USE_REFERENCE
				&& previousElement()->isUnknownType() == true
				//&& previous2ndElement()->langTokenType() != LangTokenType::USE_REFERENCE
				&& previous2ndElement()->isUnknownType() == false //The previous2nd element has to be something else. Something known.
				)
			{
				//So we found two unknown things after each other: Tester tester
				//and the later tester is set_token and the Tester is previousElement.
				//We also put a requirement of not having the element before these two to be Unknown... we'll see how that goes.

				#ifdef DEBUG_RAE_HUMAN
					cout<<"because previousElement was unknown we set this to UNKNOWN_DEFINITION: "<<set_token<<"\n";
					//rae::log("because previousElement was unknown we set this to unknown too: ", set_token, "\n");
				#endif
				//newUnknownUseMember(set_token);
				//Hahaaa, we don't even have to create it!
				//previousElement()->langTokenType(LangTokenType::UNKNOWN_DEFINITION);
				previousElement()->langTokenType(LangTokenType::DEFINE_REFERENCE);
				previousElement()->type( previousElement()->name() ); //Your name is now your type: Tester
				previousElement()->name(set_token); //your name is now set_token: tester

				//setNameAndCheckForPreviousDefinitions

				addToUnknownDefinitions(previousElement());
				
				#ifdef DEBUG_RAE_HUMAN
					cout<<"Fixed UNKNOWN_DEFINITION is now: "<<previousElement()->toString()<<"\n";
					//rae::log("because previousElement was unknown we set this to unknown too: ", set_token, "\n");
				#endif

				//!!!!
				return;
			}
			//an array definition:
			else if(previousElement()->langTokenType() == LangTokenType::BRACKET_END
				&& previous2ndElement()->langTokenType() == LangTokenType::BRACKET_BEGIN
				//&& previous2ndElement()->langTokenType() != LangTokenType::USE_REFERENCE
				&& previous2ndElement()->previousElement()->isUnknownType() == true
				)
			{
				LangElement* our_array_definition = previous2ndElement()->previousElement();

				#ifdef DEBUG_RAE_HUMAN
					cout<<"because previousElement was unknown we set this to DEFINE_ARRAY or in_class: "<<set_token<<"\n";
					//rae::log("because previousElement was unknown we set this to unknown too: ", set_token, "\n");
				#endif
				//newUnknownUseMember(set_token);
				//Hahaaa, we don't even have to create it!
				//previousElement()->langTokenType(LangTokenType::UNKNOWN_DEFINITION);
				
				//Ignore _IN_CLASS untill handleUnknownDefinitions.
				/*if( our_array_definition->parent()->langTokenType() == LangTokenType::CLASS )
				{
					our_array_definition->langTokenType(LangTokenType::DEFINE_ARRAY_IN_CLASS);
				}
				else
				{*/
					//our_array_definition->langTokenType(LangTokenType::DEFINE_ARRAY);
				
				cout<<"Got a bracket end. TODO something with it. VECTOR\n";
				assert(0);
				//////////////////our_array_definition->typeType(TypeType::C_ARRAY);
				
				//}
				
				//setNameAndCheckForPreviousDefinitions

				our_array_definition->type( our_array_definition->name() ); //Your name is now your type: Tester
				our_array_definition->name(set_token); //your name is now set_token: tester

				addToUnknownDefinitions(our_array_definition);
				
				#ifdef DEBUG_RAE_HUMAN
					cout<<"Fixed UNKNOWN_DEFINITION is now an array: "<<previousElement()->toString()<<"\n";
					//rae::log("because previousElement was unknown we set this to unknown too: ", set_token, "\n");
				#endif

				//!!!!
				return;
			}
		}

		LangElement* our_new_element;

		LangElement* found_elem = searchToken(set_token);
		if( found_elem )
		{
			#ifdef DEBUG_RAE_HUMAN
			cout<<"Found user token: "<<found_elem->toString()<<" "<<found_elem->namespaceString()<<"\n";
			//rae::log("Found user token: ", found_elem->toString(), "\n");
			#endif

			//Check if it is a valid token in this context: //check already in searchToken now...
			//if( checkIfTokenIsValidInCurrentContext(currentParentElement(), found_elem) == true )
			//{
				switch(found_elem->langTokenType())
				{
					default:
					break;
					case LangTokenType::CLASS:
						expectingToken(LangTokenType::DEFINE_REFERENCE_NAME);

						{
							//LangElement* clob = 

							if(expectingTypeType() == TypeType::UNDEFINED)
							{
								reportError("handleUserDefinedToken. expectingTypeType was UNDEFINED, when we found a class. Compiler error.");
							}
							
							our_new_element = newDefineReference(expectingTypeType(), found_elem, set_token);

							/*if( clob->parentLangTokenType() == LangTokenType::CLASS )
							{
								//we are inside a class definition, not a func:
								clob->
							}*/
						}
					break;
					case LangTokenType::FUNC:
					case LangTokenType::FUNC_CALL:
						our_new_element = newLangElement(LangTokenType::FUNC_CALL, TypeType::UNDEFINED, set_token);		
					break;
					/*case LangTokenType::DEFINE_VECTOR:
					case LangTokenType::DEFINE_VECTOR_IN_CLASS:
						our_new_element = newUseVector(found_elem);
					break;
					case LangTokenType::DEFINE_ARRAY:
					case LangTokenType::DEFINE_ARRAY_IN_CLASS:
						our_new_element = newUseArray(found_elem);
					break;
					*/
					case LangTokenType::DEFINE_FUNC_RETURN:
					case LangTokenType::DEFINE_FUNC_ARGUMENT:
					//case LangTokenType::DEFINE_REFERENCE_IN_CLASS:
					case LangTokenType::DEFINE_REFERENCE:
					//case LangTokenType::DEFINE_BUILT_IN_TYPE_IN_CLASS:
					//case LangTokenType::DEFINE_BUILT_IN_TYPE:

						//REFACTOR: OK. now all these newUseVectors could be replaced
						//with newUseReference() which would set also the typeType e.g. to ::VECTOR.
						
						our_new_element = newUseReference(found_elem);
						
						/*
						if(found_elem->typeType() == TypeType::REF)
						{
							our_new_element = newUseReference(found_elem);
						}
						else if(found_elem->typeType() == TypeType::VECTOR)
						{
							our_new_element = newUseVector(found_elem);
						}
						else if(found_elem->typeType() == TypeType::C_ARRAY)
						{
							our_new_element = newUseArray(found_elem);
						}
						else if(found_elem->typeType() == TypeType::BUILT_IN_TYPE)
						{
							our_new_element = newUseBuiltInType(found_elem);
						}*/

					break;/*
					case LangTokenType::USE_VECTOR:
						reportError("handleUserDefinedToken. Found USE_VECTOR. And we're not supposed to find those.");
					break;
					case LangTokenType::USE_ARRAY:
						reportError("handleUserDefinedToken. Found USE_ARRAY. And we're not supposed to find those.");
					break;
					*/
					case LangTokenType::USE_REFERENCE:
						reportError("handleUserDefinedToken. Found USE_REFERENCE. And we're not supposed to find those.");
					break;
				}
			/*}
			else //token is not valid in this context
			{
				//TODO this is not really an error yet. We just need some debugging cout here...
				//The error will come if it's still not found after unknownrefs are checked.
				reportError("Maybe not an error... Token is not valid in this context: " + LangTokenType::toString(set_token) );
				newUnknownUseReference(set_token);
			}*/
		}
		else
		{
			#ifdef DEBUG_RAE_HUMAN
			cout<<"Didn't find: "<<set_token<<" creating unknown ref.\n";
			//rae::log("Didn't find: ", set_token, " creating unknown ref.\n");
			#endif
			//specifically don't do our_new_element = , because this is already unkown and will be handled later...
			//Oh well. found_elem would be null anyway...
			newUnknownUseReference2(set_token);
		}

		//Hmm. We only check if it's valid here... It's kind of stupid not to be able to
		//check it already when searching for tokens...
/*
		if( our_new_element && found_elem )//because we have found_elem it can't be unknown already... (so we don't add it two times to the list.)
		{
			if( checkIfTokenIsValidInCurrentContext(our_new_element, found_elem) )
			{
				//It's ok.
			}
			else
			{
				//set it "back" to unknown... will be handled later again.
 				//our_new_element->langTokenType(LangTokenType::UNKNOWN_USE_REFERENCE);
 				our_new_element->isUnknownType(true);
 				addToUnknowns(our_new_element);
			}
		}
*/
		cleanUpUnknownUseReferences();
	}

	void handleUnknownTokens()
	{
		/*if( unknownDefinitions.empty() && unknownUseReferences.empty() && unknownUseMembers.empty() )
		{
			return;
		}*/

		//handleUnknownDefinitions();
		//handleUnknownUseReferences();
		//handleUnknownUseMembers();

		handleUnknownTokens( unknownDefinitions );
		handleCheckForPreviousDefinitionsList();
		handleUnknownTokens( unknownUseReferences );
		handleUnknownTokens( unknownUseMembers );
	}

	void handleCheckForPreviousDefinitionsList()
	{
		foreach(LangElement* lang_elem, checkForPreviousDefinitionsList)
		{
			//if( checkForPreviousDefinitions(lang_elem) == 0 )
			//string temp_name = lang_elem->name();
			//lang_elem->name("");
			//we take the name away so that it won't find itself.
			//we have to use searchToken version, because we want to find
			//name, name. not type, name, because that will just find the
			//class...
			//Should redesign the search tools.

			//ignore all of that. except should redesign... now using LangElement* version
			//with type swapped out...

			string temp_type = lang_elem->type();
			lang_elem->type("");

			//if( checkForPreviousDefinitions(temp_name) == false )
			if( checkForPreviousDefinitions(lang_elem) == 0 )
			{
				lang_elem->isUnknownType(false);
			}

			lang_elem->type(temp_type);
		}

		cleanUpUnknownTokens(checkForPreviousDefinitionsList);
	}

	void handleUnknownTokens(vector<LangElement*>& unknown_tokens )
	{
		if( unknown_tokens.empty() )
		{
			return;
		}

		LangElement* found_elem;

		#ifdef DEBUG_RAE_HUMAN
		cout<<"\n\n"<<moduleName()<<" handleUnknownDefinitions() START.\n";
		//rae::log("handleUnknownTokens() START.\n");
		#endif

		foreach( LangElement* lang_elem, unknown_tokens )
		{
			//if(lang_elem->langTokenType() != LangTokenType::UNKNOWN_DEFINITION)
			//if(lang_elem->langTokenType() != LangTokenType::DEFINE_REFERENCE && 
			if( lang_elem->isUnknownType() == false )
			{
				#ifdef DEBUG_RAE_HUMAN
				cout<<"\nNOT going to start looking for, because it is not anymore UNKNOWN_DEFINITION: "<<lang_elem->toString()<<"\n";
				#endif
				continue;
			}

			#ifdef DEBUG_RAE_HUMAN
			cout<<"\n\nSTART looking for unknown DEFINITION: "<<lang_elem->toString()<<"\n";
			#endif

			//found_elem = searchUserDefinedTokens( lang_elem->name() );
			found_elem = searchElementAndCheckIfValid( lang_elem );
			if( found_elem )
			{
				#ifdef DEBUG_RAE_HUMAN
				cout<<"Found unknown DEFINITION original class or type: "<<found_elem->toString()<<" "<<found_elem->namespaceString()<<"\n";
				cout<<"and the definition element is: "<<lang_elem->toString()<<"\n";
				//rae::log("Found unknown user token: ", found_elem->toString(), "\n");
				//rae::log("and the lang elem was: ", lang_elem->toString(), "\n");
				#endif

				switch(found_elem->langTokenType())
				{
					default:
						reportError("handleUnknownTokens found this: " + found_elem->toString(), lang_elem);
					break;
					case LangTokenType::CLASS:
						
						if(lang_elem->langTokenType() == LangTokenType::DEFINE_FUNC_ARGUMENT
							|| lang_elem->langTokenType() == LangTokenType::DEFINE_FUNC_RETURN
							)
						{
							lang_elem->isUnknownType(false);
							if(lang_elem->typeType() == TypeType::UNDEFINED)
							{
								lang_elem->typeType(TypeType::REF);//This is probably not needed, if we get func_arguments to be created
								//as refs as default. We should actually know their typetype when we create them! FIXIT. TODO.
							}
							addToUserDefinedTokens(lang_elem);
						}
						else if(lang_elem->langTokenType() == LangTokenType::DEFINE_REFERENCE )
						{	/*
							if(lang_elem->typeType() == TypeType::C_ARRAY )//|| lang_elem->langTokenType() == LangTokenType::DEFINE_ARRAY_IN_CLASS)
							{
								if(lang_elem->parent() && lang_elem->parent()->langTokenType() == LangTokenType::CLASS)
								{
									/////lang_elem->langTokenType(LangTokenType::DEFINE_ARRAY_IN_CLASS);
									lang_elem->parent()->createObjectAutoInitAndFree(lang_elem);
									//listOfAutoInitObjects.push_back(lang_elem);	
								}
								else
								{
									////lang_elem->langTokenType(LangTokenType::DEFINE_ARRAY);	
								}
								lang_elem->isUnknownType(false);
								addToUserDefinedTokens(lang_elem);
							}
							else //umm, ref, built_in, vector? why is this separate from array?
							{
							*/

								//mark that we found the definition for future use:
								if( lang_elem->definitionElement() )
								{
									reportError("Found a definition for the second time. Compiler bug. earlier definition: " + lang_elem->definitionElement()->toString() + " later definition: " + found_elem->toString(), lang_elem );
								}
								else
								{
									lang_elem->definitionElement( found_elem );
									#ifdef DEBUG_RAE_HUMAN
										cout<<"CLASSWAY marked a definitionElement: "<<lang_elem->definitionElement()->toString()<<" while looking for "<<lang_elem->toString()<<"\n";
									#endif
								}

								//if we are inside a class, we need to create auto_inits and auto_free to constructors and destructors.
								if(lang_elem->parent() && lang_elem->parent()->langTokenType() == LangTokenType::CLASS)
								{
									////////////////////lang_elem->langTokenType(LangTokenType::DEFINE_REFERENCE_IN_CLASS);
									lang_elem->parent()->createObjectAutoInitAndFree(lang_elem);
									//listOfAutoInitObjects.push_back(lang_elem);	
								}
								else
								{
									//lang_elem->langTokenType(LangTokenType::DEFINE_REFERENCE);	
								}
								lang_elem->isUnknownType(false);
								addToUserDefinedTokens(lang_elem);
							////}
						}
					break;
					case LangTokenType::FUNC:
					case LangTokenType::FUNC_CALL:
						//newLangElement(LangTokenType::FUNC_CALL, set_token);

						#ifdef DEBUG_RAE_HUMAN
						cout<<"\n\n\n\n\n\nDEFINITION FUNC_CALL Found. I bet this can be removed, because it shouldn't happen!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n";
						#endif

						lang_elem->langTokenType(LangTokenType::FUNC_CALL);
						lang_elem->isUnknownType(false);
					break;
					/*	
					case LangTokenType::DEFINE_VECTOR:
					case LangTokenType::DEFINE_VECTOR_IN_CLASS:
						#ifdef DEBUG_RAE_HUMAN
						cout<<"\n\n\n\n\n\nDEFINITION DEFINE_VECTOR Found. I bet this can be removed, because it shouldn't happen!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n";
						#endif

						//newUseVector(found_elem);
						lang_elem->langTokenType(LangTokenType::USE_VECTOR);
						lang_elem->isUnknownType(false);
					break;
					case LangTokenType::DEFINE_ARRAY:
					case LangTokenType::DEFINE_ARRAY_IN_CLASS:
						#ifdef DEBUG_RAE_HUMAN
						cout<<"\n\n\n\n\n\nDEFINITION DEFINE_ARRAY Found. I bet this can be removed, because it shouldn't happen!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n";
						#endif

						//newUseArray(found_elem);
						lang_elem->langTokenType(LangTokenType::USE_ARRAY);
						lang_elem->isUnknownType(false);
					break;
					*/
					case LangTokenType::DEFINE_FUNC_RETURN:
					case LangTokenType::DEFINE_FUNC_ARGUMENT:
					//case LangTokenType::DEFINE_REFERENCE_IN_CLASS:
					case LangTokenType::DEFINE_REFERENCE:
					//case LangTokenType::DEFINE_BUILT_IN_TYPE_IN_CLASS:
					//case LangTokenType::DEFINE_BUILT_IN_TYPE:
						//#ifdef DEBUG_RAE_HUMAN
						//cout<<"\n\n\n\n\n\nDEFINITION OTHER STUFF THAT IS NOT NEEDED Found. I bet this can be removed, because it shouldn't happen!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n";
						//#endif

						//newUseReference(found_elem);
					//We don't create a nu element, instead we modify the existing unknown element.
						lang_elem->langTokenType(LangTokenType::USE_REFERENCE);
						lang_elem->type( found_elem->type() );//copy the class type etc.
						lang_elem->typeType( found_elem->typeType() );//copy the typetype. ref, built_in, array, vector etc.
						lang_elem->isUnknownType(false);

						//mark that we found the definition for future use:
						if( lang_elem->definitionElement() )
						{
							reportError("Found a definition for the second time. Compiler bug. earlier definition: " + lang_elem->definitionElement()->toString() + " later definition: " + found_elem->toString(), lang_elem );
						}
						else
						{
							lang_elem->definitionElement( found_elem );
							#ifdef DEBUG_RAE_HUMAN
								cout<<"NEWWAY marked a definitionElement: "<<lang_elem->definitionElement()->toString()<<" while looking for "<<lang_elem->toString()<<"\n";
							#endif
						}

						//And here's the optimization I was talking about: check for following FUNC_CALL and USE_REFERENCE.
						//If we already know the class we are dealing with, which is found in lang_elem->definitionElement()->definitionElement()
						if(lang_elem->nextElement() && lang_elem->nextElement()->langTokenType() == LangTokenType::REFERENCE_DOT
							&& lang_elem->definitionElement()->definitionElement() )
						{
							#ifdef DEBUG_RAE_HUMAN
							cout<<"Yes a REFERENCE_DOT!!!!!\n";
							#endif
							//this is our reference dot, and the element after it is unknown.
							if(lang_elem->nextElement()->nextElement() && lang_elem->nextElement()->nextElement()->isUnknownType() == true )
							{
								#ifdef DEBUG_RAE_HUMAN
								cout<<"Yes another nextElement and it is unknown!!!!!\n";
								#endif
								LangElement* maybe_member = lang_elem->definitionElement()->definitionElement()->hasFunctionOrMember(lang_elem->nextElement()->nextElement()->name() );
								
								#ifdef DEBUG_RAE_HUMAN
								cout<<"looking for name: "<<lang_elem->nextElement()->nextElement()->name()<<"\n";
								cout<<"from: "<<lang_elem->definitionElement()->definitionElement()->toString()<<"\n";
								#endif

								lang_elem->definitionElement()->definitionElement()->printOutListOfFunctions();
								lang_elem->definitionElement()->definitionElement()->printOutListOfReferences();

								if( maybe_member )
								{
									if( maybe_member->langTokenType() == LangTokenType::FUNC )
									{
										#ifdef DEBUG_RAE_HUMAN
										cout<<"NEWWAY found a FUNC definition "<<maybe_member->toString()<<" while looking for "<<lang_elem->nextElement()->nextElement()->toString()<<"\n";
										#endif
										lang_elem->nextElement()->nextElement()->langTokenType(LangTokenType::FUNC_CALL);
									}
									else if( maybe_member->langTokenType() == LangTokenType::DEFINE_REFERENCE )
									{
										#ifdef DEBUG_RAE_HUMAN
										cout<<"NEWWAY found a DEFINE_REFERENCE "<<maybe_member->toString()<<" while looking for "<<lang_elem->nextElement()->nextElement()->toString()<<"\n";
										#endif
										lang_elem->nextElement()->nextElement()->langTokenType(LangTokenType::USE_REFERENCE);
										lang_elem->nextElement()->nextElement()->typeType( maybe_member->typeType() );//Umm, could it be a vector?
									}
									else
									{
										reportError("Found a member func or reference, but it wasn't either. Compiler is confused. maybe_member: " + maybe_member->toString(), lang_elem );
									}

									lang_elem->nextElement()->nextElement()->isUnknownType(false);
									lang_elem->nextElement()->nextElement()->definitionElement(maybe_member);//And this is important. The .funcCall points to func definition now.
									//and use reference to the define reference.
								}
								#ifdef DEBUG_RAE_HUMAN
								else
								{
									cout<<"No maybe_member in there!\n";
								}
								#endif
							}	
						}
						#ifdef DEBUG_RAE_HUMAN
						else //if(lang_elem->name() == "tester")
						{
							if( lang_elem->nextElement() == 0 )
								cout<<"No nextElement!!!!!\n";
							else
								cout<<"The next element was: "<<lang_elem->nextElement()->toString()<<"\n";
						}
						#endif
					break;
					/*case LangTokenType::USE_VECTOR:
						reportError("handleUnknownDefinitions. Found USE_VECTOR. And we're not supposed to find those.");
					break;
					case LangTokenType::USE_ARRAY:
						reportError("handleUnknownDefinitions. Found USE_ARRAY. And we're not supposed to find those.");
					break;
					*/
					case LangTokenType::USE_REFERENCE:
						reportError("handleUnknownDefinitions. Found USE_REFERENCE. And we're not supposed to find those.");
					break;
				}

				#ifdef DEBUG_RAE_HUMAN
				cout<<"fixed lang elem is: "<<lang_elem->toString()<<"\n";
				//rae::log("fixed lang elem is: ", lang_elem->toString(), "\n");
				#endif
			}
			else
			{
				reportError("Didn't find definition: " + lang_elem->toString() );

				#ifdef DEBUG_RAE_HUMAN
				cout<<"Didn't find: "<<lang_elem->toString()<<" it remains unknown DEFINITION.\n";
				#endif
				//rae::log("Didn't find: ", lang_elem->toString(), " it remains unknown ref.\n");
				//newUnknownUseReference(set_token);
			}	
		}

		cleanUpUnknownTokens(unknown_tokens);
	}

/*
	void handleUnknownDefinitions()
	{
		if( unknownDefinitions.empty() )
		{
			return;
		}

		LangElement* found_elem;

		#ifdef DEBUG_RAE_HUMAN
		cout<<"\n\n"<<moduleName()<<" handleUnknownDefinitions() START.\n";
		//rae::log("handleUnknownTokens() START.\n");
		#endif

		foreach( LangElement* lang_elem, unknownDefinitions )
		{
			//if(lang_elem->langTokenType() != LangTokenType::UNKNOWN_DEFINITION)
			//if(lang_elem->langTokenType() != LangTokenType::DEFINE_REFERENCE && 
			if( lang_elem->isUnknownType() == false )
			{
				#ifdef DEBUG_RAE_HUMAN
				cout<<"NOT going to start looking for, because it is not anymore UNKNOWN_DEFINITION: "<<lang_elem->toString()<<"\n";
				#endif
				continue;
			}

			#ifdef DEBUG_RAE_HUMAN
			cout<<"START looking for unknown DEFINITION: "<<lang_elem->toString()<<"\n";
			#endif

			//found_elem = searchUserDefinedTokens( lang_elem->name() );
			found_elem = searchElementAndCheckIfValid( lang_elem );
			if( found_elem )
			{
				#ifdef DEBUG_RAE_HUMAN
				cout<<"Found unknown DEFINITION original class or type: "<<found_elem->toString()<<" "<<found_elem->namespaceString()<<"\n";
				cout<<"and the definition element is: "<<lang_elem->toString()<<"\n";
				//rae::log("Found unknown user token: ", found_elem->toString(), "\n");
				//rae::log("and the lang elem was: ", lang_elem->toString(), "\n");
				#endif

				switch(found_elem->langTokenType())
				{
					default:
					break;
					case LangTokenType::CLASS:
						/////////expectingToken = LangTokenType::DEFINE_REFERENCE_NAME;
						////////{
							//LangElement* clob = 
						///////	newDefineReference(set_token);

							//if( clob->parentLangTokenType() == LangTokenType::CLASS )
							//{
							//	//we are inside a class definition, not a func:
							//	clob->
							//}
						/////////}

						if(lang_elem->langTokenType() == LangTokenType::DEFINE_ARRAY )//|| lang_elem->langTokenType() == LangTokenType::DEFINE_ARRAY_IN_CLASS)
						{
							if(lang_elem->parent() && lang_elem->parent()->langTokenType() == LangTokenType::CLASS)
							{
								lang_elem->langTokenType(LangTokenType::DEFINE_ARRAY_IN_CLASS);
								lang_elem->parent()->createObjectAutoInitAndFree(lang_elem);
								//listOfAutoInitObjects.push_back(lang_elem);	
							}
							else
							{
								lang_elem->langTokenType(LangTokenType::DEFINE_ARRAY);	
							}
							lang_elem->isUnknownType(false);
							addToUserDefinedTokens(lang_elem);
						}
						else
						{
							if(lang_elem->parent() && lang_elem->parent()->langTokenType() == LangTokenType::CLASS)
							{
								lang_elem->langTokenType(LangTokenType::DEFINE_REFERENCE_IN_CLASS);
								lang_elem->parent()->createObjectAutoInitAndFree(lang_elem);
								//listOfAutoInitObjects.push_back(lang_elem);	
							}
							else
							{
								lang_elem->langTokenType(LangTokenType::DEFINE_REFERENCE);	
							}
							lang_elem->isUnknownType(false);
							addToUserDefinedTokens(lang_elem);
						}
					break;
					case LangTokenType::FUNC:
					case LangTokenType::FUNC_CALL:
						//newLangElement(LangTokenType::FUNC_CALL, set_token);

						#ifdef DEBUG_RAE_HUMAN
						cout<<"\n\n\n\n\n\nDEFINITION FUNC_CALL Found. I bet this can be removed, because it shouldn't happen!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n";
						#endif

						lang_elem->langTokenType(LangTokenType::FUNC_CALL);
						lang_elem->isUnknownType(false);
					break;
					case LangTokenType::DEFINE_VECTOR:
					case LangTokenType::DEFINE_VECTOR_IN_CLASS:
						#ifdef DEBUG_RAE_HUMAN
						cout<<"\n\n\n\n\n\nDEFINITION DEFINE_VECTOR Found. I bet this can be removed, because it shouldn't happen!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n";
						#endif

						//newUseVector(found_elem);
						lang_elem->langTokenType(LangTokenType::USE_VECTOR);
						lang_elem->isUnknownType(false);
					break;
					case LangTokenType::DEFINE_ARRAY:
					case LangTokenType::DEFINE_ARRAY_IN_CLASS:
						#ifdef DEBUG_RAE_HUMAN
						cout<<"\n\n\n\n\n\nDEFINITION DEFINE_ARRAY Found. I bet this can be removed, because it shouldn't happen!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n";
						#endif

						//newUseArray(found_elem);
						lang_elem->langTokenType(LangTokenType::USE_ARRAY);
						lang_elem->isUnknownType(false);
					break;
					case LangTokenType::DEFINE_FUNC_RETURN:
					case LangTokenType::DEFINE_FUNC_ARGUMENT:
					case LangTokenType::DEFINE_REFERENCE_IN_CLASS:
					case LangTokenType::DEFINE_REFERENCE:
					case LangTokenType::DEFINE_BUILT_IN_TYPE_IN_CLASS:
					case LangTokenType::DEFINE_BUILT_IN_TYPE:
						#ifdef DEBUG_RAE_HUMAN
						cout<<"\n\n\n\n\n\nDEFINITION OTHER STUFF THAT IS NOT NEEDED Found. I bet this can be removed, because it shouldn't happen!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n";
						#endif

						//newUseReference(found_elem);
					//We don't create a nu element, instead we modify the existing unknown element.
						lang_elem->langTokenType(LangTokenType::USE_REFERENCE);
						lang_elem->type( found_elem->type() );//copy the class type etc.
						lang_elem->isUnknownType(false);
					break;
					case LangTokenType::USE_VECTOR:
						reportError("handleUnknownDefinitions. Found USE_VECTOR. And we're not supposed to find those.");
					break;
					case LangTokenType::USE_ARRAY:
						reportError("handleUnknownDefinitions. Found USE_ARRAY. And we're not supposed to find those.");
					break;
					case LangTokenType::USE_REFERENCE:
						reportError("handleUnknownDefinitions. Found USE_REFERENCE. And we're not supposed to find those.");
					break;
				}

				#ifdef DEBUG_RAE_HUMAN
				cout<<"fixed lang elem is: "<<lang_elem->toString()<<"\n";
				//rae::log("fixed lang elem is: ", lang_elem->toString(), "\n");
				#endif
			}
			else
			{
				reportError("Didn't find definition: " + lang_elem->toString() );

				#ifdef DEBUG_RAE_HUMAN
				cout<<"Didn't find: "<<lang_elem->toString()<<" it remains unknown DEFINITION.\n";
				#endif
				//rae::log("Didn't find: ", lang_elem->toString(), " it remains unknown ref.\n");
				//newUnknownUseReference(set_token);
			}	
		}

		cleanUpUnknownDefinitions();
	}


	void handleUnknownUseReferences()
	{
		if( unknownUseReferences.empty() )
		{
			return;
		}

		LangElement* found_elem;
	
		#ifdef DEBUG_RAE_HUMAN
		cout<<"\n\n"<<moduleName()<<" handleUnknownUseReferences() START.\n";
		//rae::log("handleUnknownUseReferences() START.\n");
		#endif

		foreach( LangElement* lang_elem, unknownUseReferences )
		{
			//if(lang_elem->langTokenType() != LangTokenType::UNKNOWN_USE_REFERENCE)
			if( lang_elem->isUnknownType() == false )
			{
				#ifdef DEBUG_RAE_HUMAN
				cout<<"NOT going to start looking for, because it is not anymore UNKNOWN_USE_REFERENCE: "<<lang_elem->toString()<<"\n";
				#endif
				continue;
			}

			#ifdef DEBUG_RAE_HUMAN
			cout<<"START looking for unknown USE_REFERENCE: "<<lang_elem->toString()<<"\n";
			#endif

			//found_elem = searchUserDefinedTokens( lang_elem->name() );
			found_elem = searchElementAndCheckIfValid( lang_elem );
			if( found_elem )
			{
				#ifdef DEBUG_RAE_HUMAN
				cout<<"Found unknown USE_REFERENCE original definition: "<<found_elem->toString()<<" "<<found_elem->namespaceString()<<"\n";
				cout<<"and the USE_REFERENCE element is: "<<lang_elem->toString()<<"\n";
				//rae::log("Found unknown user token: ", found_elem->toString(), "\n");
				//rae::log("and the lang elem was: ", lang_elem->toString(), "\n");
				#endif

				switch(found_elem->langTokenType())
				{
					default:
					break;
					case LangTokenType::CLASS:
						#ifdef DEBUG_RAE_HUMAN
						cout<<"\n\n\n\n\n\nUSE_REFERENCE CLASS Found. I bet this can be removed, because it shouldn't happen!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n";
						#endif

						/////////expectingToken = LangTokenType::DEFINE_REFERENCE_NAME;
						////////{
							//LangElement* clob = 
						///////	newDefineReference(set_token);

							//if( clob->parentLangTokenType() == LangTokenType::CLASS )
							//{
							//	//we are inside a class definition, not a func:
							//	clob->
							//}
						/////////}
								lang_elem->langTokenType(LangTokenType::DEFINE_REFERENCE);
								lang_elem->isUnknownType(false);
					break;
					case LangTokenType::FUNC:
					case LangTokenType::FUNC_CALL:
						//newLangElement(LangTokenType::FUNC_CALL, set_token);
						#ifdef DEBUG_RAE_HUMAN
						cout<<"\n\n\n\n\n\nUSE_REFERENCE FUNC_CALL Found. I bet this can be removed, because it shouldn't happen!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n";
						#endif

						lang_elem->langTokenType(LangTokenType::FUNC_CALL);
						lang_elem->isUnknownType(false);
					break;
					case LangTokenType::DEFINE_VECTOR:
					case LangTokenType::DEFINE_VECTOR_IN_CLASS:
						//newUseVector(found_elem);
						lang_elem->langTokenType(LangTokenType::USE_VECTOR);
						lang_elem->isUnknownType(false);
					break;
					case LangTokenType::DEFINE_ARRAY:
					case LangTokenType::DEFINE_ARRAY_IN_CLASS:
						//newUseArray(found_elem);
						lang_elem->langTokenType(LangTokenType::USE_ARRAY);
						lang_elem->isUnknownType(false);
					break;
					case LangTokenType::DEFINE_FUNC_RETURN:
					case LangTokenType::DEFINE_FUNC_ARGUMENT:
					case LangTokenType::DEFINE_REFERENCE_IN_CLASS:
					case LangTokenType::DEFINE_REFERENCE:
					case LangTokenType::DEFINE_BUILT_IN_TYPE_IN_CLASS:
					case LangTokenType::DEFINE_BUILT_IN_TYPE:
						//newUseReference(found_elem);
					//We don't create a nu element, instead we modify the existing unknown element.
						lang_elem->langTokenType(LangTokenType::USE_REFERENCE);
						lang_elem->type( found_elem->type() );//copy the class type etc.
						lang_elem->isUnknownType(false);
					break;
					case LangTokenType::USE_VECTOR:
						reportError("handleUnknownUseReferences. Found USE_VECTOR. And we're not supposed to find those.");
					break;
					case LangTokenType::USE_ARRAY:
						reportError("handleUnknownUseReferences. Found USE_ARRAY. And we're not supposed to find those.");
					break;
					case LangTokenType::USE_REFERENCE:
						reportError("handleUnknownUseReferences. Found USE_REFERENCE. And we're not supposed to find those.");
					break;
				}

				#ifdef DEBUG_RAE_HUMAN
				cout<<"fixed lang elem is: "<<lang_elem->toString()<<"\n";
				//rae::log("fixed lang elem is: ", lang_elem->toString(), "\n");
				#endif
			}
			else
			{
				reportError("Didn't find reference: " + lang_elem->toString() );

				#ifdef DEBUG_RAE_HUMAN
				cout<<"Didn't find: "<<lang_elem->toString()<<" it remains unknown USE_REFERENCE.\n";
				//rae::log("Didn't find: ", lang_elem->toString(), " it remains unknown ref.\n");
				#endif
				//newUnknownUseReference(set_token);
			}	
		}

		cleanUpUnknownUseReferences();
	}

	void handleUnknownUseMembers()
	{
		if( unknownUseMembers.empty() )
		{
			return;
		}

		LangElement* found_elem;

		#ifdef DEBUG_RAE_HUMAN
		cout<<"\n\n"<<moduleName()<<" handleUnknownUseMembers() START.\n";
		//rae::log("handleUnknownUseMembers() START.\n");
		#endif

		foreach( LangElement* lang_elem, unknownUseMembers )
		{
			//if(lang_elem->langTokenType() != LangTokenType::UNKNOWN_USE_MEMBER)
			if( lang_elem->isUnknownType() == false )
			{
				#ifdef DEBUG_RAE_HUMAN
				cout<<"NOT going to start looking for, because it is not anymore UNKNOWN_USE_MEMBER: "<<lang_elem->toString()<<"\n";
				#endif
				continue;
			}

			#ifdef DEBUG_RAE_HUMAN
			cout<<"START looking for unknown USE_MEMBER: "<<lang_elem->toString()<<"\n";
			#endif

			//found_elem = searchUserDefinedTokens( lang_elem->name() );
			found_elem = searchElementAndCheckIfValid( lang_elem );
			if( found_elem )
			{
				#ifdef DEBUG_RAE_HUMAN
				cout<<"Found unknown USE_MEMBER original function or member: "<<found_elem->toString()<<" "<<found_elem->namespaceString()<<"\n";
				cout<<"and the USE_MEMBER element is: "<<lang_elem->toString()<<"\n";
				//rae::log("Found unknown user token: ", found_elem->toString(), "\n");
				//rae::log("and the lang elem was: ", lang_elem->toString(), "\n");
				#endif

				switch(found_elem->langTokenType())
				{
					default:
					break;
					case LangTokenType::CLASS:
						#ifdef DEBUG_RAE_HUMAN
						cout<<"\n\n\n\n\nUSE_MEMBER CLASS Found. I bet this can be removed, because it shouldn't happen!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n";
						#endif

						/////////expectingToken = LangTokenType::DEFINE_REFERENCE_NAME;
						////////{
							//LangElement* clob = 
						///////	newDefineReference(set_token);

							//if( clob->parentLangTokenType() == LangTokenType::CLASS )
							//{
							//	//we are inside a class definition, not a func:
							//	clob->
							//}
						/////////}
								lang_elem->langTokenType(LangTokenType::DEFINE_REFERENCE);
								lang_elem->isUnknownType(false);
					break;
					case LangTokenType::FUNC:
					case LangTokenType::FUNC_CALL:
						//newLangElement(LangTokenType::FUNC_CALL, set_token);
						lang_elem->langTokenType(LangTokenType::FUNC_CALL);
						lang_elem->isUnknownType(false);
					break;
					case LangTokenType::DEFINE_VECTOR:
					case LangTokenType::DEFINE_VECTOR_IN_CLASS:
						//newUseVector(found_elem);
						#ifdef DEBUG_RAE_HUMAN
						cout<<"\n\n\n\n\nUSE_MEMBER DEFINE_VECTOR Found. I bet this can be removed, because it shouldn't happen!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n";
						#endif

						lang_elem->langTokenType(LangTokenType::USE_VECTOR);
						lang_elem->isUnknownType(false);
					break;
					case LangTokenType::DEFINE_ARRAY:
					case LangTokenType::DEFINE_ARRAY_IN_CLASS:
						#ifdef DEBUG_RAE_HUMAN
						cout<<"\n\n\n\n\nUSE_MEMBER DEFINE_ARRAY Found. I bet this can be removed, because it shouldn't happen!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n";
						#endif
						//newUseArray(found_elem);
						lang_elem->langTokenType(LangTokenType::USE_ARRAY);
						lang_elem->isUnknownType(false);
					break;
					case LangTokenType::DEFINE_FUNC_RETURN:
					case LangTokenType::DEFINE_FUNC_ARGUMENT:
						#ifdef DEBUG_RAE_HUMAN
						cout<<"\n\n\n\n\nUSE_MEMBER FUNC_PARAM or DEFINE_FUNC_RETURN Found. I bet this can be removed, because it shouldn't happen!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n";
						#endif
					break;
					case LangTokenType::DEFINE_REFERENCE_IN_CLASS:
					case LangTokenType::DEFINE_REFERENCE:
					case LangTokenType::DEFINE_BUILT_IN_TYPE_IN_CLASS:
					case LangTokenType::DEFINE_BUILT_IN_TYPE:
						//#ifdef DEBUG_RAE_HUMAN
						//cout<<"\n\n\n\n\nUSE_MEMBER OTHER STUFF that we don't need Found. I bet this can be removed, because it shouldn't happen!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n";
						//#endif

						//newUseReference(found_elem);
					//We don't create a nu element, instead we modify the existing unknown element.
						lang_elem->langTokenType(LangTokenType::USE_REFERENCE);//TODO do we need USE_MEMBER or USE_BUILT_IN_TYPE???
						lang_elem->type( found_elem->type() );//copy the class type etc.
						lang_elem->isUnknownType(false);
					break;
					case LangTokenType::USE_VECTOR:
						reportError("handleUnknownUseMembers. Found USE_VECTOR. And we're not supposed to find those.");
					break;
					case LangTokenType::USE_ARRAY:
						reportError("handleUnknownUseMembers. Found USE_ARRAY. And we're not supposed to find those.");
					break;
					case LangTokenType::USE_REFERENCE:
						reportError("handleUnknownUseMembers. Found USE_REFERENCE. And we're not supposed to find those.");
					break;
				}

				#ifdef DEBUG_RAE_HUMAN
				cout<<"fixed lang elem is: "<<lang_elem->toString()<<"\n";
				//rae::log("fixed lang elem is: ", lang_elem->toString(), "\n");
				#endif
			}
			else
			{
				reportError("Didn't find member: " + lang_elem->toString() );

				#ifdef DEBUG_RAE_HUMAN
				cout<<"Didn't find: "<<lang_elem->toString()<<" it remains unknown USE_REFERENCE.\n";
				//rae::log("Didn't find: ", lang_elem->toString(), " it remains unknown ref.\n");
				#endif
				//newUnknownUseReference(set_token);
			}	
		}

		cleanUpUnknownUseMembers();
	}
*/





  /*bool checkToken(string check_string)
	{
		//FILE must exist. and it must be opened.
		//int currentChar;
		foreach( char cr, check_string )
		{
			//if( EOF == (currentChar = getc(currentFile)) )
			if( readChar() == false )
				return false;
			if( currentChar != cr )//wrong char found.
				return false;
		}
		return true;
        }*/
	
	string getUntilEndOfLine()
	{
		//FILE must exist. and it must be opened.
		
		string res;
		
		readChar();//read the first char here.
		
		while( currentChar != '\n' )
		{
			res.push_back( char(currentChar) );
			readChar();
		}
		return res;
	}


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
				//writer.nextToken( langElements[i+1]->langTokenType() );
				writer.nextElement( set_elem.langElements[i+1] );
			}
			//set_elem.langElements[i]->write(writer);
			writeElement( writer, *set_elem.langElements[i]);
		}
	}

	void writeElement( StringFileWriter& writer, LangElement& set_elem )
	{
		//int count_elem = 0;

		switch(set_elem.langTokenType())
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
							//writer.nextToken( langElements[i+1]->langTokenType() );
							writer.nextElement( set_elem.langElements[i+1] );
						}
						langElements[i]->write(writer);
					}
					*/
					iterateWrite(writer, set_elem);
				}
			break;
			case LangTokenType::UNDEFINED:
			case LangTokenType::EMPTY: //more intentional than undefined... I guess they are the same thing...
				//we don't write undefined. it's like empty...
				iterateWrite(writer, set_elem);
			break;
			case LangTokenType::INFO_FUNC_PARAM:
				//don't write anything... Oh... let's write some info. Maybe we should have an option for this:
				
				//writer.writeString("/*");
				//writer.writeString(m_name);
				//writer.writeString(":*/");

			break;
			case LangTokenType::NEWLINE:

			{
				if( writer.lineNeedsSemicolon() == true
					&& writer.previousToken() != LangTokenType::SEMICOLON
					&& writer.previousToken() != LangTokenType::NEWLINE
					&& writer.previousToken() != LangTokenType::NEWLINE_BEFORE_SCOPE_END
					&& writer.previousToken() != LangTokenType::CONSTRUCTOR
					&& writer.previousToken() != LangTokenType::DESTRUCTOR
					&& writer.previousToken() != LangTokenType::FUNC
					&& writer.previousToken() != LangTokenType::MAIN
					/*&& writer.previousToken() != LangTokenType::SCOPE_BEGIN
					&& writer.previousToken() != LangTokenType::SCOPE_END
					&& writer.previousToken() != LangTokenType::FUNC
					&& writer.previousToken() != LangTokenType::DEFINE_FUNC_ARGUMENT
					&& writer.previousToken() != LangTokenType::DEFINE_FUNC_RETURN //not actually needed...
					&& writer.previousToken() != LangTokenType::CLASS
					&& writer.previousToken() != LangTokenType::COMMENT
					//&& writer.previousToken() != LangTokenType::STAR_COMMENT
					&& writer.previousToken() != LangTokenType::VISIBILITY_DEFAULT*/
				)
				{
					/*if( parentLangTokenType() == LangTokenType::CLASS || parentLangTokenType() == LangTokenType::ENUM )
					{
						//rae::log("NEWLINESTRANGE. This thing has parent CLASS and not func.", "\n");
						//rae::log("previousToken: ", LangTokenType::toString(writer.previousToken());
						//rae::log(" nextToken: ", LangTokenType::toString(writer.nextToken()), "\n");
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
					if( writer.previousToken() == LangTokenType::PARENTHESIS_END_FUNC_PARAM_TYPES )
					{
						so_do_we_write_newline = false;
					}
					else if( writer.previous2ndToken() == LangTokenType::FUNC
						|| writer.previous2ndToken() == LangTokenType::CONSTRUCTOR
						|| writer.previous2ndToken() == LangTokenType::DESTRUCTOR
					)
					{
						//hpp after func.
						//no newline after func in hpp.
						//makes it prettier.
						//no newline here.
						so_do_we_write_newline = false;
					}
					//if( writer.previous3rdToken() == LangTokenType::PARENTHESIS_END_FUNC_PARAM_TYPES )
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

					//if( writer.nextToken() == LangTokenType::VISIBILITY_DEFAULT )
					if( writer.nextToken() == LangTokenType::VISIBILITY_DEFAULT )
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
			case LangTokenType::NEWLINE_BEFORE_SCOPE_END:

				if( writer.lineNeedsSemicolon() == true
					&& writer.previousToken() != LangTokenType::SEMICOLON
					&& writer.previousToken() != LangTokenType::NEWLINE
					&& writer.previousToken() != LangTokenType::NEWLINE_BEFORE_SCOPE_END
					&& writer.previousToken() != LangTokenType::CONSTRUCTOR
					&& writer.previousToken() != LangTokenType::DESTRUCTOR
					&& writer.previousToken() != LangTokenType::FUNC
					&& writer.previousToken() != LangTokenType::MAIN
					/*&& writer.previousToken() != LangTokenType::SCOPE_BEGIN
					&& writer.previousToken() != LangTokenType::SCOPE_END
					&& writer.previousToken() != LangTokenType::FUNC
					&& writer.previousToken() != LangTokenType::DEFINE_FUNC_ARGUMENT
					&& writer.previousToken() != LangTokenType::DEFINE_FUNC_RETURN //not actually needed...
					&& writer.previousToken() != LangTokenType::CLASS
					&& writer.previousToken() != LangTokenType::COMMENT
					//&& writer.previousToken() != LangTokenType::STAR_COMMENT
					&& writer.previousToken() != LangTokenType::VISIBILITY_DEFAULT
					*/
				)
				{
					writer.writeChar(';');
				}
				writer.lineNeedsSemicolon(true);

				writer.writeChar( '\n' );
				//we need one less indent on SCOPE_END.
				writer.currentIndentMinus();
				
				if( writer.nextToken() == LangTokenType::VISIBILITY_DEFAULT )
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
			case LangTokenType::OVERRIDE:
				writer.writeString("/*override:*/");
			break;
				/*
			case LangTokenType::DEFINE_BUILT_IN_TYPE_IN_CLASS:
				writeVisibilityForElement(writer, set_elem);

				writer.writeString( set_elem.builtInTypeStringCpp() );
				writer.writeChar(' ');
				writer.writeString(set_elem.name());
			break;
				 */
			
			case LangTokenType::INIT_DATA:
			
			{
				bool write_init_data = true;

				if( writer.isHeader() == true )//hpp
				{
					if( set_elem.previousToken() == LangTokenType::DEFINE_FUNC_ARGUMENT)
					{
						write_init_data = true;
					}
					else if( set_elem.previousToken() == LangTokenType::DEFINE_REFERENCE )
					{
						write_init_data = false;
					}
				}
				else// if( writer.isHeader() == false )//cpp
				{
					if( set_elem.previousToken() == LangTokenType::DEFINE_FUNC_ARGUMENT)
					{
						write_init_data = false;//here we don't want default initializers. c++ doesn't allow them...
					}
					else if( set_elem.previousToken() == LangTokenType::DEFINE_REFERENCE )
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
			case LangTokenType::AUTO_INIT:
				//case LangTokenType::BUILT_IN_TYPE_AUTO_INIT:
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
				//case LangTokenType::OBJECT_AUTO_INIT:
				else if(set_elem.typeType() == TypeType::REF)
				{
					//writer.writeString(m_type);
					//writer.writeString("* ");
					writer.writeString(set_elem.name());
					writer.writeString(" = new ");
					writer.writeString(set_elem.typeInCpp());
					writer.writeString("()");
				}
				//case LangTokenType::TEMPLATE_AUTO_INIT:
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
				//case LangTokenType::VECTOR_AUTO_INIT:
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
				//case LangTokenType::C_ARRAY_AUTO_INIT:
				
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
			case LangTokenType::FREE:
			case LangTokenType::AUTO_FREE:
			//case LangTokenType::OBJECT_AUTO_FREE:
			//case LangTokenType::TEMPLATE_AUTO_FREE:
			//case LangTokenType::VECTOR_AUTO_FREE://TODO do we need to clear the vector before delete?
					//writer.writeString(m_type);
					//writer.writeString("* ");
					
					//case LangTokenType::C_ARRAY_AUTO_FREE:
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
			case LangTokenType::BOOL:
				writer.writeString( "bool " );
				writer.writeString(m_name);
			break;
			case LangTokenType::BYTE:
				writer.writeString( "int8_t " );
				writer.writeString(m_name);
			break;
			case LangTokenType::UBYTE:
				writer.writeString( "uint8_t " );
				writer.writeString(m_name);
			break;
			case LangTokenType::CHAR:
				writer.writeString( "char " );
				writer.writeString(m_name);
			break;
			case LangTokenType::WCHAR:
				writer.writeString( "wchar " );//TODO check if it works and is int32_t (or 16 bits better...)
				writer.writeString(m_name);
			break;
			case LangTokenType::DCHAR:
				writer.writeString( "int32_t " );
				writer.writeString(m_name);
			break;
			case LangTokenType::INT:
				writer.writeString( "int32_t " );
				writer.writeString(m_name);
			break;
			case LangTokenType::UINT:
				writer.writeString( "uint32_t " );
				writer.writeString(m_name);
			break;
			case LangTokenType::LONG:
				writer.writeString( "int64_t " );
				writer.writeString(m_name);
			break;
			case LangTokenType::ULONG:
				writer.writeString( "uint64_t " );
				writer.writeString(m_name);
			break;
			case LangTokenType::FLOAT:
				writer.writeString( "float " );
				writer.writeString(m_name);
			break;
			case LangTokenType::DOUBLE:
				writer.writeString( "double " );
				writer.writeString(m_name);
			break;
			//case LangTokenType::REAL:
			//	writer.writeString( "real " );
			//break;
			case LangTokenType::INT_IN_CLASS:
				writer.writeString( "int32_t " );
				writer.writeString(m_name);
			break;
			*/
			case LangTokenType::EQUALS:
				/*
				if( writer.previousToken() == LangTokenType::DEFINE_BUILT_IN_TYPE_IN_CLASS )
				{
					//sneaky. write comment slashes:
					writer.writeString("; //");
				}
				*/
				if( writer.previousElement()
					&& writer.previousElement()->isInClass()
					&& writer.previousToken() == LangTokenType::DEFINE_REFERENCE
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
			case LangTokenType::COMMA:
				writer.writeChar( ',' );
				writer.writeChar( ' ' );
			break;
			case LangTokenType::PLUS:
				if( writer.previousToken() == LangTokenType::PLUS || writer.nextToken() == LangTokenType::PLUS )
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
			case LangTokenType::MINUS:
				if( writer.previousToken() == LangTokenType::MINUS || writer.nextToken() == LangTokenType::MINUS )
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
			case LangTokenType::SEMICOLON:
				if( writer.nextToken() == LangTokenType::NEWLINE || writer.nextToken() == LangTokenType::NEWLINE_BEFORE_SCOPE_END )
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
			case LangTokenType::DIVIDE:
				writer.writeChar( ' ' );
				writer.writeChar( '/' );
				writer.writeChar( ' ' );
			break;
			case LangTokenType::BIGGER_THAN:
				writer.writeChar( ' ' );
				writer.writeChar( '>' );
				writer.writeChar( ' ' );
			break;
			case LangTokenType::SMALLER_THAN:
				writer.writeChar( ' ' );
				writer.writeChar( '<' );
				writer.writeChar( ' ' );
			break;
			case LangTokenType::REFERENCE_DOT:
				if( writer.previousToken() == LangTokenType::NUMBER )
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
			case LangTokenType::USE_MEMBER:
			case LangTokenType::USE_REFERENCE:
				if( set_elem.typeType() == TypeType::VECTOR )
				{
					if( writer.nextToken() == LangTokenType::BRACKET_BEGIN )
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
			//case LangTokenType::UNKNOWN_USE_REFERENCE:
			//	writer.writeString("/*possible error:*/");
			//	writer.writeString( set_elem.name() );
			//break;
			//case LangTokenType::USE_ARRAY:
			//	writer.writeString( set_elem.name() );
			//break;
			case LangTokenType::PARENTHESIS_BEGIN:
			case LangTokenType::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES:
				writer.writeChar( '(' );
			break;
			case LangTokenType::PARENTHESIS_BEGIN_FUNC_RETURN_TYPES:
				//no output.
			break;
			case LangTokenType::PARENTHESIS_END:
			case LangTokenType::PARENTHESIS_END_FUNC_PARAM_TYPES:
				writer.writeChar( ')' );
			break;
			case LangTokenType::PARENTHESIS_END_FUNC_RETURN_TYPES:
				//no output
			break;
			case LangTokenType::LOG_SEPARATOR:
			case LangTokenType::PARENTHESIS_BEGIN_LOG:
			case LangTokenType::PARENTHESIS_BEGIN_LOG_LN:
				writer.writeChar( '<' );
				writer.writeChar( '<' );
			break;
			case LangTokenType::PARENTHESIS_END_LOG:
				writer.writeString("<<\"\\n\"");//That's a little funky looking line, but it's supposed to look just like that. Hope your syntax highlighting can handle it.
				//it's just , "\n"
			break;
			case LangTokenType::PARENTHESIS_END_LOG_LN:
				/////writer.writeChar( ';' );
				
			break;
			case LangTokenType::BRACKET_BEGIN:
				//if( set_elem.previousToken() == LangTokenType::DEFINE_ARRAY || set_elem.previousToken() == LangTokenType::DEFINE_ARRAY_IN_CLASS )
				
				//if( set_elem.previousToken() == LangTokenType::DEFINE_REFERENCE && set_elem.previousElement()->typeType() == TypeType::C_ARRAY )
				//{
					//ignore them after definition.
				//}
				//else
				//{
					writer.writeString(set_elem.name());
				//}
			break;
			case LangTokenType::BRACKET_END:
				//if( set_elem.previousToken() == LangTokenType::BRACKET_BEGIN
					//NO: && (set_elem.previous2ndToken() == LangTokenType::DEFINE_ARRAY || set_elem.previous2ndToken() == LangTokenType::DEFINE_ARRAY_IN_CLASS)
				   //&& (set_elem.previous2ndToken() == LangTokenType::DEFINE_REFERENCE && set_elem.previous2ndElement()->typeType() == TypeType::C_ARRAY )
					//)
				//{
					//ignore them after definition.
				//}
				//else
				//{
					writer.writeString(set_elem.name());
				//}
			break;
			case LangTokenType::BRACKET_DEFINE_ARRAY_BEGIN:
				//writer.writeString("*");//a dynamic array is a pointer in C/C++
				//already written in NEW_ARRAY...
			break;
			case LangTokenType::BRACKET_DEFINE_ARRAY_END:
				//writer.writeString("*");//a dynamic array is a pointer in C/C++
				//so we write nothing here.
			break;
			
			/*case LangTokenType::DEFINE_REFERENCE_IN_CLASS:
				writeVisibilityForElement(writer, set_elem);

				//if( parentLangTokenType() == LangTokenType::CLASS )
				//{
					writer.writeString(set_elem.typeInCpp());
					writer.writeString("* ");
					writer.writeString(set_elem.name());
				//}	
			break;
			*/
			case LangTokenType::DEFINE_REFERENCE:
			
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
						writer.writeString(set_elem.name());
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
						writer.writeString(set_elem.name());
					}
					else
					{	
						if( set_elem.typeType() == TypeType::REF )
						{
							writer.writeString(set_elem.typeInCpp());
							writer.writeString("* ");
							writer.writeString(set_elem.name());
							writer.writeString(" = new ");
							writer.writeString(set_elem.typeInCpp());
							writer.writeString("()");
						}
						else //opt or link
						{
							if(set_elem.nextToken() == LangTokenType::EQUALS )
							{
								writer.writeString(set_elem.typeInCpp());
								writer.writeString("* ");
								writer.writeString(set_elem.name());		
							}
							else
							{
								writer.writeString(set_elem.typeInCpp());
								writer.writeString("* ");
								writer.writeString(set_elem.name());
								writer.writeString(" = new ");
								writer.writeString(set_elem.typeInCpp());
								writer.writeString("()");
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
			//case LangTokenType::DEFINE_BUILT_IN_TYPE:
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
						writer.writeString(set_elem.name());
						
						if( writer.nextToken() == LangTokenType::EQUALS )
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
				//break;
				//case LangTokenType::DEFINE_ARRAY_IN_CLASS:
				/*else if( set_elem.typeType() == TypeType::C_ARRAY )
				{
					if(set_elem.isInClass())
					{
						writeVisibilityForElement(writer, set_elem);

						//if( parentLangTokenType() == LangTokenType::CLASS )
						//{
						writer.writeString(set_elem.typeInCpp());
						writer.writeString("* ");
						writer.writeString(set_elem.name());
						//}
					}	
				//break;
				//case LangTokenType::DEFINE_ARRAY:
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
				//case LangTokenType::DEFINE_VECTOR_IN_CLASS:
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
				//case LangTokenType::DEFINE_VECTOR:
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
			case LangTokenType::TEMPLATE_SECOND_TYPE:
				//ignore. already written above.
			break;
			
			/*
			case LangTokenType::USE_VECTOR:
				if( writer.nextToken() == LangTokenType::BRACKET_BEGIN )
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
			case LangTokenType::IF:
			case LangTokenType::FOR:
			case LangTokenType::FOREACH:
				writer.lineNeedsSemicolon(false);
				writer.writeString(set_elem.name());
			break;
			case LangTokenType::FUNC_CALL:
				writer.writeString(set_elem.name());
				if( writer.nextToken() != LangTokenType::PARENTHESIS_BEGIN)
					writer.writeString("()");
			break;
			case LangTokenType::SCOPE_BEGIN:
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
			case LangTokenType::SCOPE_END:
				//writer.currentIndentMinus();
				//writer.writeIndents();
				writer.lineNeedsSemicolon(false);
				writer.writeString( set_elem.name() );
				if( set_elem.parentLangTokenType() == LangTokenType::CLASS || set_elem.parentLangTokenType() == LangTokenType::ENUM )
				{
					writer.writeString( ";\n\n" );
					//rae::log("writing }; SCOPE_END for class or enum.");
				}
				else if( set_elem.parentLangTokenType() == LangTokenType::FUNC || set_elem.parentLangTokenType() == LangTokenType::CONSTRUCTOR || set_elem.parentLangTokenType() == LangTokenType::DESTRUCTOR )
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
			case LangTokenType::QUOTE:
				//writer.writeChar('\"');
				writer.writeString(set_elem.name());
				//writer.writeChar('\"');
			break;
			case LangTokenType::COMMENT:
				if( writer.lineNeedsSemicolon() == true
					&& writer.previousToken() != LangTokenType::SEMICOLON
					&& writer.previousToken() != LangTokenType::NEWLINE
					&& writer.previousToken() != LangTokenType::NEWLINE_BEFORE_SCOPE_END
					/*&& writer.previousToken() != LangTokenType::SCOPE_BEGIN
					&& writer.previousToken() != LangTokenType::SCOPE_END
					&& writer.previousToken() != LangTokenType::FUNC
					&& writer.previousToken() != LangTokenType::DEFINE_FUNC_ARGUMENT
					&& writer.previousToken() != LangTokenType::DEFINE_FUNC_RETURN //not actually needed...
					&& writer.previousToken() != LangTokenType::CLASS
					&& writer.previousToken() != LangTokenType::COMMENT
					//&& writer.previousToken() != LangTokenType::STAR_COMMENT
					&& writer.previousToken() != LangTokenType::VISIBILITY_DEFAULT
					*/
				)
				{
					writer.writeChar(';');
				}

				writer.lineNeedsSemicolon(false);

				writer.writeString(set_elem.name());
			break;
			case LangTokenType::PLUS_COMMENT:
				//So if the plus comment is the only thing on this line
				//Then we don't need a semicolon...
				if(set_elem.previousToken() == LangTokenType::NEWLINE)
				{
					writer.lineNeedsSemicolon(false);
				}

				writer.writeString("/*");//comment it out in c++ syntax!
				writer.writeString(set_elem.name());
				writer.writeString("*/");
			break;
			case LangTokenType::STAR_COMMENT:
				//writer.writeChars("/*", 2);

				//So if the star comment is the only thing on this line
				//Then we don't need a semicolon...
				if(set_elem.previousToken() == LangTokenType::NEWLINE)
				{
					writer.lineNeedsSemicolon(false);
				}

				writer.writeString(set_elem.name());
				//writer.writeChar('/');
			break;
			case LangTokenType::LOG:
				writer.writeString("std::cout");
			break;
			case LangTokenType::LOG_LN:
				writer.writeString("std::cout");
			break;

			/*
			case LangTokenType::CONSTRUCTOR:
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
			case LangTokenType::DESTRUCTOR:
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
			case LangTokenType::FUNC:
			case LangTokenType::CONSTRUCTOR:
			case LangTokenType::DESTRUCTOR:
			case LangTokenType::MAIN:
				//writer.writeIndents();
				
				{

				if( writer.isHeader() == false )//cpp
				{
					writer.lineNeedsSemicolon(false);
				}
				else if( set_elem.parent() && set_elem.parent()->langTokenType() == LangTokenType::CLASS )
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
					if( set_elem.langTokenType() == LangTokenType::FUNC || (set_elem.langTokenType() == LangTokenType::MAIN && writer.isHeader() == false) )
					{
						//NOT: reuse myelem //<-- That might be a good keyword for Rae! Not really.
						//LangElement* myelem = set_elem.langElements.front();
						LangElement* myelem = set_elem.searchFirst(LangTokenType::DEFINE_FUNC_RETURN);

						/*
						if( myelem != 0 )
						{
							//rae::log("func_elem: front: ", myelem->langTokenTypeString();
							writer.writeString( myelem->langTokenTypeString() );
							writer.writeChar( '\n' );
							writer.writeIndents();
						}
						*/



						if( myelem != 0 )
						{
							if( myelem->langTokenType() == LangTokenType::DEFINE_FUNC_RETURN )
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

						if( writer.isHeader() == false )//cpp
						{
							if( set_elem.langTokenType() != LangTokenType::MAIN && set_elem.parent() )
							{
								//This one does writes the class name in cpp func implementations.
								//Class::funcName(params)
								writer.writeString( set_elem.parent()->name() );
								writer.writeString( "::" );
							}
						}

						writer.writeString( set_elem.name() );
					}//if func
					else if( set_elem.langTokenType() == LangTokenType::CONSTRUCTOR )
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
					else if( set_elem.langTokenType() == LangTokenType::DESTRUCTOR )
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

				}//just to make a nu LangElement*

				
				////////////////////////writer.writeChar( '(' );
				/////////writer.writeChar( ' ' );

				if( writer.isHeader() == false )//cpp
				{
					if( set_elem.langTokenType() == LangTokenType::MAIN )
					{
						writer.writeString( "(int argc, char* const argv[])" );
						/*
						foreach( LangElement* elem, set_elem.langElements )
						{
							if( elem->langTokenType() == LangTokenType::DEFINE_FUNC_RETURN )
							{
								//do nothing
							}
							else if( elem->langTokenType() == LangTokenType::VISIBILITY )
							{
								//do nothing
							}
							else if( elem->langTokenType() == LangTokenType::DEFINE_FUNC_ARGUMENT )
							{
								//DON'T WANT THESE FOR MAIN, FOR NOW...
								
								//writer.writeString( elem->type() );
								//writer.writeChar( ' ' );
								//writer.writeString( elem->name() );
								
							}
							else if( elem->langTokenType() == LangTokenType::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES )
							{
								//already written for main
							}
							else if( elem->langTokenType() == LangTokenType::PARENTHESIS_END_FUNC_PARAM_TYPES )
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
							for(uint i = 0; i < set_elem.langElements.size(); i++)
							{
								if(i+1 < set_elem.langElements.size())
								{
									//This misses some nextTokens, which are not children of
									//this LangElement, but maybe that doesn't matter that much,
									//they'll be members of different things anyway, and might not
									//be related.
									//writer.nextToken( set_elem.langElements[i+1]->langTokenType() );
									writer.nextElement( set_elem.langElements[i+1] );
								}

								if( set_elem.langElements[i]->langTokenType() == LangTokenType::DEFINE_FUNC_RETURN )
								{
									//do nothing
								}
								else if( set_elem.langElements[i]->langTokenType() == LangTokenType::VISIBILITY )
								{
									//do nothing
								}
								else if( set_elem.langElements[i]->langTokenType() == LangTokenType::DEFINE_FUNC_ARGUMENT )
								{
									//DON'T WANT THESE FOR MAIN, FOR NOW...
									
									//writer.writeString( elem->type() );
									//writer.writeChar( ' ' );
									//writer.writeString( elem->name() );
									
								}
								else if( set_elem.langElements[i]->langTokenType() == LangTokenType::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES )
								{
									//already written for main
								}
								else if( set_elem.langElements[i]->langTokenType() == LangTokenType::PARENTHESIS_END_FUNC_PARAM_TYPES )
								{
									//already written for main...

									/////////writer.writeChar( ' ' );
									//writer.writeChar( ')' );
									//writer.writeChar( '\n' );
								}
								else
								{
									//set_elem.langElements[i]->write(writer);
									writeElement(writer, *set_elem.langElements[i]);
								}
							}
						}
					}
				}//end isHeader cpp
				
					if( set_elem.langTokenType() == LangTokenType::MAIN )
					{
						//do nothing for main, it's already been written...
					}
					else //a normal FUNC
					{
						/*
						foreach( LangElement* elem, set_elem.langElements )
						{
							if( elem->langTokenType() == LangTokenType::DEFINE_FUNC_RETURN )
							{
								//do nothing
							}
							else if( elem->langTokenType() == LangTokenType::VISIBILITY )
							{
								//do nothing
							}
							else if( elem->langTokenType() == LangTokenType::DEFINE_FUNC_ARGUMENT )
							{
								writer.writeString( elem->type() );
								writer.writeChar( ' ' );
								writer.writeString( elem->name() );
								
							}
							//else if( elem->langTokenType() == LangTokenType::PARENTHESIS_END_FUNC_PARAM_TYPES )
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
									//writer.nextToken( set_elem.langElements[i+1]->langTokenType() );
									writer.nextElement( set_elem.langElements[i+1] );
								}

								if( set_elem.langElements[i]->langTokenType() == LangTokenType::DEFINE_FUNC_RETURN )
								{
									//do nothing, already written
								}
								else if( set_elem.langElements[i]->langTokenType() == LangTokenType::VISIBILITY )
								{
									//do nothing, already written
								}
								else if( set_elem.langElements[i]->langTokenType() == LangTokenType::DEFINE_FUNC_ARGUMENT )
								{
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
								/*else if( set_elem.langElements[i]->langTokenType() == LangTokenType::PARENTHESIS_END_FUNC_PARAM_TYPES )
								{
									/////////writer.writeChar( ' ' );
									writer.writeChar( ')' );
									///////writer.writeChar( '\n' );
								}*/
								else if( set_elem.langElements[i]->langTokenType() == LangTokenType::NEWLINE)
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
								else if( set_elem.langElements[i]->langTokenType() == LangTokenType::PARENTHESIS_BEGIN
									|| set_elem.langElements[i]->langTokenType() == LangTokenType::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES
									|| set_elem.langElements[i]->langTokenType() == LangTokenType::PARENTHESIS_END
									|| set_elem.langElements[i]->langTokenType() == LangTokenType::PARENTHESIS_END_FUNC_PARAM_TYPES)
								{
									//handle this separately because of isHeader.
									set_elem.langElements[i]->write(writer);
								}*/
								else
								{
									/*
									debug:
									if( langTokenType() == LangTokenType::CONSTRUCTOR )
									{
										//rae::log("const: elem: ", i, " is ", set_elem.langElements[i]->toString(), "\n");

										if( set_elem.langElements[i]->parentLangTokenType() == LangTokenType::CLASS || set_elem.langElements[i]->parentLangTokenType() == LangTokenType::ENUM )
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
				
/*
				uint count_elem = 0;
				foreach( LangElement* elem, set_elem.langElements )
				{
					if( count_elem == 0)
					{
						//the first element
						if( elem->langTokenType() == LangTokenType::RETURN)
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
			case LangTokenType::CLASS:
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
							if( set_elem.langElements[i]->langTokenType() == LangTokenType::FUNC
								|| set_elem.langElements[i]->langTokenType() == LangTokenType::CONSTRUCTOR
								|| set_elem.langElements[i]->langTokenType() == LangTokenType::DESTRUCTOR
							)
							{
								if(i+1 < set_elem.langElements.size())
								{
									//This misses some nextTokens, which are not children of
									//this LangElement, but maybe that doesn't matter that much,
									//they'll be members of different things anyway, and might not
									//be related.
									//writer.nextToken( set_elem.langElements[i+1]->langTokenType() );
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
			case LangTokenType::VISIBILITY:
				if( writer.isHeader() == true )
				{
					/*ON PURPOSE LEFT OUT: if( writer.currentDefaultVisibility() != set_elem.name() )
					{
						writer.nextNeedsDefaultVisibility(true);//tanne need to fix this maybe to an int 2,1,0
					}
					*/
					
					//writer.writeIndents(); //don't indent public: etc...
					if( writer.nextToken() == LangTokenType::NEWLINE )
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
			case LangTokenType::VISIBILITY_DEFAULT:
				if( writer.isHeader() == true )
				{
					//if( LangElement::isVisibilityNameAllowedInCpp(set_elem.name()) )
					//{
						writer.currentDefaultVisibility(set_elem.name());
						writer.nextNeedsDefaultVisibility(false);

						//writer.writeIndents(); //don't indent public: etc...
						if( writer.nextToken() == LangTokenType::NEWLINE )
						{
							writer.lineNeedsSemicolon(false);
						}

						writer.writeString( LangElement::getVisibilityNameInCpp(set_elem.name()) );

						//writer.writeString( set_elem.name() );
						//writer.writeString( ": " );
					/*}
					else
					{
						if( writer.nextToken() == LangTokenType::NEWLINE )
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
			case LangTokenType::IMPORT_NAME:
			case LangTokenType::MODULE_NAME:
				//just ignore this type...
			break;
			case LangTokenType::IMPORT:
				if( writer.isHeader() == true )
				{
					writer.lineNeedsSemicolon(false);
					writer.writeString( string("#include \"") );
					int not_on_first = 0;
					foreach( LangElement* elem, set_elem.langElements )
					{
						if( elem->langTokenType() == LangTokenType::IMPORT_NAME )
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

					if( set_elem.nextElement() && set_elem.nextElement()->langTokenType() != LangTokenType::IMPORT )
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
					LangElement* last_module_name_element = set_elem.searchLast( LangTokenType::IMPORT_NAME );
					writer.writeString("#include \"");
					writer.writeString( last_module_name_element->name() );
					writer.writeString(".hpp\"");

					iterateWrite(writer, set_elem);
				}
			break;
			case LangTokenType::MODULE:
				if( writer.isHeader() == true )
				{
					writer.lineNeedsSemicolon(false);
					writer.writeString( string("#ifndef _") );
					foreach( LangElement* elem, set_elem.langElements )
					{
						if( elem->langTokenType() == LangTokenType::MODULE_NAME )
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
						if( elem->langTokenType() == LangTokenType::MODULE_NAME )
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
					LangElement* last_module_name_element = set_elem.searchLast( LangTokenType::MODULE_NAME );
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
			case LangTokenType::CLOSE_MODULE:
				if( writer.isHeader() == true )
				{
					writer.lineNeedsSemicolon(false);
					writer.writeString( string("#endif // _") );
					
					/*foreach( LangElement* elem, set_elem.langElements )
					{
						if( elem->langTokenType() == LangTokenType::MODULE_NAME )
						{
							writer.writeString( elem->name() );
							writer.writeChar('_');
						}
					}*/

					//our set_elem.parent is the module now:
					if( set_elem.parent() && set_elem.parent()->langTokenType() == LangTokenType::MODULE)
					{
						foreach( LangElement* elem, set_elem.parent()->langElements )
						{
							if( elem->langTokenType() == LangTokenType::MODULE_NAME )
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
		//writer.previousToken( langTokenType() );
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
		else if(writer.previousToken() == LangTokenType::VISIBILITY)
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

};

