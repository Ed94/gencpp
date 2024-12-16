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

	gen_Builder src_hello = gen_builder_open("gen/hello.c");
	gen_CodeVar hello_var = gen_parse_variable(code(
		char const* hello_gencpp_str = "HELLO GENCPP C11 !";
	));
	gen_builder_print( & src_hello, (gen_Code)hello_var );
	gen_builder_write(& src_hello);

	gen_CodeBody body = gen_parse_file("gen/hello.c");
	for (gen_Code code = gen_iterator(CodeBody, body, code)) switch (code->Type) {
		case CT_Variable:
			gen_CodeVar var = (gen_CodeVar) code;
			gen_log_fmt("%S", var->Value);
		break;
	}

	gen_deinit(& ctx);
	return 0;
}
