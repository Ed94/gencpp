## Navigation

[Top](../Readme.md)

<- [docs - General](Readme.md)

## Current Design

`AST` is the actual managed node object for the library.  
Its raw and really not meant to be used directly.

All user interaction must be with its pointer so the type they deal with is `AST*`.  
In order to abstract away constant use of `AST*` its wrapped in a Code type which can be either:

When its the [C generated variant of the library](../gen_c_library/)
```c
typedef AST* Code;
tyepdef AST_<name>* Code<name>;
...
```

**or**

For C++:
```cpp
struct Code {
    AST* ast;
};
struct Code<name> {
    ...

    AST_<name>* ast;
};
```

The full definitions of all asts are within:

* [`ast.hpp`](../base/components/ast.hpp)
* [`ast_types.hpp`](../base/components/ast_types.hpp)
* [`code_types.hpp`](../base/components/code_types.hpp)

The C/C++ interface procedures are located with `ast.hpp` (for the Code type), and `code_types.hpp` for all others.

## Serialization

All code types can either serialize using a function of the pattern:

```c
StrBuilder <prefix>_to_string(Code code);
// or
<prefix>_to_string(Code code, StrBuilder& result);
```

Where the first generates strings allocated using Allocator_StringArena and the other appends an existing strings with their backed allocator.

Serialization of for the AST is defined for `Code` in [`ast.chpp`](../base/components/ast.cpp) with `code_to_strbuilder_ptr` & `code_to_string`.  
Serializtion for the rest of the code types is within [`code_serialization.cpp`](../base/components/code_serialization.cpp).  
Gencpp's serialization does not provide coherent formatting of the code. The user should use a formatter after serializing.
