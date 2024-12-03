#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "ast_types.hpp"
#endif

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

void set_allocator_data_arrays ( AllocatorInfo data_array_allocator );
void set_allocator_code_pool   ( AllocatorInfo pool_allocator );
void set_allocator_lexer       ( AllocatorInfo lex_allocator );
void set_allocator_string_arena( AllocatorInfo string_allocator );
void set_allocator_string_table( AllocatorInfo string_allocator );
void set_allocator_type_table  ( AllocatorInfo type_reg_allocator );

#pragma region Upfront

CodeAttributes def_attributes( StrC content );
CodeComment    def_comment   ( StrC content );

struct Opts_def_struct {
	Code           body;
	CodeType       parent;
	AccessSpec     parent_access;
	CodeAttributes attributes;
	ModuleFlag     mflags;
	CodeType*      interfaces;
	s32            num_interfaces;
};
CodeClass def_class( StrC name, Opts_def_struct otps GEN_PARAM_DEFAULT );

struct Opts_def_constructor {
	CodeParam params;
	Code      initializer_list;
	Code      body;
};
CodeConstructor def_constructor( Opts_def_constructor opts GEN_PARAM_DEFAULT );

CodeDefine def_define( StrC name, StrC content );

struct Opts_def_destructor {
	Code           body;
	CodeSpecifiers specifiers;
};
CodeDestructor def_destructor( Opts_def_destructor opts GEN_PARAM_DEFAULT );

struct Opts_def_enum {
	Code           body;
	CodeType       type;
	EnumT          specifier;
	CodeAttributes attributes;
	ModuleFlag     mflags;
};
CodeEnum def_enum( StrC name, Opts_def_enum opts GEN_PARAM_DEFAULT );

CodeExec   def_execution  ( StrC content );
CodeExtern def_extern_link( StrC name, Code body );
CodeFriend def_friend     ( Code symbol );

struct Opts_def_function {
	CodeParam       params;
	CodeType        ret_type;
	Code            body;
	CodeSpecifiers  specs;
	CodeAttributes  attrs;
	ModuleFlag      mflags;
};
CodeFn def_function( StrC name, Opts_def_function opts GEN_PARAM_DEFAULT );

struct Opts_def_include   { b32        foreign; };
struct Opts_def_module    { ModuleFlag mflags;  };
struct Opts_def_namespace { ModuleFlag mflags;  };
CodeInclude def_include  ( StrC content,         Opts_def_include   opts GEN_PARAM_DEFAULT );
CodeModule  def_module   ( StrC name,            Opts_def_module    opts GEN_PARAM_DEFAULT );
CodeNS      def_namespace( StrC name, Code body, Opts_def_namespace opts GEN_PARAM_DEFAULT );

struct Opts_def_operator {
	CodeParam       params;
	CodeType        ret_type;
	Code            body;
	CodeSpecifiers  specifiers;
	CodeAttributes  attributes;
	ModuleFlag      mflags;
};
CodeOperator def_operator( OperatorT op, StrC nspace, Opts_def_operator opts GEN_PARAM_DEFAULT );

struct Opts_def_operator_cast {
	Code           body;
	CodeSpecifiers specs;
};
CodeOpCast def_operator_cast( CodeType type, Opts_def_operator_cast opts GEN_PARAM_DEFAULT );

struct Opts_def_param { Code value; };
CodeParam  def_param ( CodeType type, StrC name, Opts_def_param opts GEN_PARAM_DEFAULT );
CodePragma def_pragma( StrC directive );

CodePreprocessCond def_preprocess_cond( EPreprocessCond type, StrC content );

CodeSpecifiers def_specifier( SpecifierT specifier );

CodeStruct def_struct( StrC name, Opts_def_struct opts GEN_PARAM_DEFAULT );

struct Opts_def_template { ModuleFlag mflags; };
CodeTemplate def_template( CodeParam params, Code definition, Opts_def_template opts GEN_PARAM_DEFAULT );

struct Opts_def_type {
	Code           arrayexpr;
	CodeSpecifiers specifiers;
	CodeAttributes attributes;
};
CodeType def_type( StrC name, Opts_def_type opts GEN_PARAM_DEFAULT );

struct Opts_def_typedef {
	CodeAttributes attributes;
	ModuleFlag     mflags;
};
CodeTypedef def_typedef( StrC name, Code type, Opts_def_typedef opts GEN_PARAM_DEFAULT );

struct Opts_def_union {
	CodeAttributes attributes;
	ModuleFlag     mflags;
};
CodeUnion def_union( StrC name, Code body, Opts_def_union opts GEN_PARAM_DEFAULT );

struct Opts_def_using {
	CodeAttributes attributes;
	ModuleFlag     mflags;
};
CodeUsing def_using( StrC name, Code type, Opts_def_using opts GEN_PARAM_DEFAULT );

CodeUsing def_using_namespace( StrC name );

struct Opts_def_variable
{
	Code           value;
	CodeSpecifiers specifiers;
	CodeAttributes attributes;
	ModuleFlag     mflags;
};
CodeVar def_variable( CodeType type, StrC name, Opts_def_variable opts GEN_PARAM_DEFAULT ); 

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

// TODO(Ed) : Implmeent the new parser API design.

#if 0
namespace parser {
	struct StackNode
	{
		StackNode* Prev;

		Token Start;
		Token Name;       // The name of the AST node (if parsed)
		StrC  FailedProc; // The name of the procedure that failed
	};
	// Stack nodes are allocated the error's allocator

	struct Error
	{
		String     message;
		StackNode* context_stack;
	};
}

struct ParseInfo
{
	Arena FileMem;
	Arena TokMem;
	Arena CodeMem;

	FileContents         FileContent;
	Array<parser::Token> Tokens;
	Array<parser::Error> Errors;
	// Errors are allocated to a dedicated general arena.
};

CodeBody parse_file( StrC path );
#endif

CodeClass       parse_class        ( StrC class_def       );
CodeConstructor parse_constructor  ( StrC constructor_def );
CodeDestructor  parse_destructor   ( StrC destructor_def  );
CodeEnum        parse_enum         ( StrC enum_def        );
CodeBody        parse_export_body  ( StrC export_def      );
CodeExtern      parse_extern_link  ( StrC exten_link_def  );
CodeFriend      parse_friend       ( StrC friend_def      );
CodeFn          parse_function     ( StrC fn_def          );
CodeBody        parse_global_body  ( StrC body_def        );
CodeNS          parse_namespace    ( StrC namespace_def   );
CodeOperator    parse_operator     ( StrC operator_def    );
CodeOpCast      parse_operator_cast( StrC operator_def    );
CodeStruct      parse_struct       ( StrC struct_def      );
CodeTemplate    parse_template     ( StrC template_def    );
CodeType        parse_type         ( StrC type_def        );
CodeTypedef     parse_typedef      ( StrC typedef_def     );
CodeUnion       parse_union        ( StrC union_def       );
CodeUsing       parse_using        ( StrC using_def       );
CodeVar         parse_variable     ( StrC var_def         );

#pragma endregion Parsing

#pragma region Untyped text

ssize   token_fmt_va( char* buf, usize buf_size, s32 num_tokens, va_list va );
//! Do not use directly. Use the token_fmt macro instead.
StrC token_fmt_impl( ssize, ... );

Code untyped_str      ( StrC content);
Code untyped_fmt      ( char const* fmt, ... );
Code untyped_token_fmt( char const* fmt, s32 num_tokens, ... );

#pragma endregion Untyped text

#pragma endregion Gen Interface
