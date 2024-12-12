## Navigation

[Top](../Readme.md)

<- [docs - General](Readme.md)

# Parser's Algorithim

gencpp uses a hand-written recursive descent parser. Both the lexer and parser currently handle a full C/C++ file in a single pass.

## Notable implementation background

### Lexer

The lex procedure does the lexical pass of content provided as a `Str` type.  
The tokens are stored (for now) in `Lexer_Tokens`.

Fields:

```cpp
Array<Token> Arr;
s32          Idx;
```

What token types are supported can be found in [ETokType.csv](../base/enums/ETokType.csv) you can also find the token types in [ETokType.h](../base/components/gen/etoktype.cpp) , which is the generated enum from the csv file.

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
<code type> parse_<definition type>( Str def )
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

***NOTE: This is still heavily in an alpha state. A large swaph of this can change, make sure these docs are up to date before considering them 1:1 with the repo commit your considering.***

## `parse_array_decl`

1. Check if its an array declaration with no expression.
    1. Consume and return empty array declaration
2. Opening square bracket
3. Consume expression
4. Closing square bracket
5. If adjacent opening bracket
    1. Repeat array declaration parse until no brackets remain

## `parse_assignment_expression`

1. Eat the assignment operator
2. Make sure there is content or at least an end statement after.
3. Flatten the assignment expression to an untyped Code string.

## `parse_attributes`

1. Check for standard attribute
2. Check for GNU attribute
3. Check for MSVC attribute
4. Check for a token registered as an attribute
  a. Check and grab the arguments of a token registered of an attribute if it has any.
5. Repeat for chained attributes. Flatten them to a single attribute AST node.

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
    1. Ignore dangling end statements
    2. Newline : ast constant
    3. Comment : `parse_comment`
    4. Access_Public : ast constant
    5. Access_Protected : ast constant
    6. Access_Private : ast constant
    7. Decl_Class : `parse_complicated_definition`
    8. Decl_Enum : `parse_complicated_definition`
    9. Decl_Friend : `parse_friend`
    10. Decl_Operator : `parse_operator_cast`
    11. Decl_Struct : `parse_complicated_definition`
    12. Decl_Template : `parse_template`
    13. Decl_Typedef : `parse_typedef`
    14. Decl_Union : `parse_complicated_definition`
    15. Decl_Using : `parse_using`
    16. Operator == '~'
        1. `parse_destructor`
    17. Preprocess_Define : `parse_define`
    18. Preprocess_Include : `parse_include`
    19. Preprocess_Conditional (if, ifdef, ifndef, elif, else, endif) : `parse_preprocess_cond` or else/endif ast constant
    20. Preprocess_Macro : `parse_simple_preprocess`
    21. Preprocess_Pragma : `parse_pragma`
    22. Preprocess_Unsupported : `parse_simple_preprocess`
    23. StaticAssert : `parse_static_assert`
    24. The following compound into a resolved definition or declaration:
        1. Attributes (Standard, GNU, MSVC) : `parse_attributes`
        2. Specifiers (consteval, constexpr, constinit, explicit, forceinline, inline, mutable, neverinline, static, volatile, virtual)
        3. Possible Destructor : `parse_destructor`
        4. Possible User defined operator cast : `parse_operator_cast`
        5. Possible Constructor : `parse_constructor`
        6. Something that has the following: (identifier, const, unsigned, signed, short, long, bool, char, int, double)
            1. Possible Constructor `parse_constructor`
            2. Possible Operator, Function, or varaible : `parse_operator_function_or_variable`
    25. Something completely unknown (will just make untyped...) : `parse_untyped`

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
    5. If the `token[-2]` is an assign classifier, and the starting tokens were the which type with possible `class` token after, its an enum forward declaration.
    6. If any of the above is the case, `parse_operator_function_or_variable`
4. If the `token[2]` is a vendor fundamental type (builtin) then it is an enum forward declaration.
5. If the previous token was a closing curly brace, its a definition : `parse_forward_or_definition`
6. If the previous token was a closing square brace, its an array definition : `parse_operator_function_or_variable`

## `parse_define`

