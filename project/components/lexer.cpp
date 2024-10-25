#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "interface.upfront.cpp"
#include "gen/etoktype.cpp"
#endif

namespace parser {

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

	operator bool()
	{
		return Text && Length && Type != TokType::Invalid;
	}

	operator StrC()
	{
		return { Length, Text };
	}

	bool is_access_operator()
	{
		return bitfield_is_equal( u32, Flags, TF_AccessOperator );
	}

	bool is_access_specifier()
	{
		return bitfield_is_equal( u32, Flags, TF_AccessSpecifier );
	}

	bool is_attribute()
	{
		return bitfield_is_equal( u32, Flags, TF_Attribute );
	}

	bool is_operator()
	{
		return bitfield_is_equal( u32, Flags, TF_Operator );
	}

	bool is_preprocessor()
	{
		return bitfield_is_equal( u32, Flags, TF_Preprocess );
	}

	bool is_preprocess_cond()
	{
		return bitfield_is_equal( u32, Flags, TF_Preprocess_Cond );
	}

	bool is_specifier()
	{
		return bitfield_is_equal( u32, Flags, TF_Specifier );
	}

	bool is_end_definition()
	{
		return bitfield_is_equal( u32, Flags, TF_EndDefinition );
	}

	AccessSpec to_access_specifier()
	{
		return scast(AccessSpec, Type);
	}

	String to_string()
	{
		String result = String::make_reserve( GlobalAllocator, kilobytes(4) );

		StrC type_str = ETokType::to_str( Type );

		result.append_fmt( "Line: %d Column: %d, Type: %.*s Content: %.*s"
			, Line, Column
			, type_str.Len, type_str.Ptr
			, Length, Text
		);

		return result;
	}
};

constexpr Token NullToken { nullptr, 0, TokType::Invalid, false, 0, TF_Null };

struct TokArray
{
	Array<Token> Arr;
	s32          Idx;

	bool __eat( TokType type );

	Token& current( bool skip_formatting = true )
	{
		if ( skip_formatting )
		{
			while ( Arr[Idx].Type == TokType::NewLine || Arr[Idx].Type == TokType::Comment  )
				Idx++;
		}

		return Arr[Idx];
	}

	Token& previous( bool skip_formatting = false )
	{
		s32 idx = this->Idx;

		if ( skip_formatting )
		{
			while ( Arr[idx].Type == TokType::NewLine )
				idx--;

			return Arr[idx];
		}

		return Arr[idx - 1];
	}

	Token& next( bool skip_formatting = false )
	{
		s32 idx = this->Idx;

		if ( skip_formatting )
		{
			while ( Arr[idx].Type == TokType::NewLine )
				idx++;

			return Arr[idx + 1];
		}

		return Arr[idx + 1];
	}

	Token& operator []( s32 idx )
	{
		return Arr[idx];
	}
};

global Arena_256KB     defines_map_arena;
global HashTable<StrC> defines;
global Array<Token>    Tokens;

#define current ( * scanner )

#define move_forward()          \
	{                           \
		if ( current == '\n' )  \
		{                       \
			line++;             \
			column = 1;         \
		}                       \
		else                    \
		{                       \
			column++;           \
		}                       \
		left--;                 \
		scanner++;              \
	}

#define SkipWhitespace()                        \
	while ( left && char_is_space( current ) )  \
	{                                           \
		move_forward();                         \
	}

#define end_line()                          \
	do                                      \
	{                                       \
		while ( left && current == ' ' )    \
		{                                   \
			move_forward();                 \
		}                                   \
		if ( left && current == '\r' )      \
		{                                   \
			move_forward();                 \
			move_forward();                 \
		}                                   \
		else if ( left && current == '\n' ) \
		{                                   \
			move_forward();                 \
		}                                   \
	}                                       \
	while (0)

enum
{
	Lex_Continue,
	Lex_ReturnNull,
};

