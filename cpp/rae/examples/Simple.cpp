//this file is automatically created from Rae programming language module:
///Users/joonaz/Dropbox/jonas/2014/ohjelmointi/rae_programming_language/cpp/rae/examples/Simple
#include "Simple.hpp"

//class Simple

Simple::~Simple()
{
}

Simple::Simple()
{
	nobirches = 0L;//line: 26
	notrees = 9;//line: 7
	
}

void Simple::forest(/*override:*/int32_t notrees)
{
	notrees = 0;//line: 13
	nobirches = 2;//line: 14
	
	std::cout<<"original: "<<notrees<<"\n";//line: 16
	
	{
		/*override:*/int32_t notrees = 5;//line: 19
		std::cout<<"another: "<<notrees<<"\n";
	}
	
	std::cout<<"original: "<<notrees<<"\n";//line: 23
	std::cout<<nobirches<<"\n";
}

//return type name: 
int32_t main(int argc, char* const argv[])
{
	Simple* simple = new Simple();//line: 34
	simple->forest(3);//line: 35
	std::cout<<"from main: "<<simple->notrees<<"\n";//line: 36
	delete simple;//line: 37
	
	return(0);
}
