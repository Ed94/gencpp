/*
These constructors are the most implementation intensive other than the editor or scanner.
*/

namespace Parser
{
	struct Token
	{
		char const* Text;
		sptr        Length;
		TokType     Type;
		bool 	    IsAssign;
		s32         Line;
		s32         Column;
		// TokFlags Flags;

		operator bool()
		{
			return Text && Length && Type != TokType::Invalid;
		}

		operator StrC()
		{
			return { Length, Text };
		}

		bool is_access_specifier()
		{
			return Type >= TokType::Access_Private && Type <= TokType::Access_Public;
		}

		bool is_attribute()
		{
			return Type > TokType::Attributes_Start;
		}

		bool is_preprocessor()
		{
			return Type >= TokType::Preprocess_Define && Type <= TokType::Preprocess_EndIf;
		}

		bool is_specifier()
		{
			return (Type <= TokType::Star && Type >= TokType::Spec_Alignas)
				|| Type == TokType::Ampersand
				|| Type == TokType::Ampersand_DBL
			;
		}

		AccessSpec to_access_specifier()
		{
			return scast(AccessSpec, Type);
		}
	};

	constexpr Token NullToken { nullptr, 0, TokType::Invalid, false, 0, 0 };

	struct TokArray
	{
		Array<Token> Arr;
		s32          Idx;

		bool __eat( TokType type );

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

	struct StackNode
	{
		StackNode* Prev;

		Token Name;        // The name of the AST node (if parsed)
		StrC  ProcName;    // The name of the procedure
	};

	struct ParseContext
	{
		TokArray   Tokens;
		StackNode* Scope;

		String to_string()
		{
			String result = String::make_reserve( GlobalAllocator, kilobytes(4) );

			result.append_fmt("\tContext:\n");

			char* current = Tokens.current().Text;
			sptr  length  = Tokens.current().Length;
			while ( current != Tokens.back().Text && current != '\n' )
			{
				current++;
				length--;
			}

			String line = String::make( GlobalAllocator, { length, Tokens.current().Text } );
			result.append_fmt("\t(%d, %d): %s", Tokens.current().Line, Tokens.current().Column, line );
			line.free();

			StackNode* current = Scope;
			do
			{
				if ( current->Name )
				{
					result.append_fmt("\tProcedure: %s, AST Name: %s\n", current->ProcName, (StrC)current->Name );
				}
				else
				{
					result.append_fmt("\tProcedure: %s\n", current->ProcName );
				}

				current = current->Prev;

				name.free();
			}
			while ( current );
			return result;
		}

		void push( StackNode* node )
		{
			node->Prev = Scope;
			Scope      = node;
		}

		void pop()
		{
			Context.Scope = Context.Scope->Prev;
		}
	};

	global ParseContext Context;

	bool TokArray::__eat( TokType type )
	{
		if ( Arr.num() - Idx <= 0 )
		{
			log_failure( "No tokens left.\n%s", Context.to_string() );
			return false;
		}

		if ( Arr[Idx].Type != type )
		{
			String token_str = String::make( GlobalAllocator, { Arr[Idx].Length, Arr[Idx].Text } );

			log_failure( "Parse Error, TokArray::eat, Expected: %s, not %s (%d, %d)`\n%s", ETokType::to_str(type), token_str, Context.to_string() );

			return false;
		}

		Idx++;
		return true;
	}

	enum TokFlags : u32
	{
		IsAssign = bit(0),
	};

