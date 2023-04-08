# gencpp

An attempt at simple staged metaprogramming for c/c++.

This library is intended for small-to midsized projects.

### TOC

* [Notes](#notes)
* [Usage](#usage)
* [Gen's DSL](#gens-dsl)
* [Building](#notes)
* [Outline](#outline)
* [What is not provided](#what-is-not-provided)
* [The four constructors](#there-are-four-sets-of-interfaces-for-code-ast-generation-the-library-provides)
* [Code generation and modification](#code-generation-and-modification)
* [On multithreading](#on-multi-threading)
* [On extending with whatever features you want](#on-extending-with-whatever-features-you-want)
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

* Macro or template generation      : This library is to avoid those, adding support for them adds unnecessary complexity.
                                        If you desire define them outside the gen_time scopes.
* Expression validation             : Execution expressions are defined using the untyped string API.
                                        There is no parse API for validating expressions (possibly will add in the future)
* Modern C++ (STL library) features
* Modern C++ RTTI                   : This is kinda covered with the last point, but just wanted to emphasize.

Exceptions brought in from "Modern C++":

* consteval
* constinit
* explicit
* export
* noexcept
* import
* final
* module
* override
* &&
* virtual

As mentioned in [Usage](#Usage), the user is provided Code objects by calling the constructor functions to generate them or find existing matches.

The AST is managed by the library, however the user may specificy memory configuration.

Notes:

* The allocator definitions used are exposed to the user incase they want to dictate memory usage*
* ASTs are wrapped for the user in a Code struct which essentially a warpper for a AST* type.
* Both AST and Code have member symbols but their data layout is enforced to be POD types.

Data layout of AST struct:

```cpp
AST*              Parent;
string_const      Name;
string_const      Comment;
union {
    array(AST*)   Entries;
    string_const  Content;
};
CodeT             Type;
OperatorT         Op;
bool              Readonly;
u8                _64_Align[23];
```

*`CodeT` is a typedef for `ECode::Type` which has an underlying type of u32*
*`OperatorT` is a typedef for `EOperator::Type` which has an underlying type of u32.*

ASTs can be set to readonly by calling Code's lock() member function.
Adding comments is always available even if the AST is set to readonly.

## There are four sets of interfaces for Code AST generation the library provides

* Upfront
* Incremental
* Parsing
* Untyped

### Upfront Construction

All component ASTs must be previously constructed, and provided on creation of the code AST.
The construction will fail and return InvalidCode otherwise.

Interface :

* def_class
* def_enum
* def_enum_class
* def_friend
* def_function
* def_namespace
* def_operator
* def_param
* def_params
* def_specifier
* def_specifiers
* def_struct
* def_variable
* def_type
* def_typedef
* def_using
* def_class_body
* def_enum_body
* def_function_body   NOTE: Use this for operator bodies as well.
* def_global_body
* def_namespace_body
* def_struct_body

Usage:

```c++
<name> = def_<function type>( ... );

Code <name>
{
    ...
    <name> = def_<function name>( ... );
}

```

### Incremental construction

A Code AST is provided but the body is not complete.

* code.add( AST* )                     // Adds AST with validation.
* code.add_entry( AST* )               // Adds AST entry without validation.

Code ASTs may be explictly validated at anytime using Code's check() member function.

Interface :

* make_class
* make_enum
* make_enum_class
* make_function
* make_global_body
* make_namespace
* make_operator
* make_params
* make_specifiers
* make_struct

Usage:

```cpp
Code <name> = make_<function name>( ... )
{
    <name>->add( ... );
    ...
}
```

### Parse construction

A string provided to the API is parsed for the intended language construct.

Interface :

* parse_class
* parse_enum
* parse_friend
* parse_function
* parse_global_body
* parse_namespace
* parse_operator
* parse_struct
* parse_strucs
* parse_variable
* parse_type
* parse_typedef
* parse_using
* parse_classes
* parse_enums
* parse_functions
* parse_namespaces
* parse_operators
* parse_variables
* parse_typedefs
* parse_usings

Usage:

```cpp
Code <name> = parse_<function name>( string with code );

Code <name> = def_<function name>( ..., parse_<function name>(
    <string with code>
));

Code <name> = make_<function name>( ... )
{
    <name>->add( parse_<function name>(
        <string with code>
    ));
}
```

The parse API treats any execution scope definitions with no validation and are turned into untyped Code ASTs.
This includes the assignmetn of variables; due to the library not yet supporting c/c++ expression parsing.

### Untyped constructions

Code ASTs are constructed using unvalidated strings.

Interface :

* token_fmt
* untyped_str
* untyped_fmt
* untyped_token_fmt

During serialization any untyped Code AST has its string value directly injected inline of
whatever context the content existed as an entry within.
Even though these are not validated from somewhat correct c/c++ syntax or components, it doesn't mean that
Untyped code can be added as any component of a Code AST:

* Untyped code cannot have children, thus there cannot be recursive injection this way.
* Untyped code can only be a child of a parent of body AST, or for values of an assignment (ex: variable assignment).

These restrictions help prevent abuse of untyped code to some extent.

Usage Conventions:

```cpp
Code <name> = def_varaible( <type>, <name>, untyped_<function name>(
    <string with code>
));
```

Template metaprogramming in the traditional sense becomes possible with the use of `token_fmt` and parse constructors:

```cpp
char const* token_key, token_value, ...;
char const* template = txt(
    Code with {key value} to replace with token_values
    ...
);
char const* gen_code_str = token_fmt( template, num_tokens, token, ... );
Code        <name>       = parse_<function name>( gen_code_str );
```

## Code generation and modification

There are three provided interfaces:

* Builder
* Editor
* Scanner

Editor and Scanner are disabled by default, use `GEN_FEATURE_EDITOR` and `GEN_FEATURE_SCANNER` to enable them.

### Builder is a similar object to the jai language's string_builder.

* The purpose of it is to generate a file.
* A file is specified and opened for writting using the open( file_path) ) function.
* The code is provided via print( code ) function  will be seralized to its buffer.
* When all seralization is finished, use the write() command to write the buffer to the file.

### Editor is for editing a series of files based on a set of requests provided to it.

* The purpose is to overrite a specific file, it places its contents in a buffer to scan.
* Requests are populated using the following interface:
  * add : Add code.
  * remove : Remove code.
  * replace: Replace code.

All three have the same parameters with exception to remove which only has SymbolInfo and Policy:

* SymbolInfo:
  * File      : The file the symbol resides in. Leave null to indicate to search all files.
  * Marker    : #define symbol that indicates a location or following signature is valid to manipulate. Leave null to indicate that the signature should only be used.
  * Signature : Use a Code symbol to find a valid location to manipulate, can be further filtered with the marker. Leave null to indicate that the marker should only be used.

* Policy : Additional policy info for completing the request (empty for now)
* Code   : Code to inject if adding, or replace existing code with.

Additionally if `GEN_FEATURE_EDITOR_REFACTOR` is defined, refactor( file_path, specification_path ) wil be made available.
Refactor is based of the refactor library and uses its interface.
It will on call add a request to the queue to run the refactor script on the file.

### Scanner allows the user to generate Code ASTs by reading files.

* The purpose is to grab definitions to generate metadata or generate new code from these definitions.
* Requests are populated using the add( SymbolInfo, Policy ) function. The symbol info is the same as the one used for the editor. So is the case with Policy.

The file will only be read from, no writting supported.

One great use case is for example: generating the single-header library for gencpp!

### Additional Info (Editor and Scanner)

When all requests have been populated, call process_requests().
It will provide an output of receipt data of the results when it completes.

Files may be added to the Editor and Scanner additionally with add_files( num, files ).
This is intended for when you have requests that are for multiple files.

Request queue in both Editor and Scanner are cleared once process_requests completes.

## On multi-threading:

Its intended eventually for this library to support multi-threading at some point,
however for now it does not.

The following changes would have to be made:

* Setup static data accesss with fences if more than one thread will generate ASTs
* Make sure local peristent data of functions are also thread local.
* The builder should be done on a per-thread basis.
* Due to the design of the editor and scanner, it will most likely
        be best to make each file a job to process request entries on.
        Receipts should have an an array to store per thread.
        They can be combined to the final reciepts array when all files have been processed.

For now single-threaded should be pretty quick even without heavy optimizations.

## On extending with whatever features you want

This library is relatively very small, and you can easily extend it.

The untyped codes and builder/editor/scanner can be technically be used to circumvent
any sort of constrictions the library has with: modern c++, templates, macros, etc.

Typical use case is for getting define constants an old C/C++ library with the scanner:
Code parse_defines() can emit a custom code AST with Macro_Constant type.

Another would be getting preprocessor or template metaprogramming Codes from Unreal Engine definitions, etc.

## Why

Macros in c/c++ are usually painful to debug, and templates can be unless your on a monsterous IDE (and even then fail often).

Templates also have a heavy cost to compile-times due to their recursive nature of expansion if complex code is getting generated, or if heavy type checking system is used (assertsion require expansion, etc).

Unfortunately most programming langauges opt the approach of internally processing the generated code immediately within the AST or not expose it to the user in a nice way to even introspect as a text file.

Stage metaprogramming doesn't have this problem, since its entire purpose is to create those generated files that the final program will reference instead.

This is technically what the macro preprocessor does in a basic form, however a proper metaprogram for generation is easier to deal with for more complex generation.

The drawback naturally is generation functions, at face value, are harder to grasp than something following a template pattern (for simple generation). This drawback becomes less valid the more complex the code generation becomes.

Thus a rule of thumb is if its a simple definition you can get away with just the preprocessor `#define`, or if the templates being used don't break the debugger or your compile times, this is most likely not needed.

However, if:

* The code being generated becomes complex
* You enjoy actually *seeing* the generated code instead of just the error symbols or the pdb symbols.
* You value your debugging expereince, and would like to debug your metaprogram, without having to step through the debug version of the compiler (if you even can)
* You want to roll your own runtime reflection system
* You want to maintain a series of libraries for internal use, but don't want to deal with manaual merging as often when they update.
* Want to create tailored headers for your code or for your libraries since you usually don't need the majority of the code within them.

Then this might help you boostrap a toolset todo so.

# TODO:

* Need problably a better name, I found a few repos with this same one...
* Actually get to version 1.
* Make a test suite made up of collections based of the ZPL library templated colllection macros and the memory module.
* Generate a single-header library.
* Generate a C-supported single-header library.
* Remove full ZPL dependency, move into Bloat header/source only what is used.
* This library has heavy string allocations, most likely will make a string flyweight for it.
