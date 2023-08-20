#pragma once

#include "gen.hpp"

GEN_NS_BEGIN
#include "dependencies/parsing.hpp"
GEN_NS_END

using namespace gen;

CodeBody gen_ecode( char const* path )
{
	char  scratch_mem[kilobytes(1)];
	Arena scratch = Arena::init_from_memory( scratch_mem, sizeof(scratch_mem) );

	file_read_contents( scratch, zero_terminate, path );

	CSV_Object csv_nodes;
	csv_parse( &csv_nodes, scratch_mem, GlobalAllocator, false );

	Array<ADT_Node> enum_strs = csv_nodes.nodes[0].nodes;

	String enum_entries   = String::make_reserve( GlobalAllocator, kilobytes(1) );
	String to_str_entries = String::make_reserve( GlobalAllocator, kilobytes(1) );

	for ( ADT_Node node : enum_strs )
	{
		char const* code = node.string;
		enum_entries.append_fmt( "%s,\n", code );
		to_str_entries.append_fmt( "{ sizeof(\"%s\"), \"%s\" },\n", code, code );
	}

	CodeEnum enum_code = parse_enum( token_fmt( "entries", (StrC)enum_entries, stringize(
		enum Type : u32
		{
			<entries>
			NumTypes
		};
	)));

#pragma push_macro( "local_persist" )
#undef local_persist
	CodeFn to_str = parse_function( token_fmt( "entries", (StrC)to_str_entries, stringize(
		StrC to_str( Type type )
		{
			local_persist
			StrC lookup[] {
				<entries>
			};

			return lookup[ type ];
		}
	)));
#pragma pop_macro( "local_persist" )

	CodeNS    nspace = def_namespace( name(ECode), def_namespace_body( args( enum_code, to_str ) ) );
	CodeUsing code_t = def_using( name(CodeT), def_type( name(ECode::Type) ) );

	return def_global_body( args( nspace, code_t ) );
}

CodeBody gen_eoperator( char const* path )
{
	char scratch_mem[kilobytes(4)];
	Arena scratch = Arena::init_from_memory( scratch_mem, sizeof(scratch_mem) );

	file_read_contents( scratch, zero_terminate, path );

	CSV_Object csv_nodes;
	csv_parse( &csv_nodes, scratch_mem, GlobalAllocator, false );

	Array<ADT_Node> enum_strs = csv_nodes.nodes[0].nodes;
	Array<ADT_Node> str_strs  = csv_nodes.nodes[1].nodes;

	String enum_entries   = String::make_reserve( GlobalAllocator, kilobytes(1) );
	String to_str_entries = String::make_reserve( GlobalAllocator, kilobytes(1) );

	for (uw idx = 0; idx < enum_strs.num(); idx++)
	{
		char const* enum_str     = enum_strs[idx].string;
		char const* entry_to_str = str_strs [idx].string;

		enum_entries.append_fmt( "%s,\n", enum_str );
		to_str_entries.append_fmt( "{ sizeof(\"%s\"), \"%s\" },\n", entry_to_str, entry_to_str);
	}

	CodeEnum  enum_code = parse_enum(token_fmt("entries", (StrC)enum_entries, stringize(
		enum Type : u32
		{
			<entries>
			NumOps
		};
	)));

#pragma push_macro( "local_persist" )
#undef local_persist
	CodeFn to_str = parse_function(token_fmt("entries", (StrC)to_str_entries, stringize(
		StrC to_str( Type op )
		{
			local_persist
			StrC lookup[] {
				<entries>
			};

			return lookup[ op ];
		}
	)));
#pragma pop_macro( "local_persist" )

	CodeNS nspace = def_namespace( name(EOperator), def_namespace_body( args( enum_code, to_str ) ) );

	CodeUsing operator_t = def_using( name(OperatorT), def_type( name(EOperator::Type) ) );

	return def_global_body( args( nspace, operator_t ) );
}

