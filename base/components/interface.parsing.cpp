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
	Context* ctx = _ctx; ParseInfo info = struct_zero(ParseInfo);

	if (lexed.tokens.num == 0 && lexed.tokens.ptr == nullptr) {
		ctx->parser = struct_zero(ParseContext);
		if (check_parse_args(lexed.text) == false) {
			info.messages = ctx->parser.messages;
			goto done;
		}
		lexed = lex(ctx, lexed.text);
	}
	info.lexed = lexed;

	// TODO(Ed): ParseInfo should be set to the parser context.

	ctx->parser        = struct_zero(ParseContext);
	ctx->parser.tokens = lexed.tokens; {
		ctx->parser.lexed_messages = lexed.messages;
		ParseStackNode scope = NullScope; parser_push(& ctx->parser, & scope);
		CodeBody      result = parse_global_nspace(ctx,CT_Global_Body); (void)result;
		parser_pop(& ctx->parser);
	}
done:
	return info;
}

internal b32 parse_base_begin(Context* ctx, Str def, ParseInfo* info, char const* function_name) {
	if (ctx == nullptr || info == nullptr) GEN_FATAL("parse base: null context or info");

	info[0]      = struct_zero(ParseInfo);
	info->result = Code_Invalid;
	ctx->parser  = struct_zero(ParseContext);
	if (_check_parse_args(ctx, def, function_name) == false) {
		info->messages = ctx->parser.messages;
		return false;
	}

	info->lexed = lex(ctx, def);
	ctx->parser.lexed_messages = info->lexed.messages;
	if (info->lexed.tokens.num == 0) {
		info->messages = ctx->parser.messages;
		return false;
	}

	ctx->parser.tokens = info->lexed.tokens;
	return true;
}

void parse_class_base(Context* ctx, Str def, ParseInfo* info) {
	CodeClass result = InvalidCode;
	if (parse_base_begin(ctx, def, info, "parse_class") == false) return;
	/*Parse*/ {
		ParseStackNode scope = NullScope; parser_push(& ctx->parser, & scope);
		result = (CodeClass) parse_class_struct( ctx, Tok_Decl_Class, parser_not_inplace_def );
		parser_pop(& ctx->parser);
	}
	info->result   = cast(Code, result);
	info->messages = ctx->parser.messages;
}

CodeClass parse_class(Str def, Opts_parse opts) {
	Opts_parse resolved = get_optional(opts); Context* ctx = resolved.ctx;
	if (ctx == nullptr) ctx = _ctx; if (ctx == nullptr) GEN_FATAL("parse_class: no context");
	ParseInfo* info = resolved.info; if (info == nullptr) info = & ctx->parse_info;
	parse_class_base(ctx, def, info); return cast(CodeClass, info->result);
}

void parse_constructor_base(Context* ctx, Str def, ParseInfo* info)
{
	CodeConstructor result = InvalidCode;
	if (parse_base_begin(ctx, def, info, "parse_constructor") == false) return;
	/*Parse*/ {
		ParseStackNode scope = NullScope; parser_push(& ctx->parser, & scope);

		// TODO(Ed): Constructors can have prefix attributes
		CodeSpecifiers specifiers = NullCode;
		Specifier      specs_found[ 16 ] = { Spec_NumSpecifiers };
		s32            NumSpecifiers = 0;
		while ( left && tok_is_specifier(currtok) )
		{
			b32 ignore_spec = false;
			Specifier spec  = str_to_specifier( currtok.Text );
			switch ( spec ) {
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
				info->result   = cast(Code, result);
				info->messages = ctx->parser.messages;
				return;
			}

			// Every specifier after would be considered part of the type type signature
			if (ignore_spec) break;

			specs_found[ NumSpecifiers ] = spec;
			++ NumSpecifiers;
			eat( currtok.Type );
		}

		if ( NumSpecifiers ) {
			specifiers = def_specifiers_arr( NumSpecifiers, specs_found );
			// <specifiers> ...
		}
		result = parser_parse_constructor(ctx, specifiers);
		parser_pop(& ctx->parser);
	}
	info->result   = cast(Code, result);
	info->messages = ctx->parser.messages;
}

