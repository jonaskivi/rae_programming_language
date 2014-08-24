
#include "ReportError.hpp"
#include "LangElement.hpp"

	void ReportError::reportWarning(string set, LangElement* set_elem)
	{
		//set_elem->parseError(ParseError::SYNTAX_ERROR);
		rlutil::setColor(rlutil::YELLOW);
		cout<<"WARNING: ";
		rlutil::setColor(rlutil::WHITE);
		cout<<set<<" / line: "<<set_elem->lineNumber().line<<" / in: "<<set_elem->namespaceString()<<"\n";
		//rae::log("ERROR: ", lineNumber.line, " ", set, "\n");	
		
		ReportError::m_countErrors++;		
	}

	void ReportError::reportError(string set, LangElement* set_elem)
	{
		set_elem->parseError(ParseError::SYNTAX_ERROR);
		rlutil::setColor(rlutil::RED);
		cout<<"ERROR: ";
		rlutil::setColor(rlutil::WHITE);
		cout<<set<<" / line: "<<set_elem->lineNumber().line<<" / in: "<<set_elem->namespaceString()<<"\n";
		//rae::log("ERROR: ", lineNumber.line, " ", set, "\n");	
		
		ReportError::m_countErrors++;		
	}

	void ReportError::compilerError(string set)
	{
		//set_elem->parseError(ParseError::COMPILER_ERROR);
		rlutil::setColor(rlutil::RED);
		cout<<"ERROR: ";
		rlutil::setColor(rlutil::WHITE);
		cout<<set<<"\n";
		
		ReportError::m_countErrors++;		
	}


	int ReportError::m_countWarnings = 0;
	int ReportError::m_countErrors = 0;


