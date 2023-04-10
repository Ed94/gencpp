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

The size target of this library is to stay under 5000 sloc (data & interface code).
With the dependency code being under 10000 sloc. (Containers, Memory, String handling, Language bloat)

Any dependencies from the zpl library will be exposed manually with using declarations into global scope.
They will be removed when the library is feature complete for version 1 (zero dependencies milestone).

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

The design a constructive builder of a sort of AST for the code to generate.
The user is given `Code` typed objects that are used to build up the AST.

Example using each construction interface:

#### Upfront

```cpp
Code t_uw           = def_type( name(uw) );
Code t_allocator    = def_type( name(allocator) );
Code t_string_cosnt = def_type( name(char), def_specifiers( 2, ESpecifier::Const, ESpecifier::Ptr ) );

Code header;
{
    Code num       = def_variable( t_uw,        name(Num) );
    Code cap       = def_variable( t_uw,        name(Capacity) );
    Code mem_alloc = def_variable( t_allocator, name(Allocator) );
    Code body      = make_struct_body( num, cap, mem_alloc );

    header = def_struct( name(ArrayHeader), __, __, body );
}
```

#### Incremental

```cpp
// Types are done the same with upfront. Incremental does not have a full interface replacment.

// Get a Code AST from the CodePool.
Code header = make_struct( name(ArrayHeader) );
{
    // Make a struct body.
    Code body = header.body();

    // Members
    body->add( def_variable( t_uw,        name(Num)) );
    body->add( def_variable( t_uw,        name(Capacity)) );
    body->add( def_variable( t_allocator, name(Allocator)) );
}
```

#### Parse

```cpp
Code header = parse_struct( code(
    struct ArrayHeader
    {
        uw        Num;
        uw        Capacity;
        allocator Allocator;
    };
));

```

Parse will automatically generate any types that have not been used previously.

#### Undtyped

```cpp
Code header = untyped_str(
    R("struct ArrayHeader
    {
        #define Using_ArrayHeader_Data \
        uw        Num;                 \
        uw        Capacity;            \
        allocator Allocator;
        Using_ArrayHeader_Data
    };)"
);

```

`name` is a helper macro for providing a string literal with its size, intended for the name paraemter of functions.
`code` is a helper macro for providing a string literal with its size, but intended for code string parameters.

All three constrcuton interfaces will generate the following C code:

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

If you don't mind a low amount of macros (61 sloc), a DSL may be optionally defined with:

```cpp
GEN_DEFINE_DSL
```

Using the previous example to show usage:

```cpp
Code type_ns(uw)           = type( uw );
Code type_ns(allocator)    = type( allocator );
Code type_ns(string_const) = type( char, specifiers( Const, Ptr ) );

make( struct, ArrayHeader )
{
    Code
    body = ArrayHeader.body();
    body->add( variable( uw,        Num       ));
    body->add( variable( uw,        Capacity  ));
    body->add( variable( allocaotr, Allocator ));
}

// Or using parse!
Code type_ns(uw)           = type_code( uw );
Code type_ns(allocator)    = type_code( allocator );
Code type_ns(string_const) = type_code( char const* );

Code header = struct_code(
    struct ArrayHeader
    {
        uw        Num;
        uw        Capacity;
        allocator Allocator;
    };
);
```

`type_ns` is a helper macro for providing refering to a typename if using the c-namespace naming convention.

## Building

An example of building is provided in the test directory.

There are two meson build files the one within test is the program's build specification.
The other one in the gen directory within test is the metaprogram's build specification.

Both of them build the same source file: `test.cpp`. The only differences between them is that gen need a different relative path to the include directories and defines the macro definition: `gen_time`.

This method is setup where all the metaprogram's code are the within the same files as the regular program's code.
If in your use case, decide to have exclusive separation or partial separation of the metaprogam's code from the program's code files then your build configuration would need to change to reflect that (specifically the sources).

## Outline

### *WHAT IS NOT PROVIDED*

* Macro or template generation                : This library is to avoid those, adding support for them adds unnecessary complexity.
* Vendor provided dynamic dispatch (virtuals) : Roll your own, this library might roll its own vtable/interface generation helpers in the future.
* RTTI                                        : This is kinda covered with the last point, but just wanted to emphasize.
* Exceptions                                  : Most fo the
* Execution statment validation               : Execution expressions are defined using the untyped string API.

Keywords in from "Modern C++":

* constexpr : Great to store compile-time constants, (easier to garanteed when emitted from gentime)
* consteval : Technically fine so long as templates are not used. Need to make sure to execute in moderation.
* constinit : Better than constexpr at doing its job, however, its only c++ 20.
* export    : Useful if c++ modules ever come around to actually being usable.
* import    : ^^
* module    : ^^

These features are in as they either are not horrible when used conservatively or are a performance benefit (modules).

