/*
	gencpp: An attempt at "simple" staged metaprogramming for c/c++.

	See Readme.md for more information from the project repository.

	Public Address:
	https://github.com/Ed94/gencpp
*/
#pragma once

#if ! defined(GEN_DONT_ENFORCE_GEN_TIME_GUARD) && ! defined(GEN_TIME)
#error Gen.hpp : GEN_TIME not defined
#endif

#include "gen.push_ignores.inline.hpp"

//! If its desired to roll your own dependencies, define GEN_ROLL_OWN_DEPENDENCIES before including this file.
// Dependencies are derived from the c-zpl library: https://github.com/zpl-c/zpl
#ifndef GEN_ROLL_OWN_DEPENDENCIES
#	include "gen.dep.hpp"
#endif

namespace gen {

#pragma region Types
using LogFailType = sw(*)(char const*, ...);

// By default this library will either crash or exit if an error is detected while generating codes.
// Even if set to not use fatal, fatal will still be used for memory failures as the library is unusable when they occur.
#ifdef GEN_DONT_USE_FATAL
	constexpr LogFailType log_failure = log_fmt;
#else
	constexpr LogFailType log_failure = fatal;
#endif

namespace ECode
{
#	define Define_Types           \
	Entry( Untyped )              \
	Entry( Comment )              \
	Entry( Access_Private )       \
	Entry( Access_Protected )     \
	Entry( Access_Public )        \
	Entry( PlatformAttributes )   \
	Entry( Class )                \
	Entry( Class_Fwd )            \
	Entry( Class_Body )           \
	Entry( Enum )                 \
	Entry( Enum_Fwd )             \
	Entry( Enum_Body )            \
	Entry( Enum_Class )           \
	Entry( Enum_Class_Fwd )       \
	Entry( Execution )            \
	Entry( Export_Body )          \
	Entry( Extern_Linkage )       \
	Entry( Extern_Linkage_Body )  \
	Entry( Friend )               \
	Entry( Function )             \
	Entry( Function_Fwd )         \
	Entry( Function_Body )        \
	Entry( Global_Body )          \
	Entry( Module )               \
	Entry( Namespace )            \
	Entry( Namespace_Body )       \
	Entry( Operator )             \
	Entry( Operator_Fwd )         \
	Entry( Operator_Member )      \
	Entry( Operator_Member_Fwd )  \
	Entry( Operator_Cast )		  \
	Entry( Operator_Cast_Fwd )    \
	Entry( Parameters )           \
	Entry( Preprocessor_Include ) \
	Entry( Specifiers )           \
	Entry( Struct )               \
	Entry( Struct_Fwd )           \
	Entry( Struct_Body )          \
	Entry( Template )             \
	Entry( Typedef )              \
	Entry( Typename )             \
	Entry( Union )			      \
	Entry( Union_Body) 		      \
	Entry( Using )                \
	Entry( Using_Namespace )      \
	Entry( Variable )

	enum Type : u32
	{
	#	define Entry( Type ) Type,
		Define_Types
	#	undef Entry

		Num_Types,
		Invalid
	};

	inline
	StrC to_str( Type type )
	{
		static
		StrC lookup[Num_Types] = {
		#	define Entry( Type ) { sizeof(stringize(Type)), stringize(Type) },
			Define_Types
		#	undef Entry
		};

		return lookup[ type ];
	}

#	undef Define_Types
}
using CodeT = ECode::Type;

// Used to indicate if enum definitoin is an enum class or regular enum.
enum class EnumT : u8
{
	Regular,
	Class
};

constexpr EnumT EnumClass   = EnumT::Class;
constexpr EnumT EnumRegular = EnumT::Regular;

namespace EOperator
{
#	define Define_Operators       \
	Entry( Assign,          =  )  \
	Entry( Assign_Add,      += )  \
	Entry( Assign_Subtract, -= )  \
	Entry( Assign_Multiply, *= )  \
	Entry( Assign_Divide,   /= )  \
	Entry( Assign_Modulo,   %= )  \
	Entry( Assign_BAnd,     &= )  \
	Entry( Assign_BOr,      |= )  \
	Entry( Assign_BXOr,     ^= )  \
	Entry( Assign_LShift,   <<= ) \
	Entry( Assign_RShift,   >>= ) \
	Entry( Increment,       ++ )  \
	Entry( Decrement,       -- )  \
	Entry( Unary_Plus,      + )   \
	Entry( Unary_Minus,     - )   \
	Entry( UnaryNot,        ! )   \
	Entry( Add,             + )   \
	Entry( Subtract,        - )   \
	Entry( Multiply,        * )   \
	Entry( Divide,          / )   \
	Entry( Modulo,          % )   \
	Entry( BNot,            ~ )   \
	Entry( BAnd,            & )   \
	Entry( BOr,             | )   \
	Entry( BXOr,            ^ )   \
	Entry( LShift,          << )  \
	Entry( RShift,          >> )  \
	Entry( LAnd,            && )  \
	Entry( LOr,             || )  \
	Entry( LEqual,          == )  \
	Entry( LNot,            != )  \
	Entry( Lesser,          < )   \
	Entry( Greater,         > )   \
	Entry( LesserEqual,     <= )  \
	Entry( GreaterEqual,    >= )  \
	Entry( Subscript,       [] )  \
	Entry( Indirection,     * )   \
	Entry( AddressOf,       & )   \
	Entry( MemberOfPointer, -> )  \
	Entry( PtrToMemOfPtr,   ->* ) \
	Entry( FunctionCall,    () )

	enum Type : u32
	{
	#	define Entry( Type_, Token_ ) Type_,
		Define_Operators
	#	undef Entry
		Comma,

		Num_Ops,
		Invalid
	};

	inline
	char const* to_str( Type op )
	{
		local_persist
		char const* lookup[ Num_Ops ] = {
		#	define Entry( Type_, Token_ ) stringize(Token_),
			Define_Operators
		#	undef Entry
			","
		};

		return lookup[ op ];
	}

#	undef Define_Operators
}
using OperatorT = EOperator::Type;

namespace ESpecifier
{
/*
	Note: The following are handled separately:
	attributes
	alignas
*/

#	define Define_Specifiers                     \
	Entry( Invalid,          INVALID )           \
	Entry( Const,            const )             \
	Entry( Consteval,        consteval )         \
	Entry( Constexpr,        constexpr )         \
	Entry( Constinit,        constinit )         \
	Entry( External_Linkage, extern )            \
	Entry( Global,           global )            \
	Entry( Inline,           inline )            \
	Entry( Internal_Linkage, internal )          \
	Entry( Local_Persist,    local_persist )     \
	Entry( Mutable,          mutable )           \
	Entry( Ptr,              * )                 \
	Entry( Ref,              & )                 \
	Entry( Register,         register )          \
	Entry( RValue,           && )                \
	Entry( Static,           static  )           \
	Entry( Thread_Local,     thread_local )      \
	Entry( Volatile,         volatile )

