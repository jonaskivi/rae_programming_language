#ifndef RAE_COMPILER_SOURCEPARSER_HPP
#define RAE_COMPILER_SOURCEPARSER_HPP

#include <algorithm>
#include <ciso646>

#include "ReportError.hpp"
#include "rae_helpers.hpp"
#include "LangElement.hpp"
#include "StringFileWriter.hpp"

namespace Rae
{

class LangCompiler;

extern LangCompiler* g_compiler;

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
		/*for(LangElement* elem : langElements)
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
			if(langElements[i] && langElements[i]->scope() == 0)
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
		//m_expectingTypeType = TypeType::UNDEFINED;
		//m_expectingContainer = ContainerType::UNDEFINED;
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

		m_previousElement = nullptr;
		m_unfinishedElement = nullptr;

		isReceivingInitData = false;		
	}

	bool debugWriteLineNumbers;//= false
	
	//Hmm. Currently this system does not work. doReturnToExpectToken just
	//sets it to Token::UNDEFINED every time... maybe we should just remove this system,
	//and just use expectingToken(Token::UNDEFINED) every time. And use Role for these situations,
	//when we need to know some more context about what we were doing before.
	public: Token::e doReturnToExpectToken()
	{
		if( returnToExpectTokenStack.empty() )
		{
			expectingToken(Token::UNDEFINED);
			//cout<<"NO TOKENS: Returned to expectToken: Token::UNDEFINED.\n";
			return m_expectingToken;
		}

		//else
		//expectingToken( returnToExpectTokenStack.back() );
		returnToExpectTokenStack.pop_back();

		if( returnToExpectTokenStack.empty() )
		{
			expectingToken(Token::UNDEFINED);
			//cout<<"POPPED AND NO TOKENS: Returned to expectToken: Token::UNDEFINED.\n";
			return m_expectingToken;
		}
		else
		{
			m_expectingToken = returnToExpectTokenStack.back();
		}

		//cout<<"Returned to expectToken: "<<Token::toString(m_expectingToken)<<"\n";
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
		#ifdef DEBUG_RAE_PARSER
			cout<<"set expectingToken to: "<<Token::toString(set)<<"\n";
		#endif
		m_expectingToken = set;
		//returnToExpectTokenStack.push_back(set);
	}
	protected: Token::e m_expectingToken;// = EDLTokenType::TITLE;

	public: void setNameForExpectingName(string set)
	{
		setNameAndCheckForPreviousDefinitions( m_expectingNameFor, set );
		expectingToken(Token::UNDEFINED);
	}
	public: LangElement* expectingNameFor(){ return m_expectingNameFor; }
	public: void expectingNameFor(LangElement* set)
	{
		m_expectingNameFor = set;
		expectingToken(Token::EXPECTING_NAME);
	}
	protected: LangElement* m_expectingNameFor;

	/*
	//NOT YET USED:
	public: void setTypeForExpectingType(string set)
	{
		if(m_expectinTypeFor)
			m_expectingTypeFor->type(set);
		expectingToken(Token::UNDEFINED);
	}
	public: LangElement* expectingTypeFor(){ return m_expectingTypeFor; }
	public: void expectingTypeFor(LangElement* set)
	{
		m_expectingTypeFor = set;
		expectingToken(Token::EXPECTING_TYPE);
	}
	protected: LangElement* m_expectingTypeFor;
	*/

	//secondary expecting
	/*
	public: TypeType::e expectingTypeType(){ return m_expectingTypeType; }
	public: void expectingTypeType(TypeType::e set){ m_expectingTypeType = set; }
	protected: TypeType::e m_expectingTypeType;
	*/
/*
	public: ContainerType::e expectingContainer(){ return m_expectingContainer; }
	public: void expectingContainer(ContainerType::e set){ m_expectingContainer = set; }
	protected: ContainerType::e m_expectingContainer;
*/
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
		
		// a scope element will only become a parent if it's an empty scope. Otherwise the parent will be
		// the class, func, enum, etc. statement, which is our current parent, and we put that to the stack.
		//if( scopeElementStack.empty() == false )
		//if( currentParentElement() && currentParentElement()->token() == Token::SCOPE_BEGIN )
		//if( shouldNewestScopeBeAParentElement() == true )
		LangElement* iter = our_scope_elem->previousElement();
		while(iter)
		{
			if( iter->isFunc() || iter->token() == Token::ENUM || iter->token() == Token::CLASS )
			{
				// if we get here, we are the first SCOPE_BEGIN in this func, enum or class.
				break;
			}
			else if( iter->token() == Token::SCOPE_BEGIN )
			{
				// We are not the first SCOPE_BEGIN so this is an empty scope which will be a parent then.
				currentParentElement(our_scope_elem);
				break;
			}
			iter = iter->previousElement();
		}

		//put the scope or class or func, or init_data object to scope stack.
		scopeElementStackPush( currentParentElement() ); //NOT true anymore: but we put the class or func (or limiting scope) in the stack. Not the actual scope object (unless it is a limiting scope.)
		
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

