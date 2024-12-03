#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "gen/etoktype.cpp"
#include "interface.upfront.cpp"
#include "lexer.cpp"
#include "parser.cpp"
#endif

// Publically Exposed Interface

CodeClass parse_class( StrC def )
{
	check_parse_args( def );
	using namespace parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	Context.Tokens = toks;
	push_scope();
	CodeClass result = (CodeClass) parse_class_struct( TokType::Decl_Class );
	Context.pop();
	return result;
}

CodeConstructor parse_constructor( StrC def )
{
	check_parse_args( def );
	using namespace parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	// TODO(Ed): Constructors can have prefix attributes

	CodeSpecifiers specifiers;
	SpecifierT     specs_found[ 16 ] { ESpecifier::NumSpecifiers };
	s32            NumSpecifiers = 0;

	while ( left && is_specifier(currtok) )
	{
		SpecifierT spec = ESpecifier::to_type( to_str(currtok) );

		b32 ignore_spec = false;

		switch ( spec )
		{
			case ESpecifier::Constexpr :
			case ESpecifier::Explicit:
			case ESpecifier::Inline :
			case ESpecifier::ForceInline :
			case ESpecifier::NeverInline :
				break;

			case ESpecifier::Const :
				ignore_spec = true;
				break;

			default :
				log_failure( "Invalid specifier %s for variable\n%s", ESpecifier::to_str( spec ), Context.to_string() );
				Context.pop();
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
	CodeConstructor result = parse_constructor( specifiers );
	return result;
}

CodeDestructor parse_destructor( StrC def )
{
	check_parse_args( def );
	using namespace parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	// TODO(Ed): Destructors can have prefix attributes
	// TODO(Ed): Destructors can have virtual

	Context.Tokens        = toks;
	CodeDestructor result = parse_destructor();
	return result;
}

CodeEnum parse_enum( StrC def )
{
	check_parse_args( def );
	using namespace parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
	{
		Context.pop();
		return InvalidCode;
	}

	Context.Tokens = toks;
	return parse_enum();
}

CodeBody parse_export_body( StrC def )
{
	check_parse_args( def );
	using namespace parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	Context.Tokens = toks;
	return parse_export_body();
}

CodeExtern parse_extern_link( StrC def )
{
	check_parse_args( def );
	using namespace parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	Context.Tokens = toks;
	return parse_extern_link();
}

CodeFriend parse_friend( StrC def )
{
	check_parse_args( def );
	using namespace parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	Context.Tokens = toks;
	return parse_friend();
}

CodeFn parse_function( StrC def )
{
	check_parse_args( def );
	using namespace parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	Context.Tokens = toks;
	return (CodeFn) parse_function();
}

CodeBody parse_global_body( StrC def )
{
	check_parse_args( def );
	using namespace parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	Context.Tokens = toks;
	push_scope();
	CodeBody result = parse_global_nspace( ECode::Global_Body );
	Context.pop();
	return result;
}

CodeNS parse_namespace( StrC def )
{
	check_parse_args( def );
	using namespace parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	Context.Tokens = toks;
	return parse_namespace();
}

CodeOperator parse_operator( StrC def )
{
	check_parse_args( def );
	using namespace parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	Context.Tokens = toks;
	return (CodeOperator) parse_operator();
}

CodeOpCast parse_operator_cast( StrC def )
{
	check_parse_args( def );
	using namespace parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	Context.Tokens = toks;
	return parse_operator_cast();
}

CodeStruct parse_struct( StrC def )
{
	check_parse_args( def );
	using namespace parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	Context.Tokens = toks;
	push_scope();
	CodeStruct result = (CodeStruct) parse_class_struct( TokType::Decl_Struct );
	Context.pop();
	return result;
}

CodeTemplate parse_template( StrC def )
{
	check_parse_args( def );
	using namespace parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	Context.Tokens = toks;
	return parse_template();
}

CodeType parse_type( StrC def )
{
	check_parse_args( def );
	using namespace parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	Context.Tokens = toks;
	return parse_type();
}

CodeTypedef parse_typedef( StrC def )
{
	check_parse_args( def );
	using namespace parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	Context.Tokens = toks;
	return parse_typedef();
}

CodeUnion parse_union( StrC def )
{
	check_parse_args( def );
	using namespace parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	Context.Tokens = toks;
	return parse_union();
}

CodeUsing parse_using( StrC def )
{
	check_parse_args( def );
	using namespace parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	Context.Tokens = toks;
	return parse_using();
}

CodeVar parse_variable( StrC def )
{
	check_parse_args( def );
	using namespace parser;

	TokArray toks = lex( def );
	if ( toks.Arr == nullptr )
		return InvalidCode;

	Context.Tokens = toks;
	return parse_variable();
}

// Undef helper macros
#	undef check_parse_args
#	undef currtok
#	undef prevtok
#   undef nexttok
#	undef eat
#	undef left
#	undef check
#	undef push_scope
