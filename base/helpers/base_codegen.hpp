#pragma once

#if GEN_INTELLISENSE_DIRECTIVES
#	include "../gen.hpp"
#	include "misc.hpp"

using namespace gen;
#endif

CodeBody gen_ecode( char const* path, bool use_c_definition = false )
{
	FixedArena_32KB scratch;       fixed_arena_init(& scratch);
	AllocatorInfo   scratch_info = fixed_arena_allocator_info(& scratch);

	CSV_Columns2 csv_enum                 = parse_csv_two_columns( scratch_info, path );
	StrBuilder   enum_entries             = strbuilder_make_reserve( GlobalAllocator, kilobytes(1) );
	StrBuilder   to_c_str_entries         = strbuilder_make_reserve( GlobalAllocator, kilobytes(1) );
	StrBuilder   to_keyword_c_str_entries = strbuilder_make_reserve( GlobalAllocator, kilobytes(1) );

	for ( ssize idx = 0; idx < array_num(csv_enum.Col_1); ++ idx ) 	{
		char const* code    = csv_enum.Col_1[idx].string;
		char const* keyword = csv_enum.Col_2[idx].string;
		// TODO(Ed): to_c_str_entries and the others in here didn't have proper sizing of the Str slice.
		strbuilder_append_fmt( & enum_entries,             "CT_%s,\n", code );
		strbuilder_append_fmt( & to_c_str_entries,         "{ \"%s\",  sizeof(\"%s\") - 1 },\n", code,    code );
		strbuilder_append_fmt( & to_keyword_c_str_entries, "{  \"%s\", sizeof(\"%s\") - 1 },\n", keyword, keyword );
	}

	CodeEnum enum_code;
	if (use_c_definition) {
		enum_code = parse_enum(token_fmt_impl((3 + 1) / 2, "entries", strbuilder_to_str(enum_entries),
			"enum CodeType enum_underlying(u32) { <entries> CT_NumTypes, CT_UnderlyingType = GEN_U32_MAX };"
		));
	}
	else {
		enum_code = parse_enum(token_fmt_impl((3 + 1) / 2, "entries", strbuilder_to_str(enum_entries),
			"enum CodeType : u32 { <entries> CT_NumTypes, CT_UnderlyingType = GEN_U32_MAX };"
		));
	}

#pragma push_macro("local_persist")
#undef local_persist
	Str      lookup_size  = strbuilder_to_str(strbuilder_fmt_buf(GlobalAllocator, "%d", array_num(csv_enum.Col_1) ));
	CodeBody to_c_str_fns = parse_global_body( token_fmt(
		"entries",  strbuilder_to_str(to_c_str_entries)
	,	"keywords", strbuilder_to_str(to_keyword_c_str_entries)
	,	"num",      lookup_size
	, stringize(
		inline
		Str codetype_to_str( CodeType type )
		{
			local_persist
			Str lookup[<num>] = {
				<entries>
			};
			return lookup[ type ];
		}

		inline
		Str codetype_to_keyword_str( CodeType type )
		{
			local_persist
			Str lookup[ <num> ] = {
				<keywords>
			};
			return lookup[ type ];
		}
	)));
#pragma pop_macro("local_persist")

	CodeBody result = def_body(CT_Global_Body);
	body_append(result, enum_code);

	if (use_c_definition) {
		CodeTypedef code_t = parse_typedef(code(typedef enum CodeType CodeType; ));
		body_append(result, code_t);
	}

	body_append(result, to_c_str_fns);

	if (! use_c_definition) {
		#pragma push_macro("forceinline")
		#undef forceinline
		CodeBody alias_mappings = parse_global_body(code(
			forceinline Str to_str        (CodeType type) { return codetype_to_str(type);         }
			forceinline Str to_keyword_str(CodeType type) { return codetype_to_keyword_str(type); }
		));
		#pragma pop_macro("forceinline")
		body_append(result, alias_mappings);
	}
	return result;
}

