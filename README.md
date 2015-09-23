Rae Programming Language
========================

A native statically typed toy programming language
- optionals, no semicolons, single source files, short words instead of sigils
- compiles to almost readable C++11
- the compiler will break on about any code you throw at it, and it is written in the most horrible style, as it was just intented to be a temporary proof of concept. :)
- influenced by C++, D, Python, Nim, Swift, Rust and Jai

# Helloworld

	module hello

	func (int) main ([string] args)
	{
		log("Hello World")
		return 0
	}

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

