/*
	gencpp: An attempt at simple staged metaprogramming for c/c++.

	See Readme.md for more information from the project repository.

	Public Address:
	https://github.com/Ed94/gencpp
*/
#pragma once

#include "Bloat.hpp"

// Temporarily here for debugging purposes.
#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
// #define GEN_DONT_USE_FATAL

// #define GEN_FEATURE_PARSING
// #define GEN_FEATURE_EDITOR
// #define GEN_FEATURE_SCANNER


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
			#	define Entry( Type ) { txt_n_len( Type ) },
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
		Entry( Invalid,          INVALID )           \
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
		};

		// Specifier to string
		inline
		StrC to_str( Type specifier )
		{
			local_persist
			StrC lookup[ Num_Specifiers ] = {
			#	define Entry( Spec_, Code_ ) { txt_n_len(Code_) },
				Define_Specifiers
			#	undef Entry
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

					keymap[index] = crc32( enum_str.Ptr, enum_str.Len);
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

		#undef Define_Specifiers
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
	using StringCached = String const;

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
		void add_entry( AST* other );

		inline
		AST* body()
		{
			return entry( 0 );
		}

		AST* duplicate();

		AST*& entry( u32 idx )
		{
			return DynamicEntries ? ArrDyn[ idx ] : ArrStatic[ idx ];
		}

		inline
		bool has_entries()
		{
			return num_entries();
		}

		inline
		bool is_invalid()
		{
			return Type != ECode::Invalid;
		}

		inline
		bool is_equal( AST* other );

		inline
		s32 num_entries()
		{
			return DynamicEntries ? array_count( ArrDyn ) : StaticIndex;
		}

		// Parameter

		inline
		AST* get_param( s32 index )
		{
			if ( index <= 0 )
				return this;

			return entry( index + 1 );
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
			return entry( 0 );
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
			return false;
		}

		inline
		bool typename_is_ref()
		{
			assert_crash("not implemented");
			return false;
		}

		inline
		AST* typename_specifiers()
		{
			return entry( 0 );
		}

		// Serialization

		inline
		char const* debug_str()
		{
			char const* fmt = txt(
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
			- sizeof(AccessSpec)   // ParentAccess
			- sizeof(u32) 		   // StaticIndex
			- sizeof(bool) * 1     // DynamicEntries
			- sizeof(u8) * 2 )     // _Align_Pad
		/ sizeof(AST*);

		constexpr static
		uw ArrSpecs_Cap = ArrS_Cap * (sizeof(AST*) / sizeof(SpecifierT));

	#	define Using_AST_POD                           \
		union {                                        \
			AST*          ArrStatic[AST::ArrS_Cap];    \
			Array(AST*)   ArrDyn;                      \
			StringCached  Content;                     \
			SpecifierT    ArrSpecs[AST::ArrSpecs_Cap]; \
		};                                             \
		AST*              Parent;                      \
		StringCached      Name;                        \
		CodeT             Type;                        \
		OperatorT         Op;                          \
		ModuleFlag        ModuleFlags;                 \
		AccessSpec        ParentAccess;				   \
		u32               StaticIndex;                 \
		bool              DynamicEntries;              \
		u8                _Align_Pad[3];

		Using_AST_POD
	};

	struct AST_POD
	{
		Using_AST_POD
	#	undef Using_CodePOD
	};

	constexpr sw size_AST = sizeof(AST);
	constexpr sw size_POD = sizeof(AST_POD);

	// Its intended for the AST to have equivalent size to its POD.
	// All extra functionality within the AST namespace should just be syntatic sugar.
	static_assert( sizeof(AST)     == sizeof(AST_POD), "ERROR: AST IS NOT POD" );
	static_assert( sizeof(AST_POD) == AST_POD_Size,    "ERROR: AST POD is not size of AST_POD_Size" );

	/*
		AST* typedef as to not constantly have to add the '*' as this is written often..

		If GEN_ENFORCE_READONLY_AST is defined, readonly assertions will be done on any member dreference,
		and the 'gen API' related functions. will set their created ASTs to readonly before returning.

		Casting to AST* will bypass.
	*/
	struct Code
	{
	#pragma region Statics
		// Used to identify ASTs that should always be duplicated. (Global constant ASTs)
		static Code Global;

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

		#ifdef GEN_ENFORCE_READONLY_AST
			if ( ast->Readonly )
			{
				log_failure("Attempted to a body AST from a readonly AST!");
				return Invalid;
			}
		#endif

			return * (Code*)( ast->body() );
		}

		inline
		String to_string() const
		{
			return ast->to_string();
		}

		inline
		void set_global()
		{
			if ( ast == nullptr )
			{
				log_failure("Code::set_global: Cannot set code as global, AST is null!");
				return;
			}

			ast->Parent = Global.ast;
		}

		inline
		operator bool() const
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
			if ( other.ast == nullptr )
			{
				log_failure("Attempted to assign a nullptr!");
				return *this;
			}

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

			return ast;
		}
	#pragma endregion Member Functions

		AST* ast;
	};

	struct Code_POD
	{
		AST_POD* ast;
	};

	static_assert( sizeof(Code) == sizeof(Code_POD), "ERROR: Code is not POD" );

	// Used when the its desired when omission is allowed in a definition.
	constexpr Code NoCode = { nullptr };
