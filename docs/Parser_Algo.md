## Navigation

[Top](../Readme.md)

<- [docs - General](Readme.md)

# Parser's Algorithim

gencpp uses a hand-written recursive descent parser. Both the lexer and parser currently handle a full C/C++ file in a single pass.

## Notable implementation background

### Lexer

File: [lexer.cpp](../base/components/lexer.cpp)

The `lex` procedure does the lexical pass of content provided as a `Str` type.  
The tokens are stored (for now) in `Lexer_Tokens`.

Fields:

```cpp
Array<Token> Arr;
s32          Idx;
```

What token types are supported can be found in [ETokType.csv](../base/enums/ETokType.csv) you can also find the token types in [ETokType.h](../base/components/gen/etoktype.cpp) , which is the generated enum from the csv file.

```cpp
struct Token
{
    Str     Text;
    TokType Type;
    s32     Line;
    s32     Column;
    u32     Flags;
}
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
* `TF_Macro_Functional` : Used to indicate macro token is flagged as `MF_Functional`.
* `TF_Macro_Expects_Body` : Used to indicate macro token is flagged as `MF_Expects_Body`.

### Parser

Files:

* [interface.parsering.cpp](../base/components/interface.parsing.cpp)
* [parser.cpp](../base/components/parser.cpp)

The parser has a limited user interface, only specific types of definitions or statements are expected to be provided by the user directly when using to construct an AST dynamically. It however does attempt to provide capability to parse a full C/C++ from production codebases.

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

The most top-level parsing procedure used for C/C++ file parsing is `parse_global_body`.  
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

The parsing implementation contains throughut the codeapths to indicate how far their contextual AST node has been resolved.

Example:

```c
internal
CodeFn parser_parse_function()
{
    push_scope();

    Specifier specs_found[16] = { Spec_NumSpecifiers };
    s32        NumSpecifiers = 0;

    CodeAttributes attributes = { nullptr };
    CodeSpecifiers specifiers = { nullptr };
    ModuleFlag     mflags     = ModuleFlag_None;

    if ( check(Tok_Module_Export) ) {
        mflags = ModuleFlag_Export;
        eat( Tok_Module_Export );
    }
    // <export>

    attributes = parse_attributes();
    // <export> <Attributes>

    while ( left && tok_is_specifier(currtok) )
    {
        Specifier spec = str_to_specifier( tok_to_str(currtok) );

        switch ( spec )
        {
            GEN_PARSER_FUNCTION_ALLOWED_SPECIFIER_CASES:
            break;

            default:
                log_failure( "Invalid specifier %S for functon\n%SB", spec_to_str(spec), parser_to_strbuilder(_ctx->parser) );
                parser_pop(& _ctx->parser);
                return InvalidCode;
        }

        if ( spec == Spec_Const )
            continue;

        specs_found[NumSpecifiers] = spec;
        NumSpecifiers++;
        eat( currtok.Type );
    }

    if ( NumSpecifiers ) {
        specifiers = def_specifiers_arr( NumSpecifiers, specs_found );
    }
    // <export> <Attributes> <Specifiers>

    CodeTypename ret_type = parser_parse_type(parser_not_from_template, nullptr);
    if ( cast(Code, ret_type) == Code_Invalid ) {
        parser_pop(& _ctx->parser);
        return InvalidCode;
    }
    // <export> <Attributes> <Specifiers> <ReturnType>

    Token name = parse_identifier(nullptr);
    _ctx->parser.Scope->Name = name.Text;
    if ( ! tok_is_valid(name) ) {
        parser_pop(& _ctx->parser);
        return InvalidCode;
    }
    // <export> <Attributes> <Specifiers> <ReturnType> <Name>

    CodeFn result = parse_function_after_name( mflags, attributes, specifiers, ret_type, name );
    // <export> <Attributes> <Specifiers> <ReturnType> <Name> ...

    parser_pop(& _ctx->parser);
    return result;
}
```

In the above `parse_function` implementation:

`// <intutive expression of AST component> ...`

