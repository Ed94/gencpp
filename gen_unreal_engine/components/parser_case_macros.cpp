// These macros are used in the swtich cases within parser.cpp

#define GEN_PARSER_CLASS_STRUCT_BODY_ALLOWED_MEMBER_TOK_SPECIFIER_CASES \
case Tok_Spec_Consteval:              \
case Tok_Spec_Constexpr:              \
case Tok_Spec_Constinit:              \
case Tok_Spec_Explicit:               \
case Tok_Spec_ForceInline:            \
case Tok_Spec_ForceInline_Debuggable: \
case Tok_Spec_Inline:                 \
case Tok_Spec_Mutable:                \
case Tok_Spec_NeverInline:            \
case Tok_Spec_Static:                 \
case Tok_Spec_Volatile:               \
case Tok_Spec_Virtual

#define GEN_PARSER_CLASS_STRUCT_BODY_ALLOWED_MEMBER_SPECIFIER_CASES \
case Spec_Constexpr:              \
case Spec_Constinit:              \
case Spec_Explicit:               \
case Spec_Inline:                 \
case Spec_ForceInline:            \
case Spec_ForceInline_Debuggable: \
case Spec_Mutable:                \
case Spec_NeverInline:            \
case Spec_Static:                 \
case Spec_Volatile:               \
case Spec_Virtual

#define GEN_PARSER_CLASS_GLOBAL_NSPACE_ALLOWED_MEMBER_TOK_SPECIFIER_CASES \
case Tok_Spec_Consteval:              \
case Tok_Spec_Constexpr:              \
case Tok_Spec_Constinit:              \
case Tok_Spec_Extern:                 \
case Tok_Spec_ForceInline:            \
case Tok_Spec_ForceInline_Debuggable: \
case Tok_Spec_Global:                 \
case Tok_Spec_Inline:                 \
case Tok_Spec_Internal_Linkage:       \
case Tok_Spec_NeverInline:            \
case Tok_Spec_Static

#define GEN_PARSER_CLASS_GLOBAL_NSPACE_ALLOWED_MEMBER_SPECIFIER_CASES \
case Spec_Constexpr:              \
case Spec_Constinit:              \
case Spec_ForceInline:            \
case Spec_ForceInline_Debuggable: \
case Spec_Global:                 \
case Spec_External_Linkage:       \
case Spec_Internal_Linkage:       \
case Spec_Inline:                 \
case Spec_Mutable:                \
case Spec_NeverInline:            \
case Spec_Static:                 \
case Spec_Volatile

#define GEN_PARSER_FRIEND_ALLOWED_SPECIFIER_CASES \
case Spec_Const:       \
case Spec_Inline:      \
case Spec_ForceInline

#define GEN_PARSER_FUNCTION_ALLOWED_SPECIFIER_CASES \
case Spec_Const:                  \
case Spec_Consteval:              \
case Spec_Constexpr:              \
case Spec_External_Linkage:       \
case Spec_Internal_Linkage:       \
case Spec_ForceInline:            \
case Spec_ForceInline_Debuggable: \
case Spec_Inline:                 \
case Spec_NeverInline:            \
case Spec_Static

#define GEN_PARSER_OPERATOR_ALLOWED_SPECIFIER_CASES \
case Spec_Const:       \
case Spec_Constexpr:   \
case Spec_ForceInline: \
case Spec_Inline:      \
case Spec_NeverInline: \
case Spec_Static

#define GEN_PARSER_TEMPLATE_ALLOWED_SPECIFIER_CASES \
case Spec_Const:                  \
case Spec_Constexpr:              \
case Spec_Constinit:              \
case Spec_External_Linkage:       \
case Spec_Global:                 \
case Spec_Inline:                 \
case Spec_ForceInline:            \
case Spec_ForceInline_Debuggable: \
case Spec_Local_Persist:          \
case Spec_Mutable:                \
case Spec_Static:                 \
case Spec_Thread_Local:           \
case Spec_Volatile

#define GEN_PARSER_VARIABLE_ALLOWED_SPECIFIER_CASES \
case Spec_Const:            \
case Spec_Constexpr:        \
case Spec_Constinit:        \
case Spec_External_Linkage: \
case Spec_Global:           \
case Spec_Inline:           \
case Spec_Local_Persist:    \
case Spec_Mutable:          \
case Spec_Static:           \
case Spec_Thread_Local:     \
case Spec_Volatile

#define GEN_PARSER_TYPENAME_ALLOWED_SUFFIX_SPECIFIER_CASES \
case Spec_Const:    \
case Spec_Ptr:      \
case Spec_Ref:      \
case Spec_RValue
