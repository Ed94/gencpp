/*
	gencpp: A simple staged metaprogramming library for C++.

	The library is mostly a compositon of code element constructors.
	These build up a code AST to then serialize with a file builder.

	This library is intended for small-to midsize projects.

	AST type checking supports only a small subset of c++.
	See the 'ECode' namespace and 'gen API' region to see what is supported.

	### *WHAT IS NOT PROVIDED*

	* Macro or template generation      : This library is to avoid those, adding support for them adds unnecessary complexity.
	                                      If you desire define them outside the gen_time scopes.
	* Expression validation             : Execution expressions are defined using the untyped string API.
	                                      There is no parse API for validating expression (possibly will add in the future)
	* Modern C++ (STL library) features
	* Modern C++ RTTI                   : This is kinda covered with the last point, but just wanted to emphasize.

	Exceptions brought in from "Modern C++":
		Specifiers:
		* consteval
		* constinit
		* explicit
		* export
		* noexcept
		* import
		* final
		* module
		* override
		* &&
		* virtual

	These features are in as they are just specifiers and aren't hard to implement seralization or validation.

	The AST is managed by the library and provided the user via its interface prodedures.

	Notes:

	* The allocator definitions used are exposed to the user incase they want to dictate memory usage
	* ASTs are wrapped for the user in a Code struct which essentially a warpper for a AST* type.
	* Both AST and Code have member symbols but their data layout is enforced to be POD types.

	Data layout of AST struct:

	CodeT             Type;
	bool              Readonly;
	AST*              Parent;
	string            Name;
	string            Comment;
	union {
		array(AST*)   Entries;
		string        Content;
	};

	*`CodeT` is a typedef for `ECode::Type` which is the type of the enum.*

	ASTs can be set to readonly by calling Code's lock() member function.
	Adding comments is always available even if the AST is set to readonly.

	### There are four sets of interfaces for Code AST generation the library provides

	* Upfront
	* Incremental
	* Parsing
	* Untyped

	### Upfront Construction

	All component ASTs must be previously constructed, and provided on creation of the code AST.
	The construction will fail and return InvalidCode otherwise.

	Interface :

	* def_class
	* def_enum
	* def_enum_class
	* def_friend
	* def_function
	* def_namespace
	* def_operator
	* def_param
	* def_params
	* def_specifier
	* def_specifiers
	* def_struct
	* def_variable
	* def_type
	* def_typedef
	* def_using
	* def_using_namespace

	* def_class_body
	* def_enum_body
	* def_function_body
	* def_global_body
	* def_operator_body
	* def_namespace_body
	* def_struct_body

	### Incremental construction

	A Code ast is provided but only completed upfront if all components are provided.
	Components are then added using the AST API for adding ASTs:

	* code.add( AST* )                     // Adds AST with validation.
	* code.add_entry( AST* )               // Adds AST entry without validation.

	Code ASTs may be explictly validated at anytime using Code's check() member function.

	Interface :

	* make_class
	* make_enum
	* make_enum_class
	* make_function
	* make_global_body
	* make_namespace
	* make_operator
	* make_params
	* make_specifiers
	* make_struct

	### Parse construction

	A string provided to the API is parsed for the intended language construct.

	Interface :

	* parse_class
	* parse_enum
	* parse_friend
	* parse_function
	* parse_global_body
	* parse_namespace
	* parse_operator
	* parse_struct
	* parse_strucs
	* parse_variable
	* parse_type
	* parse_typedef
	* parse_using

	* parse_classes
	* parse_enums
	* parse_functions
	* parse_namespaces
	* parse_operators
	* parse_variables
	* parse_typedefs
	* parse_usings

	The parse API treats any execution scope definitions with no validation and are turned into untyped Code ASTs.
	This includes the assignmetn of variables; due to the library not yet supporting c/c++ expression parsing.

	The plural variants provide an array of codes, its up to the user to add them to a body AST
	(they are not auto-added to a body)

	### Untyped constructions

	Code ASTs are constructed using unvalidated strings.

	Interface :

	* untyped_str
	* untyped_fmt
	* untyped_token_fmt

	During serialization any untyped Code AST is has its string value directly injected inline of
	whatever context the content existed as an entry within.
	Even though thesee are not validated from somewhat correct c/c++ syntax or components, it doesn't mean that
	Untyped code can be added as any component of a Code AST:

	* Untyped code cannot have children, thus there cannot be recursive injection this way.
	* Untyped code can only be a child of a parent of body AST, or for values of an assignment.

	These restrictions help prevent abuse of untyped code to some extent.
*/

