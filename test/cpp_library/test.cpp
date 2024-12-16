#define GEN_IMPLEMENTATION
#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#include "gen.hpp"

int main()
{
	using namespace gen;
	Context ctx = {};
	gen::init( & ctx);

	Builder src_hello = builder_open("gen/hello.cpp");
	CodeVar hello_var = parse_variable( code(
		constexpr char const* hello_gencpp_str = "HELLO GENCPP !";
	));
	src_hello.print(hello_var);
	src_hello.write();

	CodeBody body = parse_file("gen/hello.cpp");
	for ( Code code : body ) switch (code->Type) {
		case CT_Variable:
			CodeVar var = cast(CodeVar, code);
			log_fmt("%S", var->Value);
		break;
	}

	gen::deinit(& ctx);
	return 0;
}
