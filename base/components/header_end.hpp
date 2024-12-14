#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "inlines.hpp"
#include "gen/ast_inlines.hpp"
#endif

#pragma region Constants

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
	extern Context* _ctx;

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
