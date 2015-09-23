#ifndef RAE_COMPILER_SOURCEPARSER_HPP
#define RAE_COMPILER_SOURCEPARSER_HPP

#include <algorithm>
#include <ciso646>

#include "ReportError.hpp"
#include "rae_helpers.hpp"
#include "Element.hpp"
#include "StringFileWriter.hpp"

namespace Rae
{

class Compiler;

extern Compiler* g_compiler;

enum class ParserType
{
	RAE,
	CPP
};


class SourceParser
{
public:

	SourceParser()
	{
		init();
	}
	
	SourceParser(string set_filename, bool do_parse = true)
	{
		read(set_filename);
	}

	~SourceParser()
	{
		#ifdef DEBUG_RAE_DESTRUCTORS
			cout<<"~SourceParser() START: module: "<<moduleName()<<"\n";
		#endif

		for(uint i = 0; i < elements.size(); i++)
		{
			if(elements[i] && elements[i]->scope() == 0)
			{
				#ifdef DEBUG_RAE_DESTRUCTORS
					cout<<"~SourceParser() going to delete elem: "<<elements[i]->toString()<<"\n";
				#endif
				delete elements[i];
			}
			else
			{
				#ifdef DEBUG_RAE_DESTRUCTORS
					cout<<"~SourceParser() DID NOT delete object."<<"\n";
				#endif
			}
		}	
		elements.clear();

		// here's all vectors I can remember. But I guess we wouldn't need to clear them.
		unknownDefinitions.clear();
		unknownUseReferences.clear();
		unknownUseMembers.clear();

		#ifdef DEBUG_RAE_DESTRUCTORS
			cout<<"~SourceParser() END: module: "<<moduleName()<<"\n";
		#endif
	}

