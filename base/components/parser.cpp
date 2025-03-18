#ifdef INTELLISENSE_DIRECTIVES
#pragma once
#include "gen/etoktype.hpp"
#include "parser_case_macros.cpp"
#include "interface.upfront.cpp"
#include "lexer.cpp"
#endif

// TODO(Ed) : Rename ETok_Capture_Start, ETok_Capture_End to Open_Parenthesis adn Close_Parenthesis

constexpr bool lex_dont_skip_formatting = false;
constexpr bool      lex_skip_formatting = true;

void parser_push( ParseContext* ctx, ParseStackNode* node )
{
	node->Prev = ctx->Scope;
	ctx->Scope = node;

#if 0 && GEN_BUILD_DEBUG
	log_fmt("\tEntering parser: %.*s\n", Scope->ProcName.Len, Scope->ProcName.Ptr );
#endif
}

void parser_pop(ParseContext* ctx)
{
#if 0 && GEN_BUILD_DEBUG
	log_fmt("\tPopping parser: %.*s\n", Scope->ProcName.Len, Scope->ProcName.Ptr );
#endif
	ctx->Scope = ctx->Scope->Prev;
}

StrBuilder parser_to_strbuilder(ParseContext const* ctx, AllocatorInfo temp)
{
	StrBuilder result = strbuilder_make_reserve( temp, kilobytes(4) );

	Token scope_start = * ctx->Scope->Start;
	Token last_valid  = (ctx->token_id >= ctx->tokens.num) ? ctx->tokens[ctx->tokens.num -1] : (* lex_peek(ctx, true));

	sptr        length  = scope_start.Text.Len;
	char const* current = scope_start.Text.Ptr + length;
	while ( current <= ctx->tokens[ctx->tokens.num - 1].Text.Ptr && (* current) != '\n' && length < 74 )
	{
		current++;
		length++;
	}

	Str scope_str = { scope_start.Text.Ptr, length };
	StrBuilder line = strbuilder_make_str( temp, scope_str );
	strbuilder_append_fmt( & result, "\tScope    : %s\n", line );
	strbuilder_free(& line);

	sptr   dist            = (sptr)last_valid.Text.Ptr - (sptr)scope_start.Text.Ptr + 2;
	sptr   length_from_err = dist;

	Str        err_str       = { last_valid.Text.Ptr, length_from_err };
	StrBuilder line_from_err = strbuilder_make_str( temp, err_str );

	if ( length_from_err < 100 )
		strbuilder_append_fmt(& result, "\t(%d, %d):%*c\n", last_valid.Line, last_valid.Column, length_from_err, '^' );
	else
		strbuilder_append_fmt(& result, "\t(%d, %d)\n", last_valid.Line, last_valid.Column );

	ParseStackNode* curr_scope = ctx->Scope;
	s32 level = 0;
	do
	{
		if ( curr_scope->Name.Ptr ) {
			strbuilder_append_fmt(& result, "\t%d: %s, AST Name: %.*s\n", level, curr_scope->ProcName.Ptr, curr_scope->Name.Len, curr_scope->Name.Ptr );
		}
		else {
			strbuilder_append_fmt(& result, "\t%d: %s\n", level, curr_scope->ProcName.Ptr );
		}

		curr_scope = curr_scope->Prev;
		level++;
	}
	while ( curr_scope );
	return result;
}

bool lex__eat(Context* ctx, ParseContext* parser, TokType type)
{
	if ( parser->tokens.num - parser->token_id <= 0 ) {
		log_failure( "No tokens left.\n%s", parser_to_strbuilder(parser, ctx->Allocator_Temp) );
		return false;
	}

	Token at_idx = parser->tokens[ parser->token_id ];

	if ( ( at_idx.Type == Tok_NewLine && type != Tok_NewLine )
	||   ( at_idx.Type == Tok_Comment && type != Tok_Comment ) )
	{
		parser->token_id ++;
	}

	b32 not_accepted  = at_idx.Type != type;
	b32 is_identifier = at_idx.Type == Tok_Identifier;
	if ( not_accepted )
	{
		Macro* macro = lookup_macro(at_idx.Text);
		b32 accept_as_identifier = macro && bitfield_is_set(MacroFlags, macro->Flags, MF_Allow_As_Identifier );
		not_accepted             = type == Tok_Identifier && accept_as_identifier ? false : true;
	}
	if ( not_accepted )
	{
		Token tok = * lex_current( parser, lex_skip_formatting );
		log_failure( "Parse Error, TokArray::eat, Expected: ' %s ' not ' %.*s ' (%d, %d)`\n%s"
			, toktype_to_str(type).Ptr
			, at_idx.Text.Len, at_idx.Text.Ptr
			, tok.Line
			, tok.Column
			, parser_to_strbuilder(parser, ctx->Allocator_Temp)
		);
		GEN_DEBUG_TRAP();
		return false;
	}

#if 0 && GEN_BUILD_DEBUG
	log_fmt("Ate: %SB\n", self->Arr[Idx].to_strbuilder() );
#endif

	parser->token_id ++;
	return true;
}

internal
void parser_init(Context* ctx)
{
	ctx->Lexer_Tokens = array_init_reserve(Token, ctx->Allocator_DyanmicContainers, ctx->InitSize_LexerTokens );
}

internal
void parser_deinit(Context* ctx)
{
	Array(Token) null_array = { nullptr };
	ctx->Lexer_Tokens = null_array;
}

#pragma region Helper Macros

#define check_parse_args( def ) _check_parse_args(& ctx->parser, def, stringize(_func_) )
bool _check_parse_args(ParseContext* parser, Str def, char const* func_name )
{
	if ( def.Len <= 0 )
	{
		log_failure( c_str_fmt_buf("gen::%s: length must greater than 0", func_name) );
		parser_pop(parser);
		return false;
	}
	if ( def.Ptr == nullptr )
	{
		log_failure( c_str_fmt_buf("gen::%s: def was null", func_name) );
		parser_pop(parser);
		return false;
	}
	return true;
}

#	define currtok_noskip (* lex_current( &  ctx->parser, lex_dont_skip_formatting ))
#	define currtok        (* lex_current( & ctx->parser, lex_skip_formatting ))
#	define peektok        (* lex_peek(& ctx->parser, lex_skip_formatting))
#	define prevtok        (* lex_previous( & ctx->parser, lex_dont_skip_formatting))
#	define nexttok		  (* lex_next( & ctx->parser, lex_skip_formatting ))
#	define nexttok_noskip (* lex_next( & ctx->parser, lex_dont_skip_formatting))
#	define eat( Type_ )   lex__eat(ctx, & ctx->parser, Type_ )
#	define left           ( ctx->parser.tokens.num - ctx->parser.token_id )

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

#if GEN_COMPILER_CPP
#	define NullScope { nullptr, {nullptr, 0}, lex_current( &  ctx->parser, lex_dont_skip_formatting ), Str{nullptr, 0}, txt( __func__ ), { nullptr} }
#else
#	define NullScope (ParseStackNode){ nullptr, {nullptr, 0}, lex_current( &  ctx->parser.Tokens, lex_dont_skip_formatting ), (Str){nullptr, 0}, txt( __func__ ), { nullptr} }
#endif

#pragma endregion Helper Macros

// Procedure Forwards ( Entire parser internal parser interface )

internal Code               parse_array_decl                   (Context* ctx);
internal CodeAttributes     parse_attributes                   (Context* ctx);
internal CodeComment        parse_comment                      (Context* ctx);
internal Code               parse_complicated_definition       (Context* ctx, TokType which);
internal CodeBody           parse_class_struct_body            (Context* ctx, TokType which, Token name);
internal Code               parse_class_struct                 (Context* ctx, TokType which, bool inplace_def);
internal Code               parse_expression                   (Context* ctx);
internal Code               parse_forward_or_definition        (Context* ctx, TokType which, bool is_inplace);
internal CodeFn             parse_function_after_name          (Context* ctx, ModuleFlag mflags, CodeAttributes attributes, CodeSpecifiers specifiers, CodeTypename ret_type, Token name);
internal Code               parse_function_body                (Context* ctx);
internal CodeBody           parse_global_nspace                (Context* ctx, CodeType which);
internal Code               parse_global_nspace_constructor_destructor(Context* ctx, CodeSpecifiers specifiers);
internal Token              parse_identifier                   (Context* ctx, bool* possible_member_function);
internal CodeInclude        parse_include                      (Context* ctx);
internal Code               parse_macro_as_definiton           (Context* ctx, CodeAttributes attributes, CodeSpecifiers specifiers);
internal CodeOperator       parse_operator_after_ret_type      (Context* ctx, ModuleFlag mflags, CodeAttributes attributes, CodeSpecifiers specifiers, CodeTypename ret_type);
internal Code               parse_operator_function_or_variable(Context* ctx, bool expects_function, CodeAttributes attributes, CodeSpecifiers specifiers);
internal CodePragma         parse_pragma                       (Context* ctx);
internal CodeParams         parse_params                       (Context* ctx, bool use_template_capture);
internal CodePreprocessCond parse_preprocess_cond              (Context* ctx);
internal Code               parse_simple_preprocess            (Context* ctx, TokType which);
internal Code               parse_static_assert                (Context* ctx);
internal void               parse_template_args                (Context* ctx, Token* token );
internal CodeVar            parse_variable_after_name          (Context* ctx, ModuleFlag mflags, CodeAttributes attributes, CodeSpecifiers specifiers, CodeTypename type, Str name);
internal CodeVar            parse_variable_declaration_list    (Context* ctx);

internal CodeClass       parser_parse_class           (Context* ctx, bool inplace_def );
internal CodeConstructor parser_parse_constructor     (Context* ctx, CodeSpecifiers specifiers );
internal CodeDefine      parser_parse_define          (Context* ctx);
internal CodeDestructor  parser_parse_destructor      (Context* ctx, CodeSpecifiers specifiers );
internal CodeEnum        parser_parse_enum            (Context* ctx, bool inplace_def );
internal CodeBody        parser_parse_export_body     (Context* ctx);
internal CodeBody        parser_parse_extern_link_body(Context* ctx);
internal CodeExtern      parser_parse_extern_link     (Context* ctx);
internal CodeFriend      parser_parse_friend          (Context* ctx);
internal CodeFn          parser_parse_function        (Context* ctx);
internal CodeNS          parser_parse_namespace       (Context* ctx);
internal CodeOpCast      parser_parse_operator_cast   (Context* ctx, CodeSpecifiers specifiers );
internal CodeStruct      parser_parse_struct          (Context* ctx, bool inplace_def );
internal CodeVar         parser_parse_variable        (Context* ctx);
internal CodeTemplate    parser_parse_template        (Context* ctx);
internal CodeTypename    parser_parse_type            (Context* ctx, bool from_template, bool* is_function );
internal CodeTypedef     parser_parse_typedef         (Context* ctx);
internal CodeUnion       parser_parse_union           (Context* ctx, bool inplace_def );
internal CodeUsing       parser_parse_using           (Context* ctx);

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
StrBuilder parser_strip_formatting(Context* ctx, Str raw_text, bool preserve_newlines )
{
	StrBuilder content = strbuilder_make_reserve( ctx->Allocator_Temp, raw_text.Len );

	if ( raw_text.Len == 0 )
		return content;

#define cut_length ( scanner - raw_text.Ptr - last_cut )
#define cut_ptr    ( raw_text.Ptr   + last_cut )
#define pos        ( rcast( sptr, scanner ) - rcast( sptr, raw_text.Ptr ) )
#define move_fwd() do { scanner++; tokleft--; } while(0)

	s32         tokleft  = raw_text.Len;
	sptr        last_cut = 0;
	char const* scanner  = raw_text.Ptr;

	if ( scanner[0] == ' ' ) {
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

			strbuilder_append_c_str_len( & content, cut_ptr, cut_length );
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

			strbuilder_append_c_str_len( & content, cut_ptr, cut_length );
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

			strbuilder_append_c_str_len( & content,  cut_ptr, cut_length );
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

			strbuilder_append_c_str_len( & content,  cut_ptr, cut_length );
			last_cut = rcast( sptr,  scanner ) - rcast( sptr, raw_text.Ptr );
			continue;
		}

		// Tabs
		if (scanner[0] == '\t')
		{
			if (pos > last_cut)
				strbuilder_append_c_str_len( & content, cut_ptr, cut_length);

			if ( * strbuilder_back( content ) != ' ' )
				strbuilder_append_char( & content, ' ' );

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

				strbuilder_append_c_str_len( & content,  cut_ptr, cut_length );
				last_cut = rcast( sptr, scanner ) - rcast( sptr, raw_text.Ptr );
				continue;
			}

			if ( pos > last_cut )
				strbuilder_append_c_str_len( & content,  cut_ptr, cut_length );

			// Replace with a space
			if ( * strbuilder_back( content ) != ' ' )
				strbuilder_append_char( & content,  ' ' );

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

				strbuilder_append_c_str_len( & content,  cut_ptr, cut_length );
				last_cut = rcast( sptr, scanner ) - rcast( sptr, raw_text.Ptr );
				continue;
			}

			if ( pos > last_cut )
				strbuilder_append_c_str_len( & content,  cut_ptr, cut_length );

			// Replace with a space
			if ( * strbuilder_back( content ) != ' ' )
				strbuilder_append_char( & content,  ' ' );

			move_fwd();

			last_cut = rcast( sptr, scanner ) - rcast( sptr, raw_text.Ptr );
			continue;
		}

		// Escaped newlines
		if ( scanner[0] == '\\' )
		{
			strbuilder_append_c_str_len( & content,  cut_ptr, cut_length );

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
			strbuilder_append_c_str_len( & content,  cut_ptr, cut_length );
			do
			{
				move_fwd();
			}
			while ( tokleft && char_is_space( scanner[0] ) );

			last_cut = rcast( sptr, scanner ) - rcast( sptr, raw_text.Ptr );

			// Preserve only 1 space of formattting
			char* last = strbuilder_back(content);
			if ( last == nullptr || * last != ' ' )
				strbuilder_append_char( & content, ' ' );

			continue;
		}

		move_fwd();
	}

	if ( last_cut < raw_text.Len ) {
		strbuilder_append_c_str_len( & content,  cut_ptr, raw_text.Len - last_cut );
	}

#undef cut_ptr
#undef cut_length
#undef pos
#undef move_fwd

	return content;
}

StrBuilder parser_strip_formatting_2(TokenSlice tokens)
{
	// TODO(Ed): Use this to produce strings for validation purposes. We shouldn't serialize down from tokens once we start storing token slices for content.
	StrBuilder result = struct_zero(StrBuilder);
	return result;
}

internal
Code parse_array_decl(Context* ctx)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	if ( check( Tok_Operator ) && currtok.Text.Ptr[0] == '[' && currtok.Text.Ptr[1] == ']' )
	{
		Code array_expr = untyped_str( txt(" ") );
		eat( Tok_Operator );
		// []

		parser_pop(& ctx->parser);
		return array_expr;
	}

	if ( check( Tok_BraceSquare_Open ) )
	{
		eat( Tok_BraceSquare_Open );
		// [

		if ( left == 0 )
		{
			log_failure( "Error, unexpected end of array declaration ( '[]' scope started )\n%s", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
			parser_pop(& ctx->parser);
			return InvalidCode;
		}

		if ( currtok.Type == Tok_BraceSquare_Close )
		{
			log_failure( "Error, empty array expression in definition\n%s", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
			parser_pop(& ctx->parser);
			return InvalidCode;
		}

		TokenSlice tokens = { & currtok, 1 };
		Token untyped_tok = currtok;

		while ( left && currtok.Type != Tok_BraceSquare_Close )
		{
			eat( currtok.Type );
			++ tokens.num;
		}

		// untyped_tok.Text.Len = ( (sptr)prevtok.Text.Ptr + prevtok.Text.Len ) - (sptr)untyped_tok.Text.Ptr;
		untyped_tok.Text = token_range_to_str(untyped_tok, prevtok);

		Code array_expr = untyped_str( untyped_tok.Text );
		// Code array_expr = untyped_toks( tokens ); // TODO(Ed): Use token slice instead of untyped strings.
		// [ <Content>

		if ( left == 0 )
		{
			log_failure( "Error, unexpected end of array declaration, expected ]\n%s", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
			parser_pop(& ctx->parser);
			return InvalidCode;
		}

		if ( currtok.Type != Tok_BraceSquare_Close )
		{
			log_failure( "%s: Error, expected ] in array declaration, not %s\n%s", toktype_to_str( currtok.Type ), parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
			parser_pop(& ctx->parser);
			return InvalidCode;
		}

		eat( Tok_BraceSquare_Close );
		// [ <Content> ]

		// Its a multi-dimensional array
		if ( check( Tok_BraceSquare_Open ))
		{
			Code adjacent_arr_expr = parse_array_decl(ctx);
			// [ <Content> ][ <Content> ]...

			array_expr->Next = adjacent_arr_expr;
		}

		parser_pop(& ctx->parser);
		return array_expr;
	}

	parser_pop(& ctx->parser);
	return NullCode;
}

internal inline
CodeAttributes parse_attributes(Context* ctx)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope);

	Token start = currtok;
	s32   len   = 0;

	// There can be more than one attribute. If there is flatten them to a single string.
	// TODO(Ed): Support chaining attributes (Use parameter linkage pattern)
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

			len = ( ( sptr )prevtok.Text.Ptr + prevtok.Text.Len ) - ( sptr )start.Text.Ptr;
		}
		else if ( check( Tok_Decl_GNU_Attribute ) )
		{
			eat( Tok_Decl_GNU_Attribute );
			eat( Tok_Paren_Open );
			eat( Tok_Paren_Open );
			// __attribute__((

			while ( left && currtok.Type != Tok_Paren_Close )
			{
				eat( currtok.Type );
			}
			// __attribute__(( <Content>

			eat( Tok_Paren_Close );
			eat( Tok_Paren_Close );
			// __attribute__(( <Content> ))

			len = ( ( sptr )prevtok.Text.Ptr + prevtok.Text.Len ) - ( sptr )start.Text.Ptr;
		}
		else if ( check( Tok_Decl_MSVC_Attribute ) )
		{
			eat( Tok_Decl_MSVC_Attribute );
			eat( Tok_Paren_Open );
			// __declspec(

			while ( left && currtok.Type != Tok_Paren_Close )
			{
				eat( currtok.Type );
			}
			// __declspec( <Content>

			eat( Tok_Paren_Close );
			// __declspec( <Content> )

			len = ( ( sptr )prevtok.Text.Ptr + prevtok.Text.Len ) - ( sptr )start.Text.Ptr;
		}
		else if ( tok_is_attribute(currtok) )
		{
			eat( currtok.Type );
			// <Attribute>

			// If its a macro based attribute, this could be a functional macro such as Unreal's UE_DEPRECATED(...)
			if ( check( Tok_Paren_Open))
			{
				eat( Tok_Paren_Open );

				s32 level = 0;
				while (left && currtok.Type != Tok_Paren_Close && level == 0)
				{
					if (currtok.Type == Tok_Paren_Open)
						++ level;
					if (currtok.Type == Tok_Paren_Close)
						--level;
					eat(currtok.Type);
				}
				eat(Tok_Paren_Close);
			}

			len = ( ( sptr )prevtok.Text.Ptr + prevtok.Text.Len ) - ( sptr )start.Text.Ptr;
			// <Attribute> ( ... )
		}
	}

	if ( len > 0 )
	{
		Str attribute_txt = { start.Text.Ptr, len };
		parser_pop(& ctx->parser);

		StrBuilder name_stripped = parser_strip_formatting(ctx, attribute_txt, parser_strip_formatting_dont_preserve_newlines );

		Code result     = make_code();
		result->Type    = CT_PlatformAttributes;
		result->Name    = cache_str( strbuilder_to_str(name_stripped) );
		result->Content = result->Name;
		// result->Token   =
		return ( CodeAttributes )result;
	}

	parser_pop(& ctx->parser);
	return NullCode;
}

internal
Code parse_class_struct(Context* ctx, TokType which, bool inplace_def)
{
	if ( which != Tok_Decl_Class && which != Tok_Decl_Struct ) {
		log_failure( "Error, expected class or struct, not %s\n%s", toktype_to_str( which ), parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
		return InvalidCode;
	}

	Token name = NullToken;

	AccessSpec     access     = AccessSpec_Default;
	CodeTypename   parent     = { nullptr };
	CodeBody       body       = { nullptr };
	CodeAttributes attributes = { nullptr };
	ModuleFlag     mflags     = ModuleFlag_None;

	Code result = InvalidCode;

	if ( check(Tok_Module_Export) ) {
		mflags = ModuleFlag_Export;
		eat( Tok_Module_Export );
	}
	// <ModuleFlags>

	eat( which );
	// <ModuleFlags> <class/struct>

	attributes = parse_attributes(ctx);
	// <ModuleFlags> <class/struct> <Attributes>

	if ( check( Tok_Identifier ) ) {
		name = parse_identifier(ctx, nullptr);
		ctx->parser.Scope->Name = name.Text;
	}
	// <ModuleFlags> <class/struct> <Attributes> <Name>

	CodeSpecifiers specifiers = NullCode;
	if ( check(Tok_Spec_Final)) {
		specifiers = def_specifier(Spec_Final);
		eat(Tok_Spec_Final);
	}
	// <ModuleFlags> <class/struct> <Attributes> <Name> <final>

	local_persist
	char interface_arr_mem[ kilobytes(4) ] = {0};
	Array(CodeTypename) interfaces = {nullptr};

	// TODO(Ed) : Make an AST_DerivedType, we'll store any arbitary derived type into there as a linear linked list of them.
	if ( check( Tok_Assign_Classifer ) )
	{
		eat( Tok_Assign_Classifer );
		// <ModuleFlags> <class/struct> <Attributes> <Name> <final> :

		if ( tok_is_access_specifier(currtok) ) {
			access = tok_to_access_specifier(currtok);
			// <ModuleFlags> <class/struct> <Attributes> <Name> <final> : <Access Specifier>
			eat( currtok.Type );
		}

		Token parent_tok = parse_identifier(ctx, nullptr);
		parent = def_type( parent_tok.Text );
		// <ModuleFlags> <class/struct> <Attributes> <Name> <final> : <Access Specifier> <Parent/Interface Name>

		if (check(Tok_Comma))
		{
			Arena arena = arena_init_from_memory( interface_arr_mem, kilobytes(4) );
			interfaces  = array_init_reserve(CodeTypename, arena_allocator_info(& arena), 4 );
			do
			{
				eat( Tok_Comma );
				// <ModuleFlags> <class/struct> <Attributes> <Name> <final> : <Access Specifier> <Name>,

				if ( tok_is_access_specifier(currtok) ) {
					eat(currtok.Type);
				}
				Token interface_tok = parse_identifier(ctx, nullptr);

				array_append( interfaces, def_type( interface_tok.Text ) );
				// <ModuleFlags> <class/struct> <Attributes> <Name> <final> : <Access Specifier> <Name>, ...
			}
			while ( check(Tok_Comma) );
		}
	}

	if ( check( Tok_BraceCurly_Open ) ) {
		body = parse_class_struct_body( ctx, which, name );
	}
	// <ModuleFlags> <class/struct> <Attributes> <Name> <final> : <Access Specifier> <Name>, ... { <Body> }

	CodeComment inline_cmt = NullCode;
	if ( ! inplace_def )
	{
		Token stmt_end = currtok;
		eat( Tok_Statement_End );
		// <ModuleFlags> <class/struct> <Attributes> <Name> <final> : <Access Specifier> <Name>, ... { <Body> };

		if ( currtok_noskip.Type == Tok_Comment && currtok_noskip.Line == stmt_end.Line )
			inline_cmt = parse_comment(ctx);
		// <ModuleFlags> <class/struct> <Attributes> <Name> <final> : <Access Specifier> <Name>, ... { <Body> }; <InlineCmt>
	}

	s32 num_interfaces = scast(s32, interfaces ? array_num(interfaces) : 0);

	if ( which == Tok_Decl_Class )
		result = cast(Code, def_class( name.Text, def_assign( body, parent, access, attributes, interfaces, num_interfaces, specifiers, mflags ) ));

	else
		result = cast(Code, def_struct( name.Text, def_assign( body, (CodeTypename)parent, access, attributes, interfaces, num_interfaces, specifiers, mflags ) ));

	if ( inline_cmt )
		result->InlineCmt = cast(Code, inline_cmt);

	if (interfaces)
		array_free(interfaces);
	return result;
}

internal neverinline
CodeBody parse_class_struct_body(Context* ctx, TokType which, Token name)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

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

		// ctx->parser.Scope->Start = currtok_noskip;

		if ( currtok_noskip.Type == Tok_Preprocess_Hash )
			eat( Tok_Preprocess_Hash );

		switch ( currtok_noskip.Type )
		{
			case Tok_Statement_End: {
				// TODO(Ed): Convert this to a general warning procedure
				log_fmt("Dangling end statement found %SB\n", tok_to_strbuilder(currtok_noskip));
				eat( Tok_Statement_End );
				continue;
			}
			case Tok_NewLine: {
				member = fmt_newline;
				eat( Tok_NewLine );
				break;
			}
			case Tok_Comment: {
				member = cast(Code, parse_comment(ctx));
				break;
			}
			case Tok_Access_Public: {
				member = access_public;
				eat( Tok_Access_Public );
				eat( Tok_Assign_Classifer );
				// public:
				break;
			}
			case Tok_Access_Protected: {
				member = access_protected;
				eat( Tok_Access_Protected );
				eat( Tok_Assign_Classifer );
				// protected:
				break;
			}
			case Tok_Access_Private: {
				member = access_private;
				eat( Tok_Access_Private );
				eat( Tok_Assign_Classifer );
				// private:
				break;
			}
			case Tok_Decl_Class: {
				member = parse_complicated_definition(ctx, Tok_Decl_Class );
				// class
				break;
			}
			case Tok_Decl_Enum: {
				member = parse_complicated_definition(ctx, Tok_Decl_Enum );
				// enum
				break;
			}
			case Tok_Decl_Friend: {
				member = cast(Code, parser_parse_friend(ctx));
				// friend
				break;
			}
			case Tok_Decl_Operator: {
				member = cast(Code, parser_parse_operator_cast(ctx, NullCode));
				// operator <Type>()
				break;
			}
			case Tok_Decl_Struct: {
				member = parse_complicated_definition(ctx, Tok_Decl_Struct );
				// struct
				break;
			}
			case Tok_Decl_Template: {
				member = cast(Code, parser_parse_template(ctx));
				// template< ... >
				break;
			}
			case Tok_Decl_Typedef: {
				member = cast(Code, parser_parse_typedef(ctx));
				// typedef
				break;
			}
			case Tok_Decl_Union: {
				member = parse_complicated_definition(ctx, Tok_Decl_Union );
				// union
				break;
			}
			case Tok_Decl_Using: {
				member = cast(Code, parser_parse_using(ctx));
				// using
				break;
			}
			case Tok_Operator:
			{
				//if ( currtok.Text[0] != '~' )
				//{
				//	log_failure( "Operator token found in global body but not destructor unary negation\n%s", to_strbuilder(ctx->parser) );
				//	return InvalidCode;
				//}

				member = cast(Code, parser_parse_destructor(ctx, NullCode));
				// ~<Name>()
				break;
			}
			case Tok_Preprocess_Define: {
				member = cast(Code, parser_parse_define(ctx));
				// #define
				break;
			}
			case Tok_Preprocess_Include:
			{
				member = cast(Code, parse_include(ctx));
				// #include
				break;
			}

			case Tok_Preprocess_If:
			case Tok_Preprocess_IfDef:
			case Tok_Preprocess_IfNotDef:
			case Tok_Preprocess_ElIf:
				member = cast(Code, parse_preprocess_cond(ctx));
				// #<Condition>
			break;

			case Tok_Preprocess_Else: {
				member = cast(Code, preprocess_else);
				eat( Tok_Preprocess_Else );
				// #else
				break;
			}
			case Tok_Preprocess_EndIf: {
				member = cast(Code, preprocess_endif);
				eat( Tok_Preprocess_EndIf );
				// #endif
				break;
			}

			case Tok_Preprocess_Macro_Stmt: {
				member = cast(Code, parse_simple_preprocess(ctx, Tok_Preprocess_Macro_Stmt ));
				break;
			}

			// case Tok_Preprocess_Macro:
			// 	// <Macro>
			// 	macro_found = true;
			// 	goto Preprocess_Macro_Bare_In_Body;
			// break;

			case Tok_Preprocess_Pragma: {
				member = cast(Code, parse_pragma(ctx));
				// #pragma
				break;
			}

			case Tok_Preprocess_Unsupported: {
				member = cast(Code, parse_simple_preprocess(ctx, Tok_Preprocess_Unsupported));
				// #<UNKNOWN>
				break;
			}

			case Tok_StaticAssert: {
				member = parse_static_assert(ctx);
				// static_assert
				break;
			}

			case Tok_Preprocess_Macro_Expr:
			{
				if ( ! tok_is_attribute(currtok))
				{
					log_failure("Unbounded macro expression residing in class/struct body\n%S", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp));
					return InvalidCode;
				}
			}
			//! Fallthrough intended
			case Tok_Attribute_Open:
			case Tok_Decl_GNU_Attribute:
			case Tok_Decl_MSVC_Attribute:
		#define Entry( attribute, str ) case attribute:
			GEN_DEFINE_ATTRIBUTE_TOKENS
		#undef Entry
			{
				attributes = parse_attributes(ctx);
				// <Attributes>
			}
			//! Fallthrough intended
			GEN_PARSER_CLASS_STRUCT_BODY_ALLOWED_MEMBER_TOK_SPECIFIER_CASES:
			{
				Specifier specs_found[16] = { Spec_NumSpecifiers };
				s32        NumSpecifiers = 0;

				while ( left && tok_is_specifier(currtok) )
				{
					Specifier spec = str_to_specifier( currtok.Text );

					b32 ignore_spec = false;

					switch ( spec )
					{
						GEN_PARSER_CLASS_STRUCT_BODY_ALLOWED_MEMBER_SPECIFIER_CASES:
						break;

						case Spec_Consteval:
							expects_function = true;
						break;

						case Spec_Const :
							ignore_spec = true;
						break;

						default:
							log_failure( "Invalid specifier %S for class/struct member\n%S", spec_to_str(spec), strbuilder_to_str( parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp)) );
							parser_pop(& ctx->parser);
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
					specifiers = def_specifiers_arr( NumSpecifiers, specs_found );
				}
				// <Attributes> <Specifiers>

				if ( tok_is_attribute(currtok) )
				{
					// Unfortuantely Unreal has code where there is attirbutes before specifiers
					CodeAttributes more_attributes = parse_attributes(ctx);

					if ( attributes )
					{
						StrBuilder fused = strbuilder_make_reserve( ctx->Allocator_Temp, attributes->Content.Len + more_attributes->Content.Len );
						strbuilder_append_fmt( & fused, "%SB %SB", attributes->Content, more_attributes->Content );

						Str attrib_name     = strbuilder_to_str(fused);
						attributes->Name    = cache_str( attrib_name );
						attributes->Content = attributes->Name;
						// <Attributes> <Specifiers> <Attributes>
					}

					attributes = more_attributes;
				}

				if ( currtok.Type == Tok_Operator && currtok.Text.Ptr[0] == '~' )
				{
					member = cast(Code, parser_parse_destructor(ctx, specifiers ));
					// <Attribute> <Specifiers> ~<Name>()
					break;
				}

				if ( currtok.Type == Tok_Decl_Operator )
				{
					member = cast(Code, parser_parse_operator_cast(ctx, specifiers ));
					// <Attributes> <Specifiers> operator <Type>()
					break;
				}
			}
			//! Fallthrough intentional
			case Tok_Identifier:
			case Tok_Preprocess_Macro_Typename:
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
				if ( nexttok.Type == Tok_Paren_Open && name.Text.Len && currtok.Type == Tok_Identifier )
				{
					if ( c_str_compare_len( name.Text.Ptr, currtok.Text.Ptr, name.Text.Len ) == 0 )
					{
						member = cast(Code, parser_parse_constructor(ctx, specifiers ));
						// <Attributes> <Specifiers> <Name>()
						break;
					}
				}

				member = parse_operator_function_or_variable(ctx, expects_function, attributes, specifiers );
				// <Attributes> <Specifiers> operator <Op> ...
				// or
				// <Attributes> <Specifiers> <Name> ...
			}
			break;

			default:
				Token untyped_tok = currtok;
				while ( left && currtok.Type != Tok_BraceCurly_Close )
				{
					untyped_tok.Text.Len = ( (sptr)currtok.Text.Ptr + currtok.Text.Len ) - (sptr)untyped_tok.Text.Ptr;
					eat( currtok.Type );
				}
				member = untyped_str( untyped_tok.Text );
				// Something unknown
			break;
		}

		if ( member == Code_Invalid )
		{
			log_failure( "Failed to parse member\n%s", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
			parser_pop(& ctx->parser);
			return InvalidCode;
		}
		body_append(result, member );
	}

	eat( Tok_BraceCurly_Close );
	// { <Members> }
	parser_pop(& ctx->parser);
	return result;
}

internal
CodeComment parse_comment(Context* ctx)
{	
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	CodeComment
	result          = (CodeComment) make_code();
	result->Type    = CT_Comment;
	result->Content = cache_str( currtok_noskip.Text );
	// result->Token   = currtok_noskip;
	eat( Tok_Comment );

	parser_pop(& ctx->parser);
	return result;
}

