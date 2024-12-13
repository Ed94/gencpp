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
	TF_UnderlyingType = GEN_U32_MAX,
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

AccessSpec tok_to_access_specifier(Token tok)
{
	return scast(AccessSpec, tok.Type);
}

Str tok_to_str(Token tok)
{
	Str str = { tok.Text, tok.Length };
	return str;
}

bool tok_is_valid( Token tok )
{
	return tok.Text && tok.Length && tok.Type != Tok_Invalid;
}

bool tok_is_access_operator(Token tok)
{
	return bitfield_is_equal( u32, tok.Flags, TF_AccessOperator );
}

bool tok_is_access_specifier(Token tok)
{
	return bitfield_is_equal( u32, tok.Flags, TF_AccessSpecifier );
}

bool tok_is_attribute(Token tok)
{
	return bitfield_is_equal( u32, tok.Flags, TF_Attribute );
}

bool tok_is_operator(Token tok)
{
	return bitfield_is_equal( u32, tok.Flags, TF_Operator );
}

bool tok_is_preprocessor(Token tok)
{
	return bitfield_is_equal( u32, tok.Flags, TF_Preprocess );
}

bool tok_is_preprocess_cond(Token tok)
{
	return bitfield_is_equal( u32, tok.Flags, TF_Preprocess_Cond );
}

bool tok_is_specifier(Token tok)
{
	return bitfield_is_equal( u32, tok.Flags, TF_Specifier );
}

bool tok_is_end_definition(Token tok)
{
	return bitfield_is_equal( u32, tok.Flags, TF_EndDefinition );
}

