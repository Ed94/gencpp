# gencpp

An attempt at simple staged metaprogramming for c/c++.

The library API is a composition of code element constructors.  
These build up a code AST to then serialize with a file builder.

This code base attempts follow the [handmade philosophy](https://handmade.network/manifesto),  
its not meant to be a black box metaprogramming utility, its meant for the user to extend for their project domain.

## Notes

The project has reached an *alpha* state, all the current functionality works for the test cases but it will most likely break in many other cases.  
The [issues](https://github.com/Ed94/gencpp/issues) marked with v1.0 Feature indicate whats left before the library is considered feature complete.

A `natvis` and `natstepfilter` are provided in the scripts directory.

***The editor and scanner have not been implemented yet. The scanner will come first, then the editor.***

A C variant is hosted [here](https://github.com/Ed94/genc); I will complete it when this library is feature complete, it should be easier to make than this...

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
The user is provided `Code` objects that are used to build up the AST.

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
Code header = code_str(
    struct ArrayHeader
    {
        uw        Num;
        uw        Capacity;
        allocator Allocator;
    };
);
```

`name` is a helper macro for providing a string literal with its size, intended for the name parameter of functions.  
`code` is a helper macro for providing a string literal with its size, but intended for code string parameters.  
`args` is a helper macro for providing the number of arguments to varadic constructors.
`code_str` is a helper macro for writting `untyped_str( code( <content> ))`

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
*(The library currently uses clang-format for formatting, beaware its pretty slow...)*

## Building

See the [scripts directory](scripts/).
