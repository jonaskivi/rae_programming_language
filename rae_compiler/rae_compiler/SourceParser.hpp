#ifndef RAE_COMPILER_SOURCEPARSER_HPP
#define RAE_COMPILER_SOURCEPARSER_HPP

#include "ReportError.hpp"

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



///////////////////put this into rae_helpers.cpp:

PathType::e checkPathType(boost::filesystem::path currentFilenamePath)
{	
	PathType::e path_type;
	path_type = PathType::DOES_NOT_EXIST; //we're a bit pessimistic here.

	try
	{
		if (boost::filesystem::exists(currentFilenamePath))    // does currentFilenamePath actually exist?
		{
			if (boost::filesystem::is_regular_file(currentFilenamePath))        // is currentFilenamePath a regular file?
			{
				#ifdef DEBUG_RAE_HUMAN
				cout << currentFilenamePath << " size is " << boost::filesystem::file_size(currentFilenamePath) << "\n";
				//rae::log(currentFilenamePath.string(), " size is ", boost::filesystem::file_size(currentFilenamePath), "\n");
				#endif

				path_type = PathType::FILE;
			}
			else if( boost::filesystem::is_directory(currentFilenamePath) )      // is currentFilenamePath a directory?
			{
				path_type = PathType::DIRECTORY;

				cout << currentFilenamePath << " is a directory containing:\n";
				//rae::log(currentFilenamePath.string(), "is a directory containing:\n");


				typedef vector<boost::filesystem::path> vec;             // store paths,
				vec v;                                // so we can sort them later

				copy(boost::filesystem::directory_iterator(currentFilenamePath), boost::filesystem::directory_iterator(), back_inserter(v));

				sort(v.begin(), v.end());             // sort, since directory iteration
													  // is not ordered on some file systems

				for (vec::const_iterator it (v.begin()); it != v.end(); ++it)
				{
				  cout << "   " << *it << "\n";
					//rae::log("   ", *it, "\n");
				}
			}
			else
			{
				//Hmm, we are not handling this in PathType... oh well. What is it really? A link? A ufo?

				cout << currentFilenamePath << " exists, but is neither a regular file nor a directory\n";
				//rae::log(currentFilenamePath, " exists, but is neither a regular file nor a directory\n");
			}
		}
		else
		{
			cout << currentFilenamePath << " does not exist\n";
			//rae::log(currentFilenamePath, " does not exist\n");

			path_type = PathType::DOES_NOT_EXIST;
		}
	}
	catch (const boost::filesystem::filesystem_error& ex)
	{
		cout << ex.what() << "\n";
		//rae::log(ex.what(), "\n");
	}

	return path_type;
}


