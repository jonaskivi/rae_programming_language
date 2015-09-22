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

