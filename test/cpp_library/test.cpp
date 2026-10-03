#define GEN_IMPLEMENTATION
#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#include "gen.hpp"

global int g_failures = 0;

internal void expect(int condition, gen::Str name) {
	if (condition == false) { gen::log_fmt("FAIL %S\n", name); ++ g_failures; }
}

internal void expect_fragments(gen::Str text, gen::Str label) {
	expect(text.Ptr != nullptr && text.Len > 0, label);
	expect(gen::str_contains(text, txt("baseline_message")), label);
	expect(gen::str_contains(text, txt("char")),             label);
	expect(gen::str_contains(text, txt("const")),            label);
	expect(gen::str_contains(text, txt("*")),                label);
	expect(gen::str_contains(text, txt("GENCPP_BASELINE")),  label);
}

internal void expect_variable(gen::CodeVar var, gen::Str label) {
	expect(var.ast != nullptr, label); if (var.ast == nullptr) return;
	expect(var->Type == gen::CT_Variable, label);
	expect(gen::str_contains(var->Name, txt("baseline_message")), label);
	expect(var->ValueType.ast != nullptr, label);
	expect(var->Value.ast != nullptr, label);
	if (var->Value.ast != nullptr) {
		expect(gen::str_contains(var->Value->Content, txt("GENCPP_BASELINE")), label);
	}
}

int main()
{
	using namespace gen;
	Context ctx = {}; init(&ctx);

	Opts_def_type type_opts = {};
	type_opts.specifiers = def_specifiers(args(Spec_Const, Spec_Ptr));
	CodeTypename type    = def_type(name(char), type_opts);
	Opts_def_variable opts = {}; opts.value = untyped_str(code("\"GENCPP_BASELINE\""));
	CodeVar upfront = def_variable(type, name(baseline_message), opts);
	expect_variable(upfront, txt("cpp upfront"));
	expect_fragments(strbuilder_to_str(code_to_strbuilder((Code)upfront)), txt("cpp upfront serialized"));

	Code    untyped        = code_str(char const* baseline_message = "GENCPP_BASELINE";); expect(untyped.ast != nullptr,  txt("cpp untyped"));
	Str     untyped_text   = strbuilder_to_str(code_to_strbuilder(untyped));              expect_fragments(untyped_text,  txt("cpp untyped content"));
	CodeVar parsed_untyped = parse_variable(untyped_text);                                expect_variable(parsed_untyped, txt("cpp untyped reparsed"));

	CodeVar parsed = parse_variable(code(char const* baseline_message = "GENCPP_BASELINE"; ));
	expect_variable(parsed, txt("cpp parsed"));
	expect_fragments(strbuilder_to_str(code_to_strbuilder((Code)parsed)), txt("cpp parsed serialized"));

	deinit(&ctx);
	if (g_failures != 0) { log_fmt("cpp baseline smoke failed: %d\n", g_failures); return 1; }
	log_fmt("cpp baseline smoke passed\n");
	return 0;
}
