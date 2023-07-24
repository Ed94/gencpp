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
	Entry( Consteval,        consteval )         \
	Entry( Constexpr,        constexpr )         \
	Entry( Constinit,        constinit )         \
	Entry( Explicit,         explicit )          \
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
	Entry( Volatile,         volatile )          \
	Entry( Virtual,          virtual )           \
	Entry( Const,            const )             \
	Entry( Final,            final )             \
	Entry( Override,         override )

	enum Type : u32
	{
	#	define Entry( Specifier, Code ) Specifier,
		Define_Specifiers
	#	undef Entry

		Num_Specifiers,
	};

	inline
	bool is_trailing( Type specifier )
	{
		return specifier > Virtual;
	}

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

	Override these to change the attribute to your own unique identifier convention.

	The tokenizer identifies attribute defines with the GEN_Define_Attribute_Tokens macros.
	See the example below and the Define_TokType macro used in gen.cpp to know the format.
	While the library can parse raw attributes, most projects use defines to wrap them for compiler
	platform indendence. The token define allows support for them without having to modify the library.
*/
#if defined(GEN_SYSTEM_WINDOWS) || defined( __CYGWIN__ )
#ifndef GEN_Attribute_Keyword
#	define GEN_API_Export_Code   __declspec(dllexport)
#	define GEN_API_Import_Code   __declspec(dllimport)
#	define GEN_Attribute_Keyword __declspec
#endif

constexpr char const* Attribute_Keyword = stringize( GEN_Attribute_Keyword);

#elif GEN_HAS_ATTRIBUTE( visibility ) || GEN_GCC_VERSION_CHECK( 3, 3, 0 )
#ifndef GEN_Attribute_Keyword
#	define GEN_API_Export_Code   __attribute__ ((visibility ("default")))
#	define GEN_API_Import_Code   __attribute__ ((visibility ("default")))
#	define GEN_Attribute_Keyword __attribute__
#endif

constexpr char const* Attribute_Keyword = stringize( GEN_Attribute_Keyword );

#else
#ifndef GEN_Attribute_Keyword
#	define GEN_API_Export_Code
#	define GEN_API_Import_Code
#	define GEN_Attribute_Keyword
#endif

constexpr char const* Attribute_Keyword = "";
#endif

#ifndef GEN_Define_Attribute_Tokens
#	define GEN_Define_Attribute_Tokens         \
	Entry( API_Export, "GEN_API_Export_Code" ) \
	Entry( API_Import, "GEN_API_Import_Code" )
#endif

