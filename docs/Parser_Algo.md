# Parser's Algorithim

gencpp uses a hand-written recursive descent parser. Both the lexer and parser currently handle a full C/C++ file in a single pass.

## Notable implementation background

### Lexer

The lex procedure does the lexical pass of content provided as a `StrC` type.
The tokens are stored (for now) in `gen::parser::Tokens`.

Fields:

```cpp
Array<Token> Arr;
s32          Idx;
```


What token types are supported can be found in [ETokType.csv](../project/enums/ETokType.csv) you can also find the token types in [ETokType.h](../project/components/gen/etoktype.cpp) , which is the generated enum from the csv file.

Tokens are defined with the struct `gen::parser::Token`:

Fields:

```cpp
char const* Text;
sptr        Length;
TokType     Type;
s32         Line;
s32         Column;
u32         Flags;
```

Flags is a bitfield made up of TokFlags (Token Flags):

* `TF_Operator` : Any operator token used in expressions
* `TF_Assign`
  * Using statment assignment
  * Parameter argument default value assignment
  * Variable declaration initialization assignment
* `TF_Preprocess` : Related to a preprocessing directive
* `TF_Preprocess_Cond` : A preprocess conditional
* `TF_Attribute` : An attribute token
* `TF_AccessSpecifier` : An accesor operation token
* `TF_Specifier` : One of the specifier tokens
* `TF_EndDefinition` : Can be interpreted as an end definition for a scope.
* `TF_Formatting` : Considered a part of the formatting
* `TF_Literal` : Anything considered a literal by C++.

I plan to replace IsAssign with a general flags field and properly keep track of all operator types instead of abstracting it away to `ETokType::Operator`.

Traversing the tokens is done with the following interface macros:

| Macro | Description |
| --- | --- |
| `currtok_noskip` | Get the current token without skipping whitespace |
| `currtok` | Get the current token, skip any whitespace tokens |
| `prevtok` | Get the previous token (does not skip whitespace) |
| `nexttok` | Get the next token (does not skip whitespace) |
| `eat( Token Type )` | Check to see if the current token is of the given type, if so, advance Token's index to the next token |
| `left` | Get the number of tokens left in the token array |
| `check_noskip` | Check to see if the current token is of the given type, without skipping whitespace |
| `check` | Check to see if the current token is of the given type, skip any whitespace tokens |

### Parser

The parser has a limited user interface, only specific types of definitions or statements are expected to be provided by the user directly when using to construct an AST dynamically (See SOA for example). It however does attempt to provide capability to parse a full C/C++ from production codebases. 

Each public user interface procedure has the following format:

```cpp
<code type> parse_<definition type>( StrC def )
{
    check_parse_args( def );
    using namespace Parser;

    TokArray toks = lex( def );
    if ( toks.Arr == nullptr )
        return CodeInvalid;

    // Parse the tokens and return a constructed AST using internal procedures
    ...
}
```

The most top-level parsing procedure used for C/C++ file parsing is `parse_global_body`:

It uses a helper procedure called `parse_global_nspace`.

Each internal procedure will have the following format:

```cpp
internal
<code type> parse_<definition_type>( <empty or contextual params> )
{
    push_scope();

    ...

    <code type> result = (<code type>) make_code();
    ...

    Context.pop();
    return result;
}
```

Below is an outline of the general alogirithim used for these internal procedures. The intention is to provide a basic briefing to aid the user in traversing the actual code definitions. These appear in the same order as they are in the `parser.cpp` file

## `parse_array_decl`

1. Check if its an array declaration with no expression.
    1. Consume and return empty array declaration
2. Opening square bracket
3. Consume expression
4. Closing square bracket
5. If adjacent opening bracket
    1. Repeat array declaration parse until no brackets remain

## `parse_attributes`

1. Check for standard attribute
2. Check for GNU attribute
3. Check for MSVC attribute
4. Check for a token registered as an attribute

## `parse_class_struct`

1. Check for export module specifier
2. class or struct keyword
3. `parse_attributes`
4. If identifier : `parse_identifier`
5. Parse inherited parent or interfaces
6. If opening curly brace : `parse_class_struct_body`
7. If not an inplace definition
    1. End statement
    2. Check for inline comment

