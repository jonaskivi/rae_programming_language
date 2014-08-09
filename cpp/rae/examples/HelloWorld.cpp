//this file is automatically created from Rae programming language module:
///Users/joonaz/Dropbox/jonas/2014/ohjelmointi/rae_programming_language/cpp/rae/examples/HelloWorld
#include "HelloWorld.hpp"

//class Tester

Tester::Tester()
{
}

Tester::~Tester()
{
}

void Tester::logMe()
{
	std::cout<<"Hello ";
}


//class HelloWorld

HelloWorld::HelloWorld()
{
	azero = 0;//line: 43
	initMeToo = 0.0;//line: 42
	afloat = 248.52;//line: 41
	anotherNumber = 42;//line: 40
	num25 = 4;//line: 39
	num = 5;//line: 50
	tester_ref = new Tester();
}

HelloWorld::~HelloWorld()
{
	delete tester_ref;
}

void HelloWorld::sayHello()
{
	tester.logMe();//line: 29
	std::cout<<"World!"<<"\n";//line: 30
	std::cout<<"tester_ref: "<<"\n";//line: 31
	tester_ref->logMe();
}

//return type name: result
int32_t HelloWorld::count(int32_t param1, int32_t param2)
{
	return(param1 + param2);//or you could just do: result = param1 + param2
}

//return type name: 
int32_t main(int argc, char* const argv[])
{
	HelloWorld hello; //semicolons are allowed, but not required. And this is a ref = new that you'll have to .free()
	int32_t val = 5;//line: 59
	int32_t another_zero = 0;//line: 60
	float i_want_to_be_zero = 0.0f;//line: 61
	
	hello.sayHello();//line: 63
	std::cout<<"5 + 2 = ";//line: 64
	std::cout<<hello.count(hello.num, 2)<<"\n";//line: 65
	
	hello.tester.logMe();//line: 67
	
	//log(""")
	//opt Tester a_tester = hello.tester
	//a_tester.logMe
	
	return(0);
}