1. Define directive
2. Get identifier
3. Get Content (Optional)

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
    1. Ignore dangling end statements
    2. NewLine : ast constant
    3. Comment : `parse_comment`
    4. Decl_Cass : `parse_complicated_definition`
    5. Decl_Enum : `parse_complicated_definition`
    6. Decl_Extern_Linkage : `parse_extern_link`
    7. Decl_Namespace : `parse_namespace`
    8. Decl_Struct : `parse_complicated_definition`
    9. Decl_Template : `parse_template`
    10. Decl_Typedef : `parse_typedef`
    11. Decl_Union : `parse_complicated_definition`
    12. Decl_Using : `parse_using`
    13. Preprocess_Define : `parse_define`
    14. Preprocess_Include : `parse_include`
    15. Preprocess_If, IfDef, IfNotDef, Elif : `parse_preprocess_cond`
    16. Preprocess_Else : ast constant
    17. Preprocess_Endif : ast constant
    18. Preprocess_Macro : `parse_simple_preprocess`
    19. Preprocess_Pragma : `parse_pragma`
    20. Preprocess_Unsupported : `parse_simple_preprocess`
    21. StaticAssert : `parse_static_assert`
    22. Module_Export : `parse_export_body`
    23. Module_Import : NOT_IMPLEMENTED
    24. The following compound into a resolved definition or declaration:
        1. Attributes ( Standard, GNU, MSVC, Macro ) : `parse_attributes`
        2. Specifiers ( consteval, constexpr, constinit, extern, forceinline, global, inline, internal_linkage, neverinline, static )
        3. Is either ( identifier, const specifier, long, short, signed, unsigned, bool, char, double, int)
            1. Attempt to parse as construtor or destructor : `parse_global_nspace_constructor_destructor`
            2. If its an operator cast (definition outside class) : `parse_operator_cast`
            3. Its an operator, function, or varaible : `parse_operator_function_or_varaible`
4. If its not a global body, consume the closing curly brace

## `parse_global_nspace_constructor_destructor`

1. Look ahead for the start of the arguments for a possible constructor/destructor
2. Go back past the identifier
3. Check to see if its a destructor by checking for the `~`
4. Continue the next token should be a `::`
5. Determine if the next valid identifier (ignoring possible template parameters) is the same as the first identifier of the function.
6. If it is we have either a constructor or destructor so parse using their respective functions (`parse_constructor`, `parse_destructor`).

## `parse_identifier`

This is going to get heavily changed down the line to have a more broken down "identifier expression" so that the qualifier, template args, etc, can be distinguished between the targeted identifier.  
The function can parse all of them, however the AST node compresses them all into a string.

1. Consume first identifier
2. `parse_template_args`
3. While there is a static symbol accessor ( `::` )
    1. Consume `::`
    2. Consume member identifier
    3. `parse_template args` (for member identifier)
    4. If a `~` is encounted and the scope is for a destructor's identifier, do not consume it and return with what parsed.

## `parse_include`

1. Consume include directive
2. Consume the path

## `parse_operator_after_ret_type`

This is needed as a operator defintion is not easily resolvable early on, as such this function handles resolving a operator after its been made ceratin that the type of declaration or definition is indeed for a operator signature.

By the point this function is called the following are known : export module flag, attributes, specifiers, return type

1. If there is any qualifiers for the operator, parse them
2. Consume operator keyword
3. Determine the operator type (This will be offloaded to the lexer moreso than how it is now) & consume
4. `parse_params`
5. If there is no parameters this is operator is a member of pointer if its symbols is a *.
6. Parse postfix specifiers
7. If there is a opening curly brace, `parse function_body`
8. Otherwise: consume end statement, check for inline comment.

## `parse_operator_function_or_variable`

When this function is called, attribute and specifiers may have been resolved, however what comes next can still be either an operator, function, or varaible.

1. Check for preprocessor macro, if there is one : `parse_simple_preprocess`
2. `parse_type` (Does the bulk of the work)
3. Begin lookahead to see if we get qualifiers or we eventually find the operator declaration
4. If we find an operator keyword : `parse_operator_after_ret_type`
5. otherwise :
    1. `parse_identifier`
    2. If we se a opening parenthesis (capture start), its a function : `parse_function_after_name`
    3. Its a variable : `parse_variable_after_name`

## `parse_pragma`

1. Consume pragma directive
2. Process the token content into cached string

## `parse_params`

1. Consume either a `(` or `<` based on `use_template_capture` arg
2. If the we immdiately find a closing token, consume it and finish.
3. If we encounter a varadic argument, consume it and return a `param_varadic` ast constant
4. `parse_type`
5. If we have a macro, parse it (Unreal has macros as tags to parameters and or as entire arguments).
6. So long as next token isn't a comma
  a. If we have an identifier
    1. Consume it
    2. Check for assignment:
      a. Consume assign operator
      b. Parse the expression
7. While we continue to encounter commas
    a. Consume them
    b. Repeat steps 3 to 6.2.b
8. Consume the closing token

## `parse_preprocess_cond`

1. Parse conditional directive
2. Process directive's content expression

## `parse_simple_preprocess`

There is still decent room for improvement in this setup. Right now the entire macro's relevant tokens are shoved into an untyped AST. It would be better to store it instead in an `AST_Macro` node instead down the line.

1. Consume the macro token
2. Check for an opening curly brace
    1. Consume opening curly brace
    2. Until the closing curly is encountered consume all tokens.
    3. If the parent context is a typedef
        1. Check for end stement
            1. Consume it
            2. Consume potential inline comment