When it comes to excution statements:
There is no parse API for validating excution statements (possibly will add in the future, but very limited in what it can do).
This reason there isn't one: thats where the can of worms open for parsing validation.
For most metaprogramming (espcially for c/c++), expression validation is not necessary, it can be done by the compiler for the runtime program.
Most of the time, the critical complex metaprogramming conundrums are actaully producing the frame of abstractions around the expressions.
Thus its not very much a priority to add such a level of complexity to the library when there isn't a high reward or need for it.

To further this point, lets say you do have an error with an execution statment. It will either be caught by the c++ compiler when compiling the target program, or at runtime for the program.

* If its not caught by the compiler, the only downside is the error appers on the generated function. Those with knowledge of how that definition was generated know where to find the code that inlined that expression in that file for that definition.
* If its caught at runtime. The expression will be shown in a stack trace if debug symbols are enabled in the generated function body. Yet again those with knowledge of how that definition was generated know where to find the code that inlined that expression.

In both these cases will get objectively better debug information than you would normally get on most c++ compilers with complex macros or templates.

### The Data & Interface

As mentioned in [Usage](#Usage), the user is provided Code objects by calling the constructor functions to generate them or find existing matches.

The AST is managed by the library and provided the user via its interface prodedures.
However, the user may specificy memory configuration.

Data layout of AST struct:

```cpp
AST*              Parent;
CachedString      Name;
CachedString      Comment;
union {
    array(AST*)   Entries;
    CachedString  Content;
};
CodeT             Type;
OperatorT         Op;
bool              Readonly;
u8                _64_Align[23];
```

*`CodeT` is a typedef for `ECode::Type` which has an underlying type of `u32`*
*`OperatorT` is a typedef for `EOperator::Type` which has an underlying type of `u32`*

ASTs can be set to readonly by calling Code's lock() member function.
Adding comments is always available even if the AST is set to readonly.

Data Notes:

* The allocator definitions used are exposed to the user incase they want to dictate memory usage
  * You'll find the memory handling in `init`, `gen_string_allocator`, `get_cached_string`, `make_code`, and `make_code_entries`.
* ASTs are wrapped for the user in a Code struct which essentially a warpper for a AST* type.
* Both AST and Code have member symbols but their data layout is enforced to be POD types.
* This library treats memory failures as fatal.
* The default setup assumes large definition sets may be provided to bodies so AST::Entires are dynamic arrays.
  * They're allocated to arenas currently and are pretty wasteful if they go over their reserve size (its never recycled).
  * Most likely will need to implement a dynamic-sized bucket allocation strategy for the entry arrays if memory is getting stressed.
  * Otherwise if you are using fixed size entries and your definitions are under 128~512 entries for the body, you may be better of with a fixed-sized array.
* Strings are stored in their own set of arenas. AST constructors use cached strings for names, and content.

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
Code <name> = parse_<function name>( <string with code> );

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

### Builder is a similar object to the jai language's string_builder

* The purpose of it is to generate a file.
* A file is specified and opened for writting using the open( file_path) ) function.
* The code is provided via print( code ) function  will be seralized to its buffer.
* When all seralization is finished, use the write() command to write the buffer to the file.

### Editor is for editing a series of files based on a set of requests provided to it

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

### Scanner allows the user to generate Code ASTs by reading files

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

## On multi-threading

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

* Your compile time complexity becomes large.
* You enjoy actually *seeing* the generated code instead of just the error symbols or the pdb symbols.
* You value your debugging expereince, and would like to debug your metaprogram, without having to step through the debug version of the compiler (if you even can)
* Want to roll your own reflection system
* Want to maintain a series of libraries for internal use, but don't want to deal with manual merging as often when they update.
* Want to create tailored headers for your code or for your libraries since you usually don't need the majority of the code within them.

Then this might help you boostrap a toolset todo so.

# TODO

* May be in need of a better name, I found a few repos with this same one...
* Make a test suite made up of collections based of the ZPL library templated colllection macros and the memory module.
* Remove full ZPL dependency, move into Bloat header/source only what is used.
* Generate a single-header library.
* Generate a C-supported single-header library.
* Actually get to version 1.
* Review if the upfront or incremental constructors are actually a net benefit vs just using the parse constructors.
  * They exist as a artifact of learning what was possible or not possible with staged metaprogramming in C++ (the parse interface was the last to get fleshed out)
  * Most likely at least Incremental could possibly be removed in favor of just using the parse constructors.
  * Possible merits are ergonomics for very dynamic generation or performance reasons.
  * They'll most likely stay until its evident that they are not necessary.
* Review memory handling for the AST, specifically relating to:
  * Giving type asts a dedicated memory arenas.
  * Giving specifier definitions a dedicated memory arenas and hashtable lookup.
  * Possibly adding a dedicated block allocator for the dynamic arrays of AST::Entires.