	enum Type : u32
	{
	#	define Entry( Specifier, Code ) Specifier,
		Define_Specifiers
	#	undef Entry

		Num_Specifiers,
	};

	// Specifier to string
	inline
	StrC to_str( Type specifier )
	{
		local_persist
		StrC lookup[ Num_Specifiers ] = {
		#	pragma push_macro( "global" )
		#	pragma push_macro( "internal" )
		#	pragma push_macro( "local_persist" )
		#	undef global
		#	undef internal
		#	undef local_persist

		#	define Entry( Spec_, Code_ ) { sizeof(stringize(Code_)), stringize(Code_) },
			Define_Specifiers
		#	undef Entry

		#	pragma pop_macro( "global" )
		#	pragma pop_macro( "internal" )
		#	pragma pop_macro( "local_persist" )
		};

		return lookup[ specifier ];
	}

	inline
	Type to_type( StrC str )
	{
		local_persist
		u32 keymap[ Num_Specifiers ];
		do_once_start
			for ( u32 index = 0; index < Num_Specifiers; index++ )
			{
				StrC enum_str = to_str( (Type)index );

				// We subtract 1 to remove the null terminator
				// This is because the tokens lexed are not null terminated.
				keymap[index] = crc32( enum_str.Ptr, enum_str.Len - 1);
			}
		do_once_end

		u32 hash = crc32( str.Ptr, str.Len );

		for ( u32 index = 0; index < Num_Specifiers; index++ )
		{
			if ( keymap[index] == hash )
				return (Type)index;
		}

		return Invalid;
	}

#	undef Define_Specifiers
}
using SpecifierT = ESpecifier::Type;

enum class AccessSpec : u32
{
	Default,
	Public,
	Protected,
	Private,

	Num_AccessSpec,
	Invalid,
};

inline
char const* to_str( AccessSpec type )
{
	local_persist
	char const* lookup[ (u32)AccessSpec::Num_AccessSpec ] = {
		"",
		"public",
		"protected",
		"private",
	};

	if ( type > AccessSpec::Public )
		return "Invalid";

	return lookup[ (u32)type ];
}

enum class ModuleFlag : u32
{
	None    = 0,
	Export  = bit(0),
	Import  = bit(1),
	// Private = bit(2),

	Num_ModuleFlags,
	Invalid,
};

ModuleFlag operator|( ModuleFlag A, ModuleFlag B)
{
	return (ModuleFlag)( (u32)A | (u32)B );
}

/*
	Predefined attributes

	Used for the parser constructors to identify non-standard attributes
*/
namespace Attribute
{
#if defined(GEN_SYSTEM_WINDOWS) || defined( __CYGWIN__ )
#	define GEN_API_
#	define GEN_API_Export_Code   __declspec(dllexport)
#	define GEN_API_Import_Code   __declspec(dllimport)
#	define GEN_Attribute_Keyword __declspec

	constexpr char const* API_Export = stringize( GEN_API_Export_Code  );
	constexpr char const* API_Import = stringize( GEN_API_Import_Code  );
	constexpr char const* Keyword    = stringize( GEN_Attribute_Keyword);

#elif GEN_HAS_ATTRIBUTE( visibility ) || GEN_GCC_VERSION_CHECK( 3, 3, 0 )
#	define GEN_API_Export_Code   __attribute__ ((visibility ("default")))
#	define GEN_API_Import_Code   __attribute__ ((visibility ("default")))
#	define GEN_Attribute_Keyword __attribute__

	constexpr char const* API_Export = stringize( GEN_API_Export_Code );
	constexpr char const* API_Import = stringize( GEN_API_Import_Code );
	constexpr char const* Keyword    = stringize( GEN_Attribute_Keyword);

#else
#	define GEN_API_Export_Code
#	define GEN_API_Import_Code
#	define GEN_Attribute_Keyword

	constexpr char const* API_Export = "";
	constexpr char const* API_Import = "";
	constexpr char const* Keyword    = "";
#endif
}
#pragma endregion Types

#pragma region Data Structures
// Implements basic string interning. Data structure is based off the ZPL Hashtable.
using StringTable = HashTable<String const>;

// Represents strings cached with the string table.
// Should never be modified, if changed string is desired, cache_string( str ) another.
using StringCached = String const;

struct AST;
struct AST_Body;
struct AST_Attributes;
struct AST_Comment;
struct AST_Class;
struct AST_Enum;
struct AST_Exec;
struct AST_Extern;
struct AST_Include;
struct AST_Friend;
struct AST_Fn;
struct AST_Module;
struct AST_Namespace;
struct AST_Operator;
struct AST_OpCast;
struct AST_Param;
struct AST_Specifier;
struct AST_Struct;
struct AST_Template;
struct AST_Type;
struct AST_Typedef;
struct AST_Union;
struct AST_Using;
struct AST_UsingNamespace;
struct AST_Var;

struct Code;
struct CodeBody;
// These are to offer ease of use and optionally strong type safety for the AST.
struct CodeAttributes;
struct CodeComment;
struct CodeClass;
struct CodeEnum;
struct CodeExec;
struct CodeExtern;
struct CodeInclude;
struct CodeFriend;
struct CodeFn;
struct CodeModule;
struct CodeNamespace;
struct CodeOperator;
struct CodeOpCast;
struct CodeParam;
struct CodeSpecifier;
struct CodeStruct;
struct CodeTemplate;
struct CodeType;
struct CodeTypedef;
struct CodeUnion;
struct CodeUsing;
struct CodeUsingNamespace;
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

	#define Using_Code( Typename )         \
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
	operator bool()                        \
	{                                      \
		return ast != nullptr;             \
	}

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

	Using_Code( Code );

	AST* ast;

#ifdef GEN_ENFORCE_STRONG_CODE_TYPES
#	define operator explicit operator
#endif
	operator CodeAttributes() const;
	operator CodeComment() const;
	operator CodeClass() const;
	operator CodeExec() const;
	operator CodeEnum() const;
	operator CodeExtern() const;
	operator CodeInclude() const;
	operator CodeFriend() const;
	operator CodeFn() const;
	operator CodeModule() const;
	operator CodeNamespace() const;
	operator CodeOperator() const;
	operator CodeOpCast() const;
	operator CodeParam() const;
	operator CodeSpecifier() const;
	operator CodeStruct() const;
	operator CodeTemplate() const;
	operator CodeType() const;
	operator CodeTypedef() const;
	operator CodeUnion() const;
	operator CodeUsing() const;
	operator CodeUsingNamespace() const;
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
	String      to_string  ();
	char const* type_str();
	bool        validate_body();