CodeBody gen_eoperator( char const* path, bool use_c_definition = false )
{
	FixedArena_16KB scratch;       fixed_arena_init(& scratch);
	AllocatorInfo   scratch_info = fixed_arena_allocator_info(& scratch);

	CSV_Columns2 csv_enum       = parse_csv_two_columns( scratch_info, path );
	StrBuilder enum_entries     = strbuilder_make_reserve( GlobalAllocator, 32 );
	StrBuilder to_c_str_entries = strbuilder_make_reserve( GlobalAllocator, 32 );

	for (usize idx = 0; idx < array_num(csv_enum.Col_1); idx++) {
		char const* enum_str     = csv_enum.Col_1[idx].string;
		char const* entry_to_str = csv_enum.Col_2[idx].string;
		strbuilder_append_fmt( & enum_entries,     "Op_%s,\n", enum_str );
		strbuilder_append_fmt( & to_c_str_entries, "{ \"%s\", sizeof(\"%s\") - 1 },\n", entry_to_str, entry_to_str);
	}

	CodeEnum  enum_code;
	if (use_c_definition)
	{
	#pragma push_macro("enum_underlying")
	#undef enum_underlying
		enum_code = parse_enum(token_fmt("entries", strbuilder_to_str(enum_entries), stringize(
			enum Operator enum_underlying(u32)
			{
				<entries>
				Op_NumOps,
				Op_UnderlyingType = GEN_U32_MAX
			};
		)));
	#pragma pop_macro("enum_underlying")
	}
	else
	{
		enum_code = parse_enum(token_fmt("entries", strbuilder_to_str(enum_entries), stringize(
			enum Operator : u32
			{
				<entries>
				Op_NumOps,
				Op_UnderlyingType = GEN_U32_MAX
			};
		)));
	}

#pragma push_macro("local_persist")
#undef local_persist
	Str lookup_size = strbuilder_to_str(strbuilder_fmt_buf(GlobalAllocator, "%d", array_num(csv_enum.Col_1) ));
	CodeFn to_str   = parse_function(token_fmt(
		"entries", strbuilder_to_str(to_c_str_entries)
	,	"num",     lookup_size
	, stringize(
		inline
		Str operator_to_str( Operator op )
		{
			local_persist
			Str lookup[<num>] = {
				<entries>
			};

			return lookup[ op ];
		}
	)));
#pragma pop_macro("local_persist")

	CodeBody result = def_body(CT_Global_Body);
	body_append(result, enum_code);
	if ( use_c_definition ) {
		CodeTypedef operator_t = parse_typedef(code( typedef enum Operator Operator; ));
		body_append(result, operator_t);
	}

	body_append(result, to_str);

	if (! use_c_definition)
	{
	#pragma push_macro("forceinline")
	#undef forceinline
		CodeBody alias_mappings = parse_global_body(code(
			forceinline Str to_str(Operator op) { return operator_to_str(op); }
		));
	#pragma pop_macro("forceinline")
		body_append(result, alias_mappings);
	}
	return result;
}

