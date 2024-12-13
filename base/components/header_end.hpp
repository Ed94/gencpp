#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "inlines.hpp"
#include "gen/ast_inlines.hpp"
#endif

#pragma region Constants

#ifndef GEN_GLOBAL_BUCKET_SIZE
#	define GEN_GLOBAL_BUCKET_SIZE megabytes(8)
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
#ifndef TokenMap_FixedArena
#	define TokenMap_FixedArena FixedArena_8KB
#endif
#ifndef GEN_LEX_ALLOCATOR_SIZE
#	define GEN_LEX_ALLOCATOR_SIZE megabytes(4)
#endif
#ifndef GEN_BUILDER_STR_BUFFER_RESERVE
#	define GEN_BUILDER_STR_BUFFER_RESERVE megabytes(2)
#endif

// These constexprs are used for allocation behavior of data structures
// or string handling while constructing or serializing.
// Change them to suit your needs.

constexpr s32 InitSize_DataArrays = 16;

// NOTE: This limits the maximum size of an allocation
// If you are generating a string larger than this, increase the size of the bucket here.
constexpr usize  Global_BucketSize      = GEN_GLOBAL_BUCKET_SIZE;
constexpr s32 CodePool_NumBlocks        = GEN_CODEPOOL_NUM_BLOCKS;
constexpr s32 SizePer_StringArena       = GEN_SIZE_PER_STRING_ARENA;

constexpr s32 MaxCommentLineLength      = GEN_MAX_COMMENT_LINE_LENGTH;
constexpr s32 MaxNameLength             = GEN_MAX_NAME_LENGTH;
constexpr s32 MaxUntypedStrLength       = GEN_MAX_UNTYPED_STR_LENGTH;
// constexpr s32 TokenFmt_TokenMap_MemSize	= GEN_TOKEN_FMT_TOKEN_MAP_MEM_SIZE;
constexpr s32 LexAllocator_Size         = GEN_LEX_ALLOCATOR_SIZE;
constexpr s32 Builder_StrBufferReserve  = GEN_BUILDER_STR_BUFFER_RESERVE;

extern Str enum_underlying_sig;

extern Code access_public;
extern Code access_protected;
extern Code access_private;

extern CodeAttributes attrib_api_export;
extern CodeAttributes attrib_api_import;

extern Code module_global_fragment;
extern Code module_private_fragment;

extern Code fmt_newline;

extern CodePragma pragma_once;

extern CodeParams param_varadic;

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
extern CodeSpecifiers spec_noexcept;
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

extern CodeTypename t_empty; // Used with varaidc parameters. (Exposing just in case its useful for another circumstance)
extern CodeTypename t_auto;
extern CodeTypename t_void;
extern CodeTypename t_int;
extern CodeTypename t_bool;
extern CodeTypename t_char;
extern CodeTypename t_wchar_t;
extern CodeTypename t_class;
extern CodeTypename t_typename;

#ifdef GEN_DEFINE_LIBRARY_CODE_CONSTANTS
	// Predefined typename codes. Are set to readonly and are setup during gen::init()

	extern CodeTypename t_b32;

	extern CodeTypename t_s8;
	extern CodeTypename t_s16;
	extern CodeTypename t_s32;
	extern CodeTypename t_s64;

	extern CodeTypename t_u8;
	extern CodeTypename t_u16;
	extern CodeTypename t_u32;
	extern CodeTypename t_u64;

	extern CodeTypename t_ssize;
	extern CodeTypename t_usize;

	extern CodeTypename t_f32;
	extern CodeTypename t_f64;
#endif

#pragma endregion Constants

// Used by the lexer to persistently treat all these identifiers as preprocessor defines.
// Populate with strings via gen::get_cached_string.
// Functional defines must have format: id( ;at minimum to indicate that the define is only valid with arguments.
extern Array(StringCached) PreprocessorDefines;

#ifdef GEN_EXPOSE_BACKEND
	// Global allocator used for data with process lifetime.
	extern AllocatorInfo  GlobalAllocator;
	extern Array(Arena) Global_AllocatorBuckets;

	extern Array(Pool)  CodePools;
	extern Array(Arena) StringArenas;

	extern StringTable StringCache;

	extern Arena LexArena;

	extern AllocatorInfo Allocator_DataArrays;
	extern AllocatorInfo Allocator_CodePool;
	extern AllocatorInfo Allocator_Lexer;
	extern AllocatorInfo Allocator_StringArena;
	extern AllocatorInfo Allocator_StringTable;
	extern AllocatorInfo Allocator_TypeTable;
#endif
