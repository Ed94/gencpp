#pragma once

// This is the non-bootstraped version of the ECode. This will be obsolete once bootstrap is stress tested.

namespace ECode
{
#	define Define_Types           \
	Entry( Invalid )              \
	Entry( Untyped )              \
	Entry( NewLine )              \
	Entry( Comment )              \
	Entry( Access_Private )       \
	Entry( Access_Protected )     \
	Entry( Access_Public )        \
	Entry( PlatformAttributes )   \
	Entry( Class )                \
	Entry( Class_Fwd )            \
	Entry( Class_Body )           \
	Entry( Constructor )          \
	Entry( Constructor_Fwd )      \
	Entry( Destructor )		      \
	Entry( Destructor_Fwd )       \
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
	Entry( Operator_Cast )		  \
	Entry( Operator_Cast_Fwd )    \
	Entry( Parameters )           \
	Entry( Preprocess_Define )    \
	Entry( Preprocess_If )        \
	Entry( Preprocess_IfDef )     \
	Entry( Preprocess_IfNotDef )  \
	Entry( Preprocess_ElIf )      \
	Entry( Preprocess_Else )      \
	Entry( Preprocess_EndIf )     \
	Entry( Preprocess_Include )   \
	Entry( Preprocess_Pragma )    \
	Entry( Specifiers )           \
	Entry( Struct )               \
	Entry( Struct_Fwd )           \
	Entry( Struct_Body )          \
	Entry( Template )             \
	Entry( Typedef )              \
	Entry( Typename )             \
	Entry( Union )                \
	Entry( Union_Body)            \
	Entry( Using )                \
	Entry( Using_Namespace )      \
	Entry( Variable )

	enum Type : u32
	{
	#	define Entry( Type ) Type,
		Define_Types
	#	undef Entry

		Num_Types
	};

	inline
	StrC to_str( Type type )
	{
		static
		StrC lookup[Num_Types] = {
		#	define Entry( Type ) { sizeof(stringize(Type)), stringize(Type) },
			Define_Types
		#	undef Entry
		};

		return lookup[ type ];
	}

#	undef Define_Types
}
using CodeT = ECode::Type;
using ECode::to_str;

