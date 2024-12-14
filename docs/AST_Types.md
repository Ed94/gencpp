## Navigation

[Top](../Readme.md)

<- [docs - General](Readme.md)

# AST Types Documentation

While the Readme for docs covers the data layout per AST, this will focus on the AST types avaialble, and their nuances.

## Body

These are containers representing a scope body of a definition that can be of the following `CodeType` type:

* Class_Body
* Enum_Body
* Export_Body
* Extern_Linkage_Body
* Function_Body
* Global_Body
* Namespace_Body
* Struct_Body
* Union_Body

Fields:

```cpp
StrCached   Name;
Code           Front;
Code           Back;
Token*         Tok;
Code           Parent;
CodeT          Type;
s32            NumEntries;
```

The `Front` member represents the start of the link list and `Back` the end.
NumEntries is the number of entries in the body.

Parent should have a compatible CodeType type for the type of defintion used.

Serialization:

Will output only the entries, the braces are handled by the parent.

```cpp
<Front>
...
<Back>
```

## Attributes

Represent standard or vendor specific C/C++ attributes.

Fields:

```cpp
StrCached   Content;
StrCached   Name;
Code           Prev;
Code           Next;
Token*         Tok;
Code           Parent;
CodeT          Type;
```

Serialization:

```cpp
<Content>
```

While the parser supports the `__declspec` and `__attribute__` syntax, the upfront constructor ( def_attributes ) must have the user specify the entire attribute, including the `[[]]`, `__declspec` or `__attribute__` parts.

## Comment

Stores a comment.

Fields:

```cpp
StrCached   Content;
StrCached   Name;
Code           Prev;
Code           Next;
Token*         Tok;
Code           Parent;
CodeT          Type;
```

Serialization:

```cpp
<Content>
```

The parser will perserve comments found if residing with a body or in accepted inline-to-definition locations.
Otherwise they will be skipped by the TokArray::__eat and TokArray::current( skip foramtting enabled ) functions.

The upfront constructor: `def_comment` expects to recieve a comment without the `//` or `/* */` parts. It will add them during construction.

## Class & Struct

Fields:

```cpp
CodeComment    InlineCmt;  // Only supported by forward declarations
CodeAttributes Attributes;
CodeType       ParentType;
CodeBody       Body;
StrCached   Name;
CodeType       Prev;
CodeType       Next;
Token*         Tok;
Code           Parent;
CodeT          Type;
ModuleFlag     ModuleFlags;
AccessSpec     ParentAccess;
```

Serialization:

```cpp
// Class_Fwd
<ModuleFlags> <class/struct> <Name>; <InlineCmt>

// Class
<ModuleFlags> <class/struct> <Attributes> <Name> : <ParentAccess> <ParentType>, public <ParentType->Next>, ... <InlineCmt>
{
    <Body>
};
```

You'll notice that only one parent type is supported only with parent access. This library only supports single inheritance, the rest are assumed to be interfaces and are given public acess specifiers.

## Constructor

Fields:

```cpp
CodeComment    InlineCmt;  // Only supported by forward declarations
Code           InitializerList;
CodeParams     Params;
Code           Body;
StrCached   Name;
Code           Prev;
Code           Next;
Token*         Tok;
Code           Parent;
CodeT          Type;
```

Serialization:

```cpp
// Constructor_Fwd
<Specs> <Parent->Name>( <Params> ); <InlineCmt>

// Constructor
<Specs> <Parent->Name>( <Params> ) <InlineCmt>
    : <InitializerList>
{
    <Body>
}

// Constructor Source Implementation
<Specs> <Parent>::~<Parent->Name>( <Params> ) <Specs>
{
    <Body>
}
```

## Define

Represents a preprocessor define

Fields:

```cpp
StrCached   Content;
StrCached   Name;
Code           Prev;
Code           Next;
Token*         Tok;
Code           Parent;
CodeT          Type;
```

Serialization:

```cpp
#define <Name> <Content>
```

## Destructor

Fields:

```cpp
CodeComment    InlineCmt;
CodeSpecifiers Specs;
Code           Body;
StrCached   Name;
Code           Prev;
Code           Next;
Token*         Tok;
Code           Parent;
CodeT          Type;
```

Serialization:

```cpp
// Destructor_Fwd
<Specs> ~<Parent->Name>( <Params> ) <Specs>; <InlineCmt>

// Destructor
<Specs> ~<Parent->Name>( <Params> ) <Specs>
{
    <Body>
}

// Destructor Source Implementation
<Specs> <Parent>::~<Parent->Name>( <Params> ) <Specs>
{
    <Body>
}
```

## Enum

Fields:

```cpp
CodeComment    InlineCmt;
CodeAttributes Attributes;
CodeType       UnderlyingType;
Code           UnderlyingTypeMacro;
CodeBody       Body;
Code           Prev;
Code           Next;
Token*         Tok;
Code           Parent;
StrCached   Name;
CodeT          Type;
ModuleFlag     ModuleFlags;
```

