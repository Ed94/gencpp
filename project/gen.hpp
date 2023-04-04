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
	                                      Code injection to file or based off a file contents is not supported by the api. 
										  However nothing is stopping you using the library for that purpose.
	* Modern c++ (STL library) features

	As mentioned in [Usage](#Usage), the user is provided Code objects by calling the interface procedures to generate them or find existing matches.

	The AST is managed by the library and provided the user via its interface prodedures.

	Notes:

	* The allocator definitions used are exposed to the user incase they want to dictate memory usage*
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

	* def_forward_decl
	* def_class
	* def_global_body
	* def_proc
	* def_proc_body
	* def_namespace
	* def_namespace_body
	* def_param
	* def_params
	* def_operator
	* def_specifier
	* def_specifiers
	* def_struct
	* def_struct_body
	* def_variable
	* def_type
	* def_using
	* def_using_namespace

	### Incremental construction

	A Code ast is provided but only completed upfront if all components are provided.
	Components are then added using the AST API for adding ASTs:

	* code.add( AST* )         // Adds AST with validation.
	* code.add_entry( AST* )   // Adds AST entry without validation.
	* code.add_content( AST* ) // Adds AST string content without validation.

	Code ASTs may be explictly validated at anytime using Code's check() member function.

	Interface :

	* make_forward_decl
	* make_class
	* make_global_body
	* make_proc
	* make_namespace
	* make_params
	* make_operator
	* make_specifiers
	* make_struct
	* make_variable
	* make_type
	* make_using
	* make_using_namespace

	### Parse construction

	A string provided to the API is parsed for the intended language construct.

	Interface :

	* parse_forward_decl
	* parse_class
	* parse_glboal_body
	* parse_proc
	* parse_namespace
	* parse_params
	* parse_operator
	* parse_specifiers
	* parse_struct
	* parse_variable
	* parse_type
	* parse_using
	* parse_using

	The parse API treats any execution scope definitions with no validation and are turned into untyped Code ASTs.
	This includes the assignmetn of variables; due to the library not yet supporting c/c++ expression parsing.

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
// #define GEN_USE_FATAL

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
			Invalid,        // Used only with improperly created Code nodes
			Untyped,        // User provided raw string
			Decl_Function,  // <specifier> <type> <name> ( <params> )
			Decl_Type,      // <type> <name>;
			Function,       // <type> <name>( <parameters> )
			Function_Body,  // { <body> }
			Namespace,      // Define a namespace
			Namespace_Body, // { <body> } 
			Parameters,     // <type> <param> ...
			Specifiers,     // Used with functions, structs, variables
			Struct,         // struct <specifier> <name> <parent>
			Struct_Body,    // {<body> }
			Variable,       // <type> <name>
			Typedef,        // typedef <type> <alias>
			Typename,       // Typename, used with other types
			Using,          // using <name> = <type>
			Unit,           // Represents a file.

			Num_Types
		};

		inline
		char const* str( Type type )
		{
			static 
			char const* lookup[Num_Types] = {
				"Invalid",
				"Untyped",
				"Decl_Function",
				"Decl_Type",
				"Function",
				"Function_Body",
				"Namespace",
				"Namespace_Body",
				"Parameters",
				"Specifiers",
				"Struct",
				"Struct_Body",
				"Variable",
				"Typedef",
				"Typename",
				"Using",
				"Unit",
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
			static 
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
			Attribute,          // [ <attributes ]
			Alignas,            // alignas(#)
			Constexpr,          // constexpr
			Const,              // const
			Inline,             // inline
			RValue,             // 

			C_Linkage,          // extern "C"
			API_Import,         // Vendor specific way dynamic import symbol
			API_Export,         // Vendor specific way to dynamic export
			External_Linkage,   // extern
			Internal_Linkage,   // static (within unit file)
			Static_Member,      // static (within sturct/class)
			Local_Persist,      // static (within function)
			Thread_Local,       // thread_local

			Num_Specifiers,
			Invalid
		};

		// Specifier to string
		inline
		char const* to_str( Type specifier )
		{
			static 
			char const* lookup[ Num_Specifiers ] = {
				"alignas",
				"constexpr",
				"const",
				"inline",

				"extern \"C\"",

			#if defined(ZPL_SYSTEM_WINDOWS) && 0// API_Import and API_Export strings
				"__declspec(dllexport)",
				"__declspec(dllimport)",
			#elif defined(ZPL_SYSTEM_MACOS) || 1
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
			static 
			u32 keymap[ Num_Specifiers ];
			do_once_start
				for ( u32 index = 0; index < Num_Specifiers; index++ )
				{
					char const* enum_str = to_str( (Type)index );

					keymap[index] = crc32( enum_str, strnlen(enum_str, 42) );
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

	// TODO: If perf needs it, convert layout an SOA format.
	/* 
		Simple AST POD with functionality to seralize into C++ syntax.

		ASTs are currently stored as an AOS. They are always reconstructed on demand.
		Thus redundant AST can easily occur.
		Not sure if its better to store them in a hashmap.
	*/
	struct AST
	{
	#pragma region Member API
		forceinline
		void add( AST* other )
		{
			array_append( Entries, other );

			other->Parent = this;
		}

		forceinline
		bool has_entries() const
		{
			static bool lookup[ ECode::Num_Types] = {
				false, // Invalid
				false, // Untyped
				true,  // Decl_Function
				true,  // Decl_Type
				true,  // Function
				true,  // Parameters
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
		char const* type_str() const
		{
			return ECode::str( Type );
		}

		string to_string() const;

	#pragma endregion Member API

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
				fatal("Code::body: Type is invalid, cannot get");

			if ( ast->Entries == nullptr || array_count(ast->Entries) == 0 )
				fatal("Code::body: Entries of ast not properly setup.");

			return pcast( Code, ast->Entries[0]);
		}

		forceinline
		void lock()
		{
		#ifdef GEN_ENFORCE_READONLY_AST
			ast->Readonly = true;
		#endif
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
			ast = other.ast;

			return *this;
		}

		forceinline
		AST* operator ->() 
		{
		#ifdef GEN_ENFORCE_READONLY_AST 
			if ( ast == nullptr )
				fatal("Attempt to dereference a nullptr!");

			if ( ast->Readonly )
				fatal("Attempted to access a member from a readonly ast!");			
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
		Type registy: Used to store Typename ASTs. Types are registered by their string literal value.

		Purely used as a memory optimization.
		Strings made with the Typename ASTs are stored in thier own arena allocator.
		TODO: Implement and replace usage of def_type.
	*/
	ZPL_TABLE_DECLARE( ZPL_EXTERN, TypeRegistry, type_reg_, Code );

#pragma region gen API
	/*
		Initialize the library.
		This currently just initializes the CodePool.
	*/	
	void init();

	#pragma region Upfront
	/*
		Foward Declare a type:
		<specifiers> <type> <name>;
	*/
	Code def_fwd_type( Code type, char const* name, Code specifiers = UnusedCode );

	/*
		Foward Declare a function:
		<specifiers> <name> ( <params> );
	*/
	Code def_fwd_proc( char const* name
		, Code specifiers
		, Code params
		, Code ret_type
	);

	/*
		Define an expression:
		< c/c++ expression >

		TODO: Evalute if you want to validiate at the execution layer during gen_time (dosen't seem necessary)
	*/
	// Code def_expression( Code value );

	/*
		Define a function:
		<specifiers> <name> ( <params> )
		{
			<body>
		}
	*/
	Code def_proc( char const* name
		, Code specifiers
		, Code params
		, Code ret_type
		, Code body 
	);

	/*
		Define a fucntion body:
		{
			<entry>

			...
		}

		There will be an empty line separation between entires
	*/
	Code def_proc_body( s32 num, ... );
	Code def_proc_body( s32 num, Code* codes );

	/*
		Define a namespace;
		namespace <name>
		{
			<body>
		}
	*/
	Code def_namespace( char const* name, Code body );

	/*
		Define a namespace body:
		{
			<entry>

			...
		}

		There will be an empty line separation between entires
	*/
	Code def_namespace_body( s32 num, ... );

	/*
		Define a set of parameters for a function:
		<name> <type>, ...
	*/
	Code def_params( s32 num, ... );
	Code def_params( s32 num, char const** params );

	/*
		Define an operator definition.
	*/
	Code def_operator( OperatorT op, Code specifiers, Code params, Code ret_type, Code body );

	/*
		Define a set of specifiers for a function, struct, type, or varaible

		Note: 		
		If alignas is specified the procedure expects the next argument to be the alignment value.
		If attribute is specified the procedure expects the next argument to be its content as a string.
	*/
	Code def_specifiers( s32 num , ... );
	Code def_specifiers( s32 num, SpecifierT* specs );

	/*
		Define a struct:
		struct <specifiers> <name> : <parent>
		{
			<body>
		}
	*/
	Code def_struct( char const* name, Code body, Code parent = UnusedCode, Code specifiers = UnusedCode );

	/*
		Define a struct's body:
		{
			<entry>

			...
		}

		There will be an empty line separation between entires
	*/
	Code def_struct_body( s32 num, ... );
	Code def_struct_body( s32 num, Code* codes );

	/*
		Define a variable:
		<specifiers> <type> <name> = <value>;
	*/
	Code def_variable( Code type, char const* name, Code value = UnusedCode, Code specifiers = UnusedCode );

	/*
		Define a typename AST value.
		Useless by itself, its intended to be used in conjunction with other Code.

	Planned - Not yet Implemented:
		Typename Codes are not held in the CodePool, instead they are stored in a 
		type registry (hastable where the key is a crc hash of the name string).

		If a key exists the existing code value will be provided.
	*/
	Code def_type( char const* name );

	/*
		Define a using typedef:
		using <name> = <type>;
	*/
	Code def_using( char const* name, Code type );

	/*
		Define a using namespace:
		using namespace <name>;

		Can only be used in either a 
	*/
	Code def_using_namespace( char const* name );
	#pragma endregion Upfront

	#pragma region Incremental
	/*
		Provides an incomplete procedure AST but sets the intended type.
		Any adds will be type checked.

		Body is automatically made. Use body() to retrieve.
	*/
	Code make_proc( char const* name
		, Code specifiers = UnusedCode
		, Code params = UnusedCode
		, Code ret_type = UnusedCode
	);
	
	/*
		Provides an incomplete struct AST but sets the intended type.
		Any adds will be type checked.

		Body is automatically made. Use body() to retrieve.
	*/
	Code make_struct( char const* name, Code parent = UnusedCode, Code specifiers = UnusedCode );

/*
	Creates a unit file.

	These represent an encapsulation of a generated file
	Used this if you need to pass around a group of Code entires at file scope level.

	The name provided is the name of the file.
*/
Code make_file_body( char const* name );
	#pragma endregion Incremental

	/*
	*/
	Code parse_variable( char const* var_def, s32 length );

	/*
	*/
	Code parse_using( char const* using_def, s32 length );

	/*

	*/
	Code parse_operator( char const* operator_def, s32 length );

	/*
		Define a procedure by parsing a string.

		Note: This parser only supports the language features the library supports
		Any other features used and the lex or parse operation will fail.

		This is not a full-on c/c++ parser, it literally only grabs 
		what it needs to reconstruct the Code AST for seralization in the 
		builder, nothing else.
	*/
	Code parse_proc( char const* proc_def, s32 length );

	/*
		Define a struct by parsing a string.

		Note: This parser only supports the language features the library supports
		Any other features used and the lex or parse operation will fail.

		This is not a full-on c/c++ parser, it literally only grabs 
		what it needs to reconstruct the Code AST for seralization in the 
		builder, nothing else.
	*/
	Code parse_struct( char const* struct_def, s32 length );

	/*
	
	*/
	s32 parse_vars( char const* vars_def, s32 length, Code* out_vars_codes );

	/*

	*/
	s32 parse_usings( char const* usings_def, s32 length, Code* out_usings_codes );

	#pragma region Untyped text
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
	#pragma endregion Untyped text

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
#pragma endregion gen API
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

#   define untyped( Value_ )                      gen::untyped_str( txt(Value_) )
#	define code_token( Fmt_, ... )                gen::untyped_token_fmt( Fmt_, VA_NARGS( __VA_ARGS__), __VA_ARGS__ )
#	define code_fmt( Fmt_, ... )                  gen::untyped_fmt( Fmt_, __VA_ARGS__ )
#	define specifiers( ... )                      gen::def_specifiers( VA_NARGS( __VA_ARGS__ ), __VA_ARGS__ )
#	define type( Value_ )                         gen::def_type( txt(Value_) )
#	define type_fmt( Fmt_, ... )                  gen::def_type( bprintf( Fmt_, __VA_ARGS__ ) )
#	define using( Name_, Type_ )		          gen::def_using( #Name_, Type_ )
#	define var( Type_, Name_, ... )               gen::def_variable( Type_, #Name_, __VA_ARGS__ )

#   define make( Type_, Name_, ... )                                Code Name_ = make_##Type_( #Name_, __VA_ARGS__ );
#	define proc( Name_, Specifiers_, RetType_, Parameters_, Body_ ) Name_ = gen::def_proc( #Name_, Specifiers_, Parameters_, RetType_, Body_ )
#	define proc_body( ... )                                                 gen::def_proc_body( VA_NARGS( __VA_ARS__ ), __VA_ARGS__ )
#	define params( ... )                                                    gen::def_params( VA_NARGS( __VA_ARGS__ ) / 2, __VA_ARGS__ )
#	define struct( Name_, Parent_, Specifiers_, Body_ )             Name_ = gen::def_struct( #Name_, Body_, Parent_, Specifiers_ )
#	define struct_body( ... )                                               gen::def_struct_body( VA_NARGS( __VA_ARGS__ ), __VA_ARGS__ )

#	define add_var( Type_, Name_, ... ) add( gen::def_variable( t_##Type_, #Name_, __VA_ARGS__ ) )
#	define add_untyped( Value_ )        add( gen::untyped_str( txt( Value ) ) )
#	define add_ret_type( ... )
#	define add_params( ... )

#	define proc_code( Def_ )   gen::parse_proc( txt( Def_ ), sizeof( txt( Def_ )) )
#	define struct_code( Def_ ) gen::parse_struct( txt( Def_ ), sizeof( txt( Def_ )) )
#endif
#pragma endregion MACROS

#pragma region CONSTANTS
#ifdef GEN_DEFINE_LIBRARY_CODE_CONSTANTS 
namespace gen
{
	// Predefined typename codes.
	// These are not set until gen::init is called.
	// This just preloads a bunch of Code types into the code pool.

	extern const Code t_void;

	extern const Code t_bool;
	extern const Code t_char;
	extern const Code t_wchar_t;

	extern const Code t_s8;
	extern const Code t_s16;
	extern const Code t_s32;
	extern const Code t_s64;

	extern const Code t_u8;
	extern const Code t_u16;
	extern const Code t_u32;
	extern const Code t_u64;

	extern const Code t_sw;
	extern const Code t_uw;

	extern const Code t_f32;
	extern const Code t_f64;

	extern const Code spec_constexpr;
	extern const Code spec_inline;
}
#endif
#pragma endregion CONSTANTS
#endif
