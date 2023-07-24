/*
These constructors are the most implementation intensive other than the editor or scanner.
*/

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

	struct TokArray
	{
		Array<Token> Arr;
		s32          Idx;

		bool __eat( TokType type, char const* context )
		{
			if ( Arr.num() - Idx <= 0 )
			{
				log_failure( "gen::%s: No tokens left", context );
				return Code::Invalid;
			}

			if ( Arr[Idx].Type != type )
			{
				String token_str = String::make( GlobalAllocator, { Arr[Idx].Length, Arr[Idx].Text } );

				log_failure( "gen::%s: expected %s, got %s", context, str_tok_type(type), str_tok_type(Arr[Idx].Type) );

				return Code::Invalid;
			}

			Idx++;
			return true;
		}

		Token& current()
		{
			return Arr[Idx];
		}

		Token& previous()
		{
			return Arr[Idx - 1];
		}

		Token* next()
		{
			return Idx + 1 < Arr.num() ? &Arr[Idx + 1] : nullptr;
		}
	};

	TokArray lex( StrC content, bool keep_preprocess_directives = false )
	{
	#	define current ( * scanner )

	#	define move_forward() \
		left--;               \
		scanner++

	#	define SkipWhitespace()                     \
		while ( left && char_is_space( current ) )  \
		{                                           \
			move_forward();                         \
		}

	#	define SkipWhitespace_Checked( Context_, Msg_, ... )             \
		while ( left && char_is_space( current ) )                       \
		{                                                                \
			move_forward();                                              \
		}                                                                \
		if ( left <= 0 )                                                 \
		{                                                                \
			log_failure( "gen::" txt(Context_) ": " Msg_, __VA_ARGS__ ); \
			return { 0, nullptr };                                       \
		}

		local_persist thread_local
		Array<Token> Tokens = { nullptr };

		s32         left    = content.Len;
		char const* scanner = content.Ptr;

		char const* word        = scanner;
		s32         word_length = 0;

		SkipWhitespace();
		if ( left <= 0 )
		{
			log_failure( "gen::lex: no tokens found (only whitespace provided)" );
			return { { nullptr }, 0 };
		}

		if ( Tokens )
		{
			Tokens.free();
		}

		Tokens = Array<Token>::init_reserve( LexArena, content.Len / 6 );

		while (left )
		{
			Token token = { nullptr, 0, TokType::Invalid, false };

			SkipWhitespace();
			if ( left <= 0 )
				break;

			switch ( current )
			{
				case '#':
					token.Text   = scanner;
					token.Length = 1;
					token.Type   = TokType::Preprocessor_Directive;
					move_forward();

					while (left && current != '\n' )
					{
						if ( current == '\\'  )
						{
							move_forward();

							if ( current != '\n' && keep_preprocess_directives )
							{
								log_failure( "gen::lex: invalid preprocessor directive, will still grab but will not compile %s", token.Text );
							}
						}

						move_forward();
						token.Length++;
					}
					goto FoundToken;

				case '.':
					token.Text   = scanner;
					token.Length = 1;
					token.Type   = TokType::Access_MemberSymbol;

					if (left)
						move_forward();

					if ( current == '.' )
					{
						move_forward();
						if( current == '.' )
						{
							token.Length = 3;
							token.Type = TokType::Varadic_Argument;
							move_forward();
						}
						else
						{
							log_failure( "gen::lex: invalid varadic argument, expected '...' got '..%c'", current );
						}
					}

					goto FoundToken;

				case '&' :
					token.Text   = scanner;
					token.Length = 1;
					token.Type   = TokType::Ampersand;

					if (left)
						move_forward();

					if ( current == '&' )	// &&
					{
						token.Length  = 2;
						token.Type    = TokType::Ampersand_DBL;

						if (left)
							move_forward();
					}

					goto FoundToken;

				case ':':
					token.Text   = scanner;
					token.Length = 1;
					token.Type   = TokType::Assign_Classifer;

					if (left)
						move_forward();

					if ( current == ':' )
					{
						move_forward();
						token.Type  = TokType::Access_StaticSymbol;
						token.Length++;
					}
					goto FoundToken;

				case '{':
					token.Text   = scanner;
					token.Length = 1;
					token.Type   = TokType::BraceCurly_Open;

					if (left)
						move_forward();
					goto FoundToken;

				case '}':
					token.Text   = scanner;
					token.Length = 1;
					token.Type   = TokType::BraceCurly_Close;

					if (left)
						move_forward();
					goto FoundToken;

				case '[':
					token.Text   = scanner;
					token.Length = 1;
					token.Type   = TokType::BraceSquare_Open;
					if ( left )
					{
						move_forward();

						if ( current == ']' )
						{
							token.Length = 2;
							token.Type   = TokType::Operator;
							move_forward();
						}
					}
					goto FoundToken;

				case ']':
					token.Text   = scanner;
					token.Length = 1;
					token.Type   = TokType::BraceSquare_Close;

					if (left)
						move_forward();
					goto FoundToken;

				case '(':
					token.Text   = scanner;
					token.Length = 1;
					token.Type   = TokType::Capture_Start;

					if (left)
						move_forward();
					goto FoundToken;

				case ')':
					token.Text   = scanner;
					token.Length = 1;
					token.Type   = TokType::Capture_End;

					if (left)
						move_forward();
					goto FoundToken;

				case '\'':
					token.Text   = scanner;
					token.Length = 1;
					token.Type   = TokType::Char;

					move_forward();

					while ( left && current != '\'' )
					{
						move_forward();
						token.Length++;
					}

					if ( left )
					{
						move_forward();
						token.Length++;
					}
					goto FoundToken;

				case ',':
					token.Text   = scanner;
					token.Length = 1;
					token.Type   = TokType::Comma;

					if (left)
						move_forward();
					goto FoundToken;

				case '*':
					token.Text   = scanner;
					token.Length = 1;
					token.Type   = TokType::Star;

					if (left)
						move_forward();
					goto FoundToken;

				case ';':
					token.Text   = scanner;
					token.Length = 1;
					token.Type   = TokType::Statement_End;

					if (left)
						move_forward();
					goto FoundToken;

				case '"':
					token.Text   = scanner;
					token.Length = 1;
					token.Type   = TokType::String;

					move_forward();
					while ( left )
					{
						if ( current == '"' )
						{
							move_forward();
							break;
						}

						if ( current == '\\' )
						{
							move_forward();
							token.Length++;

							if ( left )
							{
								move_forward();
								token.Length++;
							}
							continue;
						}

						move_forward();
						token.Length++;
					}
					goto FoundToken;

				// All other operators we just label as an operator and move forward.
				case '=':
					token.Text     = scanner;
					token.Length   = 1;
					token.Type     = TokType::Operator;
					token.IsAssign = true;

					if (left)
						move_forward();

					goto FoundToken;

				case '+':
				case '%':
				case '^':
				case '~':
				case '!':
				case '<':
				case '>':
				case '|':
					token.Text   = scanner;
					token.Length = 1;
					token.Type   = TokType::Operator;

					if (left)
						move_forward();

					if ( current == '=' )
					{
						token.Length++;
						token.IsAssign = true;

						if (left)
							move_forward();
					}
					else while ( left && current == *(scanner - 1) && token.Length < 3 )
					{
						token.Length++;

						if (left)
							move_forward();
					}
					goto FoundToken;

				// Dash is unfortunatlly a bit more complicated...
				case '-':
					token.Text   = scanner;
					token.Length = 1;
					token.Type   = TokType::Operator;
					if ( left )
					{
						move_forward();

						if ( current == '>'  )
						{
							token.Length++;
							move_forward();

							if ( current == '*' )
							{
								token.Length++;
								move_forward();
							}
						}
						else if ( current == '=' )
						{
							token.Length++;
							token.IsAssign = true;

							if (left)
								move_forward();
						}
						else while ( left && current == *(scanner - 1) && token.Length < 3 )
						{
							token.Length++;

							if (left)
								move_forward();
						}
					}
					goto FoundToken;

				case '/':
					token.Text   = scanner;
					token.Length = 1;
					token.Type   = TokType::Operator;

					if ( left )
					{
						move_forward();

						if ( current == '/' )
						{
							token.Type = TokType::Comment;

							move_forward();
							token.Text   = scanner;
							token.Length = 0;

							while ( left && current != '\n' )
							{
								move_forward();
								token.Length++;
							}
						}
						else if ( current == '*' )
						{
							token.Type = TokType::Comment;

							move_forward();
							token.Text   = scanner;
							token.Length = 0;

							while ( left && ( current != '*' && *(scanner + 1) != '/' ) )
							{
								move_forward();
								token.Length++;
							}
							move_forward();
							move_forward();
						}
					}
					goto FoundToken;
			}

			if ( char_is_alpha( current ) || current == '_' )
			{
				token.Text   = scanner;
				token.Length = 1;
				move_forward();

				while ( left && ( char_is_alphanumeric(current) || current == '_' ) )
				{
					move_forward();
					token.Length++;
				}

				goto FoundToken;
			}
			else if ( char_is_digit(current) )
			{
				// This is a very brute force lex, no checks are done for validity of literal.

				token.Text   = scanner;
				token.Length = 1;
				token.Type   = TokType::Number;
				move_forward();

				if (left
				&& (	current == 'x' || current == 'X'
					||	current == 'b' || current == 'B'
					||  current == 'o' || current == 'O' )
				)
				{
					move_forward();
					token.Length++;

					while ( left && char_is_hex_digit(current) )
					{
						move_forward();
						token.Length++;
					}

					goto FoundToken;
				}

				while ( left && char_is_digit(current) )
				{
					move_forward();
					token.Length++;
				}

				if ( left && current == '.' )
				{
					move_forward();
					token.Length++;

					while ( left && char_is_digit(current) )
					{
						move_forward();
						token.Length++;
					}
				}

				goto FoundToken;
			}
			else
			{
				String context_str = String::fmt_buf( GlobalAllocator, "%s", scanner, min( 100, left ) );

				log_failure( "Failed to lex token %s", context_str );

				// Skip to next whitespace since we can't know if anything else is valid until then.
				while ( left && ! char_is_space( current ) )
				{
					move_forward();
				}
			}

		FoundToken:

			if ( token.Type != TokType::Invalid )
			{
				if ( token.Type == TokType::Preprocessor_Directive && keep_preprocess_directives == false )
					continue;

				Tokens.append( token );
				continue;
			}

			TokType type = get_tok_type( token.Text, token.Length );

			if ( type == TokType::Invalid)
				type = TokType::Identifier;

			token.Type = type;
			Tokens.append( token );
		}

		if ( Tokens.num() == 0 )
		{
			log_failure( "Failed to lex any tokens" );
			return { { nullptr }, 0 };
		}

		return { Tokens, 0 };
	#	undef current
	#	undef move_forward
	#	undef SkipWhitespace
	#	undef SkipWhitespace_Checked
	}
}

