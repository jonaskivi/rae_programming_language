#ifndef RAE_COMPILER_REPORTERROR_HPP
#define RAE_COMPILER_REPORTERROR_HPP

#include <string>
	using namespace std;

class LangElement;

class ReportError
{
public:
/*
	ReportError()
	: m_countWarnings(0),
	m_countErrors(0)
	{

	}
*/
	/*static void reportWarning(string set, LineNumber lineNumber)
	{
		cout<<"Warning: "<<set<<" / line: "<<lineNumber.line<<" / Module: "<<moduleName()<<"\n";
		//rae::log("Warning: ", lineNumber.line, " ", set, "\n");
		m_countWarnings++;
	}

	static void reportError(string set, LineNumber lineNumber)
	{
		rlutil::setColor(rlutil::RED);
		cout<<"ERROR: ";
		rlutil::setColor(rlutil::WHITE);
		cout<<set<<" / line: "<<lineNumber.line<<" / Module: "<<moduleName()<<"\n";
		//rae::log("ERROR: ", lineNumber.line, " ", set, "\n");
		m_countErrors++;
	}
	*/
	
	static void reportWarning(string set, LangElement* set_elem);
	static void reportError(string set, LangElement* set_elem);
	static void reportError(string set, int set_line_number, string in_which_namespace);
	static void compilerError(string set, LangElement* set_elem);
	static void compilerError(string set);

	public: static int countWarnings()
	{
		return m_countWarnings;
	}
	public: static void countWarnings(int set)
	{
		m_countWarnings = set;
	}
	protected: static int m_countWarnings;
	
	public: static int countErrors()
	{
		return m_countErrors;
	}
	public: static void countErrors(int set)
	{
		m_countErrors = set;
	}
	protected: static int m_countErrors;
};

		

#endif


