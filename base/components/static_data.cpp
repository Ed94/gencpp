#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "gen.hpp"
#endif

#pragma region StaticData

// TODO : Convert global allocation strategy to use a slab allocation strategy.
global AllocatorInfo  GlobalAllocator;
global Array( Arena )   Global_AllocatorBuckets;

// TODO(Ed) : Make the code pool a dynamic arena
global Array( Pool )  CodePools         = { nullptr };
global Array( Arena ) StringArenas      = { nullptr };

global StringTable StringCache;

global Arena LexArena;

global AllocatorInfo Allocator_DataArrays  = {0};
global AllocatorInfo Allocator_CodePool    = {0};
global AllocatorInfo Allocator_Lexer       = {0};
global AllocatorInfo Allocator_StringArena = {0};
global AllocatorInfo Allocator_StringTable = {0};
global AllocatorInfo Allocator_TypeTable   = {0};

#pragma endregion StaticData

#pragma region Constants

global Code access_public;
global Code access_protected;
global Code access_private;

global CodeAttributes attrib_api_export;
global CodeAttributes attrib_api_import;

global Code module_global_fragment;
global Code module_private_fragment;

global Code fmt_newline;

global CodeParam param_varadic;

global CodePragma pragma_once;

global CodePreprocessCond preprocess_else;
global CodePreprocessCond preprocess_endif;

global CodeSpecifiers spec_const;
global CodeSpecifiers spec_consteval;
global CodeSpecifiers spec_constexpr;
global CodeSpecifiers spec_constinit;
global CodeSpecifiers spec_extern_linkage;
global CodeSpecifiers spec_final;
global CodeSpecifiers spec_forceinline;
global CodeSpecifiers spec_global;
global CodeSpecifiers spec_inline;
global CodeSpecifiers spec_internal_linkage;
global CodeSpecifiers spec_local_persist;
global CodeSpecifiers spec_mutable;
global CodeSpecifiers spec_noexcept;
global CodeSpecifiers spec_neverinline;
global CodeSpecifiers spec_override;
global CodeSpecifiers spec_ptr;
global CodeSpecifiers spec_pure;
global CodeSpecifiers spec_ref;
global CodeSpecifiers spec_register;
global CodeSpecifiers spec_rvalue;
global CodeSpecifiers spec_static_member;
global CodeSpecifiers spec_thread_local;
global CodeSpecifiers spec_virtual;
global CodeSpecifiers spec_volatile;

global CodeTypename t_empty;
global CodeTypename t_auto;
global CodeTypename t_void;
global CodeTypename t_int;
global CodeTypename t_bool;
global CodeTypename t_char;
global CodeTypename t_wchar_t;
global CodeTypename t_class;
global CodeTypename t_typename;

global Array(StringCached) PreprocessorDefines;

#ifdef GEN_DEFINE_LIBRARY_CODE_CONSTANTS
global CodeTypename t_b32;

global CodeTypename t_s8;
global CodeTypename t_s16;
global CodeTypename t_s32;
global CodeTypename t_s64;

global CodeTypename t_u8;
global CodeTypename t_u16;
global CodeTypename t_u32;
global CodeTypename t_u64;

global CodeTypename t_ssize;
global CodeTypename t_usize;

global CodeTypename t_f32;
global CodeTypename t_f64;
#endif

#pragma endregion Constants