CodeBody gen_especifier( char const* path )
{
	char scratch_mem[kilobytes(4)];
	Arena scratch = Arena::init_from_memory( scratch_mem, sizeof(scratch_mem) );

	file_read_contents( scratch, zero_terminate, path );

	CSV_Object csv_nodes;
	csv_parse( &csv_nodes, scratch_mem, GlobalAllocator, false );

	Array<ADT_Node> enum_strs = csv_nodes.nodes[0].nodes;
	Array<ADT_Node> str_strs  = csv_nodes.nodes[1].nodes;

	String enum_entries   = String::make_reserve( GlobalAllocator, kilobytes(1) );
	String to_str_entries = String::make_reserve( GlobalAllocator, kilobytes(1) );

	for (uw idx = 0; idx < enum_strs.num(); idx++)
	{
		char const* enum_str     = enum_strs[idx].string;
		char const* entry_to_str = str_strs [idx].string;

		enum_entries.append_fmt( "%s,\n", enum_str );
		to_str_entries.append_fmt( "{ sizeof(\"%s\"), \"%s\" },\n", entry_to_str, entry_to_str);
	}

	CodeEnum  enum_code = parse_enum(token_fmt("entries", (StrC)enum_entries, stringize(
		enum Type : u32
		{
			<entries>
			NumSpecifiers
		};
	)));

	CodeFn is_trailing = parse_function(token_fmt("specifier", (StrC)to_str_entries, stringize(
		bool is_trailing( Type specifier )
		{
			return specifier > Virtual;
		}
	)));

#pragma push_macro( "local_persist" )
#pragma push_macro( "do_once_start" )
#pragma push_macro( "do_once_end" )
#undef local_persist
#undef do_once_start
#undef do_once_end
	CodeFn to_str = parse_function(token_fmt("entries", (StrC)to_str_entries, stringize(
		StrC to_str( Type type )
		{
			local_persist
			StrC lookup[] {
				<entries>
			};

			return lookup[ type ];
		}
	)));

	CodeFn to_type = parse_function( token_fmt( "entries", (StrC)to_str_entries, stringize(
		Type to_type( StrC str )
		{
			local_persist
			u32 keymap[ NumSpecifiers ];
			do_once_start
				for ( u32 index = 0; index < NumSpecifiers; index++ )
				{
					StrC enum_str = to_str( (Type)index );

					// We subtract 1 to remove the null terminator
					// This is because the tokens lexed are not null terminated.
					keymap[index] = crc32( enum_str.Ptr, enum_str.Len - 1);
				}
			do_once_end

			u32 hash = crc32( str.Ptr, str.Len );

			for ( u32 index = 0; index < NumSpecifiers; index++ )
			{
				if ( keymap[index] == hash )
					return (Type)index;
			}

			return Invalid;
		}
	)));
#pragma pop_macro( "local_persist" )
#pragma pop_macro( "do_once_start" )
#pragma pop_macro( "do_once_end" )

	CodeNS nspace = def_namespace( name(ESpecifier), def_namespace_body( args( enum_code, is_trailing, to_str, to_type ) ) );

	CodeUsing specifier_t = def_using( name(SpecifierT), def_type( name(ESpecifier::Type) ) );

	return def_global_body( args( nspace, specifier_t ) );
}

