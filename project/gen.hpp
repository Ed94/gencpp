/*
	gencpp: A simple staged metaprogramming library for C++.
	
	This library is intended for small-to midsize projects.

	AST type checking supports only a small subset of c++. 
	See the 'ECode' namespace and 'gen API' region to see what is supported.

	### *WHAT IS NOT PROVIDED*

	* Macro or template generation      : This library is to avoid those, adding support for them adds unnecessary complexity. 
	                                      If you desire define them outside the gen_time scopes. 
	* Expression validation             : Execution expressions are defined using the untyped string API. 
	                                      There is no parse API for validating expression (possibly will add in the future)
	* Complete file parser DSL          : This isn't like the unreal header tool. 
	                                      Code injection to file or based off a file contents is not supported by the api. However nothing is stopping you using the library for that purpose.
	* Modern c++ (STL library) features

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
	* def_class_body
	* def_class_fwd
	* def_enum
	* def_enum_class
	* def_enum_body
	* def_global_body
	* def_namespace
	* def_namespace_body
	* def_operator
	* def_operator_fwd
	* def_param
	* def_params
	* def_proc
	* def_proc_body
	* def_proc_fwd
	* def_specifier
	* def_specifiers
	* def_struct
	* def_struct_body
	* def_struct_fwd
	* def_variable
	* def_type
	* def_typedef
	* def_using
	* def_using_namespace

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
	* make_global_body
	* make_namespace
	* make_operator
	* make_params
	* make_proc
	* make_specifiers
	* make_struct
	* make_variable
	* make_type
	* make_typedef
	* make_using

	### Parse construction

	A string provided to the API is parsed for the intended language construct.

	Interface :

	* parse_class
	* parse_classes
	* parse_enum
	* parse_enums
	* parse_global_body
	* parse_namespace
	* parse_namespaces
	* parse_operator
	* parse_operators
	* parse_proc
	* parse_procs
	* parse_struct
	* parse_strucs
	* parse_variable
	* parse_variables
	* parse_type
	* parse_typedef
	* parse_typedefs
	* parse_using
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
	Even though thse are not validated from somewhat correct c/c++ syntax or components, it doesn't mean that
	Untyped code can be added as any component of a Code AST:

	* Untyped code cannot have children, thus there cannot be recursive injection this way.
	* Untyped code can only be a child of a parent of body AST, or for values of an assignment.

	These restrictions help prevent abuse of untyped code to some extent.
*/

#pragma once

#include "Bloat.hpp"

// Temporarily here for debugging purposes.
#define gen_time
#define GEN_ENFORCE_READONLY_AST
// #define GEN_DEFINE_DSL
#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
// #define GEN_BAN_CPP_TEMPLATES
#define GEN_USE_FATAL

#ifdef gen_time
namespace gen
{
	using LogFailType = sw(*)(char const*, ...);

#ifdef GEN_BAN_CPP_TEMPLATES
	#define template static_assert("Templates are banned within gen_time scope blocks")
#endif

#ifdef GEN_USE_FATAL
	ct LogFailType log_failure = fatal;
#else
	ct LogFailType log_failure = log_fmt;
#endif

	namespace ECode
	{
		enum Type : u8
		{
			Invalid,        
			Untyped,        
			Access_Public,
			Access_Private,
			Access_Protected,
			Class,          
			Enum,           
			Enum_Body,      
			Global_Body,    
			Namespace,      
			Namespace_Body, 
			Parameters,     
			Proc,           
			Proc_Body,      
			Proc_Forward,   
			Specifiers,     
			Struct,         
			Struct_Body,    
			Variable,       
			Typedef,        
			Typename,       
			Using,          

			Num_Types
		};

