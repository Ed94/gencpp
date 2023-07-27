// This is the non-bootstraped version of the ESpecifier. This will be obsolete once bootstrap is stress tested.

namespace ESpecifier
{
/*
	Note: The following are handled separately:
	attributes
	alignas
*/

#	define Define_Specifiers                     \
	Entry( Invalid,          INVALID )           \
	Entry( Consteval,        consteval )         \
	Entry( Constexpr,        constexpr )         \
	Entry( Constinit,        constinit )         \
	Entry( Explicit,         explicit )          \
	Entry( External_Linkage, extern )            \
	Entry( Global,           global )            \
	Entry( Inline,           inline )            \
	Entry( Internal_Linkage, internal )          \
	Entry( Local_Persist,    local_persist )     \
	Entry( Mutable,          mutable )           \
	Entry( Ptr,              * )                 \
	Entry( Ref,              & )                 \
	Entry( Register,         register )          \
	Entry( RValue,           && )                \
	Entry( Static,           static  )           \
	Entry( Thread_Local,     thread_local )      \
	Entry( Volatile,         volatile )          \
	Entry( Virtual,          virtual )           \
	Entry( Const,            const )             \
	Entry( Final,            final )             \
	Entry( Override,         override )

	enum Type : u32
	{
	#	define Entry( Specifier, Code ) Specifier,
		Define_Specifiers
	#	undef Entry

		Num_Specifiers,
	};

	inline
	bool is_trailing( Type specifier )
	{
		return specifier > Virtual;
	}

	// Specifier to string
	inline
	StrC to_str( Type specifier )
	{
		local_persist
		StrC lookup[ Num_Specifiers ] = {
		#	pragma push_macro( "global" )
		#	pragma push_macro( "internal" )
		#	pragma push_macro( "local_persist" )
		#	undef global
		#	undef internal
		#	undef local_persist

		#	define Entry( Spec_, Code_ ) { sizeof(stringize(Code_)), stringize(Code_) },
			Define_Specifiers
		#	undef Entry

		#	pragma pop_macro( "global" )
		#	pragma pop_macro( "internal" )
		#	pragma pop_macro( "local_persist" )
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

				// We subtract 1 to remove the null terminator
				// This is because the tokens lexed are not null terminated.
				keymap[index] = crc32( enum_str.Ptr, enum_str.Len - 1);
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

#	undef Define_Specifiers
}
using SpecifierT = ESpecifier::Type;