				//if there are no constructors then add one. create constructor. create default constructor.
				if( listOfDestructors.empty() == true )
				{
					
					LangElement* a_con = scope_elem->newLangElementToTopOfClass(lineNumber, Token::DESTRUCTOR, TypeType::UNDEFINED, "free" );
					listOfDestructors.push_back(a_con);
					a_con->newLangElement(lineNumber, Token::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES, TypeType::UNDEFINED, "(");
					a_con->newLangElement(lineNumber, Token::PARENTHESIS_END_FUNC_PARAM_TYPES, TypeType::UNDEFINED, ")");
					a_con->newLangElement(lineNumber, Token::NEWLINE);
					//LangElement* a_scop = 
					a_con->newLangElement(lineNumber, Token::SCOPE_BEGIN, TypeType::UNDEFINED, "{");
					a_con->newLangElement(lineNumber, Token::NEWLINE_BEFORE_SCOPE_END);
					a_con->newLangElement(lineNumber, Token::SCOPE_END, TypeType::UNDEFINED, "}");
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
					a_con->newLangElement(lineNumber, Token::PARENTHESIS_BEGIN_FUNC_RETURN_TYPES, TypeType::UNDEFINED, "(");
					a_con->newLangElement(lineNumber, Token::PARENTHESIS_END_FUNC_RETURN_TYPES, TypeType::UNDEFINED, ")");
					a_con->newLangElement(lineNumber, Token::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES, TypeType::UNDEFINED, "(");
					a_con->newLangElement(lineNumber, Token::PARENTHESIS_END_FUNC_PARAM_TYPES, TypeType::UNDEFINED, ")");
					a_con->newLangElement(lineNumber, Token::NEWLINE, TypeType::UNDEFINED, "\n");
					//LangElement* a_scop = 
					a_con->newLangElement(lineNumber, Token::SCOPE_BEGIN, TypeType::UNDEFINED, "{");
					a_con->newLangElement(lineNumber, Token::NEWLINE_BEFORE_SCOPE_END, TypeType::UNDEFINED, "\n");
					a_con->newLangElement(lineNumber, Token::SCOPE_END, TypeType::UNDEFINED, "}");
					//a_con->newLangElement(lineNumber, Token::NEWLINE);
					//a_con->newLangElement(lineNumber, Token::NEWLINE);
					
				}