#pragma region Helper Macros
#	define check_parse_args( func, def )                                       \
if ( def.Len <= 0 )                                                        \
{                                                                          \
	log_failure( "gen::" stringize(func) ": length must greater than 0" ); \
	return CodeInvalid;                                                    \
}                                                                          \
if ( def.Ptr == nullptr )                                                  \
{                                                                          \
	log_failure( "gen::" stringize(func) ": def was null" );               \
	return CodeInvalid;                                                    \
}

#	define nexttok 	    toks.next()
#	define currtok      toks.current()
#	define prevtok      toks.previous()
#	define eat( Type_ ) toks.__eat( Type_, context )
#	define left         ( toks.Arr.num() - toks.Idx )

#	define check( Type_ ) ( left && currtok.Type == Type_ )
#pragma endregion Helper Macros

struct ParseContext
{
	ParseContext* Parent;
	char const*   Fn;
};

internal Code parse_function_body( Parser::TokArray& toks, char const* context );
internal Code parse_global_nspace( Parser::TokArray& toks, char const* context );

internal CodeClass     parse_class           ( Parser::TokArray& toks, char const* context );
internal CodeEnum      parse_enum            ( Parser::TokArray& toks, char const* context );
internal CodeBody      parse_export_body     ( Parser::TokArray& toks, char const* context );
internal CodeBody      parse_extern_link_body( Parser::TokArray& toks, char const* context );
internal CodeExtern    parse_exten_link      ( Parser::TokArray& toks, char const* context );
internal CodeFriend    parse_friend          ( Parser::TokArray& toks, char const* context );
internal CodeFn        parse_function        ( Parser::TokArray& toks, char const* context );
internal CodeNamespace parse_namespace       ( Parser::TokArray& toks, char const* context );
internal CodeOpCast    parse_operator_cast   ( Parser::TokArray& toks, char const* context );
internal CodeStruct    parse_struct          ( Parser::TokArray& toks, char const* context );
internal CodeVar       parse_variable        ( Parser::TokArray& toks, char const* context );
internal CodeTemplate  parse_template        ( Parser::TokArray& toks, char const* context );
internal CodeType      parse_type            ( Parser::TokArray& toks, char const* context );
internal CodeTypedef   parse_typedef         ( Parser::TokArray& toks, char const* context );
internal CodeUnion     parse_union           ( Parser::TokArray& toks, char const* context );
internal CodeUsing     parse_using           ( Parser::TokArray& toks, char const* context );

internal inline
Code parse_array_decl( Parser::TokArray& toks, char const* context )
{
	using namespace Parser;

	if ( check( TokType::BraceSquare_Open ) )
	{
		eat( TokType::BraceSquare_Open );

		if ( left == 0 )
		{
			log_failure( "%s: Error, unexpected end of typedef definition ( '[]' scope started )", stringize(parse_typedef) );
			return Code::Invalid;
		}

		if ( currtok.Type == TokType::BraceSquare_Close )
		{
			log_failure( "%s: Error, empty array expression in typedef definition", stringize(parse_typedef) );
			return Code::Invalid;
		}

		Token untyped_tok = currtok;

		while ( left && currtok.Type != TokType::BraceSquare_Close )
		{
			eat( currtok.Type );
		}

		untyped_tok.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)untyped_tok.Text;

		Code array_expr = untyped_str( untyped_tok );

		if ( left == 0 )
		{
			log_failure( "%s: Error, unexpected end of type definition, expected ]", stringize(parse_typedef) );
			return Code::Invalid;
		}

		if ( currtok.Type != TokType::BraceSquare_Close )
		{
			log_failure( "%s: Error, expected ] in type definition, not %s", stringize(parse_typedef), str_tok_type( currtok.Type ) );
			return Code::Invalid;
		}

		eat( TokType::BraceSquare_Close );
		return array_expr;
	}

	return { nullptr };
}

internal inline
CodeAttributes parse_attributes( Parser::TokArray& toks, char const* context )
{
	using namespace Parser;

	Token start;
	s32 len = 0;

	if ( check(TokType::Attribute_Open) )
	{
		eat( TokType::Attribute_Open);

		while ( left && currtok.Type != TokType::Attribute_Close )
		{
			eat( currtok.Type );
		}

		eat( TokType::Attribute_Close );

		s32 len = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)start.Text;
	}

	else if ( check(TokType::Decl_GNU_Attribute) )
	{
		eat(TokType::BraceCurly_Open);
		eat(TokType::BraceCurly_Open);

		while ( left && currtok.Type != TokType::BraceCurly_Close )
		{
			eat(currtok.Type);
		}

		eat(TokType::BraceCurly_Close);
		eat(TokType::BraceCurly_Close);

		s32 len = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)start.Text;
	}

	else if ( check(TokType::Decl_MSVC_Attribute) )
	{
		eat( TokType::Decl_MSVC_Attribute );
		eat( TokType::BraceCurly_Open);

		while ( left && currtok.Type != TokType::BraceCurly_Close )
		{
			eat(currtok.Type);
		}

		eat(TokType::BraceCurly_Close);

		s32 len = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)start.Text;
	}

	else if ( tok_is_attribute( currtok ) )
	{
		eat(currtok.Type);
		s32 len = start.Length;
	}

	if ( len > 0 )
	{
		StrC attribute_txt = { len, start.Text };
		return def_attributes( attribute_txt );
	}

	return { nullptr };
}

internal inline
Parser::Token parse_identifier( Parser::TokArray& toks, char const* context )
{
	using namespace Parser;
	Token name = currtok;

	eat( TokType::Identifier );

	while ( check( TokType::Access_StaticSymbol ) )
	{
		eat( TokType::Access_StaticSymbol );

		if ( left == 0 )
		{
			log_failure( "%s: Error, unexpected end of type definition, expected identifier", context );
			return { nullptr, 0, TokType::Invalid };
		}

		if ( currtok.Type != TokType::Identifier )
		{
			log_failure( "%s: Error, expected identifier in type definition, not %s", context, str_tok_type( currtok.Type ) );
			return { nullptr, 0, TokType::Invalid };
		}

		name.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)name.Text;
		eat( TokType::Identifier );
	}

	return name;
}

