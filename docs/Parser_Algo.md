# Parser's Algorithim

gencpp uses a hand-written recursive descent parser. Both the lexer and parser handle a full C/C++ file in a single pass.

## Notable implementation background

### Lexer

The lex procedure does the lexical pass of content provided as a `StrC` type.
The tokens are stored (for now) in `gen::Parser::Tokens`.

Fields:
```cpp
Array<Token> Arr;
s32          Idx;
```


What token types are supported can be found in [ETokType.csv](../project/enums/ETokType.csv) you can also find the token types in [ETokType.h](../project/components/gen/etoktype.cpp) , which is the generated enum from the csv file.

Tokens are defined with the struct `gen::Parser::Token`:

Fields:
```cpp
char const* Text;
sptr        Length;
TokType     Type;
s32         Line;
s32         Column;
bool 	    IsAssign;
```

`IsAssign` is a flag that is set when the token is an assignment operator. Which is used for various purposes:

* Using statment assignment
* Parameter argument default value assignment
* Variable declaration initialization assignment

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
CodeStruct parse_<definition type>( StrC def )
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

Each internal procedure will be 

## parse_global_nspace

1. Make sure the type provided to the helper function is a `Namespace_Body`, `Global_Body`, `Export_Body`, `Extern_Linkage_body`.
2. If its not a `Global_Body` eat the opening brace for the scope.
3. 


## parse_type

This is the worst part of the parser. Because other than actual expression values in C++, typenames are the second worst thing to parse in the langauge.

