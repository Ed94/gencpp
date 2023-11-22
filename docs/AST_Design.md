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
The actual content per type of AST is covered within [AST_Types.md](AST_Types.md).

These are pure PODS that just have the lay members relevant to the type of AST node they represent.  
Each of them has a Code type alias specific to it.

Again, the simpliest case for these would be a type alias.

```cpp
using struct AST_Typedef CodeTypedef;
```

As of November 21st, 2023, the AST has had a strict layout for how its content is laid out.  
This will be abandoned during its redesign that will occur starting with support for statments & expressions for either execution and type declarations.  
Having a strict layout is too resctrictive vs allowing each AST type to have maximum control over the layout.

The redesign will occur after the following todos are addressed:

* [Improvements Lexer & Token struct#27](https://github.com/Ed94/gencpp/issues/27)
* [Generalize AST Flags to a single 4-byte flag#42](https://github.com/Ed94/gencpp/issues/42)
* [AST-Code Object Redesign.#38](https://github.com/Ed94/gencpp/issues/38)
* [Code-AST Documentation#40](https://github.com/Ed94/gencpp/issues/40)
* [AST::debug_str() improvements#33](https://github.com/Ed94/gencpp/issues/33)
* [AST::is_equal implemented and works with singleheader-test#31](https://github.com/Ed94/gencpp/issues/31)
* [Parser : Add ability to have a parse failure and continue with errors recorded.#35](https://github.com/Ed94/gencpp/issues/35)
* [Scanner : Add CodeFile#29](https://github.com/Ed94/gencpp/issues/29)
* [Auxiliary : AST visual debugger#36](https://github.com/Ed94/gencpp/issues/36)

