#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "gen/etoktype.cpp"
#include "interface.upfront.cpp"
#include "lexer.cpp"
#endif

GEN_NS_PARSER_BEGIN

// TODO(Ed) : Rename ETok_Capture_Start, ETok_Capture_End to Open_Parenthesis adn Close_Parenthesis

constexpr bool lex_dont_skip_formatting = false;
constexpr bool      lex_skip_formatting = true;

struct StackNode
{
	StackNode* Prev;

	Token Start;
	Token Name;        // The name of the AST node (if parsed)
	StrC  ProcName;    // The name of the procedure
};

struct ParseContext
{
	TokArray   Tokens;
	StackNode* Scope;
};

void parser_push( ParseContext* ctx, StackNode* node )
{
	node->Prev = ctx->Scope;
	ctx->Scope = node;

#if 0 && Build_Debug
	log_fmt("\tEntering Context: %.*s\n", Scope->ProcName.Len, Scope->ProcName.Ptr );
#endif
}

void parser_pop(ParseContext* ctx)
{
#if 0 && Build_Debug
	log_fmt("\tPopping  Context: %.*s\n", Scope->ProcName.Len, Scope->ProcName.Ptr );
#endif
	ctx->Scope = ctx->Scope->Prev;
}

String parser_to_string(ParseContext ctx)
{
	String result = string_make_reserve( GlobalAllocator, kilobytes(4) );

	Token scope_start = ctx.Scope->Start;
	Token last_valid  = ctx.Tokens.Idx >= array_num(ctx.Tokens.Arr) ? ctx.Tokens.Arr[array_num(ctx.Tokens.Arr) -1] : (* lex_current(& ctx.Tokens, true));

	sptr        length  = scope_start.Length;
	char const* current = scope_start.Text + length;
	while ( current <= array_back( ctx.Tokens.Arr)->Text && *current != '\n' && length < 74 )
	{
		current++;
		length++;
	}

	StrC scope_strc = { length, scope_start.Text };
	String line = string_make_strc( GlobalAllocator, scope_strc );
	string_append_fmt( & result, "\tScope    : %s\n", line );
	string_free(& line);

	sptr   dist            = (sptr)last_valid.Text - (sptr)scope_start.Text + 2;
	sptr   length_from_err = dist;

	StrC err_strc        = { length_from_err, last_valid.Text };
	String line_from_err = string_make_strc( GlobalAllocator, err_strc );

	if ( length_from_err < 100 )
		string_append_fmt(& result, "\t(%d, %d):%*c\n", last_valid.Line, last_valid.Column, length_from_err, '^' );
	else
		string_append_fmt(& result, "\t(%d, %d)\n", last_valid.Line, last_valid.Column );

	StackNode* curr_scope = ctx.Scope;
	s32 level = 0;
	do
	{
		if ( tok_is_valid(curr_scope->Name) )
		{
			string_append_fmt(& result, "\t%d: %s, AST Name: %.*s\n", level, curr_scope->ProcName.Ptr, curr_scope->Name.Length, curr_scope->Name.Text );
		}
		else
		{
			string_append_fmt(& result, "\t%d: %s\n", level, curr_scope->ProcName.Ptr );
		}

		curr_scope = curr_scope->Prev;
		level++;
	}
	while ( curr_scope );
	return result;
}

global ParseContext Context;

bool lex__eat(TokArray* self, TokType type )
{
	if ( array_num(self->Arr) - self->Idx <= 0 )
	{
		log_failure( "No tokens left.\n%s", parser_to_string(Context) );
		return false;
	}

	Token at_idx = self->Arr[ self->Idx ];

	if ( ( at_idx.Type == Tok_NewLine && type != Tok_NewLine )
	||   ( at_idx.Type == Tok_Comment && type != Tok_Comment ) )
	{
		self->Idx ++;
	}

	if ( at_idx.Type != type )
	{
		Token tok = * lex_current( self, lex_skip_formatting );
		log_failure( "Parse Error, TokArray::eat, Expected: ' %s ' not ' %.*s ' (%d, %d)`\n%s"
			, toktype_to_str(type).Ptr
			, at_idx.Length, at_idx.Text
			, tok.Line
			, tok.Column
			, parser_to_string(Context)
		);

		return false;
	}

#if 0 && Build_Debug
	log_fmt("Ate: %S\n", self->Arr[Idx].to_string() );
#endif

	self->Idx ++;
	return true;
}

internal
void parser_init()
{
	Lexer_Tokens = array_init_reserve(Token, arena_allocator_info( & LexArena)
		, ( LexAllocator_Size - sizeof( ArrayHeader ) ) / sizeof(Token)
	);

	fixed_arena_init(& Lexer_defines_map_arena);
	Lexer_defines = hashtable_init_reserve(StrC, fixed_arena_allocator_info( & Lexer_defines_map_arena), 256 );
}

internal
void parser_deinit()
{
	Array(Token) null_array = { nullptr };
	Lexer_Tokens = null_array;
}

#pragma region Helper Macros

#define check_parse_args( def ) _check_parse_args(def, stringize(_func_) )
bool _check_parse_args( StrC def, char const* func_name )
{
	if ( def.Len <= 0 )
	{
		log_failure( str_fmt_buf("gen::%s: length must greater than 0", func_name) );
		parser_pop(& Context);
		return false;
	}
	if ( def.Ptr == nullptr )
	{
		log_failure( str_fmt_buf("gen::%s: def was null", func_name) );
		parser_pop(& Context);
		return false;
	}
	return true;
}

#	define currtok_noskip (* lex_current( &  Context.Tokens, lex_dont_skip_formatting ))
#	define currtok        (* lex_current( & Context.Tokens, lex_skip_formatting ))
#	define peektok        (* lex_peek(Context.Tokens, lex_skip_formatting))
#	define prevtok        (* lex_previous( Context.Tokens, lex_dont_skip_formatting))
#	define nexttok		  (* lex_next( Context.Tokens, lex_skip_formatting ))
#	define nexttok_noskip (* lex_next( Context.Tokens, lex_dont_skip_formatting))
#	define eat( Type_ )   lex__eat( & Context.Tokens, Type_ )
#	define left           ( array_num(Context.Tokens.Arr) - Context.Tokens.Idx )

#if GEN_COMPILER_CPP
#	define def_assign( ... ) { __VA_ARGS__ }
#else
#	define def_assign( ... ) __VA_ARGS__
#endif


#ifdef check
#define CHECK_WAS_DEFINED
#pragma push_macro("check")
#undef check
#endif

#	define check_noskip( Type_ ) ( left && currtok_noskip.Type == Type_ )
#	define check( Type_ )        ( left && currtok.Type        == Type_ )

#	define push_scope()                                                                                  \
	GEN_NS_PARSER StackNode scope = { nullptr, currtok_noskip, GEN_NS_PARSER NullToken, txt( __func__ ) }; \
	parser_push( & GEN_NS_PARSER Context, & scope )

#pragma endregion Helper Macros

// Procedure Forwards ( Entire parser internal parser interface )

internal Code               parse_array_decl                   ();
internal CodeAttributes     parse_attributes                   ();
internal CodeComment        parse_comment                      ();
internal Code               parse_complicated_definition       ( TokType which );
internal CodeBody           parse_class_struct_body            ( TokType which, Token name );
internal Code               parse_class_struct                 ( TokType which, bool inplace_def );
internal CodeDefine         parse_define                       ();
internal Code               parse_expression                   ();
internal Code               parse_forward_or_definition        ( TokType which, bool is_inplace );
internal CodeFn             parse_function_after_name          ( ModuleFlag mflags, CodeAttributes attributes, CodeSpecifiers specifiers, CodeTypename ret_type, Token name );
internal Code               parse_function_body                ();
internal CodeBody           parse_global_nspace                ( CodeType which );
internal Code               parse_global_nspace_constructor_destructor( CodeSpecifiers specifiers );
internal Token              parse_identifier                   ( bool* possible_member_function );
internal CodeInclude        parse_include                      ();
internal CodeOperator       parse_operator_after_ret_type      ( ModuleFlag mflags, CodeAttributes attributes, CodeSpecifiers specifiers, CodeTypename ret_type );
internal Code               parse_operator_function_or_variable( bool expects_function, CodeAttributes attributes, CodeSpecifiers specifiers );
internal CodePragma         parse_pragma                       ();
internal CodeParam          parse_params                       ( bool use_template_capture );
internal CodePreprocessCond parse_preprocess_cond              ();
internal Code               parse_simple_preprocess            ( TokType which, bool dont_consume_braces );
internal Code               parse_static_assert                ();
internal void               parse_template_args                ( Token* token );
internal CodeVar            parse_variable_after_name          ( ModuleFlag mflags, CodeAttributes attributes, CodeSpecifiers specifiers, CodeTypename type, StrC name );
internal CodeVar            parse_variable_declaration_list    ();

internal CodeClass       parser_parse_class    ( bool inplace_def );
internal CodeConstructor parser_parse_constructor     ( CodeSpecifiers specifiers );
internal CodeDestructor  parser_parse_destructor      ( CodeSpecifiers specifiers );
internal CodeEnum        parser_parse_enum            ( bool inplace_def );
internal CodeBody        parser_parse_export_body     ();
internal CodeBody        parser_parse_extern_link_body();
internal CodeExtern      parser_parse_extern_link     ();
internal CodeFriend      parser_parse_friend          ();
internal CodeFn          parser_parse_function        ();
internal CodeNS          parser_parse_namespace       ();
internal CodeOpCast      parser_parse_operator_cast   ( CodeSpecifiers specifiers );
internal CodeStruct      parser_parse_struct          ( bool inplace_def );
internal CodeVar         parser_parse_variable        ();
internal CodeTemplate    parser_parse_template        ();
internal CodeTypename    parser_parse_type            ( bool from_template, bool* is_function );
internal CodeTypedef     parser_parse_typedef         ();
internal CodeUnion       parser_parse_union           ( bool inplace_def );
internal CodeUsing       parser_parse_using           ();

constexpr bool parser_inplace_def         = true;
constexpr bool parser_not_inplace_def     = false;
constexpr bool parser_dont_consume_braces = true;
constexpr bool parser_consume_braces      = false;
constexpr bool parser_not_from_template   = false;

constexpr bool parser_use_parenthesis = false;

// Internal parsing functions

constexpr bool parser_strip_formatting_dont_preserve_newlines = false;
/*
	This function was an attempt at stripping formatting from any c++ code.
	It has edge case failures that prevent it from being used in function bodies.
*/
internal
String parser_strip_formatting( StrC raw_text, bool preserve_newlines )
{
	String content = string_make_reserve( GlobalAllocator, raw_text.Len );

	if ( raw_text.Len == 0 )
		return content;

#define cut_length ( scanner - raw_text.Ptr - last_cut )
#define cut_ptr    ( raw_text.Ptr   + last_cut )
#define pos        ( rcast( sptr, scanner ) - rcast( sptr, raw_text.Ptr ) )
#define move_fwd() do { scanner++; tokleft--; } while(0)

	s32         tokleft  = raw_text.Len;
	sptr        last_cut = 0;
	char const* scanner  = raw_text.Ptr;

	if ( scanner[0] == ' ' )
	{
		move_fwd();
		last_cut = 1;
	}

	bool within_string     = false;
	bool within_char       = false;
	bool must_keep_newline = false;
	while ( tokleft )
	{
		// Skip over the content of string literals
		if ( scanner[0] == '"' )
		{
			move_fwd();

			while ( tokleft && ( scanner[0] != '"' || *( scanner - 1 ) == '\\' ) )
			{
				if ( scanner[0] == '\\' && tokleft > 1 )
				{
					scanner += 2;
					tokleft -= 2;
				}
				else
				{
					move_fwd();
				}
			}

			// Skip the closing "
			if ( tokleft )
				move_fwd();

			string_append_c_str_len( & content, cut_ptr, cut_length );
			last_cut = rcast(sptr, scanner ) - rcast( sptr, raw_text.Ptr );
			continue;
		}

		// Skip over the content of character literals
		if ( scanner[0] == '\'' )
		{
			move_fwd();

			while ( tokleft
			&& ( scanner[0] != '\''
				|| ( *(scanner -1 ) == '\\' )
			) )
			{
				move_fwd();
			}

			// Skip the closing '
			if ( tokleft )
				move_fwd();

			string_append_c_str_len( & content, cut_ptr, cut_length );
			last_cut = rcast( sptr, scanner ) - rcast( sptr, raw_text.Ptr );
			continue;
		}

		// Block comments
		if ( tokleft > 1 && scanner[0] == '/' && scanner[1] == '*' )
		{
			while ( tokleft > 1 && !(scanner[0] == '*' && scanner[1] == '/') )
				move_fwd();

			scanner += 2;
			tokleft -= 2;

			string_append_c_str_len( & content,  cut_ptr, cut_length );
			last_cut = rcast( sptr, scanner ) - rcast( sptr, raw_text.Ptr );
			continue;
		}

		// Line comments
		if ( tokleft > 1 && scanner[0] == '/' && scanner[1] == '/' )
		{
			must_keep_newline = true;

			scanner += 2;
			tokleft -= 2;

			while ( tokleft && scanner[ 0 ] != '\n' )
				move_fwd();

			if (tokleft)
				move_fwd();

			string_append_c_str_len( & content,  cut_ptr, cut_length );
			last_cut = rcast( sptr,  scanner ) - rcast( sptr, raw_text.Ptr );
			continue;
		}

		// Tabs
		if (scanner[0] == '\t')
		{
			if (pos > last_cut)
				string_append_c_str_len( & content, cut_ptr, cut_length);

			if ( * string_back( content ) != ' ' )
				string_append_char( & content, ' ' );

			move_fwd();
			last_cut = rcast( sptr, scanner) - rcast( sptr, raw_text.Ptr);
			continue;
		}

		if ( tokleft > 1 && scanner[0] == '\r' && scanner[1] == '\n' )
		{
			if ( must_keep_newline || preserve_newlines )
			{
				must_keep_newline = false;

				scanner += 2;
				tokleft -= 2;

				string_append_c_str_len( & content,  cut_ptr, cut_length );
				last_cut = rcast( sptr, scanner ) - rcast( sptr, raw_text.Ptr );
				continue;
			}

			if ( pos > last_cut )
				string_append_c_str_len( & content,  cut_ptr, cut_length );

			// Replace with a space
			if ( * string_back( content ) != ' ' )
				string_append_char( & content,  ' ' );

			scanner += 2;
			tokleft -= 2;

			last_cut = rcast( sptr, scanner ) - rcast( sptr, raw_text.Ptr );
			continue;
		}

		if ( scanner[0] == '\n' )
		{
			if ( must_keep_newline || preserve_newlines )
			{
				must_keep_newline = false;

				move_fwd();

				string_append_c_str_len( & content,  cut_ptr, cut_length );
				last_cut = rcast( sptr, scanner ) - rcast( sptr, raw_text.Ptr );
				continue;
			}

			if ( pos > last_cut )
				string_append_c_str_len( & content,  cut_ptr, cut_length );

			// Replace with a space
			if ( * string_back( content ) != ' ' )
				string_append_char( & content,  ' ' );

			move_fwd();

			last_cut = rcast( sptr, scanner ) - rcast( sptr, raw_text.Ptr );
			continue;
		}

		// Escaped newlines
		if ( scanner[0] == '\\' )
		{
			string_append_c_str_len( & content,  cut_ptr, cut_length );

			s32 amount_to_skip = 1;
			if ( tokleft > 1 && scanner[1] == '\n' )
			{
				amount_to_skip = 2;
			}
			else if ( tokleft > 2 && scanner[1] == '\r' && scanner[2] == '\n' )
			{
				amount_to_skip = 3;
			}

			if ( amount_to_skip > 1 && pos == last_cut )
			{
				scanner += amount_to_skip;
				tokleft -= amount_to_skip;
			}
			else
				move_fwd();

			last_cut = rcast( sptr, scanner ) - rcast( sptr, raw_text.Ptr );
			continue;
		}

		// Consectuive spaces
		if ( tokleft > 1 && char_is_space( scanner[0] ) && char_is_space( scanner[ 1 ] ) )
		{
			string_append_c_str_len( & content,  cut_ptr, cut_length );
			do
			{
				move_fwd();
			}
			while ( tokleft && char_is_space( scanner[0] ) );

			last_cut = rcast( sptr, scanner ) - rcast( sptr, raw_text.Ptr );

			// Preserve only 1 space of formattting
			char* last = string_back(content);
			if ( last == nullptr || * last != ' ' )
				string_append_char( & content, ' ' );

			continue;
		}

		move_fwd();
	}

	if ( last_cut < raw_text.Len )
	{
		string_append_c_str_len( & content,  cut_ptr, raw_text.Len - last_cut );
	}

#undef cut_ptr
#undef cut_length
#undef pos
#undef move_fwd

	return content;
}

internal
Code parse_array_decl()
{
	push_scope();

	if ( check( Tok_Operator ) && currtok.Text[0] == '[' && currtok.Text[1] == ']' )
	{
		Code array_expr = untyped_str( tok_to_str(currtok) );
		eat( Tok_Operator );
		// []

		parser_pop(& Context);
		return array_expr;
	}

	if ( check( Tok_BraceSquare_Open ) )
	{
		eat( Tok_BraceSquare_Open );
		// [

		if ( left == 0 )
		{
			log_failure( "Error, unexpected end of array declaration ( '[]' scope started )\n%s", parser_to_string(Context) );
			parser_pop(& Context);
			return InvalidCode;
		}

		if ( currtok.Type == Tok_BraceSquare_Close )
		{
			log_failure( "Error, empty array expression in definition\n%s", parser_to_string(Context) );
			parser_pop(& Context);
			return InvalidCode;
		}

		Token untyped_tok = currtok;

		while ( left && currtok.Type != Tok_BraceSquare_Close )
		{
			eat( currtok.Type );
		}

		untyped_tok.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)untyped_tok.Text;

		Code array_expr = untyped_str( tok_to_str(untyped_tok) );
		// [ <Content>

		if ( left == 0 )
		{
			log_failure( "Error, unexpected end of array declaration, expected ]\n%s", parser_to_string(Context) );
			parser_pop(& Context);
			return InvalidCode;
		}

		if ( currtok.Type != Tok_BraceSquare_Close )
		{
			log_failure( "%s: Error, expected ] in array declaration, not %s\n%s", toktype_to_str( currtok.Type ), parser_to_string(Context) );
			parser_pop(& Context);
			return InvalidCode;
		}

		eat( Tok_BraceSquare_Close );
		// [ <Content> ]

		// Its a multi-dimensional array
		if ( check( Tok_BraceSquare_Open ))
		{
			Code adjacent_arr_expr = parse_array_decl();
			// [ <Content> ][ <Content> ]...

			array_expr->Next = adjacent_arr_expr;
		}

		parser_pop(& Context);
		return array_expr;
	}

	parser_pop(& Context);
	return NullCode;
}

internal inline
CodeAttributes parse_attributes()
{
	push_scope();

	Token start = currtok;
	s32   len   = 0;

	// There can be more than one attribute. If there is flatten them to a single string.
	// TODO(Ed): Support keeping an linked list of attributes similar to parameters
	while ( left && tok_is_attribute(currtok) )
	{
		if ( check( Tok_Attribute_Open ) )
		{
			eat( Tok_Attribute_Open );
			// [[

			while ( left && currtok.Type != Tok_Attribute_Close )
			{
				eat( currtok.Type );
			}
			// [[ <Content>

			eat( Tok_Attribute_Close );
			// [[ <Content> ]]

			len = ( ( sptr )prevtok.Text + prevtok.Length ) - ( sptr )start.Text;
		}
		else if ( check( Tok_Decl_GNU_Attribute ) )
		{
			eat( Tok_Decl_GNU_Attribute );
			eat( Tok_Capture_Start );
			eat( Tok_Capture_Start );
			// __attribute__((

			while ( left && currtok.Type != Tok_Capture_End )
			{
				eat( currtok.Type );
			}
			// __attribute__(( <Content>

			eat( Tok_Capture_End );
			eat( Tok_Capture_End );
			// __attribute__(( <Content> ))

			len = ( ( sptr )prevtok.Text + prevtok.Length ) - ( sptr )start.Text;
		}
		else if ( check( Tok_Decl_MSVC_Attribute ) )
		{
			eat( Tok_Decl_MSVC_Attribute );
			eat( Tok_Capture_Start );
			// __declspec(

			while ( left && currtok.Type != Tok_Capture_End )
			{
				eat( currtok.Type );
			}
			// __declspec( <Content>

			eat( Tok_Capture_End );
			// __declspec( <Content> )

			len = ( ( sptr )prevtok.Text + prevtok.Length ) - ( sptr )start.Text;
		}
		else if ( tok_is_attribute(currtok) )
		{
			eat( currtok.Type );
			// <Attribute>

			// If its a macro based attribute, this could be a functional macro such as Unreal's UE_DEPRECATED(...)
			if ( check( Tok_Capture_Start))
			{
				eat( Tok_Capture_Start );

				s32 level = 0;
				while (left && currtok.Type != Tok_Capture_End && level == 0)
				{
					if (currtok.Type == Tok_Capture_Start)
						++ level;
					if (currtok.Type == Tok_Capture_End)
						--level;
					eat(currtok.Type);
				}
				eat(Tok_Capture_End);
			}

			len = ( ( sptr )prevtok.Text + prevtok.Length ) - ( sptr )start.Text;
			// <Attribute> ( ... )
		}
	}

	if ( len > 0 )
	{
		StrC attribute_txt = { len, start.Text };
		parser_pop(& Context);

		String name_stripped = parser_strip_formatting( attribute_txt, parser_strip_formatting_dont_preserve_newlines );

		Code result     = make_code();
		result->Type    = CT_PlatformAttributes;
		result->Name    = get_cached_string( string_to_strc(name_stripped) );
		result->Content = result->Name;
		// result->Token   =

		return ( CodeAttributes )result;
	}

	parser_pop(& Context);
	return NullCode;
}