	template< class Type >
	Type cast()
	{
		return * this;
	}

	operator Code();
	operator CodeBody();
	operator CodeAttributes();
	operator CodeComment();
	operator CodeClass();
	operator CodeEnum();
	operator CodeExec();
	operator CodeExtern();
	operator CodeInclude();
	operator CodeFriend();
	operator CodeFn();
	operator CodeModule();
	operator CodeNamespace();
	operator CodeOperator();
	operator CodeOpCast();
	operator CodeParam();
	operator CodeSpecifier();
	operator CodeStruct();
	operator CodeTemplate();
	operator CodeType();
	operator CodeTypedef();
	operator CodeUnion();
	operator CodeUsing();
	operator CodeUsingNamespace();
	operator CodeVar();
#	pragma endregion Member Functions

	constexpr static
	uw ArrSpecs_Cap =
	(
			AST_POD_Size
			- sizeof(AST*) * 3
			- sizeof(StringCached)
			- sizeof(CodeT)
			- sizeof(ModuleFlag)
			- sizeof(s32)
	)
	/ sizeof(SpecifierT) -1; // -1 for 4 extra bytes

	union {
		struct
		{
			AST*      Attributes;     // Class, Enum, Function, Struct, Typedef, Union, Using, Variable
			AST*      Specs;          // Function, Operator, Type symbol, Variable
			union {
				AST*  ParentType;     // Class, Struct
				AST*  ReturnType;     // Function, Operator
				AST*  UnderlyingType; // Enum, Typedef
				AST*  ValueType;      // Parameter, Variable
			};
			AST*      Params;         // Function, Operator, Template
			union {
				AST*  ArrExpr;        // Type Symbol
				AST*  Body;           // Class, Enum, Function, Namespace, Struct, Union
				AST*  Declaration;    // Friend, Template
				AST*  Value;          // Parameter, Variable
			};
		};
		StringCached  Content;        // Attributes, Comment, Execution, Include
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
			AST*      Attributes;     // Class, Enum, Function, Struct, Typedef, Union, Using, Variable
			AST*      Specs;          // Function, Operator, Type symbol, Variable
			union {
				AST*  ParentType;     // Class, Struct
				AST*  ReturnType;     // Function, Operator
				AST*  UnderlyingType; // Enum, Typedef
				AST*  ValueType;      // Parameter, Variable
			};
			AST*      Params;         // Function, Operator, Template
			union {
				AST*  ArrExpr;        // Type Symbol
				AST*  Body;           // Class, Enum, Function, Namespace, Struct, Union
				AST*  Declaration;    // Friend, Template
				AST*  Value;          // Parameter, Variable
			};
		};
		StringCached  Content;        // Attributes, Comment, Execution, Include
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
		OperatorT     Op;
		AccessSpec    ParentAccess;
		s32           NumEntries;
	};
};

// Its intended for the AST to have equivalent size to its POD.
// All extra functionality within the AST namespace should just be syntatic sugar.
static_assert( sizeof(AST)     == sizeof(AST_POD), "ERROR: AST IS NOT POD" );
static_assert( sizeof(AST_POD) == AST_POD_Size,    "ERROR: AST POD is not size of AST_POD_Size" );

// Used when the its desired when omission is allowed in a definition.
#define NoCode      { nullptr }
#define CodeInvalid (* Code::Invalid.ast) // Uses an implicitly overloaded cast from the AST to the desired code type.

#pragma region Code Types
#define Define_CodeType( Typename )                           \
struct Code##Typename                                         \
{                                                             \
	Using_Code( Code##Typename );                             \
	AST* raw() 				                                  \
	{                                                         \
		return rcast( AST*, ast );                            \
	}                                                         \
	operator Code()                                           \
	{                                                         \
		return * rcast( Code*, this );                        \
	}                                                         \
	AST_##Typename* operator->()                              \
	{                                                         \
		if ( ast == nullptr )                                 \
		{                                                     \
			log_failure("Attempt to dereference a nullptr!"); \
			return nullptr;                                   \
		}                                                     \
		return ast;                                           \
	}                                                         \
	AST_##Typename* ast;                                      \
}

struct CodeBody
{
	Using_Code( CodeBody );

	void append( Code other )
	{
		raw()->append( other.ast );
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

Define_CodeType( Attributes );
Define_CodeType( Comment );
Define_CodeType( Class );
Define_CodeType( Enum );
Define_CodeType( Exec );
Define_CodeType( Extern );
Define_CodeType( Include );
Define_CodeType( Friend );
Define_CodeType( Fn );
Define_CodeType( Module );
Define_CodeType( Namespace );
Define_CodeType( Operator );
Define_CodeType( OpCast );
Define_CodeType( Struct );
Define_CodeType( Template );
Define_CodeType( Type );
Define_CodeType(Typedef);
Define_CodeType( Union );
Define_CodeType( Using );
Define_CodeType( Var );

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

struct CodeSpecifier
{
	Using_Code( CodeSpecifier );

	bool append( SpecifierT spec )
	{
		if ( raw()->NumEntries == AST::ArrSpecs_Cap )
		{
			log_failure("CodeSpecifier: Attempted to append over %d specifiers to a specifiers AST!", AST::ArrSpecs_Cap );
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
	AST_Specifier* operator->()
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

	AST_Specifier* ast;
};

#undef Define_CodeType
#undef Using_Code
#pragma endregion Code Types

#pragma region Filtered ASTs
/*
	Show only relevant members of the AST for its type.
	AST* fields are replaced with Code types.
		- Guards assignemnts to AST* fields to ensure the AST is duplicated if assigned to another parent.
*/

struct AST_Body
{
	char              _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
	Code              Front;
	Code              Back;
	Code              Parent;
	StringCached      Name;
	CodeT             Type;
	char              _PAD_UNUSED_[ sizeof(ModuleFlag) ];
	s32 			  NumEntries;
};
static_assert( sizeof(AST_Body) == sizeof(AST), "ERROR: AST_Filtered is not the same size as AST");

struct AST_Attributes
{
	union {
		char          _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		StringCached  Content;
	};
	Code              Prev;
	Code              Next;
	Code              Parent;
	StringCached      Name;
	CodeT             Type;
	char              _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Attributes) == sizeof(AST), "ERROR: AST_Attributes is not the same size as AST");

struct AST_Comment
{
	union {
		char          _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		StringCached  Content;
	};
	Code              Prev;
	Code              Next;
	Code              Parent;
	StringCached      Name;
	CodeT             Type;
	char              _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Comment) == sizeof(AST), "ERROR: AST_Comment is not the same size as AST");

struct AST_Class
{
	union {
		char                _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			CodeAttributes  Attributes;
			char 	        _PAD_SPECS_ [ sizeof(AST*) ];
			CodeType        ParentType;
			char 	        _PAD_PARAMS_[ sizeof(AST*) ];
			CodeBody        Body;
		};
	};
	Code                    Prev;
	Code                    Next;
	Code                    Parent;
	StringCached            Name;
	CodeT                   Type;
	ModuleFlag              ModuleFlags;
	AccessSpec              ParentAccess;
};
static_assert( sizeof(AST_Class) == sizeof(AST), "ERROR: AST_Class is not the same size as AST");

struct AST_Enum
{
	union {
		char                _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			CodeAttributes Attributes;
			char           _PAD_SPEC_  [ sizeof(AST*) ];
			CodeType       UnderlyingType;
			char	       _PAD_PARAMS_[ sizeof(AST*) ];
			CodeBody       Body;
		};
	};
	Code                   Prev;
	Code                   Next;
	Code                   Parent;
	StringCached           Name;
	CodeT                  Type;
	ModuleFlag             ModuleFlags;
	char 			       _PAD_UNUSED_[ sizeof(u32) ];
};
static_assert( sizeof(AST_Enum) == sizeof(AST), "ERROR: AST_Enum is not the same size as AST");

struct AST_Exec
{
	union {
		char          _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			char      _PAD_PROPERTIES_[ sizeof(AST*) * 5 ];
		};
	};
	Code              Prev;
	Code              Next;
	Code              Parent;
	StringCached      Name;
	CodeT             Type;
	char              _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Exec) == sizeof(AST), "ERROR: AST_Exec is not the same size as AST");