CodeBody gen_especifier( char const* path, bool use_c_definition = false )
{
	FixedArena_16KB scratch;       fixed_arena_init(& scratch);
	AllocatorInfo   scratch_info = fixed_arena_allocator_info(& scratch);

	CSV_Columns2 csv_enum       = parse_csv_two_columns(   scratch_info, path );
	StrBuilder enum_entries     = strbuilder_make_reserve( scratch_info, kilobytes(1) );
	StrBuilder to_c_str_entries = strbuilder_make_reserve( scratch_info, kilobytes(1) );

	for (usize idx = 0; idx < array_num(csv_enum.Col_1); idx++)
	{
		char const* enum_str     = csv_enum.Col_1[idx].string;
		char const* entry_to_str = csv_enum.Col_2[idx].string;
		strbuilder_append_fmt( & enum_entries,     "Spec_%s,\n", enum_str );
		strbuilder_append_fmt( & to_c_str_entries, "{ \"%s\", sizeof(\"%s\") - 1 },\n", entry_to_str, entry_to_str);
	}

	CodeEnum enum_code;
	if (use_c_definition)
	{
	#pragma push_macro("enum_underlying")
	#undef enum_underlying
		enum_code = parse_enum(token_fmt("entries", strbuilder_to_str(enum_entries), stringize(
			enum Specifier enum_underlying(u32)
			{
				<entries>
				Spec_NumSpecifiers,
				Spec_UnderlyingType = GEN_U32_MAX
			};
		)));
	#pragma pop_macro("enum_underlying")
	}
	else
	{
		enum_code = parse_enum(token_fmt("entries", strbuilder_to_str(enum_entries), stringize(
			enum Specifier : u32
			{
				<entries>
				Spec_NumSpecifiers,
				Spec_UnderlyingType = GEN_U32_MAX
			};
		)));
	}
	CodeFn is_trailing = parse_function(token_fmt("specifier", strbuilder_to_str(to_c_str_entries), stringize(
		inline
		bool spec_is_trailing( Specifier specifier )
		{
			return specifier > Spec_Virtual;
		}
	)));

#pragma push_macro("local_persist")
#pragma push_macro("do_once_start")
#pragma push_macro("do_once_end")
#pragma push_macro("forceinline")
#pragma push_macro("neverinline")
#undef local_persist
#undef do_once_start
#undef do_once_end
#undef forceinline
#undef neverinline
	Str lookup_size = strbuilder_to_str(strbuilder_fmt_buf(GlobalAllocator, "%d", array_num(csv_enum.Col_1) ));
	CodeFn to_str   = parse_function(token_fmt(
		"entries", strbuilder_to_str(to_c_str_entries)
	,	"num",     lookup_size
	, stringize(
		inline
		Str spec_to_str( Specifier type )
		{
			local_persist
			Str lookup[<num>] = {
				<entries>
			};

			return lookup[ type ];
		}
	)));

	CodeFn to_type = parse_function( token_fmt( "entries", strbuilder_to_str(to_c_str_entries), stringize(
		inline
		Specifier str_to_specifier( Str str )
		{
			local_persist
			u32 keymap[ Spec_NumSpecifiers ];
			do_once_start
				for ( u32 index = 0; index < Spec_NumSpecifiers; index++ )
				{
					Str enum_str = spec_to_str( (Specifier)index );

					// We subtract 1 to remove the null terminator
					// This is because the tokens lexed are not null terminated.
					keymap[index] = crc32( enum_str.Ptr, enum_str.Len  );
				}
			do_once_end

			u32 hash = crc32( str.Ptr, str.Len );

			for ( u32 index = 0; index < Spec_NumSpecifiers; index++ )
			{
				if ( keymap[index] == hash )
					return (Specifier)index;
			}

			return Spec_Invalid;
		}
	)));
#pragma pop_macro("local_persist")
#pragma pop_macro("do_once_start")
#pragma pop_macro("do_once_end")
#pragma pop_macro("forceinline")
#pragma pop_macro("neverinline")

	CodeBody result = def_body(CT_Global_Body);
	body_append(result, enum_code);
	if (use_c_definition)
	{
		CodeTypedef specifier_t = parse_typedef( code(typedef u32 Specifier; ));
		body_append(result, specifier_t);
	}

	body_append(result, to_str);
	body_append(result, is_trailing);
	body_append(result, to_type);

	if (! use_c_definition)
	{
	#pragma push_macro("forceinline")
	#undef forceinline
		CodeBody alias_mappings = parse_global_body(code(
			forceinline Str      to_str (Specifier spec)            { return spec_to_str(spec); }
			forceinline Specifier to_type( Str str )                { return str_to_specifier(str); }
			forceinline bool      is_trailing( Specifier specifier ) { return spec_is_trailing(specifier); }
		));
	#pragma pop_macro("forceinline")
		body_append(result, alias_mappings);
	}
	return result;
}

