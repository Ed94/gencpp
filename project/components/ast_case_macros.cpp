#	define GEN_AST_BODY_CLASS_UNALLOWED_TYPES \
	case PlatformAttributes:                  \
	case Class_Body:                          \
	case Enum_Body:                           \
	case Extern_Linkage:                      \
	case Function_Body:                       \
	case Function_Fwd:                        \
	case Global_Body:                         \
	case Namespace:                           \
	case Namespace_Body:                      \
	case Operator:                            \
	case Operator_Fwd:                        \
	case Parameters:                          \
	case Specifiers:                          \
	case Struct_Body:                         \
	case Typename:
#	define GEN_AST_BODY_STRUCT_UNALLOWED_TYPES GEN_AST_BODY_CLASS_UNALLOWED_TYPES

#	define GEN_AST_BODY_FUNCTION_UNALLOWED_TYPES \
	case Access_Public:                          \
	case Access_Protected:                       \
	case Access_Private:                         \
	case PlatformAttributes:                     \
	case Class_Body:                             \
	case Enum_Body:                              \
	case Extern_Linkage:                         \
	case Friend:                                 \
	case Function_Body:                          \
	case Function_Fwd:                           \
	case Global_Body:                            \
	case Namespace:                              \
	case Namespace_Body:                         \
	case Operator:                               \
	case Operator_Fwd:                           \
	case Operator_Member:                        \
	case Operator_Member_Fwd:                    \
	case Parameters:                             \
	case Specifiers:                             \
	case Struct_Body:                            \
	case Typename:

#	define GEN_AST_BODY_GLOBAL_UNALLOWED_TYPES \
	case Access_Public: 				       \
	case Access_Protected: 				       \
	case Access_Private: 				       \
	case PlatformAttributes:                   \
	case Class_Body: 					       \
	case Enum_Body: 					       \
	case Execution: 					       \
	case Friend: 						       \
	case Function_Body: 				       \
	case Namespace_Body: 				       \
	case Operator_Member: 				       \
	case Operator_Member_Fwd: 			       \
	case Parameters: 					       \
	case Specifiers: 					       \
	case Struct_Body: 					       \
	case Typename:
#	define GEN_AST_BODY_EXPORT_UNALLOWED_TYPES         GEN_AST_BODY_GLOBAL_UNALLOWED_TYPES
#	define GEN_AST_BODY_EXTERN_LINKAGE_UNALLOWED_TYPES GEN_AST_BODY_GLOBAL_UNALLOWED_TYPES

#	define GEN_AST_BODY_NAMESPACE_UNALLOWED_TYPES \
	case Access_Public: 				          \
	case Access_Protected: 				          \
	case Access_Private: 				          \
	case PlatformAttributes:                      \
	case Class_Body: 					          \
	case Enum_Body: 					          \
	case Execution: 					          \
	case Friend: 						          \
	case Function_Body: 				          \
	case Namespace_Body: 				          \
	case Operator_Member: 				          \
	case Operator_Member_Fwd: 			          \
	case Parameters: 					          \
	case Specifiers: 					          \
	case Struct_Body: 					          \
	case Typename:

