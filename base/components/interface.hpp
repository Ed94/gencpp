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

#if 0
enum LogLevel : u32
{
	Info,
	Warning,
	Panic,
};

struct LogEntry
{
	Str   msg;
	u32   line_num;
	void* data;
};

typedef void LoggerCallback(LogEntry entry);
#endif

// Note(Ed): This is subject to heavily change 
// with upcoming changes to the library's fallback (default) allocations strategy;
// and major changes to lexer/parser context usage.
struct Context
{
// User Configuration

// Persistent Data Allocation
	AllocatorInfo Allocator_DyanmicContainers; // By default will use a genral slab allocator (TODO(Ed): Currently does not)
	AllocatorInfo Allocator_Pool;              // By default will use the growing vmem reserve (TODO(Ed): Currently does not)
	AllocatorInfo Allocator_StrCache;          // By default will use a dedicated slab allocator (TODO(Ed): Currently does not)

// Temporary Allocation
	AllocatorInfo Allocator_Temp;

	// LoggerCallaback* log_callback; // TODO(Ed): Impl user logger callback as an option.

// 
	u32 Max_CommentLineLength; // Used by def_comment
	u32 Max_StrCacheLength;    // Any cached string longer than this is always allocated again.

	u32 InitSize_BuilderBuffer;
	u32 InitSize_CodePoolsArray;
	u32 InitSize_StringArenasArray;

	u32 CodePool_NumBlocks;

	// TODO(Ed): Review these... (No longer needed if using the proper allocation strategy)
	u32 InitSize_LexArena;
	u32 SizePer_StringArena;

// Parser

	// Used by the lexer to persistently treat all these identifiers as preprocessor defines.
	// Populate with strings via gen::cache_str.
	// Functional defines must have format: id( ;at minimum to indicate that the define is only valid with arguments.
	Array(StrCached) PreprocessorDefines;

// Backend
	// The fallback allocator is utilized if any fo the three above allocators is not specified by the user.
	u32 InitSize_Fallback_Allocator_Bucket_Size;
	Array(Arena) Fallback_AllocatorBuckets;

	// Array(Token) LexerTokens;

	Array(Pool)  CodePools;
	Array(Arena) StringArenas;

	Arena LexArena;

	StringTable StrCache;
};

// Initialize the library.
void init(Context* ctx);

// Currently manually free's the arenas, code for checking for leaks.
// However on Windows at least, it doesn't need to occur as the OS will clean up after the process.
void deinit(Context* ctx);

// Clears the allocations, but doesn't return to the heap, the calls init() again.
// Ease of use.
void reset(Context* ctx);

void set_context(Context* ctx);

// Alternative way to add a preprocess define entry for the lexer & parser to utilize 
// if the user doesn't want to use def_define
void set_preprocess_define( Str id, b32 is_functional );

// Used internally to retrive or make string allocations.
// Strings are stored in a series of string arenas of fixed size (SizePer_StringArena)
StrCached cache_str( Str str );

/*
	This provides a fresh Code AST.
	The gen interface use this as their method from getting a new AST object from the CodePool.
	Use this if you want to make your own API for formatting the supported Code Types.
*/
Code make_code();

// Set these before calling gen's init() procedure.

#pragma region Upfront

CodeAttributes def_attributes( Str content );
CodeComment    def_comment   ( Str content );

struct Opts_def_struct {
	CodeBody       body;
	CodeTypename   parent;
	AccessSpec     parent_access;
	CodeAttributes attributes;
	CodeTypename*  interfaces;
	s32            num_interfaces;
	ModuleFlag     mflags;
};
CodeClass def_class( Str name, Opts_def_struct opts GEN_PARAM_DEFAULT );

struct Opts_def_constructor {
	CodeParams params;
	Code      initializer_list;
	Code      body;
};
CodeConstructor def_constructor( Opts_def_constructor opts GEN_PARAM_DEFAULT );

struct Opts_def_define {
	b32 dont_append_preprocess_defines;
};
CodeDefine def_define( Str name, Str content, Opts_def_define opts GEN_PARAM_DEFAULT );