#pragma endregion Data Structures

#pragma region Gen Interface
	// Initialize the library.
	// This currently just initializes the CodePool.
	void init();

	// Currently manually free's the arenas, code for checking for leaks.
	// However on Windows at least, it doesn't need to occur as the OS will clean up after the process.
	void deinit();

	/*
		Use this only if you know you generated the code you needed to a file.
		And rather get rid of current code asts instead of growing the pool memory.
		TODO: Need to put permanent ASTs into a separate set of memory. (I might just remove this tbh as it might be useless)
	*/
	void clear_code_memory();

	// Used internally to retrive or make string allocations.
	// Strings are stored in a series of string arenas of fixed size (SizePer_StringArena)
	StringCached get_cached_string( StrC str );

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
	Code def_attributes( StrC content );
	Code def_comment   ( StrC content );

	Code def_class( StrC name
		, Code body         = NoCode
		, Code parent       = NoCode, AccessSpec access = AccessSpec::Default
		, Code attributes   = NoCode
		, ModuleFlag mflags = ModuleFlag::None );

	Code def_enum( StrC
		, Code       body      = NoCode,      Code type       = NoCode
		, EnumT      specifier = EnumRegular, Code attributes = NoCode
		, ModuleFlag mflags    = ModuleFlag::None );

	Code def_execution  ( StrC content );
	Code def_extern_link( StrC name, Code body, ModuleFlag mflags = ModuleFlag::None );
	Code def_friend     ( Code symbol );

	Code def_function( StrC name
		, Code       params     = NoCode, Code ret_type   = NoCode, Code body = NoCode
		, Code       specifiers = NoCode, Code attributes = NoCode
		, ModuleFlag mflags     = ModuleFlag::None );

	Code def_include  ( StrC content );
	Code def_module   ( StrC name,            ModuleFlag mflags = ModuleFlag::None );
	Code def_namespace( StrC name, Code body, ModuleFlag mflags = ModuleFlag::None );

	Code def_operator( OperatorT op
		, Code       params     = NoCode, Code ret_type   = NoCode, Code body = NoCode
		, Code       specifiers = NoCode, Code attributes = NoCode
		, ModuleFlag mflags     = ModuleFlag::None );

	Code def_param     ( Code type, StrC name, Code value = NoCode );
	Code def_specifier( SpecifierT specifier );

	Code def_struct( StrC name
		, Code       body
		, Code       parent     = NoCode, AccessSpec access = AccessSpec::Default
		, Code       attributes = NoCode
		, ModuleFlag mflags     = ModuleFlag::None );

	Code def_type   ( StrC name, Code arrayexpr = NoCode, Code specifiers = NoCode );
	Code def_typedef( StrC name, Code type, Code attributes = NoCode, ModuleFlag mflags = ModuleFlag::None );

	Code def_union( StrC name, Code body, Code attributes = NoCode, ModuleFlag mflags = ModuleFlag::None );

	Code def_using( StrC name, Code type = NoCode
		, Code       attributess = NoCode
		, ModuleFlag mflags      = ModuleFlag::None );

	Code def_using_namespace( StrC name );

	Code def_variable( Code type, StrC name, Code value = NoCode
		, Code       specifiers = NoCode, Code attributes = NoCode
		, ModuleFlag mflags     = ModuleFlag::None );

	Code def_class_body      ( s32 num, ... );
	Code def_class_body      ( s32 num, Code* codes );
	Code def_enum_body       ( s32 num, ... );
	Code def_enum_body       ( s32 num, Code* codes );
	Code def_export_body     ( s32 num, ... );
	Code def_export_body     ( s32 num, Code* codes);
	Code def_extern_link_body( s32 num, ... );
	Code def_extern_link_body( s32 num, Code* codes );
	Code def_function_body   ( s32 num, ... );
	Code def_function_body   ( s32 num, Code* codes );
	Code def_global_body     ( s32 num, ... );
	Code def_global_body     ( s32 num, Code* codes );
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

	#pragma region Parsing
	#ifdef GEN_FEATURE_PARSING
	Code parse_class      ( StrC class_def     );
	Code parse_enum       ( StrC enum_def      );
	Code parse_export_body( StrC export_def    );
	Code parse_exten_link ( StrC exten_link_def);
	Code parse_friend     ( StrC friend_def    );
	Code parse_function   ( StrC fn_def        );
	Code parse_global_body( StrC body_def      );
	Code parse_namespace  ( StrC namespace_def );
	Code parse_operator   ( StrC operator_def  );
	Code parse_struct     ( StrC struct_def    );
	Code parse_type       ( StrC type_def      );
	Code parse_typedef    ( StrC typedef_def   );
	Code parse_union      ( StrC union_def     );
	Code parse_using      ( StrC using_def     );
	Code parse_variable   ( StrC var_def       );
	#endif
	#pragma endregion Parsing

	#pragma region Untyped text
	sw token_fmt_va( char* buf, uw buf_size, char const* fmt, s32 num_tokens, va_list va );

	inline
	StrC token_fmt( char const* fmt, sw num_tokens, ... )
	{
		local_persist thread_local
		char buf[ZPL_PRINTF_MAXLEN] = { 0 };
		mem_set( buf, 0, ZPL_PRINTF_MAXLEN );

		va_list va;
		va_start(va, fmt);
		sw result = token_fmt_va(buf, ZPL_PRINTF_MAXLEN, fmt, num_tokens, va);
		va_end(va);

		return { result, buf };
	}

	Code untyped_str      ( StrC content);
	Code untyped_fmt      ( char const* fmt, ... );
	Code untyped_token_fmt( char const* fmt, s32 num_tokens, ... );
	#pragma endregion Untyped text

	struct Builder
	{
		FileInfo File;
		String   Buffer;

		void print( Code );
		void print_fmt( char const* fmt, ... );

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

#	define __ NoCode

// This represents the naming convention for all typename Codes generated.
#	define type_ns( Name_ ) t_##Name_

//	Convienence for defining any name used with the gen api.
//  Lets you provide the length and string literal to the functions without the need for the DSL.
#	define name( Id_ )   { txt_n_len( Id_ ) }

//  Same as name just used to indicate intention of literal for code instead of names.
#	define code( ... ) { txt_n_len( __VA_ARGS__ ) }
#pragma endregion Macros

#pragma region Constants
#ifdef GEN_DEFINE_LIBRARY_CODE_CONSTANTS
namespace gen
{
	// Predefined typename codes. Are set to readonly and are setup during gen::init()

	extern Code type_ns( b32 );

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

	extern Code type_ns( auto );
	extern Code type_ns( void );
	extern Code type_ns( int );
	extern Code type_ns( bool );
	extern Code type_ns( char );
	extern Code type_ns( wchar_t );

	extern Code access_public;
	extern Code access_protected;
	extern Code access_private;

	extern Code module_global_fragment;
	extern Code module_private_fragment;

	extern Code pragma_once;

	extern Code spec_const;
	extern Code spec_consteval;
	extern Code spec_constexpr;
	extern Code spec_constinit;
	extern Code spec_extern_linkage;
	extern Code spec_inline;
	extern Code spec_internal_linkage;
	extern Code spec_local_persist;
	extern Code spec_mutable;
	extern Code spec_ptr;
	extern Code spec_ref;
	extern Code spec_register;
	extern Code spec_rvalue;
	extern Code spec_static_member;
	extern Code spec_thread_local;
	extern Code spec_volatile;
	extern Code spec_type_signed;
	extern Code spec_type_unsigned;
	extern Code spec_type_short;
	extern Code spec_type_long;
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
			array_append( ArrDyn, to_add );

		else
		{
			if ( StaticIndex < ArrS_Cap )
			{
				ArrStatic[StaticIndex] = to_add;
				StaticIndex++;
			}
			else
			{
				ArrDyn = make_code_entries();

				s32 index = 0;
				do
				{
					array_append( ArrDyn, ArrStatic[index] );
				}
				while ( StaticIndex--, StaticIndex );

				array_append( ArrDyn, to_add );
			}
		}

		to_add->Parent = this;
	}
}
#pragma endregion Inlines

// end: gen_time
#endif
