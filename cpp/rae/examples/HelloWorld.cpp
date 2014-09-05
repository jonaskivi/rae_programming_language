//this file is automatically created from Rae programming language module:
///Users/joonaz/Dropbox/jonas/2014/ohjelmointi/rae_programming_language/cpp/rae/examples/HelloWorld
#include "HelloWorld.hpp"

//class Tester

Tester::Tester()
{
	data = 2;
;
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
}

HelloWorld::~HelloWorld()
{
//free tester_opt
}

void HelloWorld::sayHello()
{
tester.logMe();//line: 35
std::cout<<"World!"<<"\n";//line: 36
std::cout<<"tester_opt: "<<"\n";//line: 37
//NOT YET: tester_opt?.logMe()
 RAE ERROR /*possible error, unknown token:*/tester_opt-> RAE ERROR /*possible error, unknown token:*/logMe();
}

int32_t HelloWorld::count(int32_t param1, Tester* param2)
{
return(param1 + param2->data);
}

Tester* tester_opt//= new Tester RAE ERROR 
Tester* tester_notINDATAint32_t main(int argc, char* const argv[])
{
 RAE ERROR HelloWorld hello;//semicolons are allowed, but not required.

//Rae does not use = for pointing to an object. Instead use -> to point.
///////////////link Tester tester_lnk2 -> hello.tester_opt
//DOES NOT WORK ATM: 
 RAE ERROR rae::link<Tester> tester_lnk;//line: 107
/*
	log("LINK fails to say hello:")
	tester_lnk?.logMe()
	//if(tester_lnk.obj) tester_lnk.obj->logMe();

	lock( hello, other, hello.something )//if any of them is null or fails to lock, then skip.
	{
		hello.something.destroyHelloAndSomething(other)//fails to destroy because of lock.
		hello.something.stillUseHello()
	}
	else lock(hello)
	{
		hello.justWithHelloThen
	}
	else
	{
		error("Couldn't lock hello.")
	}

	lock_wait( hello, hello.something )//if any of them is null then skip, if already locked then wait, forever.
	{
		hello.something.destroyHelloAndSomething()//fails to destroy because of lock.
		hello.something.stillUseHello()
	}
	
	hello! hello.something!
	{
		hello.something.destroyHelloAndSomething()
		hello.something.stillUseHello()
	}

	hello? hello.something?
	{
		hello.something.destroyHelloAndSomething()
		hello.something.stillUseHello()
	}
	

	hello?.something?.tester?.logMe
	if(hello.obj && hello.obj->something.obj && hello.obj->something.obj->tester.obj ) tester_lnk.obj->logMe();
*/
tester_lnk.linkTo(hello. RAE ERROR /*possible error, unknown token:*/tester);//line: 149

std::cout<<"LINK saying hello:"<<"\n";//line: 151
tester_lnk.obj->logMe();//line: 152

//Should give us: tester_lnk.linkTo(&hello.tester);

////////////////////////////////////link Tester tester_lnk3 -> hello.testerLink
//ERROR: can't point to with a value type: val Tester tester_val4 -> hello.testerVal
///////////////////////////val Tester tester_val4 = hello.testerVal
//A link to a return value is not allowed, because val as return type is temporary:
//link Tester tester_lnk5 -> hello.testerVal
//log_s("link to a return value: tester_val4: ")
//////////////////////////////tester_val4.logMe

//val Tester tester_val5 -> hello.testerVal2

hello.sayHello();//line: 166

std::cout<<"5 + 2 = ";//line: 168

//the following line will not run if tester_lnk is null.
//NOT YET: log(hello.count(int param1: hello.num, ref Tester param2: tester_lnk?))

//NOT_YET: log(hello.count( hello.num, hello.tester ))

hello. RAE ERROR /*possible error, unknown token:*/tester-> RAE ERROR /*possible error, unknown token:*/data = 3;//line: 175

return(0);
}