CodeConstructor parse_constructor(Str def, Opts_parse opts) {
	Opts_parse resolved = get_optional(opts); Context* ctx = resolved.ctx;
	if (ctx == nullptr) ctx = _ctx; if (ctx == nullptr) GEN_FATAL("parse_constructor: no context");
	 ParseInfo* info = resolved.info; if (info == nullptr) info = & ctx->parse_info;
	parse_constructor_base(ctx, def, info); return cast(CodeConstructor, info->result);
}

void parse_define_base(Context* ctx, Str def, ParseInfo* info) {
	CodeDefine result = InvalidCode;
	if (parse_base_begin(ctx, def, info, "parse_define") == false) return;
	/*Parse*/ {
		ParseStackNode scope = NullScope; parser_push(& ctx->parser, & scope);
		result = parser_parse_define(ctx);
		parser_pop(& ctx->parser);
	}
	info->result   = cast(Code, result);
	info->messages = ctx->parser.messages;
}

CodeDefine parse_define(Str def, Opts_parse opts) {
	Opts_parse resolved = get_optional(opts); Context* ctx = resolved.ctx;
	if (ctx == nullptr) ctx = _ctx; if (ctx == nullptr) GEN_FATAL("parse_define: no context");
	ParseInfo* info = resolved.info; if (info == nullptr) info = & ctx->parse_info;
	parse_define_base(ctx, def, info); return cast(CodeDefine, info->result);
}

void parse_destructor_base(Context* ctx, Str def, ParseInfo* info) {
	CodeDestructor result = InvalidCode;
	if (parse_base_begin(ctx, def, info, "parse_destructor") == false) return;

	// TODO(Ed): Destructors can have prefix attributes
	// TODO(Ed): Destructors can have virtual

	result = parser_parse_destructor(ctx, NullCode);
	info->result   = cast(Code, result);
	info->messages = ctx->parser.messages;
}

CodeDestructor parse_destructor(Str def, Opts_parse opts) {
	Opts_parse resolved = get_optional(opts); Context* ctx = resolved.ctx;
	if (ctx == nullptr) ctx = _ctx; if (ctx == nullptr) GEN_FATAL("parse_destructor: no context");
	ParseInfo* info = resolved.info; if (info == nullptr) info = & ctx->parse_info;
	parse_destructor_base(ctx, def, info); return cast(CodeDestructor, info->result);
}

void parse_enum_base(Context* ctx, Str def, ParseInfo* info) {
	CodeEnum result = InvalidCode;
	if (parse_base_begin(ctx, def, info, "parse_enum") == false) return;

	result = parser_parse_enum(ctx, parser_not_inplace_def);
	info->result   = cast(Code, result);
	info->messages = ctx->parser.messages;
}

CodeEnum parse_enum(Str def, Opts_parse opts) {
	Opts_parse resolved = get_optional(opts); Context* ctx = resolved.ctx;
	if (ctx == nullptr) ctx = _ctx; if (ctx == nullptr) GEN_FATAL("parse_enum: no context");
	ParseInfo* info = resolved.info; if (info == nullptr) info = & ctx->parse_info;
	parse_enum_base(ctx, def, info); return cast(CodeEnum, info->result);
}

void parse_export_body_base(Context* ctx, Str def, ParseInfo* info) {
	CodeBody result = InvalidCode;
	if (parse_base_begin(ctx, def, info, "parse_export_body") == false) return;

	result = parser_parse_export_body(ctx);
	info->result   = cast(Code, result);
	info->messages = ctx->parser.messages;
}

CodeBody parse_export_body(Str def, Opts_parse opts) {
	Opts_parse resolved = get_optional(opts); Context* ctx = resolved.ctx;
	if (ctx == nullptr) ctx = _ctx; if (ctx == nullptr) GEN_FATAL("parse_export_body: no context");
	ParseInfo* info = resolved.info; if (info == nullptr) info = & ctx->parse_info;
	parse_export_body_base(ctx, def, info); return cast(CodeBody, info->result);
}

void parse_extern_link_base(Context* ctx, Str def, ParseInfo* info) {
	CodeExtern result = InvalidCode;
	if (parse_base_begin(ctx, def, info, "parse_extern_link") == false) return;

	result = parser_parse_extern_link(ctx);
	info->result   = cast(Code, result);
	info->messages = ctx->parser.messages;
}

