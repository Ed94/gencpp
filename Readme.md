# gencpp

An attempt at simple staged metaprogramming for c/c++.

This library is intended for small-to midsize projects that want rapid complation times.
for fast debugging.

## Notes

This project is not minimum feature complete yet.  
Version 1 will have c and a subset of c++ features available to it.

I will generate with this library a C99 or 11 variant when Version 1 is complete.  
A single-header version will also be genrated.

## How it works

A metaprogram is built to generate files before the main program is built. We'll term runtime for this program as `gen_time`. The metaprogram's core implementation are within `gen.hpp` and `gen.cpp` in the project directory.

`gen.cpp` \`s  `main()` is defined as `gen_main()` which the user will have to define once for their program. There they will dictate everything that should be generated.

In order to keep the locality of this code within the same files the following pattern may be used:

Within `program.cpp` :
```cpp
#include "gen.hpp"

#ifdef gen_time

...

u32 gen_main()
{
	...
}
#endif


#ifndef gen_time
#include "program.gen.cpp"

	// Regular runtime dependent on the generated code here.
#endif
```
This is ofc entirely optional and the metaprogram can be quite separated from the runtime in file organization.

## Building

An example of building is provided in the test directory.

There are two meson build files the one within test is the program's build specification.  
The other one in the gen directory within test is the metaprogram's build specification.

Both of them build the same source file: `test.cpp`. The only differences between them is that gen need a different relative path to the include directories and defines the macro definition: `gen_time`.

This method is setup where all the metaprogram's code are the within the same files as the regular program's code.
If in your use case, decide to have exclusive separation or partial separation of the metaprogam's code from the program's code files then your build configuration would need to change to reflect that (specifically the sources).

## Why

Macros in c/c++ are usually painful to debug, and templates can be unless your on a monsterous IDE (and even then fail often).

Templates also have a heavy cost to compile-times due to their recursive nature of expansion if complex code is getting generated, or if heavy type checking system is used (assertsion require expansion, etc).

Unfortunately most programming langauges opt the approach of internally processing the generated code immediately within the AST or not expose it to the user in a nice way to even introspect as a text file.  

Stage metaprogramming doesn't have this problem, since its entire purpose is to create those generated files that the final program will reference instead.

This is technically what the macro preprocessor does in a basic form, however a proper metaprogram for generation is easier to deal with for more complex generation.

The drawback naturally is generation functions, at face value, are harder to grasp than something following a template pattern (for simple generation). This drawback becomes less valid the more complex the code generation becomes.

Thus a rule of thumb is if its a simple definition you can get away with just the preprocessor `#define`, or if the templates being used don't break the debugger or your compile times, this is most likely not neded.

However, if the code being generated becomes complex, or from a datatable or database, this will be easier to deal with.

# TODO:

* Need problably a better name, I found a few repos with this same one...
* Actually get to version 1.
* Make a test suite made up of collections based of the ZPL library templated colllection macros and the memory module.
* Generate a single-header library.
* Generate a C-supported single-header library.
