#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "components/types.hpp"
#endif

// This file was generated automatially by gencpp's bootstrap.cpp (See: https://github.com/Ed94/gencpp)

#define GEN_DEFINE_ATTRIBUTE_TOKENS Entry( Tok_Attribute_GEN_API, "GEN_API" )

enum TokType : u32
{
	Tok_Invalid,
	Tok_Access_Private,
	Tok_Access_Protected,
	Tok_Access_Public,
	Tok_Access_MemberSymbol,
	Tok_Access_StaticSymbol,
	Tok_Ampersand,
	Tok_Ampersand_DBL,
	Tok_Assign_Classifer,
	Tok_Attribute_Open,
	Tok_Attribute_Close,
	Tok_BraceCurly_Open,
	Tok_BraceCurly_Close,
	Tok_BraceSquare_Open,
	Tok_BraceSquare_Close,
	Tok_Paren_Open,
	Tok_Paren_Close,
	Tok_Comment,
	Tok_Comment_End,
	Tok_Comment_Start,
	Tok_Char,
	Tok_Comma,
	Tok_Decl_Class,
	Tok_Decl_GNU_Attribute,
	Tok_Decl_MSVC_Attribute,
	Tok_Decl_Enum,
	Tok_Decl_Extern_Linkage,
	Tok_Decl_Friend,
	Tok_Decl_Module,
	Tok_Decl_Namespace,
	Tok_Decl_Operator,
	Tok_Decl_Struct,
	Tok_Decl_Template,
	Tok_Decl_Typedef,
	Tok_Decl_Using,
	Tok_Decl_Union,
	Tok_Identifier,
	Tok_Module_Import,
	Tok_Module_Export,
	Tok_NewLine,
	Tok_Number,
	Tok_Operator,
	Tok_Preprocess_Hash,
	Tok_Preprocess_Define,
	Tok_Preprocess_Define_Param,
	Tok_Preprocess_If,
	Tok_Preprocess_IfDef,
	Tok_Preprocess_IfNotDef,
	Tok_Preprocess_ElIf,
	Tok_Preprocess_Else,
	Tok_Preprocess_EndIf,
	Tok_Preprocess_Include,
	Tok_Preprocess_Pragma,
	Tok_Preprocess_Content,
	Tok_Preprocess_Macro_Expr,
	Tok_Preprocess_Macro_Stmt,
	Tok_Preprocess_Macro_Typename,
	Tok_Preprocess_Unsupported,
	Tok_Spec_Alignas,
	Tok_Spec_Const,
	Tok_Spec_Consteval,
	Tok_Spec_Constexpr,
	Tok_Spec_Constinit,
	Tok_Spec_Explicit,
	Tok_Spec_Extern,
	Tok_Spec_Final,
	Tok_Spec_ForceInline,
	Tok_Spec_Global,
	Tok_Spec_Inline,
	Tok_Spec_Internal_Linkage,
	Tok_Spec_LocalPersist,
	Tok_Spec_Mutable,
	Tok_Spec_NeverInline,
	Tok_Spec_Override,
	Tok_Spec_Static,
	Tok_Spec_ThreadLocal,
	Tok_Spec_Volatile,
	Tok_Spec_Virtual,
	Tok_Star,
	Tok_Statement_End,
	Tok_StaticAssert,
	Tok_String,
	Tok_Type_Typename,
	Tok_Type_Unsigned,
	Tok_Type_Signed,
	Tok_Type_Short,
	Tok_Type_Long,
	Tok_Type_bool,
	Tok_Type_char,
	Tok_Type_int,
	Tok_Type_double,
	Tok_Type_MS_int8,
	Tok_Type_MS_int16,
	Tok_Type_MS_int32,
	Tok_Type_MS_int64,
	Tok_Type_MS_W64,
	Tok_Varadic_Argument,
	Tok___Attributes_Start,
	Tok_Attribute_GEN_API,
	Tok_NumTokens
};

