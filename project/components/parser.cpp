#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "gen/etoktype.cpp"
#include "interface.upfront.cpp"
#include "lexer.cpp
#endif

namespace parser {

constexpr bool dont_skip_formatting = false;

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

	void push( StackNode* node )
	{
		node->Prev = Scope;
		Scope      = node;

	#if 0 && Build_Debug
		log_fmt("\tEntering Context: %.*s\n", Scope->ProcName.Len, Scope->ProcName.Ptr );
	#endif
	}

	void pop()
	{
	#if 0 && Build_Debug
		log_fmt("\tPopping  Context: %.*s\n", Scope->ProcName.Len, Scope->ProcName.Ptr );
	#endif
		Scope = Scope->Prev;
	}

	String to_string()
	{
		String result = String::make_reserve( GlobalAllocator, kilobytes(4) );

		Token scope_start = Scope->Start;
		Token last_valid  = Tokens.Idx >= Tokens.Arr.num() ? Tokens.Arr[Tokens.Arr.num() -1] : Tokens.current();

		sptr        length  = scope_start.Length;
		char const* current = scope_start.Text + length;
		while ( current <= Tokens.Arr.back().Text && *current != '\n' && length < 74 )
		{
			current++;
			length++;
		}

		String line = String::make( GlobalAllocator, { length, scope_start.Text } );
		result.append_fmt("\tScope    : %s\n", line );
		line.free();

		sptr   dist            = (sptr)last_valid.Text - (sptr)scope_start.Text + 2;
		sptr   length_from_err = dist;
		String line_from_err   = String::make( GlobalAllocator, { length_from_err, last_valid.Text } );

		if ( length_from_err < 100 )
			result.append_fmt("\t(%d, %d):%*c\n", last_valid.Line, last_valid.Column, length_from_err, '^' );
		else
			result.append_fmt("\t(%d, %d)\n", last_valid.Line, last_valid.Column );

		StackNode* curr_scope = Scope;
		s32 level = 0;
		do
		{
			if ( curr_scope->Name )
			{
				result.append_fmt("\t%d: %s, AST Name: %.*s\n", level, curr_scope->ProcName.Ptr, curr_scope->Name.Length, curr_scope->Name.Text );
			}
			else
			{
				result.append_fmt("\t%d: %s\n", level, curr_scope->ProcName.Ptr );
			}

			curr_scope = curr_scope->Prev;
			level++;
		}
		while ( curr_scope );
		return result;
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

	if ( ( Arr[ Idx ].Type == TokType::NewLine && type != TokType::NewLine )
	||   ( Arr[ Idx ].Type == TokType::Comment && type != TokType::Comment ) )
	{
		Idx++;
	}

	if ( Arr[Idx].Type != type )
	{
		log_failure( "Parse Error, TokArray::eat, Expected: ' %s ' not ' %.*s ' (%d, %d)`\n%s"
			, ETokType::to_str(type).Ptr
			, Arr[Idx].Length, Arr[Idx].Text
			, current().Line
			, current().Column
			, Context.to_string()
		);

		return false;
	}

#if 0 && Build_Debug
	log_fmt("Ate: %S\n", Arr[Idx].to_string() );
#endif

	Idx++;
	return true;
}

internal
void init()
{
	Tokens = Array<Token>::init_reserve( LexArena
		, ( LexAllocator_Size - sizeof( Array<Token>::Header ) ) / sizeof(Token)
	);
}

internal
void deinit()
{
	parser::Tokens = { nullptr };
}

#pragma region Helper Macros

#	define check_parse_args( def )                                             \
if ( def.Len <= 0 )                                                            \
{                                                                              \
	log_failure( "gen::" stringize(__func__) ": length must greater than 0" ); \
	parser::Context.pop();                                                     \
	return CodeInvalid;                                                        \
}                                                                              \
if ( def.Ptr == nullptr )                                                      \
{                                                                              \
	log_failure( "gen::" stringize(__func__) ": def was null" );               \
	parser::Context.pop();                                                     \
	return CodeInvalid;                                                        \
}

#	define currtok_noskip Context.Tokens.current( dont_skip_formatting )
#	define currtok        Context.Tokens.current()
#	define prevtok        Context.Tokens.previous()
#	define nexttok		  Context.Tokens.next()
#	define eat( Type_ )   Context.Tokens.__eat( Type_ )
#	define left           ( Context.Tokens.Arr.num() - Context.Tokens.Idx )

#	define check_noskip( Type_ ) ( left && currtok_noskip.Type == Type_ )
#	define check( Type_ )        ( left && currtok.Type == Type_ )

#	define push_scope()                                               \
	StackNode scope { nullptr, currtok_noskip, NullToken, txt( __func__ ) }; \
	Context.push( & scope )

#pragma endregion Helper Macros

// Procedure Forwards ( Entire parser internal parser interface )

internal Code               parse_array_decl                   ();
internal CodeAttributes     parse_attributes                   ();
internal CodeComment        parse_comment                      ();
internal Code               parse_compilcated_definition       ();
internal CodeBody           parse_class_struct_body            ( TokType which, Token name = NullToken );
internal Code               parse_class_struct                 ( TokType which, bool inplace_def );
internal CodeDefine         parse_define                       ();
internal Code               parse_forward_or_definition         ( TokType which, bool is_inplace );
internal CodeFn             parse_function_after_name          ( ModuleFlag mflags, CodeAttributes attributes, CodeSpecifiers specifiers, CodeType ret_type, Token name );
internal Code               parse_function_body                ();
internal Code               parse_global_nspace                ();
internal Token              parse_identifier                   ( bool* possible_member_function = nullptr );
internal CodeInclude        parse_include                      ();
internal CodeOperator       parse_operator_after_ret_type      ( ModuleFlag mflags, CodeAttributes attributes, CodeSpecifiers specifiers, CodeType ret_type );
internal Code               parse_operator_function_or_variable( bool expects_function, CodeAttributes attributes, CodeSpecifiers specifiers );
internal CodePragma         parse_pragma                       ();
internal CodeParam          parse_params                       ( bool use_template_capture = false );
internal CodePreprocessCond parse_preprocess_cond              ();
internal Code               parse_simple_preprocess            ( TokType which );
internal Code               parse_static_assert                ();
internal void               parse_template_args                ( Token& token );
internal CodeVar            parse_variable_after_name          ( ModuleFlag mflags, CodeAttributes attributes, CodeSpecifiers specifiers, CodeType type, StrC name );
internal CodeVar            parse_variable_declaration_list    ();

internal CodeClass       parse_class           ( bool inplace_def = false );
internal CodeConstructor parse_constructor     ();
internal CodeDestructor  parse_destructor      ( CodeSpecifiers specifiers = NoCode );
internal CodeEnum        parse_enum            ( bool inplace_def = false );
internal CodeBody        parse_export_body     ();
internal CodeBody        parse_extern_link_body();
internal CodeExtern      parse_extern_link     ();
internal CodeFriend      parse_friend          ();
internal CodeFn          parse_function        ();
internal CodeNS          parse_namespace       ();
internal CodeOpCast      parse_operator_cast   ( CodeSpecifiers specifiers = NoCode );
internal CodeStruct      parse_struct          ( bool inplace_def = false );
internal CodeVar         parse_variable        ();
internal CodeTemplate    parse_template        ();
internal CodeType        parse_type            ( bool* is_function = nullptr );
internal CodeTypedef     parse_typedef         ();
internal CodeUnion       parse_union           ( bool inplace_def = false );
internal CodeUsing       parse_using           ();

constexpr bool inplace_def = true;

// Internal parsing functions

constexpr bool strip_formatting_dont_preserve_newlines = false;
/*
	This function was an attempt at stripping formatting from any c++ code.
	It has edge case failures that prevent it from being used in function bodies.
*/
internal
String strip_formatting( StrC raw_text, bool preserve_newlines = true )
{
	String content = String::make_reserve( GlobalAllocator, raw_text.Len );

	if ( raw_text.Len == 0 )
		return content;

#define cut_length ( scanner - raw_text.Ptr - last_cut )
#define cut_ptr    ( raw_text.Ptr   + last_cut )
#define pos        ( sptr( scanner ) - sptr( raw_text.Ptr ) )
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

			content.append( cut_ptr, cut_length );
			last_cut = sptr( scanner ) - sptr( raw_text.Ptr );
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

			content.append( cut_ptr, cut_length );
			last_cut = sptr( scanner ) - sptr( raw_text.Ptr );
			continue;
		}

		// Block comments
		if ( tokleft > 1 && scanner[0] == '/' && scanner[1] == '*' )
		{
			while ( tokleft > 1 && !(scanner[0] == '*' && scanner[1] == '/') )
				move_fwd();

			scanner += 2;
			tokleft -= 2;

			content.append( cut_ptr, cut_length );
			last_cut = sptr( scanner ) - sptr( raw_text.Ptr );
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

			content.append( cut_ptr, cut_length );
			last_cut = sptr( scanner ) - sptr( raw_text.Ptr );
			continue;
		}

		// Tabs
		if (scanner[0] == '\t')
		{
			if (pos > last_cut)
				content.append(cut_ptr, cut_length);

			if ( content.back() != ' ' )
				content.append(' ');

			move_fwd();
			last_cut = sptr(scanner) - sptr(raw_text.Ptr);
			continue;
		}

		if ( tokleft > 1 && scanner[0] == '\r' && scanner[1] == '\n' )
		{
			if ( must_keep_newline || preserve_newlines )
			{
				must_keep_newline = false;

				scanner += 2;
				tokleft -= 2;

				content.append( cut_ptr, cut_length );
				last_cut = sptr( scanner ) - sptr( raw_text.Ptr );
				continue;
			}

			if ( pos > last_cut )
				content.append( cut_ptr, cut_length );

			// Replace with a space
			if ( content.back() != ' ' )
				content.append( ' ' );

			scanner += 2;
			tokleft -= 2;

			last_cut = sptr( scanner ) - sptr( raw_text.Ptr );
			continue;
		}

		if ( scanner[0] == '\n' )
		{
			if ( must_keep_newline || preserve_newlines )
			{
				must_keep_newline = false;

				move_fwd();

				content.append( cut_ptr, cut_length );
				last_cut = sptr( scanner ) - sptr( raw_text.Ptr );
				continue;
			}

			if ( pos > last_cut )
				content.append( cut_ptr, cut_length );

			// Replace with a space
			if ( content.back() != ' ' )
				content.append( ' ' );

			move_fwd();

			last_cut = sptr( scanner ) - sptr( raw_text.Ptr );
			continue;
		}