3. Otherwise do steps 3 to 3.1.2
4. Shove it all in an untyped string

## `parse_static_assert`

1. Consume static assert and opening curly brace
2. Consume all tokens until the the closing brace is reached.
3. Consume curly brace and end statement
4. Place all tokens within braces into a content for the assert.

## `parse_template_args`

This will get changed heavily once we have better support for typename expressions

1. Consume opening angle bracket
2. Consume all tokens until closing angle bracket
3. Consme closing angle bracket
4. Return the currtok with the ammended length.

## `parse_variable_after_name`

This is needed as a variable defintion is not easily resolvable early on, it takes a long evaluation period before its known that the declaration or definition is a variable. As such this function handles resolving a variable.

By the point this function is called the following are known : export module flag, attributes, specifiers, value type, name

1. If its an assignment, parse the assignment expression (currently to an untyped string)
2. If its an opening curly brace, parse the expression within (currnelty to an untyped stirng).
    1. Consume the closing curly brace
3. If its a `:`, we're dealing with bitfield definition:
    1. Consume the assign classifier
    2. Consume the expression (currently to an untyped string)
4. If a comma is encountered : `parse_variable declaration_list`
5. Consume statement end
6. Check for inline comment

## `parse_variable_declaration_list`

1. Consume the comma
2. Parse specifiers
3. `parse_variable_after_name`

## `parse_class`

1. `parse_class_struct`

## `parse_constructor`

This currently doesn't support postfix specifiers (planning to in the future)

1. `parse_identifier`
2. `parse_parameters`
3. If currtok is a `:`
    1. Consume `:`
    2. Parse the initializer list
    3. `parse_function_body`
4. If currtok is an opening curly brace
    1. `parse_function_body`
5. Otherwise:
    1. Consume statement end
    2. Check for inline comment

## `parse_destructor`

1. Check for and consume virtual specifier
2. Check for the `~` operator
3. `parse_identifier`
4. Consume opening and closing parenthesis
5. Check for assignment operator:
    1. Consume assignment op
    2. Consume pure specifier `0`
6. If not pure virtual & currtok is opening curly brace:
    1. `parse_function_body`
7. Otherwise:
    1. Consume end statement
    2. If currtok is comment : `parse_comment`

## `parse_enum`

1. Consume enum token
2. Check for and consume class token
3. `parse_attributes`
4. If there is an identifier consume it
5. Check for a `:`
    1. Consume `:`
    2. `parse_type`
6. If there is a body parse it (Consume `{`):
    1. Newline : ast constant
    2. Comment : `parse_comment`
    3. Preprocess_Define : `parse_define`
    4. Preprocess_Conditional (if, ifdef, ifndef, elif ) : `parse_preprocess_cond`
    5. Preprocess_Else : ast constant
    6. Preprocess_Endif : ast constant
    7. Preprocess_Macro : `parse_simple_preprocess`
    8. Preprocess_Pragma : `parse_pragma`
    9. Preprocess_Unsupported : `parse_smple_preprocess`
    10. An actual enum entry
        1. Consume identifier
        2. If there is an assignment operator:
            1. Consume operator
            2. Consume the expression (assigned to untyped string for now)
            3. If a macro is encountered consume it (Unreal UMETA macro support)
        3. If there is a comma, consume it

## `parse_export_body`

1. `parse_global_nspace`

## `parse_extern_link_body`

1. `parse_global_nspace`

## `parse_extern_link`

1. Consume Decl_Extern_Linkage
2. Consume the linkage identifier
3. `parse_extern_link_body`

## `parse_friend`

1. Consume `friend`
2. `parse_type`
3. If the currok is an identifier its a function declaration or definition
   1. `parse_function_after_name`
4. Consume end statement so long as its not a function definion
5. Check for inline comment, `parse_comment` if exists

## `parse_function`

1. Check and parse for `export`
2. `parse_attributes`
3. Parse specifiers
4. `parse_type`
5. `parse_identifier`
6. `parse_function_after_name`

## `parse_namespace`

1. Consume namespace declaration
2. Parse identifier
3. `parse_global_namespace`

## `parse_operator`

1. Check for and parse export declaration
2. `parse_attributes`
3. Parse specifiers
4. `parse_type`
5. `parse_operator_after_ret_type`

## `parse_operator_cast`