struct AST_Extern
{
	union {
		char          _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			char      _PAD_PROPERTIES_[ sizeof(AST*) * 4 ];
			CodeBody  Body;
		};
	};
	Code              Prev;
	Code              Next;
	Code              Parent;
	StringCached      Name;
	CodeT             Type;
	char 			  _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Extern) == sizeof(AST), "ERROR: AST_Extern is not the same size as AST");

struct AST_Include
{
	union {
		char          _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		StringCached  Content;
	};
	Code              Prev;
	Code              Next;
	Code              Parent;
	StringCached      Name;
	CodeT             Type;
	char 			  _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Include) == sizeof(AST), "ERROR: AST_Include is not the same size as AST");

struct AST_Friend
{
	union {
		char          _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			char      _PAD_PROPERTIES_[ sizeof(AST*) * 4 ];
			Code      Declaration;
		};
	};
	Code              Prev;
	Code              Next;
	Code              Parent;
	StringCached      Name;
	CodeT             Type;
	char 			  _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Friend) == sizeof(AST), "ERROR: AST_Friend is not the same size as AST");

struct AST_Fn
{
	union {
		char                _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			CodeAttributes  Attributes;
			CodeSpecifier   Specs;
			CodeType        ReturnType;
			CodeParam 	    Params;
			CodeBody        Body;
		};
	};
	Code                    Prev;
	Code                    Parent;
	Code                    Next;
	StringCached            Name;
	CodeT                   Type;
	ModuleFlag              ModuleFlags;
	char 			        _PAD_UNUSED_[ sizeof(u32) ];
};
static_assert( sizeof(AST_Fn) == sizeof(AST), "ERROR: AST_Fn is not the same size as AST");

struct AST_Module
{
	char              _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
	Code              Prev;
	Code              Next;
	Code              Parent;
	StringCached      Name;
	CodeT             Type;
	ModuleFlag        ModuleFlags;
	char 			  _PAD_UNUSED_[ sizeof(u32) ];
};
static_assert( sizeof(AST_Module) == sizeof(AST), "ERROR: AST_Module is not the same size as AST");

struct AST_Namespace
{
	union {
		char          _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct {
			char 	  _PAD_PROPERTIES_[ sizeof(AST*) * 4 ];
			CodeBody  Body;
		};
	};
	Code              Prev;
	Code              Next;
	Code              Parent;
	StringCached      Name;
	CodeT             Type;
	ModuleFlag        ModuleFlags;
	char 			  _PAD_UNUSED_[ sizeof(u32) ];
};
static_assert( sizeof(AST_Namespace) == sizeof(AST), "ERROR: AST_Namespace is not the same size as AST");

struct AST_Operator
{
	union {
		char                _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			CodeAttributes  Attributes;
			CodeSpecifier   Specs;
			CodeType        ReturnType;
			CodeParam 	    Params;
			CodeBody        Body;
		};
	};
	Code                    Prev;
	Code                    Next;
	Code                    Parent;
	StringCached            Name;
	CodeT                   Type;
	ModuleFlag              ModuleFlags;
	OperatorT               Op;
};
static_assert( sizeof(AST_Operator) == sizeof(AST), "ERROR: AST_Operator is not the same size as AST");

struct AST_OpCast
{
	union {
		char          _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			char 	  _PAD_PROPERTIES_[ sizeof(AST*) * 2 ];
			CodeType  ValueType;
			char 	  _PAD_PROPERTIES_2_[ sizeof(AST*) ];
			CodeBody  Body;
		};
	};
	Code              Prev;
	Code              Next;
	Code              Parent;
	StringCached      Name;
	CodeT             Type;
	char 			  _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_OpCast) == sizeof(AST), "ERROR: AST_OpCast is not the same size as AST");

struct AST_Param
{
	union {
		char 		  _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			char 	  _PAD_PROPERTIES_2_[ sizeof(AST*) * 2 ];
			CodeType  ValueType;
			char 	  _PAD_PROPERTIES_[ sizeof(AST*) ];
			Code      Value;
		};
	};
	CodeParam         Last;
	CodeParam         Next;
	Code              Parent;
	StringCached      Name;
	CodeT             Type;
	char 			  _PAD_UNUSED_[ sizeof(ModuleFlag) ];
	s32               NumEntries;
};
static_assert( sizeof(AST_Param) == sizeof(AST), "ERROR: AST_Param is not the same size as AST");

struct AST_Specifier
{
	SpecifierT        ArrSpecs[ AST::ArrSpecs_Cap ];
	Code              Prev;
	Code              Next;
	Code              Parent;
	StringCached      Name;
	CodeT             Type;
	char 			  _PAD_UNUSED_[ sizeof(ModuleFlag) ];
	s32               NumEntries;
};
	static_assert( sizeof(AST_Specifier) == sizeof(AST), "ERROR: AST_Specifier is not the same size as AST");

