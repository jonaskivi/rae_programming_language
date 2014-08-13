//this file is automatically created from Rae programming language module:
///Users/joonaz/Dropbox/jonas/2014/ohjelmointi/rae_programming_language/cpp/rae/examples/HelloWorld
#ifndef _rae_examples_HelloWorld_hpp_
#define _rae_examples_HelloWorld_hpp_

#include <stdint.h>
#include <iostream>
#include <string>
#include <vector>


class Tester
{
	public: Tester();//line: 12
	~Tester();//line: 12
public: 
	void logMe();//line: 7
	int32_t data;
};


class HelloWorld
{
public: 
	
	HelloWorld();//line: 20
	~HelloWorld();//line: 24
	public: void sayHello();//line: 29
	public: int32_t count(int32_t param1, Tester* param2);//line: 39
	int32_t num;//line: 44
	protected: int64_t anotherNumber;//line: 45
	public: float afloat;//line: 46
	
	Tester tester;//line: 48
	
	Tester* tester_opt;
};

#endif // _rae_examples_HelloWorld_hpp_

