#pragma region Gen Interface

// Initialize the library.
// This currently just initializes the CodePool.
void init();

// Currently manually free's the arenas, code for checking for leaks.
// However on Windows at least, it doesn't need to occur as the OS will clean up after the process.
void deinit();

// Clears the allocations, but doesn't return to the heap, the calls init() again.
// Ease of use.
void reset();

// Used internally to retrive or make string allocations.
// Strings are stored in a series of string arenas of fixed size (SizePer_StringArena)
StringCached get_cached_string( StrC str );

/*
	This provides a fresh Code AST.
	The gen interface use this as their method from getting a new AST object from the CodePool.
	Use this if you want to make your own API for formatting the supported Code Types.
*/
Code make_code();

// Set these before calling gen's init() procedure.
// Data

void set_allocator_data_arrays ( AllocatorInfo data_array_allocator );
void set_allocator_code_pool   ( AllocatorInfo pool_allocator );
void set_allocator_lexer       ( AllocatorInfo lex_allocator );
void set_allocator_string_arena( AllocatorInfo string_allocator );
void set_allocator_string_table( AllocatorInfo string_allocator );
void set_allocator_type_table  ( AllocatorInfo type_reg_allocator );

#pragma region Upfront

CodeAttributes def_attributes( StrC content );
CodeComment    def_comment   ( StrC content );

CodeClass def_class( StrC name
	, Code           body         = NoCode
	, CodeType       parent       = NoCode, AccessSpec access = AccessSpec::Default
	, CodeAttributes attributes   = NoCode
	, ModuleFlag     mflags       = ModuleFlag::None
	, CodeType*      interfaces   = nullptr, s32 num_interfaces = 0 );

CodeDefine def_define( StrC name, StrC content );

CodeEnum def_enum( StrC name
	, Code         body      = NoCode,      CodeType       type       = NoCode
	, EnumT        specifier = EnumRegular, CodeAttributes attributes = NoCode
	, ModuleFlag   mflags    = ModuleFlag::None );

CodeExec   def_execution  ( StrC content );
CodeExtern def_extern_link( StrC name, Code body );
CodeFriend def_friend     ( Code symbol );

CodeFn def_function( StrC name
	, CodeParam      params     = NoCode, CodeType       ret_type   = NoCode, Code body = NoCode
	, CodeSpecifiers specifiers = NoCode, CodeAttributes attributes = NoCode
	, ModuleFlag mflags     = ModuleFlag::None );

CodeInclude   def_include  ( StrC content );
CodeModule    def_module   ( StrC name,            ModuleFlag mflags = ModuleFlag::None );
CodeNS        def_namespace( StrC name, Code body, ModuleFlag mflags = ModuleFlag::None );

CodeOperator def_operator( OperatorT op, StrC nspace
	, CodeParam      params     = NoCode, CodeType       ret_type   = NoCode, Code body = NoCode
	, CodeSpecifiers specifiers = NoCode, CodeAttributes attributes = NoCode
	, ModuleFlag     mflags     = ModuleFlag::None );

CodeOpCast def_operator_cast( CodeType type, Code body = NoCode, CodeSpecifiers specs = NoCode );

CodeParam  def_param ( CodeType type, StrC name, Code value = NoCode );
CodePragma def_pragma( StrC directive );

CodePreprocessCond def_preprocess_cond( EPreprocessCond type, StrC content );

CodeSpecifiers def_specifier( SpecifierT specifier );

CodeStruct def_struct( StrC name
	, Code           body       = NoCode
	, CodeType       parent     = NoCode, AccessSpec access = AccessSpec::Default
	, CodeAttributes attributes = NoCode
	, ModuleFlag     mflags     = ModuleFlag::None
	, CodeType*      interfaces = nullptr, s32 num_interfaces = 0 );

CodeTemplate def_template( CodeParam params, Code definition, ModuleFlag mflags = ModuleFlag::None );