#pragma once

#include "Bloat.hpp"

// Temporarily here for debugging purposes.
#define gen_time


#define GEN_BAN_CPP_TEMPLATES
#define GEN_ENFORCE_READONLY_AST
#define GEN_DEFINE_DSL
#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_USE_FATAL
#define GEN_FEATURE_EDITOR
#define GEN_FEATURE_SCANNER


#ifdef gen_time
namespace gen
{
	using LogFailType = sw(*)(char const*, ...);

#	ifdef GEN_BAN_CPP_TEMPLATES
#		define template static_assert("Templates are banned within gen_time scope blocks")
#	endif

#	ifdef GEN_USE_FATAL
	ct LogFailType log_failure = fatal;
#	else
	ct LogFailType log_failure = log_fmt;
#	endif

	namespace ECode
	{
#		define Define_Types       \
		Entry( Untyped )          \
		Entry( Access_Public )    \
		Entry( Access_Private )   \
		Entry( Access_Protected ) \
		Entry( Class )            \
		Entry( Class_FwdDecl )    \
		Entry( Class_Body )       \
		Entry( Enum )             \
		Entry( Enum_FwdDecl )     \
		Entry( Enum_Body )        \
		Entry( Friend )           \
		Entry( Global_Body )      \
		Entry( Namespace )        \
		Entry( Namespace_Body )   \
		Entry( Parameters )       \
		Entry( Function )         \
		Entry( Function_FwdDecl ) \
		Entry( Function_Body )    \
		Entry( Specifiers )       \
		Entry( Struct )           \
		Entry( Struct_FwdDecl )   \
		Entry( Struct_Body )      \
		Entry( Variable )         \
		Entry( Typedef )          \
		Entry( Typename )         \
		Entry( Using )

		enum Type : u32
		{
#			define Entry( Type ) Type,
			Define_Types
#			undef Entry

			Num_Types,
			Invalid
		};

		inline
		char const* str( Type type )
		{
			static
			char const* lookup[Num_Types] = {
#				define Entry( Type ) txt( Type ),
				Define_Types
#				undef Entry
			};

			return lookup[ type ];
		}

#		undef Define_Types
	}
	using CodeT = ECode::Type;

	namespace EOperator
	{
#		define Define_Operators          \
			Entry( Assign, =  )          \
			Entry( Assign_Add, += )      \
			Entry( Assign_Subtract, -= ) \
			Entry( Assgin_Multiply, *= ) \
			Entry( Assgin_Divide, /= )   \
			Entry( Assgin_Modulo, %= )   \
			Entry( Assgin_BAnd, &= )     \
			Entry( Assgin_BOr, &= )      \
			Entry( Assign_BXOr, ^= )     \
			Entry( Assign_LShift, <<= )  \
			Entry( Assign_RShift, >>= )  \
			Entry( Increment, ++ )       \
			Entry( Decrement, -- )       \
			Entry( Unary_Plus, + )       \
			Entry( Unary_Minus, - )      \
			Entry( Add, + )              \
			Entry( Subtract, - )         \
			Entry( Multiply, * )         \
			Entry( Divide, / )           \
			Entry( Modulo, % )           \
			Entry( BNot, ~ )             \
			Entry( BAnd, & )             \
			Entry( BOr, | )              \
			Entry( BXOr, ^ )             \
			Entry( LShift, << )          \
			Entry( RShift, >> )          \
			Entry( LNot, ! )             \
			Entry( LAnd, && )            \
			Entry( LOr, || )             \
			Entry( Equals, == )          \
			Entry( NotEquals, != )       \
			Entry( Lesser, < )           \
			Entry( Greater, > )          \
			Entry( LesserEqual, <= )     \
			Entry( GreaterEqual, >= )    \
			Entry( Subscript, [] )       \
			Entry( Indirection, * )      \
			Entry( AddressOf, & )        \
			Entry( MemberOfPointer, -> ) \
			Entry( PtrToMemOfPtr, ->* )  \
			Entry( FunctionCall, () )

		enum Type : u32
		{
#			define Entry( Type, Token ) Type,
			Define_Operators
#			undef Entry
			Comma,

			Num_Ops,
			Invalid
		};

		inline
		char const* str( Type op )
		{
			local_persist
			char const* lookup[ Num_Ops ] = {
#				define Entry( Type, Token ) txt(Token),
				Define_Operators
#				undef Entry
				","
			};

			return lookup[ op ];
		}
#		undef Define_Operators
	}
	using OperatorT = EOperator::Type;

