#ifdef INTELLISENSE_DIRECTIVES
#pragma once
#include "interface.upfront.cpp"
#include "gen/etoktype.hpp"
#endif

StrBuilder tok_to_strbuilder(Token tok)
{
	StrBuilder result   = strbuilder_make_reserve( _ctx->Allocator_Temp, kilobytes(4) );
	Str        type_str = toktype_to_str( tok.Type );

	strbuilder_append_fmt( & result, "Line: %d Column: %d, Type: %.*s Content: %.*s"
		, tok.Line, tok.Column
		, type_str.Len, type_str.Ptr
		, tok.Text.Len, tok.Text.Ptr
	);
	return result;
}

bool lex__eat(Context* ctx, TokArray* self, TokType type );

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

enum
{
	Lex_Continue,
	Lex_ReturnNull,
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

// TODO(Ed): We need to to attempt to recover from a lex failure?
s32 lex_preprocessor_define( LexContext* ctx )
{
	Token name = { { ctx->scanner, 1 }, Tok_Identifier, ctx->line, ctx->column, TF_Preprocess };
	move_forward();

	while ( ctx->left && ( char_is_alphanumeric((* ctx->scanner)) || (* ctx->scanner) == '_' ) ) {
		move_forward();
		name.Text.Len++;
	}

	Specifier spec    = str_to_specifier( name.Text );
	TokType   attrib  = str_to_toktype( name.Text );
	b32 not_specifier = spec   == Spec_Invalid;
	b32 not_attribute = attrib <= Tok___Attributes_Start;

	Macro  macro            = { name.Text, MT_Expression, (MacroFlags)0 };
	Macro* registered_macro = lookup_macro(name.Text);

	if ( registered_macro == nullptr && not_specifier && not_attribute ) {
		log_fmt("Warning: '%S' was not registered before the lexer processed its #define directive, it will be registered as a expression macro\n"
			, name.Text 
		);
		// GEN_DEBUG_TRAP();
	}
	array_append( _ctx->Lexer_Tokens, name );

	if ( ctx->left && (* ctx->scanner) == '(' )
	{
		if (registered_macro && ! macro_is_functional(* registered_macro)) {
			log_fmt("Warning: %S registered macro is not flagged as functional yet the definition detects opening parenthesis '(' for arguments\n"
				, name.Text
			);
			// GEN_DEBUG_TRAP();
		}
		else {
			macro.Flags |= MF_Functional;
		}

		Token opening_paren = { { ctx->scanner, 1 }, Tok_Paren_Open, ctx->line, ctx->column, TF_Preprocess };
		array_append( _ctx->Lexer_Tokens, opening_paren );
		move_forward();

		Token last_parameter = {};
		// We need to tokenize the define's arguments now:
		while( ctx->left && * ctx->scanner != ')')
		{
			skip_whitespace();
			
			Str possible_varadic = { ctx->scanner, 3 };
			if ( ctx->left > 3 && str_are_equal( txt("..."), possible_varadic ) ) {
				Token parameter = { { ctx->scanner, 3 }, Tok_Preprocess_Define_Param, ctx->line, ctx->column, TF_Preprocess };
				move_forward();
				move_forward();
				move_forward();

				array_append(_ctx->Lexer_Tokens, parameter);
				skip_whitespace();
				last_parameter = parameter;

				while ( (* ctx->scanner) == '\\' ) {
					move_forward();
					skip_whitespace();
				}
				if (* ctx->scanner != ')' )
				{
					log_failure("lex_preprocessor_define(%d, %d): Expected a ')' after '...' (varaidc macro param) %S\n"
						, ctx->line
						, ctx->column
						, name.Text
					);
					return Lex_ReturnNull;
				}
				break;
			}
			else if ( (* ctx->scanner) == '\\' ) {
				move_forward();
				skip_whitespace();
				continue;
			}
			else if ( char_is_alpha( (* ctx->scanner) ) || (* ctx->scanner) == '_' )
			{
				Token parameter = { { ctx->scanner, 1 }, Tok_Preprocess_Define_Param, ctx->line, ctx->column, TF_Preprocess };
				move_forward();

				while ( ctx->left && ( char_is_alphanumeric((* ctx->scanner)) || (* ctx->scanner) == '_' ) )
				{
					move_forward();
					parameter.Text.Len++;
				}
				array_append(_ctx->Lexer_Tokens, parameter);
				skip_whitespace();
				last_parameter = parameter;
			}
			else {
				log_failure("lex_preprocessor_define(%d, %d): Expected a '_' or alpha character for a parameter name for %S\n"
					, ctx->line
					, ctx->column
					, name.Text
				);
				return Lex_ReturnNull;
			}

			if (* ctx->scanner == ')' )
				break;

			// There should be a comma
			if ( * ctx->scanner != ',' ) {
				log_failure("lex_preprocessor_define(%d, %d): Expected a comma after parameter %S for %S\n"
					, ctx->line
					, ctx->column
					, last_parameter.Text
					, name.Text
				);
				return Lex_ReturnNull;
			}
			Token comma = { { ctx->scanner, 1 }, Tok_Comma, ctx->line, ctx->column, TF_Preprocess };
			array_append(_ctx->Lexer_Tokens, comma);
			move_forward();
		}
		
		if ( * ctx->scanner != ')' ) {
			log_failure("lex_preprocessor_define(%d, %d): Expected a ')' after last_parameter %S for %S (ran out of characters...)\n"
				, ctx->line
				, ctx->column
				, last_parameter.Text
				, name.Text
			);
			return Lex_ReturnNull;
		}
		Token closing_paren = { { ctx->scanner, 1 }, Tok_Paren_Close, ctx->line, ctx->column, TF_Preprocess };
		array_append(_ctx->Lexer_Tokens, closing_paren);
		move_forward();
	}
	else if ( registered_macro && macro_is_functional( * registered_macro) ) {
		if (registered_macro && ! macro_is_functional(* registered_macro)) {
			log_fmt("Warning: %S registered macro is flagged as functional yet the definition detects no opening parenthesis '(' for arguments\n"
				, name.Text
			);
			GEN_DEBUG_TRAP();
		}
	}

	if ( registered_macro == nullptr ) {
		register_macro(macro);
	}

	// Define's content handled by lex_preprocessor_directive (the original caller of this)
	return Lex_Continue;
}

// TODO(Ed): We need to to attempt to recover from a lex failure?
s32 lex_preprocessor_directive( LexContext* ctx )
{
	char const* hash = ctx->scanner;
	Token hash_tok = { { hash, 1 }, Tok_Preprocess_Hash, ctx->line, ctx->column, TF_Preprocess };
	array_append( _ctx->Lexer_Tokens, hash_tok  );

	move_forward();
	skip_whitespace();

	ctx->token.Text.Ptr = ctx->scanner;
	while (ctx->left && ! char_is_space((* ctx->scanner)) )
	{
		move_forward();
		ctx->token.Text.Len++;
	}

	ctx->token.Type = str_to_toktype( ctx->token.Text );

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
				ctx->token.Text.Len++;

				if ( (* ctx->scanner) == '\r' )
				{
					move_forward();
					ctx->token.Text.Len++;
				}

				if ( (* ctx->scanner) == '\n' )
				{
					move_forward();
					ctx->token.Text.Len++;
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
				ctx->token.Text.Len++;
			}

			if ( (* ctx->scanner) == '\n' )
			{
				move_forward();
				ctx->token.Text.Len++;
				break;
			}

			move_forward();
			ctx->token.Text.Len++;
		}