		local_persist
		char const* str( Type type )
		{
			static 
			char const* lookup[Num_Types] = {
				"Invalid",
				"Untyped",
				"Access_Public",
				"Access_Private",
				"Access_Protected",
				"Class",
				"Enum",
				"Enum_Body",
				"Global_Body",
				"Namespace",
				"Namespace_Body",
				"Parameters",
				"Proc",
				"Proc_Body",
				"Proc_Forward",
				"Specifiers",
				"Struct",
				"Struct_Body",
				"Variable",
				"Typedef",
				"Typename",
				"Using",
			};

			return lookup[ type ];
		}
	}
	using CodeT = ECode::Type;

	namespace EOperator
	{
		enum Type : u8
		{
			Add,
			Subtract,
			Multiply,
			Divide,
			Modulo,

			Num_Ops
		};

		inline 
		char const* str( Type op )
		{
			local_persist
			char const* lookup[ Num_Ops ] = {
				"+",
				"-",
				"*",
				"/",
			};

			return lookup[ op ];
		}
	}
	using OperatorT = EOperator::Type;

	namespace ESpecifier
	{
		enum Type : u8
		{
			Attribute,          
			Alignas,            
			Constexpr,          
			Const,              
			Inline,             
			Pointer,            
			Reference,          
			RValue,             

			C_Linkage,          
			API_Import,         
			API_Export,         
			External_Linkage,   
			Internal_Linkage,   
			Static_Member,      
			Local_Persist,      
			Thread_Local,       

			Invalid,
			Num_Specifiers,
		};

		// Specifier to string
		inline
		char const* to_str( Type specifier )
		{
			local_persist
			char const* lookup[ Num_Specifiers ] = {
				"alignas",
				"constexpr",
				"const",
				"inline",
				"*",
				"&",
				"&&",

				"extern \"C\"",

			#if defined(ZPL_SYSTEM_WINDOWS)
				"__declspec(dllexport)",
				"__declspec(dllimport)",
			#elif defined(ZPL_SYSTEM_MACOS)
				"__attribute__ ((visibility (\"default\")))",
				"__attribute__ ((visibility (\"default\")))",
			#endif

				"extern",
				"static",
				"static",
				"static",
				"thread_local"
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
			array_append( Entries, other );

			other->Parent = this;
		}

		forceinline
		AST* body()
		{
			return Entries[0];
		}

		forceinline
		bool check();

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
			};

			return lookup[Type];
		}

		forceinline
		bool is_invalid() const
		{
			return Type != ECode::Invalid;
		}

		forceinline
		char const* type_str() const
		{
			return ECode::str( Type );
		}

		string to_string() const;

	#pragma endregion Member Procedures

	#define Using_Code_POD          \
		CodeT             Type;     \
		bool              Readonly; \
		AST*              Parent;   \
		string            Name;     \
		string            Comment;  \
		union {                     \
			array(AST*)   Entries;  \
			string        Content;  \
		};