Will be conventionlly used where by that point in time for the codepath: `<intutive expression of AST component>` should be resolved for the AST.

## Outline of parsing codepaths

Below is an outline of the general alogirithim used for these internal procedures. The intention is to provide a basic briefing to aid the user in traversing the actual code definitions. These appear in the same order as they are in the `parser.cpp` file

***NOTE: This is still heavily in an alpha state. A large swaph of this can change, make sure these docs are up to date before considering them 1:1 with the repo commit your considering.***

## `parse_array_decl`

1. Push parser scope
2. Check for empty array `[]`
   1. Return untyped string with single space if found
3. Check for opening bracket `[`
   1. Validate parser not at EOF
   2. Reject empty array expression
   3. Capture expression tokens until closing bracket
   4. Calculate expression span length
   5. Convert to untyped string
4. Validate and consume closing bracket `]`
5. Handle multi-dimensional case
   1. If adjacent `[` detected, recursively parse
   2. Link array expressions via Next pointer
6. Pop parser scope
7. Return array expression or NullCode on failure

## `parse_attributes`

1. Push parser scope and initialize tracking
   1. Store initial token position
   2. Initialize length counter
2. Process attributes while available 
   1. Handle C++ style attributes `[[...]]`
       1. Consume opening `[[`
       2. Capture content until closing `]]`
       3. Calculate attribute span length
   2. Handle GNU style `__attribute__((...))`
       1. Consume `__attribute__` keyword and opening `((`
       2. Capture content until closing `))`
       3. Calculate attribute span length
   3. Handle MSVC style `__declspec(...)`
       1. Consume `__declspec` and opening `(`
       2. Capture content until closing `)`
       3. Calculate attribute span length
   4. Handle macro-style attributes
       1. Consume attribute token
       2. If followed by parentheses
           1. Handle nested parentheses tracking
           2. Capture content maintaining paren balance
       3. Calculate attribute span length
3. Generate attribute code if content captured
   1. Create attribute text span from start position and length
   2. Strip formatting from attribute text
   3. Construct Code node
       1. Set type to `CT_PlatformAttributes`
       2. Cache and set Name and Content fields
   4. Return as CodeAttributes
4. Pop parser scope
5. Return NullCode if no attributes found

## `parse_class_struct`

1. Validate token type is class or struct
   1. Return InvalidCode if validation fails
2. Initialize class/struct metadata
   1. Access specifier (default)
   2. Parent class/struct reference
   3. Class/struct body
   4. Attributes
   5. Module flags
3. Parse module export flag if present
   1. Set ModuleFlag_Export
   2. Consume export token
4. Consume class/struct token
5. Parse attributes via `parse_attributes()`
6. Parse class/struct identifier
   1. Update parser scope name
7. Initialize interface array (4KB arena)
8. Parse inheritance/implementation
   1. If classifier token (`:`) present
       1. Parse access specifier if exists
       2. Parse parent class/struct name
       3. Parse additional interfaces
           1. Separated by commas
           2. Optional access specifiers
           3. Store in interface array
9. Parse class body if present
   1. Triggered by opening brace
   2. Parse via `parse_class_struct_body`
10. Handle statement termination
    1. Skip for inplace definitions
    2. Consume semicolon
    3. Parse inline comment if present
11. Construct result node
    1. Create class definition if Tok_Decl_Class
    2. Create struct definition if Tok_Decl_Struct
    3. Attach inline comment if exists
12. Cleanup interface array and return result

## `parse_class_struct_body`

1. Initialize scope and body structure
   1. Push parser scope
   2. Consume opening brace
   3. Create code node with `CT_Class_Body` or `CT_Struct_Body` type
2. Parse body members while not at closing brace
   1. Initialize member parsing state
       1. Code member (InvalidCode)
       2. Attributes (null)
       3. Specifiers (null)
       4. Function expectation flag
   2. Handle preprocessor hash if present
   3. Process member by token type in switch statement
       1. Statement termination - warn and skip
       2. Newline - format member
       3. Comments - parse comment
       4. Access specifiers - `public/protected/private`
       5. Declarations - `class/enum/struct/union/typedef/using`
       6. Operators - `destructors/casts`
       7. Preprocessor directives - `define/include/conditionals/pragmas`
       8. Preprocessor statement macros
       9. Report naked preprocossor expression macros detected as an error.
       10. Static assertions
       11. Attributes and specifiers
           1. Parse attributes
           2. Parse valid member specifiers
           3. Handle `attribute-specifier-attribute` case
       12. Identifiers and types
           1. Check for constructors
           2. Parse `operator/function/variable`
       13. Default - capture unknown content until closing brace
   4. Validate member parsing
       1. Return InvalidCode if member invalid
       2. Append valid member to body
3. Finalize body
   1. Consume closing brace
   2. Pop parser scope
   3. Return completed CodeBody

## `parse_comment`

1. Just wrap the token into a cached string ( the lexer did the processing )

## `parse_complicated_definition`

1. Initialize parsing context
   1. Push scope
   2. Set inplace flag false
   3. Get token array reference
2. Scan ahead for statement termination
   1. Track brace nesting level
   2. Find first semicolon at level 0
3. Handle declaration variants
   1. Forward declaration case
       1. Check if only 2 tokens before semicolon
       2. Parse via `parse_forward_or_definition`
   2. Function with trailing specifiers
       1. Identify trailing specifiers
       2. Check for function pattern
       3. Parse as `operator/function/variable`
       4. Return `InvalidCode` if pattern invalid
   3. Identifier-based declarations
       1. Check identifier patterns
           1. Inplace definition `{...} id;`
           2. Namespace type variable `which id id;`
           3. Enum with class qualifier
           4. `Pointer/reference` types
       2. Parse as `operator/function/variable` if valid
       3. Return `InvalidCode` if pattern invalid
   4. Basic type declarations
       1. Validate enum class pattern
       2. Parse via `parser_parse_enum`
       3. Return `InvalidCode` if invalid
   5. Direct definitions
       1. Handle closing brace - `parse_forward_or_definition`
       2. Handle array definitions - `parse_operator_function_or_variable`
       3. Return InvalidCode for unknown patterns

## `parse_assignment_expression`

1. Initialize expression parsing
   1. Null expression pointer
   2. Consume assignment operator token
   3. Capture initial expression token
2. Validate expression presence
   1. Check for immediate termination
   2. Return `InvalidCode` if missing expression
3. Parse balanced expression
   1. Track nesting level for
       1. Curly braces
       2. Parentheses
   2. Continue until
       1. End of input, or
       2. Statement terminator, or
       3. Unnested comma
   3. Consume tokens sequentially
4. Generate expression code
   1. Calculate expression span length
   2. Convert to untyped string
   3. Return expression node

## `parse_forward_or_definition`

1. Declaration type routing
   1. Class (`Tok_Decl_Class`) -> `parser_parse_class`
   2. Enum (`Tok_Decl_Enum`) -> `parser_parse_enum` 
   3. Struct (`Tok_Decl_Struct`) -> `parser_parse_struct`
   4. Union (`Tok_Decl_Union`) -> `parser_parse_union`
2. Error handling
   1. Return `InvalidCode` for unsupported token types
   2. Log failure with parser context

`is_inplace` flag propagates to specialized codepaths to maintain parsing context.

## `parse_function_after_name`

This is needed as a function defintion is not easily resolvable early on, as such this function handles resolving a function
after its been made ceratin that the type of declaration or definition is indeed for a function signature.

By the point this function is called the following are known : export module flag, attributes, specifiers, return type, & name

1. Parameter parsing
   1. Push scope
   2. Parse parameter list with parentheses