## `parse_class_struct_body`

1. Opening curly brace
2. Parse the body (Possible options):
    1. Newline : ast constant
    2. Comment : `parse_comment`
    3. Access_Public : ast constant
    4. Access_Protected : ast constant
    5. Access_Private : ast constant
    6. Decl_Class : `parse_complicated_definition`
    7. Decl_Enum : `parse_complicated_definition`
    8. Decl_Friend : `parse_friend`
    9. Decl_Operator : `parse_operator_cast`
    10. Decl_Struct : `parse_complicated_definition`
    11. Decl_Template : `parse_template`
    12. Decl_Typedef : `parse_typedef`
    13. Decl_Union : `parse_complicated_definition`
    14. Decl_Using : `parse_using`
    15. Operator == '~'
        1. `parse_destructor`
    16. Preprocess_Define : `parse_define`
    17. Preprocess_Include : `parse_include`
    18. Preprocess_Conditional (if, ifdef, ifndef, elif, else, endif) : `parse_preprocess_cond` or else/endif ast constant
    19. Preprocess_Macro : `parse_simple_preprocess`
    20. Preprocess_Pragma : `parse_pragma`
    21. Preprocess_Unsupported : `parse_simple_preprocess`
    22. StaticAssert : `parse_static_assert`
    23. The following compound into a resolved definition or declaration:
        1. Attributes (Standard, GNU, MSVC) : `parse_attributes`
        2. Specifiers (consteval, constexpr, constinit, forceinline, inline, mutable, neverinline, static, volatile)
        3. Possible Destructor : `parse_destructor`
        4. Possible User defined operator cast : `parse_operator_cast`
        5. Possible Constructor : `parse_constructor`
        6. Something that has the following: (identifier, const, unsigned, signed, short, long, bool, char, int, double)
            1. Possible Constructor `parse_constructor`
            2. Possible Operator, Function, or varaible : `parse_operator_function_or_variable`
    24. Something completely unknown (will just make untyped...) : `parse_untyped`

## `parse_comment`

1. Just wrap the token into a cached string ( the lexer did the processing )

## `parse_compilcated_definition`

This is a helper function used by the following functions to help resolve a declaration or definition:

* `parse_class_struct_body`
* `parse_global_nspace`
* `parse_union`

A portion of the code in `parse_typedef` is very similar to this as both have to resolve a similar issue.

1. Look ahead to the termination token (End statement)
2. Check to see if it fits the pattern for a forward declare
3. If the previous token was an identifier ( `token[-1]` ):
    1. Look back one more token : `[-2]`
    2. If the token has a closing brace its an inplace definition
    3. If the `token[-2]` is an identifier & `token[-3]` is the declaration type, its a variable using a namespaced type.
    4. If the `token[-2]` is an indirection, then its a variable using a namespaced/forwarded type.
    5. If any of the above is the case, `parse_operator_function_or_variable`
4. If the previous token was a closing curly brace, its a definition : `parse_forward_or_definition`
5. If the previous token was a closing square brace, its an array definition : `parse_operator_function_or_variable`

## `parse_define`

1. Define directive
2. Get identifier
3. Get Content

## `parse_forward_or_definition`

* Parse any of the following for either a forward declaration or definition:
    1. Decl_Class : `parse_class`
    2. Decl_Enum : `parse_enum`
    3. Decl_Struct : `parse_struct`
    4. Decl_Union : `parse_union`

## `parse_function_after_name`

This is needed as a function defintion is not easily resolvable early on, as such this function handles resolving a function
after its been made ceratin that the type of declaration or definition is indeed for a function signature.

By the point this function is called the following are known : export module flag, attributes, specifiers, return type, & name

1. `parse_parameters`
2. parse postfix specifiers (we do not check if the specifier here is correct or not to be here... yet)
3. If there is a body : `parse_body`
4. Otherwise :
     1. Statment end
     2. Check for inline comment

## `parse_function_body`

