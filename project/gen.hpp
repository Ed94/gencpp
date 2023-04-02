#pragma once

#include "Bloat.hpp"

#ifdef gen_time
namespace gen
{
	ct sw ColumnLimit = 256;
	ct sw MaxLines    = kilobytes(256);

	using LineStr = char[ColumnLimit];

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

	char const* specifier_str( Specifier specifier )
	{
		static char const* lookup[ Num_Specifiers ] = {
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
			"thread_local"
		};

		return lookup[ specifier ];
	}

	struct Code
	{
		enum EType : u8
		{
			Invalid,
			Unused,

			Untyped, // User provided raw string.

			Decl_Type,
			Decl_Function,
			Parameters,  // Used with functions.
			Struct,     
			Struct_Body,
			Function,
			Function_Body,
			Specifiers,
			Variable,
			Typename,

			Num_Types
		};

	#pragma region Member API
		void comment( string value )
		{
			Comment = value;
		}

		forceinline
		void add( Code other )
		{
			array_append( Entries, other );
		}

		forceinline
		void add( array(Code) other )
		{
			array_appendv( Entries, other, sizeof(other) );
		}

		forceinline
		void add( Code* entries, u32 num_entries )
		{
			array_appendv( Entries, entries, num_entries );
		}

		forceinline
		bool has_entries()
		{
			static bool lookup[Num_Types] = {
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

		string to_string();

		forceinline
		operator bool()
		{
			return Type != Invalid;
		}

		operator char const*()
		{
			return to_string();
		}

	#if 0
		bool operator ==( Code& other )
		{
			bool children_equal = true;

			#define is( Value_ ) Type == Value_

			if ( has_children() )
			{
				u32 left = array_count( Children );
				do
				{
					
				}
				while ( left--, left > 0 )
			}

			return 
				Type == other.Type 
			&&	Name == other.Name
			&&  children_equal
			;
		}
	#endif
	#pragma endregion Member API

	#define Using_Code_POD         \
		Code::EType       Type;    \
		string            Name;    \
		string            Comment; \
		union {                    \
			array(Code)   Entries; \
			string        Content; \
		};

		Using_Code_POD;
	};

	using CodeType = Code::EType;

	struct Code_POD
	{
		Using_Code_POD;
	};

	constexpr Code UnusedCode = { Code::Unused, nullptr, nullptr, { nullptr }  };

	void init();

	Code decl_type( char const* name, Code type, Code specifiers = UnusedCode );

	Code decl_fn( char const* name
		, Code specifiers
		, Code params
		, Code ret_type
	);

	Code def_parameters( s32 num, ... );

	Code def_function( char const* name
		, Code specifiers
		, Code params
		, Code ret_type
		, Code body 
	);
	Code def_function_body( u32 num, ... );

	Code def_namespace( char const* name, Code body );
	Code def_namespace_body( u32 num, ... );

	Code def_specifiers( u32 num , ... );

	Code def_struct( char const* name, Code body, Code parent = UnusedCode, Code specifiers = UnusedCode );
	Code def_struct_body( u32 num, ... );

	Code def_variable( char const* name, Code type, Code value = UnusedCode, Code specifiers = UnusedCode );

	Code def_type( char const* name );

	Code def_using( char const* name, Code type );

	Code untyped_fmt( char const* fmt, ... );

	Code token_fmt( char const* fmt, ... );


	struct Builder
	{
		zpl_file File;
		string   Buffer;

		void print( Code );

		bool open( char const* path );
		void write();
	};
}

#define gen_main main
#endif
