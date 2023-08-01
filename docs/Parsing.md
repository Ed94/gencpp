# Parsing

The library features a naive parser tailored for only what the library needs to construct the supported syntax of C++ into its AST.  
This parser does not, and should not do the compiler's job. By only supporting this minimal set of features, the parser is kept under 5000 loc.

The parsing implementation supports the following for the user:

```cpp
CodeClass     parse_class        ( StrC class_def     );
CodeEnum      parse_enum         ( StrC enum_def      );
CodeBody      parse_export_body  ( StrC export_def    );
CodeExtern    parse_extern_link  ( StrC exten_link_def);
CodeFriend    parse_friend       ( StrC friend_def    );
CodeFn        parse_function     ( StrC fn_def        );
CodeBody      parse_global_body  ( StrC body_def      );
CodeNamespace parse_namespace    ( StrC namespace_def );
CodeOperator  parse_operator     ( StrC operator_def  );
CodeOpCast    parse_operator_cast( StrC operator_def  );
CodeStruct    parse_struct       ( StrC struct_def    );
CodeTemplate  parse_template     ( StrC template_def  );
CodeType      parse_type         ( StrC type_def      );
CodeTypedef   parse_typedef      ( StrC typedef_def   );
CodeUnion     parse_union        ( StrC union_def     );
CodeUsing     parse_using        ( StrC using_def     );
CodeVar       parse_variable     ( StrC var_def       );
```

***Parsing will aggregate any tokens within a function body or expression statement to an untyped Code AST.***

Everything is done in one pass for both the preprocessor directives and the rest of the language.  
The parser performs no macro expansion as the scope of gencpp feature-set is to only support the preprocessor for the goal of having rudimentary awareness of preprocessor ***conditionals***,  ***defines***, and ***includes***, and ***`pragmas`**.  

The keywords supported for the preprocessor are:

* include
* define
* if
* ifdef
* elif
* endif
* undef
* pragma

Each directive `#` line is considered one preproecessor unit, and will be treated as one Preprocessor AST. *These ASTs will be considered members or entries of braced scope they reside within*.
All keywords except *include* are suppported as members of a scope for a class/struct, global, or namespace body.  

Any preprocessor definition abuse that changes the syntax of the core language is unsupported and will fail to parse if not kept within an execution scope (function body, or expression assignment).

Exceptions to the above rule (If its too hard to keep track of just follow the above notion):

* Typedefs allow of a macro exansion to be defined after the keyword; Ex: `typedef GEN_FILE_OPEN_PROC( file_open_proc );`

