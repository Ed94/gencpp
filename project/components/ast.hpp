#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "types.hpp"
#include "gen/ecode.hpp"
#include "gen/eoperator.hpp"
#include "gen/especifier.hpp"
#endif

struct AST;
struct AST_Body;
struct AST_Attributes;
struct AST_Comment;
struct AST_Constructor;
struct AST_Class;
struct AST_Define;
struct AST_Destructor;
struct AST_Enum;
struct AST_Exec;
struct AST_Extern;
struct AST_Include;
struct AST_Friend;
struct AST_Fn;
struct AST_Module;
struct AST_NS;
struct AST_Operator;
struct AST_OpCast;
struct AST_Param;
struct AST_Pragma;
struct AST_PreprocessCond;
struct AST_Specifiers;
struct AST_Struct;
struct AST_Template;
struct AST_Type;
struct AST_Typedef;
struct AST_Union;
struct AST_Using;
struct AST_Var;

struct Code;
struct CodeBody;
// These are to offer ease of use and optionally strong type safety for the AST.
struct CodeAttributes;
struct CodeComment;
struct CodeClass;
struct CodeConstructor;
struct CodeDefine;
struct CodeDestructor;
struct CodeEnum;
struct CodeExec;
struct CodeExtern;
struct CodeInclude;
struct CodeFriend;
struct CodeFn;
struct CodeModule;
struct CodeNS;
struct CodeOperator;
struct CodeOpCast;
struct CodeParam;
struct CodePreprocessCond;
struct CodePragma;
struct CodeSpecifiers;
struct CodeStruct;
struct CodeTemplate;
struct CodeType;
struct CodeTypedef;
struct CodeUnion;
struct CodeUsing;
struct CodeVar;

/*
	AST* wrapper
	- Not constantly have to append the '*' as this is written often..
	- Allows for implicit conversion to any of the ASTs (raw or filtered).
*/
struct Code
{
#	pragma region Statics
	// Used to identify ASTs that should always be duplicated. (Global constant ASTs)
	static Code Global;

	// Used to identify invalid generated code.
	static Code Invalid;
#	pragma endregion Statics

#	define Using_Code( Typename )         \
	char const* debug_str();               \
	Code        duplicate();			   \
	bool        is_equal( Code other );    \
	bool        is_valid();                \
	void        set_global();              \
	String      to_string();               \
	Typename&   operator = ( AST* other ); \
	Typename&   operator = ( Code other ); \
	bool        operator ==( Code other ); \
	bool        operator !=( Code other ); \
	operator bool();

	Using_Code( Code );

	template< class Type >
	Type cast()
	{
		return * rcast( Type*, this );
	}

	AST* operator ->()
	{
		return ast;
	}
	Code& operator ++();
	Code& operator*()
	{
		return *this;
	}

	AST* ast;

#ifdef GEN_ENFORCE_STRONG_CODE_TYPES
#	define operator explicit operator
#endif
	operator CodeAttributes() const;
	operator CodeComment() const;
	operator CodeConstructor() const;
	operator CodeDestructor() const;
	operator CodeClass() const;
	operator CodeDefine() const;
	operator CodeExec() const;
	operator CodeEnum() const;
	operator CodeExtern() const;
	operator CodeInclude() const;
	operator CodeFriend() const;
	operator CodeFn() const;
	operator CodeModule() const;
	operator CodeNS() const;
	operator CodeOperator() const;
	operator CodeOpCast() const;
	operator CodeParam() const;
	operator CodePragma() const;
	operator CodePreprocessCond() const;
	operator CodeSpecifiers() const;
	operator CodeStruct() const;
	operator CodeTemplate() const;
	operator CodeType() const;
	operator CodeTypedef() const;
	operator CodeUnion() const;
	operator CodeUsing() const;
	operator CodeVar() const;
	operator CodeBody() const;
	#undef operator
};

struct Code_POD
{
	AST* ast;
};

static_assert( sizeof(Code) == sizeof(Code_POD), "ERROR: Code is not POD" );

// Desired width of the AST data structure.
constexpr u32 AST_POD_Size = 128;

/*
	Simple AST POD with functionality to seralize into C++ syntax.
*/
struct AST
{
#	pragma region Member Functions
	void        append     ( AST* other );
	char const* debug_str  ();
	AST*        duplicate  ();
	Code&       entry      ( u32 idx );
	bool        has_entries();
	bool        is_equal   ( AST* other );
	char const* type_str();
	bool        validate_body();

	neverinline String to_string();

	template< class Type >
	Type cast()
	{
		return * this;
	}

	operator Code();
	operator CodeBody();
	operator CodeAttributes();
	operator CodeComment();
	operator CodeConstructor();
	operator CodeDestructor();
	operator CodeClass();
	operator CodeDefine();
	operator CodeEnum();
	operator CodeExec();
	operator CodeExtern();
	operator CodeInclude();
	operator CodeFriend();
	operator CodeFn();
	operator CodeModule();
	operator CodeNS();
	operator CodeOperator();
	operator CodeOpCast();
	operator CodeParam();
	operator CodePragma();
	operator CodePreprocessCond();
	operator CodeSpecifiers();
	operator CodeStruct();
	operator CodeTemplate();
	operator CodeType();
	operator CodeTypedef();
	operator CodeUnion();
	operator CodeUsing();
	operator CodeVar();
#	pragma endregion Member Functions

	constexpr static
	uw ArrSpecs_Cap =
	(
			AST_POD_Size
			- sizeof(AST*) * 4
			- sizeof(StringCached)
			- sizeof(CodeT)
			- sizeof(ModuleFlag)
			- sizeof(u32)
			- sizeof(s32)
	)
	/ sizeof(SpecifierT); // -1 for 4 extra bytes

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
};

struct AST_POD
{
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
};

// Its intended for the AST to have equivalent size to its POD.
// All extra functionality within the AST namespace should just be syntatic sugar.
static_assert( sizeof(AST)     == sizeof(AST_POD), "ERROR: AST IS NOT POD" );
static_assert( sizeof(AST_POD) == AST_POD_Size,    "ERROR: AST POD is not size of AST_POD_Size" );

// Used when the its desired when omission is allowed in a definition.
#define NoCode      { nullptr }
#define CodeInvalid (* Code::Invalid.ast) // Uses an implicitly overloaded cast from the AST to the desired code type.

#pragma region Code Types

struct CodeBody
{
	Using_Code( CodeBody );

	void append( Code other )
	{
		raw()->append( other.ast );
	}
	void append( CodeBody body )
	{
		for ( Code entry : body )
		{
			append( entry );
		}
	}
	bool has_entries()
	{
		return rcast( AST*, ast )->has_entries();
	}
	AST* raw()
	{
		return rcast( AST*, ast );
	}
	AST_Body* operator->()
	{
		return ast;
	}
	operator Code()
	{
		return * rcast( Code*, this );
	}
#pragma region Iterator
	Code begin()
	{
		if ( ast )
			return { rcast( AST*, ast)->Front };

		return { nullptr };
	}
	Code end()
	{
		return { rcast(AST*, ast)->Back->Next };
	}
#pragma endregion Iterator

	AST_Body* ast;
};

struct CodeClass
{
	Using_Code( CodeClass );

	void add_interface( CodeType interface );

	AST* raw()
	{
		return rcast( AST*, ast );
	}
	operator Code()
	{
		return * rcast( Code*, this );
	}
	AST_Class* operator->()
	{
		if ( ast == nullptr )
		{
			log_failure("Attempt to dereference a nullptr");
			return nullptr;
		}
		return ast;
	}
	AST_Class* ast;
};

struct CodeParam
{
	Using_Code( CodeParam );

	void append( CodeParam other );

