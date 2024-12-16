#pragma once
#include "gen.hpp"

using namespace gen;

void convert_cpp_enum_to_c( CodeEnum to_convert, CodeBody to_append )
{
#pragma push_macro("enum_underlying")
#undef enum_underlying
	StrCached type = to_convert->UnderlyingType ? to_convert->UnderlyingType.to_strbuilder().to_str() : to_convert->Name;
	CodeTypedef tdef = parse_typedef(token_fmt("type", type, "name", to_convert->Name, stringize( typedef enum <type> <name>; )));
	if (to_convert->UnderlyingType)
	{
		to_convert->UnderlyingTypeMacro = untyped_str(token_fmt("type", to_convert->UnderlyingType->Name, stringize(enum_underlying(<type>))));
		to_convert->UnderlyingType      = CodeTypename{nullptr};
	}
	to_append.append(to_convert);
	to_append.append(tdef);
#pragma pop_macro("enum_underlying")
}

b32 ignore_preprocess_cond_block( Str cond_sig, Code& entry_iter, CodeBody& parsed_body, CodeBody& body )
{
	b32 found = false;
	CodePreprocessCond cond = cast(CodePreprocessCond, entry_iter);
	if ( cond->Content.is_equal(cond_sig) )
	{
		//log_fmt("Preprocess cond found: %S\n", cond->Content);
		found = true;

		s32 depth = 0;
		for(b32 continue_for = true; continue_for && entry_iter != parsed_body.end(); ) switch (entry_iter->Type)
		{
			case CT_Preprocess_If:
			case CT_Preprocess_IfDef:
			case CT_Preprocess_IfNotDef:
				++ depth;
		 		++ entry_iter;
			break;

			case CT_Preprocess_Else:
				++ entry_iter;
				if (depth == 1) for(; entry_iter != parsed_body.end(); ++ entry_iter)
				{
					if ( entry_iter->Type == CT_Preprocess_EndIf)
						break;

					body.append(entry_iter);
				}
			break;

			case CT_Preprocess_EndIf:
			{
				-- depth;
				if (depth == 0) {
					continue_for = false;
					break;
				}
				++ entry_iter;
			}
			break;
			default:
				++ entry_iter;
			break;
		}
	}

	return found;
}

constexpr bool GenericSel_One_Arg = true;
enum GenericSelectionOpts : u32 { GenericSel_Default, GenericSel_By_Ref, GenericSel_Direct_Type };
Code gen_generic_selection_function_macro( s32 num_slots, Str macro_name, GenericSelectionOpts opts = GenericSel_Default, bool one_arg = false )
{
/* Implements:
	#define GEN_FUNCTION_GENERIC_EXAMPLE( selector_arg, ... ) _Generic(      \
	(selector_arg),                                                          \
		GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT( FunctionID__ARGS_SIG_1 )     \
		GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT( FunctionID__ARGS_SIG_2 )     \
		...                                                                  \
		GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT_LAST(FunctionID__ARGS_SIG_N ) \
	) GEN_RESOLVED_FUNCTION_CALL( selector_arg )
*/
	local_persist
	StrBuilder define_builder = StrBuilder::make_reserve(_ctx->Allocator_Temp, kilobytes(64));
	define_builder.clear();

	Str macro_begin;
	if (opts == GenericSel_Direct_Type) {
		macro_begin = token_fmt( "macro_name", (Str)macro_name,
R"(#define <macro_name>(selector_arg, ...) _Generic( (*(selector_arg*)NULL ), \
)"
		);
	}
	else {
		macro_begin = token_fmt( "macro_name", (Str)macro_name,
R"(#define <macro_name>(selector_arg, ...) _Generic( (selector_arg), \
)"
		);
	}
	define_builder.append(macro_begin);

	for ( s32 slot = 1; slot <= num_slots; ++ slot )
	{
		Str slot_str = StrBuilder::fmt_buf(_ctx->Allocator_Temp, "%d", slot).to_str();
		define_builder.append( token_fmt( "macro_name", macro_name, "slot", slot_str,
R"(GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT( GENERIC_SLOT_<slot>__<macro_name> ) \
)"
		));
	}

	define_builder.append( txt("default: gen_generic_selection_fail\\\n") );

	if ( ! one_arg )
	{
		if (opts == GenericSel_By_Ref)
			define_builder.append(txt(")\\\nGEN_RESOLVED_FUNCTION_CALL( & selector_arg, __VA_ARGS__ )"));
		else if (opts == GenericSel_Direct_Type)
			define_builder.append(txt(")\\\nGEN_RESOLVED_FUNCTION_CALL( __VA_ARGS__ )"));
		else
			define_builder.append(txt(")\\\nGEN_RESOLVED_FUNCTION_CALL( selector_arg, __VA_ARGS__ )"));
	}
	else
	{
		if (opts == GenericSel_By_Ref)
			define_builder.append(txt(")\\\nGEN_RESOLVED_FUNCTION_CALL( & selector_arg )"));
		else if (opts == GenericSel_Direct_Type)
			define_builder.append(txt(")\\\nGEN_RESOLVED_FUNCTION_CALL()"));
		else
			define_builder.append(txt(")\\\nGEN_RESOLVED_FUNCTION_CALL( selector_arg )"));
	}

	// Add gap for next definition
	define_builder.append(txt("\n\n"));

	Code macro = untyped_str(define_builder.to_str());
	return macro;
}

