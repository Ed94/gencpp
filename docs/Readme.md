# General Docs

[Top](../Readme.md)

Contains:

* [AST_Design](./AST_Design.md): Overview of ASTs
* [AST Types](./AST_Types.md): Listing of all AST types along with their Code type interface.
* [Parsing](./Parsing.md): Overview of the parsing interface.
* [Parser Algo](./Parser_Algo.md): In-depth breakdown of the parser's implementation.

### *CURRENTLY UNSUPPORTED*

**There is no support for validating expressions.**  
Its a [todo](https://github.com/Ed94/gencpp/issues/49)

**Only trivial template support is provided.**
The intention is for only simple, non-recursive substitution.
The parameters of the template are treated like regular parameter AST entries.
This means that the typename entry for the parameter AST would be either:

* `class`
* `typename`
* A fundamental type, function, or pointer type.

***Concepts and Constraints are not supported***  
Its a [todo](https://github.com/Ed94/gencpp/issues/21)

### Feature Macros:

* `GEN_DEFINE_ATTRIBUTE_TOKENS` : Allows user to define their own attribute macros for use in parsing.
  * This can be generated using base.cpp.
* `GEN_DEFINE_LIBRARY_CORE_CONSTANTS` : Optional typename codes as they are non-standard to C/C++ and not necessary to library usage
* `GEN_DONT_ENFORCE_GEN_TIME_GUARD` : By default, the library ( gen.hpp/ gen.cpp ) expects the macro `GEN_TIME` to be defined, this disables that.
* `GEN_ENFORCE_STRONG_CODE_TYPES` : Enforces casts to filtered code types.
* `GEN_EXPOSE_BACKEND` : Will expose symbols meant for internal use only.
* `GEN_ROLL_OWN_DEPENDENCIES` : Optional override so that user may define the dependencies themselves.
* `GEN_DONT_ALLOW_INVALID_CODE` (Not implemented yet) : Will fail when an invalid code is constructed, parsed, or serialized.
* `GEN_C_LIKE_PP` : Setting to `<true or 1>` Will prevent usage of function defnitions using references and structs with member functions. Structs will still have user-defined operator conversions, for-range support, and other operator overloads

### The Data & Interface

As mentioned in root readme, the user is provided Code objects by calling the constructor's functions to generate them or find existing matches.

The AST is managed by the library and provided to the user via its interface.  
However, the user may specifiy memory configuration.

[Data layout of AST struct (Subject to heavily change with upcoming todos)](../base/components/ast.hpp#L396-461)  

https://github.com/Ed94/gencpp/blob/eea4ebf5c40d5d87baa465abfb1be30845b2377e/base/components/ast.hpp#L396-L461

*`StringCahced` is a typedef for `Str` (a string slice), to denote it is an interned string*  
*`CodeType` is enum taggin the type of code. Has an underlying type of `u32`*  
*`OperatorT` is a typedef for `EOperator::Type` which has an underlying type of `u32`*  
*`StrBuilder` is the dynamically allocated string type for the library*  

AST widths are setup to be AST_POD_Size.  
The width dictates how much the static array can hold before it must give way to using an allocated array:

```cpp
constexpr static
int AST_ArrSpecs_Cap =
(
    AST_POD_Size
    - sizeof(Code)
    - sizeof(StrCached)
    - sizeof(Code) * 2
    - sizeof(Token*)
    - sizeof(Code)
    - sizeof(CodeType)
    - sizeof(ModuleFlag)
    - sizeof(u32)
)
/ sizeof(Specifier) - 1;
```

*Ex: If the AST_POD_Size is 128 the capacity of the static array is 20.*

Data Notes:

* The allocator definitions used are exposed to the user incase they want to dictate memory usage
  * You'll find the memory handling in `init`, `deinit`, `reset`, `gen_strbuilder_allocator`, `cache_str`, `make_code`.
  * Allocators are defined with the `AllocatorInfo` structure found in [`memory.hpp`](../base/dependencies/memory.hpp)
  * Most of the work is just defining the allocation procedure:

```cpp
    void* ( void* allocator_data, AllocType type, ssize size, ssize alignment, void* old_memory, ssize old_size, u64 flags );
```

* ASTs are wrapped for the user in a Code struct which is a wrapper for a AST* type.
* Code types have member symbols but their data layout is enforced to be POD types.
* This library treats memory failures as fatal.
* Cached Strings are stored in their own set of arenas. AST constructors use cached strings for names, and content.
  * `StringArenas`, `StringCache`, `Allocator_StringArena`, and `Allocator_StringTable` are the associated containers or allocators.
* Strings used for serialization and file buffers are not contained by those used for cached strings.
  * They are currently using `FallbackAllocator`, which are tracked array of arenas that grows as needed (adds buckets when one runs out).
  * Memory within the buckets is not reused, so its inherently wasteful.
  * I will be augmenting the default allocator with virtual memory & a slab allocator in the [future](https://github.com/Ed94/gencpp/issues/12)
* Intrusive linked lists used children nodes on bodies, and parameters.
* Its intended to generate the AST in one go and serialize after. The constructors and serializer are designed to be a "one pass, front to back" setup.
* Allocations can be tuned by defining the folloiwng macros (will be moved to runtime configuration in the future):
  * `GEN_GLOBAL_BUCKET_SIZE` : Size of each bucket area for the global allocator
  * `GEN_CODEPOOL_NUM_BLOCKS` : Number of blocks per code pool in the code allocator
  * `GEN_SIZE_PER_STRING_ARENA` : Size per arena used with string caching.
  * `GEN_MAX_COMMENT_LINE_LENGTH` : Longest length a comment can have per line.
  * `GEN_MAX_NAME_LENGTH` : Max length of any identifier.
  * `GEN_MAX_UNTYPED_STR_LENGTH` : Max content length for any untyped code.
  * `TokenMap_FixedArena` : token_fmt_va uses local_persit memory of this arena type for the hashtable.
  * `GEN_LEX_ALLOCATOR_SIZE`
  * `GEN_BUILDER_STR_BUFFER_RESERVE`

The following CodeTypes are used which the user may optionally use strong typing with if they enable: `GEN_ENFORCE_STRONG_CODE_TYPES`

* CodeBody : Has support for `for : range` iterating across Code objects.
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
* CodeParams : Has support for `for : range` iterating across parameters.
* CodePreprocessCond
* CodePragma
* CodeSpecifiers : Has support for `for : range` iterating across specifiers.
* CodeStruct
* CodeTemplate
* CodeTypename
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
* def_pragma
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
Str value = txt("Something");

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
* `spec_forceinline`
* `spec_global` (global macro)
* `spec_inline`
* `spec_internal_linkage` (internal macro)
* `spec_local_persist` (local_persist macro)
* `spec_mutable`
* `spec_neverinline`
* `spec_noexcept`
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
* `t_ssize` (ssize_t)
* `t_usize` (size_t)
* `t_f32`
* `t_f64`

## Extent of operator overload validation

The AST and constructors will be able to validate that the arguments provided for the operator type match the expected form:

* If return type must match a parameter
* If number of parameters is correct
* If added as a member symbol to a class or struct, that operator matches the requirements for the class (types match up)
* There is no support for validating new & delete operations (yet)

The user is responsible for making sure the code types provided are correct
and have the desired specifiers assigned to them beforehand.

## Code generation and modification

There are two provided auxillary interfaces:

* Builder
* Scanner

### Builder is a similar object to the jai language's strbuilder_builder

* The purpose of it is to generate a file.
* A file is specified and opened for writing using the open( file_path) function.
* The code is provided via print( code ) function  will be serialized to its buffer.
* When all serialization is finished, use the write() command to write the buffer to the file.

### Scanner Auxillary Interface

* The purpose is to scan or parse files
* Some with two basic functions to convert a fil to code: `scan_file` and `parse_file`
  * `scan_file`: Merely grabs the file and stores it in an untyped Code.
  * `parse_file`: Will parse the file using `parse_global_body` and return a `CodeBody`.
* Two basic functions for grabbing columns from a CSV: `parse_csv_one_column` and `parse_csv_two_columns`
