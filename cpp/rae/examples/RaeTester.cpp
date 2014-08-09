//this file is automatically created from Rae programming language module:
///Users/joonaz/Dropbox/jonas/2014/ohjelmointi/rae_programming_language/cpp/rae/examples/RaeTester
#include "RaeTester.hpp"
;//how about a (comme{nt here!!!! NEW VERSION9
#include "Tester.hpp"/*/+2

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
//class SameName

SameName::SameName()
{
}

SameName::~SameName()
{
}

void SameName::otherFunc()
{
	
}


//class RaeTester

RaeTester::RaeTester()
{
	thisShouldBePublic = 0.0;//line: 140
	alibraryInHere = 0;//line: 90
	i_can_init_myself = "Something";//line: 89
	how_is_it = "";//line: 88
	azero = 0;//line: 87
	initMeToo = 0.0;//line: 86
	afloat = 248.52;//line: 85
	anotherNumber = 42;//line: 84
	num25 = 4;//line: 83
	num = 5;//line: 138
	testers = new std::vector<ref*>;//line: 98
	anotherTester = new Tester();
}

RaeTester::RaeTester(int32_t set_num, uint32_t set_num25)
{
	thisShouldBePublic = 0.0;//line: 140
	alibraryInHere = 0;//line: 90
	i_can_init_myself = "Something";//line: 89
	how_is_it = "";//line: 88
	azero = 0;//line: 87
	initMeToo = 0.0;//line: 86
	afloat = 248.52;//line: 85
	anotherNumber = 42;//line: 84
	num25 = 4;//line: 83
	num = 5;//line: 138
	testers = new std::vector<ref*>;//line: 98
	anotherTester = new Tester();//line: 47
	num = set_num;//line: 48
	num25 = set_num25;
}

RaeTester::~RaeTester()
{
	delete testers;//line: 98
	delete anotherTester;
}

void RaeTester::sayHello()
{
	//we need some keyword that tells that we actually want a temp object to be a class member:
	//Keeps it's state like a static value, but you can zero it out on this function like this.
	//Would this make the memory allocations faster for these temporary objects?
	//member val Calculator calc
	//this could be a usefull keyword in constructors too. I think python has something like this.
	//members would be protected by default. but you could define them public:
	//member public maybe Calculator calculator
	//but if it's a maybe this will only be allocated and initialized when this func is run the first time! before that it will be null!
	//But val and ref members will be inited in constructors.
	//another option for the keyword would be keep:
	//keep maybe Calculator calculator
	//or from existing keywords, changing the way """ works with classes (it would be per object.)
	//or """:
	//this maybe Calculator calculator
	//static maybe Calculator calculator
	
	tester.logMe();//line: 75
	std::cout<<""Tester 0.1!"<<"\n";
}

//return type name: result
int32_t RaeTester::count(int32_t param1, int32_t param2)
{
	return(param1 + param2);//or you could just do: result = param1 + param2
}

void RaeTester::createNewArrayTester()
{
	Tester atest;//make this named atest so it collides with the same name param...
	arrayTesters->push_back(atest);
	//TODO use emplace on C++11
}

void RaeTester::logArrayTesters()
{
	for(uint32_t i = 0; i < arrayTesters->size(); i++)
	{
		(*arrayTesters)[i]->logMe();//line: 125
		std::cout<<""from arrayTesters: "<<" and a another\" "<<i<<" That is all we know. And here's a rarity.afloat: "<<afloat<<"\n";//rarity.afloat
		nameConflict((*arrayTesters)[i]);
	}
	
	/*TODO foreach(Tester atest; arrayTesters)
		{
			atest.logMe
		}*/
}

void RaeTester::createNewTester()
{
	Tester* atest = new Tester();//line: 147
	testers->push_back(atest);
	
	if(testers->size() > 2)
	{
		testerParams((*testers)[0], (*testers)[1]);
	}
	delete atest;
}

void RaeTester::testerParams( one,  two)
{
	one->/*possible error, unknown token:*/value = one->/*possible error, unknown token:*/value + two->/*possible error, unknown token:*/value;//line: 158
	std::cout<<"testerParams: one.value: "<<one->/*possible error, unknown token:*/value<<" two.value: "<<two->/*possible error, unknown token:*/value<<"\n";
}

