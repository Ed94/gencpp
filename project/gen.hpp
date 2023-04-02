#pragma once

#ifdef gen_time
#include "Bloat.hpp"

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
			Function,
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
			return Type == Invalid;
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

	#define Using_Code_POD           \
		Code::EType       Type;      \
		string            Name;      \
		string            Comment;   \
		union {                      \
			array(Code)   Entries;   \
			string        Content;   \
		};

		Using_Code_POD;
	};

	using CodeType = Code::EType;

	struct Code_POD
	{
		Using_Code_POD;
	};

	Code decl_type( char const* name, Code specifiers, Code type);

	Code decl_fn( char const* name
		, Code specifiers
		, Code params
		, Code ret_type
	);

	Code make_parameters( u32 num, ... );

	Code make_fmt( char const* fmt, ... );

	Code make_function( char const* name
		, Code specifiers
		, Code params
		, Code ret_type
		, Code body 
	);

	Code make_specifiers( u32 num , ... );

	// Code make_variable( char const* name, char const* type );

	// Code make_template( Code subject, u32 num_dependents, ... );

	// Code make_type( char const* name );

	// Code make_using( char const* name, char const* type );


	struct File
	{
		zpl_file file;
		string   Content;

		s32 print( Code );

		bool open( char const* Path );
		void write();
	};
}
#endif
