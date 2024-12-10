#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "components/types.hpp"
#endif

// This file was generated automatially by gencpp's bootstrap.cpp (See: https://github.com/Ed94/gencpp)

GEN_NS_PARSER_BEGIN

#define GEN_DEFINE_ATTRIBUTE_TOKENS Entry( Tok_Attribute_API_Export, "GEN_API_Export_Code" ) Entry( Tok_Attribute_API_Import, "GEN_API_Import_Code" )

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
	Tok_Capture_Start,
	Tok_Capture_End,
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
	Tok_Preprocess_If,
	Tok_Preprocess_IfDef,
	Tok_Preprocess_IfNotDef,
	Tok_Preprocess_ElIf,
	Tok_Preprocess_Else,
	Tok_Preprocess_EndIf,
	Tok_Preprocess_Include,
	Tok_Preprocess_Pragma,
	Tok_Preprocess_Content,
	Tok_Preprocess_Macro,
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
	Tok_Attribute_API_Export,
	Tok_Attribute_API_Import,
	Tok_NumTokens
};

inline StrC toktype_to_str( TokType type )
{
	local_persist StrC lookup[] = {
		{ sizeof( "__invalid__" ),         "__invalid__"         },
		{ sizeof( "private" ),             "private"             },
		{ sizeof( "protected" ),           "protected"           },
		{ sizeof( "public" ),              "public"              },
		{ sizeof( "." ),                   "."                   },
		{ sizeof( "::" ),                  "::"                  },
		{ sizeof( "&" ),                   "&"                   },
		{ sizeof( "&&" ),                  "&&"                  },
		{ sizeof( ":" ),                   ":"                   },
		{ sizeof( "[[" ),                  "[["                  },
		{ sizeof( "]]" ),                  "]]"                  },
		{ sizeof( "{" ),                   "{"                   },
		{ sizeof( "}" ),                   "}"                   },
		{ sizeof( "[" ),                   "["                   },
		{ sizeof( "]" ),                   "]"                   },
		{ sizeof( "(" ),                   "("                   },
		{ sizeof( ")" ),                   ")"                   },
		{ sizeof( "__comment__" ),         "__comment__"         },
		{ sizeof( "__comment_end__" ),     "__comment_end__"     },
		{ sizeof( "__comment_start__" ),   "__comment_start__"   },
		{ sizeof( "__character__" ),       "__character__"       },
		{ sizeof( "," ),                   ","                   },
		{ sizeof( "class" ),               "class"               },
		{ sizeof( "__attribute__" ),       "__attribute__"       },
		{ sizeof( "__declspec" ),          "__declspec"          },
		{ sizeof( "enum" ),                "enum"                },
		{ sizeof( "extern" ),              "extern"              },
		{ sizeof( "friend" ),              "friend"              },
		{ sizeof( "module" ),              "module"              },
		{ sizeof( "namespace" ),           "namespace"           },
		{ sizeof( "operator" ),            "operator"            },
		{ sizeof( "struct" ),              "struct"              },
		{ sizeof( "template" ),            "template"            },
		{ sizeof( "typedef" ),             "typedef"             },
		{ sizeof( "using" ),               "using"               },
		{ sizeof( "union" ),               "union"               },
		{ sizeof( "__identifier__" ),      "__identifier__"      },
		{ sizeof( "import" ),              "import"              },
		{ sizeof( "export" ),              "export"              },
		{ sizeof( "__new_line__" ),        "__new_line__"        },
		{ sizeof( "__number__" ),          "__number__"          },
		{ sizeof( "__operator__" ),        "__operator__"        },
		{ sizeof( "#" ),                   "#"                   },
		{ sizeof( "define" ),              "define"              },
		{ sizeof( "if" ),                  "if"                  },
		{ sizeof( "ifdef" ),               "ifdef"               },
		{ sizeof( "ifndef" ),              "ifndef"              },
		{ sizeof( "elif" ),                "elif"                },
		{ sizeof( "else" ),                "else"                },
		{ sizeof( "endif" ),               "endif"               },
		{ sizeof( "include" ),             "include"             },
		{ sizeof( "pragma" ),              "pragma"              },
		{ sizeof( "__macro_content__" ),   "__macro_content__"   },
		{ sizeof( "__macro__" ),           "__macro__"           },
		{ sizeof( "__unsupported__" ),     "__unsupported__"     },
		{ sizeof( "alignas" ),             "alignas"             },
		{ sizeof( "const" ),               "const"               },
		{ sizeof( "consteval" ),           "consteval"           },
		{ sizeof( "constexpr" ),           "constexpr"           },
		{ sizeof( "constinit" ),           "constinit"           },
		{ sizeof( "explicit" ),            "explicit"            },
		{ sizeof( "extern" ),              "extern"              },
		{ sizeof( "final" ),               "final"               },
		{ sizeof( "forceinline" ),         "forceinline"         },
		{ sizeof( "global" ),              "global"              },
		{ sizeof( "inline" ),              "inline"              },
		{ sizeof( "internal" ),            "internal"            },
		{ sizeof( "local_persist" ),       "local_persist"       },
		{ sizeof( "mutable" ),             "mutable"             },
		{ sizeof( "neverinline" ),         "neverinline"         },
		{ sizeof( "override" ),            "override"            },
		{ sizeof( "static" ),              "static"              },
		{ sizeof( "thread_local" ),        "thread_local"        },
		{ sizeof( "volatile" ),            "volatile"            },
		{ sizeof( "virtual" ),             "virtual"             },
		{ sizeof( "*" ),                   "*"                   },
		{ sizeof( ";" ),                   ";"                   },
		{ sizeof( "static_assert" ),       "static_assert"       },
		{ sizeof( "__string__" ),          "__string__"          },
		{ sizeof( "typename" ),            "typename"            },
		{ sizeof( "unsigned" ),            "unsigned"            },
		{ sizeof( "signed" ),              "signed"              },
		{ sizeof( "short" ),               "short"               },
		{ sizeof( "long" ),                "long"                },
		{ sizeof( "bool" ),                "bool"                },
		{ sizeof( "char" ),                "char"                },
		{ sizeof( "int" ),                 "int"                 },
		{ sizeof( "double" ),              "double"              },
		{ sizeof( "__int8" ),              "__int8"              },
		{ sizeof( "__int16" ),             "__int16"             },
		{ sizeof( "__int32" ),             "__int32"             },
		{ sizeof( "__int64" ),             "__int64"             },
		{ sizeof( "_W64" ),                "_W64"                },
		{ sizeof( "..." ),                 "..."                 },
		{ sizeof( "__attrib_start__" ),    "__attrib_start__"    },
		{ sizeof( "GEN_API_Export_Code" ), "GEN_API_Export_Code" },
		{ sizeof( "GEN_API_Import_Code" ), "GEN_API_Import_Code" },
	};
	return lookup[type];
}

inline TokType strc_to_toktype( StrC str )
{
	local_persist u32 keymap[Tok_NumTokens];
	do_once_start for ( u32 index = 0; index < Tok_NumTokens; index++ )
	{
		StrC enum_str = toktype_to_str( (TokType)index );
		keymap[index] = crc32( enum_str.Ptr, enum_str.Len - 1 );
	}
	do_once_end u32 hash = crc32( str.Ptr, str.Len );
	for ( u32 index = 0; index < Tok_NumTokens; index++ )
	{
		if ( keymap[index] == hash )
			return (TokType)index;
	}
	return Tok_Invalid;
}

GEN_NS_PARSER_END