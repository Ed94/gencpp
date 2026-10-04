#define GEN_TIME
#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#include "gen.cpp"

using namespace gen;

global s32 g_failures = 0;

internal inline void expect(int condition, Str label) {
	if (condition == false) { log_fmt("FAIL %S\n", label); ++ g_failures; }
}

internal inline Token canary_at(Str text) {
	Token tok = NullToken;
	tok.Type   = Tok_Comma;
	tok.Text   = text;
	tok.Line   = 99;
	tok.Column = 99;
	return tok;
}

internal inline Token ident(Str text) {
	Token tok = NullToken;
	tok.Type   = Tok_Identifier;
	tok.Text   = text;
	tok.Line   = 1;
	tok.Column = 1;
	return tok;
}

internal b32 canaries_intact(Token* storage, s32 logical) {
	Token expected = canary_at(txt("CANARY"));
	for (s32 id = 0; id < 3; ++id) {
		Token got = storage[logical + id];
		if (got.Type != expected.Type || got.Text.Ptr != expected.Text.Ptr || got.Line != 99)
			return false;
	}
	return true;
}

internal inline b32 points_at_canary(Token* got, Token* storage, s32 logical) {
	return got == & storage[logical] || got == & storage[logical + 1] || got == & storage[logical + 2];
}

internal void plant(Token* storage, s32 logical, Token* src, s32 src_num) {
	for (s32 id = 0; id < src_num; ++id) storage[id]           = src[id];
	for (s32 id = 0; id < 3;       ++id) storage[logical + id] = canary_at(txt("CANARY"));
}

internal void bind(ParseContext* parser, Token* storage, s32 logical, s32 token_id) {
	parser->tokens.ptr = storage;
	parser->tokens.num = logical;
	parser->token_id   = token_id;
	parser->scope      = nullptr;
	parser->messages   = nullptr;
}

internal void expect_helper(ParseContext* parser, Token* storage, s32 logical, Str label) {
	s32 before = parser->token_id;
	Token* cur  = lex_current (parser, lex_dont_skip_formatting);
	Token* peek = lex_peek    (parser, lex_dont_skip_formatting);
	Token* next = lex_next    (parser, lex_dont_skip_formatting);
	Token* prev = lex_previous(parser, lex_dont_skip_formatting);
	expect(parser->token_id == before, label);
	expect(canaries_intact(storage, logical), label);
	expect(points_at_canary(cur,  storage, logical) == false, label);
	expect(points_at_canary(peek, storage, logical) == false, label);
	expect(points_at_canary(next, storage, logical) == false, label);
	expect(points_at_canary(prev, storage, logical) == false, label);
	expect(cur != nullptr && peek != nullptr && next != nullptr && prev != nullptr, label);
}

internal void expect_row_empty(Str label) {
	Token storage[3]    = {}; plant(storage, 0, nullptr, 0);
	ParseContext parser = {}; bind(& parser, storage, 0, 0);
	expect_helper(& parser, storage, 0, label);
	expect(lex_current (& parser, lex_dont_skip_formatting)->Type == Tok_Invalid, label);
	expect(lex_peek    (& parser, lex_dont_skip_formatting)->Type == Tok_Invalid, label);
	expect(lex_next    (& parser, lex_dont_skip_formatting)->Type == Tok_Invalid, label);
	expect(lex_previous(& parser, lex_dont_skip_formatting)->Type == Tok_Invalid, label);
}

internal void expect_row_single(Str label) {
	Token src    [1]    = { ident(txt("only")) };
	Token storage[4]    = {}; plant(storage, 1, src, 1);
	ParseContext parser = {}; bind(& parser, storage, 1, 0);
	expect_helper(& parser, storage, 1, label);
	expect(lex_current (& parser, lex_dont_skip_formatting)->Text.Ptr == src[0].Text.Ptr, label);
	expect(lex_peek    (& parser, lex_dont_skip_formatting)->Text.Ptr == src[0].Text.Ptr, label);
	expect(lex_next    (& parser, lex_dont_skip_formatting)->Type     == Tok_Invalid,     label);
	expect(lex_previous(& parser, lex_dont_skip_formatting)->Type     == Tok_Invalid,     label);
}

internal void expect_row_ends(Str label) {
	Token src    [2] = { ident(txt("first")), ident(txt("last")) };
	Token storage[5] = {}; plant(storage, 2, src, 2);

	ParseContext parser = {};
	bind(& parser, storage, 2, 0);
	expect_helper(& parser, storage, 2, label);
	expect(lex_current (& parser, lex_dont_skip_formatting)->Text.Ptr == src[0].Text.Ptr, label);
	expect(lex_next    (& parser, lex_dont_skip_formatting)->Text.Ptr == src[1].Text.Ptr, label);
	expect(lex_previous(& parser, lex_dont_skip_formatting)->Type     == Tok_Invalid,     label);

	bind(& parser, storage, 2, 1);
	expect_helper(& parser, storage, 2, label);
	expect(lex_current (& parser, lex_dont_skip_formatting)->Text.Ptr == src[1].Text.Ptr, label);
	expect(lex_next    (& parser, lex_dont_skip_formatting)->Type     == Tok_Invalid,     label);
	expect(lex_previous(& parser, lex_dont_skip_formatting)->Text.Ptr == src[0].Text.Ptr, label);

	bind(& parser, storage, 2, 2);
	expect_helper(& parser, storage, 2, label);
	expect(lex_current (& parser, lex_dont_skip_formatting)->Type     == Tok_Invalid,     label);
	expect(lex_peek    (& parser, lex_dont_skip_formatting)->Type     == Tok_Invalid,     label);
	expect(lex_next    (& parser, lex_dont_skip_formatting)->Type     == Tok_Invalid,     label);
	expect(lex_previous(& parser, lex_dont_skip_formatting)->Text.Ptr == src[1].Text.Ptr, label);
}

