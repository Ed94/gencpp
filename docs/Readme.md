## Documentation

The project has no external dependencies beyond:

* `errno.h`
* `stat.h`
* `stdarg.h`
* `stddef.h`
* `stdio.h`
* `copyfile.h` (Mac)
* `types.h`    (Linux)
* `unistd.h`   (Linux/Mac)
* `intrin.h`   (Windows)
* `io.h`       (Windows with gcc)
* `windows.h`  (Windows)

Dependencies for the project are wrapped within `GENCPP_ROLL_OWN_DEPENDENCIES` (Defining it will disable them).  
The majority of the dependency's implementation was derived from the [c-zpl library](https://github.com/zpl-c/zpl).

This library was written in a subset of C++ where the following are not used at all:

* RAII (Constructors/Destructors), lifetimes are managed using named static or regular functions.
* Language provide dynamic dispatch, RTTI
* Object-Oriented Inheritance
* Exceptions

Polymorphic & Member-functions are used as an ergonomic choice, along with a conserative use of operator overloads.  
There are only 4 template definitions in the entire library. (`Array<Type>`, `Hashtable<Type>`, `swap<Type>`, and `AST/Code::cast<Type>`)

Two generic templated containers are used throughout the library:

* `template< class Type> struct Array`
* `template< class Type> struct HashTable`

Both Code and AST definitions have a `template< class Type> Code/AST cast()`. Its just an alternative way to explicitly cast to each other.

`template< class Type> swap( Type& a, Type& b)` is used over a macro.

Otherwise the library is free of any templates.

### *WHAT IS NOT PROVIDED*

Keywords kept from "Modern C++":

* constexpr : Great to store compile-time constants.
* consteval : Technically fine, need to make sure to execute in moderation.
* constinit : Better than constexpr at doing its job, however, its only c++ 20.
* export    : Useful if c++ modules ever come around to actually being usable.
* import    : ^^
* module    : ^^

**There is no support for validating expressions.**  
Its difficult to parse without enough benefits (At the metaprogramming level).  

**Only trivial template support is provided.**  
The intention is for only simple, non-recursive substitution.  
The parameters of the template are treated like regular parameter AST entries.  
This means that the typename entry for the parameter AST would be either:

* `class`
* `typename`
* A fundamental type, function, or pointer type.

Anything beyond this usage is not supported by parse_template for arguments (at least not intentionally).  
Use at your own mental peril.

*Concepts and Constraints are not supported, its usage is non-trivial substitution.*

### The Data & Interface

As mentioned in root readme, the user is provided Code objects by calling the constructor's functions to generate them or find existing matches.

The AST is managed by the library and provided the user via its interface.  
However, the user may specifiy memory configuration.

Data layout of AST struct:

```cpp
union {
    struct
    {
        AST*      Attributes;      // Class, Enum, Function, Struct, Typedef, Union, Using, Variable
        AST*      Specs;           // Function, Operator, Type symbol, Variable
        union {
            AST*  InitializerList; // Constructor, Destructor
            AST*  ParentType;      // Class, Struct
            AST*  ReturnType;      // Function, Operator
            AST*  UnderlyingType;  // Enum, Typedef
            AST*  ValueType;       // Parameter, Variable
        };
        union {
            AST*  BitfieldSize;    // Varaiable (Class/Struct Data Member)
            AST*  Params;          // Function, Operator, Template
        };
        union {
            AST*  ArrExpr;         // Type Symbol
            AST*  Body;            // Class, Constructr, Destructor, Enum, Function, Namespace, Struct, Union
            AST*  Declaration;     // Friend, Template
            AST*  Value;           // Parameter, Variable
        };
    };
    StringCached  Content;                     // Attributes, Comment, Execution, Include
    SpecifierT    ArrSpecs[AST::ArrSpecs_Cap]; // Specifiers
};
union {
    AST* Prev;
    AST* Front;
    AST* Last;
};
union {
    AST* Next;
    AST* Back;
};
AST*              Parent;
StringCached      Name;
CodeT             Type;
ModuleFlag        ModuleFlags;
union {
    b32           IsFunction; // Used by typedef to not serialize the name field.
    OperatorT     Op;
    AccessSpec    ParentAccess;
    s32           NumEntries;
};
```