internal
Code parse_class_struct( TokType which, bool inplace_def )
{
	if ( which != Tok_Decl_Class && which != Tok_Decl_Struct )
	{
		log_failure( "Error, expected class or struct, not %s\n%s", toktype_to_str( which ), parser_to_string(Context) );
		return InvalidCode;
	}

	Token name = { nullptr, 0, Tok_Invalid };

	AccessSpec     access     = AccessSpec_Default;
	CodeTypename   parent     = { nullptr };
	CodeBody       body       = { nullptr };
	CodeAttributes attributes = { nullptr };
	ModuleFlag     mflags     = ModuleFlag_None;

	Code result = InvalidCode;

	if ( check(Tok_Module_Export) )
	{
		mflags = ModuleFlag_Export;
		eat( Tok_Module_Export );
	}
	// <ModuleFlags>

	eat( which );
	// <ModuleFlags> <class/struct>

	attributes = parse_attributes();
	// <ModuleFlags> <class/struct> <Attributes>

	if ( check( Tok_Identifier ) )
	{
		name = parse_identifier(nullptr);
		Context.Scope->Name = name;
	}
	// <ModuleFlags> <class/struct> <Attributes> <Name>

	local_persist
	char interface_arr_mem[ kilobytes(4) ] = {0};
	Array(CodeTypename) interfaces; {
		Arena arena = arena_init_from_memory( interface_arr_mem, kilobytes(4) );
		interfaces  = array_init_reserve(CodeTypename, arena_allocator_info(& arena), 4 );
	}

	// TODO(Ed) : Make an AST_DerivedType, we'll store any arbitary derived type into there as a linear linked list of them.
	if ( check( Tok_Assign_Classifer ) )
	{
		eat( Tok_Assign_Classifer );
		// <ModuleFlags> <class/struct> <Attributes> <Name> :

		if ( tok_is_access_specifier(currtok) )
		{
			access = tok_to_access_specifier(currtok);
			// <ModuleFlags> <class/struct> <Attributes> <Name> : <Access Specifier>
			eat( currtok.Type );
		}

		Token parent_tok = parse_identifier(nullptr);
		parent = def_type( tok_to_str(parent_tok) );
		// <ModuleFlags> <class/struct> <Attributes> <Name> : <Access Specifier> <Parent/Interface Name>

		while ( check(Tok_Comma) )
		{
			eat( Tok_Comma );
			// <ModuleFlags> <class/struct> <Attributes> <Name> : <Access Specifier> <Name>,

			if ( tok_is_access_specifier(currtok) )
			{
				eat(currtok.Type);
			}
			Token interface_tok = parse_identifier(nullptr);

			array_append( interfaces, def_type( tok_to_str(interface_tok) ) );
			// <ModuleFlags> <class/struct> <Attributes> <Name> : <Access Specifier> <Name>, ...
		}
	}

	if ( check( Tok_BraceCurly_Open ) )
	{
		body = parse_class_struct_body( which, name );
	}
	// <ModuleFlags> <class/struct> <Attributes> <Name> : <Access Specifier> <Name>, ... { <Body> }

	CodeComment inline_cmt = NullCode;
	if ( ! inplace_def )
	{
		Token stmt_end = currtok;
		eat( Tok_Statement_End );
		// <ModuleFlags> <class/struct> <Attributes> <Name> : <Access Specifier> <Name>, ... { <Body> };

		if ( currtok_noskip.Type == Tok_Comment && currtok_noskip.Line == stmt_end.Line )
			inline_cmt = parse_comment();
		// <ModuleFlags> <class/struct> <Attributes> <Name> : <Access Specifier> <Name>, ... { <Body> }; <InlineCmt>
	}

	if ( which == Tok_Decl_Class )
		result = cast(Code, def_class( tok_to_str(name), def_assign( body, parent, access, attributes, nullptr, 0, mflags ) ));

	else
		result = cast(Code, def_struct( tok_to_str(name), def_assign( body, (CodeTypename)parent, access, attributes, nullptr, 0, mflags ) ));


	if ( inline_cmt )
		result->InlineCmt = cast(Code, inline_cmt);

	array_free(interfaces);
	return result;
}

internal neverinline
CodeBody parse_class_struct_body( TokType which, Token name )
{
	push_scope();

	eat( Tok_BraceCurly_Open );
	// {

	CodeBody
	result = (CodeBody) make_code();

	if ( which == Tok_Decl_Class )
		result->Type = CT_Class_Body;

	else
		result->Type = CT_Struct_Body;

	while ( left && currtok_noskip.Type != Tok_BraceCurly_Close )
	{
		Code           member     = Code_Invalid;
		CodeAttributes attributes = { nullptr };
		CodeSpecifiers specifiers = { nullptr };

		bool expects_function = false;

		// Context.Scope->Start = currtok_noskip;

		if ( currtok_noskip.Type == Tok_Preprocess_Hash )
			eat( Tok_Preprocess_Hash );

		b32 macro_found = true;

		switch ( currtok_noskip.Type )
		{
			case Tok_Statement_End:
			{
				// TODO(Ed): Convert this to a general warning procedure
				log_fmt("Dangling end statement found %S\n", tok_to_string(currtok_noskip));
				eat( Tok_Statement_End );
				continue;
			}
			case Tok_NewLine:
				member = fmt_newline;
				eat( Tok_NewLine );
			break;

			case Tok_Comment:
				member = cast(Code, parse_comment());
			break;

			case Tok_Access_Public:
				member = access_public;
				eat( Tok_Access_Public );
				eat( Tok_Assign_Classifer );
				// public:
			break;

			case Tok_Access_Protected:
				member = access_protected;
				eat( Tok_Access_Protected );
				eat( Tok_Assign_Classifer );
				// protected:
			break;

			case Tok_Access_Private:
				member = access_private;
				eat( Tok_Access_Private );
				eat( Tok_Assign_Classifer );
				// private:
			break;

			case Tok_Decl_Class:
				member = parse_complicated_definition( Tok_Decl_Class );
				// class
			break;

			case Tok_Decl_Enum:
				member = parse_complicated_definition( Tok_Decl_Enum );
				// enum
			break;

			case Tok_Decl_Friend:
				member = cast(Code, parser_parse_friend());
				// friend
			break;

			case Tok_Decl_Operator:
				member = cast(Code, parser_parse_operator_cast(NullCode));
				// operator <Type>()
			break;

			case Tok_Decl_Struct:
				member = parse_complicated_definition( Tok_Decl_Struct );
				// struct
			break;

			case Tok_Decl_Template:
				member = cast(Code, parser_parse_template());
				// template< ... >
			break;

			case Tok_Decl_Typedef:
				member = cast(Code, parser_parse_typedef());
				// typedef
			break;

			case Tok_Decl_Union:
				member = parse_complicated_definition( Tok_Decl_Union );
				// union
			break;

			case Tok_Decl_Using:
				member = cast(Code, parser_parse_using());
				// using
			break;

			case Tok_Operator:
				//if ( currtok.Text[0] != '~' )
				//{
				//	log_failure( "Operator token found in global body but not destructor unary negation\n%s", to_string(Context) );
				//	return InvalidCode;
				//}

				member = cast(Code, parser_parse_destructor(NullCode));
				// ~<Name>()
			break;

			case Tok_Preprocess_Define:
				member = cast(Code, parse_define());
				// #define
			break;

			case Tok_Preprocess_Include:
				member = cast(Code, parse_include());
				// #include
			break;

			case Tok_Preprocess_If:
			case Tok_Preprocess_IfDef:
			case Tok_Preprocess_IfNotDef:
			case Tok_Preprocess_ElIf:
				member = cast(Code, parse_preprocess_cond());
				// #<Condition>
			break;

			case Tok_Preprocess_Else:
				member = cast(Code, preprocess_else);
				eat( Tok_Preprocess_Else );
				// #else
			break;

			case Tok_Preprocess_EndIf:
				member = cast(Code, preprocess_endif);
				eat( Tok_Preprocess_EndIf );
				// #endif
			break;

			case Tok_Preprocess_Macro:
				// <Macro>
				macro_found = true;
				goto Preprocess_Macro_Bare_In_Body;
			break;

			case Tok_Preprocess_Pragma:
				member = cast(Code, parse_pragma());
				// #pragma
			break;

			case Tok_Preprocess_Unsupported:
				member = cast(Code, parse_simple_preprocess( Tok_Preprocess_Unsupported, parser_consume_braces ));
				// #<UNKNOWN>
			break;

			case Tok_StaticAssert:
				member = parse_static_assert();
				// static_assert
			break;

			case Tok_Attribute_Open:
			case Tok_Decl_GNU_Attribute:
			case Tok_Decl_MSVC_Attribute:
		#define Entry( attribute, str ) case attribute:
			GEN_DEFINE_ATTRIBUTE_TOKENS
		#undef Entry
			{
				attributes = parse_attributes();
				// <Attributes>
			}
			//! Fallthrough intended
			case Tok_Spec_Consteval:
			case Tok_Spec_Constexpr:
			case Tok_Spec_Constinit:
			case Tok_Spec_Explicit:
			case Tok_Spec_ForceInline:
			case Tok_Spec_Inline:
			case Tok_Spec_Mutable:
			case Tok_Spec_NeverInline:
			case Tok_Spec_Static:
			case Tok_Spec_Volatile:
			case Tok_Spec_Virtual:
			{
				Specifier specs_found[16] = { Spec_NumSpecifiers };
				s32        NumSpecifiers = 0;

				while ( left && tok_is_specifier(currtok) )
				{
					Specifier spec = strc_to_specifier( tok_to_str(currtok) );

					b32 ignore_spec = false;

					switch ( spec )
					{
						case Spec_Constexpr:
						case Spec_Constinit:
						case Spec_Explicit:
						case Spec_Inline:
						case Spec_ForceInline:
						case Spec_Mutable:
						case Spec_NeverInline:
						case Spec_Static:
						case Spec_Volatile:
						case Spec_Virtual:
						break;

						case Spec_Consteval:
							expects_function = true;
						break;

						case Spec_Const :
							ignore_spec = true;
						break;

						default:
							log_failure( "Invalid specifier %s for variable\n%s", spec_to_str(spec), parser_to_string(Context) );
							parser_pop(& Context);
							return InvalidCode;
					}

					// Every specifier after would be considered part of the type type signature
					if (ignore_spec)
						break;

					specs_found[NumSpecifiers] = spec;
					NumSpecifiers++;
					eat( currtok.Type );
				}

				if ( NumSpecifiers )
				{
					specifiers = def_specifiers( NumSpecifiers, specs_found );
				}
				// <Attributes> <Specifiers>

				if ( tok_is_attribute(currtok) )
				{
					// Unfortuantely Unreal has code where there is attirbutes before specifiers
					CodeAttributes more_attributes = parse_attributes();

					if ( attributes )
					{
						String fused = string_make_reserve( GlobalAllocator, attributes->Content.Len + more_attributes->Content.Len );
						string_append_fmt( & fused, "%S %S", attributes->Content, more_attributes->Content );

						StrC attrib_name = { string_length(fused), fused };
						attributes->Name    = get_cached_string( attrib_name );
						attributes->Content = attributes->Name;
						// <Attributes> <Specifiers> <Attributes>
					}

					attributes = more_attributes;
				}

				if ( currtok.Type == Tok_Operator && currtok.Text[0] == '~' )
				{
					member = cast(Code, parser_parse_destructor( specifiers ));
					// <Attribute> <Specifiers> ~<Name>()
					break;
				}

				if ( currtok.Type == Tok_Decl_Operator )
				{
					member = cast(Code, parser_parse_operator_cast( specifiers ));
					// <Attributes> <Specifiers> operator <Type>()
					break;
				}
			}
			//! Fallthrough intentional
			case Tok_Identifier:
			case Tok_Spec_Const:
			case Tok_Type_Unsigned:
			case Tok_Type_Signed:
			case Tok_Type_Short:
			case Tok_Type_Long:
			case Tok_Type_bool:
			case Tok_Type_char:
			case Tok_Type_int:
			case Tok_Type_double:
			{
				if ( nexttok.Type == Tok_Capture_Start && name.Length && currtok.Type == Tok_Identifier )
				{
					if ( str_compare_len( name.Text, currtok.Text, name.Length ) == 0 )
					{
						member = cast(Code, parser_parse_constructor( specifiers ));
						// <Attributes> <Specifiers> <Name>()
						break;
					}
				}

				if (macro_found)
				{
				Preprocess_Macro_Bare_In_Body:
					b32 lone_macro = nexttok.Type == Tok_Statement_End || nexttok_noskip.Type == Tok_NewLine;
					if (lone_macro)
					{
						member = cast(Code, parse_simple_preprocess( Tok_Preprocess_Macro, parser_consume_braces ));
						// <Macro>;

						if ( member == Code_Invalid )
						{
							log_failure( "Failed to parse member\n%s", parser_to_string(Context) );
							parser_pop(& Context);
							return InvalidCode;
						}
						continue;
					}

					// We have a macro but its most likely behaving as a typename
					// <Macro ...
				}

				member = parse_operator_function_or_variable( expects_function, attributes, specifiers );
				// <Attributes> <Specifiers> operator <Op> ...
				// or
				// <Attributes> <Specifiers> <Name> ...
			}
			break;

			default:
				Token untyped_tok = currtok;

				while ( left && currtok.Type != Tok_BraceCurly_Close )
				{
					untyped_tok.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)untyped_tok.Text;
					eat( currtok.Type );
				}

				member = untyped_str( tok_to_str(untyped_tok) );
				// Something unknown
			break;
		}

		if ( member == Code_Invalid )
		{
			log_failure( "Failed to parse member\n%s", parser_to_string(Context) );
			parser_pop(& Context);
			return InvalidCode;
		}

		body_append(result, member );
	}

	eat( Tok_BraceCurly_Close );
	// { <Members> }
	parser_pop(& Context);
	return result;
}

internal
CodeComment parse_comment()
{
	push_scope();

	CodeComment
	result          = (CodeComment) make_code();
	result->Type    = CT_Comment;
	result->Content = get_cached_string( tok_to_str(currtok_noskip) );
	result->Name    = result->Content;
	// result->Token   = currtok_noskip;
	eat( Tok_Comment );

	parser_pop(& Context);
	return result;
}

internal
Code parse_complicated_definition( TokType which )
{
	push_scope();

	bool is_inplace = false;

	TokArray tokens = Context.Tokens;

	s32 idx         = tokens.Idx;
	s32 level       = 0;
	for ( ; idx < array_num(tokens.Arr); idx++ )
	{
		if ( tokens.Arr[ idx ].Type == Tok_BraceCurly_Open )
			level++;

		if ( tokens.Arr[ idx ].Type == Tok_BraceCurly_Close )
			level--;

		if ( level == 0 && tokens.Arr[ idx ].Type == Tok_Statement_End )
			break;
	}

	if ( ( idx - 2 ) == tokens.Idx )
	{
		// Its a forward declaration only
		Code result = parse_forward_or_definition( which, is_inplace );
		// <class, enum, struct, or union> <Name>;
		parser_pop(& Context);
		return result;
	}

	Token tok = tokens.Arr[ idx - 1 ];
	if ( tok_is_specifier(tok) && spec_is_trailing( strc_to_specifier( tok_to_str(tok))) )
	{
		// <which> <type_identifier>(...) <specifier> ...;

		s32   spec_idx = idx - 1;
		Token spec     = tokens.Arr[spec_idx];
		while ( tok_is_specifier(spec) && spec_is_trailing( strc_to_specifier( tok_to_str(spec))) )
		{
			-- spec_idx;
			spec = tokens.Arr[spec_idx];
		}

		if ( tokens.Arr[spec_idx].Type == Tok_Capture_End )
		{
			// Forward declaration with trailing specifiers for a procedure
			tok = tokens.Arr[spec_idx];

			Code result = parse_operator_function_or_variable( false, NullCode, NullCode );
			// <Attributes> <Specifiers> <ReturnType/ValueType> <operator <Op>, or Name> ...
			parser_pop(& Context);
			return result;
		}

		log_failure( "Unsupported or bad member definition after %s declaration\n%s", toktype_to_str(which), parser_to_string(Context) );
		parser_pop(& Context);
		return InvalidCode;
	}
	if ( tok.Type == Tok_Identifier )
	{
		tok = tokens.Arr[ idx - 2 ];
		bool is_indirection = tok.Type == Tok_Ampersand || tok.Type == Tok_Star;
		bool ok_to_parse    = false;

		if ( tok.Type == Tok_BraceCurly_Close )
		{
			// Its an inplace definition
			// <which> <type_identifier> { ... } <identifier>;
			ok_to_parse = true;
			is_inplace  = true;
		}
		else if ( tok.Type == Tok_Identifier && tokens.Arr[ idx - 3 ].Type == which )
		{
			// Its a variable with type ID using <which> namespace.
			// <which> <type_identifier> <identifier>;
			ok_to_parse = true;
		}
		else if ( tok.Type == Tok_Assign_Classifer
		&& (	( tokens.Arr[idx - 5].Type == which && tokens.Arr[idx - 4].Type == Tok_Decl_Class )
			||	( tokens.Arr[idx - 4].Type == which))
		)
		{
			// Its a forward declaration of an enum
			// <enum>         <type_identifier> : <identifier>;
			// <enum> <class> <type_identifier> : <identifier>;
			ok_to_parse = true;
			Code result = cast(Code, parser_parse_enum( ! parser_inplace_def));
			parser_pop(& Context);
			return result;
		}
		else if ( is_indirection )
		{
			// Its a indirection type with type ID using struct namespace.
			// <which> <type_identifier>* <identifier>;
			ok_to_parse = true;
		}

		if ( ! ok_to_parse )
		{
			log_failure( "Unsupported or bad member definition after %s declaration\n%s", toktype_to_str(which), parser_to_string(Context) );
			parser_pop(& Context);
			return InvalidCode;
		}

		Code result = parse_operator_function_or_variable( false, NullCode, NullCode );
		// <Attributes> <Specifiers> <ReturnType/ValueType> <operator <Op>, or Name> ...
		parser_pop(& Context);
		return result;
	}
	else if ( tok.Type >= Tok_Type_Unsigned && tok.Type <= Tok_Type_MS_W64 )
	{
		tok = tokens.Arr[ idx - 2 ];

		if ( tok.Type != Tok_Assign_Classifer
		|| (	( tokens.Arr[idx - 5].Type != which && tokens.Arr[idx - 4].Type != Tok_Decl_Class )
			&&	( tokens.Arr[idx - 4].Type != which))
		)
		{
			log_failure( "Unsupported or bad member definition after %s declaration\n%s", toktype_to_str(which), parser_to_string(Context) );
			parser_pop(& Context);
			return InvalidCode;
		}

		// Its a forward declaration of an enum class
		// <enum>         <type_identifier> : <identifier>;
		// <enum> <class> <type_identifier> : <identifier>;
		Code result = cast(Code, parser_parse_enum( ! parser_inplace_def));
		parser_pop(& Context);
		return result;
	}
	else if ( tok.Type == Tok_BraceCurly_Close )
	{
		// Its a definition
		Code result = parse_forward_or_definition( which, is_inplace );
		// <which> { ... };
		parser_pop(& Context);
		return result;
	}
	else if ( tok.Type == Tok_BraceSquare_Close )
	{
		// Its an array definition
		Code result = parse_operator_function_or_variable( false, NullCode, NullCode );
		// <which> <type_identifier> <identifier> [ ... ];
		parser_pop(& Context);
		return result;
	}
	else
	{
		log_failure( "Unsupported or bad member definition after %s declaration\n%S", toktype_to_str(which).Ptr, parser_to_string(Context) );
		parser_pop(& Context);
		return InvalidCode;
	}
}