struct AST_Struct
{
	union {
		char               _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			CodeAttributes Attributes;
			char 	       _PAD_SPECS_ [ sizeof(AST*) ];
			CodeType       ParentType;
			char 	       _PAD_PARAMS_[ sizeof(AST*) ];
			CodeBody       Body;
		};
	};
	Code                   Prev;
	Code                   Next;
	Code                   Parent;
	StringCached           Name;
	CodeT                  Type;
	ModuleFlag             ModuleFlags;
	AccessSpec             ParentAccess;
};
static_assert( sizeof(AST_Struct) == sizeof(AST), "ERROR: AST_Struct is not the same size as AST");

struct AST_Template
{
	union {
		char 		       _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			char 	       _PAD_PROPERTIES_[ sizeof(AST*) * 3 ];
			CodeParam 	   Params;
			Code           Declaration;
		};
	};
	Code                   Prev;
	Code                   Next;
	Code                   Parent;
	StringCached           Name;
	CodeT                  Type;
	ModuleFlag             ModuleFlags;
	char 			       _PAD_UNUSED_[ sizeof(u32) ];
};
static_assert( sizeof(AST_Template) == sizeof(AST), "ERROR: AST_Template is not the same size as AST");

struct AST_Type
{
	union {
		char 		       _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			CodeAttributes Attributes;
			CodeSpecifier  Specs;
			char 	       _PAD_PROPERTIES_[ sizeof(AST*) * 2 ];
			Code           ArrExpr;
		};
	};
	Code                   Prev;
	Code                   Next;
	Code                   Parent;
	StringCached           Name;
	CodeT                  Type;
	char 			       _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Type) == sizeof(AST), "ERROR: AST_Type is not the same size as AST");

struct AST_Typedef
{
	union {
		char 		       _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			CodeAttributes Attributes;
			char 	       _PAD_SPECS_     [ sizeof(AST*) ];
			Code           UnderlyingType;
			char 	       _PAD_PROPERTIES_[ sizeof(AST*) * 2 ];
		};
	};
	Code                   Prev;
	Code                   Next;
	Code                   Parent;
	StringCached           Name;
	CodeT                  Type;
	ModuleFlag             ModuleFlags;
	char 			       _PAD_UNUSED_[ sizeof(u32) ];
};
static_assert( sizeof(AST_Typedef) == sizeof(AST), "ERROR: AST_Typedef is not the same size as AST");

struct AST_Union
{
	union {
		char 		       _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			CodeAttributes Attributes;
			char 	       _PAD_PROPERTIES_[ sizeof(AST*) * 3 ];
			CodeBody       Body;
		};
	};
	Code                   Prev;
	Code                   Next;
	Code                   Parent;
	StringCached           Name;
	CodeT                  Type;
	ModuleFlag             ModuleFlags;
	char 			       _PAD_UNUSED_[ sizeof(u32) ];
};
static_assert( sizeof(AST_Union) == sizeof(AST), "ERROR: AST_Union is not the same size as AST");

struct AST_Using
{
	union {
		char 		        _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			CodeAttributes  Attributes;
			char 	        _PAD_SPECS_     [ sizeof(AST*) ];
			CodeType        UnderlyingType;
			char 	        _PAD_PROPERTIES_[ sizeof(AST*) * 2 ];
		};
	};
	Code                    Prev;
	Code                    Next;
	Code                    Parent;
	StringCached            Name;
	CodeT                   Type;
	ModuleFlag              ModuleFlags;
	char 			        _PAD_UNUSED_[ sizeof(u32) ];
};
static_assert( sizeof(AST_Using) == sizeof(AST), "ERROR: AST_Using is not the same size as AST");

struct AST_Var
{
	union {
		char               _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			CodeAttributes Attributes;
			CodeSpecifier  Specs;
			CodeType       ValueType;
			char 	       _PAD_PROPERTIES_[ sizeof(AST*) ];
			Code           Value;
		};
	};
	Code                   Prev;
	Code                   Next;
	Code                   Parent;
	StringCached           Name;
	CodeT                  Type;
	ModuleFlag             ModuleFlags;
	char 			       _PAD_UNUSED_[ sizeof(u32) ];
};
static_assert( sizeof(AST_Var) == sizeof(AST), "ERROR: AST_Var is not the same size as AST");
#pragma endregion Filtered ASTs

#pragma endregion Data Structures

#pragma region Gen Interface
// Initialize the library.
// This currently just initializes the CodePool.
void init();

// Currently manually free's the arenas, code for checking for leaks.
// However on Windows at least, it doesn't need to occur as the OS will clean up after the process.
void deinit();

// Clears the allocations, but doesn't return to the heap, the calls init() again.
// Ease of use.
void reset();

// Used internally to retrive or make string allocations.
// Strings are stored in a series of string arenas of fixed size (SizePer_StringArena)
StringCached get_cached_string( StrC str );

/*
	This provides a fresh Code AST.
	The gen interface use this as their method from getting a new AST object from the CodePool.
	Use this if you want to make your own API for formatting the supported Code Types.
*/
Code make_code();

// Set these before calling gen's init() procedure.
// Data

void set_allocator_data_arrays       ( AllocatorInfo data_array_allocator );
void set_allocator_code_pool         ( AllocatorInfo pool_allocator );
void set_allocator_lexer             ( AllocatorInfo lex_allocator );
void set_allocator_string_arena      ( AllocatorInfo string_allocator );
void set_allocator_string_table      ( AllocatorInfo string_allocator );
void set_allocator_type_table        ( AllocatorInfo type_reg_allocator );

#pragma region Upfront
CodeAttributes def_attributes( StrC content );
CodeComment    def_comment   ( StrC content );

CodeClass def_class( StrC name
	, Code           body         = NoCode
	, CodeType       parent       = NoCode, AccessSpec access = AccessSpec::Default
	, CodeAttributes attributes   = NoCode
	, ModuleFlag mflags = ModuleFlag::None );

CodeEnum def_enum( StrC name
	, Code         body      = NoCode,      CodeType       type       = NoCode
	, EnumT        specifier = EnumRegular, CodeAttributes attributes = NoCode
	, ModuleFlag   mflags    = ModuleFlag::None );

CodeExec   def_execution  ( StrC content );
CodeExtern def_extern_link( StrC name, Code body );
CodeFriend def_friend     ( Code symbol );