	void init()
	{
		m_parserType = ParserType::RAE;

		lineNumber.line = 1; // linenumbers start from 1, but we keep it default to 0 to see if it's really been set.

		debugWriteLineNumbers = true;

		stringIndex = 0;
		isWriteToFile = true;
		isFileBased = true;

		isDone = false;
		isWholeToken = false;
		isWholeToken2 = false;
		isEndOfLine = false;
		isSingleLineComment = false;
		expectingToken(Token::UNDEFINED);
		m_expectingRole = Role::UNDEFINED;

		foundToken = Token::UNDEFINED;

		isPleaseRehandleChar = false;

		isQuoteReady = false;
		isWaitingForQuoteEnd = false;
		
		isStarCommentReady = false;
		isWaitingForStarCommentEnd = false;

		isPlusCommentReady = false;
		nroWaitingForPlusCommentEnd = 0;

		isWaitingForPragmaToken = false;
		isPassthroughMode = false;
		isPassthroughSourceMode = false;

		isWaitingForNamespaceDot = false;
		nextElementWillGetNamespace = nullptr;

		isCppBindingsParseMode = false;

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
	
	// Hmm. Currently this system does not work properly. doReturnToExpectToken just
	// sets it to Token::UNDEFINED every time... maybe we should just remove this system,
	// and just use expectingToken(Token::UNDEFINED) every time. And use Role for these situations,
	// when we need to know some more context about what we were doing before.
	public: Token::e doReturnToExpectToken()
	{
		#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_EXPECTING_TOKEN)
			cout << "doReturnToExpectToken() START.n";
        	for(uint i = 0; i < returnToExpectTokenStack.size(); ++i)
            	cout << "expect stack: " << i << " " << Token::toString(returnToExpectTokenStack[i]) << "\n";
        #endif
        
		if( returnToExpectTokenStack.empty() )
		{
			expectingToken(Token::UNDEFINED);
            //m_expectingToken = Token::UNDEFINED;
			//
			//cout<<"NO TOKENS: Returned to expectToken: Token::UNDEFINED.\n";
			return m_expectingToken;
		}

		//else
		//expectingToken( returnToExpectTokenStack.back() );
		returnToExpectTokenStack.pop_back();

		if( returnToExpectTokenStack.empty() )
		{
			expectingToken(Token::UNDEFINED);
            //m_expectingToken = Token::UNDEFINED;
			//
			//cout<<"POPPED AND NO TOKENS: Returned to expectToken: Token::UNDEFINED.\n";
			return m_expectingToken;
		}
		else
		{
			//m_expectingToken = returnToExpectTokenStack.back();
			expectingToken( returnToExpectTokenStack.back() );
            //cout << "POPPED and going to: " << Token::toString(m_expectingToken)<<"\n";
		}

		//
		//cout<<"Returned to expectToken: "<<Token::toString(m_expectingToken)<<"\n";
		return m_expectingToken;
	}

	// Now the system might work, when using this explicit function
	public: void pushExpectingToken(Token::e set)
	{
		#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_EXPECTING_TOKEN)
			cout<<"PUSH expectingToken to: "<<Token::toString(set)<<"\n";
		#endif
		m_expectingToken = set;
		returnToExpectTokenStack.push_back(set);
	}

	public: Token::e whatIsReturnToExpectToken()//don't use this... just for if you need it to check what it is.
	{
		if( returnToExpectTokenStack.empty() )
			return Token::UNDEFINED;

		//else
		return returnToExpectTokenStack.back();
	}

	protected: vector<Token::e> returnToExpectTokenStack;
	public: Token::e expectingToken(){ return m_expectingToken; }
	public: void expectingToken(Token::e set)
	{
		#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_EXPECTING_TOKEN)
			cout<<"set expectingToken to: "<<Token::toString(set)<<"\n";
		#endif
		m_expectingToken = set;
	}
	protected: Token::e m_expectingToken;

	public: void setNameForExpectingName(string set)
	{
		#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_EXPECTING_NAME)
			cout << "Got name: <" <<  set <<"> for EXPECTING_NAME: " << m_expectingNameFor->toSingleLineString() << ". Now checking it.\n";
		#endif

		setNameAndCheckForPreviousDefinitions( m_expectingNameFor, set );
		doReturnToExpectToken();
	}
	public: Element* expectingNameFor(){ return m_expectingNameFor; }
	public: void expectingNameFor(Element* set)
	{
		#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_EXPECTING_NAME)
			cout << "expectingNameFor: " <<  set->toSingleLineString() << "\n";
			if(m_expectingRole == Role::FUNC_RETURN)
			{
				cout << "expectingNameFor INFO. Now this is a FUNC_RETURN so there might be no name coming.\n";
			}
		#endif
		m_expectingNameFor = set;
		pushExpectingToken(Token::EXPECTING_NAME);
	}
	protected: Element* m_expectingNameFor;

	public: void setTypeForExpectingType(string set)
	{
		//setTypeAndCheckForPreviousDefinitions( m_expectingNameFor, set );
		// TODO check if type exists
		m_expectingTypeFor->type(set);
		doReturnToExpectToken();
	}
	public: Element* expectingTypeFor(){ return m_expectingTypeFor; }
	public: void expectingTypeFor(Element* set)
	{
		m_expectingTypeFor = set;
		pushExpectingToken(Token::EXPECTING_TYPE);
	}
	protected: Element* m_expectingTypeFor;
	
	public: Role::e expectingRole(){ return m_expectingRole; }
	public: void expectingRole(Role::e set){ m_expectingRole = set; }
	protected: Role::e m_expectingRole;

public:

	Token::e foundToken;
	
	// This is similar to expectingRole and expectingToken TODO make better.
	bool isReceivingInitData;

	bool isSingleLineComment;// = false; //TODO rename to isWaitingForSingleLineComment for concistency.
	string currentComment;
	
	//for handling quotes.
	bool isQuoteReady;
	bool isWaitingForQuoteEnd;

	string currentQuote;

	/*for handling star comments like this one.*/
	// and now also #* rae star comments like this one *#
	bool isStarCommentReady;
	bool isWaitingForStarCommentEnd;
	string currentStarComment;

	/* plus comments are quite like star comments, but they override them.
	And you need a matching number to end them, so you can have them inside each other.*/
	bool isPlusCommentReady;
	int nroWaitingForPlusCommentEnd;
	string currentPlusComment;

	//for handling the @ special tokens such as @cpp
	bool isWaitingForPragmaToken;
	bool isPassthroughMode;
	bool isPassthroughSourceMode;

	bool isWaitingForNamespaceDot;
	Element* nextElementWillGetNamespace;

	//after @cpp_bindings
	bool isCppBindingsParseMode;

	string currentModuleHeaderFileName;//e.g. HelloWorld.hpp, we pass this into the cpp writer.

	//low level parser:

	int currentChar;
	
	//two systems doing basically the same thing. TODO for refactoring:
	int beforeChar;
	string handleSlash;
	
	bool isPleaseRehandleChar;

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

	Element* createElement(Token::e set_lang_token_type, Kind::e set_type_type, string set_name = "", string set_type = "")
	{
		Element* lang_elem = nullptr;
		
		if( currentParentElement() )
		{
			lang_elem = currentParentElement()->newElement(lineNumber, set_lang_token_type, set_type_type, set_name, set_type);
			lang_elem->namespaceElement(currentNamespace);
		
		//else if( currentModule )
		//{
		
			//lang_elem = currentModule->newElement(lineNumber, set_lang_token_type, set_type_type, set_name, set_type);

			if( previousElement() && previousElement()->token() == Token::EXTERN )
			{
				// do nothing, no parent
			}
			else if(    set_lang_token_type == Token::MODULE
				|| set_lang_token_type == Token::NAMESPACE
				|| set_lang_token_type == Token::CLASS
				|| set_lang_token_type == Token::FUNC //JONDE TODO replace these 4 lines with static isFunc() function.
				|| set_lang_token_type == Token::CONSTRUCTOR
				|| set_lang_token_type == Token::DESTRUCTOR
				|| set_lang_token_type == Token::MAIN
				|| set_lang_token_type == Token::INIT_DATA //also init data is now a parent element!
				|| set_lang_token_type == Token::PROJECT
				//and it holds all the init_data inside it.
				)
			{
				currentParentElement(lang_elem);
			}
			//elements.push_back( lang_elem );
			//#ifdef DEBUG_RAE
			//	cout<<"BASE Add element: "<<Token::toString(set_lang_token_type)<<" name:>"<<set_name<<"< type:"<<set_type<<"\n");
			//#endif
		}
		else
		{
			ReportError::compilerError("createElement() No current parent element found. Have you defined a module using the module keyword?");
			cout<<"tried to create:"<<Token::toString(set_lang_token_type)<<" name: "<<set_name<<" type: "<<set_type<<"\n";
			//exit(0);
		}

		if(lang_elem)
		{
			// Must use raw here because previousElement func call now skips whitespace, which is strange, but maybe helps somewhere.
			lang_elem->previousElement( m_previousElement );
			m_previousElement->nextElement(lang_elem);
			//lang_elem->previousElement( previousElement() );
			//previousElement()->nextElement(lang_elem);

			if(nextElementWillGetNamespace)
			{
				lang_elem->useNamespace(nextElementWillGetNamespace);
				nextElementWillGetNamespace = nullptr;
			}
		
		//previous2ndElement = previousElement;
		
			m_previousElement = lang_elem;//this is where we set previousElement.

		//Hmm. Now that I think of it. previousElement and currentParentElement are the same! We'll there might be some differences because of those ifs above...

		}

		return lang_elem;

		/*
		Element* lang_elem = new Element(lineNumber, set_lang_token_type, set_name, set_type);
		currentParentElement() = lang_elem;
		elements.push_back( lang_elem );
		cout<<"BASE Add element: "<<Token::toString(set_lang_token_type)<<" name:>"<<set_name<<"< type:"<<set_type<<"\n");
		return lang_elem;
		*/
	}

	void createScopeBegin()
	{
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


		Element* our_scope_elem = createElement(Token::SCOPE_BEGIN, Kind::UNDEFINED, "{");
		
		
		//MOVED to createElement. Remove from here if it starts working.

		// a scope element will only become a parent if it's an empty scope. Otherwise the parent will be
		// the class, func, enum, etc. statement, which is our current parent, and we put that to the stack.
		//if( scopeElementStack.empty() == false )
		//if( currentParentElement() && currentParentElement()->token() == Token::SCOPE_BEGIN )
		//if( shouldNewestScopeBeAParentElement() == true )
		Element* iter = our_scope_elem->previousElement();
		while(iter)
		{
			if( iter->isFunc() || iter->token() == Token::ENUM || iter->token() == Token::CLASS || iter->token() == Token::NAMESPACE)
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
	}

	void createScopeEnd()
	{
		Element* scope_elem = 0;

		if( scopeElementStack.empty() == true )
		{
			//lang_elem = createElement(set_lang_token_type, Kind::UNDEFINED, set_token);
			ReportError::reportError("unmatched scope end \"}\".", previousElement() );
		}
		else
		{
			scope_elem = scopeElementStack.back();
		}
		
		if( scope_elem )
		{
			scope_elem->freeOwned();
			//createNewLine();
		}

		if( currentParentElement() && currentParentElement()->currentElement() && currentParentElement()->currentElement()->token() == Token::NEWLINE )
		{
			//REVIEW: This code (currentParentElement()->currentElement()) looks very strange. I wonder why it works!

			//we had a newline. mark it.
			currentParentElement()->currentElement()->token(Token::NEWLINE_BEFORE_SCOPE_END);
		}
		
		if( scope_elem )
		{
			Element* lang_elem = scope_elem->newElement(lineNumber, Token::SCOPE_END, Kind::UNDEFINED, "}" );

			if(lang_elem)
			{
				lang_elem->previousElement(previousElement());
				previousElement()->nextElement(lang_elem);
			}

			if( scope_elem->token() == Token::NAMESPACE )
			{
				#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_NAMESPACE)
					cout<<"end of namespace: " << scope_elem->name() << "\n";
				#endif
				currentNamespace = scope_elem->namespaceElement(); // Can be nullptr, but that is just good.
			}
			else if( scope_elem->token() == Token::CLASS )
			{
				#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_CLASS)
					cout<<"end of class.\n\n";
				#endif

				//class ends, do stuff:

				//if there are no constructors then add one. create constructor. create default constructor.
				if( listOfDestructors.empty() == true )
				{
					
					Element* a_con = scope_elem->newElementToTopOfClass(lineNumber, Token::DESTRUCTOR, Kind::UNDEFINED, "free" );
					listOfDestructors.push_back(a_con);
					a_con->newElement(lineNumber, Token::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES, Kind::UNDEFINED, "(");
					a_con->newElement(lineNumber, Token::PARENTHESIS_END_FUNC_PARAM_TYPES, Kind::UNDEFINED, ")");
					a_con->newElement(lineNumber, Token::NEWLINE);
					//Element* a_scop = 
					a_con->newElement(lineNumber, Token::SCOPE_BEGIN, Kind::UNDEFINED, "{");
					a_con->newElement(lineNumber, Token::NEWLINE_BEFORE_SCOPE_END);
					a_con->newElement(lineNumber, Token::SCOPE_END, Kind::UNDEFINED, "}");
					//a_con->newElement(lineNumber, Token::NEWLINE);
					//a_con->newElement(lineNumber, Token::NEWLINE);
				
				}

				if( listOfConstructors.empty() == true )
				{
					#ifdef DEBUG_RAE_HUMAN
						cout<<"Creating a constructor.\n";
					#endif

					Element* a_con = scope_elem->newElementToTopOfClass(lineNumber, Token::CONSTRUCTOR, Kind::UNDEFINED, "init" );
					listOfConstructors.push_back(a_con);
					a_con->newElement(lineNumber, Token::PARENTHESIS_BEGIN_FUNC_RETURN_TYPES, Kind::UNDEFINED, "(");
					a_con->newElement(lineNumber, Token::PARENTHESIS_END_FUNC_RETURN_TYPES, Kind::UNDEFINED, ")");
					a_con->newElement(lineNumber, Token::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES, Kind::UNDEFINED, "(");
					a_con->newElement(lineNumber, Token::PARENTHESIS_END_FUNC_PARAM_TYPES, Kind::UNDEFINED, ")");
					a_con->newElement(lineNumber, Token::NEWLINE, Kind::UNDEFINED, "\n");
					//Element* a_scop = 
					a_con->newElement(lineNumber, Token::SCOPE_BEGIN, Kind::UNDEFINED, "{");
					a_con->newElement(lineNumber, Token::NEWLINE_BEFORE_SCOPE_END, Kind::UNDEFINED, "\n");
					a_con->newElement(lineNumber, Token::SCOPE_END, Kind::UNDEFINED, "}");
					//a_con->newElement(lineNumber, Token::NEWLINE);
					//a_con->newElement(lineNumber, Token::NEWLINE);
					
				}

				if( listOfConstructors.empty() == false )
				{
					if( listOfAutoInitObjects.empty() == false )
					{
						for( Element* elem : listOfConstructors )
						{
							//Moved this boilerplate to writer and RaeStdLib.hpp inject things.
							/*
							//more link boilerplate for the constructors:
							//Element(LineNumber& set_line_number, Token::e set_lang_token_type, Kind::e set_type_type, string set_name = "", string set_type = "")
							Element* rae_link_list_init = new Element(lineNumber, Token::USE_REFERENCE, Kind::UNDEFINED, string set_name = "", string set_type = "");
							auto_init_elem->token(Token::AUTO_INIT);
							elem->addElementToTopOfFunc(auto_init_elem);
							*/
							for( Element* init_ob : listOfAutoInitObjects )
							{
								if( init_ob )
								{
									/*
									if( init_ob->token() == Token::DEFINE_VECTOR_IN_CLASS )
									{
										elem->newElementToTopWithNewline( elem->lineNumber(), Token::VECTOR_AUTO_INIT, init_ob->name(), init_ob->type() );
									}
									else if( init_ob->token() == Token::DEFINE_ARRAY_IN_CLASS )
									{
										elem->newElementToTopWithNewline( elem->lineNumber(), Token::C_ARRAY_AUTO_INIT, init_ob->name(), init_ob->type() );
									}
									*/
									/*
									if( init_ob->kind() == Kind::VECTOR )
									{
										elem->newElementToTopWithNewline( elem->lineNumber(), Token::VECTOR_AUTO_INIT, Kind::VECTOR, init_ob->name(), init_ob->type() );
									}
									else if( init_ob->kind() == Kind::TEMPLATE )
									{
										elem->newElementToTopWithNewline( elem->lineNumber(), Token::TEMPLATE_AUTO_INIT, Kind::TEMPLATE, init_ob->name(), init_ob->type() );
									}
									else if( init_ob->kind() == Kind::C_ARRAY )
									{
										elem->newElementToTopWithNewline( elem->lineNumber(), Token::C_ARRAY_AUTO_INIT, Kind::C_ARRAY, init_ob->name(), init_ob->type() );
									}
									else
									{	
										elem->newElementToTopWithNewline( elem->lineNumber(), Token::OBJECT_AUTO_INIT, Kind::REF, init_ob->name(), init_ob->type() );
									}*/

									if( init_ob->kind() == Kind::LINK && init_ob->initData() == nullptr )
									{
										//Don't do anything. if link and no initdata.
									}
									else if( init_ob->kind() == Kind::VAL && init_ob->initData() == nullptr )
									{
										//Don't do anything. if val and no initdata.
									}
									else //all other cases we create an AUTO_INIT object by copying.
									{
										Element* auto_init_elem = init_ob->copy();
										auto_init_elem->token(Token::AUTO_INIT);
										elem->addAutoInitElementToFunc(auto_init_elem);
									}
								}
							}
						}
					}

					if( listOfBuiltInTypesInit.empty() == false )
					{
						for( Element* elem : listOfConstructors )
						{
							for( Element* init_ob : listOfBuiltInTypesInit )
							{		
								//Element* bui_le = elem->newElementToTopWithNewline( elem->lineNumber(), Token::BUILT_IN_TYPE_AUTO_INIT, Kind::BUILT_IN_TYPE, init_ob->name(), init_ob->type(), init_ob->builtInType() );//TODO value...
								
								Element* auto_init_elem = init_ob->copy();
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
						for( Element* elem : listOfDestructors )
						{
							for( Element* init_ob : listOfAutoInitObjects )
							{
								if( init_ob )
								{
									if( init_ob->kind() == Kind::LINK )//&& init_ob->initData() == nullptr )
									{
										//Don't do anything. if link and no initdata.
									}
									else if( init_ob->kind() == Kind::VAL)// && init_ob->initData() == nullptr )
									{
										//Don't do anything. if val and no initdata.
									}
									else
									{
										Element* auto_init_elem = init_ob->copy();
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
			else if( scope_elem->isFunc() )
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
			#endif
		}
		else
		{
			createElement(Token::SCOPE_END, Kind::UNDEFINED, "}");
 		}
	 	
	 	scopeElementStackPop();

	}

	void scopeElementStackPush(Element* set)
	{
		#ifdef DEBUG_RAE_PARSER
			if(set)
				cout << "scopeElementStackPush: " << set->toSingleLineString() << "\n";
		#endif
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
			// Definitely don't push to scope stack here. Otherwise it will add the element twice.
			currentParentElement( scopeElementStack.back(), /*push_to_scope_stack:*/false);
			if( currentParentElement() )
			{
				if( currentParentElement()->token() == Token::CLASS )
				{
					//...so if the current scope is another class, we mark it again.
					currentClass = currentParentElement();
				}
				else if( currentParentElement()->isFunc() )
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
			currentParentElement(currentModule, false);
		}
		else
		{
			cout<<"error in scopeElementStackPop. no currentModule!!!!!!\n";
		}

		#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_PARENT_ELEMENT)
			if(currentParentElement()) 
				cout << "resetParentElementToCurrentScope: " << currentParentElement()->toSingleLineString() << "\n";
		#endif
	}

	void resetParentElementToAboveParent()
	{
		if( currentParentElement() && currentParentElement()->parent() )
		{
			currentParentElement( currentParentElement()->parent(), false);
			if( currentParentElement() )
			{
				if( currentParentElement()->token() == Token::CLASS )
				{
					//...so if the current scope is another class, we mark it again.
					currentClass = currentParentElement();
				}
				else if( currentParentElement()->isFunc() )
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
			currentParentElement(currentModule, false);
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

		Element* initdata_start_elem = currentParentElement();

		if(initdata_start_elem && initdata_start_elem->token() != Token::INIT_DATA)
		{
			ReportError::reportError("unmatched initdata end.", initdata_start_elem );
			return;
		}
		else
		{
			#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_INIT_DATA)
				if(initdata_start_elem)
					cout<<"End initdata for: " << initdata_start_elem->toSingleLineString() << "\n";
				else cout << "endInitData for NO INIT_DATA.";
			#endif
		}
		
		scopeElementStackPop();
		doReturnToExpectToken(); // Note that we set expectingtoken already here.
	}

	Element* createParenthesisBegin(Token::e set_lang_token_type, string set_token)
	{
		Element* lang_elem = createElement(set_lang_token_type, Kind::UNDEFINED, set_token);
		//parenthesisStack.push_back(currentParentElement());
		parenthesisStack.push_back(lang_elem);

		//NOPE: Added parenthesis to be parents too.
		//currentParentElement(lang_elem);

		//NOTE: we push the parenthesis_begin elements to the stack!
		//not the currentParentElements like in the scopeElementsStack...!
		return lang_elem;
	}

	// a small helper, just so we can get the token easily...
	Token::e parenthesisStackTokenType()
	{
		if( parenthesisStack.empty() )
		{
			ReportError::reportError("SourceParser::parenthesisStackTokenType() parenthesisStack is empty.", previousElement() );
			return Token::UNDEFINED;
		}

		Element* stack_elem = parenthesisStack.back();

		if(stack_elem != nullptr)
		{
			return stack_elem->token();
		}
		//else
		return Token::UNDEFINED;
	}

	Element* createParenthesisEnd(Token::e set_lang_token_type, string set_token)
	{
		Element* stack_elem;
		Element* lang_elem;

		if( parenthesisStack.empty() == true )
		{
			lang_elem = createElement(set_lang_token_type, Kind::UNDEFINED, set_token);
			ReportError::reportError("unmatched parenthesis end \")\".", lang_elem);
		}
		else
		{
			stack_elem = parenthesisStack.back();

			if( stack_elem )
			{
				//We just do matching: is it good enough:

				lang_elem = createElement(Token::matchParenthesisEnd(stack_elem->token()), Kind::UNDEFINED, set_token);
				lang_elem->pairElement(stack_elem);
				stack_elem->pairElement(lang_elem);

				#ifdef DEBUG_RAE
					/*cout<<"MATCH parenthesis: from: "<<Token::toString(stack_elem->token())
						<<" to: "<<Token::toString(Token::matchParenthesisEnd(stack_elem->token()))
						<<" caller: "<<Token::toString(set_lang_token_type)<<"\n");*/
				#endif

				/*
				if( stack_elem->token() == Token::PARENTHESIS_BEGIN_LOG )
				{
					stack_elem->newElement(lineNumber, set_lang_token_type, set_token );
				}
				else
				{
					stack_elem->newElement(lineNumber, set_lang_token_type, set_token );
				}
				*/
				
				//cout<<"End parenthesis for: "<<Token::toString( scope_elem->token() );
			}
			else
			{
				lang_elem = createElement(set_lang_token_type, Kind::UNDEFINED, set_token);
				ReportError::reportError("unmatched parenthesis end 2 \")\".", lang_elem);
	 		}
		 	
	 		parenthesisStack.pop_back();
	 	}

	 	//NOPE: Added parenthesis to be parents too.
	 	//resetParentElementToCurrentScope();

 		return lang_elem;
	}

	Element* createDefineArray(Kind::e set_type_type = Kind::VAL)
	{
		Element* lang_elem = createElement(Token::BRACKET_DEFINE_ARRAY_BEGIN, set_type_type, "", "array" );
		
		lang_elem->containerType( ContainerType::ARRAY );

		// Brackets don't create a new scope, but they are still currentParent. That's why we don't push to scopeElementStack here.
		currentParentElement(lang_elem, true);
		//currentArray = lang_elem;

		bracketStack.push_back(lang_elem);

		/*if( lang_elem->parentToken() == Token::CLASS )
		{
			listOfAutoInitObjects.push_back(lang_elem);
		}*/

		addToUserDefinedTokens(lang_elem);

		return lang_elem;
	}

	Element* createBracketBegin(Token::e set_lang_token_type, string set_token)
	{
		Element* lang_elem = createElement(set_lang_token_type, Kind::UNDEFINED, set_token);
		
		// Brackets don't create a new scope, but they are still currentParent. That's why we don't push to scopeElementStack here.
		currentParentElement(lang_elem, true);

		bracketStack.push_back(lang_elem);

		//cout<<"bracketStack.push_back() called.\n");

		//NOTE: we push the bracket_begin elements to the stack!
		//not the currentParentElements like in the scopeElementsStack...!
		return lang_elem;
	}

	Element* createBracketEnd(Token::e set_lang_token_type, string set_token)
	{
		Element* stack_elem;
		Element* lang_elem;

		if( bracketStack.empty() == true )
		{
			lang_elem = createElement(set_lang_token_type, Kind::UNDEFINED, set_token);
			ReportError::reportError("unmatched bracket end \"]\".", lang_elem);
		}
		else
		{			
			stack_elem = bracketStack.back();

			if( stack_elem )
			{
				//cout<<"we has stack_elem in createBracketEnd.\n");
				//We just do matching: is it good enough:

				lang_elem = createElement(Token::matchBracketEnd(stack_elem->token()), Kind::UNDEFINED, set_token);

				//cout<<"I bet we've crashed.\n");


				#ifdef DEBUG_RAE_BRACKET
					//cout<<"MATCH bracket: from: "<<Token::toString(stack_elem->token())
					//		<<" to: "<<Token::toString(Token::matchBracketEnd(stack_elem->token()))
					//		<<" caller: "<<Token::toString(set_lang_token_type)<<"\n";
					ReportError::reportInfo("Match bracket start: ", stack_elem);
					ReportError::reportInfo("Match bracket end: ", lang_elem);

				#endif

				/*
				if( stack_elem->token() == Token::bracket_BEGIN_LOG )
				{
					stack_elem->newElement(lineNumber, set_lang_token_type, set_token );
				}
				else
				{
					stack_elem->newElement(lineNumber, set_lang_token_type, set_token );
				}
				*/
				
				//cout<<"End bracket for: "<<Token::toString( scope_elem->token() );
			}
			else
			{
				lang_elem = createElement(set_lang_token_type, Kind::UNDEFINED, set_token);
	 		}
		 	
	 		bracketStack.pop_back();
	 	}

	 	//resetParentElementToCurrentScope();
	 	/////resetParentElementToAboveParent();
	 	scopeElementStackPop();
	 	currentReference = nullptr;

 		return lang_elem;
	}
/*
	Element* createImport(string set_name)
	{
		Element* lang_elem;
		
		if( currentModule )
		{
			lang_elem = currentModule->newElement(lineNumber, Token::IMPORT, Kind::UNDEFINED, set_name);
			currentParentElement(lang_elem);
			
			elements.push_back( lang_elem );
			#ifdef DEBUG_RAE
				cout<<"createImport: "<<Token::toString(Token::IMPORT)<<" name:>"<<set_name<<"\n";
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
			ReportError::reportError("createImport() No current module found. Compilers fault. Something went wrong.");
		}

		return lang_elem;
	}
*/

	Element* createImport(string set_name)
	{
		Element* lang_elem;
		
		lang_elem = createElement(Token::IMPORT, Kind::UNDEFINED, set_name);
			
			#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_IMPORT)
				cout<<"createImport: "<<Token::toString(Token::IMPORT)<<" name:>"<<set_name<<"\n";
				cout << "currentParentElement: " << currentParentElement()->toSingleLineString() << "\n";
			#endif

		currentTempElement = lang_elem;

		expectingToken(Token::IMPORT_NAME);

		return lang_elem;
	}

	Element* createModule(string set_name)
	{
		//Element* lang_elem = createElement(Token::MODULE, set_name);

		Element* lang_elem;
		
		lang_elem = new Element(lineNumber, Token::MODULE, Kind::UNDEFINED, set_name);
		currentParentElement(lang_elem);
			
			elements.push_back( lang_elem );
			
			#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_MODULE)
				cout<<"BASE Add module element: "<<Token::toString(Token::MODULE)<<" name:>"<<set_name<<"\n";
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
	Element* closeModule()
	{
		Element* lang_elem = createElement(Token::CLOSE_MODULE);
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

	Element* closeModule()
	{
        Element* lang_elem;
		//Element* lang_elem = createElement(Token::CLOSE_MODULE);
		//if( currentModule && lang_elem )
		if( currentModule )
		{
			//let's put the closeModule to be the last element in module...
			
			lang_elem = currentModule->newElement(lineNumber, Token::CLOSE_MODULE, Kind::UNDEFINED);

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
//Old createClass which puts the classes to the same level as modules in the
//hierarchy...
	Element* createClass(string set_name)
	{
		Element* lang_elem = createElement(Token::CLASS, set_name);
		currentClass = lang_elem;
		addToUserDefinedTokens(lang_elem);
		//currentParentElement() = lang_elem; //already did this inside createElement...
		return lang_elem;
	}
*/	
/*
	Element* createClass(string set_name)
	{
		Element* lang_elem;
		if( currentModule )
		{
			//Hmm, we put the name of the class into the name AND type params!!!
			lang_elem = currentModule->newElement(lineNumber, Token::CLASS, Kind::REF, set_name, set_name);
			currentParentElement(lang_elem);//Hmm... we should make this easier...
			currentClass = lang_elem;

			addToClasses(lang_elem);
			addToUserDefinedTokens(lang_elem);
			
			#ifdef DEBUG_RAE_HUMAN
				cout<<"\n\ncreateClass: "<<set_name<<"\n";
			#endif
		}
		else //no class, so we just make a global class... Um... this shouldn't happen. We have to have a module.
			//This is an error.
		{
			//lang_elem = createElement(Token::CLASS, set_name);
			//currentFunc = lang_elem;
			ReportError::reportError("createClass() No current module found. Compilers fault. Something went wrong.");
		}

		return lang_elem;
	}
*/
	Element* createClass(string set_name)
	{
		Element* lang_elem;
		//Hmm, we put the name of the class into the name AND type params!!!
		//lang_elem = createElement(Token::CLASS, Kind::REF, set_name, set_name);
		lang_elem = createElement(Token::CLASS, Kind::UNDEFINED, set_name, set_name); //Why did we have kind set to REF?
		
		if(lang_elem->isExtern() == false)
			currentClass = lang_elem;

		addToClasses(lang_elem);
		addToUserDefinedTokens(lang_elem);
		
		#ifdef DEBUG_RAE_HUMAN
			cout<<"\n\ncreateClass: "<<set_name<<"\n";
		#endif
		
		return lang_elem;
	}

	Element* createNamespace(string set_name)
	{
		Element* lang_elem;
		lang_elem = createElement(Token::NAMESPACE, Kind::UNDEFINED, set_name);
		currentNamespace = lang_elem;
		addToUserDefinedTokens(lang_elem);
		return lang_elem;
	}
/*
	Element* createFunc(string set_name = "")
	{
		Element* lang_elem;
		if( currentClass )
		{
			lang_elem = currentClass->newElement(lineNumber, Token::FUNC, Kind::UNDEFINED, set_name);
			currentParentElement(lang_elem);//Hmm... we should make this easier...
			currentFunc = lang_elem;
			//currentParentElement() = lang_elem;
		}
		else //no class, so we just make a global func...
		{
			lang_elem = createElement(Token::FUNC, Kind::UNDEFINED, set_name);
			currentFunc = lang_elem;
		}
		addToUserDefinedTokens(lang_elem);

		return lang_elem;
	}
*/

	Element* createFunc(string set_name = "")
	{
		Element* lang_elem;
		lang_elem = createElement(Token::FUNC, Kind::UNDEFINED, set_name);
		currentFunc = lang_elem;
		
		addToUserDefinedTokens(lang_elem);

		return lang_elem;
	}

	Element* createQuote(string set_name = "")
	{
		/*Element* lang_elem;
		if( currentParentElement() )
		{
			lang_elem = currentParentElement()->newElement(Token::QUOTE, set_name);
			//currentParentElement() = lang_elem;//Hmm... we should make this easier...
			//currentFunc = lang_elem;
			//currentParentElement() = lang_elem;
		}
		else //no current element, so we just make a comment...
		{
			lang_elem = createElement(Token::QUOTE, set_name);
		}
		return lang_elem;*/

		#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_QUOTES)
			cout<<"createQuote: "<<set_name<<"\n";
		#endif
		
		return createElement(Token::QUOTE, Kind::UNDEFINED, set_name);		
	}
	
	Element* createPlusComment(string set_name = "")
	{
		#ifdef DEBUG_COMMENTS
			cout<<"createPlusComment: "<<set_name<<"\n";
		#endif

		return createElement(Token::PLUS_COMMENT, Kind::UNDEFINED, set_name);		
	}

	Element* createStarComment(string set_name = "")
	{
		#ifdef DEBUG_COMMENTS
			cout<<"createStarComment: "<<set_name<<"\n";
		#endif

		return createElement(Token::STAR_COMMENT, Kind::UNDEFINED, set_name);		
	}	

	Element* createComment(string set_name)
	{
		/*Element* lang_elem;
		if( currentParentElement() )
		{
			lang_elem = currentParentElement()->newElement(Token::COMMENT, set_name);
			//currentParentElement() = lang_elem;//Hmm... we should make this easier...
			currentFunc = lang_elem;
			//currentParentElement() = lang_elem;
		}
		else //no current element, so we just make a comment...
		{
			lang_elem = createElement(Token::COMMENT, set_name);
		}
		return lang_elem;
		*/
	
		#ifdef DEBUG_COMMENTS
			cout<<"createComment: "<<set_name<<"\n";
		#endif

		if( isReceivingInitData == true )
		{
			// We must end initdata if there's any end of the line comments coming in.
			endInitData();
		}

		return createElement(Token::COMMENT, Kind::UNDEFINED, set_name);
	}

	Element* createPointToElement()
	{
		return createElement(Token::POINT_TO, Kind::UNDEFINED, "->");
	}

	void createNewLine()
	{
		#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_NEWLINE)
			cout << "createNewline at parent: " << currentParentElement()->toSingleLineString() << "\n";
		#endif

		unfinishedElement(nullptr);
		currentReference = nullptr;

		createElement(Token::NEWLINE, Kind::UNDEFINED, "\n");

	}


	Element* createDefineBuiltInType(BuiltInType::e set_built_in_type, Role::e set_role, string set_type, string set_name = "")
	{
		Element* lang_elem = createElement(Token::DEFINE_REFERENCE, Kind::BUILT_IN_TYPE, set_name, set_type);
		lang_elem->builtInType(set_built_in_type);
		lang_elem->role(set_role);
		
		currentReference = lang_elem;

		if( lang_elem->parentToken() == Token::CLASS )
		{
			listOfBuiltInTypesInit.push_back(lang_elem);			
		}

		addToUserDefinedTokens(lang_elem);

		return lang_elem;
	}

	// using createUseReference for the time being.
	Element* createUseBuiltInType(Element* set_elem)
	{
		if( set_elem == 0 )
			return 0;

		Element* lang_elem = createElement(Token::USE_REFERENCE, Kind::BUILT_IN_TYPE, set_elem->name(), set_elem->type() );
		
		#ifdef DEBUG_RAE_HUMAN
			cout<<"createUseBuiltInType: "<<set_elem->toString()<<"\n";
		#endif

		return lang_elem;
	}

	Element* createDefineFuncReturn(string set_type, string set_name = "")
	{
		Element* lang_elem = createElement(Token::DEFINE_FUNC_RETURN, Kind::UNDEFINED, set_name, set_type);
		currentReference = lang_elem;

		if( BuiltInType::isBuiltInType(set_type) )
		{
			//ok.
			lang_elem->kind(Kind::BUILT_IN_TYPE);
			addToUserDefinedTokens(lang_elem);
		}
		else
		{
			Element* found_elem = searchToken(set_type);
			if( found_elem )
			{
				//ok.
				lang_elem->kind( found_elem->kind() );//is it ok to just copy the type from here...???
				addToUserDefinedTokens(lang_elem);
			}
			else
			{
				lang_elem->isUnknownType(true);
				addToUnknownDefinitions( lang_elem );
			}
		}

		#ifdef DEBUG_RAE_HUMAN
			cout<<"createDefineFuncReturn: type: "<<set_type<<" name: "<<lang_elem->namespaceString()<<"<\n";
		#endif

		return lang_elem;
	}
/*
REMOVED:
	Element* createDefineFuncArgument(string set_type, string set_name = "")
	{

		cout<<"Remove this FUNC_ARGUMENT thing, and just use parent() which is scope, to know if it's inside a func def. Use DEFINE_REFERENCE for them instead.\n";
		assert(0);

		Element* lang_elem = createElement(Token::DEFINE_FUNC_ARGUMENT, Kind::UNDEFINED, set_name, set_type);
		currentReference = lang_elem;

		if( BuiltInType::isBuiltInType(set_type) )
		{
			//ok.
			lang_elem->kind(Kind::BUILT_IN_TYPE);
			addToUserDefinedTokens(lang_elem);
		}
		else
		{
			Element* found_elem = searchToken(set_type);
			if( found_elem )
			{
				//ok.
				lang_elem->kind(found_elem->kind());//is it ok to just copy the typetype here?
				addToUserDefinedTokens(lang_elem);
			}
			else
			{
				lang_elem->isUnknownType(true);
				addToUnknownDefinitions( lang_elem );
			}
		}

		#ifdef DEBUG_RAE_HUMAN
			cout<<"createDefineFuncArgument: type: "<<set_type<<" name: "<<lang_elem->namespaceString()<<"<\n";
		#endif

		return lang_elem;
	}
*/

	/*
	//REMOVE typically I just re-created a function that already existed:
	Element* createKind(Kind::e set_typetype, Role::e set_role)
	{
		Element* our_ref = createDefineReference(set_typetype, set_role );
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

	Element* createDefineReference(const string& set_typetype, Role::e set_role, Element* maybe_found_class = nullptr, string set_type = "", string set_name = "")
	{
		return createDefineReference( Kind::fromString(set_typetype), set_role, maybe_found_class, set_type, set_name );
	}
	
	Element* createDefineReference(Kind::e set_type_type, Role::e set_role, Element* maybe_found_class = nullptr, string set_type = "", string set_name = "")
	{
		Element* lang_elem = createElement(Token::DEFINE_REFERENCE, set_type_type, set_name, set_type);
		//lang_elem->kind(Kind::REF);
		lang_elem->role(set_role);
		currentReference = lang_elem;

		if(maybe_found_class)
		{
			lang_elem->definitionElement(maybe_found_class);
		}

		/*
		if( lang_elem->parent() )
		{
			if( set_type_type != Kind::VAL )
			{
				//HERE'S THE OLD OWN SYSTEM, THAT works with lang_eleme.freeOwned()
				//If we ever happen to need it again. It is usefull with pointer new and delete
				//but now that we're using shared_ptr, it's not so usefull, because we're just
				//values now.
				lang_elem->parent()->own(lang_elem);
			}
		}
		*/
		if( lang_elem->parentToken() == Token::CLASS && set_type_type != Kind::PTR )
		{
			// We are inside a class definition, so we autoinit in constructors. Except not raw pointers.
			listOfAutoInitObjects.push_back(lang_elem);
		}

		addToUserDefinedTokens(lang_elem);

		#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_REFERENCES)
			cout<<"createDefineReference: type: "<<set_type<<" name: "<<lang_elem->namespaceString()<<"<\n";
		#endif

		return lang_elem;
	}

	Element* createUseReference(Element* set_definition_elem)
	{
		if( set_definition_elem == 0 )
			return 0;

		Element* lang_elem = createElement(Token::USE_REFERENCE, set_definition_elem->kind(), set_definition_elem->name(), set_definition_elem->type() );
		
		lang_elem->containerType( set_definition_elem->containerType() );
		lang_elem->definitionElement(set_definition_elem);

		#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_REFERENCES)
			cout<<"createUseReference of definition: "<<set_definition_elem->toString()<<"\n";
		#endif

		return lang_elem;
	}

	Element* createFuncCall(Element* set_definition_elem)
	{
		if( set_definition_elem == 0 )
			return 0;

		Element* lang_elem = createElement(Token::FUNC_CALL, set_definition_elem->kind(), set_definition_elem->name(), set_definition_elem->type() );
		
		lang_elem->definitionElement(set_definition_elem);

		#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_FUNCS)
			cout<<"createFuncCall of definition: "<<set_definition_elem->toString()<<"\n";
		#endif

		return lang_elem;	
	}

/*
	Element* createUnknown(string set_token)
	{
		Element* lang_elem = createElement(Token::UNKNOWN, set_token );
		addToUnknowns(lang_elem);

		#ifdef DEBUG_RAE_HUMAN
			cout<<"createUnknown: "<<set_token<<"\n";
		#endif

		return lang_elem;
	}
*/
	Element* createUnknownDefinition(string set_token)
	{
		//Element* lang_elem = createElement(Token::UNKNOWN_DEFINITION, set_token );
		Element* lang_elem = createElement(Token::DEFINE_REFERENCE, Kind::UNDEFINED, set_token );
		lang_elem->isUnknownType(true);
		addToUnknownDefinitions(lang_elem);

		#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_UNKNOWN)
			cout<<"createUnknownDefinition: "<<set_token<<"\n";
		#endif

		return lang_elem;
	}

	Element* createUnknownUseReference2(string set_token)
	{
		//Element* lang_elem = createElement(Token::UNKNOWN_USE_REFERENCE, set_token );
		Element* lang_elem = createElement(Token::USE_REFERENCE, Kind::UNDEFINED, set_token );
		lang_elem->isUnknownType(true);
		addToUnknownUseReferences(lang_elem);

		#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_REFERENCES) || defined(DEBUG_RAE_UNKNOWN)
			cout<<"createUnknownUseReference: "<<set_token<<"\n";
		#endif

		return lang_elem;
	}

	Element* createUnknownUseMember(string set_token)
	{
		//Element* lang_elem = createElement(Token::UNKNOWN_USE_MEMBER, set_token );
		Element* lang_elem = createElement(Token::USE_MEMBER, Kind::UNDEFINED, set_token );
		lang_elem->isUnknownType(true);
		addToUnknownUseMembers(lang_elem);

		#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_REFERENCES) || defined(DEBUG_RAE_UNKNOWN)
			cout<<"createUnknownUseMember: "<<set_token<<"\n";
		#endif

		return lang_elem;
	}

	Element* createNumber(string set_name)
	{
		#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_NUMBERS)
			cout<<"createNumber: "<<set_name<<"\n";
		#endif
		return createElement(Token::NUMBER, Kind::UNDEFINED, set_name);	
	}

	vector<Element*> listOfImports;//a list of imports used in this module.

	vector<Element*> classes;

	//Do we need this: and it probably should be a map or something with fast text search.
	vector<Element*> userDefinedTokens;

	//We'll list them separately now, because we'll have to handle them in this order:
	vector<Element*> unknownDefinitions;//->both of them: Tester tester
	vector<Element*> unknownUseReferences;//tester or tester.doSomething() //without a class. and the func is not usereference.
	//unless it's a global func. 
	vector<Element*> unknownUseMembers;//tester.unknownUseMember() or tester.unknownUseMember //unknown after a reference dot.
	
	//mostly func arguments:
	vector<Element*> checkForPreviousDefinitionsList;

	//This is the tree of all our elements.
	vector<Element*> elements;
	//The three is something like this, even though this Compiler class
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
	//So, we could just rename that "elements" to be "modules"...
	//But maybe we'll just call it elements, as it's just a big tree anyway.
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

	Element* currentModule;
	Element* currentNamespace;
	Element* currentClass;
	Element* currentFunc;
	Element* currentEnum;
	Element* currentTemplate;

	Element* currentTempElement;

	Element* currentReference;

	// val, opt, ref will now create an element, that will be stored in unfinishedElement.
	public: Element* unfinishedElement() { return m_unfinishedElement; }
	public: void unfinishedElement(Element* set) { m_unfinishedElement = set; }
	protected: Element* m_unfinishedElement;
	

    // Hmm. We wanted to skip whitespace in previousElement stuff, because it would mess things up.
    // So previousElement never returns whitespace elements.
	public: Element* previousElement() // Hmm. Then rename to previousElementNonWhitespace() for no confusion.
    {
        if (m_previousElement && m_previousElement->isWhiteSpace() )
        {
            return m_previousElement->previousElement();
        }
        return m_previousElement;
    }
	public: void previousElement(Element* set) { m_previousElement = set; }
	protected: Element* m_previousElement;

	public: Element* previous2ndElement()
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
	//Element* previousElement;
	//Element* previous2ndElement;

	//These lists are temporary lists that are only used while parsing the current class.
	//After the class ends, these lists are cleared.
	vector<Element*> listOfConstructors;//we need these lists because we have to add stuff there later...
	vector<Element*> listOfDestructors;
	vector<Element*> listOfAutoInitObjects;
	vector<Element*> listOfBuiltInTypesInit;


	public: Element* currentScopeElement()
	{
		if( m_currentParentElement->isBeginsScope() )
			return m_currentParentElement;
		//else
		return m_currentParentElement->scope();
	}
	//This is either a module, class, func or an empty scope
	public: Element* currentParentElement() { return m_currentParentElement; }
	public: void currentParentElement(Element* set, bool push_to_scope_stack = true)
	{
		#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_PARENT_ELEMENT)
			if(set)
			{
				cout<<"currentParentElement set to: "<<set->toSingleLineString()<<"\n";
			}
			else cout<<"currentParentElement set to null.\n";
		#endif
		
		m_currentParentElement = set;

		// a scope element will only become a parent if it's an empty scope. Otherwise the parent will be
		// the class, func, enum, etc. statement, which is our current parent, and we put that to the stack.
		//if( scopeElementStack.empty() == false )
		//if( currentParentElement() && currentParentElement()->token() == Token::SCOPE_BEGIN )
		//if( shouldNewestScopeBeAParentElement() == true )
		/*Element* iter = our_scope_elem->previousElement();
		while(iter)
		{
			if( iter->isFunc() || iter->token() == Token::ENUM || iter->token() == Token::CLASS || iter->token() == Token::NAMESPACE)
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
		*/
		
		//put the scope or class or func, or init_data object to scope stack.
		if(push_to_scope_stack)
			scopeElementStackPush( m_currentParentElement ); //NOT true anymore: but we put the class or func (or limiting scope) in the stack. Not the actual scope object (unless it is a limiting scope.)
	}
	protected: Element* m_currentParentElement;
	
	vector<Element*> scopeElementStack; //sort of like a parentElementStack

	vector<Element*> parenthesisStack;

	vector<Element*> bracketStack; // also used as a sort of templateStack and arrayDefineStack!

public:
	
	bool isWriteToFile; // When set to false, we don't generate any output code for this module/SourceParser object.
	bool isFileBased;

	FILE* currentFile;
	string currentFilename;
	std::string currentFilenamePath;

	string moduleName(string separator_char_str = ".")
	{
		if(currentModule == 0)
		{
			return "no module";
		}

		string a_module_name = "";

		int not_on_first = 0;

		for(Element* elem : currentModule->elements)
		{
			if( elem->token() == Token::MODULE_DIR || elem->token() == Token::MODULE_NAME )
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
				break;
			}
		}

		return a_module_name;
	}

	// compares modules as strings: "rae.examples.Tester"
	bool isModuleOnListOfImports(string set_module)
	{
		#ifdef DEBUG_RAE_PARSER
		cout<<"isModuleOnListOfImports: module: "<<set_module<<"\n";
		#endif
		for(Element* an_import : listOfImports)
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
		currentFilenamePath = currentFilename;
	}
	
	void parse()
	{
		cout<<"\n";
		rlutil::setColor(rlutil::BROWN);
		cout<<"Parsing: ";
		rlutil::setColor(rlutil::WHITE);
		cout<<currentFilenamePath<<"\n";

		if(currentFilenamePath == "")
			return;

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
		if(isWriteToFile == false)
			return;

		for( Element* module_elem : elements )
		{
			if( module_elem->token() != Token::MODULE )
			{
				#ifdef DEBUG_RAE_HUMAN
					cout<<"Interesting: we have something in top hierarchy: "<<module_elem->tokenString()<<"\n";
				#endif
				continue; //skip it
			}

			string module_filename;
			string module_name; 

			for( Element* elem : module_elem->elements )
			{
				if( elem->token() == Token::MODULE_DIR || elem->token() == Token::MODULE_NAME )
				{
					module_name += elem->name();
					module_name += "/"; //a directory separator
				}
				else
				{
					break;
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

			string outputHeaderPath = module_filename;
			//outputHeaderPath.replace_extension(".hpp");
			outputHeaderPath = replaceExtension(outputHeaderPath, ".hpp");

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

			createPathIfNotExist(parentPath(outputHeaderPath));
			createPathIfNotExist(parentPath(outputFilePath));

			StringFileWriter writer;
			writer.create( outputHeaderPath, /*isHeader*/true );

			writer.writeString("// this file is automatically created from Rae programming language module:\n//");
			writer.writeString( module_filename );
			writer.writeString("\n");
			
			
			writeElement(writer, *module_elem);
				//writer.writeString( elem->name() );
				//fwrite(&record.name,sizeof(char),record.nameLength,fp);
			
			//fwrite( buffer, sizeof(char), sizeof(buffer), pFile);
				
			writer.close();

			StringFileWriter writer2;
			writer2.create( outputFilePath, /*isHeader*/false );

			writer2.writeString("// this file is automatically created from Rae programming language module:\n//");
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
				string debugFilePath = module_filename;
				debugFilePath = replaceExtension(debugFilePath, ".debugtree");

				createPathIfNotExist(parentPath(debugFilePath));

				StringFileWriter writer3;
				writer3.create( debugFilePath, /*isHeader doesn't matter here*/false );

				writer3.writeString("// this file is automatically created from Rae programming language module:\n//");
				writer3.writeString( module_filename );
				writer3.writeString("\n");
				writeDebugTree(writer3, *module_elem);
				writer3.close();
            #endif
            #ifdef DEBUG_RAE_DEBUGTREE2
				debugFilePath = replaceExtension(debugFilePath, ".debugtree2");

				createPathIfNotExist(parentPath(debugFilePath));

				StringFileWriter writer4;
				writer4.create( debugFilePath, /*isHeader doesn't matter here*/false );

				writer4.writeString("// this file is automatically created from Rae programming language module:\n//");
				writer4.writeString( module_filename );
				writer4.writeString("\n");
				writeDebugTree2(writer4, *module_elem);	
				writer4.close();
			#endif
			//end debugtree
			
			
			
			//cout<<"\n\nheader: "<<outputHeaderPath<<"\n";
			//cout<<"file: "<<outputFilePath<<"\n";
			
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

		}//end for modules elements...
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
		if (isPleaseRehandleChar)
		{
			isPleaseRehandleChar = false;
			handleSlash = "";
			// rehandle currentChar. Currently only because of minus handling.
			return handleChar();
		}
		else // normal handling:
		{
			lineNumber.totalCharNumber++; // Ok, 1 is the first element.
			lineNumber.column++; // 1 is the first char in line. (unlike in C arrays...)

			handleSlash = "";
			handleSlash += currentChar; // put the char we had in the last "frame" here. So it's the char before the currentChar
			// that we read in the next line.
			beforeChar = currentChar; // the same but with an int.
		
			if( EOF != (currentChar = getc(currentFile)) )
			{
				return handleChar();
			}
			//else
				isDone = true; // Quit parsing
				return false;
		}
		return false;
	}

	bool readCharFromString()
	{
		if (isPleaseRehandleChar)
		{
			isPleaseRehandleChar = false;
			handleSlash = "";
			// rehandle currentChar. Currently only because of minus handling.
			return handleChar();
		}
		else // normal handling:
		{
			lineNumber.totalCharNumber++; // Ok, 1 is the first element.
			lineNumber.column++; // 1 is the first char in line. (unlike in C arrays...)

			handleSlash = "";
			handleSlash += currentChar; // put the char we had in the last "frame" here. So it's the char before the currentChar
			// that we read in the next line.
			beforeChar = currentChar; // the same but with an int.
		
			if( stringIndex < stringBasedSourceText.size() )
			{
				currentChar = stringBasedSourceText.at(stringIndex);
				stringIndex++;
				return handleChar();
			}
			//else
				isDone = true; // Quit parsing
				return false;
		}
		return false;
	}

	unsigned stringIndex;

	// return false if EOF or other error...
	// puts result in currentChar.
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
			
			#if defined(DEBUG_DEBUGMODULENAME)
				if (moduleName() == g_debugModuleName)
					cout << " cur: " << (char)currentChar << " word: " << currentWord << " wholeToken: " << wholeToken << "\n";
			#endif

			if( currentChar == '\n')
			{
				//just handling the linenumbers here, so that none get lost.
				lineNumber.line++;
			}
		
			if( currentChar == '\r')
			{
				// TODO report carriage return once per compile for the first file that contains it...
				//cout << "STRANGE CHAR! Ignore it. It's probably carriage return.\n";
				return false;
			}
	
			if( isPassthroughMode == true || isPassthroughSourceMode == true )
			{
				if( isWaitingForPragmaToken == true )
				{
					//enter or space (or slash for comments) to end pragma.
					if( isWhiteSpace(currentChar) || currentChar == '/' )
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
				if( currentChar == '/' || currentChar == '#' )
				{
					if( handleSlash == "+" )
					{
						//one ending plus and slash.
						nroWaitingForPlusCommentEnd--;

						//we need to have a matching number of plus comments to end commenting.
						if( nroWaitingForPlusCommentEnd == 0 )
						{
							wholeToken = "+";
							wholeToken += currentChar;
							isWholeToken = true;
							currentWord = "";
							
							//currentLine += currentChar;

							//currentPlusComment += currentChar;
							currentPlusComment.pop_back();
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
					if( handleSlash == "/" || handleSlash == "#" )
					{
						nroWaitingForPlusCommentEnd++;
					}
				}
				else if( currentChar == '*' )
				{
					//start a star comment, but we don't want to do that.
					if( handleSlash == "/" || handleSlash == "#" )
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
				if( currentChar == '/' || currentChar == '#')
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
							
							wholeToken = "*";
							wholeToken += currentChar;
							isWholeToken = true;
							currentWord = "";
							
							//currentLine += currentChar;

							isWaitingForStarCommentEnd = false;
							currentStarComment.pop_back();
							//currentStarComment += currentChar;
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
					//createComment(endOfLine);
					createComment(currentComment);
					currentComment = "";
					
					wholeToken = "\n";
					isWholeToken = true;
					///////////////////////////////////////createNewLine();
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
				if( handleSlash == "/" || handleSlash == "#" )
				{
					if( nroWaitingForPlusCommentEnd == 0 )
					{
						//start plus comment
						#ifdef DEBUG_COMMENTS
							cout<<"Start plus_comment HandleSlash /+\n";
						#endif

						////////Extended comment.
						///////wholeToken = "/*";
						///////isWholeToken = true;
						//////currentWord = "";
						//handleSlash = "";
						
						currentPlusComment = "";
						//currentPlusComment = handleSlash;
						//currentPlusComment += "+";
						nroWaitingForPlusCommentEnd++;
						return true;
					}
				}
				else if(isWaitingForPragmaToken == false)
				//a very special case for @c++ so that the plusses don't get
				//passed through as tokens. NOTE: I bet this is deprecated now, and the pragmas are @cpp...
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
				if( handleSlash == "/" || handleSlash == "#" )
				{
					if( isWaitingForStarCommentEnd == false )
					{
						//start comment
						#ifdef DEBUG_COMMENTS
							cout<<"Start star_comment HandleSlash /*\n";
						#endif

						////////Extended comment.
						///////wholeToken = "/*";
						///////isWholeToken = true;
						//////currentWord = "";
						//handleSlash = "";
						
						currentStarComment = "";
						//currentStarComment = handleSlash;
						//currentStarComment += currentChar;
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
					return true; // WHY? Why not...
				}
			}

// handle simple one line comments:

			if( handleSlash == "#" )
			{
				if(currentChar == '*')
				{
					assert(0);
					// We handled this already
				}
				else if(currentChar == '+')
				{
					assert(0);
					// We handled this already
				}
				else
				{
					wholeToken = currentWord;//"#"
					isWholeToken = true;

					currentWord = string("");

					isPleaseRehandleChar = true;

					return true;
				}
			}

			/*if( currentChar == '#' )
			{
				#ifdef DEBUG_COMMENTS
					cout << "Start single_line_comment #\n";
				#endif

				wholeToken = "#";
				isWholeToken = true;
				currentWord = "";

				isSingleLineComment = true;

				currentComment = "";

				return true;
			}*/

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

					currentComment = "";

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
				//whitespace: space, enter or tab (or slash for comments) ends a pragma token.
				if( isWhiteSpace(currentChar) || currentChar == '/' )
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

					// Beware of this bug: causes some strange behaviour
                    //currentWord = "" + currentChar;
					currentWord = string("");

					isPleaseRehandleChar = true;

					return true;
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

				return true;
			}
			else if( currentChar == '#' )
			{
				//Don't do anything. our # will be in the poorly named
				//handleSlash, which is really pastLetters...

				//OK. We'll do something. Cut the incoming word and send it:
				wholeToken = currentWord;
				isWholeToken = true;

				currentWord = "#"; //Oh, we'll put it here too...
				currentLine += currentChar;//We'll still put the minus here...

				return true;
			}
			/*
			else if( currentChar == '#' )
			{
				wholeToken = currentWord;
				isWholeToken = true;
				
				wholeToken2 = "#";
				isWholeToken2 = true;
				currentWord = "";
				
				currentLine += currentChar;
			}*/
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
					// Extended+ comment.
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
				createNumber(num);
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

public:
	ParserType parserType() { return m_parserType; }
	void parserType(ParserType set) { m_parserType = set; }
	protected: ParserType m_parserType;	
public:
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
						#ifdef DEBUG_RAE_TOKENIZER
							if(!isWhiteSpace(wholeToken))
								cout << "TOKEN:>" << wholeToken << " line: " << lineNumber.toString() << " mod: " << moduleName() << "<\n";
						#endif
						isWholeToken = false;
						//cout<<"calling handleToken on wholeToken1\n";
						if (parserType() == ParserType::RAE)
							handleToken(wholeToken);
						else if (parserType() == ParserType::CPP)
							handleTokenCpp(wholeToken);
					}
					else isWholeToken = false;
				}
				if( isWholeToken2 )
				{
					if( wholeToken2 != "" )//ignore empty tokens...
					{
						#ifdef DEBUG_RAE_TOKENIZER
							if(!isWhiteSpace(wholeToken2))
								cout << "TOKEN2:>" << wholeToken2 << " line: " << lineNumber.toString() << " mod: " << moduleName() << "<\n";
						#endif
						isWholeToken2 = false;
						//cout<<"calling handleToken on wholeToken2!!!!\n";
						if (parserType() == ParserType::RAE)
							handleToken(wholeToken2);
						else if (parserType() == ParserType::CPP)
							handleTokenCpp(wholeToken2);
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
				createClass(wholeToken);
				//wholeToken = "";
				expectingToken = Token::UNDEFINED;
			}
		}
		else if( expectingToken() == Token::COMMENT )
		{
			if( readChar() && isEndOfLine )
			{
				isEndOfLine = false;
				createComment(endOfLine);
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
					//createClass(wholeToken);
					//wholeToken = "";
					expectingToken = Token::UNDEFINED;
				}
				else cout<<"waiting real func on next line.\n";
	*/		
				/*if( wholeToken[0] != '(' )
				{
					cout<<"nu12938719238719273 func: "<<wholeToken<<"\n";
					//createClass(wholeToken);
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

	// These all return true if they handled the token. False if not.
	bool handleTokenRaeCppCommon(string set_token);
	// parse C++ tokens
	bool handleTokenCpp(string set_token);
	// parse Rae tokens
	bool handleToken(string set_token);

	
	//return true if it was a number...
	bool handleNumber(string set_token)
	{
		if(set_token.size() == 0)
			return false;

		if( isNumericChar(set_token[0]) )
		{
			createElement(Token::NUMBER, Kind::UNDEFINED, set_token);

			#ifdef DEBUG_RAE_HUMAN
				cout<<"createNumber: "<<set_token<<"\n";
			#endif

			return true;
		}

		return false;
	}

	void addToClasses(Element* set_elem)
	{
		classes.push_back(set_elem);
	}

	void addToUserDefinedTokens(Element* set_elem)
	{
		userDefinedTokens.push_back(set_elem);
	}

	void addToCheckForPreviousDefinitionsList(Element* set_elem)
	{
		checkForPreviousDefinitionsList.push_back(set_elem);
	}

	void addToUnknownDefinitions(Element* set_elem)
	{
		unknownDefinitions.push_back(set_elem);
	}

	//this also add to unknownUseReferences, but we don't really know what it is... might be helpful later. or not.
	void addToUnknowns(Element* set_elem)
	{
		unknownUseReferences.push_back(set_elem);
	}

	void addToUnknownUseReferences(Element* set_elem)
	{
		unknownUseReferences.push_back(set_elem);
	}

	void addToUnknownUseMembers(Element* set_elem)
	{
		unknownUseMembers.push_back(set_elem);
	}

	static bool isKnownType(Element* set)
	{
		if (!set)
			return true;
		return !set->isUnknownType();
	}

	void cleanUpUnknownTokens(vector<Element*>& unknown_tokens)
	{
		#ifdef DEBUG_RAE_PARSER
		cout<<"unknown_tokens.size before remove: "<<unknown_tokens.size()<<"\n";
		#endif
		
		// remove unknown tokens that are now known:
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
		
		// remove unknown tokens that are now known:
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
		
		// remove unknown tokens that are now known:
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
		
		// remove unknown tokens that are now known:
		unknownUseMembers.erase(std::remove_if(unknownUseMembers.begin(),
                         unknownUseMembers.end(),
						 isKnownType ),
        unknownUseMembers.end());

		#ifdef DEBUG_RAE_PARSER
		cout<<"unknownUseMembers.size after remove: "<<unknownUseMembers.size()<<"\n";
		#endif
	}

	/*
	void addToUnknownUserTokens(Element* set_elem)
	{
		unknownUserTokens.push_back(set_elem);
	}
	*/

	/*
	Element* searchUnknownUserTokens(string set_token)
	{
		//TODO this is very inefficient... just a draft. use a map... or something.
		for(Element* elem : unknownUserTokens)
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
	Element* searchUserDefinedTokens(string set_token)
	{
		//TODO this is very inefficient... just a draft. use a map... or something.
		for(Element* elem : userDefinedTokens)
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

	Element* classHasFunctionOrValue(string set_class, string set_name)
	{
		Element* found_class = 0;

		#ifdef DEBUG_RAE_PARSER
		cout<<"looking for class: "<<set_class<<" val or func: "<<set_name<<"\n";
		#endif

		#ifdef DEBUG_DEBUGNAME
			if (g_debugName == set_name || g_debugName == set_class)
				cout << "looking for class: " << set_class << " val or func: " << set_name << "\n";
		#endif

		for(Element* elem : userDefinedTokens)
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
					cout<<"not a class type...\n";
					#endif
				}
			}
		}

		if( found_class )
		{
			Element* found_func_or_val = found_class->searchName(set_name);
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

		/*for(Element* elem : classes)
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

	Element* searchElementDefinitionInScope(Element* set_elem, Element* search_from_scope)
	{
		if(set_elem == 0 || search_from_scope == 0)
			return 0;

		for(Element* elem : search_from_scope->elements)
		{
			if(set_elem == elem)
			{
				continue;
			}

			if( elem->token() == Token::NAMESPACE )
			{
				if( set_elem->name() == elem->name() )
				{
					cout << "THIS IS THE SAME NAMESPACE. " << set_elem->toSingleLineString() << " and earlier: " << elem->toSingleLineString() << "\n";;
				}
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
						cout<<"BUT IT'S NOT VALID!!!!\n";
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
		Element* prev_def = searchToken(set_token);
		if(prev_def)
		{
			if(previous2ndToken() == Token::OVERRIDE 
				|| previousToken() == Token::OVERRIDE )
			{
				//no errors.
			}
			else if(prev_def->token() != Token::MODULE_DIR || prev_def->token() != Token::MODULE_NAME)
			{
				//if( previousElement() )
				//	cout<<"PREVIOUSTOKEN WAS: "<<previousElement()->toString();

				ReportError::reportError("redefinition of name: " + set_token + "\nUse \"override\" to use the same name again.", previousElement() );
				ReportError::reportError("previous definition is here: ", prev_def );
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

	Element* checkForPreviousDefinitions(Element* set_elem)
	{
		Element* prev_def = searchElementAndCheckIfValid(set_elem);
		if(prev_def)
		{
			if(set_elem->previous2ndToken() == Token::OVERRIDE 
				|| set_elem->previousToken() == Token::OVERRIDE )
			{
				//no errors.
			}
			else if(prev_def->token() != Token::MODULE_DIR || prev_def->token() != Token::MODULE_NAME)
			{
				//if( previousElement() )
				//	cout<<"PREVIOUSTOKEN WAS: "<<previousElement()->toString();

				ReportError::reportError("redefinition of name:\nUse \"override\" to use the same name again:", set_elem );
				ReportError::reportError("previous definition is here: ", prev_def );
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
	void setNameAndCheckForPreviousDefinitions(Element* elem_to_set, string set_token)
	{
		if(elem_to_set == 0)
		{
			ReportError::reportError("Compiler error. setNameAndCheckForPreviousDefinitions. elem_to_set is null.", previousElement() );
			return;
		}

		//TODO check if name is valid!
		//if( isValidName(set) )

		Element* prev_def = searchToken(set_token);
		if(prev_def)
		{
			if(elem_to_set->previous2ndToken() == Token::OVERRIDE
				|| elem_to_set->previousToken() == Token::OVERRIDE )
			{
				//no errors.
			}
			else if(prev_def->token() != Token::MODULE_DIR || prev_def->token() != Token::MODULE_NAME)
			{
				ReportError::reportError("redefinition of name: " + set_token, elem_to_set );
				ReportError::reportError("previous definition is here: ", prev_def );
			}
			
			elem_to_set->name(set_token);
		}
		else
		{
			elem_to_set->name(set_token);
		}
	}
	
	Element* searchElementDefinitionFromParentScopes(Element* set_elem, Element* search_from_scope)
	{
		while(search_from_scope != 0)
		{
			Element* res = searchElementDefinitionInScope(set_elem, search_from_scope);
			if(res)
			{
				return res;
			}
			//continue searching:
			search_from_scope = search_from_scope->scope();
		}
		return 0;
	}

	// This should be good... a bit messy though.
	Element* searchElementAndCheckIfValidLocal(Element* set_elem)
	{
		#ifdef DEBUG_DEBUGNAME
			if (g_debugName == set_elem->name())
			{
				rlutil::setColor(rlutil::LIGHTBLUE);
				cout << "searchElementAndCheckIfValidLocal. module: " << moduleName() << " looking for: " << set_elem->name() << " " << set_elem->toSingleLineString() << "\n";
				rlutil::setColor(rlutil::WHITE);
			}
		#endif

		#ifdef DEBUG_RAE_PARSER
		//TEMP:
		//if(moduleName() == "rae.examples.Tester")
		//{
			cout<<"searching for: "<<set_elem->toString()<<"\n";
		//}
		#endif

		if( set_elem->scope() )
		{
			Element* res = searchElementDefinitionFromParentScopes(set_elem, set_elem->parent() );//scopeElementStack.back() );
			if(res)
			{
				#ifdef DEBUG_RAE_PARSER
				cout<<"searchElementAndCheckIfValidLocal. ScopeFinder found it.\n";
				#endif

				#ifdef DEBUG_DEBUGNAME
					if (g_debugName == set_elem->name())
					{
						rlutil::setColor(rlutil::GREEN);
						cout << "searchElementAndCheckIfValidLocal. module: " << moduleName() << " Found: " << set_elem->name() << " in: " << res->toSingleLineString() << "\n";
						rlutil::setColor(rlutil::WHITE);
					}
				#endif
				return res;
			}
		}

		//return searchUserDefinedTokens(set_token);

		//Then search in current module.
		for(Element* elem : userDefinedTokens)
		{
			if(set_elem == elem)
			{
				continue;
			}

			if( set_elem->isDefinition() && elem->token() == Token::CLASS )
			{
				if( elem->type() == set_elem->type() )
				{
					#ifdef DEBUG_RAE_PARSER
						cout<<"searchElementAndCheckIfValidLocal: found1: "<<elem->type()<<" : "<<elem->toString()<<"\n";
					#endif
					#ifdef DEBUG_DEBUGNAME
						if (g_debugName == set_elem->name())
							cout << "searchElementAndCheckIfValidLocal: found2: " << elem->type() << " : " << elem->toSingleLineString() << "\n";
					#endif
					if( checkIfTokenIsValidInCurrentContext(set_elem, elem) )
					{
						return elem;
					}
					else
					{
						#ifdef DEBUG_RAE_PARSER
							cout<<"BUT IT'S NOT VALID!\n";
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
			// We are only interested in definitions.
			else if( elem->isDefinition() ) //ok so we don't check for: set_elem->isUseReference()
			{
				#ifdef DEBUG_DEBUGNAME
					if (g_debugName == set_elem->name() || g_debugName == set_elem->type() || g_debugName == set_elem->typedefNewType() )
						cout << "searchElementAndCheckIfValidLocal: looking for: " << set_elem->toSingleLineString() << " and checking: " << elem->toSingleLineString() << "\n";
				#endif

				if( elem->name() == set_elem->name() )
				{
					#ifdef DEBUG_RAE_PARSER
					cout << "searchElementAndCheckIfValidLocal: found definition: " << elem->name() << " : " << elem->toString() << "\n";
					#endif
					#ifdef DEBUG_DEBUGNAME
						if (g_debugName == set_elem->name())
						{
							rlutil::setColor(rlutil::RED);
							cout << "searchElementAndCheckIfValidLocal: found3: " << elem->type() << " : " << elem->toSingleLineString() << "\n";
							rlutil::setColor(rlutil::WHITE);
						}
					#endif
					if( checkIfTokenIsValidInCurrentContext(set_elem, elem) )
					{
						return elem;
					}
					else
					{
						#ifdef DEBUG_DEBUGNAME
							if (g_debugName == set_elem->name())
							{
								rlutil::setColor(rlutil::RED);
								cout << "searchElementAndCheckIfValidLocal: FOUND IT BUT IT IS NOT VALID: " << elem->type() << " : " << elem->toSingleLineString() << "\n";
								rlutil::setColor(rlutil::WHITE);
							}
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

		//Then search in other modules outside this SourceParser.


		return 0;
	}

	Element* searchElementAndCheckIfValid(Element* set_elem);

	// A general search that will use other smaller searches to check everywhere.
	Element* searchToken(string set_token)
	{
		/*if(currentFunc)
		{
			Element* found_elem = currentFunc->searchFunctionParams(set_token);
			if(found_elem)
			{
				return found_elem;
			}
		}

		return searchUserDefinedTokens(set_token);
		*/

		//now we do a temporary Element* object so we can use the normal search tools. But for funny effect, we destroy the
		//temp element afterwards!

		//and then handleUserDefinedTokens creates another one with the calls that add it to the elements in the right places.

		Element* temp_elem = new Element(lineNumber, Token::USE_REFERENCE, Kind::UNDEFINED, set_token, set_token );
		temp_elem->isUnknownType(true);
		temp_elem->parent( currentParentElement() );
		temp_elem->previousElement( previousElement() );

		Element* res = searchElementAndCheckIfValid(temp_elem);

		delete temp_elem;

		return res;
	}

	bool isCompatibleNamespaces( Element* current_context_use, Element* found_definition )
	{
		//listOfUsingNamespaces ... something
		//current_context_use->namespaceSomething()()()
		return true;
	}

	bool checkIfTokenIsValidInCurrentContext( Element* current_context_use, Element* found_definition )
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

			// Here we check for the namespace being valid
			if( isCompatibleNamespaces(current_context_use, found_definition) == false )
			{
				return false;
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
					cout<<"checkIfTokenIsValidInCurrentContext: it's ok. it's valid. someclass.somefunc\n";
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
					cout<<"checkIfTokenIsValidInCurrentContext: not ok. no such symbol in class.\n";
					#endif
					return false;//now we know that the class didn't have that symbol. or it isn't yet defined.
				}
			}
			else if( current_context_use->previousToken() == Token::REFERENCE_DOT )
			{
				//This is the array case, but we could use this for the case up there too. This is more recent anyway...
				//But I guess the case up there is a bit simpler so we might just leave it there too.

				Element* prev_ref = current_context_use->searchClosestPreviousUseReference();
				if(prev_ref)
				{
					//////////if( prev_ref->kind() == Kind::TEMPLATE )
					if( prev_ref->type() == "array" )
					{
						if( prev_ref->definitionElement() )
						{
                            // TODO fix comments:
							//useVectors definitionElement would be
							//vector!(Tester) useVector
							//and its templateSecondType is Tester, if it is found already at this point in parsing.
							Element* temp_second_type = prev_ref->definitionElement()->templateSecondType();
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
									cout<<"checkIfTokenIsValidInCurrentContext: no templateSecondType in definitionElement, in useVector.\n";
									cout<<"definitionElement was: "<<prev_ref->definitionElement()->toString()<<"\n";
									cout<<"prev_ref was: "<<prev_ref->toString()<<"\n";
								#endif
								return false;
							}
						}
						else
						{
							#ifdef DEBUG_RAE_PARSER
								cout<<"checkIfTokenIsValidInCurrentContext: no definitionElement in useVector.\n";
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
							cout<<"checkIfTokenIsValidInCurrentContext: VECTOR reference thing: it's ok. it's valid. somevector[i].somefunc\n";
							#endif
							return true;
						}
						else
						{
							#ifdef DEBUG_RAE_PARSER
							cout<<"checkIfTokenIsValidInCurrentContext: Vector call: not ok. no such symbol in class.\n";
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
							ReportError::reportInfo("NOT valid different parent funcs. parents: ", current_context_use->parentFunc(), found_definition->parentFunc());
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
						//so maybe we'll add the visibility as a param to Element, and only VISIBILITY_DEFAULT will be put on the list of
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
			Element* our_array_definition = bracketStack.back();

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
		if( handleNumber(set_token) )
			return;

		#ifdef DEBUG_RAE_HUMAN
			cout<<"trying to handle: "<<set_token<<" in line: "<<lineNumber.line<<"\n";
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
				#endif
				createUnknownUseMember(set_token);
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
				#endif
				//createUnknownUseMember(set_token);
				//Hahaaa, we don't even have to create it!
				//previousElement()->token(Token::UNKNOWN_DEFINITION);
				previousElement()->token(Token::DEFINE_REFERENCE);
				previousElement()->type( previousElement()->name() ); //Your name is now your type: Tester
				previousElement()->name(set_token); //your name is now set_token: tester

				//setNameAndCheckForPreviousDefinitions

				addToUnknownDefinitions(previousElement());
				
				#ifdef DEBUG_RAE_HUMAN
					cout<<"Fixed UNKNOWN_DEFINITION is now: "<<previousElement()->toString()<<"\n";
				#endif

				//!!!!
				return;
			}
		}

		Element* our_new_element;

		Element* found_elem = searchToken(set_token);
		if( found_elem )
		{
			#ifdef DEBUG_RAE_HUMAN
				cout<<"Found user token: "<<found_elem->toString()<<" "<<found_elem->namespaceString()<<"\n";
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
					case Token::CPP_TYPEDEF:
						#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_CPP_TYPEDEF)
							cout << "Found typedef usage111111!!!\n";
						#endif
					case Token::CLASS:

						if( bracketStack.empty() ) //not in the middle of a template list.
						{
							expectingToken(Token::DEFINE_REFERENCE_NAME);
						}

						if(currentReference == nullptr)
						{
							#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_REFERENCES)
								cout << "Default val case. Creating a createDefineReference for a class or typedef definition: " << set_token <<"\n";
							#endif
							//ReportError::reportError("handleUserDefinedToken. currentReference was null, when we found a class. Compiler error. set_token: " + set_token, previousElement() );
						
							// This case is just a class name without any preceeding val or opt, so no currentReference has been created.
							// We'll create a new reference that defaults to VAL.
							if( !bracketStack.empty() )
							{
								createDefineReference( "val", Role::TEMPLATE_PARAMETER, found_elem, set_token );
								// Not waiting for the name as it's a template parameter.
							}
							else
							{
								Element* our_ref = createDefineReference( "val", expectingRole(), found_elem, set_token );
								unfinishedElement(our_ref); // Still waiting for the name.
							}
						}
						else
						{
							#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_REFERENCES)
								cout << "We have some currentReference dangling. Setting it's class or typedef definition to " << set_token << ": " << currentReference->toSingleLineString() << "\n";
							#endif
							// expectingRole can be undefined for now. If it's set it's most likely
							// just FUNC_RETURN or FUNC_ARGUMENT. But this remark is early days, so
							// it might change in the future.

							//our_new_element = createDefineReference(expectingKind(), expectingRole(), found_elem, set_token);
							currentReference->definitionElement(found_elem);
							currentReference->type(set_token);

							if(nextElementWillGetNamespace)
							{
								currentReference->useNamespace(nextElementWillGetNamespace);
								nextElementWillGetNamespace = nullptr;
							}

							/*if( clob->parentToken() == Token::CLASS )
							{
								//we are inside a class definition, not a func:
								clob->
							}*/
						}
					break;
					case Token::FUNC:
						our_new_element = createFuncCall(found_elem);
					break;
					case Token::FUNC_CALL:
						cout<<"We should not find FUNC_CALLs...\n";
						assert(0);
					break;
					case Token::BRACKET_DEFINE_ARRAY_BEGIN:
					case Token::BRACKET_DEFINE_STATIC_ARRAY_BEGIN:
					case Token::DEFINE_FUNC_RETURN:
					//REMOVED: case Token::DEFINE_FUNC_ARGUMENT:
					//case Token::DEFINE_REFERENCE_IN_CLASS:
					case Token::DEFINE_REFERENCE:
					//case Token::DEFINE_BUILT_IN_TYPE_IN_CLASS:
					//case Token::DEFINE_BUILT_IN_TYPE:
						our_new_element = createUseReference(found_elem);
					break;
					case Token::USE_REFERENCE:
						ReportError::reportError("handleUserDefinedToken. Found USE_REFERENCE. And we're not supposed to find those.", previousElement() );
					break;
					case Token::CPP_PRE_DEFINE:
					{
						#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_CPP_PREPROCESSOR)
							cout << "Found #define1111 usage!!!" << set_token << "\n";
						#endif

						Element* our_ref = createElement( Token::USE_REFERENCE, Kind::VAL, set_token, set_token);
						our_ref->definitionElement(found_elem);
					}
					break;
					case Token::ALIAS:
						#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_ALIAS)
							cout << "FOUND use of ALIAS1111. " << set_token << " found_elem: " << found_elem->toSingleLineString();
							if (found_elem->definitionElement())
								cout << " definition: " << found_elem->definitionElement()->toSingleLineString() << "\n";
							else cout << " but no definitionElement for alias.\n";
						#endif

						//assert(0);

						// Hmm. I wonder if we can just re-handle the alias use as the old_type...
						//recursive:
						return handleUserDefinedToken( found_elem->typedefOldType() );
					break;
					case Token::NAMESPACE:
						#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_NAMESPACE)
							rlutil::setColor(rlutil::BLUE);
							cout << "11111111handleUserDefinedToken found a namespace: " << found_elem->name() << " set_token: " << set_token << "\n";
							rlutil::setColor(rlutil::WHITE);
						#endif
						
						nextElementWillGetNamespace = found_elem;
						isWaitingForNamespaceDot = true;

						// If we have a currentReference and it's name is set:
						// val Type something = glm::Type(1,2,3)
						// so this namespace is after assignment maybe...
						/*
						if (currentReference == nullptr || currentReference->name() != "")
						{
							currentReference = nullptr; // just in case we got name() != "" case, as mentioned above.

							// In fact, this could be:
							// a type definition: Rae::SomeType, which would then default to be of kind val
							// a func call: Rae::CallSomeFunc
							// array definition: [Rae::SomeType]
							if( !bracketStack.empty() )
							{
								Element* our_ref = createDefineReference( "val", Role::TEMPLATE_PARAMETER );
								our_ref->useNamespace(found_elem);
								cout << "Namespace stuff created1: " << our_ref->toSingleLineString() << "\n";
							}
							else
							{
								Element* our_ref = createElement( Token::USE_NAMESPACE, Kind::UNDEFINED, set_token, set_token);
								our_ref->useNamespace(found_elem);
								unfinishedElement(our_ref); // Still waiting for the name etc.
								cout << "Namespace stuff created2: " << our_ref->toSingleLineString() << "\n";
							}
							
							isWaitingForNamespaceDot = true;
							cout << "isWaitingForNamespaceDot1 with " << found_elem->toSingleLineString() << "\n";
						}
						else
						{
							cout << "And we have a currentReference dangling." << currentReference->toSingleLineString() << "\n";
							//Element* lang_elem = new Element(lineNumber, Token::USE_NAMESPACE, Kind::UNDEFINED, set_token);
							//lang_elem->definitionElement(found_elem);

							cout << "setting useNamespace to: " << found_elem->name() << "\n";
							currentReference->useNamespace(found_elem);

							isWaitingForNamespaceDot = true;

							cout << "isWaitingForNamespaceDot2 with " << found_elem->toSingleLineString() << "\n";
						}

						//if (unfinishedElement())
						//	cout << "And we have an unfinishedElement dangling." << unfinishedElement()->toSingleLineString() << "\n";
						
						rlutil::setColor(rlutil::WHITE);
						//Element* lang_elem = createElement(Token::USE_NAMESPACE, Kind::UNDEFINED, set_token);
						//lang_elem->definitionElement(found_elem);
						*/
                    break;

				}
			/*}
			else //token is not valid in this context
			{
				//TODO this is not really an error yet. We just need some debugging cout here...
				//The error will come if it's still not found after unknownrefs are checked.
				ReportError::reportError("Maybe not an error... Token is not valid in this context: " + Token::toString(set_token) );
				createUnknownUseReference(set_token);
			}*/
		}
		else
		{
			#ifdef DEBUG_RAE_HUMAN
				cout<<"Didn't find: "<<set_token<<" creating unknown ref.\n";
			#endif
			#ifdef DEBUG_DEBUGNAME
				if( set_token == g_debugName )
				{
					rlutil::setColor(rlutil::RED);
					cout << "handleUserDefinedToken Didn't find: " << set_token << " in line: " << lineNumber.line << " module: " << moduleName() << "\n";
					rlutil::setColor(rlutil::WHITE);
				}
			#endif
			//specifically don't do our_new_element = , because this is already unknown and will be handled later...
			//Oh well. found_elem would be null anyway...
			if(currentReference == nullptr)
			{
				#ifdef DEBUG_RAE_HUMAN
					cout<<"createUnknownUseReference2: "<<set_token<<" creating unknown ref.\n";
				#endif
				createUnknownUseReference2(set_token);
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
				createUnknownUseReference2(set_token);
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
		#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_UNKNOWN)
			cout << "handleUnknownTokens START.\n";
		
			cout << "unknownDefinitions: " << unknownDefinitions.size() << "\n";
			cout << "unknownUseReferences: " << unknownUseReferences.size() << "\n";
			cout << "unknownUseMembers: " << unknownUseMembers.size() << "\n";
		#endif

		/*if( unknownDefinitions.empty() && unknownUseReferences.empty() && unknownUseMembers.empty() )
		{
			return;
		}*/

		//handleUnknownDefinitions();
		//handleUnknownUseReferences();
		//handleUnknownUseMembers();

		for( uint i = 0; i < 4; ++i )
		{
			#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_UNKNOWN)
				rlutil::setColor(rlutil::RED);
				cout << "handleUnknownTokens time: " << i << "\n";
				rlutil::setColor(rlutil::WHITE);
			#endif
			
			handleUnknownTokens( unknownDefinitions );
			handleCheckForPreviousDefinitionsList();
			handleUnknownTokens( unknownUseReferences );
			handleUnknownTokens( unknownUseMembers );

			#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_UNKNOWN)
				if( unknownDefinitions.empty() && unknownUseReferences.empty() && unknownUseMembers.empty() )
				{
					//if(i > 0)
					ReportError::reportInfo("Parsed module " + numberToString(i+1) + " times. All unknown references found.", moduleName() );
					return;
				}
			#endif
		}

		if( !unknownDefinitions.empty() && !unknownUseReferences.empty() && !unknownUseMembers.empty() )
		{
			ReportError::reportError("After parsing the source 4 times, there are still " + numberToString(unknownDefinitions.size() + unknownUseReferences.size() + unknownUseMembers.size()) + " unknown references.", 0, moduleName() );
		}
		#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_UNKNOWN)
			else cout << "All unknowns handled OK.\n";
		#endif
	}

	void handleCheckForPreviousDefinitionsList()
	{
		for(Element* lang_elem : checkForPreviousDefinitionsList)
		{
			//if( checkForPreviousDefinitions(lang_elem) == 0 )
			//string temp_name = lang_elem->name();
			//lang_elem->name("");
			//we take the name away so that it won't find itself.
			//we have to use searchToken version, because we want to find
			//name, name. not type, name, because that will just find the
			//class...
			//Should redesign the search tools.

			//ignore all of that. except should redesign... now using Element* version
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

	void tryToConnectDefinitionToNextReferenceDot(Element* our_unknown_elem, Element* definition_elem )
	{
		if(our_unknown_elem && our_unknown_elem->isUnknownType() == true && definition_elem)
		{
			#ifdef DEBUG_RAE_HUMAN
			cout<<"Yes another nextElement and it is unknown!!!!!\n";
			#endif
			Element* maybe_member = definition_elem->hasFunctionOrMember(our_unknown_elem->name() );
			
			#ifdef DEBUG_RAE_HUMAN
			cout<<"looking for name: "<<our_unknown_elem->name()<<"\n";
			cout<<"from: "<<definition_elem->toString()<<"\n";
			definition_elem->printOutListOfFunctions();
			definition_elem->printOutListOfReferences();
			#endif

			if( maybe_member )
			{
				if( maybe_member->isFunc() )
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
					our_unknown_elem->kind( maybe_member->kind() );//Umm, could it be a vector?
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

	void handleUnknownTokens(vector<Element*>& unknown_tokens )
	{
		if( unknown_tokens.empty() )
		{
			return;
		}

		Element* found_elem;

		#ifdef DEBUG_RAE_HUMAN
			cout<<"\n\n"<<moduleName()<<" handleUnknownDefinitions() START.\n";
		#endif

		//for( Element* lang_elem : unknown_tokens )
		for(uint i = 0; i < unknown_tokens.size(); ++i) // Had to change this to a normal for, because ALIAS needed to re-handle one index and do a --i;
		{
			Element* lang_elem = unknown_tokens[i];

			if( lang_elem->isExtern() == true )
			{
				cout << "handleUnknownTokens() it is extern.\n";
				lang_elem->isUnknownType(false);
				continue;
			}

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
                    case Token::CPP_TYPEDEF:
						cout << "Found typedef usage22222!!!\n";
					case Token::CLASS:
						
						if(lang_elem->token() == Token::USE_REFERENCE
							&& lang_elem->kind() == Kind::UNDEFINED
							&& lang_elem->nextElement()
							&& lang_elem->nextElement()->isUnknownType() == true
							&& lang_elem->nextElement()->token() == Token::USE_REFERENCE)
						{
							cout<<"This really happened.\n";
							// For strange reasons we can say, that this is probably a case of default val:
							// Tester tester
							lang_elem->token(Token::DEFINE_REFERENCE);
							lang_elem->kind(Kind::VAL);

							lang_elem->definitionElement(found_elem);
							lang_elem->type( lang_elem->name() );

							lang_elem->name( lang_elem->nextElement()->name() );

							//if we are inside a class, we need to create auto_inits and auto_free to constructors and destructors.
							Element* current_scope = lang_elem->scope();
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
							if(lang_elem->kind() == Kind::UNDEFINED)
							{
								ReportError::reportError("SourceParser.handleUnknownTokens() Found undefined and setting it to REF. This is todo.", lang_elem);
								lang_elem->kind(Kind::REF);//This is probably not needed, if we get func_arguments to be created
								//as refs as default. We should actually know their typetype when we create them! FIXIT. TODO.
							}
							addToUserDefinedTokens(lang_elem);
						}
						else if(lang_elem->token() == Token::DEFINE_REFERENCE )
						{	/*
							if(lang_elem->kind() == Kind::C_ARRAY )//|| lang_elem->token() == Token::DEFINE_ARRAY_IN_CLASS)
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
								Element* current_scope = lang_elem->scope();
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
						//createElement(Token::FUNC_CALL, set_token);

						// This can certainly be found!

						//#ifdef DEBUG_RAE_HUMAN
						//////cout<<"\n\n\n\n\n\nDEFINITION FUNC_CALL Found. I bet this can be removed, because it shouldn't happen!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n\n\n\n";
						//////ReportError::reportError("DEFINITION FUNC_CALL Found. I bet this can be removed, because it shouldn't happen!!!!!!", lang_elem, found_elem );//lang_elem->previous2ndElement() );
						//#endif
						//assert(0);

						lang_elem->token(Token::FUNC_CALL);
						lang_elem->isUnknownType(false);
					break;
					case Token::BRACKET_DEFINE_ARRAY_BEGIN:
					case Token::BRACKET_DEFINE_STATIC_ARRAY_BEGIN:
					case Token::DEFINE_FUNC_RETURN:
					//REMOVED: case Token::DEFINE_FUNC_ARGUMENT:
					case Token::DEFINE_REFERENCE:
						// We don't create a new element, instead we modify the existing unknown element.
						lang_elem->token(Token::USE_REFERENCE);
						lang_elem->type( found_elem->type() );//copy the class type etc.
						lang_elem->kind( found_elem->kind() );//copy the typetype. ref, built_in, array, vector etc.
						lang_elem->containerType( found_elem->containerType() );
						lang_elem->isUnknownType(false);

						// mark that we found the definition for future use:
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
								Element* maybe_member = lang_elem->definitionElement()->definitionElement()->hasFunctionOrMember(lang_elem->nextElement()->nextElement()->name() );
								
								#ifdef DEBUG_RAE_HUMAN
								cout<<"looking for name: "<<lang_elem->nextElement()->nextElement()->name()<<"\n";
								cout<<"from: "<<lang_elem->definitionElement()->definitionElement()->toString()<<"\n";
								lang_elem->definitionElement()->definitionElement()->printOutListOfFunctions();
								lang_elem->definitionElement()->definitionElement()->printOutListOfReferences();
								#endif

								if( maybe_member )
								{
									if( maybe_member->isFunc() )
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
										lang_elem->nextElement()->nextElement()->kind( maybe_member->kind() );//Umm, could it be a vector?
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
							Element* bracket_end = lang_elem->nextElement()->searchFirst(Token::BRACKET_END);
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
					case Token::CPP_PRE_DEFINE:
						cout << "Found #define2222 usage!!!" << found_elem->toSingleLineString() << "\n";
						//assert(0);
						lang_elem->token(Token::USE_REFERENCE);
						lang_elem->kind(Kind::VAL);
						lang_elem->definitionElement(found_elem);
					break;
					case Token::ALIAS:
						#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_ALIAS)
							cout << "FOUND use of ALIAS2222. " << lang_elem->toSingleLineString() << "\nfound_elem: " << found_elem->toSingleLineString();
							if (found_elem->definitionElement())
								cout << "\ndefinition: " << found_elem->definitionElement()->toSingleLineString() << "\n";
							else cout << "\nbut no definitionElement for alias.\n";
						#endif

						//assert(0);

						// Convert in place:
						//if (found_elem->typedefOldType() == found_elem->name())
						//{
							#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_ALIAS)
								cout << "alias222 set name to alias name." << found_elem->typedefOldType() << "\n";
							#endif
							lang_elem->name(found_elem->typedefOldType());
							lang_elem->isUnknownType(true);
							lang_elem->definitionElement(found_elem->definitionElement());
						/*}
						else if (lang_elem->typedefOldType() == found_elem->type())
						{
							cout << "STRANGER alias222 set type to alias type." << found_elem->name() << "\n";
							lang_elem->type(found_elem->type());
						}*/
						#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_ALIAS)
							cout << "ALIAS going to step back in time: " << i;
						#endif
						--i;
						#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_ALIAS)
							cout << "alias i: -> " << i << "\n";;
						#endif
						continue;
					break;
					case Token::NAMESPACE:
						#if defined(DEBUG_RAE_PARSER) || defined(DEBUG_RAE_NAMESPACE)
							cout << "22222222handleUnknownTokens found a namespace: " << found_elem->name() << "\n";
						#endif
						lang_elem->token(Token::USE_NAMESPACE);
						lang_elem->definitionElement(found_elem);
						lang_elem->isUnknownType(false);
					break;
				}

				#ifdef DEBUG_RAE_HUMAN
					cout<<"fixed lang elem is: "<<lang_elem->toString()<<"\n";
				#endif
			}
			else
			{
				if(lang_elem->previousElement() && lang_elem->previousToken() == Token::REFERENCE_DOT )
				{
					#if (_MSC_VER == 1700) // Visual Studio 2012
						ReportError::reportWarning("Didn't find definition. Does the class have a member with that name? Check spelling? Did you define it somewhere? Is it accessible in this scope?", lang_elem );
					#else
						ReportError::reportWarning("Didn't find definition. Does the class have a member with that name? Check spelling? Did you define it somewhere? Is it accessible in this scope?", { lang_elem, lang_elem->previousElement(), lang_elem->previous2ndElement() } );
					#endif
				}
				else
				{
					ReportError::reportWarning("Didn't find definition. Check spelling? Did you define it somewhere? Is it accessible in this scope?", lang_elem );
				}

				#ifdef DEBUG_RAE_HUMAN
					cout<<"Didn't find: "<<lang_elem->toString()<<" it remains unknown DEFINITION.\n";
				#endif
				//createUnknownUseReference(set_token);
			}	
		}

		cleanUpUnknownTokens(unknown_tokens);
	}

	#include "SourceValidate.hpp"

	#include "SourceWriter.hpp"

};

}

#endif //RAE_COMPILER_SOURCEPARSER_HPP