internal inline
CodeDefine parse_define()
{
	push_scope();
	eat( Tok_Preprocess_Define );
	// #define

	CodeDefine
	define = (CodeDefine) make_code();
	define->Type = CT_Preprocess_Define;

	if ( ! check( Tok_Identifier ) )
	{
		log_failure( "Error, expected identifier after #define\n%s", parser_to_string(Context) );
		parser_pop(& Context);
		return InvalidCode;
	}

	Context.Scope->Name = currtok;
	define->Name = get_cached_string( tok_to_str(currtok) );
	eat( Tok_Identifier );
	// #define <Name>

	if ( ! check( Tok_Preprocess_Content ))
	{
		log_failure( "Error, expected content after #define %s\n%s", define->Name, parser_to_string(Context) );
		parser_pop(& Context);
		return InvalidCode;
	}

	if ( currtok.Length == 0 )
	{
		define->Content = get_cached_string( tok_to_str(currtok) );
		eat( Tok_Preprocess_Content );
		// #define <Name> <Content>

		parser_pop(& Context);
		return define;
	}

	define->Content = get_cached_string( string_to_strc( parser_strip_formatting( tok_to_str(currtok), parser_strip_formatting_dont_preserve_newlines )) );
	eat( Tok_Preprocess_Content );
	// #define <Name> <Content>

	parser_pop(& Context);
	return define;
}

internal inline
Code parse_assignment_expression()
{
	Code expr = { nullptr };

	eat( Tok_Operator );
	// <Attributes> <Specifiers> <ValueType> <Name> =

	Token expr_tok = currtok;

	if ( currtok.Type == Tok_Statement_End && currtok.Type != Tok_Comma )
	{
		log_failure( "Expected expression after assignment operator\n%s", parser_to_string(Context) );
		parser_pop(& Context);
		return InvalidCode;
	}

	s32 level = 0;
	while ( left && currtok.Type != Tok_Statement_End && (currtok.Type != Tok_Comma || level > 0) )
	{
		if (currtok.Type == Tok_BraceCurly_Open )
			level++;
		if (currtok.Type == Tok_BraceCurly_Close )
			level--;
		if (currtok.Type == Tok_Capture_Start)
			level++;
		else if (currtok.Type == Tok_Capture_End)
			level--;

		eat( currtok.Type );
	}

	expr_tok.Length = ( ( sptr )currtok.Text + currtok.Length ) - ( sptr )expr_tok.Text - 1;
	expr            = untyped_str( tok_to_str(expr_tok) );
	// = <Expression>
	return expr;
}

internal inline
Code parse_forward_or_definition( TokType which, bool is_inplace )
{
	Code result = InvalidCode;

	switch ( which )
	{
		case Tok_Decl_Class:
			result = cast(Code, parser_parse_class( is_inplace ));
			return result;

		case Tok_Decl_Enum:
			result = cast(Code, parser_parse_enum( is_inplace ));
			return result;

		case Tok_Decl_Struct:
			result = cast(Code, parser_parse_struct( is_inplace ));
			return result;

		case Tok_Decl_Union:
			result = cast(Code, parser_parse_union( is_inplace ));
			return result;

		default:
			log_failure( "Error, wrong token type given to parse_complicated_definition "
				"(only supports class, enum, struct, union) \n%s"
				, parser_to_string(Context) );

			return InvalidCode;
	}
}

// Function parsing is handled in multiple places because its initial signature is shared with variable parsing
internal inline
CodeFn parse_function_after_name(
	  ModuleFlag     mflags
	, CodeAttributes attributes
	, CodeSpecifiers specifiers
	, CodeTypename   ret_type
	, Token          name
)
{
	push_scope();
	CodeParam params = parse_params(parser_use_parenthesis);
	// <Attributes> <Specifiers> <ReturnType> <Name> ( <Parameters> )

	// TODO(Ed), Review old comment : These have to be kept separate from the return type's specifiers.
	while ( left && tok_is_specifier(currtok) )
	{
		if ( specifiers == nullptr )
		{
			specifiers = def_specifier( strc_to_specifier( tok_to_str(currtok)) );
			eat( currtok.Type );
			continue;
		}

		specifiers_append(specifiers, strc_to_specifier( tok_to_str(currtok)) );
		eat( currtok.Type );
	}
	// <Attributes> <Specifiers> <ReturnType> <Name> ( <Paraemters> ) <Specifiers>

	CodeBody    body       = NullCode;
	CodeComment inline_cmt = NullCode;
	if ( check( Tok_BraceCurly_Open ) )
	{
		body = cast(CodeBody, parse_function_body());
		if ( cast(Code, body) == Code_Invalid )
		{
			parser_pop(& Context);
			return InvalidCode;
		}
		// <Attributes> <Specifiers> <ReturnType> <Name> ( <Paraemters> ) <Specifiers> { <Body> }
	}
	else if ( check(Tok_Operator) && currtok.Text[0] == '=' )
	{
		eat(Tok_Operator);
		specifiers_append(specifiers, Spec_Pure );

		eat( Tok_Number);
		Token stmt_end = currtok;
		eat( Tok_Statement_End );
		// <Attributes> <Specifiers> <ReturnType> <Name> ( <Paraemters> ) <Specifiers> = 0;

		if ( currtok_noskip.Type == Tok_Comment && currtok_noskip.Line == stmt_end.Line )
			inline_cmt = parse_comment();
		// <Attributes> <Specifiers> <ReturnType> <Name> ( <Paraemters> ) <Specifiers>; <InlineCmt>
	}
	else
	{
		Token stmt_end = currtok;
		eat( Tok_Statement_End );
		// <Attributes> <Specifiers> <ReturnType> <Name> ( <Paraemters> ) <Specifiers>;

		if ( currtok_noskip.Type == Tok_Comment && currtok_noskip.Line == stmt_end.Line )
			inline_cmt = parse_comment();
			// <Attributes> <Specifiers> <ReturnType> <Name> ( <Paraemters> ) <Specifiers>; <InlineCmt>
	}

	String
	name_stripped = string_make_strc( GlobalAllocator, tok_to_str(name) );
	strip_space(name_stripped);

	CodeFn
	result              = (CodeFn) make_code();
	result->Name        = get_cached_string( string_to_strc(name_stripped) );
	result->ModuleFlags = mflags;

	if ( body )
	{
		switch ( body->Type )
		{
			case CT_Function_Body:
			case CT_Untyped:
				break;

			default:
			{
				log_failure("Body must be either of Function_Body or Untyped type, %s\n%s", code_debug_str(body), parser_to_string(Context));
				parser_pop(& Context);
				return InvalidCode;
			}
		}

		result->Type = CT_Function;
		result->Body = body;
	}
	else
	{
		result->Type = CT_Function_Fwd;
	}

	if ( attributes )
		result->Attributes = attributes;

	if ( specifiers )
		result->Specs = specifiers;

	result->ReturnType = ret_type;

	if ( params )
		result->Params = params;

	if ( inline_cmt )
		result->InlineCmt = inline_cmt;

	parser_pop(& Context);
	return result;
}

internal
Code parse_function_body()
{
	push_scope();

	eat( Tok_BraceCurly_Open );

	CodeBody
	result = (CodeBody) make_code();
	result->Type = CT_Function_Body;

	// TODO : Support actual parsing of function body
	Token start = currtok_noskip;

	s32 level = 0;
	while ( left && ( currtok_noskip.Type != Tok_BraceCurly_Close || level > 0 ) )
	{
		if ( currtok_noskip.Type == Tok_BraceCurly_Open )
			level++;

		else if ( currtok_noskip.Type == Tok_BraceCurly_Close && level > 0 )
			level--;

		eat( currtok_noskip.Type );
	}

	Token past = prevtok;

	s32 len = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)start.Text;

	if ( len > 0 )
	{
		StrC str = { len, start.Text };
		body_append( result, cast(Code, def_execution( str )) );
	}

	eat( Tok_BraceCurly_Close );

	parser_pop(& Context);
	return cast(Code, result);
}

internal neverinline
CodeBody parse_global_nspace( CodeType which )
{
	push_scope();

	if ( which != CT_Namespace_Body && which != CT_Global_Body && which != CT_Export_Body && which != CT_Extern_Linkage_Body )
		return InvalidCode;

	if ( which != CT_Global_Body )
		eat( Tok_BraceCurly_Open );
		// {

	CodeBody
	result = (CodeBody) make_code();
	result->Type = which;

	while ( left && currtok_noskip.Type != Tok_BraceCurly_Close )
	{
		Code           member     = Code_Invalid;
		CodeAttributes attributes = { nullptr };
		CodeSpecifiers specifiers = { nullptr };

		bool expects_function = false;

		// Context.Scope->Start = currtok_noskip;

		if ( currtok_noskip.Type == Tok_Preprocess_Hash )
			eat( Tok_Preprocess_Hash );

		b32 macro_found = false;

		switch ( currtok_noskip.Type )
		{
			case Tok_Comma:
			{
				log_failure("Dangling comma found: %S\nContext:\n%S", tok_to_string(currtok), parser_to_string(Context));
				parser_pop( & Context);
				return InvalidCode;
			}
			break;
			case Tok_Statement_End:
			{
				// TODO(Ed): Convert this to a general warning procedure
				log_fmt("Dangling end statement found %S\n", tok_to_string(currtok_noskip));
				eat( Tok_Statement_End );
				continue;
			}
			case Tok_NewLine:
				// Empty lines are auto skipped by Tokens.current()
				member = fmt_newline;
				eat( Tok_NewLine );
			break;

			case Tok_Comment:
				member = cast(Code, parse_comment());
			break;

			case Tok_Decl_Class:
				member = parse_complicated_definition( Tok_Decl_Class );
				// class
			break;

			case Tok_Decl_Enum:
				member = parse_complicated_definition( Tok_Decl_Enum );
				// enum
			break;

			case Tok_Decl_Extern_Linkage:
				if ( which == CT_Extern_Linkage_Body )
					log_failure( "Nested extern linkage\n%s", parser_to_string(Context) );

				member = cast(Code, parser_parse_extern_link());
				// extern "..." { ... }
			break;

			case Tok_Decl_Namespace:
				member = cast(Code, parser_parse_namespace());
				// namespace <Name> { ... }
			break;

			case Tok_Decl_Struct:
				member = parse_complicated_definition( Tok_Decl_Struct );
				// struct ...
			break;

			case Tok_Decl_Template:
				member = cast(Code, parser_parse_template());
				// template<...> ...
			break;

			case Tok_Decl_Typedef:
				member = cast(Code, parser_parse_typedef());
				// typedef ...
			break;

			case Tok_Decl_Union:
				member = parse_complicated_definition( Tok_Decl_Union );
				// union ...
			break;

			case Tok_Decl_Using:
				member = cast(Code, parser_parse_using());
				// using ...
			break;

			case Tok_Preprocess_Define:
				member = cast(Code, parse_define());
				// #define ...
			break;

			case Tok_Preprocess_Include:
				member = cast(Code, parse_include());
				// #include ...
			break;

			case Tok_Preprocess_If:
			case Tok_Preprocess_IfDef:
			case Tok_Preprocess_IfNotDef:
			case Tok_Preprocess_ElIf:
				member = cast(Code, parse_preprocess_cond());
				// #<Conditional> ...
			break;

			case Tok_Preprocess_Else:
				member = cast(Code, preprocess_else);
				eat( Tok_Preprocess_Else );
				// #else
			break;

			case Tok_Preprocess_EndIf:
				member = cast(Code, preprocess_endif);
				eat( Tok_Preprocess_EndIf );
				// #endif
			break;

			case Tok_Preprocess_Macro: {
				// <Macro>
				macro_found = true;
				goto Preprocess_Macro_Bare_In_Body;
			}
			break;

			case Tok_Preprocess_Pragma: {
				member = cast(Code, parse_pragma());
				// #pragma ...
			}
			break;

			case Tok_Preprocess_Unsupported: {
				member = cast(Code, parse_simple_preprocess( Tok_Preprocess_Unsupported, parser_consume_braces ));
				// #<UNSUPPORTED> ...
			}
			break;

			case Tok_StaticAssert: {
				member = cast(Code, parse_static_assert());
				// static_assert( <Conditional Expression>, ... );
			}
			break;

			case Tok_Module_Export: {
				if ( which == CT_Export_Body )
					log_failure( "Nested export declaration\n%s", parser_to_string(Context) );

				member = cast(Code, parser_parse_export_body());
				// export { ... }
			}
			break;

			case Tok_Module_Import: {
				not_implemented( context );
				// import ...
			}
			//! Fallthrough intentional
			case Tok_Attribute_Open:
			case Tok_Decl_GNU_Attribute:
			case Tok_Decl_MSVC_Attribute:
		#define Entry( attribute, str ) case attribute:
			GEN_DEFINE_ATTRIBUTE_TOKENS
		#undef Entry
			{
				attributes = parse_attributes();
				// <Attributes>
			}
			//! Fallthrough intentional
			case Tok_Spec_Consteval:
			case Tok_Spec_Constexpr:
			case Tok_Spec_Constinit:
			case Tok_Spec_Extern:
			case Tok_Spec_ForceInline:
			case Tok_Spec_Global:
			case Tok_Spec_Inline:
			case Tok_Spec_Internal_Linkage:
			case Tok_Spec_NeverInline:
			case Tok_Spec_Static:
			{
				Specifier specs_found[16] = { Spec_NumSpecifiers };
				s32        NumSpecifiers = 0;

				while ( left && tok_is_specifier(currtok) )
				{
					Specifier spec = strc_to_specifier( tok_to_str(currtok) );

					bool ignore_spec = false;

					switch ( spec )
					{
						case Spec_Constexpr:
						case Spec_Constinit:
						case Spec_ForceInline:
						case Spec_Global:
						case Spec_External_Linkage:
						case Spec_Internal_Linkage:
						case Spec_Inline:
						case Spec_Mutable:
						case Spec_NeverInline:
						case Spec_Static:
						case Spec_Volatile:
						break;

						case Spec_Consteval:
							expects_function = true;
						break;

						case Spec_Const:
							ignore_spec = true;
						break;

						default:
							StrC spec_str = spec_to_str(spec);

							log_failure( "Invalid specifier %.*s for variable\n%s", spec_str.Len, spec_str, parser_to_string(Context) );
							parser_pop(& Context);
							return InvalidCode;
					}

					if (ignore_spec)
						break;

					specs_found[NumSpecifiers] = spec;
					NumSpecifiers++;
					eat( currtok.Type );
				}

				if ( NumSpecifiers )
				{
					specifiers = def_specifiers( NumSpecifiers, specs_found );
				}
				// <Attributes> <Specifiers>
			}
			//! Fallthrough intentional
			case Tok_Identifier:
			case Tok_Spec_Const:
			case Tok_Type_Long:
			case Tok_Type_Short:
			case Tok_Type_Signed:
			case Tok_Type_Unsigned:
			case Tok_Type_bool:
			case Tok_Type_char:
			case Tok_Type_double:
			case Tok_Type_int:
			{
				// This s only in a scope so that Preprocess_Macro_Bare_In_Body works without microsoft extension warnings
				{
					Code constructor_destructor = parse_global_nspace_constructor_destructor( specifiers );
					// Possible constructor implemented at global file scope.
					if ( constructor_destructor )
					{
						member = constructor_destructor;
						break;
					}

					bool found_operator_cast_outside_class_implmentation = false;
					s32  idx = Context.Tokens.Idx;

					for ( ; idx < array_num(Context.Tokens.Arr); idx++ )
					{
						Token tok = Context.Tokens.Arr[ idx ];

						if ( tok.Type == Tok_Identifier )
						{
							idx++;
							tok = Context.Tokens.Arr[ idx ];
							if ( tok.Type == Tok_Access_StaticSymbol )
								continue;

							break;
						}

						if ( tok.Type == Tok_Decl_Operator )
							found_operator_cast_outside_class_implmentation = true;

						break;
					}

					if ( found_operator_cast_outside_class_implmentation )
					{
						member = cast(Code, parser_parse_operator_cast( specifiers ));
						// <Attributes> <Specifiers> <Name>::operator <Type>() { ... }
						break;
					}
				}

				if (macro_found)
				{
				Preprocess_Macro_Bare_In_Body:
					b32 lone_macro = nexttok.Type == Tok_Statement_End || nexttok_noskip.Type == Tok_NewLine;
					if (lone_macro)
					{
						member = parse_simple_preprocess( Tok_Preprocess_Macro, parser_consume_braces );
						// <Macro>;

						if ( member == Code_Invalid )
						{
							log_failure( "Failed to parse member\n%s", parser_to_string(Context) );
							parser_pop(& Context);
							return InvalidCode;
						}
						goto Member_Resolved_To_Lone_Macro;
					}

					// We have a macro but its most likely behaving as a typename
					// <Macro ...
				}

				member = parse_operator_function_or_variable( expects_function, attributes, specifiers );
				// <Attributes> <Specifiers> ...
			}
		}

	Member_Resolved_To_Lone_Macro:
		if ( member == Code_Invalid )
		{
			log_failure( "Failed to parse member\nToken: %S\nContext:\n%S", tok_to_string(currtok_noskip), parser_to_string(Context) );
			parser_pop(& Context);
			return InvalidCode;
		}

		// log_fmt("Global Body Member: %s", member->debug_str());
		body_append(result, member );
	}

	if ( which != CT_Global_Body )
		eat( Tok_BraceCurly_Close );
		// { <Body> }

	parser_pop(& Context);
	return result;
}

internal inline
Code parse_global_nspace_constructor_destructor( CodeSpecifiers specifiers )
{
	Code result = { nullptr };

	/*
		To check if a definition is for a constructor we can go straight to the opening parenthesis for its parameters
		From There we work backwards to see if we come across two identifiers with the same name between an member access
		:: operator, there can be template parameters on the left of the :: so we ignore those.
		Whats important is that	its back to back.

		This has multiple possible faults. What we parse using this method may not filter out if something has a "return type"
		This is bad since technically you could have a namespace nested into another namespace with the same name.
		If this awful pattern is done the only way to distiguish with this coarse parse is to know there is no return type defined.

		TODO(Ed): We could fix this by attempting to parse a type, but we would have to have a way to have it soft fail and rollback.
	*/
	TokArray tokens = Context.Tokens;

	s32   idx = tokens.Idx;
	Token nav = tokens.Arr[ idx ];
	for ( ; idx < array_num(tokens.Arr); idx++, nav = tokens.Arr[ idx ] )
	{
		if ( nav.Text[0] == '<' )
		{
			// Skip templated expressions as they mey have expressions with the () operators
			s32 capture_level  = 0;
			s32 template_level = 0;
			for ( ; idx < array_num(tokens.Arr); idx++, nav = tokens.Arr[idx] )
			{
				if (nav.Text[ 0 ] == '<')
					++ template_level;

				if (nav.Text[ 0 ] == '>')
					-- template_level;
				if (nav.Type == Tok_Operator && nav.Text[1] == '>')
					-- template_level;

				if ( nav.Type == Tok_Capture_Start)
				{
					if (template_level != 0 )
						++ capture_level;
					else
						break;
				}

				if ( template_level != 0 && nav.Type == Tok_Capture_End)
					-- capture_level;
			}
		}

		if ( nav.Type == Tok_Capture_Start )
			break;
	}

	-- idx;
	Token tok_right = tokens.Arr[idx];
	Token tok_left  = NullToken;

	if (tok_right.Type != Tok_Identifier)
	{
		// We're not dealing with a constructor if there is no identifier right before the opening of a parameter's scope.
		return result;
	}

	-- idx;
	tok_left = tokens.Arr[idx];
	// <Attributes> <Specifiers> ... <Identifier>

	bool possible_destructor = false;
	if ( tok_left.Type == Tok_Operator && tok_left.Text[0] == '~')
	{
		possible_destructor = true;
		-- idx;
		tok_left = tokens.Arr[idx];
	}

	if ( tok_left.Type != Tok_Access_StaticSymbol )
		return result;

	-- idx;
	tok_left = tokens.Arr[idx];
	// <Attributes> <Specifiers> ... :: <Identifier>

	// We search toward the left until we find the next valid identifier
	s32 capture_level  = 0;
	s32 template_level = 0;
	while ( idx != tokens.Idx )
	{
		if (tok_left.Text[ 0 ] == '<')
			++ template_level;

		if (tok_left.Text[ 0 ] == '>')
			-- template_level;
		if (tok_left.Type == Tok_Operator && tok_left.Text[1] == '>')
			-- template_level;

		if ( template_level != 0 && tok_left.Type == Tok_Capture_Start)
			++ capture_level;

		if ( template_level != 0 && tok_left.Type == Tok_Capture_End)
			-- capture_level;

		if ( capture_level == 0 && template_level == 0  && tok_left.Type == Tok_Identifier )
			break;

		-- idx;
		tok_left = tokens.Arr[idx];
	}

	bool is_same = str_compare_len( tok_right.Text, tok_left.Text, tok_right.Length ) == 0;
	if (tok_left.Type == Tok_Identifier && is_same)
	{
		// We have found the pattern we desired
		if (possible_destructor)
		{
			// <Name> :: ~<Name> (
			result = cast(Code, parser_parse_destructor( specifiers ));
		}
		else {
			// <Name> :: <Name> (
			result = cast(Code, parser_parse_constructor( specifiers ));
		}
	}

	return result;
}