CodeFn def_function( StrC name
	, CodeParam      params     = NoCode, CodeType       ret_type   = NoCode, Code body = NoCode
	, CodeSpecifier specifiers = NoCode, CodeAttributes attributes = NoCode
	, ModuleFlag mflags     = ModuleFlag::None );

CodeInclude   def_include  ( StrC content );
CodeModule    def_module   ( StrC name,            ModuleFlag mflags = ModuleFlag::None );
CodeNamespace def_namespace( StrC name, Code body, ModuleFlag mflags = ModuleFlag::None );

CodeOperator def_operator( OperatorT op
	, CodeParam      params     = NoCode, CodeType       ret_type   = NoCode, Code body = NoCode
	, CodeSpecifier specifiers = NoCode, CodeAttributes attributes = NoCode
	, ModuleFlag     mflags     = ModuleFlag::None );

CodeOpCast def_operator_cast( CodeType type, Code body = NoCode );

CodeParam      def_param    ( CodeType type, StrC name, Code value = NoCode );
CodeSpecifier def_specifier( SpecifierT specifier );

CodeStruct def_struct( StrC name
	, Code           body       = NoCode
	, CodeType       parent     = NoCode, AccessSpec access = AccessSpec::Default
	, CodeAttributes attributes = NoCode
	, ModuleFlag     mflags     = ModuleFlag::None );

CodeTemplate def_template( CodeParam params, Code definition, ModuleFlag mflags = ModuleFlag::None );

CodeType    def_type   ( StrC name, Code arrayexpr = NoCode, CodeSpecifier specifiers = NoCode, CodeAttributes attributes = NoCode );
CodeTypedef def_typedef( StrC name, Code type, CodeAttributes attributes = NoCode, ModuleFlag mflags = ModuleFlag::None );

CodeUnion def_union( StrC name, Code body, CodeAttributes attributes = NoCode, ModuleFlag mflags = ModuleFlag::None );

CodeUsing def_using( StrC name, CodeType type = NoCode
	, CodeAttributes attributess = NoCode
	, ModuleFlag     mflags      = ModuleFlag::None );

CodeUsing def_using_namespace( StrC name );

CodeVar def_variable( CodeType type, StrC name, Code value = NoCode
	, CodeSpecifier specifiers = NoCode, CodeAttributes attributes = NoCode
	, ModuleFlag     mflags     = ModuleFlag::None );

// Constructs an empty body. Use AST::validate_body() to check if the body is was has valid entries.
CodeBody def_body( CodeT type );

// There are two options for defining a struct body, either varadically provided with the args macro to auto-deduce the arg num,
/// or provide as an array of Code objects.

CodeBody      def_class_body      ( s32 num, ... );
CodeBody      def_class_body      ( s32 num, Code* codes );
CodeBody      def_enum_body       ( s32 num, ... );
CodeBody      def_enum_body       ( s32 num, Code* codes );
CodeBody      def_export_body     ( s32 num, ... );
CodeBody      def_export_body     ( s32 num, Code* codes);
CodeBody      def_extern_link_body( s32 num, ... );
CodeBody      def_extern_link_body( s32 num, Code* codes );
CodeBody      def_function_body   ( s32 num, ... );
CodeBody      def_function_body   ( s32 num, Code* codes );
CodeBody      def_global_body     ( s32 num, ... );
CodeBody      def_global_body     ( s32 num, Code* codes );
CodeBody      def_namespace_body  ( s32 num, ... );
CodeBody      def_namespace_body  ( s32 num, Code* codes );
CodeParam     def_params          ( s32 num, ... );
CodeParam     def_params          ( s32 num, CodeParam* params );
CodeSpecifier def_specifiers      ( s32 num, ... );
CodeSpecifier def_specifiers      ( s32 num, SpecifierT* specs );
CodeBody      def_struct_body     ( s32 num, ... );
CodeBody      def_struct_body     ( s32 num, Code* codes );
CodeBody      def_union_body      ( s32 num, ... );
CodeBody      def_union_body      ( s32 num, Code* codes );
#pragma endregion Upfront

#pragma region Parsing
CodeClass      parse_class        ( StrC class_def     );
CodeEnum       parse_enum         ( StrC enum_def      );
CodeBody       parse_export_body  ( StrC export_def    );
CodeExtern     parse_extern_link  ( StrC exten_link_def);
CodeFriend     parse_friend       ( StrC friend_def    );
CodeFn         parse_function     ( StrC fn_def        );
CodeBody       parse_global_body  ( StrC body_def      );
CodeNamespace  parse_namespace    ( StrC namespace_def );
CodeOperator   parse_operator     ( StrC operator_def  );
CodeOpCast     parse_operator_cast( StrC operator_def  );
CodeStruct     parse_struct       ( StrC struct_def    );
CodeTemplate   parse_template     ( StrC template_def  );
CodeType       parse_type         ( StrC type_def      );
CodeTypedef    parse_typedef      ( StrC typedef_def   );
CodeUnion      parse_union        ( StrC union_def     );
CodeUsing      parse_using        ( StrC using_def     );
CodeVar        parse_variable     ( StrC var_def       );
#pragma endregion Parsing

#pragma region Untyped text
sw   token_fmt_va( char* buf, uw buf_size, s32 num_tokens, va_list va );
StrC token_fmt_impl( sw, ... );

Code untyped_str      ( StrC content);
Code untyped_fmt      ( char const* fmt, ... );
Code untyped_token_fmt( char const* fmt, s32 num_tokens, ... );
#pragma endregion Untyped text

#pragma endregion Gen Interaface

struct Builder
{
	FileInfo File;
	String   Buffer;

	void print( Code );
	void print_fmt( char const* fmt, ... );

	bool open( char const* path );
	void write();
};

#pragma region Inlines
void AST::append( AST* other )
{
	if ( other->Parent )
		other = other->duplicate();

	other->Parent = this;

	if ( Front == nullptr )
	{
		Front = other;
		Back  = other;

		NumEntries++;
		return;
	}

	AST*
	Current       = Back;
	Current->Next = other;
	other->Prev   = Current;
	Back          = other;
	NumEntries++;
}

char const* AST::debug_str()
{
	char const* fmt = stringize(
		\nCode Debug:
		\nType    : %s
		\nParent  : %s
		\nName    : %s
		\nComment : %s
	);

	// These should be used immediately in a log.
	// Thus if its desired to keep the debug str
	// for multiple calls to bprintf,
	// allocate this to proper string.
	return str_fmt_buf( fmt
	,	type_str()
	,	Parent   ? Parent->Name : ""
	,	Name     ? Name         : ""
	);
}

Code& AST::entry( u32 idx )
{
	AST** current = & Front;
	while ( idx >= 0 && current != nullptr )
	{
		if ( idx == 0 )
			return * rcast( Code*, current);

		current = & ( * current )->Next;
		idx--;
	}

	return * rcast( Code*, current);
}

