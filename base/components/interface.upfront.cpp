#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "interface.cpp"
#endif

#pragma region Upfront

enum OpValidateResult : u32
{
	OpValResult_Fail,
	OpValResult_Global,
	OpValResult_Member
};

internal neverinline
OpValidateResult operator__validate( Operator op, CodeParams params_code, CodeTypename ret_type, CodeSpecifiers specifier )
{
	if ( op == Op_Invalid )
	{
		log_failure("gen::def_operator: op cannot be invalid");
		return OpValResult_Fail;
	}

#pragma region Helper Macros
#	define check_params()                                                                                                   \
	if ( ! params_code )                                                                                                    \
	{                                                                                                                       \
		log_failure("gen::def_operator: params is null and operator %S requires it", operator_to_str(op));                  \
		return OpValResult_Fail;                                                                                            \
	}                                                                                                                       \
	if ( params_code->Type != CT_Parameters )                                                                               \
	{                                                                                                                       \
		log_failure("gen::def_operator: params is not of Parameters type - %S", code_debug_str( cast(Code, params_code)));  \
		return OpValResult_Fail;                                                                                            \
	}

#	define check_param_eq_ret()                                                                            \
	if ( ! is_member_symbol && ! code_is_equal(cast(Code, params_code->ValueType), cast(Code, ret_type)) ) \
	{                                                                                                      \
		log_failure("gen::def_operator: operator %S requires first parameter to equal return type\n"       \
			"param types: %S\n"                                                                            \
			"return type: %S",                                                                             \
			operator_to_str(op),                                                                           \
			code_debug_str(cast(Code, params_code)),                                                       \
			code_debug_str(cast(Code, ret_type))                                                           \
		);                                                                                                 \
		return OpValResult_Fail;                                                                           \
	}
#pragma endregion Helper Macros

	if ( ! ret_type )
	{
		log_failure("gen::def_operator: ret_type is null but is required by operator %S", operator_to_str(op));
	}

	if ( ret_type->Type != CT_Typename )
	{
		log_failure("gen::def_operator: operator %S - ret_type is not of typename type - %S",
			operator_to_str(op),
			code_debug_str(cast(Code, ret_type))
		);
		return OpValResult_Fail;
	}

	bool is_member_symbol = false;

	switch ( op )
	{
#	define specs( ... ) num_args( __VA_ARGS__ ), __VA_ARGS__
		case Op_Assign:
			check_params();

			if ( params_code->NumEntries > 1 )
			{
				log_failure("gen::def_operator: "
					"operator %S does not support non-member definition (more than one parameter provided) - %S",
					operator_to_str(op),
					code_debug_str(cast(Code, params_code))
				);
				return OpValResult_Fail;
			}

			is_member_symbol = true;
			break;

		case Op_Assign_Add:
		case Op_Assign_Subtract:
		case Op_Assign_Multiply:
		case Op_Assign_Divide:
		case Op_Assign_Modulo:
		case Op_Assign_BAnd:
		case Op_Assign_BOr:
		case Op_Assign_BXOr:
		case Op_Assign_LShift:
		case Op_Assign_RShift:
			check_params();

			if ( params_code->NumEntries == 1 )
				is_member_symbol = true;

			else
				check_param_eq_ret();

			if (params_code->NumEntries > 2 )
			{
				log_failure("gen::def_operator: operator %S may not be defined with more than two parametes - param count; %d\n%S"
					, operator_to_str(op)
					, params_code->NumEntries
					, code_debug_str(cast(Code, params_code))
				);
				return OpValResult_Fail;
			}
			break;

		case Op_Increment:
		case Op_Decrement:
			// If its not set, it just means its a prefix member op.
			if ( params_code )
			{
				if ( params_code->Type != CT_Parameters )
				{
					log_failure("gen::def_operator: operator %S params code provided is not of Parameters type - %S"
						, operator_to_str(op)
						, code_debug_str(cast(Code, params_code))
					);
					return OpValResult_Fail;
				}

				switch ( params_code->NumEntries )
				{
					case 1:
						if ( code_is_equal((Code)params_code->ValueType, (Code)t_int ) )
							is_member_symbol = true;

						else
							check_param_eq_ret();
						break;

					case 2:
						check_param_eq_ret();

						if ( ! code_is_equal((Code)params_get(params_code, 1), (Code)t_int ) )
						{
							log_failure("gen::def_operator: "
								"operator %S requires second parameter of non-member definition to be int for post-decrement",
								operator_to_str(op)
							);
							return OpValResult_Fail;
						}
						break;

					default:
						log_failure("gen::def_operator: operator %S recieved unexpected number of parameters recived %d instead of 0-2"
							, operator_to_str(op)
							, params_code->NumEntries
						);
						return OpValResult_Fail;
				}
			}
			break;

		case Op_Unary_Plus:
		case Op_Unary_Minus:
			if ( ! params_code )
				is_member_symbol = true;

			else
			{
				if ( params_code->Type != CT_Parameters )
				{
					log_failure("gen::def_operator: params is not of Parameters type - %S", code_debug_str((Code)params_code));
					return OpValResult_Fail;
				}

				if ( code_is_equal((Code)params_code->ValueType, (Code)ret_type ) )
				{
					log_failure("gen::def_operator: "
						"operator %S is non-member symbol yet first paramter does not equal return type\n"
						"param type: %S\n"
						"return type: %S\n"
						, code_debug_str((Code)params_code)
						, code_debug_str((Code)ret_type)
					);
					return OpValResult_Fail;
				}

				if ( params_code->NumEntries > 1 )
				{
					log_failure("gen::def_operator: operator %S may not have more than one parameter - param count: %d"
						, operator_to_str(op)
						, params_code->NumEntries
					);
					return OpValResult_Fail;
				}
			}
			break;

		case Op_BNot:
		{
			// Some compilers let you do this...
		#if 0
			if ( ! ret_type.is_equal( t_bool) )
			{
				log_failure( "gen::def_operator: operator %S return type is not a boolean - %S", operator_to_str(op) code_debug_str(params_code) );
				return OpValidateResult::Fail;
			}
		#endif

			if ( ! params_code )
				is_member_symbol = true;

			else
			{
				if ( params_code->Type != CT_Parameters )
				{
					log_failure( "gen::def_operator: operator %S - params is not of Parameters type - %S", operator_to_str(op), code_debug_str((Code)params_code) );
					return OpValResult_Fail;
				}

				if ( params_code->NumEntries > 1 )
				{
					log_failure(
						"gen::def_operator: operator %S may not have more than one parameter - param count: %d",
						operator_to_str( op ),
						params_code->NumEntries
					);
					return OpValResult_Fail;
				}
			}
			break;
		}

		case Op_Add:
		case Op_Subtract:
		case Op_Multiply:
		case Op_Divide:
		case Op_Modulo:
		case Op_BAnd:
		case Op_BOr:
		case Op_BXOr:
		case Op_LShift:
		case Op_RShift:
			check_params();

			switch ( params_code->NumEntries )
			{
				case 1:
					is_member_symbol = true;
					break;

				case 2:
					// This is allowed for arithemtic operators
					// if ( ! code_is_equal((Code)params_code->ValueType, (Code)ret_type ) )
					// {
					// 	log_failure("gen::def_operator: "
					// 		"operator %S is non-member symbol yet first paramter does not equal return type\n"
					// 		"param type: %S\n"
					// 		"return type: %S\n"
					// 		, code_debug_str((Code)params_code)
					// 		, code_debug_str((Code)ret_type)
					// 	);
					// 	return OpValResult_Fail;
					// }
					break;

				default:
					log_failure("gen::def_operator: operator %S recieved unexpected number of paramters recived %d instead of 0-2"
						, operator_to_str(op)
						, params_code->NumEntries
					);
					return OpValResult_Fail;
			}
			break;

		case Op_UnaryNot:
			if ( ! params_code )
				is_member_symbol = true;

			else
			{
				if ( params_code->Type != CT_Parameters )
				{
					log_failure("gen::def_operator: operator %S - params is not of Parameters type - %S", operator_to_str(op), code_debug_str((Code)params_code));
					return OpValResult_Fail;
				}

				if ( params_code->NumEntries != 1 )
				{
					log_failure("gen::def_operator: operator %S recieved unexpected number of paramters recived %d instead of 0-1"
						, operator_to_str(op)
						, params_code->NumEntries
					);
					return OpValResult_Fail;
				}
			}

			if ( ! code_is_equal((Code)ret_type, (Code)t_bool ))
			{
				log_failure("gen::def_operator: operator %S return type must be of type bool - %S"
					, operator_to_str(op)
					, code_debug_str((Code)ret_type)
				);
				return OpValResult_Fail;
			}
			break;

		case Op_LAnd:
		case Op_LOr:
		case Op_LEqual:
		case Op_LNot:
		case Op_Lesser:
		case Op_Greater:
		case Op_LesserEqual:
		case Op_GreaterEqual:
			check_params();

			switch ( params_code->NumEntries )
			{
				case 1:
					is_member_symbol = true;
					break;

				case 2:
					break;

				default:
					log_failure("gen::def_operator: operator %S recieved unexpected number of paramters recived %d instead of 1-2"
						, operator_to_str(op)
						, params_code->NumEntries
					);
					return OpValResult_Fail;
			}
			break;

		case Op_Indirection:
		case Op_AddressOf:
		case Op_MemberOfPointer:
			if ( params_code && params_code->NumEntries > 1)
			{
				log_failure("gen::def_operator: operator %S recieved unexpected number of paramters recived %d instead of 0-1"
					, operator_to_str(op)
					, params_code->NumEntries
				);
				return OpValResult_Fail;
			}
			else
			{
				is_member_symbol = true;
			}
			break;

		case Op_PtrToMemOfPtr:
			if ( params_code )
			{
				log_failure("gen::def_operator: operator %S expects no paramters - %S", operator_to_str(op), code_debug_str((Code)params_code));
				return OpValResult_Fail;
			}
			break;

		case Op_Subscript:
		case Op_FunctionCall:
		case Op_Comma:
			check_params();
			break;

		case Op_New:
		case Op_Delete:
			// This library doesn't support validating new and delete yet.
			break;
#	undef specs
	}

	return is_member_symbol ? OpValResult_Member : OpValResult_Global;
#	undef check_params
#	undef check_ret_type
#	undef check_param_eq_ret
}