	namespace ESpecifier
	{
#		if defined(ZPL_SYSTEM_WINDOWS)
#			define API_Export_Code __declspec(dllexport)
#			define API_Import_Code __declspec(dllimport)
#		elif defined(ZPL_SYSTEM_MACOS)
#			define API_Export_Code __attribute__ ((visibility ("default")))
#			define API_Import_Code __attribute__ ((visibility ("default")))
#		endif

#		if defined(ZPL_MODULE_THREADING)
#			define Thread_Local_Code thread_local
#		else
#			define Thread_Local_Code "NOT DEFINED"
#		endif

		// Entry( Explicit, explicit )           \

		#define Define_Specifiers                \
		Entry( API_Import, API_Export_Code )     \
		Entry( API_Export, API_Import_Code )     \
		Entry( Attribute, "You cannot stringize an attribute this way" ) \
		Entry( Alignas, alignas )                \
		Entry( Const, const )                    \
		Entry( C_Linkage, extern "C" )           \
		Entry( Consteval, consteval )            \
		Entry( Constexpr, constexpr )            \
		Entry( Constinit, constinit )            \
		Entry( Export, export )                  \
		Entry( External_Linkage, extern )        \
		Entry( Import, import )                  \
		Entry( Inline, inline )                  \
		Entry( Internal_Linkage, static )        \
		Entry( Final, final )                    \
		Entry( Local_Persist, static )           \
		Entry( Module, module )                  \
		Entry( Mutable, mutable )                \
		Entry( NoExcept, noexcept )              \
		Entry( Override, override )              \
		Entry( Pointer, * )                      \
		Entry( Reference, & )                    \
		Entry( Register, register )              \
		Entry( RValue, && )                      \
		Entry( Static_Member, static  )          \
		Entry( Thread_Local, Thread_Local_Code ) \
		Entry( Virtual, virtual )                \
		Entry( Volatile, volatile )

		enum Type : u32
		{
#			define Entry( Specifier, Code ) Specifier,
			Define_Specifiers
#			undef Entry

			Num_Specifiers,
			Invalid,
		};

		// Specifier to string
		inline
		char const* to_str( Type specifier )
		{
			local_persist
			char const* lookup[ Num_Specifiers ] = {
#				define Entry( Spec_, Code_ ) txt(Code_),
				Define_Specifiers
#				undef Entry
			};

			return lookup[ specifier ];
		}

