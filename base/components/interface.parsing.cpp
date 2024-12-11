#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "gen/etoktype.cpp"
#include "interface.upfront.cpp"
#include "lexer.cpp"
#include "parser.cpp"
#endif

// Publically Exposed Interface

void parser_define_macro( StrC )

CodeClass parse_class( StrC def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	Context.Tokens = toks;
	push_scope();
	CodeClass result = (CodeClass) parse_class_struct( Tok_Decl_Class, parser_not_inplace_def );
	parser_pop(& Context);
	return result;
}

CodeConstructor parse_constructor( StrC def )
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
		Specifier spec = strc_to_specifier( tok_to_str(currtok) );

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
				log_failure( "Invalid specifier %s for variable\n%s", spec_to_str( spec ), parser_to_string(Context) );
				parser_pop(& Context);
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

	Context.Tokens         = toks;
	CodeConstructor result = parser_parse_constructor( specifiers );
	return result;
}

CodeDestructor parse_destructor( StrC def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	// TODO(Ed): Destructors can have prefix attributes
	// TODO(Ed): Destructors can have virtual

	Context.Tokens        = toks;
	CodeDestructor result = parser_parse_destructor(NullCode);
	return result;
}

CodeEnum parse_enum( StrC def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
	{
		parser_pop(& Context);
		return InvalidCode;
	}

	Context.Tokens = toks;
	return parser_parse_enum( parser_not_inplace_def);
}

CodeBody parse_export_body( StrC def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	Context.Tokens = toks;
	return parser_parse_export_body();
}

CodeExtern parse_extern_link( StrC def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	Context.Tokens = toks;
	return parser_parse_extern_link();
}

CodeFriend parse_friend( StrC def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	Context.Tokens = toks;
	return parser_parse_friend();
}

CodeFn parse_function( StrC def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	Context.Tokens = toks;
	return (CodeFn) parser_parse_function();
}

CodeBody parse_global_body( StrC def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	Context.Tokens = toks;
	push_scope();
	CodeBody result = parse_global_nspace( CT_Global_Body );
	parser_pop(& Context);
	return result;
}

CodeNS parse_namespace( StrC def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	Context.Tokens = toks;
	return parser_parse_namespace();
}

CodeOperator parse_operator( StrC def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	Context.Tokens = toks;
	return (CodeOperator) parser_parse_operator();
}

CodeOpCast parse_operator_cast( StrC def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	Context.Tokens = toks;
	return parser_parse_operator_cast(NullCode);
}

CodeStruct parse_struct( StrC def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	Context.Tokens = toks;
	push_scope();
	CodeStruct result = (CodeStruct) parse_class_struct( Tok_Decl_Struct, parser_not_inplace_def );
	parser_pop(& Context);
	return result;
}

CodeTemplate parse_template( StrC def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	Context.Tokens = toks;
	return parser_parse_template();
}

CodeTypename parse_type( StrC def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	Context.Tokens = toks;
	return parser_parse_type( parser_not_from_template, nullptr);
}

CodeTypedef parse_typedef( StrC def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	Context.Tokens = toks;
	return parser_parse_typedef();
}

CodeUnion parse_union( StrC def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	Context.Tokens = toks;
	return parser_parse_union( parser_not_inplace_def);
}

CodeUsing parse_using( StrC def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	Context.Tokens = toks;
	return parser_parse_using();
}

CodeVar parse_variable( StrC def )
{
	GEN_USING_NS_PARSER;
	check_parse_args( def );

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	Context.Tokens = toks;
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
