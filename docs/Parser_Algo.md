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

Below is an outline of the general alogirithim used for these internal procedures. The intention is provide a basic briefing to aid the user in traversing the actual code definitions. These appear in the same order as they are in the `parser.cpp` file

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



## `parse_class_struct_body`

## `parse_comment`

## `parse_compilcated_definition`

## `parse_define`

## `parse_forward_or_definition`

## `parse_function_after_name`

## `parse_function_body`

## `parse_global_nspace`

1. Make sure the type provided to the helper function is a `Namespace_Body`, `Global_Body`, `Export_Body`, `Extern_Linkage_body`.
2. If its not a `Global_Body` eat the opening brace for the scope.
3. `

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
    8. Preprocess_Conditional
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

