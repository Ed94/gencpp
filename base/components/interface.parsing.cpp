#ifdef INTELLISENSE_DIRECTIVES
#pragma once
#include "gen/etoktype.hpp"
#include "interface.upfront.cpp"
#include "lexer.cpp"
#include "parser.cpp"
#endif

// Publically Exposed Interface

ParseInfo wip_parse_str(LexedInfo lexed, ParseOpts* opts)
{
	// TODO(Ed): Lift this.
	Context* ctx = _ctx;

	if (lexed.tokens.num == 0 && lexed.tokens.ptr == nullptr) {
		check_parse_args(lexed.text);
		lexed = lex(ctx, lexed.text);
	}
	ParseInfo info = struct_zero(ParseInfo);
	info.lexed = lexed;

	// TODO(Ed): ParseInfo should be set to the parser context.

	ctx->parser = struct_zero(ParseContext);
	ctx->parser.tokens = lexed.tokens;

	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope);

	CodeBody result = parse_global_nspace(ctx,CT_Global_Body);

	parser_pop(& ctx->parser);
	return info;
}

CodeClass parse_class( Str def )
{
	// TODO(Ed): Lift this.
	Context* ctx = _ctx;

	check_parse_args( def );

	ctx->parser = struct_zero(ParseContext);

	LexedInfo lexed = lex(ctx, def);
	ctx->parser.tokens = lexed.tokens;
	if ( ctx->parser.tokens.ptr == nullptr )
		return InvalidCode;

	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope);
	CodeClass result = (CodeClass) parse_class_struct( ctx, Tok_Decl_Class, parser_not_inplace_def );
	parser_pop(& ctx->parser);
	return result;
}

CodeConstructor parse_constructor(Str def )
{
	// TODO(Ed): Lift this.
	Context* ctx = _ctx;

	check_parse_args( def );

	ctx->parser = struct_zero(ParseContext);

	LexedInfo lexed = lex(ctx, def);
	ctx->parser.tokens = lexed.tokens;
	if ( ctx->parser.tokens.ptr == nullptr )
		return InvalidCode;

	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope);

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
				log_failure( "Invalid specifier %s for variable\n%S", spec_to_str( spec ), parser_to_strbuilder(& ctx->parser, ctx->Allocator_Temp) );
				parser_pop(& ctx->parser);
				return InvalidCode;
		}

		// Every specifier after would be considered part of the type type signature
		if (ignore_spec)
			break;

		specs_found[ NumSpecifiers ] = spec;
		NumSpecifiers++;
		eat( currtok.Type );
	}

	if ( NumSpecifiers ) {
		specifiers = def_specifiers_arr( NumSpecifiers, specs_found );
		// <specifiers> ...
	}

	CodeConstructor result = parser_parse_constructor(ctx, specifiers);
	parser_pop(& ctx->parser);
	return result;
}

CodeDefine parse_define( Str def )
{
	// TODO(Ed): Lift this.
	Context* ctx = _ctx;
	
	check_parse_args( def );

	ctx->parser = struct_zero(ParseContext);

	LexedInfo lexed = lex(ctx, def);
	ctx->parser.tokens = lexed.tokens;
	if ( ctx->parser.tokens.ptr == nullptr )
		return InvalidCode;

	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope);
	CodeDefine result = parser_parse_define(ctx);
	parser_pop(& ctx->parser);
	return result;
}

CodeDestructor parse_destructor( Str def )
{
	// TODO(Ed): Lift this.
	Context* ctx = _ctx;

	check_parse_args( def );

	ctx->parser = struct_zero(ParseContext);

	LexedInfo lexed = lex(ctx, def);
	ctx->parser.tokens = lexed.tokens;
	if ( ctx->parser.tokens.ptr == nullptr )
		return InvalidCode;

	// TODO(Ed): Destructors can have prefix attributes
	// TODO(Ed): Destructors can have virtual

	CodeDestructor result = parser_parse_destructor(ctx, NullCode);
	return result;
}

CodeEnum parse_enum( Str def )
{
	// TODO(Ed): Lift this.
	Context* ctx = _ctx;

	check_parse_args( def );

	ctx->parser = struct_zero(ParseContext);

	LexedInfo lexed = lex(ctx, def);
	ctx->parser.tokens = lexed.tokens;
	if ( ctx->parser.tokens.ptr == nullptr ) {
		return InvalidCode;
	}

	return parser_parse_enum(ctx, parser_not_inplace_def);
}

CodeBody parse_export_body( Str def )
{
	// TODO(Ed): Lift this.
	Context* ctx = _ctx;

	check_parse_args( def );

	ctx->parser = struct_zero(ParseContext);

	LexedInfo lexed = lex(ctx, def);
	ctx->parser.tokens = lexed.tokens;
	if ( ctx->parser.tokens.ptr == nullptr )
		return InvalidCode;

	return parser_parse_export_body(ctx);
}

CodeExtern parse_extern_link( Str def )
{
	// TODO(Ed): Lift this.
	Context* ctx = _ctx;

	check_parse_args( def );

	ctx->parser = struct_zero(ParseContext);

	LexedInfo lexed = lex(ctx, def);
	ctx->parser.tokens = lexed.tokens;
	if ( ctx->parser.tokens.ptr == nullptr )
		return InvalidCode;

	return parser_parse_extern_link(ctx);
}

CodeFriend parse_friend( Str def )
{
	// TODO(Ed): Lift this.
	Context* ctx = _ctx;

	check_parse_args( def );

	ctx->parser = struct_zero(ParseContext);

	LexedInfo lexed = lex(ctx, def);
	ctx->parser.tokens = lexed.tokens;
	if ( ctx->parser.tokens.ptr == nullptr )
		return InvalidCode;

	return parser_parse_friend(ctx);
}

