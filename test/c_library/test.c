#define GEN_DONT_USE_FATAL
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
	expect(var->Value     != 0, label);
	if (var->Value != 0) {
		expect(gen_str_contains(var->Value->Content, gen_txt("GENCPP_BASELINE")), label);
	}
}

gen_internal gen_Code_POD code_pod(gen_Code code) { gen_Code_POD result = { (gen_AST*)code }; return result; }

gen_internal void expect_parse_info(void* result, ParseInfo* info, gen_Str label) {
	expect(result != 0, label);
	void* info_result = info->result;
	expect(info_result == result, label);
	expect(info->lexed.tokens.num > 0, label);
	expect(info->lexed.messages == 0 && info->messages == 0, label);
}

#include "dependencies/strings.c"
#include "dependencies/simd.c"

int main()
{
	gen_Context ctx = {0}; gen_init(&ctx);
	expect_text_template(GEN_PRINTF_MAXLEN);
	expect_simd_lanes();

	gen_CodeTypename type    = gen_def_type(name(char), .specifiers = gen_def_specifiers(2, Spec_Const, Spec_Ptr) );
	gen_CodeVar      upfront = gen_def_variable(type, name(baseline_message), 
		.value = gen_untyped_str(code("\"GENCPP_BASELINE\""))
	);
	expect_variable(upfront, gen_txt("c11 upfront"));
	expect_fragments(gen_strbuilder_to_str(gen_code_to_strbuilder(upfront)), gen_txt("c11 upfront serialized"));

	gen_Code           array_expr  = gen_untyped_str(gen_txt("3"));
	gen_CodeSpecifiers type_specs  = gen_def_specifier(Spec_Const);
	gen_CodeAttributes type_attrs  = gen_def_attributes(gen_txt("[[maybe_unused]]"));
	gen_CodeTypename   option_type = gen_def_type(name(Payload),
		.type_tag       = Tag_Struct,
		.gen_array_expr = array_expr,
		.specifiers     = type_specs,
		.attributes     = type_attrs
	);
	expect(option_type != 0, gen_txt("c11 type options"));
	if (option_type != 0) {
		expect(option_type->TypeTag == Tag_Struct,                        gen_txt("c11 type tag"));
		expect((gen_Code)option_type->ArrExpr    == array_expr,           gen_txt("c11 type array expression"));
		expect((gen_Code)option_type->Specs      == (gen_Code)type_specs, gen_txt("c11 type specifiers"));
		expect((gen_Code)option_type->Attributes == (gen_Code)type_attrs, gen_txt("c11 type attributes"));
		gen_Str type_text = gen_strbuilder_to_str(gen_code_to_strbuilder((gen_Code)option_type));
		expect(gen_str_contains(type_text, gen_txt("struct Payload")),   gen_txt("c11 type tag serialization"));
		expect(gen_str_contains(type_text, gen_txt("const")),            gen_txt("c11 type specifier serialization"));
		expect(gen_str_contains(type_text, gen_txt("[[maybe_unused]]")), gen_txt("c11 type attribute serialization"));
	}

	gen_Code           value          = gen_untyped_str(code("\"UPFRONT_OPTION\""));
	gen_CodeSpecifiers variable_specs = gen_def_specifier(Spec_Static);
	gen_CodeAttributes variable_attrs = gen_def_attributes(gen_txt("[[maybe_unused]]"));
	gen_CodeVar option_variable       = gen_def_variable(option_type, name(option_payload),
		.value      = value,
		.specifiers = variable_specs,
		.attributes = variable_attrs,
		.mflags     = ModuleFlag_Export
	);
	expect(option_variable != 0, gen_txt("c11 variable options"));
	if (option_variable != 0) {
		expect((gen_Code)option_variable->ValueType   == (gen_Code)option_type,    gen_txt("c11 variable type"));
		expect(          option_variable->Value       == value,                    gen_txt("c11 variable value"));
		expect((gen_Code)option_variable->Specs       == (gen_Code)variable_specs, gen_txt("c11 variable specifiers"));
		expect((gen_Code)option_variable->Attributes  == (gen_Code)variable_attrs, gen_txt("c11 variable attributes"));
		expect(          option_variable->ModuleFlags == ModuleFlag_Export,        gen_txt("c11 variable module flags"));
		gen_Str variable_text = gen_strbuilder_to_str(gen_code_to_strbuilder((gen_Code)option_variable));
		expect(gen_str_contains(variable_text, gen_txt("option_payload")),   gen_txt("c11 variable name serialization"));
		expect(gen_str_contains(variable_text, gen_txt("UPFRONT_OPTION")),   gen_txt("c11 variable value serialization"));
		expect(gen_str_contains(variable_text, gen_txt("static")),           gen_txt("c11 variable specifier serialization"));
		expect(gen_str_contains(variable_text, gen_txt("[[maybe_unused]]")), gen_txt("c11 variable attribute serialization"));
		expect(gen_str_contains(variable_text, gen_txt("[ 3 ]")),            gen_txt("c11 variable array serialization"));
		expect(gen_str_contains(variable_text, gen_txt("export")),           gen_txt("c11 variable module flag serialization"));
	}

	gen_CodeVar        class_member       = gen_def_variable(gen_def_type(name(u32)), name(class_option_member));
	gen_CodeBody       class_body         = gen_def_class_body(1, code_pod((gen_Code)class_member));
	gen_CodeTypename   class_parent       = gen_def_type(name(ClassOptionBase));
	gen_CodeTypename   class_interfaces[] = { gen_def_type(name(IClassOption)) };
	gen_CodeAttributes class_attributes   = gen_def_attributes(gen_txt("[[maybe_unused]]"));
	gen_CodeSpecifiers class_specifiers   = gen_def_specifier(Spec_Final);
	gen_CodeClass option_class = gen_def_class(name(ClassOption),
		.body           = class_body,
		.parent         = class_parent,
		.parent_access  = AccessSpec_Public,
		.attributes     = class_attributes,
		.interfaces     = class_interfaces,
		.num_interfaces = 1,
		.specifiers     = class_specifiers,
		.mflags         = ModuleFlag_Export
	);
	expect(option_class != 0, gen_txt("c11 class options"));
	if (option_class != 0) {
		expect((gen_Code)option_class->Body             == (gen_Code)class_body,          gen_txt("c11 class body"));
		expect((gen_Code)option_class->ParentType       == (gen_Code)class_parent,        gen_txt("c11 class parent"));
		expect(          option_class->ParentAccess     == AccessSpec_Public,             gen_txt("c11 class parent access"));
		expect((gen_Code)option_class->Attributes       == (gen_Code)class_attributes,    gen_txt("c11 class attributes"));
		expect((gen_Code)option_class->ParentType->Next == (gen_Code)class_interfaces[0], gen_txt("c11 class interface"));
		expect((gen_Code)option_class->Specs            == (gen_Code)class_specifiers,    gen_txt("c11 class specifiers"));
		expect(          option_class->ModuleFlags      == ModuleFlag_Export,             gen_txt("c11 class module flags"));
		gen_Str class_text = gen_strbuilder_to_str(gen_code_to_strbuilder((gen_Code)option_class));
		expect(gen_str_contains(class_text, gen_txt("ClassOption")),         gen_txt("c11 class name serialization"));
		expect(gen_str_contains(class_text, gen_txt("ClassOptionBase")),     gen_txt("c11 class parent serialization"));
		expect(gen_str_contains(class_text, gen_txt("IClassOption")),        gen_txt("c11 class interface serialization"));
		expect(gen_str_contains(class_text, gen_txt("public")),              gen_txt("c11 class access serialization"));
		expect(gen_str_contains(class_text, gen_txt("class_option_member")), gen_txt("c11 class body serialization"));
		expect(gen_str_contains(class_text, gen_txt("[[maybe_unused]]")),    gen_txt("c11 class attribute serialization"));
		expect(gen_str_contains(class_text, gen_txt("final")),               gen_txt("c11 class final specifier serialization"));
		expect(gen_str_contains(class_text, gen_txt("export")),              gen_txt("c11 class module flag serialization"));
	}

	gen_CodeVar        struct_member       = gen_def_variable(gen_def_type(name(u32)), name(struct_option_member));
	gen_CodeBody       struct_body         = gen_def_struct_body(1, code_pod((gen_Code)struct_member));
	gen_CodeTypename   struct_parent       = gen_def_type(name(StructOptionBase));
	gen_CodeTypename   struct_interfaces[] = { gen_def_type(name(IStructOption)) };
	gen_CodeAttributes struct_attributes   = gen_def_attributes(gen_txt("[[maybe_unused]]"));
	gen_CodeSpecifiers struct_specifiers   = gen_def_specifier(Spec_Final);
	gen_CodeStruct option_struct = gen_def_struct(name(StructOption),
		.body           = struct_body,
		.parent         = struct_parent,
		.parent_access  = AccessSpec_Public,
		.attributes     = struct_attributes,
		.interfaces     = struct_interfaces,
		.num_interfaces = 1,
		.specifiers     = struct_specifiers,
		.mflags         = ModuleFlag_Export
	);
	expect(option_struct != 0, gen_txt("c11 struct options"));
	if (option_struct != 0) {
		expect((gen_Code)option_struct->Body             == (gen_Code)struct_body,          gen_txt("c11 struct body"));
		expect((gen_Code)option_struct->ParentType       == (gen_Code)struct_parent,        gen_txt("c11 struct parent"));
		expect(          option_struct->ParentAccess     == AccessSpec_Public,              gen_txt("c11 struct parent access"));
		expect((gen_Code)option_struct->Attributes       == (gen_Code)struct_attributes,    gen_txt("c11 struct attributes"));
		expect((gen_Code)option_struct->ParentType->Next == (gen_Code)struct_interfaces[0], gen_txt("c11 struct interface"));
		expect((gen_Code)option_struct->Specs            == (gen_Code)struct_specifiers,    gen_txt("c11 struct specifiers"));
		expect(          option_struct->ModuleFlags      == ModuleFlag_Export,              gen_txt("c11 struct module flags"));
		gen_Str struct_text = gen_strbuilder_to_str(gen_code_to_strbuilder((gen_Code)option_struct));
		expect(gen_str_contains(struct_text, gen_txt("StructOption")),         gen_txt("c11 struct name serialization"));
		expect(gen_str_contains(struct_text, gen_txt("StructOptionBase")),     gen_txt("c11 struct parent serialization"));
		expect(gen_str_contains(struct_text, gen_txt("IStructOption")),        gen_txt("c11 struct interface serialization"));
		expect(gen_str_contains(struct_text, gen_txt("public")),               gen_txt("c11 struct access serialization"));
		expect(gen_str_contains(struct_text, gen_txt("struct_option_member")), gen_txt("c11 struct body serialization"));
		expect(gen_str_contains(struct_text, gen_txt("[[maybe_unused]]")),     gen_txt("c11 struct attribute serialization"));
		expect(gen_str_contains(struct_text, gen_txt("final")),                gen_txt("c11 struct final specifier serialization"));
		expect(gen_str_contains(struct_text, gen_txt("export")),               gen_txt("c11 struct module flag serialization"));
	}

	gen_CodeBody       enum_body       = gen_def_enum_body(1, code_pod((gen_Code)gen_untyped_str(gen_txt("Option_None"))));
	gen_CodeTypename   enum_underlying = gen_def_type(name(u32));
	gen_CodeAttributes enum_attributes = gen_def_attributes(gen_txt("[[maybe_unused]]"));
	gen_CodeEnum option_enum = gen_def_enum(name(OptionEnum),
		.body       = enum_body,
		.type       = enum_underlying,
		.specifier  = EnumDecl_Class,
		.attributes = enum_attributes,
		.mflags     = ModuleFlag_Export
	);
	expect(option_enum != 0, gen_txt("c11 enum options"));
	if (option_enum != 0) {
		expect((gen_Code)option_enum->Body           == (gen_Code)enum_body,       gen_txt("c11 enum body"));
		expect((gen_Code)option_enum->UnderlyingType == (gen_Code)enum_underlying, gen_txt("c11 enum underlying type"));
		expect(          option_enum->Type           == CT_Enum_Class,             gen_txt("c11 enum specifier"));
		expect((gen_Code)option_enum->Attributes     == (gen_Code)enum_attributes, gen_txt("c11 enum attributes"));
		expect(          option_enum->ModuleFlags    == ModuleFlag_Export,         gen_txt("c11 enum module flags"));
		gen_Str enum_text = gen_strbuilder_to_str(gen_code_to_strbuilder((gen_Code)option_enum));
		expect(gen_str_contains(enum_text, gen_txt("export")),           gen_txt("c11 enum module flag serialization"));
		expect(gen_str_contains(enum_text, gen_txt("enum class")),       gen_txt("c11 enum specifier serialization"));
		expect(gen_str_contains(enum_text, gen_txt("[[maybe_unused]]")), gen_txt("c11 enum attribute serialization"));
		expect(gen_str_contains(enum_text, gen_txt("u32")),              gen_txt("c11 enum type serialization"));
		expect(gen_str_contains(enum_text, gen_txt("Option_None")),      gen_txt("c11 enum body serialization"));
	}

	gen_Code     enum_macro = gen_untyped_str(gen_txt("ENUM_BASE_TYPE"));
	gen_CodeEnum macro_enum = gen_def_enum(name(MacroOptionEnum),
		.body       = gen_def_enum_body(1, code_pod((gen_Code)gen_untyped_str(gen_txt("Macro_None")))),
		.type_macro = enum_macro
	);
	expect(macro_enum != 0, gen_txt("c11 enum type macro option"));
	if (macro_enum != 0) {
		expect(macro_enum->UnderlyingTypeMacro == enum_macro, gen_txt("c11 enum type macro"));
		expect(gen_str_contains(gen_strbuilder_to_str(gen_code_to_strbuilder((gen_Code)macro_enum)), gen_txt("ENUM_BASE_TYPE")), gen_txt("c11 enum type macro serialization"));
	}

	gen_CodeBody       union_body       = gen_def_union_body(1, code_pod((gen_Code)gen_untyped_str(gen_txt("u32 union_option_value;"))));
	gen_CodeAttributes union_attributes = gen_def_attributes(gen_txt("[[maybe_unused]]"));
	gen_CodeUnion option_union = gen_def_union(name(OptionUnion), union_body,
		.attributes = union_attributes,
		.mflags     = ModuleFlag_Export
	);
	expect(option_union != 0, gen_txt("c11 union options"));
	if (option_union != 0) {
		expect((gen_Code)option_union->Body        == (gen_Code)union_body,       gen_txt("c11 union body"));
		expect((gen_Code)option_union->Attributes  == (gen_Code)union_attributes, gen_txt("c11 union attributes"));
		expect(          option_union->ModuleFlags == ModuleFlag_Export,          gen_txt("c11 union module flags"));
		gen_Str union_text = gen_strbuilder_to_str(gen_code_to_strbuilder((gen_Code)option_union));
		expect(gen_str_contains(union_text, gen_txt("OptionUnion")),        gen_txt("c11 union name serialization"));
		expect(gen_str_contains(union_text, gen_txt("union_option_value")), gen_txt("c11 union body serialization"));
		expect(gen_str_contains(union_text, gen_txt("[[maybe_unused]]")),   gen_txt("c11 union attribute serialization"));
		expect(gen_str_contains(union_text, gen_txt("export")),             gen_txt("c11 union module flag serialization"));
	}

	gen_Code           typedef_type       = (gen_Code)gen_def_type(name(u32));
	gen_CodeAttributes typedef_attributes = gen_def_attributes(gen_txt("[[maybe_unused]]"));
	gen_CodeTypedef option_typedef = gen_def_typedef(name(OptionTypedef), typedef_type,
		.attributes = typedef_attributes,
		.mflags     = ModuleFlag_Export
	);
	expect(option_typedef != 0, gen_txt("c11 typedef options"));
	if (option_typedef != 0) {
		expect(          option_typedef->UnderlyingType == typedef_type,                 gen_txt("c11 typedef underlying type"));
		expect((gen_Code)option_typedef->Attributes     == (gen_Code)typedef_attributes, gen_txt("c11 typedef attributes"));
		expect(          option_typedef->ModuleFlags    == ModuleFlag_Export,            gen_txt("c11 typedef module flags"));
		gen_Str typedef_text = gen_strbuilder_to_str(gen_code_to_strbuilder((gen_Code)option_typedef));
		expect(gen_str_contains(typedef_text, gen_txt("OptionTypedef")),    gen_txt("c11 typedef name serialization"));
		expect(gen_str_contains(typedef_text, gen_txt("u32")),              gen_txt("c11 typedef type serialization"));
		expect(gen_str_contains(typedef_text, gen_txt("[[maybe_unused]]")), gen_txt("c11 typedef attribute serialization"));
		expect(gen_str_contains(typedef_text, gen_txt("export")),           gen_txt("c11 typedef module flag serialization"));
	}

	gen_CodeTypename   using_type       = gen_def_type(name(u32));
	gen_CodeAttributes using_attributes = gen_def_attributes(gen_txt("[[maybe_unused]]"));
	gen_CodeUsing option_using = gen_def_using(name(OptionUsing), using_type,
		.attributes = using_attributes,
		.mflags     = ModuleFlag_Export
	);
	expect(option_using != 0, gen_txt("c11 using options"));
	if (option_using != 0) {
		expect((gen_Code)option_using->UnderlyingType == (gen_Code)using_type,       gen_txt("c11 using underlying type"));
		expect((gen_Code)option_using->Attributes     == (gen_Code)using_attributes, gen_txt("c11 using attributes"));
		expect(          option_using->ModuleFlags    == ModuleFlag_Export,          gen_txt("c11 using module flags"));
		gen_Str using_text = gen_strbuilder_to_str(gen_code_to_strbuilder((gen_Code)option_using));
		expect(gen_str_contains(using_text, gen_txt("OptionUsing")),      gen_txt("c11 using name serialization"));
		expect(gen_str_contains(using_text, gen_txt("[[maybe_unused]]")), gen_txt("c11 using attribute serialization"));
		expect(gen_str_contains(using_text, gen_txt("export")),           gen_txt("c11 using module flag serialization"));
	}

	gen_CodeParams   template_params = gen_def_param(gen_def_type(name(class)), name(T));
	gen_CodeTemplate option_template = gen_def_template(template_params, (gen_Code)option_struct, .mflags = ModuleFlag_Export);
	expect(option_template != 0, gen_txt("c11 template options"));
	if (option_template != 0) {
		expect((gen_Code)option_template->Params      == (gen_Code)template_params, gen_txt("c11 template parameters"));
		expect(          option_template->Declaration == (gen_Code)option_struct,   gen_txt("c11 template declaration"));
		expect(          option_template->ModuleFlags == ModuleFlag_Export,         gen_txt("c11 template module flags"));
		gen_Str template_text = gen_strbuilder_to_str(gen_code_to_strbuilder((gen_Code)option_template));
		expect(gen_str_contains(template_text, gen_txt("export")),       gen_txt("c11 template module flag serialization"));
		expect(gen_str_contains(template_text, gen_txt("template")),     gen_txt("c11 template serialization"));
		expect(gen_str_contains(template_text, gen_txt("StructOption")), gen_txt("c11 template declaration serialization"));
	}

	gen_Macro      source_macro        = { gen_txt("OptionParamSource"), MT_Expression, MF_Functional }; gen_register_macro(source_macro);
	gen_CodeDefine parsed_macro_source = gen_parse_define(gen_txt("#define OptionParamSource(param) param\n"));
	expect(parsed_macro_source != 0, gen_txt("c11 define parameter source"));
	gen_MacroFlags define_flags  = MF_Functional | MF_Allow_As_Identifier;
	gen_CodeDefine option_define = gen_def_define(name(OptionMacro), MT_Expression,
		.params  = parsed_macro_source->Params,
		.content = gen_txt("param + 1"),
		.flags   = define_flags
	);
	expect(option_define != 0, gen_txt("c11 define options"));
	if (option_define != 0) {
		expect(option_define->Params == parsed_macro_source->Params, gen_txt("c11 define parameters"));
		expect(gen_str_contains(option_define->Body->Content,        gen_txt("param + 1")), gen_txt("c11 define content"));
		gen_Macro* option_macro = lookup_macro(name(OptionMacro));
		expect(option_macro != 0, gen_txt("c11 define registration"));
		if (option_macro != 0) expect(option_macro->Flags == define_flags, gen_txt("c11 define flags"));
		gen_Str define_text = gen_strbuilder_to_str(gen_code_to_strbuilder((gen_Code)option_define));
		expect(gen_str_contains(define_text, gen_txt("OptionMacro")),         gen_txt("c11 define name serialization"));
		expect(gen_str_contains(define_text, gen_txt("OptionMacro( param)")), gen_txt("c11 define parameters serialization"));
		expect(gen_str_contains(define_text, gen_txt("param + 1")),           gen_txt("c11 define content serialization"));
	}

	gen_CodeDefine no_register_define = gen_def_define(name(OptionMacroNoRegister), MT_Expression,
		.dont_register_to_preprocess_macros = true
	);
	expect(no_register_define != 0,                        gen_txt("c11 define registration suppression"));
	expect(lookup_macro(name(OptionMacroNoRegister)) == 0, gen_txt("c11 define suppression flag"));

	gen_CodeInclude local_include = gen_def_include(gen_txt("option_local.h"));
	expect(local_include != 0, gen_txt("c11 local include option"));
	if (local_include != 0) {
		expect(gen_str_contains(local_include->Content, gen_txt("\"option_local.h\"")), gen_txt("c11 quoted include"));
	}

	gen_CodeInclude foreign_include = gen_def_include(gen_txt("option_system.h"), .foreign = true);
	expect(foreign_include != 0, gen_txt("c11 foreign include option"));
	if (foreign_include != 0) {
		expect(gen_str_contains(foreign_include->Content, gen_txt("<option_system.h>")), gen_txt("c11 foreign include path"));
		gen_Str include_text = gen_strbuilder_to_str(gen_code_to_strbuilder((gen_Code)foreign_include));
		expect(gen_str_contains(include_text, gen_txt("#include <option_system.h>")), gen_txt("c11 foreign include directive serialization"));
	}

	gen_CodeModule option_module = gen_def_module(name(OptionModule), .mflags = ModuleFlag_Export);
	expect(option_module != 0, gen_txt("c11 module options"));
	if (option_module != 0) {
		expect(option_module->ModuleFlags == ModuleFlag_Export, gen_txt("c11 module flags"));
		expect(gen_str_contains(gen_strbuilder_to_str(gen_code_to_strbuilder((gen_Code)option_module)), gen_txt("export")), gen_txt("c11 module serialization"));
	}

	gen_CodeVar  namespace_member = gen_def_variable(gen_def_type(name(u32)), name(namespace_option_value));
	gen_CodeBody namespace_body   = gen_def_namespace_body(1, code_pod((gen_Code)namespace_member));
	gen_CodeNS   option_namespace = gen_def_namespace(name(OptionNamespace), namespace_body, .mflags = ModuleFlag_Export);
	expect(option_namespace != 0, gen_txt("c11 namespace options"));
	if (option_namespace != 0) {
		expect((gen_Code)option_namespace->Body        == (gen_Code)namespace_body, gen_txt("c11 namespace body"));
		expect(          option_namespace->ModuleFlags == ModuleFlag_Export,        gen_txt("c11 namespace flags"));
		gen_Str namespace_text = gen_strbuilder_to_str(gen_code_to_strbuilder((gen_Code)option_namespace));
		expect(gen_str_contains(namespace_text, gen_txt("OptionNamespace")),        gen_txt("c11 namespace name serialization"));
		expect(gen_str_contains(namespace_text, gen_txt("namespace_option_value")), gen_txt("c11 namespace body serialization"));
		expect(gen_str_contains(namespace_text, gen_txt("export")),                 gen_txt("c11 namespace serialization"));
	}

	gen_Code       parameter_value  = gen_untyped_str(code(7));
	gen_CodeParams option_parameter = gen_def_param(gen_def_type(name(u32)), name(option_parameter_value), .value = parameter_value);
	expect(option_parameter != 0, gen_txt("c11 parameter options"));
	if (option_parameter != 0) {
		expect(option_parameter->Value == parameter_value, gen_txt("c11 parameter value"));
		gen_Str parameter_text = gen_strbuilder_to_str(gen_code_to_strbuilder((gen_Code)option_parameter));
		expect(gen_str_contains(parameter_text, gen_txt("= 7")), gen_txt("c11 parameter value serialization"));
	}

	gen_CodeParams     function_params     = gen_def_param(gen_def_type(name(u32)), name(function_option_arg));
	gen_CodeTypename   function_return     = gen_def_type(name(FunctionOptionReturn));
	gen_CodeBody       function_body       = gen_def_function_body(1, code_pod((gen_Code)gen_untyped_str(gen_txt("return function_option_arg;"))));
	gen_CodeAttributes function_attributes = gen_def_attributes(gen_txt("[[maybe_unused]]"));
	gen_CodeSpecifiers function_specs      = gen_def_specifier(Spec_Inline);
	gen_CodeFn option_function = gen_def_function(name(OptionFunction),
		.params   = function_params,
		.ret_type = function_return,
		.body     = function_body,
		.specs    = function_specs,
		.attrs    = function_attributes,
		.mflags   = ModuleFlag_Export
	);
	expect(option_function != 0, gen_txt("c11 function options"));
	if (option_function != 0) {
		expect((gen_Code)option_function->Params      == (gen_Code)function_params,     gen_txt("c11 function parameters"));
		expect((gen_Code)option_function->ReturnType  == (gen_Code)function_return,     gen_txt("c11 function return type"));
		expect((gen_Code)option_function->Body        == (gen_Code)function_body,       gen_txt("c11 function body"));
		expect((gen_Code)option_function->Specs       == (gen_Code)function_specs,      gen_txt("c11 function specifiers"));
		expect((gen_Code)option_function->Attributes  == (gen_Code)function_attributes, gen_txt("c11 function attributes"));
		expect(          option_function->ModuleFlags == ModuleFlag_Export, gen_txt("c11 function module flags"));
		gen_Str function_text = gen_strbuilder_to_str(gen_code_to_strbuilder((gen_Code)option_function));
		expect(gen_str_contains(function_text, gen_txt("OptionFunction")),                       gen_txt("c11 function name serialization"));
		expect(gen_str_contains(function_text, gen_txt("FunctionOptionReturn OptionFunction(")), gen_txt("c11 function return type serialization"));
		expect(gen_str_contains(function_text, gen_txt("function_option_arg")),                  gen_txt("c11 function parameters serialization"));
		expect(gen_str_contains(function_text, gen_txt("return function_option_arg")),           gen_txt("c11 function body serialization"));
		expect(gen_str_contains(function_text, gen_txt("inline")),                               gen_txt("c11 function specifier serialization"));
		expect(gen_str_contains(function_text, gen_txt("[[maybe_unused]]")),                     gen_txt("c11 function attribute serialization"));
		expect(gen_str_contains(function_text, gen_txt("export")),                               gen_txt("c11 function module flag serialization"));
	}

	gen_CodeParams constructor_params = gen_def_param(gen_def_type(name(u32)), name(constructor_option_arg));
	gen_Code       initializer_list   = gen_untyped_str(gen_txt("option_member( 0 )"));
	gen_CodeBody   constructor_body   = gen_def_function_body(1, code_pod((gen_Code)gen_untyped_str(gen_txt("option_member = constructor_option_arg;"))));
	gen_CodeConstructor option_constructor = gen_def_constructor(
		.params           = constructor_params,
		.initializer_list = initializer_list,
		.body             = (gen_Code)constructor_body
	);
	expect(option_constructor != 0, gen_txt("c11 constructor options"));
	if (option_constructor != 0) {
		expect((gen_Code)option_constructor->Params          == (gen_Code)constructor_params, gen_txt("c11 constructor parameters"));
		expect(          option_constructor->InitializerList == initializer_list,             gen_txt("c11 constructor initializer list"));
		expect(          option_constructor->Body            == (gen_Code)constructor_body,   gen_txt("c11 constructor body"));
	}

	gen_Code_POD  constructor_pod        = code_pod((gen_Code)option_constructor);
	gen_CodeBody  constructor_class_body = gen_def_class_body(1, constructor_pod);
	gen_CodeClass constructor_class      = gen_def_class(name(ConstructorOptions), .body = constructor_class_body);
	expect(constructor_class != 0, gen_txt("c11 constructor serialization parent"));
	if (constructor_class != 0) {
		gen_Str constructor_text = gen_strbuilder_to_str(gen_code_to_strbuilder((gen_Code)constructor_class));
		expect(gen_str_contains(constructor_text, gen_txt("ConstructorOptions(")),        gen_txt("c11 constructor name serialization"));
		expect(gen_str_contains(constructor_text, gen_txt("u32 constructor_option_arg")), gen_txt("c11 constructor parameter serialization"));
		expect(gen_str_contains(constructor_text, gen_txt("option_member( 0 )")),         gen_txt("c11 constructor initializer serialization"));
		expect(gen_str_contains(constructor_text, gen_txt("constructor_option_arg")),     gen_txt("c11 constructor body serialization"));
	}

	gen_Code           destructor_body   = gen_untyped_str(gen_txt("release_option_resource();"));
	gen_CodeSpecifiers destructor_specs  = gen_def_specifier(Spec_Virtual);
	gen_CodeDestructor option_destructor = gen_def_destructor(
		.body       = destructor_body,
		.specifiers = destructor_specs
	);
	expect(option_destructor != 0, gen_txt("c11 destructor options"));
	if (option_destructor != 0) {
		expect(option_destructor->Body == destructor_body, gen_txt("c11 destructor body"));
		expect((gen_Code)option_destructor->Specs == (gen_Code)destructor_specs, gen_txt("c11 destructor specifiers"));
		gen_Code_POD destructor_pod = code_pod((gen_Code)option_destructor);
		gen_CodeBody destructor_class_body = gen_def_class_body(1, destructor_pod);
		gen_CodeClass destructor_class = gen_def_class(name(DestructorOptions), .body = destructor_class_body);
		expect(destructor_class != 0, gen_txt("c11 destructor serialization parent"));
		if (destructor_class != 0) {
			gen_Str destructor_text = gen_strbuilder_to_str(gen_code_to_strbuilder((gen_Code)destructor_class));
			if (gen_str_contains(destructor_text, gen_txt("virtual ~DestructorOptions()")) == false)   gen_log_fmt("%S\n", destructor_text);
			expect(gen_str_contains(destructor_text, gen_txt("virtual ~DestructorOptions()")), gen_txt("c11 destructor signature serialization"));
			expect(gen_str_contains(destructor_text, gen_txt("release_option_resource();")),   gen_txt("c11 destructor body serialization"));
		}
	}

	gen_CodeTypename   operator_type       = gen_def_type(name(OperatorOptionType));
	gen_CodeTypename   operator_return     = gen_def_type(name(OperatorOptionReturn));
	gen_CodeParams     operator_lhs        = gen_def_param(operator_type, name(operator_option_lhs));
	gen_CodeParams     operator_rhs        = gen_def_param(operator_type, name(operator_option_rhs));
	gen_CodeParams     operator_params     = gen_def_params(2, code_pod((gen_Code)operator_lhs), code_pod((gen_Code)operator_rhs));
	gen_CodeBody       operator_body       = gen_def_function_body(1, code_pod((gen_Code)gen_untyped_str(gen_txt("return operator_option_lhs;"))));
	gen_CodeSpecifiers operator_specs      = gen_def_specifier(Spec_Inline);
	gen_CodeAttributes operator_attributes = gen_def_attributes(gen_txt("[[maybe_unused]]"));
	gen_CodeOperator option_operator = gen_def_operator(Op_Add, gen_txt(""),
		.params     = operator_params,
		.ret_type   = operator_return,
		.body       = operator_body,
		.specifiers = operator_specs,
		.attributes = operator_attributes,
		.mflags     = ModuleFlag_Export
	);
	expect(option_operator != 0, gen_txt("c11 operator options"));
	if (option_operator != 0) {
		expect((gen_Code)option_operator->Params      == (gen_Code)operator_params,     gen_txt("c11 operator parameters"));
		expect((gen_Code)option_operator->ReturnType  == (gen_Code)operator_return,     gen_txt("c11 operator return type"));
		expect((gen_Code)option_operator->Body        == (gen_Code)operator_body,       gen_txt("c11 operator body"));
		expect((gen_Code)option_operator->Specs       == (gen_Code)operator_specs,      gen_txt("c11 operator specifiers"));
		expect((gen_Code)option_operator->Attributes  == (gen_Code)operator_attributes, gen_txt("c11 operator attributes"));
		expect(          option_operator->ModuleFlags == ModuleFlag_Export,             gen_txt("c11 operator module flags"));
		gen_Str operator_text = gen_strbuilder_to_str(gen_code_to_strbuilder((gen_Code)option_operator));
		expect(gen_str_contains(operator_text, gen_txt("OperatorOptionReturn")),       gen_txt("c11 operator return type serialization"));
		expect(gen_str_contains(operator_text, gen_txt("operator +")),                 gen_txt("c11 operator name serialization"));
		expect(gen_str_contains(operator_text, gen_txt("operator_option_lhs")),        gen_txt("c11 operator parameters serialization"));
		expect(gen_str_contains(operator_text, gen_txt("operator_option_rhs")),        gen_txt("c11 operator second parameter serialization"));
		expect(gen_str_contains(operator_text, gen_txt("return operator_option_lhs")), gen_txt("c11 operator body serialization"));
		expect(gen_str_contains(operator_text, gen_txt("inline")),                     gen_txt("c11 operator specifier serialization"));
		expect(gen_str_contains(operator_text, gen_txt("[[maybe_unused]]")),           gen_txt("c11 operator attribute serialization"));
		expect(gen_str_contains(operator_text, gen_txt("export")),                     gen_txt("c11 operator module flag serialization"));
		if (gen_str_starts_with(operator_text, gen_txt("export [[maybe_unused]]  inline\nOperatorOptionReturn")) == false) gen_log_fmt("%S\n", operator_text);
		expect(gen_str_starts_with(operator_text, gen_txt("export [[maybe_unused]]  inline\nOperatorOptionReturn")), gen_txt("c11 operator one-attribute prefix serialization"));
	}

	gen_CodeTypename   cast_type  = gen_def_type(name(s32));
	gen_CodeBody       cast_body  = gen_def_function_body(1, code_pod((gen_Code)gen_untyped_str(gen_txt("return 0;"))));
	gen_CodeSpecifiers cast_specs = gen_def_specifier(Spec_Const);
	gen_CodeOpCast option_cast = gen_def_operator_cast(cast_type,
		.body  = cast_body,
		.specs = cast_specs
	);
	expect(option_cast != 0, gen_txt("c11 operator cast options"));
	if (option_cast != 0) {
		expect((gen_Code)option_cast->ValueType == (gen_Code)cast_type,  gen_txt("c11 operator cast type"));
		expect((gen_Code)option_cast->Body      == (gen_Code)cast_body,  gen_txt("c11 operator cast body"));
		expect((gen_Code)option_cast->Specs     == (gen_Code)cast_specs, gen_txt("c11 operator cast specifiers"));
		gen_Str cast_text = gen_strbuilder_to_str(gen_code_to_strbuilder((gen_Code)option_cast));
		expect(gen_str_contains(cast_text, gen_txt("operator s32")), gen_txt("c11 operator cast serialization"));
		expect(gen_str_contains(cast_text, gen_txt("const")),        gen_txt("c11 operator cast specifier serialization"));
		expect(gen_str_contains(cast_text, gen_txt("return 0")),     gen_txt("c11 operator cast body serialization"));
	}

	gen_Code untyped = gen_code_str(char const* baseline_message = "GENCPP_BASELINE"; );
	expect(untyped != 0, gen_txt("c11 untyped"));

	gen_Str untyped_text = gen_strbuilder_to_str(gen_code_to_strbuilder(untyped));
	expect_fragments(untyped_text,                    gen_txt("c11 untyped content"));
	expect_variable(gen_parse_variable(untyped_text), gen_txt("c11 untyped reparsed"));

	gen_CodeVar parsed = gen_parse_variable(code(char const* baseline_message = "GENCPP_BASELINE"; ));
	expect_variable(parsed,                                                 gen_txt("c11 parsed"));
	expect_fragments(gen_strbuilder_to_str(gen_code_to_strbuilder(parsed)), gen_txt("c11 parsed serialized"));

	gen_CodeClass       parser_class         = gen_parse_class(gen_txt("class ParseClass {};"));
	gen_CodeConstructor parser_constructor   = gen_parse_constructor(gen_txt("ParseClass() {}"));
	gen_CodeDefine      parser_define        = gen_parse_define(gen_txt("#define PARSE_DEFINE 1"));
	gen_CodeDestructor  parser_destructor    = gen_parse_destructor(gen_txt("~ParseClass();"));
	gen_CodeEnum        parser_enum          = gen_parse_enum(gen_txt("enum ParseEnum { ParseValue };"));
	gen_CodeBody        parser_export_body   = gen_parse_export_body(gen_txt("{ int export_value; }"));
	gen_CodeExtern      parser_extern        = gen_parse_extern_link(gen_txt("extern \"C\" { int extern_value; }"));
	gen_CodeFriend      parser_friend        = gen_parse_friend(gen_txt("friend int parse_friend_function();"));
	gen_CodeFn          parser_function      = gen_parse_function(gen_txt("int parse_function();"));
	gen_CodeBody        parser_global_body   = gen_parse_global_body(gen_txt("struct ParseGlobal { int value; };"));
	gen_CodeNS          parser_namespace     = gen_parse_namespace(gen_txt("namespace ParseNamespace {}"));
	gen_CodeOperator    parser_operator      = gen_parse_operator(gen_txt("s32 operator +(s32 lhs, s32 rhs);"));
	gen_CodeOpCast      parser_operator_cast = gen_parse_operator_cast(gen_txt("operator s32() const;"));
	gen_CodeStruct      parser_struct        = gen_parse_struct(gen_txt("struct ParseStruct {};"));
	gen_CodeTemplate    parser_template      = gen_parse_template(gen_txt("template<class T> struct ParseTemplate {};"));
	gen_CodeTypename    parser_type          = gen_parse_type(gen_txt("const s32*"));
	gen_CodeTypedef     parser_typedef       = gen_parse_typedef(gen_txt("typedef s32 ParseTypedef;"));
	gen_CodeUnion       parser_union         = gen_parse_union(gen_txt("union ParseUnion { s32 value; };"));
	gen_CodeUsing       parser_using         = gen_parse_using(gen_txt("using ParseUsing = s32;"));
	gen_CodeVar         parser_variable      = gen_parse_variable(gen_txt("s32 parse_variable;"));

	expect(parser_class         != 0 && parser_class->Type         == CT_Class,             gen_txt("c11 parse class fixture"));
	expect(parser_constructor   != 0 && parser_constructor->Type   == CT_Constructor,       gen_txt("c11 parse constructor fixture"));
	expect(parser_define        != 0 && parser_define->Type        == CT_Preprocess_Define, gen_txt("c11 parse define fixture"));
	expect(parser_destructor    != 0 && parser_destructor->Type    == CT_Destructor_Fwd,    gen_txt("c11 parse destructor fixture"));
	expect(parser_enum          != 0 && parser_enum->Type          == CT_Enum,              gen_txt("c11 parse enum fixture"));
	expect(parser_export_body   != 0 && parser_export_body->Type   == CT_Export_Body,       gen_txt("c11 parse export body fixture"));
	expect(parser_extern        != 0 && parser_extern->Type        == CT_Extern_Linkage,    gen_txt("c11 parse extern fixture"));
	expect(parser_friend        != 0 && parser_friend->Type        == CT_Friend,            gen_txt("c11 parse friend fixture"));
	expect(parser_function      != 0 && parser_function->Type      == CT_Function_Fwd,      gen_txt("c11 parse function fixture"));
	expect(parser_global_body   != 0 && parser_global_body->Type   == CT_Global_Body,       gen_txt("c11 parse global body fixture"));
	expect(parser_namespace     != 0 && parser_namespace->Type     == CT_Namespace,         gen_txt("c11 parse namespace fixture"));
	expect(parser_operator      != 0 && parser_operator->Type      == CT_Operator_Fwd,      gen_txt("c11 parse operator fixture"));
	expect(parser_operator_cast != 0 && parser_operator_cast->Type == CT_Operator_Cast_Fwd, gen_txt("c11 parse operator cast fixture"));
	expect(parser_struct        != 0 && parser_struct->Type        == CT_Struct,            gen_txt("c11 parse struct fixture"));
	expect(parser_template      != 0 && parser_template->Type      == CT_Template,          gen_txt("c11 parse template fixture"));
	expect(parser_type          != 0 && parser_type->Type          == CT_Typename,          gen_txt("c11 parse type fixture"));
	expect(parser_typedef       != 0 && parser_typedef->Type       == CT_Typedef,           gen_txt("c11 parse typedef fixture"));
	expect(parser_union         != 0 && parser_union->Type         == CT_Union,             gen_txt("c11 parse union fixture"));
	expect(parser_using         != 0 && parser_using->Type         == CT_Using,             gen_txt("c11 parse using fixture"));
	expect(parser_variable      != 0 && parser_variable->Type      == CT_Variable,          gen_txt("c11 parse variable fixture"));

	gen_CodeBody parsed_global_options = gen_parse_global_body(gen_txt("struct ParseGlobalOptions { s32 value; };"));
	expect(parsed_global_options != 0, gen_txt("c11 default parse global body options"));
	void* default_result      = parsed_global_options;
	void* default_info_result = ctx.gen_parse_info.result;
	expect(default_info_result == default_result,                                       gen_txt("c11 default parse info result"));
	expect(ctx.gen_parse_info.lexed.tokens.num > 0,                                     gen_txt("c11 default parse info tokens"));
	expect(ctx.gen_parse_info.lexed.messages  == 0 && ctx.gen_parse_info.messages == 0, gen_txt("c11 default parse info messages"));

	ParseInfo caller_info = {0};
	gen_CodeBody parsed_explicit_global = gen_parse_global_body(gen_txt("struct ParseExplicitOptions { s32 value; };"), .ctx = & ctx, .info = & caller_info);
	expect(parsed_explicit_global != 0, gen_txt("c11 explicit parse global body options"));
	void* explicit_result      = parsed_explicit_global;
	void* explicit_info_result = caller_info.result;
	expect(explicit_info_result == explicit_result, gen_txt("c11 explicit parse info result"));
	expect(caller_info.lexed.tokens.num > 0,        gen_txt("c11 explicit parse info tokens"));

	ParseInfo variable_info = {0};
	gen_CodeVar parsed_option_variable = gen_parse_variable(gen_txt("s32 parsed_option_variable;"), .ctx = & ctx, .info = & variable_info);
	expect(parsed_option_variable != 0 && parsed_option_variable->Type == CT_Variable,        gen_txt("c11 explicit parse variable options"));
	expect(gen_str_contains(parsed_option_variable->Name, gen_txt("parsed_option_variable")), gen_txt("c11 explicit parse variable name"));
	void* variable_result      = parsed_option_variable;
	void* variable_info_result = variable_info.result;
	expect(variable_info_result == variable_result, gen_txt("c11 explicit parse variable info result"));

	ParseInfo lexer_info = {0};
	gen_CodeVar parsed_lexer_prefix = gen_parse_variable(gen_txt("s32 parsed_lexer_prefix;@"), .ctx = & ctx, .info = & lexer_info);
	expect(parsed_lexer_prefix != 0 && parsed_lexer_prefix->Type == CT_Variable, gen_txt("c11 parse lexer prefix result"));
	void* lexer_result      = parsed_lexer_prefix;
	void* lexer_info_result = lexer_info.result;
	expect(lexer_info_result == lexer_result, gen_txt("c11 parse lexer prefix info result"));
	expect(lexer_info.lexed.tokens.num > 0,   gen_txt("c11 parse lexer prefix retained tokens"));
	expect(lexer_info.lexed.messages  != 0,   gen_txt("c11 parse lexer prefix diagnostic"));

	ParseInfo matrix_info = {0};
	gen_Opts_parse_global_body legacy_parse_options = { .ctx = & ctx, .info = & matrix_info };
	gen_CodeBody matrix_global_body = gen_parse_global_body(gen_txt("struct ParseGlobal { int value; };"), .ctx = legacy_parse_options.ctx, .info = legacy_parse_options.info);
	expect(matrix_global_body != 0 && matrix_global_body->Type == CT_Global_Body, gen_txt("c11 global body options matrix"));
	expect_parse_info(matrix_global_body, & matrix_info, gen_txt("c11 global body ParseInfo matrix"));

	gen_CodeClass matrix_class = gen_parse_class(gen_txt("class ParseClass {};"), .ctx = & ctx, .info = & matrix_info);
	expect(matrix_class != 0 && matrix_class->Type == CT_Class, gen_txt("c11 class options matrix"));
	expect_parse_info(matrix_class, & matrix_info, gen_txt("c11 class ParseInfo matrix"));

	gen_CodeConstructor matrix_constructor = gen_parse_constructor(gen_txt("ParseClass() {}"), .ctx = & ctx, .info = & matrix_info);
	expect(matrix_constructor != 0 && matrix_constructor->Type == CT_Constructor, gen_txt("c11 constructor options matrix"));
	expect_parse_info(matrix_constructor, & matrix_info, gen_txt("c11 constructor ParseInfo matrix"));

	gen_CodeDefine matrix_define = gen_parse_define(gen_txt("#define PARSE_DEFINE 1"), .ctx = & ctx, .info = & matrix_info);
	expect(matrix_define != 0 && matrix_define->Type == CT_Preprocess_Define, gen_txt("c11 define options matrix"));
	expect_parse_info(matrix_define, & matrix_info, gen_txt("c11 define ParseInfo matrix"));

	gen_CodeDestructor matrix_destructor = gen_parse_destructor(gen_txt("~ParseClass();"), .ctx = & ctx, .info = & matrix_info);
	expect(matrix_destructor != 0 && matrix_destructor->Type == CT_Destructor_Fwd, gen_txt("c11 destructor options matrix"));
	expect_parse_info(matrix_destructor, & matrix_info, gen_txt("c11 destructor ParseInfo matrix"));

	gen_CodeEnum matrix_enum = gen_parse_enum(gen_txt("enum ParseEnum { ParseValue };"), .ctx = & ctx, .info = & matrix_info);
	expect(matrix_enum != 0 && matrix_enum->Type == CT_Enum, gen_txt("c11 enum options matrix"));
	expect_parse_info(matrix_enum, & matrix_info, gen_txt("c11 enum ParseInfo matrix"));

	gen_CodeBody matrix_export_body = gen_parse_export_body(gen_txt("{ int export_value; }"), .ctx = & ctx, .info = & matrix_info);
	expect(matrix_export_body != 0 && matrix_export_body->Type == CT_Export_Body, gen_txt("c11 export body options matrix"));
	expect_parse_info(matrix_export_body, & matrix_info, gen_txt("c11 export body ParseInfo matrix"));

	gen_CodeExtern matrix_extern = gen_parse_extern_link(gen_txt("extern \"C\" { int extern_value; }"), .ctx = & ctx, .info = & matrix_info);
	expect(matrix_extern != 0 && matrix_extern->Type == CT_Extern_Linkage, gen_txt("c11 extern options matrix"));
	expect_parse_info(matrix_extern, & matrix_info, gen_txt("c11 extern ParseInfo matrix"));

	gen_CodeFriend matrix_friend = gen_parse_friend(gen_txt("friend int parse_friend_function();"), .ctx = & ctx, .info = & matrix_info);
	expect(matrix_friend != 0 && matrix_friend->Type == CT_Friend, gen_txt("c11 friend options matrix"));
	expect_parse_info(matrix_friend, & matrix_info, gen_txt("c11 friend ParseInfo matrix"));

	gen_CodeFn matrix_function = gen_parse_function(gen_txt("int parse_function();"), .ctx = & ctx, .info = & matrix_info);
	expect(matrix_function != 0 && matrix_function->Type == CT_Function_Fwd, gen_txt("c11 function options matrix"));
	expect_parse_info(matrix_function, & matrix_info, gen_txt("c11 function ParseInfo matrix"));

	gen_CodeNS matrix_namespace = gen_parse_namespace(gen_txt("namespace ParseNamespace {}"), .ctx = & ctx, .info = & matrix_info);
	expect(matrix_namespace != 0 && matrix_namespace->Type == CT_Namespace, gen_txt("c11 namespace options matrix"));
	expect_parse_info(matrix_namespace, & matrix_info, gen_txt("c11 namespace ParseInfo matrix"));

	gen_CodeOperator matrix_operator = gen_parse_operator(gen_txt("s32 operator +(s32 lhs, s32 rhs);"), .ctx = & ctx, .info = & matrix_info);
	expect(matrix_operator != 0 && matrix_operator->Type == CT_Operator_Fwd, gen_txt("c11 operator options matrix"));
	expect_parse_info(matrix_operator, & matrix_info, gen_txt("c11 operator ParseInfo matrix"));

	gen_CodeOpCast matrix_operator_cast = gen_parse_operator_cast(gen_txt("operator s32() const;"), .ctx = & ctx, .info = & matrix_info);
	expect(matrix_operator_cast != 0 && matrix_operator_cast->Type == CT_Operator_Cast_Fwd, gen_txt("c11 operator cast options matrix"));
	expect_parse_info(matrix_operator_cast, & matrix_info, gen_txt("c11 operator cast ParseInfo matrix"));

	gen_CodeStruct matrix_struct = gen_parse_struct(gen_txt("struct ParseStruct {};"), .ctx = & ctx, .info = & matrix_info);
	expect(matrix_struct != 0 && matrix_struct->Type == CT_Struct, gen_txt("c11 struct options matrix"));
	expect_parse_info(matrix_struct, & matrix_info, gen_txt("c11 struct ParseInfo matrix"));

	gen_CodeTemplate matrix_template = gen_parse_template(gen_txt("template<class T> struct ParseTemplate {};"), .ctx = & ctx, .info = & matrix_info);
	expect(matrix_template != 0 && matrix_template->Type == CT_Template, gen_txt("c11 template options matrix"));
	expect_parse_info(matrix_template, & matrix_info, gen_txt("c11 template ParseInfo matrix"));

	gen_CodeTypename matrix_type = gen_parse_type(gen_txt("const s32*"), .ctx = & ctx, .info = & matrix_info);
	expect(matrix_type != 0 && matrix_type->Type == CT_Typename, gen_txt("c11 type options matrix"));
	expect_parse_info(matrix_type, & matrix_info, gen_txt("c11 type ParseInfo matrix"));

	gen_CodeTypedef matrix_typedef = gen_parse_typedef(gen_txt("typedef s32 ParseTypedef;"), .ctx = & ctx, .info = & matrix_info);
	expect(matrix_typedef != 0 && matrix_typedef->Type == CT_Typedef, gen_txt("c11 typedef options matrix"));
	expect_parse_info(matrix_typedef, & matrix_info, gen_txt("c11 typedef ParseInfo matrix"));

	gen_CodeUnion matrix_union = gen_parse_union(gen_txt("union ParseUnion { s32 value; };"), .ctx = & ctx, .info = & matrix_info);
	expect(matrix_union != 0 && matrix_union->Type == CT_Union, gen_txt("c11 union options matrix"));
	expect_parse_info(matrix_union, & matrix_info, gen_txt("c11 union ParseInfo matrix"));

	gen_CodeUsing matrix_using = gen_parse_using(gen_txt("using ParseUsing = s32;"), .ctx = & ctx, .info = & matrix_info);
	expect(matrix_using != 0 && matrix_using->Type == CT_Using, gen_txt("c11 using options matrix"));
	expect_parse_info(matrix_using, & matrix_info, gen_txt("c11 using ParseInfo matrix"));

	gen_CodeVar matrix_variable = gen_parse_variable(gen_txt("s32 parse_variable;"), .ctx = & ctx, .info = & matrix_info);
	expect(matrix_variable != 0 && matrix_variable->Type == CT_Variable, gen_txt("c11 variable options matrix"));
	expect_parse_info(matrix_variable, & matrix_info, gen_txt("c11 variable ParseInfo matrix"));

	ParseInfo    comma_info        = {0};
	gen_CodeBody parsed_comma_body = gen_parse_global_body(gen_txt("struct ParseComma { s32 value; };\n,"), .ctx = & ctx, .info = & comma_info);
	expect(parsed_comma_body != 0 && parsed_comma_body->Type == CT_Global_Body, gen_txt("c11 comma parser result"));
	void* comma_result      = parsed_comma_body;
	void* comma_info_result = comma_info.result;
	expect(comma_info_result          == comma_result,                        gen_txt("c11 comma ParseInfo result"));
	expect(comma_info.lexed.tokens.num > 0 && comma_info.lexed.messages == 0, gen_txt("c11 comma retained tokens"));
	expect(comma_info.messages        != 0,                                   gen_txt("c11 comma parser diagnostic"));
	if (comma_info.messages != 0)
		expect(gen_str_contains(comma_info.messages->Content, gen_txt("Dangling comma")), gen_txt("c11 comma diagnostic content"));

	ParseInfo    recovery_info  = {0};
	gen_CodeBody recovered_body = gen_parse_global_body(gen_txt("s32 before; s32 broken(s32 value; s32 after;"), .ctx = &ctx, .info = &recovery_info);
	gen_Str      recovery_text  = {0};
	if (recovered_body != 0) recovery_text = gen_strbuilder_to_str(gen_code_to_strbuilder((gen_Code)recovered_body));
	expect(recovered_body != 0 && recovered_body->Type == CT_Global_Body, gen_txt("c11 file-scope recovery body"));

	void* recovered_result = recovered_body;
	expect(recovery_info.result   == recovered_result, gen_txt("c11 file-scope recovery ParseInfo result"));
	expect(recovery_info.messages != 0,                gen_txt("c11 file-scope recovery diagnostic"));
	if (recovery_info.messages != 0) {
		expect(gen_str_contains(recovery_info.messages->Content, gen_txt("Expected")), gen_txt("c11 file-scope recovery mismatch text"));
		expect(recovery_info.messages->Next == 0,                                      gen_txt("c11 file-scope recovery single diagnostic"));
	}
	expect(gen_str_contains(recovery_text, gen_txt("before")), gen_txt("c11 file-scope recovery keeps preceding member"));
	expect(gen_str_contains(recovery_text, gen_txt("after")), gen_txt("c11 file-scope recovery keeps following member"));
	gen_Code recovered_member = 0;
	if (recovered_body != 0) recovered_member = recovered_body->Front;
	gen_Code recovered_before = recovered_member;
	expect(recovered_before != 0 && recovered_before->Type == CT_Variable, gen_txt("c11 mismatch recovery keeps preceding member typed"));
	if (recovered_before != 0)
		expect(gen_str_are_equal(recovered_before->Name, gen_txt("before")), gen_txt("c11 mismatch recovery preceding member name"));
	if (recovered_member != 0) recovered_member = recovered_member->Next;
	expect(recovered_member != 0 && recovered_member->Type == CT_Untyped, gen_txt("c11 file-scope recovery stores malformed member as untyped"));
	if (recovered_member != 0)
		expect(gen_str_are_equal(gen_strbuilder_to_str(gen_code_to_strbuilder(recovered_member)), gen_txt("s32 broken(s32 value;")), gen_txt("c11 file-scope recovery retains exact mismatch span"));
	gen_Code recovered_after = recovered_member != 0 ? recovered_member->Next : 0;
	expect(recovered_after != 0 && recovered_after->Type == CT_Variable, gen_txt("c11 file-scope recovery keeps following member typed"));
	if (recovered_after != 0)
		expect(gen_str_are_equal(recovered_after->Name, gen_txt("after")), gen_txt("c11 file-scope recovery following member name"));

	ParseInfo    invalid_member_info = {0};
	gen_CodeBody invalid_member_body = gen_parse_global_body(gen_txt("s32 before; = { 1; 2 }; s32 after;"), .ctx = &ctx, .info = &invalid_member_info);
	gen_Str      invalid_member_text = {0};
	if (invalid_member_body != 0)
		invalid_member_text = gen_strbuilder_to_str(gen_code_to_strbuilder((gen_Code)invalid_member_body));
	expect(invalid_member_body != 0 && invalid_member_body->Type == CT_Global_Body, gen_txt("c11 invalid member recovery body"));
	void* invalid_member_result = invalid_member_body;
	expect(invalid_member_info.result == invalid_member_result, gen_txt("c11 invalid member recovery ParseInfo result"));
	expect(invalid_member_info.messages != 0, gen_txt("c11 invalid member recovery diagnostic"));
	if (invalid_member_info.messages != 0) {
		expect(gen_str_contains(invalid_member_info.messages->Content, gen_txt("Failed to parse member")), gen_txt("c11 invalid member recovery diagnostic text"));
		expect(invalid_member_info.messages->Next == 0,                                                    gen_txt("c11 invalid member recovery single diagnostic"));
	}
	expect(gen_str_contains(invalid_member_text, gen_txt("before")), gen_txt("c11 invalid member recovery keeps preceding member"));
	expect(gen_str_contains(invalid_member_text, gen_txt("after")),  gen_txt("c11 invalid member recovery keeps following member"));
	gen_Code invalid_member_node = 0;
	if (invalid_member_body != 0) invalid_member_node = invalid_member_body->Front;
	gen_Code invalid_member_before = invalid_member_node;
	expect(invalid_member_before != 0 && invalid_member_before->Type == CT_Variable, gen_txt("c11 invalid member keeps preceding member typed"));
	if (invalid_member_before != 0)
		expect(gen_str_are_equal(invalid_member_before->Name, gen_txt("before")), gen_txt("c11 invalid member preceding member name"));
	if (invalid_member_node != 0) invalid_member_node = invalid_member_node->Next;
	expect(invalid_member_node != 0 && invalid_member_node->Type == CT_Untyped, gen_txt("c11 invalid member recovery stores member as untyped"));
	if (invalid_member_node != 0)
		expect(gen_str_are_equal(gen_strbuilder_to_str(gen_code_to_strbuilder(invalid_member_node)), gen_txt("= { 1; 2 };")), gen_txt("c11 invalid member recovery retains exact source span"));
	gen_Code invalid_member_after = invalid_member_node != 0 ? invalid_member_node->Next : 0;
	expect(invalid_member_after != 0 && invalid_member_after->Type == CT_Variable, gen_txt("c11 invalid member recovery keeps following member typed"));
	if (invalid_member_after != 0)
		expect(gen_str_are_equal(invalid_member_after->Name, gen_txt("after")), gen_txt("c11 invalid member recovery following member name"));

	ParseInfo      import_recovery_info    = {0};
	gen_Opts_parse import_recovery_options = { .ctx = &ctx, .info = &import_recovery_info };
	gen_CodeBody   import_recovery_body    = gen_parse_global_body(gen_txt("s32 before; import foo; s32 after;"), .ctx = import_recovery_options.ctx, .info = import_recovery_options.info);
	gen_Code       import_recovery_node    = 0;
	if (import_recovery_body != 0) import_recovery_node = import_recovery_body->Front;
	gen_Code import_recovery_before = import_recovery_node;
	expect(import_recovery_before != 0 && import_recovery_before->Type == CT_Variable, gen_txt("c11 import recovery keeps preceding member typed"));
	if (import_recovery_before != 0)
		expect(gen_str_are_equal(import_recovery_before->Name, gen_txt("before")), gen_txt("c11 import recovery preceding member name"));
	if (import_recovery_node != 0) import_recovery_node = import_recovery_node->Next;
	expect(import_recovery_body != 0 && import_recovery_body->Type == CT_Global_Body, gen_txt("c11 import recovery body"));
	expect(import_recovery_info.messages != 0, gen_txt("c11 import recovery diagnostic"));
	if (import_recovery_info.messages != 0)
		expect(import_recovery_info.messages->Next == 0, gen_txt("c11 import recovery single diagnostic"));
	expect(import_recovery_node != 0 && import_recovery_node->Type == CT_Untyped, gen_txt("c11 import recovery preserves member as untyped"));
	if (import_recovery_node != 0)
		expect(gen_str_are_equal(gen_strbuilder_to_str(gen_code_to_strbuilder(import_recovery_node)), gen_txt("import foo;")), gen_txt("c11 import recovery exact source span"));
	gen_Code import_recovery_after = import_recovery_node != 0 ? import_recovery_node->Next : 0;
	expect(import_recovery_after != 0 && import_recovery_after->Type == CT_Variable, gen_txt("c11 import recovery keeps following member typed"));
	if (import_recovery_after != 0)
		expect(gen_str_are_equal(import_recovery_after->Name, gen_txt("after")), gen_txt("c11 import recovery following member name"));

	gen_Macro unbounded_macro = { gen_txt("UnboundedRecoveryMacro"), MT_Expression, MF_Null };
	gen_register_macro(unbounded_macro);
	ParseInfo      macro_recovery_info    = {0};
	gen_Opts_parse macro_recovery_options = { .ctx = &ctx, .info = &macro_recovery_info };
	gen_CodeBody   macro_recovery_body    = gen_parse_global_body(gen_txt("s32 before; UnboundedRecoveryMacro; s32 after;"), .ctx = macro_recovery_options.ctx, .info = macro_recovery_options.info);
	gen_Code       macro_recovery_node    = 0;
	if (macro_recovery_body != 0) macro_recovery_node = macro_recovery_body->Front;
	gen_Code macro_recovery_before = macro_recovery_node;
	expect(macro_recovery_before != 0 && macro_recovery_before->Type == CT_Variable, gen_txt("c11 macro recovery keeps preceding member typed"));
	if (macro_recovery_before != 0)
		expect(gen_str_are_equal(macro_recovery_before->Name, gen_txt("before")), gen_txt("c11 macro recovery preceding member name"));
	if (macro_recovery_node != 0) macro_recovery_node = macro_recovery_node->Next;
	expect(macro_recovery_body          != 0 && macro_recovery_body->Type == CT_Global_Body, gen_txt("c11 unbounded macro recovery body"));
	expect(macro_recovery_info.messages != 0,                                                gen_txt("c11 unbounded macro recovery diagnostic"));
	if (macro_recovery_info.messages != 0)
		expect(macro_recovery_info.messages->Next == 0, gen_txt("c11 unbounded macro recovery single diagnostic"));
	expect(macro_recovery_node != 0 && macro_recovery_node->Type == CT_Untyped, gen_txt("c11 unbounded macro recovery preserves member as untyped"));
	if (macro_recovery_node != 0)
		expect(gen_str_are_equal(gen_strbuilder_to_str(gen_code_to_strbuilder(macro_recovery_node)), gen_txt("UnboundedRecoveryMacro;")), gen_txt("c11 unbounded macro recovery exact source span"));
	gen_Code macro_recovery_after = macro_recovery_node != 0 ? macro_recovery_node->Next : 0;
	expect(macro_recovery_after != 0 && macro_recovery_after->Type == CT_Variable, gen_txt("c11 unbounded macro recovery keeps following member typed"));
	if (macro_recovery_after != 0)
		expect(gen_str_are_equal(macro_recovery_after->Name, gen_txt("after")), gen_txt("c11 unbounded macro recovery following member name"));

	ParseInfo    eof_recovery_info = {0};
	gen_CodeBody eof_recovery_body = gen_parse_global_body(gen_txt("s32 before; s32 broken(s32 value"), .ctx = &ctx, .info = &eof_recovery_info);
	gen_Code     eof_recovery_node = 0;
	if (eof_recovery_body != 0) eof_recovery_node = eof_recovery_body->Front;
	gen_Code eof_recovery_before = eof_recovery_node;
	expect(eof_recovery_before != 0 && eof_recovery_before->Type == CT_Variable, gen_txt("c11 EOF recovery keeps preceding member typed"));
	if (eof_recovery_before != 0)
		expect(gen_str_are_equal(eof_recovery_before->Name, gen_txt("before")), gen_txt("c11 EOF recovery preceding member name"));
	if (eof_recovery_node != 0) eof_recovery_node = eof_recovery_node->Next;
	expect(eof_recovery_body          != 0 && eof_recovery_body->Type == CT_Global_Body, gen_txt("c11 end-of-slice recovery body"));
	expect(eof_recovery_info.messages != 0,                                              gen_txt("c11 end-of-slice recovery diagnostic"));
	if (eof_recovery_info.messages != 0)
		expect(eof_recovery_info.messages->Next == 0, gen_txt("c11 end-of-slice recovery single diagnostic"));
	expect(eof_recovery_node != 0 && eof_recovery_node->Type == CT_Untyped, gen_txt("c11 end-of-slice member is untyped"));
	if (eof_recovery_node != 0)
		expect(gen_str_are_equal(gen_strbuilder_to_str(gen_code_to_strbuilder(eof_recovery_node)), gen_txt("s32 broken(s32 value")), gen_txt("c11 end-of-slice recovery exact source span"));
	if (eof_recovery_node != 0)
		expect(eof_recovery_node->Next == 0, gen_txt("c11 end-of-slice recovery has no following node"));

	ParseInfo    comma_recovery_info = {0};
	gen_CodeBody comma_recovery_body = gen_parse_global_body(gen_txt("s32 before; , s32 after;"), .ctx = &ctx, .info = &comma_recovery_info);
	gen_Code     comma_recovery_node = 0;
	if (comma_recovery_body != 0) comma_recovery_node = comma_recovery_body->Front;
	gen_Code comma_recovery_before = comma_recovery_node;
	expect(comma_recovery_before != 0 && comma_recovery_before->Type == CT_Variable, gen_txt("c11 comma recovery keeps preceding member typed"));
	if (comma_recovery_before != 0)
		expect(gen_str_are_equal(comma_recovery_before->Name, gen_txt("before")), gen_txt("c11 comma recovery preceding member name"));
	if (comma_recovery_node != 0) comma_recovery_node = comma_recovery_node->Next;
	expect(comma_recovery_body          != 0 && comma_recovery_body->Type == CT_Global_Body, gen_txt("c11 comma recovery body"));
	expect(comma_recovery_info.messages != 0,                                                gen_txt("c11 comma recovery diagnostic"));
	if (comma_recovery_info.messages != 0) {
		expect(gen_str_contains(comma_recovery_info.messages->Content, gen_txt("Dangling comma")), gen_txt("c11 comma recovery diagnostic text"));
		expect(comma_recovery_info.messages->Next == 0,                                            gen_txt("c11 comma recovery single diagnostic"));
	}
	expect(comma_recovery_node != 0 && comma_recovery_node->Type == CT_Untyped, gen_txt("c11 comma recovery preserves token as untyped"));
	if (comma_recovery_node != 0)
		expect(gen_str_are_equal(gen_strbuilder_to_str(gen_code_to_strbuilder(comma_recovery_node)), gen_txt(",")), gen_txt("c11 comma recovery exact source span"));
	gen_Code comma_recovery_after = comma_recovery_node != 0 ? comma_recovery_node->Next : 0;
	expect(comma_recovery_after != 0 && comma_recovery_after->Type == CT_Variable, gen_txt("c11 comma recovery keeps following member typed"));
	if (comma_recovery_after != 0)
		expect(gen_str_are_equal(comma_recovery_after->Name, gen_txt("after")), gen_txt("c11 comma recovery following member name"));

	gen_CodeBody recovery_reuse = gen_parse_global_body(gen_txt("s32 reused_after_recovery;"), .ctx = &ctx, .info = &recovery_info);
	gen_Str recovery_reuse_text = {0};
	if (recovery_reuse != 0)
		recovery_reuse_text = gen_strbuilder_to_str(gen_code_to_strbuilder((gen_Code)recovery_reuse));
	expect(recovery_reuse         != 0 && recovery_reuse->Type == CT_Global_Body,   gen_txt("c11 parser context reuse after recovery"));
	expect(recovery_info.messages == 0,                                             gen_txt("c11 parser context reuse clears prior diagnostics"));
	expect(gen_str_contains(recovery_reuse_text, gen_txt("reused_after_recovery")), gen_txt("c11 parser context reuse parses later input"));

	ParseInfo      nested_struct_info    = {0};
	gen_Opts_parse nested_struct_options = { .ctx = &ctx, .info = &nested_struct_info };
	gen_CodeStruct nested_struct         = gen_parse_struct(gen_txt("struct NestedRecovery { s32 before; s32 broken(s32 value; s32 after; };"), .ctx = nested_struct_options.ctx, .info = nested_struct_options.info);
	expect(nested_struct != 0 && nested_struct->Type == CT_Struct, gen_txt("c11 nested struct recovery result"));
	void* nested_struct_result      = nested_struct;
	void* nested_struct_info_result = nested_struct_info.result;
	expect(nested_struct_info_result   == nested_struct_result, gen_txt("c11 nested struct recovery ParseInfo result"));
	expect(nested_struct_info.messages != 0,                    gen_txt("c11 nested struct recovery diagnostic"));
	if (nested_struct_info.messages != 0)
		expect(gen_str_contains(nested_struct_info.messages->Content, gen_txt("Expected")), gen_txt("c11 nested struct recovery diagnostic text"));
	gen_Code nested_struct_member = nested_struct != 0 && nested_struct->Body != 0 ? nested_struct->Body->Front : 0;
	expect(nested_struct_member != 0 && nested_struct_member->Type == CT_Variable, gen_txt("c11 nested struct recovery keeps preceding member typed"));
	if (nested_struct_member != 0)
		expect(gen_str_are_equal(nested_struct_member->Name, gen_txt("before")), gen_txt("c11 nested struct recovery preceding member name"));
	if (nested_struct_member != 0) nested_struct_member = nested_struct_member->Next;
	expect(nested_struct_member != 0 && nested_struct_member->Type == CT_Untyped, gen_txt("c11 nested struct recovery retains failed member"));
	if (nested_struct_member != 0)
		expect(gen_str_are_equal(gen_strbuilder_to_str(gen_code_to_strbuilder(nested_struct_member)), gen_txt("s32 broken(s32 value;")), gen_txt("c11 nested struct recovery exact failed span"));
	gen_Code nested_struct_after = nested_struct_member != 0 ? nested_struct_member->Next : 0;
	expect(nested_struct_after != 0 && nested_struct_after->Type == CT_Variable, gen_txt("c11 nested struct recovery keeps following member typed"));
	if (nested_struct_after != 0)
		expect(gen_str_are_equal(nested_struct_after->Name, gen_txt("after")), gen_txt("c11 nested struct recovery following member name"));
	gen_Str nested_struct_text = {0};
	if (nested_struct != 0)
		nested_struct_text = gen_strbuilder_to_str(gen_code_to_strbuilder((gen_Code)nested_struct));
	expect(gen_str_contains(nested_struct_text, gen_txt("s32 after")), gen_txt("c11 nested struct recovery serializes following member"));

	ParseInfo      nested_namespace_info    = {0};
	gen_Opts_parse nested_namespace_options = { .ctx = &ctx, .info = &nested_namespace_info };
	gen_CodeNS     nested_namespace         = gen_parse_namespace(gen_txt("namespace NestedRecovery { s32 before; s32 broken(s32 value; s32 after; }"), .ctx = nested_namespace_options.ctx, .info = nested_namespace_options.info);
	expect(nested_namespace != 0 && nested_namespace->Type == CT_Namespace, gen_txt("c11 nested namespace recovery result"));
	void* nested_namespace_result      = nested_namespace;
	void* nested_namespace_info_result = nested_namespace_info.result;
	expect(nested_namespace_info_result == nested_namespace_result, gen_txt("c11 nested namespace recovery ParseInfo result"));
	expect(nested_namespace_info.messages != 0,                     gen_txt("c11 nested namespace recovery diagnostic"));
	if (nested_namespace_info.messages != 0)
		expect(gen_str_contains(nested_namespace_info.messages->Content, gen_txt("Expected")), gen_txt("c11 nested namespace recovery diagnostic text"));
	gen_Code nested_namespace_member = nested_namespace != 0 && nested_namespace->Body != 0 ? nested_namespace->Body->Front : 0;
	expect(nested_namespace_member != 0 && nested_namespace_member->Type == CT_Variable, gen_txt("c11 nested namespace recovery keeps preceding member typed"));
	if (nested_namespace_member != 0)
		expect(gen_str_are_equal(nested_namespace_member->Name, gen_txt("before")), gen_txt("c11 nested namespace recovery preceding member name"));
	if (nested_namespace_member != 0) nested_namespace_member = nested_namespace_member->Next;
	expect(nested_namespace_member != 0 && nested_namespace_member->Type == CT_Untyped, gen_txt("c11 nested namespace recovery retains failed member"));
	if (nested_namespace_member != 0)
		expect(gen_str_are_equal(gen_strbuilder_to_str(gen_code_to_strbuilder(nested_namespace_member)), gen_txt("s32 broken(s32 value;")), gen_txt("c11 nested namespace recovery exact failed span"));
	gen_Code nested_namespace_after = nested_namespace_member != 0 ? nested_namespace_member->Next : 0;
	expect(nested_namespace_after != 0 && nested_namespace_after->Type == CT_Variable, gen_txt("c11 nested namespace recovery keeps following member typed"));
	if (nested_namespace_after != 0)
		expect(gen_str_are_equal(nested_namespace_after->Name, gen_txt("after")), gen_txt("c11 nested namespace recovery following member name"));
	gen_Str nested_namespace_text = {0};
	if (nested_namespace != 0)
		nested_namespace_text = gen_strbuilder_to_str(gen_code_to_strbuilder((gen_Code)nested_namespace));
	expect(gen_str_contains(nested_namespace_text, gen_txt("s32 after")), gen_txt("c11 nested namespace recovery serializes following member"));

	ParseInfo      nested_brace_info    = {0};
	gen_Opts_parse nested_brace_options = { .ctx = &ctx, .info = &nested_brace_info };
	gen_CodeStruct nested_brace_struct  = gen_parse_struct(gen_txt("struct NestedBraceRecovery { s32 before; s32 broken(s32 { 1; s32 after; };"), .ctx = nested_brace_options.ctx, .info = nested_brace_options.info);
	expect(nested_brace_struct != 0 && nested_brace_struct->Type == CT_Struct, gen_txt("c11 nested brace recovery result"));
	void* nested_brace_result      = nested_brace_struct;
	void* nested_brace_info_result = nested_brace_info.result;
	expect(nested_brace_info_result == nested_brace_result, gen_txt("c11 nested brace recovery ParseInfo result"));
	expect(nested_brace_info.messages != 0, gen_txt("c11 nested brace recovery diagnostic"));
	gen_Code nested_brace_member = nested_brace_struct != 0 && nested_brace_struct->Body != 0 ? nested_brace_struct->Body->Front : 0;
	expect(nested_brace_member != 0 && nested_brace_member->Type == CT_Variable, gen_txt("c11 nested brace recovery keeps preceding member typed"));
	if (nested_brace_member != 0) nested_brace_member = nested_brace_member->Next;
	expect(nested_brace_member != 0 && nested_brace_member->Type == CT_Untyped, gen_txt("c11 nested brace recovery retains failed member"));
	if (nested_brace_member != 0)
		expect(gen_str_are_equal(gen_strbuilder_to_str(gen_code_to_strbuilder(nested_brace_member)), gen_txt("s32 broken(s32 { 1;")), gen_txt("c11 nested brace recovery exact failed span"));
	gen_Code nested_brace_after = nested_brace_member != 0 ? nested_brace_member->Next : 0;
	expect(nested_brace_after != 0 && nested_brace_after->Type == CT_Variable, gen_txt("c11 nested brace recovery keeps following member typed"));
	if (nested_brace_after != 0)
		expect(gen_str_are_equal(nested_brace_after->Name, gen_txt("after")), gen_txt("c11 nested brace recovery following member name"));
	gen_Str nested_brace_text = {0};
	if (nested_brace_struct != 0)
		nested_brace_text = gen_strbuilder_to_str(gen_code_to_strbuilder((gen_Code)nested_brace_struct));
	expect(gen_str_contains(nested_brace_text, gen_txt("s32 after")), gen_txt("c11 nested brace recovery serializes following member"));

	ParseInfo      nested_namespace_brace_info    = {0};
	gen_Opts_parse nested_namespace_brace_options = { .ctx = &ctx, .info = &nested_namespace_brace_info };
	gen_CodeNS     nested_namespace_brace         = gen_parse_namespace(gen_txt("namespace NestedBraceRecovery { s32 before; s32 broken(s32 { 1; s32 after; }"), .ctx = nested_namespace_brace_options.ctx, .info = nested_namespace_brace_options.info);
	expect(nested_namespace_brace != 0 && nested_namespace_brace->Type == CT_Namespace, gen_txt("c11 nested namespace brace recovery result"));
	void* nested_namespace_brace_result      = nested_namespace_brace;
	void* nested_namespace_brace_info_result = nested_namespace_brace_info.result;
	expect(nested_namespace_brace_info_result == nested_namespace_brace_result, gen_txt("c11 nested namespace brace recovery ParseInfo result"));
	gen_Code nested_namespace_brace_member = nested_namespace_brace != 0 && nested_namespace_brace->Body != 0 ? nested_namespace_brace->Body->Front : 0;
	expect(nested_namespace_brace_member != 0 && nested_namespace_brace_member->Type == CT_Variable, gen_txt("c11 nested namespace brace recovery keeps preceding member typed"));
	if (nested_namespace_brace_member != 0) nested_namespace_brace_member = nested_namespace_brace_member->Next;
	expect(nested_namespace_brace_member != 0 && nested_namespace_brace_member->Type == CT_Untyped, gen_txt("c11 nested namespace brace recovery retains failed member"));
	if (nested_namespace_brace_member != 0)
		expect(gen_str_are_equal(gen_strbuilder_to_str(gen_code_to_strbuilder(nested_namespace_brace_member)), gen_txt("s32 broken(s32 { 1;")), gen_txt("c11 nested namespace brace recovery exact failed span"));
	gen_Code nested_namespace_brace_after = nested_namespace_brace_member != 0 ? nested_namespace_brace_member->Next : 0;
	expect(nested_namespace_brace_after != 0 && nested_namespace_brace_after->Type == CT_Variable, gen_txt("c11 nested namespace brace recovery keeps following member typed"));
	if (nested_namespace_brace_after != 0)
		expect(gen_str_are_equal(nested_namespace_brace_after->Name, gen_txt("after")), gen_txt("c11 nested namespace brace recovery following member name"));
	gen_Str nested_namespace_brace_text = {0};
	if (nested_namespace_brace != 0)
		nested_namespace_brace_text = gen_strbuilder_to_str(gen_code_to_strbuilder((gen_Code)nested_namespace_brace));
	expect(gen_str_contains(nested_namespace_brace_text, gen_txt("s32 after")), gen_txt("c11 nested namespace brace recovery serializes following member"));

	ParseInfo      nested_brace_tail_info    = {0};
	gen_Opts_parse nested_brace_tail_options = { .ctx = &ctx, .info = &nested_brace_tail_info };
	gen_CodeStruct nested_brace_tail_struct  = gen_parse_struct(gen_txt("struct NestedBraceTailRecovery { s32 before; s32 broken(s32 { 1; 2 } foo{ 3; s32 after; };"), .ctx = nested_brace_tail_options.ctx, .info = nested_brace_tail_options.info);
	expect(nested_brace_tail_struct != 0 && nested_brace_tail_struct->Type == CT_Struct, gen_txt("c11 nested brace tail recovery result"));
	gen_Code nested_brace_tail_member = nested_brace_tail_struct != 0 && nested_brace_tail_struct->Body != 0 ? nested_brace_tail_struct->Body->Front : 0;
	if (nested_brace_tail_member != 0) nested_brace_tail_member = nested_brace_tail_member->Next;
	expect(nested_brace_tail_member != 0 && nested_brace_tail_member->Type == CT_Untyped, gen_txt("c11 nested brace tail recovery retains failed member"));
	if (nested_brace_tail_member != 0)
		expect(gen_str_are_equal(gen_strbuilder_to_str(gen_code_to_strbuilder(nested_brace_tail_member)), gen_txt("s32 broken(s32 { 1; 2 } foo{ 3;")), gen_txt("c11 nested brace tail recovery exact failed span"));
	gen_Code nested_brace_tail_after = nested_brace_tail_member != 0 ? nested_brace_tail_member->Next : 0;
	expect(nested_brace_tail_after != 0 && nested_brace_tail_after->Type == CT_Variable, gen_txt("c11 nested brace tail recovery keeps following member typed"));
	if (nested_brace_tail_after != 0)
		expect(gen_str_are_equal(nested_brace_tail_after->Name, gen_txt("after")), gen_txt("c11 nested brace tail recovery following member name"));

	ParseInfo      nested_namespace_tail_info    = {0};
	gen_Opts_parse nested_namespace_tail_options = { .ctx = &ctx, .info = &nested_namespace_tail_info };
	gen_CodeNS     nested_namespace_tail         = gen_parse_namespace(gen_txt("namespace NestedBraceTailRecovery { s32 before; s32 broken(s32 { 1; 2 } foo{ 3; s32 after; }"), .ctx = nested_namespace_tail_options.ctx, .info = nested_namespace_tail_options.info);
	expect(nested_namespace_tail != 0 && nested_namespace_tail->Type == CT_Namespace, gen_txt("c11 nested namespace brace tail recovery result"));
	void* nested_namespace_tail_result      = nested_namespace_tail;
	void* nested_namespace_tail_info_result = nested_namespace_tail_info.result;
	expect(nested_namespace_tail_info_result == nested_namespace_tail_result, gen_txt("c11 nested namespace brace tail recovery ParseInfo result"));
	gen_Code nested_namespace_tail_member = nested_namespace_tail != 0 && nested_namespace_tail->Body != 0 ? nested_namespace_tail->Body->Front : 0;
	if (nested_namespace_tail_member != 0) nested_namespace_tail_member = nested_namespace_tail_member->Next;
	expect(nested_namespace_tail_member != 0 && nested_namespace_tail_member->Type == CT_Untyped, gen_txt("c11 nested namespace brace tail recovery retains failed member"));
	if (nested_namespace_tail_member != 0)
		expect(gen_str_are_equal(gen_strbuilder_to_str(gen_code_to_strbuilder(nested_namespace_tail_member)), gen_txt("s32 broken(s32 { 1; 2 } foo{ 3;")), gen_txt("c11 nested namespace brace tail recovery exact failed span"));
	gen_Code nested_namespace_tail_after = nested_namespace_tail_member != 0 ? nested_namespace_tail_member->Next : 0;
	expect(nested_namespace_tail_after != 0 && nested_namespace_tail_after->Type == CT_Variable, gen_txt("c11 nested namespace brace tail recovery keeps following member typed"));
	if (nested_namespace_tail_after != 0)
		expect(gen_str_are_equal(nested_namespace_tail_after->Name, gen_txt("after")), gen_txt("c11 nested namespace brace tail recovery following member name"));

	ParseInfo      nested_inner_close_info    = {0};
	gen_Opts_parse nested_inner_close_options = { .ctx = &ctx, .info = &nested_inner_close_info };
	gen_CodeStruct nested_inner_close_struct  = gen_parse_struct(gen_txt("struct NestedInnerCloseRecovery { s32 before; s32 broken(s32 { 1; 2 } foo{ 3; s32 after; struct Inner { s32 x; }; s32 tail; };"), .ctx = nested_inner_close_options.ctx, .info = nested_inner_close_options.info);
	expect(nested_inner_close_struct != 0 && nested_inner_close_struct->Type == CT_Struct, gen_txt("c11 nested inner-close recovery result"));
	gen_Code nested_inner_close_member = nested_inner_close_struct != 0 && nested_inner_close_struct->Body != 0 ? nested_inner_close_struct->Body->Front : 0;
	if (nested_inner_close_member != 0) nested_inner_close_member = nested_inner_close_member->Next;
	expect(nested_inner_close_member != 0 && nested_inner_close_member->Type == CT_Untyped, gen_txt("c11 nested inner-close recovery retains failed member"));
	if (nested_inner_close_member != 0)
		expect(gen_str_are_equal(gen_strbuilder_to_str(gen_code_to_strbuilder(nested_inner_close_member)), gen_txt("s32 broken(s32 { 1; 2 } foo{ 3;")), gen_txt("c11 nested inner-close recovery exact failed span"));
	gen_Code nested_inner_close_after = nested_inner_close_member != 0 ? nested_inner_close_member->Next : 0;
	expect(nested_inner_close_after != 0 && nested_inner_close_after->Type == CT_Variable, gen_txt("c11 nested inner-close recovery keeps following member typed"));
	if (nested_inner_close_after != 0)
		expect(gen_str_are_equal(nested_inner_close_after->Name, gen_txt("after")), gen_txt("c11 nested inner-close recovery following member name"));
	gen_Code nested_inner_close_inner = nested_inner_close_after != 0 ? nested_inner_close_after->Next : 0;
	expect(nested_inner_close_inner != 0 && nested_inner_close_inner->Type == CT_Struct, gen_txt("c11 nested inner-close recovery parses nested struct"));
	gen_Code nested_inner_close_tail = nested_inner_close_inner != 0 ? nested_inner_close_inner->Next : 0;
	expect(nested_inner_close_tail != 0 && nested_inner_close_tail->Type == CT_Variable, gen_txt("c11 nested inner-close recovery parses trailing member"));

	ParseInfo      inner_before_sync_info    = {0};
	gen_Opts_parse inner_before_sync_options = { .ctx = &ctx, .info = &inner_before_sync_info };
	gen_CodeStruct inner_before_sync         = gen_parse_struct(gen_txt("struct NestedInnerBeforeSync { s32 before; s32 broken(foo{ struct Inner { s32 x; }; s32 after; };"), .ctx = inner_before_sync_options.ctx, .info = inner_before_sync_options.info);
	expect(inner_before_sync != 0 && inner_before_sync->Type == CT_Struct, gen_txt("c11 inner-before-sync recovery result"));
	gen_Code inner_before_sync_member = inner_before_sync != 0 && inner_before_sync->Body != 0 ? inner_before_sync->Body->Front : 0;
	expect(inner_before_sync_member != 0 && inner_before_sync_member->Type == CT_Variable, gen_txt("c11 inner-before-sync recovery keeps preceding member typed"));
	if (inner_before_sync_member != 0) inner_before_sync_member = inner_before_sync_member->Next;
	expect(inner_before_sync_member != 0 && inner_before_sync_member->Type == CT_Untyped, gen_txt("c11 inner-before-sync recovery retains failed member"));
	if (inner_before_sync_member != 0)
		expect(gen_str_are_equal(gen_strbuilder_to_str(gen_code_to_strbuilder(inner_before_sync_member)), gen_txt("s32 broken(foo{ struct Inner { s32 x; };")), gen_txt("c11 inner-before-sync recovery exact failed span"));
	gen_Code inner_before_sync_after = inner_before_sync_member != 0 ? inner_before_sync_member->Next : 0;
	expect(inner_before_sync_after != 0 && inner_before_sync_after->Type == CT_Variable, gen_txt("c11 inner-before-sync recovery keeps following member typed"));
	if (inner_before_sync_after != 0)
		expect(gen_str_are_equal(inner_before_sync_after->Name, gen_txt("after")), gen_txt("c11 inner-before-sync recovery following member name"));

	ParseInfo      global_namespace_recovery_info = {0};
	gen_Opts_parse global_namespace_recovery_options = { .ctx = &ctx, .info = &global_namespace_recovery_info };
	gen_CodeBody   global_namespace_recovery         = gen_parse_global_body(gen_txt("namespace NestedScopeInGlobal { s32 before; s32 broken(s32 { 1; 2 } foo{ 3; s32 after; } struct Later { s32 value; };"), .ctx = global_namespace_recovery_options.ctx, .info = global_namespace_recovery_options.info);
	expect(global_namespace_recovery != 0 && global_namespace_recovery->Type == CT_Global_Body, gen_txt("c11 global namespace recovery body"));
	void* global_namespace_recovery_result      = global_namespace_recovery;
	void* global_namespace_recovery_info_result = global_namespace_recovery_info.result;
	expect(global_namespace_recovery_info_result == global_namespace_recovery_result, gen_txt("c11 global namespace recovery ParseInfo result"));
	gen_Code global_namespace_node = global_namespace_recovery != 0 ? global_namespace_recovery->Front : 0;
	expect(global_namespace_node != 0 && global_namespace_node->Type == CT_Namespace, gen_txt("c11 global namespace recovery preserves namespace"));
	gen_Code global_namespace_member = global_namespace_node != 0 && global_namespace_node->Body != 0 ? global_namespace_node->Body->Front : 0;
	if (global_namespace_member != 0) global_namespace_member = global_namespace_member->Next;
	expect(global_namespace_member != 0 && global_namespace_member->Type == CT_Untyped, gen_txt("c11 global namespace recovery retains failed member"));
	if (global_namespace_member != 0)
		expect(gen_str_are_equal(gen_strbuilder_to_str(gen_code_to_strbuilder(global_namespace_member)), gen_txt("s32 broken(s32 { 1; 2 } foo{ 3;")), gen_txt("c11 global namespace recovery exact failed span"));
	gen_Code global_namespace_after = global_namespace_member != 0 ? global_namespace_member->Next : 0;
	expect(global_namespace_after != 0 && global_namespace_after->Type == CT_Variable, gen_txt("c11 global namespace recovery keeps following member typed"));
	if (global_namespace_after != 0)
		expect(gen_str_are_equal(global_namespace_after->Name, gen_txt("after")), gen_txt("c11 global namespace recovery following member name"));
	gen_Code global_later_struct = global_namespace_node != 0 ? global_namespace_node->Next : 0;
	expect(global_later_struct != 0 && global_later_struct->Type == CT_Struct, gen_txt("c11 global namespace recovery preserves following global struct"));
	if (global_later_struct != 0)
		expect(gen_str_are_equal(global_later_struct->Name, gen_txt("Later")), gen_txt("c11 global namespace recovery following global name"));

	ParseInfo      global_struct_recovery_info    = {0};
	gen_Opts_parse global_struct_recovery_options = { .ctx = &ctx, .info = &global_struct_recovery_info };
	gen_CodeBody   global_struct_recovery         = gen_parse_global_body(gen_txt("struct GlobalNestedRecovery { s32 before; s32 broken(s32 value; s32 after; }; struct LaterGlobal { s32 value; };"), .ctx = global_struct_recovery_options.ctx, .info = global_struct_recovery_options.info);
	expect(global_struct_recovery != 0 && global_struct_recovery->Type == CT_Global_Body, gen_txt("c11 global struct recovery body"));
	gen_Code global_struct_node   = global_struct_recovery != 0                                  ? global_struct_recovery->Front : 0;
	gen_Code global_struct_before = global_struct_node     != 0 && global_struct_node->Body != 0 ? global_struct_node->Body->Front : 0;
	gen_Code global_struct_member = global_struct_before   != 0                                  ? global_struct_before->Next : 0;
	gen_Code global_struct_after  = global_struct_member   != 0                                  ? global_struct_member->Next : 0;
	gen_Code global_later_class   = global_struct_node     != 0                                  ? global_struct_node->Next   : 0;
	expect(global_struct_node   != 0 && global_struct_node->Type   == CT_Struct,   gen_txt("c11 global struct recovery preserves partial struct"));
	expect(global_struct_member != 0 && global_struct_member->Type == CT_Untyped,  gen_txt("c11 global struct recovery retains failed member"));
	if (global_struct_member != 0)
		expect(gen_str_are_equal(gen_strbuilder_to_str(gen_code_to_strbuilder(global_struct_member)), gen_txt("s32 broken(s32 value;")), gen_txt("c11 global struct recovery exact failed span"));
	expect(global_struct_after != 0 && global_struct_after->Type == CT_Variable, gen_txt("c11 global struct recovery keeps following member"));
	if (global_struct_after != 0)
		expect(gen_str_are_equal(global_struct_after->Name, gen_txt("after")), gen_txt("c11 global struct recovery following member name"));
	expect(global_later_class != 0 && global_later_class->Type == CT_Struct, gen_txt("c11 global struct recovery keeps following global struct"));

	ParseInfo      nested_owner_info    = {0};
	gen_Opts_parse nested_owner_options = { .ctx = & ctx, .info = & nested_owner_info };
	gen_CodeStruct nested_owner         = gen_parse_struct(gen_txt("struct NestedOwner { s32 before; struct InnerRecovery { s32 inner_before; s32 broken(s32 value; s32 inner_after; }; s32 after; };"), .ctx = nested_owner_options.ctx, .info = nested_owner_options.info);
	expect(nested_owner != 0 && nested_owner->Type == CT_Struct, gen_txt("c11 nested owner recovery result"));
	void* nested_owner_result      = nested_owner;
	void* nested_owner_info_result = nested_owner_info.result;
	expect(nested_owner_info_result   == nested_owner_result, gen_txt("c11 nested owner recovery ParseInfo result"));
	expect(nested_owner_info.messages != 0,                   gen_txt("c11 nested owner recovery diagnostic"));

	gen_Code nested_owner_before       = nested_owner != 0 && nested_owner->Body != 0 ? nested_owner->Body->Front : 0;
	gen_Code nested_owner_inner        = nested_owner_before != 0 ? nested_owner_before->Next : 0;
	gen_Code nested_owner_after        = nested_owner_inner  != 0 ? nested_owner_inner->Next  : 0;
	gen_Code nested_owner_inner_before = nested_owner_inner != 0 && nested_owner_inner->Body != 0 ? nested_owner_inner->Body->Front : 0;
	gen_Code nested_owner_inner_failed = nested_owner_inner_before != 0 ? nested_owner_inner_before->Next : 0;
	gen_Code nested_owner_inner_after  = nested_owner_inner_failed != 0 ? nested_owner_inner_failed->Next : 0;
	expect(nested_owner_before       != 0 && nested_owner_before->Type       == CT_Variable, gen_txt("c11 nested owner keeps preceding member typed"));
	expect(nested_owner_inner        != 0 && nested_owner_inner->Type        == CT_Struct,   gen_txt("c11 nested owner retains inner struct AST"));
	expect(nested_owner_inner_before != 0 && nested_owner_inner_before->Type == CT_Variable, gen_txt("c11 nested inner recovery keeps preceding member typed"));
	expect(nested_owner_inner_failed != 0 && nested_owner_inner_failed->Type == CT_Untyped,  gen_txt("c11 nested inner recovery retains failed member"));
	expect(nested_owner_inner_after  != 0 && nested_owner_inner_after->Type  == CT_Variable, gen_txt("c11 nested inner recovery keeps following member typed"));
	expect(nested_owner_after        != 0 && nested_owner_after->Type        == CT_Variable, gen_txt("c11 nested owner keeps following member typed"));

#if ! GEN_BUILD_DEBUG
	gen_CodeVar      wrong_array_value  = gen_def_variable(gen_def_type(name(u32)), name(invalid_array_expr));
	gen_CodeTypename invalid_array_type = gen_def_type(name(InvalidArrayOption), .gen_array_expr = (gen_Code)wrong_array_value);
	expect((gen_Code)invalid_array_type == gen_Code_Invalid, gen_txt("c11 wrong type invalid code"));

	gen_CodeBody  missing_union_body = 0;
	gen_CodeUnion missing_union      = gen_def_union(name(MissingUnionBody), missing_union_body);
	expect((gen_Code)missing_union == gen_Code_Invalid, gen_txt("c11 missing body invalid code"));
#endif

	gen_deinit(& ctx);
	if (g_failures != 0) { gen_log_fmt("c11 baseline smoke failed: %d\n", g_failures); return 1; }
	gen_log_fmt("c11 baseline smoke passed\n");
	return 0;
}
