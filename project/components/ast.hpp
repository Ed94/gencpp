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

namespace Parser
{
	struct Token;
}

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

#	define Using_Code( Typename )          \
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
	forceinline Type cast()
	{
		return * rcast( Type*, this );
	}

	AST* operator ->()
	{
		return ast;
	}
	Code& operator ++();
	
	
	// TODO(Ed) : Remove this overload.
//	auto& operator*()
//	{
//		return *this;
//	}

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
constexpr int const AST_POD_Size = 128;

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
	forceinline Type cast()
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
	int ArrSpecs_Cap =
	(
			AST_POD_Size
			- sizeof(AST*) * 3
			- sizeof(Parser::Token*)
			- sizeof(AST*)
			- sizeof(StringCached)
			- sizeof(CodeT)
			- sizeof(ModuleFlag)
			- sizeof(int)
	)
	/ sizeof(int) - 1; // -1 for 4 extra bytes

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
				AST*  SuffixSpecs;      // Only used with typenames, to store the function suffix if typename is function signature. ( May not be needed )
			};
		};
		StringCached  Content;          // Attributes, Comment, Execution, Include
		struct {
			SpecifierT ArrSpecs[ArrSpecs_Cap]; // Specifiers
			AST*       NextSpecs;              // Specifiers; If ArrSpecs is full, then NextSpecs is used.
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
	Parser::Token*    Token; // Reference to starting token, only avaialble if it was derived from parsing.
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
				AST*  SuffixSpecs;      // Only used with typenames, to store the function suffix if typename is function signature. ( May not be needed )
			};
		};
		StringCached  Content;          // Attributes, Comment, Execution, Include
		struct {
			SpecifierT ArrSpecs[AST::ArrSpecs_Cap]; // Specifiers
			AST*       NextSpecs;                   // Specifiers; If ArrSpecs is full, then NextSpecs is used.
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
	Parser::Token*    Token; // Reference to starting token, only avaialble if it was derived from parsing.
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
};

struct test {
	SpecifierT ArrSpecs[AST::ArrSpecs_Cap]; // Specifiers
	AST* NextSpecs;                         // Specifiers; If ArrSpecs is full, then NextSpecs is used.
};

constexpr int pls = sizeof(test);

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
	void to_string( String& result );
	void to_string_export( String& result );
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
	
	void to_string_def( String& result );
	void to_string_fwd( String& result );

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
	void to_string( String& result );
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
		// return { (AST_Param*) rcast( AST*, ast)->Last };
		return { nullptr };
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
	void to_string( String& result );
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
	
	void to_string_def( String& result );
	void to_string_fwd( String& result );

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

struct CodeConstructor
{
	Using_Code( CodeConstructor );
	
	void to_string_def( String& result );
	void to_string_fwd( String& result );
	
	AST*             raw();
	operator         Code();
	AST_Constructor* operator->();
	AST_Constructor* ast;
};

struct CodeDefine
{
	Using_Code( CodeDefine );
	
	void to_string( String& result );
	
	AST*        raw();
	operator    Code();
	AST_Define* operator->();
	AST_Define* ast;
};

struct CodeDestructor
{
	Using_Code( CodeDestructor );
	
	void to_string_def( String& result );
	void to_string_fwd( String& result );
	
	AST*             raw();
	operator         Code();
	AST_Destructor* operator->();
	AST_Destructor* ast;
};

struct CodeEnum
{
	Using_Code( CodeEnum );
	
	void to_string_def( String& result );
	void to_string_fwd( String& result );
	
	AST*      raw();
	operator  Code();
	AST_Enum* operator->();
	AST_Enum* ast;
};

Define_CodeType( Exec );

struct CodeExtern
{
	Using_Code( CodeExtern );
	
	void to_string( String& result );
	
	AST*        raw();
	operator    Code();
	AST_Extern* operator->();
	AST_Extern* ast;
};

struct CodeInclude
{
	Using_Code( CodeInclude );
	
	void to_string( String& result );
	
	AST*         raw();
	operator     Code();
	AST_Include* operator->();
	AST_Include* ast;
};

struct CodeFriend
{
	Using_Code( CodeFriend );
	
	void to_string( String& result );
	
	AST*        raw();
	operator    Code();
	AST_Friend* operator->();
	AST_Friend* ast;
};

struct CodeFn
{
	Using_Code( CodeFriend );
	
	void to_string_def( String& result );
	void to_string_fwd( String& result );
	
	AST*     raw();
	operator Code();
	AST_Fn*  operator->();
	AST_Fn*  ast;
};

struct CodeModule
{
	Using_Code( CodeModule );
	
	void to_string( String& result );
	
	AST*        raw();
	operator    Code();
	AST_Module* operator->();
	AST_Module* ast;
};

struct CodeNS
{
	Using_Code( CodeNS );
	
	void to_string( String& result );
	
	AST*     raw();
	operator Code();
	AST_NS*  operator->();
	AST_NS*  ast;
};

struct CodeOperator
{
	Using_Code( CodeOperator );
	
	void to_string_def( String& result );
	void to_string_fwd( String& result );
	
	AST*          raw();
	operator      Code();
	AST_Operator* operator->();
	AST_Operator* ast;
};

struct CodeOpCast
{
	Using_Code( CodeOpCast );
	
	void to_string_def( String& result );
	void to_string_fwd( String& result );
	
	AST*        raw();
	operator    Code();
	AST_OpCast* operator->();
	AST_OpCast* ast;
};

struct CodePragma
{
	Using_Code( CodePragma );
	
	void to_string( String& result );
	
	AST*        raw();
	operator    Code();
	AST_Pragma* operator->();
	AST_Pragma* ast;
};

struct CodePreprocessCond
{
	Using_Code( CodePreprocessCond );
	
	void to_string_if( String& result );
	void to_string_ifdef( String& result );
	void to_string_ifndef( String& result );
	void to_string_elif( String& result );
	void to_string_else( String& result );
	void to_string_endif( String& result );
	
	AST*                raw();
	operator            Code();
	AST_PreprocessCond* operator->();
	AST_PreprocessCond* ast;
};

struct CodeTemplate
{
	Using_Code( CodeTemplate );
	
	void to_string( String& result );
	
	AST*          raw();
	operator      Code();
	AST_Template* operator->();
	AST_Template* ast;
};

struct CodeType
{
	Using_Code( CodeType );
	
	void to_string( String& result );
	
	AST*      raw();
	operator  Code();
	AST_Type* operator->();
	AST_Type* ast;
};

struct CodeTypedef
{
	Using_Code( CodeTypedef );
	
	void to_string( String& result );
	
	AST*         raw();
	operator     Code();
	AST_Typedef* operator->();
	AST_Typedef* ast;
};

struct CodeUnion
{
	Using_Code( CodeUnion );
	
	void to_string( String& result );
	
	AST*         raw();
	operator     Code();
	AST_Union* operator->();
	AST_Union* ast;
};

struct CodeUsing
{
	Using_Code( CodeUsing );
	
	void to_string( String& result );
	void to_string_ns( String& result );
	
	AST*       raw();
	operator   Code();
	AST_Using* operator->();
	AST_Using* ast;
};

struct CodeVar
{
	Using_Code( CodeVar );
	
	void to_string( String& result );
	
	AST*     raw();
	operator Code();
	AST_Var* operator->();
	AST_Var* ast;
};

#undef Define_CodeType
#undef Using_Code

#pragma endregion Code Types
