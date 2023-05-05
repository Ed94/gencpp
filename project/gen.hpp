/*
	gencpp: An attempt at simple staged metaprogramming for c/c++.

	See Readme.md for more information from the project repository.

	Public Address:
	https://github.com/Ed94/gencpp
*/
#pragma once

#include "Bloat.hpp"

// Temporarily here for debugging purposes.
#define GEN_DEFINE_DSL
#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
// #define GEN_DONT_USE_FATAL
#define GEN_ENFORCE_READONLY_AST

#define GEN_FEATURE_INCREMENTAL
#define GEN_FEATURE_PARSING
#define GEN_FEATURE_EDITOR
#define GEN_FEATURE_SCANNER


#ifdef gen_time
namespace gen
{
	using LogFailType = sw(*)(char const*, ...);

	// By default this library will either crash or exit if an error is detected while generating codes.
	// Even if set to not use fatal, fatal will still be used for memory failures as the library is unusable when they occur.
#	ifdef GEN_DONT_USE_FATAL
		constexpr LogFailType log_failure = log_fmt;
#	else
		constexpr LogFailType log_failure = fatal;
#	endif

	namespace ECode
	{
	#	define Define_Types           \
		Entry( Untyped )              \
		Entry( Comment )              \
		Entry( Access_Private )       \
		Entry( Access_Protected )     \
		Entry( Access_Public )        \
		Entry( Attributes )           \
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
		Entry( Parameters )           \
		Entry( Preprocessor_Include ) \
		Entry( Specifiers )           \
		Entry( Struct )               \
		Entry( Struct_Fwd )           \
		Entry( Struct_Body )          \
		Entry( Typedef )              \
		Entry( Typename )             \
		Entry( Union )			      \
		Entry( Union_Fwd )		      \
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
		char const* to_str( Type type )
		{
			static
			char const* lookup[Num_Types] = {
			#	define Entry( Type ) txt( Type ),
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

	enum class UsingT : u8
	{
		Regular,
		Namespace
	};

	constexpr UsingT UsingRegular   = UsingT::Regular;
	constexpr UsingT UsingNamespace = UsingT::Namespace;

	namespace EOperator
	{
	#	define Define_Operators       \
		Entry( Assign,          =  )  \
		Entry( Assign_Add,      += )  \
		Entry( Assign_Subtract, -= )  \
		Entry( Assgin_Multiply, *= )  \
		Entry( Assgin_Divide,   /= )  \
		Entry( Assgin_Modulo,   %= )  \
		Entry( Assgin_BAnd,     &= )  \
		Entry( Assgin_BOr,      |= )  \
		Entry( Assign_BXOr,     ^= )  \
		Entry( Assign_LShift,   <<= ) \
		Entry( Assign_RShift,   >>= ) \
		Entry( Increment,       ++ )  \
		Entry( Decrement,       -- )  \
		Entry( Unary_Plus,      + )   \
		Entry( Unary_Minus,     - )   \
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
		Entry( LNot,            ! )   \
		Entry( LAnd,            && )  \
		Entry( LOr,             || )  \
		Entry( Equals,          == )  \
		Entry( NotEquals,       != )  \
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
			using something = u8;
			typedef u8 another;

			local_persist
			char const* lookup[ Num_Ops ] = {
			#	define Entry( Type_, Token_ ) txt(Token_),
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

		#define Define_Specifiers                    \
		Entry( Const,            const )             \
		Entry( Consteval,        consteval )         \
		Entry( Constexpr,        constexpr )         \
		Entry( Constinit,        constinit )         \
		Entry( External_Linkage, extern )            \
		Entry( Inline,           inline )            \
		Entry( Internal_Linkage, static )            \
		Entry( Local_Persist,    static )            \
		Entry( Mutable,          mutable )           \
		Entry( Ptr,              * )                 \
		Entry( Ref,              & )                 \
		Entry( Register,         register )          \
		Entry( RValue,           && )                \
		Entry( Static_Member,    static  )           \
		Entry( Thread_Local,     thread_local )      \
		Entry( Volatile,         volatile )          \
		Entry( Type_Signed,      signed )            \
		Entry( Type_Unsigned,	 unsigned )          \
		Entry( Type_Short,       short )             \
		Entry( Type_Long,        long )

		enum Type : u32
		{
		#	define Entry( Specifier, Code ) Specifier,
			Define_Specifiers
		#	undef Entry

			Num_Specifiers,
			Invalid,
		};

		// Specifier to string
		inline
		char const* to_str( Type specifier )
		{
			local_persist
			char const* lookup[ Num_Specifiers ] = {
			#	define Entry( Spec_, Code_ ) txt(Code_),
				Define_Specifiers
			#	undef Entry
			};

			return lookup[ specifier ];
		}

		inline
		Type to_type( char const* str, s32 length )
		{
			local_persist
			u32 keymap[ Num_Specifiers ];
			do_once_start
				for ( u32 index = 0; index < Num_Specifiers; index++ )
				{
					char const* enum_str = to_str( (Type)index );

					keymap[index] = crc32( enum_str, str_len(enum_str, 42) );
				}
			do_once_end

			u32 hash = crc32(str, length );

			for ( u32 index = 0; index < Num_Specifiers; index++ )
			{
				if ( keymap[index] == hash )
					return (Type)index;
			}

			return Invalid;
		}

		#undef Define_Specifiers
	}
	using SpecifierT = ESpecifier::Type;

	enum class AccessSpec : u32
	{
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
			"private",
			"protected",
			"public",
		};

		if ( type > AccessSpec::Public )
			return lookup[ (u32)AccessSpec::Invalid ];

		return lookup[ (u32)type ];
	}

	enum class ModuleFlag : u32
	{
		None    = 0,
		Export  = bit(0),
		Import  = bit(1),
		Private = bit(2),

		Num_ModuleFlags,
		Invalid,
	};

	/*
		Predefined attributes

		Used for the parser constructors to identify non-standard attributes
	*/
	namespace Attribute
	{
	#if defined(ZPL_SYSTEM_WINDOWS) || defined( __CYGWIN__ )
	#	define GEN_API_
	#	define GEN_API_Export_Code   __declspec(dllexport)
	#	define GEN_API_Import_Code   __declspec(dllimport)
	#	define GEN_Attribute_Keyword __declspec

		constexpr char const* API_Export = txt( GEN_API_Export_Code  );
		constexpr char const* API_Import = txt( GEN_API_Import_Code  );
		constexpr char const* Keyword    = txt( GEN_Attribute_Keyword);

	#elif ZPL_HAS_ATTRIBUTE( visibility ) || ZPL_GCC_VERSION_CHECK( 3, 3, 0 ) || ZPL_INTEL_VERSION_CHECK( 13, 0, 0 )
	#	define GEN_API_Export_Code   __attribute__ ((visibility ("default")))
	#	define GEN_API_Import_Code   __attribute__ ((visibility ("default")))
	#	define GEN_Attribute_Keyword __attribute__

		constexpr char const* API_Export = txt( GEN_API_Export_Code );
		constexpr char const* API_Import = txt( GEN_API_Import_Code );
		constexpr char const* Keyword    = txt( GEN_Attribute_Keyword);

	#else
	#	define GEN_API_Export_Code
	#	define GEN_API_Import_Code
	#	define GEN_Attribute_Keyword

		constexpr char const* API_Export = "";
		constexpr char const* API_Import = "";
		constexpr char const* Keyword    = "";
	#endif
	}

#pragma region Data Structures
	// Implements basic string interning. Data structure is based off the ZPL Hashtable.
	ZPL_TABLE_DECLARE( ZPL_EXTERN, StringTable, str_tbl_, String );

	// Represents strings cached with the string table.
	// Should never be modified, if changed string is desired, cache_string( str ) another.
	using StringCached = char const*;

	// Desired width of the AST data structure.
	constexpr u32 AST_POD_Size = 256;

	// TODO: If perf needs it, convert layout an SOA format.
	/*
		Simple AST POD with functionality to seralize into C++ syntax.

		ASTs are currently stored as an AOS. They are always reconstructed on demand.
		Thus redundant AST can easily occur.
		Not sure if its better to store them in a hashmap.

		Any type specific functions assume the construction of the AST was done correctly.
	*/
	struct AST
	{
	#pragma region Member Functions

		// Used with incremental constructors
		// Adds and checks entries to see if they are valid additions the type of ast.
		bool add( AST* other );

		void add_entry( AST* other );

		inline
		AST* body()
		{
			return Entries[0];
		}

		AST* duplicate();

		inline
		bool has_entries()
		{
			return Entries[0];
		}

		inline
		bool is_invalid()
		{
			return Type != ECode::Invalid;
		}

		inline
		s32 num_entries()
		{
			return DynamicEntries ? array_count(Entries) : StaticIndex;
		}

		// Class/Struct

		inline
		AST* parent()
		{
			return Entries[1];
		}

		// Enum

		inline
		AST* underlying_type()
		{
			return Entries[1];
		}

		// Parameter

		bool add_param( AST* type, s32 length, char const* name );

		inline
		AST* get_param( s32 index )
		{
			if ( index <= 0 )
				return this;

			return Entries[ index + 1 ];
		}

		inline
		s32 param_count()
		{
			// The first entry (which holds the type) represents the first parameter.
			return num_entries();
		}

		inline
		AST* param_type()
		{
			return Entries[0];
		}

		// Specifiers

		inline
		bool add_specifier( SpecifierT spec )
		{
			if ( StaticIndex == AST::ArrSpecs_Cap )
			{
				log_failure("AST::add_specifier: Attempted to add over %d specifiers to a specifiers AST!", AST::ArrSpecs_Cap );
				return false;
			}

			ArrSpecs[ StaticIndex ] = spec;
			StaticIndex++;
			return true;
		}

		inline
		s32 has_specifier( SpecifierT spec )
		{
			for ( s32 idx = 0; idx < StaticIndex; idx++ )
			{
				if ( ArrSpecs[StaticIndex] == spec )
					return idx;
			}

			return -1;
		}

		// Typename

		inline
		bool typename_is_ptr()
		{
			assert_crash("not implemented");
		}

		inline
		bool typename_is_ref()
		{
			assert_crash("not implemented");
		}

		inline
		AST* typename_specifiers()
		{
			return Entries[0];
		}

		// Serialization

		inline
		char const* debug_str()
		{
			char const* fmt = txt(
				\nCode Debug:
				\nType    : %s
				\nReadonly: %s
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
			,	Readonly ? "true"       : "false"
			,	Parent   ? Parent->Name : ""
			,	Name     ? Name         : ""
			,	Comment  ? Comment      : ""
			);
		}

		inline
		char const* type_str()
		{
			return ECode::to_str( Type );
		}

		String to_string();
	#pragma endregion Member Functions

		constexpr static
		uw ArrS_Cap =
		( 	AST_POD_Size
			- sizeof(AST*)         // Parent
			- sizeof(StringCached) // Name
			- sizeof(CodeT) 	   // Type
			- sizeof(OperatorT)    // Op
			- sizeof(ModuleFlag)   // ModuleFlags
			- sizeof(u32) 		   // StaticIndex
			- sizeof(bool) * 2     // Readonly, DynamicEntries
			- sizeof(u8) * 6 )     // _Align_Pad
		/ sizeof(AST*);

		constexpr static
		uw ArrSpecs_Cap = ArrS_Cap * (sizeof(AST*) / sizeof(SpecifierT));

	#	define Using_Code_POD                          \
		union {                                        \
			AST*          ArrStatic[AST::ArrS_Cap];    \
			Array(AST*)   Entries;                     \
			StringCached  Content;                     \
			SpecifierT    ArrSpecs[AST::ArrSpecs_Cap]; \
		};                                             \
		AST*              Parent;                      \
		StringCached      Name;                        \
		CodeT             Type;                        \
		OperatorT         Op;                          \
		ModuleFlag        ModuleFlags;                 \
		u32               StaticIndex;                 \
		bool              Readonly;                    \
		bool              DynamicEntries;              \
		u8                _Align_Pad[6];

		Using_Code_POD
	};

	struct CodePOD
	{
		Using_Code_POD
	#	undef Using_CodePOD;
	};

	constexpr sw size_AST = sizeof(AST);
	constexpr sw size_POD = sizeof(CodePOD);

	// Its intended for the AST to have equivalent size to its POD.
	// All extra functionality within the AST namespace should just be syntatic sugar.
	static_assert( sizeof(AST)     == sizeof(CodePOD), "ERROR: AST IS NOT POD" );
	static_assert( sizeof(CodePOD) == AST_POD_Size,    "ERROR: AST POD is not size of AST_POD_Size" );

	/*
		AST* typedef as to not constantly have to add the '*' as this is written often..

		If GEN_ENFORCE_READONLY_AST is defined, readonly assertions will be done on any member dreference,
		and the 'gen API' related functions. will set their created ASTs to readonly before returning.

		Casting to AST* will bypass.
	*/
	struct Code
	{
	#pragma region Statics
		// Used to identify invalid generated code.
		static Code Invalid;
	#pragma endregion Statics

	#pragma region Member Functions
		inline
		Code body()
		{
			if ( ast == nullptr )
			{
				log_failure("Code::body: AST is null!");
				return Invalid;
			}

			if ( ast->Type == ECode::Invalid )
			{
				log_failure("Code::body: Type is invalid, cannot get");
				return Invalid;
			}

			if ( ast->Entries == nullptr || array_count(ast->Entries) == 0 )
			{
				log_failure("Code::body: Entries of ast not properly setup.");
				return Invalid;
			}

#		ifdef GEN_ENFORCE_READONLY_AST
			if ( ast->Readonly )
			{
				log_failure("Attempted to a body AST from a readonly AST!");
				return Invalid;
			}
#		endif

			return * (Code*)( ast->body() );
		}

		inline
		void lock()
		{
			ast->Readonly = true;
		}

		inline
		char const* to_string()
		{
			return ast->to_string();
		}

		inline
		operator bool()
		{
			return ast;
		}

		inline
		bool operator ==( Code other )
		{
			return ast == other.ast;
		}

		inline
		bool operator !=( Code other )
		{
			return ast != other.ast;
		}

		inline
		operator AST*()
		{
			return ast;
		}

		inline
		Code& operator=( Code other )
		{
			if ( ast == nullptr )
			{
				log_failure("Attempt to set with a null AST!");
				return *this;
			}

		#ifdef GEN_ENFORCE_READONLY_AST
			if ( ast->Readonly )
			{
				log_failure("Attempted to set a readonly AST!");
				return *this;
			}
		#endif

			ast = other.ast;

			return *this;
		}

		inline
		AST* operator->()
		{
			if ( ast == nullptr )
			{
				log_failure("Attempt to dereference a nullptr!");
				return nullptr;
			}

#		ifdef GEN_ENFORCE_READONLY_AST
			if ( ast->Readonly )
			{
				log_failure("Attempted to access a member from a readonly AST!");
				return nullptr;
			}
#		endif

			return ast;
		}
	#pragma endregion Member Functions

		AST* ast;
	};
	static_assert( sizeof(Code) == sizeof(AST*), "ERROR: Code is not POD" );

	// Used when the its desired when omission is allowed in a definition.
	constexpr Code NoCode = { nullptr };
	// extern const Code InvalidCode;

	/*
		Type Table: Used to store Typename ASTs. Types are registered by their string literal value.

		Provides interning specific to Typename ASTs.
		Interning for other types should be possible (specifiers) with this, so long as they
		don't have an set of child AST entries (Use the content field).
	*/
	ZPL_TABLE_DECLARE( ZPL_EXTERN, TypeTable, type_tbl_, Code );
#pragma endregion Data Structures

#pragma region Gen Interface
	// Initialize the library.
	// This currently just initializes the CodePool.
	void init();

	/*
		Use this only if you know you generated the code you needed to a file.
		And rather get rid of current code asts instead of growing the pool memory.
		This generally can be done everytime a file is generated
		TODO: In order for this to work, the type map needs its own arenas so do specifiers.
	*/
	void clear_code_memory();

	// Used internally to retrive or make string allocations.
	// Strings are stored in a series of string arenas of fixed size (SizePer_StringArena)
	StringCached get_cached_string( char const* cstr, s32 length );

	/*
		This provides a fresh Code AST.
		The gen interface use this as their method from getting a new AST object from the CodePool.
		Use this if you want to make your own API for formatting the supported Code Types.
	*/
	Code make_code();

	// This provides a fresh Code AST array for the entries field of the AST.
	// This is done separately from the regular CodePool allocator.
	Array(AST*) make_code_entries();

	// Set these before calling gen's init() procedure.
	// Data

	void set_allocator_data_arrays      ( AllocatorInfo data_array_allocator );
	void set_allocator_code_pool        ( AllocatorInfo pool_allocator );
	void set_allocator_code_enries_arena( AllocatorInfo pool_allocator );
	void set_allocator_string_arena     ( AllocatorInfo string_allocator );
	void set_allocator_string_table     ( AllocatorInfo string_allocator );
	void set_allocator_type_table       ( AllocatorInfo type_reg_allocator );

#	pragma region Upfront
	Code def_comment   ( s32 length, char const* content );
	Code def_attributes( s32 length, char const* content );

	Code def_class( s32 length, char const* name
		, Code body         = NoCode
		, Code parent       = NoCode, AccessSpec access     = AccessSpec::Public
		, Code specifiers   = NoCode, Code       attributes = NoCode
		, ModuleFlag mflags = ModuleFlag::None );

	Code def_enum( s32 length, char const* name
		, Code       body      = NoCode,      Code type       = NoCode
		, EnumT      specifier = EnumRegular, Code attributes = NoCode
		, ModuleFlag mflags    = ModuleFlag::None );

	Code def_execution  ( s32 length, char const* content );
	Code def_extern_link( s32 length, char const* name, Code body, ModuleFlag mflags = ModuleFlag::None );
	Code def_friend     ( Code symbol );

	Code def_function( s32 length, char const* name
		, Code       params     = NoCode, Code ret_type   = NoCode, Code body = NoCode
		, Code       specifiers = NoCode, Code attributes = NoCode
		, ModuleFlag mflags     = ModuleFlag::None );

	Code def_include  ( s32 length, char const* path );
	Code def_module   ( s32 length, char const* name,            ModuleFlag mflags = ModuleFlag::None );
	Code def_namespace( s32 length, char const* name, Code body, ModuleFlag mflags = ModuleFlag::None );

	Code def_operator( OperatorT op
		, Code       params     = NoCode, Code ret_type   = NoCode, Code body = NoCode
		, Code       specifiers = NoCode, Code attributes = NoCode
		, ModuleFlag mflags     = ModuleFlag::None );

	Code def_param     ( Code type, s32 length, char const* name, Code value = NoCode );
	Code def_specifier( SpecifierT specifier );

	Code def_struct( s32 length, char const* name
		, Code      body
		, Code      parent     = NoCode, AccessSpec access
		, Code      specifiers = NoCode, Code       attributes = NoCode
		, ModuleFlag mflags    = ModuleFlag::None );

	Code def_typedef( s32 length, char const* name, Code type, Code attributes = NoCode, ModuleFlag mflags = ModuleFlag::None );
	Code def_type   ( s32 length, char const* name, Code arrayexpr = NoCode, Code specifiers = NoCode );

	Code def_union( s32 length, char const* name, Code body = NoCode, Code attributes = NoCode, ModuleFlag mflags = ModuleFlag::None );

	Code def_using( s32 length, char const* name, UsingT specifier = UsingRegular
		, Code       type        = NoCode
		, Code       attributess = NoCode
		, ModuleFlag mflags      = ModuleFlag::None );

	Code def_variable( Code type, s32 length, char const* name, Code value = NoCode
		, Code       specifiers = NoCode, Code attributes = NoCode
		, ModuleFlag mflags     = ModuleFlag::None );

	Code def_class_body      ( s32 num, ... );
	Code def_enum_body       ( s32 num, ... );
	Code def_enum_body       ( s32 num, Code* codes );
	Code def_export_body     ( s32 num, ... );
	Code def_export_body     ( s32 num, Code* codes);
	Code def_extern_link_body( s32 num, ... );
	Code def_extern_link_body( s32 num, Code* codes );
	Code def_global_body     ( s32 num, ... );
	Code def_global_body     ( s32 num, Code* codes );
	Code def_function_body   ( s32 num, ... );
	Code def_function_body   ( s32 num, Code* codes );
	Code def_namespace_body  ( s32 num, ... );
	Code def_namespace_body  ( s32 num, Code* codes );
	Code def_params          ( s32 num, ... );
	Code def_params          ( s32 num, Code* params );
	Code def_specifiers      ( s32 num , ... );
	Code def_specifiers      ( s32 num, SpecifierT* specs );
	Code def_struct_body     ( s32 num, ... );
	Code def_struct_body     ( s32 num, Code* codes );
	Code def_union_body      ( s32 num, ... );
	Code def_union_body      ( s32 num, Code* codes );
#	pragma endregion Upfront

#	pragma region Incremental
#	ifdef GEN_FEATURE_INCREMENTAL
	Code make_class( s32 length, char const* name
		, Code       parent     = NoCode, AccessSpec access     = AccessSpec::Public
		, Code       specifiers = NoCode, Code       attributes = NoCode
		, ModuleFlag mflags     = ModuleFlag::None );

	Code make_enum( s32 length, char const* name
		, Code type       = NoCode, EnumT      specifier = EnumRegular
		, Code attributes = NoCode, ModuleFlag mflags    = ModuleFlag::None );

	Code make_export_body   ( s32 length = 1, char const* name = "" );
	Code make_extern_linkage( s32 length,     char const* name, ModuleFlag mflags = ModuleFlag::None );

	Code make_function( s32 length, char const* name
		, Code       params     = NoCode, Code ret_type   = NoCode
		, Code       specifiers = NoCode, Code attributes = NoCode
		, ModuleFlag mflags     = ModuleFlag::None );

	Code make_global_body( s32 length = 1, char const* name = "" );
	Code make_namespace  ( s32 length,     char const* name, ModuleFlag mflags = ModuleFlag::None );

	Code make_operator( OperatorT op
		, Code       params     = NoCode, Code ret_type   = NoCode
		, Code       specifiers = NoCode, Code attributes = NoCode
		, ModuleFlag mflags     = ModuleFlag::None );

	Code make_params    ();
	Code make_specifiers();

	Code make_struct( s32 length, char const* name
		, Code       parent     = NoCode, AccessSpec access
		, Code       specifiers = NoCode, Code       attributes = NoCode
		, ModuleFlag mflags     = ModuleFlag::None );

	Code make_union( s32 length, char const* name, Code attributes = NoCode, ModuleFlag mflags = ModuleFlag::None );
#	endif
#	pragma endregion Incremental

	#pragma region Parsing
	#ifdef GEN_FEATURE_PARSING
	Code parse_class      ( s32 length, char const* class_def     );
	Code parse_enum       ( s32 length, char const* enum_def      );
	Code parse_export_body( s32 length, char const* export_def    );
	Code parse_exten_link ( s32 length, char const* exten_link_def);
	Code parse_friend     ( s32 length, char const* friend_def    );
	Code parse_function   ( s32 length, char const* fn_def        );
	Code parse_global_body( s32 length, char const* body_def      );
	Code parse_namespace  ( s32 length, char const* namespace_def );
	Code parse_operator   ( s32 length, char const* operator_def  );
	Code parse_struct     ( s32 length, char const* struct_def    );
	Code parse_type       ( s32 length, char const* type_def      );
	Code parse_typedef    ( s32 length, char const* typedef_def   );
	Code parse_union      ( s32 length, char const* union_def     );
	Code parse_using      ( s32 length, char const* using_def     );
	Code parse_variable   ( s32 length, char const* var_def       );
	#endif
	#pragma endregion Parsing

	#pragma region Untyped text
	sw token_fmt_va( char* buf, uw buf_size, char const* fmt, s32 num_tokens, va_list va );

	inline
	char const* token_fmt( char const* fmt, sw num_tokens, ... )
	{
		local_persist thread_local
		char buf[ZPL_PRINTF_MAXLEN] = { 0 };

		va_list va;
		va_start(va, fmt);
		token_fmt_va(buf, ZPL_PRINTF_MAXLEN, fmt, num_tokens, va);
		va_end(va);

		return buf;
	}

	Code untyped_str      ( s32 length, char const* str);
	Code untyped_fmt      (             char const* fmt, ... );
	Code untyped_token_fmt(             char const* fmt, s32 num_tokens, ... );
	#pragma endregion Untyped text

	struct Builder
	{
		FileInfo File;
		String   Buffer;

		void print( Code );

		bool open( char const* path );
		void write();
	};

#if defined(GEN_FEATURE_EDITOR) || defined(GEN_FEATURE_SCANNER)
	struct SymbolInfo
	{
		StringCached File;
		char const*  Marker;
		Code         Signature;
	};
#endif

#ifdef GEN_FEATURE_EDITOR
	struct Policy
	{
		// Nothing for now.
	};

	enum class SymbolType : u32
	{
		Code,
		Line,
		Marker
	};

	struct Editor
	{
		enum RequestType : u32
		{
			Add,
			Replace,
			Remove
		};

		struct SymbolData
		{
			Policy      Policy;
			SymbolInfo  Info;
		};

		struct RequestEntry
		{
			union {
				SymbolData Symbol;
				String     Specification;
			};
			RequestType    Type;
		};

		struct Receipt
		{
			StringCached File;
			Code         Found;
			Code         Written;
			bool         Result;
		};

		static AllocatorInfo Allocator;

		static void set_allocator( AllocatorInfo  allocator );

		Array(FileInfo)     Files;
		String              Buffer;
		Array(RequestEntry) Requests;

		void add_files( s32 num, char const** files );

		void add    ( SymbolInfo definition,  Policy policy, Code to_inject );
		void remove ( SymbolInfo definition,  Policy policy );
		void replace( SymbolInfo definition,  Policy policy, Code to_replace);

#	ifdef GEN_FEATURE_EDITOR_REFACTOR
		void refactor( char const* file_path, char const* specification_path );
#	endif

		bool process_requests( Array(Receipt) out_receipts );
	};
#endif

#	ifdef GEN_FEATURE_SCANNER
	struct Scanner
	{
		struct RequestEntry
		{
			SymbolInfo Info;
		};

		struct Receipt
		{
			StringCached File;
			Code         Defintion;
			bool         Result;
		};

		AllocatorInfo MemAlloc;

		static void set_allocator( AllocatorInfo allocator );

		Array(FileInfo)     Files;
		String              Buffer;
		Array(RequestEntry) Requests;

		void add_files( s32 num, char const** files );

		void add( SymbolInfo signature, Policy policy );

		bool process_requests( Array(Receipt) out_receipts );
	};
#	endif
#pragma endregion Gen Interface
}

#pragma region Macros
#	define gen_main main

#	define __                     NoCode
#	define spec_alignas( Value_ ) ESpecifier::Alignas, Value

// This represents the naming convention for all typename Codes generated.
// Used by the DSL but can also be used without it.
#	define type_ns( Name_ ) t_##Name_

//	Convienence for defining any name used with the gen interface.
//  Lets you provide the length and string literal to the functions without the need for the DSL.
#	define name( Id_ )   txt_n_len( Id_ )

//  Same as name just used to indicate intention of literal for code instead of names.
#	define code( Code_ ) txt_n_len( Code_ )

/*
	gen's Domain Specific Langauge.

	Completely optional, makes the code gen syntax less verbose and cumbersome...
	Since its C macros ends up looking like a lisp dialect...

	Longforms auto-define the variable.
	Shorthands are just the function call.

	Anything below the make() macro is intended to be syntactically used in the follwing format:
	make( <type>, <name> )
	{
		...
	}

	Where ... are whatever is deemed necessary to produce the definition for the def( <name> ).

	The code macros are used to embed c/c++ to insert into the desired lcoation.
*/
#ifdef GEN_DEFINE_DSL

// Boilerplate

/*
	In order to provide some convient syntax sugar this polymoprhic macro boilerplate is needed for:
	* function( ... )
	* operator( ... )
	* params( ... )

	macrofn based off of: https://stackoverflow.com/questions/3046889/optional-parameters-with-c-macros
	specifically: https://stackoverflow.com/a/56038661
*/

#	define macrofn_chooser(_f0, _f1, _f2, _f3, _f4, _f5, _f6, _f7, _f8, _f9, _f10, _f11, _f12, _f13, _f14, _f15, _f16, ...) _f16
#	define macrofn_recomposer(ArgsWithParentheses_) macrofn_chooser ArgsWithParentheses_
#	define macrofn_chose_from_arg_num(F, ...)       macrofn_recomposer((__VA_ARGS__, F##_16, F##_15, F##_14, F##_13, F##_12, F##_11, F##_10, F##_9, F##_8, F##_7, F##_6, F##_5, F##_4, F##_3, F##_2, F##_1, ))
#	define marcofn_no_arg_expander(Func_)           ,,,,,,,,,,,,,,,,Func_ ## _0
#	define macrofn_finder(Func_, ...)               macrofn_chose_from_arg_num(Func_, marcofn_no_arg_expander __VA_ARGS__ (Func_))
#	define macrofn_polymorphic(Func_, ...)          macrofn_finder(Func_, __VA_ARGS__)(__VA_ARGS__)

#	define function_5( Name_, Params_, RetType_, Specifiers_, Body_ ) gen::def_function( txt_n_len( Name_ ), macro_expand( Params_ ), type_ns(RetType_), Specifiers_, Body_ )
#	define function_4( Name_, Params_, RetType_, Specifiers_ )        gen::def_function( txt_n_len( Name_ ), macro_expand( Params_ ), type_ns(RetType_), Specifiers_ )
#	define function_3( Name_, Params_, RetType_ )                     gen::def_function( txt_n_len( Name_ ), macro_expand( Params_ ), type_ns(RetType_) )
#	define function_2( Name_, Params_ )                               gen::def_function( txt_n_len( Name_ ), macro_expand( Params_ ) )
#	define function_1( Name_ )                                        gen::def_function( txt_n_len( Name_ ) )

#	define params_12( T_1, V_1, T_2, V_2, T_3, V_3, T_4, V_4, T_5, V_5, T_6, V_6 ) gen::def_params( 12, type_ns(T_1), txt_n_len( V_1), type_ns(T_2), txt_n_len( V_2), type_ns(T_3), txt_n_len( V_3), type_ns(T_4), txt_n_len( V_4), type_ns(T_5), txt_n_len( V_5), type_ns(T_6), txt_n_len(V_6))
#	define params_10( T_1, V_1, T_2, V_2, T_3, V_3, T_4, V_4, T_5, V_5 )           gen::def_params( 10, type_ns(T_1), txt_n_len( V_1), type_ns(T_2), txt_n_len( V_2), type_ns(T_3), txt_n_len( V_3), type_ns(T_4), txt_n_len( V_4), type_ns(T_5), txt_n_len( V_5))
#	define params_8(  T_1, V_1, T_2, V_2, T_3, V_3, T_4, V_4 )                     gen::def_params( 8, type_ns(T_1), txt_n_len( V_1), type_ns(T_2), txt_n_len( V_2), type_ns(T_3), txt_n_len( V_3), type_ns(T_4), txt_n_len( V_4) )
#	define params_6(  T_1, V_1, T_2, V_2, T_3, V_3 )                               gen::def_params( 6, type_ns(T_1), txt_n_len( V_1), type_ns(T_2), txt_n_len( V_2), type_ns(T_3), txt_n_len( V_3))
#	define params_4(  T_1, V_1, T_2, V_2 )                                         gen::def_params( 4, type_ns(T_1), txt_n_len( V_1), type_ns(T_2), txt_n_len( V_2))
#	define params_2(  T_1, V_1 )                                                   gen::def_param (    type_ns(T_1), txt_n_len( V_1))
#	define params_bad static_assert("params(...): Invalid number of parameters provided.")
#	define params_11  params_bad
#	define params_9   params_bad
#	define params_7   params_bad
#	define params_5   params_bad
#	define params_3   params_bad
#	define params_1   params_bad

// Upfront

#	define comment( Value_ )			  gen::def_comment( sizeof(Value), Value_ )
#	define attribute( Value_ )            gen::def_attribute( txt_n_len(Value_) )
#	define class( Name_, ... )            gen::def_class( txt_n_len(Name_), __VA_ARGS__ )
#	define enum( Name_, Type_, Body_ )    gen::def_enum ( txt_n_len(Name_), type_ns(Type_), Body_ )
#	define extern_linkage( Name_, Body_ ) gen::def_extern_linkage( txt_n_len(Name_), Body_ )
#	define function( ... )                macrofn_polymorphic( function, __VA_ARGS__ )
#	define namespace( Name_, Body_ )      gen::def_namespace      ( txt_n_len(Name_),  Body_ )
#	define operator( Op_, ... )           macrofn_polymorphic( operator, __VA_ARGS__ )
#	define params( ... )                  macrofn_polymorphic( params, __VA_ARGS__ )
#	define specifiers( ... )              gen::def_specifiers     ( macro_num_args( __VA_ARGS__ ), __VA_ARGS__ )
#	define struct( Name_, ... )           gen::def_struct         ( txt_n_len(Name_), __VA_ARGS__ )
#	define variable( Type_, Name_, ... )  gen::def_variable       ( type_ns(Type_), txt_n_len(Name_), __VA_ARGS__ )
#	define type( Value_, ... )            gen::def_type           ( txt_n_len(Value_), __VA_ARGS__ )
#	define type_fmt( Fmt_, ... )          gen::def_type           ( bprintf( Fmt_, __VA_ARGS__ ) )
#   define union( Name_, ... )            gen::def_union          ( txt_n_len(Name_), __VA_ARGS__ )
#	define using( Name_, ... )		  	  gen::def_using          ( txt_n_len(Name_), __VA_ARGS__ )

#	define class_body(          ... )     gen::def_class_body    ( macro_num_args( __VA_ARGS__ ), __VA_ARGS__ )
#	define enum_body(           ... )     gen::def_enum_body     ( macro_num_args( __VA_ARGS__ ), __VA_ARGS__ )
#	define extern_linkage_body( ... )     gen::def_extern_linkage_body( macro_num_args( __VA_ARGS__ ), __VA_ARGS__ )
#	define global_body(         ... )     gen::def_global_body   ( macro_num_args( __VA_ARGS__ ), __VA_ARGS__ )
#	define function_body(       ... )     gen::def_function_body ( macro_num_args( __VA_ARGS__ ), __VA_ARGS__ )
#	define namespace_body(      ... )     gen::def_namespace_body( macro_num_args( __VA_ARGS__ ), __VA_ARGS__ )
#	define operator_body(       ... )     gen::def_operator_body ( macro_num_args( __VA_ARGS__ ), __VA_ARGS__ )
#	define struct_body(         ... )     gen::def_struct_body   ( macro_num_args( __VA_ARGS__ ), __VA_ARGS__ )
#	define union_body(          ... )     gen::def_union_body    ( macro_num_args( __VA_ARGS__ ), __VA_ARGS__ )

#	ifdef GEN_FEATURE_INCREMENTAL
// Incremental

#   define make( ConstructType_, Name_, ... ) Code Name_ = make_##ConstructType_( txt_n_len(Name_), __VA_ARGS__ );
#	endif

#	ifdef GEN_FEATURE_PARSING
// Parsing

#	define class_code(       ... ) gen::parse_class      ( txt_n_len( __VA_ARGS__ ))
#	define enum_code(        ... ) gen::parse_enum       ( txt_n_len( __VA_ARGS__ ))
#	define function_code(    ... ) gen::parse_function   ( txt_n_len( __VA_ARGS__ ))
#	define global_body_code( ... ) gen::parse_global_body( txt_n_len( __VA_ARGS__ ))
#	define operator_code(    ... ) gen::parse_operator   ( txt_n_len( __VA_ARGS__ ))
#	define namespace_code(   ... ) gen::parse_namespace  ( txt_n_len( __VA_ARGS__ ))
#	define struct_code(      ... ) gen::parse_struct     ( txt_n_len( __VA_ARGS__ ))
#	define variable_code(    ... ) gen::parse_variable   ( txt_n_len( __VA_ARGS__ ))
#	define type_code(        ... ) gen::parse_type       ( txt_n_len( __VA_ARGS__ ))
#	define typedef_code(     ... ) gen::parse_typedef    ( txt_n_len( __VA_ARGS__ ))
#	define union_code(       ... ) gen::parse_union      ( txt_n_len( __VA_ARGS__ ))
#	define using_code(       ... ) gen::parse_code       ( txt_n_len( __VA_ARGS__ ))
#	endif

// Untyped

#   define code_str(   ... )       gen::untyped_str      ( txt_n_len(__VA_ARGS__) )
#	define code_fmt(   Fmt_, ... ) gen::untyped_fmt      ( Fmt_, __VA_ARGS__ )
#	define code_token( Fmt_, ... ) gen::untyped_token_fmt( Fmt_, macro_num_args( __VA_ARGS__) / 2, __VA_ARGS__ )
#endif
#pragma endregion Macros

#pragma region Constants
#ifdef GEN_DEFINE_LIBRARY_CODE_CONSTANTS
namespace gen
{
	// Predefined typename codes. Are set to readonly and are setup during gen::init()

	extern Code type_ns( s8 );
	extern Code type_ns( s16 );
	extern Code type_ns( s32 );
	extern Code type_ns( s64 );

	extern Code type_ns( u8 );
	extern Code type_ns( u16 );
	extern Code type_ns( u32 );
	extern Code type_ns( u64 );

	extern Code type_ns( sw );
	extern Code type_ns( uw );

	extern Code type_ns( f32 );
	extern Code type_ns( f64 );
}
#endif

namespace gen
{
	// These constexprs are used for allocation heavior of data structurs
	// or string handling while constructing or serializing.
	// Change them to suit your needs.

	constexpr s32 InitSize_DataArrays  = 16;
	constexpr s32 InitSize_StringTable = megabytes(4);
	constexpr s32 InitSize_TypeTable   = megabytes(4);

	constexpr s32 CodePool_NumBlocks        = 4096;
	constexpr s32 InitSize_CodeEntiresArray = 512;
	constexpr s32 SizePer_CodeEntriresArena = megabytes(16);
	constexpr s32 SizePer_StringArena       = megabytes(32);

	constexpr s32 MaxCommentLineLength      = 1024;
	constexpr s32 MaxNameLength             = 128;
	constexpr s32 MaxUntypedStrLength       = kilobytes(640);
	constexpr s32 StringTable_MaxHashLength = kilobytes(1);

	// Predefined Codes. Are set to readonly and are setup during gen::init()

	extern Code type_ns( void );
	extern Code type_ns( int );
	extern Code type_ns( bool );
	extern Code type_ns( char );
	extern Code type_ns( wchar_t );

	extern Code access_public;
	extern Code access_protected;
	extern Code access_private;

	extern Code spec_constexpr;
	extern Code spec_const;
	extern Code spec_inline;
	extern Code spec_ptr;
	extern Code spec_ref;
}
#pragma endregion Constants

#pragma region Inlines
namespace gen
{
	inline
	void AST::add_entry( AST* other )
	{
		AST* to_add = other->Parent ?
			other->duplicate() : other;

		if (DynamicEntries)
			array_append( Entries, to_add );

		else
		{
			if ( StaticIndex < ArrS_Cap )
			{
				ArrStatic[StaticIndex] = to_add;
				StaticIndex++;
			}
			else
			{
				Entries = make_code_entries();

				s32 index = 0;
				do
				{
					array_append( Entries, ArrStatic[index] );
				}
				while ( StaticIndex--, StaticIndex );

				array_append( Entries, to_add );
			}
		}

		to_add->Parent = this;
	}

	inline
	bool AST::add_param( AST* type, s32 length, char const* name )
	{
		if ( Type != ECode::Function )
		{
			log_failure( "gen::AST::add_param: this AST is not a function - %s", debug_str() );
			return Code::Invalid;
		}

		if ( length <= 0 )
		{
			log_failure( "gen::AST::add_param: Invalid name length provided - %d", length );
			return Code::Invalid;
		}

		if ( name == nullptr )
		{
			log_failure( "gen::AST::add_param: name is null");
			return Code::Invalid;
		}

		s32
		score  = 0;
		score += Name       == nullptr;
		score += Entries[0] == nullptr;

		if ( score == 1 )
		{
			log_failure("gen::AST::add_param: this AST has bad data - %s", debug_str() );
			return false;
		}
		else if ( score == 2)
		{
			Name       = name;
			Entries[0] = type;
			return true;
		}

		Code
		result = make_code();
		result->Type = ECode::Parameters;

		result->add_entry( result );
		return true;
	}
}
#pragma endregion Inlines

// end: gen_time
#endif
