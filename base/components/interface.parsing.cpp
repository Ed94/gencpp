#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "gen/etoktype.cpp"
#include "interface.upfront.cpp"
#include "lexer.cpp"
#include "parser.cpp"
#endif

// Publically Exposed Interface

CodeClass parse_class( Str def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	parser_ctx.Tokens = toks;
	push_scope();
	CodeClass result = (CodeClass) parse_class_struct( Tok_Decl_Class, parser_not_inplace_def );
	parser_pop(& parser_ctx);
	return result;
}

CodeConstructor parse_constructor( Str def )
{
	GEN_USING_NS_PARSER;
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
		Specifier spec = str_to_specifier( tok_to_str(currtok) );

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
				log_failure( "Invalid specifier %s for variable\n%S", spec_to_str( spec ), parser_to_strbuilder(parser_ctx) );
				parser_pop(& parser_ctx);
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
		specifiers = def_specifiers( NumSpecifiers, specs_found );
		// <specifiers> ...
	}

	parser_ctx.Tokens         = toks;
	CodeConstructor result = parser_parse_constructor( specifiers );
	return result;
}

CodeDestructor parse_destructor( Str def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	// TODO(Ed): Destructors can have prefix attributes
	// TODO(Ed): Destructors can have virtual

	parser_ctx.Tokens        = toks;
	CodeDestructor result = parser_parse_destructor(NullCode);
	return result;
}

CodeEnum parse_enum( Str def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
	{
		parser_pop(& parser_ctx);
		return InvalidCode;
	}

	parser_ctx.Tokens = toks;
	return parser_parse_enum( parser_not_inplace_def);
}

CodeBody parse_export_body( Str def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	parser_ctx.Tokens = toks;
	return parser_parse_export_body();
}

CodeExtern parse_extern_link( Str def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	parser_ctx.Tokens = toks;
	return parser_parse_extern_link();
}

CodeFriend parse_friend( Str def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	parser_ctx.Tokens = toks;
	return parser_parse_friend();
}

CodeFn parse_function( Str def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	parser_ctx.Tokens = toks;
	return (CodeFn) parser_parse_function();
}

CodeBody parse_global_body( Str def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	parser_ctx.Tokens = toks;
	push_scope();
	CodeBody result = parse_global_nspace( CT_Global_Body );
	parser_pop(& parser_ctx);
	return result;
}

CodeNS parse_namespace( Str def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	parser_ctx.Tokens = toks;
	return parser_parse_namespace();
}

CodeOperator parse_operator( Str def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	parser_ctx.Tokens = toks;
	return (CodeOperator) parser_parse_operator();
}

CodeOpCast parse_operator_cast( Str def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	parser_ctx.Tokens = toks;
	return parser_parse_operator_cast(NullCode);
}

CodeStruct parse_struct( Str def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	parser_ctx.Tokens = toks;
	push_scope();
	CodeStruct result = (CodeStruct) parse_class_struct( Tok_Decl_Struct, parser_not_inplace_def );
	parser_pop(& parser_ctx);
	return result;
}

CodeTemplate parse_template( Str def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	parser_ctx.Tokens = toks;
	return parser_parse_template();
}

CodeTypename parse_type( Str def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	parser_ctx.Tokens = toks;
	return parser_parse_type( parser_not_from_template, nullptr);
}

CodeTypedef parse_typedef( Str def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	parser_ctx.Tokens = toks;
	return parser_parse_typedef();
}

CodeUnion parse_union( Str def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	parser_ctx.Tokens = toks;
	return parser_parse_union( parser_not_inplace_def);
}

CodeUsing parse_using( Str def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	parser_ctx.Tokens = toks;
	return parser_parse_using();
}

CodeVar parse_variable( Str def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	parser_ctx.Tokens = toks;
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