internal
CodeParam parse_params( Parser::TokArray& toks, char const* context, bool use_template_capture = false )
{
	using namespace Parser;
	using namespace ECode;

	if ( ! use_template_capture )
		eat( TokType::Capture_Start );

	else
	{
		if ( check ( TokType::Operator ) && currtok.Text[0] == '<' )
			eat( TokType::Operator );
	}

	if ( ! use_template_capture &&  check(TokType::Capture_End) )
	{
		eat( TokType::Capture_End );
		return { nullptr };
	}

	CodeType type  = { nullptr };
	Code     value = { nullptr };

	if ( check( TokType::Varadic_Argument) )
	{
		eat( TokType::Varadic_Argument );

		return param_varadic;
	}

	type = parse_type( toks, context );
	if ( type == Code::Invalid )
		return CodeInvalid;

	Token name = { nullptr, 0, TokType::Invalid, false };

	if ( check( TokType::Identifier )  )
	{
		name = currtok;
		eat( TokType::Identifier );

		if ( currtok.IsAssign )
		{
			eat( TokType::Operator );

			Token value_tok = currtok;

			if ( currtok.Type == TokType::Statement_End )
			{
				log_failure( "gen::%s: Expected value after assignment operator", context );
				return CodeInvalid;
			}

			while ( left && currtok.Type != TokType::Statement_End )
			{
				value_tok.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)value_tok.Text;
				eat( currtok.Type );
			}

			value = parse_type( toks, context );
		}
	}

	CodeParam
	result       = (CodeParam) make_code();
	result->Type = Parameters;

	if ( name.Length > 0 )
		result->Name = get_cached_string( name );

	result->ValueType = type;

	if ( value )
		result->Value = value;

	result->NumEntries++;

	while ( left
		&& use_template_capture ?
				currtok.Type != TokType::Operator && currtok.Text[0] !=  '>'
			:	currtok.Type != TokType::Capture_End )
	{
		eat( TokType::Comma );

		Code type  = { nullptr };
		Code value = { nullptr };

		if ( check( TokType::Varadic_Argument) )
		{
			eat( TokType::Varadic_Argument );
			result.append( param_varadic );
			continue;
		}

		type = parse_type( toks, context );
		if ( type == Code::Invalid )
			return CodeInvalid;

		name = { nullptr, 0, TokType::Invalid, false };

		if ( check( TokType::Identifier )  )
		{
			name = currtok;
			eat( TokType::Identifier );

			if ( currtok.IsAssign )
			{
				eat( TokType::Operator );

				Token value_tok = currtok;

				if ( currtok.Type == TokType::Statement_End )
				{
					log_failure( "gen::%s: Expected value after assignment operator", context );
					return CodeInvalid;
				}

				while ( left && currtok.Type != TokType::Statement_End )
				{
					value_tok.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)value_tok.Text;
					eat( currtok.Type );
				}

				value = parse_type( toks, context );
			}
		}

		CodeParam
		param       = (CodeParam) make_code();
		param->Type = Parameters;

		if ( name.Length > 0 )
			param->Name = get_cached_string( name );

		param->ValueType = type;

		if ( value )
			param->Value = value;

		result.append( param );
	}

	if ( ! use_template_capture )
		eat( TokType::Capture_End );

	else
	{
		if ( ! check( TokType::Operator) || currtok.Text[0] != '>' )
		{
			log_failure("gen::parse_template: expected '<' after 'template' keyword. %s", str_tok_type( currtok.Type ));
			return CodeInvalid;
		}
		eat( TokType::Operator );
	}

	return result;
#	undef context
}

// Function parsing is handled in multiple places because its initial signature is shared with variable parsing
internal inline
CodeFn parse_function_after_name(
	  ModuleFlag        mflags
	, CodeAttributes    attributes
	, CodeSpecifiers    specifiers
	, CodeType          ret_type
	, StrC              name
	, Parser::TokArray& toks
	, char const*        context
)
{
	using namespace Parser;

	CodeParam params = parse_params( toks, stringize(parse_function) );

	while ( left && tok_is_specifier( currtok ) )
	{
		specifiers.append( ESpecifier::to_type(currtok) );
		eat( currtok.Type );
	}

	CodeBody body = { nullptr };
	if ( check( TokType::BraceCurly_Open ) )
	{
		body = parse_function_body( toks, stringize(parse_function) );
		if ( body == Code::Invalid )
			return CodeInvalid;
	}
	else
	{
		eat( TokType::Statement_End );
	}

	using namespace ECode;

	CodeFn
	result              = (CodeFn) make_code();
	result->Name        = get_cached_string( name );
	result->ModuleFlags = mflags;

	if ( body )
	{
		switch ( body->Type )
		{
			case Function_Body:
			case Untyped:
				break;

			default:
			{
				log_failure("gen::def_function: body must be either of Function_Body or Untyped type. %s", body.debug_str());
				return CodeInvalid;
			}
		}

		result->Type = Function;
		result->Body = body;
	}
	else
	{
		result->Type = Function_Fwd;
	}

	if ( specifiers )
		result->Specs = specifiers;

	result->ReturnType = ret_type;

	if ( params )
		result->Params = params;

	return result;
}

internal inline
CodeOperator parse_operator_after_ret_type( ModuleFlag mflags
	, CodeAttributes attributes
	, CodeSpecifiers  specifiers
	, CodeType       ret_type
	, Parser::TokArray& toks
	, char const* context )
{
	using namespace Parser;
	using namespace EOperator;

	// Parse Operator
	eat( TokType::Decl_Operator );

	if ( ! check( TokType::Operator ) )
	{
		log_failure( "gen::%s: Expected operator after 'operator' keyword", context );
		return CodeInvalid;
	}

	OperatorT op = Invalid;
	switch ( currtok.Text[0] )
	{
		case '+':
		{
			if ( currtok.Text[1] == '=' )
				op = Assign_Add;

			else
				op = Add;
		}
		break;
		case '-':
		{
			if ( currtok.Text[1] == '=' )
				op = Assign_Subtract;

			else
				op = Subtract;
		}
		break;
		case '*':
		{
			if ( currtok.Text[1] == '=' )
				op = Assign_Multiply;

			else
			{
				Token& finder = prevtok;
				while ( finder.Type != TokType::Decl_Operator )
				{
					if ( finder.Type == TokType::Identifier)
					{
						op = Indirection;
						break;
					}
				}

				if ( op == Invalid)
					op = Multiply;
			}
		}
		break;
		case '/':
		{
			if ( currtok.Text[1] == '=' )
				op = Assign_Divide;

			else
				op = Divide;
		}
		break;
		case '%':
		{
			if ( currtok.Text[1] == '=' )
				op = Assign_Modulo;

			else
				op = Modulo;
		}
		break;
		case '&':
		{
			if ( currtok.Text[1] == '=' )
				op = Assign_BAnd;

			else if ( currtok.Text[1] == '&' )
				op = LAnd;

			else
			{


				if ( op == Invalid )
				op = BAnd;
			}
		}
		break;
		case '|':
		{
			if ( currtok.Text[1] == '=' )
				op = Assign_BOr;

			else if ( currtok.Text[1] == '|' )
				op = LOr;

			else
				op = BOr;
		}
		break;
		case '^':
		{
			if ( currtok.Text[1] == '=' )
				op = Assign_BXOr;

			else
				op = BXOr;
		}
		break;
		case '~':
		{
			op = BNot;
		}
		break;
		case '!':
		{
			if ( currtok.Text[1] == '=' )
				op = LNot;

			else
				op = UnaryNot;
		}
		break;
		case '=':
		{
			if ( currtok.Text[1] == '=' )
				op = LEqual;

			else
				op = Assign;
		}
		break;
		case '<':
		{
			if ( currtok.Text[1] == '=' )
				op = LEqual;

			else if ( currtok.Text[1] == '<' )
			{
				if ( currtok.Text[2] == '=' )
					op = Assign_LShift;

				else
					op = LShift;
			}
			else
				op = Lesser;
		}
		break;
		case '>':
		{
			if ( currtok.Text[1] == '=' )
				op = GreaterEqual;

			else if ( currtok.Text[1] == '>' )
			{
				if ( currtok.Text[2] == '=' )
					op = Assign_RShift;

				else
					op = RShift;
			}
			else
				op = Greater;
		}
		break;
		case '(':
		{
			if ( currtok.Text[1] == ')' )
				op = FunctionCall;

			else
				op = Invalid;
		}
		break;
		case '[':
		{
			if ( currtok.Text[1] == ']' )
				op = Subscript;

			else
				op = Invalid;
		}
		break;
		default:
		{
			break;
		}
	}

	if ( op == Invalid )
	{
		log_failure( "gen::%s: Invalid operator '%s'", context, currtok.Text );
		return CodeInvalid;
	}

	eat( TokType::Operator );

	// Parse Params
	CodeParam params = parse_params( toks, stringize(parse_operator) );

	while ( left && tok_is_specifier( currtok ) )
	{
		specifiers.append( ESpecifier::to_type(currtok) );
		eat( currtok.Type );
	}

	// Parse Body
	CodeBody body = { nullptr };
	if ( check( TokType::BraceCurly_Open ) )
	{
		body = parse_function_body( toks, stringize(parse_function) );
		if ( body == Code::Invalid )
			return CodeInvalid;
	}
	else
	{
		eat( TokType::Statement_End );
	}

	// OpValidateResult check_result = operator__validate( op, params, ret_type, specifiers );
	CodeOperator result = def_operator( op, params, ret_type, body, specifiers, attributes, mflags );
	return result;
}