/////////////////end rae_helpers.cpp



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
		//m_returnToExpectToken = Token::UNDEFINED;
		//m_expectingToken = Token::UNDEFINED;
		expectingToken(Token::UNDEFINED);
		m_expectingTypeType = TypeType::UNDEFINED;
		m_expectingRole = Role::UNDEFINED;
		expectingChar = 'T';
		foundToken = Token::UNDEFINED;

		//isInsideLogStatement = false;

		injectPointToEndParenthesis = false;

		isQuoteReady = false;
		isWaitingForQuoteEnd = false;
		
		isStarCommentReady = false;
		isWaitingForStarCommentEnd = false;
		
		isPlusCommentReady = false;
		nroWaitingForPlusCommentEnd = 0;

		isWaitingForPragmaToken = false;
		isPassthroughMode = false;
		isPassthroughSourceMode = false;

		m_currentParentElement = 0;
		currentModule = 0;
		currentClass = 0;
		currentFunc = 0;
		currentEnum = 0;
		currentTemplate = 0;
		currentReference = 0;
		currentTempElement = 0;

		isReceivingInitData = false;		
	}

	bool debugWriteLineNumbers;//= false
	
	public: Token::e doReturnToExpectToken()
	{
		if( returnToExpectTokenStack.empty() )
		{
			expectingToken(Token::UNDEFINED);
			cout<<"NO TOKENS: Returned to expectToken: Token::UNDEFINED.\n";
			return m_expectingToken;
		}

		//else
		//expectingToken( returnToExpectTokenStack.back() );
		returnToExpectTokenStack.pop_back();

		if( returnToExpectTokenStack.empty() )
		{
			expectingToken(Token::UNDEFINED);
			cout<<"POPPED AND NO TOKENS: Returned to expectToken: Token::UNDEFINED.\n";
			return m_expectingToken;
		}
		else
		{
			m_expectingToken = returnToExpectTokenStack.back();
		}

		cout<<"Returned to expectToken: "<<Token::toString(m_expectingToken)<<"\n";
		return m_expectingToken;
		//return m_returnToExpectToken;
	}
	/*
	public: void addReturnToExpectToken(Token::e set)
	{
		returnToExpectTokenStack.push_back(set);
		//m_returnToExpectToken = set;
	}*/
	public: Token::e whatIsReturnToExpectToken()//don't use this... just for if you need it to check what it is.
	{
		if( returnToExpectTokenStack.empty() )
			return Token::UNDEFINED;

		//else
		return returnToExpectTokenStack.back();
		//return m_returnToExpectToken;
	}
	//protected: Token::e m_returnToExpectToken;
	protected: vector<Token::e> returnToExpectTokenStack;
	public: Token::e expectingToken(){ return m_expectingToken; }
	public: void expectingToken(Token::e set)
	{
		//#ifdef DEBUG_RAE_PARSER
			cout<<"set expectingToken to: "<<Token::toString(set)<<"\n";
		//#endif
		m_expectingToken = set;
		//returnToExpectTokenStack.push_back(set);
	}
	protected: Token::e m_expectingToken;// = EDLTokenType::TITLE;

	//secondary expecting
	public: TypeType::e expectingTypeType(){ return m_expectingTypeType; }
	public: void expectingTypeType(TypeType::e set){ m_expectingTypeType = set; }
	protected: TypeType::e m_expectingTypeType;

	public: Role::e expectingRole(){ return m_expectingRole; }
	public: void expectingRole(Role::e set){ m_expectingRole = set; }
	protected: Role::e m_expectingRole;

	public:
	int expectingChar;// = 'T';
	Token::e foundToken;// = EDLTokenType::UNDEFINED;
	
	//This is similar to expectingRole and expectingToken TODO make better.
	bool isReceivingInitData;

	bool isSingleLineComment;// = false; //TODO rename to isWaitingForSingleLineComment for concistency.
	string currentComment;
	
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

	//for handling the @ special tokens such as @c++
	bool isWaitingForPragmaToken;
	bool isPassthroughMode;
	bool isPassthroughSourceMode;

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

	LangElement* newLangElement(Token::e set_lang_token_type, TypeType::e set_type_type, string set_name = "", string set_type = "")
	{
		LangElement* lang_elem;
		
		if( currentParentElement() )
		{
			lang_elem = currentParentElement()->newLangElement(lineNumber, set_lang_token_type, set_type_type, set_name, set_type);
		
		//else if( currentModule )
		//{
		
			//lang_elem = currentModule->newLangElement(lineNumber, set_lang_token_type, set_type_type, set_name, set_type);
			if( set_lang_token_type == Token::MODULE
				|| set_lang_token_type == Token::CLASS
				|| set_lang_token_type == Token::FUNC
				|| set_lang_token_type == Token::INIT_DATA //also init data is now a parent element!
				//and it holds all the init_data inside it.
				)
			{
				currentParentElement(lang_elem);
			}
			//langElements.push_back( lang_elem );
			//#ifdef DEBUG_RAE
			//	cout<<"BASE Add langElement: "<<Token::toString(set_lang_token_type)<<" name:>"<<set_name<<"< type:"<<set_type<<"\n");
			//#endif
		}
		else
		{
			ReportError::compilerError("newLangElement() No current parent element found. Do you have a module? Compilers fault. Something went wrong.");
			cout<<"tried to create:"<<Token::toString(set_lang_token_type)<<" name: "<<set_name<<" type: "<<set_type<<"\n";
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
		cout<<"BASE Add langElement: "<<Token::toString(set_lang_token_type)<<" name:>"<<set_name<<"< type:"<<set_type<<"\n");
		return lang_elem;
		*/
	}

	void newScopeBegin()
	{
		/*if( currentParentElement() )
		{
			currentParentElement()->newLangElement(Token::SCOPE_BEGIN, "{");
			cout<<"Begin scope for: "<<Token::toString( currentParentElement()->token() );
		}
		else
		{
			newLangElement(Token::SCOPE_BEGIN, "{");
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


		LangElement* our_scope_elem = newLangElement(Token::SCOPE_BEGIN, TypeType::UNDEFINED, "{");
		
		//If we're already inside a func. Then we create a so called limiting scope, which will act as parent for
		//it's insides.
		//This only works if the parent is a func. Otherwise it wouldn't make any sense, or would it?
		//if( currentParentElement() && currentParentElement()->token() == Token::FUNC )
		if( scopeElementStack.empty() == false )
		{
			//if( scopeElementStack.back()->token() == Token::FUNC )
			//{
				currentParentElement(our_scope_elem);
			//}
		}

		//put the scope or class or func, or init_data object to scope stack.
		scopeElementStack.push_back( currentParentElement() ); //NOT true anymore: but we put the class or func (or limiting scope) in the stack. Not the actual scope object (unless it is a limiting scope.)
		
	}

	void newScopeEnd()
	{
		LangElement* scope_elem = 0;

		if( scopeElementStack.empty() == true )
		{
			//lang_elem = newLangElement(set_lang_token_type, TypeType::UNDEFINED, set_token);
			ReportError::reportError("unmatched scope end \"}\".", previousElement() );
		}
		else
		{
			scope_elem = scopeElementStack.back();
		}
		
		if( scope_elem )
		{
			scope_elem->freeOwned();
			//newLine();
		}

		if( currentParentElement() && currentParentElement()->currentElement() && currentParentElement()->currentElement()->token() == Token::NEWLINE )
		{
			//REVIEW: This code (currentParentElement()->currentElement()) looks very strange. I wonder why it works!

			//we had a newline. mark it.
			currentParentElement()->currentElement()->token(Token::NEWLINE_BEFORE_SCOPE_END);
		}
		
		if( scope_elem )
		{
			LangElement* new_lang_elem = scope_elem->newLangElement(lineNumber, Token::SCOPE_END, TypeType::UNDEFINED, "}" );

			if(new_lang_elem)
			{
				new_lang_elem->previousElement(previousElement());
				previousElement()->nextElement(new_lang_elem);
			}

			if( scope_elem->token() == Token::CLASS )
			{
				#ifdef DEBUG_RAE_HUMAN
				cout<<"end of class.\n\n";
				#endif

				//class ends, do stuff:

				//if there are no constructors then add one.
				if( listOfDestructors.empty() == true )
				{
					
					LangElement* a_con = scope_elem->newLangElementToTopOfClass(lineNumber, Token::DESTRUCTOR, TypeType::UNDEFINED, "free" );
					listOfDestructors.push_back(a_con);
					a_con->newLangElement(lineNumber, Token::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES);
					a_con->newLangElement(lineNumber, Token::PARENTHESIS_END_FUNC_PARAM_TYPES);
					a_con->newLangElement(lineNumber, Token::NEWLINE);
					LangElement* a_scop = a_con->newLangElement(lineNumber, Token::SCOPE_BEGIN, TypeType::UNDEFINED, "{");
					a_scop->newLangElement(lineNumber, Token::NEWLINE_BEFORE_SCOPE_END);
					a_scop->newLangElement(lineNumber, Token::SCOPE_END, TypeType::UNDEFINED, "}");
					//a_con->newLangElement(lineNumber, Token::NEWLINE);
					//a_con->newLangElement(lineNumber, Token::NEWLINE);
				
				}

				if( listOfConstructors.empty() == true )
				{
					#ifdef DEBUG_RAE_HUMAN
					cout<<"Creating a constructor.\n";
					#endif

					LangElement* a_con = scope_elem->newLangElementToTopOfClass(lineNumber, Token::CONSTRUCTOR, TypeType::UNDEFINED, "new" );
					listOfConstructors.push_back(a_con);
					a_con->newLangElement(lineNumber, Token::PARENTHESIS_BEGIN_FUNC_RETURN_TYPES);
					a_con->newLangElement(lineNumber, Token::PARENTHESIS_END_FUNC_RETURN_TYPES);
					a_con->newLangElement(lineNumber, Token::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES);
					a_con->newLangElement(lineNumber, Token::PARENTHESIS_END_FUNC_PARAM_TYPES);
					a_con->newLangElement(lineNumber, Token::NEWLINE);
					LangElement* a_scop = a_con->newLangElement(lineNumber, Token::SCOPE_BEGIN, TypeType::UNDEFINED, "{");
					a_scop->newLangElement(lineNumber, Token::NEWLINE_BEFORE_SCOPE_END);
					a_scop->newLangElement(lineNumber, Token::SCOPE_END, TypeType::UNDEFINED, "}");
					//a_con->newLangElement(lineNumber, Token::NEWLINE);
					//a_con->newLangElement(lineNumber, Token::NEWLINE);
					
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
									if( init_ob->token() == Token::DEFINE_VECTOR_IN_CLASS )
									{
										elem->newLangElementToTopWithNewline( elem->lineNumber(), Token::VECTOR_AUTO_INIT, init_ob->name(), init_ob->type() );
									}
									else if( init_ob->token() == Token::DEFINE_ARRAY_IN_CLASS )
									{
										elem->newLangElementToTopWithNewline( elem->lineNumber(), Token::C_ARRAY_AUTO_INIT, init_ob->name(), init_ob->type() );
									}
									*/
									/*
									if( init_ob->typeType() == TypeType::VECTOR )
									{
										elem->newLangElementToTopWithNewline( elem->lineNumber(), Token::VECTOR_AUTO_INIT, TypeType::VECTOR, init_ob->name(), init_ob->type() );
									}
									else if( init_ob->typeType() == TypeType::TEMPLATE )
									{
										elem->newLangElementToTopWithNewline( elem->lineNumber(), Token::TEMPLATE_AUTO_INIT, TypeType::TEMPLATE, init_ob->name(), init_ob->type() );
									}
									else if( init_ob->typeType() == TypeType::C_ARRAY )
									{
										elem->newLangElementToTopWithNewline( elem->lineNumber(), Token::C_ARRAY_AUTO_INIT, TypeType::C_ARRAY, init_ob->name(), init_ob->type() );
									}
									else
									{	
										elem->newLangElementToTopWithNewline( elem->lineNumber(), Token::OBJECT_AUTO_INIT, TypeType::REF, init_ob->name(), init_ob->type() );
									}*/
									LangElement* auto_init_elem = init_ob->copy();
									auto_init_elem->token(Token::AUTO_INIT);
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
								//LangElement* bui_le = elem->newLangElementToTopWithNewline( elem->lineNumber(), Token::BUILT_IN_TYPE_AUTO_INIT, TypeType::BUILT_IN_TYPE, init_ob->name(), init_ob->type(), init_ob->builtInType() );//TODO value...
								
								LangElement* auto_init_elem = init_ob->copy();
								auto_init_elem->token(Token::AUTO_INIT);
								elem->addElementToTopOfFunc(auto_init_elem);
								//THIS is not needed, as the init_ob->copy() already does this:
								//auto_init_elem->initData( init_ob->initData() );//Copy initdata!!
								
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
									//THIS IS THE CORRECT AUTO_FREE IF WE EVER HAPPEN TO NEED IT AGAIN:
									LangElement* auto_init_elem = init_ob->copy();
									auto_init_elem->token(Token::AUTO_FREE);
									elem->addElementToTopOfFunc(auto_init_elem);
									*/
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
			else if( scope_elem->token() == Token::FUNC )
			{
				#ifdef DEBUG_RAE_HUMAN
				cout<<"end of function.\n\n";
				#endif
				currentFunc = 0;
			}
			else if( scope_elem->token() == Token::ENUM )
			{
				currentEnum = 0;
			}

			#ifdef DEBUG_RAE
			cout<<"End scope for: "<<Token::toString( scope_elem->token() );
			//rae::log("End scope for: ",Token::toString( scope_elem->token() ));
			#endif
		}
		else
		{
			newLangElement(Token::SCOPE_END, TypeType::UNDEFINED, "}");
 		}
	 	
	 	if( scopeElementStack.empty() == false )
		{
 			scopeElementStack.pop_back();
 		}
		
		//That removed an element so we have to check for empty again:

		if( scopeElementStack.empty() == false )
		{
			currentParentElement( scopeElementStack.back() );
			if( currentParentElement() )
			{
				if( currentParentElement()->token() == Token::CLASS )
				{
					//...so if the current scope is another class, we mark it again.
					currentClass = currentParentElement();
				}
				else if( currentParentElement()->token() == Token::FUNC )
				{
					currentFunc = currentParentElement();
				}
				else if( currentParentElement()->token() == Token::ENUM )
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

	//this could also be be renamed to endCurrentParentElement(), but as we're
	//currently using it just for this one thing:
	void endInitData()
	{
		isReceivingInitData = false;

		LangElement* initdata_start_elem = currentParentElement();

		if(initdata_start_elem && initdata_start_elem->token() != Token::INIT_DATA)
		{
			ReportError::reportError("unmatched initdata end.", initdata_start_elem );
			return;
		}
		else
		{
			#ifdef DEBUG_RAE
			cout<<"End init_data.\n" );
			//rae::log("End scope for: ",Token::toString( scope_elem->token() ));
			#endif
		}

		/*if( scopeElementStack.empty() == true )
		{
			//lang_elem = newLangElement(set_lang_token_type, TypeType::UNDEFINED, set_token);
			ReportError::reportError("unmatched initdata end.", previousElement() );
			return;
		}
		else
		{
			scope_elem = scopeElementStack.back();
		}*/
		
		//removed freeOwned from here.
		//removed checking for NEWLINE_BEFORE_SCOPE_END
		
		//That removed an element so we have to check for empty again:

		if( scopeElementStack.empty() == false )
		{
			//This replaces the INIT_DATA element from being the currentParentElement. Usually it will now be the class again.
			currentParentElement( scopeElementStack.back() );
			if( currentParentElement() )
			{
				if( currentParentElement()->token() == Token::CLASS )
				{
					//...so if the current scope is another class, we mark it again.
					currentClass = currentParentElement();
				}
				else if( currentParentElement()->token() == Token::FUNC )
				{
					currentFunc = currentParentElement();
				}
				else if( currentParentElement()->token() == Token::ENUM )
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
			cout<<"error in SourceParser.endInitData(). no currentModule!!!!!!\n";
		}	
	}

	LangElement* newParenthesisBegin(Token::e set_lang_token_type, string set_token)
	{
		LangElement* lang_elem = newLangElement(set_lang_token_type, TypeType::UNDEFINED, set_token);
		//parenthesisStack.push_back(currentParentElement());
		parenthesisStack.push_back(lang_elem);

		//NOTE: we push the parenthesis_begin elements to the stack!
		//not the currentParentElements like in the scopeElementsStack...!
		return lang_elem;
	}

	//a small helper, just so we can get the token easily...
	Token::e parenthesisStackTokenType()
	{
		LangElement* stack_elem = parenthesisStack.back();

		if(stack_elem)
		{
			return stack_elem->token();
		}
		//else
		return Token::UNDEFINED;
	}

	LangElement* newParenthesisEnd(Token::e set_lang_token_type, string set_token)
	{
		LangElement* stack_elem;
		LangElement* lang_elem;

		if( parenthesisStack.empty() == true )
		{
			lang_elem = newLangElement(set_lang_token_type, TypeType::UNDEFINED, set_token);
			ReportError::reportError("unmatched parenthesis end \")\".", lang_elem);
		}
		else
		{
			stack_elem = parenthesisStack.back();

			if( stack_elem )
			{
				//We just do matching: is it good enough:

				lang_elem = newLangElement(Token::matchParenthesisEnd(stack_elem->token()), TypeType::UNDEFINED, set_token);

				#ifdef DEBUG_RAE
				/*cout<<"MATCH parenthesis: from: "<<Token::toString(stack_elem->token())
					<<" to: "<<Token::toString(Token::matchParenthesisEnd(stack_elem->token()))
					<<" caller: "<<Token::toString(set_lang_token_type)<<"\n");*/
				//rae::log("match parenthesis...\n");
				#endif

				/*
				if( stack_elem->token() == Token::PARENTHESIS_BEGIN_LOG )
				{
					stack_elem->newLangElement(lineNumber, set_lang_token_type, set_token );
				}
				else
				{
					stack_elem->newLangElement(lineNumber, set_lang_token_type, set_token );
				}
				*/
				
				//cout<<"End parenthesis for: "<<Token::toString( scope_elem->token() );
			}
			else
			{
				lang_elem = newLangElement(set_lang_token_type, TypeType::UNDEFINED, set_token);
	 		}
		 	
	 		parenthesisStack.pop_back();
	 	}

 		return lang_elem;
	}

	LangElement* newBracketBegin(Token::e set_lang_token_type, string set_token)
	{
		LangElement* lang_elem = newLangElement(set_lang_token_type, TypeType::UNDEFINED, set_token);
		//bracketStack.push_back(currentParentElement());
		bracketStack.push_back(lang_elem);

		//cout<<"bracketStack.push_back() called.\n");

		//NOTE: we push the bracket_begin elements to the stack!
		//not the currentParentElements like in the scopeElementsStack...!
		return lang_elem;
	}

	LangElement* newBracketEnd(Token::e set_lang_token_type, string set_token)
	{
		LangElement* stack_elem;
		LangElement* lang_elem;

		if( bracketStack.empty() == true )
		{
			lang_elem = newLangElement(set_lang_token_type, TypeType::UNDEFINED, set_token);
			ReportError::reportError("unmatched bracket end \"]\".", lang_elem);
		}
		else
		{			
			stack_elem = bracketStack.back();

			if( stack_elem )
			{
				//cout<<"we has stack_elem in newBracketEnd.\n");
				//We just do matching: is it good enough:

				lang_elem = newLangElement(Token::matchBracketEnd(stack_elem->token()), TypeType::UNDEFINED, set_token);

				//cout<<"I bet we've crashed.\n");


				#ifdef DEBUG_RAE
				cout<<"MATCH bracket: from: "<<Token::toString(stack_elem->token())
					<<" to: "<<Token::toString(Token::matchBracketEnd(stack_elem->token()))
					<<" caller: "<<Token::toString(set_lang_token_type)<<"\n";
				//rae::log("match bracket.\n");
				#endif

				/*
				if( stack_elem->token() == Token::bracket_BEGIN_LOG )
				{
					stack_elem->newLangElement(lineNumber, set_lang_token_type, set_token );
				}
				else
				{
					stack_elem->newLangElement(lineNumber, set_lang_token_type, set_token );
				}
				*/
				
				//cout<<"End bracket for: "<<Token::toString( scope_elem->token() );
			}
			else
			{
				lang_elem = newLangElement(set_lang_token_type, TypeType::UNDEFINED, set_token);
	 		}
		 	
	 		bracketStack.pop_back();
	 	}

 		return lang_elem;
	}
/*
	LangElement* newImport(string set_name)
	{
		LangElement* lang_elem;
		
		if( currentModule )
		{
			lang_elem = currentModule->newLangElement(lineNumber, Token::IMPORT, TypeType::UNDEFINED, set_name);
			currentParentElement(lang_elem);
			
			langElements.push_back( lang_elem );
			#ifdef DEBUG_RAE
				cout<<"newImport: "<<Token::toString(Token::IMPORT)<<" name:>"<<set_name<<"\n";
				//rae::log("newImport: ",Token::toString(Token::IMPORT)," name:>",set_name<<"\n");
			#endif

			//previous2ndElement = previousElement;
			previousElement(lang_elem);

			//currentModule = lang_elem;
			//currentParentElement() = lang_elem;
			currentTempElement = lang_elem;

			expectingToken(Token::IMPORT_NAME);

		}
		else 
		{
			ReportError::reportError("newImport() No current module found. Compilers fault. Something went wrong.");
		}

		return lang_elem;
	}
*/

	LangElement* newImport(string set_name)
	{
		LangElement* lang_elem;
		
		lang_elem = newLangElement(Token::IMPORT, TypeType::UNDEFINED, set_name);
			
			#ifdef DEBUG_RAE
				cout<<"newImport: "<<Token::toString(Token::IMPORT)<<" name:>"<<set_name<<"\n";
				//rae::log("newImport: ",Token::toString(Token::IMPORT)," name:>",set_name<<"\n");
			#endif

		currentTempElement = lang_elem;

		expectingToken(Token::IMPORT_NAME);

		return lang_elem;
	}

	LangElement* newModule(string set_name)
	{
		//LangElement* lang_elem = newLangElement(Token::MODULE, set_name);

		LangElement* lang_elem;
		
		lang_elem = new LangElement(lineNumber, Token::MODULE, TypeType::UNDEFINED, set_name);
		currentParentElement(lang_elem);
			
			langElements.push_back( lang_elem );
			#ifdef DEBUG_RAE
				cout<<"BASE Add module langElement: "<<Token::toString(Token::MODULE)<<" name:>"<<set_name<<"\n";
				//rae::log("BASE Add module langElement: ",Token::toString(Token::MODULE)," name:>",set_name<<"\n");
			#endif

		//previous2ndElement = previousElement;
		previousElement(lang_elem);

		currentModule = lang_elem;
		//currentParentElement() = lang_elem;

		expectingToken(Token::MODULE_NAME);

		return lang_elem;
	}

	/*
	//old version put the closemodule to top level in hierarchy.
	LangElement* closeModule()
	{
		LangElement* lang_elem = newLangElement(Token::CLOSE_MODULE);
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
		//LangElement* lang_elem = newLangElement(Token::CLOSE_MODULE);
		//if( currentModule && lang_elem )
		if( currentModule )
		{
			//let's put the closeModule to be the last element in module...
			
			lang_elem = currentModule->newLangElement(lineNumber, Token::CLOSE_MODULE, TypeType::UNDEFINED);

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
		LangElement* lang_elem = newLangElement(Token::CLASS, set_name);
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
			lang_elem = currentModule->newLangElement(lineNumber, Token::CLASS, TypeType::REF, set_name, set_name);
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
			//lang_elem = newLangElement(Token::CLASS, set_name);
			//currentFunc = lang_elem;
			ReportError::reportError("newClass() No current module found. Compilers fault. Something went wrong.");
		}

		return lang_elem;
	}
*/
	LangElement* newClass(string set_name)
	{
		LangElement* lang_elem;
		//Hmm, we put the name of the class into the name AND type params!!!
		//lang_elem = newLangElement(Token::CLASS, TypeType::REF, set_name, set_name);
		lang_elem = newLangElement(Token::CLASS, TypeType::UNDEFINED, set_name, set_name); //Why did we have typeType set to REF?
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
			lang_elem = currentClass->newLangElement(lineNumber, Token::FUNC, TypeType::UNDEFINED, set_name);
			currentParentElement(lang_elem);//Hmm... we should make this easier...
			currentFunc = lang_elem;
			//currentParentElement() = lang_elem;
		}
		else //no class, so we just make a global func...
		{
			lang_elem = newLangElement(Token::FUNC, TypeType::UNDEFINED, set_name);
			currentFunc = lang_elem;
		}
		addToUserDefinedTokens(lang_elem);

		return lang_elem;
	}
*/

	LangElement* newFunc(string set_name = "")
	{
		LangElement* lang_elem;
		lang_elem = newLangElement(Token::FUNC, TypeType::UNDEFINED, set_name);
		currentFunc = lang_elem;
		
		addToUserDefinedTokens(lang_elem);

		return lang_elem;
	}

	LangElement* newQuote(string set_name = "")
	{
		/*LangElement* lang_elem;
		if( currentParentElement() )
		{
			lang_elem = currentParentElement()->newLangElement(Token::QUOTE, set_name);
			//currentParentElement() = lang_elem;//Hmm... we should make this easier...
			//currentFunc = lang_elem;
			//currentParentElement() = lang_elem;
		}
		else //no current element, so we just make a comment...
		{
			lang_elem = newLangElement(Token::QUOTE, set_name);
		}
		return lang_elem;*/

		#ifdef DEBUG_RAE_HUMAN
			cout<<"newQuote: "<<set_name<<"\n";
			//rae::log("newQuote: ",set_name,"\n");
		#endif
		
		return newLangElement(Token::QUOTE, TypeType::UNDEFINED, set_name);		
	}
	
	LangElement* newPlusComment(string set_name = "")
	{
		#ifdef DEBUG_RAE_HUMAN
			cout<<"newPlusComment: "<<set_name<<"\n";
			//rae::log("newPlusComment: ",set_name,"\n");
		#endif

		return newLangElement(Token::PLUS_COMMENT, TypeType::UNDEFINED, set_name);		
	}

	LangElement* newStarComment(string set_name = "")
	{
		#ifdef DEBUG_RAE_HUMAN
			cout<<"newStarComment: "<<set_name<<"\n";
			//rae::log("newStarComment: ",set_name,"\n");
		#endif

		return newLangElement(Token::STAR_COMMENT, TypeType::UNDEFINED, set_name);		
	}	

	LangElement* newComment(string set_name)
	{
		/*LangElement* lang_elem;
		if( currentParentElement() )
		{
			lang_elem = currentParentElement()->newLangElement(Token::COMMENT, set_name);
			//currentParentElement() = lang_elem;//Hmm... we should make this easier...
			currentFunc = lang_elem;
			//currentParentElement() = lang_elem;
		}
		else //no current element, so we just make a comment...
		{
			lang_elem = newLangElement(Token::COMMENT, set_name);
		}
		return lang_elem;
		*/
	
		#ifdef DEBUG_RAE_HUMAN
			cout<<"newComment: "<<set_name<<"\n";
			//rae::log("newComment: ",set_name,"\n");
		#endif

		return newLangElement(Token::COMMENT, TypeType::UNDEFINED, set_name);
	}

	bool injectPointToEndParenthesis;

	LangElement* newPointToElement()
	{
		injectPointToEndParenthesis = true;
		return newLangElement(Token::POINT_TO, TypeType::UNDEFINED, "->");
	}

	void newLine()
	{
		if(injectPointToEndParenthesis == true)
		{
			injectPointToEndParenthesis = false;
			newLangElement(Token::POINT_TO_END_PARENTHESIS, TypeType::UNDEFINED, ")");
		}

		/*if( currentParentElement() )
		{
			currentParentElement()->newLangElement(Token::NEWLINE, "\n");
			//cout<<"Begin scope for: "<<Token::toString( currentParentElement()->token() );
		}
		else
		{
			cout<<"newLine() AGAIN added to the BASE level.\n");
			newLangElement(Token::NEWLINE, "\n");
		}*/
		
		//if()
		if(currentParentElement() && currentParentElement()->token() != Token::MODULE )
		{
			newLangElement(Token::NEWLINE, TypeType::UNDEFINED, "\n");
		}
		else if( previousToken() == Token::COMMENT )
		{
			//if it's a comment, then do the newline anyway...
			newLangElement(Token::NEWLINE, TypeType::UNDEFINED, "\n");	
		}


	}


	LangElement* newDefineBuiltInType(BuiltInType::e set_built_in_type, Role::e set_role, string set_type, string set_name = "")
	{
		//LangElement* lang_elem = newLangElement(Token::DEFINE_BUILT_IN_TYPE, set_name, set_type);
		LangElement* lang_elem = newLangElement(Token::DEFINE_REFERENCE, TypeType::BUILT_IN_TYPE, set_name, set_type);
		lang_elem->builtInType(set_built_in_type);
		lang_elem->role(set_role);
		//lang_elem->typeType(TypeType::BUILT_IN_TYPE);
		
		currentReference = lang_elem;

		if( lang_elem->parentToken() == Token::CLASS )
		{
			//we are inside a class definition, not a func:
			//lang_elem->token( Token::matchBuiltIntTypesToInClass(set_lang_token_type) );
			/////lang_elem->token( Token::DEFINE_BUILT_IN_TYPE_IN_CLASS );

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
		
		//LangElement* lang_elem = newLangElement(Token::USE_BUILT_IN_TYPE, set_elem->name(), set_elem->type() );
		LangElement* lang_elem = newLangElement(Token::USE_REFERENCE, TypeType::BUILT_IN_TYPE, set_elem->name(), set_elem->type() );
		
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

		//LangElement* lang_elem = newLangElement(Token::DEFINE_VECTOR, set_name, set_type);
		LangElement* lang_elem = newLangElement(Token::DEFINE_REFERENCE, TypeType::VECTOR, set_name, "vector");
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

	LangElement* newDefineFuncReturn(string set_type, string set_name = "")
	{
		LangElement* lang_elem = newLangElement(Token::DEFINE_FUNC_RETURN, TypeType::UNDEFINED, set_name, set_type);
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

		LangElement* lang_elem = newLangElement(Token::DEFINE_FUNC_ARGUMENT, TypeType::UNDEFINED, set_name, set_type);
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

	LangElement* newDefineReference(TypeType::e set_type_type, Role::e set_role, LangElement* maybe_found_class, string set_type, string set_name = "")
	{
		LangElement* lang_elem = newLangElement(Token::DEFINE_REFERENCE, set_type_type, set_name, set_type);
		//lang_elem->typeType(TypeType::REF);
		lang_elem->role(set_role);
		currentReference = lang_elem;

		if(maybe_found_class)
		{
			lang_elem->definitionElement(maybe_found_class);
		}
		/*
		if( lang_elem->parent() )
		{
			if( set_type_type != TypeType::VAL )
			{
				//HERE'S THE OLD OWN SYSTEM, THAT works with lang_eleme.freeOwned()
				//If we ever happen to need it again. It is usefull with pointer new and delete
				//but now that we're using shared_ptr, it's not so usefull, because we're just
				//values now.
				lang_elem->parent()->own(lang_elem);
			}
		}
		*/
		if( lang_elem->parentToken() == Token::CLASS )
		{
			//we are inside a class definition, not a func:
			/////////lang_elem->token(Token::DEFINE_REFERENCE_IN_CLASS);

			if( set_type_type == TypeType::REF )
			{
				listOfAutoInitObjects.push_back(lang_elem);
				//TODO VAL and OPT?
			}
		}

		addToUserDefinedTokens(lang_elem);

		#ifdef DEBUG_RAE_HUMAN
			cout<<"newDefineReference: type: "<<set_type<<" name: "<<lang_elem->namespaceString()<<"<\n";
			//rae::log("newDefineReference: ",set_type," ",set_name,"\n");
		#endif

		return lang_elem;
	}

	LangElement* newUseReference(LangElement* set_definition_elem)
	{
		if( set_definition_elem == 0 )
			return 0;

		LangElement* lang_elem = newLangElement(Token::USE_REFERENCE, set_definition_elem->typeType(), set_definition_elem->name(), set_definition_elem->type() );
		
		lang_elem->definitionElement(set_definition_elem);

		#ifdef DEBUG_RAE_HUMAN
			cout<<"newUseReference of definition: "<<set_definition_elem->toString()<<"\n";
			//rae::log("newUseReference: ",set_elem->toString(),"\n");
		#endif

		return lang_elem;
	}


	LangElement* newFuncCall(LangElement* set_definition_elem)
	{
		if( set_definition_elem == 0 )
			return 0;

		LangElement* lang_elem = newLangElement(Token::FUNC_CALL, set_definition_elem->typeType(), set_definition_elem->name(), set_definition_elem->type() );
		
		lang_elem->definitionElement(set_definition_elem);

		#ifdef DEBUG_RAE_HUMAN
			cout<<"newFuncCall of definition: "<<set_definition_elem->toString()<<"\n";
			//rae::log("newUseReference: ",set_elem->toString(),"\n");
		#endif

		return lang_elem;	
	}

/*
	LangElement* newUnknown(string set_token)
	{
		LangElement* lang_elem = newLangElement(Token::UNKNOWN, set_token );
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
		//LangElement* lang_elem = newLangElement(Token::UNKNOWN_DEFINITION, set_token );
		LangElement* lang_elem = newLangElement(Token::DEFINE_REFERENCE, TypeType::UNDEFINED, set_token );
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
		//LangElement* lang_elem = newLangElement(Token::UNKNOWN_USE_REFERENCE, set_token );
		LangElement* lang_elem = newLangElement(Token::USE_REFERENCE, TypeType::UNDEFINED, set_token );
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
		//LangElement* lang_elem = newLangElement(Token::UNKNOWN_USE_MEMBER, set_token );
		LangElement* lang_elem = newLangElement(Token::USE_MEMBER, TypeType::UNDEFINED, set_token );
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
		//LangElement* lang_elem = newLangElement(Token::USE_ARRAY, set_elem->name(), set_elem->type() );
		LangElement* lang_elem = newLangElement(Token::USE_REFERENCE, TypeType::C_ARRAY, set_elem->name(), set_elem->type() );
		//lang_elem->typeType(TypeType::C_ARRAY);

		lang_elem->definitionElement(set_elem);
		
		ReportError::reportError("Don't use C_ARRAYs with this syntax at the moment.");

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
		//LangElement* lang_elem = newLangElement(Token::USE_VECTOR, set_elem->name(), set_elem->type() );
		LangElement* lang_elem = newLangElement(Token::USE_REFERENCE, TypeType::VECTOR, set_elem->name(), set_elem->type() );
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
		return newLangElement(Token::NUMBER, TypeType::UNDEFINED, set_name);	
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

	int currentLineNumber()
	{
		return lineNumber.line;
	}

	string namespaceString()
	{
		if( previousElement() )
		{
			return previousElement()->namespaceString();
		}
		//we most likely have a previousElement all the time, so the rest of these cases are just in case.
		else if( currentParentElement() )
		{
			return currentParentElement()->namespaceString();
		}
		else if( currentFunc )
		{
			return currentFunc->namespaceString();
		}
		else if( currentClass )
		{
			return currentClass->namespaceString();
		}
		else if( currentModule )
		{
			return currentModule->namespaceString();
		}
		//else
		return "namespace is not yet defined.";
	}

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
	public: Token::e previousToken()
	{
		if(m_previousElement == 0)
			return Token::UNDEFINED;
		return m_previousElement->token();
	}
	public: Token::e previous2ndToken()
	{
		if(m_previousElement && m_previousElement->previousElement())
			return m_previousElement->previousElement()->token();
		//else
		return Token::UNDEFINED;
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
			if( elem->token() == Token::MODULE_NAME )
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
	

	#include "RaeStdLib.hpp"

	
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

		if( ReportError::countWarnings() > 0)
		cout<<"\nNumber of warnings from stdlib: "<<ReportError::countWarnings()<<"\n\n";

		if( ReportError::countErrors() > 0)
			cout<<"\nNumber of errors from stdlib: "<<ReportError::countErrors()<<"\n\n";
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
		cout<<"\n";
		rlutil::setColor(rlutil::BROWN);
		cout<<"Parsing: ";
		rlutil::setColor(rlutil::WHITE);
		cout<<currentFilenamePath.string()<<"\n";

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
	}
	
	void write(string folder_path_to_write_to)
	{
		/*
		//if( fileParsedOk == false )
		if( sourceFiles.empty() )
		{
			//cout<<
			ReportError::reportError("write() None of the source files were parsed correctly. Can't write anything.");
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
			if( module_elem->token() != Token::MODULE )
			{
				#ifdef DEBUG_RAE_HUMAN
				cout<<"Interesting: we have something in top hierarchy: "<<module_elem->tokenString()<<"\n";
				//rae::log("Interesting: we have something in top hierarchy: ", module_elem->tokenString(), "\n");
				#endif
				continue; //skip it
			}


			boost::filesystem::path module_filename;

			string module_name; 

			foreach( LangElement* elem, module_elem->langElements )
			{
				if( elem->token() == Token::MODULE_NAME )
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
			rlutil::setColor(rlutil::BROWN);
			cout<<"Writing C++: ";
			rlutil::setColor(rlutil::WHITE);
			cout<<module_filename.string()<<" .hpp and .cpp\n";
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

			if( ReportError::countWarnings() > 0)
			{
				cout<<"\nNumber of ";
				rlutil::setColor(rlutil::CYAN);
				cout<<"warnings: ";
				rlutil::setColor(rlutil::WHITE);
				cout<<ReportError::countWarnings()<<"\n\n";
			}

			if( ReportError::countErrors() > 0)
			{
				cout<<"\nNumber of ";
				rlutil::setColor(rlutil::BLUE);
				cout<<"errors: ";
				rlutil::setColor(rlutil::WHITE);
				cout<<ReportError::countErrors()<<"\n\n";
			}

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
			
			
			if( isPassthroughMode == true || isPassthroughSourceMode == true )
			{
				if( isWaitingForPragmaToken == true )
				{
					//enter or space (or slash for comments) to end pragma.
					if( currentChar == '\n' || currentChar == ' ' || currentChar == '/' )
					{
						isWaitingForPragmaToken = false;
						wholeToken = currentWord;
						isWholeToken = true;

						isWaitingForPragmaToken = false;

						currentWord = "";
						currentLine += currentChar;

						if( currentChar == '\n' )
						{
							isWholeToken2 = true;
							wholeToken2 = "\n";
						}
					}
					else
					{
						//cout<<"waiting for end pragma end.";
						currentWord += currentChar;
						currentLine += currentChar;
					}

					return true;//would get called anyway, but here for "clarity"
				}
				else
				{
					//In passthroughmode we try to separate them as lines. But instead
					//using currentLine, we're using currentWord, as I'm not sure how well
					//we are currently supporting currentLine.
					if( currentChar == '\n')
					{
						wholeToken = currentWord;
						wholeToken += currentChar;
						currentWord = "";
						currentLine = "";
						isWholeToken = true;

						//cout<<"PASSTHROUGH: "<<wholeToken;
					}
					else if( currentChar == '@' )
					{
						wholeToken = currentWord;
						isWholeToken = true;

						//cout<<"Got @. Starting to wait for a @end pragma.\n";

						isWaitingForPragmaToken = true;

						currentWord = "@";
						currentLine += currentChar;
					}
					else
					{
						currentWord += currentChar;
						currentLine += currentChar;
					}
				}			

				return true;
			}

			//list of separators:

			//handle quotes:

			if( nroWaitingForPlusCommentEnd == 0 && isWaitingForStarCommentEnd == false && isSingleLineComment == false )//we don't want quotes to mess with comments.
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

			//if( expectingToken() == Token::COMMENT_LINE_END )
			if( isSingleLineComment == true )
			{
				//cout<<"Waiting comment end...\n";
			
				//if( set_token == "\n" )
				if( currentChar == '\n' )
				{
					//cout<<"...hurray! Comment finally ended.\n";
				
					/*if( isEndOfLine )
					{
						cout<<"We has isEndOfLine...:>"<<endOfLine<<"<\n";
					}
					else cout<<"We don't have isEndOfLine...:>"<<endOfLine<<"<\n";
					*/
					//newComment(endOfLine);
					newComment(currentComment);
					currentComment = "";
					newLine();
					//expectingToken = Token::UNDEFINED;
					isSingleLineComment = false;
					isEndOfLine = false;
				}
				else
				{
					currentComment += currentChar;
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
				else if(isWaitingForPragmaToken == false)
				//a very special case for @c++ so that the plusses don't get
				//passed through as tokens.
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
					//Comment time until newline.
					/*
					wholeToken = currentWord;
					isWholeToken = true;//end token 1.
					wholeToken2 = "//";//put // to token 2.
					isWholeToken2 = true;
					currentWord = "";
					*/
					//handleSlash = "";

					//isWaitingForCommentEnd = true;

					wholeToken = "//";//put // to token 1.
					isWholeToken = true;
					currentWord = "";

					isSingleLineComment = true;

					currentComment = "//";

					return true;
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

			if(isWaitingForPragmaToken == true)
			{
				//space or enter (or slash for comments) ends a pragma token.
				if( currentChar == ' ' || currentChar == '\n' || currentChar == '/' )
				{
					#ifdef DEBUG_RAE_HUMAN
						cout<<"End @ pragma: "<<currentWord<<"\n";
					#endif

					wholeToken = currentWord;
					isWholeToken = true;

					isWaitingForPragmaToken = false;

					currentWord = "";
					currentLine += currentChar;

					if( currentChar == '\n' )
					{
						isWholeToken2 = true;
						wholeToken2 = "\n";
					}
				}
				else
				{
					#ifdef DEBUG_RAE_PARSER
						cout<<"waiting for pragma end.";
					#endif
					currentWord += currentChar;
					currentLine += currentChar;
				}

				return true;
			}

			//handle other stuff:

			//The minus sign is handled in a deferred way.
			//We wait for -> and if we don't get it we,
			//got "-".
			if( handleSlash == "-" )
			{
				if(currentChar == '>')
				{
					//We got ->

					wholeToken = "->";//"-"
					isWholeToken = true;
					
					wholeToken2 = "";
					isWholeToken2 = false;
					currentWord = "";
					
					currentLine += currentChar;

					return true;
				}
				else
				{
					wholeToken = currentWord;//"-"
					isWholeToken = true;
				}
			}
			
			//The normal way to handle minus:
			/*
			if( currentChar == '-' )
			{
				//cout<<"early We got -\n";
				
				wholeToken = currentWord;
				isWholeToken = true;
				
				wholeToken2 = "-";
				isWholeToken2 = true;
				currentWord = "";
				
				currentLine += currentChar;
				
			}*/
			//The abnormal way to handle minus:
			if( currentChar == '-' )
			{
				//Don't do anything. our - will be in the poorly named
				//handleSlash, which is really pastLetters...

				//OK. We'll do something. Cut the incoming word and send it:
				wholeToken = currentWord;
				isWholeToken = true;

				currentWord = "-"; //Oh, we'll put it here too...
				currentLine += currentChar;//We'll still put the minus here...
			}
			else if( currentChar == '@' )
			{
				wholeToken = currentWord;
				isWholeToken = true;

				#ifdef DEBUG_RAE_HUMAN
					cout<<"Got @. Starting to wait for a pragma.\n";
				#endif

				isWaitingForPragmaToken = true;

				currentWord = "@";
				currentLine += currentChar;
			}
			else if( currentChar == '.' )
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

	

	void parseIdle()
	{				
		//while( EOF != (currentChar = getc(currentFile)) )
		//if( expectingToken() == Token::UNDEFINED )
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
							expectingToken = Token::CLASS_NAME;
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
							expectingToken = Token::FUNC_DEFINITION;
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
							expectingToken = Token::COMMENT;
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
		else if( expectingToken() == Token::CLASS_NAME )
		{
			if( readChar() && isWholeToken )
			{
				isWholeToken = false;
				newClass(wholeToken);
				//wholeToken = "";
				expectingToken = Token::UNDEFINED;
			}
		}
		else if( expectingToken() == Token::COMMENT )
		{
			if( readChar() && isEndOfLine )
			{
				isEndOfLine = false;
				newComment(endOfLine);
				expectingToken = Token::UNDEFINED;
			}
		}
		else if( expectingToken() == Token::FUNC_DEFINITION )
		{
			if( readChar() && isEndOfLine )
			{
				isEndOfLine = false;
				
				if( endOfLine != "" )
				{
					cout<<"nu09817240918274091782 func: "<<endOfLine<<"\n";
					//newClass(wholeToken);
					//wholeToken = "";
					expectingToken = Token::UNDEFINED;
				}
				else cout<<"waiting real func on next line.\n";
	*/		
				/*if( wholeToken[0] != '(' )
				{
					cout<<"nu12938719238719273 func: "<<wholeToken<<"\n";
					//newClass(wholeToken);
					//wholeToken = "";
					expectingToken = Token::UNDEFINED;
				}*/
		/*	}
		}
		else
		{
			cout<<"Unhandled expecting Token:"<<Token::toString(expectingToken)<<"\n";
			expectingToken = Token::UNDEFINED;
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

		//We catch all tokens when doing PRAGMA_CPP until we get a website address
		//such as: forsaken_woods@endisnear.com
		//Oh. @Pragmas need to end with space or enter, so that might handle that case.
		if( expectingToken() == Token::PRAGMA_CPP )
		{
			if(set_token == "@end")
			{
				isPassthroughMode = false;
				newLangElement(Token::PRAGMA_CPP_END, TypeType::UNDEFINED, set_token);
				doReturnToExpectToken();
			}
			else
			{
				newLangElement(Token::PASSTHROUGH, TypeType::UNDEFINED, set_token);
			}
			return;
		}
		else if( expectingToken() == Token::PRAGMA_CPP_SRC )
		{
			if(set_token == "@end")
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

		if( set_token == "\t" )
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
			//expectingToken = Token::COMMENT_LINE_END;
			isSingleLineComment = true;
			currentLine = "//";
			//cout<<"Waiting for comment line to end.\n";
			return;
		}
	
		//

		if( set_token == "\"")
		{
			if( isQuoteReady == true )
			{
				isQuoteReady = false;

				if( expectingToken() == Token::INIT_DATA )
				{
					if( currentReference )
					{
						#ifdef DEBUG_RAE_HUMAN
						cout<<"string init_data: "<<currentQuote<<" Special handling for newQuote. INIT_DATA.\n";
						#endif
						//special handling for quote i.e. string initData.
						LangElement* in_dat = newLangElement(Token::INIT_DATA, TypeType::UNDEFINED, currentQuote);
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
		
		if( expectingToken() == Token::INIT_DATA )
		{
			if( set_token == "=" )
			{
				//create a init_data element and continue waiting for the actual data!
				LangElement* in_dat = newLangElement(Token::INIT_DATA, TypeType::UNDEFINED, set_token);
				currentReference->initData(in_dat);

				isReceivingInitData = true;

				expectingToken( Token::ACTUAL_INIT_DATA );

				#ifdef DEBUG_RAE_HUMAN
					cout<<"= starting to receive init_data for "<<currentReference->type()<<" "<<currentReference->namespaceString()<<"\n";
				#endif

			}
			else if( set_token == "\n" )
			{
				//No initdata.
				doReturnToExpectToken();
				newLine();				
			}
			else if(set_token == ")")
			{
				if(expectingRole() == Role::FUNC_RETURN )
				{
					#ifdef DEBUG_RAE_PARSER
					cout<<"Got ending ) for FUNC_RETURNs (in INIT_DATA). Going back to FUNC_DEFINITION.\n";
					#endif

					expectingRole(Role::UNDEFINED);//could also be Role::FUNC_DEFINITION, but we don't need that, so.
					//TODO: check parenthesisStack:
					newParenthesisEnd(Token::PARENTHESIS_END_FUNC_RETURN_TYPES, ")");
					expectingToken(Token::FUNC_DEFINITION);
				}
				else if( expectingRole() == Role::FUNC_PARAMETER )
				{
					#ifdef DEBUG_RAE_PARSER
					cout<<"Got ending ) for FUNC_PARAMETERs in INIT_DATA. Going back to UNDEFINED.\n";
					#endif

					expectingRole(Role::UNDEFINED);//could also be Role::FUNC_DEFINITION, but we don't need that, so.
					//TODO: check parenthesisStack:
					newParenthesisEnd(Token::PARENTHESIS_END_FUNC_PARAM_TYPES, ")");
					expectingToken(Token::UNDEFINED);
				}
				else
				{
					ReportError::reportError(") parenthesis_end - can't be initdata for built in type.\n", previousElement() );
				}
			}
			else if( set_token == "," )
			{
				if( expectingRole() == Role::FUNC_RETURN || expectingRole() == Role::FUNC_PARAMETER )
				{
					//No initdata.
					
					newLangElement(Token::COMMA, TypeType::UNDEFINED, ",");
					//expectingToken(Token::UNDEFINED);
					doReturnToExpectToken();
				}
				else
				{
					ReportError::reportError("unexpected , COMMA while waiting for initdata for built in type.\n", previousElement() );	
				}
			}
		}
		else if( expectingToken() == Token::ACTUAL_INIT_DATA )
		{
			//we should make a expectingToken() == Token::ACTUAL_INIT_DATA that we wait after we get 
			//the =. And also to handle //comments /**/ as not being init_data, or can they be initDATA? or passed with it?

			
			if( set_token == "." )
			{
				if( previousElement() && previousToken() == Token::NUMBER )
				{
					//newLangElement(Token::DOT, set_token);
					previousElement()->name( previousElement()->name() + set_token );
					expectingToken(Token::NUMBER_AFTER_DOT);
					//expectingRole(Role::INIT_DATA);

				}
				else
				{
					ReportError::reportError("A strange dot while waiting for INIT_DATA. Floating point numbers are written: 0.0", previousElement() );
					newLangElement(Token::REFERENCE_DOT, TypeType::UNDEFINED, set_token);
				}
			}
			else if( set_token == "\n" )
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

				if(currentParentElement() && currentParentElement()->token() == Token::INIT_DATA)
				{
					endInitData();//This is important. This ends our init_data being the currentParentElement (which receives the init_data.)
				}
				else
				{
					ReportError::reportError("Waiting for INIT_DATA: Got NEWLINE but there was no INIT_DATA element.", previousElement() );
					//TODO ReportError::additionalInfo("currentParentElement: ", currentParentElement() );
					cout<<"currentParentElement: ";
					if(currentParentElement()) cout<<currentParentElement()->toString()<<"\n"; else cout<<"nullptr"<<"\n";
				}	

				doReturnToExpectToken();
				newLine();				
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
					//WHY does this set the token of an already existing langElement?????????? TODO
					currentReference->token(Token::INFO_FUNC_PARAM);
				}
				//expectingToken = Token::UNDEFINED;
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
				//expectingToken = Token::UNDEFINED; //TEMP
				doReturnToExpectToken();
			}
			else if( set_token == "null" )
			{
				if(currentParentElement() && currentParentElement()->token() == Token::INIT_DATA)
				{
					LangElement* in_dat = newLangElement(Token::RAE_NULL, TypeType::UNDEFINED, set_token);
				}
				else
				{
					ReportError::reportError("Got null as init_data, but there was no = before the init_data.", previousElement() );
				}
				
				#ifdef DEBUG_RAE_HUMAN
					cout<<"Set null as init_data for "<<currentReference->type()<<" "<<currentReference->namespaceString()<<"\n";
				#endif
			}
			else
			{
				if(currentParentElement() && currentParentElement()->token() == Token::INIT_DATA)
				{
					if( handleNumber(set_token) )
					{
						//don't do anything. We've done it already!
					}
					else
					{
						//we should have a handleInitData() func!
						//newLangElement(Token::STRING, TypeType::UNDEFINED, set_token); //Hmm STRINGs like "a string" are Token::QUOTES.
						cout<<"TODO better handling of init_data in initialization. "<<set_token<<"\n";
					}
				}
				else
				{
					ReportError::reportError("Got null as init_data, but there was no = before the init_data.", previousElement() );
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
					//cout<<"ERROR: "<<lineNumber.line<<" was expecting an INIT_DATA. It should be a number or new.\n";
					ReportError::reportError("expecting an INIT_DATA. It should be a number or new.", previousElement() );
				}*/
				//expectingToken = Token::UNDEFINED;
				
				//doReturnToExpectToken();
			}
		}
		else if( expectingToken() == Token::IMPORT_NAME )
		{
			if( set_token[0] == '.' )
			{
				expectingToken( Token::IMPORT_NAME );
			}
			else if( set_token == "\n" )
			{
				#ifdef DEBUG_RAE
					cout<<"Line change. End of IMPORT_NAME.\n";
					//rae::log("Line change. End of IMPORT_NAME.\n");
				#endif
				if( currentTempElement )
				{
					//currentModuleHeaderFileName = currentTempElement->searchLast(Token::IMPORT_NAME)->name();
					#ifdef DEBUG_RAE
					//	cout<<"last module name was: "<<currentModuleHeaderFileName;
					#endif
					
					/*
					string a_import_whole_name = "";

					int not_on_first = 0;

					foreach(LangElement* elem, currentTempElement->langElements)
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
				//expectingToken = Token::UNDEFINED;
				doReturnToExpectToken();
				currentTempElement = 0;
			}
			else
			{
				if( currentTempElement )
					currentTempElement->newLangElement(lineNumber, Token::IMPORT_NAME, TypeType::UNDEFINED, set_token);

				expectingToken(Token::IMPORT_NAME);
			}
		}
		else if( expectingToken() == Token::MODULE_NAME )
		{
			if( set_token[0] == '.' )
			{
				//cout<<"Got func_def (. Waiting return_types.\n";
				
				//expectingToken(Token::MODULE_NAME);
			}
			else if( set_token == "\n" )
			{
				#ifdef DEBUG_RAE
					cout<<"Line change. End of MODULE_NAME.\n";
					//rae::log("Line change. End of MODULE_NAME.\n");
				#endif
				if( currentModule )
				{
					currentModuleHeaderFileName = currentModule->searchLast(Token::MODULE_NAME)->name();
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
				//expectingToken(Token::UNDEFINED);
				doReturnToExpectToken();
			}
			else
			{
				if( currentModule )
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
		else if( expectingToken() == Token::PARENTHESIS_BEGIN_LOG )
		{
			if( set_token == "(" )
			{
				newParenthesisBegin(Token::PARENTHESIS_BEGIN_LOG, set_token);
			}
			else
			{
				//cout<<"ERROR: ";
				//rae::log("ERROR: ");
				//lineNumber.printOut();
				//cout<<" calling log(). No parenthesis after log.";
				//rae::log(" calling log(). No parenthesis after log.");

				ReportError::reportError("calling log(). No parenthesis after log.", previousElement() );
			}

			//expectingToken = Token::UNDEFINED;
			doReturnToExpectToken();
		}
		else if( expectingToken() == Token::PARENTHESIS_BEGIN_LOG_S )
		{
			if( set_token == "(" )
			{
				newParenthesisBegin(Token::PARENTHESIS_BEGIN_LOG_S, set_token);
			}
			else
			{
				//cout<<"ERROR: ";
				//"ERROR: "
				//lineNumber.printOut();
				//cout<<" calling log_s(). No parenthesis after log_s.";
				
				ReportError::reportError(" calling log_s(). No parenthesis after log_s.", previousElement() );
			}

			//expectingToken = Token::UNDEFINED;
			doReturnToExpectToken();
		}
		else if( expectingToken() == Token::NUMBER_AFTER_DOT )
		{
			if( previousElement() && previousToken() == Token::NUMBER )//the previousElement() check is needed because we use it next.
			{
				if( isNumericChar(set_token[0]) )
				{	
					previousElement()->name( previousElement()->name() + set_token );
					//expectingToken = Token::UNDEFINED;
					//doReturnToExpectToken();
				}
				else
				{
					//cout<<"ERROR: "<<lineNumber.line<<" float number messed up, after dot.";
					ReportError::reportError(" float number messed up, after dot.", previousElement());
				}

				if( isReceivingInitData == true )
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
		else if( expectingToken() == Token::ARRAY_VECTOR_STUFF )
		{
			if( set_token == "[" )
			{
				//Shouldn't happen.
				ReportError::reportError("Duplicate [. Compiler TODO.", previousElement());
				//expectingToken(Token::VECTOR_STUFF);
			}
			else if( set_token == "]" )
			{
				if(previousElement())
				{
					if( previousElement()->token() == Token::DEFINE_REFERENCE )
					{
						#ifdef DEBUG_RAE_PARSER
						cout<<"making the previous DEFINE_REFERENCE_IN_CLASS an array.\n";
						#endif
						/*
						//SomeClass[] someVector
						//----here->------------
						//OLD behaviour turned it into a C_ARRAY, but now it's the preferred way to use vectors.
						previousElement()->typeType(TypeType::C_ARRAY);
						expectingToken(Token::DEFINE_C_ARRAY_NAME);
						*/
						previousElement()->typeType(TypeType::VECTOR);
						previousElement()->createTemplateSecondType( previousElement()->type() );
						previousElement()->type("vector");
						expectingToken(Token::VECTOR_NAME);
					}
					else
					{
						ReportError::reportError("An ending array bracket ] in a strange place. The element before is not a type. It is: " + previousElement()->toString(), previousElement() );
					}
				}
			}
			else
			{
				//A static array. TODO.
				ReportError::reportError("Maybe a static array. TODO.", previousElement());
			}
		}
		else if( expectingToken() == Token::VECTOR_STUFF )
		{
			if( set_token == "!" )
			{
				expectingToken(Token::VECTOR_STUFF);
			}
			else if( set_token == "(" )
			{
				expectingToken(Token::VECTOR_STUFF);
			}
			else if( set_token == ")" )
			{
				expectingToken(Token::VECTOR_NAME);
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
				expectingToken(Token::VECTOR_STUFF);
			}
		}
		else if( expectingToken() == Token::VECTOR_NAME )
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
			//expectingToken = Token::UNDEFINED;
			doReturnToExpectToken();			
		}
		else if( expectingToken() == Token::CLASS_TEMPLATE_SECOND_TYPE )
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
					//currentClass->newLangElement(lineNumber, Token::TEMPLATE_SECOND_TYPE, TypeType::TEMPLATE/*are you sure this is not confusing...*/, /*string set_name:*/ "undefined", /*string set_type:*/ set_token );
					currentClass->createTemplateSecondType(set_token);
				}
				//expectingToken(Token::TEMPLATE_STUFF);
			}
		}
		else if( expectingToken() == Token::TEMPLATE_STUFF )
		{
			//if( set_token == "!" )//No, we won't get this on templates... We already got it.
			//{
				//expectingToken(Token::TEMPLATE_STUFF);
			//}
			//else
			if( set_token == "(" )
			{
				//just eat this token.
				//expectingToken(Token::TEMPLATE_STUFF);
			}
			else if( set_token == ")" )
			{
				expectingToken(Token::TEMPLATE_NAME);
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
					//currentReference->newLangElement(lineNumber, Token::TEMPLATE_SECOND_TYPE, TypeType::TEMPLATE/*are you sure this is not confusing...*/, /*string set_name:*/ "undefined", /*string set_type:*/ set_token );
						currentReference->createTemplateSecondType( set_token );
				}
				//expectingToken(Token::TEMPLATE_STUFF);
			}
		}
		else if( expectingToken() == Token::TEMPLATE_NAME )
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
			//expectingToken = Token::UNDEFINED;
			doReturnToExpectToken();			
		}
		else if( expectingToken() == Token::UNDEFINED )
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


			if( set_token == "\n" )
			{
					newLine();
			}
			else if( set_token == ";" )
			{
				newLangElement(Token::SEMICOLON, TypeType::UNDEFINED, set_token);
			}
			else if( set_token == "{" )
			{
				newScopeBegin();
			}
			else if( set_token == "}" )
			{
				newScopeEnd();
			}
			else if( set_token == "func" )
			{
				#ifdef DEBUG_RAE
				cout<<"Got func. Waiting func_definition.\n";
				//rae::log("Got func. Waiting func_definition.\n");
				#endif
				expectingToken(Token::FUNC_DEFINITION);
				newFunc();
			}
			else if( set_token == "." )
			{
				if( previousElement() && previousToken() == Token::NUMBER )
				{
					//newLangElement(Token::DOT, set_token);
					previousElement()->name( previousElement()->name() + set_token );
					expectingToken(Token::NUMBER_AFTER_DOT);
				}
				else
				{
					newLangElement(Token::REFERENCE_DOT, TypeType::UNDEFINED, set_token);
				}
			}
			else if( set_token == "," )
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
				
				if( expectingRole() == Role::FUNC_RETURN )
				{
					//TODO: Hmm. These are the same as normal case. So we could just remove the expectingRoles here...
					newLangElement(Token::COMMA, TypeType::UNDEFINED, ",");
				}
				else if( expectingRole() == Role::FUNC_PARAMETER )
				{
					newLangElement(Token::COMMA, TypeType::UNDEFINED, ",");
				}
				else //Role::UNDEFINED or something. Normal function body etc.
				{
					Token::e parenthesis_type = parenthesisStackTokenType();

					if( parenthesis_type == Token::PARENTHESIS_BEGIN_LOG || parenthesis_type == Token::PARENTHESIS_BEGIN_LOG_S )
					{
						newLangElement(Token::LOG_SEPARATOR, TypeType::UNDEFINED, set_token);//it is still a comma "," but we write it out as << for C++
					}
					else
					{
						newLangElement(Token::COMMA, TypeType::UNDEFINED, set_token);
					}
				}
			}
			else if( set_token == "val" )
			{
				expectingToken(Token::DEFINE_REFERENCE);
				expectingTypeType(TypeType::VAL);
			}
			else if( set_token == "ref" )
			{
				expectingToken(Token::DEFINE_REFERENCE);
				expectingTypeType(TypeType::REF);
			}
			else if( set_token == "opt" )
			{
				expectingToken(Token::DEFINE_REFERENCE);
				expectingTypeType(TypeType::OPT);
			}
			else if( set_token == "link" )
			{
				expectingToken(Token::DEFINE_REFERENCE);
				expectingTypeType(TypeType::LINK);
			}
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
			/*else if( set_token == "real" )
			{
				expectingToken = Token::DEFINE_BUILT_IN_TYPE_NAME;
				newDefineBuiltInType(BuiltInType::REAL, set_token);
			}*/
			else if( set_token == "string" )
			{
				expectingToken(Token::DEFINE_BUILT_IN_TYPE_NAME);
				newDefineBuiltInType(BuiltInType::STRING, expectingRole(), set_token);
			}
			else if( set_token == "vector" )
			{
				expectingToken(Token::VECTOR_STUFF);
				newDefineVector("unknown", "unknown");
			}
			else if( set_token == "(" )
			{
				if( previousToken() == Token::CLASS )
				{
					#ifdef DEBUG_RAE_HUMAN
					cout<<"( means template class.\n";
					#endif
					//a simple class template
					previousElement()->typeType(TypeType::TEMPLATE);
					expectingToken(Token::CLASS_TEMPLATE_SECOND_TYPE);
				}
				else if( expectingRole() == Role::FUNC_PARAMETER )
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
			else if( set_token == ")" )
			{
				if( expectingRole() == Role::FUNC_RETURN )
				{
					#ifdef DEBUG_RAE_PARSER
					cout<<"Got ending ) for FUNC_RETURNs in UNDEFINED. Going back to FUNC_DEFINITION.\n";
					#endif				
	
					expectingRole(Role::UNDEFINED);//could also be Role::FUNC_DEFINITION, but we don't need that, so.
					//TODO: check parenthesisStack:
					newParenthesisEnd(Token::PARENTHESIS_END_FUNC_RETURN_TYPES, ")");
					expectingToken(Token::FUNC_DEFINITION);
				}
				else if( expectingRole() == Role::FUNC_PARAMETER )
				{
					#ifdef DEBUG_RAE_PARSER
					cout<<"Got ending ) for FUNC_PARAMETERs in UNDEFINED. Going back to UNDEFINED.\n";
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
			else if( set_token == "[" )
			{
				//cout<<"BRACKET we got a bracket in the normal place and it is strange!\n";

				cout<<"Wise people say, this should never get called. SourceParser line 3806.\n";
				assert(0);

				if(previousElement())
				{
					if( previousElement()->token() == Token::DEFINE_REFERENCE )
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
						expectingToken(Token::DEFINE_C_ARRAY_NAME);
						*/
						cout<<"A bracket but we didn't have a DEFINE_REFERENCE? Maybe it is an error.\n";
						/*not up to date, as it won't be called...
						previousElement()->typeType(TypeType::VECTOR);
						previousElement()->createTemplateSecondType( previousElement()->type() );
						previousElement()->type("vector");
						expectingToken(Token::VECTOR_NAME);
						*/
					}
					else
					{
						#ifdef DEBUG_RAE_PARSER
						cout<<"normal newBracketBegin.";
						#endif
						newBracketBegin(Token::BRACKET_BEGIN, set_token);
					}
				}
				else ReportError::reportError("a bracket, but no previousElement. This is a compiler error. We'll need to fix it.", previousElement());
				
			}
			else if( set_token == "]" )
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
					cout<<"normal newBracketEnd.";
					#endif
					newBracketEnd(Token::BRACKET_END, set_token);
				//}
			}
			else if( set_token == "->" )
			{
				//newLangElement(Token::POINT_TO, TypeType::UNDEFINED, set_token);
				newPointToElement();
			}
			else if( set_token == "+" )
			{
				newLangElement(Token::PLUS, TypeType::UNDEFINED, set_token);
			}
			else if( set_token == "-" )
			{
				newLangElement(Token::MINUS, TypeType::UNDEFINED, set_token);
			}
			else if( set_token == "=" )
			{
				newLangElement(Token::EQUALS, TypeType::UNDEFINED, set_token);
			}
			else if( set_token == "*" )
			{
				newLangElement(Token::STAR, TypeType::UNDEFINED, set_token);
			}
			else if( set_token == "/" )
			{
				newLangElement(Token::DIVIDE, TypeType::UNDEFINED, set_token);
			}
			else if( set_token == "<" )
			{
				newLangElement(Token::SMALLER_THAN, TypeType::UNDEFINED, set_token);
			}
			else if( set_token == ">" )
			{
				newLangElement(Token::BIGGER_THAN, TypeType::UNDEFINED, set_token);
			}
			else if( set_token == "\"" )
			{
				//shouldn't happen.
				//newLangElement(Token::STAR, set_token);
			}
			else if( set_token == "*/" )
			{
				//newLangElement(Token::DIVIDE, set_token);
			}
			else if( set_token == "/*" )
			{
				//newLangElement(Token::DIVIDE, set_token);
			}
			else if( set_token == "if" )
			{
				newLangElement(Token::IF, TypeType::UNDEFINED, set_token);
			}
			else if( set_token == "for" )
			{
				newLangElement(Token::FOR, TypeType::UNDEFINED, set_token);
			}
			else if( set_token == "foreach" )
			{
				newLangElement(Token::FOREACH, TypeType::UNDEFINED, set_token);
			}
			else if( set_token == "log_s" )
			{
				//isInsideLogStatement = true;
				newLangElement(Token::LOG_S, TypeType::UNDEFINED, set_token);
				expectingToken(Token::PARENTHESIS_BEGIN_LOG_S);

			}
			else if( set_token == "log" )
			{
				//isInsideLogStatement = true;
				newLangElement(Token::LOG, TypeType::UNDEFINED, set_token);
				expectingToken(Token::PARENTHESIS_BEGIN_LOG);
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
			else if( set_token == ":" )
			{
				if( previousElement() )
				{
					if( previousElement()->token() == Token::VISIBILITY )
					{
						//change it.
						previousElement()->token(Token::VISIBILITY_DEFAULT);
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
					//if( previousElement()->token() == Token::DEFINE_REFERENCE )
					//{
					
						//change the typetype.
						previousElement()->typeType(TypeType::TEMPLATE);
					//}
				}
				expectingToken(Token::TEMPLATE_STUFF);
			}
			else if( set_token == "return" )
			{
				#ifdef DEBUG_RAE
				cout<<"Got return.\n";
				//rae::log("Got return.\n");
				#endif
				//expectingToken = Token::PARENTHESIS_BEGIN_RETURN;
				newLangElement(Token::RETURN, TypeType::UNDEFINED, set_token);
			}
			else if( set_token == "new" )
			{
				cout<<"TODO Got new. Waiting new_class.\n";
				//rae::log("TODO Got new. Waiting new_class.\n");
			//expectingToken = Token::FUNC_DEFINITION;
			//newFunc();
			}
			else if( set_token == "free" )
			{
				//cout<<"TODO Got free. Waiting free_class.\n";
				////rae::log("TODO Got free. Waiting free_class.\n");
				expectingToken(Token::FREE_NAME);
				//newFunc();
				if( previousElement() )
				{
					if( previousElement()->token() == Token::REFERENCE_DOT )
					{
						if( previous2ndElement() && (previous2ndElement()->token() == Token::USE_REFERENCE )
							//|| previous2ndElement->token() == Token::UNKNOWN_USE_REFERENCE)
						   )
						{
							//hide the REFERENCE_DOT from something.free
							previousElement()->token(Token::UNDEFINED);
							previousElement()->name( "" );

							previous2ndElement()->token(Token::FREE);

							//expectingToken(Token::UNDEFINED);
							doReturnToExpectToken();
						}
					}	
				}
			}
			else if( set_token == "delete" )
			{
				cout<<"TODO Got delete. there's no delete keyword in Rae. Use free instead.\n";
				//rae::log("TODO Got delete. Waiting delete_class.\n");
			//expectingToken = Token::FUNC_DEFINITION;
			//newFunc();
			}
			else if( set_token == "override" )
			{
				newLangElement(Token::OVERRIDE, TypeType::UNDEFINED, set_token);
			}
			else if( set_token == "class" )
			{
			////rae::log("found WORD class:>", set_token, "<\n");
				expectingToken(Token::CLASS_NAME);
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
			//try to automatically figure out where to put this c++ code. Usually it goes to headers. Inside funcs it goes to cpp files.
			else if( set_token == "@c++" )
			{
				isPassthroughMode = true;
				//cout<<"To PASSTHROUGH mode.\n";
				newLangElement(Token::PRAGMA_CPP, TypeType::UNDEFINED, set_token);
				expectingToken(Token::PRAGMA_CPP);
			}
			//copy to c++ header .hpp:
			else if( set_token == "@c++hpp" )
			{
				isPassthroughMode = true;
				//cout<<"To PASSTHROUGH mode.\n";
				newLangElement(Token::PRAGMA_CPP_HDR, TypeType::UNDEFINED, set_token);
				expectingToken(Token::PRAGMA_CPP_HDR);
			}
			//copy to c++ source file .cpp:
			else if( set_token == "@c++cpp" )
			{
				isPassthroughSourceMode = true;
				//cout<<"To PASSTHROUGH mode.\n";
				newLangElement(Token::PRAGMA_CPP_SRC, TypeType::UNDEFINED, set_token);
				expectingToken(Token::PRAGMA_CPP_SRC);
			}
			else if( set_token == "@asm" )
			{
				ReportError::reportError("@asm is reserved for assembler, but it is not yet implemented.", currentLineNumber(), namespaceString() );
			}
			else if( set_token == "@ecma" )
			{
				ReportError::reportError("@ecma is reserved for ecmascript, but it is not yet implemented.", currentLineNumber(), namespaceString() );
			}
			else if( set_token == "@javascript" )
			{
				ReportError::reportError("@javascript is reserved, but use @ecma instead, but it is not yet implemented.", currentLineNumber(), namespaceString() );
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
				newLangElement(Token::EQUALS, set_token);
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
		else if( expectingToken() == Token::FREE_NAME )
		{
			//TODO check if name is a valid name...
			newLangElement(Token::FREE, TypeType::UNDEFINED, set_token);
			//expectingToken = Token::UNDEFINED;
			doReturnToExpectToken();
		}
		else if( expectingToken() == Token::DEFINE_BUILT_IN_TYPE_NAME )
		{
			if(set_token == ")")
			{
				if(expectingRole() == Role::FUNC_RETURN )
				{
					#ifdef DEBUG_RAE_PARSER
					cout<<"Got ending ) for FUNC_RETURNs in DEFINE_BUILT_IN_TYPE_NAME. Going back to FUNC_DEFINITION.\n";
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
			else if( currentReference )
			{
				setNameAndCheckForPreviousDefinitions(currentReference, set_token);

				/*if( checkIfNewDefinedNameIsValid(currentReference, set_token) )
				{
					currentReference->name(set_token);
				}
				else
				{
					ReportError::reportError("redefinition of name: " + set_token);
				}*/
			
				//And now we'll see what to do next:
				if(expectingRole() == Role::FUNC_RETURN )
				{
					//Back to waiting for UNDEFINED as that handles the coming ")" the best.
					expectingToken(Token::UNDEFINED);
				}
				else
				{
					//in the normal function body case (which is still Role::UNDEFINED
					//because we haven't yet needed e.g. Role::FUNCTION_BODY for anything)
					//we wait for initdata if there is some:
					expectingToken(Token::INIT_DATA);
				}
			}
		}
		else if( expectingToken() == Token::DEFINE_REFERENCE )
		{
			handleUserDefinedToken(set_token);
		}
		else if( expectingToken() == Token::DEFINE_REFERENCE_NAME )
		{
			if(set_token == ")")
			{
				if(expectingRole() == Role::FUNC_RETURN )
				{
					#ifdef DEBUG_RAE_PARSER
					cout<<"Got ending ) for FUNC_RETURNs in DEFINE_REFERENCE_NAME. Going back to FUNC_DEFINITION.\n";
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
			else if( set_token == "[")
			{
				#ifdef DEBUG_RAE_PARSER
				cout<<"BRACKET we got a bracket in DEFINE_REFERENCE_NAME!\n";
				#endif

				if(previousElement())
				{
					if( previousElement()->token() == Token::DEFINE_REFERENCE )
					{
						expectingToken(Token::ARRAY_VECTOR_STUFF);
					}
					else
					{
						ReportError::reportError("A starting array bracket [ in a strange place. The element before is not a type. It is: " + previousElement()->toString(), previousElement() );
					}
				}
			}
			else if( set_token == "]")
			{
				//newBracketEnd(Token::BRACKET_DEFINE_ARRAY_END, set_token);
			}
			else if( set_token == "*" )
			{
				//TODO pointer type...
				ReportError::reportError("TODO pointer type.", previousElement());
				newLangElement(Token::STAR, TypeType::UNDEFINED, set_token);
			}
			else if( set_token == "?" )
			{
				//A huge TODO.
				ReportError::reportError("Huge todo on line 4535. handle null pointers: ????.", previousElement());
				//newLangElement(Token::STAR, set_token);
			}
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
			else if( currentReference )
			{
				//currentReference->name(set_token);
				setNameAndCheckForPreviousDefinitions(currentReference, set_token);
			
				if(expectingRole() == Role::FUNC_RETURN )
				{
					//Back to waiting for UNDEFINED as that handles the coming ")" the best.
					expectingToken(Token::UNDEFINED);
				}
				else
				{	
					expectingToken(Token::INIT_DATA);
					//expectingToken = Token::UNDEFINED;
					//doReturnToExpectToken();
				}
			}
			
		}
		else if( expectingToken() == Token::CLASS_NAME )
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
		else if( expectingToken() == Token::FUNC_DEFINITION )
		{
			if( set_token[0] == '\n' )
			{
				#ifdef DEBUG_RAE
					cout<<"Got NEWLINE. Ignore it because func.\n";
					//rae::log("Got NEWLINE. Ignore it because func.\n");
				#endif
				//expectingToken = Token::FUNC_RETURN_TYPE;
			}
			else if( set_token[0] == '(' )
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
				if( currentFunc )
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
					if( set_token == "init" )
					{
						#ifdef DEBUG_RAE
						//rae::log("Got new. It's a CONSTRUCTOR.\n");
						#endif
						currentFunc->token( Token::CONSTRUCTOR );
						listOfConstructors.push_back(currentFunc);
					}
					else if( set_token == "free" )
					{
						#ifdef DEBUG_RAE
						//rae::log("Got free. It's a DESTRUCTOR.\n");
						#endif
						currentFunc->token( Token::DESTRUCTOR );
						listOfDestructors.push_back(currentFunc);
					}
					else if( set_token == "delete" )
					{
						//#ifdef DEBUG_RAE
						ReportError::reportError("\"delete\" keyword is not used in the Rae programming language. Please use \"free\" instead.", previousElement() );
						//rae::log("delete is deprecated. please use free instead.\n");
						//#endif
						currentFunc->token( Token::DESTRUCTOR );
						listOfDestructors.push_back(currentFunc);
					}
					else if( set_token == "main" )
					{
						#ifdef DEBUG_RAE_HUMAN
						cout<<("Got main. It's a MAIN.\n");
						#endif
						currentFunc->token( Token::MAIN );
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
				//rae::log("ERROR: ");
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
			//rae::log("ERROR: ");
			lineNumber.printOut();
			//rae::log(" token: ", set_token, " expecting: ", Token::toString(expectingToken), " lineText: ", currentLine, "\n");
			//expectingToken = Token::UNDEFINED;
			doReturnToExpectToken();
		}
	*/
	}

	

	//return true if it was a number...
	bool handleNumber(string set_token)
	{
		if(set_token.size() == 0)
			return false;

		if( isNumericChar(set_token[0]) )
		{
			newLangElement(Token::NUMBER, TypeType::UNDEFINED, set_token);

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
		//boost::remove_if( unknownDefinitions, bind(&LangElement::token, _1) != Token::UNKNOWN_DEFINITION );
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
		//boost::remove_if( unknownDefinitions, bind(&LangElement::token, _1) != Token::UNKNOWN_DEFINITION );
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
		//boost::remove_if( unknownUseReferences, bind(&LangElement::token, _1) != Token::UNKNOWN_USE_REFERENCE );
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
		//boost::remove_if( unknownUseMembers, bind(&LangElement::token, _1) != Token::UNKNOWN_USE_MEMBER );
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
				if(elem->token() == Token::CLASS)
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

			//if( (set_elem->token() == Token::DEFINE_REFERENCE || set_elem->token() == Token::DEFINE_ARRAY)
			if( set_elem->isDefinition() && elem->token() == Token::CLASS )
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
			if(previous2ndToken() == Token::OVERRIDE 
				|| previousToken() == Token::OVERRIDE )
			{
				//no errors.
			}
			else
			{
				//if( previousElement() )
				//	cout<<"PREVIOUSTOKEN WAS: "<<previousElement()->toString();

				ReportError::reportError("redefinition of name: " + set_token + "\nUse \"override\" to use the same name again.", previousElement() );
				ReportError::reportError("previous definition is here: " + prev_def->toString(), prev_def );
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
			if(set_elem->previous2ndToken() == Token::OVERRIDE 
				|| set_elem->previousToken() == Token::OVERRIDE )
			{
				//no errors.
			}
			else
			{
				//if( previousElement() )
				//	cout<<"PREVIOUSTOKEN WAS: "<<previousElement()->toString();

				ReportError::reportError("redefinition of name: " + set_elem->toString() + "\nUse \"override\" to use the same name again.", previousElement() );
				ReportError::reportError("previous definition is here: " + prev_def->toString(), prev_def );
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
			ReportError::reportError("Compiler error. setNameAndCheckForPreviousDefinitions. elem_to_set is null.", previousElement() );
			return;
		}

		LangElement* prev_def = searchToken(set_token);
		if(prev_def)
		{
			if(elem_to_set->previous2ndToken() == Token::OVERRIDE
				|| elem_to_set->previousToken() == Token::OVERRIDE )
			{
				//no errors.
			}
			else
			{
				ReportError::reportError("redefinition of name: " + set_token, previousElement() );
				ReportError::reportError("previous definition is here: " + prev_def->toString(), prev_def );
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
			//if( elem->token() == Token::DEFINE_REFERENCE || elem->token() == Token::DEFINE_REFERENCE_IN_CLASS )
			//if( set_elem->token() == Token::UNKNOWN_DEFINITION && elem->token() == Token::CLASS )
			//if( (set_elem->token() == Token::DEFINE_REFERENCE || set_elem->token() == Token::DEFINE_ARRAY)
				//&& set_elem->isUnknownType() == true //how is this needed? Who says we can't check on stuff that we already now?
			//	&& elem->token() == Token::CLASS )
			
			if(set_elem == elem)
			{
				continue;
			}

			if( set_elem->isDefinition() && elem->token() == Token::CLASS )
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
	bool checkIfNewDefinedNameIsValid(Token::e set_token_type, string set_token)
	{
		//Hmm.. let's build a temp object and stuff...

		Token::e use_type_to_use = Token::UNDEFINED;

		//OK. this should be a func called something like: matchUseTypeToDefineType(Token::e set_type):
		if( set_token_type == Token::DEFINE_BUILT_IN_TYPE
			|| set_token_type == Token::DEFINE_BUILT_IN_TYPE_IN_CLASS
			|| set_token_type == Token::DEFINE_REFERENCE
			|| set_token_type == Token::DEFINE_REFERENCE_IN_CLASS
		)
		{
			use_type_to_use = Token::USE_REFERENCE;
		}
		else if( set_token_type == Token::DEFINE_VECTOR
			|| set_token_type == Token::DEFINE_VECTOR_IN_CLASS
		)
		{
			use_type_to_use = Token::USE_VECTOR;
		}
		else if( set_token_type == Token::DEFINE_ARRAY
			|| set_token_type == Token::DEFINE_ARRAY_IN_CLASS
		)
		{
			use_type_to_use = Token::USE_ARRAY;
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

		LangElement* temp_elem = new LangElement(lineNumber, Token::USE_REFERENCE, TypeType::UNDEFINED, set_token, set_token );
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
			/*if(current_context_use->token() == Token::DEFINE_REFERENCE
				|| current_context_use->token() == Token::DEFINE_REFERENCE_IN_CLASS
				|| current_context_use->token() == Token::DEFINE_BUILT_IN_TYPE
				|| current_context_use->token() == Token::DEFINE_BUILT_IN_TYPE_IN_CLASS
				|| current_context_use->token() == Token::DEFINE_ARRAY
				|| current_context_use->token() == Token::DEFINE_ARRAY_IN_CLASS
				|| current_context_use->token() == Token::DEFINE_VECTOR
				|| current_context_use->token() == Token::DEFINE_VECTOR_IN_CLASS
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
			if( found_definition->isUserDefinableToken() )
			{
				#ifdef DEBUG_RAE_PARSER
				if(current_context_use->name() == debug_string_con)
		  	{
		  		cout<<"valid because the found_definition is a class or enum or other user definable type.\n";
		  	}
		  	#endif
				return true;
			}

			if( current_context_use->previousToken() == Token::REFERENCE_DOT && current_context_use->previous2ndToken() == Token::USE_REFERENCE )
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
			else if( current_context_use->previousToken() == Token::REFERENCE_DOT )
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
					if(current_context_use->previousToken() == Token::REFERENCE_DOT)
						cout<<"YES a REFERENCE_DOT.\n";
					else cout<<"NO REFERENCE_DOT.\n";

					if(current_context_use->previous2ndToken() == Token::USE_REFERENCE)
						cout<<"YES a USE_REFERENCE before that.\n";
					else cout<<"NO USE_REFERENCE before that.\n";

					cout<<"previous: "<<current_context_use->previousElement()->toString()<<"\n";
					cout<<"previous 2nd: "<<current_context_use->previous2ndElement()->toString()<<"\n";
				}	
				#endif
			}
			else
			{
				ReportError::reportError("checkIfTokenIsValidInCurrentContext() current_context_use doesn't have a previousElement. Compiler error.", previousElement() );
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
			//if(previousElement()->token() == Token::REFERENCE_DOT && previous2ndElement()->token() == Token::UNKNOWN_USE_REFERENCE)
			if(previousElement()->token() == Token::REFERENCE_DOT && previous2ndElement()->token() == Token::USE_REFERENCE && previous2ndElement()->isUnknownType() == true )
			{
				#ifdef DEBUG_RAE_HUMAN
					cout<<"because previousElement was unknown and there was a REFERENCE_DOT we set this to UNKNOWN_USE_MEMBER: "<<set_token<<"\n";
					//rae::log("because previousElement was unknown we set this to unknown too: ", set_token, "\n");
				#endif
				newUnknownUseMember(set_token);
				return;
			}
			//definitions: Tester tester <-- both are definitions... NO! We'll fix the first one and set the second token to be empty.
			//else if(previousElement()->token() == Token::UNKNOWN_USE_REFERENCE && previous2ndElement()->token() != Token::UNKNOWN_USE_REFERENCE)
			else if(previousElement()->token() == Token::USE_REFERENCE
				&& previousElement()->isUnknownType() == true
				//&& previous2ndElement()->token() != Token::USE_REFERENCE
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
				//previousElement()->token(Token::UNKNOWN_DEFINITION);
				previousElement()->token(Token::DEFINE_REFERENCE);
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
			else if(previousElement()->token() == Token::BRACKET_END
				&& previous2ndElement()->token() == Token::BRACKET_BEGIN
				//&& previous2ndElement()->token() != Token::USE_REFERENCE
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
				//previousElement()->token(Token::UNKNOWN_DEFINITION);
				
				//Ignore _IN_CLASS untill handleUnknownDefinitions.
				/*if( our_array_definition->parent()->token() == Token::CLASS )
				{
					our_array_definition->token(Token::DEFINE_ARRAY_IN_CLASS);
				}
				else
				{*/
					//our_array_definition->token(Token::DEFINE_ARRAY);
				
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
				switch(found_elem->token())
				{
					default:
					break;
					case Token::CLASS:
						expectingToken(Token::DEFINE_REFERENCE_NAME);

						{
							//LangElement* clob = 

							if(expectingTypeType() == TypeType::UNDEFINED)
							{
								ReportError::reportError("handleUserDefinedToken. expectingTypeType was UNDEFINED, when we found a class. Compiler error.", previousElement() );
							}
							
							//expectingRole can be undefined for now. If it's set it's most likely
							//just FUNC_RETURN or FUNC_ARGUMENT. But this remark is early days, so
							//it might change in the future.

							our_new_element = newDefineReference(expectingTypeType(), expectingRole(), found_elem, set_token);

							/*if( clob->parentToken() == Token::CLASS )
							{
								//we are inside a class definition, not a func:
								clob->
							}*/
						}
					break;
					case Token::FUNC:
						our_new_element = newFuncCall(found_elem);
					break;
					case Token::FUNC_CALL:
						cout<<"We should not find FUNC_CALLs...\n";
						assert(0);
					break;
					/*case Token::DEFINE_VECTOR:
					case Token::DEFINE_VECTOR_IN_CLASS:
						our_new_element = newUseVector(found_elem);
					break;
					case Token::DEFINE_ARRAY:
					case Token::DEFINE_ARRAY_IN_CLASS:
						our_new_element = newUseArray(found_elem);
					break;
					*/
					case Token::DEFINE_FUNC_RETURN:
					case Token::DEFINE_FUNC_ARGUMENT:
					//case Token::DEFINE_REFERENCE_IN_CLASS:
					case Token::DEFINE_REFERENCE:
					//case Token::DEFINE_BUILT_IN_TYPE_IN_CLASS:
					//case Token::DEFINE_BUILT_IN_TYPE:

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
					case Token::USE_VECTOR:
						ReportError::reportError("handleUserDefinedToken. Found USE_VECTOR. And we're not supposed to find those.");
					break;
					case Token::USE_ARRAY:
						ReportError::reportError("handleUserDefinedToken. Found USE_ARRAY. And we're not supposed to find those.");
					break;
					*/
					case Token::USE_REFERENCE:
						ReportError::reportError("handleUserDefinedToken. Found USE_REFERENCE. And we're not supposed to find those.", previousElement() );
					break;
				}
			/*}
			else //token is not valid in this context
			{
				//TODO this is not really an error yet. We just need some debugging cout here...
				//The error will come if it's still not found after unknownrefs are checked.
				ReportError::reportError("Maybe not an error... Token is not valid in this context: " + Token::toString(set_token) );
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
 				//our_new_element->token(Token::UNKNOWN_USE_REFERENCE);
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
			//if(lang_elem->token() != Token::UNKNOWN_DEFINITION)
			//if(lang_elem->token() != Token::DEFINE_REFERENCE && 
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

				switch(found_elem->token())
				{
					default:
						ReportError::reportError("handleUnknownTokens found this: " + found_elem->toString(), lang_elem);
					break;
					case Token::CLASS:
						
						if(lang_elem->token() == Token::DEFINE_FUNC_ARGUMENT
							|| lang_elem->token() == Token::DEFINE_FUNC_RETURN
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
						else if(lang_elem->token() == Token::DEFINE_REFERENCE )
						{	/*
							if(lang_elem->typeType() == TypeType::C_ARRAY )//|| lang_elem->token() == Token::DEFINE_ARRAY_IN_CLASS)
							{
								if(lang_elem->parent() && lang_elem->parent()->token() == Token::CLASS)
								{
									/////lang_elem->token(Token::DEFINE_ARRAY_IN_CLASS);
									lang_elem->parent()->createObjectAutoInitAndFree(lang_elem);
									//listOfAutoInitObjects.push_back(lang_elem);	
								}
								else
								{
									////lang_elem->token(Token::DEFINE_ARRAY);	
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
									ReportError::reportError("Found a definition for the second time. Compiler bug. earlier definition: " + lang_elem->definitionElement()->toString() + " later definition: " + found_elem->toString(), lang_elem );
								}
								else
								{
									lang_elem->definitionElement( found_elem );
									#ifdef DEBUG_RAE_HUMAN
										cout<<"CLASSWAY marked a definitionElement: "<<lang_elem->definitionElement()->toString()<<" while looking for "<<lang_elem->toString()<<"\n";
									#endif
								}

								//if we are inside a class, we need to create auto_inits and auto_free to constructors and destructors.
								if(lang_elem->parent() && lang_elem->parent()->token() == Token::CLASS)
								{
									////////////////////lang_elem->token(Token::DEFINE_REFERENCE_IN_CLASS);
									//THIS USED TO BE IT: lang_elem->parent()->createObjectAutoInitAndFree(lang_elem);
									lang_elem->parent()->createObjectAutoInitInConstructors(lang_elem);
									//listOfAutoInitObjects.push_back(lang_elem);	
								}
								else
								{
									//lang_elem->token(Token::DEFINE_REFERENCE);	
								}
								lang_elem->isUnknownType(false);
								addToUserDefinedTokens(lang_elem);
							////}
						}
					break;
					case Token::FUNC:
					case Token::FUNC_CALL:
						//newLangElement(Token::FUNC_CALL, set_token);

						//#ifdef DEBUG_RAE_HUMAN
						cout<<"\n\n\n\n\n\nDEFINITION FUNC_CALL Found. I bet this can be removed, because it shouldn't happen!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n";
						//#endif
						assert(0);

						lang_elem->token(Token::FUNC_CALL);
						lang_elem->isUnknownType(false);
					break;
					/*	
					case Token::DEFINE_VECTOR:
					case Token::DEFINE_VECTOR_IN_CLASS:
						#ifdef DEBUG_RAE_HUMAN
						cout<<"\n\n\n\n\n\nDEFINITION DEFINE_VECTOR Found. I bet this can be removed, because it shouldn't happen!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n";
						#endif

						//newUseVector(found_elem);
						lang_elem->token(Token::USE_VECTOR);
						lang_elem->isUnknownType(false);
					break;
					case Token::DEFINE_ARRAY:
					case Token::DEFINE_ARRAY_IN_CLASS:
						#ifdef DEBUG_RAE_HUMAN
						cout<<"\n\n\n\n\n\nDEFINITION DEFINE_ARRAY Found. I bet this can be removed, because it shouldn't happen!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n";
						#endif

						//newUseArray(found_elem);
						lang_elem->token(Token::USE_ARRAY);
						lang_elem->isUnknownType(false);
					break;
					*/
					case Token::DEFINE_FUNC_RETURN:
					case Token::DEFINE_FUNC_ARGUMENT:
					//case Token::DEFINE_REFERENCE_IN_CLASS:
					case Token::DEFINE_REFERENCE:
					//case Token::DEFINE_BUILT_IN_TYPE_IN_CLASS:
					//case Token::DEFINE_BUILT_IN_TYPE:
						//#ifdef DEBUG_RAE_HUMAN
						//cout<<"\n\n\n\n\n\nDEFINITION OTHER STUFF THAT IS NOT NEEDED Found. I bet this can be removed, because it shouldn't happen!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n";
						//#endif

						//newUseReference(found_elem);
					//We don't create a nu element, instead we modify the existing unknown element.
						lang_elem->token(Token::USE_REFERENCE);
						lang_elem->type( found_elem->type() );//copy the class type etc.
						lang_elem->typeType( found_elem->typeType() );//copy the typetype. ref, built_in, array, vector etc.
						lang_elem->isUnknownType(false);

						//mark that we found the definition for future use:
						if( lang_elem->definitionElement() )
						{
							ReportError::reportError("Found a definition for the second time. Compiler bug. earlier definition: " + lang_elem->definitionElement()->toString() + " later definition: " + found_elem->toString(), lang_elem );
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
						if(lang_elem->nextElement() && lang_elem->nextElement()->token() == Token::REFERENCE_DOT
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
								lang_elem->definitionElement()->definitionElement()->printOutListOfFunctions();
								lang_elem->definitionElement()->definitionElement()->printOutListOfReferences();
								#endif

								if( maybe_member )
								{
									if( maybe_member->token() == Token::FUNC )
									{
										#ifdef DEBUG_RAE_HUMAN
										cout<<"NEWWAY found a FUNC definition "<<maybe_member->toString()<<" while looking for "<<lang_elem->nextElement()->nextElement()->toString()<<"\n";
										#endif
										lang_elem->nextElement()->nextElement()->token(Token::FUNC_CALL);
									}
									else if( maybe_member->token() == Token::DEFINE_REFERENCE )
									{
										#ifdef DEBUG_RAE_HUMAN
										cout<<"NEWWAY found a DEFINE_REFERENCE "<<maybe_member->toString()<<" while looking for "<<lang_elem->nextElement()->nextElement()->toString()<<"\n";
										#endif
										lang_elem->nextElement()->nextElement()->token(Token::USE_REFERENCE);
										lang_elem->nextElement()->nextElement()->typeType( maybe_member->typeType() );//Umm, could it be a vector?
									}
									else
									{
										ReportError::reportError("Found a member func or reference, but it wasn't either. Compiler is confused. maybe_member: " + maybe_member->toString(), lang_elem );
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
					/*case Token::USE_VECTOR:
						ReportError::reportError("handleUnknownDefinitions. Found USE_VECTOR. And we're not supposed to find those.");
					break;
					case Token::USE_ARRAY:
						ReportError::reportError("handleUnknownDefinitions. Found USE_ARRAY. And we're not supposed to find those.");
					break;
					*/
					case Token::USE_REFERENCE:
						ReportError::reportError("handleUnknownDefinitions. Found USE_REFERENCE. And we're not supposed to find those.", lang_elem);
					break;
				}

				#ifdef DEBUG_RAE_HUMAN
				cout<<"fixed lang elem is: "<<lang_elem->toString()<<"\n";
				//rae::log("fixed lang elem is: ", lang_elem->toString(), "\n");
				#endif
			}
			else
			{
				if(lang_elem->previousElement() && lang_elem->previousToken() == Token::REFERENCE_DOT )
				{
					ReportError::reportError("Didn't find definition. Does the class have a member with that name? Check spelling? Did you define it somewhere? Is it accessible in this scope?", lang_elem );
				}
				else
				{
					ReportError::reportError("Didn't find definition. Check spelling? Did you define it somewhere? Is it accessible in this scope?", lang_elem );
				}

				#ifdef DEBUG_RAE_HUMAN
				cout<<"Didn't find: "<<lang_elem->toString()<<" it remains unknown DEFINITION.\n";
				#endif
				//rae::log("Didn't find: ", lang_elem->toString(), " it remains unknown ref.\n");
				//newUnknownUseReference(set_token);
			}	
		}

		cleanUpUnknownTokens(unknown_tokens);
	}

	#include "SourceValidate.hpp"

	#include "SourceWriter.hpp"

};

#endif //RAE_COMPILER_SOURCEPARSER_HPP