internal
Code parse_complicated_definition(Context* ctx, TokType which)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	b32 is_inplace = false;
	b32 is_fn_def  = false;

	TokenSlice tokens = ctx->parser.tokens;

	s32 idx         = ctx->parser.token_id;
	s32 level       = 0;
	b32 had_def     = false;
	b32 had_paren   = false;
	for ( ; idx < tokens.num; idx++ )
	{
		if ( tokens[ idx ].Type == Tok_BraceCurly_Open )
			level++;

		if ( tokens[ idx ].Type == Tok_BraceCurly_Close ) {
			level--;
			had_def = level == 0;
		}

		b32 found_fn_def = had_def && had_paren;

		if ( level == 0 && (tokens[ idx ].Type == Tok_Statement_End || found_fn_def) )
			break;
	}

	is_fn_def = had_def && had_paren;
	if (is_fn_def)
	{
		// Function definition with <which> on return type
		Code result = parse_operator_function_or_variable(ctx, false, NullCode, NullCode);
		// <which> <typename>(...) ... { ... }
		parser_pop(& ctx->parser);
		return result;
	}

	if ( ( idx - 2 ) == ctx->parser.token_id )
	{
		// It's a forward declaration only
		Code result = parse_forward_or_definition(ctx, which, is_inplace );
		// <class, enum, struct, or union> <Name>;
		parser_pop(& ctx->parser);
		return result;
	}

	Token tok = tokens[ idx - 1 ];
	if ( tok_is_specifier(tok) && spec_is_trailing( str_to_specifier( tok.Text)) )
	{
		// <which> <type_identifier>(...) <specifier> ...;

		s32   spec_idx = idx - 1;
		Token spec     = tokens[spec_idx];
		while ( tok_is_specifier(spec) && spec_is_trailing( str_to_specifier( spec.Text)) )
		{
			-- spec_idx;
			spec = tokens[spec_idx];
		}

		if ( tokens[spec_idx].Type == Tok_Paren_Close )
		{
			// Forward declaration with trailing specifiers for a procedure
			tok = tokens[spec_idx];

			Code result = parse_operator_function_or_variable(ctx, false, NullCode, NullCode);
			// <Attributes> <Specifiers> <ReturnType/ValueType> <operator <Op>, or Name> ...
			parser_pop(& ctx->parser);
			return result;
		}

		log_failure( "Unsupported or bad member definition after %s declaration\n%s", toktype_to_str(which), parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
		parser_pop(& ctx->parser);
		return InvalidCode;
	}
	if ( tok.Type == Tok_Identifier )
	{
		tok = tokens[ idx - 2 ];
		bool is_indirection = tok.Type == Tok_Ampersand || tok.Type == Tok_Star;
		bool ok_to_parse    = false;

		if ( tok.Type == Tok_BraceCurly_Close )
		{
			// Its an inplace definition
			// <which> <type_identifier ?> { ... } <identifier>;
			ok_to_parse = true;
			is_inplace  = true;

			CodeTypename type = cast(CodeTypename, parse_forward_or_definition(ctx, which, is_inplace));

			// Should be a name right after the type.
			Token name = parse_identifier(ctx, nullptr);
			ctx->parser.Scope->Name = name.Text;

			CodeVar result = parse_variable_after_name(ctx, ModuleFlag_None, NullCode, NullCode, type, name.Text);
			parser_pop(& ctx->parser);
			return (Code) result;
		}
		else if ( tok.Type == Tok_Identifier && tokens[ idx - 3 ].Type == which )
		{
			// Its a variable with type ID using <which> namespace.
			// <which> <type_identifier> <identifier>;
			ok_to_parse = true;
		}
		else if ( tok.Type == Tok_Assign_Classifer
		&& (	( tokens[idx - 5].Type == which && tokens[idx - 4].Type == Tok_Decl_Class )
			||	( tokens[idx - 4].Type == which))
		)
		{
			// Its a forward declaration of an enum
			// <enum>         <type_identifier> : <identifier>;
			// <enum> <class> <type_identifier> : <identifier>;
			ok_to_parse = true;
			Code result = cast(Code, parser_parse_enum(ctx, ! parser_inplace_def));
			parser_pop(& ctx->parser);
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
			log_failure( "Unsupported or bad member definition after %s declaration\n%s", toktype_to_str(which), parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
			parser_pop(& ctx->parser);
			return InvalidCode;
		}

		Code result = parse_operator_function_or_variable(ctx, false, NullCode, NullCode );
		// <Attributes> <Specifiers> <ReturnType/ValueType> <operator <Op>, or Name> ...
		parser_pop(& ctx->parser);
		return result;
	}
	else if ( tok.Type >= Tok_Type_Unsigned && tok.Type <= Tok_Type_MS_W64 )
	{
		tok = tokens[ idx - 2 ];

		if ( tok.Type != Tok_Assign_Classifer
		|| (	( tokens[idx - 5].Type != which && tokens[idx - 4].Type != Tok_Decl_Class )
			&&	( tokens[idx - 4].Type != which))
		)
		{
			log_failure( "Unsupported or bad member definition after %s declaration\n%s", toktype_to_str(which), parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
			parser_pop(& ctx->parser);
			return InvalidCode;
		}

		// Its a forward declaration of an enum class
		// <enum>         <type_identifier> : <identifier>;
		// <enum> <class> <type_identifier> : <identifier>;
		Code result = cast(Code, parser_parse_enum(ctx, ! parser_inplace_def));
		parser_pop(& ctx->parser);
		return result;
	}
	else if ( tok.Type == Tok_BraceCurly_Close )
	{
		// Its a definition
		Code result = parse_forward_or_definition(ctx, which, is_inplace );
		// <which> { ... };
		parser_pop(& ctx->parser);
		return result;
	}
	else if ( tok.Type == Tok_BraceSquare_Close )
	{
		// Its an array definition
		Code result = parse_operator_function_or_variable(ctx, false, NullCode, NullCode );
		// <which> <type_identifier> <identifier> [ ... ];
		parser_pop(& ctx->parser);
		return result;
	}
	else
	{
		log_failure( "Unsupported or bad member definition after %s declaration\n%SB", toktype_to_str(which).Ptr, parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
		parser_pop(& ctx->parser);
		return InvalidCode;
	}
}

internal inline
Code parse_assignment_expression(Context* ctx)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	Code expr = { nullptr };

	eat( Tok_Operator );
	// <Attributes> <Specifiers> <ValueType> <Name> =

	Token expr_tok = currtok;

	if ( currtok.Type == Tok_Statement_End && currtok.Type != Tok_Comma )
	{
		log_failure( "Expected expression after assignment operator\n%s", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
		parser_pop(& ctx->parser);
		return InvalidCode;
	}

	s32 level = 0;
	while ( left && currtok.Type != Tok_Statement_End && (currtok.Type != Tok_Comma || level > 0) )
	{
		if (currtok.Type == Tok_BraceCurly_Open )
			level++;
		if (currtok.Type == Tok_BraceCurly_Close )
			level--;
		if (currtok.Type == Tok_Paren_Open)
			level++;
		else if (currtok.Type == Tok_Paren_Close)
			level--;

		eat( currtok.Type );
	}

	if (left) {
		expr_tok.Text.Len = ( ( sptr )currtok.Text.Ptr + currtok.Text.Len ) - ( sptr )expr_tok.Text.Ptr - 1;
	}
	expr = untyped_str( expr_tok.Text );
	// = <Expression>

	parser_pop(& ctx->parser);
	return expr;
}

internal inline
Code parse_forward_or_definition(Context* ctx, TokType which, bool is_inplace )
{
	Code result = InvalidCode;

	switch ( which )
	{
		case Tok_Decl_Class:
			result = cast(Code, parser_parse_class(ctx, is_inplace ));
			return result;

		case Tok_Decl_Enum:
			result = cast(Code, parser_parse_enum(ctx, is_inplace ));
			return result;

		case Tok_Decl_Struct:
			result = cast(Code, parser_parse_struct(ctx, is_inplace ));
			return result;

		case Tok_Decl_Union:
			result = cast(Code, parser_parse_union(ctx, is_inplace ));
			return result;

		default:
			log_failure( "Error, wrong token type given to parse_complicated_definition "
				"(only supports class, enum, struct, union) \n%s"
				, parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );

			return InvalidCode;
	}
}

// Function parsing is handled in multiple places because its initial signature is shared with variable parsing
internal inline
CodeFn parse_function_after_name(Context* ctx
	, ModuleFlag     mflags
	, CodeAttributes attributes
	, CodeSpecifiers specifiers
	, CodeTypename   ret_type
	, Token          name
)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	CodeParams params = parse_params(ctx, parser_use_parenthesis);
	// <Attributes> <Specifiers> <ReturnType> <Name> ( <Parameters> )

	Code suffix_specs = NullCode;

	// TODO(Ed), Review old comment : These have to be kept separate from the return type's specifiers.
	while ( left && tok_is_specifier(currtok) )
	{
		// For Unreal's PURE_VIRTUAL Support
		Macro* macro = lookup_macro( currtok.Text );
		if (macro && tok_is_specifier(currtok))
		{
			suffix_specs = parse_simple_preprocess(ctx, Tok_Preprocess_Macro_Expr);
			continue;
		}
		if ( specifiers == nullptr )
		{
			specifiers = def_specifier( str_to_specifier( currtok.Text) );
			eat( currtok.Type );
			continue;
		}

		specifiers_append(specifiers, str_to_specifier( currtok.Text) );
		eat( currtok.Type );
	}
	// <Attributes> <Specifiers> <ReturnType> <Name> ( <Paraemters> ) <Specifiers>

	// Check for trailing specifiers...
	CodeAttributes post_rt_attributes = parse_attributes(ctx);
	if (post_rt_attributes)
	{
		if (attributes)
		{
			StrBuilder merged   =  strbuilder_fmt_buf(ctx->Allocator_Temp, "%S %S", attributes->Content, post_rt_attributes->Content);
			attributes->Content = cache_str(strbuilder_to_str(merged));
		}
		else
		{
			attributes = post_rt_attributes;
		}
	}
	// <Attributes> <Specifiers> <ReturnType> <Name> ( <Paraemters> ) <Specifiers> <Attributes> 

	CodeBody    body       = NullCode;
	CodeComment inline_cmt = NullCode;
	if ( check( Tok_BraceCurly_Open ) )
	{
		body = cast(CodeBody, parse_function_body(ctx));
		if ( cast(Code, body) == Code_Invalid )
		{
			parser_pop(& ctx->parser);
			return InvalidCode;
		}
		// <Attributes> <Specifiers> <ReturnType> <Name> ( <Paraemters> ) <Specifiers> { <Body> }
	}
	else if ( check(Tok_Operator) && currtok.Text.Ptr[0] == '=' )
	{
		eat(Tok_Operator);
		if ( specifiers == nullptr )
		{
			specifiers       = (CodeSpecifiers) make_code();
			specifiers->Type = CT_Specifiers;
		}
		if ( str_are_equal(nexttok.Text, txt("delete")))
		{
			specifiers_append(specifiers, Spec_Delete);
			eat(currtok.Type);	
			// <Attributes> <Specifiers> <ReturnType> <Name> ( <Paraemters> ) <Specifiers> = delete
		}
		else
		{
			specifiers_append(specifiers, Spec_Pure );

			eat( Tok_Number);
			// <Attributes> <Specifiers> <ReturnType> <Name> ( <Paraemters> ) <Specifiers> = 0
		}
		Token stmt_end = currtok;
		eat( Tok_Statement_End );
		
		if ( currtok_noskip.Type == Tok_Comment && currtok_noskip.Line == stmt_end.Line )
			inline_cmt = parse_comment(ctx);
			// <Attributes> <Specifiers> <ReturnType> <Name> ( <Paraemters> ) <Specifiers> < = 0 or delete > ; <InlineCmt>
	}


	if (body == nullptr)
	{
		Token stmt_end = currtok;
		eat( Tok_Statement_End );
		// <Attributes> <Specifiers> <ReturnType> <Name> ( <Paraemters> ) <Specifiers> <Attributes> < = 0 or delete > ;

		if ( currtok_noskip.Type == Tok_Comment && currtok_noskip.Line == stmt_end.Line )
			inline_cmt = parse_comment(ctx);
			// <Attributes> <Specifiers> <ReturnType> <Name> ( <Paraemters> ) <Specifiers> < = 0 or delete > ; <InlineCmt>
	}

	StrBuilder
	name_stripped = strbuilder_make_str( ctx->Allocator_Temp, name.Text );
	strbuilder_strip_space(name_stripped);

	CodeFn
	result              = (CodeFn) make_code();
	result->Name        = cache_str( strbuilder_to_str(name_stripped) );
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
				log_failure("Body must be either of Function_Body or Untyped type, %s\n%s", code_debug_str(body), parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp));
				parser_pop(& ctx->parser);
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

	if ( suffix_specs )
		result->SuffixSpecs = suffix_specs;

	result->ReturnType = ret_type;

	if ( params )
		result->Params = params;

	if ( inline_cmt )
		result->InlineCmt = inline_cmt;

	parser_pop(& ctx->parser);
	return result;
}

internal
Code parse_function_body(Context* ctx)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

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

	s32 len = ( (sptr)prevtok.Text.Ptr + prevtok.Text.Len ) - (sptr)start.Text.Ptr;

	if ( len > 0 )
	{
		Str str = { start.Text.Ptr, len };
		body_append( result, cast(Code, def_execution( str )) );
	}

	eat( Tok_BraceCurly_Close );

	parser_pop(& ctx->parser);
	return cast(Code, result);
}

internal neverinline
CodeBody parse_global_nspace(Context* ctx, CodeType which)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

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

		// ctx->parser.Scope->Start = currtok_noskip;

		if ( currtok_noskip.Type == Tok_Preprocess_Hash )
			eat( Tok_Preprocess_Hash );

		b32 macro_found = false;

		switch ( currtok_noskip.Type )
		{
			case Tok_Comma:
			{
				log_failure("Dangling comma found: %SB\nContext:\n%SB", tok_to_strbuilder(currtok), parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp));
				parser_pop( & ctx->parser);
				return InvalidCode;
			}
			break;
			case Tok_Statement_End:
			{
				// TODO(Ed): Convert this to a general warning procedure
				log_fmt("Dangling end statement found %SB\n", tok_to_strbuilder(currtok_noskip));
				eat( Tok_Statement_End );
				continue;
			}
			case Tok_NewLine:
				// Empty lines are auto skipped by Tokens.current()
				member = fmt_newline;
				eat( Tok_NewLine );
			break;

			case Tok_Comment:
				member = cast(Code, parse_comment(ctx));
			break;

			case Tok_Decl_Class:
				member = parse_complicated_definition(ctx, Tok_Decl_Class );
				// class
			break;

			case Tok_Decl_Enum:
				member = parse_complicated_definition(ctx, Tok_Decl_Enum );
				// enum
			break;

			case Tok_Decl_Extern_Linkage:
				if ( which == CT_Extern_Linkage_Body )
					log_failure( "Nested extern linkage\n%s", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );

				member = cast(Code, parser_parse_extern_link(ctx));
				// extern "..." { ... }
			break;

			case Tok_Decl_Namespace:
				member = cast(Code, parser_parse_namespace(ctx));
				// namespace <Name> { ... }
			break;

			case Tok_Decl_Struct:
				member = parse_complicated_definition(ctx, Tok_Decl_Struct );
				// struct ...
			break;

			case Tok_Decl_Template:
				member = cast(Code, parser_parse_template(ctx));
				// template<...> ...
			break;

			case Tok_Decl_Typedef:
				member = cast(Code, parser_parse_typedef(ctx));
				// typedef ...
			break;

			case Tok_Decl_Union:
				member = parse_complicated_definition(ctx, Tok_Decl_Union );
				// union ...
			break;

			case Tok_Decl_Using:
				member = cast(Code, parser_parse_using(ctx));
				// using ...
			break;

			case Tok_Preprocess_Define:
				member = cast(Code, parser_parse_define(ctx));
				// #define ...
			break;

			case Tok_Preprocess_Include:
				member = cast(Code, parse_include(ctx));
				// #include ...
			break;

			case Tok_Preprocess_If:
			case Tok_Preprocess_IfDef:
			case Tok_Preprocess_IfNotDef:
			case Tok_Preprocess_ElIf:
				member = cast(Code, parse_preprocess_cond(ctx));
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

			case Tok_Preprocess_Macro_Stmt: {
				member = cast(Code, parse_simple_preprocess(ctx, Tok_Preprocess_Macro_Stmt ));
				break;
			}

			case Tok_Preprocess_Pragma: {
				member = cast(Code, parse_pragma(ctx));
				// #pragma ...
			}
			break;

			case Tok_Preprocess_Unsupported: {
				member = cast(Code, parse_simple_preprocess(ctx, Tok_Preprocess_Unsupported ));
				// #<UNSUPPORTED> ...
			}
			break;

			case Tok_StaticAssert: {
				member = cast(Code, parse_static_assert(ctx));
				// static_assert( <Conditional Expression>, ... );
			}
			break;

			case Tok_Module_Export: {
				if ( which == CT_Export_Body )
					log_failure( "Nested export declaration\n%s", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );

				member = cast(Code, parser_parse_export_body(ctx));
				// export { ... }
			}
			break;

			case Tok_Module_Import: {
				// import ...
				log_failure( "gen::%s: This function is not implemented" );
				return InvalidCode;
			}
			break;

			case Tok_Preprocess_Macro_Expr: 
			{
				if ( ! tok_is_attribute(currtok))
				{
					log_failure("Unbounded macro expression residing in class/struct body\n%S", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp));
					return InvalidCode;
				}
			}
			//! Fallthrough intentional
			case Tok_Attribute_Open:
			case Tok_Decl_GNU_Attribute:
			case Tok_Decl_MSVC_Attribute:
		#define Entry( attribute, str ) case attribute:
			GEN_DEFINE_ATTRIBUTE_TOKENS
		#undef Entry
			{
				attributes = parse_attributes(ctx);
				// <Attributes>
			}
			//! Fallthrough intentional
			GEN_PARSER_CLASS_GLOBAL_NSPACE_ALLOWED_MEMBER_TOK_SPECIFIER_CASES:
			{
				Specifier specs_found[16] = { Spec_NumSpecifiers };
				s32        NumSpecifiers = 0;

				while ( left && tok_is_specifier(currtok) )
				{
					Specifier spec = str_to_specifier( currtok.Text );

					bool ignore_spec = false;

					switch ( spec )
					{
						GEN_PARSER_CLASS_GLOBAL_NSPACE_ALLOWED_MEMBER_SPECIFIER_CASES:
						break;

						case Spec_Consteval:
							expects_function = true;
						break;

						case Spec_Const:
							ignore_spec = true;
						break;

						default:
							Str spec_str = spec_to_str(spec);

							log_failure( "Invalid specifier %S for variable\n%S", spec_str, strbuilder_to_str( parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp)) );
							parser_pop(& ctx->parser);
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
					specifiers = def_specifiers_arr( NumSpecifiers, specs_found );
				}
				// <Attributes> <Specifiers>
			}
			//! Fallthrough intentional
			case Tok_Identifier:
			case Tok_Preprocess_Macro_Typename:
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
					Code constructor_destructor = parse_global_nspace_constructor_destructor( ctx, specifiers );
					// Possible constructor implemented at global file scope.
					if ( constructor_destructor )
					{
						member = constructor_destructor;
						break;
					}

					bool found_operator_cast_outside_class_implmentation = false;
					s32  idx = ctx->parser.token_id;

					for ( ; idx < ctx->parser.tokens.num; idx++ )
					{
						Token tok = ctx->parser.tokens[ idx ];

						if ( tok.Type == Tok_Identifier )
						{
							idx++;
							tok = ctx->parser.tokens[ idx ];
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
						member = cast(Code, parser_parse_operator_cast(ctx, specifiers ));
						// <Attributes> <Specifiers> <Name>::operator <Type>() { ... }
						break;
					}
				}

				member = parse_operator_function_or_variable(ctx, expects_function, attributes, specifiers );
				// <Attributes> <Specifiers> ...
			}
		}

		if ( member == Code_Invalid )
		{
			log_failure( "Failed to parse member\nToken: %SB\nContext:\n%SB", tok_to_strbuilder(currtok_noskip), parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
			parser_pop(& ctx->parser);
			return InvalidCode;
		}

		// log_fmt("Global Body Member: %s", member->debug_str());
		body_append(result, member );
	}

	if ( which != CT_Global_Body )
		eat( Tok_BraceCurly_Close );
		// { <Body> }

	parser_pop(& ctx->parser);
	return result;
}

internal inline
Code parse_global_nspace_constructor_destructor(Context* ctx, CodeSpecifiers specifiers)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

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
	TokenSlice tokens = ctx->parser.tokens;

	s32   idx = ctx->parser.token_id;
	Token nav = tokens[ idx ];
	for ( ; idx < tokens.num; idx++, nav = tokens[ idx ] )
	{
		if ( nav.Text.Ptr[0] == '<' )
		{
			// Skip templated expressions as they mey have expressions with the () operators
			s32 capture_level  = 0;
			s32 template_level = 0;
			for ( ; idx < tokens.num; idx++, nav = tokens.ptr[idx] )
			{
				if (nav.Text.Ptr[ 0 ] == '<')
					++ template_level;

				if (nav.Text.Ptr[ 0 ] == '>')
					-- template_level;
				if (nav.Type == Tok_Operator && nav.Text.Ptr[1] == '>')
					-- template_level;

				if ( nav.Type == Tok_Paren_Open)
				{
					if (template_level != 0 )
						++ capture_level;
					else
						break;
				}

				if ( template_level != 0 && nav.Type == Tok_Paren_Close)
					-- capture_level;
			}
		}

		if ( nav.Type == Tok_Paren_Open )
			break;
	}

	-- idx;
	Token tok_right = tokens.ptr[idx];
	Token tok_left  = NullToken;

	if (tok_right.Type != Tok_Identifier)
	{
		parser_pop(& ctx->parser);
		// We're not dealing with a constructor if there is no identifier right before the opening of a parameter's scope.
		return result;
	}

	-- idx;
	tok_left = tokens.ptr[idx];
	// <Attributes> <Specifiers> ... <Identifier>

	bool possible_destructor = false;
	if ( tok_left.Type == Tok_Operator && tok_left.Text.Ptr[0] == '~')
	{
		possible_destructor = true;
		-- idx;
		tok_left = tokens.ptr[idx];
	}

	if ( tok_left.Type != Tok_Access_StaticSymbol ) {
		parser_pop(& ctx->parser);
		return result;
	}

	-- idx;
	tok_left = tokens.ptr[idx];
	// <Attributes> <Specifiers> ... :: <Identifier>

	// We search toward the left until we find the next valid identifier
	s32 capture_level  = 0;
	s32 template_level = 0;
	while ( idx != ctx->parser.token_id )
	{
		if (tok_left.Text.Ptr[ 0 ] == '<')
			++ template_level;

		if (tok_left.Text.Ptr[ 0 ] == '>')
			-- template_level;
		if (tok_left.Type == Tok_Operator && tok_left.Text.Ptr[1] == '>')
			-- template_level;

		if ( template_level != 0 && tok_left.Type == Tok_Paren_Open)
			++ capture_level;

		if ( template_level != 0 && tok_left.Type == Tok_Paren_Close)
			-- capture_level;

		if ( capture_level == 0 && template_level == 0  && tok_left.Type == Tok_Identifier )
			break;

		-- idx;
		tok_left = tokens.ptr[idx];
	}

	bool is_same = c_str_compare_len( tok_right.Text.Ptr, tok_left.Text.Ptr, tok_right.Text.Len ) == 0;
	if (tok_left.Type == Tok_Identifier && is_same)
	{
		// We have found the pattern we desired
		if (possible_destructor)
		{
			// <Name> :: ~<Name> (
			result = cast(Code, parser_parse_destructor(ctx, specifiers ));
		}
		else {
			// <Name> :: <Name> (
			result = cast(Code, parser_parse_constructor(ctx, specifiers ));
		}
	}

	parser_pop(& ctx->parser);
	return result;
}