// Variable parsing is handled in multiple places because its initial signature is shared with function parsing
internal inline
CodeVar parse_variable_after_name(
	  ModuleFlag        mflags
	, CodeAttributes    attributes
	,CodeSpecifiers    specifiers
	, CodeType          type
	, StrC              name
	, Parser::TokArray& toks
	, char const*       context )
{
	using namespace Parser;

	Code array_expr = parse_array_decl( toks, stringize(parse_variable) );

	Code expr = { nullptr };

	if ( currtok.IsAssign )
	{
		eat( TokType::Operator );

		Token expr_tok = currtok;

		if ( currtok.Type == TokType::Statement_End )
		{
			log_failure( "gen::parse_variable: expected expression after assignment operator" );
			return CodeInvalid;
		}

		while ( left && currtok.Type != TokType::Statement_End )
		{
			eat( currtok.Type );
		}

		expr_tok.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)expr_tok.Text;
		expr            = untyped_str( expr_tok );
	}

	eat( TokType::Statement_End );

	using namespace ECode;

	CodeVar
	result              = (CodeVar) make_code();
	result->Type        = Variable;
	result->Name        = get_cached_string( name );
	result->ModuleFlags = mflags;

	result->ValueType = type;

	if (array_expr )
		type->ArrExpr = array_expr;

	if ( attributes )
		result->Attributes = attributes;

	if ( specifiers )
		result->Specs = specifiers;

	if ( expr )
		result->Value = expr;

	return result;
}

internal inline
Code parse_variable_assignment( Parser::TokArray& toks, char const* context )
{
	using namespace Parser;

	Code expr = Code::Invalid;

	if ( currtok.IsAssign )
	{
		eat( TokType::Operator );

		Token expr_tok = currtok;

		if ( currtok.Type == TokType::Statement_End )
		{
			log_failure( "gen::parse_variable: expected expression after assignment operator" );
			return Code::Invalid;
		}

		while ( left && currtok.Type != TokType::Statement_End )
		{
			expr_tok.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)expr_tok.Text;
			eat( currtok.Type );
		}

		expr = untyped_str( expr_tok );
	}

	return expr;
}

internal inline
Code parse_operator_function_or_variable( bool expects_function, CodeAttributes attributes, CodeSpecifiers specifiers, Parser::TokArray& toks, char const* context )
{
	using namespace Parser;

	Code result = Code::Invalid;

	CodeType type = parse_type( toks, stringize(parse_variable) );

	if ( type == Code::Invalid )
		return CodeInvalid;

	if ( check( TokType::Operator) )
	{
		// Dealing with an operator overload
		result = parse_operator_after_ret_type( ModuleFlag::None, attributes, specifiers, type, toks, stringize(parse_template) );
	}
	else
	{
		StrC name = currtok;
		eat( TokType::Identifier );

		if ( check( TokType::Capture_Start) )
		{
			// Dealing with a function

			result = parse_function_after_name( ModuleFlag::None, attributes, specifiers, type, name, toks, stringize(parse_template) );
		}
		else
		{
			if ( expects_function )
			{
				log_failure( "gen::parse_operator_function_or_variable: expected function declaration (consteval was used)" );
				return Code::Invalid;
			}

			// Dealing with a variable
			result = parse_variable_after_name( ModuleFlag::None, attributes, specifiers, type, name, toks, stringize(parse_template) );
		}
	}

	return result;
}

internal
CodeBody parse_class_struct_body( Parser::TokType which, Parser::TokArray& toks, char const* context )
{
	using namespace Parser;
	using namespace ECode;

	eat( TokType::BraceCurly_Open );

	CodeBody
	result = (CodeBody) make_code();

	if ( which == TokType::Decl_Class )
		result->Type = Class_Body;

	else
		result->Type = Struct_Body;

	while ( left && currtok.Type != TokType::BraceCurly_Close )
	{
		Code           member     = Code::Invalid;
		CodeAttributes attributes = { nullptr };
		CodeSpecifiers specifiers = { nullptr };

		bool expects_function = false;

		switch ( currtok.Type )
		{
			case TokType::Comment:
				member = def_comment( currtok );
				eat( TokType::Comment );
			break;

			case TokType::Access_Public:
				member = access_public;
				eat( TokType::Access_Public );
				eat( TokType::Assign_Classifer );
			break;

			case TokType::Access_Protected:
				member = access_protected;
				eat( TokType::Access_Protected );
				eat( TokType::Assign_Classifer );
			break;

			case TokType::Access_Private:
				member = access_private;
				eat( TokType::Access_Private );
				eat( TokType::Assign_Classifer );
			break;

			case TokType::Decl_Class:
				member = parse_class( toks, context );
			break;

			case TokType::Decl_Enum:
				member = parse_enum( toks, context );
			break;

			case TokType::Decl_Friend:
				member = parse_friend( toks, context );
			break;

			case TokType::Decl_Operator:
				member = parse_operator_cast( toks, context );
			break;

			case TokType::Decl_Struct:
				member = parse_struct( toks, context );
			break;

			case TokType::Decl_Template:
				member = parse_template( toks, context );
			break;

			case TokType::Decl_Typedef:
				member = parse_typedef( toks, context );
			break;

			case TokType::Decl_Union:
				member = parse_variable( toks, context );
			break;

			case TokType::Decl_Using:
				member = parse_using( toks, context );
			break;

			case TokType::Attribute_Open:
			case TokType::Decl_GNU_Attribute:
			case TokType::Decl_MSVC_Attribute:
		#define Entry( attribute, str ) case TokType::attribute:
			GEN_Define_Attribute_Tokens
		#undef Entry
			{
				attributes = parse_attributes( toks, context );
			}
			//! Fallthrough intended
			case TokType::Spec_Consteval:
			case TokType::Spec_Constexpr:
			case TokType::Spec_Constinit:
			case TokType::Spec_Inline:
			case TokType::Spec_Mutable:
			case TokType::Spec_Static:
			case TokType::Spec_Volatile:
			{
				SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
				s32        num_specifiers = 0;

				while ( left && tok_is_specifier( currtok ) )
				{
					SpecifierT spec = ESpecifier::to_type( currtok );

					switch ( spec )
					{
						case ESpecifier::Constexpr:
						case ESpecifier::Constinit:
						case ESpecifier::Inline:
						case ESpecifier::Mutable:
						case ESpecifier::Static:
						case ESpecifier::Volatile:
						break;

						case ESpecifier::Consteval:
							expects_function = true;
						break;

						default:
							log_failure( "gen::parse_class_struct_body: invalid specifier " "%s" " for variable", ESpecifier::to_str(spec) );
							return CodeInvalid;
					}

					specs_found[num_specifiers] = spec;
					num_specifiers++;
					eat( currtok.Type );
				}

				if ( num_specifiers )
				{
					specifiers = def_specifiers( num_specifiers, specs_found );
				}
			}
			//! Fallthrough intentional
			case TokType::Identifier:
			case TokType::Spec_Const:
			case TokType::Type_Unsigned:
			case TokType::Type_Signed:
			case TokType::Type_Short:
			case TokType::Type_Long:
			{
				member = parse_operator_function_or_variable( expects_function, attributes, specifiers, toks, context );
			}
			break;

			default:
				Token untyped_tok = currtok;

				while ( left && currtok.Type != TokType::BraceCurly_Close )
				{
					untyped_tok.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)untyped_tok.Text;
					eat( currtok.Type );
				}

				member = untyped_str( untyped_tok );
			break;
		}

		if ( member == Code::Invalid )
		{
			log_failure( "gen::parse_variable: failed to parse member" );
			return CodeInvalid;
		}

		result.append( member );
	}

	eat( TokType::BraceCurly_Close );
	return result;
}

