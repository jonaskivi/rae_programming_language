Rae Programming Language
========================

A native statically typed toy programming language
- optionals, no semicolons, single source files, short words instead of sigils
- compiles to almost readable C++11
- the compiler will break on about any code you throw at it, and it is written in the most horrible style, as it was just intented to be a temporary proof of concept. :)
- influenced by C++, D, Python, Nim, Swift, Rust and Jai

# A sort of like Helloworld

```python
module examples.hello

class HelloWorld
{
pub:
	# constructor:
	func init
	{
	}
	
	# destructor:
	func free
	{
	}
	
	func log_int_array (ref[int] someints) let
	{
		# foreach loop
		loop( int a_num in someints )
		{
			log("foreach: ", a_num)
		}

		# for loop
		loop( uint i = 0; i < someints.size; ++i )
		{
			log("for: ", someints[i])
		}

		# TODO while loop. Currently you can use loop(;;)
		# loop(1)
	}

	func create_an_array
	{
		[int] my_numbers
		my_numbers.create(1)
		my_numbers.create(2)
		my_numbers.create(3)
		
		log_int_array(my_numbers)
	}

	# multiple return values are TODO
	func (int result) int_count (let int param1, let int param2) pub
	{
		return param1 + param2
	}

	# properties with the prop keyword are TODO
	pub func (int) number     { return m_number }
	pub func number (int set) { m_number = set  }
	priv int m_number = 5 # default initializers are copied to constructors.

	func (string) world { return m_world }
	priv string m_world = "World"

	func (bool) is_it_true
	{
		return number < 10
	}
}

func (int) main ([string] args)
{
	string hello_rae = "Hello "
	log_s(hello_rae)

	# every Rae object has a kind, which can be one of:
	# val, ref, opt, link or ptr
	#val is default so this is the same as: val HelloWorld hello
	HelloWorld hello

	# TODO you can later call funcs like this:
	# hello.number = 9
	hello.number(9)

	if hello.is_it_true
	{
		log(hello.world)
	}

	return 0
}
```


# Some features

The defining feature of Rae is that each variable has a kind, which can be one of:
	
	val : for value. Value types are usually the default, so you can leave it out.
	
	ref : for reference. Refs own their memory.
	Reference types are the default for function parameters.
	
	opt : for optional. Opts own their memory.
	
	link : for link. A sort of pointer in two directions. Links don't own their memory.
	They can point to vals, refs or opts.
	
	ptr : a pointer type for interfacing with C/C++.

You use them like this:
	opt Something m_something = null

Array syntax is like this:
	
	# A reference array of optional Somethings
	ref[opt Something] m_somethings

	# A value array of value Somethings
	[Something] m_valueSomethings

	# An array of ints, and it's initializer list
	[int] m_values = [1, 42, 7]


# TODO
- Inheritance, multiple inheritance, interfaces
- Multiple return values
- Make the compiler more robust, and well, rewrite it :)
- Built in types don't handle kind correctly
- Strings are practically not handled at all in many cases

# Getting started

Go to some folder on the command line and then:

	git clone https://github.com/jonaskivi/rae_programming_language.git
	cd rae_programming_language
	make
	# you should be greeted with a "Hello World" text
	# optionally: will install to /usr/local/bin/raec
	sudo make install

You could also build it with the premake4 script found in rae_compiler by doing:

	cd rae_compiler
	# for OS X:
	premake4 xcode4
	# or for Windows
	premake4 vs2012

Open the project file and build it. (On Xcode 5 I needed to change the SDK to 10.9 and the C++ dialect and stdlib versions to C++11.)
You should then copy the raec to somewhere in your path to use it in other directories.
Then you could test it with the rae_render project found in https://github.com/jonaskivi/rae_render
or write your own.

Python syntax highlighting works reasonably well for Rae.



