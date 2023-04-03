/*
	gencpp: A simple staged metaprogramming library for C++.

	This library is intended for small-to midsize projects that want rapid complation times 
	for fast debugging.

	AST type checking supports only a small subset of c++. 
	See the 'ECode' namespace and 'gen API' region to see what is supported.

	There is no support for accessability fields in structs.
*/

#pragma once

#include "Bloat.hpp"

// Defined by default.

#define GEN_ENABLE_READONLY_AST
// #define GEN_DEFINE_DSL

#define gen_time
#ifdef gen_time
namespace gen
{
	#if 0
	ct sw ColumnLimit = 256;
	ct sw MaxLines    = kilobytes(256);

	using LineStr = char[ColumnLimit];
	#endif

	// Specifier Type
	enum Specifier : u8
	{
		Alignas,            // alignas(#)
		Constexpr,          // constexpr
		Inline,             // inline

		C_Linkage,          // extern "C"
		API_Import,         // Vendor specific way dynamic import symbol
		API_Export,         // Vendor specific way to dynamic export
		External_Linkage,   // extern
		Internal_Linkage,   // static (within unit file)
		Static_Member,      // static (within sturct/class)
		Local_Persist,      // static (within function)
		Thread_Local,       // thread_local

		Num_Specifiers
	};

	// Specifier to string
	inline
	char const* specifier_str( Specifier specifier )
	{
		static 
		char const* lookup[ Num_Specifiers ] = {
			"alignas",
			"constexpr",
			"inline",

			"extern \"C\"",
		#if defined(ZPL_SYSTEM_WINDOWS)
			"__declspec(dllexport)",
			"__declspec(dllimport)",
		#elif defined(ZPL_SYSTEM_MACOS)
			"__attribute__ ((visibility ("default")))",
			"__attribute__ ((visibility ("default")))",
		#endif
			"extern",
			"static",
			"static",
			"static",
			"thread_local"
		};

		return lookup[ specifier ];
	}

	// Code Type
	namespace ECode
	{
		enum Type : u8
		{
			Invalid,

			Untyped, // User provided raw string.

			Decl_Function,  // Forward a function
			Decl_Type,      // Forward a type.
			Function,       // <type> <name>( <parameters> )
			Function_Body,  // { <body> }
			Namespace,
			Namespace_Body, 
			Parameters,  // Used with functions.
			Specifiers,
			Struct,     
			Struct_Body,
			Variable,
			Typedef,
			Typename,
			Using,

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
				"Decl_type",
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
				"using"
			};

			return lookup[ type ];
		}
	}
	using CodeT = ECode::Type;

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
		bool has_entries()
		{
			static bool lookup[ ECode::Num_Types] = {
				false, // Invalid
				false, // Unused
				false, // Untyped
				true,  // Decl_Type
				true,  // Decl_Function
				true,  // Parameter
				true,  // Struct
				true,  // Function
				false, // Specifier
				true,  // Variable
				true,  // Typename
			};

			return lookup[Type];
		}

		forceinline
		bool is_invalid()
		{
			return Type != ECode::Invalid;
		}

		forceinline
		char const* type_str()
		{
			return ECode::str( Type );
		}

		string to_string();

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

		If GEN_ENABLE_READONLY_AST is defined, readonly assertions will be done on any member dreference, 
		and the 'gen API' related functions. will set their created ASTs to readonly before returning.

		Casting to AST* will bypass.
	*/
	struct Code
	{
		AST* ast;

		forceinline
		operator bool()
		{
			return ast->is_invalid();
		}

		bool operator ==( Code other )
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

	#ifdef GEN_ENABLE_READONLY_AST
		forceinline
		AST* operator ->() 
		{
			if ( ast == nullptr )
				fatal("Attempt to dereference a nullptr!");

			if ( ast->Readonly )
				fatal("Attempted to access a member from a readonly ast!");			

			return ast;
		}

		Code& operator *() = delete;
	#endif
	};

	// Used when the its desired when omission is allowed in a definition.
	ct Code UnusedCode = { nullptr };

	// Used internally for the most part to identify invaidly generated code.
	ct CodePOD InvalidCode = { ECode::Invalid, false, nullptr, nullptr, nullptr, { nullptr } };

	/*
		Type registy: Used to store Typename ASTs. Types are registered by their string literal value.

		Purely used as a memory optimization.
		Strings made with the Typename ASTs are stored in thier own arena allocator.
		TODO: Implement and replace usage of def_type.
	*/
	// ZPL_TABLE_DECLARE( ZPL_EXTERN, TypeRegistry, type_reg_, Code );