		// Escaped newlines
		if ( scanner[0] == '\\' )
		{
			content.append( cut_ptr, cut_length );

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

			last_cut = sptr( scanner ) - sptr( raw_text.Ptr );
			continue;
		}

		// Consectuive spaces
		if ( tokleft > 1 && char_is_space( scanner[0] ) && char_is_space( scanner[ 1 ] ) )
		{
			content.append( cut_ptr, cut_length );
			do
			{
				move_fwd();
			}
			while ( tokleft && char_is_space( scanner[0] ) );

			last_cut = sptr( scanner ) - sptr( raw_text.Ptr );

			// Preserve only 1 space of formattting
			if ( content.back() != ' ' )
				content.append( ' ' );

			continue;
		}

		move_fwd();
	}

	if ( last_cut < raw_text.Len )
	{
		content.append( cut_ptr, raw_text.Len - last_cut );
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

	if ( check( TokType::Operator ) && currtok.Text[0] == '[' && currtok.Text[1] == ']' )
	{
		Code array_expr = untyped_str( currtok );
		eat( TokType::Operator );

		Context.pop();
		return array_expr;
	}

	if ( check( TokType::BraceSquare_Open ) )
	{
		eat( TokType::BraceSquare_Open );

		if ( left == 0 )
		{
			log_failure( "Error, unexpected end of array declaration ( '[]' scope started )\n%s", Context.to_string() );
			Context.pop();
			return CodeInvalid;
		}

		if ( currtok.Type == TokType::BraceSquare_Close )
		{
			log_failure( "Error, empty array expression in definition\n%s", Context.to_string() );
			Context.pop();
			return CodeInvalid;
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
			Context.pop();
			return CodeInvalid;
		}

		if ( currtok.Type != TokType::BraceSquare_Close )
		{
			log_failure( "%s: Error, expected ] in array declaration, not %s\n%s", ETokType::to_str( currtok.Type ), Context.to_string() );
			Context.pop();
			return CodeInvalid;
		}

		eat( TokType::BraceSquare_Close );

		// Its a multi-dimensional array
		if ( check( TokType::BraceSquare_Open ))
		{
			Code adjacent_arr_expr = parse_array_decl();

			array_expr->Next = adjacent_arr_expr.ast;
		}

		Context.pop();
		return array_expr;
	}

	Context.pop();
	return { nullptr };
}

internal inline
CodeAttributes parse_attributes()
{
	push_scope();

	Token start = NullToken;
	s32 len = 0;

	if ( check(TokType::Attribute_Open) )
	{
		eat( TokType::Attribute_Open);

		start = currtok;
		while ( left && currtok.Type != TokType::Attribute_Close )
		{
			eat( currtok.Type );
		}

		eat( TokType::Attribute_Close );

		s32 len = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)start.Text;
	}

	else if ( check(TokType::Decl_GNU_Attribute) )
	{
		eat(TokType::Capture_Start);
		eat(TokType::Capture_Start);

		start = currtok;
		while ( left && currtok.Type != TokType::Capture_End )
		{
			eat(currtok.Type);
		}

		eat(TokType::Capture_End);
		eat(TokType::Capture_End);

		s32 len = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)start.Text;
	}

	else if ( check(TokType::Decl_MSVC_Attribute) )
	{
		eat( TokType::Decl_MSVC_Attribute );
		eat( TokType::Capture_Start);

		start = currtok;
		while ( left && currtok.Type != TokType::Capture_End )
		{
			eat(currtok.Type);
		}

		eat(TokType::Capture_End);

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
		Context.pop();

		String name_stripped = strip_formatting( attribute_txt, strip_formatting_dont_preserve_newlines );

		Code
		result          = make_code();
		result->Type    = ECode::PlatformAttributes;
		result->Name    = get_cached_string( name_stripped );
		result->Content = result->Name;
		// result->Token   = 

		return (CodeAttributes) result;
	}

	Context.pop();
	return { nullptr };
}

internal
CodeComment parse_comment()
{
	StackNode scope { nullptr, currtok_noskip, NullToken, txt( __func__ ) };
	Context.push( & scope );

	CodeComment
	result          = (CodeComment) make_code();
	result->Type    = ECode::Comment;
	result->Content = get_cached_string( currtok_noskip );
	result->Name    = result->Content;
	// result->Token   = currtok_noskip;
	eat( TokType::Comment );

	Context.pop();
	return result;
}

internal
Code parse_complicated_definition( TokType which )
{
	push_scope();

	bool is_inplace = false;

	TokArray tokens = Context.Tokens;

	s32 idx = tokens.Idx;
	s32 level = 0;
	for ( ; idx < tokens.Arr.num(); idx ++ )
	{
		if ( tokens[idx].Type == TokType::BraceCurly_Open )
			level++;

		if ( tokens[idx].Type == TokType::BraceCurly_Close )
			level--;

		if ( level == 0 && tokens[idx].Type == TokType::Statement_End )
			break;
	}

	if ( (idx - 2 ) == tokens.Idx )
	{
		// Its a forward declaration only
		Code result = parse_forward_or_definition( which, is_inplace );
		Context.pop();
		return result;
	}

	Token tok = tokens[ idx - 1 ];
	if ( tok.Type == TokType::Identifier )
	{
		tok = tokens[ idx - 2 ];

		bool is_indirection = tok.Type == TokType::Ampersand
		||                    tok.Type == TokType::Star;

		bool ok_to_parse = false;

		if ( tok.Type == TokType::BraceCurly_Close )
		{
			// Its an inplace definition
			// <which> <type_identifier> { ... } <identifier>;
			ok_to_parse = true;
			is_inplace  = true;
		}
		else if ( tok.Type == TokType::Identifier && tokens[ idx - 3 ].Type == TokType::Decl_Struct )
		{
			// Its a variable with type ID using struct namespace.
			// <which> <type_identifier> <identifier>;
			ok_to_parse = true;
		}
		else if ( is_indirection )
		{
			// Its a indirection type with type ID using struct namespace.
			// <which> <type_identifier>* <identifier>;
			ok_to_parse = true;
		}

		if ( ! ok_to_parse )
		{
			log_failure( "Unsupported or bad member definition after struct declaration\n%s", Context.to_string() );
			Context.pop();
			return CodeInvalid;
		}

		Code result = parse_operator_function_or_variable( false, { nullptr }, { nullptr } );
		Context.pop();
		return result;
	}
	else if ( tok.Type == TokType::BraceCurly_Close )
	{
		// Its a definition
		// <which> { ... };
		Code result = parse_forward_or_definition( which, is_inplace );
		Context.pop();
		return result;
	}
	else if ( tok.Type == TokType::BraceSquare_Close)
	{
		// Its an array definition
		// <which> <type_identifier> <identifier> [ ... ];
		Code result = parse_operator_function_or_variable( false, { nullptr }, { nullptr } );
		Context.pop();
		return result;
	}
	else
	{
		log_failure( "Unsupported or bad member definition after struct declaration\n%s", Context.to_string() );
		Context.pop();
		return CodeInvalid;
	}
}

internal neverinline
CodeBody parse_class_struct_body( TokType which, Token name )
{
	using namespace ECode;
	push_scope();

	eat( TokType::BraceCurly_Open );

	CodeBody
	result = (CodeBody) make_code();

	if ( which == TokType::Decl_Class )
		result->Type = Class_Body;

	else
		result->Type = Struct_Body;

	while ( left && currtok_noskip.Type != TokType::BraceCurly_Close )
	{
		Code           member     = Code::Invalid;
		CodeAttributes attributes = { nullptr };
		CodeSpecifiers specifiers = { nullptr };

		bool expects_function = false;

		Context.Scope->Start = currtok_noskip;

		if ( currtok_noskip.Type == TokType::Preprocess_Hash )
			eat( TokType::Preprocess_Hash );

		switch ( currtok_noskip.Type )
		{
			case TokType::NewLine:
				member = fmt_newline;
				eat( TokType::NewLine );
			break;

			case TokType::Comment:
				member = parse_comment();
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
				member = parse_complicated_definition( TokType::Decl_Class );
			break;

			case TokType::Decl_Enum:
				member = parse_complicated_definition( TokType::Decl_Enum );
			break;

			case TokType::Decl_Friend:
				member = parse_friend();
			break;

			case TokType::Decl_Operator:
				member = parse_operator_cast();
			break;

			case TokType::Decl_Struct:
				member = parse_complicated_definition( TokType::Decl_Struct );
			break;

			case TokType::Decl_Template:
				member = parse_template();
			break;

			case TokType::Decl_Typedef:
				member = parse_typedef();
			break;

			case TokType::Decl_Union:
				member = parse_complicated_definition( TokType::Decl_Union );
			break;

			case TokType::Decl_Using:
				member = parse_using();
			break;

			case TokType::Operator:
				if ( currtok.Text[0] != '~' )
				{
					log_failure( "Operator token found in global body but not destructor unary negation\n%s", Context.to_string() );
					return CodeInvalid;
				}

				member = parse_destructor();
			break;

			case TokType::Preprocess_Define:
				member = parse_define();
			break;

			case TokType::Preprocess_Include:
				member = parse_include();
			break;

			case TokType::Preprocess_If:
			case TokType::Preprocess_IfDef:
			case TokType::Preprocess_IfNotDef:
			case TokType::Preprocess_ElIf:
				member = parse_preprocess_cond();
			break;

			case TokType::Preprocess_Macro:
				member = parse_simple_preprocess( TokType::Preprocess_Macro );
			break;

			case TokType::Preprocess_Pragma:
				member = parse_pragma();
			break;

			case TokType::Preprocess_Else:
				member = preprocess_else;
				eat( TokType::Preprocess_Else );
			break;

			case TokType::Preprocess_EndIf:
				member = preprocess_endif;
				eat( TokType::Preprocess_EndIf );
			break;

			case TokType::Preprocess_Unsupported:
				member = parse_simple_preprocess( TokType::Preprocess_Unsupported );
			break;

			case TokType::StaticAssert:
				member = parse_static_assert();
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
			case TokType::Spec_ForceInline:
			case TokType::Spec_Inline:
			case TokType::Spec_Mutable:
			case TokType::Spec_NeverInline:
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
						case ESpecifier::ForceInline:
						case ESpecifier::Mutable:
						case ESpecifier::NeverInline:
						case ESpecifier::Static:
						case ESpecifier::Volatile:
						break;

						case ESpecifier::Consteval:
							expects_function = true;
						break;

						default:
							log_failure( "Invalid specifier %s for variable\n%s", ESpecifier::to_str(spec), Context.to_string() );
							Context.pop();
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

				if ( currtok.Type == TokType::Operator && currtok.Text[0] == '~' )
				{
					member = parse_destructor( specifiers );
					break;
				}

				if ( currtok.Type == TokType::Decl_Operator )
				{
					member = parse_operator_cast( specifiers );
					break;
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
				if ( nexttok.Type == TokType::Capture_Start && name.Length && currtok.Type == TokType::Identifier )
				{
					if ( str_compare( name.Text, currtok.Text, name.Length ) == 0 )
					{
						member = parse_constructor();
						break;
					}
				}

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
			Context.pop();
			return CodeInvalid;
		}

		result.append( member );
	}

	eat( TokType::BraceCurly_Close );
	Context.pop();
	return result;
}

internal
Code parse_class_struct( TokType which, bool inplace_def = false )
{
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
	{
		name = parse_identifier();
		Context.Scope->Name = name;
	}

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

		Token parent_tok = parse_identifier();
		parent = def_type( parent_tok );

		while ( check(TokType::Comma) )
		{
			eat(TokType::Access_Public);

			if ( currtok.is_access_specifier() )
			{
				eat(currtok.Type);
			}

			Token interface_tok = parse_identifier();

			interfaces.append( def_type( interface_tok ) );
		}
	}

	if ( check( TokType::BraceCurly_Open ) )
	{
		body = parse_class_struct_body( which, name );
	}

	CodeComment inline_cmt = NoCode;
	if ( ! inplace_def )
	{
		Token stmt_end = currtok;
		eat( TokType::Statement_End );

		if ( currtok_noskip.Type == TokType::Comment && currtok_noskip.Line == stmt_end.Line )
			inline_cmt = parse_comment();
	}

	if ( which == TokType::Decl_Class )
		result = def_class( name, body, parent, access, attributes, mflags );

	else
		result = def_struct( name, body, (CodeType)parent, access, attributes, mflags );

	if ( inline_cmt )
		result->InlineCmt = inline_cmt;

	interfaces.free();
	return result;
}

