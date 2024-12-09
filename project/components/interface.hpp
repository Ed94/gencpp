#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "ast_types.hpp"
#endif

#pragma region Gen Interface
/*
 /      \                       |      \          |  \                      /      \
|  ▓▓▓▓▓▓\ ______  _______       \▓▓▓▓▓▓_______  _| ▓▓_    ______   ______ |  ▓▓▓▓▓▓\ ______   _______  ______
| ▓▓ __\▓▓/      \|       \       | ▓▓ |       \|   ▓▓ \  /      \ /      \| ▓▓_  \▓▓|      \ /       \/      \
| ▓▓|    \  ▓▓▓▓▓▓\ ▓▓▓▓▓▓▓\      | ▓▓ | ▓▓▓▓▓▓▓\\▓▓▓▓▓▓ |  ▓▓▓▓▓▓\  ▓▓▓▓▓▓\ ▓▓ \     \▓▓▓▓▓▓\  ▓▓▓▓▓▓▓  ▓▓▓▓▓▓\
| ▓▓ \▓▓▓▓ ▓▓    ▓▓ ▓▓  | ▓▓      | ▓▓ | ▓▓  | ▓▓ | ▓▓ __| ▓▓    ▓▓ ▓▓   \▓▓ ▓▓▓▓    /      ▓▓ ▓▓     | ▓▓    ▓▓
| ▓▓__| ▓▓ ▓▓▓▓▓▓▓▓ ▓▓  | ▓▓     _| ▓▓_| ▓▓  | ▓▓ | ▓▓|  \ ▓▓▓▓▓▓▓▓ ▓▓     | ▓▓     |  ▓▓▓▓▓▓▓ ▓▓_____| ▓▓▓▓▓▓▓▓
 \▓▓    ▓▓\▓▓     \ ▓▓  | ▓▓    |   ▓▓ \ ▓▓  | ▓▓  \▓▓  ▓▓\▓▓     \ ▓▓     | ▓▓      \▓▓    ▓▓\▓▓     \\▓▓     \
  \▓▓▓▓▓▓  \▓▓▓▓▓▓▓\▓▓   \▓▓     \▓▓▓▓▓▓\▓▓   \▓▓   \▓▓▓▓  \▓▓▓▓▓▓▓\▓▓      \▓▓       \▓▓▓▓▓▓▓ \▓▓▓▓▓▓▓ \▓▓▓▓▓▓▓
*/

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
	CodeTypename   parent;
	AccessSpec     parent_access;
	CodeAttributes attributes;
	ModuleFlag     mflags;
	CodeTypename*  interfaces;
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
	CodeTypename   type;
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
	CodeTypename    ret_type;
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
	CodeTypename    ret_type;
	Code            body;
	CodeSpecifiers  specifiers;
	CodeAttributes  attributes;
	ModuleFlag      mflags;
};
CodeOperator def_operator( Operator op, StrC nspace, Opts_def_operator opts GEN_PARAM_DEFAULT );

struct Opts_def_operator_cast {
	Code           body;
	CodeSpecifiers specs;
};
CodeOpCast def_operator_cast( CodeTypename type, Opts_def_operator_cast opts GEN_PARAM_DEFAULT );

struct Opts_def_param { Code value; };
CodeParam  def_param ( CodeTypename type, StrC name, Opts_def_param opts GEN_PARAM_DEFAULT );
CodePragma def_pragma( StrC directive );

CodePreprocessCond def_preprocess_cond( EPreprocessCond type, StrC content );

CodeSpecifiers def_specifier( Specifier specifier );

CodeStruct def_struct( StrC name, Opts_def_struct opts GEN_PARAM_DEFAULT );

struct Opts_def_template { ModuleFlag mflags; };
CodeTemplate def_template( CodeParam params, Code definition, Opts_def_template opts GEN_PARAM_DEFAULT );

struct Opts_def_type {
	ETypenameTag   type_tag;
	Code           arrayexpr;
	CodeSpecifiers specifiers;
	CodeAttributes attributes;
};
CodeTypename def_type( StrC name, Opts_def_type opts GEN_PARAM_DEFAULT );

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
CodeVar def_variable( CodeTypename type, StrC name, Opts_def_variable opts GEN_PARAM_DEFAULT );

// Constructs an empty body. Use AST::validate_body() to check if the body is was has valid entries.
CodeBody def_body( CodeType type );

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
CodeSpecifiers def_specifiers      ( s32 num, Specifier* specs );
CodeBody       def_struct_body     ( s32 num, ... );
CodeBody       def_struct_body     ( s32 num, Code* codes );
CodeBody       def_union_body      ( s32 num, ... );
CodeBody       def_union_body      ( s32 num, Code* codes );

#pragma endregion Upfront

#pragma region Parsing

// TODO(Ed) : Implmeent the new parser API design.

#if 0
GEN_NS_PARSER_BEGIN
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
GEN_NS_PARSER_END

struct ParseInfo
{
	Arena FileMem;
	Arena TokMem;
	Arena CodeMem;

	FileContents FileContent;
	Array<Token> Tokens;
	Array<Error> Errors;
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
CodeTypename    parse_type         ( StrC type_def        );
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

#pragma region Macros

#ifndef token_fmt
#	define gen_main main

#	define __ NullCode

	//	Convienence for defining any name used with the gen api.
	//  Lets you provide the length and string literal to the functions without the need for the DSL.
#	define name( Id_ )   { sizeof(stringize( Id_ )) - 1, stringize(Id_) }

	//  Same as name just used to indicate intention of literal for code instead of names.
#	define code( ... ) { sizeof(stringize(__VA_ARGS__)) - 1, stringize( __VA_ARGS__ ) }

	// Provides the number of arguments while passing args inplace.
#	define args( ... ) num_args( __VA_ARGS__ ), __VA_ARGS__

	// Just wrappers over common untyped code definition constructions.
#	define code_str( ... ) GEN_NS untyped_str( code( __VA_ARGS__ ) )
#	define code_fmt( ... ) GEN_NS untyped_str( token_fmt( __VA_ARGS__ ) )

#	define parse_fmt( type, ... ) GEN_NS parse_##type( token_fmt( __VA_ARGS__ ) )

	/*
	Takes a format string (char const*) and a list of tokens (StrC) and returns a StrC of the formatted string.
	Tokens are provided in '<'identifier'>' format where '<' '>' are just angle brakcets (you can change it in token_fmt_va)
	---------------------------------------------------------
		Example - A string with:
			typedef <type> <name> <name>;
		Will have a token_fmt arguments populated with:
			"type", strc_for_type,
			"name", strc_for_name,
		and:
			stringize( typedef <type> <name> <name>; ) )
	-----------------------------------------------------------
	So the full call for this example would be:
		token_fmt(
			"type", strc_for_type
		,	"name", strc_for_name
		,	stringize(
			typedef <type> <name> <name>
		));
	!----------------------------------------------------------
	! Note: token_fmt_va is whitespace sensitive for the tokens.
	! This can be alleviated by skipping whitespace between brackets but it was choosen to not have that implementation by default.
	*/
#	define token_fmt( ... ) GEN_NS token_fmt_impl( (num_args( __VA_ARGS__ ) + 1) / 2, __VA_ARGS__ )
#endif

#pragma endregion Macros

#pragma endregion Gen Interface
