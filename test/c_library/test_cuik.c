#define GEN_IMPLEMENTATION
#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#include "gen_singleheader.h"

#define gen_iterator( Type, container, iter )   \
	        gen_begin_ ## Type(container);      \
	iter != gen_end_   ## Type(container);      \
	code  = gen_next_  ## Type(container, iter)

int main()
{
	gen_Context ctx = {0};
	gen_init(& ctx);

	gen_register_macros( args(
		((gen_Macro){ txt("USERN"),     MT_Expression, MF_Functional }),
		((gen_Macro){ txt("USERI"),     MT_Expression, MF_Functional }),
		((gen_Macro){ txt("USERI"),     MT_Expression, MF_Functional }),
		((gen_Macro){ txt("FOR_USERS"), MT_Statement,  MF_Functional }),
		((gen_Macro){ txt("FOR_SUCC"),  MT_Statement,  MF_Functional })
	));

	gen_CodeBody h_passes = gen_parse_file("Cuik/tb/opt/passes.h\n");
	for (gen_Code code = gen_iterator(CodeBody, h_passes, code)) switch (code->Type) {
		case CT_Function:
			gen_log_fmt("%S:\t%S params: %S return type: %S\n"
				, gen_codetype_to_str(code->Type)
				, code->Name
				, gen_strbuilder_to_str( gen_params_to_strbuilder( (gen_CodeParams) code->Params))
				, gen_strbuilder_to_str( gen_typename_to_strbuilder((gen_CodeTypename) code->ReturnType))
			);
		break;

		case CT_Variable:
			gen_log_fmt("%S:\t%S Type: %S\n", gen_codetype_to_str(code->Type), code->Name, code->ValueType);
		break;

		case CT_Struct:
		case CT_Typedef:
			gen_log_fmt("%S:\t%S\n", gen_codetype_to_str(code->Type), code->Name);
		break;
	}

	gen_deinit(& ctx);
	return 0;
}