*`CodeT` is a typedef for `ECode::Type` which has an underlying type of `u32`*  
*`OperatorT` is a typedef for `EOperator::Type` which has an underlying type of `u32`*  
*`StringCahced` is a typedef for `String const`, to denote it is an interned string*  
*`String` is the dynamically allocated string type for the library*

AST widths are setup to be AST_POD_Size.
The width dictates how much the static array can hold before it must give way to using an allocated array:

```cpp
constexpr static
uw ArrSpecs_Cap =
(
    AST_POD_Size
    - sizeof(AST*) * 3
    - sizeof(StringCached)
    - sizeof(CodeT)
    - sizeof(ModuleFlag)
    - sizeof(u32)
)
/ sizeof(SpecifierT) -1; // -1 for 4 extra bytes (Odd num of AST*)
```

*Ex: If the AST_POD_Size is 128 the capacity of the static array is 20.*

Data Notes:

* The allocator definitions used are exposed to the user incase they want to dictate memory usage
  * You'll find the memory handling in `init`, `deinit`, `reset`, `gen_string_allocator`, `get_cached_string`, `make_code`.
* ASTs are wrapped for the user in a Code struct which is a wrapper for a AST* type.
* Both AST and Code have member symbols but their data layout is enforced to be POD types.
* This library treats memory failures as fatal.
* Cached Strings are stored in their own set of arenas. AST constructors use cached strings for names, and content.
  * `StringArenas`, `StringCache`, `Allocator_StringArena`, and `Allocator_StringTable` are the associated containers or allocators.
* Strings used for serialization and file buffers are not contained by those used for cached strings.
  * They are currently using `GlobalAllocator`, which are tracked array of arenas that grows as needed (adds buckets when one runs out).
  * Memory within the buckets is not reused, so its inherently wasteful.
  * I will be augmenting the single arena with a simple slag allocator.
* Linked lists used children nodes on bodies, and parameters.
* Its intended to generate the AST in one go and serialize after. The constructors and serializer are designed to be a "one pass, front to back" setup.
* Allocations can be tuned by defining the folloiwng macros:
  * `GEN_GLOBAL_BUCKET_SIZE` : Size of each bucket area for the global allocator
  * `GEN_CODEPOOL_NUM_BLOCKS` : Number of blocks per code pool in the code allocator
  * `GEN_SIZE_PER_STRING_ARENA` : Size per arena used with string caching.
  * `GEN_MAX_COMMENT_LINE_LENGTH` : Longest length a comment can have per line.
  * `GEN_MAX_NAME_LENGTH` : Max length of any identifier.
  * `GEN_MAX_UNTYPED_STR_LENGTH` : Max content length for any untyped code.
  * `GEN_TOKEN_FMT_TOKEN_MAP_MEM_SIZE` : token_fmt_va uses local_persit memory of this size for the hashtable.
  * `GEN_LEX_ALLOCATOR_SIZE`
  * `GEN_BUILDER_STR_BUFFER_RESERVE`

The following CodeTypes are used which the user may optionally use strong typing with if they enable: `GEN_ENFORCE_STRONG_CODE_TYPES`

* CodeBody : Has support for `for-range` iterating across Code objects.
* CodeAttributes
* CodeComment
* CodeClass
* CodeConstructor
* CodeDefine
* CodeDestructor
* CodeEnum
* CodeExec
* CodeExtern
* CodeInclude
* CodeFriend
* CodeFn
* CodeModule
* CodeNS
* CodeOperator
* CodeOpCast
* CodeParam : Has support for `for-range` iterating across parameters.
* CodePreprocessCond
* CodePragma
* CodeSpecifiers : Has support for `for-range` iterating across specifiers.
* CodeStruct
* CodeTemplate
* CodeType
* CodeTypedef
* CodeUnion
* CodeUsing
* CodeVar

Each Code boy has an associated "filtered AST" with the naming convention: `AST_<CodeName>`
Unrelated fields of the AST for that node type are omitted and only necessary padding members are defined otherwise.
Retrieving a raw version of the ast can be done using the `raw()` function defined in each AST.

