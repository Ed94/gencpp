#ifdef INTELLISENSE_DIRECTIVES
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

enum LogLevel : u32
{
	LL_Null,
	LL_Note,
	LL_Warning,
	LL_Error,
	LL_Fatal,
	LL_UnderlyingType = GEN_U32_MAX,
};

Str loglevel_to_str(LogLevel level) 
{
	local_persist
	Str lookup[] = {
		txt("Info"),
		txt("Warning"),
		txt("Panic"),
	};
	return lookup[level];
}

struct LogEntry
{
	Str      msg;
	LogLevel level;
};

typedef void LoggerProc(LogEntry entry);

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

// Initalization config
	u32 Max_CommentLineLength; // Used by def_comment
	u32 Max_StrCacheLength;    // Any cached string longer than this is always allocated again.

	u32 InitSize_BuilderBuffer;
	u32 InitSize_CodePoolsArray;
	u32 InitSize_StringArenasArray;

	u32 CodePool_NumBlocks;

	// TODO(Ed): Review these... (No longer needed if using the proper allocation strategy)
	u32 InitSize_LexerTokens;
	u32 SizePer_StringArena;

	u32 InitSize_StrCacheTable;
	u32 InitSize_MacrosTable;

// TODO(Ed): Symbol Table
	// Keep track of all resolved symbols (naemspaced identifiers)

// Logging

	LoggerProc* Logger;

// Parser

	// Used by the lexer to persistently treat all these identifiers as preprocessor defines.
	// Populate with strings via gen::cache_str.
	// Functional defines must have format: id( ;at minimum to indicate that the define is only valid with arguments.
	MacroTable Macros;

// Backend

	// The fallback allocator is utilized if any fo the three above allocators is not specified by the user.
	u32 InitSize_Fallback_Allocator_Bucket_Size;
	Array(Arena) Fallback_AllocatorBuckets;

	StringTable token_fmt_map;

	// Array(Token) LexerTokens;

	Array(Pool)  CodePools;
	Array(Arena) StringArenas;

	StringTable StrCache;

	// TODO(Ed): This needs to be just handled by a parser context
	Array(Token) Lexer_Tokens;

	// TODO(Ed): Active parse context vs a parse result need to be separated conceptually
	ParseContext parser;

	// TODO(Ed): Formatting - This will eventually be in a separate struct when in the process of serialization of the builder.
	s32 temp_serialize_indent;
};

// TODO(Ed): Eventually this library should opt out of an implicit context for baseline implementation
// This would automatically make it viable for multi-threaded purposes among other things
// An implicit context interface will be provided instead as wrapper procedures as convience.
GEN_API extern Context* _ctx;

// By default this library will either crash or exit if an error is detected while generating codes.
// Even if set to not use GEN_FATAL, GEN_FATAL will still be used for memory failures as the library is unusable when they occur.
#ifdef GEN_DONT_USE_FATAL
	#define log_failure log_fmt
#else
	#define log_failure GEN_FATAL
#endif

// TODO(Ed): Swap all usage of this with logger_fmt (then rename logger_fmt to log_fmt)
inline
ssize log_fmt(char const* fmt, ...)
{
	ssize res;
	va_list va;

	va_start(va, fmt);
	res = c_str_fmt_out_va(fmt, va);
	va_end(va);

	return res;
}

inline
void logger_fmt(Context* ctx, LogLevel level, char const* fmt, ...)
{
	local_persist thread_local
	PrintF_Buffer buf = struct_init(PrintF_Buffer, {0});

	va_list va;
	va_start(va, fmt);
	ssize res = c_str_fmt_va(buf, GEN_PRINTF_MAXLEN, fmt, va) -1;
	va_end(va);

	StrBuilder msg = strbuilder_make_length(ctx->Allocator_Temp, buf, res);

	LogEntry entry = { strbuilder_to_str(msg), level };
	ctx->Logger(entry);
}

// Initialize the library. There first ctx initialized must exist for lifetime of other contextes that come after as its the one that
GEN_API void init(Context* ctx);

// Currently manually free's the arenas, code for checking for leaks.
// However on Windows at least, it doesn't need to occur as the OS will clean up after the process.
GEN_API void deinit(Context* ctx);

// Retrieves the active context (not usually needed, but here in case...)
GEN_API Context* get_context();

// Clears the allocations, but doesn't free the memory, then calls init() again.
// Ease of use.
GEN_API void reset(Context* ctx);

GEN_API void set_context(Context* ctx);

// Mostly intended for the parser
GEN_API Macro* lookup_macro( Str Name );