	TokArray lex( StrC content, bool keep_preprocess_directives = true )
	{
	#	define current ( * scanner )

	#	define move_forward()       \
		{                           \
			if ( current == '\n' )  \
			{                       \
				line++;             \
				column = 0;         \
			}                       \
			else                    \
			{                       \
				column++;           \
			}                       \
			left--;                 \
			scanner++;              \
		}

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

		s32 line   = 0;
		s32 column = 0;

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
			Token token = { nullptr, 0, TokType::Invalid, false, line, column };

			SkipWhitespace();
			if ( left <= 0 )
				break;

			switch ( current )
			{
				// TODO : Need to handle the preprocessor as a separate pass.
				case '#':
					token.Text   = scanner;
					token.Length = 1;
					move_forward();

					while (left && current != '\n' )
					{
						if ( token.Type == ETokType::Invalid && current == ' ' )
						{
							token.Type = ETokType::to_type( token );
						}

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

					if (left) {
						move_forward();
					}

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
				if ( token.is_preprocessor() && keep_preprocess_directives == false )
					continue;

				Tokens.append( token );
				continue;
			}

			TokType type = ETokType::to_type( token );

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

#	define check_parse_args( def )                                               \
if ( def.Len <= 0 )                                                              \
{                                                                                \
	log_failure( "gen::" stringize( __func__ ) ": length must greater than 0" ); \
	return CodeInvalid;                                                          \
}                                                                                \
if ( def.Ptr == nullptr )                                                        \
{                                                                                \
	log_failure( "gen::" stringize( __func__ ) ": def was null" );               \
	return CodeInvalid;                                                          \
}

#	define nexttok 	    Context.Tokens.next()
#	define currtok      Context.Tokens.current()
#	define prevtok      Context.Tokens.previous()
#	define eat( Type_ ) Context.Tokens.__eat( Type_ )
#	define left         ( Context.Tokens.Arr.num() - Context.Tokens.Idx )

#	define check( Type_ ) ( left && currtok.Type == Type_ )

#	define push_scope()                                           \
	StackNode scope { nullptr, NullToken, txt_StrC( __func__ ) }; \
	Context.push( & scope )

#pragma endregion Helper Macros

internal Code parse_function_body();
internal Code parse_global_nspace();

internal CodeClass     parse_class           ();
internal CodeEnum      parse_enum            ();
internal CodeBody      parse_export_body     ();
internal CodeBody      parse_extern_link_body();
internal CodeExtern    parse_exten_link      ();
internal CodeFriend    parse_friend          ();
internal CodeFn        parse_function        ();
internal CodeNamespace parse_namespace       ();
internal CodeOpCast    parse_operator_cast   ();
internal CodeStruct    parse_struct          ();
internal CodeVar       parse_variable        ();
internal CodeTemplate  parse_template        ();
internal CodeType      parse_type            ();
internal CodeTypedef   parse_typedef         ();
internal CodeUnion     parse_union           ();
internal CodeUsing     parse_using           ();

internal inline
Code parse_array_decl()
{
	using namespace Parser;
	push_scope();

	if ( check( TokType::BraceSquare_Open ) )
	{
		eat( TokType::BraceSquare_Open );

		if ( left == 0 )
		{
			log_failure( "Error, unexpected end of array declaration ( '[]' scope started )\n%s", Context.to_string() );
			return Code::Invalid;
		}

		if ( currtok.Type == TokType::BraceSquare_Close )
		{
			log_failure( "Error, empty array expression in typedef definition\n%s", Context.to_string() );
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
			log_failure( "Error, unexpected end of array declaration, expected ]\n%s", Context.to_string() );
			return Code::Invalid;
		}

		if ( currtok.Type != TokType::BraceSquare_Close )
		{
			log_failure( "%s: Error, expected ] in array declaration, not %s\n%s", ETokType::to_str( currtok.Type ), Context.to_string() );
			return Code::Invalid;
		}

		eat( TokType::BraceSquare_Close );
		return array_expr;
	}

	Context.pop();
	return { nullptr };
}

internal inline
CodeAttributes parse_attributes()
{
	using namespace Parser;
	push_scope();

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

	else if ( currtok.is_attribute() )
	{
		eat(currtok.Type);
		s32 len = start.Length;
	}

	if ( len > 0 )
	{
		StrC attribute_txt = { len, start.Text };
		return def_attributes( attribute_txt );
	}

	pop_context();
	return { nullptr };
}

internal inline
Parser::Token parse_identifier()
{
	using namespace Parser;
	push_scope();

	Token name = currtok;

	eat( TokType::Identifier );

	while ( check( TokType::Access_StaticSymbol ) )
	{
		eat( TokType::Access_StaticSymbol );

		if ( left == 0 )
		{
			log_failure( "Error, unexpected end of static symbol identifier\n%s", Context.to_string() );
			return { nullptr, 0, TokType::Invalid };
		}

		if ( currtok.Type != TokType::Identifier )
		{
			log_failure( "Error, expected static symbol identifier, not %s\n%s", ETokType::to_str( currtok.Type ), Context.to_string() );
			return { nullptr, 0, TokType::Invalid };
		}

		name.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)name.Text;
		eat( TokType::Identifier );
	}

	if ( check( TokType::Operator ) && currtok.Text[0] == '<' )
	{
		// Template arguments can be complex so were not validating if they are correct.
		s32 level = 0;
		while ( left && (currtok.Text[0] != '>' || level > 0 ) )
		{
			if ( currtok.Text[0] == '<' )
				level++;

			else if ( currtok.Text[0] == '>' && level > 0 )
				level--;

			eat( currtok.Type );
		}

		if ( left == 0 )
		{
			log_failure( "Error, unexpected end of template arguments\n%s", Context.to_string() );
			return { nullptr, 0, TokType::Invalid };
		}

		eat( TokType::Operator );

		name.Length = ( (sptr)prevtok.Text + (sptr)prevtok.Length ) - (sptr)name.Text;
	}

	pop_context();
	return name;
}

internal
CodeParam parse_params( bool use_template_capture = false )
{
	using namespace Parser;
	using namespace ECode;
	push_scope();

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

	type = parse_type();
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
				log_failure( "Expected value after assignment operator\n%s.", Context.to_string() );
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
					log_failure( "Expected value after assignment operator\n%s", Context.to_string() );
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
			log_failure("Expected '<' after 'template' keyword\n%s", Context.to_string() );
			return CodeInvalid;
		}
		eat( TokType::Operator );
	}

