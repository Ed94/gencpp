# Forward

Was never satisfied with how I did the wrap of the management of the AST.  
For C++, the current design may be as good as it gets for the limitations of the langauge.

I'll at least try in this issue to brainstorm something simpiler without losing ergonomics.  
This will also be a good place to document the current design.

## Current Design

`AST` is the actual managed node object for the library.  
Its raw and really not meant to be used directly.

All user interaction must be with its pointer so the type they deal with is `AST*`.  
For user-facing code, they should never be giveen a nullptr. Instead, they should be given a designated `Invalid` AST node.

In order to abstract away constant use of `AST*`, I wanted to provide a wrapper for it.

The simpliest being just a type alias.

```cpp
using Code = AST*;
```

This is what the genc library would have to use due to its constraints of a langauge.
Anything else and it would either be an unergonomic mess of struct wrapping with a mess of macros & procedures to interface with it.

Further, to provide intuitive filters on the AST, there are AST types (covered in [AST_Types.md](AST_Types.md)).

These are pure PODS that just have the lay members relevant to the type of AST node they represent.  
Each of them has a Code type alias specific to it.

Again, the simpliest case for these would be a type alias.

```cpp
using struct AST_Typedef CodeTypedef;
```
