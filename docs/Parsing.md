## Navigation

[Top](../Readme.md)

<- [docs - General](Readme.md)

# Parsing

The library features a naive single-pass parser, tailored for only what the library needs; for construction of C++ code into gencpp's AST for *"front-end"* meta-programming purposes.

This parser does not, and should not do the compiler's job. By only supporting this minimal set of features, the parser is kept (so far) around ~7000 loc. I hope to keep it under 10-15k loc worst case.

You can think of this parser as *frontend parser* vs a *semantic parser*. Its intuitively similar to WYSIWYG. What you ***precerive*** as the syntax from the user-side before the compiler gets a hold of it, is what you get.

User exposed interface:

```cpp
CodeClass       parse_class        ( Str class_def       );
CodeConstructor parse_constructor  ( Str constructor_def );
CodeDefine      parse_define       ( Str define_def      );
CodeDestructor  parse_destructor   ( Str destructor_def  );
CodeEnum        parse_enum         ( Str enum_def        );
CodeBody        parse_export_body  ( Str export_def      );
CodeExtern      parse_extern_link  ( Str exten_link_def  );
CodeFriend      parse_friend       ( Str friend_def      );
CodeFn          parse_function     ( Str fn_def          );
CodeBody        parse_global_body  ( Str body_def        );
CodeNS          parse_namespace    ( Str namespace_def   );
CodeOperator    parse_operator     ( Str operator_def    );
CodeOpCast      parse_operator_cast( Str operator_def    );
CodeStruct      parse_struct       ( Str struct_def      );
CodeTemplate    parse_template     ( Str template_def    );
CodeType        parse_type         ( Str type_def        );
CodeTypedef     parse_typedef      ( Str typedef_def     );
CodeUnion       parse_union        ( Str union_def       );
CodeUsing       parse_using        ( Str using_def       );
CodeVar         parse_variable     ( Str var_def         );
```

To parse file buffers, use the `parse_global_body` function.

***Parsing will aggregate any tokens within a function body or expression statement to an untyped Code AST.***

Everything is done in one pass for both the preprocessor directives and the rest of the language.  
The parser performs no macro expansion as the scope of gencpp feature-set is to only support the preprocessor for the goal of having rudimentary awareness of preprocessor ***conditionals***,  ***defines***, ***includes***, and ***pragmas***.  

The keywords supported for the preprocessor are:

* include
* define
* if
* ifdef
* elif
* endif
* pragma

Each directive `#` line is considered one preproecessor unit, and will be treated as one Preprocessor AST node.
If a directive is used with an unsupported keyword its will be processed as an untyped AST.

The preprocessor lines are stored as members of their associated scope they are parsed within ( Global, Namespace, Class/Struct ).
***Again: These ASTs will be considered members or entries of braced scope they reside within***

Any preprocessor definition abuse that changes the syntax of the core language is unsupported and will fail to parse if not kept within an execution scope (function body, or expression assignment).  
Exceptions:

* varaible definitions are allowed for a preprocessed macro `extern MACRO();`
* function definitions are allowed for a preprocessed macro: `neverinline MACRO() { ... }`
  * Disable with: `#define GEN_PARSER_DISABLE_MACRO_FUNCTION_SIGNATURES`
* typedefs allow for a preprocessed macro: `typedef MACRO();`
  * Disable with: `#define GEN_PARSER_DISABLE_MACRO_TYPEDEF`
* Macros can behave as typenames
* There is some macro support in parameters for functions or templates *(Specifically added to support parsing Unreal Engine source)*.

*(Exceptions are added on an on-demand basis)*
*(See functions `parse_operator_function_or_variable` and `parse_typedef` )*

Adding your own exceptions is possible by simply modifying the parser to allow for the syntax you need.

*Note: You could interpret this strictness as a feature. This would allow the user to see if their codebase or a third-party's codebase contains some egregious preprocessor abuse.*

Macros used within a file should be registered by the user before parsing. This can be done two ways:

1. The register macro interface within [interface.hpp](../base/components/interface.hpp).
2. Using `def_define` to create a CodeDefine and making sure to not set `opts.dont_register_to_preprocess_macros` to `true`.

## Registering macros

While the registeration of macros in the meta-program's side for parsing can be considered tedius, its necessary for the parser to accurately resolve the macros intent in one pass (and it provides some hygenics by verifying that they are used as intended).

The following can be used to register a macro:

```c
GEN_API void register_macro( Macro macro );
GEN_API void register_macros( s32 num, ... );
GEN_API void register_macros_arr( s32 num, Macro* macros );
```

The Macro typename is defined with the following in [parser_types.hpp](../base/components/parser_types.hpp):

```c
struct Macro
{
    StrCached  Name;
    MacroType  Type;
    MacroFlags Flags;
};
```

The macro can be designated one of the following types:

* `MT_Expression`: Intended to resolve to an expression expansion.
* `MT_Statement`: Intended to resolve an statement expansion.
* `MT_Typename`: Intended to resolve to a typename.

Additioonally tthe following flags may be set:

* `MF_Functional`: The macro intended to be passed arguments are at least have the calling `()` as part of its usage.
* `MF_Expects_Body`: The parser should expect a braced-body `{ ... }` after the macro signature `<name> <params>`
* `MF_Allow_As_Identifier`: Will allow the macro to be an acceptable token/s when an `Tok_Identifier` is expected.
* `MF_Allow_As_Attribute`: Will allow the macro to be an acceptable token/s when an attribute token/s is expected.
* `MF_Allow_As_Definition`: Will allow the macro be an acceptable token/s when the parser expects a declartion or definition to resolve after attributes or specifiers have been identified beforehand.
  * This flag requires that the macro is of type `MT_Statement` to make any sense of usage.

If a macro is not defined the following warning will be issued if `GEN_BUILD_DEBUG=1` during lexing within [lexer.cpp](../base/components/lexer.cpp) - `lex_preprocessor_define`:

```c
log_fmt("Warning: '%S' was not registered before the lexer processed its #define directive, it will be registered as a expression macro\n"
    , name.Text 
);
```

Further within the same scope, the lexer will issue a warning if it detects a macro was not flagged as function but has an open parenthesis `(` token right after is name with no whitespace:

```c
log_fmt("Warning: %S registered macro is not flagged as functional yet the definition detects opening parenthesis '(' for arguments\n"
    , name.Text
);
```

Macros are tracked using a `MacroTable Macros;` defined as a member of the library's `Context`.

```c
typedef HashTable(Macro) MacroTable;
```

## Notes

* Empty lines used throughout the file are preserved for formatting purposes during ast serialization (they have a dedicated Token: `Tok_NewLine`).
* Numeric literals are not checked for validity.
* The parse API treats any execution scope definitions with no validation and are turned into untyped Code ASTs. (There is a [todo](https://github.com/Ed94/gencpp/issues/49) to add support)
  * *This includes the assignment of variables.*
* Attributes ( `[[]]` (standard), `__declspec` (Microsoft), or `__attribute__` (GNU) )
  * Assumed to *come before specifiers* (`const`, `constexpr`, `extern`, `static`, etc) for a function or right afterthe return type.
  * Or in the usual spot for class, structs, (*right after the declaration keyword*)
  * typedefs have attributes with the type (`parse_type`)
* Parsing attributes can be extended to support user defined macros by defining `GEN_DEFINE_ATTRIBUTE_TOKENS` (see `gen.hpp` for the formatting)
  * This is useful for example: parsing Unreal `Module_API` macros.

**The lexer & parser do not gracefully attempt to continue when it comes across incorrect code, and doesn't properly track errors into a listing (yet).**