CodeExtern parse_extern_link(Str def, Opts_parse opts) {
	Opts_parse resolved = get_optional(opts); Context* ctx = resolved.ctx;
	if (ctx == nullptr) ctx = _ctx; if (ctx == nullptr) GEN_FATAL("parse_extern_link: no context");
	ParseInfo* info = resolved.info; if (info == nullptr) info = & ctx->parse_info;
	parse_extern_link_base(ctx, def, info); return cast(CodeExtern, info->result);
}

void parse_friend_base(Context* ctx, Str def, ParseInfo* info) {
	CodeFriend result = InvalidCode;
	if (parse_base_begin(ctx, def, info, "parse_friend") == false) return;

	result = parser_parse_friend(ctx);
	info->result   = cast(Code, result);
	info->messages = ctx->parser.messages;
}

CodeFriend parse_friend(Str def, Opts_parse opts) {
	Opts_parse resolved = get_optional(opts); Context* ctx = resolved.ctx;
	if (ctx == nullptr) ctx = _ctx; if (ctx == nullptr) GEN_FATAL("parse_friend: no context");
	ParseInfo* info = resolved.info; if (info == nullptr) info = & ctx->parse_info;
	parse_friend_base(ctx, def, info); return cast(CodeFriend, info->result);
}

void parse_function_base(Context* ctx, Str def, ParseInfo* info) {
	CodeFn result = InvalidCode;
	if (parse_base_begin(ctx, def, info, "parse_function") == false) return;

	result = (CodeFn) parser_parse_function(ctx);
	info->result   = cast(Code, result);
	info->messages = ctx->parser.messages;
}

CodeFn parse_function(Str def, Opts_parse opts) {
	Opts_parse resolved = get_optional(opts);
	Context* ctx = resolved.ctx;
	if (ctx == nullptr) ctx = _ctx; if (ctx == nullptr) GEN_FATAL("parse_function: no context");
	ParseInfo* info = resolved.info; if (info == nullptr) info = & ctx->parse_info;
	parse_function_base(ctx, def, info); return cast(CodeFn, info->result);
}

void parse_global_body_base( Context* ctx, Str def, ParseInfo* info )
{
	LexedInfo lexed; CodeBody  body;
	if ( ctx == nullptr || info == nullptr ) GEN_FATAL( "parse_global_body_base: null context or info" );

	info[0] = struct_zero( ParseInfo );
	lexed   = struct_zero( LexedInfo );
	body    = (CodeBody) make_code();
	body->Type = CT_Global_Body;

	ctx->parser = struct_zero( ParseContext );
	if ( check_parse_args( def ) == false ) {
		info->result   = cast( Code, body );
		info->messages = ctx->parser.messages;
		return;
	}

	lexed       = lex( ctx, def );
	info->lexed = lexed;
	ctx->parser.lexed_messages = lexed.messages;
	if ( lexed.tokens.num == 0 ) {
		info->result   = cast( Code, body );
		info->messages = ctx->parser.messages;
		return;
	}

	ctx->parser.tokens = lexed.tokens; {
		ParseStackNode scope = NullScope;
		parser_push( & ctx->parser, & scope );
		body = parse_global_nspace( ctx, CT_Global_Body );
		parser_pop( & ctx->parser );
	}
	info->result   = cast( Code, body );
	info->messages = ctx->parser.messages;
}

CodeBody parse_global_body( Str def, Opts_parse opts ) {
	Opts_parse resolved = get_optional(opts); Context* ctx = resolved.ctx;
	if ( ctx == nullptr ) ctx = _ctx; if ( ctx == nullptr ) GEN_FATAL( "parse_global_body: no context" );
	ParseInfo* info = resolved.info; if ( info == nullptr ) info = & ctx->parse_info;
	parse_global_body_base( ctx, def, info ); return cast( CodeBody, info->result );
}

void parse_namespace_base(Context* ctx, Str def, ParseInfo* info) {
	CodeNS result = InvalidCode;
	if (parse_base_begin(ctx, def, info, "parse_namespace") == false) return;

	result = parser_parse_namespace(ctx);
	info->result   = cast(Code, result);
	info->messages = ctx->parser.messages;
}