internal void expect_skip_end(Str label)
{
	Token newlines[2] = { NullToken, NullToken };
	newlines[0].Type = Tok_NewLine; newlines[0].Text = txt("\n");
	newlines[1].Type = Tok_NewLine; newlines[1].Text = txt("\n");
	Token storage[5] = {}; plant(storage, 2, newlines, 2);

	ParseContext parser = {}; bind(& parser, storage, 2, 0);
	Token* got = lex_current(&parser, lex_skip_formatting);
	expect(got->Type       == Tok_Invalid, label);
	expect(parser.token_id == 2,           label);
	expect(canaries_intact(storage, 2),    label);
	expect(points_at_canary(got, storage, 2) == false, label);

	bind(& parser, storage, 2, 0); got = lex_peek(&parser, lex_skip_formatting);
	expect(parser.token_id == 0,           label);
	expect(got->Type       == Tok_Invalid, label);
	expect(canaries_intact(storage, 2),    label);

	Token comments[1] = { NullToken };
	comments[0].Type = Tok_Comment; comments[0].Text = txt("// note");
	plant(storage, 1, comments, 1); bind(& parser, storage, 1, 0);
	got = lex_current(&parser, lex_skip_formatting);
	expect(got->Type       == Tok_Invalid, label);
	expect(parser.token_id == 1,           label);
	expect(canaries_intact(storage, 1),    label);
	expect(points_at_canary(got, storage, 1) == false, label);
}

internal void expect_boundary(Str def, Str label, Str mark) {
	Context ctx = {};
	init(&ctx);
	CodeVar parsed = parse_variable(def);
	expect(cast(Code, parsed) == Code_Invalid, label);
	expect(_ctx->parser.messages != nullptr, label);
	expect(_ctx->parser.messages->Scope == nullptr, label);
	expect(_ctx->parser.messages->Content.Ptr != nullptr && _ctx->parser.messages->Content.Len > 0, label);
	expect(str_contains(_ctx->parser.messages->Content, mark), label);
	CodeVar again = parse_variable(txt("char const* baseline_message = \"GENCPP_BASELINE\";"));
	expect(again.ast != nullptr, label);
	if (again.ast != nullptr) {
		expect(again->Type == CT_Variable, label);
		expect(str_contains(again->Name, txt("baseline_message")), label);
	}
	deinit(&ctx);
}

internal void expect_eat_end(Str label) {
	Context ctx = {}; init(&ctx);
	Token src    [1] = { ident(txt("x")) };
	Token storage[4] = {};
	plant(storage, 1, src, 1); bind(& ctx.parser, storage, 1, 1);
	b32 ate = lex__eat(&ctx, & ctx.parser, Tok_Identifier);
	expect(ate == false, label);
	expect(ctx.parser.token_id <= ctx.parser.tokens.num, label);
	expect(canaries_intact(storage, 1), label);
	deinit(& ctx);
}

internal void expect_direct(Str label) {
	Context ctx = {}; init(& ctx);
	Token src    [2] = { ident(txt("int")), ident(txt("name")) };
	Token storage[5] = {}; plant(storage, 2, src, 2);
	bind(& ctx.parser, storage, 2, 0);
	Code got = parse_operator_function_or_variable(& ctx, false, NullCode, NullCode);
	expect(canaries_intact(storage, 2), label);
	(void)got;

	Token op[2] = {};
	op[0]      = NullToken;
	op[0].Type = Tok_Decl_Operator;
	op[0].Text = txt("operator");
	op[0].Line = 1;
	op[1]      = ident(operator_to_str(Op_Delete));
	plant(storage, 2, op, 2);
	ctx.parser = {}; bind(& ctx.parser, storage, 2, 0);
	CodeOperator op_got = parse_operator_after_ret_type(& ctx, ModuleFlag_None, NullCode, NullCode, NullCode);
	expect(cast(Code, op_got) == Code_Invalid, label);
	expect(canaries_intact(storage, 2),        label);
	deinit(& ctx);
}

internal void expect_builder(Str label) {
	Token storage[3]    = {}; plant(storage, 0, nullptr, 0);
	ParseContext parser = {}; bind(& parser, storage, 0, 0);
	StrBuilder built = parser_to_strbuilder(& parser, _ctx->Allocator_Temp);
	Str        text  = strbuilder_to_str(built);
	expect(str_contains(text, txt("empty")), label);
	expect(canaries_intact(storage, 0),      label);
	strbuilder_free(& built);
}

int main()
{
	using namespace gen;
	Context ctx = {}; init(& ctx);
	expect_row_empty (txt("empty"));
	expect_row_single(txt("single"));
	expect_row_ends  (txt("ends"));
	expect_skip_end  (txt("skip"));
	expect_builder   (txt("builder"));
	deinit(& ctx);

	Str null_def = { nullptr, 0 };
	Str null_ptr = { nullptr, 4 };
	expect_boundary(null_def,         txt("null-len"),    txt("gen::parse_variable: length must greater than 0"));
	expect_boundary(null_ptr,         txt("null-ptr"),    txt("gen::parse_variable: def was null"));
	expect_boundary(txt(""),          txt("zero-len"),    txt("gen::parse_variable: length must greater than 0"));
	expect_boundary(txt(" \n\t"),     txt("whitespace"),  txt("no tokens found"));
	expect_boundary(txt("// note\n"), txt("comment"),     txt("only formatting tokens"));
	expect_eat_end (txt("eat"));
	expect_direct  (txt("direct"));

	if (g_failures != 0) {
		log_fmt("parser bounds failed: %d\n", g_failures); return 1;
	}
	log_fmt("parser bounds passed\n");
	return 0;
}