CodeFn parse_function( Str def )
{
	// TODO(Ed): Lift this.
	Context* ctx = _ctx;

	check_parse_args( def );

	ctx->parser = struct_zero(ParseContext);

	LexedInfo lexed = lex(ctx, def);
	ctx->parser.tokens = lexed.tokens;
	if ( ctx->parser.tokens.ptr == nullptr )
		return InvalidCode;

	return (CodeFn) parser_parse_function(ctx);
}

CodeBody parse_global_body( Str def )
{
	// TODO(Ed): Lift this.
	Context* ctx = _ctx;

	check_parse_args( def );

	ctx->parser = struct_zero(ParseContext);

	LexedInfo lexed = lex(ctx, def);
	ctx->parser.tokens = lexed.tokens;
	if ( ctx->parser.tokens.ptr == nullptr )
		return InvalidCode;

	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope);
	CodeBody result = parse_global_nspace(ctx, CT_Global_Body );
	parser_pop(& ctx->parser);
	return result;
}

CodeNS parse_namespace( Str def )
{
	// TODO(Ed): Lift this.
	Context* ctx = _ctx;

	check_parse_args( def );

	ctx->parser = struct_zero(ParseContext);

	LexedInfo lexed = lex(ctx, def);
	ctx->parser.tokens = lexed.tokens;
	if ( ctx->parser.tokens.ptr == nullptr )
		return InvalidCode;

	return parser_parse_namespace(ctx);
}

CodeOperator parse_operator( Str def )
{
	// TODO(Ed): Lift this.
	Context* ctx = _ctx;

	check_parse_args( def );

	ctx->parser = struct_zero(ParseContext);

	LexedInfo lexed = lex(ctx, def);
	ctx->parser.tokens = lexed.tokens;
	if ( ctx->parser.tokens.ptr == nullptr )
		return InvalidCode;

	return (CodeOperator) parser_parse_operator(ctx);
}

CodeOpCast parse_operator_cast( Str def )
{
	// TODO(Ed): Lift this.
	Context* ctx = _ctx;

	check_parse_args( def );

	ctx->parser = struct_zero(ParseContext);

	LexedInfo lexed = lex(ctx, def);
	ctx->parser.tokens = lexed.tokens;
	if ( ctx->parser.tokens.ptr == nullptr )
		return InvalidCode;

	return parser_parse_operator_cast(ctx, NullCode);
}

CodeStruct parse_struct( Str def )
{
	// TODO(Ed): Lift this.
	Context* ctx = _ctx;

	check_parse_args( def );

	ctx->parser = struct_zero(ParseContext);

	LexedInfo lexed = lex(ctx, def);
	ctx->parser.tokens = lexed.tokens;
	if ( ctx->parser.tokens.ptr == nullptr )
		return InvalidCode;

	ParseStackNode scope = NullScope;
	parser_push(& ctx->parser, & scope);
	CodeStruct result = (CodeStruct) parse_class_struct( ctx, Tok_Decl_Struct, parser_not_inplace_def );
	parser_pop(& ctx->parser);
	return result;
}

CodeTemplate parse_template( Str def )
{
	// TODO(Ed): Lift this.
	Context* ctx = _ctx;

	check_parse_args( def );

	ctx->parser = struct_zero(ParseContext);

	LexedInfo lexed = lex(ctx, def);
	ctx->parser.tokens = lexed.tokens;
	if ( ctx->parser.tokens.ptr == nullptr )
		return InvalidCode;

	return parser_parse_template(ctx);
}

CodeTypename parse_type( Str def )
{
	// TODO(Ed): Lift this.
	Context* ctx = _ctx;

	check_parse_args( def );

	ctx->parser = struct_zero(ParseContext);

	LexedInfo lexed = lex(ctx, def);
	ctx->parser.tokens = lexed.tokens;
	if ( ctx->parser.tokens.ptr == nullptr )
		return InvalidCode;

	return parser_parse_type( ctx, parser_not_from_template, nullptr);
}

CodeTypedef parse_typedef( Str def )
{
	// TODO(Ed): Lift this.
	Context* ctx = _ctx;

	check_parse_args( def );

	ctx->parser = struct_zero(ParseContext);

	LexedInfo lexed = lex(ctx, def);
	ctx->parser.tokens = lexed.tokens;
	if ( ctx->parser.tokens.ptr == nullptr )
		return InvalidCode;

	return parser_parse_typedef(ctx);
}

CodeUnion parse_union( Str def )
{
	// TODO(Ed): Lift this.
	Context* ctx = _ctx;

	check_parse_args( def );

	ctx->parser = struct_zero(ParseContext);

	LexedInfo lexed = lex(ctx, def);
	ctx->parser.tokens = lexed.tokens;
	if ( ctx->parser.tokens.ptr == nullptr )
		return InvalidCode;

	return parser_parse_union(ctx, parser_not_inplace_def);
}

CodeUsing parse_using( Str def )
{
	// TODO(Ed): Lift this.
	Context* ctx = _ctx;

	check_parse_args( def );

	ctx->parser = struct_zero(ParseContext);

	LexedInfo lexed = lex(ctx, def);
	ctx->parser.tokens = lexed.tokens;
	if ( ctx->parser.tokens.ptr == nullptr )
		return InvalidCode;

	return parser_parse_using(ctx);
}

CodeVar parse_variable( Str def )
{
	// TODO(Ed): Lift this.
	Context* ctx = _ctx;

	check_parse_args( def );

	ctx->parser = struct_zero(ParseContext);

	LexedInfo lexed = lex(ctx, def);
	ctx->parser.tokens = lexed.tokens;
	if ( ctx->parser.tokens.ptr == nullptr )
		return InvalidCode;

	return parser_parse_variable(ctx);
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
#undef NullScope
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