#pragma region gen API
	/*
		Initialize the library.
		This currently just initializes the CodePool.
	*/	
	void init();

	/*
		Foward Declare a type:
		<specifiers> <type> <name>;
	*/
	Code decl_type( char const* name, Code type, Code specifiers = UnusedCode );

	/*
		Foward Declare a function:
		<specifiers> <name> ( <params> );
	*/
	Code decl_fn( char const* name
		, Code specifiers
		, Code params
		, Code ret_type
	);

	/*
		Define an expression:
		< c/c++ expression >
	*/
	Code def_expression( Code value );

	/*
		Define a function:
		<specifiers> <name> ( <params> )
		{
			<body>
		}
	*/
	Code def_function( char const* name
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

		Each entry is provided an empty line separation.
	*/
	Code def_function_body( s32 num, ... );

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

		Each entry is provided an empty line separation.
	*/
	Code def_namespace_body( s32 num, ... );

	/*
		Define a set of parameters for a function:
		<name> <type>, ...
	*/
	Code def_parameters( s32 num, ... );

	/*
		Define a set of specifiers for a function, struct, type, or varaible
	*/
	Code def_specifiers( s32 num , ... );

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

		Each entry is provided an empty line separation.
	*/
	Code def_struct_body( s32 num, ... );

	/*
		Define a variable:
		<specifiers> <type> <name> = <value>;
	*/
	Code def_variable( char const* name, Code type, Code value = UnusedCode, Code specifiers = UnusedCode );

	/*
		Define a type AST value.
		Useless by itself, its intended to be used in conjunction with 
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

	/*
		Define an untyped code string.

		Untyped code may be used in bodies of functions, namespaces, or structs
		or the in places where expressions may be placed.

		Because the code within it is untyped, errors will naturally not be provided.
		Consider this an a preprocessor define.
	*/
	Code untyped_str( char const* str );

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
	Code token_fmt( char const* fmt, s32 num_tokens, ... );


	/*
		Creates a unit file.

		These represent an encapsulation of a generated file
		Used this if you need to pass around a group of Code entires at file scope level.

		The name provided is the name of the file.
	*/
	Code create_Unit( char const* name );

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

#	define __ UnusedCode

/*
	gen's Domain Specific Langauge.

	Completely optional, makes the code gen syntax less verbose..
*/
#ifdef GEN_DEFINE_DSL
#	define type( Name_, Value_ )        Code Name_ = gen::def_type( txt(Value_) )
#	define type_fmt( Name_, Fmt_, ... ) Code Name_ = gen::def_type( bprintf( Fmt_, __VA_ARGS__ ) )
#   define value( Name_, Value_ )       Code Name_ = gen::untyped_str( Value_ )
#	define specifiers( Name_, ... )     Code Name_ = gen::def_specifiers( VA_NARGS( __VA_ARGS__ ), __VA_ARGS__ )
#	define using( Name_, Type_ )		Code Name_ = gen::def_using( #Name_, Type_ )

#	define var( Name_, Type_, Value_, Specifiers_ ) \
		Code Name_ = gen::def_variable( #Name_, Type_, untyped_str( #Value_ ), Specifiers_ )

// #   define def ( Name _ ) Code Name_;

#	define params( ... ) gen::def_parameters( VA_NARGS( __VA_ARGS__ ) / 2, __VA_ARGS__ )

/*
	Defines scoped symbol.

	Used with:
	- function
	- namespace
	- struct
*/
#	define def( Name_ ) Code Name_;

#	define function( Name_, Specifiers_, ReturnType_, Parameters_, Body_ ) \
		Name_ = gen::def_function( #Name_, Specifiers_, Parameters_, ReturnType_, Body_ )

#	define function_body( ... ) \
		gen::def_function_body( VA_NARGS( __VA_ARS__ ), __VA_ARGS__ )
	
#	define struct( Name_, Parent_, Specifiers_, Body_ ) \
		Name_ = gen::def_struct( #Name_, Body_, Parent_, Specifiers_ )

#	define struct_body( ... ) \
		gen::def_struct_body( VA_NARGS( __VA_ARGS__ ), __VA_ARGS__ )
#endif
#pragma endregion MACROS

#pragma region CONSTANTS
namespace gen
{
	// Predefined typename codes.

	extern const Code t_bool;
	extern const Code t_sw;
	extern const Code t_uw;

	extern const Code spec_inline;
}
#pragma endregion CONSTANTS
#endif