		Type to_type( char const* str, s32 length )
		{
			local_persist
			u32 keymap[ Num_Specifiers ];
			do_once_start
				for ( u32 index = 0; index < Num_Specifiers; index++ )
				{
					char const* enum_str = to_str( (Type)index );

					keymap[index] = crc32( enum_str, zpl_strnlen(enum_str, 42) );
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
	}
	using SpecifierT = ESpecifier::Type;

#pragma region Data Structures
	// TODO: If perf needs it, convert layout an SOA format.
	/*
		Simple AST POD with functionality to seralize into C++ syntax.

		ASTs are currently stored as an AOS. They are always reconstructed on demand.
		Thus redundant AST can easily occur.
		Not sure if its better to store them in a hashmap.
	*/
	struct AST
	{
	#pragma region Member Procedures
		bool add( AST* other );

		forceinline
		void add_entry( AST* other )
		{
			Code to_add = other->Parent ?
				other->duplicate() : other;

			array_append( Entries, to_add );

			to_add->Parent = this;
		}
		forceinline
		AST* body()
		{
			return Entries[0];
		}

		forceinline
		bool check();

		Code duplicate();

		forceinline
		bool has_entries() const
		{
			static bool lookup[ ECode::Num_Types] = {
				false, // Invalid
				false, // Untyped
				false,
				false,
				false,
				true,  // Global_Body
				true,  // Parameters
				true,  // Proc
				true,  // Proc_Body
				true,  // Proc_Forward
				false, // Specifies
				true,  // Struct
				true,  // Struct_Body
				true,  // Variable
				true,  // Typedef
				true,  // Typename
				true,  // Using
			};

			return lookup[Type];
		}

		forceinline
		bool is_invalid() const
		{
			return Type != ECode::Invalid;
		}

		forceinline
		char const* debug_str() const
		{
			char const* fmt = txt(
				\nCode Debug:
				\nType    : %s
				\nReadonly: %s
				\nParent  : %s
				\nName    : %s
				\nComment : %s
			);

			bprintf( fmt
			,	type_str()
			,	Readonly ? "true"       : "false"
			,	Parent   ? Parent->Name : ""
			,	Name     ? Name         : ""
			,	Comment  ? Comment      : ""
			);
		}

		forceinline
		char const* type_str() const
		{
			return ECode::str( Type );
		}

		string to_string() const;
	#pragma endregion Member Procedures

	#define Using_Code_POD               \
		AST*              Parent;        \
		ro_string         Name;          \
		ro_string         Comment;       \
		union {                          \
			array(AST*)   Entries;       \
			ro_string     Content;       \
		};                               \
		CodeT             Type;          \
		OperatorT         Op;            \
		bool              Readonly;      \
		u8                _64_Align[23];

		Using_Code_POD;
	};

	struct CodePOD
	{
		Using_Code_POD;
	};

	ct u32 sizeof_AST = sizeof(AST);
	ct u32 sizeof_CODE = sizeof(CodePOD);

	// Its intended for the AST to have equivalent size to its POD.
	// All extra functionality within the AST namespace should just be syntatic sugar.
	static_assert( sizeof(AST) == sizeof(CodePOD), "ERROR: AST IS NOT POD" );

	/*
		AST* typedef as to not constantly have to add the '*' as this is written often..

		If GEN_ENFORCE_READONLY_AST is defined, readonly assertions will be done on any member dreference,
		and the 'gen API' related functions. will set their created ASTs to readonly before returning.

		Casting to AST* will bypass.
	*/
	struct Code
	{
	#pragma region Member Procedures
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

		forceinline
		void lock()
		{
			ast->Readonly = true;
		}

		forceinline
		operator bool() const
		{
			return ast && ast->is_invalid();
		}

		bool operator ==( Code other ) const
		{
			return ast == other.ast;
		}

		operator AST*()
		{
			return ast;
		}

		Code& operator =( Code other )
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

		forceinline
		AST* operator ->()
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
	#pragma endregion Member Procedures

		// Used internally for the most part to identify invaidly generated code.
		static const Code Invalid;

		// Data
		AST* ast;
	};
	static_assert( sizeof(Code) == sizeof(AST*), "ERROR: Code is not POD" );

	// Used when the its desired when omission is allowed in a definition.
	ct Code NoCode = { nullptr };
	// extern const Code InvalidCode;

	/*
	*/
	ZPL_TABLE_DECLARE( ZPL_EXTERN, StringTable, str_tbl_, string );

	using ro_string = char const*;

	/*
		Type registy: Used to store Typename ASTs. Types are registered by their string literal value.

		Purely used as a memory optimization.
		Strings made with the Typename ASTs are stored in thier own arena allocator.
		TODO: Implement and replace usage of def_type.
	*/
	ZPL_TABLE_DECLARE( ZPL_EXTERN, TypeTable, type_tbl_, Code );
#pragma endregion Data Structures

#pragma region Gen Interface
	/*
		Initialize the library.
		This currently just initializes the CodePool.
	*/
	void init();

	// Use this only if you know you generated the code you needed to a file.
	// And rather get rid of current code asts instead of growing the pool memory.
	void clear_code_pool();

	/*
		Used internally to retrive or make string allocations.
		Strings are stored in a series of string arenas of fixed size (SizePer_StringArena)
	*/
	ro_string cached_string( char const* cstr, s32 length );

	/*
		This provides a fresh Code AST struct.
		The gen interface use this as their method from getting a new AST object from the CodePool.
		Use this if you want to make your own API for formatting the supported Code Types.
	*/
	Code make_code();

	// Set these before calling gen's init() procedure.

	void set_init_reserve_code_pool   ( sw size );
	void set_init_reserve_string_arena( sw size );
	void set_init_reserve_string_table( sw size );
	void set_init_reserve_type_table  ( sw size );

	void set_allocator_code_pool   ( allocator pool_allocator );
	void set_allocator_string_arena( allocator string_allocator );
	void set_allocator_string_table( allocator string_allocator );
	void set_allocator_type_table  ( allocator type_reg_allocator );

#	pragma region Upfront
	Code def_class          (             char const* name, Code parent = NoCode,                         Code specifiers = NoCode, Code body = NoCode );
	Code def_class          ( s32 length, char const* name, Code parent = NoCode,                         Code specifiers = NoCode, Code body = NoCode );
	Code def_enum           (             char const* name, Code type   = NoCode,                                                   Code body = NoCode);
	Code def_enum           ( s32 length, char const* name, Code type   = NoCode,                                                   Code body = NoCode );
	Code def_friend         ( Code symbol );
	Code def_function       (             char const* name, Code params = NoCode, Code ret_type = NoCode, Code specifiers = NoCode, Code body = NoCode );
	Code def_function       ( s32 length, char const* name, Code params = NoCode, Code ret_type = NoCode, Code specifiers = NoCode, Code body = NoCode );
	Code def_namespace      (             char const* name,                                                                         Code body );
	Code def_namespace      ( s32 length, char const* name,                                                                         Code body );
	Code def_operator       (             OperatorT   op,   Code params = NoCode, Code ret_type = NoCode, Code specifiers = NoCode, Code body = NoCode );

	Code def_param          ( Code type,             char const* name );
	Code def_param          ( Code type, s32 length, char const* name );

	Code def_specifier      ( SpecifierT specifier );

	Code def_struct         (             char const* name, Code parent     = NoCode, Code specifiers = NoCode, Code body = NoCode );
	Code def_struct         ( s32 length, char const* name, Code parent     = NoCode, Code specifiers = NoCode, Code body = NoCode );
	Code def_type           (             char const* name, Code specifiers = NoCode );
	Code def_type           ( s32 length, char const* name, Code specifiers = NoCode );
	Code def_using          (             char const* name, Code type );
	Code def_using          ( s32 length, char const* name, Code type );
	Code def_using_namespace(             char const* name );
	Code def_using_namespace( s32 length, char const* name );

	Code def_variable       ( Code type,             char const* name, Code value = NoCode, Code specifiers = NoCode );
	Code def_variable       ( Code type, s32 length, char const* name, Code value = NoCode, Code specifiers = NoCode );

	Code def_class_body     ( s32 num, ... );
	Code def_enum_body      ( u32 num, ... );
	Code def_enum_body      ( u32 num, Code* Values );
	Code def_global_body    ( s32 num, ... );
	Code def_function_body  ( s32 num, ... );
	Code def_function_body  ( s32 num, Code* codes );
	Code def_namespace_body ( s32 num, ... );
	Code def_operator_body  ( s32 num, ... );
	Code def_params         ( s32 num, ... );
	Code def_params_macro   ( s32 num, ... );
	Code def_params         ( s32 num, Code* params );
	Code def_specifiers     ( s32 num , ... );
	Code def_specifiers     ( s32 num, SpecifierT* specs );
	Code def_struct_body    ( s32 num, ... );
	Code def_struct_body    ( s32 num, Code* codes );
#	pragma endregion Upfront

#	pragma region Incremental
	Code make_class       (             char const* name, Code parent = NoCode, Code specifiers = NoCode );
	Code make_class       ( s32 length, char const* name, Code parent = NoCode, Code specifiers = NoCode );
	Code make_enum        (             char const* name, Code type   = NoCode, Code body       = NoCode );
	Code make_enum        ( s32 length, char const* name, Code type   = NoCode, Code body       = NoCode );
	Code make_enum_class  (             char const* name, Code type   = NoCode, Code body       = NoCode );
	Code make_enum_class  ( s32 length, char const* name, Code type   = NoCode, Code body       = NoCode );
	Code make_function    (             char const* name, Code params = NoCode, Code ret_type   = NoCode, Code specifiers = NoCode );
	Code make_function    ( s32 length, char const* name, Code params = NoCode, Code ret_type   = NoCode, Code specifiers = NoCode );
	Code make_global_body (             char const* name = "", s32 num = 0, ... );
	Code make_global_body ( s32 length, char const* name = "", s32 num = 0, ... );
	Code make_namespace   (             char const* name );
	Code make_namespace   ( s32 length, char const* name );
	Code make_operator    (              OperatorT   op,  Code params = NoCode, Code ret_type = NoCode, Code specifiers = NoCode );
	Code make_params      ();
	Code make_specifiers  ();
	Code make_struct      (             char const* name, Code parent = NoCode,                         Code specifiers = NoCode );
	Code make_struct      ( s32 length, char const* name, Code parent = NoCode,                         Code specifiers = NoCode );
#	pragma endregion Incremental

#	pragma region Parsing
	Code parse_class      ( s32 length, char const* class_def     );
	Code parse_enum       ( s32 length, char const* enum_def      );
	Code parse_friend     ( s32 length, char const* friend_def    );
	Code parse_function   ( s32 length, char const* fn_def        );
	Code parse_global_body( s32 length, char const* body_def      );
	Code parse_namespace  ( s32 length, char const* namespace_def );
	Code parse_operator   ( s32 length, char const* operator_def  );
	Code parse_struct     ( s32 length, char const* struct_def    );
	Code parse_variable   ( s32 length, char const* var_def       );
	Code parse_type       ( s32 length, char const* type_def      );
	Code parse_typedef    ( s32 length, char const* typedef_def   );
	Code parse_using      ( s32 length, char const* using_def     );

	s32 parse_classes   ( s32 length, char const* class_defs,     Code* out_class_codes );
	s32 parse_enums     ( s32 length, char const* enum_defs,      Code* out_enum_codes );
	s32 parse_friends   ( s32 length, char const* friend_defs,    Code* out_friend_codes );
	s32 parse_functions ( s32 length, char const* fn_defs,        Code* out_fn_codes );
	s32 parse_namespaces( s32 length, char const* namespace_defs, Code* out_namespaces_codes );
	s32 parse_operators ( s32 length, char const* operator_defs,  Code* out_operator_codes );
	s32 parse_structs   ( s32 length, char const* struct_defs,    Code* out_struct_codes );
	s32 parse_variables ( s32 length, char const* vars_def,       Code* out_var_codes );
	s32 parse_typedefs  ( s32 length, char const* typedef_def,    Code* out_typedef_codes );
	s32 parse_usings    ( s32 length, char const* usings_def,     Code* out_using_codes );
#	pragma endregion Parsing

#	pragma region Untyped text
	Code untyped_str      (             char const* str );
	Code untyped_str      ( s32 length, char const* str);
	Code untyped_fmt      (             char const* fmt, ... );
	Code untyped_token_fmt(             char const* fmt, s32 num_tokens, ... );
#	pragma endregion Untyped text

	struct Builder
	{
		zpl_file File;
		string   Buffer;

		void print( Code );

		bool open( char const* path );
		void write();
	};

#ifdef GEN_FEATURE_EDITOR
	struct Policy
	{

	};

	struct SymbolInfo
	{
		ro_string File;
		Code      Signature;
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
				string     Specification;
			};
			RequestType    Type;
		};

