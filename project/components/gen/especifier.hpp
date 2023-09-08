#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#endif

// This file was generated automatially by gencpp's bootstrap.cpp (See: https://github.com/Ed94/gencpp)

namespace ESpecifier
{
	enum Type : u32
	{
		Invalid,
		Consteval,
		Constexpr,
		Constinit,
		Explicit,
		External_Linkage,
		ForceInline,
		Global,
		Inline,
		Internal_Linkage,
		Local_Persist,
		Mutable,
		NeverInline,
		Ptr,
		Ref,
		Register,
		RValue,
		Static,
		Thread_Local,
		Volatile,
		Virtual,
		Const,
		Final,
		NoExceptions,
		Override,
		Pure,
		NumSpecifiers
	};

	bool is_trailing( Type specifier )
	{
		return specifier > Virtual;
	}

	StrC to_str( Type type )
	{
		local_persist StrC lookup[] {
			{sizeof( "INVALID" ),        "INVALID"      },
			{ sizeof( "consteval" ),     "consteval"    },
			{ sizeof( "constexpr" ),     "constexpr"    },
			{ sizeof( "constinit" ),     "constinit"    },
			{ sizeof( "explicit" ),      "explicit"     },
			{ sizeof( "extern" ),        "extern"       },
			{ sizeof( "forceinline" ),   "forceinline"  },
			{ sizeof( "global" ),        "global"       },
			{ sizeof( "inline" ),        "inline"       },
			{ sizeof( "internal" ),      "internal"     },
			{ sizeof( "local_persist" ), "local_persist"},
			{ sizeof( "mutable" ),       "mutable"      },
			{ sizeof( "neverinline" ),   "neverinline"  },
			{ sizeof( "*" ),             "*"            },
			{ sizeof( "&" ),             "&"            },
			{ sizeof( "register" ),      "register"     },
			{ sizeof( "&&" ),            "&&"           },
			{ sizeof( "static" ),        "static"       },
			{ sizeof( "thread_local" ),  "thread_local" },
			{ sizeof( "volatile" ),      "volatile"     },
			{ sizeof( "virtual" ),       "virtual"      },
			{ sizeof( "const" ),         "const"        },
			{ sizeof( "final" ),         "final"        },
			{ sizeof( "noexcept" ),      "noexcept"     },
			{ sizeof( "override" ),      "override"     },
			{ sizeof( "= 0" ),           "= 0"          },
		};
		return lookup[ type ];
	}

	Type to_type( StrC str )
	{
		local_persist u32 keymap[ NumSpecifiers ];
		do_once_start for ( u32 index = 0; index < NumSpecifiers; index++ )
		{
			StrC enum_str   = to_str( ( Type )index );
			keymap[ index ] = crc32( enum_str.Ptr, enum_str.Len - 1 );
		}
		do_once_end u32 hash = crc32( str.Ptr, str.Len );
		for ( u32 index = 0; index < NumSpecifiers; index++ )
		{
			if ( keymap[ index ] == hash )
				return ( Type )index;
		}
		return Invalid;
	}

}    // namespace ESpecifier

using SpecifierT = ESpecifier::Type;