				if( listOfConstructors.empty() == false )
				{
					if( listOfAutoInitObjects.empty() == false )
					{
						for( LangElement* elem : listOfConstructors )
						{
							//Moved this boilerplate to writer and RaeStdLib.hpp inject things.
							/*
							//more link boilerplate for the constructors:
							//LangElement(LineNumber& set_line_number, Token::e set_lang_token_type, TypeType::e set_type_type, string set_name = "", string set_type = "")
							LangElement* rae_link_list_init = new LangElement(lineNumber, Token::USE_REFERENCE, TypeType::UNDEFINED, string set_name = "", string set_type = "");
							auto_init_elem->token(Token::AUTO_INIT);
							elem->addElementToTopOfFunc(auto_init_elem);
							*/
							for( LangElement* init_ob : listOfAutoInitObjects )
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

									if( init_ob->typeType() == TypeType::LINK && init_ob->initData() == nullptr )
									{
										//Don't do anything. if link and no initdata.
									}
									else if( init_ob->typeType() == TypeType::VAL && init_ob->initData() == nullptr )
									{
										//Don't do anything. if val and no initdata.
									}
									else //all other cases we create an AUTO_INIT object by copying.
									{
										LangElement* auto_init_elem = init_ob->copy();
										auto_init_elem->token(Token::AUTO_INIT);
										elem->addAutoInitElementToFunc(auto_init_elem);
									}
								}
							}
						}
					}

					if( listOfBuiltInTypesInit.empty() == false )
					{
						for( LangElement* elem : listOfConstructors )
						{
							for( LangElement* init_ob : listOfBuiltInTypesInit )
							{		
								//LangElement* bui_le = elem->newLangElementToTopWithNewline( elem->lineNumber(), Token::BUILT_IN_TYPE_AUTO_INIT, TypeType::BUILT_IN_TYPE, init_ob->name(), init_ob->type(), init_ob->builtInType() );//TODO value...
								
								LangElement* auto_init_elem = init_ob->copy();
								auto_init_elem->token(Token::AUTO_INIT);
								elem->addAutoInitElementToFunc(auto_init_elem);
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
						for( LangElement* elem : listOfDestructors )
						{
							for( LangElement* init_ob : listOfAutoInitObjects )
							{
								if( init_ob )
								{
									if( init_ob->typeType() == TypeType::LINK )//&& init_ob->initData() == nullptr )
									{
										//Don't do anything. if link and no initdata.
									}
									else if( init_ob->typeType() == TypeType::VAL)// && init_ob->initData() == nullptr )
									{
										//Don't do anything. if val and no initdata.
									}
									else
									{
										LangElement* auto_init_elem = init_ob->copy();
										auto_init_elem->token(Token::AUTO_FREE);
										elem->addElementToTopOfFunc(auto_init_elem);
									}
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
				currentClass = nullptr;
			}
			else if( scope_elem->token() == Token::FUNC )
			{
				#ifdef DEBUG_RAE_HUMAN
				cout<<"end of function.\n\n";
				#endif
				currentFunc = nullptr;
			}
			else if( scope_elem->token() == Token::ENUM )
			{
				currentEnum = nullptr;
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
	 	
	 	scopeElementStackPop();

	}

	void scopeElementStackPush(LangElement* set)
	{
		scopeElementStack.push_back(set);
	}

	void scopeElementStackPop()
	{
		if( scopeElementStack.empty() == false )
		{
 			scopeElementStack.pop_back();
 		}
		
		//That removed an element so we have to check for empty again:

		resetParentElementToCurrentScope();
	}

	void resetParentElementToCurrentScope()
	{
		if( !scopeElementStack.empty() )
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
			cout<<"error in scopeElementStackPop. no currentModule!!!!!!\n";
		}
	}

	void resetParentElementToAboveParent()
	{
		if( currentParentElement() && currentParentElement()->parent() )
		{
			currentParentElement( currentParentElement()->parent() );
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
			cout<<"error in resetParentElementToAboveParent. no currentModule!!!!!!\n";
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
			cout<<"End init_data.\n";
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
		
		resetParentElementToCurrentScope();
		doReturnToExpectToken(); // Note that we set expectingtoken already here.
	}

	LangElement* newParenthesisBegin(Token::e set_lang_token_type, string set_token)
	{
		LangElement* lang_elem = newLangElement(set_lang_token_type, TypeType::UNDEFINED, set_token);
		//parenthesisStack.push_back(currentParentElement());
		parenthesisStack.push_back(lang_elem);

		//NOPE: Added parenthesis to be parents too.
		//currentParentElement(lang_elem);

		//NOTE: we push the parenthesis_begin elements to the stack!
		//not the currentParentElements like in the scopeElementsStack...!
		return lang_elem;
	}

	//a small helper, just so we can get the token easily...
	Token::e parenthesisStackTokenType()
	{
		if( parenthesisStack.empty() )
		{
			ReportError::reportError("SourceParser::parenthesisStackTokenType() parenthesisStack is empty.", previousElement() );
			return Token::UNDEFINED;
		}

		LangElement* stack_elem = parenthesisStack.back();

		if(stack_elem != nullptr)
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
				lang_elem->pairElement(stack_elem);
				stack_elem->pairElement(lang_elem);

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
				ReportError::reportError("unmatched parenthesis end 2 \")\".", lang_elem);
	 		}
		 	
	 		parenthesisStack.pop_back();
	 	}

	 	//NOPE: Added parenthesis to be parents too.
	 	//resetParentElementToCurrentScope();

 		return lang_elem;
	}

	LangElement* newDefineArray(TypeType::e set_type_type = TypeType::VAL)
	{
		LangElement* lang_elem = newLangElement(Token::BRACKET_DEFINE_ARRAY_BEGIN, set_type_type, "", "array" );
		
		lang_elem->containerType( ContainerType::ARRAY );

		//Brackets don't create a new scope, but they are still currentParent. That's why we don't push to scopeElementStack here.
		currentParentElement(lang_elem);
		//currentArray = lang_elem;

		bracketStack.push_back(lang_elem);

		/*if( lang_elem->parentToken() == Token::CLASS )
		{
			listOfAutoInitObjects.push_back(lang_elem);
		}*/

		addToUserDefinedTokens(lang_elem);

		return lang_elem;
	}

	LangElement* newBracketBegin(Token::e set_lang_token_type, string set_token)
	{
		LangElement* lang_elem = newLangElement(set_lang_token_type, TypeType::UNDEFINED, set_token);
		
		//Brackets don't create a new scope, but they are still currentParent. That's why we don't push to scopeElementStack here.
		currentParentElement(lang_elem);

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


				#ifdef DEBUG_RAE_BRACKET
				//cout<<"MATCH bracket: from: "<<Token::toString(stack_elem->token())
			//		<<" to: "<<Token::toString(Token::matchBracketEnd(stack_elem->token()))
			//		<<" caller: "<<Token::toString(set_lang_token_type)<<"\n";
				//rae::log("match bracket.\n");
				ReportError::reportInfo("Match bracket start: ", stack_elem);
				ReportError::reportInfo("Match bracket end: ", lang_elem);

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

	 	//resetParentElementToCurrentScope();
	 	resetParentElementToAboveParent();
	 	currentReference = nullptr;

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

		if( isReceivingInitData == true )
		{
			// We must end initdata if there's any end of the line comments coming in.
			endInitData();
		}

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
		unfinishedElement(nullptr);
		currentReference = nullptr;

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


	LangElement* newDefineVector(string set_template_second_type, string set_name = "");

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
/*
REMOVED:
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
*/

	/*
	//REMOVE typically I just re-created a function that already existed:
	LangElement* createTypeType(TypeType::e set_typetype, Role::e set_role)
	{
		LangElement* our_ref = newDefineReference(set_typetype, set_role );
		return our_ref;
		
		//if( !bracketStack.empty() ) //REMOVE
		//{
			//if(previous2ndElement() && previous2ndElement()->token() == Token::BRACKET_BEGIN)
			//if( currentParentElement() && currentParentElement()->token() == Token::BRACKET_BEGIN )
			//{
				//previous2ndElement()->token(Token::BRACKET_DEFINE_ARRAY_BEGIN);
				//currentParentElement()->token(Token::BRACKET_DEFINE_ARRAY_BEGIN);

				//our_ref->containerType( ContainerType::ARRAY );
			//}
		//}
	}
	*/

	LangElement* newDefineReference(const string& set_typetype, Role::e set_role, LangElement* maybe_found_class = nullptr, string set_type = "", string set_name = "")
	{
		return newDefineReference( TypeType::fromString(set_typetype), set_role, maybe_found_class, set_type, set_name );
	}
	
	LangElement* newDefineReference(TypeType::e set_type_type, Role::e set_role, LangElement* maybe_found_class = nullptr, string set_type = "", string set_name = "")
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
		if( lang_elem->parentToken() == Token::CLASS && set_type_type != TypeType::PTR )
		{
			// We are inside a class definition, so we autoinit in constructors. Expect not raw pointers.
			listOfAutoInitObjects.push_back(lang_elem);
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
		
		lang_elem->containerType( set_definition_elem->containerType() );
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

		assert(0); //REMOVE THIS FUNC?

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

	
	//boost::signals2::signal<void (string)> newImportSignal;

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

	// val, opt, ref will now create an element, that will be stored in unfinishedElement.
	public: LangElement* unfinishedElement() { return m_unfinishedElement; }
	public: void unfinishedElement(LangElement* set) { m_unfinishedElement = set; }
	protected: LangElement* m_unfinishedElement;
	

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


	public: LangElement* currentScopeElement()
	{
		if( m_currentParentElement->isBeginsScope() )
			return m_currentParentElement;
		//else
		return m_currentParentElement->scope();
	}
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
	
	vector<LangElement*> scopeElementStack; //sort of like a parentElementStack

	vector<LangElement*> parenthesisStack;

	vector<LangElement*> bracketStack; // also used as a sort of templateStack and arrayDefineStack!

public:
	
	bool isWriteToFile;
	bool isFileBased;

	FILE* currentFile;
	string currentFilename;//
	//REMOVE BOOST: boost::filesystem::path currentFilenamePath;//this is the whole filename and the path...
	std::string currentFilenamePath;

	string moduleName(string separator_char_str = ".")
	{
		if(currentModule == 0)
		{
			return "no module";
		}

		string a_module_name = "";

		int not_on_first = 0;

		for(LangElement* elem : currentModule->langElements)
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
				break;//break the for... hope this works.
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
		for(LangElement* an_import : listOfImports)
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
		cout<<currentFilenamePath<<"\n";

		if(currentFilenamePath == "")
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

		for(boost::filesystem::path set_path : sourceFiles)
		{
			writeCppFile(set_path);
		}
		*/
	
		/*
		for( LangElement* elem : langElements )
		{
			cout<<elem->name()<<"\n");
		}
		*/

		for( LangElement* module_elem : langElements )
		{
			if( module_elem->token() != Token::MODULE )
			{
				#ifdef DEBUG_RAE_HUMAN
				cout<<"Interesting: we have something in top hierarchy: "<<module_elem->tokenString()<<"\n";
				//rae::log("Interesting: we have something in top hierarchy: ", module_elem->tokenString(), "\n");
				#endif
				continue; //skip it
			}


			string module_filename;

			string module_name; 

			for( LangElement* elem : module_elem->langElements )
			{
				if( elem->token() == Token::MODULE_NAME )
				{
					module_name += elem->name();
					module_name += "/"; //a directory separator
				}
				else
				{
					break;//break the for... hope this works.
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
			
			//boost::filesystem::path outputHeaderPath = module_filename;
			string outputHeaderPath = module_filename;
			//outputHeaderPath.replace_extension(".hpp");
			outputHeaderPath = replaceExtension(outputHeaderPath, ".hpp");
		
			//REMOVE_BOOST: boost::filesystem::path outputFilePath = module_filename;
			string outputFilePath = module_filename;
			outputFilePath = replaceExtension(outputFilePath, ".cpp");

			//#ifdef DEBUG_RAE_HUMAN
			//cout<<"\n\nWriting C++:\n\n";
			rlutil::setColor(rlutil::BROWN);
			cout << "Writing C++: ";
			rlutil::setColor(rlutil::WHITE);
			cout << module_filename << " .hpp and .cpp\n";
			//cout<<"outputHeaderPath: "<<outputHeaderPath.string()<<"\n";
			//cout<<"outputFilePath: "<<outputFilePath.string()<<"\n";
			//#endif

			//rae::log("\n\nWriting C++:\n\n");
			//rae::log("module_filename: ", module_filename.string(), "\n");
			//rae::log("outputHeaderPath: ", outputHeaderPath.string(), "\n");
			//rae::log("outputFilePath: ", outputFilePath.string(), "\n");
			
			createPathIfNotExist(parentPath(outputHeaderPath));
			createPathIfNotExist(parentPath(outputFilePath));

			/*REMOVE BOOST if(boost::filesystem::exists(outputHeaderPath.parent_path()) == false)
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
			*/

			StringFileWriter writer;
			writer.create( outputHeaderPath, /*isHeader*/true );

			writer.writeString("//this file is automatically created from Rae programming language module:\n//");
			writer.writeString( module_filename );
			writer.writeString("\n");
			
			
			writeElement(writer, *module_elem);
				//writer.writeString( elem->name() );
				//fwrite(&record.name,sizeof(char),record.nameLength,fp);
			
			//fwrite( buffer, sizeof(char), sizeof(buffer), pFile);
				
			writer.close();

			StringFileWriter writer2;
			writer2.create( outputFilePath, /*isHeader*/false );

			writer2.writeString("//this file is automatically created from Rae programming language module:\n//");
			writer2.writeString( module_filename );
			writer2.writeString("\n");

				
			//module_elem->write(writer2);
			writeElement(writer2, *module_elem);
					//writer.writeString( elem->name() );
					//fwrite(&record.name,sizeof(char),record.nameLength,fp);
				
				//fwrite( buffer, sizeof(char), sizeof(buffer), pFile);
				
			writer2.close();

			//debugtree:

			#ifdef DEBUG_RAE_DEBUGTREE
			//REMOVE BOOST boost::filesystem::path debugFilePath = module_filename;
			string debugFilePath = module_filename;
			debugFilePath = replaceExtension(debugFilePath, ".debugtree");

			createPathIfNotExist(parentPath(debugFilePath));

			//REMOVE BOOST: if(boost::filesystem::exists(debugFilePath.parent_path()) == false)
			/*if( checkPathType(parentPath(debugFilePath)) == PathType::DOES_NOT_EXIST )
			{
				//if(boost::filesystem::create_directories( debugFilePath.parent_path() ) )
					//cout << "Created folder: " << parentPath(debugFilePath) << "\n";
				//else
				cout << "Failed to create folder: " << parentPath(debugFilePath) << "\n";
			}*/

			StringFileWriter writer3;
			writer3.create( debugFilePath, /*isHeader doesn't matter here*/false );

			writer3.writeString("//this file is automatically created from Rae programming language module:\n//");
			writer3.writeString( module_filename );
			writer3.writeString("\n");
			writeDebugTree(writer3, *module_elem);	
			writer3.close();

			debugFilePath = replaceExtension(debugFilePath, ".debugtree2");

			createPathIfNotExist(parentPath(debugFilePath));

			//REMOVE BOOST: if(boost::filesystem::exists(debugFilePath.parent_path()) == false)
			/*if (checkPathType(parentPath(debugFilePath)) == PathType::DOES_NOT_EXIST)
			{
				//if(boost::filesystem::create_directories( debugFilePath.parent_path() ) )
					//cout << "Created folder: " << parentPath(debugFilePath) << "\n";
				//else
					cout << "Failed to create folder: " << parentPath(debugFilePath) << "\n";
			}*/

			StringFileWriter writer4;
			writer4.create( debugFilePath, /*isHeader doesn't matter here*/false );

			writer4.writeString("//this file is automatically created from Rae programming language module:\n//");
			writer4.writeString( module_filename );
			writer4.writeString("\n");
			writeDebugTree2(writer4, *module_elem);	
			writer4.close();

			#endif
			//end debugtree
			
			
			
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

		}//end for modules langElements...
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

	unsigned stringIndex;

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

						//cout<<"PASSTHROUGH_HDR: "<<wholeToken;
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
					
					wholeToken = "\n";
					isWholeToken = true;
					///////////////////////////////////////newLine();
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
					return true; // WHY?
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
							cout<<"RAE_ERROR: NON STANDARD CMX3600 EDL file. Stopping parser. Sorry. Add a TITLE line to the top.";
							isDone = true;
						}
							
					break;
					default:
						cout<<"RAE_ERROR: NON STANDARD CMX3600 EDL file. Stopping parser. Sorry. Add a TITLE line to the top.";
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

/*
	void checkPreviousBracketArrayDefineEtc(Token::e set_normal_case)
	{
		if(bracketStack.empty())
			expectingToken(set_normal_case);
		else
		{
			expectingToken(Token::ARRAY_VECTOR_STUFF);
			if(previousElement() && previousElement()->token() == Token::BRACKET_BEGIN)
			{
				previousElement()->token(Token::BRACKET_DEFINE_ARRAY_BEGIN);
			}
		}
	}
*/

	void setUnfinishedElementToStaticArray()
	{
		if( !bracketStack.empty() && unfinishedElement() != nullptr)
		{
			unfinishedElement()->containerType( ContainerType::STATIC_ARRAY );
		}
	}

	void handleToken(string set_token);
	
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

	static bool isKnownType(LangElement* set)
	{
		if (!set)
			return true;
		return !set->isUnknownType();
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
						 isKnownType ),
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
						 isKnownType ),
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
                         isKnownType),
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
						 isKnownType ),
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
		for(LangElement* elem : unknownUserTokens)
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
		for(LangElement* elem : userDefinedTokens)
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

		#ifdef DEBUG_DEBUGNAME
			if (g_debugName == set_name || g_debugName == set_class)
				cout << "looking for class: " << set_class << " val or func: " << set_name << "\n";
		#endif

		for(LangElement* elem : userDefinedTokens)
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
				#ifdef DEBUG_DEBUGNAME
					if (g_debugName == set_name || g_debugName == set_class)
						cout << "found func or val: " << set_name << " in class: " << set_class << "\n";
				#endif
				#ifdef DEBUG_RAE_PARSER
				cout<<"found func or val too.\n";
				#endif
				return found_func_or_val;
			}
			else
			{
				#ifdef DEBUG_DEBUGNAME
					if (g_debugName == set_name || g_debugName == set_class)
						cout << "found func or val: " << set_name << " in class: " << set_class << "\n";
				#endif
				#ifdef DEBUG_RAE_PARSER
				cout<<"didn't find func or val though.\n";
				#endif
			}
		}

		/*for(LangElement* elem : classes)
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

		for(LangElement* elem : search_from_scope->langElements)
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
					#ifdef DEBUG_DEBUGNAME
						if (g_debugName == set_elem->name() )
							cout << "searchScope: found name in class: " << elem->type() << " : " << elem->toSingleLineString() << "\n";
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
						#ifdef DEBUG_DEBUGNAME
							if (g_debugName == set_elem->name())
								cout << "searchScope: BUT IT'S NOT VALID: " << elem->type() << " : " << elem->toSingleLineString() << "\n";
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
					#ifdef DEBUG_DEBUGNAME
						if (g_debugName == set_elem->name())
							cout << "searchScope: found definition: " << elem->type() << " : " << elem->toSingleLineString() << "\n";
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
					cout<<"looking: " << set_elem->name() << " it is not: "<<elem->toString()<<"\n";
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
	

	// This a func to set a name and to check if the name was used before.
	// The source file should then contain "override" so that no errors come.
	void setNameAndCheckForPreviousDefinitions(LangElement* elem_to_set, string set_token)
	{
		if(elem_to_set == 0)
		{
			ReportError::reportError("Compiler error. setNameAndCheckForPreviousDefinitions. elem_to_set is null.", previousElement() );
			return;
		}

		//TODO check if name is valid!
		//if( isValidName(set) )

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
			search_from_scope = search_from_scope->scope();
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
		
		if( set_elem->scope() )
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
		for(LangElement* elem : userDefinedTokens)
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
					#ifdef DEBUG_DEBUGNAME
						if (g_debugName == set_elem->name())
							cout << "searchElementAndCheckIfValidLocal: found: " << elem->type() << " : " << elem->toSingleLineString() << "\n";
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
					#ifdef DEBUG_DEBUGNAME
						if (g_debugName == set_elem->name())
							cout << "searchElementAndCheckIfValidLocal: found2: " << elem->type() << " : " << elem->toSingleLineString() << "\n";
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

	LangElement* searchElementAndCheckIfValid(LangElement* set_elem);

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
	//REMOVE BOOST: boost::signals2::signal<LangElement* (SourceParser*, LangElement*)> searchElementInOtherModulesSignal;

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
		#ifdef DEBUG_DEBUGNAME
		if(current_context_use->name() == g_debugName)
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
				#ifdef DEBUG_DEBUGNAME
				if (current_context_use->name() == g_debugName)
			  	{
			  		cout<<"definitions are always valid.\n";
			  	}
			  	#endif

				return true;
			}

			//also if found_elem is a class or enum. We can use those always. 
			if( found_definition->isUserDefinableToken() )
			{
				#ifdef DEBUG_DEBUGNAME
				if(current_context_use->name() == g_debugName)
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
					#ifdef DEBUG_DEBUGNAME
						if (current_context_use->name() == g_debugName)
						{
							cout << "it's ok. it's valid. someclass.somefunc\n";
						}
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
					//////////if( prev_ref->typeType() == TypeType::VECTOR || prev_ref->typeType() == TypeType::TEMPLATE )
					if( prev_ref->type() == "array" )
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
								
								//if( classHasFunctionOrValue( prev_ref->definitionElement()->templateSecondTypeString(), current_context_use->name() ) )
								if( classHasFunctionOrValue( temp_second_type->type(), current_context_use->name() ) )
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
				#ifdef DEBUG_DEBUGNAME
				if(current_context_use->name() == g_debugName)
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
			if( current_context_use->scope() == found_definition->scope() )
			{
				#ifdef DEBUG_DEBUGNAME
				if(current_context_use->name() == g_debugName)
				{
					cout<<"valid in SAME PARENT.\n";
				}
				#endif

				return true;
			}
			else if( current_context_use->scope() && found_definition->scope() )
			{
				//note to self: here we are most probably only dealing with USE_REFERENCEs, USE_MEMBERs and FUNC_CALLs.
				//all definitions were most likely already checked.

				#ifdef DEBUG_DEBUGNAME
				if(current_context_use->name() == g_debugName)
				{
					cout<<"getting desperate here. not in same parent. no reference dot type use and all that... but both have parents.\n";
					cout<<"current_context_use.parent is: "<<current_context_use->scope()->toString()<<"\n";
					cout<<"found_definition.parent is: "<<found_definition->scope()->toString()<<"\n";
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
							#ifdef DEBUG_DEBUGNAME
							if(current_context_use->name() == g_debugName)
							{
								cout<<"valid. same func. and definition is before use.\n";
							}
							#endif
							return true;
						}
						else
						{
							#ifdef DEBUG_DEBUGNAME
							if(current_context_use->name() == g_debugName)
							{
								cout<<"NOT valid. same func, but definition is after use.\n";
							}
							#endif
							return false;
						}
					}
					else if( current_context_use->parentFunc() != found_definition->parentFunc() )//different funcs.
					{
						#ifdef DEBUG_DEBUGNAME
						if(current_context_use->name() == g_debugName)
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
						#ifdef DEBUG_DEBUGNAME
						if(current_context_use->name() == g_debugName)
						{
							cout<<"valid. same class. definition in class (not in func) and use in func.\n";
						}
						#endif
						return true;					
					}
					else
					{
						#ifdef DEBUG_DEBUGNAME
						if(current_context_use->name() == g_debugName)
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
							
							cout<<"\nand their scopes:\ncontext_use: "<<current_context_use->scope()->toString()<<"\nfound_stuff: "<<found_definition->scope()->toString();
							
						}
						//rae::log("checkIfValid. An unhandled case. Check this thing...\n");
						#endif
					}
				}
				else
				{
					#ifdef DEBUG_DEBUGNAME
					if(current_context_use->name() == g_debugName)
					{
						cout<<"NOT valid because this case is Unhandled1.\n";
						cout<<"context_use: "<<current_context_use->toString()<<"\nfound_stuff: "<<found_definition->toString();
						cout<<"\nand their scopes:\ncontext_use: "<<current_context_use->scope()->toString()<<"\nfound_stuff: "<<found_definition->scope()->toString();
					}
					//rae::log("checkIfValid. An unhandled case. Check this thing...\n");
					#endif
				}

				/*
				//They actually have parents.
				if( current_context_use->parent()->parent() == found_definition->parent()->parent() )
				{
					//in the same class, but perhaps in different funcs...
					if(current_context_use->name() == g_debugName)
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
					if(current_context_use->name() == g_debugName)
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
					if(current_context_use->name() == g_debugName)
					{
						cout<<"NOT valid context is wrong.\n";
					}
					return false;
				}
				else
				{
					if(current_context_use->name() == g_debugName)
					{
						cout<<"NOT valid because this case is Unhandled1.\n";
						cout<<"context_use: "<<current_context_use->toString()<<"\nfound_stuff: "<<found_definition->toString();
						cout<<"\nand their parents:\ncontext_use: "<<current_context_use->parent()->toString()<<"\nfound_stuff: "<<found_definition->parent()->toString();
					}
					//rae::log("checkIfValid. An unhandled case. Check this thing...\n");
				}
				*/
			}
			else if( found_definition->scope() == 0 )
			{
				#ifdef DEBUG_DEBUGNAME
				//maybe it's a global definition, because it doesn't have a parent?
				if(current_context_use->name() == g_debugName)
				{
					cout<<"Umm. valid in maybe its a global thing..\n";
				}
				#endif
				return true;
			}
			else if( current_context_use->scope() == 0 )
			{
				#ifdef DEBUG_DEBUGNAME
				if(current_context_use->name() == g_debugName)
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
				#ifdef DEBUG_DEBUGNAME
				if(current_context_use->name() == g_debugName)
				{
					cout<<"NOT valid because this case is Unhandled2.\n";
				}
				//rae::log("checkIfValid. An unhandled case. Check this thing...\n");
				#endif
			}
		}

		#ifdef DEBUG_DEBUGNAME
		if(current_context_use->name() == g_debugName)
		{
			cout<<"valid END.\n";
		}
		#endif
		return false;


		//used to default to true... Hmm? is this a good default... What if we don't have current_context, so we're global...
		//That's a case for true... but if found_elem is null, then that's just an error...
	}

	/*
	//an array definition:
		if( not bracketStack.empty() )
		{
			LangElement* our_array_definition = bracketStack.back();

			#ifdef DEBUG_RAE_HUMAN
				cout<<"setting array name to be: "<<set_token<<"\n";
			#endif
			
			//setNameAndCheckForPreviousDefinitions

			//our_array_definition->type( our_array_definition->name() );
			our_array_definition->name(set_token);

			///////addToUnknownDefinitions(our_array_definition);
			
			return;
		}
	*/

	void handleUserDefinedToken(string set_token)
	{
		////rae::log("Looking for: ", set_token, "\n");

		if( handleNumber(set_token) )
			return;

		#ifdef DEBUG_RAE_HUMAN
			cout<<"trying to handle: "<<set_token<<" in line: "<<lineNumber.line<<"\n";
			//rae::log("trying to handle: ", set_token, "\n");
		#endif
		#ifdef DEBUG_DEBUGNAME
			if( set_token == g_debugName )
				cout << "handleUserDefinedToken START. trying to handle: " << set_token << " in line: " << lineNumber.line << "\n";
		#endif
		//Should we should just mark everything unknown at this point,
		//and later handle all the unknowns in one big pass.
		//Would it be easier? Maybe. But then again, if we only need to parse things (mostly) once
		//it could be faster...

		//if we have some typetype that we're waiting, then just jump over these checks.
		if( unfinishedElement() != nullptr && previousElement() && previous2ndElement() )
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
		}

		LangElement* our_new_element;

		LangElement* found_elem = searchToken(set_token);
		if( found_elem )
		{
			#ifdef DEBUG_RAE_HUMAN
			cout<<"Found user token: "<<found_elem->toString()<<" "<<found_elem->namespaceString()<<"\n";
			//rae::log("Found user token: ", found_elem->toString(), "\n");
			#endif
			#ifdef DEBUG_DEBUGNAME
				if (g_debugName == set_token)
					cout << "handleUserDefinedToken: found: " << found_elem->type() << " : " << found_elem->toSingleLineString() << "\n";
			#endif

			//Check if it is a valid token in this context: //check already in searchToken now...
			//if( checkIfTokenIsValidInCurrentContext(currentParentElement(), found_elem) == true )
			//{
				switch(found_elem->token())
				{
					default:
					break;
					case Token::CLASS:

						if( bracketStack.empty() ) //not in the middle of a template list.
						{
							expectingToken(Token::DEFINE_REFERENCE_NAME);
						}

						if(currentReference == nullptr)
						{
							//ReportError::reportError("handleUserDefinedToken. currentReference was null, when we found a class. Compiler error. set_token: " + set_token, previousElement() );
						
							// This case is just a class name without any preceeding val or opt, so no currentReference has been created.
							// We'll create a new reference that defaults to VAL.
							if( !bracketStack.empty() )
							{
								newDefineReference( "val", Role::TEMPLATE_PARAMETER, found_elem, set_token );
								// Not waiting for the name as it's a template parameter.
							}
							else
							{
								LangElement* our_ref = newDefineReference( "val", expectingRole(), found_elem, set_token );
								unfinishedElement(our_ref); // Still waiting for the name.
							}
						}
						else
						{	
							// expectingRole can be undefined for now. If it's set it's most likely
							// just FUNC_RETURN or FUNC_ARGUMENT. But this remark is early days, so
							// it might change in the future.

							//our_new_element = newDefineReference(expectingTypeType(), expectingRole(), found_elem, set_token);
							currentReference->definitionElement(found_elem);
							currentReference->type(set_token);

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
					case Token::BRACKET_DEFINE_ARRAY_BEGIN:
					case Token::BRACKET_DEFINE_STATIC_ARRAY_BEGIN:
					case Token::DEFINE_FUNC_RETURN:
					//REMOVED: case Token::DEFINE_FUNC_ARGUMENT:
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
			#endif
			#ifdef DEBUG_DEBUGNAME
				if( set_token == g_debugName )
					cout << "handleUserDefinedToken Didn't find: " << set_token << " in line: " << lineNumber.line << "\n";
			#endif
			//specifically don't do our_new_element = , because this is already unknown and will be handled later...
			//Oh well. found_elem would be null anyway...
			if(currentReference == nullptr)
			{
				#ifdef DEBUG_RAE_HUMAN
				cout<<"newUnknownUseReference2: "<<set_token<<" creating unknown ref.\n";
				#endif
				newUnknownUseReference2(set_token);
			}
			// We have already created a DEFINE_REFERENCE with a keyword like val opt ref. But the class is still undefined.
			else if( currentReference->isDefinition() && currentReference->type() == "" && currentReference->name() == "" )
			{
				#ifdef DEBUG_RAE_HUMAN
				cout<<"NOPE...: "<<set_token<<" there was some currentReference so we set it to type of that thing.\n";
				#endif
				if( bracketStack.empty() ) //not in the middle of a template list.
				{
					expectingToken(Token::DEFINE_REFERENCE_NAME);
				}
				currentReference->type(set_token);
				currentReference->isUnknownType(true);
			}
			else
			{
				#ifdef DEBUG_RAE_HUMAN
					cout<<"Umm: "<<set_token<<" oh no. This is some strange thing that happens on oneliners. Let's still create an unknown ref.\n";
				#endif
				newUnknownUseReference2(set_token);
			}
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
		cout << "handleUnknownTokens START.\n";

		cout << "unknownDefinitions: " << unknownDefinitions.size() << "\n";
		cout << "unknownUseReferences: " << unknownUseReferences.size() << "\n";
		cout << "unknownUseMembers: " << unknownUseMembers.size() << "\n";

		/*if( unknownDefinitions.empty() && unknownUseReferences.empty() && unknownUseMembers.empty() )
		{
			return;
		}*/

		//handleUnknownDefinitions();
		//handleUnknownUseReferences();
		//handleUnknownUseMembers();

		for( uint i = 0; i < 4; i++ )
		{
			handleUnknownTokens( unknownDefinitions );
			handleCheckForPreviousDefinitionsList();
			handleUnknownTokens( unknownUseReferences );
			handleUnknownTokens( unknownUseMembers );

			if( unknownDefinitions.empty() && unknownUseReferences.empty() && unknownUseMembers.empty() )
			{
				//if(i > 0)
				ReportError::reportInfo("Parsed module " + numberToString(i+1) + " times. All unknown references found.", moduleName() );
				return;
			}
		}

		if( !unknownDefinitions.empty() && !unknownUseReferences.empty() && !unknownUseMembers.empty() )
		{
			ReportError::reportError("After parsing the source 4 times, there are still " + numberToString(unknownDefinitions.size() + unknownUseReferences.size() + unknownUseMembers.size()) + " unknown references.", 0, moduleName() );
		}
		//TEMP:
		else cout << "All unknowns handled OK.\n";
	}

	void handleCheckForPreviousDefinitionsList()
	{
		for(LangElement* lang_elem : checkForPreviousDefinitionsList)
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

	void tryToConnectDefinitionToNextReferenceDot(LangElement* our_unknown_elem, LangElement* definition_elem )
	{
		if(our_unknown_elem && our_unknown_elem->isUnknownType() == true && definition_elem)
		{
			#ifdef DEBUG_RAE_HUMAN
			cout<<"Yes another nextElement and it is unknown!!!!!\n";
			#endif
			LangElement* maybe_member = definition_elem->hasFunctionOrMember(our_unknown_elem->name() );
			
			#ifdef DEBUG_RAE_HUMAN
			cout<<"looking for name: "<<our_unknown_elem->name()<<"\n";
			cout<<"from: "<<definition_elem->toString()<<"\n";
			definition_elem->printOutListOfFunctions();
			definition_elem->printOutListOfReferences();
			#endif

			if( maybe_member )
			{
				if( maybe_member->token() == Token::FUNC )
				{
					#ifdef DEBUG_RAE_HUMAN
					cout<<"NEWWAY found a FUNC definition "<<maybe_member->toString()<<" while looking for "<<our_unknown_elem->toString()<<"\n";
					#endif
					our_unknown_elem->token(Token::FUNC_CALL);
				}
				else if( maybe_member->token() == Token::DEFINE_REFERENCE )
				{
					#ifdef DEBUG_RAE_HUMAN
					cout<<"NEWWAY found a DEFINE_REFERENCE "<<maybe_member->toString()<<" while looking for "<<our_unknown_elem->toString()<<"\n";
					#endif
					our_unknown_elem->token(Token::USE_REFERENCE);
					our_unknown_elem->typeType( maybe_member->typeType() );//Umm, could it be a vector?
					our_unknown_elem->containerType( maybe_member->containerType() );
				}
				else
				{
					ReportError::compilerError("Found a member func or reference, but it wasn't either. Compiler is confused. maybe_member: " + maybe_member->toString(), our_unknown_elem );
				}

				our_unknown_elem->isUnknownType(false);
				our_unknown_elem->definitionElement(maybe_member);//And this is important. The .funcCall points to func definition now.
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

		for( LangElement* lang_elem : unknown_tokens )
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
				#ifdef DEBUG_DEBUGNAME
					if (g_debugName == lang_elem->name())
						cout << "handleUnknownTokens: found: " << found_elem->type() << " : " << found_elem->toSingleLineString() << "\n";
				#endif
				switch(found_elem->token())
				{
					default:
						ReportError::reportError("handleUnknownTokens found this: " + found_elem->toString(), lang_elem);
					break;
					case Token::CLASS:
						
						if(lang_elem->token() == Token::USE_REFERENCE
							&& lang_elem->typeType() == TypeType::UNDEFINED
							&& lang_elem->nextElement()
							&& lang_elem->nextElement()->isUnknownType() == true
							&& lang_elem->nextElement()->token() == Token::USE_REFERENCE)
						{
							cout<<"This really happened.\n";
							// For strange reasons we can say, that this is probably a case of default val:
							// Tester tester
							lang_elem->token(Token::DEFINE_REFERENCE);
							lang_elem->typeType(TypeType::VAL);

							lang_elem->definitionElement(found_elem);
							lang_elem->type( lang_elem->name() );

							lang_elem->name( lang_elem->nextElement()->name() );

							//if we are inside a class, we need to create auto_inits and auto_free to constructors and destructors.
							LangElement* current_scope = lang_elem->scope();
							if( current_scope && current_scope->token() == Token::CLASS)
							{
								current_scope->createObjectAutoInitInConstructors(lang_elem);
							}
							
							lang_elem->isUnknownType(false);
							lang_elem->nextElement()->token(Token::EMPTY); // set the next element to be empty. We took the name from it.
							lang_elem->nextElement()->isUnknownType(false);
							addToUserDefinedTokens(lang_elem);
						}
						else if(//REMOVED: lang_elem->token() == Token::DEFINE_FUNC_ARGUMENT
							lang_elem->token() == Token::DEFINE_FUNC_RETURN
							)
						{
							lang_elem->isUnknownType(false);
							if(lang_elem->typeType() == TypeType::UNDEFINED)
							{
								ReportError::reportError("SourceParser.handleUnknownTokens() Found undefined and setting it to REF. This is todo.", lang_elem);
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
								LangElement* current_scope = lang_elem->scope();
								if(current_scope && current_scope->token() == Token::CLASS)
								{
									////////////////////lang_elem->token(Token::DEFINE_REFERENCE_IN_CLASS);
									//THIS USED TO BE IT: lang_elem->parent()->createObjectAutoInitAndFree(lang_elem);
									current_scope->createObjectAutoInitInConstructors(lang_elem);
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
					case Token::BRACKET_DEFINE_ARRAY_BEGIN:
					case Token::BRACKET_DEFINE_STATIC_ARRAY_BEGIN:
					case Token::DEFINE_FUNC_RETURN:
					//REMOVED: case Token::DEFINE_FUNC_ARGUMENT:
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
						lang_elem->containerType( found_elem->containerType() );
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

						/*
						// And here's the optimization I was talking about: check for following FUNC_CALL and USE_REFERENCE.
						// If we already know the class we are dealing with, which is found in lang_elem->definitionElement()->definitionElement()
						if(lang_elem->nextElement() && lang_elem->definitionElement()->definitionElement()
							&& lang_elem->nextElement()->token() == Token::REFERENCE_DOT )
						{
							#ifdef DEBUG_RAE_HUMAN
							cout<<"Yes a REFERENCE_DOT!!!!!\n";
							#endif
							// this is our reference dot, and the element after it is unknown.
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
						*/

						//if(lang_elem->name() == "tester_links")
							//cout<<"I hear you man. :)\n"<<lang_elem->name();

						if(lang_elem->nextElement() && lang_elem->definitionElement()->definitionElement()
							&& lang_elem->nextElement()->token() == Token::REFERENCE_DOT )
						{
							//cout<<"we did: "<<lang_elem->name()<<"\n";
							tryToConnectDefinitionToNextReferenceDot(lang_elem->nextElement()->nextElement(), lang_elem->definitionElement()->definitionElement() );
						}
						else if( lang_elem->definitionElement()->definitionElement() // A case for ARRAYS!!! :) some_array[somehorriblewayto(whatthehell)].callMe
							&& lang_elem->nextElement()
							&& lang_elem->nextElement()->token() == Token::BRACKET_BEGIN )
						{
							//cout<<"looking for ref_dot after BRACKET_BEGIN.\n";
							ReportError::reportError("Non tested BRACKET_END array code ahead.", lang_elem);
							assert(0);
							LangElement* bracket_end = lang_elem->nextElement()->searchFirst(Token::BRACKET_END);
							if( bracket_end && bracket_end->nextElement() && bracket_end->nextElement()->token() == Token::REFERENCE_DOT)
								tryToConnectDefinitionToNextReferenceDot(bracket_end->nextElement()->nextElement(), lang_elem->definitionElement()->definitionElement() );
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
					ReportError::reportWarning("Didn't find definition. Does the class have a member with that name? Check spelling? Did you define it somewhere? Is it accessible in this scope?", { lang_elem, lang_elem->previousElement(), lang_elem->previous2ndElement() } );
				}
				else
				{
					ReportError::reportWarning("Didn't find definition. Check spelling? Did you define it somewhere? Is it accessible in this scope?", lang_elem );
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

}

#endif //RAE_COMPILER_SOURCEPARSER_HPP