		struct Receipt
		{
			ro_string File;
			Code      Found;
			Code      Written;
			bool      Result;
		};

		static allocator Allocator;

		static void set_allocator( allocator mem_allocator );

		array(zpl_file)     Files;
		string              Buffer;
		array(RequestEntry) Requests;

		void add    ( SymbolInfo definition,  Policy policy, Code to_inject );
		void replace( SymbolInfo definition,  Policy policy, Code to_replace);
		void remove ( SymbolInfo definition,  Policy policy, Code to_remove );

#	ifdef GEN_USE_REFACTOR_LIBRARY
		void refactor( char const* specification );
#	endif

		bool process_requests( array(Receipt) out_receipts );
	};
#endif

#ifdef GEN_FEATURE_SCANNER
	struct Scanner
	{
		struct RequestEntry
		{
			SymbolInfo Info;
		};

		struct Receipt
		{
			ro_string File;
			Code      Defintion;
			bool      Result;
		};

		allocator Allocator;

		static void set_allocator( allocator mem_allocator );

		array(zpl_file)     Files;
		string              Buffer;
		array(RequestEntry) Requests;

		void add( SymbolInfo signature, Policy policy );

		bool process_requests( array(Receipt) out_receipts );
	};
#endif
#pragma endregion Gen Interface
}