	Context.pop();
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
)
{
	using namespace Parser;
	push_scope();

	CodeParam params = parse_params( toks, stringize(parse_function) );

	while ( left && currtok.is_specifier() )
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
				log_failure("Body must be either of Function_Body or Untyped type, %s\n%s", body.debug_str(), Context.to_string());
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

	Context.pop();
	return result;
}

internal inline
CodeOperator parse_operator_after_ret_type(
	  ModuleFlag     mflags
	, CodeAttributes attributes
	, CodeSpecifiers specifiers
	, CodeType       ret_type
)
{
	using namespace Parser;
	using namespace EOperator;
	push_scope();

	// Parse Operator
	eat( TokType::Decl_Operator );

	if ( ! check( TokType::Operator ) )
	{
		log_failure( "Expected operator after 'operator' keyword\n%s", Context.to_string() );
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
		log_failure( "Invalid operator '%s'\n%s", currtok.Text, Context.to_string() );
		return CodeInvalid;
	}

	eat( TokType::Operator );

	// Parse Params
	CodeParam params = parse_params( toks, stringize(parse_operator) );

	while ( left && currtok.is_specifier() )
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
	Context.pop();
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
)
{
	using namespace Parser;
	push_scope();

	Code array_expr = parse_array_decl();
	Code expr       = { nullptr };

	if ( currtok.IsAssign )
	{
		eat( TokType::Operator );

		Token expr_tok = currtok;

		if ( currtok.Type == TokType::Statement_End )
		{
			log_failure( "Expected expression after assignment operator\n%s", Context.to_string() );
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

	Context.pop();
	return result;
}

internal inline
Code parse_variable_assignment()
{
	using namespace Parser;
	push_scope();

	Code expr = Code::Invalid;

	if ( currtok.IsAssign )
	{
		eat( TokType::Operator );

		Token expr_tok = currtok;

		if ( currtok.Type == TokType::Statement_End )
		{
			log_failure( "Expected expression after assignment operator\n%s", Context.to_string() );
			return Code::Invalid;
		}

		while ( left && currtok.Type != TokType::Statement_End )
		{
			expr_tok.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)expr_tok.Text;
			eat( currtok.Type );
		}

		expr = untyped_str( expr_tok );
	}

	Context.pop();
	return expr;
}

internal inline
Code parse_operator_function_or_variable( bool expects_function, CodeAttributes attributes, CodeSpecifiers specifiers )
{
	using namespace Parser;
	push_scope();

	Code result = Code::Invalid;

	CodeType type = parse_type( toks, stringize(parse_variable) );

	if ( type == Code::Invalid )
		return CodeInvalid;

	if ( check( TokType::Operator) )
	{
		// Dealing with an operator overload
		result = parse_operator_after_ret_type( ModuleFlag::None, attributes, specifiers, type );
	}
	else
	{
		StrC name = currtok;
		eat( TokType::Identifier );

		if ( check( TokType::Capture_Start) )
		{
			// Dealing with a function

			result = parse_function_after_name( ModuleFlag::None, attributes, specifiers, type, name );
		}
		else
		{
			if ( expects_function )
			{
				log_failure( "Expected function declaration (consteval was used)\n%s", Context.to_string() );
				return Code::Invalid;
			}

			// Dealing with a variable
			result = parse_variable_after_name( ModuleFlag::None, attributes, specifiers, type, name );
		}
	}

	Context.pop();
	return result;
}

internal
CodeBody parse_class_struct_body( Parser::TokType which )
{
	using namespace Parser;
	using namespace ECode;
	push_scope();

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
			GEN_DEFINE_ATTRIBUTE_TOKENS
		#undef Entry
			{
				attributes = parse_attributes();
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
				SpecifierT specs_found[16] { ESpecifier::NumSpecifiers };
				s32        NumSpecifiers = 0;

				while ( left && currtok.is_specifier() )
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
							log_failure( "Invalid specifier %s for variable\n%s", ESpecifier::to_str(spec), Context.to_string() );
							return CodeInvalid;
					}

					specs_found[NumSpecifiers] = spec;
					NumSpecifiers++;
					eat( currtok.Type );
				}

				if ( NumSpecifiers )
				{
					specifiers = def_specifiers( NumSpecifiers, specs_found );
				}
			}
			//! Fallthrough intentional
			case TokType::Identifier:
			case TokType::Spec_Const:
			case TokType::Type_Unsigned:
			case TokType::Type_Signed:
			case TokType::Type_Short:
			case TokType::Type_Long:
			case TokType::Type_char:
			case TokType::Type_int:
			case TokType::Type_double:
			{
				member = parse_operator_function_or_variable( expects_function, attributes, specifiers );
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
			log_failure( "Failed to parse member\n%s", Context.to_string() );
			return CodeInvalid;
		}