1. Look for and parse a qualifier namespace for the cast (in-case this is defined outside the class's scope)
2. Consume operator declaration
3. `parse_type`
4. Consume opening and closing parethesis
5. Check for a const qualifiying specifier
6. Check to see if this is a definition (`{`)
    1. Consume `{`
    2. Parse body to untyped string (parsing statement and expressions not supported yet)
    3. Consume `}`
7. Otherwise:
    1. Consume end statement
    2. Check for and consume comment : `parse_comment`


## `parse_struct`

1. `parse_class_struct`

## `parse_template`

Note: This currently doesn't support templated operator casts (going to need to add support for it)

1. Check for and parse export declaration
2. Consume template declaration
3. `parse_params`
4. Parse for any of the following:
   1. Decl_Class : `parse_class`
   2. Decl_Struct : `parse_struct`
   3. Decl_Union : `parse_union`
   4. Decl_Using : `parse_using`
   5. The following compound into a resolved definition or declaration:
       1. `parse_attributes`
       2. Parse specifiers
       3. Attempt to parse as constructor or destructor: `parse_global_nspace_constructor_destructor`
       4. Otherwise: `parse_operator_function_or_variable`

## `parse_type`

This function's implementation is awful and not done correctly. It will most likely be overhauled in the future as I plan to segement the AST_Type into several AST varaints along with sub-types to help produce robust type expressions.  
Hopefully I won't need to make authentic type expressions as I was hopeing to avoid that...

### Current Algorithim

Anything that is in the qualifier capture of the function typename is treated as an expression abstracted as an untyped string

1. `parse_attributes`
2. Parse specifiers
3. If the `parse_type` was called from a template parse, check to see if class was used instead of typname and consume as name.
4. This is where things get ugly for each of these depend on what the next token is.
    1. If its an in-place definition of a class, enum, struct, or union:
    2. If its a decltype (Not supported yet but draft impl there)
    3. If its a compound native type expression (unsigned, char, short, long, int, float, dobule, etc )
    4. Ends up being a regular type alias of an identifier
5. Parse specifiers (postfix)
6. We need to now look ahead to see If we're dealing with a function typename
7. If wer're dealing with a function typename:
    1. Shove the specifiers, and identifier code we have so far into a return type typename's Name (untyped string)
        1. Reset the specifiers code for the top-level typeanme
    2. Check to see if the next token is an identifier:
        1. `parse_identifier`
    3. Check to see if the next token is capture start and is not the last capture ("qualifier capture"):
        1. Consume `(`
        2. Consume expresssion between capture
        3. Consume `)`
    4. `parse_params`
    5. Parse postfix specifiers
8. Check for varaidic argument (param pack) token:
   1. Consume varadic argument token

### WIP - Alternative Algorithim

Currently wrapped up via macro: `GEN_USE_NEW_TYPENAME_PARSING`
Anything that is in the qualifier capture of the function typename is treated as an expression abstracted as an untyped string

1. `parse_attributes`
2. Parse specifiers (prefix)
3. This is where things get ugly for each of these depend on what the next token is.
    1. If its an in-place definition of a class, enum, struct, or union:
    2. If its a decltype (Not supported yet but draft impl there)
    3. If its a compound native type expression (unsigned, char, short, long, int, float, dobule, etc )
    4. Ends up being a regular type alias of an identifier
4. Parse specifiers (postfix)
    1. If any specifiers are found populate specifiers code with them.
5. We need to now look ahead to see If we're dealing with a function typename
6. If wer're dealing with a function typename:
    1. Shove the specifiers, and identifier code we have so far into a return type typename's Name (untyped string)
        1. Reset the specifiers code for the top-level typename
    2. Check to see if the next token is an identifier:
        1. `parse_identifier`
    3. Check to see if the next token is capture start and is not the last capture ("qualifier capture"):
        1. Consume `(`
        2. Parse binding specifiers
        3. `parse_identifier`
        4. `parse_parameters` -> params_nested
        5. Consume `)`
        6. Construct a nested function typename definition for the qualifier `Name`
    4. `parse_params` - > params
    5. Parse postfix specifiers
7. Check for varaidic argument (param pack) token:
   1. Consume varadic argument token

### **Later: Algorithim based on typename expressions**

## `parse_typedef`

1. Check for export module specifier
2. typedef keyword
3. If its a preprocess macro: Get the macro name
4. Else:
   1. Check to see if its a complicated definition (in-place enum, class, struct, union)
   2. If its a complicated definition:
       1. Perform the look ahead similar to `parse_complicated_definition`'s implementation
       2. Check to see if its a forward declaration : `parse_forward_declaration`
       3. If end[-1] is an identifier:
           1. Its either an in-place, varaible type qualified identifier, or indirection type:
              1. `parse_foward_or_definition`
       4. else if end[-1] is a closing curly brace
           1. Its a definition: `parse_forward_or_definition`
       5. else if end[-1] is a closing square brace
           2. Its an array definition: `parse_type`
   3. Else : `parse-type`
   4. Check for identifier : Consume the token
   5. `parse_array_decl`
5. Consume end statement
6. Check for inline comment : `parse_comment`

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