		ctx->token.Text.Len = ctx->token.Text.Len + ctx->token.Text.Ptr - hash;
		ctx->token.Text.Ptr = hash;
		array_append( _ctx->Lexer_Tokens, ctx->token );
		return Lex_Continue; // Skip found token, its all handled here.
	}

	if ( ctx->token.Type == Tok_Preprocess_Else || ctx->token.Type == Tok_Preprocess_EndIf )
	{
		ctx->token.Flags |= TF_Preprocess_Cond;
		array_append( _ctx->Lexer_Tokens, ctx->token );
		end_line();
		return Lex_Continue;
	}
	else if ( ctx->token.Type >= Tok_Preprocess_If && ctx->token.Type <= Tok_Preprocess_ElIf  )
	{
		ctx->token.Flags |= TF_Preprocess_Cond;
	}

	array_append( _ctx->Lexer_Tokens, ctx->token );

	skip_whitespace();

	if ( ctx->token.Type == Tok_Preprocess_Define )
	{
		u32 result = lex_preprocessor_define(ctx); // handles: #define <name>( <params> ) - define's content handled later on within this scope.
		if (result != Lex_Continue)
			return Lex_ReturnNull;
	}

	Token preprocess_content = { { ctx->scanner, 0 }, Tok_Preprocess_Content, ctx->line, ctx->column, TF_Preprocess };

	if ( ctx->token.Type == Tok_Preprocess_Include )
	{
		preprocess_content.Type = Tok_String;

		if ( (* ctx->scanner) != '"' && (* ctx->scanner) != '<' )
		{
			StrBuilder directive_str = strbuilder_fmt_buf( _ctx->Allocator_Temp, "%.*s", min( 80, ctx->left + preprocess_content.Text.Len ), ctx->token.Text.Ptr );

			log_failure( "gen::Parser::lex: Expected '\"' or '<' after #include, not '%c' (%d, %d)\n%s"
				, (* ctx->scanner)
				, preprocess_content.Line
				, preprocess_content.Column
				, (char*) directive_str
			);
			return Lex_ReturnNull;
		}
		move_forward();
		preprocess_content.Text.Len++;

		while ( ctx->left && (* ctx->scanner) != '"' && (* ctx->scanner) != '>' )
		{
			move_forward();
			preprocess_content.Text.Len++;
		}

		move_forward();
		preprocess_content.Text.Len++;

		if ( (* ctx->scanner) == '\r' && ctx->scanner[1] == '\n' )
		{
			move_forward();
			move_forward();
		}
		else if ( (* ctx->scanner) == '\n' )
		{
			move_forward();
		}

		array_append( _ctx->Lexer_Tokens, preprocess_content );
		return Lex_Continue; // Skip found token, its all handled here.
	}

	s32 within_string = false;
	s32 within_char   = false;

	// Consume preprocess content
	while ( ctx->left )
	{
		if ( (* ctx->scanner) == '"' && ! within_char )
			within_string ^= true;

		if ( (* ctx->scanner) == '\'' && ! within_string )
			within_char ^= true;

		if ( (* ctx->scanner) == '\\' && ! within_string && ! within_char )
		{
			move_forward();
			preprocess_content.Text.Len++;

			if ( (* ctx->scanner) == '\r' )
			{
				move_forward();
				preprocess_content.Text.Len++;
			}

			if ( (* ctx->scanner) == '\n' )
			{
				move_forward();
				preprocess_content.Text.Len++;
				continue;
			}
			else
			{
				StrBuilder directive_str = strbuilder_make_length( _ctx->Allocator_Temp, ctx->token.Text.Ptr, ctx->token.Text.Len );
				StrBuilder content_str   = strbuilder_fmt_buf( _ctx->Allocator_Temp, "%.*s", min( 400, ctx->left + preprocess_content.Text.Len ), preprocess_content.Text.Ptr );

				log_failure( "gen::Parser::lex: Invalid escape sequence '\\%c' (%d, %d)"
							" in preprocessor directive '%s' (%d, %d)\n%s"
					, (* ctx->scanner), ctx->line, ctx->column
					, directive_str, preprocess_content.Line, preprocess_content.Column
					, content_str );
				return Lex_ReturnNull;
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
		preprocess_content.Text.Len++;
	}

	array_append( _ctx->Lexer_Tokens, preprocess_content );
	return Lex_Continue; // Skip found token, its all handled here.
}

void lex_found_token( LexContext* ctx )
{
	if ( ctx->token.Type != Tok_Invalid ) {
		array_append( _ctx->Lexer_Tokens, ctx->token );
		return;
	}

	TokType type = str_to_toktype( ctx->token.Text );

	if (type == Tok_Preprocess_Define || type == Tok_Preprocess_Include) {
		ctx->token.Flags |= TF_Identifier;
	}

	if (type <= Tok_Access_Public && type >= Tok_Access_Private ) {
		ctx->token.Flags |= TF_AccessSpecifier;
	}
	if ( type > Tok___Attributes_Start ) {
		ctx->token.Flags |= TF_Attribute;
	}
	if ( type == Tok_Decl_Extern_Linkage )
	{
		skip_whitespace();

		if ( (* ctx->scanner) != '"' ) {
			type         = Tok_Spec_Extern;
			ctx->token.Flags |= TF_Specifier;
		}

		ctx->token.Type = type;
		array_append( _ctx->Lexer_Tokens, ctx->token );
		return;
	}
	if ( ( type <= Tok_Star && type >= Tok_Spec_Alignas)
			|| type == Tok_Ampersand
			|| type == Tok_Ampersand_DBL )
	{
		ctx->token.Type   = type;
		ctx->token.Flags |= TF_Specifier;
		array_append( _ctx->Lexer_Tokens, ctx->token );
		return;
	}
	if ( type != Tok_Invalid )
	{
		ctx->token.Type = type;
		array_append( _ctx->Lexer_Tokens, ctx->token );
		return;
	}

	Macro* macro = lookup_macro( ctx->token.Text );
	b32 has_args          = ctx->left && (* ctx->scanner) == '(';
	b32 resolved_to_macro = false;
	if (macro) {
		ctx->token.Type   = macrotype_to_toktype(macro->Type);
		b32 is_functional = macro_is_functional(* macro);
		resolved_to_macro = has_args ? is_functional : ! is_functional;
		if ( ! resolved_to_macro && GEN_BUILD_DEBUG ) {
			log_fmt("Info(%d, %d): %S identified as a macro but usage here does not resolve to one (interpreting as identifier)\n"
				, ctx->token.Line
				, ctx->token.Line
				, macro->Name
			);
		}
	}
	if ( resolved_to_macro )
	{
		// TODO(Ed): When we introduce a macro AST (and expression support), we'll properly lex this section.
		// Want to ignore any arguments the define may have as they can be execution expressions.
		if ( has_args ) {
			ctx->token.Flags |= TF_Macro_Functional;
		}
		if ( bitfield_is_set(MacroFlags, macro->Flags, MF_Allow_As_Attribute) ) {
			ctx->token.Flags |= TF_Attribute;
		}
		if ( bitfield_is_set(MacroFlags, macro->Flags, MF_Allow_As_Specifier ) ) {
			ctx->token.Flags |= TF_Specifier;
		}
	}
	else
	{
		ctx->token.Type = Tok_Identifier;
	}

	array_append( _ctx->Lexer_Tokens, ctx->token );
}

// TODO(Ed): We should dynamically allocate the lexer's array in Allocator_DyanmicContainers.

// TODO(Ed): We need to to attempt to recover from a lex failure?

neverinline
// void lex( Array<Token> tokens, Str content )
TokArray lex( Str content )
{
	LexContext c; LexContext* ctx = & c;
	c.content = content;
	c.left    = content.Len;
	c.scanner = content.Ptr;

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

	array_clear(_ctx->Lexer_Tokens);

	b32 preprocess_args = true;

	while (c.left )
	{
		#if 0
		if (Tokens.num())
		{
			log_fmt("\nLastTok: %SB", Tokens.back().to_strbuilder());
		}
		#endif

		{
			Token thanks_c = { { c.scanner, 0 }, Tok_Invalid, c.line, c.column, TF_Null };
			c.token = thanks_c;
		}

		bool is_define = false;

		if ( c.column == 1 )
		{
			if ( (* ctx->scanner) == '\r')
			{
				move_forward();
				c.token.Text.Len = 1;
			}

			if ( (* ctx->scanner) == '\n' )
			{
				move_forward();

				c.token.Type = Tok_NewLine;
				c.token.Text.Len++;

				array_append( _ctx->Lexer_Tokens, c.token );
				continue;
			}
		}

		c.token.Text.Len = 0;

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
								Token thanks_c = { { c.scanner, 0 }, Tok_Invalid, c.line, c.column, TF_Null };
								c.token = thanks_c;
							}
							if ( (* ctx->scanner) == '\r')
							{
								move_forward();
								c.token.Text.Len = 1;
							}

							if ( (* ctx->scanner) == '\n' )
							{
								c.token.Type = Tok_NewLine;
								c.token.Text.Len++;
								move_forward();

								array_append( _ctx->Lexer_Tokens, c.token );
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
				Str text = { c.scanner, 1 };
				c.token.Text   = text;
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
						c.token.Text.Len = 3;
						c.token.Type     = Tok_Varadic_Argument;
						c.token.Flags    = TF_Null;
						move_forward();
					}
					else
					{
						StrBuilder context_str = strbuilder_fmt_buf( _ctx->Allocator_Temp, "%s", c.scanner, min( 100, c.left ) );

						log_failure( "gen::lex: invalid varadic argument, expected '...' got '..%c' (%d, %d)\n%s", (* ctx->scanner), c.line, c.column, context_str );
					}
				}

				goto FoundToken;
			}
			case '&' :
			{
				Str text = { c.scanner, 1 };
				c.token.Text   = text;
				c.token.Type   = Tok_Ampersand;
				c.token.Flags |= TF_Operator;
				c.token.Flags |= TF_Specifier;

				if (c.left)
					move_forward();

				if ( (* ctx->scanner) == '&' )	// &&
				{
					c.token.Text.Len = 2;
					c.token.Type     = Tok_Ampersand_DBL;

					if (c.left)
						move_forward();
				}

				goto FoundToken;
			}
			case ':':
			{
				Str text = { c.scanner, 1 };
				c.token.Text = text;
				c.token.Type = Tok_Assign_Classifer;
				// Can be either a classifier (ParentType, Bitfield width), or ternary else
				// token.Type   = Tok_Colon;

				if (c.left)
					move_forward();

				if ( (* ctx->scanner) == ':' )
				{
					move_forward();
					c.token.Type = Tok_Access_StaticSymbol;
					c.token.Text.Len++;
				}
				goto FoundToken;
			}
			case '{':
			{
				Str text = { c.scanner, 1 };
				c.token.Text   = text;
				c.token.Type   = Tok_BraceCurly_Open;

				if (c.left)
					move_forward();
				goto FoundToken;
			}
			case '}':
			{
				Str text = { c.scanner, 1 };
				c.token.Text   = text;
				c.token.Type   = Tok_BraceCurly_Close;
				c.token.Flags  = TF_EndDefinition;

				if (c.left)
					move_forward();

				end_line();
				goto FoundToken;
			}
			case '[':
			{
				Str text = { c.scanner, 1 };
				c.token.Text   = text;
				c.token.Type   = Tok_BraceSquare_Open;
				if ( c.left )
				{
					move_forward();

					if ( (* ctx->scanner) == ']' )
					{
						c.token.Text.Len = 2;
						c.token.Type     = Tok_Operator;
						move_forward();
					}
				}
				goto FoundToken;
			}
			case ']':
			{
				Str text = { c.scanner, 1 };
				c.token.Text   = text;
				c.token.Type   = Tok_BraceSquare_Close;

				if (c.left)
					move_forward();
				goto FoundToken;
			}
			case '(':
			{
				Str text = { c.scanner, 1 };
				c.token.Text   = text;
				c.token.Type   = Tok_Paren_Open;

				if (c.left)
					move_forward();
				goto FoundToken;
			}
			case ')':
			{
				Str text = { c.scanner, 1 };
				c.token.Text   = text;
				c.token.Type   = Tok_Paren_Close;

				if (c.left)
					move_forward();
				goto FoundToken;
			}
			case '\'':
			{
				Str text = { c.scanner, 1 };
				c.token.Text   = text;
				c.token.Type   = Tok_Char;
				c.token.Flags  = TF_Literal;

				move_forward();

				if ( c.left && (* ctx->scanner) == '\\' )
				{
					move_forward();
					c.token.Text.Len++;

					if ( (* ctx->scanner) == '\'' )
					{
						move_forward();
						c.token.Text.Len++;
					}
				}

				while ( c.left && (* ctx->scanner) != '\'' )
				{
					move_forward();
					c.token.Text.Len++;
				}

				if ( c.left )
				{
					move_forward();
					c.token.Text.Len++;
				}
				goto FoundToken;
			}
			case ',':
			{
				Str text = { c.scanner, 1 };
				c.token.Text   = text;
				c.token.Type   = Tok_Comma;
				c.token.Flags  = TF_Operator;

				if (c.left)
					move_forward();
				goto FoundToken;
			}
			case '*':
			{
				Str text = { c.scanner, 1 };
				c.token.Text   = text;
				c.token.Type   = Tok_Star;
				c.token.Flags |= TF_Specifier;
				c.token.Flags |= TF_Operator;

				if (c.left)
					move_forward();

				if ( (* ctx->scanner) == '=' )
				{
					c.token.Text.Len++;
					c.token.Flags |= TF_Assign;
					// c.token.Type = Tok_Assign_Multiply;

					if ( c.left )
						move_forward();
				}

				goto FoundToken;
			}
			case ';':
			{
				Str text = { c.scanner, 1 };
				c.token.Text   = text;
				c.token.Type   = Tok_Statement_End;
				c.token.Flags  = TF_EndDefinition;

				if (c.left)
					move_forward();

				end_line();
				goto FoundToken;
			}
			case '"':
			{
				Str text = { c.scanner, 1 };
				c.token.Text   = text;
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
						c.token.Text.Len++;

						if ( c.left )
						{
							move_forward();
							c.token.Text.Len++;
						}
						continue;
					}

					move_forward();
					c.token.Text.Len++;
				}
				goto FoundToken;
			}
			case '?':
			{
				Str text = { c.scanner, 1 };
				c.token.Text     = text;
				c.token.Type     = Tok_Operator;
				// c.token.Type     = Tok_Ternary;
				c.token.Flags    = TF_Operator;

				if (c.left)
					move_forward();

				goto FoundToken;
			}
			case '=':
			{
				Str text = { c.scanner, 1 };
				c.token.Text     = text;
				c.token.Type     = Tok_Operator;
				// c.token.Type     = Tok_Assign;
				c.token.Flags    = TF_Operator;
				c.token.Flags   |= TF_Assign;

				if (c.left)
					move_forward();

				if ( (* ctx->scanner) == '=' )
				{
					c.token.Text.Len++;
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
				Str text = { c.scanner, 1 };
				c.token.Text   = text;
				c.token.Type   = Tok_Operator;
				c.token.Flags  = TF_Operator;
				// token.Type   = Tok_L_Or;

				if (c.left)
					move_forward();

				if ( (* ctx->scanner) == '=' )
				{
					c.token.Text.Len++;
					c.token.Flags |= TF_Assign;
					// token.Flags |= TokFlags::Assignment;
					// token.Type = Tok_Assign_L_Or;

					if (c.left)
						move_forward();
				}
				else while ( c.left && (* ctx->scanner) == *(c.scanner - 1) && c.token.Text.Len < 3 )
				{
					c.token.Text.Len++;

					if (c.left)
						move_forward();
				}
				goto FoundToken;
			}

			// Dash is unfortunately a bit more complicated...
			case '-':
			{
				Str text = { c.scanner, 1 };
				c.token.Text   = text;
				c.token.Type   = Tok_Operator;
				// token.Type = Tok_Subtract;
				c.token.Flags  = TF_Operator;
				if ( c.left )
				{
					move_forward();

					if ( (* ctx->scanner) == '>'  )
					{
						c.token.Text.Len++;
//						token.Type = Tok_Access_PointerToMemberSymbol;
						c.token.Flags |= TF_AccessOperator;
						move_forward();

						if ( (* ctx->scanner) == '*' )
						{
//							token.Type = Tok_Access_PointerToMemberOfPointerSymbol;
							c.token.Text.Len++;
							move_forward();
						}
					}
					else if ( (* ctx->scanner) == '=' )
					{
						c.token.Text.Len++;
						// token.Type = Tok_Assign_Subtract;
						c.token.Flags |= TF_Assign;

						if (c.left)
							move_forward();
					}
					else while ( c.left && (* ctx->scanner) == *(c.scanner - 1) && c.token.Text.Len < 3 )
					{
						c.token.Text.Len++;

						if (c.left)
							move_forward();
					}
				}
				goto FoundToken;
			}
			case '/':
			{
				Str text = { c.scanner, 1 };
				c.token.Text   = text;
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
						c.token.Text.Len++;
						c.token.Flags = TF_Assign;
					}
					else if ( (* ctx->scanner) == '/' )
					{
						c.token.Type     = Tok_Comment;
						c.token.Text.Len = 2;
						c.token.Flags    = TF_Null;
						move_forward();

						while ( c.left && (* ctx->scanner) != '\n' && (* ctx->scanner) != '\r' )
						{
							move_forward();
							c.token.Text.Len++;
						}

						if ( (* ctx->scanner) == '\r' )
						{
							move_forward();
							c.token.Text.Len++;
						}
						if ( (* ctx->scanner) == '\n' )
						{
							move_forward();
							c.token.Text.Len++;
						}
						array_append( _ctx->Lexer_Tokens, c.token );
						continue;
					}
					else if ( (* ctx->scanner) == '*' )
					{
						c.token.Type     = Tok_Comment;
						c.token.Text.Len = 2;
						c.token.Flags    = TF_Null;
						move_forward();

						bool star   = (* ctx->scanner)    == '*';
						bool slash  = c.scanner[1] == '/';
						bool at_end = star && slash;
						while ( c.left && ! at_end  )
						{
							move_forward();
							c.token.Text.Len++;

							star   = (* ctx->scanner)    == '*';
							slash  = c.scanner[1] == '/';
							at_end = star && slash;
						}
						c.token.Text.Len += 2;
						move_forward();
						move_forward();

						if ( (* ctx->scanner) == '\r' )
						{
							move_forward();
							c.token.Text.Len++;
						}
						if ( (* ctx->scanner) == '\n' )
						{
							move_forward();
							c.token.Text.Len++;
						}
						array_append( _ctx->Lexer_Tokens, c.token );
						// end_line();
						continue;
					}
				}
				goto FoundToken;
			}
		}

		if ( char_is_alpha( (* ctx->scanner) ) || (* ctx->scanner) == '_' )
		{
			Str text = { c.scanner, 1 };
			c.token.Text = text;
			move_forward();

			while ( c.left && ( char_is_alphanumeric((* ctx->scanner)) || (* ctx->scanner) == '_' ) ) {
				move_forward();
				c.token.Text.Len++;
			}

			goto FoundToken;
		}
		else if ( char_is_digit((* ctx->scanner)) )
		{
			// This is a very brute force lex, no checks are done for validity of literal.

			Str text = { c.scanner, 1 };
			c.token.Text   = text;
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
				c.token.Text.Len++;

				while ( c.left && char_is_hex_digit((* ctx->scanner)) ) {
					move_forward();
					c.token.Text.Len++;
				}

				goto FoundToken;
			}

			while ( c.left && char_is_digit((* ctx->scanner)) ) {
				move_forward();
				c.token.Text.Len++;
			}

			if ( c.left && (* ctx->scanner) == '.' )
			{
				move_forward();
				c.token.Text.Len++;

				while ( c.left && char_is_digit((* ctx->scanner)) ) {
					move_forward();
					c.token.Text.Len++;
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
					c.token.Text.Len++;

					// Handle 'll'/'LL' as a special case when we just processed an 'l'/'L'
					if (c.left && (prev == 'l' || prev == 'L') && ((* ctx->scanner) == 'l' || (* ctx->scanner) == 'L')) {
						move_forward();
						c.token.Text.Len++;
					}
				}
			}

			goto FoundToken;
		}
		else
		{
			s32 start = max( 0, array_num(_ctx->Lexer_Tokens) - 100 );
			log_fmt("\n%d\n", start);
			for ( s32 idx = start; idx < array_num(_ctx->Lexer_Tokens); idx++ )
			{
				log_fmt( "Token %d Type: %s : %.*s\n"
					, idx
					, toktype_to_str( _ctx->Lexer_Tokens[ idx ].Type ).Ptr
					, _ctx->Lexer_Tokens[ idx ].Text.Len, _ctx->Lexer_Tokens[ idx ].Text.Ptr
				);
			}

			StrBuilder context_str = strbuilder_fmt_buf( _ctx->Allocator_Temp, "%.*s", min( 100, c.left ), c.scanner );
			log_failure( "Failed to lex token '%c' (%d, %d)\n%s", (* ctx->scanner), c.line, c.column, context_str );

			// Skip to next whitespace since we can't know if anything else is valid until then.
			while ( c.left && ! char_is_space( (* ctx->scanner) ) ) {
				move_forward();
			}
		}

		FoundToken:
		{
			lex_found_token( ctx );
			TokType last_type = array_back(_ctx->Lexer_Tokens)->Type;
			if ( last_type == Tok_Preprocess_Macro_Stmt || last_type == Tok_Preprocess_Macro_Expr )
			{
				Token thanks_c = { { c.scanner, 0 }, Tok_Invalid, c.line, c.column, TF_Null };
				c.token = thanks_c;
				if ( (* ctx->scanner) == '\r') {
					move_forward();
					c.token.Text.Len = 1;
				}
				if ( (* ctx->scanner) == '\n' ) 
				{
					c.token.Type = Tok_NewLine;
					c.token.Text.Len++;
					move_forward();

					array_append( _ctx->Lexer_Tokens, c.token );
					continue;
				}
			}
		}
	}

	if ( array_num(_ctx->Lexer_Tokens) == 0 ) {
		log_failure( "Failed to lex any tokens" );
		TokArray tok_array =  {};
		return tok_array;
	}

	TokArray result = { _ctx->Lexer_Tokens, 0 };
	return result;
}

#undef move_forward
#undef skip_whitespace
#undef end_line
