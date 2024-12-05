// #pragma once
// #include "../project/gen.hpp"

// using namespace gen;



b32 ignore_preprocess_cond_block( StrC cond_sig, Code& entry_iter, CodeBody& body )
{
	b32 found = false;
	CodePreprocessCond cond = cast(CodePreprocessCond, entry_iter);
	if ( cond->Content.contains(cond_sig) )
	{
		log_fmt("Preprocess cond found: %SC\n", cond->Content);
		found = true;

		s32 depth = 1;
		++ entry_iter;
		for(b32 continue_for = true; continue_for && entry_iter != body.end(); ) switch
		(entry_iter->Type) {
			case CT_Preprocess_If:
			case CT_Preprocess_IfDef:
			case CT_Preprocess_IfNotDef:
				++ depth;
		 		++ entry_iter;
			break;

			case CT_Preprocess_Else:
				++ entry_iter;
				for(; continue_for && entry_iter != body.end(); ++ entry_iter)
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
    for (CodeParam param = fn->Params; param.ast; param = param->Next)
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

                    new_name.append_fmt("%SC_", to_str(spec));
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
            new_name.append_fmt("%SC_", to_str(*spec));
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