inline Str toktype_to_str( TokType type )
{
	local_persist Str lookup[] = {
		{ "__invalid__",          sizeof( "__invalid__" ) - 1          },
		{ "private",              sizeof( "private" ) - 1              },
		{ "protected",            sizeof( "protected" ) - 1            },
		{ "public",               sizeof( "public" ) - 1               },
		{ ".",		            sizeof( "." ) - 1                    },
		{ "::",		           sizeof( "::" ) - 1                   },
		{ "&",		            sizeof( "&" ) - 1                    },
		{ "&&",		           sizeof( "&&" ) - 1                   },
		{ ":",		            sizeof( ":" ) - 1                    },
		{ "[[",		           sizeof( "[[" ) - 1                   },
		{ "]]",		           sizeof( "]]" ) - 1                   },
		{ "{",		            sizeof( "{" ) - 1                    },
		{ "}",		            sizeof( "}" ) - 1                    },
		{ "[",		            sizeof( "[" ) - 1                    },
		{ "]",		            sizeof( "]" ) - 1                    },
		{ "(",		            sizeof( "(" ) - 1                    },
		{ ")",		            sizeof( ")" ) - 1                    },
		{ "__comment__",          sizeof( "__comment__" ) - 1          },
		{ "__comment_end__",      sizeof( "__comment_end__" ) - 1      },
		{ "__comment_start__",    sizeof( "__comment_start__" ) - 1    },
		{ "__character__",        sizeof( "__character__" ) - 1        },
		{ ",",		            sizeof( "," ) - 1                    },
		{ "class",                sizeof( "class" ) - 1                },
		{ "__attribute__",        sizeof( "__attribute__" ) - 1        },
		{ "__declspec",           sizeof( "__declspec" ) - 1           },
		{ "enum",                 sizeof( "enum" ) - 1                 },
		{ "extern",               sizeof( "extern" ) - 1               },
		{ "friend",               sizeof( "friend" ) - 1               },
		{ "module",               sizeof( "module" ) - 1               },
		{ "namespace",            sizeof( "namespace" ) - 1            },
		{ "operator",             sizeof( "operator" ) - 1             },
		{ "struct",               sizeof( "struct" ) - 1               },
		{ "template",             sizeof( "template" ) - 1             },
		{ "typedef",              sizeof( "typedef" ) - 1              },
		{ "using",                sizeof( "using" ) - 1                },
		{ "union",                sizeof( "union" ) - 1                },
		{ "__identifier__",       sizeof( "__identifier__" ) - 1       },
		{ "import",               sizeof( "import" ) - 1               },
		{ "export",               sizeof( "export" ) - 1               },
		{ "__new_line__",         sizeof( "__new_line__" ) - 1         },
		{ "__number__",           sizeof( "__number__" ) - 1           },
		{ "__operator__",         sizeof( "__operator__" ) - 1         },
		{ "#",		            sizeof( "#" ) - 1                    },
		{ "define",               sizeof( "define" ) - 1               },
		{ "__define_param__",     sizeof( "__define_param__" ) - 1     },
		{ "if",		           sizeof( "if" ) - 1                   },
		{ "ifdef",                sizeof( "ifdef" ) - 1                },
		{ "ifndef",               sizeof( "ifndef" ) - 1               },
		{ "elif",                 sizeof( "elif" ) - 1                 },
		{ "else",                 sizeof( "else" ) - 1                 },
		{ "endif",                sizeof( "endif" ) - 1                },
		{ "include",              sizeof( "include" ) - 1              },
		{ "pragma",               sizeof( "pragma" ) - 1               },
		{ "__macro_content__",    sizeof( "__macro_content__" ) - 1    },
		{ "__macro_expression__", sizeof( "__macro_expression__" ) - 1 },
		{ "__macro_statment__",   sizeof( "__macro_statment__" ) - 1   },
		{ "__macro_typename__",   sizeof( "__macro_typename__" ) - 1   },
		{ "__unsupported__",      sizeof( "__unsupported__" ) - 1      },
		{ "alignas",              sizeof( "alignas" ) - 1              },
		{ "const",                sizeof( "const" ) - 1                },
		{ "consteval",            sizeof( "consteval" ) - 1            },
		{ "constexpr",            sizeof( "constexpr" ) - 1            },
		{ "constinit",            sizeof( "constinit" ) - 1            },
		{ "explicit",             sizeof( "explicit" ) - 1             },
		{ "extern",               sizeof( "extern" ) - 1               },
		{ "final",                sizeof( "final" ) - 1                },
		{ "forceinline",          sizeof( "forceinline" ) - 1          },
		{ "global",               sizeof( "global" ) - 1               },
		{ "inline",               sizeof( "inline" ) - 1               },
		{ "internal",             sizeof( "internal" ) - 1             },
		{ "local_persist",        sizeof( "local_persist" ) - 1        },
		{ "mutable",              sizeof( "mutable" ) - 1              },
		{ "neverinline",          sizeof( "neverinline" ) - 1          },
		{ "override",             sizeof( "override" ) - 1             },
		{ "static",               sizeof( "static" ) - 1               },
		{ "thread_local",         sizeof( "thread_local" ) - 1         },
		{ "volatile",             sizeof( "volatile" ) - 1             },
		{ "virtual",              sizeof( "virtual" ) - 1              },
		{ "*",		            sizeof( "*" ) - 1                    },
		{ ";",		            sizeof( ";" ) - 1                    },
		{ "static_assert",        sizeof( "static_assert" ) - 1        },
		{ "__string__",           sizeof( "__string__" ) - 1           },
		{ "typename",             sizeof( "typename" ) - 1             },
		{ "unsigned",             sizeof( "unsigned" ) - 1             },
		{ "signed",               sizeof( "signed" ) - 1               },
		{ "short",                sizeof( "short" ) - 1                },
		{ "long",                 sizeof( "long" ) - 1                 },
		{ "bool",                 sizeof( "bool" ) - 1                 },
		{ "char",                 sizeof( "char" ) - 1                 },
		{ "int",		          sizeof( "int" ) - 1                  },
		{ "double",               sizeof( "double" ) - 1               },
		{ "__int8",               sizeof( "__int8" ) - 1               },
		{ "__int16",              sizeof( "__int16" ) - 1              },
		{ "__int32",              sizeof( "__int32" ) - 1              },
		{ "__int64",              sizeof( "__int64" ) - 1              },
		{ "_W64",                 sizeof( "_W64" ) - 1                 },
		{ "...",		          sizeof( "..." ) - 1                  },
		{ "__attrib_start__",     sizeof( "__attrib_start__" ) - 1     },
		{ "GEN_API",              sizeof( "GEN_API" ) - 1              },
	};
	return lookup[type];
}

inline TokType str_to_toktype( Str str )
{
	local_persist u32 keymap[Tok_NumTokens];
	do_once_start for ( u32 index = 0; index < Tok_NumTokens; index++ )
	{
		Str enum_str  = toktype_to_str( (TokType)index );
		keymap[index] = crc32( enum_str.Ptr, enum_str.Len );
	}
	do_once_end u32 hash = crc32( str.Ptr, str.Len );
	for ( u32 index = 0; index < Tok_NumTokens; index++ )
	{
		if ( keymap[index] == hash )
			return (TokType)index;
	}
	return Tok_Invalid;
}