2. Post-parameter specifier processing
   1. Collect trailing specifiers
   2. Initialize or append to existing specifiers
3. Parse function termination
   1. Function body case
       1. Parse body if open brace found
       2. Validate body type (`CT_Function_Body` or `CT_Untyped`)
   2. Pure virtual case
       1. Handle "`= 0`" syntax
       2. Append pure specifier
   3. Forward declaration case
       1. Consume statement terminator
   4. Handle inline comments for all cases
4. Construct function node
   1. Strip whitespace from name
   2. Initialize `CodeFn` with base properties
       1. Name (cached, stripped)
       2. Module flags
   3. Set node type
       1. `CT_Function` if body present
       2. `CT_Function_Fwd` if declaration only
   4. Attach components
       1. Attributes if present
       2. Specifiers if present
       3. Return type
       4. Parameters if present
       5. Inline comment if present
5. Cleanup and return
   1. Pop scope
   2. Return completed function node

## `parse_function_body`

Currently there is no actual parsing of the function body. Any content with the braces is shoved into an execution AST node.
In the future statements and expressions will be parsed.

1. Initialize body parsing
   1. Push scope
   2. Consume opening brace
   3. Create CodeBody with CT_Function_Body type
2. Capture function content
   1. Record start token position
   2. Track brace nesting level
   3. Consume tokens while
       1. Input remains AND
       2. Not at unmatched closing brace
   4. Update level counters
       1. Increment on open brace
       2. Decrement on closing brace when level > 0
3. Process captured content
   1. Calculate content length via pointer arithmetic
   2. Create execution block if content exists
       1. Construct string span from start position and length
       2. Wrap in execution node
       3. Append to body
4. Finalize
   1. Consume closing brace
   2. Pop scope
   3. Return cast body node

## `parse_global_nspace`

1. State initialization
   1. Push parser scope
   2. Validate namespace type (Global, Namespace, Export, Extern Linkage)
   3. Consume opening brace for non-global scopes
   4. Initialize `CodeBody` with specified body type: `which`
2. Member parsing loop (while not at closing brace)
   1. Reset parse state
       * Member code
       * Attributes
       * Specifiers
       * Function expectation flag
   2. Member type handling
       1. Declarations
           * `Class/Struct/Union/Enum` via `parse_complicated_definition`
           * `Template/Typedef/Using` via dedicated parsers
           * `Namespace/Export/Extern` declarations
       2. Preprocessor directivess
           * Include/Define
           * Conditionals `(if / ifdef / ifndef / elif / else / endif)`
           * Pragmas
           * Preprocessor statement macros
           * Report naked preprocossor expression macros detected as an error.
       3. Comments/Formatting
           * Newlines
           * Comments
       4. Static assertions
   3. Attributes and specifiers
       1. Parse attributes if present
       2. Collect valid specifiers (max 16)
       3. Handle `consteval` for function expectation
   4. Identifier resolution
       1. Check `constructor/destructor` implementation
       2. Look ahead for user defined operator implementation outside of class
       3. Default to `operator/function/variable` parse
3. Member validation/storage
   1. Validate parsed member
   2. Append to body if valid
   3. Return `InvalidCode` on parse failure
4. Scope finalization
   1. Consume closing brace for non-global scopes
   2. Pop parser scope
   3. Return completed body

## `parse_global_nspace_constructor_destructor`

1. Forward Token Analysis
   1. Scan for parameter list opening parenthesis
   2. Template Expression Handling
      * Track template nesting depth
      * Account for nested parentheses within templates
      * Skip until template closure or parameter start

```cpp
      // Valid patterns:
      ClassName          ::  ClassName(...)
      ClassName          :: ~ClassName(...)
      ClassName< T ... > ::  ClassName(...)
```

2. Constructor/Destructor Identification
   1. Token Validation Sequence
      * Verify identifier preceding parameters
      * Check for destructor indicator (`~`)
      * Validate scope resolution operator (`::`)
   2. Left-side Token Analysis
      * Process nested template expressions
      * Maintain template/capture level tracking
      * Locate matching identifier token