#pragma region Macros
#	define gen_main main

#	define __                     NoCode
#	define spec_alignas( Value_ ) ESpecifier::Alignas, Value

// This represents the naming convention for all typename Codes generated.
// Use this if your using the DSL if you would like a different convention.
#	define type_ns( Name_ )       t_##Name_

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
#	define marcofn_no_arg_expander(Func)            ,,,,,,,,,,,,,,,,Func_ ## _0
#	define macrofn_finder(Func_, ...)               macrofn_chose_from_arg_num(Func_, tbc_marcofn_no_arg_expander __VA_ARGS__ (Func_))
#	define macrofn_polymorphic(Func_, ...)          macrofn_finder(Func_, __VA_ARGS__)(__VA_ARGS__)

#	define function_5( Name_, Params_, RetType_, Specifiers_, Body_ ) gen::def_function( txt_n_len( Name_ ), macro_expand( Params_ ), type_ns(RetType_), Specifiers_, Body_ )
#	define function_4( Name_, Params_, RetType_, Specifiers_ )        gen::def_function( txt_n_len( Name_ ), macro_expand( Params_ ), type_ns(RetType_), Specifiers_ )
#	define function_3( Name_, Params_, RetType_ )                     gen::def_function( txt_n_len( Name_ ), macro_expand( Params_ ), type_ns(RetType_) )
#	define function_2( Name_, Params_ )                               gen::def_function( txt_n_len( Name_ ), macro_expand( Params_ ) )
#	define function_1( Name_ )                                        gen::def_function( txt_n_len( Name_ ) )

