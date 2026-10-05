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
		expect(str_contains(class_text, txt("ClassOption")),     txt("cpp class name serialization"));
		expect(str_contains(class_text, txt("ClassOptionBase")), txt("cpp class parent serialization"));
		expect(str_contains(class_text, txt("IClassOption")),    txt("cpp class interface serialization"));
		expect(str_contains(class_text, txt("public")),          txt("cpp class access serialization"));
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
		expect(str_contains(struct_text, txt("StructOption")),     txt("cpp struct name serialization"));
		expect(str_contains(struct_text, txt("StructOptionBase")), txt("cpp struct parent serialization"));
		expect(str_contains(struct_text, txt("IStructOption")),    txt("cpp struct interface serialization"));
		expect(str_contains(struct_text, txt("public")),           txt("cpp struct access serialization"));
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
		expect(str_contains(enum_text, txt("export")),      txt("cpp enum module flag serialization"));
		expect(str_contains(enum_text, txt("enum class")),  txt("cpp enum specifier serialization"));
		expect(str_contains(enum_text, txt("u32")),         txt("cpp enum type serialization"));
		expect(str_contains(enum_text, txt("Option_None")), txt("cpp enum body serialization"));
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
		expect(str_contains(define_text, txt("OptionMacro")), txt("cpp define name serialization"));
		expect(str_contains(define_text, txt("param + 1")),   txt("cpp define content serialization"));
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
	CodeTypename function_return = def_type(name(u32));
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
		expect(str_contains(constructor_text, txt("ConstructorOptions(")),    txt("cpp constructor name serialization"));
		expect(str_contains(constructor_text, txt("option_member( 0 )")),     txt("cpp constructor initializer serialization"));
		expect(str_contains(constructor_text, txt("constructor_option_arg")), txt("cpp constructor body serialization"));
	}
	Code           destructor_body = untyped_str(txt("release_option_resource();"));
	CodeSpecifiers destructor_specs = def_specifier(Spec_Virtual);
	Opts_def_destructor destructor_options = {};
	destructor_options.body       = destructor_body;
	destructor_options.specifiers = destructor_specs;
	CodeDestructor option_destructor = def_destructor(destructor_options);
	expect(option_destructor.ast != nullptr, txt("cpp destructor options"));
	if (option_destructor.ast != nullptr) {
		expect(option_destructor->Body.ast  == destructor_body.ast, txt("cpp destructor body"));
		expect(option_destructor->Specs.ast == destructor_specs.ast, txt("cpp destructor specifiers"));
	}

	CodeTypename operator_type = def_type(name(OperatorOptionType));
	CodeParams operator_lhs    = def_param(operator_type, name(operator_option_lhs));
	CodeParams operator_rhs    = def_param(operator_type, name(operator_option_rhs));
	CodeParams operator_params = def_params(args(operator_lhs, operator_rhs));
	CodeBody   operator_body   = def_function_body(args(untyped_str(txt("return operator_option_lhs;"))));
	Opts_def_operator operator_options = {};
	operator_options.params     = operator_params;
	operator_options.ret_type   = operator_type;
	operator_options.body       = operator_body;
	operator_options.specifiers = def_specifier(Spec_Inline);
	operator_options.attributes = def_attributes(txt("[[maybe_unused]]"));
	operator_options.mflags     = ModuleFlag_Export;
	CodeOperator option_operator = def_operator(Op_Add, txt(""), operator_options);
	expect(option_operator.ast != nullptr, txt("cpp operator options"));
	if (option_operator.ast != nullptr) {
		expect(option_operator->Params.ast     == operator_params.ast,             txt("cpp operator parameters"));
		expect(option_operator->ReturnType.ast == operator_type.ast,               txt("cpp operator return type"));
		expect(option_operator->Body.ast       == operator_body.ast,               txt("cpp operator body"));
		expect(option_operator->Specs.ast      == operator_options.specifiers.ast, txt("cpp operator specifiers"));
		expect(option_operator->Attributes.ast == operator_options.attributes.ast, txt("cpp operator attributes"));
		expect(option_operator->ModuleFlags    == ModuleFlag_Export,               txt("cpp operator module flags"));
		Str operator_text = strbuilder_to_str(code_to_strbuilder((Code)option_operator));
		expect(str_contains(operator_text, txt("operator +")),                 txt("cpp operator name serialization"));
		expect(str_contains(operator_text, txt("operator_option_lhs")),        txt("cpp operator parameters serialization"));
		expect(str_contains(operator_text, txt("return operator_option_lhs")), txt("cpp operator body serialization"));
		expect(str_contains(operator_text, txt("inline")),                     txt("cpp operator specifier serialization"));
		expect(str_contains(operator_text, txt("[[maybe_unused]]")),           txt("cpp operator attribute serialization"));
		expect(str_contains(operator_text, txt("export")),                     txt("cpp operator module flag serialization"));
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

	Code    untyped        = code_str(char const* baseline_message = "GENCPP_BASELINE";); expect(untyped.ast != nullptr,  txt("cpp untyped"));
	Str     untyped_text   = strbuilder_to_str(code_to_strbuilder(untyped));              expect_fragments(untyped_text,  txt("cpp untyped content"));
	CodeVar parsed_untyped = parse_variable(untyped_text);                                expect_variable(parsed_untyped, txt("cpp untyped reparsed"));

	CodeVar parsed = parse_variable(code(char const* baseline_message = "GENCPP_BASELINE"; ));
	expect_variable(parsed, txt("cpp parsed"));
	expect_fragments(strbuilder_to_str(code_to_strbuilder((Code)parsed)), txt("cpp parsed serialized"));

#if ! GEN_BUILD_DEBUG
	CodeVar       wrong_array_value   = def_variable(def_type(name(u32)), name(invalid_array_expr));
	Opts_def_type wrong_array_options = {};
	wrong_array_options.array_expr  = (Code)wrong_array_value;
	CodeTypename invalid_array_type = def_type(name(InvalidArrayOption), wrong_array_options);
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
