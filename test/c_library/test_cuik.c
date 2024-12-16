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

	gen_CodeBody h_passes = gen_parse_file("Cuik/tb/opt/passes.h");
	for (gen_Code code = gen_iterator(CodeBody, h_passes, code)) switch (code->Type) {
		case CT_Struct:
		case CT_Function:
		case CT_Variable:
		case CT_Typedef:
			gen_log_fmt("%S: %S", gen_codetype_to_str(code->Type), code->Name);
		break;
	}

	gen_deinit(& ctx);
	return 0;
}
