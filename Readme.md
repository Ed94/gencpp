# gencpp

An attempt at simple staged metaprogramming for c/c++.

This library is intended for small-to midsized projects.

### TOC

* [Notes](#notes)
* [Usage](#usage)
* [Building](#notes)
* [Outline](#outline)
* [Why](#why)
* [TODO](#todo)

## Notes

This project is not minimum feature complete yet.  
Version 1 will have C and a subset of C++ features available to it.

I will generate with this library a C99 or 11 variant when Version 1 is complete.  
A single-header version will also be generated.

## Usage

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

The design a constructive builder of a sort of AST for the code to generate.
The user is given `Code` typed objects that are used to build up the AST. 

Example:
```cpp
// Get a Code AST from the CodePool.
Code header = make_struct( "Header" );
{
    // Make a struct body.
    Code body = header.body();

    // Members
    body->add( def_varaible( uw,        "Num") );
    body->add( def_varaible( uw,        "Capacity") );
    body->add( def_varaible( allocator, "Allocator") );
}
```

This will generate the following C code:
```cpp
struct ArrayHeader
{
    uw        Num;
    uw        Capacity;
    allocator Allocator;
};
```
**Note: The formatting shown here is not how it will look. For your desired formatting its recommended to run a pass through the files with an auto-formatter.**

## Gen's DSL

If you don't mind a low amount of macros (29 lines), a DSL may be optionally defined with:

```cpp
GEN_DEFINE_DSL
```

Using the previous example to show usage:

```cpp
make( struct, ArrayHeader )
{
    Code 
    body = ArrayHeader.body();
    body->add_var( uw,        Num       );
    body->add_var( uw,        Capacity  );
    body->add_var( allocaotr, Allocator );
}
```

The `__` represents the `UnusedCode` value constant, of unneeded varaibles.
The DSL purposefully has no nested macros, with the follwing exceptions:

* `__VA_ARGS__` for parameter expnasion 
* `VA_NARGS( __VA_ARGS__ )` for determing number of paramters
* `txt(Value_)` and `txt_with_length(Value_)` to serialize a value type identifier.

## Building

An example of building is provided in the test directory.

There are two meson build files the one within test is the program's build specification.  
The other one in the gen directory within test is the metaprogram's build specification.

Both of them build the same source file: `test.cpp`. The only differences between them is that gen need a different relative path to the include directories and defines the macro definition: `gen_time`.

This method is setup where all the metaprogram's code are the within the same files as the regular program's code.
If in your use case, decide to have exclusive separation or partial separation of the metaprogam's code from the program's code files then your build configuration would need to change to reflect that (specifically the sources).

## Outline

### *WHAT IS NOT PROVIDED*

* Macro or template generation      : This library is to avoid those, 
adding support for them adds unnecessary complexity. If you desire define them outside the gen_time scopes. 
* Expression validation             : Execution expressions are defined using the untyped string API. There is no parse API for validating expression (possibly will add in the future)
* Complete file parser DSL          : This isn't like the unreal header tool. Code injection to file or based off a file contents is not supported by the api. However nothing is stopping you using the library for that purpose.
* Modern c++ (STL library) features

As mentioned in [Usage](#Usage), the user is provided Code objects by calling the interface procedures to generate them or find existing matches.

The AST is managed by the library and provided the user via its interface prodedures.

Notes:

* The allocator definitions used are exposed to the user incase they want to dictate memory usage*
* ASTs are wrapped for the user in a Code struct which essentially a warpper for a AST* type.  
* Both AST and Code have member symbols but their data layout is enforced to be POD types.

Data layout of AST struct:

```cpp
CodeT             Type;    
bool              Readonly;
AST*              Parent;  
string            Name;    
string            Comment; 
union {                    
    array(AST*)   Entries;
    string        Content;
};
```

*`CodeT` is a typedef for `ECode::Type` which is the type of the enum.*

ASTs can be set to readonly by calling Code's lock() member function.
Adding comments is always available even if the AST is set to readonly.  

### There are four sets of interfaces for Code AST generation the library provides

* Upfront
* Incremental
* Parsing
* Untyped

### Upfront Construction

All component ASTs must be previously constructed, and provided on creation of the code AST.
The construction will fail and return InvalidCode otherwise.

Interface :

* def_class
* def_class_body
* def_class_fwd
* def_enum
* def_enum_class
* def_enum_body
* def_global_body
* def_namespace
* def_namespace_body
* def_operator
* def_operator_fwd
* def_param
* def_params
* def_proc
* def_proc_body
* def_proc_fwd
* def_specifier
* def_specifiers
* def_struct
* def_struct_body
* def_struct_fwd
* def_variable
* def_type
* def_using
* def_using_namespace

### Incremental construction

A Code ast is provided but only completed upfront if all components are provided.
Components are then added using the AST API for adding ASTs:

* code.add( AST* )         // Adds AST with validation.
* code.add_entry( AST* )   // Adds AST entry without validation.
* code.add_content( AST* ) // Adds AST string content without validation.

Code ASTs may be explictly validated at anytime using Code's check() member function.

Interface :

* make_class
* make_enum
* make_enum_class
* make_fwd
* make_global_body
* make_namespace
* make_operator
* make_params
* make_proc
* make_specifiers
* make_struct
* make_variable
* make_type
* make_using

### Parse construction

A string provided to the API is parsed for the intended language construct.

Interface :

* parse_class
* parse_classes
* parse_class_fwd
* parse_classes_fwd
* parse_enum
* parse_enums
* parse_global_body
* parse_namespace
* parse_namespaces
* parse_params
* parse_proc
* parse_procs
* parse_operator
* parse_operators
* parse_specifiers
* parse_struct
* parse_strucs
* parse_variable
* parse_variables
* parse_type
* parse_types
* parse_using
* parse_usings

The parse API treats any execution scope definitions with no validation and are turned into untyped Code ASTs.
This includes the assignmetn of variables; due to the library not yet supporting c/c++ expression parsing.

### Untyped constructions

Code ASTs are constructed using unvalidated strings.

Interface :

* untyped_str
* untyped_fmt
* untyped_token_fmt

During serialization any untyped Code AST is has its string value directly injected inline of 
whatever context the content existed as an entry within.
Even though thse are not validated from somewhat correct c/c++ syntax or components, it doesn't mean that
Untyped code can be added as any component of a Code AST:

* Untyped code cannot have children, thus there cannot be recursive injection this way.
* Untyped code can only be a child of a parent of body AST, or for values of an assignment.

These restrictions help prevent abuse of untyped code to some extent.

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
* Remove full ZPL dependency, move into Bloat header/source only what is used.
* This library has heavy string allocations, most likely will make a string flyweight for it.