CodeBody gen_etoktype( char const* etok_path, char const* attr_path, bool use_c_definition = false )
{
	FixedArena_64KB scratch;       fixed_arena_init(& scratch);
	AllocatorInfo   scratch_info = fixed_arena_allocator_info(& scratch);

	FileContents enum_content = file_read_contents( scratch_info, file_zero_terminate, etok_path );

	CSV_Object csv_enum_nodes;
	csv_parse( &csv_enum_nodes, rcast(char*, enum_content.data), scratch_info, false );

	FileContents attrib_content = file_read_contents( scratch_info, file_zero_terminate, attr_path );

	CSV_Object csv_attr_nodes;
	csv_parse( &csv_attr_nodes, rcast(char*, attrib_content.data), scratch_info, false );

	Array<ADT_Node> enum_strs            = csv_enum_nodes.nodes[0].nodes;
	Array<ADT_Node> enum_c_str_strs      = csv_enum_nodes.nodes[1].nodes;
	Array<ADT_Node> attribute_strs       = csv_attr_nodes.nodes[0].nodes;
	Array<ADT_Node> attribute_c_str_strs = csv_attr_nodes.nodes[1].nodes;

	StrBuilder enum_entries             = strbuilder_make_reserve( scratch_info, kilobytes(2) );
	StrBuilder to_c_str_entries         = strbuilder_make_reserve( scratch_info, kilobytes(4) );
	StrBuilder attribute_entries        = strbuilder_make_reserve( scratch_info, kilobytes(2) );
	StrBuilder to_c_str_attributes      = strbuilder_make_reserve( scratch_info, kilobytes(4) );
	StrBuilder attribute_define_entries = strbuilder_make_reserve( scratch_info, kilobytes(4) );

	for (usize idx = 0; idx < array_num(enum_strs); idx++)
	{
		char const* enum_str     = enum_strs[idx].string;
		char const* entry_to_str = enum_c_str_strs [idx].string;

		strbuilder_append_fmt( & enum_entries,     "Tok_%s,\n",                         enum_str );
		strbuilder_append_fmt( & to_c_str_entries, "{ \"%s\", sizeof(\"%s\") - 1 },\n", entry_to_str, entry_to_str);
	}

	for ( usize idx = 0; idx < array_num(attribute_strs); idx++ )
	{
		char const* attribute_str = attribute_strs[idx].string;
		char const* entry_to_str  = attribute_c_str_strs [idx].string;

		strbuilder_append_fmt( & attribute_entries,        "Tok_Attribute_%s,\n",               attribute_str );
		strbuilder_append_fmt( & to_c_str_attributes,      "{ \"%s\", sizeof(\"%s\") - 1 },\n", entry_to_str, entry_to_str);
		strbuilder_append_fmt( & attribute_define_entries, "Entry( Tok_Attribute_%s, \"%s\" )", attribute_str, entry_to_str );

		if ( idx < array_num(attribute_strs) - 1 )
			strbuilder_append_str( & attribute_define_entries, txt(" \\\n"));
		else
			strbuilder_append_str( & attribute_define_entries, txt("\n"));
	}

#pragma push_macro("GEN_DEFINE_ATTRIBUTE_TOKENS")
#undef GEN_DEFINE_ATTRIBUTE_TOKENS
	CodeDefine attribute_entires_def = def_define( name(GEN_DEFINE_ATTRIBUTE_TOKENS), strbuilder_to_str(attribute_define_entries)  );
#pragma pop_macro("GEN_DEFINE_ATTRIBUTE_TOKENS")

	// We cannot parse this enum, it has Attribute names as enums
	CodeEnum enum_code;
	if (use_c_definition)
	{
		enum_code = parse_enum(token_fmt("entries", strbuilder_to_str(enum_entries), "attribute_toks", strbuilder_to_str(attribute_entries), stringize(
			enum TokType
			{
				<entries>
				<attribute_toks>
				Tok_NumTokens,
				Tok_UnderlyingType = GEN_U32_MAX
			};
		)));
	}
	else
	{
		enum_code = parse_enum(token_fmt("entries", strbuilder_to_str(enum_entries), "attribute_toks", strbuilder_to_str(attribute_entries), stringize(
			enum TokType : u32
			{
				<entries>
				<attribute_toks>
				Tok_NumTokens
			};
		)));
	}

#pragma push_macro("local_persist")
#pragma push_macro("do_once_start")
#pragma push_macro("do_once_end")
#undef local_persist
#undef do_once_start
#undef do_once_end
	CodeFn to_str = parse_function(token_fmt("entries", strbuilder_to_str(to_c_str_entries), "attribute_toks", strbuilder_to_str(to_c_str_attributes), stringize(
		inline
		Str toktype_to_str( TokType type )
		{
			local_persist
			Str lookup[] = {
				<entries>
				<attribute_toks>
			};

			return lookup[ type ];
		}
	)));

	CodeFn to_type = parse_function( token_fmt( "entries", strbuilder_to_str(to_c_str_entries), stringize(
		inline
		TokType str_to_toktype( Str str )
		{
			local_persist
			u32 keymap[ Tok_NumTokens ];
			do_once_start
				for ( u32 index = 0; index < Tok_NumTokens; index++ )
				{
					Str enum_str = toktype_to_str( (TokType)index );

					// We subtract 1 to remove the null terminator
					// This is because the tokens lexed are not null terminated.
					keymap[index] = crc32( enum_str.Ptr, enum_str.Len);
				}
			do_once_end

			u32 hash = crc32( str.Ptr, str.Len );

			for ( u32 index = 0; index < Tok_NumTokens; index++ )
			{
				if ( keymap[index] == hash )
					return (TokType)index;
			}

			return Tok_Invalid;
		}
	)));
#pragma pop_macro("local_persist")
#pragma pop_macro("do_once_start")
#pragma pop_macro("do_once_end")

	CodeBody result = def_body(CT_Global_Body);
	body_append(result, untyped_str(txt("GEN_NS_PARSER_BEGIN\n\n")));
	body_append(result, attribute_entires_def);
	body_append(result, enum_code);
	if (use_c_definition)
	{
		CodeTypedef td_toktype = parse_typedef( code( typedef enum TokType TokType; ));
		body_append(result, td_toktype);
	}
	body_append(result, to_str);
	body_append(result, to_type);
	body_append(result, untyped_str(txt("\nGEN_NS_PARSER_END\n\n")));
	return result;
}