bool AST::has_entries()
{
	return NumEntries;
}

char const* AST::type_str()
{
	return ECode::to_str( Type );
}

AST::operator Code()
{
	return { this };
}

Code& Code::operator ++()
{
	if ( ast )
		ast = ast->Next;

	return *this;
}

#pragma region AST & Code Gen Common
#define Define_CodeImpl( Typename )                                                  \
char const* Typename::debug_str()                                                    \
{                                                                                    \
	if ( ast == nullptr )                                                            \
		return "Code::debug_str: AST is null!";                                      \
																				     \
	return rcast(AST*, ast)->debug_str();                                            \
}                                                                                    \
Code Typename::duplicate()														     \
{                                                                                    \
	if ( ast == nullptr )                                                            \
	{                                                                                \
		log_failure("Code::duplicate: Cannot duplicate code, AST is null!");         \
		return Code::Invalid;                                                        \
	}                                                                                \
																				     \
	return { rcast(AST*, ast)->duplicate() };                                        \
}                                                                                    \
bool Typename::is_equal( Code other )                                                \
{                                                                                    \
	if ( ast == nullptr || other.ast == nullptr )                                    \
	{                                                                                \
		log_failure("Code::is_equal: Cannot compare code, AST is null!");            \
		return false;                                                                \
	}                                                                                \
																				     \
	return rcast(AST*, ast)->is_equal( other.ast );                                  \
}                                                                                    \
bool Typename::is_valid()                                                            \
{                                                                                    \
	return (AST*) ast != nullptr && rcast( AST*, ast)->Type != CodeT::Invalid;       \
}                                                                                    \
void Typename::set_global()                                                          \
{                                                                                    \
	if ( ast == nullptr )                                                            \
	{                                                                                \
		log_failure("Code::set_global: Cannot set code as global, AST is null!");    \
		return;                                                                      \
	}                                                                                \
																					 \
	rcast(AST*, ast)->Parent = Code::Global.ast;                                     \
}                                                                                    \
String Typename::to_string()													     \
{                                                                                    \
	if ( ast == nullptr )                                                            \
	{                                                                                \
		log_failure("Code::to_string: Cannot convert code to string, AST is null!"); \
		return { nullptr };                                                          \
	}                                                                                \
																					 \
	return rcast(AST*, ast)->to_string();                                            \
}                                                                                    \
Typename& Typename::operator =( Code other )                                         \
{                                                                                    \
	if ( other.ast && other->Parent )                                                \
	{                                                                                \
		ast = rcast( decltype(ast), other.ast->duplicate() );                        \
		rcast( AST*, ast)->Parent = nullptr;                                         \
	}                                                                                \
																				     \
	ast = rcast( decltype(ast), other.ast );                                         \
	return *this;                                                                    \
}                                                                                    \
bool Typename::operator ==( Code other )                                             \
{                                                                                    \
	return (AST*) ast == other.ast;                                                  \
}                                                                                    \
bool Typename::operator !=( Code other )                                             \
{                                                                                    \
	return (AST*) ast != other.ast;                                                  \
}

Define_CodeImpl( Code );
Define_CodeImpl( CodeBody );
Define_CodeImpl( CodeAttributes );
Define_CodeImpl( CodeComment );
Define_CodeImpl( CodeClass );
Define_CodeImpl( CodeEnum );
Define_CodeImpl( CodeExec );
Define_CodeImpl( CodeExtern );
Define_CodeImpl( CodeInclude );
Define_CodeImpl( CodeFriend );
Define_CodeImpl( CodeFn );
Define_CodeImpl( CodeModule );
Define_CodeImpl( CodeNamespace );
Define_CodeImpl( CodeOperator );
Define_CodeImpl( CodeOpCast );
Define_CodeImpl( CodeParam );
Define_CodeImpl( CodeSpecifier );
Define_CodeImpl( CodeStruct );
Define_CodeImpl( CodeTemplate );
Define_CodeImpl( CodeType );
Define_CodeImpl( CodeTypedef );
Define_CodeImpl( CodeUnion );
Define_CodeImpl( CodeUsing );
Define_CodeImpl( CodeVar );
#undef Define_CodeImpl