struct Opts_def_destructor {
	Code           body;
	CodeSpecifiers specifiers;
};
CodeDestructor def_destructor( Opts_def_destructor opts GEN_PARAM_DEFAULT );

struct Opts_def_enum {
	CodeBody       body;
	CodeTypename   type;
	EnumT          specifier;
	CodeAttributes attributes;
	ModuleFlag     mflags;
	Code           type_macro;
};
CodeEnum def_enum( Str name, Opts_def_enum opts GEN_PARAM_DEFAULT );

CodeExec   def_execution  ( Str content );
CodeExtern def_extern_link( Str name, CodeBody body );
CodeFriend def_friend     ( Code symbol );

struct Opts_def_function {
	CodeParams      params;
	CodeTypename    ret_type;
	CodeBody        body;
	CodeSpecifiers  specs;
	CodeAttributes  attrs;
	ModuleFlag      mflags;
};
CodeFn def_function( Str name, Opts_def_function opts GEN_PARAM_DEFAULT );

struct Opts_def_include   { b32        foreign; };
struct Opts_def_module    { ModuleFlag mflags;  };
struct Opts_def_namespace { ModuleFlag mflags;  };
CodeInclude def_include  ( Str content,             Opts_def_include   opts GEN_PARAM_DEFAULT );
CodeModule  def_module   ( Str name,                Opts_def_module    opts GEN_PARAM_DEFAULT );
CodeNS      def_namespace( Str name, CodeBody body, Opts_def_namespace opts GEN_PARAM_DEFAULT );

struct Opts_def_operator {
	CodeParams      params;
	CodeTypename    ret_type;
	CodeBody        body;
	CodeSpecifiers  specifiers;
	CodeAttributes  attributes;
	ModuleFlag      mflags;
};
CodeOperator def_operator( Operator op, Str nspace, Opts_def_operator opts GEN_PARAM_DEFAULT );

struct Opts_def_operator_cast {
	CodeBody       body;
	CodeSpecifiers specs;
};
CodeOpCast def_operator_cast( CodeTypename type, Opts_def_operator_cast opts GEN_PARAM_DEFAULT );

struct Opts_def_param { Code value; };
CodeParams  def_param ( CodeTypename type, Str name, Opts_def_param opts GEN_PARAM_DEFAULT );
CodePragma def_pragma( Str directive );

CodePreprocessCond def_preprocess_cond( EPreprocessCond type, Str content );

CodeSpecifiers def_specifier( Specifier specifier );

CodeStruct def_struct( Str name, Opts_def_struct opts GEN_PARAM_DEFAULT );

struct Opts_def_template { ModuleFlag mflags; };
CodeTemplate def_template( CodeParams params, Code definition, Opts_def_template opts GEN_PARAM_DEFAULT );

struct Opts_def_type {
	ETypenameTag   type_tag;
	Code           arrayexpr;
	CodeSpecifiers specifiers;
	CodeAttributes attributes;
};
CodeTypename def_type( Str name, Opts_def_type opts GEN_PARAM_DEFAULT );

struct Opts_def_typedef {
	CodeAttributes attributes;
	ModuleFlag     mflags;
};
CodeTypedef def_typedef( Str name, Code type, Opts_def_typedef opts GEN_PARAM_DEFAULT );

struct Opts_def_union {
	CodeAttributes attributes;
	ModuleFlag     mflags;
};
CodeUnion def_union( Str name, CodeBody body, Opts_def_union opts GEN_PARAM_DEFAULT );

struct Opts_def_using {
	CodeAttributes attributes;
	ModuleFlag     mflags;
};
CodeUsing def_using( Str name, CodeTypename type, Opts_def_using opts GEN_PARAM_DEFAULT );

CodeUsing def_using_namespace( Str name );

struct Opts_def_variable
{
	Code           value;
	CodeSpecifiers specifiers;
	CodeAttributes attributes;
	ModuleFlag     mflags;
};
CodeVar def_variable( CodeTypename type, Str name, Opts_def_variable opts GEN_PARAM_DEFAULT );

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
CodeParams     def_params          ( s32 num, ... );
CodeParams     def_params          ( s32 num, CodeParams* params );
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
	Str  FailedProc; // The name of the procedure that failed
};
// Stack nodes are allocated the error's allocator

