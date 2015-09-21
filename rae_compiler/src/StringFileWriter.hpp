#ifndef RAE_COMPILER_STRINGFILEWRITER_HPP
#define RAE_COMPILER_STRINGFILEWRITER_HPP

#include <stdio.h>

namespace Rae
{

namespace TripleOption
{
	enum e
	{
		NO,//0
		YES,//1
		NOT_YET//2
	};
}

class StringFileWriter
{
public:
	StringFileWriter()
	: m_lineNeedsEnding(""),
	m_parenthesisCount(0)
	{
		m_isFileOk = false;
		m_currentIndent = 0;

		m_previousElement = 0;
		m_previous2ndElement = 0;
		m_nextElement = 0;
		//m_previousToken = Token::UNDEFINED;
		//m_nextToken = Token::UNDEFINED;
		//m_previousPreviousToken = Token::UNDEFINED;

		m_isSkipLine = false;
		m_lineNeedsSemicolon = true;

		m_currentDefaultVisibility = "public";
		m_nextNeedsDefaultVisibility = true;
		//m_nextNeedsDefaultVisibility = TripleOption::YES;
	}

	FILE* outFile;

	public: bool isFileOk(){ return m_isFileOk; }
	protected: bool m_isFileOk;

	public: int currentIndentPlus()
	{
		#ifdef DEBUG_RAE_INDENTS
			cout<<"currentIndentPlus(): "<<(m_currentIndent + 1)<<"\n";
			writeChar('#');
		#endif
		m_currentIndent++; return m_currentIndent;
	}
	public: int currentIndentMinus()
	{
		#ifdef DEBUG_RAE_INDENTS
			cout<<"currentIndentMinus(): "<<(m_currentIndent - 1)<<"\n";
			writeChar('@');
		#endif 
		m_currentIndent--; return m_currentIndent;
	}
	public: int currentIndent() { return m_currentIndent; }
	public: void currentIndent(int set) { m_currentIndent = set; }
	protected: int m_currentIndent;

	public: LangElement* previousElement() { return m_previousElement; }
	public: void previousElement(LangElement* set) { m_previousElement = set; }
	protected: LangElement* m_previousElement;

	public: LangElement* previous2ndElement() { return m_previous2ndElement; }
	public: void previous2ndElement(LangElement* set) { m_previous2ndElement = set; }
	protected: LangElement* m_previous2ndElement;

	public: LangElement* nextElement() { return m_nextElement; }
	public: void nextElement(LangElement* set) { m_nextElement = set; }
	protected: LangElement* m_nextElement;

	public: Token::e previousToken()
	{
		if(m_previousElement == 0)
			return Token::UNDEFINED;
		return m_previousElement->token();
	}
	public: Token::e previous2ndToken()
	{
		if(m_previous2ndElement == 0)
			return Token::UNDEFINED;
		return m_previous2ndElement->token();
	}
	public: Token::e nextToken()
	{
		if(m_nextElement == 0)
			return Token::UNDEFINED;
		return m_nextElement->token();
	}

/*
	public: Token::e previousToken() { return m_previousToken; }
	public: void previousToken(Token::e set) { m_previousToken = set; }
	protected: Token::e m_previousToken;

	public: Token::e previousPreviousToken() { return m_previousPreviousToken; }
	public: void previousPreviousToken(Token::e set) { m_previousPreviousToken = set; }
	protected: Token::e m_previousPreviousToken;
	
	public: Token::e nextToken() { return m_nextToken; }
	public: void nextToken(Token::e set) { m_nextToken = set; }
	protected: Token::e m_nextToken;
*/

	public: bool writeLineEnding()
	{
		bool did_we_write_it = false;
		if( m_lineNeedsEnding != "" )
		{
			writeString( m_lineNeedsEnding );
			did_we_write_it = true;
			m_lineNeedsEnding = "";
		}
		return did_we_write_it;	
	}

	public: bool writeSemicolon()
	{
		bool did_we_write_it = false;
		if( lineNeedsSemicolon() == true
			&& parenthesisCount() <= 0 // This system is off for now. Got wrong results. Maybe try to fix it later?
			&& previousToken() != Token::SEMICOLON
			&& previousToken() != Token::NEWLINE
			&& previousToken() != Token::NEWLINE_BEFORE_SCOPE_END
			&& previousToken() != Token::CONSTRUCTOR
			&& previousToken() != Token::DESTRUCTOR
			&& previousToken() != Token::FUNC
			&& previousToken() != Token::MAIN

			&& previousToken() != Token::PARENTHESIS_BEGIN
			&& previousToken() != Token::PARENTHESIS_BEGIN_FUNC_PARAM_TYPES
			&& previousToken() != Token::PARENTHESIS_BEGIN_FUNC_RETURN_TYPES
			&& previousToken() != Token::LOG_SEPARATOR
			&& previousToken() != Token::PARENTHESIS_BEGIN_LOG
			&& previousToken() != Token::PARENTHESIS_BEGIN_LOG_S
			// Brackets didn't work here. Maybe try again later. Brackets are parent elements, so they contain their children,
			// and that's why previousToken will be a bracket even when there's children in between.
			//&& previousToken() != Token::BRACKET_BEGIN
			//&& previousToken() != Token::BRACKET_DEFINE_ARRAY_BEGIN
			//&& previousToken() != Token::BRACKET_DEFINE_STATIC_ARRAY_BEGIN
			//&& previousToken() != Token::BRACKET_INITIALIZER_LIST_BEGIN

			&& previousToken() != Token::COMMA
			&& previousToken() != Token::ASSIGNMENT

			//&& previousToken() != Token::SCOPE_BEGIN
			//&& previousToken() != Token::SCOPE_END
			//&& previousToken() != Token::FUNC
			//&& previousToken() != Token::DEFINE_FUNC_ARGUMENT
			//&& previousToken() != Token::DEFINE_FUNC_RETURN //not actually needed...
			//&& previousToken() != Token::CLASS
			//&& previousToken() != Token::COMMENT
			////&& previousToken() != Token::STAR_COMMENT
			//&& previousToken() != Token::VISIBILITY_DEFAULT
		)
		{
			//REMOVE assert(writer.previousElement() != nullptr);
			//cout<<"NEWLINE previousElement: "<<writer.previousElement()->toSingleLineString();
			//cout<<"\nlineNeedsSemicolon: "<<writer.lineNeedsSemicolon()<<"\n";

			writeChar(';');
			did_we_write_it = true;
		}
		lineNeedsSemicolon(true);
		return did_we_write_it;
	}

