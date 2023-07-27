namespace Parser
{
/*
	This is a simple lexer that focuses on tokenizing only tokens relevant to the library.
	It will not be capable of lexing C++ code with unsupported features.

	For the sake of scanning files, it can scan preprocessor directives

	Attributes_Start is only used to indicate the start of the user_defined attribute list.
*/

#	define Define_TokType \
	Entry( Access_Private,         "private" )          \
	Entry( Access_Protected,       "protected" )        \
	Entry( Access_Public,          "public" )           \
	Entry( Access_MemberSymbol,    "." )                \
	Entry( Access_StaticSymbol,    "::")                \
	Entry( Ampersand,              "&" )                \
	Entry( Ampersand_DBL,          "&&" )               \
	Entry( Assign_Classifer,       ":" )                \
	Entry( Attribute_Open, 	       "[[" )               \
	Entry( Attribute_Close, 	   "]]" )               \
	Entry( BraceCurly_Open,        "{" )                \
	Entry( BraceCurly_Close,       "}" )                \
	Entry( BraceSquare_Open,       "[" )                \
	Entry( BraceSquare_Close,      "]" )                \
	Entry( Capture_Start,          "(" )                \
	Entry( Capture_End,            ")" )                \
	Entry( Comment,                "__comment__" )      \
	Entry( Char,                   "__char__" )         \
	Entry( Comma,                  "," )                \
	Entry( Decl_Class,             "class" )            \
	Entry( Decl_GNU_Attribute,    "__attribute__" )     \
	Entry( Decl_MSVC_Attribute,    "__declspec" )       \
	Entry( Decl_Enum,              "enum" )             \
	Entry( Decl_Extern_Linkage,    "extern" )           \
	Entry( Decl_Friend,            "friend" )           \
	Entry( Decl_Module,            "module" )           \
	Entry( Decl_Namespace,         "namespace" )        \
	Entry( Decl_Operator,          "operator" )         \
	Entry( Decl_Struct,            "struct" )           \
	Entry( Decl_Template,          "template" )         \
	Entry( Decl_Typedef,           "typedef" )          \
	Entry( Decl_Using,             "using" )            \
	Entry( Decl_Union,             "union" )            \
	Entry( Identifier,             "__identifier__" )   \
	Entry( Module_Import,          "import" )           \
	Entry( Module_Export,          "export" )           \
	Entry( Number,                 "number" )           \
	Entry( Operator,               "operator" )         \
	Entry( Preprocessor_Directive, "#")                 \
	Entry( Preprocessor_Include,   "include" )          \
	Entry( Spec_Alignas,           "alignas" )          \
	Entry( Spec_Const,             "const" )            \
	Entry( Spec_Consteval,         "consteval" )        \
	Entry( Spec_Constexpr,         "constexpr" )        \
	Entry( Spec_Constinit,         "constinit" )        \
	Entry( Spec_Explicit, 		   "explicit" )         \
	Entry( Spec_Extern,            "extern" )           \
	Entry( Spec_Final, 		       "final" )            \
	Entry( Spec_Global, 		   "global" )           \
	Entry( Spec_Inline,            "inline" )           \
	Entry( Spec_Internal_Linkage,  "internal" )         \
	Entry( Spec_LocalPersist,      "local_persist" )    \
	Entry( Spec_Mutable,           "mutable" )          \
	Entry( Spec_Override,          "override" )         \
	Entry( Spec_Static,            "static" )           \
	Entry( Spec_ThreadLocal,       "thread_local" )     \
	Entry( Spec_Volatile,          "volatile")          \
	Entry( Star,                   "*" )                \
	Entry( Statement_End,          ";" )                \
	Entry( String,                 "__string__" )       \
	Entry( Type_Unsigned, 	       "unsigned" )         \
	Entry( Type_Signed,            "signed" )           \
	Entry( Type_Short,             "short" )            \
	Entry( Type_Long,              "long" )             \
	Entry( Type_char, 			   "char" )             \
	Entry( Type_int, 			   "int" )              \
	Entry( Type_double, 		   "double" )           \
	Entry( Varadic_Argument,       "..." )              \
	Entry( Attributes_Start,       "__attrib_start__" )

	enum class TokType : u32
	{
	#	define Entry( Name_, Str_ ) Name_,
		Define_TokType
		GEN_Define_Attribute_Tokens
	#	undef Entry
		Num,
		Invalid
	};

	struct Token
	{
		char const* Text;
		sptr        Length;
		TokType     Type;
		bool 	    IsAssign;

		operator bool()
		{
			return Text && Length && Type != TokType::Invalid;
		}

		operator StrC()
		{
			return { Length, Text };
		}
	};

	internal inline
	TokType get_tok_type( char const* word, s32 length )
	{
		local_persist
		StrC lookup[(u32)TokType::Num] =
		{
		#	define Entry( Name_, Str_ ) { sizeof(Str_), Str_ },
			Define_TokType
			GEN_Define_Attribute_Tokens
		#	undef Entry
		};

		for ( u32 index = 0; index < (u32)TokType::Num; index++ )
		{
			s32         lookup_len = lookup[index].Len - 1;
			char const* lookup_str = lookup[index].Ptr;

			if ( lookup_len != length )
				continue;

			if ( str_compare( word, lookup_str, lookup_len ) == 0 )
				return scast(TokType, index);
		}

		return TokType::Invalid;
	}

	internal inline
	char const* str_tok_type( TokType type )
	{
		local_persist
		char const* lookup[(u32)TokType::Num] =
		{
		#	define Entry( Name_, Str_ ) Str_,
			Define_TokType
			GEN_Define_Attribute_Tokens
		#	undef Entry
		};

		return lookup[(u32)type];
	}

#	undef Define_TokType

	internal inline
	bool tok_is_specifier( Token const& tok )
	{
		return (tok.Type <= TokType::Star && tok.Type >= TokType::Spec_Alignas)
			|| tok.Type == TokType::Ampersand
			|| tok.Type == TokType::Ampersand_DBL
		;
	}

	internal inline
	bool tok_is_access_specifier( Token const& tok )
	{
		return tok.Type >= TokType::Access_Private && tok.Type <= TokType::Access_Public;
	}

	internal inline
	AccessSpec tok_to_access_specifier( Token const& tok )
	{
		return scast(AccessSpec, tok.Type);
	}

	internal inline
	bool tok_is_attribute( Token const& tok )
	{
		return tok.Type > TokType::Attributes_Start;
	}
} // Parser