forceinline
s32 lex_preprocessor_directive(
	  StrC&            content
	, s32&             left
	, char const*&     scanner
	, s32&             line
	, s32&             column
	, HashTable<StrC>& defines
	, Token&           token )
{
	char const* hash = scanner;
	Tokens.append( { hash, 1, TokType::Preprocess_Hash, line, column, TF_Preprocess } );

	move_forward();
	SkipWhitespace();

	token.Text = scanner;
	while (left && ! char_is_space(current) )
	{
		move_forward();
		token.Length++;
	}

	token.Type = ETokType::to_type( token );

	bool   is_preprocessor = token.Type >= TokType::Preprocess_Define && token.Type <= TokType::Preprocess_Pragma;
	if ( ! is_preprocessor )
	{
		token.Type  = TokType::Preprocess_Unsupported;

		// Its an unsupported directive, skip it
		s32 within_string = false;
		s32 within_char   = false;
		while ( left )
		{
			if ( current == '"' && ! within_char )
				within_string ^= true;

			if ( current == '\'' && ! within_string )
				within_char ^= true;

			if ( current == '\\' && ! within_string && ! within_char )
			{
				move_forward();
				token.Length++;

				if ( current == '\r' )
				{
					move_forward();
					token.Length++;
				}

				if ( current == '\n' )
				{
					move_forward();
					token.Length++;
					continue;
				}
				else
				{
					log_failure( "gen::Parser::lex: Invalid escape sequence '\\%c' (%d, %d)"
								" in preprocessor directive (%d, %d)\n%.100s"
						, current, line, column
						, token.Line, token.Column, token.Text );
					break;
				}
			}

			if ( current == '\r' )
			{
				move_forward();
				token.Length++;
			}

			if ( current == '\n' )
			{
				move_forward();
				token.Length++;
				break;
			}

			move_forward();
			token.Length++;
		}

		token.Length = token.Length + token.Text - hash;
		token.Text   = hash;
		Tokens.append( token );
		return Lex_Continue; // Skip found token, its all handled here.
	}

	if ( token.Type == TokType::Preprocess_Else || token.Type == TokType::Preprocess_EndIf )
	{
		token.Flags |= TF_Preprocess_Cond;
		Tokens.append( token );
		end_line();
		return Lex_Continue;
	}
	else if ( token.Type >= TokType::Preprocess_If && token.Type <= TokType::Preprocess_ElIf  )
	{
		token.Flags |= TF_Preprocess_Cond;
	}

	Tokens.append( token );

	SkipWhitespace();

	if ( token.Type == TokType::Preprocess_Define )
	{
		Token name = { scanner, 0, TokType::Identifier, line, column, TF_Preprocess };

		name.Text   = scanner;
		name.Length = 1;
		move_forward();

		while ( left && ( char_is_alphanumeric(current) || current == '_' ) )
		{
			move_forward();
			name.Length++;
		}

		if ( left && current == '(' )
		{
			move_forward();
			name.Length++;
		}

		Tokens.append( name );

		u64 key = crc32( name.Text, name.Length );
		defines.set( key, name );
	}

	Token preprocess_content = { scanner, 0, TokType::Preprocess_Content, line, column, TF_Preprocess };

	if ( token.Type == TokType::Preprocess_Include )
	{
		preprocess_content.Type = TokType::String;

		if ( current != '"' && current != '<' )
		{
			String directive_str = String::fmt_buf( GlobalAllocator, "%.*s", min( 80, left + preprocess_content.Length ), token.Text );

			log_failure( "gen::Parser::lex: Expected '\"' or '<' after #include, not '%c' (%d, %d)\n%s"
				, current
				, preprocess_content.Line
				, preprocess_content.Column
				, directive_str.Data
			);
			return Lex_ReturnNull;
		}
		move_forward();
		preprocess_content.Length++;

		while ( left && current != '"' && current != '>' )
		{
			move_forward();
			preprocess_content.Length++;
		}

		move_forward();
		preprocess_content.Length++;

		if ( current == '\r' && scanner[1] == '\n' )
		{
			move_forward();
			move_forward();
		}
		else if ( current == '\n' )
		{
			move_forward();
		}

		Tokens.append( preprocess_content );
		return Lex_Continue; // Skip found token, its all handled here.
	}

	s32 within_string = false;
	s32 within_char   = false;

	// SkipWhitespace();
	while ( left )
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
				String directive_str = String::make_length( GlobalAllocator, token.Text, token.Length );
				String content_str   = String::fmt_buf( GlobalAllocator, "%.*s", min( 400, left + preprocess_content.Length ), preprocess_content.Text );

				log_failure( "gen::Parser::lex: Invalid escape sequence '\\%c' (%d, %d)"
							" in preprocessor directive '%s' (%d, %d)\n%s"
					, current, line, column
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

	Tokens.append( preprocess_content );
	return Lex_Continue; // Skip found token, its all handled here.
}

forceinline
void lex_found_token( StrC& content
	, s32&             left
	, char const*&     scanner
	, s32&             line
	, s32&             column
	, HashTable<StrC>& defines
	, Token&           token )
{
	if ( token.Type != TokType::Invalid )
	{
		Tokens.append( token );
		return;
	}

	TokType type = ETokType::to_type( token );

	if (type <= TokType::Access_Public && type >= TokType::Access_Private )
	{
		token.Flags |= TF_AccessSpecifier;
	}

	if ( type > TokType::__Attributes_Start )
	{
		token.Flags |= TF_Attribute;
	}

	if ( type == ETokType::Decl_Extern_Linkage )
	{
		SkipWhitespace();

		if ( current != '"' )
		{
			type         = ETokType::Spec_Extern;
			token.Flags |= TF_Specifier;
		}

		token.Type = type;
		Tokens.append( token );
		return;
	}

	if ( ( type <= TokType::Star && type >= TokType::Spec_Alignas)
			|| type == TokType::Ampersand
			|| type == TokType::Ampersand_DBL )
	{
		token.Type   = type;
		token.Flags |= TF_Specifier;
		Tokens.append( token );
		return;
	}


	if ( type != TokType::Invalid )
	{
		token.Type = type;
		Tokens.append( token );
		return;
	}

	u64 key = 0;
	if ( current == '(')
		key = crc32( token.Text, token.Length + 1 );
	else
		key = crc32( token.Text, token.Length );

	StrC* define = defines.get( key );
	if ( define )
	{
		token.Type = TokType::Preprocess_Macro;

		// Want to ignore any arguments the define may have as they can be execution expressions.
		if ( left && current == '(' )
		{
			move_forward();
			token.Length++;

			s32 level = 0;
			while ( left && (current != ')' || level > 0) )
			{
				if ( current == '(' )
					level++;

				else if ( current == ')' && level > 0 )
					level--;

				move_forward();
				token.Length++;
			}

			move_forward();
			token.Length++;
		}

		if ( current == '\r' && scanner[1] == '\n' )
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
		token.Type = TokType::Identifier;
	}

	Tokens.append( token );
}


neverinline
// TokArray lex( Array<Token> tokens, StrC content )
TokArray lex( StrC content )
{
	s32         left    = content.Len;
	char const* scanner = content.Ptr;

	char const* word        = scanner;
	s32         word_length = 0;

	s32 line   = 1;
	s32 column = 1;

	SkipWhitespace();
	if ( left <= 0 )
	{
		log_failure( "gen::lex: no tokens found (only whitespace provided)" );
		return { { nullptr }, 0 };
	}

	for ( StringCached entry : PreprocessorDefines )
	{
		s32         length  = 0;
		char const* scanner = entry.Data;
		while ( entry.length() > length && (char_is_alphanumeric( *scanner ) || *scanner == '_') )
		{
			scanner++;
			length ++;
		}
		if ( scanner[0] == '(' )
		{
			length++;
		}

		u64 key = crc32( entry.Data, length );
		defines.set( key, entry );
	}

	Tokens.clear();

	while (left )
	{
		#if 0
		if (Tokens.num())
		{
			log_fmt("\nLastTok: %S", Tokens.back().to_string());
		}
		#endif

		Token token = { scanner, 0, TokType::Invalid, line, column, TF_Null };

		bool is_define = false;

		if ( column == 1 )
		{
			if ( current == '\r')
			{
				move_forward();
				token.Length = 1;
			}

			if ( current == '\n' )
			{
				move_forward();

				token.Type = TokType::NewLine;
				token.Length++;

				Tokens.append( token );
				continue;
			}
		}

		token.Length = 0;

		SkipWhitespace();
		if ( left <= 0 )
			break;

		switch ( current )
		{
			case '#':
			{
				s32 result = lex_preprocessor_directive( content, left, scanner, line, column, defines, token );
				switch ( result )
				{
					case Lex_Continue:
						continue;

					case Lex_ReturnNull:
						return { { nullptr }, 0 };
				}
			}
			case '.':
			{
				token.Text   = scanner;
				token.Length = 1;
				token.Type   = TokType::Access_MemberSymbol;
				token.Flags  = TF_AccessOperator;

				if (left) {
					move_forward();
				}

				if ( current == '.' )
				{
					move_forward();
					if( current == '.' )
					{
						token.Length = 3;
						token.Type   = TokType::Varadic_Argument;
						token.Flags  = TF_Null;
						move_forward();
					}
					else
					{
						String context_str = String::fmt_buf( GlobalAllocator, "%s", scanner, min( 100, left ) );

						log_failure( "gen::lex: invalid varadic argument, expected '...' got '..%c' (%d, %d)\n%s", current, line, column, context_str );
					}
				}

				goto FoundToken;
			}
			case '&' :
			{
				token.Text   = scanner;
				token.Length = 1;
				token.Type   = TokType::Ampersand;
				token.Flags |= TF_Operator;
				token.Flags |= TF_Specifier;

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
			}
			case ':':
			{
				token.Text   = scanner;
				token.Length = 1;
				token.Type   = TokType::Assign_Classifer;
				// Can be either a classifier (ParentType, Bitfield width), or ternary else
				// token.Type   = TokType::Colon;

				if (left)
					move_forward();

				if ( current == ':' )
				{
					move_forward();
					token.Type  = TokType::Access_StaticSymbol;
					token.Length++;
				}
				goto FoundToken;
			}
			case '{':
			{
				token.Text   = scanner;
				token.Length = 1;
				token.Type   = TokType::BraceCurly_Open;

				if (left)
					move_forward();
				goto FoundToken;
			}
			case '}':
			{
				token.Text   = scanner;
				token.Length = 1;
				token.Type   = TokType::BraceCurly_Close;
				token.Flags  = TF_EndDefinition;

				if (left)
					move_forward();

				end_line();
				goto FoundToken;
			}
			case '[':
			{
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
			}
			case ']':
			{
				token.Text   = scanner;
				token.Length = 1;
				token.Type   = TokType::BraceSquare_Close;

				if (left)
					move_forward();
				goto FoundToken;
			}
			case '(':
			{
				token.Text   = scanner;
				token.Length = 1;
				token.Type   = TokType::Capture_Start;

				if (left)
					move_forward();
				goto FoundToken;
			}
			case ')':
			{
				token.Text   = scanner;
				token.Length = 1;
				token.Type   = TokType::Capture_End;

				if (left)
					move_forward();
				goto FoundToken;
			}
			case '\'':
			{
				token.Text   = scanner;
				token.Length = 1;
				token.Type   = TokType::Char;
				token.Flags  = TF_Literal;

				move_forward();

				if ( left && current == '\\' )
				{
					move_forward();
					token.Length++;

					if ( current == '\'' )
					{
						move_forward();
						token.Length++;
					}
				}

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
			}
			case ',':
			{
				token.Text   = scanner;
				token.Length = 1;
				token.Type   = TokType::Comma;
				token.Flags  = TF_Operator;

				if (left)
					move_forward();
				goto FoundToken;
			}
			case '*':
			{
				token.Text   = scanner;
				token.Length = 1;
				token.Type   = TokType::Star;
				token.Flags |= TF_Specifier;
				token.Flags |= TF_Operator;

				if (left)
					move_forward();

				if ( current == '=' )
				{
					token.Length++;
					token.Flags |= TF_Assign;
					// token.Type = TokType::Assign_Multiply;

					if ( left )
						move_forward();
				}

				goto FoundToken;
			}
			case ';':
			{
				token.Text   = scanner;
				token.Length = 1;
				token.Type   = TokType::Statement_End;
				token.Flags  = TF_EndDefinition;

				if (left)
					move_forward();

				end_line();
				goto FoundToken;
			}
			case '"':
			{
				token.Text   = scanner;
				token.Length = 1;
				token.Type   = TokType::String;
				token.Flags |= TF_Literal;

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
			}
			case '?':
			{
				token.Text     = scanner;
				token.Length   = 1;
				token.Type     = TokType::Operator;
				// token.Type     = TokType::Ternary;
				token.Flags    = TF_Operator;

				if (left)
					move_forward();

				goto FoundToken;
			}
			case '=':
			{
				token.Text     = scanner;
				token.Length   = 1;
				token.Type     = TokType::Operator;
				// token.Type     = TokType::Assign;
				token.Flags    = TF_Operator;
				token.Flags   |= TF_Assign;

				if (left)
					move_forward();

				if ( current == '=' )
				{
					token.Length++;
					token.Flags = TF_Operator;

					if (left)
						move_forward();
				}

				goto FoundToken;
			}
			case '+':
			{
				// token.Type = TokType::Add

			}
			case '%':
			{
				// token.Type = TokType::Modulo;

			}
			case '^':
			{
				// token.Type = TokType::B_XOr;
			}
			case '~':
			{
				// token.Type = TokType::Unary_Not;

			}
			case '!':
			{
				// token.Type = TokType::L_Not;
			}
			case '<':
			{
				// token.Type = TokType::Lesser;

			}
			case '>':
			{
				// token.Type = TokType::Greater;

			}
			case '|':
			{
				token.Text   = scanner;
				token.Length = 1;
				token.Type   = TokType::Operator;
				token.Flags  = TF_Operator;
				// token.Type   = TokType::L_Or;

				if (left)
					move_forward();

				if ( current == '=' )
				{
					token.Length++;
					token.Flags |= TF_Assign;
					// token.Flags |= TokFlags::Assignment;
					// token.Type = TokType::Assign_L_Or;

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
			}

			// Dash is unfortunatlly a bit more complicated...
			case '-':
			{
				token.Text   = scanner;
				token.Length = 1;
				token.Type   = TokType::Operator;
				// token.Type = TokType::Subtract;
				token.Flags  = TF_Operator;
				if ( left )
				{
					move_forward();

					if ( current == '>'  )
					{
						token.Length++;
//						token.Type = TokType::Access_PointerToMemberSymbol;
						token.Flags |= TF_AccessOperator;
						move_forward();

						if ( current == '*' )
						{
//							token.Type = TokType::Access_PointerToMemberOfPointerSymbol;
							token.Length++;
							move_forward();
						}
					}
					else if ( current == '=' )
					{
						token.Length++;
						// token.Type = TokType::Assign_Subtract;
						token.Flags |= TF_Assign;

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
			}
			case '/':
			{
				token.Text   = scanner;
				token.Length = 1;
				token.Type   = TokType::Operator;
				// token.Type   = TokType::Divide;
				token.Flags  = TF_Operator;
				move_forward();

				if ( left )
				{
					if ( current == '=' )
					{
						// token.Type = TokeType::Assign_Divide;
						move_forward();
						token.Length++;
						token.Flags = TF_Assign;
					}
					else if ( current == '/' )
					{
						token.Type   = TokType::Comment;
						token.Length = 2;
						token.Flags  = TF_Null;
						move_forward();

						while ( left && current != '\n' && current != '\r' )
						{
							move_forward();
							token.Length++;
						}

						if ( current == '\r' )
						{
							move_forward();
							token.Length++;
						}
						if ( current == '\n' )
						{
							move_forward();
							token.Length++;
						}
						Tokens.append( token );
						continue;
					}
					else if ( current == '*' )
					{
						token.Type   = TokType::Comment;
						token.Length = 2;
						token.Flags  = TF_Null;
						move_forward();

						bool star   = current    == '*';
						bool slash  = scanner[1] == '/';
						bool at_end = star && slash;
						while ( left && ! at_end  )
						{
							move_forward();
							token.Length++;

							star   = current    == '*';
							slash  = scanner[1] == '/';
							at_end = star && slash;
						}
						token.Length += 2;
						move_forward();
						move_forward();

						if ( current == '\r' )
						{
							move_forward();
							token.Length++;
						}
						if ( current == '\n' )
						{
							move_forward();
							token.Length++;
						}
						Tokens.append( token );
						// end_line();
						continue;
					}
				}
				goto FoundToken;
			}
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
			token.Flags  = TF_Literal;
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

				// Handle number literal suffixes in a botched way
				if (left && (
					current == 'l' || current == 'L' ||  // long/long long
					current == 'u' || current == 'U' ||  // unsigned
					current == 'f' || current == 'F' ||  // float
					current == 'i' || current == 'I' ||  // imaginary
					current == 'z' || current == 'Z'))   // complex
				{
					char prev = current;
					move_forward();
					token.Length++;

					// Handle 'll'/'LL' as a special case when we just processed an 'l'/'L'
					if (left && (prev == 'l' || prev == 'L') && (current == 'l' || current == 'L'))
					{
						move_forward();
						token.Length++;
					}
				}
			}

			goto FoundToken;
		}
		else
		{
			s32 start = max( 0, Tokens.num() - 100 );
			log_fmt("\n%d\n", start);
			for ( s32 idx = start; idx < Tokens.num(); idx++ )
			{
				log_fmt( "Token %d Type: %s : %.*s\n"
					, idx
					, ETokType::to_str( Tokens[ idx ].Type ).Ptr
					, Tokens[ idx ].Length, Tokens[ idx ].Text
				);
			}

			String context_str = String::fmt_buf( GlobalAllocator, "%.*s", min( 100, left ), scanner );
			log_failure( "Failed to lex token '%c' (%d, %d)\n%s", current, line, column, context_str );

			// Skip to next whitespace since we can't know if anything else is valid until then.
			while ( left && ! char_is_space( current ) )
			{
				move_forward();
			}
		}

	FoundToken:
		lex_found_token( content, left, scanner, line, column, defines, token );
	}

	if ( Tokens.num() == 0 )
	{
		log_failure( "Failed to lex any tokens" );
		return { { nullptr }, 0 };
	}

	defines.clear();
	// defines_map_arena.free();
	return { Tokens, 0 };
}
#undef current
#undef move_forward
#undef SkipWhitespace

// namespace parser
}