// Alternative way to add a preprocess define entry for the lexer & parser to utilize 
// if the user doesn't want to use def_define
// Macros are tracked by name so if the name already exists the entry will be overwritten.
GEN_API void register_macro( Macro macro );

// Ease of use batch registration
GEN_API void register_macros( s32 num, ... );
GEN_API void register_macros_arr( s32 num, Macro* macros );

#if GEN_COMPILER_CPP
forceinline void register_macros( s32 num, Macro* macros ) { return register_macros_arr(num, macros); }
#endif

// Used internally to retrive or make string allocations.
// Strings are stored in a series of string arenas of fixed size (SizePer_StringArena)
GEN_API StrCached cache_str( Str str );

/*
	This provides a fresh Code AST.
	The gen interface use this as their method from getting a new AST object from the CodePool.
	Use this if you want to make your own API for formatting the supported Code Types.
*/
GEN_API Code make_code();

// Set these before calling gen's init() procedure.

#pragma region Upfront

GEN_API CodeAttributes def_attributes( Str content );
GEN_API CodeComment    def_comment   ( Str content );

struct Opts_def_struct {
	CodeBody       body;
	CodeTypename   parent;
	AccessSpec     parent_access;
	CodeAttributes attributes;
	CodeTypename*  interfaces;
	s32            num_interfaces;
	CodeSpecifiers specifiers; // Only used for final specifier for now.
	ModuleFlag     mflags;
};
GEN_API CodeClass def_class( Str name, Opts_def_struct opts GEN_PARAM_DEFAULT );

struct Opts_def_constructor {
	CodeParams params;
	Code      initializer_list;
	Code      body;
};
GEN_API CodeConstructor def_constructor( Opts_def_constructor opts GEN_PARAM_DEFAULT );

struct Opts_def_define {
	CodeDefineParams params;
	Str              content;
	MacroFlags       flags;
	b32              dont_register_to_preprocess_macros;
};
GEN_API CodeDefine def_define( Str name, MacroType type, Opts_def_define opts GEN_PARAM_DEFAULT );

struct Opts_def_destructor {
	Code           body;
	CodeSpecifiers specifiers;
};
GEN_API CodeDestructor def_destructor( Opts_def_destructor opts GEN_PARAM_DEFAULT );

struct Opts_def_enum {
	CodeBody       body;
	CodeTypename   type;
	EnumT          specifier;
	CodeAttributes attributes;
	ModuleFlag     mflags;
	Code           type_macro;
};
GEN_API CodeEnum def_enum( Str name, Opts_def_enum opts GEN_PARAM_DEFAULT );

GEN_API CodeExec   def_execution  ( Str content );
GEN_API CodeExtern def_extern_link( Str name, CodeBody body );
GEN_API CodeFriend def_friend     ( Code code );

struct Opts_def_function {
	CodeParams      params;
	CodeTypename    ret_type;
	CodeBody        body;
	CodeSpecifiers  specs;
	CodeAttributes  attrs;
	ModuleFlag      mflags;
};
GEN_API CodeFn def_function( Str name, Opts_def_function opts GEN_PARAM_DEFAULT );

struct Opts_def_include   { b32        foreign; };
struct Opts_def_module    { ModuleFlag mflags;  };
struct Opts_def_namespace { ModuleFlag mflags;  };
GEN_API CodeInclude def_include  ( Str content,             Opts_def_include   opts GEN_PARAM_DEFAULT );
GEN_API CodeModule  def_module   ( Str name,                Opts_def_module    opts GEN_PARAM_DEFAULT );
GEN_API CodeNS      def_namespace( Str name, CodeBody body, Opts_def_namespace opts GEN_PARAM_DEFAULT );

struct Opts_def_operator {
	CodeParams      params;
	CodeTypename    ret_type;
	CodeBody        body;
	CodeSpecifiers  specifiers;
	CodeAttributes  attributes;
	ModuleFlag      mflags;
};
GEN_API CodeOperator def_operator( Operator op, Str nspace, Opts_def_operator opts GEN_PARAM_DEFAULT );

struct Opts_def_operator_cast {
	CodeBody       body;
	CodeSpecifiers specs;
};
GEN_API CodeOpCast def_operator_cast( CodeTypename type, Opts_def_operator_cast opts GEN_PARAM_DEFAULT );

struct Opts_def_param { Code value; };
GEN_API CodeParams def_param ( CodeTypename type, Str name, Opts_def_param opts GEN_PARAM_DEFAULT );
GEN_API CodePragma def_pragma( Str directive );

GEN_API CodePreprocessCond def_preprocess_cond( EPreprocessCond type, Str content );

GEN_API CodeSpecifiers def_specifier( Specifier specifier );