		result.append( member );
	}

	eat( TokType::BraceCurly_Close );
	Context.pop();
	return result;
}

internal
Code parse_class_struct( Parser::TokType which )
{
	using namespace Parser;
	push_scope();

	if ( which != TokType::Decl_Class && which != TokType::Decl_Struct )
	{
		log_failure( "Error, expected class or struct, not %s\n%s", ETokType::to_str( which ), Context.to_string() );
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

	attributes = parse_attributes();

	if ( check( TokType::Identifier ) )
		name = parse_identifier( toks, context );

	local_persist
	char interface_arr_mem[ kilobytes(4) ] {0};
	Array<CodeType> interfaces = Array<CodeType>::init_reserve( Arena::init_from_memory(interface_arr_mem, kilobytes(4) ), 4 );

	if ( check( TokType::Assign_Classifer ) )
	{
		eat( TokType::Assign_Classifer );

		if ( currtok.is_access_specifier() )
		{
			access = currtok.to_access_specifier();
		}

		Token parent_tok = parse_identifier( toks, context );
		parent = def_type( parent_tok );

		while ( check(TokType::Comma) )
		{
			eat(TokType::Access_Public);

			if ( currtok.is_access_specifier() )
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
	Context.pop();
	return result;
}

internal
Code parse_function_body()
{
	using namespace Parser;
	using namespace ECode;
	push_scope();

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

	Context.pop();
	return result;
}

internal
CodeBody parse_global_nspace( CodeT which )
{
	using namespace Parser;
	using namespace ECode;
	push_scope();

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
					log_failure( "Nested extern linkage\n%s", Context.to_string() );

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
					log_failure( "Nested export declaration\n%s", Context.to_string() );

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
			GEN_DEFINE_ATTRIBUTE_TOKENS
		#undef Entry
			{
				attributes = parse_attributes();
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
				SpecifierT specs_found[16] { ESpecifier::NumSpecifiers };
				s32        NumSpecifiers = 0;

				while ( left && currtok.is_specifier() )
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
							log_failure( "Invalid specifier %s for variable\n%s", ESpecifier::to_str(spec), Context.to_string() );
							return CodeInvalid;
					}

					specs_found[NumSpecifiers] = spec;
					NumSpecifiers++;
					eat( currtok.Type );
				}

				if ( NumSpecifiers )
				{
					specifiers = def_specifiers( NumSpecifiers, specs_found );
				}
			}
			//! Fallthrough intentional
			case TokType::Identifier:
			case TokType::Spec_Const:
			case TokType::Type_Long:
			case TokType::Type_Short:
			case TokType::Type_Signed:
			case TokType::Type_Unsigned:
			case TokType::Type_char:
			case TokType::Type_double:
			case TokType::Type_int:
			{
				member = parse_operator_function_or_variable( expects_function, attributes, specifiers, toks, context );
			}
		}

		if ( member == Code::Invalid )
		{
			log_failure( "Failed to parse member\n%s", Context.to_string() );
			return CodeInvalid;
		}

		result.append( member );
	}

	if ( which != Global_Body )
		eat( TokType::BraceCurly_Close );

	Context.pop();
	return result;
}

