//this file is automatically created from Rae programming language module:
///Users/joonaz/Dropbox/jonas/2014/ohjelmointi/rae_programming_language/cpp/rae/examples/OptTester
#include "OptTester.hpp"

//class Inner

Inner::Inner()
{
	data = 5;
}

Inner::~Inner()
{
}

void Inner::logMe()
{
	std::cout<<"Hello ";
}


//class Tester

Tester::Tester()
{
	data = 5;//line: 30
	m_inner = new Inner();
}

Tester::~Tester()
{
	delete m_inner;
}

void Tester::logMe()
{
	std::cout<<"Hello ";
}

Inner* Tester::inner()
{
	return(m_inner);
}


//class OptTester

OptTester::OptTester()
{
	azero = 0;//line: 64
	initMeToo = 0.0;//line: 63
	afloat = 248.52;//line: 62
	anotherNumber = 42;//line: 61
	num25 = 4;//line: 60
	num = 5;//line: 69
	tester_ref = new Tester();
}

OptTester::~OptTester()
{
	delete tester_ref;
}

void OptTester::sayHello()
{
	tester.logMe();//line: 50
	std::cout<<"World!"<<"\n";//line: 51
	std::cout<<"tester_ref: "<<"\n";//line: 52
	tester_ref->logMe();
}

int32_t OptTester::count(int32_t param1, int32_t param2)
{
	return(param1 + param2);//or you could just do: result = param1 + param2
}

int32_t main(int argc, char* const argv[])
{
	OptTester hello; //semicolons are allowed, but not required.
	int32_t val;//line: 80
	int32_t another_zero;//line: 81
	float i_want_to_be_zero;//line: 82
	
	hello.sayHello();//line: 84
	std::cout<<"5 + 2 = ";//line: 85
	std::cout<<hello.count(hello.num, 2)<<"\n";//line: 86
	
	std::cout<<"Test val: "<<"\n";//line: 88
	hello.tester.logMe();//line: 89
	std::cout<<"\nTest opt: "<<"\n";//line: 90
	hello.tester_opt->logMe();//line: 91
	std::cout<<"\n"<<"\n";//line: 92
	
	hello.tester_opt->data = 3;//line: 94
	//This is an error. Use opt with: tester_opt?.inner.data = 6
	
	//hello.tester_opt.inner.inner.inner.data = 7
	
	//log(""")
	//opt Tester a_tester = hello.tester
	//a_tester.logMe
	
	return(0);
}
