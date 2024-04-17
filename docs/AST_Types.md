# AST Types Documentation

While the Readme for docs covers the data layout per AST, this will focus on the AST types avaialble, and their nuances.

## Body

These are containers representing a scope body of a definition that can be of the following `ECode` type:

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
Code           Front;
Code           Back;
parser::Token* Tok;
Code           Parent;
StringCached   Name;
CodeT          Type;
s32            NumEntries;
```

The `Front` member represents the start of the link list and `Back` the end.
NumEntries is the number of entries in the body.

Parent should have a compatible ECode type for the type of defintion used.

Serialization:

Will output only the entries, the braces are handled by the parent.

```cpp
<Front>...
<Back>
```

## Attributes

Represent standard or vendor specific C/C++ attributes.

Fields:

```cpp
StringCached   Content;
Code           Prev;
Code           Next;
parser::Token* Tok;
Code           Parent;
StringCached   Name;
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
StringCached   Content;
Code           Prev;
Code           Next;
parser::Token* Tok;
Code           Parent;
StringCached   Name;
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
CodeType       Prev;       // Used to store references to interfaces
CodeType       Next;       // Used to store references to interfaces
parser::Token* Tok;
Code           Parent;
StringCached   Name;
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

You'll notice that only one parent type is supported only with parent access. This library only supports single inheritance, the rest must be done through interfaces.

## Constructor

Fields:

```cpp
CodeComment    InlineCmt;  // Only supported by forward declarations
Code           InitializerList;
CodeParam      Params;
Code           Body;
Code           Prev;
Code           Next;
parser::Token* Tok;
Code           Parent;
StringCached   Name;
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
StringCached   Content;
Code           Prev;
Code           Next;
parser::Token* Tok;
Code           Parent;
StringCached   Name;
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
Code           Prev;
Code           Next;
parser::Token* Tok;
Code           Parent;
StringCached   Name;
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
CodeBody       Body;
Code           Prev;
Code           Next;
parser::Token* Tok;
Code           Parent;
StringCached   Name;
CodeT          Type;
ModuleFlag     ModuleFlags;
```

Serialization:

```cpp
// Enum_Fwd
<ModuleFlags> enum class <Name> : <UnderlyingType>; <InlineCmt>

// Enum
<ModuleFlags> <enum or enum class> <Name> : <UnderlyingType>
{
    <Body>
};
```

## Execution

Just represents an execution body. Equivalent to an untyped body.
Will be obsolute when function body parsing is implemented.

Fields:

```cpp
StringCached   Content;
Code           Prev;
Code           Next;
parser::Token* Tok;
Code           Parent;
StringCached   Name;
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
Code           Prev;
Code           Next;
parser::Token* Tok;
Code           Parent;
StringCached   Name;
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
StringCached   Content;
Code           Prev;
Code           Next;
Code           Parent;
parser::Token* Tok;
StringCached   Name;
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
Code           Prev;
Code           Next;
parser::Token* Tok;
Code           Parent;
StringCached   Name;
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
CodeParam      Params;
CodeBody       Body;
Code           Prev;
Code           Next;
parser::Token* Tok;
Code           Parent;
StringCached   Name;
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
Code           Prev;
Code           Next;
parser::Token* Tok;
Code           Parent;
StringCached   Name;
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
Code           Prev;
Code           Next;
parser::Token* Tok;
Code           Parent;
StringCached   Name;
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

## Operator Overload

Fields:

```cpp
CodeComment    InlineCmt;
CodeAttributes Attributes;
CodeSpecifiers Specs;
CodeType       ReturnType;
CodeParam      Params;
CodeBody       Body;
Code           Prev;
Code           Next;
parser::Token* Tok;
Code           Parent;
StringCached   Name;
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

## Operator Cast Overload ( User-Defined Type Conversion )

Fields:

```cpp
CodeComment    InlineCmt;
CodeSpecifiers Specs;
CodeType       ValueType;
CodeBody       Body;
Code           Prev;
Code           Next;
parser::Token* Tok;
Code           Parent;
StringCached   Name;
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

## Parameters (AST_Param)

Fields:

```cpp
CodeType       ValueType;
Code           Macro;
Code           Value;
CodeParam      Last;
CodeParam      Next;
parser::Token* Tok;
Code           Parent;
StringCached   Name;
CodeT          Type;
s32            NumEntries;
```

Serialization:

```cpp
<Macro>, <Next> ... <Last>

<Macro> <ValueType> <Name>, <Next>... <Last>
```

## Pragma

Fields:

```cpp
StringCached   Content;
Code           Prev;
Code           Next;
parser::Token* Tok;
Code           Parent;
StringCached   Name;
CodeT          Type;
```

Serialization:

```cpp
#pragma <Content>
```

## Preprocessor Conditional

Fields:

```cpp
StringCached  Content;
Code          Prev;
Code          Next;
paser::Token* Tok;
Code          Parent;
StringCached  Name;
CodeT         Type;
```

Serialization:

```cpp
#<based off Type> <Content>
```

## Specifiers

Fields:

```cpp
SpecifierT     ArrSpecs[ AST::ArrSpecs_Cap ];
CodeSpecifiers NextSpecs;
Code           Prev;
Code           Next;
parser::Token* Tok;
Code           Parent;
StringCached   Name;
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
CodeParam      Params;
Code           Declaration;
Code           Prev;
Code           Next;
parser::Token* Tok;
Code           Parent;
StringCached   Name;
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
CodeParam      Params;
Code           ArrExpr;
Code           Prev;
Code           Next;
parser::Token* Tok;
Code           Parent;
StringCached   Name;
CodeT          Type;
b32            IsParamPack;
```

Serialization:

```cpp
<Attributes> <Name> <Specs> <IsParamPack ?: ...>
```

## Typedef

Behave as usual except function or macro typedefs.  
Those (macros) don't use the underlying type field as everything was serialized under the Name field.

Fields:

```cpp
CodeComment   InlineCmt;
Code          UnderlyingType;
Code          Prev;
Code          Next;
parse::Token* Tok
Code          Parent;
StringCached  Name;
CodeT         Type;
ModuleFlag    ModuleFlags;
b32           IsFunction;
```

Serialization:

```cpp
// Regular
<ModuleFlags> typedef <UnderlyingType> <Name>; <InlineCmt>

// Functions
<ModuleFlags> typedef <ReturnType> <Name>( <Parameters> ); <InlineCmt>
<ModuleFlags> typedef <ReturnType> ( <Expression that yeilds an Identifier signature> )( <Parameters> ); <InlineCmt>
```

## Union

Fields:

```cpp
CodeAttributes Attributes;
CodeBody       Body;
Code           Prev;
Code           Next;
parser::Token* Tok;
Code           Parent;
StringCached   Name;
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
Code           Prev;
Code           Next;
parser::Token* Tok;
Code           Parent;
StringCached   Name;
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
CodeVar        NextVar;
Code           Prev;
Code           Next;
parser::Token* Tok;
Code           Parent;
StringCached   Name;
CodeT          Type;
ModuleFlag     ModuleFlags;
```

Serialization:

```cpp
// Regular
<ModuleFlags> <Attributes> <Specs> <ValueType> <Name> = <Value>, NextVar ...; <InlineCmt>

// Bitfield
<ModuleFlags> <Attributes> <Specs> <ValueType> <Name> : <BitfieldSize> = <Value>, NextVar ...; <InlineCmt>
```
