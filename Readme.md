# gencpp

An attempt at simple staged metaprogramming for C/C++. Reflect and generate code for your codebase at runtime!

![splash-cpp](./docs/assets/Code_-_Insiders_2024-12-15_23-04-07.gif)
![splash-c](./docs/assets/Code_-_Insiders_2024-12-15_22-57-58.gif)

The library API is a composition of code element constructors, and a non-standards-compliant single-pass C/C++ parser.  
These build up a code AST to then serialize with a file builder, or can be traversed for staged-reflection of C/C++ code.

This code base attempts follow the [handmade philosophy](https://handmade.network/manifesto).  
Its not meant to be a black box metaprogramming utility, it should be easy to integrate into a user's project domain.

## Langauge Bindings

* [gencpp-odin](https://github.com/Ed94/gencpp-odin): Bindings for the odin programming language.

## Utility Libraries

* [UnrealGencpp](https://github.com/Ed94/UnrealGencpp): Setup as a plugin to integrate into Unreal Engine or Unreal Projects.

## Documentation

* [docs - General](./docs/Readme.md): Overview and additional docs
  * [AST_Design](./docs/AST_Design.md): Overview of ASTs
  * [AST Types](./docs/AST_Types.md): Listing of all AST types along with their Code type interface.
  * [Parsing](./docs/Parsing.md): Overview of the parsing interface.
  * [Parser Algo](./docs/Parser_Algo.md): In-depth breakdown of the parser's implementation.
* [base](./base/Readme.md): Essential (base) library.
* [gen_c_library](./gen_c_library/): C11 library variant generation (single header and segmented).
* [gen_segmented](./gen_segmented/): Segmented C++ (`gen.<hpp/cpp>`, `gen.dep.<hpp/cpp>`) generation
* [gen_singleheader](./gen_singleheader/): Singlehader C++ generation `gen.hpp`
* [gen_unreal_engine](./gen_unreal_engine/): Unreal Engine thirdparty code generation.

## Notes

This project is still in development (very much an alpha state), so expect bugs and missing features.  
See [issues](https://github.com/Ed94/gencpp/issues) for a list of known bugs or todos.

The library can already be used to generate code just fine, but the parser is where the most work is needed. If your C++ isn't "down to earth" expect issues.

A `natvis` and `natstepfilter` are provided in the scripts directory (its outdated, I'll update this readme when its not).  
*Minor update: I've been using [RAD Debugger](https://github.com/EpicGamesExt/raddebugger) with this and the code structures should be easy to debug even without natvis.*

## Usage

A metaprogram is built to generate files before the main program is built. We'll term runtime for this program as `GEN_TIME`. The metaprogram's core implementation are within `gen.hpp` and `gen.cpp` in the project directory.

`gen.cpp` \`s  `main()` is defined as `gen_main()` which the user will have to define once for their program. There they may reflect and/or generate code.

In order to keep the locality of this code within the same files the following pattern may be used (although this pattern isn't the best to use):

Within `program.cpp` :

```cpp
#ifdef GEN_TIME
#include "gen.hpp"

...

u32 gen_main()
{
    gen::Context ctx;
    gen::init(& ctx);
    ...
    gen::deinit(& ctx);
    return 0;
}
#endif

// "Stage" agnostic code.

#ifndef GEN_TIME
#include "program.gen.cpp"

    // Regular runtime dependent on the generated code here.
#endif
```

The design uses a constructive builder API for the code to generate.  
The user is provided `Code` objects that are used to build up the AST.

Example using each construction interface:

### Upfront

Validation and construction through a functional interface.

```cpp
CodeTypename t_uw           = def_type( name(usize) );
CodeTypename t_allocator    = def_type( name(allocator) );
CodeTypename t_string_const = def_type( name(char), def_specifiers( args( ESpecifier::Const, ESpecifier::Ptr ) ));

CodeStruct header;
{
    CodeVar  num       = def_variable( t_uw,        name(Num) );
    CodeVar  cap       = def_variable( t_uw,        name(Capacity) );
    CodeVar  mem_alloc = def_variable( t_allocator, name(Allocator) );
    CodeBody body      = def_struct_body( args( num, cap, mem_alloc ) );

    header = def_struct( name(ArrayHeader), { body });
}
```

### Parse

Validation through ast construction.

```cpp
CodeStruct header = parse_struct( code(
    struct ArrayHeader
    {
        usize     Num;
        usize     Capacity;
        allocator Allocator;
    };
));

```

### Untyped

No validation, just glorified text injection.

```cpp
Code header = code_str(
    struct ArrayHeader
    {
        usize     Num;
        usize     Capacity;
        allocator Allocator;
    };
);
```

`name` is a helper macro for providing a string literal with its size, intended for the name parameter of functions.  
`code` is a helper macro for providing a string literal with its size, but intended for code string parameters.  
`args` is a helper macro for providing the number of arguments to varadic constructors.  
`code_str` is a helper macro for writing `untyped_str( code( <content> ))`

All three construction interfaces will generate the following C code:

```cpp
struct ArrayHeader
{
    usize     Num;
    usize     Capacity;
    allocator Allocator;
};
```

**Note: The formatting shown here is not how it will look. For your desired formatting its recommended to run a pass through the files with an auto-formatter.**  
*(The library currently uses clang-format for formatting, beware its pretty slow...)*

## Building

See the [scripts directory](scripts/).

## Gallery

### Listing definitions in the Cuik Compiler

https://github.com/user-attachments/assets/2302240c-01f1-4e1b-a4b5-292eb3186648

### Unreal: Generating a UAttributeSet from a UDataTable

https://github.com/user-attachments/assets/2a07b743-825d-4f9f-beaf-3559e8748a4d