internal
CodeClass parse_class()
{
	push_scope();
	CodeClass result = (CodeClass) parse_class_struct( Parser::TokType::Decl_Class, toks, context );
	Context.pop();
	return result;
}

CodeClass parse_class( StrC def )
{
	check_parse_args( parse_class, def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	push_scope();
	CodeClass result = (CodeClass) parse_class_struct( TokType::Decl_Class );
	Context.pop();
	return result;
}

internal
CodeEnum parse_enum()
{
	using namespace Parser;
	using namespace ECode;
	push_scope();

	SpecifierT specs_found[16] { ESpecifier::NumSpecifiers };
	s32        NumSpecifiers = 0;

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
		log_failure( "Expected identifier for enum name\n%s", Context.to_string() );
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

	Context.pop();
	return result;
}

CodeEnum parse_enum( StrC def )
{
	check_parse_args( parse_enum, def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	return parse_enum();
}

internal inline
CodeBody parse_export_body()
{
	push_scope();
	CodeBody result = parse_global_nspace( ECode::Export_Body );
	Context.pop();
	return result;
}

CodeBody parse_export_body( StrC def )
{
	check_parse_args( parse_export_body, def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	return parse_export_body();
}

internal inline
CodeBody parse_extern_link_body()
{
	push_scope();
	CodeBody result = parse_global_nspace( ECode::Extern_Linkage_Body );
	Context.pop();
	return result;
}

internal
CodeExtern parse_extern_link()
{
	using namespace Parser;
	push_scope();

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
		log_failure( "Failed to parse body\n%s", Context.to_string() );
		return result;
	}

	result->Body = entry;

	Context.pop();
	return result;
}

CodeExtern parse_extern_link( StrC def )
{
	check_parse_args( parse_extern_link, def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	return parse_extern_link();
}

internal
CodeFriend parse_friend()
{
	using namespace Parser;
	using namespace ECode;
	push_scope();

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

	Context.pop();
	return result;
}

CodeFriend parse_friend( StrC def )
{
	check_parse_args( parse_friend, def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	return parse_friend();
}

internal
CodeFn parse_functon()
{
	using namespace Parser;
	push_scope();

	SpecifierT specs_found[16] { ESpecifier::NumSpecifiers };
	s32        NumSpecifiers = 0;

	CodeAttributes attributes = { nullptr };
	CodeSpecifiers  specifiers = { nullptr };
	ModuleFlag     mflags     = ModuleFlag::None;

	if ( check(TokType::Module_Export) )
	{
		mflags = ModuleFlag::Export;
		eat( TokType::Module_Export );
	}

	attributes = parse_attributes();

	while ( left && currtok.is_specifier() )
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
				log_failure( "Invalid specifier %s for functon\n%s", ESpecifier::to_str(spec), Context.to_string() );
				return CodeInvalid;
		}

		if ( spec == ESpecifier::Const )
			continue;

		specs_found[NumSpecifiers] = spec;
		NumSpecifiers++;
		eat( currtok.Type );
	}

	if ( NumSpecifiers )
	{
		specifiers = def_specifiers( NumSpecifiers, specs_found );
	}

	CodeType ret_type = parse_type( toks, stringize(parse_function) );
	if ( ret_type == Code::Invalid )
		return CodeInvalid;

	Token name = parse_identifier( toks, stringize(parse_function) );
	if ( ! name )
		return CodeInvalid;

	CodeFn result = parse_function_after_name( mflags, attributes, specifiers, ret_type, name );

	Context.pop();
	return result;
}

CodeFn parse_function( StrC def )
{
	check_parse_args( parse_function, def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	return (CodeFn) parse_functon();
}

CodeBody parse_global_body( StrC def )
{
	check_parse_args( parse_global_body, def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	push_scope();
	CodeBody result = parse_global_nspace( ECode::Global_Body );
	Context.pop();
	return result;
}

internal
CodeNamespace parse_namespace( Parser::TokArray& toks, char const* context )
{
	using namespace Parser;
	push_scope();

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

	Context.pop();
	return result;
}

CodeNamespace parse_namespace( StrC def )
{
	check_parse_args( parse_namespace, def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	return parse_namespace();
}

internal
CodeOperator parse_operator()
{
	using namespace Parser;
	push_scope();

	CodeAttributes attributes = { nullptr };
	CodeSpecifiers specifiers = { nullptr };
	ModuleFlag     mflags     = ModuleFlag::None;

	SpecifierT specs_found[16] { ESpecifier::NumSpecifiers };
	s32        NumSpecifiers = 0;

	if ( check(TokType::Module_Export) )
	{
		mflags = ModuleFlag::Export;
		eat( TokType::Module_Export );
	}

	attributes = parse_attributes();

	while ( left && currtok.is_specifier() )
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
				log_failure( "Invalid specifier " "%s" " for operator\n%s", ESpecifier::to_str(spec), Context.to_string() );
				return CodeInvalid;
		}

		if ( spec == ESpecifier::Const )
			continue;

		specs_found[NumSpecifiers] = spec;
		NumSpecifiers++;
		eat( currtok.Type );
	}

	if ( NumSpecifiers )
	{
		specifiers = def_specifiers( NumSpecifiers, specs_found );
	}

	// Parse Return Type
	CodeType ret_type = parse_type( toks, stringize(parse_operator) );

	CodeOperator result = parse_operator_after_ret_type( mflags, attributes, specifiers, ret_type );

	Context.pop();
	return result;
}

CodeOperator parse_operator( StrC def )
{
	check_parse_args( parse_operator, def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	return (CodeOperator) parse_operator();
}

CodeOpCast parse_operator_cast()
{
	using namespace Parser;
	push_scope();

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

	Context.pop();
	return result;
}

CodeOpCast parse_operator_cast( StrC def )
{
	check_parse_args( parse_operator_cast, def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	return parse_operator_cast();
}

internal inline
CodeStruct parse_struct()
{
	push_scope();
	CodeStruct result = (CodeStruct) parse_class_struct();
	Context.pop();
	return result;
}

CodeStruct parse_struct( StrC def )
{
	check_parse_args( parse_struct, def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	return (CodeStruct) parse_class_struct( TokType::Decl_Struct );
}

internal
CodeTemplate parse_template()
{
#	define UseTemplateCapture true

	using namespace Parser;
	push_scope();

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

		SpecifierT specs_found[16] { ESpecifier::NumSpecifiers };
		s32        NumSpecifiers = 0;

		attributes = parse_attributes();

		while ( left && currtok.is_specifier() )
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
					log_failure( "Invalid specifier %s for variable or function\n%s", ESpecifier::to_str( spec ), Context.to_string() );
					return CodeInvalid;
			}

			// Ignore const it will be handled by the type
			if ( spec == ESpecifier::Const )
				continue;

			specs_found[NumSpecifiers] = spec;
			NumSpecifiers++;
			eat( currtok.Type );
		}

		if ( NumSpecifiers )
		{
			specifiers = def_specifiers( NumSpecifiers, specs_found );
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

	Context.pop();
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

	return parse_template();
}

internal
CodeType parse_type()
{
	using namespace Parser;
	push_scope();

	Token context_tok = prevtok;

	SpecifierT specs_found[16] { ESpecifier::NumSpecifiers };
	s32        NumSpecifiers = 0;

	Token name      = { nullptr, 0, TokType::Invalid };
	Token brute_sig = { currtok.Text, 0, TokType::Invalid };

	CodeAttributes attributes = parse_attributes();

	while ( left && currtok.is_specifier() )
	{
		SpecifierT spec = ESpecifier::to_type( currtok );

		if ( spec != ESpecifier::Const )
		{
			log_failure( "Error, invalid specifier used in type definition: %s\n%s", currtok.Text, Context.to_string() );
			return CodeInvalid;
		}

		specs_found[NumSpecifiers] = spec;
		NumSpecifiers++;
		eat( currtok.Type );
	}

	if ( left == 0 )
	{
		log_failure( "Error, unexpected end of type definition\n%s", Context.to_string() );
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

	while ( left && currtok.is_specifier() )
	{
		SpecifierT spec = ESpecifier::to_type( currtok );

		if (   spec != ESpecifier::Const
			&& spec != ESpecifier::Ptr
			&& spec != ESpecifier::Ref
			&& spec != ESpecifier::RValue )
		{
			log_failure( "Error, invalid specifier used in type definition: %s\n%s", currtok.Text, Context.to_string() );
			return CodeInvalid;
		}

		specs_found[NumSpecifiers] = spec;
		NumSpecifiers++;
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
		if (NumSpecifiers)
		{
			Code specifiers = def_specifiers( NumSpecifiers, (SpecifierT*)specs_found );
			result->Specs = specifiers;
		}
	}

	result->Name = get_cached_string( name );

	if ( attributes )
		result->Attributes = attributes;

	Context.pop();
	return result;
}

CodeType parse_type( StrC def )
{
	check_parse_args( parse_type, def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	return parse_type();
}

internal
CodeTypedef parse_typedef()
{
	using namespace Parser;
	push_scope();

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
		log_failure( "Error, expected identifier for typedef\n%s", Context.to_string() );
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

	Context.pop();
	return result;
}

CodeTypedef parse_typedef( StrC def )
{
	check_parse_args( parse_typedef, def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	return parse_typedef();
}

internal
CodeUnion parse_union()
{
	using namespace Parser;
	push_scope();

	ModuleFlag mflags = ModuleFlag::None;

	if ( check(TokType::Module_Export) )
	{
		mflags = ModuleFlag::Export;
		eat( TokType::Module_Export );
	}

	eat( TokType::Decl_Union );

	CodeAttributes attributes = parse_attributes();

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

	Context.pop();
	return result;
}

CodeUnion parse_union( StrC def )
{
	check_parse_args( parse_union, def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	return parse_union();
}

internal
CodeUsing parse_using()
{
	using namespace Parser;
	push_scope();

	SpecifierT specs_found[16] { ESpecifier::Invalid };
	s32        NumSpecifiers = 0;

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

	Context.pop();
	return result;
}

CodeUsing parse_using( StrC def )
{
	check_parse_args( parse_using, def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	return parse_using();
}

internal
CodeVar parse_variable()
{
	using namespace Parser;
	push_scope();

	SpecifierT specs_found[16] { ESpecifier::NumSpecifiers };
	s32        NumSpecifiers = 0;

	ModuleFlag	   mflags     = ModuleFlag::None;
	CodeAttributes attributes = { nullptr };
	CodeSpecifiers specifiers = { nullptr };

	if ( check(TokType::Module_Export) )
	{
		mflags = ModuleFlag::Export;
		eat( TokType::Module_Export );
	}

	attributes = parse_attributes();

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
				log_failure( "Invalid specifier %s for variable\n%s", ESpecifier::to_str( spec ), Context.to_string() );
				return CodeInvalid;
		}

		// Ignore const specifiers, they're handled by the type
		if ( spec == ESpecifier::Const )
			continue;

		specs_found[NumSpecifiers] = spec;
		NumSpecifiers++;
		eat( currtok.Type );
	}

	if ( NumSpecifiers )
	{
		specifiers = def_specifiers( NumSpecifiers, specs_found );
	}

	CodeType type = parse_type( toks, context );

	if ( type == Code::Invalid )
		return CodeInvalid;

	Context.Scope->Name = current;
	eat( TokType::Identifier );

	CodeVar result = parse_variable_after_name( mflags, attributes, specifiers, type, Context.Scope->Name );

	Context.pop();
	return result;
}

CodeVar parse_variable( StrC def )
{
	check_parse_args( parse_variable, def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	return parse_variable();
}

// Undef helper macros
#	undef check_parse_args
#	undef nexttok
#	undef currtok
#	undef prevtok
#	undef eat
#	undef left
#	undef check
#	undef push_scope
