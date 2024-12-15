#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "components/types.hpp"
#endif

// This file was generated automatially by gencpp's bootstrap.cpp (See: https://github.com/Ed94/gencpp)

enum Specifier : u32
{
	Spec_Invalid,
	Spec_Consteval,
	Spec_Constexpr,
	Spec_Constinit,
	Spec_Explicit,
	Spec_External_Linkage,
	Spec_ForceInline,
	Spec_Global,
	Spec_Inline,
	Spec_Internal_Linkage,
	Spec_Local_Persist,
	Spec_Mutable,
	Spec_NeverInline,
	Spec_Ptr,
	Spec_Ref,
	Spec_Register,
	Spec_RValue,
	Spec_Static,
	Spec_Thread_Local,
	Spec_Virtual,
	Spec_Const,
	Spec_Final,
	Spec_NoExceptions,
	Spec_Override,
	Spec_Pure,
	Spec_Volatile,
	Spec_NumSpecifiers,
	Spec_UnderlyingType = 0xffffffffu
};

inline Str spec_to_str( Specifier type )
{
	local_persist Str lookup[] = {
		{ "INVALID",       sizeof( "INVALID" ) - 1       },
		{ "consteval",     sizeof( "consteval" ) - 1     },
		{ "constexpr",     sizeof( "constexpr" ) - 1     },
		{ "constinit",     sizeof( "constinit" ) - 1     },
		{ "explicit",      sizeof( "explicit" ) - 1      },
		{ "extern",        sizeof( "extern" ) - 1        },
		{ "forceinline",   sizeof( "forceinline" ) - 1   },
		{ "global",        sizeof( "global" ) - 1        },
		{ "inline",        sizeof( "inline" ) - 1        },
		{ "internal",      sizeof( "internal" ) - 1      },
		{ "local_persist", sizeof( "local_persist" ) - 1 },
		{ "mutable",       sizeof( "mutable" ) - 1       },
		{ "neverinline",   sizeof( "neverinline" ) - 1   },
		{ "*",             sizeof( "*" ) - 1             },
		{ "&",             sizeof( "&" ) - 1             },
		{ "register",      sizeof( "register" ) - 1      },
		{ "&&",            sizeof( "&&" ) - 1            },
		{ "static",        sizeof( "static" ) - 1        },
		{ "thread_local",  sizeof( "thread_local" ) - 1  },
		{ "virtual",       sizeof( "virtual" ) - 1       },
		{ "const",         sizeof( "const" ) - 1         },
		{ "final",         sizeof( "final" ) - 1         },
		{ "noexcept",      sizeof( "noexcept" ) - 1      },
		{ "override",      sizeof( "override" ) - 1      },
		{ "= 0",           sizeof( "= 0" ) - 1           },
		{ "volatile",      sizeof( "volatile" ) - 1      },
	};
	return lookup[type];
}

inline bool spec_is_trailing( Specifier specifier )
{
	return specifier > Spec_Virtual;
}

inline Specifier str_to_specifier( Str str )
{
	local_persist u32 keymap[Spec_NumSpecifiers];
	do_once_start for ( u32 index = 0; index < Spec_NumSpecifiers; index++ )
	{
		Str enum_str  = spec_to_str( (Specifier)index );
		keymap[index] = crc32( enum_str.Ptr, enum_str.Len );
	}
	do_once_end u32 hash = crc32( str.Ptr, str.Len );
	for ( u32 index = 0; index < Spec_NumSpecifiers; index++ )
	{
		if ( keymap[index] == hash )
			return (Specifier)index;
	}
	return Spec_Invalid;
}

forceinline Str to_str( Specifier spec )
{
	return spec_to_str( spec );
}

forceinline Specifier to_type( Str str )
{
	return str_to_specifier( str );
}

forceinline bool is_trailing( Specifier specifier )
{
	return spec_is_trailing( specifier );
}
