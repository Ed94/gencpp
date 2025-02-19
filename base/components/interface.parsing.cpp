#ifdef INTELLISENSE_DIRECTIVES
#pragma once
#include "gen/etoktype.hpp"
#include "interface.upfront.cpp"
#include "lexer.cpp"
#include "parser.cpp"
#endif

// Publically Exposed Interface

ParseInfo wip_parse_str(LexedInfo lexed, Opts_parse opts)
{
	TokArray toks;
	if (lexed.tokens.Num == 0 && lexed.tokens.Ptr == nullptr) {
		check_parse_args(lexed.text);
		toks = lex(lexed.text);

		TokenSlice slice = { toks.Arr, scast(s32, array_num(toks.Arr)) };
		lexed.tokens = slice;
	}
	ParseInfo info = struct_zero(ParseInfo);
	info.lexed = lexed;

	// TODO(Ed): ParseInfo should be set to the parser context.

	_ctx->parser.Tokens = toks;
	push_scope();
	CodeBody result = parse_global_nspace(CT_Global_Body);
	parser_pop(& _ctx->parser);

	return info;
}

CodeClass parse_class( Str def )
{
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	_ctx->parser.Tokens = toks;
	push_scope();
	CodeClass result = (CodeClass) parse_class_struct( Tok_Decl_Class, parser_not_inplace_def );
	parser_pop(& _ctx->parser);
	return result;
}

CodeConstructor parse_constructor( Str def )
{
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	// TODO(Ed): Constructors can have prefix attributes

	CodeSpecifiers specifiers = NullCode;
	Specifier      specs_found[ 16 ] = { Spec_NumSpecifiers };
	s32            NumSpecifiers = 0;

	while ( left && tok_is_specifier(currtok) )
	{
		Specifier spec = str_to_specifier( currtok.Text );

		b32 ignore_spec = false;

		switch ( spec )
		{
			case Spec_Constexpr :
			case Spec_Explicit:
			case Spec_Inline :
			case Spec_ForceInline :
			case Spec_NeverInline :
				break;

			case Spec_Const :
				ignore_spec = true;
				break;

			default :
				log_failure( "Invalid specifier %s for variable\n%S", spec_to_str( spec ), parser_to_strbuilder(_ctx->parser) );
				parser_pop(& _ctx->parser);
				return InvalidCode;
		}

		// Every specifier after would be considered part of the type type signature
		if (ignore_spec)
			break;

		specs_found[ NumSpecifiers ] = spec;
		NumSpecifiers++;
		eat( currtok.Type );
	}

	if ( NumSpecifiers )
	{
		specifiers = def_specifiers_arr( NumSpecifiers, specs_found );
		// <specifiers> ...
	}

	_ctx->parser.Tokens         = toks;
	CodeConstructor result = parser_parse_constructor( specifiers );
	return result;
}

CodeDefine parse_define( Str def )
{
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	_ctx->parser.Tokens = toks;
	push_scope();
	CodeDefine result = parser_parse_define();
	parser_pop(& _ctx->parser);
	return result;
}

CodeDestructor parse_destructor( Str def )
{
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	// TODO(Ed): Destructors can have prefix attributes
	// TODO(Ed): Destructors can have virtual

	_ctx->parser.Tokens        = toks;
	CodeDestructor result = parser_parse_destructor(NullCode);
	return result;
}

CodeEnum parse_enum( Str def )
{
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
	{
		parser_pop(& _ctx->parser);
		return InvalidCode;
	}

	_ctx->parser.Tokens = toks;
	return parser_parse_enum( parser_not_inplace_def);
}

CodeBody parse_export_body( Str def )
{
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	_ctx->parser.Tokens = toks;
	return parser_parse_export_body();
}

CodeExtern parse_extern_link( Str def )
{
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	_ctx->parser.Tokens = toks;
	return parser_parse_extern_link();
}

CodeFriend parse_friend( Str def )
{
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	_ctx->parser.Tokens = toks;
	return parser_parse_friend();
}

CodeFn parse_function( Str def )
{
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	_ctx->parser.Tokens = toks;
	return (CodeFn) parser_parse_function();
}

CodeBody parse_global_body( Str def )
{
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	_ctx->parser.Tokens = toks;
	push_scope();
	CodeBody result = parse_global_nspace( CT_Global_Body );
	parser_pop(& _ctx->parser);
	return result;
}

CodeNS parse_namespace( Str def )
{
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	_ctx->parser.Tokens = toks;
	return parser_parse_namespace();
}

CodeOperator parse_operator( Str def )
{
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	_ctx->parser.Tokens = toks;
	return (CodeOperator) parser_parse_operator();
}

CodeOpCast parse_operator_cast( Str def )
{
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	_ctx->parser.Tokens = toks;
	return parser_parse_operator_cast(NullCode);
}

CodeStruct parse_struct( Str def )
{
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	_ctx->parser.Tokens = toks;
	push_scope();
	CodeStruct result = (CodeStruct) parse_class_struct( Tok_Decl_Struct, parser_not_inplace_def );
	parser_pop(& _ctx->parser);
	return result;
}

CodeTemplate parse_template( Str def )
{
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	_ctx->parser.Tokens = toks;
	return parser_parse_template();
}

CodeTypename parse_type( Str def )
{
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	_ctx->parser.Tokens = toks;
	return parser_parse_type( parser_not_from_template, nullptr);
}

CodeTypedef parse_typedef( Str def )
{
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	_ctx->parser.Tokens = toks;
	return parser_parse_typedef();
}

CodeUnion parse_union( Str def )
{
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	_ctx->parser.Tokens = toks;
	return parser_parse_union( parser_not_inplace_def);
}

CodeUsing parse_using( Str def )
{
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	_ctx->parser.Tokens = toks;
	return parser_parse_using();
}

CodeVar parse_variable( Str def )
{
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	_ctx->parser.Tokens = toks;
	return parser_parse_variable();
}

// Undef helper macros
#undef check_parse_args
#undef currtok_noskip
#undef currtok
#undef peektok
#undef prevtok
#undef nexttok
#undef nexttok_noskip
#undef eat
#undef left
#undef check
#undef push_scope
#undef def_assign

// Here for C Variant
#undef lex_dont_skip_formatting
#undef lex_skip_formatting

#undef parser_inplace_def
#undef parser_not_inplace_def
#undef parser_dont_consume_braces
#undef parser_consume_braces
#undef parser_not_from_template
#undef parser_use_parenthesis
#undef parser_strip_formatting_dont_preserve_newlines