3. Parser Resolution
   1. Name Pattern Validation
      * Compare identifier tokens for exact match
   2. Specialized Parsing
      * Route to `parser_parse_destructor` for '~' prefix
      * Route to `parser_parse_constructor` for direct match
   3. Apply specifiers to resulting node
4. Return result (`NullCode` on pattern mismatch)

### Implementation Constraints

* Cannot definitively distinguish nested namespaces with identical names
* Return type detection requires parser enhancement
* Template parameter validation is syntax-based only
* Future enhancement: Implement type parsing with rollback capability

## `parse_identifier`

This is going to get heavily changed down the line to have a more broken down "identifier expression" so that the qualifier, template args, etc, can be distinguished between the targeted identifier.  
The function can parse all of them, however the AST node compresses them all into a string.

1. Initialize identifier context
   1. Push parser scope
   2. Capture initial token as name
   3. Set scope name from token text
2. Process initial identifier component
   1. Consume identifier token
   2. Parse template arguments if present
3. Handle qualified identifiers (loop while `::` found)
   1. Consume static access operator
   2. Validate token sequence:
       1. Handle destructor operator (`~`)
           * Validate destructor parsing context
           * Update name span if valid
           * Return invalid on context mismatch
       2. Process member function pointer (`*`)
           * Set possible_member_function flag if context allows
           * Return invalid if pointer unexpected
       3. Verify identifier token follows
   3. Update identifier span
       1. Extend name length to include new qualifier
       2. Consume identifier token
       3. Parse additional template arguments
4. Return completed identifier token

Notes:

* Current implementation treats identifier as single token span
* TODO: Refactor to AST-based identifier representation for:
  * Better support for nested symbol resolution

## `parse_include`

1. Consume include directive
2. Consume the path

## `parse_operator_after_ret_type`

This is needed as a operator defintion is not easily resolvable early on, as such this function handles resolving a operator after its been made ceratin that the type of declaration or definition is indeed for a operator signature.

By the point this function is called the following are known : export module flag, attributes, specifiers, and return type

1. Initialize operator context
   1. Push scope
   2. Parse qualified namespace identifier
   3. Consume `operator` keyword
2. Operator identification
   1. Validate operator token presence
   2. Set scope name from operator token
   3. Map operator token to internal operator enum:
       * Arithmetic: `+, -, *, /, %`
       * Assignment: `+=, -=, *=, /=, %=, =`
       * Bitwise: `&, |, ^, ~, >>`
       * Logical: `&&, ||, !, ==`
       * Comparison: `<, >, <=, >=`
       * Member access: `->, ->*`
       * Special: `(), [], new, delete`
   4. Handle array variants for new/delete
3. Parameter and specifier processing
   1. Parse parameter list
   2. Handle multiply/member-pointer ambiguity
   3. Collect trailing specifiers
   4. Merge with existing specifiers
4. Function body handling
   1. Parse implementation if present
   2. Otherwise consume statement terminator
   3. Capture inline comments
5. Result construction
   1. Create operator node with:
       * Operator type
       * Namespace qualification
       * Parameters
       * Return type
       * Implementation body
       * Specifiers
       * Attributes
       * Module flags
   2. Attach inline comments
6. Pop scope

## `parse_operator_function_or_variable`

When this function is called, attribute and specifiers may have been resolved, however what comes next can still be either an operator, function, or varaible.

1. Initial Type Resolution
   1. Push parsing scope
   2. Handle macro definitions via `parse_macro_as_definition`
   3. Parse base type, validate result
   4. Exit on invalid type
2. Declaration Classification
   1. Scan token stream for `operator` keyword
   2. Track static symbol access
   3. Branch handling:
       * Operator overload: Forward to `parse_operator_after_ret_type`
       * Function/variable: Parse identifier and analyze context
