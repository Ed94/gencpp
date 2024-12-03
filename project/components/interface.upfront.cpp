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

OpValidateResult operator__validate( Operator op, CodeParam params_code, CodeTypename ret_type, CodeSpecifiers specifier )
{
	if ( op == Op_Invalid )
	{
		log_failure("gen::def_operator: op cannot be invalid");
		return OpValResult_Fail;
	}

#pragma region Helper Macros
#	define check_params()                                                                                 \
	if ( ! params_code )                                                                                  \
	{                                                                                                     \
		log_failure("gen::def_operator: params is null and operator%s requires it", to_str(op));          \
		return OpValResult_Fail;                                                                          \
	}                                                                                                     \
	if ( params_code->Type != CT_Parameters )                                                             \
	{                                                                                                     \
		log_failure("gen::def_operator: params is not of Parameters type - %s", debug_str(params_code));  \
		return OpValResult_Fail;                                                                          \
	}

#	define check_param_eq_ret()                                                                     \
	if ( ! is_member_symbol && ! is_equal(params_code->ValueType, ret_type) )                       \
	{                                                                                               \
		log_failure("gen::def_operator: operator%s requires first parameter to equal return type\n" \
			"param types: %s\n"                                                                     \
			"return type: %s",                                                                      \
			to_str(op).Ptr,                                                                         \
			debug_str(params_code),                                                                 \
			debug_str(ret_type)                                                                     \
		);                                                                                          \
		return OpValResult_Fail;                                                                    \
	}
#pragma endregion Helper Macros

	if ( ! ret_type )
	{
		log_failure("gen::def_operator: ret_type is null but is required by operator%s", to_str(op));
	}

	if ( ret_type->Type != CT_Typename )
	{
		log_failure("gen::def_operator: ret_type is not of typename type - %s", debug_str(ret_type));
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
					"operator%s does not support non-member definition (more than one parameter provided) - %s",
					to_str(op),
					debug_str(params_code)
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
				log_failure("gen::def_operator: operator%s may not be defined with more than two parametes - param count; %d\n%s"
					, to_str(op)
					, params_code->NumEntries
					, debug_str(params_code)
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
					log_failure("gen::def_operator: operator%s params code provided is not of Parameters type - %s"
						, to_str(op)
						, debug_str(params_code)
					);
					return OpValResult_Fail;
				}

				switch ( params_code->NumEntries )
				{
					case 1:
						if ( is_equal(params_code->ValueType, t_int ) )
							is_member_symbol = true;

						else
							check_param_eq_ret();
						break;

					case 2:
						check_param_eq_ret();

						if ( ! is_equal(get(params_code, 1), t_int ) )
						{
							log_failure("gen::def_operator: "
								"operator%s requires second parameter of non-member definition to be int for post-decrement",
								to_str(op)
							);
							return OpValResult_Fail;
						}
						break;

					default:
						log_failure("gen::def_operator: operator%s recieved unexpected number of parameters recived %d instead of 0-2"
							, to_str(op)
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
					log_failure("gen::def_operator: params is not of Parameters type - %s", debug_str(params_code));
					return OpValResult_Fail;
				}

				if ( is_equal(params_code->ValueType, ret_type ) )
				{
					log_failure("gen::def_operator: "
						"operator%s is non-member symbol yet first paramter does not equal return type\n"
						"param type: %s\n"
						"return type: %s\n"
						, debug_str(params_code)
						, debug_str(ret_type)
					);
					return OpValResult_Fail;
				}

				if ( params_code->NumEntries > 1 )
				{
					log_failure("gen::def_operator: operator%s may not have more than one parameter - param count: %d"
						, to_str(op)
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
				log_failure( "gen::def_operator: return type is not a boolean - %s", debug_str(params_code) );
				return OpValidateResult::Fail;
			}
		#endif

			if ( ! params_code )
				is_member_symbol = true;

			else
			{
				if ( params_code->Type != CT_Parameters )
				{
					log_failure( "gen::def_operator: params is not of Parameters type - %s", debug_str(params_code) );
					return OpValResult_Fail;
				}

				if ( params_code->NumEntries > 1 )
				{
					log_failure(
					    "gen::def_operator: operator%s may not have more than one parameter - param count: %d",
					    to_str( op ),
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
					if ( ! is_equal(params_code->ValueType, ret_type ) )
					{
						log_failure("gen::def_operator: "
							"operator%s is non-member symbol yet first paramter does not equal return type\n"
							"param type: %s\n"
							"return type: %s\n"
							, debug_str(params_code)
							, debug_str(ret_type)
						);
						return OpValResult_Fail;
					}
					break;

				default:
					log_failure("gen::def_operator: operator%s recieved unexpected number of paramters recived %d instead of 0-2"
						, to_str(op)
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
					log_failure("gen::def_operator: params is not of Parameters type - %s", debug_str(params_code));
					return OpValResult_Fail;
				}

				if ( params_code->NumEntries != 1 )
				{
					log_failure("gen::def_operator: operator%s recieved unexpected number of paramters recived %d instead of 0-1"
						, to_str(op)
						, params_code->NumEntries
					);
					return OpValResult_Fail;
				}
			}

			if ( ! is_equal(ret_type, t_bool ))
			{
				log_failure("gen::def_operator: operator%s return type must be of type bool - %s"
					, to_str(op)
					, debug_str(ret_type)
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
					log_failure("gen::def_operator: operator%s recieved unexpected number of paramters recived %d instead of 1-2"
						, to_str(op)
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
				log_failure("gen::def_operator: operator%s recieved unexpected number of paramters recived %d instead of 0-1"
					, to_str(op)
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
				log_failure("gen::def_operator: operator%s expects no paramters - %s", to_str(op), debug_str(params_code));
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


#pragma region Helper Marcos
// This snippet is used in nearly all the functions.
#define name_check( Context_, Name_ )                                                                 \
{                                                                                                     \
	if ( Name_.Len <= 0 )                                                                             \
	{                                                                                                 \
		log_failure( "gen::" stringize(Context_) ": Invalid name length provided - %d",  Name_.Len ); \
		return InvalidCode;                                                                           \
	}                                                                                                 \
																									  \
	if ( Name_.Ptr == nullptr )                                                                       \
	{                                                                                                 \
		log_failure( "gen::" stringize(Context_) ": name is null" );                                  \
		return InvalidCode;                                                                           \
	}                                                                                                 \
}

#define null_check( Context_, Code_ )                                                         \
	if ( ! Code_ )                                                                            \
	{                                                                                         \
		log_failure( "gen::" stringize(Context_) ": " stringize(Code_) " provided is null" ); \
		return InvalidCode;                                                                   \
	}

#define null_or_invalid_check( Context_, Code_ )                                                \
{                                                                                               \
	if ( ! Code_ )                                                                              \
	{                                                                                           \
		log_failure( "gen::" stringize(Context_) ": " stringize(Code_) " provided is null" );   \
		return InvalidCode;                                                                     \
	}                                                                                           \
																								\
	if ( Code_->is_invalid() )                                                                  \
	{                                                                                           \
		log_failure("gen::" stringize(Context_) ": " stringize(Code_) " provided is invalid" ); \
		return InvalidCode;                                                                     \
	}                                                                                           \
}

#define not_implemented( Context_ )                             \
	log_failure( "gen::%s: This function is not implemented" ); \
	return InvalidCode;
#pragma endregion Helper Marcos


/*
The implementaiton of the upfront constructors involves doing three things:
* Validate the arguments given to construct the intended type of AST is valid.
* Construct said AST type.
* Lock the AST (set to readonly) and return the valid object.

If any of the validation fails, it triggers a call to log_failure with as much info the give the user so that they can hopefully
identify the issue without having to debug too much (at least they can debug though...)

The largest of the functions is related to operator overload definitions.
The library validates a good protion of their form and thus the argument processing for is quite a bit.
*/
CodeAttributes def_attributes( StrC content )
{
	if ( content.Len <= 0 || content.Ptr == nullptr )
	{
		log_failure( "gen::def_attributes: Invalid attributes provided" );
		return InvalidCode;
	}

	Code
	result          = make_code();
	result->Type    = CT_PlatformAttributes;
	result->Name    = get_cached_string( content );
	result->Content = result->Name;

	return (CodeAttributes) result;
}

CodeComment def_comment( StrC content )
{
	if ( content.Len <= 0 || content.Ptr == nullptr )
	{
		log_failure( "gen::def_comment: Invalid comment provided:" );
		return InvalidCode;
	}

	static char line[ MaxCommentLineLength ];

	String      cmt_formatted = string_make_reserve( GlobalAllocator, kilobytes(1) );
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

		str_copy( line, scanner, length );
		append_fmt(& cmt_formatted, "//%.*s", length, line );
		mem_set( line, 0, MaxCommentLineLength );

		scanner += length;
	}
	while ( scanner <= end );

	if ( * back(& cmt_formatted) != '\n' )
		append( & cmt_formatted, "\n" );

	Code
	result          = make_code();
	result->Type    = CT_Comment;
	result->Name    = get_cached_string( { length(cmt_formatted), cmt_formatted } );
	result->Content = result->Name;

	free(& cmt_formatted);

	return (CodeComment) result;
}

CodeConstructor def_constructor( Opts_def_constructor p )
{
	CodeParam params           = p.params;
	Code      initializer_list = p.initializer_list;
	Code      body             = p.body;

	if ( params && params->Type != CT_Parameters )
	{
		log_failure("gen::def_constructor: params must be of Parameters type - %s", debug_str(params));
		return InvalidCode;
	}

	CodeConstructor
	result = (CodeConstructor) make_code();

	if ( params )
	{
		result->Params = params;
	}

	if ( initializer_list )
	{
		result->InitializerList = initializer_list;
	}

	if ( body )
	{
		switch ( body->Type )
		{
			case CT_Function_Body:
			case CT_Untyped:
			break;

			default:
				log_failure("gen::def_constructor: body must be either of Function_Body or Untyped type - %s", debug_str(body));
				return InvalidCode;
		}

		result->Type = CT_Constructor;
		result->Body = body;
	}
	else
	{
		result->Type = CT_Constructor_Fwd;
	}

	return result;
}

CodeClass def_class( StrC name, Opts_def_struct p )
{
	name_check( def_class, name );
	
	Code           body           = p.body;
	CodeTypename   parent         = p.parent;
	AccessSpec     parent_access  = p.parent_access;
	CodeAttributes attributes     = p.attributes;
	ModuleFlag     mflags         = p.mflags;
	CodeTypename*  interfaces     = p.interfaces;
	s32            num_interfaces = p.num_interfaces;

	if ( attributes && attributes->Type != CT_PlatformAttributes )
	{
		log_failure( "gen::def_class: attributes was not a 'PlatformAttributes' type: %s", debug_str(attributes) );
		return InvalidCode;
	}

	if ( parent && ( parent->Type != CT_Class && parent->Type != CT_Struct && parent->Type != CT_Typename && parent->Type != CT_Untyped ) )
	{
		log_failure( "gen::def_class: parent provided is not type 'Class', 'Struct', 'Typeanme', or 'Untyped': %s", debug_str(parent) );
		return InvalidCode;
	}

	CodeClass
	result              = (CodeClass) make_code();
	result->Name        = get_cached_string( name );
	result->ModuleFlags = mflags;

	if ( body )
	{
		switch ( body->Type )
		{
			case CT_Class_Body:
			case CT_Untyped:
			break;

			default:
				log_failure("gen::def_class: body must be either of Class_Body or Untyped type - %s", debug_str(body));
				return InvalidCode;
		}

		result->Type = CT_Class;
		result->Body = body;
		result->Body->Parent = result; // TODO(Ed): Review this?
	}
	else
	{
		result->Type = CT_Class_Fwd;
	}

	if ( attributes )
		result->Attributes = attributes;

	if ( parent )
	{
		result->ParentAccess = parent_access;
		result->ParentType   = parent;
	}

	if ( interfaces )
	{
		for (s32 idx = 0; idx < num_interfaces; idx++ )
		{
			add_interface(result, interfaces[idx] );
		}
	}

	return result;
}

CodeDefine def_define( StrC name, StrC content )
{
	name_check( def_define, name );

	// Defines can be empty definitions
#if 0
	if ( content.Len <= 0 || content.Ptr == nullptr )
	{
		log_failure( "gen::def_define: Invalid value provided" );
		return InvalidCode;
	}
#endif

	CodeDefine
	result          = (CodeDefine) make_code();
	result->Type    = CT_Preprocess_Define;
	result->Name    = get_cached_string( name );
	if ( content.Len <= 0 || content.Ptr == nullptr )
	{
		result->Content = get_cached_string( txt("") );
	}
	else
		result->Content = get_cached_string( content );

	return result;
}

CodeDestructor def_destructor( Opts_def_destructor p )
{
	Code           body       = p.body;
	CodeSpecifiers specifiers = p.specifiers;
	
	if ( specifiers && specifiers->Type != CT_Specifiers )
	{
		log_failure( "gen::def_destructor: specifiers was not a 'Specifiers' type: %s", debug_str(specifiers) );
		return InvalidCode;
	}
	
	CodeDestructor result = (CodeDestructor) make_code();

	if ( specifiers )
		result->Specs = specifiers;

	if ( body )
	{
		switch ( body->Type )
		{
			case CT_Function_Body:
			case CT_Untyped:
			break;

			default:
				log_failure("gen::def_destructor: body must be either of Function_Body or Untyped type - %s", debug_str(body));
				return InvalidCode;
		}

		result->Type = CT_Destructor;
		result->Body = body;
	}
	else
	{
		result->Type = CT_Destructor_Fwd;
	}

	return result;
}

CodeEnum def_enum( StrC name, Opts_def_enum p )
{
	Code           body       = p.body;
	CodeTypename   type       = p.type;
	EnumT          specifier  = p.specifier;
	CodeAttributes attributes = p.attributes;
	ModuleFlag     mflags     = p.mflags;

	name_check( def_enum, name );

	if ( type && type->Type != CT_Typename )
	{
		log_failure( "gen::def_enum: enum underlying type provided was not of type Typename: %s", debug_str(type) );
		return InvalidCode;
	}

	if ( attributes && attributes->Type != CT_PlatformAttributes )
	{
		log_failure( "gen::def_enum: attributes was not a 'PlatformAttributes' type: %s", debug_str(attributes) );
		return InvalidCode;
	}

	CodeEnum
	result              = (CodeEnum) make_code();
	result->Name        = get_cached_string( name );
	result->ModuleFlags = mflags;

	if ( body )
	{
		switch ( body->Type )
		{
			case CT_Enum_Body:
			case CT_Untyped:
			break;

			default:
				log_failure( "gen::def_enum: body must be of Enum_Body or Untyped type %s", debug_str(body));
				return InvalidCode;
		}

		result->Type = specifier == EnumDecl_Class ?
			CT_Enum_Class : CT_Enum;

		result->Body = body;
	}
	else
	{
		result->Type = specifier == EnumDecl_Class ?
			CT_Enum_Class_Fwd : CT_Enum_Fwd;
	}

	if ( attributes )
		result->Attributes = attributes;

	if ( type )
	{
		result->UnderlyingType = type;
	}
	else if ( result->Type != CT_Enum_Class_Fwd && result->Type != CT_Enum_Fwd )
	{
		log_failure( "gen::def_enum: enum forward declaration must have an underlying type" );
		return InvalidCode;
	}

	return result;
}

CodeExec def_execution( StrC content )
{
	if ( content.Len <= 0 || content.Ptr == nullptr )
	{
		log_failure( "gen::def_execution: Invalid execution provided" );
		return InvalidCode;
	}

	Code
	result          = make_code();
	result->Type    = CT_Execution;
	result->Name    = get_cached_string( content );
	result->Content = result->Name;

	return (CodeExec) result;
}

CodeExtern def_extern_link( StrC name, Code body )
{
	name_check( def_extern_linkage, name );
	null_check( def_extern_linkage, body );

	if ( body->Type != CT_Extern_Linkage_Body && body->Type != CT_Untyped )
	{
		log_failure("gen::def_extern_linkage: body is not of extern_linkage or untyped type %s", debug_str(body));
		return InvalidCode;
	}

	CodeExtern
	result        = (CodeExtern)make_code();
	result->Type  = CT_Extern_Linkage;
	result->Name  = get_cached_string( name );
	result->Body  = body;

	return (CodeExtern) result;
}

CodeFriend def_friend( Code declaration )
{
	null_check( def_friend, declaration );

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
			log_failure("gen::def_friend: requires declartion to have class, function, operator, or struct - %s", debug_str(declaration));
			return InvalidCode;
	}

	CodeFriend
	result       = (CodeFriend) make_code();
	result->Type = CT_Friend;

	result->Declaration = declaration;

	return result;
}

CodeFn def_function( StrC name, Opts_def_function p )
{
	CodeParam       params     = p.params;
	CodeTypename    ret_type   = p.ret_type;
	Code            body       = p.body;
	CodeSpecifiers  specifiers = p.specs;
	CodeAttributes  attributes = p.attrs;
	ModuleFlag      mflags     = p.mflags;

	name_check( def_function, name );

	if ( params && params->Type != CT_Parameters )
	{
		log_failure( "gen::def_function: params was not a `Parameters` type: %s", debug_str(params) );
		return InvalidCode;
	}

	if ( ret_type && ret_type->Type != CT_Typename )
	{
		log_failure( "gen::def_function: ret_type was not a Typename: %s", debug_str(ret_type) );
		return InvalidCode;
	}

	if ( specifiers && specifiers->Type != CT_Specifiers )
	{
		log_failure( "gen::def_function: specifiers was not a `Specifiers` type: %s", debug_str(specifiers) );
		return InvalidCode;
	}

	if ( attributes && attributes->Type != CT_PlatformAttributes )
	{
		log_failure( "gen::def_function: attributes was not a `PlatformAttributes` type: %s", debug_str(attributes) );
		return InvalidCode;
	}

	CodeFn
	result              = (CodeFn) make_code();
	result->Name        = get_cached_string( name );
	result->ModuleFlags = mflags;

	if ( body )
	{
		switch ( body->Type )
		{
			case CT_Function_Body:
			case CT_Execution:
			case CT_Untyped:
				break;

			default:
			{
				log_failure("gen::def_function: body must be either of Function_Body, Execution, or Untyped type. %s", debug_str(body));
				return InvalidCode;
			}
		}

		result->Type = CT_Function;
		result->Body = body;
	}
	else
	{
		result->Type = CT_Function_Fwd;
	}

	if ( attributes )
		result->Attributes = attributes;

	if ( specifiers )
		result->Specs = specifiers;

	if ( ret_type )
	{
		result->ReturnType = ret_type;
	}
	else
	{
		result->ReturnType = t_void;
	}

	if ( params )
		result->Params = params;

	return result;
}

CodeInclude def_include( StrC path, Opts_def_include p )
{
	if ( path.Len <= 0 || path.Ptr == nullptr )
	{
		log_failure( "gen::def_include: Invalid path provided - %d" );
		return InvalidCode;
	}

	StrC content = p.foreign ?
			to_str( str_fmt_buf( "<%.*s>", path.Len, path.Ptr ))
		:	to_str( str_fmt_buf( "\"%.*s\"", path.Len, path.Ptr ));

	Code
	result          = make_code();
	result->Type    = CT_Preprocess_Include;
	result->Name    = get_cached_string( content );
	result->Content = result->Name;

	return (CodeInclude) result;
}

CodeModule def_module( StrC name, Opts_def_module p )
{
	name_check( def_module, name );

	Code
	result              = make_code();
	result->Type        = CT_Module;
	result->Name        = get_cached_string( name );
	result->Content     = result->Name;
	result->ModuleFlags = p.mflags;

	return (CodeModule) result;
}

CodeNS def_namespace( StrC name, Code body, Opts_def_namespace p )
{
	name_check( def_namespace, name );
	null_check( def_namespace, body);
	
	if ( body && body->Type != CT_Namespace_Body && body->Type != CT_Untyped )
	{
		log_failure("gen::def_namespace: body is not of namespace or untyped type %s", debug_str(body));
		return InvalidCode;
	}

	CodeNS
	result              = (CodeNS) make_code();
	result->Type        = CT_Namespace;
	result->Name        = get_cached_string( name );
	result->ModuleFlags = p.mflags;
	result->Body        = body;
	return result;
}

CodeOperator def_operator( Operator op, StrC nspace, Opts_def_operator p )
{
	CodeParam       params_code = p.params;
	CodeTypename    ret_type    = p.ret_type;
	Code            body        = p.body;
	CodeSpecifiers  specifiers  = p.specifiers;
	CodeAttributes  attributes  = p.attributes;
	ModuleFlag      mflags      = p.mflags;

	if ( attributes && attributes->Type != CT_PlatformAttributes )
	{
		log_failure( "gen::def_operator: PlatformAttributes was provided but its not of attributes type: %s", debug_str(attributes) );
		return InvalidCode;
	}

	if ( specifiers && specifiers->Type != CT_Specifiers )
	{
		log_failure( "gen::def_operator: Specifiers was provided but its not of specifiers type: %s", debug_str(specifiers) );
		return InvalidCode;
	}

	OpValidateResult check_result = operator__validate( op, params_code, ret_type, specifiers );

	if ( check_result == OpValResult_Fail )
	{
		return InvalidCode;
	}

	char const* name = nullptr;

	StrC op_str = to_str( op );
	if ( nspace.Len > 0 )
		name = str_fmt_buf( "%.*soperator %.*s", nspace.Len, nspace.Ptr, op_str.Len, op_str.Ptr );
	else
		name = str_fmt_buf( "operator %.*s", op_str.Len, op_str.Ptr );
	CodeOperator
	result              = (CodeOperator) make_code();
	result->Name        = get_cached_string( { str_len(name), name } );
	result->ModuleFlags = mflags;
	result->Op          = op;

	if ( body )
	{
		switch ( body->Type )
		{
			case CT_Function_Body:
			case CT_Execution:
			case CT_Untyped:
				break;

			default:
			{
				log_failure("gen::def_operator: body must be either of Function_Body, Execution, or Untyped type. %s", debug_str(body));
				return InvalidCode;
			}
		}

		result->Type = check_result == OpValResult_Global ?
			CT_Operator : CT_Operator_Member;

		result->Body = body;
	}
	else
	{
		result->Type = check_result == OpValResult_Global ?
			CT_Operator_Fwd : CT_Operator_Member_Fwd;
	}

	if ( attributes )
		result->Attributes = attributes;

	if ( specifiers )
		result->Specs = specifiers;

	result->ReturnType = ret_type;

	if (params_code)
		result->Params = params_code;

	return result;
}

CodeOpCast def_operator_cast( CodeTypename type, Opts_def_operator_cast p )
{
	Code           body       = p.body;
	CodeSpecifiers const_spec = p.specs;

	null_check( def_operator_cast, type );

	if ( type->Type != CT_Typename )
	{
		log_failure( "gen::def_operator_cast: type is not a typename - %s", debug_str(type) );
		return InvalidCode;
	}

	CodeOpCast result = (CodeOpCast) make_code();

	if (body)
	{
		result->Type = CT_Operator_Cast;

		if ( body->Type != CT_Function_Body && body->Type != CT_Execution )
		{
			log_failure( "gen::def_operator_cast: body is not of function body or execution type - %s", debug_str(body) );
			return InvalidCode;
		}

		result->Body = body;
	}
	else
	{
		result->Type = CT_Operator_Cast_Fwd;
	}

	if ( const_spec )
	{
		result->Specs = const_spec;
	}

	result->ValueType = type;
	return result;
}

CodeParam def_param( CodeTypename type, StrC name, Opts_def_param p )
{
	name_check( def_param, name );
	null_check( def_param, type );

	if ( type->Type != CT_Typename )
	{
		log_failure( "gen::def_param: type is not a typename - %s", debug_str(type) );
		return InvalidCode;
	}

	if ( p.value && p.value->Type != CT_Untyped )
	{
		log_failure( "gen::def_param: value is not untyped - %s", debug_str(p.value) );
		return InvalidCode;
	}

	CodeParam
	result       = (CodeParam) make_code();
	result->Type = CT_Parameters;
	result->Name = get_cached_string( name );

	result->ValueType = type;

	if ( p.value )
		result->Value = p.value;

	result->NumEntries++;

	return result;
}

CodePragma def_pragma( StrC directive )
{
	if ( directive.Len <= 0 || directive.Ptr == nullptr )
	{
		log_failure( "gen::def_comment: Invalid comment provided:" );
		return InvalidCode;
	}

	CodePragma
	result          = (CodePragma) make_code();
	result->Type    = CT_Preprocess_Pragma;
	result->Content = get_cached_string( directive );

	return result;
}

CodePreprocessCond def_preprocess_cond( EPreprocessCond type, StrC expr )
{
	if ( expr.Len <= 0 || expr.Ptr == nullptr )
	{
		log_failure( "gen::def_comment: Invalid comment provided:" );
		return InvalidCode;
	}

	CodePreprocessCond
	result          = (CodePreprocessCond) make_code();
	result->Content = get_cached_string( expr );

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
	append(result, spec );

	return result;
}

CodeStruct def_struct( StrC name, Opts_def_struct p )
{
	Code           body           = p.body;
	CodeTypename   parent         = p.parent;
	AccessSpec     parent_access  = p.parent_access;
	CodeAttributes attributes     = p.attributes;
	ModuleFlag     mflags         = p.mflags;
	CodeTypename*  interfaces     = p.interfaces;
	s32            num_interfaces = p.num_interfaces;

	if ( attributes && attributes->Type != CT_PlatformAttributes )
	{
		log_failure( "gen::def_struct: attributes was not a `PlatformAttributes` type - %s", debug_str(attributes) );
		return InvalidCode;
	}

	if ( parent && parent->Type != CT_Typename )
	{
		log_failure( "gen::def_struct: parent was not a `Struct` type - %s", debug_str(parent) );
		return InvalidCode;
	}

	if ( body && body->Type != CT_Struct_Body )
	{
		log_failure( "gen::def_struct: body was not a Struct_Body type - %s", debug_str(body) );
		return InvalidCode;
	}

	CodeStruct
	result              = (CodeStruct) make_code();
	result->ModuleFlags = mflags;

	if ( name )
		result->Name = get_cached_string( name );

	if ( body )
	{
		result->Type = CT_Struct;
		result->Body = body;
	}
	else
	{
		result->Type = CT_Struct_Fwd;
	}

	if ( attributes )
		result->Attributes = attributes;

	if ( parent )
	{
		result->ParentAccess = parent_access;
		result->ParentType   = parent;
	}

	if ( interfaces )
	{
		for (s32 idx = 0; idx < num_interfaces; idx++ )
		{
			add_interface(result, interfaces[idx] );
		}
	}

	return result;
}

CodeTemplate def_template( CodeParam params, Code declaration, Opts_def_template p )
{
	null_check( def_template, declaration );

	if ( params && params->Type != CT_Parameters )
	{
		log_failure( "gen::def_template: params is not of parameters type - %s", debug_str(params) );
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
			log_failure( "gen::def_template: declaration is not of class, function, struct, variable, or using type - %s", debug_str(declaration) );
	}

	CodeTemplate
	result              = (CodeTemplate) make_code();
	result->Type        = CT_Template;
	result->ModuleFlags = p.mflags;
	result->Params      = params;
	result->Declaration = declaration;
	return result;
}

CodeTypename def_type( StrC name, Opts_def_type p )
{
	name_check( def_type, name );
	
	Code           arrayexpr  = p.arrayexpr;
	CodeSpecifiers specifiers = p.specifiers;
	CodeAttributes attributes = p.attributes;

	if ( attributes && attributes->Type != CT_PlatformAttributes )
	{
		log_failure( "gen::def_type: attributes is not of attributes type - %s", debug_str(attributes) );
		return InvalidCode;
	}

	if ( specifiers && specifiers->Type != CT_Specifiers )
	{
		log_failure( "gen::def_type: specifiers is not of specifiers type - %s", debug_str(specifiers) );
		return InvalidCode;
	}

	if ( arrayexpr && arrayexpr->Type != CT_Untyped )
	{
		log_failure( "gen::def_type: arrayexpr is not of untyped type - %s", debug_str(arrayexpr) );
		return InvalidCode;
	}

	CodeTypename
	result       = (CodeTypename) make_code();
	result->Name = get_cached_string( name );
	result->Type = CT_Typename;

	if ( attributes )
		result->Attributes = attributes;

	if ( specifiers )
		result->Specs = specifiers;

	if ( arrayexpr )
		result->ArrExpr = arrayexpr;

	return result;
}

CodeTypedef def_typedef( StrC name, Code type, Opts_def_typedef p )
{
	null_check( def_typedef, type );

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
			log_failure( "gen::def_typedef: type was not a Class, Enum, Function Forward, Struct, Typename, or Union - %s", debug_str(type) );
			return InvalidCode;
	}

	if ( p.attributes && p.attributes->Type != CT_PlatformAttributes )
	{
		log_failure( "gen::def_typedef: attributes was not a PlatformAttributes - %s", debug_str(p.attributes) );
		return InvalidCode;
	}

	// Registering the type.
	Code registered_type = def_type( name );

	if ( ! registered_type )
	{
		log_failure( "gen::def_typedef: failed to register type" );
		return InvalidCode;
	}

	CodeTypedef
	result              = (CodeTypedef) make_code();
	result->Type        = CT_Typedef;
	result->ModuleFlags = p.mflags;

	result->UnderlyingType = type;

	if ( name.Len <= 0  )
	{
		if (type->Type != CT_Untyped)
		{
			log_failure( "gen::def_typedef: name was empty and type was not untyped (indicating its a function typedef) - %s", debug_str(type) );
			return InvalidCode;
		}

		result->Name       = get_cached_string( type->Name );
		result->IsFunction = true;
	}
	else
	{
		result->Name       = get_cached_string( name );
		result->IsFunction = false;
	}

	return result;
}

CodeUnion def_union( StrC name, Code body, Opts_def_union p )
{
	null_check( def_union, body );

	if ( body->Type != CT_Union_Body )
	{
		log_failure( "gen::def_union: body was not a Union_Body type - %s", debug_str(body) );
		return InvalidCode;
	}

	if ( p.attributes && p.attributes->Type != CT_PlatformAttributes )
	{
		log_failure( "gen::def_union: attributes was not a PlatformAttributes type - %s", debug_str(p.attributes) );
		return InvalidCode;
	}

	CodeUnion
	result              = (CodeUnion) make_code();
	result->ModuleFlags = p.mflags;
	result->Type        = CT_Union;

	if ( name.Ptr )
		result->Name = get_cached_string( name );

	result->Body = body;

	if ( p.attributes )
		result->Attributes = p.attributes;

	return result;
}

CodeUsing def_using( StrC name, Code type, Opts_def_using p )
{
	name_check( def_using, name );
	null_check( def_using, type );

	Code register_type = def_type( name );

	if ( ! register_type )
	{
		log_failure( "gen::def_using: failed to register type" );
		return InvalidCode;
	}

	if ( p.attributes && p.attributes->Type != CT_PlatformAttributes )
	{
		log_failure( "gen::def_using: attributes was not a PlatformAttributes type - %s", debug_str(p.attributes) );
		return InvalidCode;
	}

	CodeUsing
	result              = (CodeUsing) make_code();
	result->Name        = get_cached_string( name );
	result->ModuleFlags = p.mflags;
	result->Type        = CT_Using;

	result->UnderlyingType = type;

	if ( p.attributes )
		result->Attributes = p.attributes;

	return result;
}

CodeUsing def_using_namespace( StrC name )
{
	name_check( def_using_namespace, name );

	Code
	result          = make_code();
	result->Name    = get_cached_string( name );
	result->Content = result->Name;
	result->Type    = CT_Using_Namespace;

	return (CodeUsing) result;
}

CodeVar def_variable( CodeTypename type, StrC name, Code value, Opts_def_variable p )
{
	name_check( def_variable, name );
	null_check( def_variable, type );

	if ( p.attributes && p.attributes->Type != CT_PlatformAttributes )
	{
		log_failure( "gen::def_variable: attributes was not a `PlatformAttributes` type - %s", debug_str(p.attributes) );
		return InvalidCode;
	}

	if ( p.specifiers && p.specifiers->Type != CT_Specifiers )
	{
		log_failure( "gen::def_variable: specifiers was not a `Specifiers` type - %s", debug_str(p.specifiers) );
		return InvalidCode;
	}

	if ( type->Type != CT_Typename )
	{
		log_failure( "gen::def_variable: type was not a Typename - %s", debug_str(type) );
		return InvalidCode;
	}

	if ( value && value->Type != CT_Untyped )
	{
		log_failure( "gen::def_variable: value was not a `Untyped` type - %s", debug_str(value) );
		return InvalidCode;
	}

	CodeVar
	result              = (CodeVar) make_code();
	result->Name        = get_cached_string( name );
	result->Type        = CT_Variable;
	result->ModuleFlags = p.mflags;

	result->ValueType = type;

	if ( p.attributes )
		result->Attributes = p.attributes;

	if ( p.specifiers )
		result->Specs = p.specifiers;

	if ( value )
		result->Value = value;

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

		if (!entry)
		{
			log_failure("gen::"
						"def_class_body"
						": Provided an null entry");
			return InvalidCode;
		}

		switch (entry->Type)
		{
			GEN_AST_BODY_CLASS_UNALLOWED_TYPES
				log_failure("gen::" "def_class_body" ": Entry type is not allowed: %s", debug_str(entry));
				return InvalidCode;

			default:
			break;
		}

		append(result, entry);
	}
	while (num--, num > 0);
	va_end(va);

	return result;
}

CodeBody def_class_body( s32 num, Code* codes )
{
	def_body_code_array_start( def_class_body );

	CodeBody
	result       = (CodeBody) make_code();
	result->Type = CT_Function_Body;

	do
	{
		Code entry = *codes;
		codes++;

		if (!entry)
		{
			log_failure("gen::" "def_class_body" ": Provided an null entry");
			return InvalidCode;
		}

		switch (entry->Type)
		{
			GEN_AST_BODY_CLASS_UNALLOWED_TYPES
				log_failure("gen::" "def_class_body" ": Entry type is not allowed: %s", debug_str(entry));
				return InvalidCode;

			default:
			break;
		}

		append(result, entry);
	}
	while (num--, num > 0);

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

		if ( ! entry )
		{
			log_failure("gen::def_enum_body: Provided a null entry");
			return InvalidCode;
		}

		if ( entry->Type != CT_Untyped && entry->Type != CT_Comment )
		{
			log_failure("gen::def_enum_body: Entry type is not allowed - %s. Must be of untyped or comment type.", debug_str(entry) );
			return InvalidCode;
		}

		append(result, entry );
	}
	while ( num--, num > 0 );
	va_end(va);

	return (CodeBody) result;
}

CodeBody def_enum_body( s32 num, Code* codes )
{
	def_body_code_array_start( def_enum_body );

	CodeBody
	result       = (CodeBody) make_code();
	result->Type = CT_Enum_Body;

	do
	{
		Code entry = *codes;

		if ( ! entry )
		{
			log_failure("gen::def_enum_body: Provided a null entry");
			return InvalidCode;
		}

		if ( entry->Type != CT_Untyped && entry->Type != CT_Comment )
		{
			log_failure("gen::def_enum_body: Entry type is not allowed: %s", debug_str(entry) );
			return InvalidCode;
		}

		append(result, entry );
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

		if (!entry)
		{
			log_failure("gen::" "def_export_body" ": Provided an null entry");
			return InvalidCode;
		}

		switch (entry->Type)
		{
			GEN_AST_BODY_EXPORT_UNALLOWED_TYPES
				log_failure("gen::" "def_export_body" ": Entry type is not allowed: %s", debug_str(entry));
				return InvalidCode;

			default:
			break;
		}

		append(result, entry);
	}
	while (num--, num > 0);
	va_end(va);

	return result;
}

CodeBody def_export_body( s32 num, Code* codes )
{
	def_body_code_array_start( def_export_body );

	CodeBody
	result       = (CodeBody) make_code();
	result->Type = CT_Export_Body;

	do
	{
		Code entry = *codes;
		codes++;

		if (!entry)
		{
			log_failure("gen::" "def_export_body" ": Provided an null entry");
			return InvalidCode;
		}

		switch (entry->Type)
		{
			GEN_AST_BODY_EXPORT_UNALLOWED_TYPES
				log_failure("gen::" "def_export_body" ": Entry type is not allowed: %s", debug_str(entry));
				return InvalidCode;

			default:
			break;
		}

		append(result, entry);
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
		Code_POD pod = va_arg(va, Code_POD);
		Code entry = pcast(Code, pod);

		if (!entry)
		{
			log_failure("gen::" "def_extern_linkage_body" ": Provided an null entry");
			return InvalidCode;
		}

		switch (entry->Type)
		{
			GEN_AST_BODY_EXTERN_LINKAGE_UNALLOWED_TYPES
				log_failure("gen::" "def_extern_linkage_body" ": Entry type is not allowed: %s", debug_str(entry));
				return InvalidCode;

			default:
			break;
		}

		append(result, entry);
	}
	while (num--, num > 0);
	va_end(va);

	return result;
}

CodeBody def_extern_link_body( s32 num, Code* codes )
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
			GEN_AST_BODY_EXTERN_LINKAGE_UNALLOWED_TYPES
				log_failure("gen::" "def_extern_linkage_body" ": Entry type is not allowed: %s", debug_str(entry));
				return InvalidCode;

			default:
			break;
		}

		append(result, entry);

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

		if (!entry)
		{
			log_failure("gen::" stringize(def_function_body) ": Provided an null entry");
			return InvalidCode;
		}

		switch (entry->Type)
		{

			GEN_AST_BODY_FUNCTION_UNALLOWED_TYPES
				log_failure("gen::" stringize(def_function_body) ": Entry type is not allowed: %s", debug_str(entry));
				return InvalidCode;

			default:
			break;
		}

		append(result, entry);
	}
	while (num--, num > 0);
	va_end(va);

	return result;
}

CodeBody def_function_body( s32 num, Code* codes )
{
	def_body_code_array_start( def_function_body );

	CodeBody
	result       = (CodeBody) make_code();
	result->Type = CT_Function_Body;

	do
	{
		Code entry = *codes;
		codes++;

		if (!entry)
		{
			log_failure("gen::" "def_function_body" ": Provided an null entry");
			return InvalidCode;
		}

		switch (entry->Type)
		{
			GEN_AST_BODY_FUNCTION_UNALLOWED_TYPES
				log_failure("gen::" "def_function_body" ": Entry type is not allowed: %s", debug_str(entry));
				return InvalidCode;

			default:
			break;
		}
		append(result, entry);
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

		if (!entry)
		{
			log_failure("gen::" "def_global_body" ": Provided an null entry");
			return InvalidCode;
		}

		switch (entry->Type)
		{
			case CT_Global_Body:
				// result.append( entry.code_cast<CodeBody>() ) ;
				append( result, cast(CodeBody, entry) );
				continue;

			GEN_AST_BODY_GLOBAL_UNALLOWED_TYPES
				log_failure("gen::" "def_global_body" ": Entry type is not allowed: %s", debug_str(entry));
				return InvalidCode;

			default:
			break;
		}

		append(result, entry);
	}
	while (num--, num > 0);
	va_end(va);

	return result;
}

CodeBody def_global_body( s32 num, Code* codes )
{
	def_body_code_array_start( def_global_body );

	CodeBody
	result       = (CodeBody) make_code();
	result->Type = CT_Global_Body;

	do
	{
		Code entry = *codes;
		codes++;

		if (!entry)
		{
			log_failure("gen::" "def_global_body" ": Provided an null entry");
			return InvalidCode;
		}

		switch (entry->Type)
		{
			case CT_Global_Body:
				append(result, cast(CodeBody, entry) );
				continue;

			GEN_AST_BODY_GLOBAL_UNALLOWED_TYPES
				log_failure("gen::" "def_global_body" ": Entry type is not allowed: %s", debug_str(entry));
				return InvalidCode;

			default:
			break;
		}

		append(result, entry);
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

		if (!entry)
		{
			log_failure("gen::" "def_namespace_body" ": Provided an null entry");
			return InvalidCode;
		}

		switch (entry->Type)
		{
			GEN_AST_BODY_NAMESPACE_UNALLOWED_TYPES
				log_failure("gen::" "def_namespace_body" ": Entry type is not allowed: %s", debug_str(entry));
				return InvalidCode;

			default:
			break;
		}

		append(result, entry);
	}
	while (num--, num > 0);
	va_end(va);

	return result;
}

CodeBody def_namespace_body( s32 num, Code* codes )
{
	def_body_code_array_start( def_namespace_body );

	CodeBody
	result       = (CodeBody) make_code();
	result->Type = CT_Global_Body;

	do
	{
		Code entry = *codes;
		codes++;

		if (!entry)
		{
			log_failure("gen::" "def_namespace_body" ": Provided an null entry");
			return InvalidCode;
		}

		switch (entry->Type)
		{
			GEN_AST_BODY_NAMESPACE_UNALLOWED_TYPES
				log_failure("gen::" "def_namespace_body" ": Entry type is not allowed: %s", debug_str(entry) );
				return InvalidCode;

			default: break;
		}

		append(result, entry);
	}
	while (num--, num > 0);

	return result;
}

CodeParam def_params( s32 num, ... )
{
	def_body_start( def_params );

	va_list va;
	va_start(va, num);

	Code_POD  pod   = va_arg(va, Code_POD);
	CodeParam param = pcast( CodeParam, pod );

	null_check( def_params, param );

	if ( param->Type != CT_Parameters )
	{
		log_failure( "gen::def_params: param %d is not a Parameters", num - num + 1 );
		return InvalidCode;
	}

	CodeParam result = (CodeParam) duplicate(param);

	while ( -- num )
	{
		pod   = va_arg(va, Code_POD);
		param = pcast( CodeParam, pod );

		if ( param->Type != CT_Parameters )
		{
			log_failure( "gen::def_params: param %d is not a Parameters", num - num + 1 );
			return InvalidCode;
		}

		append(result, param );
	}
	va_end(va);

	return result;
}

CodeParam def_params( s32 num, CodeParam* codes )
{
	def_body_code_array_start( def_params );

#	define check_current()                                                                                      \
	if ( current.ast == nullptr )                                                                               \
	{                                                                                                           \
		log_failure("gen::def_params: Provide a null code in codes array");                                     \
		return InvalidCode;                                                                                     \
	}                                                                                                           \
																												\
	if (current->Type != CT_Parameters )                                                                        \
	{                                                                                                           \
		log_failure("gen::def_params: Code in coes array is not of paramter type - %s", debug_str(current) );   \
		return InvalidCode;                                                                                     \
	}

	CodeParam current = (CodeParam)duplicate(* codes);
	check_current();

	CodeParam
	result            = (CodeParam) make_code();
	result->Name      = current->Name;
	result->Type      = current->Type;
	result->ValueType = current->ValueType;

	while( codes++, current = * codes, num--, num > 0 )
	{
		check_current();
		append(result, current );
	}
#	undef check_current

	return result;
}

CodeSpecifiers def_specifiers( s32 num, ... )
{
	if ( num <= 0 )
	{
		log_failure("gen::def_specifiers: num cannot be zero or less");
		return InvalidCode;
	}

	if ( num > AST_ArrSpecs_Cap )
	{
		log_failure("gen::def_specifiers: num of speciifers to define AST larger than AST specicifier capacity - %d", num);
		return InvalidCode;
	}

	CodeSpecifiers
	result       = (CodeSpecifiers) make_code();
	result->Type = CT_Specifiers;

	va_list va;
	va_start(va, num);
	do
	{
		Specifier type = (Specifier)va_arg(va, int);

		append(result, type );
	}
	while ( --num, num );
	va_end(va);

	return result;
}

CodeSpecifiers def_specifiers( s32 num, Specifier* specs )
{
	if ( num <= 0 )
	{
		log_failure("gen::def_specifiers: num cannot be zero or less");
		return InvalidCode;
	}

	if ( num > AST_ArrSpecs_Cap )
	{
		log_failure("gen::def_specifiers: num of speciifers to define AST larger than AST specicifier capacity - %d", num);
		return InvalidCode;
	}

	CodeSpecifiers
	result       = (CodeSpecifiers) make_code();
	result->Type = CT_Specifiers;

	s32 idx = 0;
	do
	{
		append(result, specs[idx] );
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

		if (!entry)
		{
			log_failure("gen::" "def_struct_body" ": Provided an null entry");
			return InvalidCode;
		}

		switch (entry->Type)
		{
			GEN_AST_BODY_STRUCT_UNALLOWED_TYPES
				log_failure("gen::" "def_struct_body" ": Entry type is not allowed: %s", debug_str(entry));
				return InvalidCode;

			default:
			break;
		}

		append(result, entry);
	}
	while (num--, num > 0);
	va_end(va);

	return result;
}

CodeBody def_struct_body( s32 num, Code* codes )
{
	def_body_code_array_start( def_struct_body );

	CodeBody
	result       = (CodeBody) make_code();
	result->Type = CT_Struct_Body;

	do
	{
		Code entry = *codes;
		codes++;

		if (!entry)
		{
			log_failure("gen::" "def_struct_body" ": Provided an null entry");
			return InvalidCode;
		}

		switch (entry->Type)
		{
			GEN_AST_BODY_STRUCT_UNALLOWED_TYPES
				log_failure("gen::" "def_struct_body" ": Entry type is not allowed: %s", debug_str(entry) );
				return InvalidCode;

			default:
			break;
		}

		append(result, entry);
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

		if ( ! entry )
		{
			log_failure("gen::def_union_body: Provided a null entry");
			return InvalidCode;
		}

		if ( entry->Type != CT_Untyped && entry->Type != CT_Comment )
		{
			log_failure("gen::def_union_body: Entry type is not allowed - %s. Must be of untyped or comment type.", debug_str(entry) );
			return InvalidCode;
		}

		append(result, entry );
	}
	while ( num--, num > 0 );
	va_end(va);

	return result;
}

CodeBody def_union_body( s32 num, CodeUnion* codes )
{
	def_body_code_array_start( def_union_body );

	CodeBody
	result       = (CodeBody) make_code();
	result->Type = CT_Union_Body;

	do
	{
		Code entry = *codes;

		if ( ! entry )
		{
			log_failure("gen::def_union_body: Provided a null entry");
			return InvalidCode;
		}

		if ( entry->Type != CT_Untyped && entry->Type != CT_Comment )
		{
			log_failure("gen::def_union_body: Entry type is not allowed: %s", debug_str(entry) );
			return InvalidCode;
		}

		append(result, entry );
	}
	while ( codes++, num--, num > 0 );

	return (CodeBody) result;
}

#	undef name_check
#	undef null_check
#	undef null_or_invalid_check
#	undef def_body_start
#	undef def_body_code_array_start

#pragma endregion Upfront