internal
Code parse_class_struct( Parser::TokType which, Parser::TokArray& toks, char const* context )
{
	using namespace Parser;

	if ( which != TokType::Decl_Class && which != TokType::Decl_Struct )
	{
		log_failure( "%s: Error, expected class or struct, not %s", context, str_tok_type( which ) );
		return CodeInvalid;
	}

	Token name { nullptr, 0, TokType::Invalid };

	AccessSpec     access     = AccessSpec::Default;
	CodeType       parent     = { nullptr };
	CodeBody       body       = { nullptr };
	CodeAttributes attributes = { nullptr };
	ModuleFlag     mflags     = ModuleFlag::None;

	CodeClass result = CodeInvalid;

	if ( check(TokType::Module_Export) )
	{
		mflags = ModuleFlag::Export;
		eat( TokType::Module_Export );
	}

	eat( which );

	attributes = parse_attributes( toks, context );

	if ( check( TokType::Identifier ) )
		name = parse_identifier( toks, context );

	local_persist
	char interface_arr_mem[ kilobytes(4) ] {0};
	Array<CodeType> interfaces = Array<CodeType>::init_reserve( Arena::init_from_memory(interface_arr_mem, kilobytes(4) ), 4 );

	if ( check( TokType::Assign_Classifer ) )
	{
		eat( TokType::Assign_Classifer );

		if ( tok_is_access_specifier( currtok ) )
		{
			access = tok_to_access_specifier( currtok );
		}

		Token parent_tok = parse_identifier( toks, context );
		parent = def_type( parent_tok );

		while ( check(TokType::Comma) )
		{
			eat(TokType::Access_Public);

			if ( tok_is_access_specifier( currtok ) )
			{
				eat(currtok.Type);
			}

			Token interface_tok = parse_identifier( toks, context );

			interfaces.append( def_type( interface_tok ) );
		}
	}

	if ( check( TokType::BraceCurly_Open ) )
	{
		body = parse_class_struct_body( which, toks, context );
	}

	eat( TokType::Statement_End );

	if ( which == TokType::Decl_Class )
		result = def_class( name, body, parent, access
			, attributes
			, mflags
		);

	else
		result = def_struct( name, body, (CodeType)parent, access
			, attributes
			, mflags
		);

	interfaces.free();
	return result;
}

internal
Code parse_function_body( Parser::TokArray& toks, char const* context )
{
	using namespace Parser;
	using namespace ECode;

	eat( TokType::BraceCurly_Open );

	CodeBody
	result = (CodeBody) make_code();
	result->Type = Function_Body;

	Token start = currtok;

	s32 level = 0;
	while ( left && ( currtok.Type != TokType::BraceCurly_Close || level > 0 ) )
	{
		if ( currtok.Type == TokType::BraceCurly_Open )
			level++;

		else if ( currtok.Type == TokType::BraceCurly_Close && level > 0 )
			level--;

		eat( currtok.Type );
	}

	Token previous = prevtok;

	s32 len = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)start.Text;

	if ( len > 0 )
	{
		result.append( def_execution( { len, start.Text } ) );
	}

	eat( TokType::BraceCurly_Close );
	return result;
}

internal
CodeBody parse_global_nspace( CodeT which, Parser::TokArray& toks, char const* context )
{
	using namespace Parser;
	using namespace ECode;

	if ( which != Namespace_Body && which != Global_Body && which != Export_Body && which != Extern_Linkage_Body )
		return CodeInvalid;

	if ( which != Global_Body )
		eat( TokType::BraceCurly_Open );

	CodeBody
	result = (CodeBody) make_code();
	result->Type = which;

	while ( left && currtok.Type != TokType::BraceCurly_Close )
	{
		Code           member     = Code::Invalid;
		CodeAttributes attributes = { nullptr };
		CodeSpecifiers specifiers = { nullptr };

		bool expects_function = false;

		switch ( currtok.Type )
		{
			case TokType::Comment:
				member = def_comment( currtok );
				eat( TokType::Comment );
			break;

			case TokType::Decl_Enum:
				member = parse_enum( toks, context);
			break;

			case TokType::Decl_Class:
				member = parse_class( toks, context );
			break;

			case TokType::Decl_Extern_Linkage:
				if ( which == Extern_Linkage_Body )
					log_failure( "gen::parse_global_nspace: nested extern linkage" );

				member = parse_extern_link_body( toks, context );
			break;

			case TokType::Decl_Namespace:
				member = parse_namespace( toks, context );
			break;

			case TokType::Decl_Struct:
				member = parse_struct( toks, context );
			break;

			case TokType::Decl_Template:
				member = parse_template( toks, context );
			break;

			case TokType::Decl_Typedef:
				member = parse_typedef( toks, context );
			break;

			case TokType::Decl_Union:
				member = parse_union( toks, context );
			break;

			case TokType::Decl_Using:
				member = parse_using( toks, context );
			break;

			case TokType::Module_Export:
				if ( which == Export_Body )
					log_failure( "gen::parse_global_nspace: nested export declaration" );

				member = parse_export_body( toks, context );
			break;

			case TokType::Module_Import:
			{
				not_implemented( context );
			}
			//! Fallthrough intentional
			case TokType::Decl_GNU_Attribute:
			case TokType::Decl_MSVC_Attribute:
		#define Entry( attribute, str ) case TokType::attribute:
			GEN_Define_Attribute_Tokens
		#undef Entry
			{
				attributes = parse_attributes( toks, context );
			}
			//! Fallthrough intentional
			case TokType::Spec_Consteval:
			case TokType::Spec_Constexpr:
			case TokType::Spec_Constinit:
			case TokType::Spec_Extern:
			case TokType::Spec_Global:
			case TokType::Spec_Inline:
			case TokType::Spec_Internal_Linkage:
			case TokType::Spec_Static:
			{
				SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
				s32        num_specifiers = 0;

				while ( left && tok_is_specifier( currtok ) )
				{
					SpecifierT spec = ESpecifier::to_type( currtok );

					switch ( spec )
					{
						case ESpecifier::Constexpr:
						case ESpecifier::Constinit:
						case ESpecifier::Inline:
						case ESpecifier::Mutable:
						case ESpecifier::Static:
						case ESpecifier::Volatile:
						break;

						case ESpecifier::Consteval:
							expects_function = true;
						break;

						default:
							log_failure( "gen::parse_global_nspace: invalid specifier " "%s" " for variable", ESpecifier::to_str(spec) );
							return CodeInvalid;
					}

					specs_found[num_specifiers] = spec;
					num_specifiers++;
					eat( currtok.Type );
				}

				if ( num_specifiers )
				{
					specifiers = def_specifiers( num_specifiers, specs_found );
				}
			}
			//! Fallthrough intentional
			case TokType::Identifier:
			case TokType::Spec_Const:
			case TokType::Type_Long:
			case TokType::Type_Short:
			case TokType::Type_Signed:
			case TokType::Type_Unsigned:
			{
				member = parse_operator_function_or_variable( expects_function, attributes, specifiers, toks, context );
			}
		}

		if ( member == Code::Invalid )
		{
			log_failure( "gen::%s: failed to parse extern linkage member", context );
			return CodeInvalid;
		}

		result.append( member );
	}

	if ( which != Global_Body )
		eat( TokType::BraceCurly_Close );

	return result;
}

internal
CodeClass parse_class( Parser::TokArray& toks, char const* context )
{
	return (CodeClass) parse_class_struct( Parser::TokType::Decl_Class, toks, context );
}