3. Function/Variable Disambiguation
   1. Parse identifier
   2. Analyze token patterns:
       * Detect parameter capture via parenthesis
       * Check for constructor initialization pattern
       * Handle variadic argument cases
   3. Macro Expression Analysis:
       * Validate functional macros
       * Track parenthesis balance
       * Detect comma patterns
4. Declaration Parsing
   1. Function path
       * Verify function expectation (`consteval`)
       * Delegate to `parse_function_after_name`
   2. Variable path
       * Validate against function expectation
       * Forward to `parse_variable_after_name`

## `parse_macro_as_definition`

1. Validation
   1. Check token type (Tok_Preprocess_Macro_Stmt)
   2. Retrieve macro from lookup
   3. Verify `MF_Allow_As_Definition` flag
2. Macro Processing
   1. Parse via `parse_simple_preprocess`
   2. Maintain original token categorization
3. Definition Construction
   1. Format components:
       * Attributes (if present)
       * Specifiers (if present)
       * Macro content
   2. Build unified string representation
   3. Convert to untyped code node

Notes:

* Early exits return NullCode for non-qualifying macros
* TODO: Pending AST_Macro implementation for proper attribute/specifier support

## `parse_pragma`

1. Consume pragma directive
2. Process the token content into cached string

## `parse_params`

1. Parameter List Initialization
   1. Delimiter handling based on context
       * Parentheses: `(...)` for standard parameters
       * Angle brackets: `<...>` for template parameters
   2. Early return for empty parameter lists
   3. Initial parameter component initialization
       * Macro reference
       * Type information
       * Parameter value
       * Identifier token
2. Primary Parameter Processing
   1. Handle varadic arguments
   2. Process preprocessor macros (`UPARAM` style)
   3. Parse parameter sequence
       * Type information
       * Optional identifier
       * Post-name macro expressions
       * Default value expressions
   4. Value expression capture with nested structure tracking
       * Template depth counting
       * Parentheses balance
       * Text span calculation
3. Multi-Parameter Handling
   1. Parse comma-separated entries
   2. Maintain parameter structure
       * Macro context
       * Type information
       * Identifier caching
       * Post-name macro persistence
       * Value assignments
   3. Parameter list construction via `params_append`
4. Consume params capture termination token & return result.

## `parse_preprocess_cond`

1. Parse conditional directive
2. Process directive's content expression

## `parse_simple_preprocess`

1. Basic Setup
   1. Push scope
   2. Capture initial macro token
   3. Validate macro registration
       * Lookup in macro registry
       * Skip validation for unsupported macros
2. Functional Macro Processing
   1. Handle macro invocation
       * Parse opening parenthesis 
       * Track nested parenthesis level
       * Capture parameter content
       * Update macro span length
3. Macro Body Handling
   1. Process associated block if macro expects body
       * Parse curly brace delimited content
       * Track nesting level
       * Capture body content
   2. Handle statement termination
       * Context-specific semicolon handling
       * Process inline comments
       * Update macro span
4. Context-Specific Termination
   1. Special case handling
       * Enum context bypass
       * Typedef context validation
       * Global/class scope handling
   2. Statement termination rules
       * Process semicolons based on context
       * Update token span accordingly

Notes:

* Pending AST_Macro implementation for improved structure
* Current implementation uses simple token span capture

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

1. Initialization Processing
   1. Array dimension parsing
   2. Expression capture
       * Assignment expressions
       * Constructor initializations
       * Bitfield specifications
2. Expression Pattern Handling
   1. Direct assignment (`=`)
       * Parse assignment expression
   2. Brace initialization (`{}`)
       * Track nested braces
       * Capture initialization list
   3. Constructor initialization (`()`)
       * Track parenthesis nesting
       * Update initialization flag
   4. Bitfield specification (`:`)
       * Validate non-empty expression
       * Capture bitfield size
