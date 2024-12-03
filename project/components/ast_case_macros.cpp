#	define GEN_AST_BODY_CLASS_UNALLOWED_TYPES \
	case CT_PlatformAttributes:                  \
	case CT_Class_Body:                          \
	case CT_Enum_Body:                           \
	case CT_Extern_Linkage:                      \
	case CT_Function_Body:                       \
	case CT_Function_Fwd:                        \
	case CT_Global_Body:                         \
	case CT_Namespace:                           \
	case CT_Namespace_Body:                      \
	case CT_Operator:                            \
	case CT_Operator_Fwd:                        \
	case CT_Parameters:                          \
	case CT_Specifiers:                          \
	case CT_Struct_Body:                         \
	case CT_Typename:
#	define GEN_AST_BODY_STRUCT_UNALLOWED_TYPES GEN_AST_BODY_CLASS_UNALLOWED_TYPES

#	define GEN_AST_BODY_FUNCTION_UNALLOWED_TYPES \
	case CT_Access_Public:                          \
	case CT_Access_Protected:                       \
	case CT_Access_Private:                         \
	case CT_PlatformAttributes:                     \
	case CT_Class_Body:                             \
	case CT_Enum_Body:                              \
	case CT_Extern_Linkage:                         \
	case CT_Friend:                                 \
	case CT_Function_Body:                          \
	case CT_Function_Fwd:                           \
	case CT_Global_Body:                            \
	case CT_Namespace:                              \
	case CT_Namespace_Body:                         \
	case CT_Operator:                               \
	case CT_Operator_Fwd:                           \
	case CT_Operator_Member:                        \
	case CT_Operator_Member_Fwd:                    \
	case CT_Parameters:                             \
	case CT_Specifiers:                             \
	case CT_Struct_Body:                            \
	case CT_Typename:

#	define GEN_AST_BODY_GLOBAL_UNALLOWED_TYPES     \
	case CT_Access_Public:                         \
	case CT_Access_Protected:                      \
	case CT_Access_Private:                        \
	case CT_PlatformAttributes:                    \
	case CT_Class_Body:                            \
	case CT_Enum_Body:                             \
	case CT_Execution:                             \
	case CT_Friend:                                \
	case CT_Function_Body:                         \
	case CT_Namespace_Body:                        \
	case CT_Operator_Member:                       \
	case CT_Operator_Member_Fwd:                   \
	case CT_Parameters:                            \
	case CT_Specifiers:                            \
	case CT_Struct_Body:                           \
	case CT_Typename:
#	define GEN_AST_BODY_EXPORT_UNALLOWED_TYPES         GEN_AST_BODY_GLOBAL_UNALLOWED_TYPES
#	define GEN_AST_BODY_EXTERN_LINKAGE_UNALLOWED_TYPES GEN_AST_BODY_GLOBAL_UNALLOWED_TYPES

#	define GEN_AST_BODY_NAMESPACE_UNALLOWED_TYPES \
	case CT_Access_Public:                        \
	case CT_Access_Protected:                     \
	case CT_Access_Private:                       \
	case CT_PlatformAttributes:                   \
	case CT_Class_Body:                           \
	case CT_Enum_Body:                            \
	case CT_Execution:                            \
	case CT_Friend:                               \
	case CT_Function_Body:                        \
	case CT_Namespace_Body:                       \
	case CT_Operator_Member:                      \
	case CT_Operator_Member_Fwd:                  \
	case CT_Parameters:                           \
	case CT_Specifiers:                           \
	case CT_Struct_Body:                          \
	case CT_Typename:
