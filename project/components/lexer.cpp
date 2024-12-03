#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "interface.upfront.cpp"
#include "gen/etoktype.cpp"
#endif

GEN_NS_PARSER_BEGIN

enum TokFlags : u32
{
	TF_Operator		   = bit(0),
	TF_Assign          = bit(1),
	TF_Preprocess      = bit(2),
	TF_Preprocess_Cond = bit(3),
	TF_Attribute       = bit(6),
	TF_AccessOperator  = bit( 7 ),
	TF_AccessSpecifier = bit( 8 ),
	TF_Specifier       = bit( 9 ),
	TF_EndDefinition   = bit( 10 ),    // Either ; or }
	TF_Formatting      = bit( 11 ),
	TF_Literal         = bit( 12 ),

	TF_Null = 0,
};

struct Token
{
	char const* Text;
	sptr        Length;
	TokType     Type;
	s32         Line;
	s32         Column;
	u32         Flags;
};

constexpr Token NullToken { nullptr, 0, Tok_Invalid, false, 0, TF_Null };

AccessSpec to_access_specifier(Token tok)
{
	return scast(AccessSpec, tok.Type);
}

StrC to_str(Token tok)
{
	return { tok.Length, tok.Text };
}

bool is_valid( Token tok )
{
	return tok.Text && tok.Length && tok.Type != Tok_Invalid;
}

bool is_access_operator(Token tok)
{
	return bitfield_is_equal( u32, tok.Flags, TF_AccessOperator );
}

bool is_access_specifier(Token tok)
{
	return bitfield_is_equal( u32, tok.Flags, TF_AccessSpecifier );
}

bool is_attribute(Token tok)
{
	return bitfield_is_equal( u32, tok.Flags, TF_Attribute );
}

bool is_operator(Token tok)
{
	return bitfield_is_equal( u32, tok.Flags, TF_Operator );
}

bool is_preprocessor(Token tok)
{
	return bitfield_is_equal( u32, tok.Flags, TF_Preprocess );
}

bool is_preprocess_cond(Token tok)
{
	return bitfield_is_equal( u32, tok.Flags, TF_Preprocess_Cond );
}

bool is_specifier(Token tok)
{
	return bitfield_is_equal( u32, tok.Flags, TF_Specifier );
}

bool is_end_definition(Token tok)
{
	return bitfield_is_equal( u32, tok.Flags, TF_EndDefinition );
}

String to_string(Token tok)
{
	String result = string_make_reserve( GlobalAllocator, kilobytes(4) );

	StrC type_str = to_str( tok.Type );

	append_fmt( & result, "Line: %d Column: %d, Type: %.*s Content: %.*s"
		, tok.Line, tok.Column
		, type_str.Len, type_str.Ptr
		, tok.Length, tok.Text
	);

	return result;
}

struct TokArray
{
	Array(Token) Arr;
	s32          Idx;
};

bool __eat( TokType type );

Token* current(TokArray* self, bool skip_formatting )
{
	if ( skip_formatting )
	{
		while ( self->Arr[self->Idx].Type == Tok_NewLine || self->Arr[self->Idx].Type == Tok_Comment  )
			self->Idx++;
	}

	return & self->Arr[self->Idx];
}

Token* previous(TokArray self, bool skip_formatting)
{
	s32 idx = self.Idx;

	if ( skip_formatting )
	{
		while ( self.Arr[idx].Type == Tok_NewLine )
			idx --;

		return & self.Arr[idx];
	}

	return & self.Arr[idx - 1];
}

Token* next(TokArray self, bool skip_formatting)
{
	s32 idx = self.Idx;

	if ( skip_formatting )
	{
		while ( self.Arr[idx].Type == Tok_NewLine )
			idx++;

		return & self.Arr[idx + 1];
	}

	return & self.Arr[idx + 1];
}

global Arena_256KB     defines_map_arena;
global HashTable(StrC) defines;
global Array(Token)    Tokens;

#define current ( * ctx->scanner )

#define move_forward()          \
	{                           \
		if ( current == '\n' )  \
		{                       \
			ctx->line++;        \
			ctx->column = 1;    \
		}                       \
		else                    \
		{                       \
			ctx->column++;      \
		}                       \
		ctx->left--;            \
		ctx->scanner++;         \
	}

#define SkipWhitespace()                             \
	while ( ctx->left && char_is_space( current ) )  \
	{                                                \
		move_forward();                              \
	}

#define end_line()                               \
	do                                           \
	{                                            \
		while ( ctx->left && current == ' ' )    \
		{                                        \
			move_forward();                      \
		}                                        \
		if ( ctx->left && current == '\r' )      \
		{                                        \
			move_forward();                      \
			move_forward();                      \
		}                                        \
		else if ( ctx->left && current == '\n' ) \
		{                                        \
			move_forward();                      \
		}                                        \
	}                                            \
	while (0)

enum
{
	Lex_Continue,
	Lex_ReturnNull,
};

struct LexContext
{
	StrC            content;
	s32             left;
	char const*     scanner;
	s32             line;
	s32             column;
	HashTable(StrC) defines;
	Token           token;
};

forceinline
s32 lex_preprocessor_directive( LexContext* ctx )
{
	char const* hash = ctx->scanner;
	append( & Tokens, { hash, 1, Tok_Preprocess_Hash, ctx->line, ctx->column, TF_Preprocess } );

	move_forward();
	SkipWhitespace();

	ctx->token.Text = ctx->scanner;
	while (ctx->left && ! char_is_space(current) )
	{
		move_forward();
		ctx->token.Length++;
	}

	ctx->token.Type = to_toktype( to_str(ctx->token) );

	bool   is_preprocessor = ctx->token.Type >= Tok_Preprocess_Define && ctx->token.Type <= Tok_Preprocess_Pragma;
	if ( ! is_preprocessor )
	{
		ctx->token.Type  = Tok_Preprocess_Unsupported;

		// Its an unsupported directive, skip it
		s32 within_string = false;
		s32 within_char   = false;
		while ( ctx->left )
		{
			if ( current == '"' && ! within_char )
				within_string ^= true;

			if ( current == '\'' && ! within_string )
				within_char ^= true;

			if ( current == '\\' && ! within_string && ! within_char )
			{
				move_forward();
				ctx->token.Length++;

				if ( current == '\r' )
				{
					move_forward();
					ctx->token.Length++;
				}

				if ( current == '\n' )
				{
					move_forward();
					ctx->token.Length++;
					continue;
				}
				else
				{
					log_failure( "gen::Parser::lex: Invalid escape sequence '\\%c' (%d, %d)"
								" in preprocessor directive (%d, %d)\n%.100s"
						, current, ctx->line, ctx->column
						, ctx->token.Line, ctx->token.Column, ctx->token.Text );
					break;
				}
			}

			if ( current == '\r' )
			{
				move_forward();
				ctx->token.Length++;
			}

			if ( current == '\n' )
			{
				move_forward();
				ctx->token.Length++;
				break;
			}

			move_forward();
			ctx->token.Length++;
		}

		ctx->token.Length = ctx->token.Length + ctx->token.Text - hash;
		ctx->token.Text   = hash;
		append( & Tokens, ctx->token );
		return Lex_Continue; // Skip found token, its all handled here.
	}

	if ( ctx->token.Type == Tok_Preprocess_Else || ctx->token.Type == Tok_Preprocess_EndIf )
	{
		ctx->token.Flags |= TF_Preprocess_Cond;
		append( & Tokens, ctx->token );
		end_line();
		return Lex_Continue;
	}
	else if ( ctx->token.Type >= Tok_Preprocess_If && ctx->token.Type <= Tok_Preprocess_ElIf  )
	{
		ctx->token.Flags |= TF_Preprocess_Cond;
	}

	append( & Tokens, ctx->token );

	SkipWhitespace();

	if ( ctx->token.Type == Tok_Preprocess_Define )
	{
		Token name = { ctx->scanner, 0, Tok_Identifier, ctx->line, ctx->column, TF_Preprocess };

		name.Text   = ctx->scanner;
		name.Length = 1;
		move_forward();

		while ( ctx->left && ( char_is_alphanumeric(current) || current == '_' ) )
		{
			move_forward();
			name.Length++;
		}

		if ( ctx->left && current == '(' )
		{
			move_forward();
			name.Length++;
		}

		append( & Tokens, name );

		u64 key = crc32( name.Text, name.Length );
		set(& ctx->defines, key, to_str(name) );
	}

	Token preprocess_content = { ctx->scanner, 0, Tok_Preprocess_Content, ctx->line, ctx->column, TF_Preprocess };

	if ( ctx->token.Type == Tok_Preprocess_Include )
	{
		preprocess_content.Type = Tok_String;

		if ( current != '"' && current != '<' )
		{
			String directive_str = string_fmt_buf( GlobalAllocator, "%.*s", min( 80, ctx->left + preprocess_content.Length ), ctx->token.Text );

			log_failure( "gen::Parser::lex: Expected '\"' or '<' after #include, not '%c' (%d, %d)\n%s"
				, current
				, preprocess_content.Line
				, preprocess_content.Column
				, (char*) directive_str
			);
			return Lex_ReturnNull;
		}
		move_forward();
		preprocess_content.Length++;

		while ( ctx->left && current != '"' && current != '>' )
		{
			move_forward();
			preprocess_content.Length++;
		}

		move_forward();
		preprocess_content.Length++;

		if ( current == '\r' && ctx->scanner[1] == '\n' )
		{
			move_forward();
			move_forward();
		}
		else if ( current == '\n' )
		{
			move_forward();
		}

		append( & Tokens, preprocess_content );
		return Lex_Continue; // Skip found token, its all handled here.
	}

	s32 within_string = false;
	s32 within_char   = false;

	// SkipWhitespace();
	while ( ctx->left )
	{
		if ( current == '"' && ! within_char )
			within_string ^= true;

		if ( current == '\'' && ! within_string )
			within_char ^= true;

		if ( current == '\\' && ! within_string && ! within_char )
		{
			move_forward();
			preprocess_content.Length++;

			if ( current == '\r' )
			{
				move_forward();
				preprocess_content.Length++;
			}

			if ( current == '\n' )
			{
				move_forward();
				preprocess_content.Length++;
				continue;
			}
			else
			{
				String directive_str = string_make_length( GlobalAllocator, ctx->token.Text, ctx->token.Length );
				String content_str   = string_fmt_buf( GlobalAllocator, "%.*s", min( 400, ctx->left + preprocess_content.Length ), preprocess_content.Text );

				log_failure( "gen::Parser::lex: Invalid escape sequence '\\%c' (%d, %d)"
							" in preprocessor directive '%s' (%d, %d)\n%s"
					, current, ctx->line, ctx->column
					, directive_str, preprocess_content.Line, preprocess_content.Column
					, content_str );
				break;
			}
		}

		if ( current == '\r' )
		{
			move_forward();
		}

		if ( current == '\n' )
		{
			move_forward();
			break;
		}

		move_forward();
		preprocess_content.Length++;
	}

	append( & Tokens, preprocess_content );
	return Lex_Continue; // Skip found token, its all handled here.
}

forceinline
void lex_found_token( LexContext* ctx )
{
	if ( ctx->token.Type != Tok_Invalid )
	{
		append( & Tokens, ctx->token );
		return;
	}

	TokType type = to_toktype( to_str(ctx->token) );

	if (type <= Tok_Access_Public && type >= Tok_Access_Private )
	{
		ctx->token.Flags |= TF_AccessSpecifier;
	}

	if ( type > Tok___Attributes_Start )
	{
		ctx->token.Flags |= TF_Attribute;
	}

	if ( type == Tok_Decl_Extern_Linkage )
	{
		SkipWhitespace();

		if ( current != '"' )
		{
			type         = Tok_Spec_Extern;
			ctx->token.Flags |= TF_Specifier;
		}

		ctx->token.Type = type;
		append( & Tokens, ctx->token );
		return;
	}

	if ( ( type <= Tok_Star && type >= Tok_Spec_Alignas)
			|| type == Tok_Ampersand
			|| type == Tok_Ampersand_DBL )
	{
		ctx->token.Type   = type;
		ctx->token.Flags |= TF_Specifier;
		append( & Tokens, ctx->token );
		return;
	}


	if ( type != Tok_Invalid )
	{
		ctx->token.Type = type;
		append( & Tokens, ctx->token );
		return;
	}

	u64 key = 0;
	if ( current == '(')
		key = crc32( ctx->token.Text, ctx->token.Length + 1 );
	else
		key = crc32( ctx->token.Text, ctx->token.Length );

	StrC* define = get(ctx->defines, key );
	if ( define )
	{
		ctx->token.Type = Tok_Preprocess_Macro;

		// Want to ignore any arguments the define may have as they can be execution expressions.
		if ( ctx->left && current == '(' )
		{
			move_forward();
			ctx->token.Length++;

			s32 level = 0;
			while ( ctx->left && (current != ')' || level > 0) )
			{
				if ( current == '(' )
					level++;

				else if ( current == ')' && level > 0 )
					level--;

				move_forward();
				ctx->token.Length++;
			}

			move_forward();
			ctx->token.Length++;
		}

		if ( current == '\r' && ctx->scanner[1] == '\n' )
		{
			move_forward();
		}
		else if ( current == '\n' )
		{
			move_forward();
		}
	}
	else
	{
		ctx->token.Type = Tok_Identifier;
	}

	append( & Tokens, ctx->token );
}