internal inline
CodeDefine parse_define()
{
	push_scope();
	eat( TokType::Preprocess_Define );

	CodeDefine
	define = (CodeDefine) make_code();
	define->Type = ECode::Preprocess_Define;

	if ( ! check( TokType::Identifier ) )
	{
		log_failure( "Error, expected identifier after #define\n%s", Context.to_string() );
		Context.pop();
		return CodeInvalid;
	}

	Context.Scope->Name = currtok;
	define->Name = get_cached_string( currtok );
	eat( TokType::Identifier );

	if ( ! check( TokType::Preprocess_Content ))
	{
		log_failure( "Error, expected content after #define %s\n%s", define->Name, Context.to_string() );
		Context.pop();
		return CodeInvalid;
	}

	if ( currtok.Length == 0 )
	{
		define->Content = get_cached_string( currtok );
		eat( TokType::Preprocess_Content );

		Context.pop();
		return define;
	}

	define->Content = get_cached_string( strip_formatting( currtok, strip_formatting_dont_preserve_newlines ) );
	eat( TokType::Preprocess_Content );

	Context.pop();
	return define;
}

internal inline
Code parse_forward_or_definition( TokType which, bool is_inplace )
{
	Code result = CodeInvalid;

	switch ( which )
	{
		case TokType::Decl_Class:
			result = parse_class( is_inplace );
			return result;

		case TokType::Decl_Enum:
			result = parse_enum( is_inplace );
			return result;

		case TokType::Decl_Struct:
			result = parse_struct( is_inplace );
			return result;

		case TokType::Decl_Union:
			result = parse_union( is_inplace );
			return result;

		default:
			log_failure( "Error, wrong token type given to parse_complicated_definition "
				"(only supports class, enum, struct, union) \n%s"
				, Context.to_string() );

			return CodeInvalid;
	}

	return CodeInvalid;
}

// Function parsing is handled in multiple places because its initial signature is shared with variable parsing
internal inline
CodeFn parse_function_after_name(
	  ModuleFlag     mflags
	, CodeAttributes attributes
	, CodeSpecifiers specifiers
	, CodeType       ret_type
	, Token          name
)
{
	push_scope();
	CodeParam params = parse_params();

	// These have to be kept separate from the return type's specifiers.
	while ( left && currtok.is_specifier() )
	{
		if ( specifiers.ast == nullptr )
		{
			specifiers = def_specifier( ESpecifier::to_type(currtok) );
			eat( currtok.Type );
			continue;
		}

		specifiers.append( ESpecifier::to_type(currtok) );
		eat( currtok.Type );
	}

	CodeBody    body       = NoCode;
	CodeComment inline_cmt = NoCode;
	if ( check( TokType::BraceCurly_Open ) )
	{
		body = parse_function_body();
		if ( body == Code::Invalid )
		{
			Context.pop();
			return CodeInvalid;
		}
	}
	else
	{
		Token stmt_end = currtok;
		eat( TokType::Statement_End );

		if ( currtok_noskip.Type == TokType::Comment && currtok_noskip.Line == stmt_end.Line )
			inline_cmt = parse_comment();
	}

	using namespace ECode;

	String
	name_stripped = String::make( GlobalAllocator, name );
	name_stripped.strip_space();

	CodeFn
	result              = (CodeFn) make_code();
	result->Name        = get_cached_string( name_stripped );
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
				Context.pop();
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

	if ( inline_cmt )
		result->InlineCmt = inline_cmt;

	Context.pop();
	return result;
}