#define Define_AST_Cast( typename )              \
AST::operator Code ## typename()                 \
{                                                \
	return { rcast( AST_ ## typename*, this ) }; \
}

Define_AST_Cast( Body );
Define_AST_Cast( Attributes );
Define_AST_Cast( Comment );
Define_AST_Cast( Class );
Define_AST_Cast( Enum );
Define_AST_Cast( Exec );
Define_AST_Cast( Extern );
Define_AST_Cast( Include );
Define_AST_Cast( Friend );
Define_AST_Cast( Fn );
Define_AST_Cast( Module );
Define_AST_Cast( Namespace );
Define_AST_Cast( Operator );
Define_AST_Cast( OpCast );
Define_AST_Cast( Param );
Define_AST_Cast( Specifier );
Define_AST_Cast( Struct );
Define_AST_Cast( Template );
Define_AST_Cast( Type );
Define_AST_Cast( Typedef );
Define_AST_Cast( Union );
Define_AST_Cast( Using );
Define_AST_Cast( Var );
#undef Define_AST_Cast

#define Define_CodeCast( type )     \
Code::operator Code ## type() const \
{                                   \
	return { (AST_ ## type*) ast }; \
}

Define_CodeCast( Attributes );
Define_CodeCast( Comment );
Define_CodeCast( Class );
Define_CodeCast( Exec );
Define_CodeCast( Enum );
Define_CodeCast( Extern );
Define_CodeCast( Include );
Define_CodeCast( Friend );
Define_CodeCast( Fn );
Define_CodeCast( Module );
Define_CodeCast( Namespace );
Define_CodeCast( Operator );
Define_CodeCast( OpCast );
Define_CodeCast( Param );
Define_CodeCast( Specifier );
Define_CodeCast( Struct );
Define_CodeCast( Template );
Define_CodeCast( Type );
Define_CodeCast( Typedef );
Define_CodeCast( Union );
Define_CodeCast( Using );
Define_CodeCast( Var );
Define_CodeCast( Body);
#undef Define_CodeCast
#pragma endregion AST & Code Gen Common

void CodeParam::append( CodeParam other )
{
	AST* self  = (AST*) ast;
	AST* entry = (AST*) other.ast;

	if ( entry->Parent )
		entry = entry->duplicate();

	entry->Parent = self;

	if ( self->Last == nullptr )
	{
		self->Last = entry;
		self->Next = entry;
		self->NumEntries++;
		return;
	}

	self->Last->Next = entry;
	self->Last       = entry;
	self->NumEntries++;
}

CodeParam CodeParam::get( s32 idx )
{
	CodeParam param = *this;
	do
	{
		if ( ! ++ param )
			return { nullptr };

		return { (AST_Param*) param.raw()->Next };
	}
	while ( --idx );

	return { nullptr };
}

bool CodeParam::has_entries()
{
	return ast->NumEntries > 0;
}

CodeParam& CodeParam::operator ++()
{
	ast = ast->Next.ast;
	return * this;
}

CodeBody def_body( CodeT type )
{
	switch ( type )
	{
		using namespace ECode;
		case Class_Body:
		case Enum_Body:
		case Export_Body:
		case Extern_Linkage:
		case Function_Body:
		case Global_Body:
		case Namespace_Body:
		case Struct_Body:
		case Union_Body:
		break;

		default:
			log_failure( "def_body: Invalid type %s", (char const*)ECode::to_str(type) );
			return (CodeBody)Code::Invalid;
	}

	Code
	result       = make_code();
	result->Type = type;
	return (CodeBody)result;
}

//! Do not use directly. Use the token_fmt macro instead.
// Takes a format string (char const*) and a list of tokens (StrC) and returns a StrC of the formatted string.
StrC token_fmt_impl( sw num, ... )
{
	local_persist thread_local
	char buf[GEN_PRINTF_MAXLEN] = { 0 };
	mem_set( buf, 0, GEN_PRINTF_MAXLEN );

	va_list va;
	va_start(va, num );
	sw result = token_fmt_va(buf, GEN_PRINTF_MAXLEN, num, va);
	va_end(va);

	return { result, buf };
}
#pragma endregion Inlines

// namespace gen
}

#pragma region Constants
#ifdef GEN_DEFINE_LIBRARY_CODE_CONSTANTS
namespace gen
{
	// Predefined typename codes. Are set to readonly and are setup during gen::init()

	extern CodeType t_b32;

	extern CodeType t_s8;
	extern CodeType t_s16;
	extern CodeType t_s32;
	extern CodeType t_s64;

	extern CodeType t_u8;
	extern CodeType t_u16;
	extern CodeType t_u32;
	extern CodeType t_u64;

	extern CodeType t_sw;
	extern CodeType t_uw;

	extern CodeType t_f32;
	extern CodeType t_f64;
}
#endif

namespace gen
{
	// These constexprs are used for allocation behavior of data structures
	// or string handling while constructing or serializing.
	// Change them to suit your needs.

	constexpr s32 InitSize_DataArrays  = 16;
	constexpr s32 InitSize_StringTable = megabytes(4);

	// NOTE: This limits the maximum size of an allocation
	// If you are generating a string larger than this, increase the size of the bucket here.
	constexpr uw Global_BucketSize          = megabytes(10);
	constexpr s32 CodePool_NumBlocks        = kilobytes(4);
	constexpr s32 SizePer_StringArena       = megabytes(1);

	constexpr s32 MaxCommentLineLength      = 1024;
	constexpr s32 MaxNameLength             = 128;
	constexpr s32 MaxUntypedStrLength       = kilobytes(640);
	constexpr s32 StringTable_MaxHashLength = kilobytes(1);
	constexpr s32 TokenFmt_TokenMap_MemSize	= kilobytes(4);
	constexpr s32 LexAllocator_Size         = megabytes(10);
	constexpr s32 Builder_StrBufferReserve  = megabytes(1);

	extern CodeType t_auto;
	extern CodeType t_void;
	extern CodeType t_int;
	extern CodeType t_bool;
	extern CodeType t_char;
	extern CodeType t_wchar_t;
	extern CodeType t_class;
	extern CodeType t_typename;

	extern Code access_public;
	extern Code access_protected;
	extern Code access_private;

	extern Code module_global_fragment;
	extern Code module_private_fragment;

	extern Code pragma_once;

	extern CodeSpecifier spec_const;
	extern CodeSpecifier spec_consteval;
	extern CodeSpecifier spec_constexpr;
	extern CodeSpecifier spec_constinit;
	extern CodeSpecifier spec_extern_linkage;
	extern CodeSpecifier spec_global;
	extern CodeSpecifier spec_inline;
	extern CodeSpecifier spec_internal_linkage;
	extern CodeSpecifier spec_local_persist;
	extern CodeSpecifier spec_mutable;
	extern CodeSpecifier spec_ptr;
	extern CodeSpecifier spec_ref;
	extern CodeSpecifier spec_register;
	extern CodeSpecifier spec_rvalue;
	extern CodeSpecifier spec_static_member;
	extern CodeSpecifier spec_thread_local;
	extern CodeSpecifier spec_volatile;
}
#pragma endregion Constants

#pragma region Macros
#	define gen_main main

#	define __ NoCode

//	Convienence for defining any name used with the gen api.
//  Lets you provide the length and string literal to the functions without the need for the DSL.
#	define name( Id_ )   { sizeof(stringize( Id_ )) - 1, stringize(Id_) }

//  Same as name just used to indicate intention of literal for code instead of names.
#	define code( ... ) { sizeof(stringize(__VA_ARGS__)) - 1, stringize( __VA_ARGS__ ) }

#	define args( ... ) num_args( __VA_ARGS__ ), __VA_ARGS__

#	define code_str( ... ) gen::untyped_str( code( __VA_ARGS__ ) )
#	define code_fmt( ... ) gen::untyped_str( token_fmt( __VA_ARGS__ ) )

// Takes a format string (char const*) and a list of tokens (StrC) and returns a StrC of the formatted string.
#	define token_fmt( ... ) gen::token_fmt_impl( (num_args( __VA_ARGS__ ) + 1) / 2, __VA_ARGS__ )
#pragma endregion Macros

#ifdef GEN_EXPOSE_BACKEND
namespace gen
{
	// Global allocator used for data with process lifetime.
	extern AllocatorInfo  GlobalAllocator;
	extern Array< Arena > Global_AllocatorBuckets;
	extern Array< Pool >  CodePools;
	extern Array< Arena > StringArenas;

	extern StringTable StringCache;

	extern Arena LexArena;

	extern AllocatorInfo Allocator_DataArrays;
	extern AllocatorInfo Allocator_CodePool;
	extern AllocatorInfo Allocator_Lexer;
	extern AllocatorInfo Allocator_StringArena;
	extern AllocatorInfo Allocator_StringTable;
	extern AllocatorInfo Allocator_TypeTable;
}
#endif

#include "gen.pop_ignores.inline.hpp"
