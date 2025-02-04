#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#	include "interface.hpp"
#endif

#pragma region Constants
// Predefined typename codes. Are set to readonly and are setup during gen::init()

GEN_API extern Macro enum_underlying_macro;

GEN_API extern Code access_public;
GEN_API extern Code access_protected;
GEN_API extern Code access_private;

GEN_API extern CodeAttributes attrib_api_export;
GEN_API extern CodeAttributes attrib_api_import;

GEN_API extern Code module_global_fragment;
GEN_API extern Code module_private_fragment;

GEN_API extern Code fmt_newline;

GEN_API extern CodePragma pragma_once;

GEN_API extern CodeParams param_varadic;

GEN_API extern CodePreprocessCond preprocess_else;
GEN_API extern CodePreprocessCond preprocess_endif;

GEN_API extern CodeSpecifiers spec_const;
GEN_API extern CodeSpecifiers spec_consteval;
GEN_API extern CodeSpecifiers spec_constexpr;
GEN_API extern CodeSpecifiers spec_constinit;
GEN_API extern CodeSpecifiers spec_extern_linkage;
GEN_API extern CodeSpecifiers spec_final;
GEN_API extern CodeSpecifiers spec_forceinline;
GEN_API extern CodeSpecifiers spec_global;
GEN_API extern CodeSpecifiers spec_inline;
GEN_API extern CodeSpecifiers spec_internal_linkage;
GEN_API extern CodeSpecifiers spec_local_persist;
GEN_API extern CodeSpecifiers spec_mutable;
GEN_API extern CodeSpecifiers spec_neverinline;
GEN_API extern CodeSpecifiers spec_noexcept;
GEN_API extern CodeSpecifiers spec_override;
GEN_API extern CodeSpecifiers spec_ptr;
GEN_API extern CodeSpecifiers spec_pure;
GEN_API extern CodeSpecifiers spec_ref;
GEN_API extern CodeSpecifiers spec_register;
GEN_API extern CodeSpecifiers spec_rvalue;
GEN_API extern CodeSpecifiers spec_static_member;
GEN_API extern CodeSpecifiers spec_thread_local;
GEN_API extern CodeSpecifiers spec_virtual;
GEN_API extern CodeSpecifiers spec_volatile;

GEN_API extern CodeTypename t_empty; // Used with varaidc parameters. (Exposing just in case its useful for another circumstance)
GEN_API extern CodeTypename t_auto;
GEN_API extern CodeTypename t_void;
GEN_API extern CodeTypename t_int;
GEN_API extern CodeTypename t_bool;
GEN_API extern CodeTypename t_char;
GEN_API extern CodeTypename t_wchar_t;
GEN_API extern CodeTypename t_class;
GEN_API extern CodeTypename t_typename;

#ifdef GEN_DEFINE_LIBRARY_CODE_CONSTANTS
	GEN_API extern CodeTypename t_b32;

	GEN_API extern CodeTypename t_s8;
	GEN_API extern CodeTypename t_s16;
	GEN_API extern CodeTypename t_s32;
	GEN_API extern CodeTypename t_s64;

	GEN_API extern CodeTypename t_u8;
	GEN_API extern CodeTypename t_u16;
	GEN_API extern CodeTypename t_u32;
	GEN_API extern CodeTypename t_u64;

	GEN_API extern CodeTypename t_ssize;
	GEN_API extern CodeTypename t_usize;

	GEN_API extern CodeTypename t_f32;
	GEN_API extern CodeTypename t_f64;
#endif

#pragma endregion Constants