GEN_API CodeStruct def_struct( Str name, Opts_def_struct opts GEN_PARAM_DEFAULT );

struct Opts_def_template { ModuleFlag mflags; };
GEN_API CodeTemplate def_template( CodeParams params, Code definition, Opts_def_template opts GEN_PARAM_DEFAULT );

struct Opts_def_type {
	ETypenameTag   type_tag;
	Code           array_expr;
	CodeSpecifiers specifiers;
	CodeAttributes attributes;
};
GEN_API CodeTypename def_type( Str name, Opts_def_type opts GEN_PARAM_DEFAULT );

struct Opts_def_typedef {
	CodeAttributes attributes;
	ModuleFlag     mflags;
};
GEN_API CodeTypedef def_typedef( Str name, Code type, Opts_def_typedef opts GEN_PARAM_DEFAULT );

struct Opts_def_union {
	CodeAttributes attributes;
	ModuleFlag     mflags;
};
GEN_API CodeUnion def_union( Str name, CodeBody body, Opts_def_union opts GEN_PARAM_DEFAULT );

struct Opts_def_using {
	CodeAttributes attributes;
	ModuleFlag     mflags;
};
GEN_API CodeUsing def_using( Str name, CodeTypename type, Opts_def_using opts GEN_PARAM_DEFAULT );

GEN_API CodeUsing def_using_namespace( Str name );

struct Opts_def_variable
{
	Code           value;
	CodeSpecifiers specifiers;
	CodeAttributes attributes;
	ModuleFlag     mflags;
};
GEN_API CodeVar def_variable( CodeTypename type, Str name, Opts_def_variable opts GEN_PARAM_DEFAULT );

// Constructs an empty body. Use AST::validate_body() to check if the body is was has valid entries.
CodeBody def_body( CodeType type );

// There are two options for defining a struct body, either varadically provided with the args macro to auto-deduce the arg num,
/// or provide as an array of Code objects.

GEN_API CodeBody         def_class_body           ( s32 num, ... );
GEN_API CodeBody         def_class_body_arr       ( s32 num, Code* codes );
GEN_API CodeDefineParams def_define_params        ( s32 num, ... );
GEN_API CodeDefineParams def_define_params_arr    ( s32 num, CodeDefineParams* codes );
GEN_API CodeBody         def_enum_body            ( s32 num, ... );
GEN_API CodeBody         def_enum_body_arr        ( s32 num, Code* codes );
GEN_API CodeBody         def_export_body          ( s32 num, ... );
GEN_API CodeBody         def_export_body_arr      ( s32 num, Code* codes);
GEN_API CodeBody         def_extern_link_body     ( s32 num, ... );
GEN_API CodeBody         def_extern_link_body_arr ( s32 num, Code* codes );
GEN_API CodeBody         def_function_body        ( s32 num, ... );
GEN_API CodeBody         def_function_body_arr    ( s32 num, Code* codes );
GEN_API CodeBody         def_global_body          ( s32 num, ... );
GEN_API CodeBody         def_global_body_arr      ( s32 num, Code* codes );
GEN_API CodeBody         def_namespace_body       ( s32 num, ... );
GEN_API CodeBody         def_namespace_body_arr   ( s32 num, Code* codes );
GEN_API CodeParams       def_params               ( s32 num, ... );
GEN_API CodeParams       def_params_arr           ( s32 num, CodeParams* params );
GEN_API CodeSpecifiers   def_specifiers           ( s32 num, ... );
GEN_API CodeSpecifiers   def_specifiers_arr       ( s32 num, Specifier* specs );
GEN_API CodeBody         def_struct_body          ( s32 num, ... );
GEN_API CodeBody         def_struct_body_arr      ( s32 num, Code* codes );
GEN_API CodeBody         def_union_body           ( s32 num, ... );
GEN_API CodeBody         def_union_body_arr       ( s32 num, Code* codes );

#if GEN_COMPILER_CPP
forceinline CodeBody         def_class_body      ( s32 num, Code* codes )             { return def_class_body_arr(num, codes); }
forceinline CodeDefineParams def_define_params   ( s32 num, CodeDefineParams* codes ) { return def_define_params_arr(num, codes); }
forceinline CodeBody         def_enum_body       ( s32 num, Code* codes )             { return def_enum_body_arr(num, codes); }
forceinline CodeBody         def_export_body     ( s32 num, Code* codes)              { return def_export_body_arr(num, codes); }
forceinline CodeBody         def_extern_link_body( s32 num, Code* codes )             { return def_extern_link_body_arr(num, codes); }
forceinline CodeBody         def_function_body   ( s32 num, Code* codes )             { return def_function_body_arr(num, codes); }
forceinline CodeBody         def_global_body     ( s32 num, Code* codes )             { return def_global_body_arr(num, codes); }
forceinline CodeBody         def_namespace_body  ( s32 num, Code* codes )             { return def_namespace_body_arr(num, codes); }
forceinline CodeParams       def_params          ( s32 num, CodeParams* params )      { return def_params_arr(num, params); }
forceinline CodeSpecifiers   def_specifiers      ( s32 num, Specifier* specs )        { return def_specifiers_arr(num, specs); }
forceinline CodeBody         def_struct_body     ( s32 num, Code* codes )             { return def_struct_body_arr(num, codes); }
forceinline CodeBody         def_union_body      ( s32 num, Code* codes )             { return def_union_body_arr(num, codes); }
#endif