CodeNS parse_namespace(Str def, Opts_parse opts) {
	Opts_parse resolved = get_optional(opts); Context* ctx = resolved.ctx;
	if (ctx == nullptr) ctx = _ctx; if (ctx == nullptr) GEN_FATAL("parse_namespace: no context");
	ParseInfo* info = resolved.info; if (info == nullptr) info = &ctx->parse_info;
	parse_namespace_base(ctx, def, info); return cast(CodeNS, info->result);
}

void parse_operator_base(Context* ctx, Str def, ParseInfo* info) {
	CodeOperator result = InvalidCode;
	if (parse_base_begin(ctx, def, info, "parse_operator") == false) return;

	result = (CodeOperator) parser_parse_operator(ctx);
	info->result   = cast(Code, result);
	info->messages = ctx->parser.messages;
}

CodeOperator parse_operator(Str def, Opts_parse opts) {
	Opts_parse resolved = get_optional(opts); Context* ctx = resolved.ctx; ParseInfo* info;
	if (ctx == nullptr) ctx = _ctx; if (ctx == nullptr) GEN_FATAL("parse_operator: no context");
	info = resolved.info; if (info == nullptr) info = &ctx->parse_info;
	parse_operator_base(ctx, def, info); return cast(CodeOperator, info->result);
}

void parse_operator_cast_base(Context* ctx, Str def, ParseInfo* info) {
	CodeOpCast result = InvalidCode;
	if (parse_base_begin(ctx, def, info, "parse_operator_cast") == false) return;

	result = parser_parse_operator_cast(ctx, NullCode);
	info->result   = cast(Code, result);
	info->messages = ctx->parser.messages;
}

CodeOpCast parse_operator_cast(Str def, Opts_parse opts) {
	Opts_parse resolved = get_optional(opts); Context* ctx = resolved.ctx; ParseInfo* info;
	if (ctx == nullptr) ctx = _ctx; if (ctx == nullptr) GEN_FATAL("parse_operator_cast: no context");
	info = resolved.info; if (info == nullptr) info = &ctx->parse_info;
	parse_operator_cast_base(ctx, def, info); return cast(CodeOpCast, info->result);
}

void parse_struct_base(Context* ctx, Str def, ParseInfo* info) {
	CodeStruct result = InvalidCode;
	if (parse_base_begin(ctx, def, info, "parse_struct") == false) return;
	/*Parse*/ {
		ParseStackNode scope = NullScope;
		parser_push(& ctx->parser, & scope);
		result = (CodeStruct) parse_class_struct( ctx, Tok_Decl_Struct, parser_not_inplace_def );
		parser_pop(& ctx->parser);
	}
	info->result   = cast(Code, result);
	info->messages = ctx->parser.messages;
}

CodeStruct parse_struct(Str def, Opts_parse opts) {
	Opts_parse resolved = get_optional(opts); Context* ctx = resolved.ctx; ParseInfo* info;
	if (ctx == nullptr) ctx = _ctx; if (ctx == nullptr) GEN_FATAL("parse_struct: no context");
	info = resolved.info; if (info == nullptr) info = &ctx->parse_info;
	parse_struct_base(ctx, def, info); return cast(CodeStruct, info->result);
}

void parse_template_base(Context* ctx, Str def, ParseInfo* info) {
	CodeTemplate result = InvalidCode;
	if (parse_base_begin(ctx, def, info, "parse_template") == false) return;

	result = parser_parse_template(ctx);
	info->result   = cast(Code, result);
	info->messages = ctx->parser.messages;
}

CodeTemplate parse_template(Str def, Opts_parse opts) {
	Opts_parse resolved = get_optional(opts); Context* ctx = resolved.ctx; ParseInfo* info;
	if (ctx == nullptr) ctx = _ctx; if (ctx == nullptr) GEN_FATAL("parse_template: no context");
	info = resolved.info; if (info == nullptr) info = &ctx->parse_info;
	parse_template_base(ctx, def, info); return cast(CodeTemplate, info->result);
}