#	define params_12( T_1, V_1, T_2, V_2, T_3, V_3, T_4, V_4, T_5, V_5, T_6, V_6 ) gen::def_params( 6, type_ns(T_1), txt_n_len( V_1), type_ns(T_2), txt_n_len( V_2), type_ns(T_3), txt_n_len( V_3), type_ns(T_4), txt_n_len( V_4), type_ns(T_5), txt_n_len( V_5), type_ns(T_6), txt_n_len(V_6))
#	define params_10( T_1, V_1, T_2, V_2, T_3, V_3, T_4, V_4, T_5, V_5 )           gen::def_params( 5, type_ns(T_1), txt_n_len( V_1), type_ns(T_2), txt_n_len( V_2), type_ns(T_3), txt_n_len( V_3), type_ns(T_4), txt_n_len( V_4), type_ns(T_5), txt_n_len( V_5))
#	define params_8(  T_1, V_1, T_2, V_2, T_3, V_3, T_4, V_4 )                     gen::def_params( 4, type_ns(T_1), txt_n_len( V_1), type_ns(T_2), txt_n_len( V_2), type_ns(T_3), txt_n_len( V_3), type_ns(T_4), txt_n_len( V_4) )
#	define params_6(  T_1, V_1, T_2, V_2, T_3, V_3 )                               gen::def_params( 3, type_ns(T_1), txt_n_len( V_1), type_ns(T_2), txt_n_len( V_2), type_ns(T_3), txt_n_len( V_3))
#	define params_4(  T_1, V_1, T_2, V_2 )                                         gen::def_params( 2, type_ns(T_1), txt_n_len( V_1), type_ns(T_2), txt_n_len( V_2))
#	define params_2(  T_1, V_1 )                                                   gen::def_param (    type_ns(T_1), txt_n_len( V_1))
#	define params_bad static_assert("params(...): Invalid number of parameters provided.")
#	define params_11  params_bad
#	define params_9   params_bad
#	define params_7   params_bad
#	define params_5   params_bad
#	define params_3   params_bad
#	define params_1   params_bad

// Upfront

#	define class( Name_, ... )           gen::def_class( txt_n_len(Name_), __VA_ARGS__ )
#	define enum( Name_, Type_, Body_ )   gen::def_enum ( txt_n_len(Name_), type_ns(Type_), Body_ )

#	define function( ... )               macrofn_polymorphic( function, __VA_ARGS__ )
#	define namespace( Name_, Body_ )     gen::def_namespace      ( txt_n_len(Name_),  Body_ )
#	define operator( Op_, ... )          macrofn_polymorphic( operator, __VA_ARGS__ )
#	define params( ... )                 macrofn_polymorphic( params, __VA_ARGS__ )
#	define specifiers( ... )             gen::def_specifiers     ( macro_num_args( __VA_ARGS__ ), __VA_ARGS__ )
#	define struct( Name_, ... )          gen::def_struct         ( txt_n_len(Name_), __VA_ARGS__ )
#	define variable( Type_, Name_, ... ) gen::def_variable       ( type_ns(Type_), txt_n_len(Name_), __VA_ARGS__ )
#	define type( Value_, ... )           gen::def_type           ( txt_n_len(Value_), __VA_ARGS__ )
#	define type_fmt( Fmt_, ... )         gen::def_type           ( bprintf( Fmt_, __VA_ARGS__ ) )
#	define using( Name_, Type_ )		 gen::def_using          ( txt_n_len(Name_), type_ns(Type_) )
#	define using_namespace( Name_ )      gen::def_using_namespace( txt_n_len(Name_) )

