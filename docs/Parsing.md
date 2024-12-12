## Navigation

[Top](../Readme.md)

<- [docs - General](Readme.md)

# Parsing

The library features a naive single-pass parser tailored for only what the library needs to construct the supported syntax of C++ into its AST for *"front-end"* meta-programming purposes.

This parser does not, and should not do the compiler's job. By only supporting this minimal set of features, the parser is kept (so far) around ~7000 loc. I hope to keep it under 10k loc worst case.

You can think of this parser as *frontend parser* vs a *semantic parser*. Its intuitively similar to WYSIWYG. What you ***precerive*** as the syntax from the user-side before the compiler gets a hold of it, is what you get.

User exposed interface:

```cpp
CodeClass       parse_class        ( Str class_def       );
CodeConstructor parse_constructor  ( Str constructor_def );
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

Each directive `#` line is considered one preproecessor unit, and will be treated as one Preprocessor AST. 
If a directive is used with an unsupported keyword its will be processed as an untyped AST.

The preprocessor lines are stored as members of their associated scope they are parsed within. ( Global, Namespace, Class/Struct )  
***Again (Its not standard): These ASTs will be considered members or entries of braced scope they reside within***

Any preprocessor definition abuse that changes the syntax of the core language is unsupported and will fail to parse if not kept within an execution scope (function body, or expression assignment).  
Exceptions:

* function signatures are allowed for a preprocessed macro: `neverinline MACRO() { ... }`
  * Disable with: `#define GEN_PARSER_DISABLE_MACRO_FUNCTION_SIGNATURES`
* typedefs allow for a preprocessed macro: `typedef MACRO();`
  * Disable with: `#define GEN_PARSER_DISABLE_MACRO_TYPEDEF`
* Macros can behave as typenames
* There is some macro support in paramters for functions or templates *(Specifically added to support parsing Unreal Engine source)*.

*(Exceptions are added on an on-demand basis)*
*(See functions `parse_operator_function_or_variable` and `parse_typedef` )*

Adding your own exceptions is possible by simply modifying the parser to allow for the syntax you need.

*Note: You could interpret this strictness as a feature. This would allow the user to see if their codebase or a third-party's codebase some some egregious preprocessor abuse.*

If a macro is not defined withint e scope of parsing a set of files, it can be defined beforehand by:

* Appending the [`PreprocessorDefines`](https://github.com/Ed94/gencpp/blob/a18b5b97aa5cfd20242065cbf53462a623cd18fa/base/components/header_end.hpp#L137) array.
  * For functional macros a "(" just needs to be added after the name like: `<name>(` so that it will tokenize its arguments as part of the token during lexing.
* Defining a CodeDefine using `def_define`. The definition will be processed by the interface for user into `PreprocessorDefines`.
  * This can be prevented by setting the optional prameter `dont_append_preprocess_defines`.

The lexing and parsing takes shortcuts from whats expected in the standard.

* Numeric literals are not checked for validity.
* The parse API treats any execution scope definitions with no validation and are turned into untyped Code ASTs. (There is a [todo](https://github.com/Ed94/gencpp/issues/49) to add support)
  * *This includes the assignment of variables.*
* Attributes ( `[[]]` (standard), `__declspec` (Microsoft), or `__attribute__` (GNU) )
  * Assumed to *come before specifiers* (`const`, `constexpr`, `extern`, `static`, etc) for a function or right afterthe return type.
  * Or in the usual spot for class, structs, (*right after the declaration keyword*)
  * typedefs have attributes with the type (`parse_type`)
* Parsing attributes can be extended to support user defined macros by defining `GEN_DEFINE_ATTRIBUTE_TOKENS` (see `gen.hpp` for the formatting)
  * This is useful for example: parsing Unreal `Module_API` macros.

Empty lines used throughout the file are preserved for formatting purposes during ast serialization.
