//this file is automatically created from Rae programming language module:
///Users/joonaz/Dropbox/jonas/2014/ohjelmointi/rae_programming_language/cpp/rae/examples/OptTester
#ifndef _rae_examples_OptTester_hpp_
#define _rae_examples_OptTester_hpp_

#include <stdint.h>
#include <iostream>
#include <string>
#include <vector>


class Inner
{
	public: Inner();//line: 12
	~Inner();//line: 12
public: 
	void logMe();//line: 7
	int32_t data;
};


class Tester
{
	Tester();//line: 31
	~Tester();//line: 31
public: 
	void logMe();//line: 19
	int32_t data;//line: 24
	
	
	Inner* inner();//line: 27
	protected: Inner* m_inner;
};


class OptTester
{
public: 
	
	OptTester();//line: 39
	~OptTester();//line: 43
	public: void sayHello();//line: 48
	public: int32_t count(int32_t param1, int32_t param2);//line: 56
	int32_t num;//line: 61
	uint32_t num25;//line: 62
	protected: int64_t anotherNumber;//line: 63
	public: float afloat;//line: 64
	double initMeToo;//line: 65
	int32_t azero;//line: 66
	
	Tester tester;//line: 68
	
	Tester* tester_ref;//line: 70
	
	Tester* tester_opt;//line: 72
	
};

#endif // _rae_examples_OptTester_hpp_