CodeBody gen_etoktype( char const* etok_path, char const* attr_path )
{
	char  scratch_mem[kilobytes(16)];
	Arena scratch = Arena::init_from_memory( scratch_mem, sizeof(scratch_mem) );

	FileContents enum_content = file_read_contents( scratch, zero_terminate, etok_path );

	CSV_Object csv_enum_nodes;
	csv_parse( &csv_enum_nodes, rcast(char*, enum_content.data), GlobalAllocator, false );

	FileContents attrib_content = file_read_contents( scratch, zero_terminate, attr_path );

	CSV_Object csv_attr_nodes;
	csv_parse( &csv_attr_nodes, rcast(char*, attrib_content.data), GlobalAllocator, false );

	Array<ADT_Node> enum_strs          = csv_enum_nodes.nodes[0].nodes;
	Array<ADT_Node> enum_str_strs      = csv_enum_nodes.nodes[1].nodes;
	Array<ADT_Node> attribute_strs     = csv_attr_nodes.nodes[0].nodes;
	Array<ADT_Node> attribute_str_strs = csv_attr_nodes.nodes[1].nodes;

	String enum_entries             = String::make_reserve( GlobalAllocator, kilobytes(2) );
	String to_str_entries           = String::make_reserve( GlobalAllocator, kilobytes(4) );
	String attribute_entries        = String::make_reserve( GlobalAllocator, kilobytes(2) );
	String to_str_attributes        = String::make_reserve( GlobalAllocator, kilobytes(4) );
	String attribute_define_entries = String::make_reserve( GlobalAllocator, kilobytes(4) );

	for (uw idx = 0; idx < enum_strs.num(); idx++)
	{
		char const* enum_str     = enum_strs[idx].string;
		char const* entry_to_str = enum_str_strs [idx].string;

		enum_entries.append_fmt( "%s,\n", enum_str );
		to_str_entries.append_fmt( "{ sizeof(\"%s\"), \"%s\" },\n", entry_to_str, entry_to_str);
	}

	for ( uw idx = 0; idx < attribute_strs.num(); idx++ )
	{
		char const* attribute_str = attribute_strs[idx].string;
		char const* entry_to_str  = attribute_str_strs [idx].string;

		attribute_entries.append_fmt( "%s,\n", attribute_str );
		to_str_attributes.append_fmt( "{ sizeof(\"%s\"), \"%s\" },\n", entry_to_str, entry_to_str);
		attribute_define_entries.append_fmt( "Entry( %s, %s )", attribute_str, entry_to_str );

		if ( idx < attribute_strs.num() - 1 )
			attribute_define_entries.append( " \\\n");
		else
			attribute_define_entries.append( "\n");
	}

#pragma push_macro( "GEN_DEFINE_ATTRIBUTE_TOKENS" )
#undef GEN_DEFINE_ATTRIBUTE_TOKENS
	CodeDefine attribute_entires_def = def_define( name(GEN_DEFINE_ATTRIBUTE_TOKENS), attribute_define_entries  );
#pragma pop_macro( "GEN_DEFINE_ATTRIBUTE_TOKENS" )

	CodeEnum enum_code = parse_enum(token_fmt("entries", (StrC)enum_entries, "attribute_toks", (StrC)attribute_entries, stringize(
		enum Type : u32
		{
			<entries>
			<attribute_toks>
			NumTokens
		};
	)));

#pragma push_macro( "local_persist" )
#pragma push_macro( "do_once_start" )
#pragma push_macro( "do_once_end" )
#undef local_persist
#undef do_once_start
#undef do_once_end
	CodeFn to_str = parse_function(token_fmt("entries", (StrC)to_str_entries, "attribute_toks", (StrC)to_str_attributes, stringize(
		StrC to_str( Type type )
		{
			local_persist
			StrC lookup[] {
				<entries>
				<attribute_toks>
			};

			return lookup[ type ];
		}
	)));

	CodeFn to_type = parse_function( token_fmt( "entries", (StrC)to_str_entries, stringize(
		Type to_type( StrC str )
		{
			local_persist
			u32 keymap[ NumTokens ];
			do_once_start
				for ( u32 index = 0; index < NumTokens; index++ )
				{
					StrC enum_str = to_str( (Type)index );

					// We subtract 1 to remove the null terminator
					// This is because the tokens lexed are not null terminated.
					keymap[index] = crc32( enum_str.Ptr, enum_str.Len - 1);
				}
			do_once_end

			u32 hash = crc32( str.Ptr, str.Len );

			for ( u32 index = 0; index < NumTokens; index++ )
			{
				if ( keymap[index] == hash )
					return (Type)index;
			}

			return Invalid;
		}
	)));
#pragma pop_macro( "local_persist" )
#pragma pop_macro( "do_once_start" )
#pragma pop_macro( "do_once_end" )

	CodeNS    nspace     = def_namespace( name(ETokType), def_namespace_body( args( attribute_entires_def, enum_code, to_str, to_type ) ) );
	CodeUsing td_toktype = def_using( name(TokType), def_type( name(ETokType::Type) ) );

	return def_global_body( args( nspace, td_toktype ) );
}

