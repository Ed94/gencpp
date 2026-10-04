#define GEN_TIME
#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#include "gen.cpp"

using namespace gen;

global int g_failures = 0;

internal void expect(int condition, Str label) {
	if (condition == false) { log_fmt("FAIL %S\n", label); ++g_failures; }
}

internal Str body_text(CodeBody body) {
	return strbuilder_to_str(code_to_strbuilder(cast(Code, body)));
}

internal void expect_good_base(Context* ctx, Str label) {
	ParseInfo info = {};
	Str       def  = txt("struct Context { int a; int b; };");
	parse_global_body_base(ctx, def, &info);
	CodeBody body = cast(CodeBody, info.result);
	Str      text = body_text(body);
	expect(body.ast != nullptr, label);
	expect(str_contains(text, txt("Context")), label);
	expect(str_contains(text, txt("a")), label);
	expect(str_contains(text, txt("b")), label);
	expect(info.messages == nullptr, label);
	expect(info.lexed.messages == nullptr, label);
	expect(info.lexed.tokens.ptr != nullptr, label);
	expect(info.lexed.tokens.num > 0, label);
}

internal void expect_at(Str label) {
	Context   ctx  = {};
	ParseInfo info = {};
	Str       def  = txt("struct Context { int a; int b; };\n@");
	init(&ctx);
	parse_global_body_base(&ctx, def, &info);
	CodeBody body = cast(CodeBody, info.result);
	Str      text = body_text(body);
	expect(body.ast != nullptr, label);
	expect(str_contains(text, txt("Context")), label);
	expect(info.lexed.messages != nullptr, label);
	if (info.lexed.messages != nullptr)
		expect(str_contains(info.lexed.messages->content, txt("Failed to lex token")), label);
	expect(info.lexed.tokens.ptr != nullptr, label);
	expect(info.lexed.tokens.num > 0, label);
	expect_good_base(&ctx, txt("at-reuse"));
	deinit(&ctx);
}

internal void expect_comma(Str label) {
	Context   ctx  = {};
	ParseInfo info = {};
	Str       def  = txt("struct Context { int a; int b; };\n,");
	init(&ctx);
	parse_global_body_base(&ctx, def, &info);
	CodeBody body = cast(CodeBody, info.result);
	Str      text = body_text(body);
	expect(str_contains(text, txt("Context")), label);
	expect(info.messages != nullptr, label);
	if (info.messages != nullptr)
		expect(str_contains(info.messages->Content, txt("Dangling comma")), label);
	expect_good_base(&ctx, txt("comma-reuse"));
	deinit(&ctx);
}

internal void expect_space(Str label) {
	Context   ctx  = {};
	ParseInfo info = {};
	init(&ctx);
	parse_global_body_base(&ctx, txt(" \t\n"), &info);
	CodeBody body = cast(CodeBody, info.result);
	expect(body.ast != nullptr, label);
	expect(body->Type == CT_Global_Body, label);
	expect(info.lexed.tokens.num == 0, label);
	expect(info.lexed.messages != nullptr, label);
	if (info.lexed.messages != nullptr)
		expect(str_contains(info.lexed.messages->content, txt("no tokens found")), label);
	expect_good_base(&ctx, txt("space-reuse"));
	deinit(&ctx);
}

internal void expect_empty(Str label) {
	Context   ctx  = {};
	ParseInfo info = {};
	Str       def  = { nullptr, 0 };
	init(&ctx);
	parse_global_body_base(&ctx, def, &info);
	CodeBody body = cast(CodeBody, info.result);
	expect(body.ast != nullptr, label);
	expect(info.messages != nullptr, label);
	if (info.messages != nullptr)
		expect(str_contains(info.messages->Content, txt("length must greater than 0")), label);
	expect_good_base(&ctx, txt("empty-reuse"));
	deinit(&ctx);
}

internal void expect_wrapper(Str label) {
	Context  ctx  = {};
	CodeBody body;
	Str      text;
	init(&ctx);
	body = parse_global_body(txt("struct Context { int a; int b; };"));
	text = body_text(body);
	expect(str_contains(text, txt("Context")), label);
	expect(ctx.parse_info.messages == nullptr, label);
	expect(str_contains(body_text(cast(CodeBody, ctx.parse_info.result)), txt("Context")), label);
	deinit(&ctx);
}

int main()
{
	Context ctx = {}; init(& ctx);
	expect_good_base(&ctx, txt("good"));
	deinit(& ctx);
	expect_at(txt("at"));
	expect_comma(txt("comma"));
	expect_space(txt("space"));
	expect_empty(txt("empty"));
	expect_wrapper(txt("wrapper"));
	if (g_failures != 0) { log_fmt("parse body messages failed: %d\n", g_failures); return 1; }
	log_fmt("parse body messages passed\n");
	return 0;
}
