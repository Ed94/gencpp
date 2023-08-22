#pragma once
// #include "src_start.cpp"

#pragma region StaticData

// TODO : Convert global allocation strategy to use a slab allocation strategy.
global AllocatorInfo  GlobalAllocator;
global Array<Arena>   Global_AllocatorBuckets;

global Array< Pool >  CodePools         = { nullptr };
global Array< Arena > StringArenas      = { nullptr };

global StringTable StringCache;

global Arena LexArena;

global AllocatorInfo Allocator_DataArrays  = heap();
global AllocatorInfo Allocator_CodePool    = heap();
global AllocatorInfo Allocator_Lexer       = heap();
global AllocatorInfo Allocator_StringArena = heap();
global AllocatorInfo Allocator_StringTable = heap();
global AllocatorInfo Allocator_TypeTable   = heap();

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

global CodeType t_empty;
global CodeType t_auto;
global CodeType t_void;
global CodeType t_int;
global CodeType t_bool;
global CodeType t_char;
global CodeType t_wchar_t;
global CodeType t_class;
global CodeType t_typename;

#ifdef GEN_DEFINE_LIBRARY_CODE_CONSTANTS
global CodeType t_b32;

global CodeType t_s8;
global CodeType t_s16;
global CodeType t_s32;
global CodeType t_s64;

global CodeType t_u8;
global CodeType t_u16;
global CodeType t_u32;
global CodeType t_u64;

global CodeType t_sw;
global CodeType t_uw;

global CodeType t_f32;
global CodeType t_f64;
#endif

#pragma endregion Constants