CodeBody gen_ast_inlines()
{
#pragma push_macro("rcast")
#pragma push_macro("log_failure")
#undef rcast
#undef log_failure
	char const* code_impl_tmpl = stringize(
		\n
		char const* <typename>::debug_str()
		{
			if ( ast == nullptr )
				return "Code::debug_str: AST is null!";

			return rcast(AST*, ast)->debug_str();
		}
		Code <typename>::duplicate()
		{
			if ( ast == nullptr )
			{
				log_failure("Code::duplicate: Cannot duplicate code, AST is null!");
				return Code::Invalid;
			}

			return { rcast(AST*, ast)->duplicate() };
		}
		bool <typename>::is_equal( Code other )
		{
			if ( ast == nullptr || other.ast == nullptr )
			{
				log_failure("Code::is_equal: Cannot compare code, AST is null!");
				return false;
			}

			return rcast(AST*, ast)->is_equal( other.ast );
		}
		bool <typename>::is_valid()
		{
			return (AST*) ast != nullptr && rcast( AST*, ast)->Type != CodeT::Invalid;
		}
		void <typename>::set_global()
		{
			if ( ast == nullptr )
			{
				log_failure("Code::set_global: Cannot set code as global, AST is null!");
				return;
			}

			rcast(AST*, ast)->Parent = Code::Global.ast;
		}
		String <typename>::to_string()
		{
			if ( ast == nullptr )
			{
			log_failure("Code::to_string: Cannot convert code to string, AST is null!");
			return { nullptr };
			}

			return rcast(AST*, ast)->to_string();
		}
		<typename>& <typename>::operator =( Code other )
		{
			if ( other.ast && other->Parent )
			{
				ast = rcast( decltype(ast), other.ast->duplicate() );
				rcast( AST*, ast)->Parent = nullptr;
			}

			ast = rcast( decltype(ast), other.ast );
			return *this;
		}
		bool <typename>::operator ==( Code other )
		{
			return (AST*) ast == other.ast;
		}
		bool <typename>::operator !=( Code other )
		{
			return (AST*) ast != other.ast;
		}
		<typename>::operator bool()
		{
			return ast != nullptr;
		}
	);

	char const* codetype_impl_tmpl = stringize(
		AST* Code<typename>::raw()
		{
			return rcast( AST*, ast );
		}
		Code<typename>::operator Code()
		{
			return *rcast( Code*, this );
		}
		AST_<typename>* Code<typename>::operator->()
		{
			if ( ast == nullptr )
			{
				log_failure( "Attempt to dereference a nullptr!" );
				return nullptr;
			}
			return ast;
		}
		\n
	);

	CodeBody impl_code          = parse_global_body( token_fmt( "typename", StrC name(Code),               code_impl_tmpl ));
	CodeBody impl_code_body     = parse_global_body( token_fmt( "typename", StrC name(CodeBody),           code_impl_tmpl ));
	CodeBody impl_code_attr     = parse_global_body( token_fmt( "typename", StrC name(CodeAttributes),     code_impl_tmpl ));
	CodeBody impl_code_cmt      = parse_global_body( token_fmt( "typename", StrC name(CodeComment),        code_impl_tmpl ));
	CodeBody impl_code_constr   = parse_global_body( token_fmt( "typename", StrC name(CodeConstructor),    code_impl_tmpl ));
	CodeBody impl_code_class    = parse_global_body( token_fmt( "typename", StrC name(CodeClass),          code_impl_tmpl ));
	CodeBody impl_code_define   = parse_global_body( token_fmt( "typename", StrC name(CodeDefine),         code_impl_tmpl ));
	CodeBody impl_code_destruct = parse_global_body( token_fmt( "typename", StrC name(CodeDestructor),     code_impl_tmpl ));
	CodeBody impl_code_enum     = parse_global_body( token_fmt( "typename", StrC name(CodeEnum),           code_impl_tmpl ));
	CodeBody impl_code_exec     = parse_global_body( token_fmt( "typename", StrC name(CodeExec),           code_impl_tmpl ));
	CodeBody impl_code_extern   = parse_global_body( token_fmt( "typename", StrC name(CodeExtern),         code_impl_tmpl ));
	CodeBody impl_code_include  = parse_global_body( token_fmt( "typename", StrC name(CodeInclude),        code_impl_tmpl ));
	CodeBody impl_code_friend   = parse_global_body( token_fmt( "typename", StrC name(CodeFriend),         code_impl_tmpl ));
	CodeBody impl_code_fn	    = parse_global_body( token_fmt( "typename", StrC name(CodeFn),             code_impl_tmpl ));
	CodeBody impl_code_module   = parse_global_body( token_fmt( "typename", StrC name(CodeModule),         code_impl_tmpl ));
	CodeBody impl_code_ns       = parse_global_body( token_fmt( "typename", StrC name(CodeNS),             code_impl_tmpl ));
	CodeBody impl_code_op       = parse_global_body( token_fmt( "typename", StrC name(CodeOperator),       code_impl_tmpl ));
	CodeBody impl_code_opcast   = parse_global_body( token_fmt( "typename", StrC name(CodeOpCast),         code_impl_tmpl ));
	CodeBody impl_code_param    = parse_global_body( token_fmt( "typename", StrC name(CodeParam),          code_impl_tmpl ));
	CodeBody impl_code_pragma   = parse_global_body( token_fmt( "typename", StrC name(CodePragma),         code_impl_tmpl ));
	CodeBody impl_code_precond  = parse_global_body( token_fmt( "typename", StrC name(CodePreprocessCond), code_impl_tmpl ));
	CodeBody impl_code_specs    = parse_global_body( token_fmt( "typename", StrC name(CodeSpecifiers),     code_impl_tmpl ));
	CodeBody impl_code_struct   = parse_global_body( token_fmt( "typename", StrC name(CodeStruct),         code_impl_tmpl ));
	CodeBody impl_code_tmpl     = parse_global_body( token_fmt( "typename", StrC name(CodeTemplate),       code_impl_tmpl ));
	CodeBody impl_code_type     = parse_global_body( token_fmt( "typename", StrC name(CodeType),           code_impl_tmpl ));
	CodeBody impl_code_typedef  = parse_global_body( token_fmt( "typename", StrC name(CodeTypedef),        code_impl_tmpl ));
	CodeBody impl_code_union    = parse_global_body( token_fmt( "typename", StrC name(CodeUnion),          code_impl_tmpl ));
	CodeBody impl_code_using    = parse_global_body( token_fmt( "typename", StrC name(CodeUsing),          code_impl_tmpl ));
	CodeBody impl_code_var      = parse_global_body( token_fmt( "typename", StrC name(CodeVar),            code_impl_tmpl ));

	impl_code_attr.    append( parse_global_body( token_fmt( "typename", StrC name(Attributes),     codetype_impl_tmpl )));
	impl_code_cmt.     append( parse_global_body( token_fmt( "typename", StrC name(Comment),        codetype_impl_tmpl )));
	impl_code_constr.  append( parse_global_body( token_fmt( "typename", StrC name(Constructor),    codetype_impl_tmpl )));
	impl_code_define.  append( parse_global_body( token_fmt( "typename", StrC name(Define),         codetype_impl_tmpl )));
	impl_code_destruct.append( parse_global_body( token_fmt( "typename", StrC name(Destructor),     codetype_impl_tmpl )));
	impl_code_enum.    append( parse_global_body( token_fmt( "typename", StrC name(Enum),           codetype_impl_tmpl )));
	impl_code_exec.    append( parse_global_body( token_fmt( "typename", StrC name(Exec),           codetype_impl_tmpl )));
	impl_code_extern.  append( parse_global_body( token_fmt( "typename", StrC name(Extern),         codetype_impl_tmpl )));
	impl_code_include. append( parse_global_body( token_fmt( "typename", StrC name(Include),        codetype_impl_tmpl )));
	impl_code_friend.  append( parse_global_body( token_fmt( "typename", StrC name(Friend),         codetype_impl_tmpl )));
	impl_code_fn.      append( parse_global_body( token_fmt( "typename", StrC name(Fn),             codetype_impl_tmpl )));
	impl_code_module.  append( parse_global_body( token_fmt( "typename", StrC name(Module),         codetype_impl_tmpl )));
	impl_code_ns.      append( parse_global_body( token_fmt( "typename", StrC name(NS),             codetype_impl_tmpl )));
	impl_code_op.      append( parse_global_body( token_fmt( "typename", StrC name(Operator),       codetype_impl_tmpl )));
	impl_code_opcast.  append( parse_global_body( token_fmt( "typename", StrC name(OpCast),         codetype_impl_tmpl )));
	impl_code_pragma . append( parse_global_body( token_fmt( "typename", StrC name(Pragma),         codetype_impl_tmpl )));
	impl_code_precond. append( parse_global_body( token_fmt( "typename", StrC name(PreprocessCond), codetype_impl_tmpl )));
	impl_code_tmpl.    append( parse_global_body( token_fmt( "typename", StrC name(Template),       codetype_impl_tmpl )));
	impl_code_type.    append( parse_global_body( token_fmt( "typename", StrC name(Type),           codetype_impl_tmpl )));
	impl_code_typedef. append( parse_global_body( token_fmt( "typename", StrC name(Typedef),        codetype_impl_tmpl )));
	impl_code_union.   append( parse_global_body( token_fmt( "typename", StrC name(Union),          codetype_impl_tmpl )));
	impl_code_using.   append( parse_global_body( token_fmt( "typename", StrC name(Using),          codetype_impl_tmpl )));
	impl_code_var.     append( parse_global_body( token_fmt( "typename", StrC name(Var),            codetype_impl_tmpl )));

	char const* cast_tmpl = stringize(
		AST::operator Code<typename>()
		{
			return { rcast( AST_<typename>*, this ) };
		}

		Code::operator Code<typename>() const
		{
			return { (AST_<typename>*) ast };
		}
	);

	CodeBody impl_cast_body      = parse_global_body( token_fmt( "typename", StrC name(Body),           cast_tmpl ));
	CodeBody impl_cast_attribute = parse_global_body( token_fmt( "typename", StrC name(Attributes),     cast_tmpl ));
	CodeBody impl_cast_cmt       = parse_global_body( token_fmt( "typename", StrC name(Comment),        cast_tmpl ));
	CodeBody impl_cast_constr    = parse_global_body( token_fmt( "typename", StrC name(Constructor),    cast_tmpl ));
	CodeBody impl_cast_class     = parse_global_body( token_fmt( "typename", StrC name(Class),          cast_tmpl ));
	CodeBody impl_cast_define    = parse_global_body( token_fmt( "typename", StrC name(Define),         cast_tmpl ));
	CodeBody impl_cast_destruct  = parse_global_body( token_fmt( "typename", StrC name(Destructor),     cast_tmpl ));
	CodeBody impl_cast_enum      = parse_global_body( token_fmt( "typename", StrC name(Enum),           cast_tmpl ));
	CodeBody impl_cast_exec      = parse_global_body( token_fmt( "typename", StrC name(Exec),           cast_tmpl ));
	CodeBody impl_cast_extern    = parse_global_body( token_fmt( "typename", StrC name(Extern),         cast_tmpl ));
	CodeBody impl_cast_friend    = parse_global_body( token_fmt( "typename", StrC name(Friend),         cast_tmpl ));
	CodeBody impl_cast_fn        = parse_global_body( token_fmt( "typename", StrC name(Fn),             cast_tmpl ));
	CodeBody impl_cast_include   = parse_global_body( token_fmt( "typename", StrC name(Include),        cast_tmpl ));
	CodeBody impl_cast_module    = parse_global_body( token_fmt( "typename", StrC name(Module),         cast_tmpl ));
	CodeBody impl_cast_ns        = parse_global_body( token_fmt( "typename", StrC name(NS),             cast_tmpl ));
	CodeBody impl_cast_op        = parse_global_body( token_fmt( "typename", StrC name(Operator),       cast_tmpl ));
	CodeBody impl_cast_opcast    = parse_global_body( token_fmt( "typename", StrC name(OpCast),         cast_tmpl ));
	CodeBody impl_cast_param     = parse_global_body( token_fmt( "typename", StrC name(Param),          cast_tmpl ));
	CodeBody impl_cast_pragma    = parse_global_body( token_fmt( "typename", StrC name(Pragma),         cast_tmpl ));
	CodeBody impl_cast_precond   = parse_global_body( token_fmt( "typename", StrC name(PreprocessCond), cast_tmpl ));
	CodeBody impl_cast_specs     = parse_global_body( token_fmt( "typename", StrC name(Specifiers),     cast_tmpl ));
	CodeBody impl_cast_struct    = parse_global_body( token_fmt( "typename", StrC name(Struct),         cast_tmpl ));
	CodeBody impl_cast_tmpl      = parse_global_body( token_fmt( "typename", StrC name(Template),       cast_tmpl ));
	CodeBody impl_cast_type      = parse_global_body( token_fmt( "typename", StrC name(Type),           cast_tmpl ));
	CodeBody impl_cast_typedef   = parse_global_body( token_fmt( "typename", StrC name(Typedef),        cast_tmpl ));
	CodeBody impl_cast_union     = parse_global_body( token_fmt( "typename", StrC name(Union),          cast_tmpl ));
	CodeBody impl_cast_using     = parse_global_body( token_fmt( "typename", StrC name(Using),          cast_tmpl ));
	CodeBody impl_cast_var       = parse_global_body( token_fmt( "typename", StrC name(Var),            cast_tmpl ));

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
		impl_code_param,
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
		impl_cast_param,
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
		def_pragma( txt("endregion generated AST/Code cast implementation")),
		fmt_newline
	));

	return result;
#pragma pop_macro("rcast")
#pragma pop_macro("log_failure")
}