## There are three sets of interfaces for Code AST generation the library provides

* Upfront
* Parsing
* Untyped

### Upfront Construction

All component ASTs must be previously constructed, and provided on creation of the code AST.
The construction will fail and return CodeInvalid otherwise.

Interface :``

* def_attributes
  * *This is pre-appended right before the function symbol, or placed after the class or struct keyword for any flavor of attributes used.*
  * *Its up to the user to use the desired attribute formatting: `[[]]` (standard), `__declspec` (Microsoft), or `__attribute__` (GNU).*
* def_comment
* def_class
* def_constructor
* def_define
* def_destructor
* def_enum
* def_execution
  * *This is equivalent to untyped_str, except that its intended for use only in execution scopes.*
* def_extern_link
* def_friend
* def_function
* def_include
* def_module
* def_namespace
* def_operator
* def_operator_cast
* def_param
* def_params
* def_preprocess_cond
* def_specifier
* def_specifiers
* def_struct
* def_template
* def_type
* def_typedef
* def_union
* def_using
* def_using_namespace
* def_variable

Bodies:

* def_body
* def_class_body
* def_enum_body
* def_export_body
* def_extern_link_body
* def_function_body
  * *Use this for operator bodies as well*
* def_global_body
* def_namespace_body
* def_struct_body
* def_union_body

Usage:

```cpp
<name> = def_<function type>( ... );

Code <name>
{
    ...
    <name> = def_<function name>( ... );
}

```

When using the body functions, its recommended to use the args macro to auto determine the number of arguments for the varadic:
```cpp
def_global_body( args( ht_entry, array_ht_entry, hashtable ));

// instead of:
def_global_body( 3, ht_entry, array_ht_entry, hashtable );
```

If a more incremental approach is desired for the body ASTs, `Code def_body( CodeT type )` can be used to create an empty body.  
When the members have been populated use: `AST::validate_body` to verify that the members are valid entires for that type.

### Parse construction

A string provided to the API is parsed for the intended language construct.

Interface :

* parse_class
* parse_constructor
* parse_destructor
* parse_enum
* parse_export_body
* parse_extern_link
* parse_friend
  * Purposefully are only support forward declares with this constructor.
* parse_function
* parse_global_body
* parse_namespace
* parse_operator
* parse_operator_cast
* parse_struct
* parse_template
* parse_type
* parse_typedef
* parse_union
* parse_using
* parse_variable

Usage:

```cpp
Code <name> = parse_<function name>( string with code );

Code <name> = def_<function name>( ..., parse_<function name>(
    <string with code>
));
```

### Untyped constructions

Code ASTs are constructed using unvalidated strings.

Interface :

* token_fmt_va
* token_fmt
* untyped_str
* untyped_fmt
* untyped_token_fmt

During serialization any untyped Code AST has its string value directly injected inline of whatever context the content existed as an entry within.  
Even though these are not validated from somewhat correct c/c++ syntax or components, it doesn't mean that Untyped code can be added as any component of a Code AST:

* Untyped code cannot have children, thus there cannot be recursive injection this way.
* Untyped code can only be a child of a parent of body AST, or for values of an assignment (ex: variable assignment).

These restrictions help prevent abuse of untyped code to some extent.

Usage Conventions:

```cpp
Code <name> = def_variable( <type>, <name>, untyped_<function name>(
    <string with code>
));

Code <name> = untyped_str( code(
    <some code without "" quotes>
));
```

Optionally, `code_str`, and `code_fmt` macros can be used so that the code macro doesn't have to be used:
```cpp
Code <name> = code_str( <some code without "" quotes > )
```

Template metaprogramming in the traditional sense becomes possible with the use of `token_fmt` and parse constructors:

```cpp
StrC value = txt_StrC("Something");

char const* template_str = txt(
    Code with <key> to replace with token_values
    ...
);
char const* gen_code_str = token_fmt( "key", value, template_str );
Code        <name>       = parse_<function name>( gen_code_str );
```

## Predefined Codes

The following are provided predefined by the library as they are commonly used:

* `access_public`
* `access_protected`
* `access_private`
* `attrib_api_export`
* `attrib_api_import`
* `module_global_fragment`
* `module_private_fragment`
* `fmt_newline`
* `pragma_once`
* `param_varaidc` (Used for varadic definitions)
* `preprocess_else`
* `preprocess_endif`
* `spec_const`
* `spec_consteval`
* `spec_constexpr`
* `spec_constinit`
* `spec_extern_linkage` (extern)
* `spec_final`
* `spec_global` (global macro)
* `spec_inline`
* `spec_internal_linkage` (internal macro)
* `spec_local_persist` (local_persist macro)
* `spec_mutable`
* `spec_neverinline`
* `spec_override`
* `spec_ptr`
* `spec_pure`
* `spec_ref`
* `spec_register`
* `spec_rvalue`
* `spec_static_member` (static)
* `spec_thread_local`
* `spec_virtual`
* `spec_volatile`
* `t_empty` (Used for varaidc macros)
* `t_auto`
* `t_void`
* `t_int`
* `t_bool`
* `t_char`
* `t_wchar_t`
* `t_class`
* `t_typename`

Optionally the following may be defined if `GEN_DEFINE_LIBRARY_CODE_CONSTANTS` is defined

* `t_b32`
* `t_s8`
* `t_s16`
* `t_s32`
* `t_s64`
* `t_u8`
* `t_u16`
* `t_u32`
* `t_u64`
* `t_sw`
* `t_uw`
* `t_f32`
* `t_f64`

## Extent of operator overload validation

The AST and constructors will be able to validate that the arguments provided for the operator type match the expected form:

* If return type must match a parameter
* If number of parameters is correct
* If added as a member symbol to a class or struct, that operator matches the requirements for the class (types match up)

The user is responsible for making sure the code types provided are correct
and have the desired specifiers assigned to them beforehand.

## Code generation and modification

There are three provided file interfaces:

* Builder
* Editor
* Scanner

Editor and Scanner are disabled by default, use `GEN_FEATURE_EDITOR` and `GEN_FEATURE_SCANNER` to enable them.

### Builder is a similar object to the jai language's string_builder

* The purpose of it is to generate a file.
* A file is specified and opened for writing using the open( file_path) ) function.
* The code is provided via print( code ) function  will be serialized to its buffer.
* When all serialization is finished, use the write() command to write the buffer to the file.

### Editor is for editing a series of files based on a set of requests provided to it

**Note: Not implemented yet**

* The purpose is to overrite a specific file, it places its contents in a buffer to scan.
* Requests are populated using the following interface:
  * add : Add code.
  * remove : Remove code.
  * replace: Replace code.

All three have the same parameters with exception to remove which only has SymbolInfo and Policy:

* SymbolInfo:
  * File      : The file the symbol resides in. Leave null to indicate to search all files. Leave null to indicated all-file search.
  * Marker    : #define symbol that indicates a location or following signature is valid to manipulate. Leave null to indicate the signature should only be used.
  * Signature : Use a Code symbol to find a valid location to manipulate, can be further filtered with the marker. Leave null to indicate the marker should only be used.

* Policy : Additional policy info for completing the request (empty for now)
* Code   : Code to inject if adding, or replace existing code with.

Additionally if `GEN_FEATURE_EDITOR_REFACTOR` is defined, refactor( file_path, specification_path ) wil be made available.  
Refactor is based of the refactor library and uses its interface.  
It will on call add a request to the queue to run the refactor script on the file.

### Scanner allows the user to generate Code ASTs by reading files

**Note: Not implemented yet**

* The purpose is to grab definitions to generate metadata or generate new code from these definitions.
* Requests are populated using the add( SymbolInfo, Policy ) function. The symbol info is the same as the one used for the editor. So is the case with Policy.

The file will only be read from, no writing supported.

One great use case is for example: generating the single-header library for gencpp!

### Additional Info (Editor and Scanner)

When all requests have been populated, call process_requests().  
It will provide an output of receipt data of the results when it completes.

Files may be added to the Editor and Scanner additionally with add_files( num, files ).  
This is intended for when you have requests that are for multiple files.

Request queue in both Editor and Scanner are cleared once process_requests completes.