		Using_Code_POD;
	};

	struct CodePOD
	{
		Using_Code_POD;
	};

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
		AST* ast;

		Code body()
		{
			if ( ast->Type == ECode::Invalid )
			{
				log_failure("Code::body: Type is invalid, cannot get");
				return InvalidCode;
			}

			if ( ast->Entries == nullptr || array_count(ast->Entries) == 0 )
			{
				log_failure("Code::body: Entries of ast not properly setup.");
				return InvalidCode;
			}

		#ifdef GEN_ENFORCE_READONLY_AST 
			if ( ast->Readonly )
			{
				log_failure("Attempted to a body AST from a readonly AST!");			
				return InvalidCode;
			}
		#endif

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

		#ifdef GEN_ENFORCE_READONLY_AST 
			if ( ast->Readonly )
			{
				log_failure("Attempted to access a member from a readonly AST!");			
				return nullptr;
			}
		#endif

			return ast;
		}
	};
	static_assert( sizeof(Code) == sizeof(AST*), "ERROR: Code is not POD" );

	// Used when the its desired when omission is allowed in a definition.
	ct Code UnusedCode = { nullptr };

	// Used internally for the most part to identify invaidly generated code.
	extern const Code InvalidCode;

	/*
	*/
	ZPL_TABLE_DECLARE( ZPL_EXTERN, StringTable, str_tbl_, string );

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

	// Use this only if you know you generated the code you needed to a file
	// And rather get rid of current code asts instead of growing the pool memory.
	void clear_code_pool();

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
	Code def_class     ( char const* name, Code body, Code parent = UnusedCode, Code specifiers = UnusedCode );
	Code def_class_body( s32 num, ... );
	Code def_class_fwd ( char const* name );

	Code def_enum( char const* name, Code type, Code body );

	Code def_global_body( s32 num, ... );

	Code def_namespace     ( char const* name, Code body );
	Code def_namespace_body( s32 num, ... );

	Code def_operator( OperatorT op
		, Code specifiers
		, Code params
		, Code ret_type
		, Code body 
	);
	Code def_operator_fwd( OperatorT op, Code specifiers, Code params, Code ret_type );

	Code def_param( Code type, char const* name );
	Code def_params( s32 num, ... );
	Code def_params( s32 num, Code* params );

	Code def_proc( char const* name
		, Code specifiers
		, Code params
		, Code ret_type
		, Code body 
	);

	Code def_proc_body( s32 num, ... );
	Code def_proc_body( s32 num, Code* codes );

	Code def_proc_fwd( char const* name
		, Code specifiers
		, Code params
		, Code ret_type
	);

	Code def_specifier( SpecifierT* specifier );
	Code def_specifiers( s32 num , ... );
	Code def_specifiers( s32 num, SpecifierT* specs );

	Code def_struct( char const* name, Code body, Code parent = UnusedCode, Code specifiers = UnusedCode );

	Code def_struct_body( s32 num, ... );
	Code def_struct_body( s32 num, Code* codes );

	Code def_sturct_fwd();

	Code def_variable( Code type, char const* name, Code value = UnusedCode, Code specifiers = UnusedCode );

	Code def_type( char const* name, Code specifiers = UnusedCode );

	Code def_using          ( char const* name, Code type );
	Code def_using_namespace( char const* name );
#	pragma endregion Upfront

#	pragma region Incremental
	Code make_class( char const* name, Code parent = UnusedCode, Code specifiers = UnusedCode );

	Code make_enum      ( char const* name, Code type = UnusedCode, Code body = UnusedCode );
	Code make_enum_class( char const* name, Code type = UnusedCode, Code body = UnusedCode );

	Code make_global_body( char const* name = "", s32 num = 0, ... );
	
	Code make_namespace( char const* name );

	Code make_operator( OperatorT op
		, Code specifiers = UnusedCode
		, Code params = UnusedCode
		, Code ret_type = UnusedCode
		, Code body  = UnusedCode
	);

	Code make_params( s32 num, ... );

	Code make_proc( char const* name
		, Code specifiers = UnusedCode
		, Code params = UnusedCode
		, Code ret_type = UnusedCode
	);

	Code make_specifiers( s32 num , ... );
	Code make_specifiers( s32 num, SpecifierT* specs );
	
	Code make_struct( char const* name, Code parent = UnusedCode, Code specifiers = UnusedCode );

	Code make_variable( char const* name, Code type = UnusedCode, Code value = UnusedCode, Code specifiers = UnusedCode );

	Code make_type( char const* name, Code specifiers = UnusedCode );

	Code make_using( char const* name, Code type = UnusedCode, Code specifiers = UnusedCode );
#	pragma endregion Incremental