struct Error
{
	StrBuilder     message;
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

CodeBody parse_file( Str path );
#endif

CodeClass       parse_class        ( Str class_def       );
CodeConstructor parse_constructor  ( Str constructor_def );
CodeDestructor  parse_destructor   ( Str destructor_def  );
CodeEnum        parse_enum         ( Str enum_def        );
CodeBody        parse_export_body  ( Str export_def      );
CodeExtern      parse_extern_link  ( Str exten_link_def  );
CodeFriend      parse_friend       ( Str friend_def      );
CodeFn          parse_function     ( Str fn_def          );
CodeBody        parse_global_body  ( Str body_def        );
CodeNS          parse_namespace    ( Str namespace_def   );
CodeOperator    parse_operator     ( Str operator_def    );
CodeOpCast      parse_operator_cast( Str operator_def    );
CodeStruct      parse_struct       ( Str struct_def      );
CodeTemplate    parse_template     ( Str template_def    );
CodeTypename    parse_type         ( Str type_def        );
CodeTypedef     parse_typedef      ( Str typedef_def     );
CodeUnion       parse_union        ( Str union_def       );
CodeUsing       parse_using        ( Str using_def       );
CodeVar         parse_variable     ( Str var_def         );

#pragma endregion Parsing

#pragma region Untyped text

ssize   token_fmt_va( char* buf, usize buf_size, s32 num_tokens, va_list va );
//! Do not use directly. Use the token_fmt macro instead.
Str token_fmt_impl( ssize, ... );

Code untyped_str      ( Str content);
Code untyped_fmt      ( char const* fmt, ... );
Code untyped_token_fmt( s32 num_tokens, char const* fmt, ... );

#pragma endregion Untyped text

#pragma region Macros

#ifndef gen_main
#define gen_main main
#endif

#ifndef name
//	Convienence for defining any name used with the gen api.
//  Lets you provide the length and string literal to the functions without the need for the DSL.
#define name( Id_ )   { stringize(Id_), sizeof(stringize( Id_ )) - 1 }
#endif

#ifndef code
//  Same as name just used to indicate intention of literal for code instead of names.
#define code( ... ) { stringize( __VA_ARGS__ ), sizeof(stringize(__VA_ARGS__)) - 1 }
#endif

#ifndef args
// Provides the number of arguments while passing args inplace.
#define args( ... ) num_args( __VA_ARGS__ ), __VA_ARGS__
#endif

#ifndef code_str
// Just wrappers over common untyped code definition constructions.
#define code_str( ... ) GEN_NS untyped_str( code( __VA_ARGS__ ) )
#endif

#ifndef code_fmt
#define code_fmt( ... ) GEN_NS untyped_str( token_fmt( __VA_ARGS__ ) )
#endif

#ifndef parse_fmt
#define parse_fmt( type, ... ) GEN_NS parse_##type( token_fmt( __VA_ARGS__ ) )
#endif

#ifndef token_fmt
/*
Takes a format string (char const*) and a list of tokens (Str) and returns a Str of the formatted string.
Tokens are provided in '<'identifier'>' format where '<' '>' are just angle brackets (you can change it in token_fmt_va)
---------------------------------------------------------
	Example - A string with:
		typedef <type> <name> <name>;
	Will have a token_fmt arguments populated with:
		"type", str_for_type,
		"name", str_for_name,
	and:
		stringize( typedef <type> <name> <name>; )
-----------------------------------------------------------
So the full call for this example would be:
	token_fmt(
		"type", str_for_type
	,	"name", str_for_name
	,	stringize(
		typedef <type> <name> <name>
	));
!----------------------------------------------------------
! Note: token_fmt_va is whitespace sensitive for the tokens.
! This can be alleviated by skipping whitespace between brackets but it was choosen to not have that implementation by default.
*/
#define token_fmt( ... ) GEN_NS token_fmt_impl( (num_args( __VA_ARGS__ ) + 1) / 2, __VA_ARGS__ )
#endif

#pragma endregion Macros

#pragma endregion Gen Interface