CodeClass parse_class( StrC def )
{
	check_parse_args( parse_class, def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	return (CodeClass) parse_class_struct( TokType::Decl_Class, toks, stringize(parse_class) );
}

internal
CodeEnum parse_enum( Parser::TokArray& toks, char const* context )
{
	using namespace Parser;
	using namespace ECode;

	SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
	s32        num_specifiers = 0;

	Token    name       = { nullptr, 0, TokType::Invalid };
	Code     array_expr = { nullptr };
	CodeType type       = { nullptr };
	Token    body       = { nullptr, 0, TokType::Invalid };

	char  entries_code[ kilobytes(128) ] { 0 };
	s32   entries_length = 0;

	bool is_enum_class = false;

	eat( TokType::Decl_Enum );

	if ( currtok.Type == TokType::Decl_Class )
	{
		eat( TokType::Decl_Class);
		is_enum_class = true;
	}

	// TODO : Parse attributes

	if ( currtok.Type != TokType::Identifier )
	{
		log_failure( "gen::parse_enum: expected identifier for enum name" );
		return CodeInvalid;
	}

	name = currtok;
	eat( TokType::Identifier );

	if ( currtok.Type == TokType::Assign_Classifer )
	{
		eat( TokType::Assign_Classifer );

		type = parse_type( toks, stringize(parse_enum) );
		if ( type == Code::Invalid )
			return CodeInvalid;
	}

	if ( currtok.Type == TokType::BraceCurly_Open )
	{
		eat( TokType::BraceCurly_Open );

		body = currtok;

		while ( currtok.Type != TokType::BraceCurly_Close )
		{
			eat( TokType::Identifier);

			if ( currtok.Type == TokType::Operator && currtok.Text[0] == '=' )
			{
				eat( TokType::Operator );

				while ( currtok.Type != TokType::Comma && currtok.Type != TokType::BraceCurly_Close )
				{
					eat( currtok.Type );
				}
			}

			if ( currtok.Type == TokType::Comma )
			{
				eat( TokType::Comma );
			}
		}

		body.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)body.Text;

		eat( TokType::BraceCurly_Close );
	}

	eat( TokType::Statement_End );

	using namespace ECode;

	CodeEnum
	result = (CodeEnum) make_code();

	if ( body.Length )
	{
		// mem_copy( entries_code, body.Text, body.Length );

		Code untyped_body = untyped_str( body );

		result->Type = is_enum_class ? Enum_Class : Enum;
		result->Body = untyped_body;
	}
	else
	{
		result->Type = is_enum_class ? Enum_Class_Fwd : Enum_Fwd;
	}

	result->Name = get_cached_string( name );

	if ( type )
		result->UnderlyingType = type;

	return result;
}