Currently there is no actual parsing of the function body. Any content with the braces is shoved into an execution AST node.
In the future statements and expressions will be parsed.

1. Open curly brace
2. Grab all tokens between the brace and the closing brace, shove them in a execution AST node.
3. Closing curly brace

## `parse_global_nspace`

1. Make sure this is being called for a valid type (namespace, global body, export body, linkage body)
2. If its not a global body, consume the opening curly brace
3. Parse the body (Possible options):
    1. NewLine : ast constant
    2. Comment : `parse_comment`
    3. Decl_Cass : `parse_complicated_definition`
    4. Decl_Enum : `parse_complicated_definition`
    5. Decl_Extern_Linkage : `parse_extern_link`
    6. Decl_Namespace : `parse_namespace`
    7. Decl_Struct : `parse_complicated_definition`
    8. Decl_Template : `parse_template`
    9. Decl_Typedef : `parse_typedef`
    10. Decl_Union : `parse_complicated_definition`
    11. Decl_Using : `parse_using`
    12. Preprocess_Define : `parse_define`
    13. Preprocess_Include : `parse_include`
    14. Preprocess_If, IfDef, IfNotDef, Elif : `parse_preprocess_cond`
    15. Preprocess_Else : ast constant
    16. Preprocess_Endif : ast constant
    17. Preprocess_Macro : `parse_simple_preprocess`
    18. Preprocess_Pragma : `parse_pragma`
    19. Preprocess_Unsupported : `parse_simple_preprocess`
    20. StaticAssert : `parse_static_assert`
    21. Module_Export : `parse_export_body`
    22. Module_Import : NOT_IMPLEMENTED
    23. The following compound into a resolved definition or declaration:
        1. Attributes ( Standard, GNU, MSVC, Macro ) : `parse_attributes`
        2. Specifiers ( consteval, constexpr, constinit, extern, forceinline, global, inline, internal_linkage, neverinline, static )
        3. Is either ( identifier, const specifier, long, short, signed, unsigned, bool, char, double, int)
            1. If its an operator cast (definition outside class) : `parse_operator_cast`
            2. Its an operator, function, or varaible : `parse_operator_function_or_varaible`
4. If its not a global body, consuem the closing curly brace

## `parse_identifier`



## `parse_include`

## `parse_operator_after_ret_type`

## `parse_operator_function_or_variable`

## `parse_pragma`

## `parse_params`

## `parse_preprocess_cond`

## `parse_simple_preprocess`

## `parse_static_assert`

## `parse_template_args`

## `parse_variable_after_name`

## `parse_variable_declaration_list`

## `parse_class`

## `parse_constructor`

## `parse_destructor`

## `parse_enum`

## `parse_export_body`

## `parse_extern_link_body`

## `parse_extern_link`

## `parse_friend`

## `parse_function`

## `parse_namespace`

## `parse_operator`

## `parse_operator_cast`

## `parse_struct`

## `parse_template`

## `parse_type`

## `parse_typedef`

1. Check for export module specifier
2. typedef keyword
3. If its a preprocess macro: Get the macro name
4. 

## `parse_union`

1. Check for export module specifier
2. union keyword
3. `parse_attributes`
4. Check for identifier
5. Parse the body (Possible options):
    1. Newline
    2. Comment
    3. Decl_Class
    4. Decl_Enum
    5. Decl_Struct
    6. Decl_Union
    7. Preprocess_Define
    8. Preprocess_Conditional (if, ifdef, ifndef, elif, else, endif)
    9. Preprocess_Macro
    10. Preprocess_Pragma
    11. Unsupported preprocess directive
    12. Variable
6. If its not an inplace definiton: End Statement

## `parse_using`

1. Check for export module specifier
2. using keyword
3. Check to see if its a using namespace
4. Get the identifier
5. If its a regular using declaration:
    1. `parse_attributes`
    2. `parse_type`
    3. `parse_array_decl`
6. End statement
7. Check for inline comment

## `parse_variable`

1. Check for export module specifier
2. `parse_attributes`
3. `parse specifiers`
4. `parse_type`
5. `parse_identifier`
6. `parse_variable_after_name`

