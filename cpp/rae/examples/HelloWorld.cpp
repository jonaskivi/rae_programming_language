//this file is automatically created from Rae programming language module:
///home/joonaz/Dropbox/jonas/2014/ohjelmointi/rae_programming_language/cpp/rae/examples/HelloWorld
#include "HelloWorld.hpp"

//class Tester

Tester::Tester()
{	
	//-------------Rae boilerplate code-------------
	_rae_link_list = nullptr;
	//-------------end Rae boilerplate code-------------


	data = 2;
}

Tester::~Tester()
{	
	//-------------Rae boilerplate code-------------
	_rae_unlink_all();
	//-------------end Rae boilerplate code-------------


}

void Tester::logMe()
{
	std::cout<<"Hello ";
}


//class HelloWorld

HelloWorld::HelloWorld()
{	
	//-------------Rae boilerplate code-------------
	_rae_link_list = nullptr;
	//-------------end Rae boilerplate code-------------


	afloat = 248.52;//line: 48
	anotherNumber = 42;//line: 47
	num = 5;//line: 95
	tester_ref = new Tester();//line: 93
	tester_not = nullptr;//line: 91
	tester_opt = new Tester();
}

HelloWorld::~HelloWorld()
{	
	//-------------Rae boilerplate code-------------
	_rae_unlink_all();
	//-------------end Rae boilerplate code-------------


	if(tester_ref != nullptr ) { delete tester_ref; tester_ref = nullptr; }
	if(tester_not != nullptr ) { delete tester_not; tester_not = nullptr; }
	if(tester_opt != nullptr ) { delete tester_opt; tester_opt = nullptr; }
	//free tester_opt
}

void HelloWorld::sayHello()
{
	tester.logMe();//line: 35
	std::cout<<"World!"<<"\n";//line: 36
	std::cout<<"tester_opt: "<<"\n";//line: 37
	//NOT YET: tester_opt?.logMe()
	tester_opt->logMe();//line: 39
	std::cout<<"Should have said it."<<"\n";
}

int32_t HelloWorld::count(int32_t param1, Tester* param2)
{
	return(param1 + param2->data);
}

int32_t main(int argc, char* const argv[])
{
	HelloWorld hello;//semicolons are allowed, but not required.
	
	//Rae does not use = for pointing to an object. Instead use -> to point.
	///////////////link Tester tester_lnk2 -> hello.tester_opt
	//DOES NOT WORK ATM: 
	rae::link<Tester> tester_lnk;//line: 110
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
	tester_lnk.linkTo(&hello.tester);//line: 152
	
	std::cout<<"LINK saying hello:"<<"\n";//line: 154
	tester_lnk.obj->logMe();//line: 155
	
	//Should give us: tester_lnk.linkTo(&hello.tester);
	
	////////////////////////////////////link Tester tester_lnk3 -> hello.testerLink
	//ERROR: can't point to with a value type: val Tester tester_val4 -> hello.testerVal
	///////////////////////////val Tester tester_val4 = hello.testerVal
	//A link to a return value is not allowed, because val as return type is temporary:
	//link Tester tester_lnk5 -> hello.testerVal
	//log_s("link to a return value: tester_val4: ")
	//////////////////////////////tester_val4.logMe
	
	//val Tester tester_val5 -> hello.testerVal2
	
	hello.sayHello();//line: 169
	
	std::cout<<"5 + 2 = ";//line: 171
	
	std::cout<<"todo count."<<"\n";//line: 173
	
	//the following line will not run if tester_lnk is null.
	//NOT YET: log(hello.count(int param1: hello.num, ref Tester param2: tester_lnk?))
	
	std::cout<<hello.count(hello.num, hello.tester_ref)<<"\n";//line: 178
	
	std::cout<<hello.count(hello.num, hello.tester)<<"\n";//line: 180
	
	hello.tester.data = 3;//line: 182
	
	return(0);
}
