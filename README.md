Rae Programming Language
========================

A native statically typed programming language
- optionals, no semicolons, single source files, short words instead of sigils
- compiles to almost readable C++11
- the compiler will break on about any code you throw at it
- influenced by C++, D, Python, Nim, Swift, Rust and Jai

	module hello

	func (int) main ([string] args)
	{
		log("Hello World")
		return 0
	}

The defining feature of Rae is that each variable has a kind, which can be one of:
	
	val : for value. Value types are usually the default, so you can leave it out.
	
	ref : for reference. refs own their memory.
	Reference types are the default for function parameters.
	
	opt : for optional. opts own their memory.
	
	link : for link. A sort of pointer in two directions. links don't own their memory.
	They can point to vals, refs or opts.
	
	ptr : a pointer type for interfacing with C/C++.

You use them like this:
	opt Something m_something = null

Array syntax is like this:
	
	# A reference array of optional Somethings
	ref[opt Something] m_somethings

	# A value array of value Somethings
	[Something] m_valueSomethings

	# An array of ints
	[int] m_values


# TODO
- Inheritance, multiple inheritance, interfaces
- Multiple return values
- Make the compiler more robust
- Built in types don't handle kind correctly
- Strings are practically not handled at all in many cases

