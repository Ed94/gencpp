#define GEN_TIME
#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#include "gen.cpp"

using namespace gen;

global int g_failures = 0;

internal void expect(int condition, Str label) {
	if (condition == false) { log_fmt("FAIL %S\n", label); ++g_failures; }
}

internal void expect_public(Str def, Str needle, Str label) {
	Context ctx = {}; init(&ctx);
	CodeVar parsed = parse_variable(def); expect(cast(Code, parsed) == Code_Invalid, label);
	expect(_ctx->parser.messages              != nullptr, label);
	expect(_ctx->parser.messages->Scope       == nullptr, label);
	expect(_ctx->parser.messages->Content.Ptr != nullptr, label);
	expect(str_contains(_ctx->parser.messages->Content, needle), label);

	CodeVar again = parse_variable(txt("char const* baseline_message = \"GENCPP_BASELINE\";")); expect(again.ast != nullptr, label);
	if (again.ast != nullptr) {
		expect(again->Type == CT_Variable, label);
		expect(str_contains(again->Name, txt("baseline_message")), label);
	}
	deinit(&ctx);
}

internal void expect_lex_fail(Str def, Str needle, Str label) {
	Context ctx = {}; init(&ctx);
	LexedInfo lexed = lex(&ctx, def);
	expect(lexed.tokens.ptr != nullptr, label);
	expect(lexed.messages   != nullptr, label);
	if (lexed.messages != nullptr) expect(str_contains(lexed.messages->content, needle), label);
	deinit(&ctx);
}

internal void expect_lex_ok(Str def, Str label) {
	Context ctx = {}; init(&ctx);
	LexedInfo lexed = lex(&ctx, def);
	expect(lexed.tokens.ptr != nullptr, label);
	expect(lexed.tokens.num > 0,        label);
	expect(lexed.messages   == nullptr, label);
	deinit(&ctx);
}

int main()
{
	expect_public(txt("#define FOO(... x)"), txt("Expected a ')' after '...'"),          txt("L01-public"));
	expect_public(txt("#define FOO(1)"),     txt("Expected a '_' or alpha character"),   txt("L02-public"));
	expect_public(txt("#define FOO(a b)"),   txt("Expected a comma after parameter"),    txt("L03-public"));
	expect_public(txt("#define FOO(a,"),     txt("Expected a ')' after last_parameter"), txt("L04-public"));
	expect_public(txt("#foo \\q"),           txt("in preprocessor directive ("),         txt("L05-public"));
	expect_public(txt("#include foo"),       txt("after #include"),                      txt("L06-public"));
	expect_public(txt("#pragma \\q"),        txt("in preprocessor directive '"),         txt("L07-public"));
	expect_public(txt("..x"),                txt("invalid varadic argument"),            txt("L08-public"));
	expect_public(txt("@"),                  txt("Failed to lex token"),                 txt("L09-public"));

	expect_lex_fail(txt("#define FOO(... x)"), txt("Expected a ')' after '...'"),          txt("L01-lex"));
	expect_lex_fail(txt("#define FOO(1)"),     txt("Expected a '_' or alpha character"),   txt("L02-lex"));
	expect_lex_fail(txt("#define FOO(a b)"),   txt("Expected a comma after parameter"),    txt("L03-lex"));
	expect_lex_fail(txt("#define FOO(a,"),     txt("Expected a ')' after last_parameter"), txt("L04-lex"));
	expect_lex_fail(txt("#foo \\q"),           txt("in preprocessor directive ("),         txt("L05-lex"));
	expect_lex_fail(txt("#include foo"),       txt("after #include"),                      txt("L06-lex"));
	expect_lex_fail(txt("#pragma \\q"),        txt("in preprocessor directive '"),         txt("L07-lex"));
	expect_lex_fail(txt("..x"),                txt("invalid varadic argument"),            txt("L08-lex"));
	expect_lex_fail(txt("@"),                  txt("Failed to lex token"),                 txt("L09-lex"));

	expect_lex_ok(txt("#define FOO(a, b) a"), txt("S01"));
	expect_lex_ok(txt("#define FOO(...)"),    txt("S02"));
	expect_lex_ok(txt("#include \"a.h\""),    txt("S03"));
	expect_lex_ok(txt("#pragma once"),        txt("S04"));
	expect_lex_ok(txt("#foo bar"),            txt("S05"));
	expect_lex_ok(txt("..."),                 txt("S06"));

	if (g_failures != 0) { log_fmt("lexer failures failed: %d\n", g_failures); return 1; }
	log_fmt("lexer failures passed\n");
	return 0;
}