neverinline
// TokArray lex( Array<Token> tokens, StrC content )
TokArray lex( StrC content )
{
	LexContext c; LexContext* ctx = & c;
	c.content = content;
	c.left    = content.Len;
	c.scanner = content.Ptr;
	c.defines = defines;

	char const* word        = c.scanner;
	s32         word_length = 0;

	c.line   = 1;
	c.column = 1;

	SkipWhitespace();
	if ( c.left <= 0 )
	{
		log_failure( "gen::lex: no tokens found (only whitespace provided)" );
		return { {}, 0 };
	}

	foreach( StringCached*, entry, PreprocessorDefines )
	{
		s32         length  = 0;
		char const* scanner = * entry;
		while ( entry->Len > length && (char_is_alphanumeric( *scanner ) || *scanner == '_') )
		{
			c.scanner++;
			length ++;
		}
		if ( c.scanner[0] == '(' )
		{
			length++;
		}

		u64 key = crc32( * entry, length );
		set(& c.defines, key, (StrC) * entry );
	}

	clear(Tokens);

	while (c.left )
	{
		#if 0
		if (Tokens.num())
		{
			log_fmt("\nLastTok: %S", Tokens.back().to_string());
		}
		#endif

		c.token = { c.scanner, 0, Tok_Invalid, c.line, c.column, TF_Null };

		bool is_define = false;

		if ( c.column == 1 )
		{
			if ( current == '\r')
			{
				move_forward();
				c.token.Length = 1;
			}

			if ( current == '\n' )
			{
				move_forward();

				c.token.Type = Tok_NewLine;
				c.token.Length++;

				append( & Tokens, c.token );
				continue;
			}
		}

		c.token.Length = 0;

		SkipWhitespace();
		if ( c.left <= 0 )
			break;

		switch ( current )
		{
			case '#':
			{
				s32 result = lex_preprocessor_directive( ctx );
				switch ( result )
				{
					case Lex_Continue:
						continue;

					case Lex_ReturnNull:
						return { {}, 0 };
				}
			}
			case '.':
			{
				c.token.Text   = c.scanner;
				c.token.Length = 1;
				c.token.Type   = Tok_Access_MemberSymbol;
				c.token.Flags  = TF_AccessOperator;

				if (c.left) {
					move_forward();
				}

				if ( current == '.' )
				{
					move_forward();
					if( current == '.' )
					{
						c.token.Length = 3;
						c.token.Type   = Tok_Varadic_Argument;
						c.token.Flags  = TF_Null;
						move_forward();
					}
					else
					{
						String context_str = string_fmt_buf( GlobalAllocator, "%s", c.scanner, min( 100, c.left ) );

						log_failure( "gen::lex: invalid varadic argument, expected '...' got '..%c' (%d, %d)\n%s", current, c.line, c.column, context_str );
					}
				}

				goto FoundToken;
			}
			case '&' :
			{
				c.token.Text   = c.scanner;
				c.token.Length = 1;
				c.token.Type   = Tok_Ampersand;
				c.token.Flags |= TF_Operator;
				c.token.Flags |= TF_Specifier;

				if (c.left)
					move_forward();

				if ( current == '&' )	// &&
				{
					c.token.Length  = 2;
					c.token.Type    = Tok_Ampersand_DBL;

					if (c.left)
						move_forward();
				}

				goto FoundToken;
			}
			case ':':
			{
				c.token.Text   = c.scanner;
				c.token.Length = 1;
				c.token.Type   = Tok_Assign_Classifer;
				// Can be either a classifier (ParentType, Bitfield width), or ternary else
				// token.Type   = Tok_Colon;

				if (c.left)
					move_forward();

				if ( current == ':' )
				{
					move_forward();
					c.token.Type  = Tok_Access_StaticSymbol;
					c.token.Length++;
				}
				goto FoundToken;
			}
			case '{':
			{
				c.token.Text   = c.scanner;
				c.token.Length = 1;
				c.token.Type   = Tok_BraceCurly_Open;

				if (c.left)
					move_forward();
				goto FoundToken;
			}
			case '}':
			{
				c.token.Text   = c.scanner;
				c.token.Length = 1;
				c.token.Type   = Tok_BraceCurly_Close;
				c.token.Flags  = TF_EndDefinition;

				if (c.left)
					move_forward();

				end_line();
				goto FoundToken;
			}
			case '[':
			{
				c.token.Text   = c.scanner;
				c.token.Length = 1;
				c.token.Type   = Tok_BraceSquare_Open;
				if ( c.left )
				{
					move_forward();

					if ( current == ']' )
					{
						c.token.Length = 2;
						c.token.Type   = Tok_Operator;
						move_forward();
					}
				}
				goto FoundToken;
			}
			case ']':
			{
				c.token.Text   = c.scanner;
				c.token.Length = 1;
				c.token.Type   = Tok_BraceSquare_Close;

				if (c.left)
					move_forward();
				goto FoundToken;
			}
			case '(':
			{
				c.token.Text   = c.scanner;
				c.token.Length = 1;
				c.token.Type   = Tok_Capture_Start;

				if (c.left)
					move_forward();
				goto FoundToken;
			}
			case ')':
			{
				c.token.Text   = c.scanner;
				c.token.Length = 1;
				c.token.Type   = Tok_Capture_End;

				if (c.left)
					move_forward();
				goto FoundToken;
			}
			case '\'':
			{
				c.token.Text   = c.scanner;
				c.token.Length = 1;
				c.token.Type   = Tok_Char;
				c.token.Flags  = TF_Literal;

				move_forward();

				if ( c.left && current == '\\' )
				{
					move_forward();
					c.token.Length++;

					if ( current == '\'' )
					{
						move_forward();
						c.token.Length++;
					}
				}

				while ( c.left && current != '\'' )
				{
					move_forward();
					c.token.Length++;
				}

				if ( c.left )
				{
					move_forward();
					c.token.Length++;
				}
				goto FoundToken;
			}
			case ',':
			{
				c.token.Text   = c.scanner;
				c.token.Length = 1;
				c.token.Type   = Tok_Comma;
				c.token.Flags  = TF_Operator;

				if (c.left)
					move_forward();
				goto FoundToken;
			}
			case '*':
			{
				c.token.Text   = c.scanner;
				c.token.Length = 1;
				c.token.Type   = Tok_Star;
				c.token.Flags |= TF_Specifier;
				c.token.Flags |= TF_Operator;

				if (c.left)
					move_forward();

				if ( current == '=' )
				{
					c.token.Length++;
					c.token.Flags |= TF_Assign;
					// c.token.Type = Tok_Assign_Multiply;

					if ( c.left )
						move_forward();
				}

				goto FoundToken;
			}
			case ';':
			{
				c.token.Text   = c.scanner;
				c.token.Length = 1;
				c.token.Type   = Tok_Statement_End;
				c.token.Flags  = TF_EndDefinition;

				if (c.left)
					move_forward();

				end_line();
				goto FoundToken;
			}
			case '"':
			{
				c.token.Text   = c.scanner;
				c.token.Length = 1;
				c.token.Type   = Tok_String;
				c.token.Flags |= TF_Literal;

				move_forward();
				while ( c.left )
				{
					if ( current == '"' )
					{
						move_forward();
						break;
					}

					if ( current == '\\' )
					{
						move_forward();
						c.token.Length++;

						if ( c.left )
						{
							move_forward();
							c.token.Length++;
						}
						continue;
					}

					move_forward();
					c.token.Length++;
				}
				goto FoundToken;
			}
			case '?':
			{
				c.token.Text     = c.scanner;
				c.token.Length   = 1;
				c.token.Type     = Tok_Operator;
				// c.token.Type     = Tok_Ternary;
				c.token.Flags    = TF_Operator;

				if (c.left)
					move_forward();

				goto FoundToken;
			}
			case '=':
			{
				c.token.Text     = c.scanner;
				c.token.Length   = 1;
				c.token.Type     = Tok_Operator;
				// c.token.Type     = Tok_Assign;
				c.token.Flags    = TF_Operator;
				c.token.Flags   |= TF_Assign;

				if (c.left)
					move_forward();

				if ( current == '=' )
				{
					c.token.Length++;
					c.token.Flags = TF_Operator;

					if (c.left)
						move_forward();
				}

				goto FoundToken;
			}
			case '+':
			{
				// c.token.Type = Tok_Add

			}
			case '%':
			{
				// c.token.Type = Tok_Modulo;

			}
			case '^':
			{
				// c.token.Type = Tok_B_XOr;
			}
			case '~':
			{
				// c.token.Type = Tok_Unary_Not;

			}
			case '!':
			{
				// c.token.Type = Tok_L_Not;
			}
			case '<':
			{
				// c.token.Type = Tok_Lesser;

			}
			case '>':
			{
				// c.token.Type = Tok_Greater;

			}
			case '|':
			{
				c.token.Text   = c.scanner;
				c.token.Length = 1;
				c.token.Type   = Tok_Operator;
				c.token.Flags  = TF_Operator;
				// token.Type   = Tok_L_Or;

				if (c.left)
					move_forward();

				if ( current == '=' )
				{
					c.token.Length++;
					c.token.Flags |= TF_Assign;
					// token.Flags |= TokFlags::Assignment;
					// token.Type = Tok_Assign_L_Or;

					if (c.left)
						move_forward();
				}
				else while ( c.left && current == *(c.scanner - 1) && c.token.Length < 3 )
				{
					c.token.Length++;

					if (c.left)
						move_forward();
				}
				goto FoundToken;
			}

			// Dash is unfortunatlly a bit more complicated...
			case '-':
			{
				c.token.Text   = c.scanner;
				c.token.Length = 1;
				c.token.Type   = Tok_Operator;
				// token.Type = Tok_Subtract;
				c.token.Flags  = TF_Operator;
				if ( c.left )
				{
					move_forward();

					if ( current == '>'  )
					{
						c.token.Length++;
//						token.Type = Tok_Access_PointerToMemberSymbol;
						c.token.Flags |= TF_AccessOperator;
						move_forward();

						if ( current == '*' )
						{
//							token.Type = Tok_Access_PointerToMemberOfPointerSymbol;
							c.token.Length++;
							move_forward();
						}
					}
					else if ( current == '=' )
					{
						c.token.Length++;
						// token.Type = Tok_Assign_Subtract;
						c.token.Flags |= TF_Assign;

						if (c.left)
							move_forward();
					}
					else while ( c.left && current == *(c.scanner - 1) && c.token.Length < 3 )
					{
						c.token.Length++;

						if (c.left)
							move_forward();
					}
				}
				goto FoundToken;
			}
			case '/':
			{
				c.token.Text   = c.scanner;
				c.token.Length = 1;
				c.token.Type   = Tok_Operator;
				// token.Type   = Tok_Divide;
				c.token.Flags  = TF_Operator;
				move_forward();

				if ( c.left )
				{
					if ( current == '=' )
					{
						// token.Type = TokeType::Assign_Divide;
						move_forward();
						c.token.Length++;
						c.token.Flags = TF_Assign;
					}
					else if ( current == '/' )
					{
						c.token.Type   = Tok_Comment;
						c.token.Length = 2;
						c.token.Flags  = TF_Null;
						move_forward();

						while ( c.left && current != '\n' && current != '\r' )
						{
							move_forward();
							c.token.Length++;
						}

						if ( current == '\r' )
						{
							move_forward();
							c.token.Length++;
						}
						if ( current == '\n' )
						{
							move_forward();
							c.token.Length++;
						}
						append( & Tokens, c.token );
						continue;
					}
					else if ( current == '*' )
					{
						c.token.Type   = Tok_Comment;
						c.token.Length = 2;
						c.token.Flags  = TF_Null;
						move_forward();

						bool star   = current    == '*';
						bool slash  = c.scanner[1] == '/';
						bool at_end = star && slash;
						while ( c.left && ! at_end  )
						{
							move_forward();
							c.token.Length++;

							star   = current    == '*';
							slash  = c.scanner[1] == '/';
							at_end = star && slash;
						}
						c.token.Length += 2;
						move_forward();
						move_forward();

						if ( current == '\r' )
						{
							move_forward();
							c.token.Length++;
						}
						if ( current == '\n' )
						{
							move_forward();
							c.token.Length++;
						}
						append( & Tokens, c.token );
						// end_line();
						continue;
					}
				}
				goto FoundToken;
			}
		}

		if ( char_is_alpha( current ) || current == '_' )
		{
			c.token.Text   = c.scanner;
			c.token.Length = 1;
			move_forward();

			while ( c.left && ( char_is_alphanumeric(current) || current == '_' ) )
			{
				move_forward();
				c.token.Length++;
			}

			goto FoundToken;
		}
		else if ( char_is_digit(current) )
		{
			// This is a very brute force lex, no checks are done for validity of literal.

			c.token.Text   = c.scanner;
			c.token.Length = 1;
			c.token.Type   = Tok_Number;
			c.token.Flags  = TF_Literal;
			move_forward();

			if (c.left
			&& (	current == 'x' || current == 'X'
				||	current == 'b' || current == 'B'
				||  current == 'o' || current == 'O' )
			)
			{
				move_forward();
				c.token.Length++;

				while ( c.left && char_is_hex_digit(current) )
				{
					move_forward();
					c.token.Length++;
				}

				goto FoundToken;
			}

			while ( c.left && char_is_digit(current) )
			{
				move_forward();
				c.token.Length++;
			}

			if ( c.left && current == '.' )
			{
				move_forward();
				c.token.Length++;

				while ( c.left && char_is_digit(current) )
				{
					move_forward();
					c.token.Length++;
				}

				// Handle number literal suffixes in a botched way
				if (c.left && (
					current == 'l' || current == 'L' ||  // long/long long
					current == 'u' || current == 'U' ||  // unsigned
					current == 'f' || current == 'F' ||  // float
					current == 'i' || current == 'I' ||  // imaginary
					current == 'z' || current == 'Z'))   // complex
				{
					char prev = current;
					move_forward();
					c.token.Length++;

					// Handle 'll'/'LL' as a special case when we just processed an 'l'/'L'
					if (c.left && (prev == 'l' || prev == 'L') && (current == 'l' || current == 'L'))
					{
						move_forward();
						c.token.Length++;
					}
				}
			}

			goto FoundToken;
		}
		else
		{
			s32 start = max( 0, num(Tokens) - 100 );
			log_fmt("\n%d\n", start);
			for ( s32 idx = start; idx < num(Tokens); idx++ )
			{
				log_fmt( "Token %d Type: %s : %.*s\n"
					, idx
					, to_str( Tokens[ idx ].Type ).Ptr
					, Tokens[ idx ].Length, Tokens[ idx ].Text
				);
			}

			String context_str = string_fmt_buf( GlobalAllocator, "%.*s", min( 100, c.left ), c.scanner );
			log_failure( "Failed to lex token '%c' (%d, %d)\n%s", current, c.line, c.column, context_str );

			// Skip to next whitespace since we can't know if anything else is valid until then.
			while ( c.left && ! char_is_space( current ) )
			{
				move_forward();
			}
		}

	FoundToken:
		lex_found_token( ctx );
	}

	if ( num(Tokens) == 0 )
	{
		log_failure( "Failed to lex any tokens" );
		return { {}, 0 };
	}

	clear(defines);
	// defines_map_arena.free();
	return { Tokens, 0 };
}
#undef current
#undef move_forward
#undef SkipWhitespace

GEN_NS_PARSER_END