void parse_type_base(Context* ctx, Str def, ParseInfo* info) {
	CodeTypename result = InvalidCode;
	if (parse_base_begin(ctx, def, info, "parse_type") == false) return;

	result = parser_parse_type(ctx, parser_not_from_template, nullptr);
	info->result   = cast(Code, result);
	info->messages = ctx->parser.messages;
}

CodeTypename parse_type(Str def, Opts_parse opts) {
	Opts_parse resolved = get_optional(opts); Context* ctx = resolved.ctx; ParseInfo* info;
	if (ctx == nullptr) ctx = _ctx; if (ctx == nullptr) GEN_FATAL("parse_type: no context");
	info = resolved.info; if (info == nullptr) info = &ctx->parse_info;
	parse_type_base(ctx, def, info); return cast(CodeTypename, info->result);
}

void parse_typedef_base(Context* ctx, Str def, ParseInfo* info) {
	CodeTypedef result = InvalidCode;
	if (parse_base_begin(ctx, def, info, "parse_typedef") == false) return;

	result = parser_parse_typedef(ctx);
	info->result   = cast(Code, result);
	info->messages = ctx->parser.messages;
}

CodeTypedef parse_typedef(Str def, Opts_parse opts) {
	Opts_parse resolved = get_optional(opts); Context* ctx = resolved.ctx; ParseInfo* info;
	if (ctx == nullptr) ctx = _ctx; if (ctx == nullptr) GEN_FATAL("parse_typedef: no context");
	info = resolved.info; if (info == nullptr) info = &ctx->parse_info;
	parse_typedef_base(ctx, def, info); return cast(CodeTypedef, info->result);
}

void parse_union_base(Context* ctx, Str def, ParseInfo* info) {
	CodeUnion result = InvalidCode;
	if (parse_base_begin(ctx, def, info, "parse_union") == false) return;

	result = parser_parse_union(ctx, parser_not_inplace_def);
	info->result   = cast(Code, result);
	info->messages = ctx->parser.messages;
}

CodeUnion parse_union(Str def, Opts_parse opts) {
	Opts_parse resolved = get_optional(opts); Context* ctx = resolved.ctx; ParseInfo* info;
	if (ctx == nullptr) ctx = _ctx; if (ctx == nullptr) GEN_FATAL("parse_union: no context");
	info = resolved.info; if (info == nullptr) info = &ctx->parse_info;
	parse_union_base(ctx, def, info); return cast(CodeUnion, info->result);
}

void parse_using_base(Context* ctx, Str def, ParseInfo* info) {
	CodeUsing result = InvalidCode;
	if (parse_base_begin(ctx, def, info, "parse_using") == false) return;

	result = parser_parse_using(ctx);
	info->result   = cast(Code, result);
	info->messages = ctx->parser.messages;
}

CodeUsing parse_using(Str def, Opts_parse opts) {
	Opts_parse resolved = get_optional(opts); Context* ctx = resolved.ctx; ParseInfo* info;
	if (ctx == nullptr) ctx = _ctx; if (ctx == nullptr) GEN_FATAL("parse_using: no context");
	info = resolved.info; if (info == nullptr) info = &ctx->parse_info;
	parse_using_base(ctx, def, info); return cast(CodeUsing, info->result);
}

void parse_variable_base(Context* ctx, Str def, ParseInfo* info) {
	CodeVar result = InvalidCode;
	if (parse_base_begin(ctx, def, info, "parse_variable") == false) return;
	if (parser_slice_is_formatting_only(& ctx->parser)) {
		parser_record_failure(ctx, txt("parse_variable: only formatting tokens"));
	}
	else {
		result = parser_parse_variable(ctx);
	}
	info->result   = cast(Code, result);
	info->messages = ctx->parser.messages;
}

CodeVar parse_variable(Str def, Opts_parse opts) {
	Opts_parse resolved = get_optional(opts); Context* ctx = resolved.ctx; ParseInfo* info;
	if (ctx == nullptr) ctx = _ctx; if (ctx == nullptr) GEN_FATAL("parse_variable: no context");
	info = resolved.info; if (info == nullptr) info = &ctx->parse_info;
	parse_variable_base(ctx, def, info); return cast(CodeVar, info->result);
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
