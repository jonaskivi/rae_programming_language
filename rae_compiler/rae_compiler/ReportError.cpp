
#include "ReportError.hpp"
#include "LangElement.hpp"

	void ReportError::reportInfo(string set, LangElement* set_elem)
	{
		//set_elem->parseError(ParseError::SYNTAX_ERROR);
		rlutil::setColor(rlutil::LIGHTBLUE);
		cout<<"INFO: ";
		rlutil::setColor(rlutil::WHITE);
		cout<<set;

		if(set_elem)
		{
			cout<<" / line: "<<set_elem->lineNumber().line<<" / in: "<<set_elem->namespaceString();
		}

		cout<<"\n";
	}

	void ReportError::reportInfo(string set, string in_which_namespace)
	{
		//set_elem->parseError(ParseError::SYNTAX_ERROR);
		rlutil::setColor(rlutil::LIGHTBLUE);
		cout<<"INFO: ";
		rlutil::setColor(rlutil::WHITE);
		cout<<set;

		cout<<" / in: "<<in_which_namespace<<"\n";

		cout<<"\n";
	}

	/*
	void ReportError::reportWarning(string set, LangElement* set_elem)
	{
		//set_elem->parseError(ParseError::SYNTAX_ERROR);
		rlutil::setColor(rlutil::YELLOW);
		cout<<"WARNING: ";
		rlutil::setColor(rlutil::WHITE);
		cout<<set;

		if(set_elem)
		{
			cout<<" / line: "<<set_elem->lineNumber().line<<" / in: "<<set_elem->namespaceString();
		}

		cout<<"\n";

		ReportError::m_countWarnings++;		
	}
	*/

	void ReportError::reportWarning(string set, LangElement* set_elem)
	{
		rlutil::setColor(rlutil::YELLOW);
		cout<<"WARNING: ";
		rlutil::setColor(rlutil::WHITE);
		cout<<set<<"\n";

		if(set_elem)
		{
			set_elem->parseError(ParseError::SYNTAX_WARNING);
			cout<<"\tname: ";
			rlutil::setColor(rlutil::GREEN);
			cout<<set_elem->name();
			rlutil::setColor(rlutil::WHITE);
			cout<<"\n\t"<<set_elem->tokenString();
			cout<<"\n\t"<<"typetype: "<<set_elem->typeTypeString();
			cout<<"\n\t"<<"type: "<<set_elem->type();
			cout<<"\n\t"<<"containerType: "<<ContainerType::toString( set_elem->containerType() );
			cout<<"\n\tline: ";
			rlutil::setColor(rlutil::BROWN);
			cout<<set_elem->lineNumber().line;
			rlutil::setColor(rlutil::WHITE);
			cout<<" / in: ";
			rlutil::setColor(rlutil::BROWN);
			cout<<set_elem->namespaceString();
			rlutil::setColor(rlutil::WHITE);
		}
		
		cout<<"\n";
		
		ReportError::m_countWarnings++;
	}

	void ReportError::reportError(string set, LangElement* set_elem)
	{
		rlutil::setColor(rlutil::RED);
		cout<<"ERROR: ";
		rlutil::setColor(rlutil::WHITE);
		cout<<set<<"\n";

		if(set_elem)
		{
			/////////////////set_elem->parseError(ParseError::SYNTAX_ERROR);
			cout<<"\tname: ";
			rlutil::setColor(rlutil::GREEN);
			cout<<set_elem->name();
			rlutil::setColor(rlutil::WHITE);
			cout<<"\n\t"<<set_elem->tokenString();
			cout<<"\n\t"<<"typetype: "<<set_elem->typeTypeString();
			cout<<"\n\t"<<"type: "<<set_elem->type();
			cout<<"\n\t"<<"containerType: "<<ContainerType::toString( set_elem->containerType() );
			cout<<"\n\tline: ";
			rlutil::setColor(rlutil::BROWN);
			cout<<set_elem->lineNumber().line;
			rlutil::setColor(rlutil::WHITE);
			cout<<" / in: ";
			rlutil::setColor(rlutil::BROWN);
			cout<<set_elem->namespaceString();
			rlutil::setColor(rlutil::WHITE);
		}
		
		cout<<"\n";
		
		ReportError::m_countErrors++;
	}

	void ReportError::reportWarning(string set, std::initializer_list<LangElement*> set_elems)
	{
		rlutil::setColor(rlutil::YELLOW);
		cout<<"WARNING: ";
		rlutil::setColor(rlutil::WHITE);
		cout<<set<<"\n";

		bool is_first = true;

		for( LangElement* set_elem: set_elems )
		{
			if(set_elem)
			{
				if(is_first == true)
				{
					// Only the first one is set to be in error.
					set_elem->parseError(ParseError::SYNTAX_WARNING);
					is_first = false;
				}
				cout<<"\tname: ";
				rlutil::setColor(rlutil::GREEN);
				cout<<set_elem->name();
				rlutil::setColor(rlutil::WHITE);
				cout<<"\n\t"<<set_elem->tokenString();
				cout<<"\n\t"<<"typetype: "<<set_elem->typeTypeString();
				cout<<"\n\t"<<"type: "<<set_elem->type();
				cout<<"\n\t"<<"containerType: "<<ContainerType::toString( set_elem->containerType() );
				cout<<"\n\tline: ";
				rlutil::setColor(rlutil::BROWN);
				cout<<set_elem->lineNumber().line;
				rlutil::setColor(rlutil::WHITE);
				cout<<" / in: ";
				rlutil::setColor(rlutil::BROWN);
				cout<<set_elem->namespaceString();
				rlutil::setColor(rlutil::WHITE);
				cout<<"\n";
			}
		}

		ReportError::m_countWarnings++;
	}

	void ReportError::reportError(string set, std::initializer_list<LangElement*> set_elems)
	{
		rlutil::setColor(rlutil::RED);
		cout<<"ERROR: ";
		rlutil::setColor(rlutil::WHITE);
		cout<<set<<"\n";

		bool is_first = true;

		for( LangElement* set_elem: set_elems )
		{
			if(set_elem)
			{
				if(is_first == true)
				{
					// Only the first one is set to be in error.
					/////////////////set_elem->parseError(ParseError::SYNTAX_ERROR);
					is_first = false;
				}
				cout<<"\tname: ";
				rlutil::setColor(rlutil::GREEN);
				cout<<set_elem->name();
				rlutil::setColor(rlutil::WHITE);
				cout<<"\n\t"<<set_elem->tokenString();
				cout<<"\n\t"<<"typetype: "<<set_elem->typeTypeString();
				cout<<"\n\t"<<"type: "<<set_elem->type();
				cout<<"\n\t"<<"containerType: "<<ContainerType::toString( set_elem->containerType() );
				cout<<"\n\tline: ";
				rlutil::setColor(rlutil::BROWN);
				cout<<set_elem->lineNumber().line;
				rlutil::setColor(rlutil::WHITE);
				cout<<" / in: ";
				rlutil::setColor(rlutil::BROWN);
				cout<<set_elem->namespaceString();
				rlutil::setColor(rlutil::WHITE);
				cout<<"\n";
			}
		}

		ReportError::m_countErrors++;
	}

	void ReportError::reportError(string set, int set_line_number, string in_which_namespace)
	{
		rlutil::setColor(rlutil::RED);
		cout<<"ERROR: ";
		rlutil::setColor(rlutil::WHITE);
		cout<<set;

		cout<<" / line: "<<set_line_number<<" / in: "<<in_which_namespace<<"\n";
		
		ReportError::m_countErrors++;
	}

	void ReportError::compilerError(string set, LangElement* set_elem)
	{
		rlutil::setColor(rlutil::RED);
		cout<<"COMPILER ERROR: ";
		rlutil::setColor(rlutil::WHITE);
		cout<<set<<"\n";

		if(set_elem)
		{
			/////////////////set_elem->parseError(ParseError::COMPILER_ERROR);
			cout<<"\tname: ";
			rlutil::setColor(rlutil::GREEN);
			cout<<set_elem->name();
			rlutil::setColor(rlutil::WHITE);
			cout<<"\n\t"<<set_elem->tokenString();
			cout<<"\n\t"<<"typetype: "<<set_elem->typeTypeString();
			cout<<"\n\t"<<"type: "<<set_elem->type();
			cout<<"\n\t"<<"containerType: "<<ContainerType::toString( set_elem->containerType() );
			cout<<"\n\tline: ";
			rlutil::setColor(rlutil::BROWN);
			cout<<set_elem->lineNumber().line;
			rlutil::setColor(rlutil::WHITE);
			cout<<" / in: ";
			rlutil::setColor(rlutil::BROWN);
			cout<<set_elem->namespaceString();
			rlutil::setColor(rlutil::WHITE);
		}
		
		cout<<"\n";
		
		ReportError::m_countErrors++;
	}

	void ReportError::compilerError(string set)
	{
		//set_elem->parseError(ParseError::COMPILER_ERROR);
		rlutil::setColor(rlutil::RED);
		cout<<"COMPILER ERROR: ";
		rlutil::setColor(rlutil::WHITE);
		cout<<set<<"\n";
		
		ReportError::m_countErrors++;		
	}


	int ReportError::m_countWarnings = 0;
	int ReportError::m_countErrors = 0;