// TODO(Ed): I want to eventually change the identifier to its own AST type.
// This would allow distinction of the qualifier for a symbol <qualifier>::<nested symboL>
// This would also allow
internal
Token parse_identifier(Context* ctx, bool* possible_member_function)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	Token name = currtok;
	ctx->parser.Scope->Name = name.Text;

	Macro* macro = lookup_macro(currtok.Text);
	b32 accept_as_identifier = macro && bitfield_is_set(MacroFlags, macro->Flags, MF_Allow_As_Identifier );
	b32 is_decarator         = macro && bitfield_is_set(MacroFlags, macro->Flags, MF_Identifier_Decorator );

	// Typename can be: '::' <name>
	// If that is the case first  option will be Tok_Access_StaticSymbol below
	if (check(Tok_Identifier) || accept_as_identifier) 
	{
		if (is_decarator) {
			Code name_macro = parse_simple_preprocess(ctx, currtok.Type);
			name.Text.Len   = ( ( sptr )prevtok.Text.Ptr + prevtok.Text.Len ) - ( sptr )name.Text.Ptr;
		}	
		else {
			eat(Tok_Identifier);
		}
	}
	// <Name>

	parse_template_args(ctx, & name);
	// <Name><Template Args>

	while ( check( Tok_Access_StaticSymbol ) )
	{
		eat( Tok_Access_StaticSymbol );
		// <Qualifier Name> <Template Args> ::

		Token invalid = NullToken;
		if ( left == 0 )
		{
			log_failure( "Error, unexpected end of static symbol identifier\n%s", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
			parser_pop(& ctx->parser);
			return invalid;
		}

		if ( currtok.Type == Tok_Operator && currtok.Text.Ptr[0] == '~' )
		{
			bool is_destructor = str_are_equal( ctx->parser.Scope->Prev->ProcName, txt("parser_parse_destructor"));
			if (is_destructor)
			{
				name.Text.Len = ( ( sptr )prevtok.Text.Ptr + prevtok.Text.Len ) - ( sptr )name.Text.Ptr;
				parser_pop(& ctx->parser);
				return name;
			}

			log_failure( "Error, had a ~ operator after %SB but not a destructor\n%s", toktype_to_str( prevtok.Type ), parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
			parser_pop(& ctx->parser);
			return invalid;
		}

		if ( currtok.Type == Tok_Operator && currtok.Text.Ptr[0] == '*' && currtok.Text.Len == 1 )
		{
			if ( possible_member_function )
				*possible_member_function = true;

			else
			{
				log_failure( "Found a member function pointer identifier but the parsing context did not expect it\n%s", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
				parser_pop(& ctx->parser);
				return invalid;
			}
		}

		if ( currtok.Type != Tok_Identifier )
		{
			log_failure( "Error, expected static symbol identifier, not %s\n%s", toktype_to_str( currtok.Type ), parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
			parser_pop(& ctx->parser);
			return invalid;
		}

		name.Text.Len = ( (sptr)currtok.Text.Ptr + currtok.Text.Len ) - (sptr)name.Text.Ptr;
		eat( Tok_Identifier );
		// <Qualifier Name> <Template Args> :: <Name>

		parse_template_args(ctx, & name);
		// <Qualifier Name> <Template Args> :: <Name> <Template Args>
	}
	// <Qualifier Name> <Template Args> :: <Name> <Template Args> ...

	parser_pop(& ctx->parser);
	return name;
}

internal
CodeInclude parse_include(Context* ctx)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	CodeInclude
	include       = (CodeInclude) make_code();
	include->Type = CT_Preprocess_Include;
	eat( Tok_Preprocess_Include );
	// #include

	if ( ! check( Tok_String ))
	{
		log_failure( "Error, expected include string after #include\n%s", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
		parser_pop(& ctx->parser);
		return InvalidCode;
	}

	ctx->parser.Scope->Name = currtok.Text;
	include->Content = cache_str( currtok.Text );
	eat( Tok_String );
	// #include <Path> or "Path"

	parser_pop(& ctx->parser);
	return include;
}

internal
CodeOperator parse_operator_after_ret_type(Context* ctx
	, ModuleFlag     mflags
	, CodeAttributes attributes
	, CodeSpecifiers specifiers
	, CodeTypename   ret_type
)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

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

		nspace.Text.Len = ( (sptr)prevtok.Text.Ptr + prevtok.Text.Len ) - (sptr)nspace.Text.Ptr;
	}
	// <ExportFlag> <Attributes> <Specifiers> <ReturnType> <Qualifier::...>

	eat( Tok_Decl_Operator );
	// <ExportFlag> <Attributes> <Specifiers> <ReturnType> <Qualifier::...> operator

	if ( ! left && currtok.Type != Tok_Operator
		&& currtok.Type != Tok_Star
		&& currtok.Type != Tok_Ampersand
		&& currtok.Type != Tok_Ampersand_DBL )
	{
		log_failure( "Expected operator after 'operator' keyword\n%s", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
		parser_pop(& ctx->parser);
		return InvalidCode;
	}

	ctx->parser.Scope->Name = currtok.Text;

	bool was_new_or_delete = false;

	Operator op = Op_Invalid;
	switch ( currtok.Text.Ptr[0] )
	{
		case '+':
		{
			if ( currtok.Text.Ptr[1] == '=' )
				op = Op_Assign_Add;

			else if ( currtok.Text.Ptr[1] == '+' )
				op = Op_Increment;

			else
				op = Op_Add;
		}
		break;
		case '-':
		{
			if ( currtok.Text.Ptr[1] == '>' )
			{
				if ( currtok.Text.Ptr[2] == '*' )
					op = Op_MemberOfPointer;

				else
					op = Op_MemberOfPointer;

				break;
			}

			else if ( currtok.Text.Ptr[1] == '=' )
				op = Op_Assign_Subtract;

			else
				op = Op_Subtract;
		}
		break;
		case '*':
		{
			if ( currtok.Text.Ptr[1] == '=' )
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
			if ( currtok.Text.Ptr[1] == '=' )
				op = Op_Assign_Divide;

			else
				op = Op_Divide;
		}
		break;
		case '%':
		{
			if ( currtok.Text.Ptr[1] == '=' )
				op = Op_Assign_Modulo;

			else
				op = Op_Modulo;
		}
		break;
		case '&':
		{
			if ( currtok.Text.Ptr[1] == '=' )
				op = Op_Assign_BAnd;

			else if ( currtok.Text.Ptr[1] == '&' )
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
			if ( currtok.Text.Ptr[1] == '=' )
				op = Op_Assign_BOr;

			else if ( currtok.Text.Ptr[1] == '|' )
				op = Op_LOr;

			else
				op = Op_BOr;
		}
		break;
		case '^':
		{
			if ( currtok.Text.Ptr[1] == '=' )
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
			if ( currtok.Text.Ptr[1] == '=' )
				op = Op_LNot;

			else
				op = Op_UnaryNot;
		}
		break;
		case '=':
		{
			if ( currtok.Text.Ptr[1] == '=' )
				op = Op_LEqual;

			else
				op = Op_Assign;
		}
		break;
		case '<':
		{
			if ( currtok.Text.Ptr[1] == '=' )
				op = Op_LesserEqual;

			else if ( currtok.Text.Ptr[1] == '<' )
			{
				if ( currtok.Text.Ptr[2] == '=' )
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
			if ( currtok.Text.Ptr[1] == '=' )
				op = Op_GreaterEqual;

			else if ( currtok.Text.Ptr[1] == '>' )
			{
				if ( currtok.Text.Ptr[2] == '=' )
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
			if ( currtok.Text.Ptr[1] == ')' )
			{
				op = Op_FunctionCall;
				eat(Tok_Paren_Open);
			}

			else
				op = Op_Invalid;
		}
		break;
		case '[':
		{
			if ( currtok.Text.Ptr[1] == ']' )
				op = Op_Subscript;

			else
				op = Op_Invalid;
		}
		break;
		default:
		{
			Str c_str_new    = operator_to_str(Op_New);
			Str c_str_delete = operator_to_str(Op_Delete);
			if ( c_str_compare_len( currtok.Text.Ptr, c_str_new.Ptr, max(c_str_new.Len - 1, currtok.Text.Len)) == 0)
			{
				op = Op_New;
				eat( Tok_Identifier );
				was_new_or_delete = true;

				s32 idx = ctx->parser.token_id + 1;
				{
					while ( ctx->parser.tokens.ptr[ idx ].Type == Tok_NewLine )
						idx++;
				}
				Token next = ctx->parser.tokens.ptr[idx];
				if ( currtok.Type == Tok_Operator && c_str_compare_len(currtok.Text.Ptr, "[]", 2) == 0)
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
			else if ( c_str_compare_len( currtok.Text.Ptr, c_str_delete.Ptr, max(c_str_delete.Len - 1, currtok.Text.Len )) == 0)
			{
				op = Op_Delete;
				eat(Tok_Identifier);
				was_new_or_delete = true;

				s32 idx = ctx->parser.token_id + 1;
				{
					while ( ctx->parser.tokens.ptr[ idx ].Type == Tok_NewLine )
						idx++;
				}
				Token next = ctx->parser.tokens.ptr[idx];
				if ( currtok.Type == Tok_Operator && c_str_compare_len(currtok.Text.Ptr, "[]", 2) == 0)
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
					log_failure( "Invalid operator '%s'\n%s", prevtok.Text, parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
					parser_pop(& ctx->parser);
					return InvalidCode;
				}
			}
		}
	}

	if ( op == Op_Invalid )
	{
		log_failure( "Invalid operator '%s'\n%s", currtok.Text, parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
		parser_pop(& ctx->parser);
		return InvalidCode;
	}

	if ( ! was_new_or_delete)
		eat( currtok.Type );
	// <ExportFlag> <Attributes> <Specifiers> <ReturnType> <Qualifier::...> operator <Op>

	// Parse Params
	CodeParams params = parse_params(ctx, parser_use_parenthesis);
	// <ExportFlag> <Attributes> <Specifiers> <ReturnType> <Qualifier::...> operator <Op> ( <Parameters> )

	if ( params == nullptr && op == Op_Multiply )
		op = Op_MemberOfPointer;

	while ( left && tok_is_specifier(currtok) )
	{
		if ( specifiers == nullptr )
		{
			specifiers = def_specifier( str_to_specifier( currtok.Text) );
			eat( currtok.Type );
			continue;
		}

		specifiers_append(specifiers, str_to_specifier( currtok.Text) );
		eat( currtok.Type );
	}
	// <ExportFlag> <Attributes> <Specifiers> <ReturnType> <Qualifier::...> operator <Op> ( <Parameters> ) <Specifiers>
		
	// TODO(Ed): Add proper "delete" and "new" awareness
	// We're dealing with either a "= delete" or operator deletion
	if (check(Tok_Operator) && currtok.Text.Ptr[0] == '=')
	{
		eat(currtok.Type);
		if ( ! str_are_equal(currtok.Text, txt("delete")))
		{
			log_failure("Expected delete after = in operator forward instead found \"%S\"\n%SB", currtok.Text, parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp));
			parser_pop(& ctx->parser);
			return InvalidCode;
		}
		if (specifiers == nullptr)
			specifiers = def_specifier( Spec_Delete );
		else 
			specifiers_append(specifiers, Spec_Delete);
		eat(currtok.Type);
		// <ExportFlag> <Attributes> <Specifiers> <ReturnType> <Qualifier::...> operator <Op> ( <Parameters> ) <Specifiers> = delete
	}

	// Parse Body
	CodeBody    body       = { nullptr };
	CodeComment inline_cmt = NullCode;
	if ( check( Tok_BraceCurly_Open ) )
	{
		body = cast(CodeBody, parse_function_body(ctx));
		if ( cast(Code, body) == Code_Invalid )
		{
			parser_pop(& ctx->parser);
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
			inline_cmt = parse_comment(ctx);
			// <ExportFlag> <Attributes> <Specifiers> <ReturnType> <Qualifier::...> operator <Op> ( <Parameters> ) <Specifiers>; <InlineCmt>
	}

	// OpValidateResult check_result = operator__validate( op, params, ret_type, specifiers );
	CodeOperator result = def_operator( op, nspace.Text, def_assign( params, ret_type, body, specifiers, attributes, mflags ) );

	if ( inline_cmt )
		result->InlineCmt = inline_cmt;

	parser_pop(& ctx->parser);
	return result;
}

internal
Code parse_operator_function_or_variable(Context* ctx, bool expects_function, CodeAttributes attributes, CodeSpecifiers specifiers )
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	Code result = InvalidCode;

	Code macro_stmt = parse_macro_as_definiton(ctx, attributes, specifiers);
	if (macro_stmt) {
		parser_pop(& ctx->parser);
		return macro_stmt;
	}

	CodeTypename type = parser_parse_type(ctx, parser_not_from_template, nullptr );
	// <Attributes> <Specifiers> <ReturnType/ValueType>

	// Thanks Unreal
	CodeAttributes post_rt_attributes = parse_attributes(ctx);
	if (post_rt_attributes)
	{
		if (attributes)
		{
			StrBuilder merged = strbuilder_fmt_buf(ctx->Allocator_Temp, "%S %S", attributes->Content, post_rt_attributes->Content);
			attributes->Content = cache_str(strbuilder_to_str(merged));
		}
		else
		{
			attributes = post_rt_attributes;
		}
		// <Attributes> <Specifiers> <ReturnType/ValueType> <Attributes>
		// CONVERTED TO:
		// <Attributes> <Specifiers> <ReturnType/ValueType>
	}

	if ( type == InvalidCode ) {
		parser_pop(& ctx->parser);
		return InvalidCode;
	}

	bool found_operator = false;
	s32  idx            = ctx->parser.token_id;

	for ( ; idx < ctx->parser.tokens.num; idx++ )
	{
		Token tok = ctx->parser.tokens.ptr[ idx ];

		if ( tok.Type == Tok_Identifier )
		{
			idx++;
			tok = ctx->parser.tokens.ptr[ idx ];
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
		result = cast(Code, parse_operator_after_ret_type(ctx, ModuleFlag_None, attributes, specifiers, type) );
		// <Attributes> <Specifiers> <ReturnType> operator ...
	}
	else
	{
		Token name = parse_identifier(ctx, nullptr);
		ctx->parser.Scope->Name = name.Text;

		bool detected_capture = check( Tok_Paren_Open );

		// Check three tokens ahead to make sure that were not dealing with a constructor initialization...
		//                  (         350.0f    ,         <---  Could be the scenario
		// Example : <Capture_Start> <Value> <Comma>
		//                 idx         +1      +2
		bool detected_comma = ctx->parser.tokens.ptr[ ctx->parser.token_id + 2 ].Type == Tok_Comma;

		b32   detected_non_varadic_unpaired_param = detected_comma && nexttok.Type != Tok_Varadic_Argument;
		if (! detected_non_varadic_unpaired_param && nexttok.Type ==  Tok_Preprocess_Macro_Expr) for( s32 break_scope = 0; break_scope == 0; ++ break_scope)
		{
			Macro* macro = lookup_macro( nexttok.Text );
			if (macro == nullptr || ! macro_is_functional(* macro))
				break;

			// (   <Macro_Expr> (  
			// Idx      +1     +2
			s32  idx    = ctx->parser.token_id + 1;  
			s32  level = 0;

			// Find end of the token expression
			for ( ; idx < ctx->parser.tokens.num; idx++ )
			{
				Token tok = ctx->parser.tokens.ptr[ idx ];

				if ( tok.Type == Tok_Paren_Open )
					level++;
				else if ( tok.Type == Tok_Paren_Close && level > 0 )
					level--;
				if (level == 0 && tok.Type == Tok_Paren_Close)
					break;
			}
			++ idx; // Will incremnt to possible comma position

			if ( ctx->parser.tokens.ptr[ idx ].Type != Tok_Comma )
				break;

			detected_non_varadic_unpaired_param = true;
		}

		if ( detected_capture && ! detected_non_varadic_unpaired_param ) 
		{
			// Dealing with a function
			result = cast(Code, parse_function_after_name(ctx, ModuleFlag_None, attributes, specifiers, type, name ));
			// <Attributes> <Specifiers> <ReturnType> <Name> ( ...
		}
		else
		{
			if ( expects_function ) {
				log_failure( "Expected function declaration (consteval was used)\n%s", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
				parser_pop(& ctx->parser);
				return InvalidCode;
			}
			// Dealing with a variable
			result = cast(Code, parse_variable_after_name(ctx, ModuleFlag_None, attributes, specifiers, type, name.Text ));
			// <Attributes> <Specifiers> <ValueType> <Name> ...
		}
	}

	parser_pop(& ctx->parser);	
	return result;
}

internal
Code parse_macro_as_definiton(Context* ctx, CodeAttributes attributes, CodeSpecifiers specifiers)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	if (currtok.Type != Tok_Preprocess_Macro_Stmt ) {
		parser_pop(& ctx->parser);
		return NullCode;
	}
	Macro* macro                     = lookup_macro(currtok.Text);
	b32    can_resolve_to_definition = macro && bitfield_is_set(MacroFlags, macro->Flags, MF_Allow_As_Definition);
	if ( ! can_resolve_to_definition) {
		parser_pop(& ctx->parser);
		return NullCode;
	}

	// TODO(Ed): When AST_Macro is made, have it support attributs and specifiers for when its behaving as a declaration/definition.
	Code code = parse_simple_preprocess(ctx, Tok_Preprocess_Macro_Stmt );

	// Attributes and sepcifiers will be collapsed into the macro's serialization.
	StrBuilder resolved_definition = strbuilder_fmt_buf(ctx->Allocator_Temp, "%S %S %S"
		, attributes ? strbuilder_to_str( attributes_to_strbuilder(attributes)) : txt("")
		, specifiers ? strbuilder_to_str( specifiers_to_strbuilder(specifiers)) : txt("")
		, code->Content
	);
	Code result = untyped_str( strbuilder_to_str(resolved_definition) );
	parser_pop(& ctx->parser);
	return result;
}

internal
CodePragma parse_pragma(Context* ctx)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	CodePragma
	pragma       = (CodePragma) make_code();
	pragma->Type = CT_Preprocess_Pragma;
	eat( Tok_Preprocess_Pragma );
	// #pragma

	if ( ! check( Tok_Preprocess_Content )) {
		log_failure( "Error, expected content after #pragma\n%s", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
		parser_pop(& ctx->parser);
		return InvalidCode;
	}

	ctx->parser.Scope->Name = currtok.Text;

	pragma->Content = cache_str( currtok.Text );
	eat( Tok_Preprocess_Content );
	// #pragma <Content>

	parser_pop(& ctx->parser);
	return pragma;
}

internal inline
CodeParams parse_params(Context* ctx, bool use_template_capture )
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	if ( ! use_template_capture ) {
		eat( Tok_Paren_Open );
		// (
	}
	else {
		if ( check( Tok_Operator ) && currtok.Text.Ptr[ 0 ] == '<' )
			eat( Tok_Operator );
			// <
	}

	if ( ! use_template_capture && check( Tok_Paren_Close ) )
	{
		eat( Tok_Paren_Close );
		// )
		parser_pop(& ctx->parser);
		return NullCode;
	}
	else if ( check( Tok_Operator ) && currtok.Text.Ptr[ 0 ] == '>' ) 
	{
		eat( Tok_Operator );
		// >
		parser_pop(& ctx->parser);
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

		parser_pop(& ctx->parser);
		return param_varadic;
		// ( ... )
		// or < ... >
	}

	#define CheckEndParams() \
		(use_template_capture ? (currtok.Text.Ptr[ 0 ] != '>') : (currtok.Type != Tok_Paren_Close))

	// TODO(Ed): Use expression macros or this? macro as attribute?
	// Ex: Unreal has this type of macro:                 vvvvvvvvv
	// COREUOBJECT_API void CallFunction( FFrame& Stack, RESULT_DECL, UFunction* Function );
	// and:                 vvvv
	// AddComponentByClass(UPARAM(meta = (AllowAbstract = "false")) TSubclassOf<UActorComponent> Class, bool bManualAttachment, ...
	if ( check(Tok_Preprocess_Macro_Expr))
	{
		macro = parse_simple_preprocess(ctx, Tok_Preprocess_Macro_Expr);
		// ( <Macro>
	}
	if ( currtok.Type != Tok_Comma )
	{
		type = parser_parse_type(ctx, use_template_capture, nullptr );
		if ( cast(Code, type) == Code_Invalid )
		{
			parser_pop(& ctx->parser);
			return InvalidCode;
		}
		// ( <Macro> <ValueType>

		if ( check( Tok_Identifier ) || bitfield_is_set(u32, currtok.Flags, TF_Identifier) )
		{
			name = currtok;
			eat( currtok.Type );
			// ( <Macro> <ValueType> <Name>
		}

		// TODO(Ed): Use expression macro for this?
		// Unreal has yet another type of macro:
		// template<class T UE_REQUIRES(TPointerIsConvertibleFromTo<T, UInterface>::Value)>
		// class T ... and then ^this^ UE_REQUIRES shows up
		// So we need to consume that.
		if ( check( Tok_Preprocess_Macro_Expr ))
		{
			post_name_macro = parse_simple_preprocess(ctx, Tok_Preprocess_Macro_Expr );
			// ( <Macro> <ValueType> <Name> <PostNameMacro>
		}

		// C++ allows typename = expression... so anything goes....
		if ( bitfield_is_set( u32, currtok.Flags, TF_Assign ) )
		{
			eat( Tok_Operator );
			// ( <Macro> <ValueType> <Name>  =

			Token value_tok = currtok;

			if ( currtok.Type == Tok_Comma ) {
				log_failure( "Expected value after assignment operator\n%s.", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
				parser_pop(& ctx->parser);
				return InvalidCode;
			}

			s32 capture_level  = 0;
			s32 template_level = 0;
			while ( (left && ( currtok.Type != Tok_Comma ) && template_level >= 0 && CheckEndParams()) || (capture_level > 0 || template_level > 0) )
			{
				if (currtok.Text.Ptr[ 0 ] == '<')
					++ template_level;

				if (currtok.Text.Ptr[ 0 ] == '>')
					-- template_level;
				if (currtok.Type == Tok_Operator && currtok.Text.Ptr[1] == '>')
					-- template_level;

				if ( currtok.Type == Tok_Paren_Open)
					++ capture_level;

				if ( currtok.Type == Tok_Paren_Close)
					-- capture_level;

				value_tok.Text.Len = ( ( sptr )currtok.Text.Ptr + currtok.Text.Len ) - ( sptr )value_tok.Text.Ptr;
				eat( currtok.Type );
			}

			value = untyped_str( strbuilder_to_str(parser_strip_formatting(ctx, value_tok.Text, parser_strip_formatting_dont_preserve_newlines )) );
			// ( <Macro> <ValueType> <Name> = <Expression>
		}
	}

	CodeParams result = ( CodeParams )make_code();
	result->Type     = CT_Parameters;

	result->Macro = macro;

	if ( name.Text.Len > 0 )
		result->Name = cache_str( name.Text );

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
		if ( check(Tok_Preprocess_Macro_Expr))
		{
			macro = parse_simple_preprocess(ctx, Tok_Preprocess_Macro_Expr);
			// ( <Macro> <ValueType> <Name> = <Expression>, <Macro>
		}
		if ( currtok.Type != Tok_Comma )
		{
			type = cast(Code, parser_parse_type(ctx, use_template_capture, nullptr ));
			if ( type == Code_Invalid )
			{
				parser_pop(& ctx->parser);
				return InvalidCode;
			}
			// ( <Macro> <ValueType> <Name> = <Expression>, <Macro> <ValueType>

			name = NullToken;

			if ( check( Tok_Identifier ) || bitfield_is_set(u32, currtok.Flags, TF_Identifier) )
			{
				name = currtok;
				eat( currtok.Type );
				// ( <Macro> <ValueType> <Name> = <Expression>, <Macro> <ValueType> <Name>
			}

			// Unreal has yet another type of macro:
			// template<class T UE_REQUIRES(TPointerIsConvertibleFromTo<T, UInterface>::Value)>
			// class T ... and then ^this^ UE_REQUIRES shows up
			// So we need to consume that.
			if ( check( Tok_Preprocess_Macro_Expr )) {
				post_name_macro = parse_simple_preprocess(ctx, Tok_Preprocess_Macro_Expr );
				// ( <Macro> <ValueType> <Name> = <Expression>, <Macro> <ValueType> <PostNameMacro>
			}

			/// C++ allows typename = expression... so anything goes....
			if ( bitfield_is_set( u32, currtok.Flags, TF_Assign ) )
			{
				eat( Tok_Operator );
				// ( <Macro> <ValueType> <Name> = <Expression>, <Macro> <ValueType> <Name> <PostNameMacro> =

				Token value_tok = currtok;

				if ( currtok.Type == Tok_Comma ) {
					log_failure( "Expected value after assignment operator\n%s", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
					parser_pop(& ctx->parser);
					return InvalidCode;
				}

				s32 capture_level  = 0;
				s32 template_level = 0;
				while ( (left
				&& currtok.Type != Tok_Comma
				&& template_level >= 0
				&& CheckEndParams()) || (capture_level > 0 || template_level > 0) )
				{
					if (currtok.Text.Ptr[ 0 ] == '<')
						++ template_level;

					if (currtok.Text.Ptr[ 0 ] == '>')
						-- template_level;
					if (currtok.Type == Tok_Operator && currtok.Text.Ptr[1] == '>')
						-- template_level;

					if ( currtok.Type == Tok_Paren_Open)
						++ capture_level;

					if ( currtok.Type == Tok_Paren_Close)
						-- capture_level;

					value_tok.Text.Len = ( ( sptr )currtok.Text.Ptr + currtok.Text.Len ) - ( sptr )value_tok.Text.Ptr;
					eat( currtok.Type );
				}

				value = untyped_str( strbuilder_to_str(parser_strip_formatting(ctx, value_tok.Text, parser_strip_formatting_dont_preserve_newlines )) );
				// ( <Macro> <ValueType> <Name> = <Expression>, <Macro> <ValueType> <Name> <PostNameMacro> = <Expression>
			}
			// ( <Macro> <ValueType> <Name> = <Expression>, <Macro> <ValueType> <Name> <PostNameMacro> = <Expression>, ..
		}

		CodeParams param = ( CodeParams )make_code();
		param->Type     = CT_Parameters;

		param->Macro = macro;

		if ( name.Text.Len > 0 )
			param->Name = cache_str( name.Text );

		param->PostNameMacro = post_name_macro;
		param->ValueType     = cast(CodeTypename, type);

		if ( value )
			param->Value = value;

		params_append(result, param );
	}

	if ( ! use_template_capture )
	{
		eat( Tok_Paren_Close );
		// ( <Macro> <ValueType> <Name> <PostNameMacro> = <Expression>, <Macro> <ValueType> <Name> <PostNameMacro> = <Expression>, .. )
	}
	else
	{
		if ( ! check( Tok_Operator ) || currtok.Text.Ptr[ 0 ] != '>' ) {
			log_failure( "Expected '<' after 'template' keyword\n%s", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
			parser_pop(& ctx->parser);
			return InvalidCode;
		}
		eat( Tok_Operator );
		// < <Macro> <ValueType> <Name> <PostNameMacro> = <Expression>, <Macro> <ValueType> <Name> <PostNameMacro> = <Expression>, .. >
	}
	parser_pop(& ctx->parser);
	return result;
#undef context
}

internal
CodePreprocessCond parse_preprocess_cond(Context* ctx)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	if ( ! tok_is_preprocess_cond(currtok) )
	{
		log_failure( "Error, expected preprocess conditional\n%s", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
		parser_pop(& ctx->parser);
		return InvalidCode;
	}

	CodePreprocessCond
	cond       = (CodePreprocessCond) make_code();
	cond->Type = scast(CodeType, currtok.Type - ( Tok_Preprocess_If - CT_Preprocess_If ) );
	eat( currtok.Type );
	// #<Conditional>

	if ( ! check( Tok_Preprocess_Content ))
	{
		log_failure( "Error, expected content after #define\n%s", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
		parser_pop(& ctx->parser);
		return InvalidCode;
	}

	ctx->parser.Scope->Name = currtok.Text;
	cond->Content = cache_str( currtok.Text );
	eat( Tok_Preprocess_Content );
	// #<Conditiona> <Content>

	parser_pop(& ctx->parser);
	return cond;
}

internal
Code parse_simple_preprocess(Context* ctx, TokType which)
{
	// TODO(Ed): We can handle a macro a bit better than this. It's AST can be made more robust..
	// Make an AST_Macro, it should have an Name be the macro itself, with the function body being an optional function body node.
	// If we want it to terminate or have an inline comment we can possbily use its parent typedef for that info...
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	Token full_macro = currtok;
	eat( which );
	// <Macro>

	Macro* macro = lookup_macro( full_macro.Text );
	if ( which != Tok_Preprocess_Unsupported && macro == nullptr ) {
		log_failure("Expected the macro %S to be registered\n%S", full_macro,  parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp));
		return NullCode;
	}

	// TODO(Ed) : Parse this properly later (expression and statement support)
	if ( macro && macro_is_functional(* macro) )
	{
		eat( Tok_Paren_Open );

		s32 level = 0;
		while ( left && ( currtok.Type != Tok_Paren_Close || level > 0 ) )
		{
			if ( currtok.Type == Tok_Paren_Open )
				level++;

			else if ( currtok.Type == Tok_Paren_Close && level > 0 )
				level--;

			eat( currtok.Type );
		}
		eat( Tok_Paren_Close );
		// <Macro> ( <params> )

		full_macro.Text.Len = ( (sptr)prevtok.Text.Ptr + prevtok.Text.Len ) - (sptr)full_macro.Text.Ptr;
	}

	if ( macro && macro_expects_body(* macro) && peektok.Type == Tok_BraceCurly_Open )
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

		// TODO(Ed): Review this?
		Str prev_proc = ctx->parser.Scope->Prev->ProcName;
		if ( macro->Type == MT_Typename && c_str_compare_len( prev_proc.Ptr, "parser_parse_typedef", prev_proc.Len ) != 0 )
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

		full_macro.Text.Len = ( (sptr)prevtok.Text.Ptr + prevtok.Text.Len ) - (sptr)full_macro.Text.Ptr;
	}
	else
	{
		// If the macro is just a macro in the body of an AST it may have a semi-colon for the user to close on purpsoe
		// (especially for functional macros)
		Str calling_proc = ctx->parser.Scope->Prev->ProcName;

		if (str_contains(ctx->parser.Scope->Prev->ProcName, txt("parser_parse_enum")))
		{
			// Do nothing
			goto Leave_Scope_Early;
		}
		else if (macro && macro->Type == MT_Typename && str_contains(ctx->parser.Scope->Prev->ProcName, txt("parser_parse_typedef")))
		{
			if ( peektok.Type == Tok_Statement_End )
			{
				Token stmt_end = currtok;
				eat( Tok_Statement_End );
				// <Macro>;

				full_macro.Text.Len += prevtok.Text.Len;

				// TODO(Ed): Reveiw the context for this? (ESPECIALLY THIS)
				if ( currtok_noskip.Type == Tok_Comment && currtok_noskip.Line == stmt_end.Line )
				{
					eat( Tok_Comment );
					// <Macro>; <InlineCmt>

					full_macro.Text.Len += prevtok.Text.Len;
				}
			}
		}
		else if (
				str_contains(calling_proc, txt("parse_global_nspace"))
			||	str_contains(calling_proc, txt("parse_class_struct_body"))
		)
		{
			if (left && peektok.Type == Tok_Statement_End)
			{
				Token stmt_end = currtok;
				eat( Tok_Statement_End );
				// <Macro>;
				full_macro.Text.Len += prevtok.Text.Len;
			}
		}
	}

Leave_Scope_Early:
	Code result = untyped_str( full_macro.Text );
	ctx->parser.Scope->Name = full_macro.Text;

	parser_pop(& ctx->parser);
	return result;
}

internal
Code parse_static_assert(Context* ctx)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	Code
	assert       = make_code();
	assert->Type = CT_Untyped;

	Token content = currtok;

	ctx->parser.Scope->Name = content.Text;

	eat( Tok_StaticAssert );
	eat( Tok_Paren_Open );
	// static_assert(

	// TODO(Ed) : Parse this properly.
	s32 level = 0;
	while ( left && ( currtok.Type != Tok_Paren_Close || level > 0 ) )
	{
		if ( currtok.Type == Tok_Paren_Open )
			level++;
		else if ( currtok.Type == Tok_Paren_Close )
			level--;

		eat( currtok.Type );
	}
	eat( Tok_Paren_Close );
	eat( Tok_Statement_End );
	// static_assert( <Content> );

	content.Text.Len  = ( (sptr)prevtok.Text.Ptr + prevtok.Text.Len ) - (sptr)content.Text.Ptr;

	char const* str  = c_str_fmt_buf( "%.*s\n", content.Text.Len, content.Text.Ptr );
	Str content_str = { str, content.Text.Len + 1 };
	assert->Content = cache_str( content_str );
	assert->Name	= assert->Content;

	parser_pop(& ctx->parser);
	return assert;
}

/*
	This a brute-froce make all the arguments part of the token provided.
	Can have in-place function signatures, regular identifiers, in-place typenames, compile-time expressions, parameter-pack expansion, etc.
	This means that validation can only go so far, and so if there is any different in formatting
	passed the basic stripping supported it report a soft failure.
*/
internal inline
void parse_template_args(Context* ctx, Token* token)
{
	if ( currtok.Type == Tok_Operator && currtok.Text.Ptr[ 0 ] == '<' && currtok.Text.Len == 1 )
	{
		eat( Tok_Operator );
		// <

		s32 level = 0;
		while ( left && level >= 0 && ( currtok.Text.Ptr[ 0 ] != '>' || level > 0 ) )
		{
			if ( currtok.Text.Ptr[ 0 ] == '<' )
				level++;

			if ( currtok.Text.Ptr[ 0 ] == '>' )
				level--;
			if ( currtok.Type == Tok_Operator && currtok.Text.Ptr[1] == '>')
				level--;

			eat( currtok.Type );
		}
		// < <Content>

		// Due to the >> token, this could have been eaten early...
		if (level == 0)
			eat( Tok_Operator );
		// < <Content> >

		// Extend length of name to last token
		token->Text.Len = ( ( sptr )prevtok.Text.Ptr + prevtok.Text.Len ) - ( sptr )token->Text.Ptr;
	}
}

// Variable parsing is handled in multiple places because its initial signature is shared with function parsing
internal
CodeVar parse_variable_after_name(Context* ctx
	, ModuleFlag        mflags
	, CodeAttributes    attributes
	, CodeSpecifiers    specifiers
	, CodeTypename      type
	, Str               name
)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	Code array_expr    = parse_array_decl(ctx);
	Code expr          = NullCode;
	Code bitfield_expr = NullCode;

	b32 using_constructor_initializer = false;

	if ( bitfield_is_set( u32, currtok.Flags, TF_Assign ) ) {
		// <Attributes> <Specifiers> <ValueType> <Name> = <Expression>
		expr = parse_assignment_expression(ctx);
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

		expr_tok.Text.Len = ( (sptr)prevtok.Text.Ptr + prevtok.Text.Len ) - (sptr)expr_tok.Text.Ptr;
		expr              = untyped_str( expr_tok.Text );
		// <Attributes> <Specifiers> <ValueType> <Name> = { <Expression> }
	}

	if ( currtok.Type == Tok_Paren_Open )
	{
		eat( Tok_Paren_Open);
		// <Attributes> <Specifiers> <ValueType> <Name> (

		Token expr_token = currtok;

		using_constructor_initializer = true;

		s32 level = 0;
		while ( left && ( currtok.Type != Tok_Paren_Close || level > 0 ) )
		{
			if ( currtok.Type == Tok_Paren_Open )
				level++;

			else if ( currtok.Type == Tok_Paren_Close && level > 0 )
				level--;

			eat( currtok.Type );
		}

		expr_token.Text.Len = ( (sptr)prevtok.Text.Ptr + prevtok.Text.Len ) - (sptr)expr_token.Text.Ptr;
		expr                = untyped_str( expr_token.Text );
		eat( Tok_Paren_Close );
		// <Attributes> <Specifiers> <ValueType> <Name> ( <Expression> )
	}

	if ( currtok.Type == Tok_Assign_Classifer )
	{
		eat( Tok_Assign_Classifer );
		// <Attributes> <Specifiers> <ValueType> <Name> :

		Token expr_tok = currtok;

		if ( currtok.Type == Tok_Statement_End ) {
			log_failure( "Expected expression after bitfield \n%SB", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
			parser_pop(& ctx->parser);
			return InvalidCode;
		}

		while ( left && currtok.Type != Tok_Statement_End ) {
			eat( currtok.Type );
		}

		expr_tok.Text.Len = ( (sptr)prevtok.Text.Ptr + prevtok.Text.Len ) - (sptr)expr_tok.Text.Ptr;
		bitfield_expr     = untyped_str( expr_tok.Text );
		// <Attributes> <Specifiers> <ValueType> <Name> : <Expression>
	}

	CodeVar     next_var   = NullCode;
	Token       stmt_end   = NullToken;
	CodeComment inline_cmt = NullCode;
	if ( type )
	{
		if ( currtok.Type == Tok_Comma ) {
			// Were dealing with a statement with more than one declaration
			// This is only handled this way if its the first declaration
			// Otherwise its looped through in parse_variable_declaration_list
			next_var = parse_variable_declaration_list(ctx);
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
		if ( left && ( currtok_noskip.Type == Tok_Comment ) && currtok_noskip.Line == stmt_end.Line ) {
			inline_cmt = parse_comment(ctx);
			// <Attributes> <Specifiers> <ValueType> <Name> : <Expression>, ...; <InlineCmt>
			// <Attributes> <Specifiers> <ValueType> <Name> = <Expression>, ...; <InlineCmt>
			// <Attributes> <Specifiers> <ValueType> <Name> { <Expression> }, ...; <InlineCmt>
		}
	}

	CodeVar
	result              = (CodeVar) make_code();
	result->Type        = CT_Variable;
	result->Name        = cache_str( name );
	result->ModuleFlags = mflags;
	result->ValueType    = type;
	result->BitfieldSize = bitfield_expr;
	result->Attributes   = attributes;
	result->Specs        = specifiers;
	result->Value        = expr;
	result->InlineCmt    = inline_cmt;

	if (array_expr)
		type->ArrExpr = array_expr;

	if ( next_var ) {
		result->NextVar         = next_var;
		result->NextVar->Parent = cast(Code, result);
	}
	result->VarParenthesizedInit = using_constructor_initializer;

	parser_pop(& ctx->parser);
	return result;
}

/*
	Note(Ed): This does not support the following:
	* Function Pointers
*/
internal
CodeVar parse_variable_declaration_list(Context* ctx)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	CodeVar result   = NullCode;
	CodeVar last_var = NullCode;
	while ( check( Tok_Comma ) )
	{
		eat( Tok_Comma );
		// ,

		CodeSpecifiers specifiers = NullCode;

		while ( left && tok_is_specifier(currtok) )
		{
			Specifier spec = str_to_specifier( currtok.Text );

			switch ( spec )
			{
				case Spec_Const:
					if ( specifiers->NumEntries && specifiers->ArrSpecs[ specifiers->NumEntries - 1 ] != Spec_Ptr )
					{
						log_failure( "Error, const specifier must come after pointer specifier for variable declaration proceeding comma\n"
						"(Parser will add and continue to specifiers, but will most likely fail to compile)\n%SB"
						, parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );

						specifiers_append(specifiers, spec );
					}
				break;

				case Spec_Ptr:
				case Spec_Ref:
				case Spec_RValue:
				break;

				default:
				{
					log_failure( "Error, invalid specifier '%S' proceeding comma\n"
					"(Parser will add and continue to specifiers, but will most likely fail to compile)\n%S"
					, currtok.Text, strbuilder_to_str( parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp)) );
					continue;
				}
				break;
			}

			if ( specifiers )
				specifiers_append(specifiers, spec );
			else
				specifiers = def_specifier( spec );

			eat(currtok.Type);
		}
		// , <Specifiers>

		Str name = currtok.Text;
		eat( Tok_Identifier );
		// , <Specifiers> <Name>

		CodeVar var = parse_variable_after_name(ctx, ModuleFlag_None, NullCode, specifiers, NullCode, name );
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

	parser_pop(& ctx->parser);
	return result;
}

internal
CodeClass parser_parse_class(Context* ctx, bool inplace_def)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	CodeClass result = (CodeClass) parse_class_struct(ctx, Tok_Decl_Class, inplace_def );

	parser_pop(& ctx->parser);
	return result;
}

internal
CodeConstructor parser_parse_constructor(Context* ctx, CodeSpecifiers specifiers )
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	Token     identifier = parse_identifier(ctx, nullptr);
	CodeParams params     = parse_params(ctx, parser_not_from_template);
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
			if (currtok.Type == Tok_Paren_Open)
				level++;
			else if ( currtok.Type == Tok_Paren_Close )
				level--;

			eat( currtok.Type );
		}

		initializer_list_tok.Text.Len = ( ( sptr )prevtok.Text.Ptr + prevtok.Text.Len ) - ( sptr )initializer_list_tok.Text.Ptr;
		// <Name> ( <Parameters> ) : <InitializerList>

		initializer_list = untyped_str( initializer_list_tok.Text );

		// TODO(Ed): Constructors can have post-fix specifiers

		body = cast(CodeBody, parse_function_body(ctx));
		// <Name> ( <Parameters> ) : <InitializerList> { <Body> }
	}
	else if ( check( Tok_BraceCurly_Open ) )
	{
		body = cast(CodeBody, parse_function_body(ctx));
		// <Name> ( <Parameters> ) { <Body> }
	}
	// TODO(Ed): Add support for detecting constructor deletion
	else if ( check( Tok_Operator) && currtok.Text.Ptr[ 0 ] == '=' )
	{
		body = cast(CodeBody, parse_assignment_expression(ctx));
	}
	else
	{
		Token stmt_end = currtok;
		eat( Tok_Statement_End );
		// <Name> ( <Parameters> );

		if ( currtok_noskip.Type == Tok_Comment && currtok_noskip.Line == stmt_end.Line )
			inline_cmt = parse_comment(ctx);
		// <Name> ( <Parameters> ); <InlineCmt>
	}

	CodeConstructor result = ( CodeConstructor )make_code();

	result->Name = cache_str( identifier.Text );

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

	parser_pop(& ctx->parser);
	return result;
}

internal inline
CodeDefine parser_parse_define(Context* ctx)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	if ( check(Tok_Preprocess_Hash)) {
		// If parse_define is called by the user the hash reach here.
		eat(Tok_Preprocess_Hash);
	}

	eat( Tok_Preprocess_Define );
	// #define

	CodeDefine
	define = (CodeDefine) make_code();
	define->Type = CT_Preprocess_Define;
	if ( ! check( Tok_Identifier ) ) {
		log_failure( "Error, expected identifier after #define\n%s", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
		parser_pop(& ctx->parser);
		return InvalidCode;
	}
	ctx->parser.Scope->Name = currtok.Text;
	define->Name = cache_str( currtok.Text );
	eat( Tok_Identifier );
	// #define <Name>

	Macro* macro = lookup_macro(define->Name);
	if (macro_is_functional(* macro)) {
		eat( Tok_Paren_Open );
		// #define <Name> (

		// We provide the define params even if empty to make sure '()' are serialized.
		CodeDefineParams
		params = (CodeDefineParams) make_code();
		params->Type = CT_Parameters_Define;

		if ( left && currtok.Type != Tok_Paren_Close ) {
			params->Name = currtok.Text;
			params->NumEntries ++;

			eat( Tok_Preprocess_Define_Param );
			// #define <Name> ( <param>
		}
		
		while( left && currtok.Type != Tok_Paren_Close ) {
			eat( Tok_Comma );
			// #define <Name> ( <param>, 

			CodeDefineParams next_param = (CodeDefineParams) make_code();
			next_param->Type = CT_Parameters_Define;
			next_param->Name = currtok.Text;
			define_params_append(params, next_param);

			// #define  <Name> (  <param>, <next_param> ...
			eat( Tok_Preprocess_Define_Param );
		}

		eat( Tok_Paren_Close );
		// #define <Name> ( <params> )

		define->Params = params;
	}

	if ( ! check( Tok_Preprocess_Content ))
	{
		log_failure( "Error, expected content after #define %s\n%s", define->Name, parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
		parser_pop(& ctx->parser);
		return InvalidCode;
	}

	if ( currtok.Text.Len == 0 )
	{
		define->Body = untyped_str( txt("\n") );
		eat( Tok_Preprocess_Content );
		// #define <Name> ( <params> ) <Content>

		parser_pop(& ctx->parser);
		return define;
	}

	define->Body = untyped_str( strbuilder_to_str( parser_strip_formatting(ctx, currtok.Text, parser_strip_formatting_dont_preserve_newlines )) );
	eat( Tok_Preprocess_Content );
	// #define <Name> ( <params> ) <Content>

	parser_pop(& ctx->parser);
	return define;
}

internal
CodeDestructor parser_parse_destructor(Context* ctx, CodeSpecifiers specifiers)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	bool has_context         = ctx->parser.Scope && ctx->parser.Scope->Prev;
	bool is_in_global_nspace = has_context && str_are_equal( ctx->parser.Scope->Prev->ProcName, txt("parse_global_nspace") );

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
		prefix_identifier = parse_identifier(ctx, nullptr);

	if ( left && currtok.Text.Ptr[ 0 ] == '~' )
		eat( Tok_Operator );
	else
	{
		log_failure( "Expected destructor '~' token\n%s", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
		parser_pop( & ctx->parser);
		return InvalidCode;
	}
	// <Virtual Specifier> ~

	Token       identifier = parse_identifier(ctx, nullptr);
	CodeBody    body       = { nullptr };
	CodeComment inline_cmt = NullCode;
	// <Virtual Specifier> ~<Name>

	eat( Tok_Paren_Open );
	eat( Tok_Paren_Close );
	// <Virtual Specifier> ~<Name>()

	bool pure_virtual = false;

	if ( check( Tok_Operator ) && currtok.Text.Ptr[ 0 ] == '=' )
	{
		// <Virtual Specifier> ~<Name>() =

		bool skip_formatting = true;
		Token upcoming = nexttok;
		if ( left && upcoming.Text.Ptr[ 0 ] == '0' )
		{
			eat( Tok_Operator );
			eat( Tok_Number );
			// <Virtual Specifier> ~<Name>() = 0

			specifiers_append(specifiers, Spec_Pure );
		}
		else if ( left && c_str_compare_len( upcoming.Text.Ptr, "default", sizeof("default") - 1 ) == 0) 
		{
			body = cast(CodeBody, parse_assignment_expression(ctx));
			// <Virtual Specifier> ~<
		}
		else
		{
			log_failure( "Pure or default specifier expected due to '=' token\n%s", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
			parser_pop( & ctx->parser);
			return InvalidCode;
		}

		pure_virtual = true;
	}

	if ( ! pure_virtual && check( Tok_BraceCurly_Open ) )
	{
		body = cast(CodeBody, parse_function_body(ctx));
		// <Virtual Specifier> ~<Name>() { ... }
	}
	else
	{
		Token stmt_end = currtok;
		eat( Tok_Statement_End );
		// <Virtual Specifier> ~<Name>() <Pure Specifier>;

		if ( currtok_noskip.Type == Tok_Comment && currtok_noskip.Line == stmt_end.Line )
			inline_cmt = parse_comment(ctx);
		// <Virtual Specifier> ~<Name>() <Pure Specifier>; <InlineCmt>
	}

	CodeDestructor result = ( CodeDestructor )make_code();

	if ( tok_is_valid(prefix_identifier) ) {
		prefix_identifier.Text.Len += 1 + identifier.Text.Len;
		result->Name = cache_str( prefix_identifier.Text );
	}

	if ( specifiers )
		result->Specs = specifiers;

	if ( body && body->Type == CT_Function_Body ) {
		result->Body = cast(Code, body);
		result->Type = CT_Destructor;
	}
	else
		result->Type = CT_Destructor_Fwd;

	if ( inline_cmt )
		result->InlineCmt = inline_cmt;

	parser_pop(& ctx->parser);
	return result;
}

internal
CodeEnum parser_parse_enum(Context* ctx, bool inplace_def)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	Specifier specs_found[16] = { Spec_NumSpecifiers };
	s32       NumSpecifiers = 0;

	CodeAttributes attributes = { nullptr };

	Token        name       = NullToken;
	Code         array_expr = { nullptr };
	CodeTypename type       = { nullptr };

	bool is_enum_class = false;

	eat( Tok_Decl_Enum );
	// enum

	if ( currtok.Type == Tok_Decl_Class )
	{
		eat( Tok_Decl_Class);
		is_enum_class = true;
		// enum class
	}

	attributes = parse_attributes(ctx);
	// enum <class> <Attributes>

	if ( check( Tok_Identifier ) )
	{
		name = currtok;
		ctx->parser.Scope->Name = currtok.Text;
		eat( Tok_Identifier );
	}
	// enum <class> <Attributes> <Name>

	b32  use_macro_underlying = false;
	Code underlying_macro     = { nullptr };
	if ( currtok.Type == Tok_Assign_Classifer )
	{
		eat( Tok_Assign_Classifer );
		// enum <class> <Attributes> <Name> :

		type = parser_parse_type(ctx, parser_not_from_template, nullptr);
		if ( cast(Code, type) == Code_Invalid )
		{
			log_failure( "Failed to parse enum classifier\n%s", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
			parser_pop(& ctx->parser);
			return InvalidCode;
		}
		// enum <class> <Attributes> <Name> : <UnderlyingType>
	}
	else if ( currtok.Type == Tok_Preprocess_Macro_Expr )
	{
		// We'll support the enum_underlying macro
		if ( str_contains( currtok.Text, enum_underlying_macro.Name) )
		{
			use_macro_underlying = true;
			underlying_macro     = parse_simple_preprocess(ctx, Tok_Preprocess_Macro_Expr );
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
				log_failure( "Did not expect an entry after last member of enum body.\n%s", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
				parser_pop(& ctx->parser);
				break;
			}

			if ( currtok_noskip.Type == Tok_Preprocess_Hash )
				eat( Tok_Preprocess_Hash );

			switch ( currtok_noskip.Type )
			{
				case Tok_NewLine:
					member = untyped_str( currtok_noskip.Text );
					eat( Tok_NewLine );
				break;

				case Tok_Comment:
					member = cast(Code, parse_comment(ctx));
				break;

				case Tok_Preprocess_Define:
					member = cast(Code, parser_parse_define(ctx));
					// #define
				break;

				case Tok_Preprocess_If:
				case Tok_Preprocess_IfDef:
				case Tok_Preprocess_IfNotDef:
				case Tok_Preprocess_ElIf:
					member = cast(Code, parse_preprocess_cond(ctx));
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

				case Tok_Preprocess_Macro_Stmt: {
					member = cast(Code, parse_simple_preprocess(ctx, Tok_Preprocess_Macro_Stmt ));
					// <Macro>
					break;
				}

				case Tok_Preprocess_Pragma:
					member = cast(Code, parse_pragma(ctx));
					// #pragma
				break;

				case Tok_Preprocess_Unsupported:
					member = cast(Code, parse_simple_preprocess(ctx, Tok_Preprocess_Unsupported ));
					// #<UNSUPPORTED>
				break;

				default:
				{
					Token entry = currtok;

					eat( Tok_Identifier);
					// <Name>

					if ( currtok.Type == Tok_Operator && currtok.Text.Ptr[0] == '=' )
					{
						eat( Tok_Operator );
						// <Name> =

						while ( currtok.Type != Tok_Comma && currtok.Type != Tok_BraceCurly_Close ) {
							eat( currtok.Type );
						}
					}
					// <Name> = <Expression>

					// Unreal UMETA macro support
					if ( currtok.Type == Tok_Preprocess_Macro_Expr ) {
						Code macro = parse_simple_preprocess(ctx, Tok_Preprocess_Macro_Expr );
						// <Name> = <Expression> <Macro>

						// We're intentially ignoring this code as its going to be serialized as an untyped string with the rest of the enum "entry".
						// TODO(Ed): We need a CodeEnumEntry, AST_EnumEntry types
					}

					if ( currtok.Type == Tok_Comma )
					{
						//Token prev = * previous(ctx->parser.Tokens, dont_skip_formatting);
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

					Token prev     = * lex_previous(& ctx->parser, lex_dont_skip_formatting);
					entry.Text.Len = ( (sptr)prev.Text.Ptr + prev.Text.Len ) - (sptr)entry.Text.Ptr;

					member = untyped_str( entry.Text );
				}
				break;
			}

			if ( member == Code_Invalid ) {
				log_failure( "Failed to parse member\n%s", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
				parser_pop(& ctx->parser);
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
			inline_cmt = parse_comment(ctx);
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

	result->Name = cache_str( name.Text );

	if ( attributes )
		result->Attributes = attributes;

	result->UnderlyingTypeMacro = underlying_macro;
	result->UnderlyingType      = type;

	if ( inline_cmt )
		result->InlineCmt = inline_cmt;


	parser_pop(& ctx->parser);
	return result;
}

internal inline
CodeBody parser_parse_export_body(Context* ctx)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	CodeBody result = parse_global_nspace(ctx, CT_Export_Body );

	parser_pop(& ctx->parser);
	return result;
}

internal inline
CodeBody parser_parse_extern_link_body(Context* ctx)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	CodeBody result = parse_global_nspace(ctx, CT_Extern_Linkage_Body );

	parser_pop(& ctx->parser);
	return result;
}

internal
CodeExtern parser_parse_extern_link(Context* ctx)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	eat( Tok_Decl_Extern_Linkage );
	// extern

	Token name = currtok;
	eat( Tok_String );
	// extern "<Name>"

	name.Text.Ptr += 1;
	name.Text.Len -= 1;

	CodeExtern
	result       = (CodeExtern) make_code();
	result->Type = CT_Extern_Linkage;
	result->Name = cache_str( name.Text );

	CodeBody entry = parser_parse_extern_link_body(ctx);
	if ( cast(Code, entry) == Code_Invalid )
	{
		log_failure( "Failed to parse body\n%s", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
		parser_pop(& ctx->parser);
		return result;
	}
	// extern "<Name>" { <Body> }

	result->Body = entry;

	parser_pop(& ctx->parser);
	return result;
}

internal
CodeFriend parser_parse_friend(Context* ctx)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

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
			Specifier spec = str_to_specifier( currtok.Text );

			switch ( spec )
			{
				GEN_PARSER_FRIEND_ALLOWED_SPECIFIER_CASES:
				break;

				default :
					log_failure( "Invalid specifier %S for friend definition\n%S", spec_to_str( spec ), strbuilder_to_str( parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp)) );
					parser_pop(& ctx->parser);
					return InvalidCode;
			}

			// Ignore const it will be handled by the type
			if ( spec == Spec_Const )
				break;

			specs_found[ NumSpecifiers ] = spec;
			NumSpecifiers++;
			eat( currtok.Type );
		}

		if ( NumSpecifiers ) {
			specifiers = def_specifiers_arr( NumSpecifiers, specs_found );
		}
		// <friend> <specifiers>
	}

	// Type declaration or return type
	CodeTypename type = parser_parse_type(ctx, parser_not_from_template, nullptr);
	if ( cast(Code, type) == Code_Invalid ) {
		parser_pop(& ctx->parser);
		return InvalidCode;
	}
	// friend <Type>

	// Funciton declaration
	if ( currtok.Type == Tok_Identifier )
	{
		// Name
		Token name          = parse_identifier(ctx, nullptr);
		ctx->parser.Scope->Name = name.Text;
		// friend <ReturnType> <Name>

		function = parse_function_after_name(ctx, ModuleFlag_None, NullCode, specifiers, type, name );

		// Parameter list
		// CodeParams params = parse_params();
		// friend <ReturnType> <Name> ( <Parameters> )

		// function             = make_code();
		// function->Type       = Function_Fwd;
		// function->Name       = cache_str( name );
		// function->ReturnType = type;

		// if ( params )
			// function->Params = params;
	}

	// Operator declaration or definition
	if ( currtok.Type == Tok_Decl_Operator ) {
		op = parse_operator_after_ret_type(ctx, ModuleFlag_None, NullCode, specifiers, type);
	}

	CodeComment inline_cmt = NullCode;
	if ( function && function->Type == CT_Function_Fwd )
	{
		Token stmt_end = currtok;
		eat( Tok_Statement_End );
		// friend <Type>;
		// friend <ReturnType> <Name> ( <Parameters> );

		if ( currtok_noskip.Type == Tok_Comment && currtok_noskip.Line == stmt_end.Line )
			inline_cmt = parse_comment(ctx);
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

	parser_pop(& ctx->parser);
	return result;
}

internal
CodeFn parser_parse_function(Context* ctx)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	Specifier specs_found[16] = { Spec_NumSpecifiers };
	s32        NumSpecifiers = 0;

	CodeAttributes attributes = { nullptr };
	CodeSpecifiers specifiers = { nullptr };
	ModuleFlag     mflags     = ModuleFlag_None;

	if ( check(Tok_Module_Export) ) {
		mflags = ModuleFlag_Export;
		eat( Tok_Module_Export );
	}
	// <export>

	attributes = parse_attributes(ctx);
	// <export> <Attributes>

	while ( left && tok_is_specifier(currtok) )
	{
		Specifier spec = str_to_specifier( currtok.Text );

		switch ( spec )
		{
			GEN_PARSER_FUNCTION_ALLOWED_SPECIFIER_CASES:
			break;

			default:
				log_failure( "Invalid specifier %S for functon\n%SB", spec_to_str(spec), parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
				parser_pop(& ctx->parser);
				return InvalidCode;
		}

		if ( spec == Spec_Const )
			continue;

		specs_found[NumSpecifiers] = spec;
		NumSpecifiers++;
		eat( currtok.Type );
	}

	if ( NumSpecifiers ) {
		specifiers = def_specifiers_arr( NumSpecifiers, specs_found );
	}
	// <export> <Attributes> <Specifiers>

	CodeTypename ret_type = parser_parse_type(ctx, parser_not_from_template, nullptr);
	if ( cast(Code, ret_type) == Code_Invalid ) {
		parser_pop(& ctx->parser);
		return InvalidCode;
	}
	// <export> <Attributes> <Specifiers> <ReturnType>

	Token name = parse_identifier(ctx, nullptr);
	ctx->parser.Scope->Name = name.Text;
	if ( ! tok_is_valid(name) ) {
		parser_pop(& ctx->parser);
		return InvalidCode;
	}
	// <export> <Attributes> <Specifiers> <ReturnType> <Name>

	CodeFn result = parse_function_after_name(ctx, mflags, attributes, specifiers, ret_type, name );
	// <export> <Attributes> <Specifiers> <ReturnType> <Name> ...

	parser_pop(& ctx->parser);
	return result;
}

internal
CodeNS parser_parse_namespace(Context* ctx)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	eat( Tok_Decl_Namespace );
	// namespace

	Token name = parse_identifier(ctx, nullptr);
	ctx->parser.Scope->Name = name.Text;
	// namespace <Name>

	CodeBody body = parse_global_nspace(ctx, CT_Namespace_Body );
	if ( cast(Code, body) == Code_Invalid ) {
		parser_pop(& ctx->parser);
		return InvalidCode;
	}
	// namespace <Name> { <Body> }

	CodeNS
	result       = (CodeNS) make_code();
	result->Type = CT_Namespace;
	result->Name = cache_str( name.Text );

	result->Body = body;

	parser_pop(& ctx->parser);
	return result;
}

internal
CodeOperator parser_parse_operator(Context* ctx)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	CodeAttributes attributes = { nullptr };
	CodeSpecifiers specifiers = { nullptr };
	ModuleFlag     mflags     = ModuleFlag_None;

	Specifier specs_found[16] = { Spec_NumSpecifiers };
	s32        NumSpecifiers = 0;

	if ( check(Tok_Module_Export) ) {
		mflags = ModuleFlag_Export;
		eat( Tok_Module_Export );
	}
	// <export>

	attributes = parse_attributes(ctx);
	// <export> <Attributes>

	while ( left && tok_is_specifier(currtok) )
	{
		Specifier spec = str_to_specifier( currtok.Text );

		switch ( spec )
		{
			GEN_PARSER_OPERATOR_ALLOWED_SPECIFIER_CASES:
			break;

			default:
				log_failure( "Invalid specifier " "%S" " for operator\n%SB", spec_to_str(spec), parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
				parser_pop(& ctx->parser);
				return InvalidCode;
		}

		if ( spec == Spec_Const )
			continue;

		specs_found[NumSpecifiers] = spec;
		NumSpecifiers++;
		eat( currtok.Type );
	}

	if ( NumSpecifiers ) {
		specifiers = def_specifiers_arr( NumSpecifiers, specs_found );
	}
	// <export> <Attributes> <Specifiers>

	// Parse Return Type
	CodeTypename ret_type = parser_parse_type(ctx, parser_not_from_template, nullptr);
	// <export> <Attributes> <Specifiers> <ReturnType>

	CodeOperator result = parse_operator_after_ret_type(ctx, mflags, attributes, specifiers, ret_type);
	// <export> <Attributes> <Specifiers> <ReturnType> ...

	parser_pop(& ctx->parser);
	return result;
}

internal
CodeOpCast parser_parse_operator_cast(Context* ctx, CodeSpecifiers specifiers)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

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

		name.Text.Len = ( (sptr)prevtok.Text.Ptr + prevtok.Text.Len ) - (sptr)name.Text.Ptr;
	}

	eat( Tok_Decl_Operator );
	// <Specifiers> <Qualifier> :: ... operator

	CodeTypename type = parser_parse_type(ctx, parser_not_from_template, nullptr);
	// <Specifiers> <Qualifier> :: ... operator <UnderlyingType>

	Str scope_name         = { type->Name.Ptr, type->Name.Len };
	Token scope_name_tok   = { scope_name, Tok_Identifier, 0, 0, TF_Null };
	ctx->parser.Scope->Name = scope_name_tok.Text;

	eat( Tok_Paren_Open );
	eat( Tok_Paren_Close );
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
		body_str.Text.Len = ( (sptr)prevtok.Text.Ptr + prevtok.Text.Len ) - (sptr)body_str.Text.Ptr;

		eat( Tok_BraceCurly_Close );
		// <Specifiers> <Qualifier> :: ... operator <UnderlyingType>() <const> { <Body> }

		body = untyped_str( body_str.Text );
	}
	else
	{
		Token stmt_end = currtok;
		eat( Tok_Statement_End );
		// <Specifiers> <Qualifier> :: ... operator <UnderlyingType>() <const>;

		if ( currtok_noskip.Type == Tok_Comment && currtok_noskip.Line == stmt_end.Line )
			inline_cmt = parse_comment(ctx);
			// <Specifiers> <Qualifier> :: ... operator <UnderlyingType>() <const>; <InlineCmt>
	}

	CodeOpCast result = (CodeOpCast) make_code();

	if ( tok_is_valid(name) )
		result->Name = cache_str( name.Text );

	if (body) {
		result->Type = CT_Operator_Cast;
		result->Body = cast(CodeBody, body);
	}
	else {
		result->Type = CT_Operator_Cast_Fwd;
	}

	if ( specifiers )
		result->Specs = specifiers;

	result->ValueType = cast(CodeTypename, type);

	parser_pop(& ctx->parser);
	return result;
}

