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

internal b32 body_has_variable(CodeBody body, Str name) { for (Code member : body) {
	if (member->Type  == CT_Variable && member->Name.is_equal(name)) return true;
	if ((member->Type == CT_Struct || member->Type == CT_Union) && member->Body.ast != nullptr) {
		if (body_has_variable(cast(CodeBody, member->Body), name))
			return true;
	}
} return false; }

internal Str bad_at(Str good) {
	StrBuilder built = strbuilder_make_reserve(_ctx->Allocator_Temp, good.Len + 2);
	strbuilder_append_str(& built, good);
	strbuilder_append_str(& built, txt("\n@"));
	return strbuilder_to_str(built);
}

internal void expect_context_good(Context* ctx, Str label) {
	Str      def   = txt("struct Context\n{\n\tContext* parent;\n};\n\nContext* make_context();\n");
	CodeBody body  = parse_global_body(def);
	Code     found = InvalidCode;
	for (Code code : body) if (code->Type == CT_Struct && code->Name.is_equal(txt("Context")))
		found = code;
	expect(found.ast != nullptr, label); if (found.ast != nullptr) {
		Code member = InvalidCode;
		for (Code code : cast(CodeBody, found->Body)) {
			if (code->Type == CT_Variable && code->Name.is_equal(txt("parent")))
				member = code;
		}
		expect(member.ast != nullptr, label);
	}
	expect(ctx->parse_info.messages       == nullptr, label);
	expect(ctx->parse_info.lexed.messages == nullptr, label);
}

internal void expect_context_bad(Str label) {
	Context ctx = {}; init(& ctx);
	Str      good = txt("struct Context\n{\n\tContext* parent;\n};\n\nContext* make_context();\n");
	CodeBody body = parse_global_body(bad_at(good));
	Str      text = body_text(body);
	expect(str_contains(text, txt("Context")), label);
	expect(str_contains(text, txt("parent")),  label);
	expect(ctx.parse_info.lexed.messages != nullptr, label);
	if (ctx.parse_info.lexed.messages != nullptr) expect(str_contains(ctx.parse_info.lexed.messages->content, txt("Failed to lex token")), label);
	expect_context_good(&ctx, txt("context-reuse"));
	deinit(& ctx);
}

internal Str vec_def() {
	Str type      = txt("Vec2f");
	Str unit_type = txt("f32");
	return token_fmt("type", type, "unit_type", unit_type, stringize(
		struct <type>
		{
			union {
				struct {
					<unit_type> x;
					<unit_type> y;
				};
				<unit_type> Basis[2];
			};
		};
	));
}

internal void expect_vec_good(Context* ctx, Str label) {
	Str def = vec_def();
	expect(str_contains(def, txt("struct Vec2f")), txt("vec-def-struct"));
	expect(str_contains(def, txt("union")),        txt("vec-def-union"));
	expect(str_contains(def, txt("f32 x;")),       txt("vec-def-x"));
	expect(str_contains(def, txt("f32 y;")),       txt("vec-def-y"));
	expect(str_contains(def, txt("f32 Basis[2]")), txt("vec-def-Basis"));
	CodeBody body = parse_global_body(def);
	expect(body_has_variable(body, txt("x")), txt("vec-ast-x"));
	expect(body_has_variable(body, txt("y")), txt("vec-ast-y"));
	Str text = body_text(body);
	expect(str_contains(text, txt("Vec2f")), txt("vec-name"));
	expect(str_contains(text, txt("x")),     txt("vec-x"));
	expect(str_contains(text, txt("y")),     txt("vec-y"));
	expect(ctx->parse_info.messages       == nullptr, txt("vec-parser-messages"));
	expect(ctx->parse_info.lexed.messages == nullptr, txt("vec-lexer-messages"));
}

internal void expect_vec_bad(Str label) {
	Context ctx = {}; init(& ctx);
	CodeBody body = parse_global_body(bad_at(vec_def()));
	Str      text = body_text(body);
	expect(str_contains(text, txt("Vec2f")), label);
	expect(ctx.parse_info.lexed.messages != nullptr, label);
	if (ctx.parse_info.lexed.messages != nullptr) expect(str_contains(ctx.parse_info.lexed.messages->content, txt("Failed to lex token")), label);
	expect_vec_good(& ctx, txt("vec-reuse"));
	deinit(& ctx);
}

internal Str unreal_def() { return txt("UCLASS()\nclass UGasaFixture\n{\n\tUPROPERTY()\n\tint Health;\n};\n"); }

internal void expect_unreal_good(Context* ctx, Str label) {
	CodeBody body  = parse_global_body(unreal_def());
	Code     found = InvalidCode;
	for (Code code : body) if (code->Type == CT_Class && code->Name.is_equal(txt("UGasaFixture")))
		found = code;
	expect(found.ast != nullptr, label); if (found.ast != nullptr) {
		b32 saw_prop   = false;
		b32 saw_health = false;
		for (Code member : cast(CodeBody, found->Body)) {
			if (            member->Type == CT_Untyped  && member->Name.starts_with(txt("UPROPERTY"))) saw_prop   = true;
			if (saw_prop && member->Type == CT_Variable && member->Name.is_equal(txt("Health")))       saw_health = true;
		}
		expect(saw_prop,   label);
		expect(saw_health, label);
	}
	expect(ctx->parse_info.messages       == nullptr, label);
	expect(ctx->parse_info.lexed.messages == nullptr, label);
}

internal void expect_unreal_bad(Str label) {
	Context ctx = {}; init(& ctx);
	register_macros(args(
		(Macro { txt("UCLASS"),    MT_Statement, MF_Functional }),
		(Macro { txt("UPROPERTY"), MT_Statement, MF_Functional })
	));
	CodeBody body = parse_global_body(bad_at(unreal_def()));
	Str      text = body_text(body);
	expect(str_contains(text, txt("UGasaFixture")), label);
	expect(ctx.parse_info.lexed.messages != nullptr, label);
	if (ctx.parse_info.lexed.messages != nullptr) expect(str_contains(ctx.parse_info.lexed.messages->content, txt("Failed to lex token")), label);
	expect_unreal_good(&ctx, txt("unreal-reuse"));
	deinit(& ctx);
}

int main()
{
	Context ctx = {}; init(& ctx);
	expect_context_good(&ctx, txt("context"));
	deinit(& ctx);
	expect_context_bad(txt("context-at"));

	init(& ctx);
	expect_vec_good(& ctx, txt("vec"));
	deinit(& ctx);
	expect_vec_bad(txt("vec-at"));

	init(& ctx);
	register_macros(args(
		(Macro { txt("UCLASS"),    MT_Statement, MF_Functional }),
		(Macro { txt("UPROPERTY"), MT_Statement, MF_Functional })
	));
	expect_unreal_good(& ctx, txt("unreal"));
	deinit(& ctx);
	expect_unreal_bad(txt("unreal-at"));

	if (g_failures != 0) { log_fmt("dogfood parse fixtures failed: %d\n", g_failures); return 1; }
	log_fmt("dogfood parse fixtures passed\n");
	return 0;
}