internal
Code parse_function_body()
{
	using namespace ECode;
	push_scope();

	eat( TokType::BraceCurly_Open );

	CodeBody
	result = (CodeBody) make_code();
	result->Type = Function_Body;

	// TODO : Support actual parsing of function body
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

internal neverinline
CodeBody parse_global_nspace( CodeT which )
{
	using namespace ECode;

	if ( which != Namespace_Body && which != Global_Body && which != Export_Body && which != Extern_Linkage_Body )
		return CodeInvalid;

	if ( which != Global_Body )
		eat( TokType::BraceCurly_Open );

	CodeBody
	result = (CodeBody) make_code();
	result->Type = which;

	while ( left && currtok_noskip.Type != TokType::BraceCurly_Close )
	{
		Code           member     = Code::Invalid;
		CodeAttributes attributes = { nullptr };
		CodeSpecifiers specifiers = { nullptr };

		bool expects_function = false;

		Context.Scope->Start = currtok_noskip;

		if ( currtok_noskip.Type == TokType::Preprocess_Hash )
			eat( TokType::Preprocess_Hash );

		switch ( currtok_noskip.Type )
		{
			case TokType::NewLine:
				// Empty lines are auto skipped by Tokens.current()
				member = fmt_newline;
				eat( TokType::NewLine );
			break;

			case TokType::Comment:
				member = parse_comment();
			break;

			case TokType::Decl_Class:
				member = parse_complicated_definition( TokType::Decl_Class );
			break;

			case TokType::Decl_Enum:
				member = parse_complicated_definition( TokType::Decl_Enum );
			break;

			case TokType::Decl_Extern_Linkage:
				if ( which == Extern_Linkage_Body )
					log_failure( "Nested extern linkage\n%s", Context.to_string() );

				member = parse_extern_link();
			break;

			case TokType::Decl_Namespace:
				member = parse_namespace();
			break;

			case TokType::Decl_Struct:
				member = parse_complicated_definition( TokType::Decl_Struct );
			break;

			case TokType::Decl_Template:
				member = parse_template();
			break;

			case TokType::Decl_Typedef:
				member = parse_typedef();
			break;

			case TokType::Decl_Union:
				member = parse_complicated_definition( TokType::Decl_Union );
			break;

			case TokType::Decl_Using:
				member = parse_using();
			break;

			case TokType::Preprocess_Define:
				member = parse_define();
			break;

			case TokType::Preprocess_Include:
				member = parse_include();
			break;

			case TokType::Preprocess_If:
			case TokType::Preprocess_IfDef:
			case TokType::Preprocess_IfNotDef:
			case TokType::Preprocess_ElIf:
				member = parse_preprocess_cond();
			break;

			case TokType::Preprocess_Macro:
				member = parse_simple_preprocess( TokType::Preprocess_Macro );
			break;

			case TokType::Preprocess_Pragma:
				member = parse_pragma();
			break;

			case TokType::Preprocess_Else:
				member = preprocess_else;
				eat( TokType::Preprocess_Else );
			break;

			case TokType::Preprocess_EndIf:
				member = preprocess_endif;
				eat( TokType::Preprocess_EndIf );
			break;

			case TokType::Preprocess_Unsupported:
				member = parse_simple_preprocess( TokType::Preprocess_Unsupported );
			break;

			case TokType::StaticAssert:
				member = parse_static_assert();
			break;

			case TokType::Module_Export:
				if ( which == Export_Body )
					log_failure( "Nested export declaration\n%s", Context.to_string() );

				member = parse_export_body();
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
			case TokType::Spec_ForceInline:
			case TokType::Spec_Global:
			case TokType::Spec_Inline:
			case TokType::Spec_Internal_Linkage:
			case TokType::Spec_NeverInline:
			case TokType::Spec_Static:
			{
				SpecifierT specs_found[16] { ESpecifier::NumSpecifiers };
				s32        NumSpecifiers = 0;

				while ( left && currtok.is_specifier() )
				{
					SpecifierT spec = ESpecifier::to_type( currtok );

					bool ignore_spec = false;

					switch ( spec )
					{
						case ESpecifier::Constexpr:
						case ESpecifier::Constinit:
						case ESpecifier::ForceInline:
						case ESpecifier::Global:
						case ESpecifier::External_Linkage:
						case ESpecifier::Internal_Linkage:
						case ESpecifier::Inline:
						case ESpecifier::Mutable:
						case ESpecifier::NeverInline:
						case ESpecifier::Static:
						case ESpecifier::Volatile:
						break;

						case ESpecifier::Consteval:
							expects_function = true;
						break;

						case ESpecifier::Const:
							ignore_spec = true;
						break;

						default:
							StrC spec_str = ESpecifier::to_str(spec);

							log_failure( "Invalid specifier %.*s for variable\n%s", spec_str.Len, spec_str, Context.to_string() );
							return CodeInvalid;
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
				bool found_operator_cast = false;
				s32  idx                 = Context.Tokens.Idx;

				for ( ; idx < Context.Tokens.Arr.num(); idx++ )
				{
					Token tok = Context.Tokens[ idx ];

					if ( tok.Type == TokType::Identifier )
					{
						idx++;
						tok = Context.Tokens[ idx ];
						if ( tok.Type == TokType::Access_StaticSymbol )
							continue;

						break;
					}

					if ( tok.Type == TokType::Decl_Operator )
						found_operator_cast = true;

					break;
				}

				if ( found_operator_cast )
				{
					member = parse_operator_cast();
					break;
				}

				member = parse_operator_function_or_variable( expects_function, attributes, specifiers );
			}
		}

		if ( member == Code::Invalid )
		{
			log_failure( "Failed to parse member\n%s", Context.to_string() );
			return CodeInvalid;
		}

		// log_fmt("Global Body Member: %s", member->debug_str());
		result.append( member );
	}

	if ( which != Global_Body )
		eat( TokType::BraceCurly_Close );

	return result;
}

internal
Token parse_identifier( bool* possible_member_function )
{
	push_scope();

	Token name = currtok;
	Context.Scope->Name = name;
	eat( TokType::Identifier );

	parse_template_args( name );

	while ( check( TokType::Access_StaticSymbol ) )
	{
		eat( TokType::Access_StaticSymbol );

		if ( left == 0 )
		{
			log_failure( "Error, unexpected end of static symbol identifier\n%s", Context.to_string() );
			Context.pop();
			return { nullptr, 0, TokType::Invalid };
		}

		if ( currtok.Type == TokType::Operator && currtok.Text[0] == '*' && currtok.Length == 1 )
		{
			if ( possible_member_function )
				*possible_member_function = true;

			else
			{
				log_failure( "Found a member function pointer identifier but the parsing context did not expect it\n%s", Context.to_string() );
				Context.pop();
				return { nullptr, 0, TokType::Invalid };
			}
		}

		if ( currtok.Type != TokType::Identifier )
		{
			log_failure( "Error, expected static symbol identifier, not %s\n%s", ETokType::to_str( currtok.Type ), Context.to_string() );
			Context.pop();
			return { nullptr, 0, TokType::Invalid };
		}

		name.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)name.Text;
		eat( TokType::Identifier );

		parse_template_args( name );
	}

	Context.pop();
	return name;
}

internal
CodeInclude parse_include()
{
	push_scope();

	CodeInclude
	include       = (CodeInclude) make_code();
	include->Type = ECode::Preprocess_Include;
	eat( TokType::Preprocess_Include );

	if ( ! check( TokType::String ))
	{
		log_failure( "Error, expected include string after #include\n%s", Context.to_string() );
		Context.pop();
		return CodeInvalid;
	}

	Context.Scope->Name = currtok;
	include->Content = get_cached_string( currtok );
	eat( TokType::String );

	Context.pop();
	return include;
}

internal
CodeOperator parse_operator_after_ret_type(
	  ModuleFlag     mflags
	, CodeAttributes attributes
	, CodeSpecifiers specifiers
	, CodeType       ret_type
)
{
	using namespace EOperator;
	push_scope();

	Token nspace = NullToken;
	if ( check( TokType::Identifier ) )
	{
		nspace = currtok;
		while ( left && currtok.Type == TokType::Identifier )
		{
			eat( TokType::Identifier );

			if ( currtok.Type == TokType::Access_StaticSymbol )
				eat( TokType::Access_StaticSymbol );
		}

		nspace.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)nspace.Text;
	}

	eat( TokType::Decl_Operator );

	if ( ! left && currtok.Type != TokType::Operator
		&& currtok.Type != TokType::Star
		&& currtok.Type != TokType::Ampersand
		&& currtok.Type != TokType::Ampersand_DBL )
	{
		log_failure( "Expected operator after 'operator' keyword\n%s", Context.to_string() );
		Context.pop();
		return CodeInvalid;
	}

	Context.Scope->Name = currtok;

	OperatorT op = Invalid;
	switch ( currtok.Text[0] )
	{
		case '+':
		{
			if ( currtok.Text[1] == '=' )
				op = Assign_Add;

			else if ( currtok.Text[1] == '+' )
				op = Increment;

			else
				op = Add;
		}
		break;
		case '-':
		{
			if ( currtok.Text[1] == '>' )
			{
				if ( currtok.Text[2] == '*' )
					op = MemberOfPointer;

				else
					op = MemberOfPointer;

				break;
			}

			else if ( currtok.Text[1] == '=' )
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
		Context.pop();
		return CodeInvalid;
	}

	eat( currtok.Type );

	// Parse Params
	CodeParam params = parse_params();

	if ( params.ast == nullptr && op == EOperator::Multiply )
		op = MemberOfPointer;

	while ( left && currtok.is_specifier() )
	{
		if ( specifiers.ast == nullptr )
		{
			specifiers = def_specifier( ESpecifier::to_type(currtok) );
			eat( currtok.Type );
			continue;
		}

		specifiers.append( ESpecifier::to_type(currtok) );
		eat( currtok.Type );
	}

	// Parse Body
	CodeBody    body       = { nullptr };
	CodeComment inline_cmt = NoCode;
	if ( check( TokType::BraceCurly_Open ) )
	{
		body = parse_function_body();
		if ( body == Code::Invalid )
		{
			Context.pop();
			return CodeInvalid;
		}
	}
	else
	{
		Token stmt_end = currtok;
		eat( TokType::Statement_End );

		if ( currtok_noskip.Type == TokType::Comment && currtok_noskip.Line == stmt_end.Line )
			inline_cmt = parse_comment();
	}

	// OpValidateResult check_result = operator__validate( op, params, ret_type, specifiers );
	CodeOperator result = def_operator( op, nspace, params, ret_type, body, specifiers, attributes, mflags );

	if ( inline_cmt )
		result->InlineCmt = inline_cmt;

	Context.pop();
	return result;
}

internal
Code parse_operator_function_or_variable( bool expects_function, CodeAttributes attributes, CodeSpecifiers specifiers )
{
	push_scope();

	Code result = CodeInvalid;

#ifndef GEN_PARSER_DISABLE_MACRO_FUNCTION_SIGNATURES
	if ( currtok.Type == TokType::Preprocess_Macro )
	{
		// Were dealing with a macro after attributes/specifiers.
		result = parse_simple_preprocess( TokType::Preprocess_Macro );
		Context.pop();
		return result;
	}
#endif

	CodeType type = parse_type();

	if ( type == CodeInvalid )
	{
		Context.pop();
		return CodeInvalid;
	}

	bool found_operator = false;
	s32  idx            = Context.Tokens.Idx;

	for ( ; idx < Context.Tokens.Arr.num(); idx++ )
	{
		Token tok = Context.Tokens[ idx ];

		if ( tok.Type == TokType::Identifier )
		{
			idx++;
			tok = Context.Tokens[ idx ];
			if ( tok.Type == TokType::Access_StaticSymbol )
				continue;

			break;
		}

		if ( tok.Type == TokType::Decl_Operator )
			found_operator = true;

		break;
	}

	if ( found_operator )
	{
		// Dealing with an operator overload
		result = parse_operator_after_ret_type( ModuleFlag::None, attributes, specifiers, type );
	}
	else
	{
		Token name = parse_identifier();
		Context.Scope->Name = name;

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
				Context.pop();
				return CodeInvalid;
			}

			// Dealing with a variable
			result = parse_variable_after_name( ModuleFlag::None, attributes, specifiers, type, name );
		}
	}

	Context.pop();
	return result;
}

internal
CodePragma parse_pragma()
{
	push_scope();

	CodePragma
	pragma       = (CodePragma) make_code();
	pragma->Type = ECode::Preprocess_Pragma;
	eat( TokType::Preprocess_Pragma );

	if ( ! check( TokType::Preprocess_Content ))
	{
		log_failure( "Error, expected content after #pragma\n%s", Context.to_string() );
		Context.pop();
		return CodeInvalid;
	}

	Context.Scope->Name = currtok;

	pragma->Content = get_cached_string( currtok );
	eat( TokType::Preprocess_Content );

	Context.pop();
	return pragma;
}

internal inline
CodeParam parse_params( bool use_template_capture )
{
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
		Context.pop();
		return { nullptr };
	}
	else if ( check ( TokType::Operator ) && currtok.Text[0] == '>' )
	{
		eat( TokType::Operator );
		Context.pop();
		return { nullptr };
	}

	CodeType type  = { nullptr };
	Code     value = { nullptr };

	if ( check( TokType::Varadic_Argument) )
	{
		eat( TokType::Varadic_Argument );

		Context.pop();
		return param_varadic;
	}

	type = parse_type();
	if ( type == Code::Invalid )
	{
		Context.pop();
		return CodeInvalid;
	}

	Token name = NullToken;

	if ( check( TokType::Identifier )  )
	{
		name = currtok;
		eat( TokType::Identifier );

		if ( bitfield_is_equal( u32, currtok.Flags, TF_Assign ) )
		{
			eat( TokType::Operator );

			Token value_tok = currtok;

			if ( currtok.Type == TokType::Comma )
			{
				log_failure( "Expected value after assignment operator\n%s.", Context.to_string() );
				Context.pop();
				return CodeInvalid;
			}

			while ( left
				&& currtok.Type != TokType::Comma
				&& currtok.Type != TokType::Capture_End
			)
			{
				value_tok.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)value_tok.Text;
				eat( currtok.Type );
			}

			value = untyped_str( strip_formatting( value_tok, strip_formatting_dont_preserve_newlines ) );
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

		type = parse_type();
		if ( type == Code::Invalid )
		{
			Context.pop();
			return CodeInvalid;
		}

		name = { nullptr, 0, TokType::Invalid, false };

		if ( check( TokType::Identifier )  )
		{
			name = currtok;
			eat( TokType::Identifier );

			if ( bitfield_is_equal( u32, currtok.Flags, TF_Assign ) )
			{
				eat( TokType::Operator );

				Token value_tok = currtok;

				if ( currtok.Type == TokType::Comma )
				{
					log_failure( "Expected value after assignment operator\n%s", Context.to_string() );
					Context.pop();
					return CodeInvalid;
				}

				while ( left
				&& currtok.Type != TokType::Comma && currtok.Type != TokType::Capture_End
				)
				{
					value_tok.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)value_tok.Text;
					eat( currtok.Type );
				}

				value = untyped_str( strip_formatting( value_tok, strip_formatting_dont_preserve_newlines ) );
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
			Context.pop();
			return CodeInvalid;
		}
		eat( TokType::Operator );
	}

	Context.pop();
	return result;
#	undef context
}

internal
CodePreprocessCond parse_preprocess_cond()
{
	push_scope();

	if ( ! currtok.is_preprocess_cond() )
	{
		log_failure( "Error, expected preprocess conditional\n%s", Context.to_string() );
		Context.pop();
		return CodeInvalid;
	}

	CodePreprocessCond
	cond       = (CodePreprocessCond) make_code();
	cond->Type = scast(CodeT, currtok.Type - (ETokType::Preprocess_If - ECode::Preprocess_If) );
	eat( currtok.Type );

	if ( ! check( TokType::Preprocess_Content ))
	{
		log_failure( "Error, expected content after #define\n%s", Context.to_string() );
		Context.pop();
		return CodeInvalid;
	}

	Context.Scope->Name = currtok;
	cond->Content = get_cached_string( currtok );
	eat( TokType::Preprocess_Content );

	Context.pop();
	return cond;
}