#	pragma region Parsing
	Code parse_class( char const* class_def, s32 length );
	s32 parse_classes( char const* class_defs, s32 length, Code* out_classes );

	Code parse_enum( char const* enum_def, s32 length);
	s32 parse_enums( char const* enum_defs, s32 length, Code* out_enums );

	Code parse_global_body( char const* body_def, s32 length );

	Code parse_operator( char const* operator_def, s32 length );

	Code parse_proc( char const* proc_def, s32 length );
	s32 parse_procs( char const* proc_defs, s32 length, Code* out_procs );

	Code parse_struct( char const* struct_def, s32 length );
	s32 parse_structs( char const* struct_defs, s32 length, Code* out_struct_codes );

	Code parse_variable( char const* var_def, s32 length );
	s32 parse_variables( char const* vars_def, s32 length, Code* out_var_codes );

	Code parse_type( char const* type_def, s32 length );

	Code parse_typedef( char const* typedef_def, s32 length );
	s32 parse_typedef( char const* typedef_def, s32 length, Code* out_typedef_codes );

	Code parse_using ( char const* using_def, s32 length );
	s32 parse_usings( char const* usings_def, s32 length, Code* out_using_codes );
#	pragma endregion Parsing

#	pragma region Untyped text
	/*
		Define an untyped code string.

		Untyped code may be used in bodies of functions, namespaces, or structs
		or the in places where expressions may be placed.

		Because the code within it is untyped, errors will naturally not be provided.
		Consider this an a preprocessor define.
	*/
	Code untyped_str( char const* str );
	Code untyped_str( char const* str, s32 length);

	/*
		Define an untyped code string using traditional 'printf'.

		Untyped code may be used in bodies of functions, namespaces, or structs
		or the in places where expressions may be placed.

		Because the code within it is untyped, errors will naturally not be provided.
		Consider this an a preprocessor define.
	*/
	Code untyped_fmt( char const* fmt, ... );

	/*
		Define an untyped code string using token formatting:
		... { <ID> } ... Will be repalced with value of token ID.

		Values are to provided as: <char const* ID>, <char const* Value>, ...

		num_tokens : The number of ID-Value pairs provided.

		Untyped code may be used in bodies of functions, namespaces, or structs
		or the in places where expressions may be placed.

		Because the code within it is untyped, errors will naturally not be provided.
		Consider this an a preprocessor define.
	*/
	Code untyped_token_fmt( char const* fmt, s32 num_tokens, ... );
#	pragma endregion Untyped text

	/* 
		Used to generate the files.
		This is inspired by jai's usage of the string_builder with #insert.

		Its expected when using this library that Code ast will be serialized with the:
		Builder::print() proc

		The seralized content of the Code ast will be appended to Buffer within an empty line
		prepared for a another definition or to add an empty newline to the end of the file. 

		Builder::write() should be called when all Code has been seralized for that file.

		The #insert directive is thus represented by an #include of the generated file at your desired line
		of any file in the target project.
	*/
	struct Builder
	{
		zpl_file File;
		string   Buffer;

		void print( Code );

		bool open( char const* path );
		void write();
	};
#pragma endregion Gen Interface
}

#pragma region MACROS
#	define gen_main main
#	define __       UnusedCode

/*
	gen's Domain Specific Langauge.

	Completely optional, makes the code gen syntax less verbose and cumbersome...
	Since its C macros ends up looking like a lisp dialect...

	Longforms auto-define the variable.
	Shorthands are just the function call.

	Anything below the make() macro is intended to be syntacticall used int he follwing format:
	make( <type>, <name> )
	{
		...
	}

	Where ... are whatever is deemed necessary to produce the definition for the def( <name> ).

	The code macros are used to embed c/c++ to insert into the desired lcoation.
*/
#ifdef GEN_DEFINE_DSL
#   define untyped_code( Name_, Value_ )     Code Name_     = gen::untyped_str( txt(Value_) )
#	define typename( Name_, Value_ )         Code t_##Name_ = gen::def_type( txt(Value_) )
#	define typename_fmt( Name_, Fmt_, ... )  Code t_##Name_ = gen::def_type( bprintf( Fmt_, __VA_ARGS__ ) )
#	define using_type( Name_, Type_ )		 Code Name_     = gen::def_using( #Name_, t_##Type_ )
#	define variable( Type_, Name_, ... )     Code Name_     = gen::def_variable( t_##Type_, #Name_, __VA_ARGS__ )

