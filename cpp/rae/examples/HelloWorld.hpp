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
	public: Tester();//line: 10
	~Tester();//line: 10
public: 
	void logMe();//line: 7
};


class HelloWorld
{
public: 
	
	HelloWorld();//line: 18
	~HelloWorld();//line: 22
	public: void sayHello();//line: 27
	public: //return type name: result
	int32_t count(int32_t param1, int32_t param2);//line: 35
	int32_t num;//line: 40
	uint32_t num25;//line: 41
	protected: int64_t anotherNumber;//line: 42
	public: float afloat;//line: 43
	double initMeToo;//line: 44
	int32_t azero;//line: 45
	
	Tester tester/*this is a "reference". Dynamically allocated automatically.
	unless, you specify it as "scope". Then it will be like a value, but you still
	use it as a ref... Huh?*/;//line: 49
	
	Tester* tester_ref;//line: 51
	
};

#endif // _rae_examples_HelloWorld_hpp_

