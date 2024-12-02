// #pragma once
// #include "../project/gen.hpp"

// using namespace gen;

using SwapContentProc = CodeBody(void);

b32 ignore_preprocess_cond_block( StrC cond_sig, Code& entry_iter, CodeBody& body )
{
	b32 found = false;
	CodePreprocessCond cond = entry_iter.code_cast<CodePreprocessCond>();
	if ( cond->Content.contains(cond_sig) )
	{
		log_fmt("Preprocess cond found: %S\n", cond->Content);
		found = true;

		s32 depth = 1;
		++ entry_iter; for(b32 continue_for = true; continue_for && entry_iter != body.end(); ) switch
		(entry_iter->Type) {
			case ECode::Preprocess_If:
			case ECode::Preprocess_IfDef:
			case ECode::Preprocess_IfNotDef:
				depth ++;
			break;

			case ECode::Preprocess_EndIf:
			{
				depth --;
				if (depth == 0) {
					continue_for = false;
					break;
				}
			}
			break;
			default:
				++ entry_iter;
			break;
		}
	}

	return found;
}

bool swap_pragma_region_implementation( StrC region_name, SwapContentProc* swap_content, Code& entry_iter, CodeBody& body )
{
	bool found = false;
	CodePragma possible_region = entry_iter.code_cast<CodePragma>();

	String region_sig    = string_fmt_buf(GlobalAllocator, "region %s",    region_name.Ptr);
	String endregion_sig = string_fmt_buf(GlobalAllocator, "endregion %s", region_name.Ptr);
	if ( possible_region->Content.contains(region_sig))
	{
		found = true;
		// body.append(possible_region);
		body.append(swap_content());

		++ entry_iter; for(b32 continue_for = true; continue_for; ++entry_iter) switch
		(entry_iter->Type) {
			case ECode::Preprocess_Pragma:
			{
				CodePragma possible_end_region = entry_iter.code_cast<CodePragma>();
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