void RaeTester::nameConflict(/*override:*/tester)
{
	std::cout<<"nameConflict: tester.value: "<<tester->/*possible error, unknown token:*/value<<"\n";//line: 164
	tester->/*possible error, unknown token:*/value = 129;//line: 165
	{
		/*override:*/Tester* tester = new Tester();//line: 167
		tester->value = 2;//line: 168
		std::cout<<"nameConflict: tester2.value: "<<tester->value<<"\n";//line: 169
		delete tester;
	}
	std::cout<<"nameConflict: tester3.value: "<<tester->/*possible error, unknown token:*/value<<"\n";
}

void RaeTester::logTesters()
{
	for(uint32_t i = 0; i < testers->size(); i++)
	{
		(*testers)[i]->/*possible error, unknown token:*/logMe;//line: 178
		std::cout<<"from tester: "<<" and a another\" "<<i<<" That is all we know. And here's a rarity.afloat: "<<afloat<<"\n";//rarity.afloat
		nameConflict((*testers)[i]);
	}
}

void RaeTester::testFuncArrays()
{
	std::vector<Tester*>* a_local_array = new std::vector<Tester*>;//line: 188
	a_local_array->reserve(100);//line: 189
	for(uint32_t i = 0; i < 25; i++)
	{
		Tester atest;//line: 192
		a_local_array->push_back(atest);
	}
	
	std::cout<<"a_local_array size: "<<a_local_array->size()<<"\n";
}

//return type name: 
int32_t main(int argc, char* const argv[])
{
	RaeTester rarity; //This is a ref. It is auto initialized.
	int32_t val = 5;//this is a value because it is a built-in type.
	int32_t another_zero = 0;//line: 230
	float i_want_to_be_zero = 0.0f;//auto init to zero. unlike in c++ (it is not guaranteed and causes bugs on some compilers).
	/*
	RaeTester? canBeNull = null //this is a nullref. You must check for null to use it.
	//RaeTester? canBeNull //This would still auto init to = new RaeTester!!!
	//so you'll have to explicitly set it to null.
	//RaeTester cannotBeNull = null //This would be an err,
	//because normal refs can not be null.

	if(canBeNull)
	{
		//this will NOT happen, because canBeNull is not yet created.
		log("This will not happen: ", canBeNull.count(2,2))
	}

	canBeNull = RaeTester.new(64, 64)

	//this will happen.
	if(canBeNull)
	{
		log("This will not happen: ", canBeNull.count(2,2))
	}

	canBeNull.free
*/
	if(rarity)//an err!!!!!! no need for this with refs.
	{
		rarity.sayHello();//line: 257
		std::cout<<"5 + 2 = ";//line: 258
		std::cout<<rarity.count(rarity.num, 2)<<"\n";//line: 259
		
		std::cout<<"default count: ";//line: 261
		std::cout<<rarity.count()<<"\n";//line: 262
		
		std::cout<<"Arrays as vectors: "<<"\n";//line: 264
		
		for(uint32_t i = 0; i < rarity.count(1, rarity.count(1, 1)); i++)
		{
			rarity.createNewArrayTester();
		}
		
		rarity.logArrayTesters();//line: 271
		
		std::cout<<"Now for just plain vectors."<<"\n";//line: 273
		
		for(uint32_t i = 0; i < rarity.count(1, rarity.count(1, 1)); i++)
		{
			rarity.createNewTester();
		}
		
		rarity.logTesters();//line: 280
		
		std::string does_this = "Does this ";//line: 282
		
		std::cout<<does_this + "really work?"<<"\n";//line: 284
		
		std::string how_about = "How about ";//line: 286
		std::string this_really = "this really working?";//line: 287
		std::string does_it = how_about + this_really;//line: 288
		
		std::cout<<does_it<<"\n";//line: 290
		
		rarity.testFuncArrays();
	}
	
	//ERR: can not call free on normal refs. They're like values.
	
	return(0);
}
