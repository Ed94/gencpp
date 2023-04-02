# gencpp

An attempt at simple staged metaprogramming for c/c++.

This project is not minimum feature complete yet. 
Version 1 will have c and a subset of c++ features available to it.

## How it works

A metaprogram is built to generate files before the main program is built. We'll term runtime for this program as `gen_time`. The metaprogram's core implementation are within `gen.hpp` and `gen.cpp` in the project directory.

`gen.cpp` \`s  `main()` will expect to call one `gen_main()` which the user will have to define once for their program. There they will dictate everything that should be generated.

In order to keep the locality of this code within the same files the following pattern may be used:

Within `program.cpp` :
```cpp
#ifdef gen_time
#include "gen.hpp"

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

To fill in.

## Why

Macros in c/c++ are usually painful to debug, and templates can be unless your on a monsterous IDE (and even then fail often).

Unfortunately most programming langauges opt the approach of internally processing the generated code immediately within the AST or not expose it to the user in a nice way to even introspect as a text file. Stage metaprogramming doesn't have this problem, since its entire purpose is to create those generated files.

This is technically what the macro preprocessor does in a basic form, however naturally its easier to deal with for more complex generation.

The drawback naturally is generation functions at face value harder to grasp than something following a template pattern (for simple generation). This drawback becomes less valid the more complex the code generation becomes.

Thus a rule of thumb is if its a simple define you can get away with just the preprocessor, or if the templates being used don't break the debugger, this is most likely not neded.

However, if the code being generated becomes complex, or from a datatable or database, this will be easier to deal with.

# TODO:

* Need problably a better name, I found a few repos with this same one...
* Actually get to version 1.
