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
	public: void sayHello();//line: 30
	public: int32_t count(int32_t param1, Tester* param2);//line: 40
	int32_t num;//line: 45
	protected: int64_t anotherNumber;//line: 46
	public: float afloat;//line: 47
	
	//REMOVE FROM HELLO
	
	Tester testerVal();//line: 51
	Tester* testerLink();//line: 56
	/*func (val Tester)testerVal2()
	{
		return(tester_opt)
	}*/
	
	//REMOVE
	
	Tester tester;//line: 68
	
	Tester* tester_opt;//= new Tester 
};

#endif // _rae_examples_HelloWorld_hpp_