3. Multi-Variable Processing
   1. Handle comma-separated declarations
   2. Statement termination
       * Process semicolon
       * Capture inline comments
   3. Link variable chain via NextVar
4. AST Node Construction
   1. Core properties
       * Type (`CT_Variable`)
       * Name caching
       * Module flags
       * Value type
   2. Optional components
       * Array expression
       * Bitfield size
       * Attributes/Specifiers
       * Initialization value
       * Constructor flag
       * Parent/Next linkage

## `parse_variable_declaration_list`

1. Chain Initialization
   1. Initialize null variable chain head and tail
   2. Process while comma token present
2. Per-Variable Processing
   1. Specifier Collection
       * Validate specifier ordering (const after pointer)
       * Handle core specifiers: `ptr, ref, rvalue`
       * Maintain specifier chain integrity
       * Log invalid specifier usage but continue parsing
   2. Variable Declaration
       * Extract identifier name
       * Parse remainder via `parse_variable_after_name`
       * Note: Function pointers unsupported
3. Chain Management
   1. First Variable
       * Set as chain head and tail
   2. Subsequent Variables
       * Link to previous via NextVar
       * Establish parent reference
       * Update tail pointer

Limitations:

* No function pointer support

## `parser_parse_class`

1. `parse_class_struct`

## `parser_parse_constructor`

1. Core Parse Sequence
   1. Identifier extraction and parameter list capture
   2. Handle construction variants:
       * Colon-prefixed member initializer lists
       * Direct body implementation
       * Default/delete assignment forms
       * Forward declarations
2. Initializer List Processing
   1. Track nested parentheses balance
   2. Capture full initializer span
   3. Convert to untyped string representation
3. Implementation Variants
   1. Body implementation
       * Parse full function body
       * Set `CT_Constructor` type
   2. Forward declaration
       * Process terminator and comments
       * Set `CT_Constructor_Fwd` type
   3. Special forms
       * Handle assignment operator cases
       * Capture inline comments for declarations
4. AST Construction
   1. Core node attributes
       * Cached identifier name
       * Parameter list linkage
       * Specifier chain
   2. Optional components
       * Initializer list
       * Implementation body
       * Inline comments

## `parser_parse_define`

1. Token Stream Preparation
   1. Handle optional preprocessor hash
   2. Consume define directive
   3. Validate identifier presence
2. Define Node Initialization
   1. Construct CodeDefine with `CT_Preprocess_Define` type
   2. Cache identifier name
   3. Update scope context
3. Parameter Processing (Functional Macros)
   1. Initial parameter detection
       * Verify macro functionality
       * Initialize parameter list node (`CT_Parameters_Define`)
   2. Parameter chain construction
4. Content Handling
   1. Content validation
       * Verify presence
       * Handle empty content case with newline
   2. Content processing
       * Strip formatting
       * Preserve line termination
       * Create untyped node

## `parser_parse_destructor`

1. Context Validation
   1. Verify parser scope hierarchy
   2. Check global namespace context
   3. Process `virtual` specifier if present
2. Identifier Resolution
   1. Parse prefix identifier in global scope
   2. Validate destructor operator (`~`)
   3. Capture destructor name
   4. Enforce empty parameter list
3. Specifier Processing
   1. Handle pure virtual case (`= 0`)
       * Append `Spec_Pure` to specifiers
       * Set `pure_virtual` flag
   2. Process default specifier (= default)
       * Parse as assignment expression
   3. Validate specifier syntax
4. Implementation Processing
   1. Function body (non-pure case)
       * Parse complete body
       * Set `CT_Destructor` type
   2. Forward declaration
       * Handle statement termination
       * Process inline comments
       * Set `CT_Destructor_Fwd` type
5. AST Construction
   1. Build destructor node
   2. Handle qualified names
       * Concatenate prefix and identifier
   3. Attach components
       * Specifiers
       * Implementation body
       * Inline comments

## `parser_parse_enum`