internal inline
CodeStruct parser_parse_struct(Context* ctx, bool inplace_def)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );
	CodeStruct result = (CodeStruct) parse_class_struct(ctx, Tok_Decl_Struct, inplace_def );
	parser_pop(& ctx->parser);
	return result;
}

internal
CodeTemplate parser_parse_template(Context* ctx)
{
#define UseTemplateCapture true

	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	ModuleFlag mflags = ModuleFlag_None;

	if ( check( Tok_Module_Export ) ) {
		mflags = ModuleFlag_Export;
		eat( Tok_Module_Export );
	}
	// <export> template

	eat( Tok_Decl_Template );
	// <export> template

	CodeParams params = parse_params(ctx, UseTemplateCapture);
	if ( cast(Code, params) == Code_Invalid ) {
		parser_pop(& ctx->parser);
		return InvalidCode;
	}
	// <export> template< <Parameters> >

	Code definition = { nullptr };

	while ( left )
	{
		if ( check( Tok_Decl_Class ) )
		{
			definition = cast(Code, parser_parse_class(ctx, parser_not_inplace_def));
			// <export> template< <Parameters> > class ...
			break;
		}

		if ( check( Tok_Decl_Struct ) )
		{
			definition = cast(Code, parser_parse_struct(ctx, parser_not_inplace_def));
			// <export> template< <Parameters> > struct ...
			break;
		}

		if ( check( Tok_Decl_Union ) )
		{
			definition = cast(Code, parser_parse_union(ctx, parser_not_inplace_def));
			// <export> template< <Parameters> > union ...
			break;
		}

		if ( check( Tok_Decl_Using ) )
		{
			definition = cast(Code, parser_parse_using(ctx));
			// <export> template< <Parameters> > using ...
			break;
		}

		// Its either a function or a variable
		Token name                = NullToken;

		CodeAttributes attributes = { nullptr };
		CodeSpecifiers specifiers = { nullptr };

		bool expects_function     = false;

		Specifier specs_found[ 16 ] = { Spec_NumSpecifiers };
		s32        NumSpecifiers = 0;

		attributes = parse_attributes(ctx);
		// <export> template< <Parameters> > <Attributes>

		// Specifiers Parsing
		{
			while ( left && tok_is_specifier(currtok) )
			{
				Specifier spec = str_to_specifier( currtok.Text );

				switch ( spec )
				{
					GEN_PARSER_TEMPLATE_ALLOWED_SPECIFIER_CASES:
					break;

					case Spec_Consteval :
						expects_function = true;
						break;

					default :
						log_failure( "Invalid specifier %S for variable or function\n%SB", spec_to_str( spec ), parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
						parser_pop(& ctx->parser);
						return InvalidCode;
				}

				// Ignore const it will be handled by the type
				if ( spec == Spec_Const )
					break;

				specs_found[ NumSpecifiers ] = spec;
				NumSpecifiers++;
				eat( currtok.Type );
			}

			if ( NumSpecifiers ) {
				specifiers = def_specifiers_arr( NumSpecifiers, specs_found );
			}
			// <export> template< <Parameters> > <Attributes> <Specifiers>
		}


		bool has_context         = ctx->parser.Scope && ctx->parser.Scope->Prev;
		bool is_in_global_nspace = has_context && str_are_equal( ctx->parser.Scope->Prev->ProcName, txt("parse_global_nspace") );
		// Possible constructor implemented at global file scope.
		if (is_in_global_nspace)
		{
			Code constructor_destructor = parse_global_nspace_constructor_destructor(ctx, specifiers );
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
			s32  idx = ctx->parser.token_id;

			for ( ; idx < ctx->parser.tokens.num; idx++ )
			{
				Token tok = ctx->parser.tokens.ptr[ idx ];

				if ( tok.Type == Tok_Identifier )
				{
					idx++;
					tok = ctx->parser.tokens.ptr[ idx ];
					if ( tok.Type == Tok_Access_StaticSymbol )
						continue;

					break;
				}

				if ( tok.Type == Tok_Decl_Operator )
					found_operator_cast_outside_class_implmentation = true;

				break;
			}

			if ( found_operator_cast_outside_class_implmentation ) {
				definition = cast(Code, parser_parse_operator_cast(ctx, specifiers));
				// <Attributes> <Specifiers> <Name> :: operator <Type> () { ... }
				break;
			}
		}

		definition = parse_operator_function_or_variable(ctx, expects_function, attributes, specifiers);
		// <export> template< <Parameters> > <Attributes> <Specifiers> ...
		break;
	}

	CodeTemplate result = ( CodeTemplate )make_code();
	result->Type        = CT_Template;
	result->Params      = params;
	result->Declaration = definition;
	result->ModuleFlags = mflags;
	// result->Name        = definition->Name;

	parser_pop(& ctx->parser);
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
CodeTypename parser_parse_type(Context* ctx, bool from_template, bool* typedef_is_function)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	Token context_tok = prevtok;

	Specifier specs_found[ 16 ] = { Spec_NumSpecifiers };
	s32       NumSpecifiers = 0;

	Token name= NullToken;

	ETypenameTag tag = Tag_None;

	// Attributes are assumed to be before the type signature
	CodeAttributes attributes = parse_attributes(ctx);
	// <Attributes>

	// Prefix specifiers
	while ( left && tok_is_specifier(currtok) )
	{
		Specifier spec = str_to_specifier( currtok.Text );

		if ( spec != Spec_Const ) {
			log_failure( "Error, invalid specifier used in type definition: %S\n%SB", currtok.Text, parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
			parser_pop(& ctx->parser);
			return InvalidCode;
		}

		specs_found[ NumSpecifiers ] = spec;
		NumSpecifiers++;
		eat( currtok.Type );
	}
	// <Attributes> <Specifiers>

	if ( left == 0 ) {
		log_failure( "Error, unexpected end of type definition\n%SB", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
		parser_pop(& ctx->parser);
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
		Token next = nexttok;

		if (next.Type == Tok_Identifier)
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

			name = parse_identifier(ctx, nullptr);
			// <Attributes> <Specifiers> <class, enum, struct, union> <Name> 
		}
		else if  (next.Type == Tok_BraceCurly_Open)
		{
			name = currtok;
			// We have an inplace definition, we need to consume that...

			// TODO(Ed): we need to add a way for AST_CodeTypename to track an implace definition..
			b32 const inplace = true;
			Code indplace_def = cast(Code, parser_parse_struct(ctx, inplace));

			// For now we lose the structural information, 
			name.Text.Len = ( ( sptr )prevtok.Text.Ptr + prevtok.Text.Len ) - ( sptr )name.Text.Ptr;
			// <Attributes> <Specifiers> <class, enum, struct, union> <inplace def>
		}
		ctx->parser.Scope->Name = name.Text;
		// <Attributes> <Specifiers> <class, enum, struct, union> <Name>
	}

// Decltype draft implementation
#if 0
else if ( currtok.Type == Tok_DeclType )
{
	// Will have a capture and its own parsing rules, were going to just shove everything in a string (for now).
	name = currtok;
	eat( Tok_DeclType );
	// <Attributes> <Specifiers> decltype

	eat( Tok_Paren_Open );
	while ( left && currtok.Type != Tok_Paren_Close )
	{
		if ( currtok.Type == Tok_Paren_Open )
			level++;

		if ( currtok.Type == Tok_Paren_Close )
			level--;

		eat( currtok.Type );
	}
	eat( Tok_Paren_Close );

	name.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)name.Text;
	ctx->parser.Scope->Name = name;
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

		name.Text.Len = ( ( sptr )prevtok.Text.Ptr + prevtok.Text.Len ) - ( sptr )name.Text.Ptr;
		// <Attributes> <Specifiers> <Compound type expression>
	}
	else if ( currtok.Type == Tok_Type_Typename )
	{
		name = currtok;
		eat(Tok_Type_Typename);
		// <typename>

		if ( ! from_template )
		{
			name                = parse_identifier(ctx, nullptr);
			ctx->parser.Scope->Name = name.Text;
			if ( ! tok_is_valid(name) )
			{
				log_failure( "Error, failed to type signature\n%s", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
				parser_pop(& ctx->parser);
				return InvalidCode;
			}
		}
	}
	else if ( currtok.Type == Tok_Preprocess_Macro_Typename ) {
		// Typename is a macro
		// name = currtok;
		// eat(Tok_Preprocess_Macro_Typename);
		Code macro = parse_simple_preprocess(ctx, Tok_Preprocess_Macro_Typename);
		name.Text = macro->Content;
	}

	// The usual Identifier type signature that may have namespace qualifiers
	else
	{
		name = parse_identifier(ctx, nullptr);
		ctx->parser.Scope->Name = name.Text;
		if ( ! tok_is_valid(name) )
		{
			log_failure( "Error, failed to type signature\n%s", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
			parser_pop(& ctx->parser);
			return InvalidCode;
		}
		// <Attributes> <Specifiers> <Qualifier ::> <Identifier>
		// <Attributes> <Specifiers> <Identifier>
	}

	// Suffix specifiers for typename.
	while ( left && tok_is_specifier(currtok) )
	{
		Specifier spec = str_to_specifier( currtok.Text );

		switch (spec ) {
			GEN_PARSER_TYPENAME_ALLOWED_SUFFIX_SPECIFIER_CASES:
			break;

			default: {
				log_failure( "Error, invalid specifier used in type definition: %S\n%SB", currtok.Text, parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
				parser_pop(& ctx->parser);
				return InvalidCode;
			}
		}
		specs_found[ NumSpecifiers ] = spec;
		NumSpecifiers++;
		eat( currtok.Type );
	}

#ifdef GEN_USE_NEW_TYPENAME_PARSING
	if ( NumSpecifiers )
	{
		specifiers    = def_specifiers_arr( NumSpecifiers, specs_found );
		NumSpecifiers = 0;
	}
#endif
	// <Attributes> <Specifiers> <Identifier> <Specifiers>

	// For function type signatures
	CodeTypename return_type = NullCode;
	CodeParams   params      = NullCode;

#ifdef GEN_USE_NEW_TYPENAME_PARSING
	CodeParams params_nested = NullCode;
#endif

	bool   is_function_typename = false;
	Token* last_capture         = nullptr;
	{
		Token* scanner = ctx->parser.tokens.ptr + ctx->parser.token_id;

		// An identifier being within a typename's signature only occurs if were parsing a typename for a typedef.
		if ( typedef_is_function && scanner->Type == Tok_Identifier )
		{
			is_function_typename = true;
			++scanner;
		}
		is_function_typename = scanner->Type == Tok_Paren_Open;

		Token* first_capture = scanner;
		if ( is_function_typename )
		{
			// Go to the end of the signature
			while ( scanner->Type != Tok_Statement_End && scanner->Type != Tok_BraceCurly_Open )
				++scanner;

			// Go back to the first capture start found
			while ( scanner->Type != Tok_Paren_Open )
				--scanner;

			last_capture = scanner;
		}

		bool has_context   = ctx->parser.Scope && ctx->parser.Scope->Prev;
		bool is_for_opcast = has_context && str_are_equal( ctx->parser.Scope->Prev->ProcName, txt("parser_parse_operator_cast") );
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

		// StrBuilder
		// name_stripped = StrBuilder::make( FallbackAllocator, name );
		// name_stripped.strip_space();
		return_type->Name = cache_str( name.Text );

#ifdef GEN_USE_NEW_TYPENAME_PARSING
		if ( specifiers )
		{
			return_type->Specs = specifiers;
			specifiers         = nullptr;
		}

#else
		if ( NumSpecifiers )
			return_type->Specs = def_specifiers_arr( NumSpecifiers, ( Specifier* )specs_found );

		// Reset specifiers, the function itself will have its own suffix specifiers possibly.
		NumSpecifiers = 0;
#endif
		// <Attributes> <ReturnType>
		name = NullToken;

		// The next token can either be a capture for the identifier or it could be the identifier exposed.
		if ( ! check( Tok_Paren_Open ) )
		{
			// Started with an identifier immeidately, which means its of the format: <ReturnType> <identifier> <capture>;
			name = parse_identifier(ctx, nullptr);
		}
		// <Attributes> <ReturnType> <Identifier>

		// If the next token is a capture start and is not the last capture, then we're dealing with function typename whoose identifier is within the
		// capture.
		else if ( ( ctx->parser.tokens.ptr + ctx->parser.token_id ) != last_capture )
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
			eat( Tok_Paren_Open );
			// <Attributes> <ReturnType> (

			// Binding specifiers
			while ( left && currtok.is_specifier() )
			{
				Specifier spec = to_type( currtok );

				if ( spec != Spec_Ptr && spec != Spec_Ref && spec != Spec_RValue )
				{
					log_failure( "Error, invalid specifier used in type definition: %S\n%SB", toktype_to_str(currtok), to_strbuilder(ctx->parser) );
					pop(& ctx->parser);
					return InvalidCode;
				}

				specs_found[ NumSpecifiers ] = spec;
				NumSpecifiers++;
				eat( currtok.Type );
			}

			if ( NumSpecifiers )
			{
				specifiers = def_specifiers_arr( NumSpecifiers, specs_found );
			}
			NumSpecifiers = 0;
			// <Attributes> <ReturnType> ( <Specifiers>

			if ( check( Tok_Identifier ) )
				name = parse_identifier();
			// <Attributes> <ReturnType> ( <Specifiers> <Identifier>

			// Immeidate parameters

			if ( check( Tok_Paren_Open ) )
				params_nested = parse_params();
			// <Attributes> <ReturnType> ( <Specifiers> <Identifier> ( <Parameters> )

			eat( Tok_Paren_Close );
			// <Attributes> <ReturnType> ( <Specifiers> <Identifier> ( <Parameters> ) )

#else
			// Starting immediatley with a capture, most likely declaring a typename for a member function pointer.
			// Everything within this capture will just be shoved into the name field including the capture tokens themselves.
			name = currtok;

			eat( Tok_Paren_Open );
			// <Attributes> <ReturnType> (

			s32 level = 0;
			while ( left && ( currtok.Type != Tok_Paren_Close || level > 0 ) )
			{
				if ( currtok.Type == Tok_Paren_Open )
					level++;

				if ( currtok.Type == Tok_Paren_Close )
					level--;

				eat( currtok.Type );
			}
			eat( Tok_Paren_Close );
			// <Attributes> <ReturnType> ( <Expression> )

			name.Text.Len = ( ( sptr )prevtok.Text.Ptr + prevtok.Text.Len ) - ( sptr )name.Text.Ptr;
#endif
		}

		// Were now dealing with the parameters of the function
		params = parse_params(ctx, parser_use_parenthesis);
		// <Attributes> <ReturnType> <All Kinds of nonsense> ( <Parameters> )

		// Look for suffix specifiers for the function
		while ( left && tok_is_specifier(currtok) )
		{
			Specifier spec = str_to_specifier( currtok.Text );

			if ( spec != Spec_Const
					// TODO : Add support for NoExcept, l-value, volatile, l-value, etc
					// && spec != Spec_NoExcept
					&& spec != Spec_RValue )
			{
				log_failure( "Error, invalid specifier used in type definition: %S\n%S", currtok.Text, strbuilder_to_str( parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp)) );
				parser_pop(& ctx->parser);
				return InvalidCode;
			}

			specs_found[ NumSpecifiers ] = spec;
			NumSpecifiers++;
			eat( currtok.Type );
		}

#ifdef GEN_USE_NEW_TYPENAME_PARSING
		if ( NumSpecifiers )
		{
			func_suffix_specs = def_specifiers_arr( NumSpecifiers, specs_found );
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
	// result->Token = ctx->parser.Scope->Start;

	// Need to wait until were using the new parsing method to do this.
	StrBuilder name_stripped = parser_strip_formatting(ctx, name.Text, parser_strip_formatting_dont_preserve_newlines );

	// name_stripped.strip_space();

#ifdef GEN_USE_NEW_TYPENAME_PARSING
	if ( params_nested )
	{
		name_stripped.append( params_nested->to_strbuilder() );
	}
#endif

	result->Name = cache_str( strbuilder_to_str(name_stripped) );

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
		CodeSpecifiers specifiers = def_specifiers_arr( NumSpecifiers, ( Specifier* )specs_found );
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

	parser_pop(& ctx->parser);
	return result;
}

internal
CodeTypedef parser_parse_typedef(Context* ctx)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	bool  is_function = false;
	Token name        = NullToken;
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

	// TODO(Ed): UPDATE MACRO USAGE HERE
#if GEN_PARSER_DISABLE_MACRO_TYPEDEF
	if ( false )
#else
	b32 valid_macro = false;
	valid_macro |= left && currtok.Type == Tok_Preprocess_Macro_Typename;
	valid_macro |= left && currtok.Type == Tok_Preprocess_Macro_Stmt;
	// if (currtok.Type == Tok_Preprocess_Macro_Stmt)
	// {
		// PreprocessMacro* macro = lookup_macro(currtok.Text);
		// valid_macro |= macro && macro_expects_body(* macro));
	// }

	if ( valid_macro )
#endif
	{
		type          = cast(Code, t_empty);
		name          = currtok;
		Code macro    = parse_simple_preprocess(ctx, currtok.Type);
		name.Text.Len = macro->Content.Len;
		ctx->parser.Scope->Name = name.Text;
		// <ModuleFalgs> typedef <Preprocessed_Macro>

		if ( currtok.Type == Tok_Identifier )
		{
			type = macro;
			name = currtok;
			eat(Tok_Identifier);
			// <ModuleFalgs> typedef <Preprocessed_Macro> <Identifier>
		}
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
			TokenSlice tokens = ctx->parser.tokens;
			TokType    which  = currtok.Type;

			s32 idx = ctx->parser.token_id;
			s32 level = 0;
			for ( ; idx < tokens.num; idx ++ )
			{
				if ( tokens.ptr[idx].Type == Tok_BraceCurly_Open )
					level++;

				if ( tokens.ptr[idx].Type == Tok_BraceCurly_Close )
					level--;

				if ( level == 0 && tokens.ptr[idx].Type == Tok_Statement_End )
					break;
			}

			Token pre_foward_tok = currtok;
			if ( (idx - 3 ) == ctx->parser.token_id )
			{
				// Its a forward declaration only
				type = parse_forward_or_definition(ctx, which, from_typedef );
				// <ModuleFalgs> typedef <UnderlyingType: Forward Decl>
			}
			else
			{
				Token tok = tokens.ptr[ idx - 1 ];
				if ( tok.Type == Tok_Identifier )
				{
					log_fmt("Found id\n");
					tok = tokens.ptr[ idx - 2 ];

					bool is_indirection = tok.Type == Tok_Ampersand
					||                    tok.Type == Tok_Star;

					bool ok_to_parse = false;

					Token temp_3 = tokens.ptr[ idx - 3 ];

					if ( tok.Type == Tok_BraceCurly_Close )
					{
						// Its an inplace definition
						// typedef <which> <type_identifier> { ... } <identifier>;
						ok_to_parse = true;
					}
					else if ( tok.Type == Tok_Identifier && tokens.ptr[ idx - 3 ].Type == which )
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
						log_failure( "Unsupported or bad member definition after struct declaration\n%SB", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
						parser_pop(& ctx->parser);
						return InvalidCode;
					}

					// TODO(Ed) : I'm not sure if I have to use parser_parse_type here, I'd rather not as that would complicate parser_parse_type.
					// type = parser_parse_type();
					type = parse_forward_or_definition(ctx, which, from_typedef );
					// <ModuleFalgs> typedef <UnderlyingType>
				}
				else if ( tok.Type == Tok_BraceCurly_Close )
				{
					// Its a definition
					// <which> { ... };
					type = parse_forward_or_definition(ctx, currtok.Type, from_typedef );
					// <ModuleFalgs> typedef <UnderlyingType>
				}
				else if ( tok.Type == Tok_BraceSquare_Close)
				{
					// Its an array definition
					// <which> <type_identifier> <identifier> [ ... ];
					type = cast(Code, parser_parse_type(ctx, parser_not_from_template, nullptr));
					// <ModuleFalgs> typedef <UnderlyingType>
				}
				else
				{
					log_failure( "Unsupported or bad member definition after struct declaration\n%SB", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
					parser_pop(& ctx->parser);
					return InvalidCode;
				}
			}
		}
		else
		{
			bool from_template = false;
			type = cast(Code, parser_parse_type(ctx, from_template, &is_function ));
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
			log_failure( "Error, expected identifier for typedef\n%SB", parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
			parser_pop(& ctx->parser);
			return InvalidCode;
		}

		array_expr = parse_array_decl(ctx);
		// <UnderlyingType> + <ArrayExpr>
	}

	Token stmt_end = currtok;
	eat( Tok_Statement_End );
	// <ModuleFalgs> typedef <UnderlyingType> <Name>;

	CodeComment inline_cmt = NullCode;
	if ( currtok_noskip.Type == Tok_Comment && currtok_noskip.Line == stmt_end.Line )
		inline_cmt = parse_comment(ctx);
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
		result->Name       = cache_str( name.Text );
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

	parser_pop(& ctx->parser);
	return result;
}

internal neverinline
CodeUnion parser_parse_union(Context* ctx, bool inplace_def)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	ModuleFlag mflags = ModuleFlag_None;

	if ( check(Tok_Module_Export) )
	{
		mflags = ModuleFlag_Export;
		eat( Tok_Module_Export );
	}
	// <ModuleFlags>

	eat( Tok_Decl_Union );
	// <ModuleFlags> union

	CodeAttributes attributes = parse_attributes(ctx);
	// <ModuleFlags> union <Attributes>

	Str name = { nullptr, 0 };
	if ( check( Tok_Identifier ) )
	{
		name = currtok.Text;
		ctx->parser.Scope->Name = currtok.Text;
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
					member = cast(Code, parse_comment(ctx));
				break;

				// TODO(Ed) : Unions can have constructors and destructors

				case Tok_Decl_Class:
					member = parse_complicated_definition(ctx, Tok_Decl_Class );
				break;

				case Tok_Decl_Enum:
					member = parse_complicated_definition(ctx, Tok_Decl_Enum );
				break;

				case Tok_Decl_Struct:
					member = parse_complicated_definition(ctx, Tok_Decl_Struct );
				break;

				case Tok_Decl_Union:
					member = parse_complicated_definition(ctx, Tok_Decl_Union );
				break;

				case Tok_Preprocess_Define:
					member = cast(Code, parser_parse_define(ctx));
				break;

				case Tok_Preprocess_If:
				case Tok_Preprocess_IfDef:
				case Tok_Preprocess_IfNotDef:
				case Tok_Preprocess_ElIf:
					member = cast(Code, parse_preprocess_cond(ctx));
				break;

				case Tok_Preprocess_Else:
					member = cast(Code, preprocess_else);
					eat( Tok_Preprocess_Else );
				break;

				case Tok_Preprocess_EndIf:
					member = cast(Code, preprocess_endif);
					eat( Tok_Preprocess_EndIf );
				break;

				case Tok_Preprocess_Macro_Typename:
					// Its a variable with a macro typename
					member = cast(Code, parser_parse_variable(ctx));
				break;

				case Tok_Preprocess_Macro_Stmt:
					member = parse_simple_preprocess(ctx, Tok_Preprocess_Macro_Stmt );
				break;

				case Tok_Preprocess_Pragma:
					member = cast(Code, parse_pragma(ctx));
				break;

				case Tok_Preprocess_Unsupported:
					member = parse_simple_preprocess(ctx, Tok_Preprocess_Unsupported );
				break;

				default:
					member = cast(Code, parser_parse_variable(ctx));
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
		result->Name = cache_str( name );

	result->Body       = body;
	result->Attributes = attributes;

	parser_pop(& ctx->parser);
	return result;
}

internal
CodeUsing parser_parse_using(Context* ctx)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	Specifier specs_found[16] = { Spec_Invalid };
	s32        NumSpecifiers = 0;

	Token        name       = NullToken;
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
	ctx->parser.Scope->Name = name.Text;
	eat( Tok_Identifier );
	// <ModuleFlags> using <namespace> <Name>

	if ( ! is_namespace )
	{
		attributes = parse_attributes(ctx);
		// <ModuleFlags> using <Name> <Attributes>
		
		if ( bitfield_is_set( u32, currtok.Flags, TF_Assign ))
		{
			attributes = parse_attributes(ctx);
			// <ModuleFlags> using <Name> <Attributes>

			eat( Tok_Operator );
			// <ModuleFlags> using <Name> <Attributes> =

			type = parser_parse_type(ctx, parser_not_from_template, nullptr);
			// <ModuleFlags> using <Name> <Attributes> = <UnderlyingType>

			array_expr = parse_array_decl(ctx);
			// <UnderlyingType> + <ArrExpr>
		}
	}

	Token stmt_end = currtok;
	eat( Tok_Statement_End );
	// <ModuleFlags> using <namespace> <Attributes> <Name> = <UnderlyingType>;

	CodeComment inline_cmt = NullCode;
	if ( currtok_noskip.Type == Tok_Comment && currtok_noskip.Line == stmt_end.Line ) {
		inline_cmt = parse_comment(ctx);
	}
	// <ModuleFlags> using <namespace> <Attributes> <Name> = <UnderlyingType>; <InlineCmt>

	CodeUsing
	result              = (CodeUsing) make_code();
	result->Name        = cache_str( name.Text );
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

	parser_pop(& ctx->parser);
	return result;
}

internal
CodeVar parser_parse_variable(Context* ctx)
{
	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope );

	Specifier specs_found[16] = { Spec_NumSpecifiers };
	s32       NumSpecifiers = 0;

	ModuleFlag	   mflags     = ModuleFlag_None;
	CodeAttributes attributes = { nullptr };
	CodeSpecifiers specifiers = { nullptr };

	if ( check(Tok_Module_Export) ) {
		mflags = ModuleFlag_Export;
		eat( Tok_Module_Export );
	}
	// <ModuleFlags>

	attributes = parse_attributes(ctx);
	// <ModuleFlags> <Attributes>

	while ( left && tok_is_specifier(currtok) )
	{
		Specifier spec = str_to_specifier( currtok.Text );
		switch  ( spec )
		{
			GEN_PARSER_VARIABLE_ALLOWED_SPECIFIER_CASES:
			break;

			default:
				log_failure( "Invalid specifier %S for variable\n%S", spec_to_str( spec ), strbuilder_to_str( parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp)) );
				parser_pop(& ctx->parser);
				return InvalidCode;
		}

		// Ignore const specifiers, they're handled by the type
		if ( spec == Spec_Const )
			break;

		specs_found[NumSpecifiers] = spec;
		NumSpecifiers++;
		eat( currtok.Type );
	}

	if ( NumSpecifiers ) {
		specifiers = def_specifiers_arr( NumSpecifiers, specs_found );
	}
	// <ModuleFlags> <Attributes> <Specifiers>

	CodeTypename type = parser_parse_type(ctx, parser_not_from_template, nullptr);
	// <ModuleFlags> <Attributes> <Specifiers> <ValueType>

	if ( cast(Code, type) == Code_Invalid )
		return InvalidCode;

	ctx->parser.Scope->Name = parse_identifier(ctx, nullptr).Text;
	// <ModuleFlags> <Attributes> <Specifiers> <ValueType> <Name>

	CodeVar result = parse_variable_after_name(ctx, mflags, attributes, specifiers, type, ctx->parser.Scope->Name );
	// Regular  : <ModuleFlags> <Attributes> <Specifiers> <ValueType> <Name>                  = <Value>; <InlineCmt>
	// Bitfield : <ModuleFlags> <Attributes> <Specifiers> <ValueType> <Name> : <BitfieldSize> = <Value>; <InlineCmt>

	parser_pop(& ctx->parser);
	return result;
}

internal
CodeTypename parser_parse_type_alt( bool from_template, bool* typedef_is_functon )
{
	return InvalidCode;
}

#ifdef CHECK_WAS_DEFINED
#pragma pop_macro("check")
#endif
