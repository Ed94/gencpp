#define GEN_DONT_USE_FATAL
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
	expect(var->Value.ast     != nullptr, label);
	if (var->Value.ast != nullptr) {
		expect(gen::str_contains(var->Value->Content, txt("GENCPP_BASELINE")), label);
	}
}

internal void expect_parse_result(gen::Code result, gen::ParseInfo* info, gen::CodeType expected, gen::Str label) {
	expect(result.ast != nullptr, label);
	if (result.ast != nullptr) expect(result->Type == expected, label);
	expect(info->result.ast == result.ast, label);
	expect(info->lexed.tokens.num > 0, label);
	expect(info->lexed.messages == nullptr && info->messages == nullptr, label);
}

#include "dependencies/strings.cpp"
#include "dependencies/simd.cpp"

int main()
{
	using namespace gen;
	Context ctx = {}; init(&ctx);
	expect_text_template(GEN_PRINTF_MAXLEN);
	expect_simd_lanes();

	Opts_def_type type_opts = {};
	type_opts.specifiers = def_specifiers(args(Spec_Const, Spec_Ptr));
	CodeTypename type    = def_type(name(char), type_opts);
	Opts_def_variable opts = {}; opts.value = untyped_str(code("\"GENCPP_BASELINE\""));
	CodeVar upfront = def_variable(type, name(baseline_message), opts);
	expect_variable(upfront, txt("cpp upfront"));
	expect_fragments(strbuilder_to_str(code_to_strbuilder((Code)upfront)), txt("cpp upfront serialized"));

	Opts_def_type type_options = {};
	Code           array_expr  = untyped_str(txt("3"));
	CodeSpecifiers type_specs  = def_specifier(Spec_Const);
	CodeAttributes type_attrs  = def_attributes(txt("[[maybe_unused]]"));
	type_options.type_tag   = Tag_Struct;
	type_options.array_expr = array_expr;
	type_options.specifiers = type_specs;
	type_options.attributes = type_attrs;
	CodeTypename option_type = def_type(name(Payload), type_options);
	expect(option_type.ast != nullptr, txt("cpp type options"));
	if (option_type.ast != nullptr) {
		expect(option_type->TypeTag        == Tag_Struct,     txt("cpp type tag"));
		expect(option_type->ArrExpr.ast    == array_expr.ast, txt("cpp type array expression"));
		expect(option_type->Specs.ast      == type_specs.ast, txt("cpp type specifiers"));
		expect(option_type->Attributes.ast == type_attrs.ast, txt("cpp type attributes"));
		Str type_text = strbuilder_to_str(code_to_strbuilder((Code)option_type));
		expect(str_contains(type_text, txt("struct Payload")),   txt("cpp type tag serialization"));
		expect(str_contains(type_text, txt("const")),            txt("cpp type specifier serialization"));
		expect(str_contains(type_text, txt("[[maybe_unused]]")), txt("cpp type attribute serialization"));
	}

	Opts_def_variable variable_options = {};
	Code           value          = untyped_str(code("\"UPFRONT_OPTION\""));
	CodeSpecifiers variable_specs = def_specifier(Spec_Static);
	CodeAttributes variable_attrs = def_attributes(txt("[[maybe_unused]]"));
	variable_options.value      = value;
	variable_options.specifiers = variable_specs;
	variable_options.attributes = variable_attrs;
	variable_options.mflags     = ModuleFlag_Export;
	CodeVar option_variable = def_variable(option_type, name(option_payload), variable_options);
	expect(option_variable.ast != nullptr, txt("cpp variable options"));
	if (option_variable.ast != nullptr) {
		expect(option_variable->ValueType.ast  == option_type.ast,    txt("cpp variable type"));
		expect(option_variable->Value.ast      == value.ast,          txt("cpp variable value"));
		expect(option_variable->Specs.ast      == variable_specs.ast, txt("cpp variable specifiers"));
		expect(option_variable->Attributes.ast == variable_attrs.ast, txt("cpp variable attributes"));
		expect(option_variable->ModuleFlags    == ModuleFlag_Export,  txt("cpp variable module flags"));
		Str variable_text = strbuilder_to_str(code_to_strbuilder((Code)option_variable));
		expect(str_contains(variable_text, txt("option_payload")),   txt("cpp variable name serialization"));
		expect(str_contains(variable_text, txt("UPFRONT_OPTION")),   txt("cpp variable value serialization"));
		expect(str_contains(variable_text, txt("static")),           txt("cpp variable specifier serialization"));
		expect(str_contains(variable_text, txt("[[maybe_unused]]")), txt("cpp variable attribute serialization"));
		expect(str_contains(variable_text, txt("[ 3 ]")),            txt("cpp variable array serialization"));
		expect(str_contains(variable_text, txt("export")),           txt("cpp variable module flag serialization"));
	}

	CodeVar  class_member = def_variable(def_type(name(u32)), name(class_option_member));
	CodeBody class_body   = def_class_body(args(class_member));
	CodeTypename class_parent       = def_type(name(ClassOptionBase));
	CodeTypename class_interfaces[] = { def_type(name(IClassOption)) };
	Opts_def_struct class_options = {};
	class_options.body           = class_body;
	class_options.parent         = class_parent;
	class_options.parent_access  = AccessSpec_Public;
	class_options.attributes     = def_attributes(txt("[[maybe_unused]]"));
	class_options.interfaces     = class_interfaces;
	class_options.num_interfaces = 1;
	class_options.specifiers     = def_specifier(Spec_Final);
	class_options.mflags         = ModuleFlag_Export;
	CodeClass option_class = def_class(name(ClassOption), class_options);
	expect(option_class.ast != nullptr, txt("cpp class options"));
	if (option_class.ast != nullptr) {
		expect(option_class->Body.ast             == class_body.ast,                  txt("cpp class body"));
		expect(option_class->ParentType.ast       == class_parent.ast,                txt("cpp class parent"));
		expect(option_class->ParentAccess         == AccessSpec_Public,               txt("cpp class parent access"));
		expect(option_class->Attributes.ast       == class_options.attributes.ast,    txt("cpp class attributes"));
		expect(option_class->ParentType->Next.ast == ((Code)class_interfaces[0]).ast, txt("cpp class interface"));
		expect(option_class->Specs.ast            == class_options.specifiers.ast,    txt("cpp class specifiers"));
		expect(option_class->ModuleFlags          == ModuleFlag_Export,               txt("cpp class module flags"));
		Str class_text = strbuilder_to_str(code_to_strbuilder((Code)option_class));
		expect(str_contains(class_text, txt("ClassOption")),         txt("cpp class name serialization"));
		expect(str_contains(class_text, txt("ClassOptionBase")),     txt("cpp class parent serialization"));
		expect(str_contains(class_text, txt("IClassOption")),        txt("cpp class interface serialization"));
		expect(str_contains(class_text, txt("public")),              txt("cpp class access serialization"));
		expect(str_contains(class_text, txt("class_option_member")), txt("cpp class body serialization"));
		expect(str_contains(class_text, txt("[[maybe_unused]]")),    txt("cpp class attribute serialization"));
		expect(str_contains(class_text, txt("final")),               txt("cpp class final specifier serialization"));
		expect(str_contains(class_text, txt("export")),              txt("cpp class module flag serialization"));
	}

	CodeVar      struct_member       = def_variable(def_type(name(u32)), name(struct_option_member));
	CodeBody     struct_body         = def_struct_body(args(struct_member));
	CodeTypename struct_parent       = def_type(name(StructOptionBase));
	CodeTypename struct_interfaces[] = { def_type(name(IStructOption)) };
	Opts_def_struct struct_options = {};
	struct_options.body           = struct_body;
	struct_options.parent         = struct_parent;
	struct_options.parent_access  = AccessSpec_Public;
	struct_options.attributes     = def_attributes(txt("[[maybe_unused]]"));
	struct_options.interfaces     = struct_interfaces;
	struct_options.num_interfaces = 1;
	struct_options.specifiers     = def_specifier(Spec_Final);
	struct_options.mflags         = ModuleFlag_Export;
	CodeStruct option_struct = def_struct(name(StructOption), struct_options);
	expect(option_struct.ast != nullptr, txt("cpp struct options"));
	if (option_struct.ast != nullptr) {
		expect(option_struct->Body.ast             == struct_body.ast,                  txt("cpp struct body"));
		expect(option_struct->ParentType.ast       == struct_parent.ast,                txt("cpp struct parent"));
		expect(option_struct->ParentAccess         == AccessSpec_Public,                txt("cpp struct parent access"));
		expect(option_struct->Attributes.ast       == struct_options.attributes.ast,    txt("cpp struct attributes"));
		expect(option_struct->ParentType->Next.ast == ((Code)struct_interfaces[0]).ast, txt("cpp struct interface"));
		expect(option_struct->Specs.ast            == struct_options.specifiers.ast,    txt("cpp struct specifiers"));
		expect(option_struct->ModuleFlags          == ModuleFlag_Export,                txt("cpp struct module flags"));
		Str struct_text = strbuilder_to_str(code_to_strbuilder((Code)option_struct));
		expect(str_contains(struct_text, txt("StructOption")),         txt("cpp struct name serialization"));
		expect(str_contains(struct_text, txt("StructOptionBase")),     txt("cpp struct parent serialization"));
		expect(str_contains(struct_text, txt("IStructOption")),        txt("cpp struct interface serialization"));
		expect(str_contains(struct_text, txt("public")),               txt("cpp struct access serialization"));
		expect(str_contains(struct_text, txt("struct_option_member")), txt("cpp struct body serialization"));
		expect(str_contains(struct_text, txt("[[maybe_unused]]")),     txt("cpp struct attribute serialization"));
		expect(str_contains(struct_text, txt("final")),                txt("cpp struct final specifier serialization"));
		expect(str_contains(struct_text, txt("export")),               txt("cpp struct module flag serialization"));
	}

	CodeBody enum_body = def_enum_body(args(untyped_str(txt("Option_None"))));
	Opts_def_enum enum_options = {};
	enum_options.body       = enum_body;
	enum_options.type       = def_type(name(u32));
	enum_options.specifier  = EnumDecl_Class;
	enum_options.attributes = def_attributes(txt("[[maybe_unused]]"));
	enum_options.mflags     = ModuleFlag_Export;
	CodeEnum option_enum = def_enum(name(OptionEnum), enum_options);
	expect(option_enum.ast != nullptr, txt("cpp enum options"));
	if (option_enum.ast != nullptr) {
		expect(option_enum->Body.ast           == enum_body.ast,               txt("cpp enum body"));
		expect(option_enum->UnderlyingType.ast == enum_options.type.ast,       txt("cpp enum underlying type"));
		expect(option_enum->Type               == CT_Enum_Class,               txt("cpp enum specifier"));
		expect(option_enum->Attributes.ast     == enum_options.attributes.ast, txt("cpp enum attributes"));
		expect(option_enum->ModuleFlags        == ModuleFlag_Export,           txt("cpp enum module flags"));
		Str enum_text = strbuilder_to_str(code_to_strbuilder((Code)option_enum));
		expect(str_contains(enum_text, txt("export")),           txt("cpp enum module flag serialization"));
		expect(str_contains(enum_text, txt("enum class")),       txt("cpp enum specifier serialization"));
		expect(str_contains(enum_text, txt("[[maybe_unused]]")), txt("cpp enum attribute serialization"));
		expect(str_contains(enum_text, txt("u32")),              txt("cpp enum type serialization"));
		expect(str_contains(enum_text, txt("Option_None")),      txt("cpp enum body serialization"));
	}
	Code           enum_macro = untyped_str(txt("ENUM_BASE_TYPE"));
	CodeEnum macro_enum = def_enum(name(MacroOptionEnum), {
		.body       = def_enum_body(args(untyped_str(txt("Macro_None")))),
		.type_macro = enum_macro,
	});
	expect(macro_enum.ast != nullptr, txt("cpp enum type macro option"));
	if (macro_enum.ast != nullptr) {
		expect(macro_enum->UnderlyingTypeMacro.ast == enum_macro.ast, txt("cpp enum type macro"));
		expect(str_contains(strbuilder_to_str(code_to_strbuilder((Code)macro_enum)), txt("ENUM_BASE_TYPE")), txt("cpp enum type macro serialization"));
	}

	CodeBody union_body = def_union_body(args(untyped_str(txt("u32 union_option_value;"))));
	Opts_def_union union_options = {};
	union_options.attributes = def_attributes(txt("[[maybe_unused]]"));
	union_options.mflags     = ModuleFlag_Export;
	CodeUnion option_union = def_union(name(OptionUnion), union_body, union_options);
	expect(option_union.ast != nullptr, txt("cpp union options"));
	if (option_union.ast != nullptr) {
		expect(option_union->Body.ast       == union_body.ast,               txt("cpp union body"));
		expect(option_union->Attributes.ast == union_options.attributes.ast, txt("cpp union attributes"));
		expect(option_union->ModuleFlags    == ModuleFlag_Export,            txt("cpp union module flags"));
		Str union_text = strbuilder_to_str(code_to_strbuilder((Code)option_union));
		expect(str_contains(union_text, txt("OptionUnion")),        txt("cpp union name serialization"));
		expect(str_contains(union_text, txt("union_option_value")), txt("cpp union body serialization"));
		expect(str_contains(union_text, txt("[[maybe_unused]]")),   txt("cpp union attribute serialization"));
		expect(str_contains(union_text, txt("export")),             txt("cpp union module flag serialization"));
	}

	CodeAttributes typedef_attrs = def_attributes(txt("[[maybe_unused]]"));
	Code           typedef_type  = (Code)def_type(name(u32));
	Opts_def_typedef typedef_options = {};
	typedef_options.attributes = typedef_attrs;
	typedef_options.mflags     = ModuleFlag_Export;
	CodeTypedef option_typedef = def_typedef(name(OptionTypedef), typedef_type, typedef_options);
	expect(option_typedef.ast != nullptr, txt("cpp typedef options"));
	if (option_typedef.ast != nullptr) {
		expect(option_typedef->UnderlyingType.ast == typedef_type.ast,  txt("cpp typedef underlying type"));
		expect(option_typedef->Attributes.ast     == typedef_attrs.ast, txt("cpp typedef attributes"));
		expect(option_typedef->ModuleFlags        == ModuleFlag_Export, txt("cpp typedef module flags"));
		Str typedef_text = strbuilder_to_str(code_to_strbuilder((Code)option_typedef));
		expect(str_contains(typedef_text, txt("OptionTypedef")),    txt("cpp typedef name serialization"));
		expect(str_contains(typedef_text, txt("u32")),              txt("cpp typedef type serialization"));
		expect(str_contains(typedef_text, txt("[[maybe_unused]]")), txt("cpp typedef attribute serialization"));
		expect(str_contains(typedef_text, txt("export")),           txt("cpp typedef module flag serialization"));
	}

	Opts_def_using using_options = {};
	using_options.attributes = def_attributes(txt("[[maybe_unused]]"));
	using_options.mflags     = ModuleFlag_Export;
	CodeTypename using_type   = def_type(name(u32));
	CodeUsing    option_using = def_using(name(OptionUsing), using_type, using_options);
	expect(option_using.ast != nullptr, txt("cpp using options"));
	if (option_using.ast != nullptr) {
		expect(option_using->UnderlyingType.ast == using_type.ast,               txt("cpp using underlying type"));
		expect(option_using->Attributes.ast     == using_options.attributes.ast, txt("cpp using attributes"));
		expect(option_using->ModuleFlags        == ModuleFlag_Export,            txt("cpp using module flags"));
		Str using_text = strbuilder_to_str(code_to_strbuilder((Code)option_using));
		expect(str_contains(using_text, txt("OptionUsing")),      txt("cpp using name serialization"));
		expect(str_contains(using_text, txt("[[maybe_unused]]")), txt("cpp using attribute serialization"));
		expect(str_contains(using_text, txt("export")),           txt("cpp using module flag serialization"));
	}

	CodeParams   template_params = def_param(def_type(name(class)), name(T));
	CodeTemplate option_template = def_template(template_params, (Code)option_struct, { .mflags = ModuleFlag_Export });
	expect(option_template.ast != nullptr, txt("cpp template options"));
	if (option_template.ast != nullptr) {
		expect(option_template->Params.ast      == template_params.ast,       txt("cpp template parameters"));
		expect(option_template->Declaration.ast == ((Code)option_struct).ast, txt("cpp template declaration"));
		expect(option_template->ModuleFlags     == ModuleFlag_Export,         txt("cpp template module flags"));
		Str template_text = strbuilder_to_str(code_to_strbuilder((Code)option_template));
		expect(str_contains(template_text, txt("export")),       txt("cpp template module flag serialization"));
		expect(str_contains(template_text, txt("template")),     txt("cpp template serialization"));
		expect(str_contains(template_text, txt("StructOption")), txt("cpp template declaration serialization"));
	}

	Macro source_macro = { txt("OptionParamSource"), MT_Expression, MF_Functional };
	register_macro(source_macro);
	CodeDefine parsed_macro_source = parse_define(txt("#define OptionParamSource(param) param\n"));
	expect(parsed_macro_source.ast != nullptr, txt("cpp define parameter source"));
	Opts_def_define define_options = {};
	define_options.params   = parsed_macro_source->Params;
	define_options.content  = txt("param + 1");
	define_options.flags    = MF_Functional | MF_Allow_As_Identifier;
	CodeDefine option_define = def_define(name(OptionMacro), MT_Expression, define_options);
	expect(option_define.ast != nullptr, txt("cpp define options"));
	if (option_define.ast != nullptr) {
		expect(option_define->Params.ast == define_options.params.ast,       txt("cpp define parameters"));
		expect(str_contains(option_define->Body->Content, txt("param + 1")), txt("cpp define content"));
		Macro* option_macro = lookup_macro(name(OptionMacro));
		expect(option_macro != nullptr, txt("cpp define registration"));
		if (option_macro != nullptr) expect(option_macro->Flags == define_options.flags, txt("cpp define flags"));
		Str define_text = strbuilder_to_str(code_to_strbuilder((Code)option_define));
		expect(str_contains(define_text, txt("OptionMacro")),            txt("cpp define name serialization"));
		expect(str_contains(define_text, txt("OptionMacro( param)")),    txt("cpp define parameters serialization"));
		expect(str_contains(define_text, txt("param + 1")),              txt("cpp define content serialization"));
	}
	Opts_def_define no_register_options = {};
	no_register_options.dont_register_to_preprocess_macros = true;
	CodeDefine no_register_define = def_define(name(OptionMacroNoRegister), MT_Expression, no_register_options);
	expect(no_register_define.ast != nullptr, txt("cpp define registration suppression"));
	expect(lookup_macro(name(OptionMacroNoRegister)) == nullptr, txt("cpp define suppression flag"));

	CodeInclude local_include = def_include(txt("option_local.h"));
	expect(local_include.ast != nullptr, txt("cpp local include option"));
	if (local_include.ast != nullptr) {
		expect(str_contains(local_include->Content, txt("\"option_local.h\"")), txt("cpp quoted include"));
	}
	Opts_def_include foreign_include_options = {};
	foreign_include_options.foreign = true;
	CodeInclude foreign_include = def_include(txt("option_system.h"), foreign_include_options);
	expect(foreign_include.ast != nullptr, txt("cpp foreign include option"));
	if (foreign_include.ast != nullptr) {
		expect(str_contains(foreign_include->Content, txt("<option_system.h>")), txt("cpp foreign include path"));
		Str include_text = strbuilder_to_str(code_to_strbuilder((Code)foreign_include));
		expect(str_contains(include_text, txt("#include <option_system.h>")), txt("cpp foreign include directive serialization"));
	}

	CodeModule option_module = def_module(name(OptionModule), { .mflags = ModuleFlag_Export });
	expect(option_module.ast != nullptr, txt("cpp module options"));
	if (option_module.ast != nullptr) {
		expect(option_module->ModuleFlags == ModuleFlag_Export, txt("cpp module flags"));
		expect(str_contains(strbuilder_to_str(code_to_strbuilder((Code)option_module)), txt("export")), txt("cpp module serialization"));
	}
	CodeVar  namespace_member = def_variable(def_type(name(u32)), name(namespace_option_value));
	CodeBody namespace_body   = def_namespace_body(args(namespace_member));
	CodeNS   option_namespace = def_namespace(name(OptionNamespace), namespace_body, { .mflags = ModuleFlag_Export });
	expect(option_namespace.ast != nullptr, txt("cpp namespace options"));
	if (option_namespace.ast != nullptr) {
		expect(option_namespace->Body.ast    == namespace_body.ast, txt("cpp namespace body"));
		expect(option_namespace->ModuleFlags == ModuleFlag_Export,  txt("cpp namespace flags"));
		Str namespace_text = strbuilder_to_str(code_to_strbuilder((Code)option_namespace));
		expect(str_contains(namespace_text, txt("OptionNamespace")),        txt("cpp namespace name serialization"));
		expect(str_contains(namespace_text, txt("namespace_option_value")), txt("cpp namespace body serialization"));
		expect(str_contains(namespace_text, txt("export")),                 txt("cpp namespace serialization"));
	}

	Code           parameter_value = untyped_str(code(7));
	Opts_def_param parameter_options = {};
	parameter_options.value = parameter_value;
	CodeParams option_parameter = def_param(def_type(name(u32)), name(option_parameter_value), parameter_options);
	expect(option_parameter.ast != nullptr, txt("cpp parameter options"));
	if (option_parameter.ast != nullptr) {
		expect(option_parameter->Value.ast == parameter_value.ast, txt("cpp parameter value"));
		Str parameter_text = strbuilder_to_str(code_to_strbuilder((Code)option_parameter));
		expect(str_contains(parameter_text, txt("= 7")), txt("cpp parameter value serialization"));
	}

	CodeParams   function_params = def_param(def_type(name(u32)), name(function_option_arg));
	CodeTypename function_return = def_type(name(FunctionOptionReturn));
	CodeBody     function_body   = def_function_body(args(untyped_str(txt("return function_option_arg;"))));
	Opts_def_function function_options = {};
	function_options.params   = function_params;
	function_options.ret_type = function_return;
	function_options.body     = function_body;
	function_options.specs    = def_specifier(Spec_Inline);
	function_options.attrs    = def_attributes(txt("[[maybe_unused]]"));
	function_options.mflags   = ModuleFlag_Export;
	CodeFn option_function = def_function(name(OptionFunction), function_options);
	expect(option_function.ast != nullptr, txt("cpp function options"));
	if (option_function.ast != nullptr) {
		expect(option_function->Params.ast     == function_params.ast,        txt("cpp function parameters"));
		expect(option_function->ReturnType.ast == function_return.ast,        txt("cpp function return type"));
		expect(option_function->Body.ast       == function_body.ast,          txt("cpp function body"));
		expect(option_function->Specs.ast      == function_options.specs.ast, txt("cpp function specifiers"));
		expect(option_function->Attributes.ast == function_options.attrs.ast, txt("cpp function attributes"));
		expect(option_function->ModuleFlags    == ModuleFlag_Export,          txt("cpp function module flags"));
		Str function_text = strbuilder_to_str(code_to_strbuilder((Code)option_function));
		expect(str_contains(function_text, txt("OptionFunction")),             txt("cpp function name serialization"));
		expect(str_contains(function_text, txt("FunctionOptionReturn OptionFunction(")), txt("cpp function return type serialization"));
		expect(str_contains(function_text, txt("function_option_arg")),        txt("cpp function parameters serialization"));
		expect(str_contains(function_text, txt("return function_option_arg")), txt("cpp function body serialization"));
		expect(str_contains(function_text, txt("inline")),                     txt("cpp function specifier serialization"));
		expect(str_contains(function_text, txt("[[maybe_unused]]")),           txt("cpp function attribute serialization"));
		expect(str_contains(function_text, txt("export")),                     txt("cpp function module flag serialization"));
	}

	CodeParams constructor_params = def_param(def_type(name(u32)), name(constructor_option_arg));
	Code       initializer_list   = untyped_str(txt("option_member( 0 )"));
	CodeBody   constructor_body   = def_function_body(args(untyped_str(txt("option_member = constructor_option_arg;"))));
	Opts_def_constructor constructor_options = {};
	constructor_options.params           = constructor_params;
	constructor_options.initializer_list = initializer_list;
	constructor_options.body             = constructor_body;
	CodeConstructor option_constructor = def_constructor(constructor_options);
	expect(option_constructor.ast != nullptr, txt("cpp constructor options"));
	if (option_constructor.ast != nullptr) {
		expect(option_constructor->Params.ast          == constructor_params.ast,       txt("cpp constructor parameters"));
		expect(option_constructor->InitializerList.ast == initializer_list.ast,         txt("cpp constructor initializer list"));
		expect(option_constructor->Body.ast            == ((Code)constructor_body).ast, txt("cpp constructor body"));
	}
	CodeBody  constructor_class_body = def_class_body(args(option_constructor));
	CodeClass constructor_class      = def_class(name(ConstructorOptions), { .body = constructor_class_body });
	expect(constructor_class.ast != nullptr, txt("cpp constructor serialization parent"));
	if (constructor_class.ast != nullptr) {
		Str constructor_text = strbuilder_to_str(code_to_strbuilder((Code)constructor_class));
		expect(str_contains(constructor_text, txt("ConstructorOptions(")),        txt("cpp constructor name serialization"));
		expect(str_contains(constructor_text, txt("u32 constructor_option_arg")), txt("cpp constructor parameter serialization"));
		expect(str_contains(constructor_text, txt("option_member( 0 )")),         txt("cpp constructor initializer serialization"));
		expect(str_contains(constructor_text, txt("constructor_option_arg")),     txt("cpp constructor body serialization"));
	}
	Code           destructor_body  = untyped_str(txt("release_option_resource();"));
	CodeSpecifiers destructor_specs = def_specifier(Spec_Virtual);
	Opts_def_destructor destructor_options = {};
	destructor_options.body       = destructor_body;
	destructor_options.specifiers = destructor_specs;
	CodeDestructor option_destructor = def_destructor(destructor_options);
	expect(option_destructor.ast != nullptr, txt("cpp destructor options"));
	if (option_destructor.ast != nullptr) {
		expect(option_destructor->Body.ast  == destructor_body.ast,  txt("cpp destructor body"));
		expect(option_destructor->Specs.ast == destructor_specs.ast, txt("cpp destructor specifiers"));
		CodeBody destructor_class_body = def_class_body(args(option_destructor));
		CodeClass destructor_class = def_class(name(DestructorOptions), { .body = destructor_class_body });
		expect(destructor_class.ast != nullptr, txt("cpp destructor serialization parent"));
		if (destructor_class.ast != nullptr) {
			Str destructor_text = strbuilder_to_str(code_to_strbuilder((Code)destructor_class));
			expect(str_contains(destructor_text, txt("virtual ~DestructorOptions()")), txt("cpp destructor signature serialization"));
			expect(str_contains(destructor_text, txt("release_option_resource();")),   txt("cpp destructor body serialization"));
		}
	}

	CodeTypename operator_type   = def_type(name(OperatorOptionType));
	CodeTypename operator_return = def_type(name(OperatorOptionReturn));
	CodeParams   operator_lhs    = def_param(operator_type, name(operator_option_lhs));
	CodeParams   operator_rhs    = def_param(operator_type, name(operator_option_rhs));
	CodeParams   operator_params = def_params(args(operator_lhs, operator_rhs));
	CodeBody     operator_body   = def_function_body(args(untyped_str(txt("return operator_option_lhs;"))));
	Opts_def_operator operator_options = {};
	operator_options.params     = operator_params;
	operator_options.ret_type   = operator_return;
	operator_options.body       = operator_body;
	operator_options.specifiers = def_specifier(Spec_Inline);
	operator_options.attributes = def_attributes(txt("[[maybe_unused]]"));
	operator_options.mflags     = ModuleFlag_Export;
	CodeOperator option_operator = def_operator(Op_Add, txt(""), operator_options);
	expect(option_operator.ast != nullptr, txt("cpp operator options"));
	if (option_operator.ast != nullptr) {
		expect(option_operator->Params.ast     == operator_params.ast,             txt("cpp operator parameters"));
		expect(option_operator->ReturnType.ast == operator_return.ast,             txt("cpp operator return type"));
		expect(option_operator->Body.ast       == operator_body.ast,               txt("cpp operator body"));
		expect(option_operator->Specs.ast      == operator_options.specifiers.ast, txt("cpp operator specifiers"));
		expect(option_operator->Attributes.ast == operator_options.attributes.ast, txt("cpp operator attributes"));
		expect(option_operator->ModuleFlags    == ModuleFlag_Export,               txt("cpp operator module flags"));
		Str operator_text = strbuilder_to_str(code_to_strbuilder((Code)option_operator));
		expect(str_contains(operator_text, txt("OperatorOptionReturn")),       txt("cpp operator return type serialization"));
		expect(str_contains(operator_text, txt("operator +")),                 txt("cpp operator name serialization"));
		expect(str_contains(operator_text, txt("operator_option_lhs")),        txt("cpp operator parameters serialization"));
		expect(str_contains(operator_text, txt("operator_option_rhs")),        txt("cpp operator second parameter serialization"));
		expect(str_contains(operator_text, txt("return operator_option_lhs")), txt("cpp operator body serialization"));
		expect(str_contains(operator_text, txt("inline")),                     txt("cpp operator specifier serialization"));
		expect(str_contains(operator_text, txt("[[maybe_unused]]")),           txt("cpp operator attribute serialization"));
		expect(str_contains(operator_text, txt("export")),                     txt("cpp operator module flag serialization"));
		if (!str_starts_with(operator_text, txt("export [[maybe_unused]]  inline\nOperatorOptionReturn"))) log_fmt("%S\n", operator_text);
		expect(str_starts_with(operator_text, txt("export [[maybe_unused]]  inline\nOperatorOptionReturn")), txt("cpp operator one-attribute prefix serialization"));
	}

	CodeTypename   cast_type  = def_type(name(s32));
	CodeBody       cast_body  = def_function_body(args(untyped_str(txt("return 0;"))));
	CodeSpecifiers cast_specs = def_specifier(Spec_Const);
	Opts_def_operator_cast cast_options = {};
	cast_options.body  = cast_body;
	cast_options.specs = cast_specs;
	CodeOpCast option_cast = def_operator_cast(cast_type, cast_options);
	expect(option_cast.ast != nullptr, txt("cpp operator cast options"));
	if (option_cast.ast != nullptr) {
		expect(option_cast->ValueType.ast == cast_type.ast,  txt("cpp operator cast type"));
		expect(option_cast->Body.ast      == cast_body.ast,  txt("cpp operator cast body"));
		expect(option_cast->Specs.ast     == cast_specs.ast, txt("cpp operator cast specifiers"));
		Str cast_text = strbuilder_to_str(code_to_strbuilder((Code)option_cast));
		expect(str_contains(cast_text, txt("operator s32")), txt("cpp operator cast serialization"));
		expect(str_contains(cast_text, txt("const")),        txt("cpp operator cast specifier serialization"));
		expect(str_contains(cast_text, txt("return 0")),     txt("cpp operator cast body serialization"));
	}

	Code    untyped        = code_str(char const* baseline_message = "GENCPP_BASELINE";);
	Str     untyped_text   = strbuilder_to_str(code_to_strbuilder(untyped));
	CodeVar parsed_untyped = parse_variable(untyped_text);
	expect(untyped.ast != nullptr,  txt("cpp untyped"));
	expect_fragments(untyped_text,  txt("cpp untyped content"));
	expect_variable(parsed_untyped, txt("cpp untyped reparsed"));

	CodeVar parsed = parse_variable(code(char const* baseline_message = "GENCPP_BASELINE"; ));
	expect_variable(parsed, txt("cpp parsed"));
	expect_fragments(strbuilder_to_str(code_to_strbuilder((Code)parsed)), txt("cpp parsed serialized"));

	CodeClass       parser_class         = parse_class(txt("class ParseClass {};"));
	CodeConstructor parser_constructor   = parse_constructor(txt("ParseClass() {}"));
	CodeDefine      parser_define        = parse_define(txt("#define PARSE_DEFINE 1"));
	CodeDestructor  parser_destructor    = parse_destructor(txt("~ParseClass();"));
	CodeEnum        parser_enum          = parse_enum(txt("enum ParseEnum { ParseValue };"));
	CodeBody        parser_export_body   = parse_export_body(txt("{ int export_value; }"));
	CodeExtern      parser_extern        = parse_extern_link(txt("extern \"C\" { int extern_value; }"));
	CodeFriend      parser_friend        = parse_friend(txt("friend int parse_friend_function();"));
	CodeFn          parser_function      = parse_function(txt("int parse_function();"));
	CodeBody        parser_global_body   = parse_global_body(txt("struct ParseGlobal { int value; };"));
	CodeNS          parser_namespace     = parse_namespace(txt("namespace ParseNamespace {}"));
	CodeOperator    parser_operator      = parse_operator(txt("s32 operator +(s32 lhs, s32 rhs);"));
	CodeOpCast      parser_operator_cast = parse_operator_cast(txt("operator s32() const;"));
	CodeStruct      parser_struct        = parse_struct(txt("struct ParseStruct {};"));
	CodeTemplate    parser_template      = parse_template(txt("template<class T> struct ParseTemplate {};"));
	CodeTypename    parser_type          = parse_type(txt("const s32*"));
	CodeTypedef     parser_typedef       = parse_typedef(txt("typedef s32 ParseTypedef;"));
	CodeUnion       parser_union         = parse_union(txt("union ParseUnion { s32 value; };"));
	CodeUsing       parser_using         = parse_using(txt("using ParseUsing = s32;"));
	CodeVar         parser_variable      = parse_variable(txt("s32 parse_variable;"));

	expect(parser_class.ast         != nullptr && parser_class->Type         == CT_Class,             txt("cpp parse class fixture"));
	expect(parser_constructor.ast   != nullptr && parser_constructor->Type   == CT_Constructor,       txt("cpp parse constructor fixture"));
	expect(parser_define.ast        != nullptr && parser_define->Type        == CT_Preprocess_Define, txt("cpp parse define fixture"));
	expect(parser_destructor.ast    != nullptr && parser_destructor->Type    == CT_Destructor_Fwd,    txt("cpp parse destructor fixture"));
	expect(parser_enum.ast          != nullptr && parser_enum->Type          == CT_Enum,              txt("cpp parse enum fixture"));
	expect(parser_export_body.ast   != nullptr && parser_export_body->Type   == CT_Export_Body,       txt("cpp parse export body fixture"));
	expect(parser_extern.ast        != nullptr && parser_extern->Type        == CT_Extern_Linkage,    txt("cpp parse extern fixture"));
	expect(parser_friend.ast        != nullptr && parser_friend->Type        == CT_Friend,            txt("cpp parse friend fixture"));
	expect(parser_function.ast      != nullptr && parser_function->Type      == CT_Function_Fwd,      txt("cpp parse function fixture"));
	expect(parser_global_body.ast   != nullptr && parser_global_body->Type   == CT_Global_Body,       txt("cpp parse global body fixture"));
	expect(parser_namespace.ast     != nullptr && parser_namespace->Type     == CT_Namespace,         txt("cpp parse namespace fixture"));
	expect(parser_operator.ast      != nullptr && parser_operator->Type      == CT_Operator_Fwd,      txt("cpp parse operator fixture"));
	expect(parser_operator_cast.ast != nullptr && parser_operator_cast->Type == CT_Operator_Cast_Fwd, txt("cpp parse operator cast fixture"));
	expect(parser_struct.ast        != nullptr && parser_struct->Type        == CT_Struct,            txt("cpp parse struct fixture"));
	expect(parser_template.ast      != nullptr && parser_template->Type      == CT_Template,          txt("cpp parse template fixture"));
	expect(parser_type.ast          != nullptr && parser_type->Type          == CT_Typename,          txt("cpp parse type fixture"));
	expect(parser_typedef.ast       != nullptr && parser_typedef->Type       == CT_Typedef,           txt("cpp parse typedef fixture"));
	expect(parser_union.ast         != nullptr && parser_union->Type         == CT_Union,             txt("cpp parse union fixture"));
	expect(parser_using.ast         != nullptr && parser_using->Type         == CT_Using,             txt("cpp parse using fixture"));
	expect(parser_variable.ast      != nullptr && parser_variable->Type      == CT_Variable,          txt("cpp parse variable fixture"));

	CodeBody parsed_global_options = parse_global_body(txt("struct ParseGlobalOptions { s32 value; };"));
	Code     global_options_code   = parsed_global_options;
	expect(parsed_global_options.ast != nullptr,                                           txt("cpp parse global body options"));
	expect(ctx.parse_info.result.ast == global_options_code.ast,                           txt("cpp default parse info result"));
	expect(ctx.parse_info.lexed.tokens.num > 0,                                            txt("cpp default parse info tokens"));
	expect(ctx.parse_info.lexed.messages == nullptr && ctx.parse_info.messages == nullptr, txt("cpp default parse info messages"));

	ParseInfo  caller_info   = {};
	Opts_parse parse_options = { & ctx, & caller_info };
	CodeVar parsed_option_variable = parse_variable(txt("s32 parsed_option_variable;"), parse_options);
	Code    parsed_option_code     = parsed_option_variable;
	expect(parsed_option_variable.ast != nullptr && parsed_option_variable->Type == CT_Variable, txt("cpp explicit parse variable options"));
	expect(str_contains(parsed_option_variable->Name, txt("parsed_option_variable")),            txt("cpp explicit parse variable name"));
	expect(caller_info.result.ast == parsed_option_code.ast,                                     txt("cpp explicit parse info result"));
	expect(caller_info.lexed.tokens.num > 0,                                                     txt("cpp explicit parse info tokens"));

	ParseInfo  lexer_info    = {};
	Opts_parse lexer_options = { & ctx, & lexer_info };
	CodeVar parsed_lexer_prefix = parse_variable(txt("s32 parsed_lexer_prefix;@"), lexer_options);
	Code    parsed_lexer_code   = parsed_lexer_prefix;
	expect(parsed_lexer_prefix.ast != nullptr && parsed_lexer_prefix->Type == CT_Variable, txt("cpp parse lexer prefix result"));
	expect(lexer_info.result.ast == parsed_lexer_code.ast,                                 txt("cpp parse lexer prefix info result"));
	expect(lexer_info.lexed.tokens.num > 0,                                                txt("cpp parse lexer prefix retained tokens"));
	expect(lexer_info.lexed.messages != nullptr,                                           txt("cpp parse lexer prefix diagnostic"));

	expect_parse_result(parse_class        (txt("class ParseClass {};"), parse_options), & caller_info, CT_Class, txt("cpp class options matrix"));
	expect_parse_result(parse_constructor  (txt("ParseClass() {}"), parse_options), & caller_info, CT_Constructor, txt("cpp constructor options matrix"));
	expect_parse_result(parse_define       (txt("#define PARSE_DEFINE 1"), parse_options), & caller_info, CT_Preprocess_Define, txt("cpp define options matrix"));
	expect_parse_result(parse_destructor   (txt("~ParseClass();"), parse_options), & caller_info, CT_Destructor_Fwd, txt("cpp destructor options matrix"));
	expect_parse_result(parse_enum         (txt("enum ParseEnum { ParseValue };"), parse_options), & caller_info, CT_Enum, txt("cpp enum options matrix"));
	expect_parse_result(parse_export_body  (txt("{ int export_value; }"), parse_options), & caller_info, CT_Export_Body, txt("cpp export body options matrix"));
	expect_parse_result(parse_extern_link  (txt("extern \"C\" { int extern_value; }"), parse_options), & caller_info, CT_Extern_Linkage, txt("cpp extern options matrix"));
	expect_parse_result(parse_friend       (txt("friend int parse_friend_function();"), parse_options), & caller_info, CT_Friend, txt("cpp friend options matrix"));
	expect_parse_result(parse_function     (txt("int parse_function();"), parse_options), & caller_info, CT_Function_Fwd, txt("cpp function options matrix"));
	expect_parse_result(parse_global_body  (txt("struct ParseGlobal { int value; };"), parse_options), & caller_info, CT_Global_Body, txt("cpp global body options matrix"));
	expect_parse_result(parse_namespace    (txt("namespace ParseNamespace {}"), parse_options), & caller_info, CT_Namespace, txt("cpp namespace options matrix"));
	expect_parse_result(parse_operator     (txt("s32 operator +(s32 lhs, s32 rhs);"), parse_options), & caller_info, CT_Operator_Fwd, txt("cpp operator options matrix"));
	expect_parse_result(parse_operator_cast(txt("operator s32() const;"), parse_options), & caller_info, CT_Operator_Cast_Fwd, txt("cpp operator cast options matrix"));
	expect_parse_result(parse_struct       (txt("struct ParseStruct {};"), parse_options), & caller_info, CT_Struct, txt("cpp struct options matrix"));
	expect_parse_result(parse_template     (txt("template<class T> struct ParseTemplate {};"), parse_options), & caller_info, CT_Template, txt("cpp template options matrix"));
	expect_parse_result(parse_type         (txt("const s32*"), parse_options), & caller_info, CT_Typename, txt("cpp type options matrix"));
	expect_parse_result(parse_typedef      (txt("typedef s32 ParseTypedef;"), parse_options),        & caller_info, CT_Typedef,  txt("cpp typedef options matrix"));
	expect_parse_result(parse_union        (txt("union ParseUnion { s32 value; };"), parse_options), & caller_info, CT_Union,    txt("cpp union options matrix"));
	expect_parse_result(parse_using        (txt("using ParseUsing = s32;"), parse_options),          & caller_info, CT_Using,    txt("cpp using options matrix"));
	expect_parse_result(parse_variable     (txt("s32 parse_variable;"), parse_options),              & caller_info, CT_Variable, txt("cpp variable options matrix"));

	ParseInfo  comma_info        = {};
	Opts_parse comma_options     = { &ctx, &comma_info };
	CodeBody   parsed_comma_body = parse_global_body(txt("struct ParseComma { s32 value; };\n,"), comma_options);
	Code       parsed_comma_code = parsed_comma_body;
	expect(parsed_comma_body.ast != nullptr && parsed_comma_body->Type == CT_Global_Body, txt("cpp comma parser result"));
	expect(comma_info.result.ast == parsed_comma_code.ast,                                txt("cpp comma ParseInfo result"));
	expect(comma_info.lexed.tokens.num > 0 && comma_info.lexed.messages == nullptr,       txt("cpp comma retained tokens"));
	expect(comma_info.messages != nullptr,                                                txt("cpp comma parser diagnostic"));
	if (comma_info.messages != nullptr)
		expect(str_contains(comma_info.messages->Content, txt("Dangling comma")), txt("cpp comma diagnostic content"));

	ParseInfo  recovery_info    = {};
	Opts_parse recovery_options = { &ctx, &recovery_info };
	CodeBody   recovered_body   = parse_global_body(txt("s32 before; s32 broken(s32 value; s32 after;"), recovery_options);
	Str recovery_text = { nullptr, 0 };
	if (recovered_body.ast != nullptr)
		recovery_text = strbuilder_to_str(code_to_strbuilder((Code)recovered_body));
	expect(recovered_body.ast       != nullptr && recovered_body->Type == CT_Global_Body, txt("cpp file-scope recovery body"));
	expect(recovery_info.result.ast == ((Code)recovered_body).ast,                        txt("cpp file-scope recovery ParseInfo result"));
	expect(recovery_info.messages   != nullptr,                                           txt("cpp file-scope recovery diagnostic"));
	if (recovery_info.messages != nullptr)
		expect(str_contains(recovery_info.messages->Content, txt("Expected")), txt("cpp file-scope recovery mismatch text"));
	if (recovery_info.messages != nullptr)
		expect(recovery_info.messages->Next == nullptr, txt("cpp file-scope recovery single diagnostic"));
	expect(str_contains(recovery_text, txt("before")), txt("cpp file-scope recovery keeps preceding member"));
	expect(str_contains(recovery_text, txt("after")),  txt("cpp file-scope recovery keeps following member"));
	Code recovered_member = { nullptr };
	if (recovered_body.ast != nullptr) recovered_member = recovered_body->Front;
	Code recovered_before = recovered_member;
	expect(recovered_before != nullptr && recovered_before->Type == CT_Variable, txt("cpp mismatch recovery keeps preceding member typed"));
	if (recovered_before)
		expect(str_are_equal(recovered_before->Name, txt("before")), txt("cpp mismatch recovery preceding member name"));
	if (recovered_member) recovered_member = recovered_member->Next;
	expect(recovered_member != nullptr && recovered_member->Type == CT_Untyped, txt("cpp file-scope recovery stores malformed member as untyped"));
	if (recovered_member)
		expect(str_are_equal(strbuilder_to_str(code_to_strbuilder(recovered_member)), txt("s32 broken(s32 value;")), txt("cpp file-scope recovery retains exact mismatch span"));
	Code recovered_after = recovered_member ? recovered_member->Next : Code{ nullptr };
	expect(recovered_after != nullptr && recovered_after->Type == CT_Variable, txt("cpp file-scope recovery keeps following member typed"));
	if (recovered_after)
		expect(str_are_equal(recovered_after->Name, txt("after")), txt("cpp file-scope recovery following member name"));

	ParseInfo  invalid_member_info    = {};
	Opts_parse invalid_member_options = { &ctx, &invalid_member_info };
	CodeBody   invalid_member_body    = parse_global_body(txt("s32 before; = { 1; 2 }; s32 after;"), invalid_member_options);
	Str invalid_member_text = { nullptr, 0 };
	if (invalid_member_body.ast != nullptr)
		invalid_member_text = strbuilder_to_str(code_to_strbuilder((Code)invalid_member_body));
	expect(invalid_member_body.ast        != nullptr && invalid_member_body->Type == CT_Global_Body, txt("cpp invalid member recovery body"));
	expect(invalid_member_info.result.ast == ((Code)invalid_member_body).ast,                        txt("cpp invalid member recovery ParseInfo result"));
	expect(invalid_member_info.messages   != nullptr,                                                txt("cpp invalid member recovery diagnostic"));
	if (invalid_member_info.messages != nullptr) {
		expect(str_contains(invalid_member_info.messages->Content, txt("Failed to parse member")), txt("cpp invalid member recovery diagnostic text"));
		expect(invalid_member_info.messages->Next == nullptr,                                      txt("cpp invalid member recovery single diagnostic"));
	}
	expect(str_contains(invalid_member_text, txt("before")), txt("cpp invalid member recovery keeps preceding member"));
	expect(str_contains(invalid_member_text, txt("after")),  txt("cpp invalid member recovery keeps following member"));
	Code invalid_member_node = { nullptr };
	if  (invalid_member_body.ast != nullptr) invalid_member_node = invalid_member_body->Front;
	Code invalid_member_before = invalid_member_node;
	expect(invalid_member_before != nullptr && invalid_member_before->Type == CT_Variable, txt("cpp invalid member keeps preceding member typed"));
	if (invalid_member_before)
		expect(str_are_equal(invalid_member_before->Name, txt("before")), txt("cpp invalid member preceding member name"));
	if (invalid_member_node) invalid_member_node = invalid_member_node->Next;
	expect(invalid_member_node != nullptr && invalid_member_node->Type == CT_Untyped, txt("cpp invalid member recovery stores member as untyped"));
	if (invalid_member_node)
		expect(str_are_equal(strbuilder_to_str(code_to_strbuilder(invalid_member_node)), txt("= { 1; 2 };")), txt("cpp invalid member recovery retains exact source span"));
	Code   invalid_member_after = invalid_member_node ? invalid_member_node->Next : Code{ nullptr };
	expect(invalid_member_after != nullptr && invalid_member_after->Type == CT_Variable, txt("cpp invalid member recovery keeps following member typed"));
	if (invalid_member_after)
		expect(str_are_equal(invalid_member_after->Name, txt("after")), txt("cpp invalid member recovery following member name"));

	ParseInfo  import_recovery_info    = {};
	Opts_parse import_recovery_options = { &ctx, &import_recovery_info };
	CodeBody   import_recovery_body    = parse_global_body(txt("s32 before; import foo; s32 after;"), import_recovery_options);
	Code import_recovery_node = { nullptr };
	if  (import_recovery_body.ast != nullptr) import_recovery_node = import_recovery_body->Front;
	Code import_recovery_before = import_recovery_node;
	expect(import_recovery_before != nullptr && import_recovery_before->Type == CT_Variable, txt("cpp import recovery keeps preceding member typed"));
	if (import_recovery_before)
		expect(str_are_equal(import_recovery_before->Name, txt("before")), txt("cpp import recovery preceding member name"));
	if (import_recovery_node) import_recovery_node = import_recovery_node->Next;
	expect(import_recovery_body.ast != nullptr && import_recovery_body->Type == CT_Global_Body, txt("cpp import recovery body"));
	expect(import_recovery_info.messages != nullptr, txt("cpp import recovery diagnostic"));
	if (import_recovery_info.messages != nullptr)
		expect(import_recovery_info.messages->Next == nullptr, txt("cpp import recovery single diagnostic"));
	expect(import_recovery_node != nullptr && import_recovery_node->Type == CT_Untyped, txt("cpp import recovery preserves member as untyped"));
	if (import_recovery_node)
		expect(str_are_equal(strbuilder_to_str(code_to_strbuilder(import_recovery_node)), txt("import foo;")), txt("cpp import recovery exact source span"));
	Code import_recovery_after = import_recovery_node ? import_recovery_node->Next : Code{ nullptr };
	expect(import_recovery_after != nullptr && import_recovery_after->Type == CT_Variable, txt("cpp import recovery keeps following member typed"));
	if (import_recovery_after)
		expect(str_are_equal(import_recovery_after->Name, txt("after")), txt("cpp import recovery following member name"));

	Macro unbounded_macro = { txt("UnboundedRecoveryMacro"), MT_Expression, MF_Null };
	register_macro(unbounded_macro);
	ParseInfo  macro_recovery_info    = {};
	Opts_parse macro_recovery_options = { &ctx, &macro_recovery_info };
	CodeBody   macro_recovery_body    = parse_global_body(txt("s32 before; UnboundedRecoveryMacro; s32 after;"), macro_recovery_options);
	Code macro_recovery_node = { nullptr };
	if  (macro_recovery_body.ast != nullptr) macro_recovery_node = macro_recovery_body->Front;
	Code macro_recovery_before = macro_recovery_node;
	expect(macro_recovery_before != nullptr && macro_recovery_before->Type == CT_Variable, txt("cpp macro recovery keeps preceding member typed"));
	if (macro_recovery_before)
		expect(str_are_equal(macro_recovery_before->Name, txt("before")), txt("cpp macro recovery preceding member name"));
	if (macro_recovery_node) macro_recovery_node = macro_recovery_node->Next;
	expect(macro_recovery_body.ast != nullptr && macro_recovery_body->Type == CT_Global_Body, txt("cpp unbounded macro recovery body"));
	expect(macro_recovery_info.messages != nullptr, txt("cpp unbounded macro recovery diagnostic"));
	if (macro_recovery_info.messages != nullptr)
		expect(macro_recovery_info.messages->Next == nullptr, txt("cpp unbounded macro recovery single diagnostic"));
	expect(macro_recovery_node != nullptr && macro_recovery_node->Type == CT_Untyped, txt("cpp unbounded macro recovery preserves member as untyped"));
	if (macro_recovery_node)
		expect(str_are_equal(strbuilder_to_str(code_to_strbuilder(macro_recovery_node)), txt("UnboundedRecoveryMacro;")), txt("cpp unbounded macro recovery exact source span"));
	Code macro_recovery_after = macro_recovery_node ? macro_recovery_node->Next : Code{ nullptr };
	expect(macro_recovery_after != nullptr && macro_recovery_after->Type == CT_Variable, txt("cpp unbounded macro recovery keeps following member typed"));
	if (macro_recovery_after)
		expect(str_are_equal(macro_recovery_after->Name, txt("after")), txt("cpp unbounded macro recovery following member name"));

	ParseInfo  eof_recovery_info    = {};
	Opts_parse eof_recovery_options = { &ctx, &eof_recovery_info };
	CodeBody   eof_recovery_body    = parse_global_body(txt("s32 before; s32 broken(s32 value"), eof_recovery_options);
	Code eof_recovery_node = { nullptr };
	if  (eof_recovery_body.ast != nullptr) eof_recovery_node = eof_recovery_body->Front;
	Code eof_recovery_before = eof_recovery_node;
	expect(eof_recovery_before != nullptr && eof_recovery_before->Type == CT_Variable, txt("cpp EOF recovery keeps preceding member typed"));
	if (eof_recovery_before)
		expect(str_are_equal(eof_recovery_before->Name, txt("before")), txt("cpp EOF recovery preceding member name"));
	if (eof_recovery_node) eof_recovery_node = eof_recovery_node->Next;
	expect(eof_recovery_body.ast != nullptr && eof_recovery_body->Type == CT_Global_Body, txt("cpp end-of-slice recovery body"));
	expect(eof_recovery_info.messages != nullptr, txt("cpp end-of-slice recovery diagnostic"));
	if (eof_recovery_info.messages != nullptr)
		expect(eof_recovery_info.messages->Next == nullptr, txt("cpp end-of-slice recovery single diagnostic"));
	expect(eof_recovery_node != nullptr && eof_recovery_node->Type == CT_Untyped, txt("cpp end-of-slice member is untyped"));
	if (eof_recovery_node)
		expect(str_are_equal(strbuilder_to_str(code_to_strbuilder(eof_recovery_node)), txt("s32 broken(s32 value")), txt("cpp end-of-slice recovery exact source span"));
	if (eof_recovery_node)
		expect(eof_recovery_node->Next == nullptr, txt("cpp end-of-slice recovery has no following node"));

	ParseInfo  comma_recovery_info    = {};
	Opts_parse comma_recovery_options = { &ctx, &comma_recovery_info };
	CodeBody   comma_recovery_body    = parse_global_body(txt("s32 before; , s32 after;"), comma_recovery_options);
	Code comma_recovery_node = { nullptr };
	if  (comma_recovery_body.ast != nullptr) comma_recovery_node = comma_recovery_body->Front;
	Code comma_recovery_before = comma_recovery_node;
	expect(comma_recovery_before != nullptr && comma_recovery_before->Type == CT_Variable, txt("cpp comma recovery keeps preceding member typed"));
	if (comma_recovery_before)
		expect(str_are_equal(comma_recovery_before->Name, txt("before")), txt("cpp comma recovery preceding member name"));
	if (comma_recovery_node) comma_recovery_node = comma_recovery_node->Next;
	expect(comma_recovery_body.ast != nullptr && comma_recovery_body->Type == CT_Global_Body, txt("cpp comma recovery body"));
	expect(comma_recovery_info.messages != nullptr, txt("cpp comma recovery diagnostic"));
	if (comma_recovery_info.messages != nullptr) {
		expect(str_contains(comma_recovery_info.messages->Content, txt("Dangling comma")), txt("cpp comma recovery diagnostic text"));
		expect(comma_recovery_info.messages->Next == nullptr, txt("cpp comma recovery single diagnostic"));
	}
	expect(comma_recovery_node != nullptr && comma_recovery_node->Type == CT_Untyped, txt("cpp comma recovery preserves token as untyped"));
	if (comma_recovery_node)
		expect(str_are_equal(strbuilder_to_str(code_to_strbuilder(comma_recovery_node)), txt(",")), txt("cpp comma recovery exact source span"));
	Code comma_recovery_after = comma_recovery_node ? comma_recovery_node->Next : Code{ nullptr };
	expect(comma_recovery_after != nullptr && comma_recovery_after->Type == CT_Variable, txt("cpp comma recovery keeps following member typed"));
	if (comma_recovery_after)
		expect(str_are_equal(comma_recovery_after->Name, txt("after")), txt("cpp comma recovery following member name"));

	CodeBody recovery_reuse = parse_global_body(txt("s32 reused_after_recovery;"), recovery_options);
	Str recovery_reuse_text = { nullptr, 0 };
	if (recovery_reuse.ast != nullptr)
		recovery_reuse_text = strbuilder_to_str(code_to_strbuilder((Code)recovery_reuse));
	expect(recovery_reuse.ast != nullptr && recovery_reuse->Type == CT_Global_Body, txt("cpp parser context reuse after recovery"));
	expect(recovery_info.messages == nullptr, txt("cpp parser context reuse clears prior diagnostics"));
	expect(str_contains(recovery_reuse_text, txt("reused_after_recovery")), txt("cpp parser context reuse parses later input"));

	ParseInfo  nested_struct_info    = {};
	Opts_parse nested_struct_options = { &ctx, &nested_struct_info };
	CodeStruct nested_struct         = parse_struct(txt("struct NestedRecovery { s32 before; s32 broken(s32 value; s32 after; };"), nested_struct_options);
	expect(nested_struct.ast != nullptr && nested_struct->Type == CT_Struct, txt("cpp nested struct recovery result"));
	expect(nested_struct_info.result.ast == ((Code)nested_struct).ast, txt("cpp nested struct recovery ParseInfo result"));
	expect(nested_struct_info.messages != nullptr, txt("cpp nested struct recovery diagnostic"));
	if (nested_struct_info.messages != nullptr)
		expect(str_contains(nested_struct_info.messages->Content, txt("Expected")), txt("cpp nested struct recovery diagnostic text"));
	Code nested_struct_member = nested_struct.ast != nullptr && nested_struct->Body.ast != nullptr ? nested_struct->Body->Front : Code{ nullptr };
	expect(nested_struct_member != nullptr && nested_struct_member->Type == CT_Variable, txt("cpp nested struct recovery keeps preceding member typed"));
	if (nested_struct_member != nullptr)
		expect(str_are_equal(nested_struct_member->Name, txt("before")), txt("cpp nested struct recovery preceding member name"));
	if (nested_struct_member != nullptr) nested_struct_member = nested_struct_member->Next;
	expect(nested_struct_member != nullptr && nested_struct_member->Type == CT_Untyped, txt("cpp nested struct recovery retains failed member"));
	if (nested_struct_member != nullptr)
		expect(str_are_equal(strbuilder_to_str(code_to_strbuilder(nested_struct_member)), txt("s32 broken(s32 value;")), txt("cpp nested struct recovery exact failed span"));
	Code nested_struct_after = nested_struct_member != nullptr ? nested_struct_member->Next : Code{ nullptr };
	expect(nested_struct_after != nullptr && nested_struct_after->Type == CT_Variable, txt("cpp nested struct recovery keeps following member typed"));
	if (nested_struct_after != nullptr)
		expect(str_are_equal(nested_struct_after->Name, txt("after")), txt("cpp nested struct recovery following member name"));
	Str nested_struct_text = { nullptr, 0 };
	if (nested_struct.ast != nullptr)
		nested_struct_text = strbuilder_to_str(code_to_strbuilder((Code)nested_struct));
	expect(str_contains(nested_struct_text, txt("s32 after")), txt("cpp nested struct recovery serializes following member"));

	ParseInfo  nested_namespace_info    = {};
	Opts_parse nested_namespace_options = { &ctx, &nested_namespace_info };
	CodeNS     nested_namespace         = parse_namespace(txt("namespace NestedRecovery { s32 before; s32 broken(s32 value; s32 after; }"), nested_namespace_options);
	expect(nested_namespace.ast != nullptr && nested_namespace->Type == CT_Namespace, txt("cpp nested namespace recovery result"));
	expect(nested_namespace_info.result.ast == ((Code)nested_namespace).ast, txt("cpp nested namespace recovery ParseInfo result"));
	expect(nested_namespace_info.messages != nullptr, txt("cpp nested namespace recovery diagnostic"));
	if (nested_namespace_info.messages != nullptr)
		expect(str_contains(nested_namespace_info.messages->Content, txt("Expected")), txt("cpp nested namespace recovery diagnostic text"));
	Code nested_namespace_member = nested_namespace.ast != nullptr && nested_namespace->Body.ast != nullptr ? nested_namespace->Body->Front : Code{ nullptr };
	expect(nested_namespace_member != nullptr && nested_namespace_member->Type == CT_Variable, txt("cpp nested namespace recovery keeps preceding member typed"));
	if (nested_namespace_member != nullptr)
		expect(str_are_equal(nested_namespace_member->Name, txt("before")), txt("cpp nested namespace recovery preceding member name"));
	if (nested_namespace_member != nullptr) nested_namespace_member = nested_namespace_member->Next;
	expect(nested_namespace_member != nullptr && nested_namespace_member->Type == CT_Untyped, txt("cpp nested namespace recovery retains failed member"));
	if (nested_namespace_member != nullptr)
		expect(str_are_equal(strbuilder_to_str(code_to_strbuilder(nested_namespace_member)), txt("s32 broken(s32 value;")), txt("cpp nested namespace recovery exact failed span"));
	Code nested_namespace_after = nested_namespace_member != nullptr ? nested_namespace_member->Next : Code{ nullptr };
	expect(nested_namespace_after != nullptr && nested_namespace_after->Type == CT_Variable, txt("cpp nested namespace recovery keeps following member typed"));
	if (nested_namespace_after != nullptr)
		expect(str_are_equal(nested_namespace_after->Name, txt("after")), txt("cpp nested namespace recovery following member name"));
	Str nested_namespace_text = { nullptr, 0 };
	if (nested_namespace.ast != nullptr)
		nested_namespace_text = strbuilder_to_str(code_to_strbuilder((Code)nested_namespace));
	expect(str_contains(nested_namespace_text, txt("s32 after")), txt("cpp nested namespace recovery serializes following member"));

	ParseInfo  nested_brace_info    = {};
	Opts_parse nested_brace_options = { &ctx, &nested_brace_info };
	CodeStruct nested_brace_struct  = parse_struct(txt("struct NestedBraceRecovery { s32 before; s32 broken(s32 { 1; s32 after; };"), nested_brace_options);
	expect(nested_brace_struct.ast != nullptr && nested_brace_struct->Type == CT_Struct, txt("cpp nested brace recovery result"));
	expect(nested_brace_info.result.ast == ((Code)nested_brace_struct).ast, txt("cpp nested brace recovery ParseInfo result"));
	expect(nested_brace_info.messages != nullptr, txt("cpp nested brace recovery diagnostic"));
	Code nested_brace_member = nested_brace_struct.ast != nullptr && nested_brace_struct->Body.ast != nullptr ? nested_brace_struct->Body->Front : Code{ nullptr };
	expect(nested_brace_member != nullptr && nested_brace_member->Type == CT_Variable, txt("cpp nested brace recovery keeps preceding member typed"));
	if (nested_brace_member != nullptr) nested_brace_member = nested_brace_member->Next;
	expect(nested_brace_member != nullptr && nested_brace_member->Type == CT_Untyped, txt("cpp nested brace recovery retains failed member"));
	if (nested_brace_member != nullptr)
		expect(str_are_equal(strbuilder_to_str(code_to_strbuilder(nested_brace_member)), txt("s32 broken(s32 { 1;")), txt("cpp nested brace recovery exact failed span"));
	Code nested_brace_after = nested_brace_member != nullptr ? nested_brace_member->Next : Code{ nullptr };
	expect(nested_brace_after != nullptr && nested_brace_after->Type == CT_Variable, txt("cpp nested brace recovery keeps following member typed"));
	if (nested_brace_after != nullptr)
		expect(str_are_equal(nested_brace_after->Name, txt("after")), txt("cpp nested brace recovery following member name"));
	Str nested_brace_text = { nullptr, 0 };
	if (nested_brace_struct.ast != nullptr)
		nested_brace_text = strbuilder_to_str(code_to_strbuilder((Code)nested_brace_struct));
	expect(str_contains(nested_brace_text, txt("s32 after")), txt("cpp nested brace recovery serializes following member"));

	ParseInfo  nested_namespace_brace_info    = {};
	Opts_parse nested_namespace_brace_options = { &ctx, &nested_namespace_brace_info };
	CodeNS nested_namespace_brace = parse_namespace(txt("namespace NestedBraceRecovery { s32 before; s32 broken(s32 { 1; s32 after; }"), nested_namespace_brace_options);
	expect(nested_namespace_brace.ast != nullptr && nested_namespace_brace->Type == CT_Namespace, txt("cpp nested namespace brace recovery result"));
	expect(nested_namespace_brace_info.result.ast == ((Code)nested_namespace_brace).ast, txt("cpp nested namespace brace recovery ParseInfo result"));
	Code nested_namespace_brace_member = nested_namespace_brace.ast != nullptr && nested_namespace_brace->Body.ast != nullptr ? nested_namespace_brace->Body->Front : Code{ nullptr };
	expect(nested_namespace_brace_member != nullptr && nested_namespace_brace_member->Type == CT_Variable, txt("cpp nested namespace brace recovery keeps preceding member typed"));
	if (nested_namespace_brace_member != nullptr) nested_namespace_brace_member = nested_namespace_brace_member->Next;
	expect(nested_namespace_brace_member != nullptr && nested_namespace_brace_member->Type == CT_Untyped, txt("cpp nested namespace brace recovery retains failed member"));
	if (nested_namespace_brace_member != nullptr)
		expect(str_are_equal(strbuilder_to_str(code_to_strbuilder(nested_namespace_brace_member)), txt("s32 broken(s32 { 1;")), txt("cpp nested namespace brace recovery exact failed span"));
	Code nested_namespace_brace_after = nested_namespace_brace_member != nullptr ? nested_namespace_brace_member->Next : Code{ nullptr };
	expect(nested_namespace_brace_after != nullptr && nested_namespace_brace_after->Type == CT_Variable, txt("cpp nested namespace brace recovery keeps following member typed"));
	if (nested_namespace_brace_after != nullptr)
		expect(str_are_equal(nested_namespace_brace_after->Name, txt("after")), txt("cpp nested namespace brace recovery following member name"));
	Str nested_namespace_brace_text = { nullptr, 0 };
	if (nested_namespace_brace.ast != nullptr)
		nested_namespace_brace_text = strbuilder_to_str(code_to_strbuilder((Code)nested_namespace_brace));
	expect(str_contains(nested_namespace_brace_text, txt("s32 after")), txt("cpp nested namespace brace recovery serializes following member"));

	ParseInfo  nested_brace_tail_info    = {};
	Opts_parse nested_brace_tail_options = { &ctx, &nested_brace_tail_info };
	CodeStruct nested_brace_tail_struct  = parse_struct(txt("struct NestedBraceTailRecovery { s32 before; s32 broken(s32 { 1; 2 } foo{ 3; s32 after; };"), nested_brace_tail_options);
	expect(nested_brace_tail_struct.ast != nullptr && nested_brace_tail_struct->Type == CT_Struct, txt("cpp nested brace tail recovery result"));
	expect(nested_brace_tail_info.result.ast == ((Code)nested_brace_tail_struct).ast, txt("cpp nested brace tail recovery ParseInfo result"));
	expect(nested_brace_tail_info.messages != nullptr, txt("cpp nested brace tail recovery diagnostic"));
	Code nested_brace_tail_member = nested_brace_tail_struct.ast != nullptr && nested_brace_tail_struct->Body.ast != nullptr ? nested_brace_tail_struct->Body->Front : Code{ nullptr };
	if  (nested_brace_tail_member != nullptr) nested_brace_tail_member = nested_brace_tail_member->Next;
	expect(nested_brace_tail_member != nullptr && nested_brace_tail_member->Type == CT_Untyped, txt("cpp nested brace tail recovery retains failed member"));
	if (nested_brace_tail_member != nullptr)
		expect(str_are_equal(strbuilder_to_str(code_to_strbuilder(nested_brace_tail_member)), txt("s32 broken(s32 { 1; 2 } foo{ 3;")), txt("cpp nested brace tail recovery exact failed span"));
	Code nested_brace_tail_after = nested_brace_tail_member != nullptr ? nested_brace_tail_member->Next : Code{ nullptr };
	expect(nested_brace_tail_after != nullptr && nested_brace_tail_after->Type == CT_Variable, txt("cpp nested brace tail recovery keeps following member typed"));
	if (nested_brace_tail_after != nullptr)
		expect(str_are_equal(nested_brace_tail_after->Name, txt("after")), txt("cpp nested brace tail recovery following member name"));

	ParseInfo  nested_namespace_tail_info    = {};
	Opts_parse nested_namespace_tail_options = { &ctx, &nested_namespace_tail_info };
	CodeNS nested_namespace_tail = parse_namespace(txt("namespace NestedBraceTailRecovery { s32 before; s32 broken(s32 { 1; 2 } foo{ 3; s32 after; }"), nested_namespace_tail_options);
	expect(nested_namespace_tail.ast             != nullptr && nested_namespace_tail->Type == CT_Namespace, txt("cpp nested namespace brace tail recovery result"));
	expect(nested_namespace_tail_info.result.ast == ((Code)nested_namespace_tail).ast,                      txt("cpp nested namespace brace tail recovery ParseInfo result"));
	Code nested_namespace_tail_member = nested_namespace_tail.ast != nullptr && nested_namespace_tail->Body.ast != nullptr ? nested_namespace_tail->Body->Front : Code{ nullptr };
	if  (nested_namespace_tail_member != nullptr) nested_namespace_tail_member = nested_namespace_tail_member->Next;
	expect(nested_namespace_tail_member != nullptr && nested_namespace_tail_member->Type == CT_Untyped, txt("cpp nested namespace brace tail recovery retains failed member"));
	if (nested_namespace_tail_member != nullptr)
		expect(str_are_equal(strbuilder_to_str(code_to_strbuilder(nested_namespace_tail_member)), txt("s32 broken(s32 { 1; 2 } foo{ 3;")), txt("cpp nested namespace brace tail recovery exact failed span"));
	Code nested_namespace_tail_after = nested_namespace_tail_member != nullptr ? nested_namespace_tail_member->Next : Code{ nullptr };
	expect(nested_namespace_tail_after != nullptr && nested_namespace_tail_after->Type == CT_Variable, txt("cpp nested namespace brace tail recovery keeps following member typed"));
	if (nested_namespace_tail_after != nullptr)
		expect(str_are_equal(nested_namespace_tail_after->Name, txt("after")), txt("cpp nested namespace brace tail recovery following member name"));

	ParseInfo  nested_inner_close_info    = {};
	Opts_parse nested_inner_close_options = { &ctx, &nested_inner_close_info };
	CodeStruct nested_inner_close_struct  = parse_struct(txt("struct NestedInnerCloseRecovery { s32 before; s32 broken(s32 { 1; 2 } foo{ 3; s32 after; struct Inner { s32 x; }; s32 tail; };"), nested_inner_close_options);
	expect(nested_inner_close_struct.ast != nullptr && nested_inner_close_struct->Type == CT_Struct, txt("cpp nested inner-close recovery result"));
	Code   nested_inner_close_member = nested_inner_close_struct.ast != nullptr && nested_inner_close_struct->Body.ast != nullptr ? nested_inner_close_struct->Body->Front : Code{ nullptr };
	if    (nested_inner_close_member != nullptr) nested_inner_close_member = nested_inner_close_member->Next;
	expect(nested_inner_close_member != nullptr && nested_inner_close_member->Type == CT_Untyped, txt("cpp nested inner-close recovery retains failed member"));
	if    (nested_inner_close_member != nullptr)
		expect(str_are_equal(strbuilder_to_str(code_to_strbuilder(nested_inner_close_member)), txt("s32 broken(s32 { 1; 2 } foo{ 3;")), txt("cpp nested inner-close recovery exact failed span"));
	Code   nested_inner_close_after = nested_inner_close_member != nullptr ? nested_inner_close_member->Next : Code{ nullptr };
	expect(nested_inner_close_after != nullptr && nested_inner_close_after->Type == CT_Variable, txt("cpp nested inner-close recovery keeps following member typed"));
	if (nested_inner_close_after != nullptr)
		expect(str_are_equal(nested_inner_close_after->Name, txt("after")), txt("cpp nested inner-close recovery following member name"));
	Code   nested_inner_close_inner = nested_inner_close_after != nullptr ? nested_inner_close_after->Next : Code{ nullptr };
	expect(nested_inner_close_inner != nullptr && nested_inner_close_inner->Type == CT_Struct, txt("cpp nested inner-close recovery parses nested struct"));
	Code   nested_inner_close_tail = nested_inner_close_inner != nullptr ? nested_inner_close_inner->Next : Code{ nullptr };
	expect(nested_inner_close_tail != nullptr && nested_inner_close_tail->Type == CT_Variable, txt("cpp nested inner-close recovery parses trailing member"));

	ParseInfo  inner_before_sync_info    = {};
	Opts_parse inner_before_sync_options = { &ctx, &inner_before_sync_info };
	CodeStruct inner_before_sync         = parse_struct(txt("struct NestedInnerBeforeSync { s32 before; s32 broken(foo{ struct Inner { s32 x; }; s32 after; };"), inner_before_sync_options);
	expect(inner_before_sync.ast != nullptr && inner_before_sync->Type == CT_Struct, txt("cpp inner-before-sync recovery result"));
	Code inner_before_sync_member = inner_before_sync.ast != nullptr && inner_before_sync->Body.ast != nullptr ? inner_before_sync->Body->Front : Code{ nullptr };
	if  (inner_before_sync_member != nullptr) inner_before_sync_member = inner_before_sync_member->Next;
	expect(inner_before_sync_member != nullptr && inner_before_sync_member->Type == CT_Untyped, txt("cpp inner-before-sync recovery retains failed member"));
	if (inner_before_sync_member != nullptr)
		expect(str_are_equal(strbuilder_to_str(code_to_strbuilder(inner_before_sync_member)), txt("s32 broken(foo{ struct Inner { s32 x; };")), txt("cpp inner-before-sync recovery exact failed span"));
	Code   inner_before_sync_after = inner_before_sync_member != nullptr ? inner_before_sync_member->Next : Code{ nullptr };
	expect(inner_before_sync_after != nullptr && inner_before_sync_after->Type == CT_Variable, txt("cpp inner-before-sync recovery keeps following member typed"));
	if (inner_before_sync_after != nullptr)
		expect(str_are_equal(inner_before_sync_after->Name, txt("after")), txt("cpp inner-before-sync recovery following member name"));

	ParseInfo  global_namespace_recovery_info    = {};
	Opts_parse global_namespace_recovery_options = { &ctx, &global_namespace_recovery_info };
	CodeBody   global_namespace_recovery         = parse_global_body(txt("namespace NestedScopeInGlobal { s32 before; s32 broken(s32 { 1; 2 } foo{ 3; s32 after; } struct Later { s32 value; };"), global_namespace_recovery_options);
	expect(global_namespace_recovery.ast             != nullptr && global_namespace_recovery->Type == CT_Global_Body, txt("cpp global namespace recovery body"));
	expect(global_namespace_recovery_info.result.ast == ((Code)global_namespace_recovery).ast,                        txt("cpp global namespace recovery ParseInfo result"));
	Code   global_namespace_node = global_namespace_recovery.ast != nullptr ? global_namespace_recovery->Front : Code{ nullptr };
	expect(global_namespace_node != nullptr && global_namespace_node->Type == CT_Namespace, txt("cpp global namespace recovery preserves namespace"));
	Code global_namespace_member = global_namespace_node != nullptr && global_namespace_node->Body.ast != nullptr ? global_namespace_node->Body->Front : Code{ nullptr };
	if  (global_namespace_member != nullptr) global_namespace_member = global_namespace_member->Next;
	expect(global_namespace_member != nullptr && global_namespace_member->Type == CT_Untyped, txt("cpp global namespace recovery retains failed member"));
	if (global_namespace_member != nullptr)
		expect(str_are_equal(strbuilder_to_str(code_to_strbuilder(global_namespace_member)), txt("s32 broken(s32 { 1; 2 } foo{ 3;")), txt("cpp global namespace recovery exact failed span"));
	Code   global_namespace_after = global_namespace_member != nullptr ? global_namespace_member->Next : Code{ nullptr };
	expect(global_namespace_after != nullptr && global_namespace_after->Type == CT_Variable, txt("cpp global namespace recovery keeps following member typed"));
	if (global_namespace_after != nullptr)
		expect(str_are_equal(global_namespace_after->Name, txt("after")), txt("cpp global namespace recovery following member name"));
	Code   global_later_struct = global_namespace_node != nullptr ? global_namespace_node->Next : Code{ nullptr };
	expect(global_later_struct != nullptr && global_later_struct->Type == CT_Struct, txt("cpp global namespace recovery preserves following global struct"));
	if (global_later_struct != nullptr)
		expect(str_are_equal(global_later_struct->Name, txt("Later")), txt("cpp global namespace recovery following global name"));

	ParseInfo  global_struct_recovery_info    = {};
	Opts_parse global_struct_recovery_options = { &ctx, &global_struct_recovery_info };
	CodeBody   global_struct_recovery         = parse_global_body(txt("struct GlobalNestedRecovery { s32 before; s32 broken(s32 value; s32 after; }; struct LaterGlobal { s32 value; };"), global_struct_recovery_options);
	expect(global_struct_recovery.ast != nullptr && global_struct_recovery->Type == CT_Global_Body, txt("cpp global struct recovery body"));
	Code global_struct_node   = global_struct_recovery.ast != nullptr                                            ? global_struct_recovery->Front   : Code{ nullptr };
	Code global_struct_before = global_struct_node         != nullptr && global_struct_node->Body.ast != nullptr ? global_struct_node->Body->Front : Code{ nullptr };
	Code global_struct_member = global_struct_before       != nullptr                                            ? global_struct_before->Next      : Code{ nullptr };
	Code global_struct_after  = global_struct_member       != nullptr                                            ? global_struct_member->Next      : Code{ nullptr };
	Code global_later_class   = global_struct_node         != nullptr                                            ? global_struct_node->Next        : Code{ nullptr };
	expect(global_struct_node   != nullptr && global_struct_node->Type   == CT_Struct,  txt("cpp global struct recovery preserves partial struct"));
	expect(global_struct_member != nullptr && global_struct_member->Type == CT_Untyped, txt("cpp global struct recovery retains failed member"));
	if (global_struct_member != nullptr)
		expect(str_are_equal(strbuilder_to_str(code_to_strbuilder(global_struct_member)), txt("s32 broken(s32 value;")), txt("cpp global struct recovery exact failed span"));
	expect(global_struct_after != nullptr && global_struct_after->Type == CT_Variable, txt("cpp global struct recovery keeps following member"));
	if (global_struct_after != nullptr)
		expect(str_are_equal(global_struct_after->Name, txt("after")), txt("cpp global struct recovery following member name"));
	expect(global_later_class != nullptr && global_later_class->Type == CT_Struct, txt("cpp global struct recovery keeps following global struct"));

	ParseInfo  nested_owner_info    = {};
	Opts_parse nested_owner_options = { &ctx, &nested_owner_info };
	CodeStruct nested_owner         = parse_struct(txt("struct NestedOwner { s32 before; struct InnerRecovery { s32 inner_before; s32 broken(s32 value; s32 inner_after; }; s32 after; };"), nested_owner_options);
	expect(nested_owner.ast             != nullptr && nested_owner->Type == CT_Struct, txt("cpp nested owner recovery result"));
	expect(nested_owner_info.result.ast == ((Code)nested_owner).ast,                   txt("cpp nested owner recovery ParseInfo result"));
	expect(nested_owner_info.messages   != nullptr,                                    txt("cpp nested owner recovery diagnostic"));
	Code nested_owner_before       = nested_owner.ast          != nullptr && nested_owner->Body.ast != nullptr       ? nested_owner->Body->Front       : Code{ nullptr };
	Code nested_owner_inner        = nested_owner_before       != nullptr                                            ? nested_owner_before->Next       : Code{ nullptr };
	Code nested_owner_after        = nested_owner_inner        != nullptr                                            ? nested_owner_inner->Next        : Code{ nullptr };
	Code nested_owner_inner_before = nested_owner_inner        != nullptr && nested_owner_inner->Body.ast != nullptr ? nested_owner_inner->Body->Front : Code{ nullptr };
	Code nested_owner_inner_failed = nested_owner_inner_before != nullptr                                            ? nested_owner_inner_before->Next : Code{ nullptr };
	Code nested_owner_inner_after  = nested_owner_inner_failed != nullptr                                            ? nested_owner_inner_failed->Next : Code{ nullptr };
	expect(nested_owner_before       != nullptr && nested_owner_before->Type       == CT_Variable, txt("cpp nested owner keeps preceding member typed"));
	expect(nested_owner_inner        != nullptr && nested_owner_inner->Type        == CT_Struct,   txt("cpp nested owner retains inner struct AST"));
	expect(nested_owner_inner_before != nullptr && nested_owner_inner_before->Type == CT_Variable, txt("cpp nested inner recovery keeps preceding member typed"));
	expect(nested_owner_inner_failed != nullptr && nested_owner_inner_failed->Type == CT_Untyped,  txt("cpp nested inner recovery retains failed member"));
	expect(nested_owner_inner_after  != nullptr && nested_owner_inner_after->Type  == CT_Variable, txt("cpp nested inner recovery keeps following member typed"));
	expect(nested_owner_after        != nullptr && nested_owner_after->Type        == CT_Variable, txt("cpp nested owner keeps following member typed"));

#if ! GEN_BUILD_DEBUG
	CodeVar       wrong_array_value   = def_variable(def_type(name(u32)), name(invalid_array_expr));
	Opts_def_type wrong_array_options = {};
	wrong_array_options.array_expr  = (Code)wrong_array_value;
	CodeTypename  invalid_array_type = def_type(name(InvalidArrayOption), wrong_array_options);
	expect(((Code)invalid_array_type).ast == Code_Invalid.ast, txt("cpp wrong type invalid code"));

	CodeBody missing_union_body = {};
	CodeUnion missing_union = def_union(name(MissingUnionBody), missing_union_body);
	expect(((Code)missing_union).ast == Code_Invalid.ast, txt("cpp missing body invalid code"));
#endif

	deinit(&ctx);
	if (g_failures != 0) { log_fmt("cpp baseline smoke failed: %d\n", g_failures); return 1; }
	log_fmt("cpp baseline smoke passed\n");
	return 0;
}