CodeFn rename_function_to_unique_symbol(CodeFn fn, Str optional_prefix = txt(""))
{
    // Get basic components for the name
    Str        old_name = fn->Name;
    StrBuilder new_name;

    // Add prefix if provided
    if (optional_prefix.Len)
        new_name = strbuilder_fmt_buf(_ctx->Allocator_Temp, "%S_%S_", optional_prefix, old_name);
    else
        new_name = strbuilder_fmt_buf(_ctx->Allocator_Temp, "%S_", old_name);

    // Add return type to the signature
    if (fn->ReturnType)
        new_name.append_fmt("_%S", fn->ReturnType->Name);

    // Add parameter types to create a unique signature
    bool first_param = true;
    for (CodeParams param = fn->Params; param.ast; param = param->Next)
    {
        if (param->ValueType)
        {
            // Add separator for readability
            if (first_param)
            {
                new_name.append("_P_");
                first_param = false;
            }
            else
                new_name.append("_");

            // Add parameter type, handle any specifiers
            if (param->ValueType->Specs && param->ValueType->Specs->NumEntries > 0)
            {
                // Add specifiers (const, volatile, etc)
                for (Specifier spec : param->ValueType->Specs)
                {
					if (spec == Spec_Ptr) {
						new_name.append("ptr_");
						continue;
					}

                    new_name.append_fmt("%S_", spec_to_str(spec));
                }
            }

            new_name.append_fmt("%S", param->ValueType->Name);
        }
    }

    // Handle function specifiers if present
    if (fn->Specs && fn->Specs->NumEntries > 0)
    {
        new_name.append("_S_");
        for (Specifier* spec = begin(fn->Specs);
             spec != end(fn->Specs);
             ++spec)
        {
            new_name.append_fmt("%S_", spec_to_str(*spec));
        }
    }

    fn->Name = new_name;
    return fn;
}

using SwapContentProc = CodeBody(void);
bool swap_pragma_region_implementation( Str region_name, SwapContentProc* swap_content, Code& entry_iter, CodeBody& body )
{
	bool found = false;
	CodePragma possible_region = cast(CodePragma, entry_iter);

	StrBuilder region_sig    = strbuilder_fmt_buf(_ctx->Allocator_Temp, "region %s",    region_name.Ptr);
	StrBuilder endregion_sig = strbuilder_fmt_buf(_ctx->Allocator_Temp, "endregion %s", region_name.Ptr);
	if ( possible_region->Content.contains(region_sig))
	{
		found = true;
		// body.append(possible_region);
		body.append(swap_content());

		++ entry_iter;
		for(b32 continue_for = true; continue_for; ++entry_iter) switch
		(entry_iter->Type) {
			case CT_Preprocess_Pragma:
			{
				CodePragma possible_end_region = cast(CodePragma, entry_iter);
				if ( possible_end_region->Content.contains(endregion_sig) ) {
					// body.append(possible_end_region);
					continue_for = false;
				}
			}
			break;
		}
		body.append(entry_iter);
	}
	return found;
}