StrBuilder tok_to_strbuilder(Token tok)
{
	StrBuilder result = strbuilder_make_reserve( GlobalAllocator, kilobytes(4) );

	Str type_str = toktype_to_str( tok.Type );

	strbuilder_append_fmt( & result, "Line: %d Column: %d, Type: %.*s Content: %.*s"
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

bool lex__eat( TokArray* self, TokType type );

Token* lex_current(TokArray* self, bool skip_formatting )
{
	if ( skip_formatting )
	{
		while ( self->Arr[self->Idx].Type == Tok_NewLine || self->Arr[self->Idx].Type == Tok_Comment  )
			self->Idx++;
	}

	return & self->Arr[self->Idx];
}

Token* lex_peek(TokArray self, bool skip_formatting)
{
	s32 idx = self.Idx;

	if ( skip_formatting )
	{
		while ( self.Arr[idx].Type == Tok_NewLine )
			idx++;

		return & self.Arr[idx];
	}

	return & self.Arr[idx];
}

Token* lex_previous(TokArray self, bool skip_formatting)
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

Token* lex_next(TokArray self, bool skip_formatting)
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

global FixedArena_256KB Lexer_defines_map_arena;
global StringTable      Lexer_defines;
global Array(Token)     Lexer_Tokens;

enum
{
	Lex_Continue,
	Lex_ReturnNull,
};

struct LexContext
{
	Str             content;
	s32             left;
	char const*     scanner;
	s32             line;
	s32             column;
	StringTable     defines;
	Token           token;
};

forceinline
void lexer_move_forward( LexContext* ctx )
{
	if ( * ctx->scanner == '\n' ) {
		ctx->line   += 1;
		ctx->column  = 1;
	}
	else {
		++ ctx->column;
	}
	-- ctx->left;
	++ ctx->scanner;
}
#define move_forward() lexer_move_forward(ctx)

forceinline
void lexer_skip_whitespace( LexContext* ctx )
{
	while ( ctx->left && char_is_space( * ctx->scanner ) )
		move_forward();
}
#define skip_whitespace() lexer_skip_whitespace(ctx)

forceinline
void lexer_end_line( LexContext* ctx )
{
	while ( ctx->left && (* ctx->scanner) == ' ' )
		move_forward();

	if ( ctx->left && (* ctx->scanner) == '\r' ) {
		move_forward();
		move_forward();
	}
	else if ( ctx->left && (* ctx->scanner) == '\n' )
		move_forward();
}
#define end_line() lexer_end_line(ctx)

forceinline
s32 lex_preprocessor_directive( LexContext* ctx )
{
	char const* hash = ctx->scanner;
	Token hash_tok = { hash, 1, Tok_Preprocess_Hash, ctx->line, ctx->column, TF_Preprocess };
	array_append( Lexer_Tokens, hash_tok  );

	move_forward();
	skip_whitespace();

	ctx->token.Text = ctx->scanner;
	while (ctx->left && ! char_is_space((* ctx->scanner)) )
	{
		move_forward();
		ctx->token.Length++;
	}

	ctx->token.Type = str_to_toktype( tok_to_str(ctx->token) );

	bool   is_preprocessor = ctx->token.Type >= Tok_Preprocess_Define && ctx->token.Type <= Tok_Preprocess_Pragma;
	if ( ! is_preprocessor )
	{
		ctx->token.Type  = Tok_Preprocess_Unsupported;

		// Its an unsupported directive, skip it
		s32 within_string = false;
		s32 within_char   = false;
		while ( ctx->left )
		{
			if ( * ctx->scanner == '"' && ! within_char )
				within_string ^= true;

			if ( * ctx->scanner == '\'' && ! within_string )
				within_char ^= true;

			if ( * ctx->scanner == '\\' && ! within_string && ! within_char )
			{
				move_forward();
				ctx->token.Length++;

				if ( (* ctx->scanner) == '\r' )
				{
					move_forward();
					ctx->token.Length++;
				}

				if ( (* ctx->scanner) == '\n' )
				{
					move_forward();
					ctx->token.Length++;
					continue;
				}
				else
				{
					log_failure( "gen::Parser::lex: Invalid escape sequence '\\%c' (%d, %d)"
								" in preprocessor directive (%d, %d)\n%.100s"
						, (* ctx->scanner), ctx->line, ctx->column
						, ctx->token.Line, ctx->token.Column, ctx->token.Text );
					break;
				}
			}

			if ( (* ctx->scanner) == '\r' )
			{
				move_forward();
				ctx->token.Length++;
			}

			if ( (* ctx->scanner) == '\n' )
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
		array_append( Lexer_Tokens, ctx->token );
		return Lex_Continue; // Skip found token, its all handled here.
	}

	if ( ctx->token.Type == Tok_Preprocess_Else || ctx->token.Type == Tok_Preprocess_EndIf )
	{
		ctx->token.Flags |= TF_Preprocess_Cond;
		array_append( Lexer_Tokens, ctx->token );
		end_line();
		return Lex_Continue;
	}
	else if ( ctx->token.Type >= Tok_Preprocess_If && ctx->token.Type <= Tok_Preprocess_ElIf  )
	{
		ctx->token.Flags |= TF_Preprocess_Cond;
	}

	array_append( Lexer_Tokens, ctx->token );

	skip_whitespace();

	if ( ctx->token.Type == Tok_Preprocess_Define )
	{
		Token name = { ctx->scanner, 0, Tok_Identifier, ctx->line, ctx->column, TF_Preprocess };

		name.Text   = ctx->scanner;
		name.Length = 1;
		move_forward();

		while ( ctx->left && ( char_is_alphanumeric((* ctx->scanner)) || (* ctx->scanner) == '_' ) )
		{
			move_forward();
			name.Length++;
		}

		if ( ctx->left && (* ctx->scanner) == '(' )
		{
			move_forward();
			name.Length++;
		}

		array_append( Lexer_Tokens, name );

		u64 key = crc32( name.Text, name.Length );
		hashtable_set(ctx->defines, key, tok_to_str(name) );
	}

	Token preprocess_content = { ctx->scanner, 0, Tok_Preprocess_Content, ctx->line, ctx->column, TF_Preprocess };

	if ( ctx->token.Type == Tok_Preprocess_Include )
	{
		preprocess_content.Type = Tok_String;

		if ( (* ctx->scanner) != '"' && (* ctx->scanner) != '<' )
		{
			StrBuilder directive_str = strbuilder_fmt_buf( GlobalAllocator, "%.*s", min( 80, ctx->left + preprocess_content.Length ), ctx->token.Text );

			log_failure( "gen::Parser::lex: Expected '\"' or '<' after #include, not '%c' (%d, %d)\n%s"
				, (* ctx->scanner)
				, preprocess_content.Line
				, preprocess_content.Column
				, (char*) directive_str
			);
			return Lex_ReturnNull;
		}
		move_forward();
		preprocess_content.Length++;

		while ( ctx->left && (* ctx->scanner) != '"' && (* ctx->scanner) != '>' )
		{
			move_forward();
			preprocess_content.Length++;
		}

		move_forward();
		preprocess_content.Length++;

		if ( (* ctx->scanner) == '\r' && ctx->scanner[1] == '\n' )
		{
			move_forward();
			move_forward();
		}
		else if ( (* ctx->scanner) == '\n' )
		{
			move_forward();
		}

		array_append( Lexer_Tokens, preprocess_content );
		return Lex_Continue; // Skip found token, its all handled here.
	}

	s32 within_string = false;
	s32 within_char   = false;

	// SkipWhitespace();
	while ( ctx->left )
	{
		if ( (* ctx->scanner) == '"' && ! within_char )
			within_string ^= true;

		if ( (* ctx->scanner) == '\'' && ! within_string )
			within_char ^= true;

		if ( (* ctx->scanner) == '\\' && ! within_string && ! within_char )
		{
			move_forward();
			preprocess_content.Length++;

			if ( (* ctx->scanner) == '\r' )
			{
				move_forward();
				preprocess_content.Length++;
			}

			if ( (* ctx->scanner) == '\n' )
			{
				move_forward();
				preprocess_content.Length++;
				continue;
			}
			else
			{
				StrBuilder directive_str = strbuilder_make_length( GlobalAllocator, ctx->token.Text, ctx->token.Length );
				StrBuilder content_str   = strbuilder_fmt_buf( GlobalAllocator, "%.*s", min( 400, ctx->left + preprocess_content.Length ), preprocess_content.Text );

				log_failure( "gen::Parser::lex: Invalid escape sequence '\\%c' (%d, %d)"
							" in preprocessor directive '%s' (%d, %d)\n%s"
					, (* ctx->scanner), ctx->line, ctx->column
					, directive_str, preprocess_content.Line, preprocess_content.Column
					, content_str );
				break;
			}
		}

		if ( (* ctx->scanner) == '\r' )
		{
			break;
			//move_forward();
		}

		if ( (* ctx->scanner) == '\n' )
		{
			//move_forward();
			break;
		}

		move_forward();
		preprocess_content.Length++;
	}

	array_append( Lexer_Tokens, preprocess_content );
	return Lex_Continue; // Skip found token, its all handled here.
}

forceinline
void lex_found_token( LexContext* ctx )
{
	if ( ctx->token.Type != Tok_Invalid )
	{
		array_append( Lexer_Tokens, ctx->token );
		return;
	}

	TokType type = str_to_toktype( tok_to_str(ctx->token) );

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
		skip_whitespace();

		if ( (* ctx->scanner) != '"' )
		{
			type         = Tok_Spec_Extern;
			ctx->token.Flags |= TF_Specifier;
		}

		ctx->token.Type = type;
		array_append( Lexer_Tokens, ctx->token );
		return;
	}

	if ( ( type <= Tok_Star && type >= Tok_Spec_Alignas)
			|| type == Tok_Ampersand
			|| type == Tok_Ampersand_DBL )
	{
		ctx->token.Type   = type;
		ctx->token.Flags |= TF_Specifier;
		array_append( Lexer_Tokens, ctx->token );
		return;
	}


	if ( type != Tok_Invalid )
	{
		ctx->token.Type = type;
		array_append( Lexer_Tokens, ctx->token );
		return;
	}

	u64 key = 0;
	if ( (* ctx->scanner) == '(')
		key = crc32( ctx->token.Text, ctx->token.Length + 1 );
	else
		key = crc32( ctx->token.Text, ctx->token.Length );

	Str* define = hashtable_get(ctx->defines, key );
	if ( define )
	{
		ctx->token.Type = Tok_Preprocess_Macro;

		// Want to ignore any arguments the define may have as they can be execution expressions.
		if ( ctx->left && (* ctx->scanner) == '(' )
		{
			move_forward();
			ctx->token.Length++;

			s32 level = 0;
			while ( ctx->left && ((* ctx->scanner) != ')' || level > 0) )
			{
				if ( (* ctx->scanner) == '(' )
					level++;

				else if ( (* ctx->scanner) == ')' && level > 0 )
					level--;

				move_forward();
				ctx->token.Length++;
			}

			move_forward();
			ctx->token.Length++;
		}

		//if ( (* ctx->scanner) == '\r' && ctx->scanner[1] == '\n' )
		//{
		//	move_forward();
		//	ctx->token.Length++;
		//}
		//else if ( (* ctx->scanner) == '\n' )
		//{
		//	move_forward();
		//	ctx->token.Length++;
		//}
	}
	else
	{
		ctx->token.Type = Tok_Identifier;
	}

	array_append( Lexer_Tokens, ctx->token );
}

neverinline
// TokArray lex( Array<Token> tokens, Str content )
TokArray lex( Str content )
{
	LexContext c; LexContext* ctx = & c;
	c.content = content;
	c.left    = content.Len;
	c.scanner = content.Ptr;
	c.defines = Lexer_defines;

	char const* word        = c.scanner;
	s32         word_length = 0;

	c.line   = 1;
	c.column = 1;

	skip_whitespace();
	if ( c.left <= 0 )
	{
		log_failure( "gen::lex: no tokens found (only whitespace provided)" );
		TokArray null_array = {};
		return null_array;
	}

	for ( StringCached* entry = array_begin(PreprocessorDefines); entry != array_end(PreprocessorDefines); entry = array_next(PreprocessorDefines, entry))
	{
		s32         length  = 0;
		char const* entry_scanner = (*entry).Ptr;
		while ( entry->Len > length && (char_is_alphanumeric( *entry_scanner ) || *entry_scanner == '_') )
		{
			entry_scanner++;
			length ++;
		}
		if ( entry_scanner[0] == '(' )
		{
			length++;
		}

		u64 key = crc32( entry->Ptr, length );
		hashtable_set(c.defines, key, * entry );
	}

	array_clear(Lexer_Tokens);

	while (c.left )
	{
		#if 0
		if (Tokens.num())
		{
			log_fmt("\nLastTok: %SB", Tokens.back().to_strbuilder());
		}
		#endif

		{
			Token thanks_c = { c.scanner, 0, Tok_Invalid, c.line, c.column, TF_Null };
			c.token = thanks_c;
		}

		bool is_define = false;

		if ( c.column == 1 )
		{
			if ( (* ctx->scanner) == '\r')
			{
				move_forward();
				c.token.Length = 1;
			}

			if ( (* ctx->scanner) == '\n' )
			{
				move_forward();

				c.token.Type = Tok_NewLine;
				c.token.Length++;

				array_append( Lexer_Tokens, c.token );
				continue;
			}
		}

		c.token.Length = 0;

		skip_whitespace();
		if ( c.left <= 0 )
			break;

		switch ( (* ctx->scanner) )
		{
			case '#':
			{
				s32 result = lex_preprocessor_directive( ctx );
				switch ( result )
				{
					case Lex_Continue:
					{
						//TokType last_type = Tokens[array_get_header(Tokens)->Num - 2].Type;
						//if ( last_type == Tok_Preprocess_Pragma )
						{
							{
								Token thanks_c = { c.scanner, 0, Tok_Invalid, c.line, c.column, TF_Null };
								c.token = thanks_c;
							}
							if ( (* ctx->scanner) == '\r')
							{
								move_forward();
								c.token.Length = 1;
							}

							if ( (* ctx->scanner) == '\n' )
							{
								c.token.Type = Tok_NewLine;
								c.token.Length++;
								move_forward();

								array_append( Lexer_Tokens, c.token );
							}
						}
						continue;
					}

					case Lex_ReturnNull:
					{
						TokArray tok_array =  {};
						return tok_array;
					}
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

				if ( (* ctx->scanner) == '.' )
				{
					move_forward();
					if( (* ctx->scanner) == '.' )
					{
						c.token.Length = 3;
						c.token.Type   = Tok_Varadic_Argument;
						c.token.Flags  = TF_Null;
						move_forward();
					}
					else
					{
						StrBuilder context_str = strbuilder_fmt_buf( GlobalAllocator, "%s", c.scanner, min( 100, c.left ) );

						log_failure( "gen::lex: invalid varadic argument, expected '...' got '..%c' (%d, %d)\n%s", (* ctx->scanner), c.line, c.column, context_str );
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

				if ( (* ctx->scanner) == '&' )	// &&
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

				if ( (* ctx->scanner) == ':' )
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

					if ( (* ctx->scanner) == ']' )
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

				if ( c.left && (* ctx->scanner) == '\\' )
				{
					move_forward();
					c.token.Length++;

					if ( (* ctx->scanner) == '\'' )
					{
						move_forward();
						c.token.Length++;
					}
				}

				while ( c.left && (* ctx->scanner) != '\'' )
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

				if ( (* ctx->scanner) == '=' )
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
					if ( (* ctx->scanner) == '"' )
					{
						move_forward();
						break;
					}

					if ( (* ctx->scanner) == '\\' )
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

				if ( (* ctx->scanner) == '=' )
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

				if ( (* ctx->scanner) == '=' )
				{
					c.token.Length++;
					c.token.Flags |= TF_Assign;
					// token.Flags |= TokFlags::Assignment;
					// token.Type = Tok_Assign_L_Or;

					if (c.left)
						move_forward();
				}
				else while ( c.left && (* ctx->scanner) == *(c.scanner - 1) && c.token.Length < 3 )
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

					if ( (* ctx->scanner) == '>'  )
					{
						c.token.Length++;
//						token.Type = Tok_Access_PointerToMemberSymbol;
						c.token.Flags |= TF_AccessOperator;
						move_forward();

						if ( (* ctx->scanner) == '*' )
						{
//							token.Type = Tok_Access_PointerToMemberOfPointerSymbol;
							c.token.Length++;
							move_forward();
						}
					}
					else if ( (* ctx->scanner) == '=' )
					{
						c.token.Length++;
						// token.Type = Tok_Assign_Subtract;
						c.token.Flags |= TF_Assign;

						if (c.left)
							move_forward();
					}
					else while ( c.left && (* ctx->scanner) == *(c.scanner - 1) && c.token.Length < 3 )
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
					if ( (* ctx->scanner) == '=' )
					{
						// token.Type = TokeType::Assign_Divide;
						move_forward();
						c.token.Length++;
						c.token.Flags = TF_Assign;
					}
					else if ( (* ctx->scanner) == '/' )
					{
						c.token.Type   = Tok_Comment;
						c.token.Length = 2;
						c.token.Flags  = TF_Null;
						move_forward();

						while ( c.left && (* ctx->scanner) != '\n' && (* ctx->scanner) != '\r' )
						{
							move_forward();
							c.token.Length++;
						}

						if ( (* ctx->scanner) == '\r' )
						{
							move_forward();
							c.token.Length++;
						}
						if ( (* ctx->scanner) == '\n' )
						{
							move_forward();
							c.token.Length++;
						}
						array_append( Lexer_Tokens, c.token );
						continue;
					}
					else if ( (* ctx->scanner) == '*' )
					{
						c.token.Type   = Tok_Comment;
						c.token.Length = 2;
						c.token.Flags  = TF_Null;
						move_forward();

						bool star   = (* ctx->scanner)    == '*';
						bool slash  = c.scanner[1] == '/';
						bool at_end = star && slash;
						while ( c.left && ! at_end  )
						{
							move_forward();
							c.token.Length++;

							star   = (* ctx->scanner)    == '*';
							slash  = c.scanner[1] == '/';
							at_end = star && slash;
						}
						c.token.Length += 2;
						move_forward();
						move_forward();

						if ( (* ctx->scanner) == '\r' )
						{
							move_forward();
							c.token.Length++;
						}
						if ( (* ctx->scanner) == '\n' )
						{
							move_forward();
							c.token.Length++;
						}
						array_append( Lexer_Tokens, c.token );
						// end_line();
						continue;
					}
				}
				goto FoundToken;
			}
		}

		if ( char_is_alpha( (* ctx->scanner) ) || (* ctx->scanner) == '_' )
		{
			c.token.Text   = c.scanner;
			c.token.Length = 1;
			move_forward();

			while ( c.left && ( char_is_alphanumeric((* ctx->scanner)) || (* ctx->scanner) == '_' ) )
			{
				move_forward();
				c.token.Length++;
			}

			goto FoundToken;
		}
		else if ( char_is_digit((* ctx->scanner)) )
		{
			// This is a very brute force lex, no checks are done for validity of literal.

			c.token.Text   = c.scanner;
			c.token.Length = 1;
			c.token.Type   = Tok_Number;
			c.token.Flags  = TF_Literal;
			move_forward();

			if (c.left
			&& (	(* ctx->scanner) == 'x' || (* ctx->scanner) == 'X'
				||	(* ctx->scanner) == 'b' || (* ctx->scanner) == 'B'
				||  (* ctx->scanner) == 'o' || (* ctx->scanner) == 'O' )
			)
			{
				move_forward();
				c.token.Length++;

				while ( c.left && char_is_hex_digit((* ctx->scanner)) )
				{
					move_forward();
					c.token.Length++;
				}

				goto FoundToken;
			}

			while ( c.left && char_is_digit((* ctx->scanner)) )
			{
				move_forward();
				c.token.Length++;
			}

			if ( c.left && (* ctx->scanner) == '.' )
			{
				move_forward();
				c.token.Length++;

				while ( c.left && char_is_digit((* ctx->scanner)) )
				{
					move_forward();
					c.token.Length++;
				}

				// Handle number literal suffixes in a botched way
				if (c.left && (
					(* ctx->scanner) == 'l' || (* ctx->scanner) == 'L' ||  // long/long long
					(* ctx->scanner) == 'u' || (* ctx->scanner) == 'U' ||  // unsigned
					(* ctx->scanner) == 'f' || (* ctx->scanner) == 'F' ||  // float
					(* ctx->scanner) == 'i' || (* ctx->scanner) == 'I' ||  // imaginary
					(* ctx->scanner) == 'z' || (* ctx->scanner) == 'Z'))   // complex
				{
					char prev = (* ctx->scanner);
					move_forward();
					c.token.Length++;

					// Handle 'll'/'LL' as a special case when we just processed an 'l'/'L'
					if (c.left && (prev == 'l' || prev == 'L') && ((* ctx->scanner) == 'l' || (* ctx->scanner) == 'L'))
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
			s32 start = max( 0, array_num(Lexer_Tokens) - 100 );
			log_fmt("\n%d\n", start);
			for ( s32 idx = start; idx < array_num(Lexer_Tokens); idx++ )
			{
				log_fmt( "Token %d Type: %s : %.*s\n"
					, idx
					, toktype_to_str( Lexer_Tokens[ idx ].Type ).Ptr
					, Lexer_Tokens[ idx ].Length, Lexer_Tokens[ idx ].Text
				);
			}

			StrBuilder context_str = strbuilder_fmt_buf( GlobalAllocator, "%.*s", min( 100, c.left ), c.scanner );
			log_failure( "Failed to lex token '%c' (%d, %d)\n%s", (* ctx->scanner), c.line, c.column, context_str );

			// Skip to next whitespace since we can't know if anything else is valid until then.
			while ( c.left && ! char_is_space( (* ctx->scanner) ) )
			{
				move_forward();
			}
		}

		FoundToken:
		{
			lex_found_token( ctx );
			TokType last_type = array_back(Lexer_Tokens)->Type;
			if ( last_type == Tok_Preprocess_Macro )
			{
				Token thanks_c = { c.scanner, 0, Tok_Invalid, c.line, c.column, TF_Null };
				c.token = thanks_c;
				if ( (* ctx->scanner) == '\r')
				{
					move_forward();
					c.token.Length = 1;
				}

				if ( (* ctx->scanner) == '\n' )
				{
					c.token.Type = Tok_NewLine;
					c.token.Length++;
					move_forward();

					array_append( Lexer_Tokens, c.token );
					continue;
				}
			}
		}
	}

	if ( array_num(Lexer_Tokens) == 0 )
	{
		log_failure( "Failed to lex any tokens" );
		{
			TokArray tok_array =  {};
			return tok_array;
		}
	}

	hashtable_clear(Lexer_defines);
	// defines_map_arena.free();
	TokArray result = { Lexer_Tokens, 0 };
	return result;
}
#undef move_forward
#undef skip_whitespace
#undef end_line

GEN_NS_PARSER_END