// TODO(Ed): I want to eventually change the identifier to its own AST type.
// This would allow distinction of the qualifier for a symbol <qualifier>::<nested symboL>
// This would also allow
internal
Token parse_identifier( bool* possible_member_function )
{
	push_scope();

	Token name = currtok;
	Context.Scope->Name = name;
	eat( Tok_Identifier );
	// <Name>

	parse_template_args( & name );
	// <Name><Template Args>

	while ( check( Tok_Access_StaticSymbol ) )
	{
		eat( Tok_Access_StaticSymbol );
		// <Qualifier Name> <Template Args> ::

		Token invalid = { nullptr, 0, Tok_Invalid };
		if ( left == 0 )
		{
			log_failure( "Error, unexpected end of static symbol identifier\n%s", parser_to_string(Context) );
			parser_pop(& Context);
			return invalid;
		}

		if ( currtok.Type == Tok_Operator && currtok.Text[0] == '~' )
		{
			bool is_destructor = strc_are_equal( Context.Scope->Prev->ProcName, txt("parser_parse_destructor"));
			if (is_destructor)
			{
				name.Length = ( ( sptr )prevtok.Text + prevtok.Length ) - ( sptr )name.Text;
				parser_pop(& Context);
				return name;
			}

			log_failure( "Error, had a ~ operator after %S but not a destructor\n%s", toktype_to_str( prevtok.Type ), parser_to_string(Context) );
			parser_pop(& Context);
			return invalid;
		}

		if ( currtok.Type == Tok_Operator && currtok.Text[0] == '*' && currtok.Length == 1 )
		{
			if ( possible_member_function )
				*possible_member_function = true;

			else
			{
				log_failure( "Found a member function pointer identifier but the parsing context did not expect it\n%s", parser_to_string(Context) );
				parser_pop(& Context);
				return invalid;
			}
		}

		if ( currtok.Type != Tok_Identifier )
		{
			log_failure( "Error, expected static symbol identifier, not %s\n%s", toktype_to_str( currtok.Type ), parser_to_string(Context) );
			parser_pop(& Context);
			return invalid;
		}

		name.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)name.Text;
		eat( Tok_Identifier );
		// <Qualifier Name> <Template Args> :: <Name>

		parse_template_args( & name );
		// <Qualifier Name> <Template Args> :: <Name> <Template Args>
	}
	// <Qualifier Name> <Template Args> :: <Name> <Template Args> ...

	parser_pop(& Context);
	return name;
}

internal
CodeInclude parse_include()
{
	push_scope();

	CodeInclude
	include       = (CodeInclude) make_code();
	include->Type = CT_Preprocess_Include;
	eat( Tok_Preprocess_Include );
	// #include

	if ( ! check( Tok_String ))
	{
		log_failure( "Error, expected include string after #include\n%s", parser_to_string(Context) );
		parser_pop(& Context);
		return InvalidCode;
	}

	Context.Scope->Name = currtok;
	include->Content = get_cached_string( tok_to_str(currtok) );
	eat( Tok_String );
	// #include <Path> or "Path"

	parser_pop(& Context);
	return include;
}

internal
CodeOperator parse_operator_after_ret_type(
	  ModuleFlag     mflags
	, CodeAttributes attributes
	, CodeSpecifiers specifiers
	, CodeTypename   ret_type
)
{
	push_scope();

	Token nspace = NullToken;
	if ( check( Tok_Identifier ) )
	{
		nspace = currtok;
		while ( left && currtok.Type == Tok_Identifier )
		{
			eat( Tok_Identifier );

			if ( currtok.Type == Tok_Access_StaticSymbol )
				eat( Tok_Access_StaticSymbol );
		}

		nspace.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)nspace.Text;
	}
	// <ExportFlag> <Attributes> <Specifiers> <ReturnType> <Qualifier::...>

	eat( Tok_Decl_Operator );
	// <ExportFlag> <Attributes> <Specifiers> <ReturnType> <Qualifier::...> operator

	if ( ! left && currtok.Type != Tok_Operator
		&& currtok.Type != Tok_Star
		&& currtok.Type != Tok_Ampersand
		&& currtok.Type != Tok_Ampersand_DBL )
	{
		log_failure( "Expected operator after 'operator' keyword\n%s", parser_to_string(Context) );
		parser_pop(& Context);
		return InvalidCode;
	}

	Context.Scope->Name = currtok;

	bool was_new_or_delete = false;

	Operator op = Op_Invalid;
	switch ( currtok.Text[0] )
	{
		case '+':
		{
			if ( currtok.Text[1] == '=' )
				op = Op_Assign_Add;

			else if ( currtok.Text[1] == '+' )
				op = Op_Increment;

			else
				op = Op_Add;
		}
		break;
		case '-':
		{
			if ( currtok.Text[1] == '>' )
			{
				if ( currtok.Text[2] == '*' )
					op = Op_MemberOfPointer;

				else
					op = Op_MemberOfPointer;

				break;
			}

			else if ( currtok.Text[1] == '=' )
				op = Op_Assign_Subtract;

			else
				op = Op_Subtract;
		}
		break;
		case '*':
		{
			if ( currtok.Text[1] == '=' )
				op = Op_Assign_Multiply;

			else
			{
				Token finder = prevtok;
				while ( finder.Type != Tok_Decl_Operator )
				{
					if ( finder.Type == Tok_Identifier)
					{
						op = Op_Indirection;
						break;
					}
				}

				if ( op == Op_Invalid)
					op = Op_Multiply;
			}
		}
		break;
		case '/':
		{
			if ( currtok.Text[1] == '=' )
				op = Op_Assign_Divide;

			else
				op = Op_Divide;
		}
		break;
		case '%':
		{
			if ( currtok.Text[1] == '=' )
				op = Op_Assign_Modulo;

			else
				op = Op_Modulo;
		}
		break;
		case '&':
		{
			if ( currtok.Text[1] == '=' )
				op = Op_Assign_BAnd;

			else if ( currtok.Text[1] == '&' )
				op = Op_LAnd;

			else
			{


				if ( op == Op_Invalid )
				op = Op_BAnd;
			}
		}
		break;
		case '|':
		{
			if ( currtok.Text[1] == '=' )
				op = Op_Assign_BOr;

			else if ( currtok.Text[1] == '|' )
				op = Op_LOr;

			else
				op = Op_BOr;
		}
		break;
		case '^':
		{
			if ( currtok.Text[1] == '=' )
				op = Op_Assign_BXOr;

			else
				op = Op_BXOr;
		}
		break;
		case '~':
		{
			op = Op_BNot;
		}
		break;
		case '!':
		{
			if ( currtok.Text[1] == '=' )
				op = Op_LNot;

			else
				op = Op_UnaryNot;
		}
		break;
		case '=':
		{
			if ( currtok.Text[1] == '=' )
				op = Op_LEqual;

			else
				op = Op_Assign;
		}
		break;
		case '<':
		{
			if ( currtok.Text[1] == '=' )
				op = Op_LEqual;

			else if ( currtok.Text[1] == '<' )
			{
				if ( currtok.Text[2] == '=' )
					op = Op_Assign_LShift;

				else
					op = Op_LShift;
			}
			else
				op = Op_Lesser;
		}
		break;
		case '>':
		{
			if ( currtok.Text[1] == '=' )
				op = Op_GreaterEqual;

			else if ( currtok.Text[1] == '>' )
			{
				if ( currtok.Text[2] == '=' )
					op = Op_Assign_RShift;

				else
					op = Op_RShift;
			}
			else
				op = Op_Greater;
		}
		break;
		case '(':
		{
			if ( currtok.Text[1] == ')' )
				op = Op_FunctionCall;

			else
				op = Op_Invalid;
		}
		break;
		case '[':
		{
			if ( currtok.Text[1] == ']' )
				op = Op_Subscript;

			else
				op = Op_Invalid;
		}
		break;
		default:
		{
			StrC str_new    = operator_to_str(Op_New);
			StrC str_delete = operator_to_str(Op_Delete);
			if ( str_compare_len( currtok.Text, str_new.Ptr, max(str_new.Len - 1, currtok.Length)) == 0)
			{
				op = Op_New;
				eat( Tok_Identifier );
				was_new_or_delete = true;

				s32 idx = Context.Tokens.Idx + 1;
				{
					while ( Context.Tokens.Arr[ idx ].Type == Tok_NewLine )
						idx++;
				}
				Token next = Context.Tokens.Arr[idx];
				if ( currtok.Type == Tok_Operator && str_compare_len(currtok.Text, "[]", 2) == 0)
				{
					eat(Tok_Operator);
					op = Op_NewArray;
				}
				else if ( currtok.Type == Tok_BraceSquare_Open && next.Type == Tok_BraceSquare_Close)
				{
					eat(Tok_BraceSquare_Open);
					eat(Tok_BraceSquare_Close);
					op = Op_NewArray;
				}
			}
			else if ( str_compare_len( currtok.Text, str_delete.Ptr, max(str_delete.Len - 1, currtok.Length )) == 0)
			{
				op = Op_Delete;
				eat(Tok_Identifier);
				was_new_or_delete = true;

				s32 idx = Context.Tokens.Idx + 1;
				{
					while ( Context.Tokens.Arr[ idx ].Type == Tok_NewLine )
						idx++;
				}
				Token next = Context.Tokens.Arr[idx];
				if ( currtok.Type == Tok_Operator && str_compare_len(currtok.Text, "[]", 2) == 0)
				{
					eat(Tok_Operator);
					op = Op_DeleteArray;
				}
				else if ( currtok.Type == Tok_BraceSquare_Open && next.Type == Tok_BraceSquare_Close)
				{
					eat(Tok_BraceSquare_Open);
					eat(Tok_BraceSquare_Close);
					op = Op_DeleteArray;
				}
			}
			else
			{
				if ( op == Op_Invalid )
				{
					log_failure( "Invalid operator '%s'\n%s", prevtok.Text, parser_to_string(Context) );
					parser_pop(& Context);
					return InvalidCode;
				}
			}
		}
	}

	if ( op == Op_Invalid )
	{
		log_failure( "Invalid operator '%s'\n%s", currtok.Text, parser_to_string(Context) );
		parser_pop(& Context);
		return InvalidCode;
	}

	if ( ! was_new_or_delete)
		eat( currtok.Type );
	// <ExportFlag> <Attributes> <Specifiers> <ReturnType> <Qualifier::...> operator <Op>

	// Parse Params
	CodeParam params = parse_params(parser_use_parenthesis);
	// <ExportFlag> <Attributes> <Specifiers> <ReturnType> <Qualifier::...> operator <Op> ( <Parameters> )

	if ( params == nullptr && op == Op_Multiply )
		op = Op_MemberOfPointer;

	while ( left && tok_is_specifier(currtok) )
	{
		if ( specifiers == nullptr )
		{
			specifiers = def_specifier( strc_to_specifier( tok_to_str(currtok)) );
			eat( currtok.Type );
			continue;
		}

		specifiers_append(specifiers, strc_to_specifier( tok_to_str(currtok)) );
		eat( currtok.Type );
	}
	// <ExportFlag> <Attributes> <Specifiers> <ReturnType> <Qualifier::...> operator <Op> ( <Parameters> ) <Specifiers>

	// Parse Body
	CodeBody    body       = { nullptr };
	CodeComment inline_cmt = NullCode;
	if ( check( Tok_BraceCurly_Open ) )
	{
		body = cast(CodeBody, parse_function_body());
		if ( cast(Code, body) == Code_Invalid )
		{
			parser_pop(& Context);
			return InvalidCode;
		}
		// <ExportFlag> <Attributes> <Specifiers> <ReturnType> <Qualifier::...> operator <Op> ( <Parameters> ) <Specifiers> { ... }
	}
	else
	{
		Token stmt_end = currtok;
		eat( Tok_Statement_End );
		// <ExportFlag> <Attributes> <Specifiers> <ReturnType> <Qualifier::...> operator <Op> ( <Parameters> ) <Specifiers>;

		if ( currtok_noskip.Type == Tok_Comment && currtok_noskip.Line == stmt_end.Line )
			inline_cmt = parse_comment();
			// <ExportFlag> <Attributes> <Specifiers> <ReturnType> <Qualifier::...> operator <Op> ( <Parameters> ) <Specifiers>; <InlineCmt>
	}

	// OpValidateResult check_result = operator__validate( op, params, ret_type, specifiers );
	CodeOperator result = def_operator( op, tok_to_str(nspace), def_assign( params, ret_type, body, specifiers, attributes, mflags ) );

	if ( inline_cmt )
		result->InlineCmt = inline_cmt;

	parser_pop(& Context);
	return result;
}

internal
Code parse_operator_function_or_variable( bool expects_function, CodeAttributes attributes, CodeSpecifiers specifiers )
{
	push_scope();

	Code result = InvalidCode;

#ifndef GEN_PARSER_DISABLE_MACRO_FUNCTION_SIGNATURES
	b32 lone_macro = false;

	if ( currtok.Type == Tok_Preprocess_Macro && nexttok.Type == Tok_Statement_End )
	{
		// Were dealing with a lone macro after attributes/specifiers, there was a end statement ';' after.
		result = parse_simple_preprocess( Tok_Preprocess_Macro, parser_consume_braces );
		parser_pop(& Context);
		return result;
		// <Attributes> <Specifiers> <Macro>
	}
#endif

	CodeTypename type = parser_parse_type( parser_not_from_template, nullptr );
	// <Attributes> <Specifiers> <ReturnType/ValueType>

	if ( type == InvalidCode )
	{
		parser_pop(& Context);
		return InvalidCode;
	}

	bool found_operator = false;
	s32  idx            = Context.Tokens.Idx;

	for ( ; idx < array_num(Context.Tokens.Arr); idx++ )
	{
		Token tok = Context.Tokens.Arr[ idx ];

		if ( tok.Type == Tok_Identifier )
		{
			idx++;
			tok = Context.Tokens.Arr[ idx ];
			if ( tok.Type == Tok_Access_StaticSymbol )
				continue;

			break;
		}

		if ( tok.Type == Tok_Decl_Operator )
			found_operator = true;

		break;
	}

	if ( found_operator )
	{
		// Dealing with an operator overload
		result = cast(Code, parse_operator_after_ret_type( ModuleFlag_None, attributes, specifiers, type ));
		// <Attributes> <Specifiers> <ReturnType> operator ...
	}
	else
	{
		Token name = parse_identifier(nullptr);
		Context.Scope->Name = name;

		bool detected_capture = check( Tok_Capture_Start );

		// Check three tokens ahead to make sure that were not dealing with a constructor initialization...
		//                  (         350.0f    ,         <---  Could be the scenario
		// Example : <Capture_Start> <Value> <Comma>
		//                 idx         +1      +2
		bool detected_comma = Context.Tokens.Arr[ Context.Tokens.Idx + 2 ].Type == Tok_Comma;
		if ( detected_capture && ! detected_comma )
		{
			// Dealing with a function
			result = cast(Code, parse_function_after_name( ModuleFlag_None, attributes, specifiers, type, name ));
			// <Attributes> <Specifiers> <ReturnType> <Name> ( ...
		}
		else
		{
			if ( expects_function )
			{
				log_failure( "Expected function declaration (consteval was used)\n%s", parser_to_string(Context) );
				parser_pop(& Context);
				return InvalidCode;
			}

			// Dealing with a variable
			result = cast(Code, parse_variable_after_name( ModuleFlag_None, attributes, specifiers, type, tok_to_str(name) ));
			// <Attributes> <Specifiers> <ValueType> <Name> ...
		}
	}

	parser_pop(& Context);
	return result;
}

internal
CodePragma parse_pragma()
{
	push_scope();

	CodePragma
	pragma       = (CodePragma) make_code();
	pragma->Type = CT_Preprocess_Pragma;
	eat( Tok_Preprocess_Pragma );
	// #pragma

	if ( ! check( Tok_Preprocess_Content ))
	{
		log_failure( "Error, expected content after #pragma\n%s", parser_to_string(Context) );
		parser_pop(& Context);
		return InvalidCode;
	}

	Context.Scope->Name = currtok;

	pragma->Content = get_cached_string( tok_to_str(currtok) );
	eat( Tok_Preprocess_Content );
	// #pragma <Content>

	parser_pop(& Context);
	return pragma;
}