CodeBody gen_ast_inlines()
{
#pragma push_macro("GEN_NS")
#pragma push_macro("rcast")
#pragma push_macro("log_failure")
#pragma push_macro("CodeInvalid")
#undef GEN_NS
#undef rcast
#undef log_failure
#undef CodeInvalid
	char const* code_impl_tmpl = stringize(
		\n
		inline
		<typename>& <typename>::operator =( Code other )
		{
			if ( other.ast != nullptr && other->Parent != nullptr )
			{
				ast         = rcast( decltype(ast), code_duplicate(other).ast);
				ast->Parent = { nullptr };
			}

			ast = rcast( decltype( ast ), other.ast );
			return * this;
		}
		inline
		<typename>::operator bool()
		{
			return ast != nullptr;
		}
	);

	char const* codetype_impl_tmpl = stringize(
		inline
		Code<typename>::operator Code()
		{
			return *rcast( Code*, this );
		}
		inline
		AST_<typename>* Code<typename>::operator->()
		{
			if ( ast == nullptr )
			{
				log_failure( "Attempt to dereference a nullptr!\n" );
				return nullptr;
			}
			return ast;
		}
		\n
	);
#pragma pop_macro("GEN_NS")
#pragma pop_macro("CodeInvalid")

	CodeBody impl_code          = parse_global_body( token_fmt( "typename", Str name(Code),               code_impl_tmpl ));
	CodeBody impl_code_body     = parse_global_body( token_fmt( "typename", Str name(CodeBody),           code_impl_tmpl ));
	CodeBody impl_code_attr     = parse_global_body( token_fmt( "typename", Str name(CodeAttributes),     code_impl_tmpl ));
	CodeBody impl_code_cmt      = parse_global_body( token_fmt( "typename", Str name(CodeComment),        code_impl_tmpl ));
	CodeBody impl_code_constr   = parse_global_body( token_fmt( "typename", Str name(CodeConstructor),    code_impl_tmpl ));
	CodeBody impl_code_class    = parse_global_body( token_fmt( "typename", Str name(CodeClass),          code_impl_tmpl ));
	CodeBody impl_code_define   = parse_global_body( token_fmt( "typename", Str name(CodeDefine),         code_impl_tmpl ));
	CodeBody impl_code_destruct = parse_global_body( token_fmt( "typename", Str name(CodeDestructor),     code_impl_tmpl ));
	CodeBody impl_code_enum     = parse_global_body( token_fmt( "typename", Str name(CodeEnum),           code_impl_tmpl ));
	CodeBody impl_code_exec     = parse_global_body( token_fmt( "typename", Str name(CodeExec),           code_impl_tmpl ));
	CodeBody impl_code_extern   = parse_global_body( token_fmt( "typename", Str name(CodeExtern),         code_impl_tmpl ));
	CodeBody impl_code_include  = parse_global_body( token_fmt( "typename", Str name(CodeInclude),        code_impl_tmpl ));
	CodeBody impl_code_friend   = parse_global_body( token_fmt( "typename", Str name(CodeFriend),         code_impl_tmpl ));
	CodeBody impl_code_fn	    = parse_global_body( token_fmt( "typename", Str name(CodeFn),             code_impl_tmpl ));
	CodeBody impl_code_module   = parse_global_body( token_fmt( "typename", Str name(CodeModule),         code_impl_tmpl ));
	CodeBody impl_code_ns       = parse_global_body( token_fmt( "typename", Str name(CodeNS),             code_impl_tmpl ));
	CodeBody impl_code_op       = parse_global_body( token_fmt( "typename", Str name(CodeOperator),       code_impl_tmpl ));
	CodeBody impl_code_opcast   = parse_global_body( token_fmt( "typename", Str name(CodeOpCast),         code_impl_tmpl ));
	CodeBody impl_code_params   = parse_global_body( token_fmt( "typename", Str name(CodeParams),         code_impl_tmpl ));
	CodeBody impl_code_pragma   = parse_global_body( token_fmt( "typename", Str name(CodePragma),         code_impl_tmpl ));
	CodeBody impl_code_precond  = parse_global_body( token_fmt( "typename", Str name(CodePreprocessCond), code_impl_tmpl ));
	CodeBody impl_code_specs    = parse_global_body( token_fmt( "typename", Str name(CodeSpecifiers),     code_impl_tmpl ));
	CodeBody impl_code_struct   = parse_global_body( token_fmt( "typename", Str name(CodeStruct),         code_impl_tmpl ));
	CodeBody impl_code_tmpl     = parse_global_body( token_fmt( "typename", Str name(CodeTemplate),       code_impl_tmpl ));
	CodeBody impl_code_type     = parse_global_body( token_fmt( "typename", Str name(CodeTypename),       code_impl_tmpl ));
	CodeBody impl_code_typedef  = parse_global_body( token_fmt( "typename", Str name(CodeTypedef),        code_impl_tmpl ));
	CodeBody impl_code_union    = parse_global_body( token_fmt( "typename", Str name(CodeUnion),          code_impl_tmpl ));
	CodeBody impl_code_using    = parse_global_body( token_fmt( "typename", Str name(CodeUsing),          code_impl_tmpl ));
	CodeBody impl_code_var      = parse_global_body( token_fmt( "typename", Str name(CodeVar),            code_impl_tmpl ));

	body_append(impl_code_attr,     parse_global_body( token_fmt( "typename", Str name(Attributes),     codetype_impl_tmpl )));
	body_append(impl_code_cmt,      parse_global_body( token_fmt( "typename", Str name(Comment),        codetype_impl_tmpl )));
	body_append(impl_code_constr,   parse_global_body( token_fmt( "typename", Str name(Constructor),    codetype_impl_tmpl )));
	body_append(impl_code_define,   parse_global_body( token_fmt( "typename", Str name(Define),         codetype_impl_tmpl )));
	body_append(impl_code_destruct, parse_global_body( token_fmt( "typename", Str name(Destructor),     codetype_impl_tmpl )));
	body_append(impl_code_enum,     parse_global_body( token_fmt( "typename", Str name(Enum),           codetype_impl_tmpl )));
	body_append(impl_code_exec,     parse_global_body( token_fmt( "typename", Str name(Exec),           codetype_impl_tmpl )));
	body_append(impl_code_extern,   parse_global_body( token_fmt( "typename", Str name(Extern),         codetype_impl_tmpl )));
	body_append(impl_code_include,  parse_global_body( token_fmt( "typename", Str name(Include),        codetype_impl_tmpl )));
	body_append(impl_code_friend,   parse_global_body( token_fmt( "typename", Str name(Friend),         codetype_impl_tmpl )));
	body_append(impl_code_fn,       parse_global_body( token_fmt( "typename", Str name(Fn),             codetype_impl_tmpl )));
	body_append(impl_code_module,   parse_global_body( token_fmt( "typename", Str name(Module),         codetype_impl_tmpl )));
	body_append(impl_code_ns,       parse_global_body( token_fmt( "typename", Str name(NS),             codetype_impl_tmpl )));
	body_append(impl_code_op,       parse_global_body( token_fmt( "typename", Str name(Operator),       codetype_impl_tmpl )));
	body_append(impl_code_opcast,   parse_global_body( token_fmt( "typename", Str name(OpCast),         codetype_impl_tmpl )));
	body_append(impl_code_pragma,   parse_global_body( token_fmt( "typename", Str name(Pragma),         codetype_impl_tmpl )));
	body_append(impl_code_precond,  parse_global_body( token_fmt( "typename", Str name(PreprocessCond), codetype_impl_tmpl )));
	body_append(impl_code_tmpl,     parse_global_body( token_fmt( "typename", Str name(Template),       codetype_impl_tmpl )));
	body_append(impl_code_type,     parse_global_body( token_fmt( "typename", Str name(Typename),       codetype_impl_tmpl )));
	body_append(impl_code_typedef,  parse_global_body( token_fmt( "typename", Str name(Typedef),        codetype_impl_tmpl )));
	body_append(impl_code_union,    parse_global_body( token_fmt( "typename", Str name(Union),          codetype_impl_tmpl )));
	body_append(impl_code_using,    parse_global_body( token_fmt( "typename", Str name(Using),          codetype_impl_tmpl )));
	body_append(impl_code_var,      parse_global_body( token_fmt( "typename", Str name(Var),            codetype_impl_tmpl )));

	#pragma push_macro("forceinline")
	#undef forceinline
	char const* cast_tmpl = stringize(
		forceinline Code::operator Code<typename>() const
		{
			return { (AST_<typename>*) ast };
		}
	);
	#pragma pop_macro("forceinline")

	CodeBody impl_cast_body      = parse_global_body( token_fmt( "typename", Str name(Body),           cast_tmpl ));
	CodeBody impl_cast_attribute = parse_global_body( token_fmt( "typename", Str name(Attributes),     cast_tmpl ));
	CodeBody impl_cast_cmt       = parse_global_body( token_fmt( "typename", Str name(Comment),        cast_tmpl ));
	CodeBody impl_cast_constr    = parse_global_body( token_fmt( "typename", Str name(Constructor),    cast_tmpl ));
	CodeBody impl_cast_class     = parse_global_body( token_fmt( "typename", Str name(Class),          cast_tmpl ));
	CodeBody impl_cast_define    = parse_global_body( token_fmt( "typename", Str name(Define),         cast_tmpl ));
	CodeBody impl_cast_destruct  = parse_global_body( token_fmt( "typename", Str name(Destructor),     cast_tmpl ));
	CodeBody impl_cast_enum      = parse_global_body( token_fmt( "typename", Str name(Enum),           cast_tmpl ));
	CodeBody impl_cast_exec      = parse_global_body( token_fmt( "typename", Str name(Exec),           cast_tmpl ));
	CodeBody impl_cast_extern    = parse_global_body( token_fmt( "typename", Str name(Extern),         cast_tmpl ));
	CodeBody impl_cast_friend    = parse_global_body( token_fmt( "typename", Str name(Friend),         cast_tmpl ));
	CodeBody impl_cast_fn        = parse_global_body( token_fmt( "typename", Str name(Fn),             cast_tmpl ));
	CodeBody impl_cast_include   = parse_global_body( token_fmt( "typename", Str name(Include),        cast_tmpl ));
	CodeBody impl_cast_module    = parse_global_body( token_fmt( "typename", Str name(Module),         cast_tmpl ));
	CodeBody impl_cast_ns        = parse_global_body( token_fmt( "typename", Str name(NS),             cast_tmpl ));
	CodeBody impl_cast_op        = parse_global_body( token_fmt( "typename", Str name(Operator),       cast_tmpl ));
	CodeBody impl_cast_opcast    = parse_global_body( token_fmt( "typename", Str name(OpCast),         cast_tmpl ));
	CodeBody impl_cast_params    = parse_global_body( token_fmt( "typename", Str name(Params),         cast_tmpl ));
	CodeBody impl_cast_pragma    = parse_global_body( token_fmt( "typename", Str name(Pragma),         cast_tmpl ));
	CodeBody impl_cast_precond   = parse_global_body( token_fmt( "typename", Str name(PreprocessCond), cast_tmpl ));
	CodeBody impl_cast_specs     = parse_global_body( token_fmt( "typename", Str name(Specifiers),     cast_tmpl ));
	CodeBody impl_cast_struct    = parse_global_body( token_fmt( "typename", Str name(Struct),         cast_tmpl ));
	CodeBody impl_cast_tmpl      = parse_global_body( token_fmt( "typename", Str name(Template),       cast_tmpl ));
	CodeBody impl_cast_type      = parse_global_body( token_fmt( "typename", Str name(Typename),       cast_tmpl ));
	CodeBody impl_cast_typedef   = parse_global_body( token_fmt( "typename", Str name(Typedef),        cast_tmpl ));
	CodeBody impl_cast_union     = parse_global_body( token_fmt( "typename", Str name(Union),          cast_tmpl ));
	CodeBody impl_cast_using     = parse_global_body( token_fmt( "typename", Str name(Using),          cast_tmpl ));
	CodeBody impl_cast_var       = parse_global_body( token_fmt( "typename", Str name(Var),            cast_tmpl ));

	CodeBody result = def_global_body( args(
		def_pragma( txt("region generated code inline implementation")),
		fmt_newline,
		impl_code,
		impl_code_body,
		impl_code_attr,
		impl_code_cmt,
		impl_code_constr,
		impl_code_class,
		impl_code_define,
		impl_code_destruct,
		impl_code_enum,
		impl_code_exec,
		impl_code_extern,
		impl_code_friend,
		impl_code_fn,
		impl_code_include,
		impl_code_module,
		impl_code_ns,
		impl_code_op,
		impl_code_opcast,
		impl_code_params,
		impl_code_pragma,
		impl_code_precond,
		impl_code_specs,
		impl_code_struct,
		impl_code_tmpl,
		impl_code_type,
		impl_code_typedef,
		impl_code_union,
		impl_code_using,
		impl_code_var,
		fmt_newline,
		def_pragma( txt("endregion generated code inline implementation")),
		fmt_newline,
		def_pragma( txt("region generated AST/Code cast implementation")),
		untyped_str(txt("GEN_OPTIMIZE_MAPPINGS_BEGIN\n")),
		fmt_newline,
		impl_cast_body,
		impl_cast_attribute,
		impl_cast_cmt,
		impl_cast_constr,
		impl_cast_class,
		impl_cast_define,
		impl_cast_destruct,
		impl_cast_enum,
		impl_cast_exec,
		impl_cast_extern,
		impl_cast_friend,
		impl_cast_fn,
		impl_cast_include,
		impl_cast_module,
		impl_cast_ns,
		impl_cast_op,
		impl_cast_opcast,
		impl_cast_params,
		impl_cast_pragma,
		impl_cast_precond,
		impl_cast_specs,
		impl_cast_struct,
		impl_cast_tmpl,
		impl_cast_type,
		impl_cast_typedef,
		impl_cast_union,
		impl_cast_using,
		impl_cast_var,
		fmt_newline,
		untyped_str(txt("GEN_OPITMIZE_MAPPINGS_END\n")),
		def_pragma( txt("endregion generated AST/Code cast implementation")),
		fmt_newline
	));

	return result;
#pragma pop_macro("rcast")
#pragma pop_macro("log_failure")
}