	public: int parenthesisCount() { return m_parenthesisCount; }
	public: void addParenthesisCount() { ++m_parenthesisCount; }
	public: void subParenthesisCount() { --m_parenthesisCount; }
	protected: void parenthesisCount(int set) { m_parenthesisCount = set; }
	protected: int m_parenthesisCount;

	public: bool lineNeedsSemicolon() { return m_lineNeedsSemicolon; }
	public: void lineNeedsSemicolon(bool set) { m_lineNeedsSemicolon = set; }
	protected: bool m_lineNeedsSemicolon;// = true;

	public: string lineNeedsEnding() { return m_lineNeedsEnding; }
	public: void lineNeedsEnding(string set) { m_lineNeedsEnding = set; }
	protected: string m_lineNeedsEnding;

	public: bool isSkipLine() { return m_isSkipLine; }
	public: void isSkipLine(bool set) { m_isSkipLine = set; }
	protected: bool m_isSkipLine;

	public: string& currentDefaultVisibility() { return m_currentDefaultVisibility; }
	public: void currentDefaultVisibility(string set) { m_currentDefaultVisibility = set; }
	protected: string m_currentDefaultVisibility;

	
	public: bool nextNeedsDefaultVisibility() { return m_nextNeedsDefaultVisibility; }
	public: void nextNeedsDefaultVisibility(bool set) { m_nextNeedsDefaultVisibility = set; }
	protected: bool m_nextNeedsDefaultVisibility;// = true;
	

	//public: TripleOption::e nextNeedsDefaultVisibility() { return m_nextNeedsDefaultVisibility; }
	//public: void nextNeedsDefaultVisibility(TripleOption::e set) { m_nextNeedsDefaultVisibility = set; }
	/*public: TripleOption::e nextNeedsDefaultVisibility() { return m_nextNeedsDefaultVisibility; }
	public: void switchNextNeedsDefaultVisibility()
	{
		if( m_nextNeedsDefaultVisibility == TripleOption::NO )
		{
			m_nextNeedsDefaultVisibility = TripleOption::NOT_YET;
		}
		else if( m_nextNeedsDefaultVisibility == TripleOption::YES)
		{
			m_nextNeedsDefaultVisibility = TripleOption::NO;
		}
		else if( m_nextNeedsDefaultVisibility == TripleOption::NOT_YET)
		{
			m_nextNeedsDefaultVisibility = TripleOption::YES;
		}
	}
	protected: TripleOption::e m_nextNeedsDefaultVisibility;// = true;
	*/
public:	
	void create(string set_filename, bool set_is_header )
	{
		#ifdef DEBUG_FILEWRITER
			cout<<"StringFileWriter.create() START."<<"\n";
		#endif

		m_isHeader = set_is_header;

		//FILE* headerFile;
		//char buffer[] = { 'x' , 'y' , 'z' };
		outFile = fopen(set_filename.c_str(), "w");
		
		if( outFile != NULL )
		{
			m_isFileOk = true;
		}
		else
		{
			cout<<"Unable to create file: "<<set_filename<<"\n";
			m_isFileOk = false;
		}

		#ifdef DEBUG_FILEWRITER
			cout<<"StringFileWriter.create() END."<<"\n";
		#endif
	}

	public: bool isHeader() { return m_isHeader; }
	public: void isHeader(bool set) { m_isHeader = set; }
	protected: bool m_isHeader; // = false;

public:	

	void close()
	{
		#ifdef DEBUG_FILEWRITER
			cout<<"StringFileWriter.close() START."<<"\n";
		#endif
		fclose(outFile);
		#ifdef DEBUG_FILEWRITER
			cout<<"StringFileWriter.close() END."<<"\n";
		#endif
	}
	
	void writeIndents()
	{
		#ifdef DEBUG_RAE_INDENTS
			cout<<"writeIndents(): "<<m_currentIndent<<"\n";
		#endif
		for( int i = 0; i < currentIndent(); i++ )
		{
			//TODO optional spaces for indentation... :)
			writeChar('\t');
		}
	}
	
	void writeString(const string& set)
	{
		fwrite( set.c_str(), sizeof(char), set.size(), outFile );
		//fwrite( "\n", sizeof(char), 1, outFile );
	}
	
	void writeChar(char set)
	{
		fwrite( &set, sizeof(char), 1, outFile );
	}
	
	void writeChars(const char* set, int count)
	{
		fwrite( set, sizeof(char), count, outFile );
	}
};

}

#endif //RAE_COMPILER_STRINGFILEWRITER_HPP





