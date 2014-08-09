//this file is automatically created from Rae programming language module:
///Users/joonaz/Dropbox/jonas/2014/ohjelmointi/rae_programming_language/cpp/rae/examples/RaeTester
#ifndef _rae_examples_RaeTester_hpp_
#define _rae_examples_RaeTester_hpp_

#include <stdint.h>
#include <iostream>
#include <string>
#include <vector>

//how about a (comme{nt here!!!! NEW VERSION9
#include "rae/examples/Tester.hpp"
/*/+2

twoplusses

/+1
oneplus

/+

zeroplus

/o*
	Starcommenthere
*o/

zeroplus2
+/
oneplus2
1+/
twoplusses2
2+/*/
class SameName
{
	public: SameName();//line: 34
	~SameName();//line: 34
	void otherFunc();//line: 31
};


class RaeTester
{
public: 
	
	RaeTester();//line: 42
	RaeTester(int32_t set_num, uint32_t set_num25 = 0);//line: 46
	~RaeTester();//line: 52
	public: void sayHello();//line: 57
	public: //return type name: result
	int32_t count(int32_t param1 = 1, int32_t param2 = 1);//line: 79
	int32_t num;//line: 84
	uint32_t num25;//line: 85
	protected: int64_t anotherNumber;//line: 86
	public: float afloat;//line: 87
	/*hidden*/protected: double initMeToo;//line: 88
	/*library*/public: int32_t azero;//line: 89
	public: std::string how_is_it;//line: 90
	std::string i_can_init_myself;//line: 91
	
	Tester tester;//This is a value type. Allocated on the stack.
	//related C++ discussion on vectors with value types:
	//stackoverflow.com/questions/8543854/when-adding-an-element-to-a-vector-how-to-know-that-a-copy-of-the-object-is-goi
	//We should use emplace or emplace_back with vectors.
	//Should we rename them: create_object and create_object_back???
	
	/*library*/public: Tester* anotherTester;//This is a ref that mostly acts like a value type.
	//It cannot be null.
	//It is allocated on the heap.
	//You should mostly use this type only as function parameter. library is just for visibility (like public).
	//It means this is only visible to this library, which is the name of the top module, """ in this case.
	//But you could use this with e.g. arrays, but values are maybe faster in arrays, due to memory being continuous with value types.
	
	/*possible error, unknown token:*/null;//This is the way to make an empty reference.
	//But you have to check it every time to use it.
	/*possible error, unknown token:*/
;//The standard array/vector syntax is a dynamic array syntax like this:
	/*possible error, unknown token:*/valpublic: std::vector<Tester*>* arrayTesters;//a dynamic Rae array is a C++ std::vector
	//a static array is a C++ std::array or boost::array
	
	void createNewArrayTester();//line: 114
	public: void logArrayTesters();//line: 121
	
	//Another syntax for vectors, but don't use this one:
	
	protected: std::vector<ref*>* testers;//line: 139
	
	/*library*/public: int32_t alibraryInHere;//line: 141
	
	public: float thisShouldBePublic;//line: 143
	
	/*library*/public: void createNewTester();//line: 145
	public: void testerParams( one,  two);//line: 156
	void nameConflict(/*override:*/tester);//line: 162
	void logTesters();//line: 174
	//
	
	void testFuncArrays();//line: 186
	//a C-style raw array needs special syntax.
	/*
	extern c++
	{
	protected: Tester testers3[];

	func ()createNewTester3()
	{
		testers3 = new Tester[8];
		Tester atest
		testers3[0] = atest;
	}

	func ()logTesters3() public
	{
		for(uint i = 0; i < 8; i++)
		{
			testers3[raw i].logMe
		}
	}
	}

	*/
	
};

#endif // _rae_examples_RaeTester_hpp_

