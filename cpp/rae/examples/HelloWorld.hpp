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
	public: int32_t count(int32_t param1, Tester* param2);//line: 39
	int32_t num;//line: 44
	protected: int64_t anotherNumber;//line: 45
	public: float afloat;//line: 46
	
	//REMOVE FROM HELLO
	
	Tester testerVal();//line: 50
	Tester* testerLink();//line: 55
	/*func (val Tester)testerVal2()
	{
		return(tester_opt)
	}*/
	
	//
	/*
	[Tester] testers //Swift

	//A related note: template syntax with brackets?
	vector[Tester] testers
	signal[void, int, float] mouseClicked

	link [val Tester] linkToArray

	//Signals syntax uses the func syntax:
	signal (bool result, int other)mouseClicked(ref Tester a_tester, int a_value)

	//Am I wrong if I think that signals can be used in all cases where you'd use a deledate?
	//They work differently, but a signal is just more flexible.
	delegate (bool result, int other)mouseClicked(ref Tester a_tester, int a_value)
*/
	
	//REMOVE
	
	Tester tester;//line: 85
	
	Tester* tester_opt;//= new Tester 
};

#endif // _rae_examples_HelloWorld_hpp_