1. Declaration Components
   1. Basic structure processing
       * Enum type detection (`enum/enum class`)
       * Attributes parsing
       * Identifier capture
   2. Underlying type resolution
       * Standard type parsing
       * Macro-based underlying type handling
       * Classifier token validation
2. Body Processing
   1. Entry parsing loop
       * Preprocessor directives (`#define, #if, #pragma`)
       * Enum member declarations
       * Comment preservation
       * Formatting tokens
   2. Entry value handling
       * Assignment expressions
       * `UMETA` macro support
       * Entry termination (commas)
   3. Token span calculation for entries
3. AST Construction
   1. Node type determination
       * `CT_Enum/CT_Enum_Class` for definitions
       * `CT_Enum_Fwd/CT_Enum_Class_Fwd` for declarations
   2. Component attachment
       * Name caching
       * Body linkage
       * Underlying type/macro
       * Attributes
       * Inline comments

## `parser_parse_export_body`

1. `parse_global_nspace`

## `parser_parse_extern_link_body`

1. `parse_global_nspace`

## `parser_parse_extern_link`

1. Consume `Tok_Decl_Extern_Linkage`
2. Consume the linkage identifier
3. `parse_extern_link_body`

## `parser_parse_friend`

1. Consume `friend`
2. Parse specifiers
3. `parse_type`
4. If the currok is an identifier its a function declaration or definition
   1. `parse_function_after_name`
5. Otherwise its a operator: `parse_operator_after_ret_type`
6. Consume end statement so long as its not a function definion
7. Check for inline comment, `parse_comment` if exists

## `parser_parse_function`

1. Check and parse for `export`
2. `parse_attributes`
3. Parse specifiers
4. `parse_type` for return type
5. `parse_identifier`
6. `parse_function_after_name`

## `parser_parse_namespace`

1. Consume namespace declaration
2. Parse identifier
3. `parse_global_namespace`

## `parser_parse_operator`

1. Check for and parse export declaration
2. `parse_attributes`
3. Parse specifiers
4. `parse_type`
5. `parse_operator_after_ret_type`

## `parser_parse_operator_cast`

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

## `parser_parse_struct`

1. `parse_class_struct`

## `parser_parse_template`

1. Initial State Configuration
   1. Module flag handling (`export` keyword)
   2. Template parameter parsing via `parse_params`
       * Uses specialized template capture mode
       * Validates parameter list integrity
2. Declaration Type Resolution
   1. Primary type dispatch
       * `Class/Struct/Union` declarations
       * Using declarations
   2. Function/Variable handling
       * Attribute collection
       * Specifier validation (16 max)
       * Function expectation detection
3. Special Case Processing
   1. Global namespace constructors/destructors
       * Context validation
       * Delegation to `parse_global_nspace_constructor_destructor`
   2. Operator cast implementations
       * Token lookahead for operator detection
       * Static symbol access validation
       * Cast parsing delegation
4. AST Construction
   1. Template node composition
       * `CT_Template` type assignment
       * Parameter linkage
       * Declaration binding
       * Module flag preservation

## `parser_parse_type`

This implementatin will be updated in the future to properly handle functional typename signatures.

### Current Algorithim

Anything that is in the qualifier capture of the function typename is treated as an expression abstracted as an untyped string

1. `parse_attributes`
2. Parse specifiers
3. If the `parse_type` was called from a template parse, check to see if class was used instead of typname and consume as name.
4. This is where things get ugly for each of these depend on what the next token is.
    1. If its an in-place definition of a class, enum, struct, or union:
    2. If its a decltype (Not supported yet but draft impl there)
    3. If its a compound native type expression (unsigned, char, short, long, int, float, dobule, etc )
    4. If its a typename amcro
    5. A regular type alias of an identifier
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
    4. If its a typename amcro
    5. A regular type alias of an identifier
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
    9. Preprocess_Macro (`MT_Statement` or `MT_Typename`)
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
