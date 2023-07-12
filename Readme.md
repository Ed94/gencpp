# gencpp

An attempt at simple staged metaprogramming for c/c++.

The library API is a compositon of code element constructors.  
These build up a code AST to then serialize with a file builder.

### TOC

* [Notes](#notes)
* [Usage](#usage)
* [Building](#notes)
* [Outline](#outline)
* [What is not provided](#what-is-not-provided)
* [The four constructors](#there-are-four-sets-of-interfaces-for-code-ast-generation-the-library-provides)
* [Predefined Codes](#predefined-codes)
* [Code generation and modification](#code-generation-and-modification)
* [On multithreading](#on-multi-threading)
* [Extending the library](#extending-the-library)
* [TODO](#todo)

## Notes

This project is not minimum feature complete yet.  
Version 1 will have C and a subset of C++ features available to it.  

I will generate with this library a C99 or 11 variant when Version 1 is complete.  
A single-header version will also be generated.

The size target is to stay under 5-6k sloc (data & interface code).  
With the dependency code being under 10000 sloc. (Containers, Memory, String handling, Language bloat)

Any dependencies from the zpl library will be exposed manually with using declarations into global scope.  
They will be removed when the library is feature complete for version 1 (zero dependencies milestone).

*Right now the constructors are working to some extent based on testing*

***The editor and scanner will NOT be implemented by version 1. They require alot code and the focus for version 1 is to have a robust constructor API and builder, witch a wide suite of usage examples in the tests for the project.***

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

The design uses a constructive builder sort of AST for the code to generate.  
The user is given `Code` objects that are used to build up the AST.

Example using each construction interface:

### Upfront


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

### Parse

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

### Untyped

```cpp
Code header = untyped_str( R("
    struct ArrayHeader
    {
        uw        Num;
        uw        Capacity;
        allocator Allocator;
    };
)");
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

## Building

An example of building is provided in the test directory.

There are two meson build files the one within test is the program's build specification.  
The other one in the gen directory within test is the metaprogram's build specification.

Both of them build the same source file: `test.cpp`. The only differences are that gen needs a different relative path to the include directories and defines the macro definition: `gen_time`.

This method is setup where all the metaprogram's code are the within the same files as the regular program's code.  
If in your use case, you decide to have exclusive separation or partial separation of the metaprogam's code from the program's code files, then your build configuration would need to change to reflect that (specifically the sources).

## Outline

### *WHAT IS NOT PROVIDED*

* Lambdas
* Vendor provided dynamic dispatch (virtuals) : `override` and `final` specifiers complicate the specifier parsing and serialization. (I'll problably end up adding in later)
* RTTI
* Exceptions
* Execution statement validation              : Execution expressions are defined using the untyped API.

Keywords kept from "Modern C++":

* constexpr : Great to store compile-time constants.
* consteval : Technically fine, need to make sure to execute in moderation.
* constinit : Better than constexpr at doing its job, however, its only c++ 20.
* export    : Useful if c++ modules ever come around to actually being usable.
* import    : ^^
* module    : ^^

When it comes to expressions:

**There is no support for validating expressions.**  
**The reason:** Its difficult to parse with not much of a benefit from doing so.  
Most of the time, the critical complex metaprogramming conundrums are producing the frame of abstractions around the expressions (which this library provides constructors to help validate, you can skip that process by using the untyped constructors).  
Its not very much a priority to add such a level of complexity to the library when there isn't a high reward or need for it.  
Especially when the priority is to keep this library small and easy to grasp for what it is.

When it comes to templates:

Only trivial template support is provided. the intention is for only simple, non-recursive subsitution.  
The parameters of the template are treated like regular parameter AST entries.  
This means that the typename entry for the parameter AST would be either:

* `class`
* `typename`
* A fundamental type, function, or pointer type.

Anything beyond this usage is not supported by parse_template for arguments (at least not intentionally).  
Use at your own mental peril...

*Concepts and Constraints are not supported, its usage is non-tirival substiution.*

### The Data & Interface

As mentioned in [Usage](#usage), the user is provided Code objects by calling the constructor's functions to generate them or find existing matches.

The AST is managed by the library and provided the user via its interface.  
However, the user may specificy memory configuration.

Data layout of AST struct:

```cpp
union {
    AST*          ArrStatic[AST::ArrS_Cap];
    Array< AST* > ArrDyn;
    StringCached  Content;
    SpecifierT    ArrSpecs[AST::ArrSpecs_Cap];
};
AST*              Parent;
StringCached      Name;
CodeT             Type;
OperatorT         Op;
ModuleFlag        ModuleFlags;
AccessSpec        ParentAccess;
u32               StaticIndex;
bool              DynamicEntries;
u8                _Align_Pad[3];
```

*`CodeT` is a typedef for `ECode::Type` which has an underlying type of `u32`*  
*`OperatorT` is a typedef for `EOperator::Type` which has an underlying type of `u32`*  
*`StringCahced` is a typedef for `char const*` to denote it is an interned string*

AST widths are setup to be AST_POD_Size.  
The width dictates how much the static array can hold before it must give way to using an allocated array:

```cpp
constexpr static
uw ArrS_Cap =
(     AST_POD_Size
    - sizeof(AST*)         // Parent
    - sizeof(StringCached) // Name
    - sizeof(CodeT)        // Type
    - sizeof(OperatorT)    // Op
    - sizeof(ModuleFlag)   // ModuleFlags
    - sizeof(AccessSpec)   // ParentAccess
    - sizeof(u32)          // StaticIndex
    - sizeof(bool)         // DynamicEntries
    - sizeof(u8) * 3 )     // _Align_Pad
/ sizeof(AST*);
```

*Ex: If the AST_POD_Size is 256 the capacity of the static array is 27.*

ASTs can be set to readonly by calling Code's lock() member function.  
Adding comments is always available even if the AST is set to readonly.

Data Notes:

* The allocator definitions used are exposed to the user incase they want to dictate memory usage
  * You'll find the memory handling in `init`, `gen_string_allocator`, `get_cached_string`, `make_code`, and `make_code_entries`.
* ASTs are wrapped for the user in a Code struct which is a warpper for a AST* type.
* Both AST and Code have member symbols but their data layout is enforced to be POD types.
* This library treats memory failures as fatal.
* Strings are stored in their own set of arenas. AST constructors use cached strings for names, and content.
  * `StringArenas`, `StringMap`, `Allocator_StringArena`, and `Allocator_StringTable` are the associated containers or allocators.
* Strings used for seralization and file buffers are not contained by those used for cached strings.
  * They are currently using `Memory::GlobalAllocator`, which are tracked array of arenas that grows as needed (adds buckets when one runs out).
  * Memory within the buckets is not resused, so its inherently wasteful (most likely will give non-cached strings their own tailored alloator later)

Two generic templated containers throughout the library:

* `template< class Type> struct Array`
* `template< class Type> struct HashTable`

Otherwise the library is free of any templates.

## There are three sets of interfaces for Code AST generation the library provides

* Upfront
* Parsing
* Untyped

### Upfront Construction

All component ASTs must be previously constructed, and provided on creation of the code AST.
The construction will fail and return InvalidCode otherwise.

Interface :``

* def_attributes
  * *This is preappened right before the function symbol, or placed after the class or struct keyword for any flavor of attributes used.*
  * *Its up to the user to use the desired attribute formatting: `[[]]` (standard), `__declspec` (Microsoft), or `__attribute__` (GNU).*
* def_comment
* def_class
* def_enum
* def_execution
  * *This is equivalent to untyped_str, except that its intended for use only in execution scopes.*
* def_extern_link
* def_friend
* def_function
* def_include
* def_module
* def_namespace
* def_operator
* def_operator_cast
* def_param
* def_params
* def_specifier
* def_specifiers
* def_struct
* def_template
* def_type
* def_typedef
* def_union
* def_using
* def_using_namespace
* def_variable

Bodies:

* def_class_body
* def_enum_body
* def_export_body
* def_extern_link_body
* def_function_body
  * *Use this for operator bodies as well*
* def_global_body
* def_namespace_body
* def_struct_body
* def_union_body

Usage:

```cpp
<name> = def_<function type>( ... );

Code <name>
{
    ...
    <name> = def_<function name>( ... );
}

```

### Parse construction

A string provided to the API is parsed for the intended language construct.

Interface :

* parse_class
* parse_enum
* parse_export_body
* parse_extern_link
* parse_friend
  * Purposefully are only support forward declares with this constructor.
* parse_function
* parse_global_body
* parse_namespace
* parse_operator
* parse_operator_cast
* parse_struct
* parse_template
* parse_type
* parse_typedef
* parse_union
* parse_using
* parse_variable

The lexing and parsing takes shortcuts from whats expected in the standard.

* Numeric literals are not check for validity.
* The parse API treats any execution scope definitions with no validation and are turned into untyped Code ASTs.
  * *This includes the assignment of variables.*
* Attributes ( `[[]]` (standard), `__declspec` (Microsoft), or `__attribute__` (GNU) )
  * Assumed to *come before specifiers* (`const`, `constexpr`, `extern`, `static`, etc) for a function
  * Or in the usual spot for class, structs, (*right after the declaration keyword*)
  * typedefs have attributes with the type (`parse_type`)
* As a general rule; if its not available from the upfront contructors, its not available in the parsing constructors.
  * *Upfront constructors are not necessarily used in the parsing constructors, this is just a good metric to know what can be parsed.*

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

### Untyped constructions

Code ASTs are constructed using unvalidated strings.

Interface :

* token_fmt_va
* token_fmt
* untyped_str
* untyped_fmt
* untyped_token_fmt

During serialization any untyped Code AST has its string value directly injected inline of whatever context the content existed as an entry within.  
Even though these are not validated from somewhat correct c/c++ syntax or components, it doesn't mean that Untyped code can be added as any component of a Code AST:

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
char const* template_str = txt(
    Code with {key} to replace with token_values
    ...
);
char const* gen_code_str = token_fmt( template, num_tokens, { token_key, token_value }, ... );
Code        <name>       = parse_<function name>( gen_code_str );
```

## Predefined Codes

The following are provided predefined by the library as they are commonly used:

* `access_public`
* `access_protected`
* `access_private`
* `module_global_fragment`
* `module_private_fragment`
* `pragma_once`
* `spec_const`
* `spec_consteval`
* `spec_constexpr`
* `spec_constinit`
* `spec_extern_linkage` (extern)
* `spec_global` (global macro)
* `spec_inline`
* `spec_internal_linkage` (internal macro)
* `spec_local_persist` (local_persist macro)
* `spec_mutable`
* `spec_ptr`
* `spec_ref`
* `spec_register`
* `spec_rvalue`
* `spec_static_member` (static)
* `spec_thread_local`
* `spec_volatile`
* `spec_type_signed`
* `spec_type_unsigned`
* `spec_type_short`
* `spec_type_long`
* `t_auto`
* `t_void`
* `t_int`
* `t_bool`
* `t_char`
* `t_wchar_t`

Optionally the following may be defined if `GEN_DEFINE_LIBRARY_CODE_CONSTANTS` is defined

* `t_b32`
* `t_s8`
* `t_s16`
* `t_s32`
* `t_s64`
* `t_u8`
* `t_u16`
* `t_u32`
* `t_u64`
* `t_sw`
* `t_uw`
* `t_f32`
* `t_f64`

## Extent of operator overload validation

The AST and constructors will be able to validate that the arguments provided for the operator type match the expected form:

* If return type must match a parameter
* If number of parameters is correct
* If added as a member symbol to a class or struct, that operator matches the requirements for the class (types match up)

The user is responsible for making sure the code types provided are correct
and have the desired specifiers assigned to them beforehand.

## Code generation and modification

There are three provided file interfaces:

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
  * File      : The file the symbol resides in. Leave null to indicate to search all files. Leave null to indicated all-file search.
  * Marker    : #define symbol that indicates a location or following signature is valid to manipulate. Leave null to indicate the signature should only be used.
  * Signature : Use a Code symbol to find a valid location to manipulate, can be further filtered with the marker. Leave null to indicate the marker should only be used.

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

Currently unsupported. The following changes would have to be made:

* Setup static data accesss with fences if more than one thread will generate ASTs ( or keep a different set for each thread)
* Make sure local peristent data of functions are also thread local.
* The builder should be done on a per-thread basis.
* Due to the design of the editor and scanner, it will most likely be best to make each file a job to process request entries on. Receipts should have an an array to store per thread. They can be combined to the final reciepts array when all files have been processed.

## Extending the library

This library is relatively very small, and can be extended without much hassle.

The untyped codes and builder/editor/scanner can be technically be used to circumvent
any sort of constrictions the library has with: modern c++, templates, macros, etc.

Typical use case is for getting define constants an old C/C++ library with the scanner:  
Code parse_defines() can emit a custom code AST with Macro_Constant type.

Another would be getting preprocessor or template metaprogramming Codes from Unreal Engine definitions, etc.

The rules for constructing the AST are largely bound the syntax rules for what can be composed with whichever version of C++ your targeting.

The convention you'll see used throughout the API of the library is as follows:

1. Check name or parameters to make sure they are valid for the construction requested
2. Create a code object using `make_code`.
3. Populate immediate fields (Name, Type, ModuleFlags, etc)
4. Populate sub-entires using `add_entry`. If using the default seralization function `to_string`, follow the order at which entires are expected to appear (there is a strong ordering expected).

Names or Content fields are interned strings and thus showed be cached using `get_cached_string` if its desired to preserve that behavior.

`def_operator` is the most sophisitacated constructor as it has multiple permutations of definitions that could be created that are not trivial to determine if valid.

# TODO

* Implement a context stack for the parsing, allows for accurate scope validation for the AST types.
* Make a test suite thats covers some base cases and zpl containers (+ anything else suitable)
* Finish support for module specifiers and standard/platform attributes.
* Remove full ZPL dependency, move into Bloat header/source only what is used.
* Generate a single-header library.
* Actually get to version 1.
* May be in need of a better name, I found a few repos with this same one...