CodeEnum parse_enum( StrC def )
{
	check_parse_args( parse_enum, def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	return parse_enum( toks, stringize(parse_enum) );
}

internal inline
CodeBody parse_export_body( Parser::TokArray& toks, char const* context )
{
	return parse_global_nspace( ECode::Export_Body, toks, context );
}

CodeBody parse_export_body( StrC def )
{
	check_parse_args( parse_export_body, def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	return parse_export_body( toks, stringize(parse_export_body) );
}

internal inline
CodeBody parse_extern_link_body( Parser::TokArray& toks, char const* context )
{
	return parse_global_nspace( ECode::Extern_Linkage_Body, toks, context );
}

internal
CodeExtern parse_extern_link( Parser::TokArray& toks, char const* context )
{
	using namespace Parser;

	eat( TokType::Decl_Extern_Linkage );

	Token name = currtok;
	eat( TokType::String );

	name.Text   += 1;
	name.Length -= 1;

	CodeExtern
	result       = (CodeExtern) make_code();
	result->Type = ECode::Extern_Linkage;
	result->Name = get_cached_string( name );

	Code entry = parse_extern_link_body( toks, context );
	if ( entry == Code::Invalid )
	{
		log_failure( "gen::parse_extern_link: failed to parse body" );
		return result;
	}

	result->Body = entry;

	return result;
}

CodeExtern parse_extern_link( StrC def )
{
	check_parse_args( parse_extern_link, def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	return parse_extern_link( toks, stringize(parse_extern_link) );
}

internal
CodeFriend parse_friend( Parser::TokArray& toks, char const* context )
{
	using namespace Parser;
	using namespace ECode;

	eat( TokType::Decl_Friend );

	CodeFn function = { nullptr };

	// Type declaration or return type
	CodeType type = parse_type( toks, stringize(parse_friend) );
	if ( type == Code::Invalid )
		return CodeInvalid;

	// Funciton declaration
	if ( currtok.Type == TokType::Identifier )
	{
		// Name
		Token name = parse_identifier( toks, stringize(parse_friend) );

		// Parameter list
		CodeParam params = parse_params( toks, stringize(parse_friend) );

		function             = make_code();
		function->Type       = Function_Fwd;
		function->Name       = get_cached_string( name );
		function->ReturnType = type;

		if ( params )
			function->Params = params;
	}

	eat( TokType::Statement_End );

	CodeFriend
	result       = (CodeFriend) make_code();
	result->Type = Friend;

	if ( function )
		result->Declaration = function;

	else
		result->Declaration = type;

	return result;
}

CodeFriend parse_friend( StrC def )
{
	check_parse_args( parse_friend, def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	return parse_friend( toks, stringize(parse_friend) );
}

internal
CodeFn parse_functon( Parser::TokArray& toks, char const* context )
{
	using namespace Parser;

	SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
	s32        num_specifiers = 0;

	CodeAttributes attributes = { nullptr };
	CodeSpecifiers  specifiers = { nullptr };
	ModuleFlag     mflags     = ModuleFlag::None;

	if ( check(TokType::Module_Export) )
	{
		mflags = ModuleFlag::Export;
		eat( TokType::Module_Export );
	}

	attributes = parse_attributes( toks, context );

	while ( left && tok_is_specifier( currtok ) )
	{
		SpecifierT spec = ESpecifier::to_type( currtok );

		switch ( spec )
		{
			case ESpecifier::Const:
			case ESpecifier::Consteval:
			case ESpecifier::Constexpr:
			case ESpecifier::External_Linkage:
			case ESpecifier::Inline:
			case ESpecifier::Static:
			break;

			default:
				log_failure( "gen::parse_functon: invalid specifier " "%s" " for functon", ESpecifier::to_str(spec) );
				return CodeInvalid;
		}

		if ( spec == ESpecifier::Const )
			continue;

		specs_found[num_specifiers] = spec;
		num_specifiers++;
		eat( currtok.Type );
	}

	if ( num_specifiers )
	{
		specifiers = def_specifiers( num_specifiers, specs_found );
	}

	CodeType ret_type = parse_type( toks, stringize(parse_function) );
	if ( ret_type == Code::Invalid )
		return CodeInvalid;

	Token name = parse_identifier( toks, stringize(parse_function) );
	if ( ! name )
		return CodeInvalid;

	CodeFn result = parse_function_after_name( mflags, attributes, specifiers, ret_type, name, toks, context );

	return result;
}

CodeFn parse_function( StrC def )
{
	using namespace Parser;

	check_parse_args( parse_function, def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	return (CodeFn) parse_functon( toks, stringize(parse_function) );
}

CodeBody parse_global_body( StrC def )
{
	check_parse_args( parse_global_body, def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	return parse_global_nspace( ECode::Global_Body, toks, stringize(parse_global_body) );
}

internal
CodeNamespace parse_namespace( Parser::TokArray& toks, char const* context )
{
	using namespace Parser;
	eat( TokType::Decl_Namespace );

	Token name = parse_identifier( toks, stringize(parse_namespace) );

	CodeBody body = parse_global_nspace( ECode::Namespace_Body, toks, stringize(parse_namespace) );
	if ( body == Code::Invalid )
		return CodeInvalid;

	CodeNamespace
	result       = (CodeNamespace) make_code();
	result->Type = ECode::Namespace;
	result->Name = get_cached_string( name );

	result->Body = body;

	return result;
}

CodeNamespace parse_namespace( StrC def )
{
	check_parse_args( parse_namespace, def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	return parse_namespace( toks, stringize(parse_namespace) );
}

internal
CodeOperator parse_operator( Parser::TokArray& toks, char const* context )
{
	using namespace Parser;

	CodeAttributes attributes = { nullptr };
	CodeSpecifiers specifiers = { nullptr };
	ModuleFlag     mflags     = ModuleFlag::None;

	SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
	s32        num_specifiers = 0;

	if ( check(TokType::Module_Export) )
	{
		mflags = ModuleFlag::Export;
		eat( TokType::Module_Export );
	}

	attributes = parse_attributes( toks, context );

	while ( left && tok_is_specifier( currtok ) )
	{
		SpecifierT spec = ESpecifier::to_type( currtok );

		switch ( spec )
		{
			case ESpecifier::Const:
			case ESpecifier::Constexpr:
			case ESpecifier::Inline:
			case ESpecifier::Static:
			break;

			default:
				log_failure( "gen::parse_operator: invalid specifier " "%s" " for operator", ESpecifier::to_str(spec) );
				return CodeInvalid;
		}

		if ( spec == ESpecifier::Const )
			continue;

		specs_found[num_specifiers] = spec;
		num_specifiers++;
		eat( currtok.Type );
	}

	if ( num_specifiers )
	{
		specifiers = def_specifiers( num_specifiers, specs_found );
	}

	// Parse Return Type
	CodeType ret_type = parse_type( toks, stringize(parse_operator) );

	CodeOperator result = parse_operator_after_ret_type( mflags, attributes, specifiers, ret_type, toks, context );
	return result;
}

CodeOperator parse_operator( StrC def )
{
	check_parse_args( parse_operator, def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	return (CodeOperator) parse_operator( toks, stringize(parse_operator) );
}

CodeOpCast parse_operator_cast( Parser::TokArray& toks, char const* context )
{
	using namespace Parser;

	eat( TokType::Decl_Operator );

	Code type = parse_type( toks, stringize(parse_operator_cast) );

	eat( TokType::Capture_Start );
	eat( TokType::Capture_End );

	CodeSpecifiers specifiers = { nullptr };

	if ( check(TokType::Spec_Const))
		specifiers = spec_const;

	Code body = { nullptr };

	if ( check( TokType::BraceCurly_Open) )
	{
		eat( TokType::BraceCurly_Open );

		Token body_str = currtok;

		s32 level = 0;
		while ( left && ( currtok.Type != TokType::BraceCurly_Close || level > 0 ) )
		{
			if ( currtok.Type == TokType::BraceCurly_Open )
				level++;

			else if ( currtok.Type == TokType::BraceCurly_Close && level > 0 )
				level--;

			eat( currtok.Type );
		}

		body_str.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)body_str.Text;

		body = untyped_str( body_str );

		eat( TokType::BraceCurly_Close );
	}

	CodeOpCast result = (CodeOpCast) make_code();

	if (body)
	{
		result->Type = ECode::Operator_Cast;
		result->Body = body;
	}
	else
	{
		result->Type = ECode::Operator_Cast_Fwd;
	}

	if ( specifiers )
		result->Specs = specifiers;

	result->ValueType = type;

	return result;
}

CodeOpCast parse_operator_cast( StrC def )
{
	check_parse_args( parse_operator_cast, def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	return parse_operator_cast( toks, stringize(parse_operator_cast) );
}

internal inline
CodeStruct parse_struct( Parser::TokArray& toks, char const* context )
{
	return (CodeStruct) parse_class_struct( Parser::TokType::Decl_Struct, toks, stringize(parse_struct) );
}

CodeStruct parse_struct( StrC def )
{
	check_parse_args( parse_struct, def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	return (CodeStruct) parse_class_struct( TokType::Decl_Struct, toks, stringize(parse_struct) );
}

internal
CodeTemplate parse_template( Parser::TokArray& toks, char const* context )
{
#	define UseTemplateCapture true

	using namespace Parser;

	ModuleFlag mflags = ModuleFlag::None;

	if ( check(TokType::Module_Export) )
	{
		mflags = ModuleFlag::Export;
		eat( TokType::Module_Export );
	}

	eat( TokType::Decl_Template );

	Code params = parse_params( toks, stringize(parse_template), UseTemplateCapture );
	if ( params == Code::Invalid )
		return CodeInvalid;

	Code definition = { nullptr };

	while ( left )
	{
		if ( check( TokType::Decl_Class ) )
		{
			definition = parse_class( toks, stringize(parse_template) );
			break;
		}

		if ( check( TokType::Decl_Struct ) )
		{
			definition = parse_enum( toks, stringize(parse_template) );
			break;
		}

		if ( check( TokType::Decl_Using ))
		{
			definition = parse_using( toks, stringize(parse_template) );
			break;
		}

		// Its either a function or a variable
		Token name = { nullptr, 0, TokType::Invalid };

		CodeAttributes attributes = { nullptr };
		CodeSpecifiers specifiers  = { nullptr };

		bool expects_function = false;

		SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
		s32        num_specifiers = 0;

		attributes = parse_attributes( toks, stringize(parse_template) );

		while ( left && tok_is_specifier( currtok ) )
		{
			SpecifierT spec = ESpecifier::to_type( currtok );

			switch ( spec )
			{
				case ESpecifier::Const:
				case ESpecifier::Constexpr:
				case ESpecifier::Constinit:
				case ESpecifier::External_Linkage:
				case ESpecifier::Global:
				case ESpecifier::Inline:
				case ESpecifier::Local_Persist:
				case ESpecifier::Mutable:
				case ESpecifier::Static:
				case ESpecifier::Thread_Local:
				case ESpecifier::Volatile:
				break;

				case ESpecifier::Consteval:
					expects_function = true;
				break;

				default:
					log_failure( "gen::parse_template: invalid specifier %s for variable or function", ESpecifier::to_str( spec ) );
					return CodeInvalid;
			}

			// Ignore const it will be handled by the type
			if ( spec == ESpecifier::Const )
				continue;

			specs_found[num_specifiers] = spec;
			num_specifiers++;
			eat( currtok.Type );
		}

		if ( num_specifiers )
		{
			specifiers = def_specifiers( num_specifiers, specs_found );
		}

		definition = parse_operator_function_or_variable( expects_function, attributes, specifiers, toks, stringize(parse_template) );
		break;
	}

	CodeTemplate
	result               = (CodeTemplate) make_code();
	result->Type        = ECode::Template;
	result->Params      = params;
	result->Declaration = definition;
	result->ModuleFlags = mflags;

	return result;
#	undef UseTemplateCapture
}

CodeTemplate parse_template( StrC def )
{
	check_parse_args( parse_template, def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	return parse_template( toks, stringize(parse_template) );
}

internal
CodeType parse_type( Parser::TokArray& toks, char const* context )
{
	using namespace Parser;

	Token context_tok = prevtok;

	SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
	s32        num_specifiers = 0;

	Token name      = { nullptr, 0, TokType::Invalid };
	Token brute_sig = { currtok.Text, 0, TokType::Invalid };

	CodeAttributes attributes = parse_attributes( toks, context );

	while ( left && tok_is_specifier( currtok ) )
	{
		SpecifierT spec = ESpecifier::to_type( currtok );

		if ( spec != ESpecifier::Const )
		{
			log_failure( "gen::parse_type: Error, invalid specifier used in type definition: %s", currtok.Text );
			return CodeInvalid;
		}

		specs_found[num_specifiers] = spec;
		num_specifiers++;
		eat( currtok.Type );
	}

	if ( left == 0 )
	{
		log_failure( "%s: Error, unexpected end of type definition", context );
		return CodeInvalid;
	}

	if (   currtok.Type == TokType::Decl_Class
		|| currtok.Type == TokType::Decl_Enum
		|| currtok.Type == TokType::Decl_Struct )
	{
		name = currtok;
		eat( currtok.Type );

		name.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)name.Text;
		eat( TokType::Identifier );
	}
	else if ( currtok.Type >= TokType::Type_Unsigned )
	{
		name = currtok;
		eat( currtok.Type );

		while (currtok.Type >= TokType::Type_Unsigned)
		{
			eat( currtok.Type );
		}

		name.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)name.Text;
	}
	else
	{
		name = parse_identifier( toks, context );
		if ( ! name )
			return CodeInvalid;

		// Problably dealing with a templated symbol
		if ( currtok.Type == TokType::Operator && currtok.Text[0] == '<' && currtok.Length == 1 )
		{
			eat( TokType::Operator );

			s32 level = 0;
			while ( left && ( currtok.Text[0] != '>' || level > 0 ))
			{
				if ( currtok.Text[0] == '<' )
					level++;

				if ( currtok.Text[0] == '>' )
					level--;

				eat( currtok.Type );
			}

			eat( TokType::Operator );

			// Extend length of name to last token
			name.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)name.Text;
		}
	}

	while ( left && tok_is_specifier( currtok ) )
	{
		SpecifierT spec = ESpecifier::to_type( currtok );

		if (   spec != ESpecifier::Const
			&& spec != ESpecifier::Ptr
			&& spec != ESpecifier::Ref
			&& spec != ESpecifier::RValue )
		{
			log_failure( "%s: Error, invalid specifier used in type definition: %s", context, currtok.Text );
			return CodeInvalid;
		}

		specs_found[num_specifiers] = spec;
		num_specifiers++;
		eat( currtok.Type );
	}

	// Not sure if its technically possible to cast ot a function pointer user defined operator cast...
	// Supporting it is not worth the effort.
	if ( check( TokType::Capture_Start ) && context_tok.Type != TokType::Decl_Operator )
	{
		// Its a function type
		eat( TokType::Capture_Start );

		while ( check( TokType::Star ) || currtok.Type == TokType::Spec_Const )
		{
			eat( currtok.Type );
		}

		// if its a using statement there will not be an ID.
		if ( check( TokType::Identifier ) )
			eat(TokType::Identifier);

		eat( TokType::Capture_End );

		// Parameters

		eat( TokType::Capture_Start );

		// TODO : Change this to validate the parameters...
		// Bruteforce lex the parameters, no validation.
		s32 level = 0;
		while ( ! check( TokType::Capture_End ) || level > 0 )
		{
			if ( check( TokType::Capture_Start ) )
				level++;

			if ( check( TokType::Capture_End ) )
				level--;

			eat( currtok.Type );
		}

		eat(TokType::Capture_End);

		brute_sig.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)brute_sig.Text;
	}

	using namespace ECode;

	CodeType
	result       = (CodeType) make_code();
	result->Type = Typename;

	if ( brute_sig.Length > 0 )
	{
		// Bruteforce all tokens together.
		name = brute_sig;
	}
	else
	{
		if (num_specifiers)
		{
			Code specifiers = def_specifiers( num_specifiers, (SpecifierT*)specs_found );
			result->Specs = specifiers;
		}
	}

	result->Name = get_cached_string( name );

	if ( attributes )
		result->Attributes = attributes;

	return result;
}

CodeType parse_type( StrC def )
{
	check_parse_args( parse_type, def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	CodeType result = parse_type( toks, stringize(parse_type) );

	return result;
}

internal
CodeTypedef parse_typedef( Parser::TokArray& toks, char const* context )
{
	using namespace Parser;

	Token name       = { nullptr, 0, TokType::Invalid };
	Code  array_expr = { nullptr };
	Code  type       = { nullptr };

	ModuleFlag mflags = ModuleFlag::None;

	if ( check(TokType::Module_Export) )
	{
		mflags = ModuleFlag::Export;
		eat( TokType::Module_Export );
	}

	eat( TokType::Decl_Typedef );

	if ( check( TokType::Decl_Enum ) )
		type = parse_enum( toks, context );

	else if ( check(TokType::Decl_Class ) )
		type = parse_class( toks, context );

	else if ( check(TokType::Decl_Struct ) )
		type = parse_struct( toks, context );

	else if ( check(TokType::Decl_Union) )
		type = parse_union( toks, context );

	else
		type = parse_type( toks, context );

	if ( ! check( TokType::Identifier ) )
	{
		log_failure( "gen::parse_typedef: Error, expected identifier for typedef" );
		return CodeInvalid;
	}

	name = currtok;
	eat( TokType::Identifier );

	array_expr = parse_array_decl( toks, context );

	eat( TokType::Statement_End );

	using namespace ECode;

	CodeTypedef
	result              = (CodeTypedef) make_code();
	result->Type        = Typedef;
	result->Name        = get_cached_string( name );
	result->ModuleFlags = mflags;

	result->UnderlyingType = type;

	if ( type->Type == Typename && array_expr && array_expr->Type != Invalid )
		type.cast<CodeType>()->ArrExpr = array_expr;

	return result;
}

CodeTypedef parse_typedef( StrC def )
{
	check_parse_args( parse_typedef, def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	return parse_typedef( toks, stringize(parse_typedef) );
}

internal
CodeUnion parse_union( Parser::TokArray& toks, char const* context )
{
	using namespace Parser;

	ModuleFlag mflags = ModuleFlag::None;

	if ( check(TokType::Module_Export) )
	{
		mflags = ModuleFlag::Export;
		eat( TokType::Module_Export );
	}

	eat( TokType::Decl_Union );

	CodeAttributes attributes = parse_attributes( toks, context );

	StrC name = { 0, nullptr };

	if ( check( TokType::Identifier ) )
	{
		name = currtok;
		eat( TokType::Identifier );
	}

	CodeBody body = { nullptr };

	eat( TokType::BraceCurly_Open );

	body = make_code();
	body->Type = ECode::Union_Body;

	while ( ! check( TokType::BraceCurly_Close ) )
	{
		Code entry = parse_variable( toks, context );

		if ( entry )
			body.append( entry );
	}

	eat( TokType::BraceCurly_Close );
	eat( TokType::Statement_End );

	CodeUnion
	result = (CodeUnion) make_code();
	result->Type        = ECode::Union;
	result->ModuleFlags = mflags;

	if ( name )
		result->Name = get_cached_string( name );

	if ( body )
		result->Body = body;

	if ( attributes )
		result->Attributes = attributes;

	return result;
}

CodeUnion parse_union( StrC def )
{
	check_parse_args( parse_union, def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	return parse_union( toks, stringize(parse_union) );
}

internal
CodeUsing parse_using( Parser::TokArray& toks, char const* context )
{
	using namespace Parser;

	SpecifierT specs_found[16] { ESpecifier::Invalid };
	s32        num_specifiers = 0;

	Token    name       = { nullptr, 0, TokType::Invalid };
	Code     array_expr = { nullptr };
	CodeType type       = { nullptr };

	bool is_namespace = false;

	ModuleFlag     mflags     = ModuleFlag::None;
	CodeAttributes attributes = { nullptr };

	if ( check(TokType::Module_Export) )
	{
		mflags = ModuleFlag::Export;
		eat( TokType::Module_Export );
	}

	eat( TokType::Decl_Using );

	if ( currtok.Type == TokType::Decl_Namespace )
	{
		is_namespace = true;
		eat( TokType::Decl_Namespace );
	}

	name = currtok;
	eat( TokType::Identifier );

	if ( currtok.IsAssign )
	{
		attributes = parse_attributes( toks, context );

		eat( TokType::Operator );

		type = parse_type( toks, context );
	}

	array_expr = parse_array_decl( toks, context );

	eat( TokType::Statement_End );

	using namespace ECode;

	CodeUsing
	result              = (CodeUsing) make_code();
	result->Name        = get_cached_string( name );
	result->ModuleFlags = mflags;

	if ( is_namespace)
	{
		result->Type = Using_Namespace;
	}
	else
	{
		result->Type = Using;

		if ( type )
			result->UnderlyingType = type;

		if ( array_expr )
			type->ArrExpr = array_expr;

		if ( attributes )
			result->Attributes = attributes;
	}
	return result;
}

CodeUsing parse_using( StrC def )
{
	check_parse_args( parse_using, def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	return parse_using( toks, stringize(parse_using) );
}

internal
CodeVar parse_variable( Parser::TokArray& toks, char const* context )
{
	using namespace Parser;

	Token name = { nullptr, 0, TokType::Invalid };

	SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
	s32        num_specifiers = 0;

	ModuleFlag	   mflags     = ModuleFlag::None;
	CodeAttributes attributes = { nullptr };
	CodeSpecifiers specifiers = { nullptr };

	if ( check(TokType::Module_Export) )
	{
		mflags = ModuleFlag::Export;
		eat( TokType::Module_Export );
	}

	attributes = parse_attributes( toks, context );

	while ( left && tok_is_specifier( currtok ) )
	{
		SpecifierT spec = ESpecifier::to_type( currtok );

		switch ( spec )
		{
			case ESpecifier::Const:
			case ESpecifier::Constexpr:
			case ESpecifier::Constinit:
			case ESpecifier::External_Linkage:
			case ESpecifier::Global:
			case ESpecifier::Inline:
			case ESpecifier::Local_Persist:
			case ESpecifier::Mutable:
			case ESpecifier::Static:
			case ESpecifier::Thread_Local:
			case ESpecifier::Volatile:
			break;

			default:
				log_failure( "gen::parse_variable: invalid specifier %s for variable", ESpecifier::to_str( spec ) );
				return CodeInvalid;
		}

		// Ignore const specifiers, they're handled by the type
		if ( spec == ESpecifier::Const )
			continue;

		specs_found[num_specifiers] = spec;
		num_specifiers++;
		eat( currtok.Type );
	}

	if ( num_specifiers )
	{
		specifiers = def_specifiers( num_specifiers, specs_found );
	}

	CodeType type = parse_type( toks, context );

	if ( type == Code::Invalid )
		return CodeInvalid;

	name = currtok;
	eat( TokType::Identifier );

	CodeVar result = parse_variable_after_name( mflags, attributes, specifiers, type, name, toks, context );

	return result;
}

CodeVar parse_variable( StrC def )
{
	check_parse_args( parse_variable, def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	return parse_variable( toks, stringize(parse_variable) );
}

// Undef helper macros
#	undef check_parse_args
#	undef curr_tok
#	undef eat
#	undef left