#pragma endregion Upfront

#pragma region Parsing

struct ParseStackNode
{
	ParseStackNode* Prev;

	TokenSlice tokens;
	Token*     Start;
	Str        Name;     // The name of the AST node (if parsed)
	Str        ProcName; // The name of the procedure
	Code       code;     // Relevant AST node
	// TODO(Ed): When an error occurs, the parse stack is not released and instead the scope is left dangling.
};

struct ParseMessage
{
	ParseMessage*   Next;
	ParseStackNode* Scope;
	Str             Log;
	LogLevel        Level;
};

struct ParseInfo
{
	ParseMessage* messages;
	LexedInfo     lexed;
	Code          result;
};

struct Opts_parse
{
	AllocatorInfo backing_msgs;
	AllocatorInfo backing_tokens;
	AllocatorInfo backing_ast;
};

ParseInfo wip_parse_str( LexedInfo lexed, Opts_parse opts GEN_PARAM_DEFAULT );

GEN_API CodeClass       parse_class        ( Str class_def       );
GEN_API CodeConstructor parse_constructor  ( Str constructor_def );
GEN_API CodeDefine      parse_define       ( Str define_def      );
GEN_API CodeDestructor  parse_destructor   ( Str destructor_def  );
GEN_API CodeEnum        parse_enum         ( Str enum_def        );
GEN_API CodeBody        parse_export_body  ( Str export_def      );
GEN_API CodeExtern      parse_extern_link  ( Str exten_link_def  );
GEN_API CodeFriend      parse_friend       ( Str friend_def      );
GEN_API CodeFn          parse_function     ( Str fn_def          );
GEN_API CodeBody        parse_global_body  ( Str body_def        );
GEN_API CodeNS          parse_namespace    ( Str namespace_def   );
GEN_API CodeOperator    parse_operator     ( Str operator_def    );
GEN_API CodeOpCast      parse_operator_cast( Str operator_def    );
GEN_API CodeStruct      parse_struct       ( Str struct_def      );
GEN_API CodeTemplate    parse_template     ( Str template_def    );
GEN_API CodeTypename    parse_type         ( Str type_def        );
GEN_API CodeTypedef     parse_typedef      ( Str typedef_def     );
GEN_API CodeUnion       parse_union        ( Str union_def       );
GEN_API CodeUsing       parse_using        ( Str using_def       );
GEN_API CodeVar         parse_variable     ( Str var_def         );

#pragma endregion Parsing

#pragma region Untyped text

GEN_API ssize token_fmt_va( char* buf, usize buf_size, s32 num_tokens, va_list va );
//! Do not use directly. Use the token_fmt macro instead.
Str   token_fmt_impl( ssize, ... );

GEN_API Code untyped_str      ( Str content);
GEN_API Code untyped_fmt      ( char const* fmt, ... );
GEN_API Code untyped_token_fmt( s32 num_tokens, char const* fmt, ... );

#pragma endregion Untyped text

#pragma region Macros

#ifndef gen_main
#define gen_main main
#endif

#ifndef name
//	Convienence for defining any name used with the gen api.
//  Lets you provide the length and string literal to the functions without the need for the DSL.
#	if GEN_COMPILER_C
#		define name( Id_ ) (Str){ stringize(Id_), sizeof(stringize( Id_ )) - 1 }
#	else
#		define name( Id_ )  Str { stringize(Id_), sizeof(stringize( Id_ )) - 1 }
#	endif
#endif

#ifndef code
//  Same as name just used to indicate intention of literal for code instead of names.
#	if GEN_COMPILER_C
#		define code( ... ) (Str){ stringize( __VA_ARGS__ ), sizeof(stringize(__VA_ARGS__)) - 1 }
#	else
#		define code( ... )  Str { stringize( __VA_ARGS__ ), sizeof(stringize(__VA_ARGS__)) - 1 }
#	endif
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