#	define class_body(      ... )        gen::def_class_body    ( macro_num_args( __VA_ARGS__ ), __VA_ARGS__ )
#	define enum_body(       ... )        gen::def_enum_body     ( macro_num_args( __VA_ARGS__ ), __VA_ARGS__ )
#	define global_body(     ... )        gen::def_global_body   ( macro_num_args( __VA_ARGS__ ), __VA_ARGS__ )
#	define function_body(   ... )        gen::def_function_body ( macro_num_args( __VA_ARGS__ ), __VA_ARGS__ )
#	define namespace_body(  ... )        gen::def_namespace_body( macro_num_args( __VA_ARGS__ ), __VA_ARGS__ )
#	define operator_body(   ... )        gen::def_operator_body ( macro_num_args( __VA_ARGS__ ), __VA_ARGS__ )
#	define struct_body(     ... )        gen::def_struct_body   ( macro_num_args( __VA_ARGS__ ), __VA_ARGS__ )

// Incremental

#   define make( ConstructType_, Name_, ... ) Code Name_ = make_##ConstructType_( txt_n_len(Name_), __VA_ARGS__ );

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
#	define using_code(       ... ) gen::parse_code       ( txt_n_len( __VA_ARGS__ ))

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
	// Predefined typename codes.
	// These are not set until gen::init is called.
	// This just preloads a bunch of Code types into the code pool.

	extern Code type_ns( void );

	extern Code type_ns( bool );
	extern Code type_ns( char );
	extern Code type_ns( wchar_t );

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
	ct s32 MaxNameLength             = 128;
	ct s32 MaxUntypedStrLength       = kilobytes(640);
	ct s32 StringTable_MaxHashLength = kilobytes(1);

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
#endif

#pragma region Gen Interface Inlines
namespace gen
{
	forceinline
	Code def_class( char const* name, Code parent, Code specifiers, Code body )
	{
		return def_class( zpl_strnlen( name, MaxNameLength ), name, parent, specifiers, body );
	}

	forceinline
	Code def_enum( char const* name, Code type, Code body )
	{
		return def_enum( zpl_strnlen( name, MaxNameLength ), name, type, body );
	}

	forceinline
	Code def_function( char const* name, Code params, Code ret_type, Code specifiers, Code body )
	{
		return def_function( zpl_strnlen( name, MaxNameLength), name, params, ret_type, specifiers, body );
	}

	forceinline
	Code def_namespace( char const* name, Code body )
	{
		return def_namespace( zpl_strnlen( name, MaxNameLength), name, body );
	}

	forceinline
	Code def_param( Code type, char const* name )
	{
		return def_param( type, zpl_strnlen( name, MaxNameLength ), name );
	}

	forceinline
	Code def_struct( char const* name, Code parent, Code specifiers, Code body )
	{
		return def_struct( zpl_strnlen( name, MaxNameLength), name, parent, specifiers, body );
	}

	forceinline
	Code def_type( char const* name, Code specifiers )
	{
		return def_type( zpl_strnlen( name, MaxNameLength ), name, specifiers );
	}

	forceinline
	Code def_using( char const* name, Code type )
	{
		return def_using( zpl_strnlen( name, MaxNameLength ), name, type );
	}

	forceinline
	Code def_using_namespace( char const* name )
	{
		return def_using_namespace( zpl_strnlen( name, MaxNameLength ), name );
	}

	forceinline
	Code def_variable( Code type, char const* name, Code value, Code specifiers )
	{
		return def_variable( type, zpl_strnlen(name, MaxNameLength ), name, value, specifiers );
	}

	forceinline
	Code make_class( char const* name, Code parent, Code specifiers )
	{
		return make_class( zpl_strnlen(name, MaxNameLength), name, parent, specifiers );
	}

	forceinline
	Code make_enum( char const* name, Code type, Code specifiers )
	{
		return make_struct( zpl_strnlen(name, MaxNameLength), name, type, specifiers );
	}

	forceinline
	Code make_function( char const* name, Code params, Code ret_type, Code specifiers )
	{
		return make_function( zpl_strnlen(name, MaxNameLength), name, params, ret_type, specifiers );
	}

	forceinline
	Code make_namespace( char const* name )
	{
		return make_namespace( zpl_strnlen( name, MaxNameLength ), name );
	}

	forceinline
	Code make_struct( char const* name, Code parent, Code specifiers )
	{
		return make_struct( zpl_strnlen(name, MaxNameLength), name, parent, specifiers );
	}

	forceinline
	Code untyped_str( char const* str )
	{
		return untyped_str( zpl_strnlen( str, MaxUntypedStrLength ), str );
	}
}
#pragma endregion Gen Interface Inlines
