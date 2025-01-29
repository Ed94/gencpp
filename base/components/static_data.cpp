#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "../gen.hpp"
#endif

#pragma region StaticData
GEN_API global Context* _ctx;
GEN_API global u32      context_counter;

#pragma region Constants
GEN_API global Macro enum_underlying_macro;

GEN_API global Code Code_Global;
GEN_API global Code Code_Invalid;

GEN_API global Code access_public;
GEN_API global Code access_protected;
GEN_API global Code access_private;

GEN_API global CodeAttributes attrib_api_export;
GEN_API global CodeAttributes attrib_api_import;

GEN_API global Code module_global_fragment;
GEN_API global Code module_private_fragment;

GEN_API global Code fmt_newline;

GEN_API global CodeParams param_varadic;

GEN_API global CodePragma pragma_once;

GEN_API global CodePreprocessCond preprocess_else;
GEN_API global CodePreprocessCond preprocess_endif;

GEN_API global CodeSpecifiers spec_const;
GEN_API global CodeSpecifiers spec_consteval;
GEN_API global CodeSpecifiers spec_constexpr;
GEN_API global CodeSpecifiers spec_constinit;
GEN_API global CodeSpecifiers spec_extern_linkage;
GEN_API global CodeSpecifiers spec_final;
GEN_API global CodeSpecifiers spec_forceinline;
GEN_API global CodeSpecifiers spec_global;
GEN_API global CodeSpecifiers spec_inline;
GEN_API global CodeSpecifiers spec_internal_linkage;
GEN_API global CodeSpecifiers spec_local_persist;
GEN_API global CodeSpecifiers spec_mutable;
GEN_API global CodeSpecifiers spec_noexcept;
GEN_API global CodeSpecifiers spec_neverinline;
GEN_API global CodeSpecifiers spec_override;
GEN_API global CodeSpecifiers spec_ptr;
GEN_API global CodeSpecifiers spec_pure;
GEN_API global CodeSpecifiers spec_ref;
GEN_API global CodeSpecifiers spec_register;
GEN_API global CodeSpecifiers spec_rvalue;
GEN_API global CodeSpecifiers spec_static_member;
GEN_API global CodeSpecifiers spec_thread_local;
GEN_API global CodeSpecifiers spec_virtual;
GEN_API global CodeSpecifiers spec_volatile;

GEN_API global CodeTypename t_empty;
GEN_API global CodeTypename t_auto;
GEN_API global CodeTypename t_void;
GEN_API global CodeTypename t_int;
GEN_API global CodeTypename t_bool;
GEN_API global CodeTypename t_char;
GEN_API global CodeTypename t_wchar_t;
GEN_API global CodeTypename t_class;
GEN_API global CodeTypename t_typename;

#ifdef GEN_DEFINE_LIBRARY_CODE_CONSTANTS
GEN_API global CodeTypename t_b32;

GEN_API global CodeTypename t_s8;
GEN_API global CodeTypename t_s16;
GEN_API global CodeTypename t_s32;
GEN_API global CodeTypename t_s64;

GEN_API global CodeTypename t_u8;
GEN_API global CodeTypename t_u16;
GEN_API global CodeTypename t_u32;
GEN_API global CodeTypename t_u64;

GEN_API global CodeTypename t_ssize;
GEN_API global CodeTypename t_usize;

GEN_API global CodeTypename t_f32;
GEN_API global CodeTypename t_f64;
#endif

#pragma endregion Constants

#pragma endregion StaticData
