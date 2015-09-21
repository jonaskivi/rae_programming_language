#ifndef RAE_COMPILER_REPORTERROR_HPP
#define RAE_COMPILER_REPORTERROR_HPP

#include <string>
	using namespace std;

// Not in Visual Studio 2012:
//#include <initializer_list>

namespace Rae
{

class Element;

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
		cout<<"RAE_ERROR: ";
		rlutil::setColor(rlutil::WHITE);
		cout<<set<<" / line: "<<lineNumber.line<<" / Module: "<<moduleName()<<"\n";
		//rae::log("RAE_ERROR: ", lineNumber.line, " ", set, "\n");
		m_countErrors++;
	}
	*/
	
	static void reportInfo(string set, Element* set_elem);
	static void reportInfo(string set, Element* set_elem, Element* set_elem2);
	static void reportInfo(string set, string in_which_namespace);
	static void reportWarning(string set, Element* set_elem);
//not in vs2012:
#if (_MSC_VER == 1700) // Visual Studio 2012
	// Nothing here...
#else
	static void reportWarning(string set, std::initializer_list<Element*> set_elems);
#endif

	static void reportError(string set, Element* set_elem);
	static void reportError(string set, Element* set_elem, Element* set_elem2);
//not in vs2012:
#if (_MSC_VER == 1700) // Visual Studio 2012
	// Nothing here...
#else
	static void reportError(string set, std::initializer_list<Element*> set_elems);
#endif
	static void reportError(string set, int set_line_number, string in_which_namespace);
	static void compilerError(string set, Element* set_elem);
	static void compilerError(string set);

	public: static int countWarnings() { return m_countWarnings; }
	public: static void countWarnings(int set) { m_countWarnings = set; }
	protected: static int m_countWarnings;
	
	public: static int countErrors() { return m_countErrors; }
	public: static void countErrors(int set) { m_countErrors = set; }
	protected: static int m_countErrors;
};

}

#endif


