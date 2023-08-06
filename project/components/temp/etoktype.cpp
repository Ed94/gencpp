namespace Parser
{
/*
	This is a simple lexer that focuses on tokenizing only tokens relevant to the library.
	It will not be capable of lexing C++ code with unsupported features.

	For the sake of scanning files, it can scan preprocessor directives

	__Attributes_Start is only used to indicate the start of the user_defined attribute list.
*/

#ifndef GEN_DEFINE_ATTRIBUTE_TOKENS
#	define GEN_DEFINE_ATTRIBUTE_TOKENS         \
	Entry( API_Export, "GEN_API_Export_Code" ) \
	Entry( API_Import, "GEN_API_Import_Code" )
#endif

#	define Define_TokType                                \
	Entry( Invalid,                "INVALID" )           \
	Entry( Access_Private,         "private" )           \
	Entry( Access_Protected,       "protected" )         \
	Entry( Access_Public,          "public" )            \
	Entry( Access_MemberSymbol,    "." )                 \
	Entry( Access_StaticSymbol,    "::")                 \
	Entry( Ampersand,              "&" )                 \
	Entry( Ampersand_DBL,          "&&" )                \
	Entry( Assign_Classifer,       ":" )                 \
	Entry( Attribute_Open, 	       "[[" )                \
	Entry( Attribute_Close,        "]]" )                \
	Entry( BraceCurly_Open,        "{" )                 \
	Entry( BraceCurly_Close,       "}" )                 \
	Entry( BraceSquare_Open,       "[" )                 \
	Entry( BraceSquare_Close,      "]" )                 \
	Entry( Capture_Start,          "(" )                 \
	Entry( Capture_End,            ")" )                 \
	Entry( Comment,                "__comment__" )       \
	Entry( Comment_End,            "__comment_end__" )   \
	Entry( Comment_Start,          "__comment start__" ) \
	Entry( Char,                   "__character__" )     \
	Entry( Comma,                  "," )                 \
	Entry( Decl_Class,             "class" )             \
	Entry( Decl_GNU_Attribute,     "__attribute__" )     \
	Entry( Decl_MSVC_Attribute,    "__declspec" )        \
	Entry( Decl_Enum,              "enum" )              \
	Entry( Decl_Extern_Linkage,    "extern" )            \
	Entry( Decl_Friend,            "friend" )            \
	Entry( Decl_Module,            "module" )            \
	Entry( Decl_Namespace,         "namespace" )         \
	Entry( Decl_Operator,          "operator" )          \
	Entry( Decl_Struct,            "struct" )            \
	Entry( Decl_Template,          "template" )          \
	Entry( Decl_Typedef,           "typedef" )           \
	Entry( Decl_Using,             "using" )             \
	Entry( Decl_Union,             "union" )             \
	Entry( Identifier,             "__identifier__" )    \
	Entry( Module_Import,          "import" )            \
	Entry( Module_Export,          "export" )            \
	Entry( NewLine,                "__NewLine__" )       \
	Entry( Number,                 "__number__" )        \
	Entry( Operator,               "__operator__" )      \
	Entry( Preprocess_Hash,        "#" )                 \
	Entry( Preprocess_Define,      "define")             \
	Entry( Preprocess_If,          "if")                 \
	Entry( Preprocess_IfDef,       "ifdef")              \
	Entry( Preprocess_IfNotDef,    "ifndef")             \
	Entry( Preprocess_ElIf,        "elif")               \
	Entry( Preprocess_Else,        "else")               \
	Entry( Preprocess_EndIf,       "endif")              \
	Entry( Preprocess_Include,     "include" )           \
	Entry( Preprocess_Pragma,      "pragma")             \
	Entry( Preprocess_Content,     "__macro_content__")  \
	Entry( Preprocess_Macro,       "__macro__")          \
	Entry( Preprocess_Unsupported, "__unsupported__" )   \
	Entry( Spec_Alignas,           "alignas" )           \
	Entry( Spec_Const,             "const" )             \
	Entry( Spec_Consteval,         "consteval" )         \
	Entry( Spec_Constexpr,         "constexpr" )         \
	Entry( Spec_Constinit,         "constinit" )         \
	Entry( Spec_Explicit,          "explicit" )          \
	Entry( Spec_Extern,            "extern" )            \
	Entry( Spec_Final,             "final" )             \
	Entry( Spec_Global,            "global" )            \
	Entry( Spec_Inline,            "inline" )            \
	Entry( Spec_Internal_Linkage,  "internal" )          \
	Entry( Spec_LocalPersist,      "local_persist" )     \
	Entry( Spec_Mutable,           "mutable" )           \
	Entry( Spec_NeverInline,       "neverinline" )       \
	Entry( Spec_Override,          "override" )          \
	Entry( Spec_Static,            "static" )            \
	Entry( Spec_ThreadLocal,       "thread_local" )      \
	Entry( Spec_Volatile,          "volatile")           \
	Entry( Star,                   "*" )                 \
	Entry( Statement_End,          ";" )                 \
	Entry( StaticAssert,           "static_assert" )     \
	Entry( String,                 "__string__" )        \
	Entry( Type_Unsigned,          "unsigned" )          \
	Entry( Type_Signed,            "signed" )            \
	Entry( Type_Short,             "short" )             \
	Entry( Type_Long,              "long" )              \
	Entry( Type_char,              "char" )              \
	Entry( Type_int,               "int" )               \
	Entry( Type_double,            "double" )            \
	Entry( Type_MS_int8,           "__int8" )            \
	Entry( Type_MS_int16,          "__int16" )           \
	Entry( Type_MS_int32,          "__int32" )           \
	Entry( Type_MS_int64,          "__int64" )           \
	Entry( Type_MS_W64,            "_W64" )              \
	Entry( Varadic_Argument,       "..." )               \
	Entry( __Attributes_Start,     "__attrib_start__" )

	namespace ETokType
	{
		enum Type : u32
		{
		#	define Entry( Name_, Str_ ) Name_,
			Define_TokType
			GEN_DEFINE_ATTRIBUTE_TOKENS
		#	undef Entry
			NumTokens,
		};

		internal inline
		Type to_type( StrC str_tok )
		{
			local_persist
			StrC lookup[(u32)NumTokens] =
			{
			#	define Entry( Name_, Str_ ) { sizeof(Str_), Str_ },
				Define_TokType
				GEN_DEFINE_ATTRIBUTE_TOKENS
			#	undef Entry
			};

			for ( u32 index = 0; index < (u32)NumTokens; index++ )
			{
				s32         lookup_len = lookup[index].Len - 1;
				char const* lookup_str = lookup[index].Ptr;

				if ( lookup_len != str_tok.Len )
					continue;

				if ( str_compare( str_tok.Ptr, lookup_str, lookup_len ) == 0 )
					return scast(Type, index);
			}

			return Invalid;
		}

		internal inline
		StrC to_str( Type type )
		{
			local_persist
			StrC lookup[(u32)NumTokens] =
			{
			#	define Entry( Name_, Str_ ) { sizeof(Str_), Str_ },
				Define_TokType
				GEN_DEFINE_ATTRIBUTE_TOKENS
			#	undef Entry
			};

			return lookup[(u32)type];
		}
	#	undef Define_TokType
	};

	using TokType = ETokType::Type;

} // Parser

