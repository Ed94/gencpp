#pragma once
#include "gen.hpp"

using namespace gen;

void convert_cpp_enum_to_c( CodeEnum to_convert, CodeBody to_append )
{
#pragma push_macro("enum_underlying")
#undef enum_underlying
	if (to_convert->UnderlyingType)
	{
		to_convert->UnderlyingTypeMacro = untyped_str(token_fmt("type", to_convert->UnderlyingType->Name, stringize(enum_underlying(<type>))));
		to_convert->UnderlyingType      = CodeTypename{nullptr};
	}
	CodeTypedef tdef = parse_typedef(token_fmt("name", to_convert->Name, stringize( typedef enum <name> <name>; )));
	to_append.append(to_convert);
	to_append.append(tdef);
#pragma pop_macro("enum_underlying")
}

b32 ignore_preprocess_cond_block( StrC cond_sig, Code& entry_iter, CodeBody& parsed_body, CodeBody& body )
{
	b32 found = false;
	CodePreprocessCond cond = cast(CodePreprocessCond, entry_iter);
	if ( cond->Content.is_equal(cond_sig) )
	{
		//log_fmt("Preprocess cond found: %SC\n", cond->Content);
		found = true;

		s32 depth = 1;
		++ entry_iter;
		for(b32 continue_for = true; continue_for && entry_iter != parsed_body.end(); ) switch
		(entry_iter->Type) {
			case CT_Preprocess_If:
			case CT_Preprocess_IfDef:
			case CT_Preprocess_IfNotDef:
				++ depth;
		 		++ entry_iter;
			break;

			case CT_Preprocess_Else:
				++ entry_iter;
				for(; continue_for && entry_iter != parsed_body.end(); ++ entry_iter)
				{
					if (entry_iter->Type == CT_Preprocess_EndIf)
					{
						continue_for = false;
						break;
					}
					body.append(entry_iter);
				}
			break;

			case CT_Preprocess_EndIf:
			{
				depth --;
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
Code gen_generic_selection_function_macro( s32 num_slots, StrC macro_name, GenericSelectionOpts opts = GenericSel_Default, bool one_arg = false )
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
	String define_builder = String::make_reserve(GlobalAllocator, kilobytes(64));
	define_builder.clear();

	StrC macro_begin;
	if (opts == GenericSel_Direct_Type) {
		macro_begin = token_fmt( "macro_name", (StrC)macro_name,
R"(#define <macro_name>(selector_arg, ...) _Generic( (*(selector_arg*)NULL ), \
)"
		);
	}
	else {
		macro_begin = token_fmt( "macro_name", (StrC)macro_name,
R"(#define <macro_name>(selector_arg, ...) _Generic( (selector_arg), \
)"
		);
	}
	define_builder.append(macro_begin);

	for ( s32 slot = 1; slot <= num_slots; ++ slot )
	{
		StrC slot_str = String::fmt_buf(GlobalAllocator, "%d", slot).to_strc();
		if (slot == num_slots && false)
		{
			define_builder.append( token_fmt( "macro_name", macro_name, "slot", slot_str,
R"(		GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT_LAST(  GENERIC_SLOT_<slot>__<macro_name> ) \
)"
			));
			// if ( one_arg )
			// 	define_builder.append(token_fmt( "macro_name", macro_name, stringize(
			// 		default: static_assert(false, "<macro_name>: Failed to select correct function signature (Did you pass the type?)")
			// 	)));
			// else
			// 	define_builder.append(token_fmt( "macro_name", macro_name, stringize(
			// 		default: static_assert(false, "<macro_name>: Failed to select correct function signature")
			// 	)));
			continue;
		}

		define_builder.append( token_fmt( "macro_name", macro_name, "slot", slot_str,
R"(		GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT( GENERIC_SLOT_<slot>__<macro_name> ) \
)"
		));
	}

	define_builder.append( txt("default: gen_generic_selection_fail") );

	if ( ! one_arg )
	{
		if (opts == GenericSel_By_Ref)
			define_builder.append(txt("\t)\tGEN_RESOLVED_FUNCTION_CALL( & selector_arg, __VA_ARGS__ )"));
		else if (opts == GenericSel_Direct_Type)
			define_builder.append(txt("\t)\tGEN_RESOLVED_FUNCTION_CALL( __VA_ARGS__ )"));
		else
			define_builder.append(txt("\t)\tGEN_RESOLVED_FUNCTION_CALL( selector_arg, __VA_ARGS__ )"));
	}
	else
	{
		if (opts == GenericSel_By_Ref)
			define_builder.append(txt("\t)\tGEN_RESOLVED_FUNCTION_CALL( & selector_arg )"));
		else if (opts == GenericSel_Direct_Type)
			define_builder.append(txt("\t)\tGEN_RESOLVED_FUNCTION_CALL()"));
		else
			define_builder.append(txt("\t)\tGEN_RESOLVED_FUNCTION_CALL( selector_arg )"));
	}

	// Add gap for next definition
	define_builder.append(txt("\n\n"));

	Code macro = untyped_str(define_builder.to_strc());
	return macro;
}

CodeFn rename_function_to_unique_symbol(CodeFn fn, StrC optional_prefix = txt(""))
{
    // Get basic components for the name
    StrC old_name = fn->Name;
    String new_name;

    // Add prefix if provided
    if (optional_prefix.Len)
        new_name = string_fmt_buf(GlobalAllocator, "%SC_%SC_", optional_prefix, old_name);
    else
        new_name = string_fmt_buf(GlobalAllocator, "%SC_", old_name);

    // Add return type to the signature
    if (fn->ReturnType)
        new_name.append_fmt("_%SC", fn->ReturnType->Name);

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

                    new_name.append_fmt("%SC_", spec_to_str(spec));
                }
            }

            new_name.append_fmt("%SC", param->ValueType->Name);
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
            new_name.append_fmt("%SC_", spec_to_str(*spec));
        }
    }

    fn->Name = new_name;
    return fn;
}

using SwapContentProc = CodeBody(void);
bool swap_pragma_region_implementation( StrC region_name, SwapContentProc* swap_content, Code& entry_iter, CodeBody& body )
{
	bool found = false;
	CodePragma possible_region = cast(CodePragma, entry_iter);

	String region_sig    = string_fmt_buf(GlobalAllocator, "region %s",    region_name.Ptr);
	String endregion_sig = string_fmt_buf(GlobalAllocator, "endregion %s", region_name.Ptr);
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
