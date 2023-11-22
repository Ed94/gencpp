#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "components/types.hpp"
#endif

// This file was generated automatially by gencpp's bootstrap.cpp (See: https://github.com/Ed94/gencpp)

namespace parser
{
	namespace ETokType
	{
#define GEN_DEFINE_ATTRIBUTE_TOKENS Entry( API_Export, GEN_API_Export_Code ) Entry( API_Import, GEN_API_Import_Code )

		enum Type : u32
		{
			Invalid,
			Access_Private,
			Access_Protected,
			Access_Public,
			Access_MemberSymbol,
			Access_StaticSymbol,
			Ampersand,
			Ampersand_DBL,
			Assign_Classifer,
			Attribute_Open,
			Attribute_Close,
			BraceCurly_Open,
			BraceCurly_Close,
			BraceSquare_Open,
			BraceSquare_Close,
			Capture_Start,
			Capture_End,
			Comment,
			Comment_End,
			Comment_Start,
			Char,
			Comma,
			Decl_Class,
			Decl_GNU_Attribute,
			Decl_MSVC_Attribute,
			Decl_Enum,
			Decl_Extern_Linkage,
			Decl_Friend,
			Decl_Module,
			Decl_Namespace,
			Decl_Operator,
			Decl_Struct,
			Decl_Template,
			Decl_Typedef,
			Decl_Using,
			Decl_Union,
			Identifier,
			Module_Import,
			Module_Export,
			NewLine,
			Number,
			Operator,
			Preprocess_Hash,
			Preprocess_Define,
			Preprocess_If,
			Preprocess_IfDef,
			Preprocess_IfNotDef,
			Preprocess_ElIf,
			Preprocess_Else,
			Preprocess_EndIf,
			Preprocess_Include,
			Preprocess_Pragma,
			Preprocess_Content,
			Preprocess_Macro,
			Preprocess_Unsupported,
			Spec_Alignas,
			Spec_Const,
			Spec_Consteval,
			Spec_Constexpr,
			Spec_Constinit,
			Spec_Explicit,
			Spec_Extern,
			Spec_Final,
			Spec_ForceInline,
			Spec_Global,
			Spec_Inline,
			Spec_Internal_Linkage,
			Spec_LocalPersist,
			Spec_Mutable,
			Spec_NeverInline,
			Spec_Override,
			Spec_Static,
			Spec_ThreadLocal,
			Spec_Volatile,
			Spec_Virtual,
			Star,
			Statement_End,
			StaticAssert,
			String,
			Type_Unsigned,
			Type_Signed,
			Type_Short,
			Type_Long,
			Type_bool,
			Type_char,
			Type_int,
			Type_double,
			Type_MS_int8,
			Type_MS_int16,
			Type_MS_int32,
			Type_MS_int64,
			Type_MS_W64,
			Varadic_Argument,
			__Attributes_Start,
			API_Export,
			API_Import,
			NumTokens
		};