UnderlyingTypeMacro is a macro the library natively supports: `enum_underlying(type)` that is meant to behave as a wrapper for underlying type assignment.  
The `enum_underlying_sig` is a `Str` global var that can be set which will be defined within `PreprocessorDefines` and used in `parser_parse_enum` to identify a valid macro.

Serialization:

```cpp
// Enum_Fwd
<ModuleFlags> enum class <Name> : <UnderlyingType> or <UnderlyingTypeMacro> ; <InlineCmt>

// Enum
<ModuleFlags> <enum or enum class> <Name> : <UnderlyingType> or <UnderlyingTypeMacro>
{
    <Body>
};
```

## Execution

Just represents an execution body. Equivalent to an untyped body.
Will be obsolute when function body parsing is implemented.

Fields:

```cpp
StrCached   Content;
StrCached   Name;
Code           Prev;
Code           Next;
Token*         Tok;
Code           Parent;
CodeT          Type;
```

Serialization:

```cpp
<Content>
```

## External Linkage

Fields:

```cpp
CodeBody       Body;
StrCached   Name;
Code           Prev;
Code           Next;
Token*         Tok;
Code           Parent;
CodeT          Type;
```

Serialization:

```cpp
extern "<Name>"
{
    <Body>
}
```

## Include

Fields:

```cpp
StrCached   Content;
StrCached   Name;
Code           Prev;
Code           Next;
Code           Parent;
Token*         Tok;
CodeT          Type;
```

Serialization:

```cpp
#include <Content>
```

## Friend

This library (until its necessary become some third-party library to do otherwise) does not support friend declarations with in-statment function definitions.

Fields:

```cpp
CodeComment    InlineCmt;
Code           Declaration;
StrCached   Name;
Code           Prev;
Code           Next;
Token*         Tok;
Code           Parent;
CodeT          Type;
```

Serialization:

```cpp
friend <Declaration>; <InlineCmt>
```

## Function

Fields:

```cpp
CodeComment    InlineCmt;
CodeAttributes Attributes;
CodeSpecifiers Specs;
CodeType       ReturnType;
CodeParams     Params;
CodeBody       Body;
StrCached   Name;
Code           Prev;
Code           Next;
Token*         Tok;
Code           Parent;
CodeT          Type;
ModuleFlag     ModuleFlags;
```

Serialization:

```cpp
// Function_Fwd
<ModuleFlags> <Attributes> <Specs> <ReturnType> <Name>( <Params> ) <Specs>; <InlineCmt>

// Function
<ModuleFlags> <Attributes> <Specs> <ReturnType> <Name>( <Params> ) <Specs>
{
    <Body>
}
```

## Module

Fields:

```cpp
StrCached   Name;
Code           Prev;
Code           Next;
Token*         Tok;
Code           Parent;
CodeT          Type;
ModuleFlag     ModuleFlags;
```

Serialization:

```cpp
<ModuleFlags> module <Name>;
```

## Namespace

Fields:

```cpp
CodeBody       Body;
StrCached   Name;
Code           Prev;
Code           Next;
Token*         Tok;
Code           Parent;
CodeT          Type;
ModuleFlag     ModuleFlags;
```

Serialization:

```cpp
<ModuleFlags> namespace <Name>
{
    <Body>
}
```

## Operator Overload (Operator)

Fields:

```cpp
CodeComment    InlineCmt;
CodeAttributes Attributes;
CodeSpecifiers Specs;
CodeType       ReturnType;
CodeParams     Params;
CodeBody       Body;
StrCached   Name;
Code           Prev;
Code           Next;
Token*         Tok;
Code           Parent;
CodeT          Type;
ModuleFlag     ModuleFlags;
OperatorT      Op;
```

Serialization:

```cpp
// Operator_Fwd
<ModuleFlags> <Attributes> <Specs> <ReturnType> operator <Op>( <Params> ) <Specs>; <InlineCmt>

// Operator
<ModuleFlags> <Attributes> <Specs> <ReturnType> <Name>operator <Op>( <Params> ) <Specs>
{
    <Body>
}
```

## Operator Cast Overload ( User-Defined Type Conversion, OpCast )

Fields:

```cpp
CodeComment    InlineCmt;
CodeSpecifiers Specs;
CodeType       ValueType;
CodeBody       Body;
StrCached   Name;
Code           Prev;
Code           Next;
Token*         Tok;
Code           Parent;
CodeT          Type;
```

Serialization:

```cpp
// Operator_Cast_Fwd
<Specs> operator <ValueType>() <Specs>; <InlineCmt>

// Operator_Cast
<Specs> <Name>operator <ValueType>() <Specs>
{
    <Body>
}
```

## Parameters (AST_Params)

Fields:

```cpp
CodeType       ValueType;
Code           Macro;
Code           Value;
Code           PostNameMacro;
StrCached   Name;
CodeParams     Last;
CodeParams     Next;
Token*         Tok;
Code           Parent;
CodeT          Type;
s32            NumEntries;
```

Serialization:

```cpp
<Macro>, <Next> ... <Last>

<Macro> <ValueType> <Name> <PostNameMacro> = <Value>, <Next>... <Last>
```

## Pragma

Fields:

```cpp
StrCached   Content;
StrCached   Name;
Code           Prev;
Code           Next;
Token*         Tok;
Code           Parent;
CodeT          Type;
```

Serialization:

```cpp
#pragma <Content>
```

## Preprocessor Conditional

Fields:

```cpp
StrCached  Content;
StrCached  Name;
Code          Prev;
Code          Next;
Token*        Tok;
Code          Parent;
CodeT         Type;
```

Serialization:

```cpp
#<based off Type> <Content>
```

## Specifiers

Fields:

```cpp
SpecifierT     ArrSpecs[ AST_ArrSpecs_Cap ];
CodeSpecifiers NextSpecs;
StrCached   Name;
Code           Prev;
Code           Next;
Token*         Tok;
Code           Parent;
CodeT          Type;
s32            NumEntries;
```

Serialization:

```cpp
<Spec>, ...
```

## Template

Fields:

```cpp
CodeParams     Params;
Code           Declaration;
StrCached   Name;
Code           Prev;
Code           Next;
Token*         Tok;
Code           Parent;
CodeT          Type;
ModuleFlag     ModuleFlags;
```

Serialization:

```cpp
<ModuleFlags>
template< <Params> >
<Declaration>
```

## Typename

Typenames represent the type "symbol".

Fields:

```cpp
CodeAttributes Attributes;
CodeSpecifiers Specs;
CodeReturnType ReturnType;
CodeParams     Params;
Code           ArrExpr;
Code           Prev;
Code           Next;
Token*         Tok;
Code           Parent;
StrCached   Name;
CodeT          Type;
b32            IsParamPack;
ETypenameTag   TypeTag;
```

Serialization:

```cpp
<Attributes> <TypeTag> <Name> <Specs> <IsParamPack ?: ...> 
// Function
<Attributes> <ReturnType> <Name> <Params> <Specs>
```

`<Name>` currently has the full serialization of anything with

*Note: ArrExpr is not used in serialization by `typename_to_strbuilder_ref` its instead handled by a parent AST's serailization (variable, typedef, using).*

## Typedef

Behave as usual except function or macro typedefs.  
Those (macros) don't use the underlying type field as everything was serialized under the Name field.

Fields:

```cpp
CodeComment   InlineCmt;
Code          UnderlyingType;
StrCached  Name;
Code          Prev;
Code          Next;
Token*        Tok
Code          Parent;
CodeT         Type;
ModuleFlag    ModuleFlags;
b32           IsFunction;
```

Serialization:

```cpp
// Regular
<ModuleFlags> typedef <UnderlyingType> <Name> <UnderlyingType-ArrExpr>; <InlineCmt>

// Functions

// Currently:
<ModuleFlags> typedef <UnderlyingType (Serialized expression)>; <InlineCmt>

// Desired: Not handled yet
<ModuleFlags> typedef <UnderlyingType->ReturnType> UnderlyingType->Name> <UnderlyingType-ArrExpr> ( <UnderlyingType->Parameters> ); <InlineCmt>
<ModuleFlags> typedef <UnderlyingType->ReturnType> ( <Name->Namespace> for<Specs->has(Spec_Ptr) ?: *> <UnderlyingType->Name> <UnderlyingType-ArrExpr> ) ( <UnderlyingType->Parameters> ); <InlineCmt>
```

## Union

Fields:

```cpp
CodeAttributes Attributes;
CodeBody       Body;
StrCached   Name;
Code           Prev;
Code           Next;
Token*         Tok;
Code           Parent;
CodeT          Type;
ModuleFlag     ModuleFlags;
```

Serialization:

```cpp
<ModuleFlags> union <Attributes> <Name>
{
    <Body>
}
```

## Using

Fields:

```cpp
CodeComment    InlineCmt;
CodeAttributes Attributes;
CodeType       UnderlyingType;
StrCached   Name;
Code           Prev;
Code           Next;
Token*         Tok;
Code           Parent;
CodeT          Type;
ModuleFlag     ModuleFlags;
```

Serialization:

```cpp
// Regular
<ModuleFlags> using <Attributes> <Name> = <UnderlyingType>; <InlineCmt>

// Namespace
<ModuleFlags> using namespace <Name>; <InlineCmt>
```

## Variable

[Algo](./Parser_Algo.md:)

Fields:

```cpp
CodeComment    InlineCmt;
CodeAttributes Attributes;
CodeSpecifiers Specs;
CodeType       ValueType;
Code           BitfieldSize;
Code           Value;
StrCached   Name;
CodeVar        NextVar;
Code           Prev;
Code           Next;
Token*         Tok;
Code           Parent;
CodeT          Type;
ModuleFlag     ModuleFlags;
s32            VarParenthesizedInit;
```

Serialization:

```cpp
// Regular
<ModuleFlags> <Attributes> <Specs> <ValueType> <Name> = <Value>, NextVar ...; <InlineCmt>

// Bitfield
<ModuleFlags> <Attributes> <Specs> <ValueType> <Name> : <BitfieldSize> = <Value>, NextVar ...; <InlineCmt>

// VarParenthesizedInit
<Attributes> <Specs> <ValueType> <Name>( <Value>, NextVar ... ); <InlineCmt>
```