internal inline
Code parse_simple_preprocess( TokType which )
{
	push_scope();

	Token tok = currtok;
	eat( which );

	if ( currtok.Type == TokType::BraceCurly_Open )
	{
		// Eat the block scope right after the macro. Were assuming the macro defines a function definition's signature
		eat( TokType::BraceCurly_Open );

		s32 level = 0;
		while ( left && ( currtok.Type != TokType::BraceCurly_Close || level > 0 ) )
		{
			if ( currtok.Type == TokType::BraceCurly_Open )
				level++;

			else if ( currtok.Type == TokType::BraceCurly_Close && level > 0 )
				level--;

			eat( currtok.Type );
		}
		eat( TokType::BraceCurly_Close );

		StrC prev_proc = Context.Scope->Prev->ProcName;
		if ( str_compare( prev_proc.Ptr, "parse_typedef", prev_proc.Len ) != 0 )
		{
			if ( check( TokType::Statement_End ))
			{
				Token stmt_end = currtok;
				eat( TokType::Statement_End );

				if ( currtok_noskip.Type == TokType::Comment && currtok_noskip.Line == stmt_end.Line )
					eat( TokType::Comment );
			}
		}

		tok.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)tok.Text;
	}
	else
	{
		if ( str_compare( Context.Scope->Prev->ProcName.Ptr, "parse_typedef", Context.Scope->Prev->ProcName.Len ) != 0 )
		{
			if ( check( TokType::Statement_End ))
			{
				Token stmt_end = currtok;
				eat( TokType::Statement_End );

				if ( currtok_noskip.Type == TokType::Comment && currtok_noskip.Line == stmt_end.Line )
					eat( TokType::Comment );
			}
		}

		tok.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)tok.Text;
	}

	char const* content = str_fmt_buf( "%.*s ", tok.Length, tok.Text );

	Code result = untyped_str( to_str( content ) );
	Context.Scope->Name = tok;

	Context.pop();
	return result;
}

internal
Code parse_static_assert()
{
	push_scope();

	Code
	assert       = make_code();
	assert->Type = ECode::Untyped;

	Token content = currtok;

	Context.Scope->Name = content;

	eat( TokType::StaticAssert );
	eat( TokType::Capture_Start );

	s32 level = 0;
	while ( left && ( currtok.Type != TokType::Capture_End || level > 0 ) )
	{
		if ( currtok.Type == TokType::Capture_Start )
			level++;
		else if ( currtok.Type == TokType::Capture_End )
			level--;

		eat( currtok.Type );
	}
	eat( TokType::Capture_End );
	eat( TokType::Statement_End );

	content.Length  = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)content.Text;

	char const* str = str_fmt_buf( "%.*s\n", content.Length, content.Text );
	assert->Content = get_cached_string( { content.Length + 1, str } );
	assert->Name	= assert->Content;

	Context.pop();
	return assert;
}

/*
	This a brute-froce make all the arguments part of the token provided.
	Can have in-place function signatures, regular identifiers, in-place typenames, compile-time expressions, parameter-pack expansion, etc.
	This means that validation can only go so far, and so if there is any different in formatting
	passed the basic stripping supported it report a soft failure.
*/
internal inline
void parse_template_args( Token& token )
{
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
		token.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)token.Text;
	}
}

// Variable parsing is handled in multiple places because its initial signature is shared with function parsing
internal
CodeVar parse_variable_after_name(
	  ModuleFlag        mflags
	, CodeAttributes    attributes
	, CodeSpecifiers    specifiers
	, CodeType          type
	, StrC              name
)
{
	push_scope();

	Code array_expr    = parse_array_decl();
	Code expr          = { nullptr };
	Code bitfield_expr = { nullptr };

	if ( bitfield_is_equal( u32, currtok.Flags, TF_Assign ) )
	{
		eat( TokType::Operator );

		Token expr_tok = currtok;

		if ( currtok.Type == TokType::Statement_End )
		{
			log_failure( "Expected expression after assignment operator\n%s", Context.to_string() );
			Context.pop();
			return CodeInvalid;
		}

		while ( left && currtok.Type != TokType::Statement_End )
		{
			eat( currtok.Type );
		}

		expr_tok.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)expr_tok.Text - 1;
		expr            = untyped_str( expr_tok );
	}

	if ( currtok.Type == TokType::BraceCurly_Open )
	{
		Token expr_tok = currtok;

		eat( TokType::BraceCurly_Open );

		s32 level = 0;
		while ( left && ( currtok.Type != TokType::BraceCurly_Close || level > 0 ) )
		{
			if ( currtok.Type == TokType::BraceCurly_Open )
				level++;

			else if ( currtok.Type == TokType::BraceCurly_Close && level > 0 )
				level--;

			eat( currtok.Type );
		}
		eat( TokType::BraceCurly_Close );

		expr_tok.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)expr_tok.Text;
		expr            = untyped_str( expr_tok );
	}

	if ( currtok.Type == TokType::Assign_Classifer )
	{
		eat( TokType::Assign_Classifer );

		Token expr_tok = currtok;

		if ( currtok.Type == TokType::Statement_End )
		{
			log_failure( "Expected expression after bitfield \n%s", Context.to_string() );
			Context.pop();
			return CodeInvalid;
		}

		while ( left && currtok.Type != TokType::Statement_End )
		{
			eat( currtok.Type );
		}

		expr_tok.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)expr_tok.Text;
		bitfield_expr   = untyped_str( expr_tok );
	}

	CodeVar     next_var   = NoCode;
	Token       stmt_end   = NullToken;
	CodeComment inline_cmt = NoCode;
	if ( type )
	{
		if ( currtok.Type == TokType::Comma )
		{
			// Were dealing with a statement with more than one declaration
			// This is only handled this way if its the first declaration
			// Otherwise its looped through in parse_variable_declaration_list
			next_var = parse_variable_declaration_list();
		}

		// If we're dealing with a "comma-procedding then we cannot expect a statement end or inline comment
		// Any comma procedding variable will not have a type provided so it can act as a indicator to skip this
		Token stmt_end = currtok;
		eat( TokType::Statement_End );

		// Check for inline comment : <type> <identifier> = <expression>; // <inline comment>
		if ( left && ( currtok_noskip.Type == TokType::Comment ) && currtok_noskip.Line == stmt_end.Line )
		{
			inline_cmt = parse_comment();
		}
	}

	using namespace ECode;

	CodeVar
	result              = (CodeVar) make_code();
	result->Type        = Variable;
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
		result->NextVar->Parent = result;
	}

	Context.pop();
	return result;
}

/*
	Note(Ed): This does not support the following:
	* Function Pointers
*/
internal CodeVar parse_variable_declaration_list()
{
	push_scope();

	CodeVar result   = NoCode;
	CodeVar last_var = NoCode;
	while ( check( TokType::Comma ) )
	{
		eat( TokType::Comma );

		CodeSpecifiers specifiers = NoCode;

		while ( left && currtok.is_specifier() )
		{
			SpecifierT spec = ESpecifier::to_type( currtok );

			switch ( spec )
			{
				case ESpecifier::Const:
					if ( specifiers->NumEntries && specifiers->ArrSpecs[ specifiers->NumEntries - 1 ] != ESpecifier::Ptr )
					{
						log_failure( "Error, const specifier must come after pointer specifier for variable declaration proceeding comma\n"
						"(Parser will add and continue to specifiers, but will most likely fail to compile)\n%s"
						, Context.to_string() );

						specifiers.append( spec );
					}
				break;

				case ESpecifier::Ptr:
				case ESpecifier::Ref:
				case ESpecifier::RValue:
				break;

				default:
				{
					log_failure( "Error, invalid specifier '%s' proceeding comma\n"
					"(Parser will add and continue to specifiers, but will most likely fail to compile)\n%s"
					, currtok.Text, Context.to_string() );
					continue;
				}
				break;
			}

			if ( specifiers )
				specifiers.append( spec );
			else
				specifiers = def_specifier( spec );
		}

		StrC name = currtok;
		eat( TokType::Identifier );

		CodeVar var = parse_variable_after_name( ModuleFlag::None, NoCode, specifiers, NoCode, name );

		// TODO(Ed) : CodeVar is going to need a procedure to append comma-defined vars to itself.
		if ( ! result )
		{
			result.ast   = var.ast;
			last_var.ast = var.ast;
		}
		else
		{
			last_var->NextVar.ast         = var.ast;
			last_var->NextVar->Parent.ast = rcast(AST*, var.ast);
			last_var.ast                  = var.ast;
		}
	}

	Context.pop();
	return result;
}

internal
CodeClass parse_class( bool inplace_def )
{
	using namespace Parser;
	push_scope();
	CodeClass result = (CodeClass) parse_class_struct( TokType::Decl_Class, inplace_def );
	Context.pop();
	return result;
}

internal
CodeConstructor parse_constructor()
{
	using namespace Parser;
	push_scope();

	Token       identifier       = parse_identifier();
	CodeParam   params           = parse_params();
	Code        initializer_list = NoCode;
	CodeBody    body             = NoCode;
	CodeComment inline_cmt       = NoCode;

	if ( check( TokType::Assign_Classifer ) )
	{
		eat( TokType::Assign_Classifer );

		Token initializer_list_tok = NullToken;

		s32 level = 0;
		while ( left && ( currtok.Type != TokType::BraceCurly_Open || level > 0 ) )
		{
			if ( currtok.Type == TokType::BraceCurly_Open )
				level++;
			else if ( currtok.Type == TokType::BraceCurly_Close )
				level--;

			eat( currtok.Type );
		}

		initializer_list_tok.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)initializer_list_tok.Text;

		initializer_list = untyped_str( initializer_list_tok );
		body             = parse_function_body();
	}
	else if ( check( TokType::BraceCurly_Open ) )
	{
		body = parse_function_body();
	}
	else
	{
		Token stmt_end = currtok;
		eat( TokType::Statement_End );

		if ( currtok_noskip.Type == TokType::Comment && currtok_noskip.Line == stmt_end.Line )
			inline_cmt = parse_comment();
	}

	CodeConstructor result = (CodeConstructor) make_code();

	if ( params )
		result->Params = params;

	if ( initializer_list )
		result->InitializerList = initializer_list;

	if ( body )
	{
		result->Body = body;
		result->Type = ECode::Constructor;
	}
	else
		result->Type = ECode::Constructor_Fwd;

	if ( inline_cmt )
		result->InlineCmt = inline_cmt;

	Context.pop();
	return result;
}