#   define untyped( Value_ )                      gen::untyped_str( txt_with_length(Value_) )
#	define code_token( Fmt_, ... )                gen::untyped_token_fmt( Fmt_, VA_NARGS( __VA_ARGS__), __VA_ARGS__ )
#	define code_fmt( Fmt_, ... )                  gen::untyped_fmt( Fmt_, __VA_ARGS__ )
#	define specifiers( ... )                      gen::def_specifiers( VA_NARGS( __VA_ARGS__ ), __VA_ARGS__ )
#	define type( Value_ )                         gen::def_type( txt(Value_) )
#	define type_fmt( Fmt_, ... )                  gen::def_type( bprintf( Fmt_, __VA_ARGS__ ) )
#	define using( Name_, Type_ )		          gen::def_using( #Name_, Type_ )
#	define var( Type_, Name_, ... )               gen::def_variable( Type_, #Name_, __VA_ARGS__ )

#   define make( Type_, Name_, ... )                                Code Name_ = make_##Type_( #Name_, __VA_ARGS__ );
#	define enum( Name_, Type_, Body_ )                                      gen::def_enum( #Name_, t_##Type_, Body_ )
#	define proc( Name_, Specifiers_, RetType_, Parameters_, Body_ ) Name_ = gen::def_proc( #Name_, Specifiers_, Parameters_, RetType_, Body_ )
#	define proc_body( ... )                                                 gen::def_proc_body( VA_NARGS( __VA_ARS__ ), __VA_ARGS__ )
#	define params( ... )                                                    gen::def_params( VA_NARGS( __VA_ARGS__ ) / 2, __VA_ARGS__ )
#	define struct( Name_, Parent_, Specifiers_, Body_ )             Name_ = gen::def_struct( #Name_, Body_, Parent_, Specifiers_ )
#	define struct_body( ... )                                               gen::def_struct_body( VA_NARGS( __VA_ARGS__ ), __VA_ARGS__ )

#	define add_var( Value_ )     add( gen::parse_variable( txt_with_length(Value_)) )
#	define add_untyped( Value_ ) add( gen::untyped_str( txt_with_length( Value_ ) ) )
#	define add_type( Value_ )    add( gen::parse_type( txt_with_length(Value_)) )
#	define add_params( Value_ )  add( gen::parse_params( txt_with_length(Value_) ))

#	define enum_code( Def_ )   gen::parse_enum( txt( Def_ ), sizeof( txt( Def_ )) )
#	define global_code( Def_ ) gen::parse_global_body( txt_with_length( Def_ ))
#	define namespace_code( Def_ ) gen::parse_namespace( txt(Def_), sizeof( txt(Def_)) )
#	define proc_code( Def_ )   gen::parse_proc( txt( Def_ ), sizeof( txt( Def_ )) )
#	define struct_code( Def_ ) gen::parse_struct( txt( Def_ ), sizeof( txt( Def_ )) )
#	define variable_code( Def_ ) gen::parse_variable( txt_with_length( Def_ ) )
#endif
#pragma endregion MACROS

#pragma region CONSTANTS
#ifdef GEN_DEFINE_LIBRARY_CODE_CONSTANTS 
namespace gen
{
	// Predefined typename codes.
	// These are not set until gen::init is called.
	// This just preloads a bunch of Code types into the code pool.

	extern Code t_void;

	extern Code t_bool;
	extern Code t_char;
	extern Code t_wchar_t;

	extern Code t_s8;
	extern Code t_s16;
	extern Code t_s32;
	extern Code t_s64;

	extern Code t_u8;
	extern Code t_u16;
	extern Code t_u32;
	extern Code t_u64;

	extern Code t_sw;
	extern Code t_uw;

	extern Code t_f32;
	extern Code t_f64;

	extern Code spec_constexpr;
	extern Code spec_inline;
}
#endif

namespace gen
{
	extern Code access_public;
	extern Code access_protected;
	extern Code access_private;
}
#pragma endregion CONSTANTS
#endif