internal inline
CodeParam parse_params( bool use_template_capture )
{
	push_scope();

	if ( ! use_template_capture )
		eat( Tok_Capture_Start );
	// (

	else
	{
		if ( check( Tok_Operator ) && currtok.Text[ 0 ] == '<' )
			eat( Tok_Operator );
		// <
	}

	if ( ! use_template_capture && check( Tok_Capture_End ) )
	{
		eat( Tok_Capture_End );
		// )
		parser_pop(& Context);
		return NullCode;
	}
	else if ( check( Tok_Operator ) && currtok.Text[ 0 ] == '>' )
	{
		eat( Tok_Operator );
		// >
		parser_pop(& Context);
		return NullCode;
	}

	Code         macro           = { nullptr };
	CodeTypename type            = { nullptr };
	Code         value           = { nullptr };
	Token        name            = NullToken;
	Code         post_name_macro = { nullptr };

	if ( check( Tok_Varadic_Argument ) )
	{
		eat( Tok_Varadic_Argument );
		// ( or <  ...

		parser_pop(& Context);
		return param_varadic;
		// ( ... )
		// or < ... >
	}

	#define CheckEndParams() \
		(use_template_capture ? (currtok.Text[ 0 ] != '>') : (currtok.Type != Tok_Capture_End))

	// Ex: Unreal has this type of macro:                 vvvvvvvvv
	// COREUOBJECT_API void CallFunction( FFrame& Stack, RESULT_DECL, UFunction* Function );
	// and:                 vvvv
	// AddComponentByClass(UPARAM(meta = (AllowAbstract = "false")) TSubclassOf<UActorComponent> Class, bool bManualAttachment, ...
	if ( check(Tok_Preprocess_Macro))
	{
		macro = parse_simple_preprocess(Tok_Preprocess_Macro, parser_consume_braces);
		// ( <Macro>
	}
	if ( currtok.Type != Tok_Comma )
	{
		type = parser_parse_type( use_template_capture, nullptr );
		if ( cast(Code, type) == Code_Invalid )
		{
			parser_pop(& Context);
			return InvalidCode;
		}
		// ( <Macro> <ValueType>

		if ( check( Tok_Identifier ) )
		{
			name = currtok;
			eat( Tok_Identifier );
			// ( <Macro> <ValueType> <Name>
		}

		// Unreal has yet another type of macro:
		// template<class T UE_REQUIRES(TPointerIsConvertibleFromTo<T, UInterface>::Value)>
		// class T ... and then ^this^ UE_REQUIRES shows up
		// So we need to consume that.
		if ( check( Tok_Preprocess_Macro ))
		{
			post_name_macro = parse_simple_preprocess( Tok_Preprocess_Macro, parser_consume_braces );
		}

		// In template captures you can have a typename have direct assignment without a name
		// typename = typename ...
		// Which would result in a static value type from a struct expansion (traditionally)
		if ( ( name.Text || use_template_capture ) && bitfield_is_equal( u32, currtok.Flags, TF_Assign ) )
		{
			eat( Tok_Operator );
			// ( <Macro> <ValueType> <Name> =

			Token value_tok = currtok;

			if ( currtok.Type == Tok_Comma )
			{
				log_failure( "Expected value after assignment operator\n%s.", parser_to_string(Context) );
				parser_pop(& Context);
				return InvalidCode;
			}

			s32 capture_level  = 0;
			s32 template_level = 0;
			while ( (left && ( currtok.Type != Tok_Comma ) && template_level >= 0 && CheckEndParams()) || (capture_level > 0 || template_level > 0) )
			{
				if (currtok.Text[ 0 ] == '<')
					++ template_level;

				if (currtok.Text[ 0 ] == '>')
					-- template_level;
				if (currtok.Type == Tok_Operator && currtok.Text[1] == '>')
					-- template_level;

				if ( currtok.Type == Tok_Capture_Start)
					++ capture_level;

				if ( currtok.Type == Tok_Capture_End)
					-- capture_level;

				value_tok.Length = ( ( sptr )currtok.Text + currtok.Length ) - ( sptr )value_tok.Text;
				eat( currtok.Type );
			}

			value = untyped_str( string_to_strc(parser_strip_formatting( tok_to_str(value_tok), parser_strip_formatting_dont_preserve_newlines )) );
			// ( <Macro> <ValueType> <Name> = <Expression>
		}
	}

	CodeParam result = ( CodeParam )make_code();
	result->Type     = CT_Parameters;

	result->Macro = macro;

	if ( name.Length > 0 )
		result->Name = get_cached_string( tok_to_str(name) );

	result->ValueType = type;

	if ( value )
		result->Value = value;

	result->NumEntries++;

	while ( check(Tok_Comma) )
	{
		eat( Tok_Comma );
		// ( <Macro> <ValueType> <Name> = <Expression>,

		Code type  = { nullptr };
		Code value = { nullptr };

		if ( check( Tok_Varadic_Argument ) )
		{
			eat( Tok_Varadic_Argument );
			params_append(result, param_varadic );
			continue;
			// ( <Macro> <ValueType> <Name> = <Expression>, ...
		}

		// Ex: Unreal has this type of macro:                 vvvvvvvvv
		// COREUOBJECT_API void CallFunction( FFrame& Stack, RESULT_DECL, UFunction* Function );
		// and:                 vvvv
		// AddComponentByClass(UPARAM(meta = (AllowAbstract = "false")) TSubclassOf<UActorComponent> Class, bool bManualAttachment, ...
		if ( check(Tok_Preprocess_Macro))
		{
			macro = parse_simple_preprocess(Tok_Preprocess_Macro, parser_dont_consume_braces);
			// ( <Macro>
		}
		if ( currtok.Type != Tok_Comma )
		{
			type = cast(Code, parser_parse_type( use_template_capture, nullptr ));
			if ( type == Code_Invalid )
			{
				parser_pop(& Context);
				return InvalidCode;
			}
			// ( <Macro> <ValueType> <Name> = <Expression>, <Macro> <ValueType>

			Token null_token = { nullptr, 0, Tok_Invalid, false };
			name = null_token;

			if ( check( Tok_Identifier ) )
			{
				name = currtok;
				eat( Tok_Identifier );
				// ( <Macro> <ValueType> <Name> = <Expression>, <Macro> <ValueType> <Name>
			}

			// Unreal has yet another type of macro:
			// template<class T UE_REQUIRES(TPointerIsConvertibleFromTo<T, UInterface>::Value)>
			// class T ... and then ^this^ UE_REQUIRES shows up
			// So we need to consume that.
			if ( check( Tok_Preprocess_Macro ))
			{
				post_name_macro = parse_simple_preprocess( Tok_Preprocess_Macro, parser_dont_consume_braces );
			}

			// In template captures you can have a typename have direct assignment without a name
			// typename = typename ...
			// Which would result in a static value type from a struct expansion (traditionally)
			if ( ( name.Text || use_template_capture ) && bitfield_is_equal( u32, currtok.Flags, TF_Assign ) )
			{
				eat( Tok_Operator );
				// ( <Macro> <ValueType> <Name> = <Expression>, <Macro> <ValueType> <Name> =

				Token value_tok = currtok;

				if ( currtok.Type == Tok_Comma )
				{
					log_failure( "Expected value after assignment operator\n%s", parser_to_string(Context) );
					parser_pop(& Context);
					return InvalidCode;
				}

				s32 capture_level  = 0;
				s32 template_level = 0;
				while ( (left
				&& currtok.Type != Tok_Comma
				&& template_level >= 0
				&& CheckEndParams()) || (capture_level > 0 || template_level > 0) )
				{
					if (currtok.Text[ 0 ] == '<')
						++ template_level;

					if (currtok.Text[ 0 ] == '>')
						-- template_level;
					if (currtok.Type == Tok_Operator && currtok.Text[1] == '>')
						-- template_level;

					if ( currtok.Type == Tok_Capture_Start)
						++ capture_level;

					if ( currtok.Type == Tok_Capture_End)
						-- capture_level;

					value_tok.Length = ( ( sptr )currtok.Text + currtok.Length ) - ( sptr )value_tok.Text;
					eat( currtok.Type );
				}

				value = untyped_str( string_to_strc(parser_strip_formatting( tok_to_str(value_tok), parser_strip_formatting_dont_preserve_newlines )) );
				// ( <Macro> <ValueType> <Name> = <Expression>, <Macro> <ValueType> <Name> = <Expression>
			}
			// ( <Macro> <ValueType> <Name> = <Expression>, <Macro> <ValueType> <Name> = <Expression>, ..
		}

		CodeParam param = ( CodeParam )make_code();
		param->Type     = CT_Parameters;

		param->Macro = macro;

		if ( name.Length > 0 )
			param->Name = get_cached_string( tok_to_str(name) );

		param->PostNameMacro = post_name_macro;
		param->ValueType     = cast(CodeTypename, type);

		if ( value )
			param->Value = value;

		params_append(result, param );
	}

	if ( ! use_template_capture )
		eat( Tok_Capture_End );
	// ( <Macro> <ValueType> <Name> = <Expression>, <Macro> <ValueType> <Name> = <Expression>, .. )

	else
	{
		if ( ! check( Tok_Operator ) || currtok.Text[ 0 ] != '>' )
		{
			log_failure( "Expected '<' after 'template' keyword\n%s", parser_to_string(Context) );
			parser_pop(& Context);
			return InvalidCode;
		}
		eat( Tok_Operator );
		// < <Macro> <ValueType> <Name> = <Expression>, <Macro> <ValueType> <Name> = <Expression>, .. >
	}

	parser_pop(& Context);
	return result;
#undef context
}

internal
CodePreprocessCond parse_preprocess_cond()
{
	push_scope();

	if ( ! tok_is_preprocess_cond(currtok) )
	{
		log_failure( "Error, expected preprocess conditional\n%s", parser_to_string(Context) );
		parser_pop(& Context);
		return InvalidCode;
	}

	CodePreprocessCond
	cond       = (CodePreprocessCond) make_code();
	cond->Type = scast(CodeType, currtok.Type - ( Tok_Preprocess_If - CT_Preprocess_If ) );
	eat( currtok.Type );
	// #<Conditional>

	if ( ! check( Tok_Preprocess_Content ))
	{
		log_failure( "Error, expected content after #define\n%s", parser_to_string(Context) );
		parser_pop(& Context);
		return InvalidCode;
	}

	Context.Scope->Name = currtok;
	cond->Content = get_cached_string( tok_to_str(currtok) );
	eat( Tok_Preprocess_Content );
	// #<Conditiona> <Content>

	parser_pop(& Context);
	return cond;
}

internal
Code parse_simple_preprocess( TokType which, bool dont_consume_braces )
{
	// TODO(Ed): We can handle a macro a bit better than this. It's AST can be made more robust..
	// Make an AST_Macro, it should have an Name be the macro itself, with the function body being an optional function body node.
	// If we want it to terminate or have an inline comment we can possbily use its parent typedef for that info...
	push_scope();

	Token tok = currtok;
	eat( which );
	// <Macro>

	if ( ! dont_consume_braces && peektok.Type == Tok_BraceCurly_Open )
	{
		// Eat the block scope right after the macro. Were assuming the macro defines a function definition's signature
		eat( Tok_BraceCurly_Open );
		// <Macro> {

		// TODO(Ed) : Parse this properly later (expression and statement support)
		s32 level = 0;
		while ( left && ( currtok.Type != Tok_BraceCurly_Close || level > 0 ) )
		{
			if ( currtok.Type == Tok_BraceCurly_Open )
				level++;

			else if ( currtok.Type == Tok_BraceCurly_Close && level > 0 )
				level--;

			eat( currtok.Type );
		}
		eat( Tok_BraceCurly_Close );
		// <Macro> { <Body> }

		StrC prev_proc = Context.Scope->Prev->ProcName;
		if ( str_compare_len( prev_proc.Ptr, "parser_parse_typedef", prev_proc.Len ) != 0 )
		{
			if ( check( Tok_Statement_End ))
			{
				Token stmt_end = currtok;
				eat( Tok_Statement_End );
				// <Macro> { <Content> };

				if ( currtok_noskip.Type == Tok_Comment && currtok_noskip.Line == stmt_end.Line )
					eat( Tok_Comment );
					// <Macro> { <Content> }; <InlineCmt>
			}
		}

		tok.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)tok.Text;
	}
	else
	{
		// If the macro is just a macro in the body of an AST it may have a semi-colon for the user to close on purpsoe
		// (especially for functional macros)
		StrC calling_proc = Context.Scope->Prev->ProcName;

		if (strc_contains(Context.Scope->Prev->ProcName, txt("parser_parse_enum")))
		{
			// Do nothing
			goto Leave_Scope_Early;
		}
		else if (strc_contains(Context.Scope->Prev->ProcName, txt("parser_parse_typedef")))
		{
			// TODO(Ed): Reveiw the context for this?
			if ( peektok.Type == Tok_Statement_End )
			{
				Token stmt_end = currtok;
				eat( Tok_Statement_End );
				// <Macro>;

				tok.Length += prevtok.Length;

				// TODO(Ed): Reveiw the context for this? (ESPECIALLY THIS)
				if ( currtok_noskip.Type == Tok_Comment && currtok_noskip.Line == stmt_end.Line )
				{
					eat( Tok_Comment );
					// <Macro>; <InlineCmt>

					tok.Length += prevtok.Length;
				}
			}
		}
		else if (
				strc_contains(calling_proc, txt("parse_global_nspace"))
			||	strc_contains(calling_proc, txt("parse_class_struct_body"))
		)
		{
			if (peektok.Type == Tok_Statement_End)
			{
				Token stmt_end = currtok;
				eat( Tok_Statement_End );
				// <Macro>;
				tok.Length += prevtok.Length;
			}

		}
		// tok.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)tok.Text;
	}

Leave_Scope_Early:

	char const* content = str_fmt_buf( "%.*s ", tok.Length, tok.Text );

	Code result = untyped_str( to_strc_from_c_str(content) );
	Context.Scope->Name = tok;

	parser_pop(& Context);
	return result;
}

internal
Code parse_static_assert()
{
	push_scope();

	Code
	assert       = make_code();
	assert->Type = CT_Untyped;

	Token content = currtok;

	Context.Scope->Name = content;

	eat( Tok_StaticAssert );
	eat( Tok_Capture_Start );
	// static_assert(

	// TODO(Ed) : Parse this properly.
	s32 level = 0;
	while ( left && ( currtok.Type != Tok_Capture_End || level > 0 ) )
	{
		if ( currtok.Type == Tok_Capture_Start )
			level++;
		else if ( currtok.Type == Tok_Capture_End )
			level--;

		eat( currtok.Type );
	}
	eat( Tok_Capture_End );
	eat( Tok_Statement_End );
	// static_assert( <Content> );

	content.Length  = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)content.Text;

	char const* str  = str_fmt_buf( "%.*s\n", content.Length, content.Text );
	StrC content_str = { content.Length + 1, str };
	assert->Content = get_cached_string( content_str );
	assert->Name	= assert->Content;

	parser_pop(& Context);
	return assert;
}

/*
	This a brute-froce make all the arguments part of the token provided.
	Can have in-place function signatures, regular identifiers, in-place typenames, compile-time expressions, parameter-pack expansion, etc.
	This means that validation can only go so far, and so if there is any different in formatting
	passed the basic stripping supported it report a soft failure.
*/
internal inline
void parse_template_args( Token* token )
{
	if ( currtok.Type == Tok_Operator && currtok.Text[ 0 ] == '<' && currtok.Length == 1 )
	{
		eat( Tok_Operator );
		// <

		s32 level = 0;
		while ( left && level >= 0 && ( currtok.Text[ 0 ] != '>' || level > 0 ) )
		{
			if ( currtok.Text[ 0 ] == '<' )
				level++;

			if ( currtok.Text[ 0 ] == '>' )
				level--;
			if ( currtok.Type == Tok_Operator && currtok.Text[1] == '>')
				level--;

			eat( currtok.Type );
		}
		// < <Content>

		// Due to the >> token, this could have been eaten early...
		if (level == 0)
			eat( Tok_Operator );
		// < <Content> >

		// Extend length of name to last token
		token->Length = ( ( sptr )prevtok.Text + prevtok.Length ) - ( sptr )token->Text;
	}
}

// Variable parsing is handled in multiple places because its initial signature is shared with function parsing
internal
CodeVar parse_variable_after_name(
	  ModuleFlag        mflags
	, CodeAttributes    attributes
	, CodeSpecifiers    specifiers
	, CodeTypename      type
	, StrC              name
)
{
	push_scope();

	Code array_expr    = parse_array_decl();
	Code expr          = { nullptr };
	Code bitfield_expr = { nullptr };

	b32 using_constructor_initializer = false;

	if ( bitfield_is_equal( u32, currtok.Flags, TF_Assign ) )
	{
		// <Attributes> <Specifiers> <ValueType> <Name> = <Expression>
		expr = parse_assignment_expression();
	}

	if ( currtok.Type == Tok_BraceCurly_Open )
	{
		Token expr_tok = currtok;

		eat( Tok_BraceCurly_Open );
		// <Attributes> <Specifiers> <ValueType> <Name> {

		s32 level = 0;
		while ( left && ( currtok.Type != Tok_BraceCurly_Close || level > 0 ) )
		{
			if ( currtok.Type == Tok_BraceCurly_Open )
				level++;

			else if ( currtok.Type == Tok_BraceCurly_Close && level > 0 )
				level--;

			eat( currtok.Type );
		}
		eat( Tok_BraceCurly_Close );

		expr_tok.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)expr_tok.Text;
		expr            = untyped_str( tok_to_str(expr_tok) );
		// <Attributes> <Specifiers> <ValueType> <Name> = { <Expression> }
	}

	if ( currtok.Type == Tok_Capture_Start )
	{
		eat( Tok_Capture_Start);
		// <Attributes> <Specifiers> <ValueType> <Name> (

		Token expr_token = currtok;

		using_constructor_initializer = true;

		s32 level = 0;
		while ( left && ( currtok.Type != Tok_Capture_End || level > 0 ) )
		{
			if ( currtok.Type == Tok_Capture_Start )
				level++;

			else if ( currtok.Type == Tok_Capture_End && level > 0 )
				level--;

			eat( currtok.Type );
		}

		expr_token.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)expr_token.Text;
		expr              = untyped_str( tok_to_str(expr_token) );
		eat( Tok_Capture_End );
		// <Attributes> <Specifiers> <ValueType> <Name> ( <Expression> )
	}

	if ( currtok.Type == Tok_Assign_Classifer )
	{
		eat( Tok_Assign_Classifer );
		// <Attributes> <Specifiers> <ValueType> <Name> :

		Token expr_tok = currtok;

		if ( currtok.Type == Tok_Statement_End )
		{
			log_failure( "Expected expression after bitfield \n%S", parser_to_string(Context) );
			parser_pop(& Context);
			return InvalidCode;
		}

		while ( left && currtok.Type != Tok_Statement_End )
		{
			eat( currtok.Type );
		}

		expr_tok.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)expr_tok.Text;
		bitfield_expr   = untyped_str( tok_to_str(expr_tok) );
		// <Attributes> <Specifiers> <ValueType> <Name> : <Expression>
	}

	CodeVar     next_var   = NullCode;
	Token       stmt_end   = NullToken;
	CodeComment inline_cmt = NullCode;
	if ( type )
	{
		if ( currtok.Type == Tok_Comma )
		{
			// Were dealing with a statement with more than one declaration
			// This is only handled this way if its the first declaration
			// Otherwise its looped through in parse_variable_declaration_list
			next_var = parse_variable_declaration_list();
			// <Attributes> <Specifiers> <ValueType> <Name> : <Expression>, ...
			// <Attributes> <Specifiers> <ValueType> <Name> = <Expression>, ...
			// <Attributes> <Specifiers> <ValueType> <Name> { <Expression> }, ...
		}

		// If we're dealing with a "comma-procedding then we cannot expect a statement end or inline comment
		// Any comma procedding variable will not have a type provided so it can act as a indicator to skip this
		Token stmt_end = currtok;
		eat( Tok_Statement_End );
		// <Attributes> <Specifiers> <ValueType> <Name> : <Expression>, ...;
		// <Attributes> <Specifiers> <ValueType> <Name> = <Expression>, ...;
		// <Attributes> <Specifiers> <ValueType> <Name> { <Expression> }, ...;

		// Check for inline comment : <type> <identifier> = <expression>; // <inline comment>
		if ( left && ( currtok_noskip.Type == Tok_Comment ) && currtok_noskip.Line == stmt_end.Line )
		{
			inline_cmt = parse_comment();
			// <Attributes> <Specifiers> <ValueType> <Name> : <Expression>, ...; <InlineCmt>
			// <Attributes> <Specifiers> <ValueType> <Name> = <Expression>, ...; <InlineCmt>
			// <Attributes> <Specifiers> <ValueType> <Name> { <Expression> }, ...; <InlineCmt>
		}
	}

	CodeVar
	result              = (CodeVar) make_code();
	result->Type        = CT_Variable;
	result->Name        = get_cached_string( name );
	result->ModuleFlags = mflags;

	// Type can be null if we're dealing with a declaration from a variable declaration-list
	if ( type )
		result->ValueType = type;

	if (array_expr )
		type->ArrExpr = array_expr;

	if ( bitfield_expr )
		result->BitfieldSize = bitfield_expr;

	if ( attributes )
		result->Attributes = attributes;

	if ( specifiers )
		result->Specs = specifiers;

	if ( expr )
		result->Value = expr;

	if ( inline_cmt )
		result->InlineCmt = inline_cmt;

	if ( next_var )
	{
		result->NextVar         = next_var;
		result->NextVar->Parent = cast(Code, result);
	}

	result->VarConstructorInit = using_constructor_initializer;

	parser_pop(& Context);
	return result;
}

/*
	Note(Ed): This does not support the following:
	* Function Pointers
*/
internal
CodeVar parse_variable_declaration_list()
{
	push_scope();

	CodeVar result   = NullCode;
	CodeVar last_var = NullCode;
	while ( check( Tok_Comma ) )
	{
		eat( Tok_Comma );
		// ,

		CodeSpecifiers specifiers = NullCode;

		while ( left && tok_is_specifier(currtok) )
		{
			Specifier spec = strc_to_specifier( tok_to_str(currtok) );

			switch ( spec )
			{
				case Spec_Const:
					if ( specifiers->NumEntries && specifiers->ArrSpecs[ specifiers->NumEntries - 1 ] != Spec_Ptr )
					{
						log_failure( "Error, const specifier must come after pointer specifier for variable declaration proceeding comma\n"
						"(Parser will add and continue to specifiers, but will most likely fail to compile)\n%S"
						, parser_to_string(Context) );

						specifiers_append(specifiers, spec );
					}
				break;

				case Spec_Ptr:
				case Spec_Ref:
				case Spec_RValue:
				break;

				default:
				{
					log_failure( "Error, invalid specifier '%s' proceeding comma\n"
					"(Parser will add and continue to specifiers, but will most likely fail to compile)\n%SC"
					, tok_to_str(currtok), parser_to_string(Context) );
					continue;
				}
				break;
			}

			// eat(currtok.Type);

			if ( specifiers )
				specifiers_append(specifiers, spec );
			else
				specifiers = def_specifier( spec );
		}
		// , <Specifiers>

		StrC name = tok_to_str(currtok);
		eat( Tok_Identifier );
		// , <Specifiers> <Name>

		CodeVar var = parse_variable_after_name( ModuleFlag_None, NullCode, specifiers, NullCode, name );
		// , <Specifiers> <Name> ...

		if ( ! result )
		{
			result   = var;
			last_var = var;
		}
		else
		{
			last_var->NextVar         = var;
			last_var->NextVar->Parent = cast(Code, var);
			last_var                  = var;
		}
	}

	parser_pop(& Context);
	return result;
}