forceinline
bool name__check( char const* context, Str name )
{
	if ( name.Len <= 0 ) {
		log_failure( "gen::%s: Invalid name length provided - %d",  name.Len );
		return false;
	}
	if ( name.Ptr == nullptr ) {
		log_failure( "gen::%s: name is null" );
		return false;
	}
	return true;
}
#define name_check( context, name ) name__check( #context, name )

forceinline
bool null__check( char const* context, char const* code_id, Code code ) {
	if ( code == nullptr ) {
		log_failure( "gen::%s: %s provided is null", context, code_id );
		return false;
	}
	return true;
}
#define null_check( context, code ) null__check( #context, #code, cast(Code, code) )

/*
The implementation of the upfront constructors involves doing three things:
* Validate the arguments given to construct the intended type of AST is valid.
* Construct said AST type.
* Lock the AST (set to readonly) and return the valid object.

If any of the validation fails, it triggers a call to log_failure with as much info the give the user so that they can hopefully
identify the issue without having to debug too much (at least they can debug though...)

The largest of the functions is related to operator overload definitions.
The library validates a good protion of their form and thus the argument processing for is quite a bit.
*/
CodeAttributes def_attributes( Str content )
{
	if ( content.Len <= 0 || content.Ptr == nullptr ) {
		log_failure( "gen::def_attributes: Invalid attributes provided" );
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	Code
	result          = make_code();
	result->Type    = CT_PlatformAttributes;
	result->Name    = cache_str( content );
	result->Content = result->Name;
	return (CodeAttributes) result;
}

CodeComment def_comment( Str content )
{
	if ( content.Len <= 0 || content.Ptr == nullptr )
	{
		log_failure( "gen::def_comment: Invalid comment provided:" );
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}

	StrBuilder  cmt_formatted = strbuilder_make_reserve( _ctx->Allocator_Temp, kilobytes(1) );
	char const* end           = content.Ptr + content.Len;
	char const* scanner       = content.Ptr;
	s32         curr          = 0;
	do
	{
		char const* next   = scanner;
		s32         length = 0;
		while ( next != end && scanner[ length ] != '\n' )
		{
			next = scanner + length;
			length++;
		}
		length++;

		strbuilder_append_fmt(& cmt_formatted, "//%.*s", length, scanner );
		scanner += length;
	}
	while ( scanner <= end );

	if ( * strbuilder_back(cmt_formatted) != '\n' )
		strbuilder_append_str( & cmt_formatted, txt("\n") );

	Str name = strbuilder_to_str(cmt_formatted);

	Code
	result          = make_code();
	result->Type    = CT_Comment;
	result->Name    = cache_str( name );
	result->Content = result->Name;

	strbuilder_free(& cmt_formatted);

	return (CodeComment) result;
}

CodeConstructor def_constructor( Opts_def_constructor p )
{
	if ( p.params && p.params->Type != CT_Parameters ) {
		log_failure("gen::def_constructor: params must be of Parameters type - %s", code_debug_str((Code)p.params));
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}

	CodeConstructor result = (CodeConstructor) make_code();
	if ( p.params ) {
		result->Params = p.params;
	}
	if ( p.initializer_list ) {
		result->InitializerList = p.initializer_list;
	}
	if ( p.body )
	{
		switch ( p.body->Type ) {
			case CT_Function_Body:
			case CT_Untyped:
			break;

			default:
				log_failure("gen::def_constructor: body must be either of Function_Body or Untyped type - %s", code_debug_str(p.body));
				return InvalidCode;
		}

		result->Type = CT_Constructor;
		result->Body = p.body;
	}
	else
	{
		result->Type = CT_Constructor_Fwd;
	}
	return result;
}

CodeClass def_class( Str name, Opts_def_struct p )
{
	if ( ! name_check( def_class, name ) ) {
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	if ( p.attributes && p.attributes->Type != CT_PlatformAttributes ) {
		log_failure( "gen::def_class: attributes was not a 'PlatformAttributes' type: %s", code_debug_str(p.attributes) );
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	if ( p.parent && ( p.parent->Type != CT_Class && p.parent->Type != CT_Struct && p.parent->Type != CT_Typename && p.parent->Type != CT_Untyped ) ) {
		log_failure( "gen::def_class: parent provided is not type 'Class', 'Struct', 'Typeanme', or 'Untyped': %s", code_debug_str(p.parent) );
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}

	CodeClass
	result               = (CodeClass) make_code();
	result->Name         = cache_str( name );
	result->ModuleFlags  = p.mflags;
	result->Attributes   = p.attributes;
	result->Specs        = p.specifiers;
	result->ParentAccess = p.parent_access;
	result->ParentType   = p.parent;
	if ( p.body )
	{
		switch ( p.body->Type )
		{
			case CT_Class_Body:
			case CT_Untyped:
			break;

			default:
				log_failure("gen::def_class: body must be either of Class_Body or Untyped type - %s", code_debug_str(p.body));
				return InvalidCode;
		}

		result->Type         = CT_Class;
		result->Body         = p.body;
		result->Body->Parent = cast(Code, result);
	}
	else {
		result->Type = CT_Class_Fwd;
	}
	for (s32 idx = 0; idx < p.num_interfaces; idx++ ) {
		class_add_interface(result, p.interfaces[idx] );
	}
	return result;
}

CodeDefine def_define( Str name, MacroType type, Opts_def_define p )
{
	if ( ! name_check( def_define, name ) ) {
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	CodeDefine
	result          = (CodeDefine) make_code();
	result->Type    = CT_Preprocess_Define;
	result->Name    = cache_str( name );
	result->Params  = p.params;
	if ( p.content.Len <= 0 || p.content.Ptr == nullptr )
		result->Body = untyped_str( txt("\n") );
	else
		result->Body = untyped_str( strbuilder_to_str(strbuilder_fmt_buf(_ctx->Allocator_Temp, "%S\n", p.content)) );

	b32  register_define = ! p.dont_register_to_preprocess_macros;
	if ( register_define ) {
		Macro macro_entry = { result->Name, type, p.flags };
		register_macro(macro_entry);
	}
	return result;
}

CodeDestructor def_destructor( Opts_def_destructor p )
{
	if ( p.specifiers && p.specifiers->Type != CT_Specifiers ) {
		log_failure( "gen::def_destructor: specifiers was not a 'Specifiers' type: %s", code_debug_str(p.specifiers) );
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}

	CodeDestructor
	result        = (CodeDestructor) make_code();
	result->Specs = p.specifiers;
	if ( p.body )
	{
		switch ( p.body->Type )
		{
			case CT_Function_Body:
			case CT_Untyped:
			break;

			default:
				log_failure("gen::def_destructor: body must be either of Function_Body or Untyped type - %s", code_debug_str(p.body));
				return InvalidCode;
		}

		result->Type = CT_Destructor;
		result->Body = p.body;
	}
	else
	{
		result->Type = CT_Destructor_Fwd;
	}
	return result;
}

CodeEnum def_enum( Str name, Opts_def_enum p )
{
	if ( ! name_check( def_enum, name ) ) {
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	if ( p.type && p.type->Type != CT_Typename ) {
		log_failure( "gen::def_enum: enum underlying type provided was not of type Typename: %s", code_debug_str(p.type) );
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	if ( p.attributes && p.attributes->Type != CT_PlatformAttributes ) {
		log_failure( "gen::def_enum: attributes was not a 'PlatformAttributes' type: %s", code_debug_str(p.attributes) );
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}

	CodeEnum
	result              = (CodeEnum) make_code();
	result->Name        = cache_str( name );
	result->ModuleFlags = p.mflags;
	if ( p.body )
	{
		switch ( p.body->Type )
		{
			case CT_Enum_Body:
			case CT_Untyped:
			break;

			default:
				log_failure( "gen::def_enum: body must be of Enum_Body or Untyped type %s", code_debug_str(p.body));
				return InvalidCode;
		}

		result->Type = p.specifier == EnumDecl_Class ?
			CT_Enum_Class : CT_Enum;

		result->Body = p.body;
	}
	else
	{
		result->Type = p.specifier == EnumDecl_Class ?
			CT_Enum_Class_Fwd : CT_Enum_Fwd;
	}
	result->Attributes = p.attributes;

	if ( p.type ) {
		result->UnderlyingType = p.type;
	}
	else if ( p.type_macro ) {
		result->UnderlyingTypeMacro = p.type_macro;
	}
	else if ( result->Type != CT_Enum_Class_Fwd && result->Type != CT_Enum_Fwd )
	{
		log_failure( "gen::def_enum: enum forward declaration must have an underlying type" );
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	return result;
}

CodeExec def_execution( Str content )
{
	if ( content.Len <= 0 || content.Ptr == nullptr ) {
		log_failure( "gen::def_execution: Invalid execution provided" );
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	CodeExec
	result          = (CodeExec) make_code();
	result->Type    = CT_Execution;
	result->Content = cache_str( content );
	return result;
}

CodeExtern def_extern_link( Str name, CodeBody body )
{
	if ( ! name_check(def_extern_link, name) || ! null_check(def_extern_link, body) ) {
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	if ( body->Type != CT_Extern_Linkage_Body && body->Type != CT_Untyped ) {
		log_failure("gen::def_extern_linkage: body is not of extern_linkage or untyped type %s", code_debug_str(body));
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	CodeExtern
	result        = (CodeExtern)make_code();
	result->Type  = CT_Extern_Linkage;
	result->Name  = cache_str( name );
	result->Body  = body;
	return result;
}

CodeFriend def_friend( Code declaration )
{
	if ( ! null_check( def_friend, declaration ) ) {
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	switch ( declaration->Type )
	{
		case CT_Class_Fwd:
		case CT_Function_Fwd:
		case CT_Operator_Fwd:
		case CT_Struct_Fwd:
		case CT_Class:
		case CT_Function:
		case CT_Operator:
		case CT_Struct:
		break;

		default:
			log_failure("gen::def_friend: requires declartion to have class, function, operator, or struct - %s", code_debug_str(declaration));
			return InvalidCode;
	}
	CodeFriend
	result              = (CodeFriend) make_code();
	result->Type        = CT_Friend;
	result->Declaration = declaration;
	return result;
}

CodeFn def_function( Str name, Opts_def_function p )
{
	if ( ! name_check( def_function, name )) {
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	if ( p.params && p.params->Type != CT_Parameters ) {
		log_failure( "gen::def_function: params was not a `Parameters` type: %s", code_debug_str(p.params) );
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	if ( p.ret_type && p.ret_type->Type != CT_Typename ) {
		log_failure( "gen::def_function: ret_type was not a Typename: %s", code_debug_str(p.ret_type) );
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	if ( p.specs && p.specs-> Type != CT_Specifiers ) {
		log_failure( "gen::def_function: specifiers was not a `Specifiers` type: %s", code_debug_str(p.specs) );
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	if ( p.attrs && p.attrs->Type != CT_PlatformAttributes ) {
		log_failure( "gen::def_function: attributes was not a `PlatformAttributes` type: %s", code_debug_str(p.attrs) );
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}

	CodeFn
	result              = (CodeFn) make_code();
	result->Name        = cache_str( name );
	result->ModuleFlags = p.mflags;
	if ( p.body )
	{
		switch ( p.body->Type )
		{
			case CT_Function_Body:
			case CT_Execution:
			case CT_Untyped:
				break;

			default:
			{
				log_failure("gen::def_function: body must be either of Function_Body, Execution, or Untyped type. %s", code_debug_str(p.body));
				return InvalidCode;
			}
		}
		result->Type = CT_Function;
		result->Body = p.body;
	}
	else
	{
		result->Type = CT_Function_Fwd;
	}
	result->Attributes = p.attrs;
	result->Specs      = p.specs;
	result->Params     = p.params;
	result->ReturnType = p.ret_type ? p.ret_type : t_void;
	return result;
}

CodeInclude def_include( Str path, Opts_def_include p )
{
	if ( path.Len <= 0 || path.Ptr == nullptr ) {
		log_failure( "gen::def_include: Invalid path provided - %d" );
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	StrBuilder content = p.foreign ?
			strbuilder_fmt_buf( _ctx->Allocator_Temp, "<%.*s>",   path.Len, path.Ptr )
		:	strbuilder_fmt_buf( _ctx->Allocator_Temp, "\"%.*s\"", path.Len, path.Ptr );

	CodeInclude
	result          = (CodeInclude) make_code();
	result->Type    = CT_Preprocess_Include;
	result->Name    = cache_str( strbuilder_to_str(content) );
	result->Content = result->Name;
	return result;
}

CodeModule def_module( Str name, Opts_def_module p )
{
	if ( ! name_check( def_module, name )) {
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	CodeModule
	result              = (CodeModule) make_code();
	result->Type        = CT_Module;
	result->Name        = cache_str( name );
	result->ModuleFlags = p.mflags;
	return result;
}

CodeNS def_namespace( Str name, CodeBody body, Opts_def_namespace p )
{
	if ( ! name_check( def_namespace, name )) {
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	if ( ! null_check( def_namespace, body)) {
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	if ( body && body->Type != CT_Namespace_Body && body->Type != CT_Untyped ) {
		log_failure("gen::def_namespace: body is not of namespace or untyped type %s", code_debug_str(body));
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	CodeNS
	result              = (CodeNS) make_code();
	result->Type        = CT_Namespace;
	result->Name        = cache_str( name );
	result->ModuleFlags = p.mflags;
	result->Body        = body;
	return result;
}

CodeOperator def_operator( Operator op, Str nspace, Opts_def_operator p )
{
	if ( p.attributes && p.attributes->Type != CT_PlatformAttributes ) {
		log_failure( "gen::def_operator: PlatformAttributes was provided but its not of attributes type: %s", code_debug_str(p.attributes) );
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	if ( p.specifiers && p.specifiers->Type != CT_Specifiers ) {
		log_failure( "gen::def_operator: Specifiers was provided but its not of specifiers type: %s", code_debug_str(p.specifiers) );
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}

	OpValidateResult check_result = operator__validate( op, p.params, p.ret_type, p.specifiers );
	if ( check_result == OpValResult_Fail ) {
		return InvalidCode;
	}

	char const* name = nullptr;

	Str op_str = operator_to_str( op );
	if ( nspace.Len > 0 )
		name = c_str_fmt_buf( "%.*soperator %.*s", nspace.Len, nspace.Ptr, op_str.Len, op_str.Ptr );
	else
		name = c_str_fmt_buf( "operator %.*s", op_str.Len, op_str.Ptr );

	Str name_resolved = { name, c_str_len(name) };

	CodeOperator
	result              = (CodeOperator) make_code();
	result->Name        = cache_str( name_resolved );
	result->ModuleFlags = p.mflags;
	result->Op          = op;
	if ( p.body )
	{
		switch ( p.body->Type )
		{
			case CT_Function_Body:
			case CT_Execution:
			case CT_Untyped:
				break;

			default:
			{
				log_failure("gen::def_operator: body must be either of Function_Body, Execution, or Untyped type. %s", code_debug_str(p.body));
				GEN_DEBUG_TRAP();
				return InvalidCode;
			}
		}

		result->Type = check_result == OpValResult_Global ?
			CT_Operator : CT_Operator_Member;

		result->Body = p.body;
	}
	else
	{
		result->Type = check_result == OpValResult_Global ?
			CT_Operator_Fwd : CT_Operator_Member_Fwd;
	}
	result->Attributes = p.attributes;
	result->Specs      = p.specifiers;
	result->ReturnType = p.ret_type;
	result->Params     = p.params;
	return result;
}

CodeOpCast def_operator_cast( CodeTypename type, Opts_def_operator_cast p )
{
	if ( ! null_check( def_operator_cast, type )) {
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	if ( type->Type != CT_Typename ) {
		log_failure( "gen::def_operator_cast: type is not a typename - %s", code_debug_str(type) );
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}

	CodeOpCast result = (CodeOpCast) make_code();
	if (p.body)
	{
		result->Type = CT_Operator_Cast;

		if ( p.body->Type != CT_Function_Body && p.body->Type != CT_Execution ) {
			log_failure( "gen::def_operator_cast: body is not of function body or execution type - %s", code_debug_str(p.body) );
			GEN_DEBUG_TRAP();
			return InvalidCode;
		}
		result->Body = p.body;
	}
	else
	{
		result->Type = CT_Operator_Cast_Fwd;
	}
	result->Specs     = p.specs;
	result->ValueType = type;
	return result;
}

CodeParams def_param( CodeTypename type, Str name, Opts_def_param p )
{
	if ( ! name_check( def_param, name ) || ! null_check( def_param, type ) ) {
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	if ( type->Type != CT_Typename ) {
		log_failure( "gen::def_param: type is not a typename - %s", code_debug_str(type) );
		return InvalidCode;
	}
	if ( p.value && p.value->Type != CT_Untyped ) {
		log_failure( "gen::def_param: value is not untyped - %s", code_debug_str(p.value) );
		return InvalidCode;
	}
	CodeParams
	result            = (CodeParams) make_code();
	result->Type      = CT_Parameters;
	result->Name      = cache_str( name );
	result->ValueType = type;
	result->Value     = p.value;
	result->NumEntries++;
	return result;
}

CodePragma def_pragma( Str directive )
{
	if ( directive.Len <= 0 || directive.Ptr == nullptr ) {
		log_failure( "gen::def_comment: Invalid comment provided:" );
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	CodePragma
	result          = (CodePragma) make_code();
	result->Type    = CT_Preprocess_Pragma;
	result->Content = cache_str( directive );
	return result;
}

CodePreprocessCond def_preprocess_cond( EPreprocessCond type, Str expr )
{
	if ( expr.Len <= 0 || expr.Ptr == nullptr ) {
		log_failure( "gen::def_comment: Invalid comment provided:" );
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	CodePreprocessCond
	result          = (CodePreprocessCond) make_code();
	result->Content = cache_str( expr );
	switch (type)
	{
		case PreprocessCond_If:
			result->Type = CT_Preprocess_If;
		break;
		case PreprocessCond_IfDef:
			result->Type = CT_Preprocess_IfDef;
		break;
		case PreprocessCond_IfNotDef:
			result->Type = CT_Preprocess_IfNotDef;
		break;
		case PreprocessCond_ElIf:
			result->Type = CT_Preprocess_ElIf;
		break;
	}
	return result;
}

CodeSpecifiers def_specifier( Specifier spec )
{
	CodeSpecifiers
	result       = (CodeSpecifiers) make_code();
	result->Type = CT_Specifiers;
	specifiers_append(result, spec );
	return result;
}

CodeStruct def_struct( Str name, Opts_def_struct p )
{
	if ( p.attributes && p.attributes->Type != CT_PlatformAttributes ) {
		log_failure( "gen::def_struct: attributes was not a `PlatformAttributes` type - %s", code_debug_str(cast(Code, p.attributes)) );
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	if ( p.parent && p.parent->Type != CT_Typename ) {
		log_failure( "gen::def_struct: parent was not a `Struct` type - %s", code_debug_str(p.parent) );
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	if ( p.body && p.body->Type != CT_Struct_Body ) {
		log_failure( "gen::def_struct: body was not a Struct_Body type - %s", code_debug_str(p.body) );
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}

	CodeStruct
	result              = (CodeStruct) make_code();
	result->ModuleFlags = p.mflags;
	if ( name.Len )
		result->Name = cache_str( name );

	if ( p.body ) {
		result->Type = CT_Struct;
		result->Body = p.body;
	}
	else {
		result->Type = CT_Struct_Fwd;
	}
	result->Attributes   = p.attributes;
	result->Specs        = p.specifiers;
	result->ParentAccess = p.parent_access;
	result->ParentType   = p.parent;

	for (s32 idx = 0; idx < p.num_interfaces; idx++ ) {
		struct_add_interface(result, p.interfaces[idx] );
	}
	return result;
}

CodeTemplate def_template( CodeParams params, Code declaration, Opts_def_template p )
{
	if ( ! null_check( def_template, declaration ) ) {
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	if ( params && params->Type != CT_Parameters ){
		log_failure( "gen::def_template: params is not of parameters type - %s", code_debug_str(params) );
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	switch (declaration->Type )
	{
		case CT_Class:
		case CT_Function:
		case CT_Struct:
		case CT_Variable:
		case CT_Using:
		break;

		default:
			log_failure( "gen::def_template: declaration is not of class, function, struct, variable, or using type - %s", code_debug_str(declaration) );
	}
	CodeTemplate
	result              = (CodeTemplate) make_code();
	result->Type        = CT_Template;
	result->ModuleFlags = p.mflags;
	result->Params      = params;
	result->Declaration = declaration;
	return result;
}

CodeTypename def_type( Str name, Opts_def_type p )
{
	if ( ! name_check( def_type, name )) {
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	Code           arrayexpr  = p.arrayexpr;
	CodeSpecifiers specifiers = p.specifiers;
	CodeAttributes attributes = p.attributes;
	if ( p.attributes && p.attributes->Type != CT_PlatformAttributes ) {
		log_failure( "gen::def_type: attributes is not of attributes type - %s", code_debug_str((Code)p.attributes) );
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	if ( p.specifiers && p.specifiers->Type != CT_Specifiers ) {
		log_failure( "gen::def_type: specifiers is not of specifiers type - %s", code_debug_str((Code)p.specifiers) );
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	if ( p.arrayexpr && p.arrayexpr->Type != CT_Untyped ) {
		log_failure( "gen::def_type: arrayexpr is not of untyped type - %s", code_debug_str((Code)p.arrayexpr) );
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	CodeTypename
	result             = (CodeTypename) make_code();
	result->Name       = cache_str( name );
	result->Type       = CT_Typename;
	result->Attributes = p.attributes;
	result->Specs      = p.specifiers;
	result->ArrExpr    = p.arrayexpr;
	result->TypeTag    = p.type_tag;
	return result;
}

CodeTypedef def_typedef( Str name, Code type, Opts_def_typedef p )
{
	if ( ! null_check( def_typedef, type ) ) {
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	switch ( type->Type )
	{
		case CT_Class:
		case CT_Class_Fwd:
		case CT_Enum:
		case CT_Enum_Fwd:
		case CT_Enum_Class:
		case CT_Enum_Class_Fwd:
		case CT_Function_Fwd:
		case CT_Struct:
		case CT_Struct_Fwd:
		case CT_Union:
		case CT_Typename:
			break;
		default:
			log_failure( "gen::def_typedef: type was not a Class, Enum, Function Forward, Struct, Typename, or Union - %s", code_debug_str((Code)type) );
			GEN_DEBUG_TRAP();
			return InvalidCode;
	}
	if ( p.attributes && p.attributes->Type != CT_PlatformAttributes ) {
		log_failure( "gen::def_typedef: attributes was not a PlatformAttributes - %s", code_debug_str((Code)p.attributes) );
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}

	// Registering the type.
	CodeTypename registered_type = def_type( name );
	if ( ! registered_type ) {
		log_failure( "gen::def_typedef: failed to register type" );
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}

	CodeTypedef
	result                 = (CodeTypedef) make_code();
	result->Type           = CT_Typedef;
	result->ModuleFlags    = p.mflags;
	result->UnderlyingType = type;

	if ( name.Len <= 0  )
	{
		if (type->Type != CT_Untyped) {
			log_failure( "gen::def_typedef: name was empty and type was not untyped (indicating its a function typedef) - %s", code_debug_str(type) );
			GEN_DEBUG_TRAP();
			return InvalidCode;
		}
		result->Name       = cache_str( type->Name );
		result->IsFunction = true;
	}
	else
	{
		result->Name       = cache_str( name );
		result->IsFunction = false;
	}
	return result;
}

CodeUnion def_union( Str name, CodeBody body, Opts_def_union p )
{
	if ( ! null_check( def_union, body ) ) {
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	if ( body->Type != CT_Union_Body ) {
		log_failure( "gen::def_union: body was not a Union_Body type - %s", code_debug_str(body) );
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	if ( p.attributes && p.attributes->Type != CT_PlatformAttributes ) {
		log_failure( "gen::def_union: attributes was not a PlatformAttributes type - %s", code_debug_str(p.attributes) );
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	CodeUnion
	result              = (CodeUnion) make_code();
	result->ModuleFlags = p.mflags;
	result->Type        = CT_Union;
	result->Body        = body;
	result->Attributes  = p.attributes;
	if ( name.Ptr )
		result->Name = cache_str( name );
	return result;
}

CodeUsing def_using( Str name, CodeTypename type, Opts_def_using p )
{
	if ( ! name_check( def_using, name ) || null_check( def_using, type ) ) {
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}

	CodeTypename register_type = def_type( name );
	if ( ! register_type ) {
		log_failure( "gen::def_using: failed to register type" );
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	if ( p.attributes && p.attributes->Type != CT_PlatformAttributes ) {
		log_failure( "gen::def_using: attributes was not a PlatformAttributes type - %s", code_debug_str(p.attributes) );
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	CodeUsing
	result                 = (CodeUsing) make_code();
	result->Name           = cache_str( name );
	result->ModuleFlags    = p.mflags;
	result->Type           = CT_Using;
	result->UnderlyingType = type;
	result->Attributes     = p.attributes;
	return result;
}

CodeUsing def_using_namespace( Str name )
{
	if ( ! name_check( def_using_namespace, name ) ) {
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	CodeUsing
	result          = (CodeUsing) make_code();
	result->Name    = cache_str( name );
	result->Type    = CT_Using_Namespace;
	return result;
}

CodeVar def_variable( CodeTypename type, Str name, Opts_def_variable p )
{
	if ( ! name_check( def_variable, name ) || ! null_check( def_variable, type ) ) {
		GEN_DEBUG_TRAP();
		return InvalidCode;
	}
	if ( p.attributes && p.attributes->Type != CT_PlatformAttributes )
	{
		log_failure( "gen::def_variable: attributes was not a `PlatformAttributes` type - %s", code_debug_str(p.attributes) );
		return InvalidCode;
	}
	if ( p.specifiers && p.specifiers->Type != CT_Specifiers )
	{
		log_failure( "gen::def_variable: specifiers was not a `Specifiers` type - %s", code_debug_str(p.specifiers) );
		return InvalidCode;
	}
	if ( type->Type != CT_Typename )
	{
		log_failure( "gen::def_variable: type was not a Typename - %s", code_debug_str(type) );
		return InvalidCode;
	}
	if ( p.value && p.value->Type != CT_Untyped )
	{
		log_failure( "gen::def_variable: value was not a `Untyped` type - %s", code_debug_str(p.value) );
		return InvalidCode;
	}
	CodeVar
	result              = (CodeVar) make_code();
	result->Name        = cache_str( name );
	result->Type        = CT_Variable;
	result->ModuleFlags = p.mflags;
	result->ValueType   = type;
	result->Attributes  = p.attributes;
	result->Specs       = p.specifiers;
	result->Value       = p.value;
	return result;
}

#pragma region Helper Macros for def_**_body functions
#define def_body_start( Name_ )                                               \
if ( num <= 0 )                                                               \
{                                                                             \
	log_failure("gen::" stringize(Name_) ": num cannot be zero or negative"); \
	return InvalidCode;                                                       \
}

#define def_body_code_array_start( Name_ )                                     \
if ( num <= 0 )                                                                \
{                                                                              \
	log_failure("gen::" stringize(Name_) ": num cannot be zero or negative");  \
	return InvalidCode;                                                        \
}                                                                              \
																			   \
if ( codes == nullptr )                                                        \
{                                                                              \
	log_failure("gen::" stringize(Name_)" : Provided a null array of codes");  \
	return InvalidCode;                                                        \
}

#pragma endregion Helper Macros for def_**_body functions

CodeBody def_class_body( s32 num, ... )
{
	def_body_start( def_class_body );

	CodeBody result = ( CodeBody )make_code();
	result->Type    = CT_Class_Body;

	va_list va;
	va_start( va, num );
	do
	{
		Code_POD pod   = va_arg(va, Code_POD);
		Code     entry = pcast(Code, pod);
		if ( ! entry) {
			log_failure("gen::"
						"def_class_body"
						": Provided an null entry");
			return InvalidCode;
		}
		switch (entry->Type)
		{
			GEN_AST_BODY_CLASS_UNALLOWED_TYPES_CASES:
				log_failure("gen::" "def_class_body" ": Entry type is not allowed: %s", code_debug_str(entry));
				return InvalidCode;

			default:
			break;
		}
		body_append(result, entry);
	}
	while (num--, num > 0);
	va_end(va);

	return result;
}

CodeBody def_class_body_arr( s32 num, Code* codes )
{
	def_body_code_array_start( def_class_body );

	CodeBody
	result       = (CodeBody) make_code();
	result->Type = CT_Function_Body;
	do
	{
		Code entry = *codes;
		codes++;
		if ( ! entry) {
			log_failure("gen::" "def_class_body" ": Provided an null entry");
			return InvalidCode;
		}
		switch (entry->Type)
		{
			GEN_AST_BODY_CLASS_UNALLOWED_TYPES_CASES:
				log_failure("gen::" "def_class_body" ": Entry type is not allowed: %s", code_debug_str(entry));
				return InvalidCode;

			default:
			break;
		}
		body_append(result, entry);
	}
	while (num--, num > 0);

	return result;
}

CodeDefineParams def_define_params( s32 num, ... )
{
	def_body_start( def_define_params );

	va_list va;
	va_start(va, num);

	Code_POD         pod   = va_arg(va, Code_POD);
	CodeDefineParams param = pcast( CodeDefineParams, pod );

	null_check( def_define_params, param );
	if ( param->Type != CT_Parameters_Define ) {
		log_failure( "gen::def_define_params: param %d is not a parameter for a preprocessor define", num - num + 1 );
		return InvalidCode;
	}

	CodeDefineParams result = (CodeDefineParams) code_duplicate(param);
	while ( -- num )
	{
		pod   = va_arg(va, Code_POD);
		param = pcast( CodeDefineParams, pod );
		if ( param->Type != CT_Parameters_Define ) {
			log_failure( "gen::def_define_params: param %d is not a parameter for a preprocessor define", num - num + 1 );
			return InvalidCode;
		}
		define_params_append(result, param );
	}
	va_end(va);

	return result;
}

CodeDefineParams def_define_params_arr( s32 num, CodeDefineParams* codes )
{
	def_body_code_array_start( def_define_params );

#	define check_current(current)                                                                                                                 \
	if ( current == nullptr ) {                                                                                                                   \
		log_failure("gen::def_define_params: Provide a null code in codes array");                                                                \
		return InvalidCode;                                                                                                                       \
	}                                                                                                                                             \
	if (current->Type != CT_Parameters_Define ) {                                                                                                 \
		log_failure("gen::def_define_params: Code in coes array is not of paramter for preprocessor define type - %s", code_debug_str(current) ); \
		return InvalidCode;                                                                                                                       \
	}
	CodeDefineParams current = (CodeDefineParams)code_duplicate(* codes);
	check_current(current);

	CodeDefineParams
	result            = (CodeDefineParams) make_code();
	result->Name      = current->Name;
	result->Type      = current->Type;
	while( codes++, current = * codes, num--, num > 0 ) {
		check_current(current);
		define_params_append(result, current );
	}
#	undef check_current

	return result;
}

CodeBody def_enum_body( s32 num, ... )
{
	def_body_start( def_enum_body );

	CodeBody
	result       = (CodeBody) make_code();
	result->Type = CT_Enum_Body;

	va_list va;
	va_start(va, num);
	do
	{
		Code_POD pod   = va_arg(va, Code_POD);
		Code     entry = pcast(Code, pod);
		if ( ! entry ) {
			log_failure("gen::def_enum_body: Provided a null entry");
			return InvalidCode;
		}
		if ( entry->Type != CT_Untyped && entry->Type != CT_Comment ) {
			log_failure("gen::def_enum_body: Entry type is not allowed - %s. Must be of untyped or comment type.", code_debug_str(entry) );
			return InvalidCode;
		}
		body_append(result, entry );
	}
	while ( num--, num > 0 );
	va_end(va);

	return (CodeBody) result;
}

CodeBody def_enum_body_arr( s32 num, Code* codes )
{
	def_body_code_array_start( def_enum_body );

	CodeBody
	result       = (CodeBody) make_code();
	result->Type = CT_Enum_Body;
	do
	{
		Code entry = *codes;
		if ( ! entry ) {
			log_failure("gen::def_enum_body: Provided a null entry");
			return InvalidCode;
		}
		if ( entry->Type != CT_Untyped && entry->Type != CT_Comment ) {
			log_failure("gen::def_enum_body: Entry type is not allowed: %s", code_debug_str(entry) );
			return InvalidCode;
		}
		body_append(result, entry );
	}
	while ( codes++, num--, num > 0 );

	return result;
}

CodeBody def_export_body( s32 num, ... )
{
	def_body_start( def_export_body );

	CodeBody
	result       = (CodeBody) make_code();
	result->Type = CT_Export_Body;

	va_list va;
	va_start(va, num);
	do
	{
		Code_POD pod = va_arg(va, Code_POD);
		Code     entry = pcast(Code, pod);
		if ( ! entry)
		{
			log_failure("gen::" "def_export_body" ": Provided an null entry");
			return InvalidCode;
		}
		switch (entry->Type)
		{
			GEN_AST_BODY_EXPORT_UNALLOWED_TYPES_CASES:
				log_failure("gen::" "def_export_body" ": Entry type is not allowed: %s", code_debug_str(entry));
				return InvalidCode;

			default:
			break;
		}
		body_append(result, entry);
	}
	while (num--, num > 0);
	va_end(va);

	return result;
}

CodeBody def_export_body_arr( s32 num, Code* codes )
{
	def_body_code_array_start( def_export_body );

	CodeBody
	result       = (CodeBody) make_code();
	result->Type = CT_Export_Body;
	do
	{
		Code entry = *codes;
		codes++;
		if ( ! entry) {
			log_failure("gen::" "def_export_body" ": Provided an null entry");
			return InvalidCode;
		}
		switch (entry->Type)
		{
			GEN_AST_BODY_EXPORT_UNALLOWED_TYPES_CASES:
				log_failure("gen::" "def_export_body" ": Entry type is not allowed: %s", code_debug_str(entry));
				return InvalidCode;

			default:
			break;
		}
		body_append(result, entry);
	}
	while (num--, num > 0);

	return result;
}

CodeBody def_extern_link_body( s32 num, ... )
{
	def_body_start( def_extern_linkage_body );

	CodeBody
	result       = (CodeBody) make_code();
	result->Type = CT_Extern_Linkage_Body;

	va_list va;
	va_start(va, num);
	do
	{
		Code_POD pod   = va_arg(va, Code_POD);
		Code     entry = pcast(Code, pod);
		if ( ! entry) {
			log_failure("gen::" "def_extern_linkage_body" ": Provided an null entry");
			return InvalidCode;
		}
		switch (entry->Type)
		{
			GEN_AST_BODY_EXTERN_LINKAGE_UNALLOWED_TYPES_CASES:
				log_failure("gen::" "def_extern_linkage_body" ": Entry type is not allowed: %s", code_debug_str(entry));
				return InvalidCode;

			default:
			break;
		}
		body_append(result, entry);
	}
	while (num--, num > 0);
	va_end(va);

	return result;
}

CodeBody def_extern_link_body_arr( s32 num, Code* codes )
{
	def_body_code_array_start( def_extern_linkage_body );

	CodeBody
	result       = (CodeBody) make_code();
	result->Type = CT_Extern_Linkage_Body;
	do
	{
		Code entry = *codes;
		codes++;
		if (!entry)
		{
			log_failure("gen::" "def_extern_linkage_body" ": Provided an null entry");
			return InvalidCode;
		}
		switch (entry->Type)
		{
			GEN_AST_BODY_EXTERN_LINKAGE_UNALLOWED_TYPES_CASES:
				log_failure("gen::" "def_extern_linkage_body" ": Entry type is not allowed: %s", code_debug_str(entry));
				return InvalidCode;

			default:
			break;
		}
		body_append(result, entry);
	}
	while (num--, num > 0);

	return result;
}

CodeBody def_function_body( s32 num, ... )
{
	def_body_start( def_function_body );

	CodeBody
	result       = (CodeBody) make_code();
	result->Type = CT_Function_Body;

	va_list va;
	va_start(va, num);
	do
	{
		Code_POD pod   = va_arg(va, Code_POD);
		Code     entry = pcast(Code, pod);
		if ( ! entry) {
			log_failure("gen::" stringize(def_function_body) ": Provided an null entry");
			return InvalidCode;
		}
		switch (entry->Type)
		{
			GEN_AST_BODY_FUNCTION_UNALLOWED_TYPES_CASES:
				log_failure("gen::" stringize(def_function_body) ": Entry type is not allowed: %s", code_debug_str(entry));
				return InvalidCode;

			default:
			break;
		}
		body_append(result, entry);
	}
	while (num--, num > 0);
	va_end(va);

	return result;
}

CodeBody def_function_body_arr( s32 num, Code* codes )
{
	def_body_code_array_start( def_function_body );

	CodeBody
	result       = (CodeBody) make_code();
	result->Type = CT_Function_Body;
	do
	{
		Code entry = *codes;
		codes++;
		if (!entry) {
			log_failure("gen::" "def_function_body" ": Provided an null entry");
			return InvalidCode;
		}
		switch (entry->Type)
		{
			GEN_AST_BODY_FUNCTION_UNALLOWED_TYPES_CASES:
				log_failure("gen::" "def_function_body" ": Entry type is not allowed: %s", code_debug_str(entry));
				return InvalidCode;

			default:
			break;
		}
		body_append(result, entry);
	}
	while (num--, num > 0);

	return result;
}

CodeBody def_global_body( s32 num, ... )
{
	def_body_start( def_global_body );

	CodeBody
	result       = (CodeBody) make_code();
	result->Type = CT_Global_Body;

	va_list va;
	va_start(va, num);
	do
	{
		Code_POD pod   = va_arg(va, Code_POD);
		Code     entry = pcast(Code, pod);
		if ( ! entry) {
			log_failure("gen::" "def_global_body" ": Provided an null entry");
			return InvalidCode;
		}
		switch (entry->Type)
		{
			case CT_Global_Body:
				// result.body_append( entry.code_cast<CodeBody>() ) ;
				body_append_body( result, cast(CodeBody, entry) );
				continue;

			GEN_AST_BODY_GLOBAL_UNALLOWED_TYPES_CASES:
				log_failure("gen::" "def_global_body" ": Entry type is not allowed: %s", code_debug_str(entry));
				return InvalidCode;

			default:
			break;
		}
		body_append(result, entry);
	}
	while (num--, num > 0);
	va_end(va);

	return result;
}

CodeBody def_global_body_arr( s32 num, Code* codes )
{
	def_body_code_array_start( def_global_body );

	CodeBody
	result       = (CodeBody) make_code();
	result->Type = CT_Global_Body;
	do
	{
		Code entry = *codes;
		codes++;
		if ( ! entry) {
			log_failure("gen::" "def_global_body" ": Provided an null entry");
			return InvalidCode;
		}
		switch (entry->Type)
		{
			case CT_Global_Body:
				body_append_body(result, cast(CodeBody, entry) );
				continue;

			GEN_AST_BODY_GLOBAL_UNALLOWED_TYPES_CASES:
				log_failure("gen::" "def_global_body" ": Entry type is not allowed: %s", code_debug_str(entry));
				return InvalidCode;

			default:
			break;
		}

		body_append(result, entry);
	}
	while (num--, num > 0);

	return result;
}

CodeBody def_namespace_body( s32 num, ... )
{
	def_body_start( def_namespace_body );

	CodeBody
	result       = (CodeBody) make_code();
	result->Type = CT_Namespace_Body;

	va_list va;
	va_start(va, num);
	do
	{
		Code_POD pod   = va_arg(va, Code_POD);
		Code     entry = pcast(Code, pod);
		if ( ! entry) {
			log_failure("gen::" "def_namespace_body" ": Provided an null entry");
			return InvalidCode;
		}
		switch (entry->Type)
		{
			GEN_AST_BODY_NAMESPACE_UNALLOWED_TYPES_CASES:
				log_failure("gen::" "def_namespace_body" ": Entry type is not allowed: %s", code_debug_str(entry));
				return InvalidCode;

			default:
			break;
		}
		body_append(result, entry);
	}
	while (num--, num > 0);
	va_end(va);

	return result;
}

CodeBody def_namespace_body_arr( s32 num, Code* codes )
{
	def_body_code_array_start( def_namespace_body );

	CodeBody
	result       = (CodeBody) make_code();
	result->Type = CT_Global_Body;
	do
	{
		Code entry = *codes;
		codes++;
		if ( ! entry) {
			log_failure("gen::" "def_namespace_body" ": Provided an null entry");
			return InvalidCode;
		}
		switch (entry->Type)
		{
			GEN_AST_BODY_NAMESPACE_UNALLOWED_TYPES_CASES:
				log_failure("gen::" "def_namespace_body" ": Entry type is not allowed: %s", code_debug_str(entry) );
				return InvalidCode;

			default: break;
		}
		body_append(result, entry);
	}
	while (num--, num > 0);

	return result;
}

CodeParams def_params( s32 num, ... )
{
	def_body_start( def_params );

	va_list va;
	va_start(va, num);

	Code_POD  pod   = va_arg(va, Code_POD);
	CodeParams param = pcast( CodeParams, pod );

	null_check( def_params, param );
	if ( param->Type != CT_Parameters ) {
		log_failure( "gen::def_params: param %d is not a Parameters", num - num + 1 );
		return InvalidCode;
	}

	CodeParams result = (CodeParams) code_duplicate(param);
	while ( -- num )
	{
		pod   = va_arg(va, Code_POD);
		param = pcast( CodeParams, pod );
		if ( param->Type != CT_Parameters ) {
			log_failure( "gen::def_params: param %d is not a Parameters", num - num + 1 );
			return InvalidCode;
		}
		params_append(result, param );
	}
	va_end(va);

	return result;
}

CodeParams def_params_arr( s32 num, CodeParams* codes )
{
	def_body_code_array_start( def_params );

#	define check_current(current)                                                                                      \
	if ( current == nullptr ) {                                                                                        \
		log_failure("gen::def_params: Provide a null code in codes array");                                            \
		return InvalidCode;                                                                                            \
	}                                                                                                                  \
	if (current->Type != CT_Parameters ) {                                                                             \
		log_failure("gen::def_params: Code in coes array is not of paramter type - %s", code_debug_str(current) );     \
		return InvalidCode;                                                                                            \
	}
	CodeParams current = (CodeParams)code_duplicate(* codes);
	check_current(current);

	CodeParams
	result            = (CodeParams) make_code();
	result->Name      = current->Name;
	result->Type      = current->Type;
	result->ValueType = current->ValueType;
	while( codes++, current = * codes, num--, num > 0 ) {
		check_current(current);
		params_append(result, current );
	}
#	undef check_current

	return result;
}

CodeSpecifiers def_specifiers( s32 num, ... )
{
	if ( num <= 0 ) {
		log_failure("gen::def_specifiers: num cannot be zero or less");
		return InvalidCode;
	}
	if ( num > AST_ArrSpecs_Cap ) {
		log_failure("gen::def_specifiers: num of speciifers to define AST larger than AST specicifier capacity - %d", num);
		return InvalidCode;
	}
	CodeSpecifiers
	result       = (CodeSpecifiers) make_code();
	result->Type = CT_Specifiers;

	va_list va;
	va_start(va, num);
	do {
		Specifier type = (Specifier)va_arg(va, int);
		specifiers_append(result, type );
	}
	while ( --num, num );
	va_end(va);

	return result;
}

CodeSpecifiers def_specifiers_arr( s32 num, Specifier* specs )
{
	if ( num <= 0 ) {
		log_failure("gen::def_specifiers: num cannot be zero or less");
		return InvalidCode;
	}
	if ( num > AST_ArrSpecs_Cap ) {
		log_failure("gen::def_specifiers: num of speciifers to define AST larger than AST specicifier capacity - %d", num);
		return InvalidCode;
	}
	CodeSpecifiers
	result       = (CodeSpecifiers) make_code();
	result->Type = CT_Specifiers;

	s32 idx = 0;
	do {
		specifiers_append(result, specs[idx] );
		idx++;
	}
	while ( --num, num );

	return result;
}

CodeBody def_struct_body( s32 num, ... )
{
	def_body_start( def_struct_body );

	CodeBody
	result       = (CodeBody) make_code();
	result->Type = CT_Struct_Body;

	va_list va;
	va_start(va, num);
	do
	{
		Code_POD pod   = va_arg(va, Code_POD);
		Code     entry = pcast(Code, pod);
		if ( ! entry) {
			log_failure("gen::" "def_struct_body" ": Provided an null entry");
			return InvalidCode;
		}
		switch (entry->Type)
		{
			GEN_AST_BODY_STRUCT_UNALLOWED_TYPES_CASES:
				log_failure("gen::" "def_struct_body" ": Entry type is not allowed: %s", code_debug_str(entry));
				return InvalidCode;

			default:
			break;
		}
		body_append(result, entry);
	}
	while (num--, num > 0);
	va_end(va);

	return result;
}

CodeBody def_struct_body_arr( s32 num, Code* codes )
{
	def_body_code_array_start( def_struct_body );

	CodeBody
	result       = (CodeBody) make_code();
	result->Type = CT_Struct_Body;
	do
	{
		Code entry = *codes;
		codes++;
		if ( ! entry) {
			log_failure("gen::" "def_struct_body" ": Provided an null entry");
			return InvalidCode;
		}
		switch (entry->Type)
		{
			GEN_AST_BODY_STRUCT_UNALLOWED_TYPES_CASES:
				log_failure("gen::" "def_struct_body" ": Entry type is not allowed: %s", code_debug_str(entry) );
				return InvalidCode;

			default:
			break;
		}
		body_append(result, entry);
	}
	while (num--, num > 0);

	return result;
}

CodeBody def_union_body( s32 num, ... )
{
	def_body_start( def_union_body );

	CodeBody
	result       = (CodeBody) make_code();
	result->Type = CT_Union_Body;

	va_list va;
	va_start(va, num);
	do
	{
		Code_POD pod   = va_arg(va, Code_POD);
		Code     entry = pcast( Code, pod );
		if ( ! entry ) {
			log_failure("gen::def_union_body: Provided a null entry");
			return InvalidCode;
		}
		if ( entry->Type != CT_Untyped && entry->Type != CT_Comment ) {
			log_failure("gen::def_union_body: Entry type is not allowed - %s. Must be of untyped or comment type.", code_debug_str(entry) );
			return InvalidCode;
		}
		body_append(result, entry );
	}
	while ( num--, num > 0 );
	va_end(va);

	return result;
}

CodeBody def_union_body_arr( s32 num, Code* codes )
{
	def_body_code_array_start( def_union_body );

	CodeBody
	result       = (CodeBody) make_code();
	result->Type = CT_Union_Body;
	do
	{
		Code entry = *codes;
		if ( ! entry ) {
			log_failure("gen::def_union_body: Provided a null entry");
			return InvalidCode;
		}
		if ( entry->Type != CT_Untyped && entry->Type != CT_Comment ) {
			log_failure("gen::def_union_body: Entry type is not allowed: %s", code_debug_str(entry) );
			return InvalidCode;
		}
		body_append(result, entry );
	}
	while ( codes++, num--, num > 0 );

	return (CodeBody) result;
}

#	undef name_check
#	undef null_check
#	undef def_body_start
#	undef def_body_code_array_start

#pragma endregion Upfront