internal
CodeDestructor parse_destructor( CodeSpecifiers specifiers )
{
	using namespace Parser;
	push_scope();

	if ( check( TokType::Spec_Virtual ) )
	{
		if ( specifiers )
			specifiers.append( ESpecifier::Virtual );
		else
			specifiers = def_specifier( ESpecifier::Virtual );
		eat( TokType::Spec_Virtual );
	}

	if ( left && currtok.Text[0] == '~' )
		eat( TokType::Operator );
	else
	{
		log_failure( "Expected destructor '~' token\n%s", Context.to_string() );
		return CodeInvalid;
	}

	Token    identifier = parse_identifier();
	CodeBody body       = { nullptr };

	eat( TokType::Capture_Start );
	eat( TokType::Capture_End );

	if ( check( TokType::Operator ) && currtok.Text[0] == '=' )
	{
		eat( TokType::Operator );

		if ( left && currtok.Text[0] == '0' )
		{
			eat( TokType::Number );

			specifiers.append( ESpecifier::Pure );
		}
		else
		{
			log_failure( "Pure specifier expected due to '=' token\n%s", Context.to_string() );
			return CodeInvalid;
		}
	}

	CodeComment inline_cmt = NoCode;

	if ( check( TokType::BraceCurly_Open ) )
		body = parse_function_body();
	else
	{
		Token stmt_end = currtok;
		eat( TokType::Statement_End );

		if ( currtok_noskip.Type == TokType::Comment && currtok_noskip.Line == stmt_end.Line )
			inline_cmt = parse_comment();
	}

	CodeDestructor result = (CodeDestructor) make_code();

	if ( specifiers )
		result->Specs = specifiers;

	if ( body )
	{
		result->Body = body;
		result->Type = ECode::Destructor;
	}
	else
		result->Type = ECode::Destructor_Fwd;

	if ( inline_cmt )
		result->InlineCmt = inline_cmt;

	Context.pop();
	return result;
}

internal
CodeEnum parse_enum( bool inplace_def )
{
	using namespace Parser;
	using namespace ECode;
	push_scope();

	SpecifierT specs_found[16] { ESpecifier::NumSpecifiers };
	s32        NumSpecifiers = 0;

	CodeAttributes attributes = { nullptr };

	Token    name       = { nullptr, 0, TokType::Invalid };
	Code     array_expr = { nullptr };
	CodeType type       = { nullptr };

	char  entries_code[ kilobytes(128) ] { 0 };
	s32   entries_length = 0;

	bool is_enum_class = false;

	eat( TokType::Decl_Enum );

	if ( currtok.Type == TokType::Decl_Class )
	{
		eat( TokType::Decl_Class);
		is_enum_class = true;
	}

	attributes = parse_attributes();

	if ( check( TokType::Identifier ) )
	{
		name = currtok;
		Context.Scope->Name = currtok;
		eat( TokType::Identifier );
	}

	if ( currtok.Type == TokType::Assign_Classifer )
	{
		eat( TokType::Assign_Classifer );

		type = parse_type();
		if ( type == Code::Invalid )
		{
			log_failure( "Failed to parse enum classifier\n%s", Context.to_string() );
			Context.pop();
			return CodeInvalid;
		}
	}

	CodeBody body = { nullptr };

	if ( currtok.Type == TokType::BraceCurly_Open )
	{
		body       = (CodeBody) make_code();
		body->Type = ECode::Enum_Body;

		eat( TokType::BraceCurly_Open );

		Code member = CodeInvalid;

		while ( left && currtok_noskip.Type != TokType::BraceCurly_Close )
		{
			if ( currtok_noskip.Type == TokType::Preprocess_Hash )
				eat( TokType::Preprocess_Hash );

			switch ( currtok_noskip.Type )
			{
				case TokType::NewLine:
					member = untyped_str( currtok_noskip );
					eat( TokType::NewLine );
				break;

				case TokType::Comment:
					member = parse_comment();
				break;

				case TokType::Preprocess_Define:
					member = parse_define();
				break;

				case TokType::Preprocess_If:
				case TokType::Preprocess_IfDef:
				case TokType::Preprocess_IfNotDef:
				case TokType::Preprocess_ElIf:
					member = parse_preprocess_cond();
				break;

				case TokType::Preprocess_Else:
					member = preprocess_else;
					eat( TokType::Preprocess_Else );
				break;

				case TokType::Preprocess_EndIf:
					member = preprocess_endif;
					eat( TokType::Preprocess_EndIf );
				break;

				case TokType::Preprocess_Macro:
					member = parse_simple_preprocess( TokType::Preprocess_Macro );
				break;

				case TokType::Preprocess_Pragma:
					member = parse_pragma();
				break;

				case TokType::Preprocess_Unsupported:
					member = parse_simple_preprocess( TokType::Preprocess_Unsupported );
				break;

				default:
					Token entry = currtok;

					eat( TokType::Identifier);

					if ( currtok.Type == TokType::Operator && currtok.Text[0] == '=' )
					{
						eat( TokType::Operator );

						while ( currtok_noskip.Type != TokType::Comma && currtok_noskip.Type != TokType::BraceCurly_Close )
						{
							eat( currtok_noskip.Type );
						}
					}

					if ( currtok.Type == TokType::Comma )
					{
						eat( TokType::Comma );
					}

					entry.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)entry.Text;

					member = untyped_str( entry );
				break;
			}

			if ( member == Code::Invalid )
			{
				log_failure( "Failed to parse member\n%s", Context.to_string() );
				Context.pop();
				return CodeInvalid;
			}

			body.append( member );
		}

		eat( TokType::BraceCurly_Close );
	}

	CodeComment inline_cmt = NoCode;

	if ( ! inplace_def )
	{
		Token stmt_end = currtok;
		eat( TokType::Statement_End );

		if ( currtok_noskip.Type == TokType::Comment && currtok_noskip.Line == stmt_end.Line )
			inline_cmt = parse_comment();
	}

	using namespace ECode;

	CodeEnum
	result = (CodeEnum) make_code();

	if ( body.ast )
	{
		result->Type = is_enum_class ? Enum_Class : Enum;
		result->Body = body;
	}
	else
	{
		result->Type = is_enum_class ? Enum_Class_Fwd : Enum_Fwd;
	}

	result->Name = get_cached_string( name );

	if ( attributes )
		result->Attributes = attributes;

	if ( type )
		result->UnderlyingType = type;

	if ( inline_cmt )
		result->InlineCmt = inline_cmt;

	Context.pop();
	return result;
}

internal inline
CodeBody parse_export_body()
{
	using namespace Parser;
	push_scope();
	CodeBody result = parse_global_nspace( ECode::Export_Body );
	Context.pop();
	return result;
}

CodeBody parse_export_body( StrC def )
{
	check_parse_args( def );
	using namespace Parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return CodeInvalid;

	Context.Tokens = toks;
	return parse_export_body();
}

internal inline
CodeBody parse_extern_link_body()
{
	using namespace Parser;
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

	Code entry = parse_extern_link_body();
	if ( entry == Code::Invalid )
	{
		log_failure( "Failed to parse body\n%s", Context.to_string() );
		Context.pop();
		return result;
	}

	result->Body = entry;

	Context.pop();
	return result;
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
	CodeType type = parse_type();
	if ( type == Code::Invalid )
	{
		Context.pop();
		return CodeInvalid;
	}

	// Funciton declaration
	if ( currtok.Type == TokType::Identifier )
	{
		// Name
		Token name = parse_identifier();
		Context.Scope->Name = name;

		// Parameter list
		CodeParam params = parse_params();

		function             = make_code();
		function->Type       = Function_Fwd;
		function->Name       = get_cached_string( name );
		function->ReturnType = type;

		if ( params )
			function->Params = params;
	}

	Token stmt_end = currtok;
	eat( TokType::Statement_End );

	CodeComment inline_cmt = NoCode;
	if ( currtok_noskip.Type == TokType::Comment && currtok_noskip.Line == stmt_end.Line )
		inline_cmt = parse_comment();

	CodeFriend
	result       = (CodeFriend) make_code();
	result->Type = Friend;

	if ( function )
		result->Declaration = function;

	else
		result->Declaration = type;

	if ( inline_cmt )
		result->InlineCmt = inline_cmt;

	Context.pop();
	return result;
}

internal
CodeFn parse_function()
{
	using namespace Parser;
	push_scope();

	SpecifierT specs_found[16] { ESpecifier::NumSpecifiers };
	s32        NumSpecifiers = 0;

	CodeAttributes attributes = { nullptr };
	CodeSpecifiers specifiers = { nullptr };
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
			case ESpecifier::ForceInline:
			case ESpecifier::Inline:
			case ESpecifier::NeverInline:
			case ESpecifier::Static:
			break;

			default:
				log_failure( "Invalid specifier %s for functon\n%s", ESpecifier::to_str(spec), Context.to_string() );
				Context.pop();
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

	CodeType ret_type = parse_type();
	if ( ret_type == Code::Invalid )
	{
		Context.pop();
		return CodeInvalid;
	}

	Token name = parse_identifier();
	Context.Scope->Name = name;
	if ( ! name )
	{
		Context.pop();
		return CodeInvalid;
	}

	CodeFn result = parse_function_after_name( mflags, attributes, specifiers, ret_type, name );

	Context.pop();
	return result;
}

internal
CodeNS parse_namespace()
{
	using namespace Parser;
	push_scope();

	eat( TokType::Decl_Namespace );

	Token name = parse_identifier();
	Context.Scope->Name = name;

	CodeBody body = parse_global_nspace( ECode::Namespace_Body );
	if ( body == Code::Invalid )
	{
		Context.pop();
		return CodeInvalid;
	}

	CodeNS
	result       = (CodeNS) make_code();
	result->Type = ECode::Namespace;
	result->Name = get_cached_string( name );

	result->Body = body;

	Context.pop();
	return result;
}

internal
CodeOperator parse_operator()
{
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
			case ESpecifier::ForceInline:
			case ESpecifier::Inline:
			case ESpecifier::NeverInline:
			case ESpecifier::Static:
			break;

			default:
				log_failure( "Invalid specifier " "%s" " for operator\n%s", ESpecifier::to_str(spec), Context.to_string() );
				Context.pop();
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
	CodeType ret_type = parse_type();

	CodeOperator result = parse_operator_after_ret_type( mflags, attributes, specifiers, ret_type );

	Context.pop();
	return result;
}