internal
CodeClass parser_parse_class( bool inplace_def )
{
	push_scope();
	CodeClass result = (CodeClass) parse_class_struct( Tok_Decl_Class, inplace_def );
	parser_pop(& Context);
	return result;
}

internal
CodeConstructor parser_parse_constructor( CodeSpecifiers specifiers )
{
	push_scope();

	Token     identifier = parse_identifier(nullptr);
	CodeParam params     = parse_params(parser_not_from_template);
	// <Name> ( <Parameters> )

	Code        initializer_list = NullCode;
	CodeBody    body             = NullCode;
	CodeComment inline_cmt       = NullCode;

	// TODO(Ed) : Need to support postfix specifiers

	if ( check( Tok_Assign_Classifer ) )
	{
		eat( Tok_Assign_Classifer );
		// <Name> ( <Parameters> ) :

		Token initializer_list_tok = currtok;

		s32 level                  = 0;
		while ( left && ( currtok.Type != Tok_BraceCurly_Open || level > 0 ) )
		{
			if (currtok.Type == Tok_Capture_Start)
				level++;
			else if ( currtok.Type == Tok_Capture_End )
				level--;

			eat( currtok.Type );
		}

		initializer_list_tok.Length = ( ( sptr )prevtok.Text + prevtok.Length ) - ( sptr )initializer_list_tok.Text;
		// <Name> ( <Parameters> ) : <InitializerList>

		initializer_list = untyped_str( tok_to_str(initializer_list_tok) );

		// TODO(Ed): Constructors can have post-fix specifiers

		body = cast(CodeBody, parse_function_body());
		// <Name> ( <Parameters> ) : <InitializerList> { <Body> }
	}
	else if ( check( Tok_BraceCurly_Open ) )
	{
		body = cast(CodeBody, parse_function_body());
		// <Name> ( <Parameters> ) { <Body> }
	}
	else if ( check( Tok_Operator) && currtok.Text[ 0 ] == '=' )
	{
		body = cast(CodeBody, parse_assignment_expression());
	}
	else
	{
		Token stmt_end = currtok;
		eat( Tok_Statement_End );
		// <Name> ( <Parameters> );

		if ( currtok_noskip.Type == Tok_Comment && currtok_noskip.Line == stmt_end.Line )
			inline_cmt = parse_comment();
		// <Name> ( <Parameters> ); <InlineCmt>
	}

	CodeConstructor result = ( CodeConstructor )make_code();

	result->Name = get_cached_string( tok_to_str(identifier));

	result->Specs = specifiers;

	if ( params )
		result->Params = params;

	if ( initializer_list )
		result->InitializerList = initializer_list;

	if ( body && body->Type == CT_Function_Body )
	{
		result->Body = cast(Code, body);
		result->Type = CT_Constructor;
	}
	else
		result->Type = CT_Constructor_Fwd;

	if ( inline_cmt )
		result->InlineCmt = inline_cmt;

	parser_pop(& Context);
	return result;
}

internal
CodeDestructor parser_parse_destructor( CodeSpecifiers specifiers )
{
	push_scope();

	bool has_context         = Context.Scope && Context.Scope->Prev;
	bool is_in_global_nspace = has_context && strc_are_equal( Context.Scope->Prev->ProcName, txt("parse_global_nspace") );

	if ( check( Tok_Spec_Virtual ) )
	{
		if ( specifiers )
			specifiers_append(specifiers, Spec_Virtual );
		else
			specifiers = def_specifier( Spec_Virtual );
		eat( Tok_Spec_Virtual );
	}
	// <Virtual Specifier>

	Token prefix_identifier = NullToken;
	if (is_in_global_nspace)
		prefix_identifier = parse_identifier(nullptr);

	if ( left && currtok.Text[ 0 ] == '~' )
		eat( Tok_Operator );
	else
	{
		log_failure( "Expected destructor '~' token\n%s", parser_to_string(Context) );
		parser_pop( & Context);
		return InvalidCode;
	}
	// <Virtual Specifier> ~

	Token       identifier = parse_identifier(nullptr);
	CodeBody    body       = { nullptr };
	CodeComment inline_cmt = NullCode;
	// <Virtual Specifier> ~<Name>

	eat( Tok_Capture_Start );
	eat( Tok_Capture_End );
	// <Virtual Specifier> ~<Name>()

	bool pure_virtual = false;

	if ( check( Tok_Operator ) && currtok.Text[ 0 ] == '=' )
	{
		// <Virtual Specifier> ~<Name>() =

		bool skip_formatting = true;
		Token upcoming = nexttok;
		if ( left && upcoming.Text[ 0 ] == '0' )
		{
			eat( Tok_Operator );
			eat( Tok_Number );
			// <Virtual Specifier> ~<Name>() = 0

			specifiers_append(specifiers, Spec_Pure );
		}
		else if ( left && str_compare_len( upcoming.Text, "default", sizeof("default") - 1 ) == 0)
		{
			body = cast(CodeBody, parse_assignment_expression());
			// <Virtual Specifier> ~<
		}
		else
		{
			log_failure( "Pure or default specifier expected due to '=' token\n%s", parser_to_string(Context) );
			parser_pop( & Context);
			return InvalidCode;
		}

		pure_virtual = true;
	}

	if ( ! pure_virtual && check( Tok_BraceCurly_Open ) )
		body = cast(CodeBody, parse_function_body());
	// <Virtual Specifier> ~<Name>() { ... }
	else
	{
		Token stmt_end = currtok;
		eat( Tok_Statement_End );
		// <Virtual Specifier> ~<Name>() <Pure Specifier>;

		if ( currtok_noskip.Type == Tok_Comment && currtok_noskip.Line == stmt_end.Line )
			inline_cmt = parse_comment();
		// <Virtual Specifier> ~<Name>() <Pure Specifier>; <InlineCmt>
	}

	CodeDestructor result = ( CodeDestructor )make_code();

	if ( tok_is_valid(prefix_identifier) )
	{
		prefix_identifier.Length += 1 + identifier.Length;
		result->Name = get_cached_string( tok_to_str(prefix_identifier) );
	}

	if ( specifiers )
		result->Specs = specifiers;

	if ( body && body->Type == CT_Function_Body )
	{
		result->Body = cast(Code, body);
		result->Type = CT_Destructor;
	}
	else
		result->Type = CT_Destructor_Fwd;

	if ( inline_cmt )
		result->InlineCmt = inline_cmt;

	parser_pop(& Context);
	return result;
}

internal
CodeEnum parser_parse_enum( bool inplace_def )
{
	push_scope();

	Specifier specs_found[16] = { Spec_NumSpecifiers };
	s32       NumSpecifiers = 0;

	CodeAttributes attributes = { nullptr };

	Token        name       = { nullptr, 0, Tok_Invalid };
	Code         array_expr = { nullptr };
	CodeTypename type       = { nullptr };

	char  entries_code[ kilobytes(128) ] = { 0 };
	s32   entries_length = 0;

	bool is_enum_class = false;

	eat( Tok_Decl_Enum );
	// enum

	if ( currtok.Type == Tok_Decl_Class )
	{
		eat( Tok_Decl_Class);
		is_enum_class = true;
		// enum class
	}

	attributes = parse_attributes();
	// enum <class> <Attributes>

	if ( check( Tok_Identifier ) )
	{
		name = currtok;
		Context.Scope->Name = currtok;
		eat( Tok_Identifier );
	}
	// enum <class> <Attributes> <Name>

	b32  use_macro_underlying = false;
	Code underlying_macro     = { nullptr };
	if ( currtok.Type == Tok_Assign_Classifer )
	{
		eat( Tok_Assign_Classifer );
		// enum <class> <Attributes> <Name> :

		type = parser_parse_type(parser_not_from_template, nullptr);
		if ( cast(Code, type) == Code_Invalid )
		{
			log_failure( "Failed to parse enum classifier\n%s", parser_to_string(Context) );
			parser_pop(& Context);
			return InvalidCode;
		}
		// enum <class> <Attributes> <Name> : <UnderlyingType>
	}
	else if ( currtok.Type == Tok_Preprocess_Macro )
	{
		// We'll support the enum_underlying macro
		StrC sig = txt("enum_underlying(");
		if ( strc_contains( tok_to_str(currtok), sig) )
		{
			use_macro_underlying = true;
			underlying_macro     = parse_simple_preprocess( Tok_Preprocess_Macro, parser_dont_consume_braces );
		}
	}

	CodeBody body = { nullptr };

	if ( currtok.Type == Tok_BraceCurly_Open )
	{
		body       = (CodeBody) make_code();
		body->Type = CT_Enum_Body;

		eat( Tok_BraceCurly_Open );
		// enum <class> <Attributes> <Name> : <UnderlyingType> {

		Code member = InvalidCode;

		bool expects_entry = true;
		while ( left && currtok_noskip.Type != Tok_BraceCurly_Close )
		{
			if ( ! expects_entry )
			{
				log_failure( "Did not expect an entry after last member of enum body.\n%s", parser_to_string(Context) );
				parser_pop(& Context);
				break;
			}

			if ( currtok_noskip.Type == Tok_Preprocess_Hash )
				eat( Tok_Preprocess_Hash );

			switch ( currtok_noskip.Type )
			{
				case Tok_NewLine:
					member = untyped_str( tok_to_str(currtok_noskip) );
					eat( Tok_NewLine );
				break;

				case Tok_Comment:
					member = cast(Code, parse_comment());
				break;

				case Tok_Preprocess_Define:
					member = cast(Code, parse_define());
					// #define
				break;

				case Tok_Preprocess_If:
				case Tok_Preprocess_IfDef:
				case Tok_Preprocess_IfNotDef:
				case Tok_Preprocess_ElIf:
					member = cast(Code, parse_preprocess_cond());
					// #<if, ifdef, ifndef, elif> ...
				break;

				case Tok_Preprocess_Else:
					member = cast(Code, preprocess_else);
					eat( Tok_Preprocess_Else );
				break;

				case Tok_Preprocess_EndIf:
					member = cast(Code, preprocess_endif);
					eat( Tok_Preprocess_EndIf );
				break;

				case Tok_Preprocess_Macro:
					member = cast(Code, parse_simple_preprocess( Tok_Preprocess_Macro, parser_consume_braces));
					// <Macro>
				break;

				case Tok_Preprocess_Pragma:
					member = cast(Code, parse_pragma());
					// #pragma
				break;

				case Tok_Preprocess_Unsupported:
					member = cast(Code, parse_simple_preprocess( Tok_Preprocess_Unsupported, parser_consume_braces ));
					// #<UNSUPPORTED>
				break;

				default:
					Token entry = currtok;

					eat( Tok_Identifier);
					// <Name>

					if ( currtok.Type == Tok_Operator && currtok.Text[0] == '=' )
					{
						eat( Tok_Operator );
						// <Name> =

						while ( currtok.Type != Tok_Comma && currtok.Type != Tok_BraceCurly_Close )
						{
							eat( currtok.Type );
						}
					}
					// <Name> = <Expression>

					// Unreal UMETA macro support
					if ( currtok.Type == Tok_Preprocess_Macro )
					{
						eat( Tok_Preprocess_Macro );
						// <Name> = <Expression> <Macro>
					}

					if ( currtok.Type == Tok_Comma )
					{
						//Token prev = * previous(Context.Tokens, dont_skip_formatting);
						//entry.Length = ( (sptr)prev.Text + prev.Length ) - (sptr)entry.Text;

						eat( Tok_Comma );
						// <Name> = <Expression> <Macro>,
					}

					// Consume inline comments
					// if ( currtok.Type == Tok_Comment && prevtok.Line == currtok.Line )
					// {
						// eat( Tok_Comment );
						// <Name> = <Expression> <Macro>, // <Inline Comment>
					// }

					Token prev = * lex_previous(Context.Tokens, lex_dont_skip_formatting);
					entry.Length = ( (sptr)prev.Text + prev.Length ) - (sptr)entry.Text;

					member = untyped_str( tok_to_str(entry) );
				break;
			}

			if ( member == Code_Invalid )
			{
				log_failure( "Failed to parse member\n%s", parser_to_string(Context) );
				parser_pop(& Context);
				return InvalidCode;
			}

			body_append(body, member );
		}

		eat( Tok_BraceCurly_Close );
		// enum <class> <Attributes> <Name> : <UnderlyingType> { <Body> }
	}

	CodeComment inline_cmt = NullCode;

	if ( ! inplace_def )
	{
		Token stmt_end = currtok;
		eat( Tok_Statement_End );
		// enum <class> <Attributes> <Name> : <UnderlyingType> { <Body> };

		if ( currtok_noskip.Type == Tok_Comment && currtok_noskip.Line == stmt_end.Line )
			inline_cmt = parse_comment();
			// enum <class> <Attributes> <Name> : <UnderlyingType> { <Body> }; <InlineCmt>
	}

	CodeEnum
	result = (CodeEnum) make_code();

	if ( body )
	{
		result->Type = is_enum_class ? CT_Enum_Class : CT_Enum;
		result->Body = body;
	}
	else
	{
		result->Type = is_enum_class ? CT_Enum_Class_Fwd : CT_Enum_Fwd;
	}

	result->Name = get_cached_string( tok_to_str(name) );

	if ( attributes )
		result->Attributes = attributes;

	result->UnderlyingTypeMacro = underlying_macro;
	result->UnderlyingType      = type;

	if ( inline_cmt )
		result->InlineCmt = inline_cmt;


	parser_pop(& Context);
	return result;
}

internal inline
CodeBody parser_parse_export_body()
{
	push_scope();
	CodeBody result = parse_global_nspace( CT_Export_Body );
	parser_pop(& Context);
	return result;
}

internal inline
CodeBody parser_parse_extern_link_body()
{
	push_scope();
	CodeBody result = parse_global_nspace( CT_Extern_Linkage_Body );
	parser_pop(& Context);
	return result;
}

internal
CodeExtern parser_parse_extern_link()
{
	push_scope();

	eat( Tok_Decl_Extern_Linkage );
	// extern

	Token name = currtok;
	eat( Tok_String );
	// extern "<Name>"

	name.Text   += 1;
	name.Length -= 1;

	CodeExtern
	result       = (CodeExtern) make_code();
	result->Type = CT_Extern_Linkage;
	result->Name = get_cached_string( tok_to_str(name) );

	CodeBody entry = parser_parse_extern_link_body();
	if ( cast(Code, entry) == Code_Invalid )
	{
		log_failure( "Failed to parse body\n%s", parser_to_string(Context) );
		parser_pop(& Context);
		return result;
	}
	// extern "<Name>" { <Body> }

	result->Body = entry;

	parser_pop(& Context);
	return result;
}

internal
CodeFriend parser_parse_friend()
{
	push_scope();

	eat( Tok_Decl_Friend );
	// friend

	CodeFn         function   = { nullptr };
	CodeOperator   op         = { nullptr };
	CodeSpecifiers specifiers = { nullptr };

	// Specifiers Parsing
	{
		Specifier specs_found[ 16 ] = { Spec_NumSpecifiers };
		s32       NumSpecifiers = 0;

		while ( left && tok_is_specifier(currtok) )
		{
			Specifier spec = strc_to_specifier( tok_to_str(currtok) );

			switch ( spec )
			{
				case Spec_Const :
				case Spec_Inline :
				case Spec_ForceInline :
					break;

				default :
					log_failure( "Invalid specifier %s for friend definition\n%s", spec_to_str( spec ), parser_to_string(Context) );
					parser_pop(& Context);
					return InvalidCode;
			}

			// Ignore const it will be handled by the type
			if ( spec == Spec_Const )
				break;

			specs_found[ NumSpecifiers ] = spec;
			NumSpecifiers++;
			eat( currtok.Type );
		}

		if ( NumSpecifiers )
		{
			specifiers = def_specifiers( NumSpecifiers, specs_found );
		}
		// <friend> <specifiers>
	}

	// Type declaration or return type
	CodeTypename type = parser_parse_type(parser_not_from_template, nullptr);
	if ( cast(Code, type) == Code_Invalid )
	{
		parser_pop(& Context);
		return InvalidCode;
	}
	// friend <Type>

	// Funciton declaration
	if ( currtok.Type == Tok_Identifier )
	{
		// Name
		Token name          = parse_identifier(nullptr);
		Context.Scope->Name = name;
		// friend <ReturnType> <Name>

		function = parse_function_after_name( ModuleFlag_None, NullCode, specifiers, type, name );

		// Parameter list
		// CodeParam params = parse_params();
		// friend <ReturnType> <Name> ( <Parameters> )

		// function             = make_code();
		// function->Type       = Function_Fwd;
		// function->Name       = get_cached_string( name );
		// function->ReturnType = type;

		// if ( params )
			// function->Params = params;
	}

	// Operator declaration or definition
	if ( currtok.Type == Tok_Decl_Operator )
	{
		op = parse_operator_after_ret_type( ModuleFlag_None, NullCode, specifiers, type );
	}

	CodeComment inline_cmt = NullCode;
	if ( function && function->Type == CT_Function_Fwd )
	{
		Token stmt_end = currtok;
		eat( Tok_Statement_End );
		// friend <Type>;
		// friend <ReturnType> <Name> ( <Parameters> );

		if ( currtok_noskip.Type == Tok_Comment && currtok_noskip.Line == stmt_end.Line )
			inline_cmt = parse_comment();
		// friend <Type>; <InlineCmt>
		// friend <ReturnType> <Name> ( <Parameters> ); <InlineCmt>
	}

	CodeFriend result = ( CodeFriend )make_code();
	result->Type      = CT_Friend;

	if ( function )
		result->Declaration = cast(Code, function);
	else if ( op )
		result->Declaration = cast(Code, op);
	else
		result->Declaration = cast(Code, type);

	if ( inline_cmt )
		result->InlineCmt = inline_cmt;

	parser_pop(& Context);
	return result;
}

internal
CodeFn parser_parse_function()
{
	push_scope();

	Specifier specs_found[16] = { Spec_NumSpecifiers };
	s32        NumSpecifiers = 0;

	CodeAttributes attributes = { nullptr };
	CodeSpecifiers specifiers = { nullptr };
	ModuleFlag     mflags     = ModuleFlag_None;

	if ( check(Tok_Module_Export) )
	{
		mflags = ModuleFlag_Export;
		eat( Tok_Module_Export );
	}
	// <export>

	attributes = parse_attributes();
	// <export> <Attributes>

	while ( left && tok_is_specifier(currtok) )
	{
		Specifier spec = strc_to_specifier( tok_to_str(currtok) );

		switch ( spec )
		{
			case Spec_Const:
			case Spec_Consteval:
			case Spec_Constexpr:
			case Spec_External_Linkage:
			case Spec_ForceInline:
			case Spec_Inline:
			case Spec_NeverInline:
			case Spec_Static:
			break;

			default:
				log_failure( "Invalid specifier %s for functon\n%s", spec_to_str(spec), parser_to_string(Context) );
				parser_pop(& Context);
				return InvalidCode;
		}

		if ( spec == Spec_Const )
			continue;

		specs_found[NumSpecifiers] = spec;
		NumSpecifiers++;
		eat( currtok.Type );
	}

	if ( NumSpecifiers )
	{
		specifiers = def_specifiers( NumSpecifiers, specs_found );
	}
	// <export> <Attributes> <Specifiers>

	CodeTypename ret_type = parser_parse_type(parser_not_from_template, nullptr);
	if ( cast(Code, ret_type) == Code_Invalid )
	{
		parser_pop(& Context);
		return InvalidCode;
	}
	// <export> <Attributes> <Specifiers> <ReturnType>

	Token name = parse_identifier(nullptr);
	Context.Scope->Name = name;
	if ( ! tok_is_valid(name) )
	{
		parser_pop(& Context);
		return InvalidCode;
	}
	// <export> <Attributes> <Specifiers> <ReturnType> <Name>

	CodeFn result = parse_function_after_name( mflags, attributes, specifiers, ret_type, name );
	// <export> <Attributes> <Specifiers> <ReturnType> <Name> ...

	parser_pop(& Context);
	return result;
}

internal
CodeNS parser_parse_namespace()
{
	push_scope();

	eat( Tok_Decl_Namespace );
	// namespace

	Token name = parse_identifier(nullptr);
	Context.Scope->Name = name;
	// namespace <Name>

	CodeBody body = parse_global_nspace( CT_Namespace_Body );
	if ( cast(Code, body) == Code_Invalid )
	{
		parser_pop(& Context);
		return InvalidCode;
	}
	// namespace <Name> { <Body> }

	CodeNS
	result       = (CodeNS) make_code();
	result->Type = CT_Namespace;
	result->Name = get_cached_string( tok_to_str(name) );

	result->Body = body;

	parser_pop(& Context);
	return result;
}

