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
	expect(var->Value != 0, label);
	if (var->Value != 0) {
		expect(gen_str_contains(var->Value->Content, gen_txt("GENCPP_BASELINE")), label);
	}
}

gen_internal gen_Code_POD code_pod(gen_Code code) {
	gen_Code_POD result = { (gen_AST*)code };
	return result;
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
	expect_fragments(untyped_text, gen_txt("c11 untyped content"));
	expect_variable(gen_parse_variable(untyped_text), gen_txt("c11 untyped reparsed"));

	gen_CodeVar parsed = gen_parse_variable(code(char const* baseline_message = "GENCPP_BASELINE"; ));
	expect_variable(parsed, gen_txt("c11 parsed"));
	expect_fragments(gen_strbuilder_to_str(gen_code_to_strbuilder(parsed)), gen_txt("c11 parsed serialized"));

#if ! GEN_BUILD_DEBUG
	gen_CodeVar wrong_array_value = gen_def_variable(gen_def_type(name(u32)), name(invalid_array_expr));
	gen_CodeTypename invalid_array_type = gen_def_type(name(InvalidArrayOption), .gen_array_expr = (gen_Code)wrong_array_value);
	expect((gen_Code)invalid_array_type == gen_Code_Invalid, gen_txt("c11 wrong type invalid code"));

	gen_CodeBody missing_union_body = 0;
	gen_CodeUnion missing_union = gen_def_union(name(MissingUnionBody), missing_union_body);
	expect((gen_Code)missing_union == gen_Code_Invalid, gen_txt("c11 missing body invalid code"));
#endif

	gen_deinit(&ctx);
	if (g_failures != 0) { gen_log_fmt("c11 baseline smoke failed: %d\n", g_failures); return 1; }
	gen_log_fmt("c11 baseline smoke passed\n");
	return 0;
}