		StrC to_str( Type type )
		{
			local_persist StrC lookup[] {
				{sizeof( "__invalid__" ),          "__invalid__"        },
				{ sizeof( "private" ),             "private"            },
				{ sizeof( "protected" ),           "protected"          },
				{ sizeof( "public" ),              "public"             },
				{ sizeof( "." ),                   "."                  },
				{ sizeof( "::" ),                  "::"                 },
				{ sizeof( "&" ),                   "&"                  },
				{ sizeof( "&&" ),                  "&&"                 },
				{ sizeof( ":" ),                   ":"                  },
				{ sizeof( "[[" ),                  "[["                 },
				{ sizeof( "]]" ),                  "]]"                 },
				{ sizeof( "{" ),                   "{"                  },
				{ sizeof( "}" ),                   "}"                  },
				{ sizeof( "[" ),                   "["                  },
				{ sizeof( "]" ),                   "]"                  },
				{ sizeof( "(" ),                   "("                  },
				{ sizeof( ")" ),                   ")"                  },
				{ sizeof( "__comment__" ),         "__comment__"        },
				{ sizeof( "__comment_end__" ),     "__comment_end__"    },
				{ sizeof( "__comment_start__" ),   "__comment_start__"  },
				{ sizeof( "__character__" ),       "__character__"      },
				{ sizeof( "," ),                   ","                  },
				{ sizeof( "class" ),               "class"              },
				{ sizeof( "__attribute__" ),       "__attribute__"      },
				{ sizeof( "__declspec" ),          "__declspec"         },
				{ sizeof( "enum" ),                "enum"               },
				{ sizeof( "extern" ),              "extern"             },
				{ sizeof( "friend" ),              "friend"             },
				{ sizeof( "module" ),              "module"             },
				{ sizeof( "namespace" ),           "namespace"          },
				{ sizeof( "operator" ),            "operator"           },
				{ sizeof( "struct" ),              "struct"             },
				{ sizeof( "template" ),            "template"           },
				{ sizeof( "typedef" ),             "typedef"            },
				{ sizeof( "using" ),               "using"              },
				{ sizeof( "union" ),               "union"              },
				{ sizeof( "__identifier__" ),      "__identifier__"     },
				{ sizeof( "import" ),              "import"             },
				{ sizeof( "export" ),              "export"             },
				{ sizeof( "__new_line__" ),        "__new_line__"       },
				{ sizeof( "__number__" ),          "__number__"         },
				{ sizeof( "__operator__" ),        "__operator__"       },
				{ sizeof( "#" ),                   "#"                  },
				{ sizeof( "define" ),              "define"             },
				{ sizeof( "if" ),                  "if"                 },
				{ sizeof( "ifdef" ),               "ifdef"              },
				{ sizeof( "ifndef" ),              "ifndef"             },
				{ sizeof( "elif" ),                "elif"               },
				{ sizeof( "else" ),                "else"               },
				{ sizeof( "endif" ),               "endif"              },
				{ sizeof( "include" ),             "include"            },
				{ sizeof( "pragma" ),              "pragma"             },
				{ sizeof( "__macro_content__" ),   "__macro_content__"  },
				{ sizeof( "__macro__" ),           "__macro__"          },
				{ sizeof( "__unsupported__" ),     "__unsupported__"    },
				{ sizeof( "alignas" ),             "alignas"            },
				{ sizeof( "const" ),               "const"              },
				{ sizeof( "consteval" ),           "consteval"          },
				{ sizeof( "constexpr" ),           "constexpr"          },
				{ sizeof( "constinit" ),           "constinit"          },
				{ sizeof( "explicit" ),            "explicit"           },
				{ sizeof( "extern" ),              "extern"             },
				{ sizeof( "final" ),               "final"              },
				{ sizeof( "forceinline" ),         "forceinline"        },
				{ sizeof( "global" ),              "global"             },
				{ sizeof( "inline" ),              "inline"             },
				{ sizeof( "internal" ),            "internal"           },
				{ sizeof( "local_persist" ),       "local_persist"      },
				{ sizeof( "mutable" ),             "mutable"            },
				{ sizeof( "neverinline" ),         "neverinline"        },
				{ sizeof( "override" ),            "override"           },
				{ sizeof( "static" ),              "static"             },
				{ sizeof( "thread_local" ),        "thread_local"       },
				{ sizeof( "volatile" ),            "volatile"           },
				{ sizeof( "virtual" ),             "virtual"            },
				{ sizeof( "*" ),                   "*"                  },
				{ sizeof( ";" ),                   ";"                  },
				{ sizeof( "static_assert" ),       "static_assert"      },
				{ sizeof( "__string__" ),          "__string__"         },
				{ sizeof( "unsigned" ),            "unsigned"           },
				{ sizeof( "signed" ),              "signed"             },
				{ sizeof( "short" ),               "short"              },
				{ sizeof( "long" ),                "long"               },
				{ sizeof( "bool" ),                "bool"               },
				{ sizeof( "char" ),                "char"               },
				{ sizeof( "int" ),                 "int"                },
				{ sizeof( "double" ),              "double"             },
				{ sizeof( "__int8" ),              "__int8"             },
				{ sizeof( "__int16" ),             "__int16"            },
				{ sizeof( "__int32" ),             "__int32"            },
				{ sizeof( "__int64" ),             "__int64"            },
				{ sizeof( "_W64" ),                "_W64"               },
				{ sizeof( "..." ),                 "..."                },
				{ sizeof( "__attrib_start__" ),    "__attrib_start__"   },
				{ sizeof( "GEN_API_Export_Code" ), "GEN_API_Export_Code"},
				{ sizeof( "GEN_API_Import_Code" ), "GEN_API_Import_Code"},
			};
			return lookup[ type ];
		}

		Type to_type( StrC str )
		{
			local_persist u32 keymap[ NumTokens ];
			do_once_start for ( u32 index = 0; index < NumTokens; index++ )
			{
				StrC enum_str   = to_str( ( Type )index );
				keymap[ index ] = crc32( enum_str.Ptr, enum_str.Len - 1 );
			}
			do_once_end u32 hash = crc32( str.Ptr, str.Len );
			for ( u32 index = 0; index < NumTokens; index++ )
			{
				if ( keymap[ index ] == hash )
					return ( Type )index;
			}
			return Invalid;
		}

	}    // namespace ETokType

	using TokType = ETokType::Type;

}    // namespace parser
