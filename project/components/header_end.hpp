#pragma once
#include "inlines.hpp"
#include "gen/ast_inlines.hpp"

#pragma region Constants

#ifndef GEN_GLOBAL_BUCKET_SIZE
#	define GEN_GLOBAL_BUCKET_SIZE megabytes(4)
#endif
#ifndef GEN_CODEPOOL_NUM_BLOCKS
#	define GEN_CODEPOOL_NUM_BLOCKS kilobytes(16)
#endif
#ifndef GEN_SIZE_PER_STRING_ARENA
#	define GEN_SIZE_PER_STRING_ARENA megabytes(1)
#endif
#ifndef GEN_MAX_COMMENT_LINE_LENGTH
#	define GEN_MAX_COMMENT_LINE_LENGTH 1024
#endif
#ifndef GEN_MAX_NAME_LENGTH
#	define GEN_MAX_NAME_LENGTH 128
#endif
#ifndef GEN_MAX_UNTYPED_STR_LENGTH
#	define GEN_MAX_UNTYPED_STR_LENGTH megabytes(1)
#endif
#ifndef GEN_TOKEN_FMT_TOKEN_MAP_MEM_SIZE
#	define GEN_TOKEN_FMT_TOKEN_MAP_MEM_SIZE kilobytes(4)
#endif
#ifndef GEN_LEX_ALLOCATOR_SIZE
#	define GEN_LEX_ALLOCATOR_SIZE megabytes(4)
#endif
#ifndef GEN_BUILDER_STR_BUFFER_RESERVE
#	define GEN_BUILDER_STR_BUFFER_RESERVE megabytes(1)
#endif

// These constexprs are used for allocation behavior of data structures
// or string handling while constructing or serializing.
// Change them to suit your needs.

constexpr s32 InitSize_DataArrays = 16;

// NOTE: This limits the maximum size of an allocation
// If you are generating a string larger than this, increase the size of the bucket here.
constexpr uw  Global_BucketSize         = GEN_GLOBAL_BUCKET_SIZE;
constexpr s32 CodePool_NumBlocks        = GEN_CODEPOOL_NUM_BLOCKS;
constexpr s32 SizePer_StringArena       = GEN_SIZE_PER_STRING_ARENA;

constexpr s32 MaxCommentLineLength      = GEN_MAX_COMMENT_LINE_LENGTH;
constexpr s32 MaxNameLength             = GEN_MAX_NAME_LENGTH;
constexpr s32 MaxUntypedStrLength       = GEN_MAX_UNTYPED_STR_LENGTH;
constexpr s32 TokenFmt_TokenMap_MemSize	= GEN_TOKEN_FMT_TOKEN_MAP_MEM_SIZE;
constexpr s32 LexAllocator_Size         = GEN_LEX_ALLOCATOR_SIZE;
constexpr s32 Builder_StrBufferReserve  = GEN_BUILDER_STR_BUFFER_RESERVE;

extern Code access_public;
extern Code access_protected;
extern Code access_private;

extern CodeAttributes attrib_api_export;
extern CodeAttributes attrib_api_import;

extern Code module_global_fragment;
extern Code module_private_fragment;

// Exposed, but this is really used for parsing.
extern Code fmt_newline;

extern CodePragma pragma_once;

extern CodeParam param_varadic;

extern CodePreprocessCond preprocess_else;
extern CodePreprocessCond preprocess_endif;

extern CodeSpecifiers spec_const;
extern CodeSpecifiers spec_consteval;
extern CodeSpecifiers spec_constexpr;
extern CodeSpecifiers spec_constinit;
extern CodeSpecifiers spec_extern_linkage;
extern CodeSpecifiers spec_final;
extern CodeSpecifiers spec_forceinline;
extern CodeSpecifiers spec_global;
extern CodeSpecifiers spec_inline;
extern CodeSpecifiers spec_internal_linkage;
extern CodeSpecifiers spec_local_persist;
extern CodeSpecifiers spec_mutable;
extern CodeSpecifiers spec_neverinline;
extern CodeSpecifiers spec_override;
extern CodeSpecifiers spec_ptr;
extern CodeSpecifiers spec_pure;
extern CodeSpecifiers spec_ref;
extern CodeSpecifiers spec_register;
extern CodeSpecifiers spec_rvalue;
extern CodeSpecifiers spec_static_member;
extern CodeSpecifiers spec_thread_local;
extern CodeSpecifiers spec_virtual;
extern CodeSpecifiers spec_volatile;

extern CodeType t_empty; // Used with varaidc parameters. (Exposing just in case its useful for another circumstance)
extern CodeType t_auto;
extern CodeType t_void;
extern CodeType t_int;
extern CodeType t_bool;
extern CodeType t_char;
extern CodeType t_wchar_t;
extern CodeType t_class;
extern CodeType t_typename;

#ifdef GEN_DEFINE_LIBRARY_CODE_CONSTANTS
	// Predefined typename codes. Are set to readonly and are setup during gen::init()

	extern CodeType t_b32;

	extern CodeType t_s8;
	extern CodeType t_s16;
	extern CodeType t_s32;
	extern CodeType t_s64;

	extern CodeType t_u8;
	extern CodeType t_u16;
	extern CodeType t_u32;
	extern CodeType t_u64;

	extern CodeType t_sw;
	extern CodeType t_uw;

	extern CodeType t_f32;
	extern CodeType t_f64;
#endif

#pragma endregion Constants

#pragma region Macros

#	define gen_main main

#	define __ NoCode

	//	Convienence for defining any name used with the gen api.
	//  Lets you provide the length and string literal to the functions without the need for the DSL.
#	define name( Id_ )   { sizeof(stringize( Id_ )) - 1, stringize(Id_) }

	//  Same as name just used to indicate intention of literal for code instead of names.
#	define code( ... ) { sizeof(stringize(__VA_ARGS__)) - 1, stringize( __VA_ARGS__ ) }

#	define args( ... ) num_args( __VA_ARGS__ ), __VA_ARGS__

#	define code_str( ... ) GEN_NS untyped_str( code( __VA_ARGS__ ) )
#	define code_fmt( ... ) GEN_NS untyped_str( token_fmt( __VA_ARGS__ ) )

	// Takes a format string (char const*) and a list of tokens (StrC) and returns a StrC of the formatted string.
#	define token_fmt( ... ) GEN_NS token_fmt_impl( (num_args( __VA_ARGS__ ) + 1) / 2, __VA_ARGS__ )

#pragma endregion Macros

#ifdef GEN_EXPOSE_BACKEND

	// Global allocator used for data with process lifetime.
	extern AllocatorInfo  GlobalAllocator;
	extern Array< Arena > Global_AllocatorBuckets;

	extern Array< Pool >  CodePools;
	extern Array< Arena > StringArenas;

	extern StringTable StringCache;

	extern Arena LexArena;

	extern AllocatorInfo Allocator_DataArrays;
	extern AllocatorInfo Allocator_CodePool;
	extern AllocatorInfo Allocator_Lexer;
	extern AllocatorInfo Allocator_StringArena;
	extern AllocatorInfo Allocator_StringTable;
	extern AllocatorInfo Allocator_TypeTable;

#endif
