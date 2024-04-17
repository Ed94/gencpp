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

Both Code and AST definitions have a `template< class Type> Code/AST :: cast()`. Its just an alternative way to explicitly cast to each other.

`template< class Type> swap( Type& a, Type& b)` is used over a macro.

Otherwise the library is free of any templates.

### *WHAT IS NOT PROVIDED*

**There is no support for validating expressions.**  
Its difficult to parse without enough benefits (At the metaprogramming level).  
I plan to add this only at the tail of the project parsing milestone.

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

The AST is managed by the library and provided to the user via its interface.  
However, the user may specifiy memory configuration.

Data layout of AST struct (Subject to heavily change with upcoming redesign):

```cpp
union {
    struct
    {
        AST*      InlineCmt;       // Class, Constructor, Destructor, Enum, Friend, Functon, Operator, OpCast, Struct, Typedef, Using, Variable
        AST*      Attributes;      // Class, Enum, Function, Struct, Typedef, Union, Using, Variable
        AST*      Specs;           // Destructor, Function, Operator, Typename, Variable
        union {
            AST*  InitializerList; // Constructor
            AST*  ParentType;      // Class, Struct, ParentType->Next has a possible list of interfaces.
            AST*  ReturnType;      // Function, Operator, Typename
            AST*  UnderlyingType;  // Enum, Typedef
            AST*  ValueType;       // Parameter, Variable
        };
        union {
            AST*  Macro;           // Parameters
            AST*  BitfieldSize;    // Variable (Class/Struct Data Member)
            AST*  Params;          // Constructor, Function, Operator, Template, Typename
        };
        union {
            AST*  ArrExpr;          // Typename
            AST*  Body;             // Class, Constructr, Destructor, Enum, Function, Namespace, Struct, Union
            AST*  Declaration;      // Friend, Template
            AST*  Value;            // Parameter, Variable
        };
        union {
            AST*  NextVar;          // Variable; Possible way to handle comma separated variables declarations. ( , NextVar->Specs NextVar->Name NextVar->ArrExpr = NextVar->Value )
            AST*  SpecsFuncSuffix;  // Only used with typenames, to store the function suffix if typename is function signature.
        };
    };
    StringCached  Content;          // Attributes, Comment, Execution, Include
    struct {
        SpecifierT ArrSpecs[AST::ArrSpecs_Cap]; // Specifiers
        AST*       NextSpecs;                   // Specifiers
    };
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
    b32           IsFunction;  // Used by typedef to not serialize the name field.
    b32           IsParamPack; // Used by typename to know if type should be considered a parameter pack.
    OperatorT     Op;
    AccessSpec    ParentAccess;
    s32           NumEntries;
};
s32               Token;       // Handle to the token, stored in the CodeFile (Otherwise unretrivable)
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
  * Allocators are defined with the `AllocatorInfo` structure found in `dependencies\memory.hpp`
  * Most of the work is just defining the allocation procedure:

```cpp
    void* ( void* allocator_data, AllocType type, sw size, sw alignment, void* old_memory, sw old_size, u64 flags );
```

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
StrC value = txt("Something");

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
* `param_varaidc` (Used for varadic definitions)
* `pragma_once`
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
* `t_sw` (ssize_t)
* `t_uw` (size_t)
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

There are three provided auxillary interfaces:

* Builder
* Editor
* Scanner

Editor and Scanner are disabled by default, use `GEN_FEATURE_EDITOR` and `GEN_FEATURE_SCANNER` to enable them.

### Builder is a similar object to the jai language's string_builder

* The purpose of it is to generate a file.
* A file is specified and opened for writing using the open( file_path) function.
* The code is provided via print( code ) function  will be serialized to its buffer.
* When all serialization is finished, use the write() command to write the buffer to the file.

### Scanner Auxillary Interface

Provides *(eventually)* `scan_file` to automatically populate a CodeFile which contains a parsed AST (`Code`) of the file, with any contextual failures that are reported from the parser.
