#define GEN_IMPLEMENTATION
#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#include "gen_singleheader.h"

gen_global gen_s32 g_failures = 0;

gen_internal void expect(int condition, gen_Str name) {
	if (condition == false) { gen_log_fmt("FAIL %S\n", name); ++ g_failures; }
}

gen_internal void expect_fragments(gen_Str text, gen_Str label) {
	expect(text.Ptr != 0 && text.Len > 0, label);
	expect(gen_str_contains(text, gen_txt("baseline_message")), label);
	expect(gen_str_contains(text, gen_txt("char")),             label);
	expect(gen_str_contains(text, gen_txt("const")),            label);
	expect(gen_str_contains(text, gen_txt("*")),                label);
	expect(gen_str_contains(text, gen_txt("GENCPP_BASELINE")),  label);
} 

gen_internal void expect_variable(gen_CodeVar var, gen_Str label) {
	expect(var != 0, label); if (var == 0) return;
	expect(var->Type == CT_Variable, label);
	expect(gen_str_contains(var->Name, gen_txt("baseline_message")), label);
	expect(var->ValueType != 0, label);
	expect(var->Value != 0, label);
	if (var->Value != 0) {
		expect(gen_str_contains(var->Value->Content, gen_txt("GENCPP_BASELINE")), label);
	}
}

int main()
{
	gen_Context ctx = {0}; gen_init(&ctx);

	gen_CodeTypename type    = gen_def_type(name(char), .specifiers = gen_def_specifiers(2, Spec_Const, Spec_Ptr) );
	gen_CodeVar      upfront = gen_def_variable(type, name(baseline_message), 
		.value = gen_untyped_str(code("\"GENCPP_BASELINE\""))
	);
	expect_variable(upfront, gen_txt("c11 upfront"));
	expect_fragments(gen_strbuilder_to_str(gen_code_to_strbuilder(upfront)), gen_txt("c11 upfront serialized"));

	gen_Code untyped = gen_code_str(char const* baseline_message = "GENCPP_BASELINE"; );
	expect(untyped != 0, gen_txt("c11 untyped"));

	gen_Str untyped_text = gen_strbuilder_to_str(gen_code_to_strbuilder(untyped));
	expect_fragments(untyped_text, gen_txt("c11 untyped content"));
	expect_variable(gen_parse_variable(untyped_text), gen_txt("c11 untyped reparsed"));

	gen_CodeVar parsed = gen_parse_variable(code(char const* baseline_message = "GENCPP_BASELINE"; ));
	expect_variable(parsed, gen_txt("c11 parsed"));
	expect_fragments(gen_strbuilder_to_str(gen_code_to_strbuilder(parsed)), gen_txt("c11 parsed serialized"));

	gen_deinit(&ctx);
	if (g_failures != 0) { gen_log_fmt("c11 baseline smoke failed: %d\n", g_failures); return 1; }
	gen_log_fmt("c11 baseline smoke passed\n");
	return 0;
}