	CodeParam get( s32 idx );
	bool has_entries();
	AST* raw()
	{
		return rcast( AST*, ast );
	}
	AST_Param* operator->()
	{
		if ( ast == nullptr )
		{
			log_failure("Attempt to dereference a nullptr!");
			return nullptr;
		}
		return ast;
	}
	operator Code()
	{
		return { (AST*)ast };
	}
#pragma region Iterator
	CodeParam begin()
	{
		if ( ast )
			return { ast };

		return { nullptr };
	}
	CodeParam end()
	{
		return { (AST_Param*) rcast( AST*, ast)->Last };
	}
	CodeParam& operator++();
	CodeParam operator*()
	{
		return * this;
	}
#pragma endregion Iterator

	AST_Param* ast;
};

struct CodeSpecifiers
{
	Using_Code( CodeSpecifiers );

	bool append( SpecifierT spec )
	{
		if ( ast == nullptr )
		{
			log_failure("CodeSpecifiers: Attempted to append to a null specifiers AST!");
			return false;
		}

		if ( raw()->NumEntries == AST::ArrSpecs_Cap )
		{
			log_failure("CodeSpecifiers: Attempted to append over %d specifiers to a specifiers AST!", AST::ArrSpecs_Cap );
			return false;
		}

		raw()->ArrSpecs[ raw()->NumEntries ] = spec;
		raw()->NumEntries++;
		return true;
	}
	s32 has( SpecifierT spec )
	{
		for ( s32 idx = 0; idx < raw()->NumEntries; idx++ )
		{
			if ( raw()->ArrSpecs[ raw()->NumEntries ] == spec )
				return idx;
		}

		return -1;
	}
	AST* raw()
	{
		return rcast( AST*, ast );
	}
	AST_Specifiers* operator->()
	{
		if ( ast == nullptr )
		{
			log_failure("Attempt to dereference a nullptr!");
			return nullptr;
		}
		return ast;
	}
	operator Code()
	{
		return { (AST*) ast };
	}
#pragma region Iterator
	SpecifierT* begin()
	{
		if ( ast )
			return & raw()->ArrSpecs[0];

		return nullptr;
	}
	SpecifierT* end()
	{
		return raw()->ArrSpecs + raw()->NumEntries;
	}
#pragma endregion Iterator

	AST_Specifiers* ast;
};

struct CodeStruct
{
	Using_Code( CodeStruct );

	void add_interface( CodeType interface );

	AST* raw()
	{
		return rcast( AST*, ast );
	}
	operator Code()
	{
		return * rcast( Code*, this );
	}
	AST_Struct* operator->()
	{
		if ( ast == nullptr )
		{
			log_failure("Attempt to dereference a nullptr");
			return nullptr;
		}
		return ast;
	}
	AST_Struct* ast;
};

#define Define_CodeType( Typename )     \
	struct Code##Typename               \
	{                                   \
		Using_Code( Code ## Typename ); \
		AST*            raw();          \
		operator        Code();         \
		AST_##Typename* operator->();   \
		AST_##Typename* ast;            \
	}

Define_CodeType( Attributes );
Define_CodeType( Comment );
Define_CodeType( Constructor );
Define_CodeType( Define );
Define_CodeType( Destructor );
Define_CodeType( Enum );
Define_CodeType( Exec );
Define_CodeType( Extern );
Define_CodeType( Include );
Define_CodeType( Friend );
Define_CodeType( Fn );
Define_CodeType( Module );
Define_CodeType( NS );
Define_CodeType( Operator );
Define_CodeType( OpCast );
Define_CodeType( Pragma );
Define_CodeType( PreprocessCond );
Define_CodeType( Template );
Define_CodeType( Type );
Define_CodeType( Typedef );
Define_CodeType( Union );
Define_CodeType( Using );
Define_CodeType( Var );

#undef Define_CodeType
#undef Using_Code

#pragma endregion Code Types