internal
CodeOpCast parse_operator_cast( CodeSpecifiers specifiers )
{
	using namespace parser;
	push_scope();

	// TODO : Specifiers attributed to the cast

	// Operator's namespace if not within same class.
	Token name = NullToken;
	if ( check( TokType::Identifier ) )
	{
		name = currtok;
		while ( left && currtok.Type == TokType::Identifier )
		{
			eat( TokType::Identifier );

			if ( currtok.Type == TokType::Access_StaticSymbol )
				eat( TokType::Access_StaticSymbol );
		}

		name.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)name.Text;
	}

	eat( TokType::Decl_Operator );

	Code type = parse_type();

	Context.Scope->Name = { type->Name.Data, type->Name.length() };

	eat( TokType::Capture_Start );
	eat( TokType::Capture_End );

	if ( check(TokType::Spec_Const))
	{
		if ( specifiers.ast == nullptr )
			specifiers = def_specifier( ESpecifier::Const );

		else
			specifiers.append( ESpecifier::Const );

		eat( TokType::Spec_Const );
	}

	Code        body       = NoCode;
	CodeComment inline_cmt = NoCode;

	if ( check( TokType::BraceCurly_Open) )
	{
		eat( TokType::BraceCurly_Open );

		Token body_str = currtok;

		s32 level = 0;
		while ( left && ( currtok.Type != TokType::BraceCurly_Close || level > 0 ) )
		{
			if ( currtok.Type == TokType::BraceCurly_Open )
				level++;

			else if ( currtok.Type == TokType::BraceCurly_Close )
				level--;

			eat( currtok.Type );
		}
		body_str.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)body_str.Text;

		eat( TokType::BraceCurly_Close );

		body = untyped_str( body_str );
	}
	else
	{
		Token stmt_end = currtok;
		eat( TokType::Statement_End );

		if ( currtok_noskip.Type == TokType::Comment && currtok_noskip.Line == stmt_end.Line )
			inline_cmt = parse_comment();
	}

	CodeOpCast result = (CodeOpCast) make_code();

	if ( name )
		result->Name = get_cached_string( name );

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

internal inline
CodeStruct parse_struct( bool inplace_def )
{
	push_scope();
	CodeStruct result = (CodeStruct) parse_class_struct( TokType::Decl_Struct, inplace_def );
	Context.pop();
	return result;
}

