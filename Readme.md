# gencpp

An attempt at simple staged metaprogramming for c/c++.

The library API is a composition of code element constructors.  
These build up a code AST to then serialize with a file builder.

General goal is to have a less than 15k sloc library that takes at most a couple of hours to learn and make use of.

*Why 15?* Assuming a seasoned coder of C++ can read and understand around 1000-2000 lines of code per hour, 15,000 could be understood in under 16-18 hours
and have confidence in modifying for their use case.

This code base attempts follow the [handmade philosophy](https://handmade.network/manifesto),  
its not meant to be a black box metaprogramming utility, its meant for the user to extend for their project domain.

### TOC

* [Notes](#notes)
* [Usage](#usage)
* [Building](#notes)
* [Outline](#outline)
* [What is not provided](#what-is-not-provided)
* [The three constructors ](#there-are-three-sets-of-interfaces-for-code-ast-generation-the-library-provides)
* [Predefined Codes](#predefined-codes)
* [Code generation and modification](#code-generation-and-modification)
* [On multithreading](#on-multi-threading)
* [Extending the library](#extending-the-library)
* [TODO](#todo)

## Notes

The project has reached an *alpha* state, all the current functionality works for the test cases but it will most likely break in many other cases.

The project has no external dependencies beyond:

* `errno.h`    (gen.dep.cpp)
* `stat.h`     (gen.dep.cpp)
* `stdarg.h`   (gen.dep.hpp)
* `stddef.h`   (gen.dep.hpp
* `stdio.h`    (gen.dep.cpp)
* `copyfile.h` (Mac, gen.dep.cpp)
* `types.h`    (Linux, gen.dep.cpp)
* `unistd.h`   (Linux/Mac, gen.dep.cpp)
* `intrin.h`   (Windows, gen.dep.hpp)
* `io.h`       (Windows with gcc, gen.dep.cpp)
* `windows.h`  (Windows, gen.dep.cpp)

Dependencies for the project are wrapped within `GENCPP_ROLL_OWN_DEPENDENCIES` (Defining it will disable them).  
The majority of the dependency's implementation was derived from the [c-zpl library](https://github.com/zpl-c/zpl).

This library was written a subset of C++ where the following are avoided:

* RAII (Constructors/Destructors), lifetimes are managed using named static or regular functions.
* Language provide dynamic dispatch, RTTI
* Object-Oriented Inheritance

Member-functions are used as an ergonomic choice, along with a conserative use of operator overloads.

A `natvis` and `natstepfilter` are provided in the scripts directory.

***The editor and scanner have not been implemented yet. The scanner will come first, then the editor.***

A C variant is hosted [here](https://github.com/Ed94/genc); I haven't gotten headwind on it, should be easier to make than this...

## Usage

A metaprogram is built to generate files before the main program is built. We'll term runtime for this program as `GEN_TIME`. The metaprogram's core implementation are within `gen.hpp` and `gen.cpp` in the project directory.

`gen.cpp` \`s  `main()` is defined as `gen_main()` which the user will have to define once for their program. There they will dictate everything that should be generated.

In order to keep the locality of this code within the same files the following pattern may be used:

Within `program.cpp` :

```cpp
#ifdef GEN_TIME
#include "gen.hpp"

...

u32 gen_main()
{
    ...
}
#endif

#ifndef GEN_TIME
#include "program.gen.cpp"

    // Regular runtime dependent on the generated code here.
#endif

```

The design uses a constructive builder API for the code to generate.  
The user is given `Code` objects that are used to build up the AST.

Example using each construction interface:

### Upfront


```cpp
Code t_uw           = def_type( name(uw) );
Code t_allocator    = def_type( name(allocator) );
Code t_string_const = def_type( name(char), def_specifiers( args( ESpecifier::Const, ESpecifier::Ptr ) ));

Code header;
{
    Code num       = def_variable( t_uw,        name(Num) );
    Code cap       = def_variable( t_uw,        name(Capacity) );
    Code mem_alloc = def_variable( t_allocator, name(Allocator) );
    Code body      = def_struct_body( args( num, cap, mem_alloc ) );

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

### Untyped

```cpp
Code header = untyped_str( code(
    struct ArrayHeader
    {
        uw        Num;
        uw        Capacity;
        allocator Allocator;
    };
));
```

`name` is a helper macro for providing a string literal with its size, intended for the name parameter of functions.  
`code` is a helper macro for providing a string literal with its size, but intended for code string parameters.  
`args` is a helper macro for providing the number of arguments to varadic constructors.

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

Both of them build the same source file: `test.cpp`. The only differences are that gen needs a different relative path to the include directories and defines the macro definition: `GEN_TIME`.

This method is setup where all the metaprogram's code are the within the same files as the regular program's code.  

## Outline

### *WHAT IS NOT PROVIDED*

* Exceptions
* Execution statement validation : Execution expressions are defined using the untyped API.
  * Lambdas (This naturally means its unsupported)
* RAII : This needs support for constructors/destructor parsing
  * Haven't gotten around to yet, only useful (to me) for third-party scanning

Keywords kept from "Modern C++":

* constexpr : Great to store compile-time constants.
* consteval : Technically fine, need to make sure to execute in moderation.
* constinit : Better than constexpr at doing its job, however, its only c++ 20.
* export    : Useful if c++ modules ever come around to actually being usable.
* import    : ^^
* module    : ^^

When it comes to expressions:

**There is no support for validating expressions.**  
Its difficult to parse without enough benefits (At the metaprogramming level).  

When it comes to templates:

Only trivial template support is provided. the intention is for only simple, non-recursive substitution.  
The parameters of the template are treated like regular parameter AST entries.  
This means that the typename entry for the parameter AST would be either:

* `class`
* `typename`
* A fundamental type, function, or pointer type.

Anything beyond this usage is not supported by parse_template for arguments (at least not intentionally).  
Use at your own mental peril.

*Concepts and Constraints are not supported, its usage is non-trivial substitution.*

### The Data & Interface

As mentioned in [Usage](#usage), the user is provided Code objects by calling the constructor's functions to generate them or find existing matches.

The AST is managed by the library and provided the user via its interface.  
However, the user may specifiy memory configuration.

Data layout of AST struct:

```cpp
union {
    struct
    {
        AST*      Attributes;     // Class, Enum, Function, Struct, Typedef, Union, Using, Variable
        AST*      Specs;          // Function, Operator, Type symbol, Variable
        union {
            AST*  ParentType;     // Class, Struct
            AST*  ReturnType;     // Function, Operator
            AST*  UnderlyingType; // Enum, Typedef
            AST*  ValueType;      // Parameter, Variable
        };
        AST*      Params;         // Function, Operator, Template
        union {
            AST*  ArrExpr;        // Type Symbol
            AST*  Body;           // Class, Enum, Function, Namespace, Struct, Union
            AST*  Declaration;    // Friend, Template
            AST*  Value;          // Parameter, Variable
        };
    };
    StringCached  Content;        // Attributes, Comment, Execution, Include
    SpecifierT    ArrSpecs[AST::ArrSpecs_Cap]; // Specifiers
};
union {
    AST* Prev;
    AST* Front; // Used by CodeBody
    AST* Last;  // Used by CodeParam
};
union {
    AST* Next;
    AST* Back;  // Used by CodeBody
};
AST*              Parent;
StringCached      Name;
CodeT             Type;
ModuleFlag        ModuleFlags;
union {
    OperatorT     Op;
    AccessSpec    ParentAccess;
    s32           NumEntries;
};
```

*`CodeT` is a typedef for `ECode::Type` which has an underlying type of `u32`*  
*`OperatorT` is a typedef for `EOperator::Type` which has an underlying type of `u32`*  
*`StringCahced` is a typedef for `String const`, to denote it is an interned string*  
*`String` is the dynamically allocated string type for the library*

AST widths are setup to be AST_POD_Size.
The width dictates how much the static array can hold before it must give way to using an allocated array:

```cpp
constexpr static
uw ArrSpecs_Cap =
(
        AST_POD_Size
        - sizeof(AST*) * 3
        - sizeof(StringCached)
        - sizeof(CodeT)
        - sizeof(ModuleFlag)
        - sizeof(s32)
)
/ sizeof(SpecifierT) -1; // -1 for 4 extra bytes (Odd num of AST*)
```

*Ex: If the AST_POD_Size is 128 the capacity of the static array is 20.*

Data Notes:

* The allocator definitions used are exposed to the user incase they want to dictate memory usage
  * You'll find the memory handling in `init`, `gen_string_allocator`, `get_cached_string`, `make_code`.
* ASTs are wrapped for the user in a Code struct which is a wrapper for a AST* type.
* Both AST and Code have member symbols but their data layout is enforced to be POD types.
* This library treats memory failures as fatal.
* Cached Strings are stored in their own set of arenas. AST constructors use cached strings for names, and content.
  * `StringArenas`, `StringCache`, `Allocator_StringArena`, and `Allocator_StringTable` are the associated containers or allocators.
* Strings used for serialization and file buffers are not contained by those used for cached strings.
  * They are currently using `GlobalAllocator`, which are tracked array of arenas that grows as needed (adds buckets when one runs out).
  * Memory within the buckets is not reused, so its inherently wasteful.
  * I will be augmenting the single arena with a simple slag allocator.
* Linked lists used children nodes on bodies, and parameters.
* Its intended to generate the AST in one go and serialize after. The constructors and serializer are designed to be a "one pass, front to back" setup.
* Allocations can be tuned by defining the folloiwng macros:
  * `GEN_GLOBAL_BUCKET_SIZE` : Size of each bucket area for the global allocator
  * `GEN_CODEPOOL_NUM_BLOCKS` : Number of blocks per code pool in the code allocator
  * `GEN_SIZE_PER_STRING_ARENA` : Size per arena used with string caching.
  * `GEN_MAX_COMMENT_LINE_LENGTH` : Longest length a comment can have per line.
  * `GEN_MAX_NAME_LENGTH` : Max length of any identifier.
  * `GEN_MAX_UNTYPED_STR_LENGTH` : Max content length for any untyped code.
  * `GEN_TOKEN_FMT_TOKEN_MAP_MEM_SIZE` : token_fmt_va uses local_persit memory of this size for the hashtable.
  * `GEN_LEX_ALLOCATOR_SIZE`
  * `GEN_BUILDER_STR_BUFFER_RESERVE`

Two generic templated containers are used throughout the library:

* `template< class Type> struct Array`
* `template< class Type> struct HashTable`

Both Code and AST definitions have a `template< class Type> Code/AST cast()`. Its just an alternative way to explicitly cast to each other.

Otherwise the library is free of any templates.

The following CodeTypes are used which the user may optionally use strong typing with if they enable: `GEN_ENFORCE_STRONG_CODE_TYPES`

* CodeBody : Has support for `for-range` iterating across Code objects.
* CodeAttributes
* CodeComment
* CodeClass
* CodeEnum
* CodeExec
* CodeExtern
* CodeInclude
* CodeFriend
* CodeFn
* CodeModule
* CodeNamespace
* CodeOperator
* CodeOpCast
* CodeParam : Has support for `for-range` iterating across parameters.
* CodeSpecifiers : Has support for `for-range` iterating across specifiers.
* CodeStruct
* CodeTemplate
* CodeType
* CodeTypedef
* CodeUnion
* CodeUsing
* CodeUsingNamespace
* CodeVar

Each Code boy has an associated "filtered AST" with the naming convention: `AST_<CodeName>`
Unrelated fields of the AST for that node type are omitted and only necessary padding members are defined otherwise.
Retrieving a raw version of the ast can be done using the `raw()` function defined in each AST.

## There are three sets of interfaces for Code AST generation the library provides

* Upfront
* Parsing
* Untyped

### Upfront Construction

All component ASTs must be previously constructed, and provided on creation of the code AST.
The construction will fail and return Code::Invalid otherwise.

Interface :``

* def_attributes
  * *This is pre-appended right before the function symbol, or placed after the class or struct keyword for any flavor of attributes used.*
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

* def_body
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

When using the body functions, its recommended to use the args macro to auto determine the number of arguments for the varadic:
```cpp
def_global_body( args( ht_entry, array_ht_entry, hashtable ));

// instead of:
def_global_body( 3, ht_entry, array_ht_entry, hashtable );
```

If a more incremental approach is desired for the body ASTs, `Code def_body( CodeT type )` can be used to create an empty body.  
When the members have been populated use: `AST::validate_body` to verify that the members are valid entires for that type.

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
* As a general rule; if its not available from the upfront constructors, its not available in the parsing constructors.
  * *Upfront constructors are not necessarily used in the parsing constructors, this is just a good metric to know what can be parsed.*
* Parsing attributes can be extended to support user defined macros by defining `GEN_Define_Attribute_Tokens` (see `gen.hpp` for the formatting)

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
Code <name> = def_variable( <type>, <name>, untyped_<function name>(
    <string with code>
));

Code <name> = untyped_str( code(
    <some code without "" quotes>
));
```

Optionally, `code_str`, and `code_fmt` macros can be used so that the code macro doesn't have to be used:
```cpp
Code <name> = code_str( <some code without "" quotes > )
```

Template metaprogramming in the traditional sense becomes possible with the use of `token_fmt` and parse constructors:

```cpp
StrC value = txt_StrC("Something");

char const* template_str = txt(
    Code with <key> to replace with token_values
    ...
);
char const* gen_code_str = token_fmt( "key", value, template_str );
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
* `spec_final`
* `spec_global` (global macro)
* `spec_inline`
* `spec_internal_linkage` (internal macro)
* `spec_local_persist` (local_persist macro)
* `spec_mutable`
* `spec_override`
* `spec_ptr`
* `spec_ref`
* `spec_register`
* `spec_rvalue`
* `spec_static_member` (static)
* `spec_thread_local`
* `spec_virtual`
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
* `t_class`
* `t_typename`

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
* A file is specified and opened for writing using the open( file_path) ) function.
* The code is provided via print( code ) function  will be serialized to its buffer.
* When all serialization is finished, use the write() command to write the buffer to the file.

### Editor is for editing a series of files based on a set of requests provided to it

**Note: Not implemented yet**

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

**Note: Not implemented yet**

* The purpose is to grab definitions to generate metadata or generate new code from these definitions.
* Requests are populated using the add( SymbolInfo, Policy ) function. The symbol info is the same as the one used for the editor. So is the case with Policy.

The file will only be read from, no writing supported.

One great use case is for example: generating the single-header library for gencpp!

### Additional Info (Editor and Scanner)

When all requests have been populated, call process_requests().  
It will provide an output of receipt data of the results when it completes.

Files may be added to the Editor and Scanner additionally with add_files( num, files ).  
This is intended for when you have requests that are for multiple files.

Request queue in both Editor and Scanner are cleared once process_requests completes.

## On multi-threading

Currently unsupported. The following changes would have to be made:

* Setup static data access with fences if more than one thread will generate ASTs ( or keep a different set for each thread)
* Make sure local persistent data of functions are also thread local.
* The builder should be done on a per-thread basis.
* Due to the design of the editor and scanner, it will most likely be best to make each file a job to process request entries on. Receipts should have an an array to store per thread. They can be combined to the final receipts array when all files have been processed.

## Extending the library

This library is relatively very small, and can be extended without much hassle.

The convention you'll see used throughout the API of the library is as follows:

1. Check name or parameters to make sure they are valid for the construction requested
2. Create a code object using `make_code`.
3. Populate immediate fields (Name, Type, ModuleFlags, etc)
4. Populate sub-entires using `add_entry`. If using the default serialization function `to_string`, follow the order at which entires are expected to appear (there is a strong ordering expected).

Names or Content fields are interned strings and thus showed be cached using `get_cached_string` if its desired to preserve that behavior.

`def_operator` is the most sophisticated constructor as it has multiple permutations of definitions that could be created that are not trivial to determine if valid.

If extendeding parsing capability, ECode, ESpecifier, may need to be modified along with making a new `AST_<Name>` and `Code<Name>` if its desired to preserve the same interface. The lexer (see the `Parser` namespace in `gen.cpp`) will most likely also need to be extended to support any unsupported or custom tokens.

# TODO

* Implement a context stack for the parsing, allows for accurate scope validation for the AST types. (Better errors)
  * Right now the parsing errors require a debugger in most cases.
* Make a more robust test suite.
* Generate a single-header library
  * Componetize the library, make a metaprogram using gencpp to bootstrap itself.
* Implement the Scanner
* Implement the Editor
* Should the builder be an "extension" header?
  * Technically the library doesn't require it and the user can use their own filesystem library.
  * It would allow me to remove the filesystem dependencies and related functions outside of gen base headers. ( At least 1k loc reduced )
  * ADT and the CSV parser depend on it as well. The `str_fmt_file` related functions do as well but they can be ommited.
  * Scanner and editor will also depend on it so they would need to include w/e the depency header for all three file-interacting interfaces.
    * `gen.dep.files.hpp`
* Convert GlobalAllocator to a slab allocator:

```md
Slab classes (for 100 mb arena)

1KB   slab: 5MB  ( 5MB  / 1KB   ~ 5,000 blocks )
4KB   slab: 10MB ( 10MB / 4KB   ~ 2,500 blocks )
16KB  slab: 15MB ( 15MB / 16KB  ~ 960 blocks   )
64KB  slab: 15MB ( 15MB / 64KB  ~ 240 blocks   )
256KB slab: 20MB ( 20MB / 256KB ~ 80 blocks    )
512KB slab: 20MB ( 20MB / 512KB ~ 40 blocks    )
1MB   slab: 15MB ( 15MB / 1MB   ~ 15 blocks    )
```