internal
CodeOperator parser_parse_operator()
{
	push_scope();

	CodeAttributes attributes = { nullptr };
	CodeSpecifiers specifiers = { nullptr };
	ModuleFlag     mflags     = ModuleFlag_None;

	Specifier specs_found[16] = { Spec_NumSpecifiers };
	s32        NumSpecifiers = 0;

	if ( check(Tok_Module_Export) )
	{
		mflags = ModuleFlag_Export;
		eat( Tok_Module_Export );
	}
	// <export>

	attributes = parse_attributes();
	// <export> <Attributes>

	while ( left && tok_is_specifier(currtok) )
	{
		Specifier spec = strc_to_specifier( tok_to_str(currtok) );

		switch ( spec )
		{
			case Spec_Const:
			case Spec_Constexpr:
			case Spec_ForceInline:
			case Spec_Inline:
			case Spec_NeverInline:
			case Spec_Static:
			break;

			default:
				log_failure( "Invalid specifier " "%SC" " for operator\n%S", spec_to_str(spec), parser_to_string(Context) );
				parser_pop(& Context);
				return InvalidCode;
		}

		if ( spec == Spec_Const )
			continue;

		specs_found[NumSpecifiers] = spec;
		NumSpecifiers++;
		eat( currtok.Type );
	}

	if ( NumSpecifiers )
	{
		specifiers = def_specifiers( NumSpecifiers, specs_found );
	}
	// <export> <Attributes> <Specifiers>

	// Parse Return Type
	CodeTypename ret_type = parser_parse_type(parser_not_from_template, nullptr);
	// <export> <Attributes> <Specifiers> <ReturnType>

	CodeOperator result = parse_operator_after_ret_type( mflags, attributes, specifiers, ret_type );
	// <export> <Attributes> <Specifiers> <ReturnType> ...

	parser_pop(& Context);
	return result;
}

internal
CodeOpCast parser_parse_operator_cast( CodeSpecifiers specifiers )
{
	push_scope();

	// Operator's namespace if not within same class.
	Token name = NullToken;
	if ( check( Tok_Identifier ) )
	{
		name = currtok;
		while ( left && currtok.Type == Tok_Identifier )
		{
			eat( Tok_Identifier );
			// <Specifiers> <Qualifier>

			if ( currtok.Type == Tok_Access_StaticSymbol )
				eat( Tok_Access_StaticSymbol );
				// <Specifiers> <Qualifier> ::
		}
		// <Specifiers> <Qualifier> :: ...

		name.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)name.Text;
	}

	eat( Tok_Decl_Operator );
	// <Specifiers> <Qualifier> :: ... operator

	CodeTypename type = parser_parse_type(parser_not_from_template, nullptr);
	// <Specifiers> <Qualifier> :: ... operator <UnderlyingType>

	Token name_tok = { type->Name.Ptr, type->Name.Len };
	Context.Scope->Name = name_tok;

	eat( Tok_Capture_Start );
	eat( Tok_Capture_End );
	// <Specifiers> <Qualifier> :: ... operator <UnderlyingType>()

	// TODO(Ed) : operator cast can have const, volatile, l-value, r-value noexecept qualifying specifiers.
	if ( check(Tok_Spec_Const))
	{
		if ( specifiers == nullptr )
			specifiers = def_specifier( Spec_Const );

		else
			specifiers_append(specifiers, Spec_Const );

		eat( Tok_Spec_Const );
	}
	// <Specifiers> <Qualifier> :: ... operator <UnderlyingType>() <const>

	Code        body       = NullCode;
	CodeComment inline_cmt = NullCode;

	if ( check( Tok_BraceCurly_Open) )
	{
		eat( Tok_BraceCurly_Open );
		// <Specifiers> <Qualifier> :: ... operator <UnderlyingType>() <const> {

		Token body_str = currtok;

		s32 level = 0;
		while ( left && ( currtok.Type != Tok_BraceCurly_Close || level > 0 ) )
		{
			if ( currtok.Type == Tok_BraceCurly_Open )
				level++;

			else if ( currtok.Type == Tok_BraceCurly_Close )
				level--;

			eat( currtok.Type );
		}
		body_str.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)body_str.Text;

		eat( Tok_BraceCurly_Close );
		// <Specifiers> <Qualifier> :: ... operator <UnderlyingType>() <const> { <Body> }

		body = untyped_str( tok_to_str(body_str) );
	}
	else
	{
		Token stmt_end = currtok;
		eat( Tok_Statement_End );
		// <Specifiers> <Qualifier> :: ... operator <UnderlyingType>() <const>;

		if ( currtok_noskip.Type == Tok_Comment && currtok_noskip.Line == stmt_end.Line )
			inline_cmt = parse_comment();
			// <Specifiers> <Qualifier> :: ... operator <UnderlyingType>() <const>; <InlineCmt>
	}

	CodeOpCast result = (CodeOpCast) make_code();

	if ( tok_is_valid(name) )
		result->Name = get_cached_string( tok_to_str(name) );

	if (body)
	{
		result->Type = CT_Operator_Cast;
		result->Body = cast(CodeBody, body);
	}
	else
	{
		result->Type = CT_Operator_Cast_Fwd;
	}

	if ( specifiers )
		result->Specs = specifiers;

	result->ValueType = cast(CodeTypename, type);

	parser_pop(& Context);
	return result;
}

internal inline
CodeStruct parser_parse_struct( bool inplace_def )
{
	push_scope();
	CodeStruct result = (CodeStruct) parse_class_struct( Tok_Decl_Struct, inplace_def );
	parser_pop(& Context);
	return result;
}

internal
CodeTemplate parser_parse_template()
{
#define UseTemplateCapture true

	push_scope();

	ModuleFlag mflags = ModuleFlag_None;

	if ( check( Tok_Module_Export ) )
	{
		mflags = ModuleFlag_Export;
		eat( Tok_Module_Export );
	}
	// <export> template

	eat( Tok_Decl_Template );
	// <export> template

	CodeParam params = parse_params( UseTemplateCapture );
	if ( cast(Code, params) == Code_Invalid )
	{
		parser_pop(& Context);
		return InvalidCode;
	}
	// <export> template< <Parameters> >

	Code definition = { nullptr };

	while ( left )
	{
		if ( check( Tok_Decl_Class ) )
		{
			definition = cast(Code, parser_parse_class( parser_not_inplace_def));
			// <export> template< <Parameters> > class ...
			break;
		}

		if ( check( Tok_Decl_Struct ) )
		{
			definition = cast(Code, parser_parse_struct( parser_not_inplace_def));
			// <export> template< <Parameters> > struct ...
			break;
		}

		if ( check( Tok_Decl_Union ) )
		{
			definition = cast(Code, parser_parse_union( parser_not_inplace_def));
			// <export> template< <Parameters> > union ...
			break;
		}

		if ( check( Tok_Decl_Using ) )
		{
			definition = cast(Code, parser_parse_using());
			// <export> template< <Parameters> > using ...
			break;
		}

		// Its either a function or a variable
		Token name                = { nullptr, 0, Tok_Invalid };

		CodeAttributes attributes = { nullptr };
		CodeSpecifiers specifiers = { nullptr };

		bool expects_function     = false;

		Specifier specs_found[ 16 ] = { Spec_NumSpecifiers };
		s32        NumSpecifiers = 0;

		attributes = parse_attributes();
		// <export> template< <Parameters> > <Attributes>

		// Specifiers Parsing
		{
			while ( left && tok_is_specifier(currtok) )
			{
				Specifier spec = strc_to_specifier( tok_to_str(currtok) );

				switch ( spec )
				{
					case Spec_Const :
					case Spec_Constexpr :
					case Spec_Constinit :
					case Spec_External_Linkage :
					case Spec_Global :
					case Spec_Inline :
					case Spec_ForceInline :
					case Spec_Local_Persist :
					case Spec_Mutable :
					case Spec_Static :
					case Spec_Thread_Local :
					case Spec_Volatile :
						break;

					case Spec_Consteval :
						expects_function = true;
						break;

					default :
						log_failure( "Invalid specifier %s for variable or function\n%s", spec_to_str( spec ), parser_to_string(Context) );
						parser_pop(& Context);
						return InvalidCode;
				}

				// Ignore const it will be handled by the type
				if ( spec == Spec_Const )
					break;

				specs_found[ NumSpecifiers ] = spec;
				NumSpecifiers++;
				eat( currtok.Type );
			}

			if ( NumSpecifiers )
			{
				specifiers = def_specifiers( NumSpecifiers, specs_found );
			}
			// <export> template< <Parameters> > <Attributes> <Specifiers>
		}


		bool has_context         = Context.Scope && Context.Scope->Prev;
		bool is_in_global_nspace = has_context && strc_are_equal( Context.Scope->Prev->ProcName, txt("parse_global_nspace") );
		// Possible constructor implemented at global file scope.
		if (is_in_global_nspace)
		{
			Code constructor_destructor = parse_global_nspace_constructor_destructor( specifiers );
			if ( constructor_destructor )
			{
				definition = constructor_destructor;
				// <Attributes> <Specifiers> <Name> :: <Name> <Type> () { ... }
				break;
			}
		}

		// Possible user Defined operator casts
		if (is_in_global_nspace)
		{
			bool found_operator_cast_outside_class_implmentation = false;
			s32  idx = Context.Tokens.Idx;

			for ( ; idx < array_num(Context.Tokens.Arr); idx++ )
			{
				Token tok = Context.Tokens.Arr[ idx ];

				if ( tok.Type == Tok_Identifier )
				{
					idx++;
					tok = Context.Tokens.Arr[ idx ];
					if ( tok.Type == Tok_Access_StaticSymbol )
						continue;

					break;
				}

				if ( tok.Type == Tok_Decl_Operator )
					found_operator_cast_outside_class_implmentation = true;

				break;
			}

			if ( found_operator_cast_outside_class_implmentation )
			{
				definition = cast(Code, parser_parse_operator_cast( specifiers ));
				// <Attributes> <Specifiers> <Name> :: operator <Type> () { ... }
				break;
			}
		}

		definition = parse_operator_function_or_variable( expects_function, attributes, specifiers );
		// <export> template< <Parameters> > <Attributes> <Specifiers> ...
		break;
	}

	CodeTemplate result = ( CodeTemplate )make_code();
	result->Type        = CT_Template;
	result->Params      = params;
	result->Declaration = definition;
	result->ModuleFlags = mflags;
	// result->Name        = definition->Name;

	parser_pop(& Context);
	return result;
#undef UseTemplateCapture
}

/*
	This is a mess, but it works
	Parsing typename is arguably one of the worst aspects of C/C++.
	This is an effort to parse it without a full blown or half-blown compliant parser.

	Recursive function typenames are not supported, if they are used expect it to serailize just fine, but validation with AST::is_equal
	will not be possible if two ASTs share the same definiton but the formatting is slightly different:
	AST_1->Name: (* A    ( int   (*)    (short a,unsigned b,long c) ) )
	AST_2->Name: (* A       ( int(*)(short a, unsigned b, long c) ) )

	The excess whitespace cannot be stripped however, because there is no semantic awareness within the first capture group.
*/
internal
CodeTypename parser_parse_type( bool from_template, bool* typedef_is_function )
{
	push_scope();

	Token context_tok = prevtok;

	Specifier specs_found[ 16 ] = { Spec_NumSpecifiers };
	s32       NumSpecifiers = 0;

	Token name= { nullptr, 0, Tok_Invalid };

	ETypenameTag tag = Tag_None;

	// Attributes are assumed to be before the type signature
	CodeAttributes attributes = parse_attributes();
	// <Attributes>

	// Prefix specifiers
	while ( left && tok_is_specifier(currtok) )
	{
		Specifier spec = strc_to_specifier( tok_to_str(currtok) );

		if ( spec != Spec_Const )
		{
			log_failure( "Error, invalid specifier used in type definition: %SC\n%S", tok_to_str(currtok), parser_to_string(Context) );
			parser_pop(& Context);
			return InvalidCode;
		}

		specs_found[ NumSpecifiers ] = spec;
		NumSpecifiers++;
		eat( currtok.Type );
	}
	// <Attributes> <Specifiers>

	if ( left == 0 )
	{
		log_failure( "Error, unexpected end of type definition\n%S", parser_to_string(Context) );
		parser_pop(& Context);
		return InvalidCode;
	}

	if ( from_template && currtok.Type == Tok_Decl_Class )
	{
		// If a value's type is being parsed from a template, class can be used instead of typename.
		name = currtok;
		eat(Tok_Decl_Class);
		// <class>
	}

	// All kinds of nonsense can makeup a type signature, first we check for a in-place definition of a class, enum, struct, or union
	else if ( currtok.Type == Tok_Decl_Class || currtok.Type == Tok_Decl_Enum || currtok.Type == Tok_Decl_Struct
			|| currtok.Type == Tok_Decl_Union )
	{
		switch (currtok.Type) {
			case Tok_Decl_Class  : tag = Tag_Class;  break;
			case Tok_Decl_Enum   : tag = Tag_Enum;   break;
			case Tok_Decl_Struct : tag = Tag_Struct; break;
			case Tok_Decl_Union  : tag = Tag_Union;  break;
			default:
				break;
		}
		eat( currtok.Type );
		// <Attributes> <Specifiers> <class, enum, struct, union>

		name = parse_identifier(nullptr);

		// name.Length = ( ( sptr )currtok.Text + currtok.Length ) - ( sptr )name.Text;
		// eat( Tok_Identifier );
		Context.Scope->Name = name;
		// <Attributes> <Specifiers> <class, enum, struct, union> <Name>
	}

// Decltype draft implementaiton
#if 0
else if ( currtok.Type == Tok_DeclType )
{
	// Will have a capture and its own parsing rules, were going to just shove everything in a string (for now).
	name = currtok;
	eat( Tok_DeclType );
	// <Attributes> <Specifiers> decltype

	eat( Tok_Capture_Start );
	while ( left && currtok.Type != Tok_Capture_End )
	{
		if ( currtok.Type == Tok_Capture_Start )
			level++;

		if ( currtok.Type == Tok_Capture_End )
			level--;

		eat( currtok.Type );
	}
	eat( Tok_Capture_End );

	name.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)name.Text;
	Context.Scope->Name = name;
	// <Attributes> <Specifiers> decltype( <Expression > )
}
#endif

	// Check if native type keywords are used, eat them for the signature.
	// <attributes> <specifiers> <native types ...> ...
	else if ( currtok.Type >= Tok_Type_Unsigned && currtok.Type <= Tok_Type_MS_W64 )
	{
		// TODO(Ed) : Review this... Its necessary for parsing however the algo's path to this is lost...
		name = currtok;
		eat( currtok.Type );

		while ( left && currtok.Type >= Tok_Type_Unsigned && currtok.Type <= Tok_Type_MS_W64 )
		{
			eat( currtok.Type );
		}

		name.Length = ( ( sptr )prevtok.Text + prevtok.Length ) - ( sptr )name.Text;
		// <Attributes> <Specifiers> <Compound type expression>
	}
	else if ( currtok.Type == Tok_Type_Typename )
	{
		name = currtok;
		eat(Tok_Type_Typename);
		// <typename>

		if ( ! from_template )
		{
			name                = parse_identifier(nullptr);
			Context.Scope->Name = name;
			if ( ! tok_is_valid(name) )
			{
				log_failure( "Error, failed to type signature\n%s", parser_to_string(Context) );
				parser_pop(& Context);
				return InvalidCode;
			}
		}
	}
	else if ( currtok.Type == Tok_Preprocess_Macro ) {
		// Typename is a macro
		name = currtok;
		eat(Tok_Preprocess_Macro);
	}

	// The usual Identifier type signature that may have namespace qualifiers
	else
	{
		name                = parse_identifier(nullptr);
		Context.Scope->Name = name;
		if ( ! tok_is_valid(name) )
		{
			log_failure( "Error, failed to type signature\n%s", parser_to_string(Context) );
			parser_pop(& Context);
			return InvalidCode;
		}
		// <Attributes> <Specifiers> <Qualifier ::> <Identifier>
		// <Attributes> <Specifiers> <Identifier>
	}

	// Suffix specifiers for typename.
	while ( left && tok_is_specifier(currtok) )
	{
		Specifier spec = strc_to_specifier( tok_to_str(currtok) );

		if ( spec != Spec_Const && spec != Spec_Ptr && spec != Spec_Ref && spec != Spec_RValue )
		{
			log_failure( "Error, invalid specifier used in type definition: %s\n%s", currtok.Text, parser_to_string(Context) );
			parser_pop(& Context);
			return InvalidCode;
		}

		specs_found[ NumSpecifiers ] = spec;
		NumSpecifiers++;
		eat( currtok.Type );
	}

#ifdef GEN_USE_NEW_TYPENAME_PARSING
	if ( NumSpecifiers )
	{
		specifiers    = def_specifiers( NumSpecifiers, specs_found );
		NumSpecifiers = 0;
	}
#endif
	// <Attributes> <Specifiers> <Identifier> <Specifiers>

	// For function type signatures
	CodeTypename return_type = NullCode;
	CodeParam    params      = NullCode;

#ifdef GEN_USE_NEW_TYPENAME_PARSING
	CodeParam params_nested = NullCode;