internal
CodeTemplate parse_template()
{
#	define UseTemplateCapture true

	push_scope();

	ModuleFlag mflags = ModuleFlag::None;

	if ( check(TokType::Module_Export) )
	{
		mflags = ModuleFlag::Export;
		eat( TokType::Module_Export );
	}

	eat( TokType::Decl_Template );

	Code params = parse_params( UseTemplateCapture );
	if ( params == Code::Invalid )
	{
		Context.pop();
		return CodeInvalid;
	}

	Code definition = { nullptr };

	while ( left )
	{
		if ( check( TokType::Decl_Class ) )
		{
			definition = parse_class();
			break;
		}

		if ( check( TokType::Decl_Struct ) )
		{
			definition = parse_struct();
			break;
		}

		if ( check( TokType::Decl_Using ))
		{
			definition = parse_using();
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
				case ESpecifier::ForceInline:
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
					Context.pop();
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

		definition = parse_operator_function_or_variable( expects_function, attributes, specifiers );
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
CodeType parse_type( bool* typedef_is_function )
{
	push_scope();

	Token context_tok = prevtok;

	SpecifierT specs_found[16] { ESpecifier::NumSpecifiers };
	s32        NumSpecifiers = 0;

	Token name = { nullptr, 0, TokType::Invalid };

	// Attributes are assumed to be before the type signature
	CodeAttributes attributes = parse_attributes();

	// Prefix specifiers
	while ( left && currtok.is_specifier() )
	{
		SpecifierT spec = ESpecifier::to_type( currtok );

		if ( spec != ESpecifier::Const )
		{
			log_failure( "Error, invalid specifier used in type definition: %s\n%s", currtok.Text, Context.to_string() );
			Context.pop();
			return CodeInvalid;
		}

		specs_found[NumSpecifiers] = spec;
		NumSpecifiers++;
		eat( currtok.Type );
	}

	if ( left == 0 )
	{
		log_failure( "Error, unexpected end of type definition\n%s", Context.to_string() );
		Context.pop();
		return CodeInvalid;
	}

	// All kinds of nonsense can makeup a type signature, first we check for a in-place definition of a class, enum, or struct
	if (   currtok.Type == TokType::Decl_Class
		|| currtok.Type == TokType::Decl_Enum
		|| currtok.Type == TokType::Decl_Struct
		|| currtok.Type == TokType::Decl_Union )
	{
		name = currtok;
		eat( currtok.Type );

		name.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)name.Text;
		eat( TokType::Identifier );
		Context.Scope->Name = name;
	}

#if 0
	else if ( currtok.Type == TokType::DeclType )
	{
		// Will have a capture and its own parsing rules, were going to just shove everything in a string (for now).
		name = currtok;
		eat( TokType::DeclType );

		eat( TokType::Capture_Start );
		while ( left && currtok.Type != TokType::Capture_End )
		{
			if ( currtok.Type == TokType::Capture_Start )
				level++;

			if ( currtok.Type == TokType::Capture_End )
				level--;

			eat( currtok.Type );
		}
		eat( TokType::Capture_End );

		name.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)name.Text;
		Context.Scope->Name = name;
	}
#endif

	// Check if native type keywords are used, eat them for the signature.
	else if ( currtok.Type >= TokType::Type_Unsigned && currtok.Type <= TokType::Type_MS_W64 )
	{
		// TODO(Ed) : Review this... Its necessary for parsing however the algo's path to this is lost...
		name = currtok;
		eat( currtok.Type );

		while ( left && currtok.Type >= TokType::Type_Unsigned && currtok.Type <= TokType::Type_MS_W64 )
		{
			eat( currtok.Type );
		}

		name.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)name.Text;
	}

	// The usual Identifier type signature that may have namespace qualifiers
	else
	{
		name = parse_identifier();
		Context.Scope->Name = name;
		if ( ! name )
		{
			log_failure( "Error, failed to type signature\n%s", Context.to_string() );
			Context.pop();
			return CodeInvalid;
		}
	}

	// Suffix specifiers for typename.
	while ( left && currtok.is_specifier() )
	{
		SpecifierT spec = ESpecifier::to_type( currtok );

		if (   spec != ESpecifier::Const
			&& spec != ESpecifier::Ptr
			&& spec != ESpecifier::Ref
			&& spec != ESpecifier::RValue )
		{
			log_failure( "Error, invalid specifier used in type definition: %s\n%s", currtok.Text, Context.to_string() );
			Context.pop();
			return CodeInvalid;
		}

		specs_found[NumSpecifiers] = spec;
		NumSpecifiers++;
		eat( currtok.Type );
	}

#ifdef GEN_USE_NEW_TYPENAME_PARSING
	if ( NumSpecifiers )
	{
		specifiers = def_specifiers( NumSpecifiers, specs_found );
		NumSpecifiers = 0;
	}
#endif

	// For function type signatures
	CodeType  return_type = NoCode;
	CodeParam params      = NoCode;

#ifdef GEN_USE_NEW_TYPENAME_PARSING
	CodeParam params_nested = NoCode;
#endif

	bool is_function_typename = false;
	Token* last_capture = nullptr;
	{
		Token* scanner = Context.Tokens.Arr + Context.Tokens.Idx;

		// An identifier being within a typename's signature only occurs if were parsing a typename for a typedef.
		if ( typedef_is_function && scanner->Type == TokType::Identifier )
		{
			is_function_typename = true;
			++ scanner;
		}
		is_function_typename = scanner->Type == TokType::Capture_Start;

		Token* first_capture = scanner;
		if ( is_function_typename )
		{
			// Go to the end of the signature
			while ( scanner->Type != TokType::Statement_End && scanner->Type != TokType::BraceCurly_Open )
				++ scanner;

			// Go back to the first capture start found
			while ( scanner->Type != TokType::Capture_Start )
				-- scanner;

			last_capture = scanner;
		}

		bool has_context = Context.Scope && Context.Scope->Prev;
		bool is_for_opcast = has_context && str_compare( Context.Scope->Prev->ProcName, "parse_operator_cast" ) == 0;
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
		return_type       = (CodeType) make_code();
		return_type->Type = ECode::Typename;

		// String
		// name_stripped = String::make( GlobalAllocator, name );
		// name_stripped.strip_space();
		return_type->Name = get_cached_string( name );

	#ifdef GEN_USE_NEW_TYPENAME_PARSING
		if ( specifiers )
		{
			return_type->Specs = specifiers;
			specifiers         = nullptr;
		}

	#else
		if ( NumSpecifiers )
			return_type->Specs = def_specifiers( NumSpecifiers, (SpecifierT*)specs_found );

		// Reset specifiers, the function itself will have its own suffix specifiers possibly.
		NumSpecifiers = 0;
	#endif

		name = { nullptr, 0, TokType::Invalid };

		// The next token can either be a capture for the identifier or it could be the identifier exposed.
		if ( ! check( TokType::Capture_Start ) )
		{
			// Started with an identifier immeidately, which means its of the format: <ReturnType> <identifier> <capture>;
			name = parse_identifier();
		}

		// If the next token is a capture start and is not the last capture, then we're dealing with function typename whoose identifier is within the capture.
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
			eat( TokType::Capture_Start );

			// Binding specifiers
			while ( left && currtok.is_specifier() )
			{
				SpecifierT spec = ESpecifier::to_type( currtok );

				if (   spec != ESpecifier::Ptr
					&& spec != ESpecifier::Ref
					&& spec != ESpecifier::RValue )
				{
					log_failure( "Error, invalid specifier used in type definition: %s\n%s", currtok.Text, Context.to_string() );
					Context.pop();
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
			NumSpecifiers = 0;

			if ( check( TokType::Identifier ))
				name = parse_identifier();

			// Immeidate parameters

			if ( check( TokType::Capture_Start ))
				params_nested = parse_params();

		#else
			// Starting immediatley with a capture, most likely declaring a typename for a member function pointer.
			// Everything within this capture will just be shoved into the name field including the capture tokens themselves.
			name = currtok;

			eat( TokType::Capture_Start );
			s32 level = 0;
			while ( left && ( currtok.Type != TokType::Capture_End || level > 0 ))
			{
				if ( currtok.Type == TokType::Capture_Start )
					level++;

				if ( currtok.Type == TokType::Capture_End )
					level--;

				eat( currtok.Type );
			}
			eat( TokType::Capture_End );

			name.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)name.Text;
		#endif
		}

		// Were now dealing with the parameters of the function
		params = parse_params();

		// Look for suffix specifiers for the function
		while ( left && currtok.is_specifier() )
		{
			SpecifierT spec = ESpecifier::to_type( currtok );

			if (   spec != ESpecifier::Const
				// TODO : Add support for NoExcept
				// && spec != ESpecifier::NoExcept
				&& spec != ESpecifier::RValue )
			{
				log_failure( "Error, invalid specifier used in type definition: %s\n%s", currtok.Text, Context.to_string() );
				Context.pop();
				return CodeInvalid;
			}

			specs_found[NumSpecifiers] = spec;
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
	}

	bool is_param_pack = false;
	if ( check(TokType::Varadic_Argument) )
	{
		is_param_pack = true;
		eat( TokType::Varadic_Argument );
	}

	using namespace ECode;

	CodeType
	result        = (CodeType) make_code();
	result->Type  = Typename;
	// result->Token = Context.Scope->Start;

	// Need to wait until were using the new parsing method to do this.
	String name_stripped = strip_formatting( name, strip_formatting_dont_preserve_newlines );

	// name_stripped.strip_space();

#ifdef GEN_USE_NEW_TYPENAME_PARSING
	if ( params_nested )
	{
		name_stripped.append( params_nested->to_string() );
	}
#endif

	result->Name = get_cached_string( name_stripped );

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
	if (NumSpecifiers)
	{
		Code specifiers = def_specifiers( NumSpecifiers, (SpecifierT*)specs_found );
		result->Specs = specifiers;
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

	Context.pop();
	return result;
}

internal
CodeTypedef parse_typedef()
{
	push_scope();

	bool  is_function = false;
	Token name        = { nullptr, 0, TokType::Invalid };
	Code  array_expr  = { nullptr };
	Code  type        = { nullptr };

	ModuleFlag mflags = ModuleFlag::None;

	if ( check(TokType::Module_Export) )
	{
		mflags = ModuleFlag::Export;
		eat( TokType::Module_Export );
	}

	eat( TokType::Decl_Typedef );

	constexpr bool from_typedef = true;

#if GEN_PARSER_DISABLE_MACRO_TYPEDEF
	if ( false )
#else
	if ( check( TokType::Preprocess_Macro ))
#endif
	{
		type = t_empty;
		name = currtok;
		Context.Scope->Name = name;
		eat( TokType::Preprocess_Macro );
	}
	else
	{
		bool is_complicated =
				currtok.Type == TokType::Decl_Enum
			||	currtok.Type == TokType::Decl_Class
			||	currtok.Type == TokType::Decl_Struct
			||	currtok.Type == TokType::Decl_Union;

		// This code is highly correlated with parse_compilcated_definition
		if ( is_complicated )
		{
			TokArray tokens = Context.Tokens;

			s32 idx = tokens.Idx;
			s32 level = 0;
			for ( ; idx < tokens.Arr.num(); idx ++ )
			{
				if ( tokens[idx].Type == TokType::BraceCurly_Open )
					level++;

				if ( tokens[idx].Type == TokType::BraceCurly_Close )
					level--;

				if ( level == 0 && tokens[idx].Type == TokType::Statement_End )
					break;
			}

			if ( (idx - 2 ) == tokens.Idx )
			{
				// Its a forward declaration only
				type = parse_forward_or_definition( currtok.Type, from_typedef );
			}

			Token tok = tokens[ idx - 1 ];
			if ( tok.Type == TokType::Identifier )
			{
				tok = tokens[ idx - 2 ];

				bool is_indirection = tok.Type == TokType::Ampersand
				||                    tok.Type == TokType::Star;

				bool ok_to_parse = false;

				if ( tok.Type == TokType::BraceCurly_Close )
				{
					// Its an inplace definition
					// typdef <which> <type_identifier> { ... } <identifier>;
					ok_to_parse = true;
				}
				else if ( tok.Type == TokType::Identifier && tokens[ idx - 3 ].Type == TokType::Decl_Struct )
				{
					// Its a variable with type ID using struct namespace.
					// <which> <type_identifier> <identifier>;
					ok_to_parse = true;
				}
				else if ( is_indirection )
				{
					// Its a indirection type with type ID using struct namespace.
					// <which> <type_identifier>* <identifier>;
					ok_to_parse = true;
				}

				if ( ! ok_to_parse )
				{
					log_failure( "Unsupported or bad member definition after struct declaration\n%s", Context.to_string() );
					Context.pop();
					return CodeInvalid;
				}

				// TODO(Ed) : I'm not sure if I have to use parse_type here, I'd rather not as that would complicate parse_type.
				// type = parse_type();
				type = parse_forward_or_definition( currtok.Type, from_typedef );
			}
			else if ( tok.Type == TokType::BraceCurly_Close )
			{
				// Its a definition
				// <which> { ... };
				type = parse_forward_or_definition( currtok.Type, from_typedef );
			}
			else if ( tok.Type == TokType::BraceSquare_Close)
			{
				// Its an array definition
				// <which> <type_identifier> <identifier> [ ... ];
				type = parse_type();
			}
			else
			{
				log_failure( "Unsupported or bad member definition after struct declaration\n%s", Context.to_string() );
				Context.pop();
				return CodeInvalid;
			}
		}
		else
			type = parse_type( & is_function );

		if ( check( TokType::Identifier ) )
		{
			name = currtok;
			eat( TokType::Identifier );
		}
		else if ( ! is_function )
		{
			log_failure( "Error, expected identifier for typedef\n%s", Context.to_string() );
			Context.pop();
			return CodeInvalid;
		}
	}

	array_expr = parse_array_decl();

	Token stmt_end = currtok;
	eat( TokType::Statement_End );

	CodeComment inline_cmt = NoCode;
	if ( currtok_noskip.Type == TokType::Comment && currtok_noskip.Line == stmt_end.Line )
		inline_cmt = parse_comment();

	using namespace ECode;

	CodeTypedef
	result              = (CodeTypedef) make_code();
	result->Type        = Typedef;
	result->ModuleFlags = mflags;

	if ( is_function )
	{
		result->Name       = type->Name;
		result->IsFunction = true;
	}
	else
	{
		result->Name       = get_cached_string( name );
		result->IsFunction = false;
	}

	if ( type )
	{
		result->UnderlyingType         = type;
		result->UnderlyingType->Parent = rcast(AST*, result.ast);
	}
	// Type needs to be aware of its parent so that it can be serialized properly.

	if ( type->Type == Typename && array_expr && array_expr->Type != Invalid )
		type.cast<CodeType>()->ArrExpr = array_expr;

	if ( inline_cmt )
		result->InlineCmt = inline_cmt;

	Context.pop();
	return result;
}

internal neverinline
CodeUnion parse_union( bool inplace_def )
{
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
		Context.Scope->Name = currtok;
		eat( TokType::Identifier );
	}

	CodeBody body = { nullptr };

	eat( TokType::BraceCurly_Open );

	body = make_code();
	body->Type = ECode::Union_Body;

	while ( ! check_noskip( TokType::BraceCurly_Close ) )
	{
		if ( currtok_noskip.Type == TokType::Preprocess_Hash )
			eat( TokType::Preprocess_Hash );

		Code member = { nullptr };
		switch ( currtok_noskip.Type )
		{
			case TokType::NewLine:
				// Empty lines are auto skipped by Tokens.current()
				member = fmt_newline;
				eat( TokType::NewLine );
			break;

			case TokType::Comment:
				member = parse_comment();
			break;

			case TokType::Decl_Class:
				member = parse_complicated_definition( TokType::Decl_Class );
			break;

			case TokType::Decl_Enum:
				member = parse_complicated_definition( TokType::Decl_Enum );
			break;

			case TokType::Decl_Struct:
				member = parse_complicated_definition( TokType::Decl_Struct );
			break;

			case TokType::Decl_Union:
				member = parse_complicated_definition( TokType::Decl_Union );
			break;

			case TokType::Preprocess_Define:
				member = parse_define();
			break;

			case TokType::Preprocess_If:
			case TokType::Preprocess_IfDef:
			case TokType::Preprocess_IfNotDef:
			case TokType::Preprocess_ElIf:
				member = parse_preprocess_cond();
			break;

			case TokType::Preprocess_Else:
				member = preprocess_else;
				eat( TokType::Preprocess_Else );
			break;

			case TokType::Preprocess_EndIf:
				member = preprocess_endif;
				eat( TokType::Preprocess_EndIf );
			break;

			case TokType::Preprocess_Macro:
				member = parse_simple_preprocess( TokType::Preprocess_Macro );
			break;

			case TokType::Preprocess_Pragma:
				member = parse_pragma();
			break;

			case TokType::Preprocess_Unsupported:
				member = parse_simple_preprocess( TokType::Preprocess_Unsupported );
			break;

			default:
				member = parse_variable();
			break;
		}

		if ( member )
			body.append( member );
	}

	eat( TokType::BraceCurly_Close );

	if ( ! inplace_def )
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

internal
CodeUsing parse_using()
{
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
	Context.Scope->Name = name;
	eat( TokType::Identifier );

	if ( bitfield_is_equal( u32, currtok.Flags, TF_Assign ) )
	{
		attributes = parse_attributes();

		eat( TokType::Operator );

		type = parse_type();
	}

	array_expr = parse_array_decl();

	Token stmt_end = currtok;
	eat( TokType::Statement_End );

	CodeComment inline_cmt = NoCode;
	if ( currtok_noskip.Type == TokType::Comment && currtok_noskip.Line == stmt_end.Line )
	{
		inline_cmt = parse_comment();
	}

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

		if ( inline_cmt )
			result->InlineCmt = inline_cmt;
	}

	Context.pop();
	return result;
}

internal
CodeVar parse_variable()
{
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

			default:
				log_failure( "Invalid specifier %s for variable\n%s", ESpecifier::to_str( spec ), Context.to_string() );
				Context.pop();
				return CodeInvalid;
		}

		// Ignore const specifiers, they're handled by the type
		if ( spec == ESpecifier::Const )
			break;

		specs_found[NumSpecifiers] = spec;
		NumSpecifiers++;
		eat( currtok.Type );
	}

	if ( NumSpecifiers )
	{
		specifiers = def_specifiers( NumSpecifiers, specs_found );
	}

	CodeType type = parse_type();

	if ( type == Code::Invalid )
		return CodeInvalid;

	Context.Scope->Name = parse_identifier();

	CodeVar result = parse_variable_after_name( mflags, attributes, specifiers, type, Context.Scope->Name );

	Context.pop();
	return result;
}

// namespace parser
}