CodeType    def_type   ( StrC name, Code arrayexpr = NoCode, CodeSpecifiers specifiers = NoCode, CodeAttributes attributes = NoCode );
CodeTypedef def_typedef( StrC name, Code type, CodeAttributes attributes = NoCode, ModuleFlag mflags = ModuleFlag::None );

CodeUnion def_union( StrC name, Code body, CodeAttributes attributes = NoCode, ModuleFlag mflags = ModuleFlag::None );

CodeUsing def_using( StrC name, CodeType type = NoCode
	, CodeAttributes attributess = NoCode
	, ModuleFlag     mflags      = ModuleFlag::None );

CodeUsing def_using_namespace( StrC name );

CodeVar def_variable( CodeType type, StrC name, Code value = NoCode
	, CodeSpecifiers specifiers = NoCode, CodeAttributes attributes = NoCode
	, ModuleFlag     mflags     = ModuleFlag::None );

// Constructs an empty body. Use AST::validate_body() to check if the body is was has valid entries.
CodeBody def_body( CodeT type );

// There are two options for defining a struct body, either varadically provided with the args macro to auto-deduce the arg num,
/// or provide as an array of Code objects.

CodeBody       def_class_body      ( s32 num, ... );
CodeBody       def_class_body      ( s32 num, Code* codes );
CodeBody       def_enum_body       ( s32 num, ... );
CodeBody       def_enum_body       ( s32 num, Code* codes );
CodeBody       def_export_body     ( s32 num, ... );
CodeBody       def_export_body     ( s32 num, Code* codes);
CodeBody       def_extern_link_body( s32 num, ... );
CodeBody       def_extern_link_body( s32 num, Code* codes );
CodeBody       def_function_body   ( s32 num, ... );
CodeBody       def_function_body   ( s32 num, Code* codes );
CodeBody       def_global_body     ( s32 num, ... );
CodeBody       def_global_body     ( s32 num, Code* codes );
CodeBody       def_namespace_body  ( s32 num, ... );
CodeBody       def_namespace_body  ( s32 num, Code* codes );
CodeParam      def_params          ( s32 num, ... );
CodeParam      def_params          ( s32 num, CodeParam* params );
CodeSpecifiers def_specifiers      ( s32 num, ... );
CodeSpecifiers def_specifiers      ( s32 num, SpecifierT* specs );
CodeBody       def_struct_body     ( s32 num, ... );
CodeBody       def_struct_body     ( s32 num, Code* codes );
CodeBody       def_union_body      ( s32 num, ... );
CodeBody       def_union_body      ( s32 num, Code* codes );

#pragma endregion Upfront

#pragma region Parsing

CodeClass     parse_class        ( StrC class_def     );
CodeEnum      parse_enum         ( StrC enum_def      );
CodeBody      parse_export_body  ( StrC export_def    );
CodeExtern    parse_extern_link  ( StrC exten_link_def);
CodeFriend    parse_friend       ( StrC friend_def    );
CodeFn        parse_function     ( StrC fn_def        );
CodeBody      parse_global_body  ( StrC body_def      );
CodeNS        parse_namespace    ( StrC namespace_def );
CodeOperator  parse_operator     ( StrC operator_def  );
CodeOpCast    parse_operator_cast( StrC operator_def  );
CodeStruct    parse_struct       ( StrC struct_def    );
CodeTemplate  parse_template     ( StrC template_def  );
CodeType      parse_type         ( StrC type_def      );
CodeTypedef   parse_typedef      ( StrC typedef_def   );
CodeUnion     parse_union        ( StrC union_def     );
CodeUsing     parse_using        ( StrC using_def     );
CodeVar       parse_variable     ( StrC var_def       );

#pragma endregion Parsing

#pragma region Untyped text

sw   token_fmt_va( char* buf, uw buf_size, s32 num_tokens, va_list va );
//! Do not use directly. Use the token_fmt macro instead.
StrC token_fmt_impl( sw, ... );

Code untyped_str      ( StrC content);
Code untyped_fmt      ( char const* fmt, ... );
Code untyped_token_fmt( char const* fmt, s32 num_tokens, ... );

#pragma endregion Untyped text

#pragma endregion Gen Interface