#endif

	bool   is_function_typename = false;
	Token* last_capture         = nullptr;
	{
		Token* scanner = Context.Tokens.Arr + Context.Tokens.Idx;

		// An identifier being within a typename's signature only occurs if were parsing a typename for a typedef.
		if ( typedef_is_function && scanner->Type == Tok_Identifier )
		{
			is_function_typename = true;
			++scanner;
		}
		is_function_typename = scanner->Type == Tok_Capture_Start;

		Token* first_capture = scanner;
		if ( is_function_typename )
		{
			// Go to the end of the signature
			while ( scanner->Type != Tok_Statement_End && scanner->Type != Tok_BraceCurly_Open )
				++scanner;

			// Go back to the first capture start found
			while ( scanner->Type != Tok_Capture_Start )
				--scanner;

			last_capture = scanner;
		}

		bool has_context   = Context.Scope && Context.Scope->Prev;
		bool is_for_opcast = has_context && strc_are_equal( Context.Scope->Prev->ProcName, txt("parser_parse_operator_cast") );
		if ( is_for_opcast && is_function_typename && last_capture )
		{
			// If we're parsing for an operator cast, having one capture start is not enough
			// we need to make sure that the capture is not for the cast definition.
			is_function_typename = false;

			if ( last_capture == first_capture )
			{
				// The capture start in question is the first capture start, this is not a function typename.
				is_function_typename = false;
			}
		}
	}

	if ( is_function_typename )
	{
		// We're dealing with a function typename.
		// By this point, decltype should have been taken care of for return type, along with any all its specifiers

		// The previous information with exception to attributes will be considered the return type.
		return_type       = ( CodeTypename )make_code();
		return_type->Type = CT_Typename;

		// String
		// name_stripped = String::make( GlobalAllocator, name );
		// name_stripped.strip_space();
		return_type->Name = get_cached_string( tok_to_str(name) );

#ifdef GEN_USE_NEW_TYPENAME_PARSING
		if ( specifiers )
		{
			return_type->Specs = specifiers;
			specifiers         = nullptr;
		}

#else
		if ( NumSpecifiers )
			return_type->Specs = def_specifiers( NumSpecifiers, ( Specifier* )specs_found );

		// Reset specifiers, the function itself will have its own suffix specifiers possibly.
		NumSpecifiers = 0;
#endif
		// <Attributes> <ReturnType>
		name = NullToken;

		// The next token can either be a capture for the identifier or it could be the identifier exposed.
		if ( ! check( Tok_Capture_Start ) )
		{
			// Started with an identifier immeidately, which means its of the format: <ReturnType> <identifier> <capture>;
			name = parse_identifier(nullptr);
		}
		// <Attributes> <ReturnType> <Identifier>

		// If the next token is a capture start and is not the last capture, then we're dealing with function typename whoose identifier is within the
		// capture.
		else if ( ( Context.Tokens.Arr + Context.Tokens.Idx ) != last_capture )
		{
			// WIP : Possible alternative without much pain...
			// If this were to be parsed properly...
			// Eat Capture Start
			// Deal with possible binding specifiers (*, &, &&) and modifiers on those bindings (const, volatile)
			// Parse specifiers for the typename with an optional identifier,
			// we can shove these specific specifiers into a specs, and then leave the suffix ones for a separate member of the AST.
			// Parse immeidate capture which would be with parse_params()
			// Eat Capture End
#ifdef GEN_USE_NEW_TYPENAME_PARSING
			eat( Tok_Capture_Start );
			// <Attributes> <ReturnType> (

			// Binding specifiers
			while ( left && currtok.is_specifier() )
			{
				Specifier spec = to_type( currtok );

				if ( spec != Spec_Ptr && spec != Spec_Ref && spec != Spec_RValue )
				{
					log_failure( "Error, invalid specifier used in type definition: %s\n%s", currtok.Text, to_string(Context) );
					pop(& Context);
					return InvalidCode;
				}

				specs_found[ NumSpecifiers ] = spec;
				NumSpecifiers++;
				eat( currtok.Type );
			}

			if ( NumSpecifiers )
			{
				specifiers = def_specifiers( NumSpecifiers, specs_found );
			}
			NumSpecifiers = 0;
			// <Attributes> <ReturnType> ( <Specifiers>

			if ( check( Tok_Identifier ) )
				name = parse_identifier();
			// <Attributes> <ReturnType> ( <Specifiers> <Identifier>

			// Immeidate parameters

			if ( check( Tok_Capture_Start ) )
				params_nested = parse_params();
			// <Attributes> <ReturnType> ( <Specifiers> <Identifier> ( <Parameters> )

			eat( Tok_Capture_End );
			// <Attributes> <ReturnType> ( <Specifiers> <Identifier> ( <Parameters> ) )

#else
			// Starting immediatley with a capture, most likely declaring a typename for a member function pointer.
			// Everything within this capture will just be shoved into the name field including the capture tokens themselves.
			name = currtok;

			eat( Tok_Capture_Start );
			// <Attributes> <ReturnType> (

			s32 level = 0;
			while ( left && ( currtok.Type != Tok_Capture_End || level > 0 ) )
			{
				if ( currtok.Type == Tok_Capture_Start )
					level++;

				if ( currtok.Type == Tok_Capture_End )
					level--;

				eat( currtok.Type );
			}
			eat( Tok_Capture_End );
			// <Attributes> <ReturnType> ( <Expression> )

			name.Length = ( ( sptr )prevtok.Text + prevtok.Length ) - ( sptr )name.Text;
#endif
		}

		// Were now dealing with the parameters of the function
		params = parse_params(parser_use_parenthesis);
		// <Attributes> <ReturnType> <All Kinds of nonsense> ( <Parameters> )

		// Look for suffix specifiers for the function
		while ( left && tok_is_specifier(currtok) )
		{
			Specifier spec = strc_to_specifier( tok_to_str(currtok) );

			if ( spec != Spec_Const
					// TODO : Add support for NoExcept, l-value, volatile, l-value, etc
					// && spec != Spec_NoExcept
					&& spec != Spec_RValue )
			{
				log_failure( "Error, invalid specifier used in type definition: %SC\n%S", tok_to_str(currtok), parser_to_string(Context) );
				parser_pop(& Context);
				return InvalidCode;
			}

			specs_found[ NumSpecifiers ] = spec;
			NumSpecifiers++;
			eat( currtok.Type );
		}

#ifdef GEN_USE_NEW_TYPENAME_PARSING
		if ( NumSpecifiers )
		{
			func_suffix_specs = def_specifiers( NumSpecifiers, specs_found );
			NumSpecifiers     = 0;
		}
#endif
		// <Attributes> <ReturnType> <All Kinds of nonsense> ( <Parameters> ) <Specifiers>
	}
	// <Attributes> <All Kinds of nonsense>

	bool is_param_pack = false;
	if ( check( Tok_Varadic_Argument ) )
	{
		is_param_pack = true;
		eat( Tok_Varadic_Argument );
		// <Attributes> <All kinds of nonsense> ...
	}

	CodeTypename result = ( CodeTypename )make_code();
	result->Type        = CT_Typename;
	// result->Token = Context.Scope->Start;

	// Need to wait until were using the new parsing method to do this.
	String name_stripped = parser_strip_formatting( tok_to_str(name), parser_strip_formatting_dont_preserve_newlines );

	// name_stripped.strip_space();

#ifdef GEN_USE_NEW_TYPENAME_PARSING
	if ( params_nested )
	{
		name_stripped.append( params_nested->to_string() );
	}
#endif

	result->Name = get_cached_string( string_to_strc(name_stripped) );

	if ( attributes )
		result->Attributes = attributes;

#ifdef GEN_USE_NEW_TYPENAME_PARSING
	if ( specifiers )
	{
		result->Specs = specifiers;
	}

	if ( func_suffix_specs )
	{
		result->FuncSuffixSpecs = func_suffix_specs;
	}
#else
	if ( NumSpecifiers )
	{
		CodeSpecifiers specifiers = def_specifiers( NumSpecifiers, ( Specifier* )specs_found );
		result->Specs   = specifiers;
	}
#endif

	if ( is_param_pack )
		result->IsParamPack = true;

	// These following are only populated if its a function typename
	if ( return_type )
	{
		result->ReturnType = return_type;

		if ( typedef_is_function )
			*typedef_is_function = true;
	}

	if ( params )
		result->Params = params;

	result->TypeTag = tag;

	parser_pop(& Context);
	return result;
}

internal
CodeTypedef parser_parse_typedef()
{
	push_scope();

	bool  is_function = false;
	Token name        = { nullptr, 0, Tok_Invalid };
	Code  array_expr  = { nullptr };
	Code  type        = { nullptr };

	ModuleFlag mflags = ModuleFlag_None;

	if ( check(Tok_Module_Export) )
	{
		mflags = ModuleFlag_Export;
		eat( Tok_Module_Export );
	}
	// <ModuleFlags>

	eat( Tok_Decl_Typedef );
	// <ModuleFlags> typedef

	const bool from_typedef = true;

#if GEN_PARSER_DISABLE_MACRO_TYPEDEF
	if ( false )
#else
	if ( check( Tok_Preprocess_Macro ))
#endif
	{
		type = cast(Code, t_empty);
		name = currtok;
		Context.Scope->Name = name;
		eat( Tok_Preprocess_Macro );
		// <ModuleFalgs> typedef <Preprocessed_Macro>

		if ( currtok.Type == Tok_Identifier )
		{
			StrC name_str = { name.Length, name.Text };
			type = untyped_str(name_str);
			name = currtok;
			eat(Tok_Identifier);
		}
		// <ModuleFalgs> typedef <Preprocessed_Macro> <Identifier>
	}
	else
	{
		bool is_complicated =
				currtok.Type == Tok_Decl_Enum
			||	currtok.Type == Tok_Decl_Class
			||	currtok.Type == Tok_Decl_Struct
			||	currtok.Type == Tok_Decl_Union;


		// This code is highly correlated with parse_complicated_definition
		if ( is_complicated )
		{
			TokArray tokens = Context.Tokens;
			TokType  which  = currtok.Type;

			s32 idx = tokens.Idx;
			s32 level = 0;
			for ( ; idx < array_num(tokens.Arr); idx ++ )
			{
				if ( tokens.Arr[idx].Type == Tok_BraceCurly_Open )
					level++;

				if ( tokens.Arr[idx].Type == Tok_BraceCurly_Close )
					level--;

				if ( level == 0 && tokens.Arr[idx].Type == Tok_Statement_End )
					break;
			}

			Token pre_foward_tok = currtok;
			if ( (idx - 3 ) == tokens.Idx )
			{
				// Its a forward declaration only
				type = parse_forward_or_definition( which, from_typedef );
				// <ModuleFalgs> typedef <UnderlyingType: Forward Decl>
			}
			else
			{
				Token tok = tokens.Arr[ idx - 1 ];
				if ( tok.Type == Tok_Identifier )
				{
					log_fmt("Found id\n");
					tok = tokens.Arr[ idx - 2 ];

					bool is_indirection = tok.Type == Tok_Ampersand
					||                    tok.Type == Tok_Star;

					bool ok_to_parse = false;

					Token temp_3 = tokens.Arr[ idx - 3 ];

					if ( tok.Type == Tok_BraceCurly_Close )
					{
						// Its an inplace definition
						// typedef <which> <type_identifier> { ... } <identifier>;
						ok_to_parse = true;
					}
					else if ( tok.Type == Tok_Identifier && tokens.Arr[ idx - 3 ].Type == which )
					{
						// Its a variable with type ID using which namespace.
						// typedef <which> <type_identifier> <identifier>;
						ok_to_parse = true;
					}
					else if ( is_indirection )
					{
						// Its a indirection type with type ID using struct namespace.
						// typedef <which> <type_identifier>* <identifier>;
						ok_to_parse = true;
					}

					if ( ! ok_to_parse )
					{
						log_failure( "Unsupported or bad member definition after struct declaration\n%S", parser_to_string(Context) );
						parser_pop(& Context);
						return InvalidCode;
					}

					// TODO(Ed) : I'm not sure if I have to use parser_parse_type here, I'd rather not as that would complicate parser_parse_type.
					// type = parser_parse_type();
					type = parse_forward_or_definition( which, from_typedef );
					// <ModuleFalgs> typedef <UnderlyingType>
				}
				else if ( tok.Type == Tok_BraceCurly_Close )
				{
					// Its a definition
					// <which> { ... };
					type = parse_forward_or_definition( currtok.Type, from_typedef );
					// <ModuleFalgs> typedef <UnderlyingType>
				}
				else if ( tok.Type == Tok_BraceSquare_Close)
				{
					// Its an array definition
					// <which> <type_identifier> <identifier> [ ... ];
					type = cast(Code, parser_parse_type(parser_not_from_template, nullptr));
					// <ModuleFalgs> typedef <UnderlyingType>
				}
				else
				{
					log_failure( "Unsupported or bad member definition after struct declaration\n%S", parser_to_string(Context) );
					parser_pop(& Context);
					return InvalidCode;
				}
			}
		}
		else
		{
			bool from_template = false;
			type = cast(Code, parser_parse_type( from_template, &is_function ));
			// <ModuleFalgs> typedef <UnderlyingType>
		}

		if ( check( Tok_Identifier ) )
		{
			name = currtok;
			eat( Tok_Identifier );
			// <ModuleFalgs> typedef <UnderlyingType> <Name>
		}
		else if ( ! is_function )
		{
			log_failure( "Error, expected identifier for typedef\n%S", parser_to_string(Context) );
			parser_pop(& Context);
			return InvalidCode;
		}

		array_expr = parse_array_decl();
		// <UnderlyingType> + <ArrayExpr>
	}

	Token stmt_end = currtok;
	eat( Tok_Statement_End );
	// <ModuleFalgs> typedef <UnderlyingType> <Name>;

	CodeComment inline_cmt = NullCode;
	if ( currtok_noskip.Type == Tok_Comment && currtok_noskip.Line == stmt_end.Line )
		inline_cmt = parse_comment();
		// <ModuleFalgs> typedef <UnderlyingType> <Name> <ArrayExpr>; <InlineCmt>

	CodeTypedef
	result              = (CodeTypedef) make_code();
	result->Type        = CT_Typedef;
	result->ModuleFlags = mflags;

	if ( is_function )
	{
		result->Name       = type->Name;
		result->IsFunction = true;
	}
	else
	{
		result->Name       = get_cached_string( tok_to_str(name) );
		result->IsFunction = false;
	}

	if ( type )
	{
		result->UnderlyingType         = type;
		result->UnderlyingType->Parent = cast(Code, result);
	}
	// Type needs to be aware of its parent so that it can be serialized properly.

	if ( type->Type == CT_Typename && array_expr && array_expr->Type != CT_Invalid )
		cast(CodeTypename, type)->ArrExpr = array_expr;

	if ( inline_cmt )
		result->InlineCmt = inline_cmt;

	parser_pop(& Context);
	return result;
}

internal neverinline
CodeUnion parser_parse_union( bool inplace_def )
{
	push_scope();

	ModuleFlag mflags = ModuleFlag_None;

	if ( check(Tok_Module_Export) )
	{
		mflags = ModuleFlag_Export;
		eat( Tok_Module_Export );
	}
	// <ModuleFlags>

	eat( Tok_Decl_Union );
	// <ModuleFlags> union

	CodeAttributes attributes = parse_attributes();
	// <ModuleFlags> union <Attributes>

	StrC name = { 0, nullptr };
	if ( check( Tok_Identifier ) )
{
		name = tok_to_str(currtok);
		Context.Scope->Name = currtok;
		eat( Tok_Identifier );
	}
	// <ModuleFlags> union <Attributes> <Name>

	CodeBody body = { nullptr };

	if ( ! inplace_def || ! check(Tok_Identifier) )
	{
		eat( Tok_BraceCurly_Open );
		// <ModuleFlags> union <Attributes> <Name> {

		body = cast(CodeBody, make_code());
		body->Type = CT_Union_Body;

		while ( ! check_noskip( Tok_BraceCurly_Close ) )
		{
			if ( currtok_noskip.Type == Tok_Preprocess_Hash )
				eat( Tok_Preprocess_Hash );

			Code member = { nullptr };
			switch ( currtok_noskip.Type )
			{
				case Tok_NewLine:
					member = fmt_newline;
					eat( Tok_NewLine );
				break;

				case Tok_Comment:
					member = cast(Code, parse_comment());
				break;

				// TODO(Ed) : Unions can have constructors and destructors

				case Tok_Decl_Class:
					member = parse_complicated_definition( Tok_Decl_Class );
				break;

				case Tok_Decl_Enum:
					member = parse_complicated_definition( Tok_Decl_Enum );
				break;

				case Tok_Decl_Struct:
					member = parse_complicated_definition( Tok_Decl_Struct );
				break;

				case Tok_Decl_Union:
					member = parse_complicated_definition( Tok_Decl_Union );
				break;

				case Tok_Preprocess_Define:
					member = cast(Code, parse_define());
				break;

				case Tok_Preprocess_If:
				case Tok_Preprocess_IfDef:
				case Tok_Preprocess_IfNotDef:
				case Tok_Preprocess_ElIf:
					member = cast(Code, parse_preprocess_cond());
				break;

				case Tok_Preprocess_Else:
					member = cast(Code, preprocess_else);
					eat( Tok_Preprocess_Else );
				break;

				case Tok_Preprocess_EndIf:
					member = cast(Code, preprocess_endif);
					eat( Tok_Preprocess_EndIf );
				break;

				case Tok_Preprocess_Macro:
					if ( nexttok.Type == Tok_Identifier ) {
						// Its a variable with a macro typename
						member = cast(Code, parser_parse_variable());
						break;
					}

					member = parse_simple_preprocess( Tok_Preprocess_Macro, parser_consume_braces );
				break;

				case Tok_Preprocess_Pragma:
					member = cast(Code, parse_pragma());
				break;

				case Tok_Preprocess_Unsupported:
					member = parse_simple_preprocess( Tok_Preprocess_Unsupported, parser_consume_braces );
				break;

				default:
					member = cast(Code, parser_parse_variable());
				break;
			}

			if ( member )
				body_append(body, member );
		}
		// <ModuleFlags> union <Attributes> <Name> { <Body>

		eat( Tok_BraceCurly_Close );
		// <ModuleFlags> union <Attributes> <Name> { <Body> }
	}

	if ( ! inplace_def )
		eat( Tok_Statement_End );
		// <ModuleFlags> union <Attributes> <Name> { <Body> };

	CodeUnion
	result = (CodeUnion) make_code();
	result->Type        = body ? CT_Union : CT_Union_Fwd;
	result->ModuleFlags = mflags;

	if ( name.Len )
		result->Name = get_cached_string( name );

	result->Body       = body;
	result->Attributes = attributes;

	parser_pop(& Context);
	return result;
}

internal
CodeUsing parser_parse_using()
{
	push_scope();

	Specifier specs_found[16] = { Spec_Invalid };
	s32        NumSpecifiers = 0;

	Token        name       = { nullptr, 0, Tok_Invalid };
	Code         array_expr = { nullptr };
	CodeTypename type       = { nullptr };

	bool is_namespace = false;

	ModuleFlag     mflags     = ModuleFlag_None;
	CodeAttributes attributes = { nullptr };

	if ( check(Tok_Module_Export) )
	{
		mflags = ModuleFlag_Export;
		eat( Tok_Module_Export );
	}
	// <ModuleFlags>

	eat( Tok_Decl_Using );
	// <ModuleFlags> using

	if ( currtok.Type == Tok_Decl_Namespace )
	{
		is_namespace = true;
		eat( Tok_Decl_Namespace );
		// <ModuleFlags> using namespace
	}

	name = currtok;
	Context.Scope->Name = name;
	eat( Tok_Identifier );
	// <ModuleFlags> using <namespace> <Name>

	if ( ! is_namespace )
	{
		if ( bitfield_is_equal( u32, currtok.Flags, TF_Assign ) )
		{
			attributes = parse_attributes();
			// <ModuleFlags> using <Name> <Attributes>

			eat( Tok_Operator );
			// <ModuleFlags> using <Name> <Attributes> =

			type = parser_parse_type(parser_not_from_template, nullptr);
			// <ModuleFlags> using <Name> <Attributes> = <UnderlyingType>

			array_expr = parse_array_decl();
			// <UnderlyingType> + <ArrExpr>
		}
	}

	Token stmt_end = currtok;
	eat( Tok_Statement_End );
	// <ModuleFlags> using <namespace> <Attributes> <Name> = <UnderlyingType>;

	CodeComment inline_cmt = NullCode;
	if ( currtok_noskip.Type == Tok_Comment && currtok_noskip.Line == stmt_end.Line )
	{
		inline_cmt = parse_comment();
	}
	// <ModuleFlags> using <namespace> <Attributes> <Name> = <UnderlyingType>; <InlineCmt>

	CodeUsing
	result              = (CodeUsing) make_code();
	result->Name        = get_cached_string( tok_to_str(name) );
	result->ModuleFlags = mflags;

	if ( is_namespace)
	{
		result->Type = CT_Using_Namespace;
	}
	else
	{
		result->Type = CT_Using;

		if ( type )
			result->UnderlyingType = type;

		if ( array_expr )
			type->ArrExpr = array_expr;

		if ( attributes )
			result->Attributes = attributes;

		if ( inline_cmt )
			result->InlineCmt = inline_cmt;
	}

	parser_pop(& Context);
	return result;
}

internal
CodeVar parser_parse_variable()
{
	push_scope();

	Specifier specs_found[16] = { Spec_NumSpecifiers };
	s32       NumSpecifiers = 0;

	ModuleFlag	   mflags     = ModuleFlag_None;
	CodeAttributes attributes = { nullptr };
	CodeSpecifiers specifiers = { nullptr };

	if ( check(Tok_Module_Export) )
	{
		mflags = ModuleFlag_Export;
		eat( Tok_Module_Export );
	}
	// <ModuleFlags>

	attributes = parse_attributes();
	// <ModuleFlags> <Attributes>

	while ( left && tok_is_specifier(currtok) )
	{
		Specifier spec = strc_to_specifier( tok_to_str(currtok) );
		switch  ( spec )
		{
			case Spec_Const:
			case Spec_Constexpr:
			case Spec_Constinit:
			case Spec_External_Linkage:
			case Spec_Global:
			case Spec_Inline:
			case Spec_Local_Persist:
			case Spec_Mutable:
			case Spec_Static:
			case Spec_Thread_Local:
			case Spec_Volatile:
			break;

			default:
				log_failure( "Invalid specifier %s for variable\n%s", spec_to_str( spec ), parser_to_string(Context) );
				parser_pop(& Context);
				return InvalidCode;
		}

		// Ignore const specifiers, they're handled by the type
		if ( spec == Spec_Const )
			break;

		specs_found[NumSpecifiers] = spec;
		NumSpecifiers++;
		eat( currtok.Type );
	}

	if ( NumSpecifiers )
	{
		specifiers = def_specifiers( NumSpecifiers, specs_found );
	}
	// <ModuleFlags> <Attributes> <Specifiers>

	CodeTypename type = parser_parse_type(parser_not_from_template, nullptr);
	// <ModuleFlags> <Attributes> <Specifiers> <ValueType>

	if ( cast(Code, type) == Code_Invalid )
		return InvalidCode;

	Context.Scope->Name = parse_identifier(nullptr);
	// <ModuleFlags> <Attributes> <Specifiers> <ValueType> <Name>

	CodeVar result = parse_variable_after_name( mflags, attributes, specifiers, type, tok_to_str(Context.Scope->Name) );
	// Regular  : <ModuleFlags> <Attributes> <Specifiers> <ValueType> <Name>                  = <Value>; <InlineCmt>
	// Bitfield : <ModuleFlags> <Attributes> <Specifiers> <ValueType> <Name> : <BitfieldSize> = <Value>; <InlineCmt>

	parser_pop(& Context);
	return result;
}


GEN_NS_PARSER_END

#ifdef CHECK_WAS_DEFINED
#pragma pop_macro("check")
#endif