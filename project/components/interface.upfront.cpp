#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "interface.cpp"
#endif

#pragma region Upfront

enum class OpValidateResult : u32
{
	Fail,
	Global,
	Member
};

OpValidateResult operator__validate( OperatorT op, CodeParam params_code, CodeType ret_type, CodeSpecifiers specifier )
{
	using namespace EOperator;

	if ( op == EOperator::Invalid )
	{
		log_failure("gen::def_operator: op cannot be invalid");
		return OpValidateResult::Fail;
	}

#pragma region Helper Macros
#	define check_params()                                                                                 \
	if ( ! params_code )                                                                                  \
	{                                                                                                     \
		log_failure("gen::def_operator: params is null and operator%s requires it", to_str(op));          \
		return OpValidateResult::Fail;                                                                    \
	}                                                                                                     \
	if ( params_code->Type != ECode::Parameters )                                                         \
	{                                                                                                     \
		log_failure("gen::def_operator: params is not of Parameters type - %s", params_code.debug_str()); \
		return OpValidateResult::Fail;                                                                    \
	}

#	define check_param_eq_ret()                                                                     \
	if ( ! is_member_symbol && ! params_code->ValueType.is_equal( ret_type) )                       \
	{                                                                                               \
		log_failure("gen::def_operator: operator%s requires first parameter to equal return type\n" \
			"param types: %s\n"                                                                     \
			"return type: %s",                                                                      \
			to_str(op).Ptr,                                                                         \
			params_code.debug_str(),                                                                \
			ret_type.debug_str()                                                                    \
		);                                                                                          \
		return OpValidateResult::Fail;                                                              \
	}
#pragma endregion Helper Macros

	if ( ! ret_type )
	{
		log_failure("gen::def_operator: ret_type is null but is required by operator%s", to_str(op));
	}

	if ( ret_type->Type != ECode::Typename )
	{
		log_failure("gen::def_operator: ret_type is not of typename type - %s", ret_type.debug_str());
		return OpValidateResult::Fail;
	}

	bool is_member_symbol = false;

	switch ( op )
	{
#	define specs( ... ) num_args( __VA_ARGS__ ), __VA_ARGS__
		case Assign:
			check_params();

			if ( params_code->NumEntries > 1 )
			{
				log_failure("gen::def_operator: "
					"operator%s does not support non-member definition (more than one parameter provided) - %s",
					to_str(op),
					params_code.debug_str()
				);
				return OpValidateResult::Fail;
			}

			is_member_symbol = true;
			break;

		case Assign_Add:
		case Assign_Subtract:
		case Assign_Multiply:
		case Assign_Divide:
		case Assign_Modulo:
		case Assign_BAnd:
		case Assign_BOr:
		case Assign_BXOr:
		case Assign_LShift:
		case Assign_RShift:
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
					, params_code.debug_str()
				);
				return OpValidateResult::Fail;
			}
			break;

		case Increment:
		case Decrement:
			// If its not set, it just means its a prefix member op.
			if ( params_code )
			{
				if ( params_code->Type != ECode::Parameters )
				{
					log_failure("gen::def_operator: operator%s params code provided is not of Parameters type - %s"
						, to_str(op)
						, params_code.debug_str()
					);
					return OpValidateResult::Fail;
				}

				switch ( params_code->NumEntries )
				{
					case 1:
						if ( params_code->ValueType.is_equal( t_int ) )
							is_member_symbol = true;

						else
							check_param_eq_ret();
						break;

					case 2:
						check_param_eq_ret();

						if ( ! params_code.get(1).is_equal( t_int ) )
						{
							log_failure("gen::def_operator: "
								"operator%s requires second parameter of non-member definition to be int for post-decrement",
								to_str(op)
							);
							return OpValidateResult::Fail;
						}
						break;

					default:
						log_failure("gen::def_operator: operator%s recieved unexpected number of parameters recived %d instead of 0-2"
							, to_str(op)
							, params_code->NumEntries
						);
						return OpValidateResult::Fail;
				}
			}
			break;

		case Unary_Plus:
		case Unary_Minus:
			if ( ! params_code )
				is_member_symbol = true;

			else
			{
				if ( params_code->Type != ECode::Parameters )
				{
					log_failure("gen::def_operator: params is not of Parameters type - %s", params_code.debug_str());
					return OpValidateResult::Fail;
				}

				if ( params_code->ValueType.is_equal( ret_type ) )
				{
					log_failure("gen::def_operator: "
						"operator%s is non-member symbol yet first paramter does not equal return type\n"
						"param type: %s\n"
						"return type: %s\n"
						, params_code.debug_str()
						, ret_type.debug_str()
					);
					return OpValidateResult::Fail;
				}

				if ( params_code->NumEntries > 1 )
				{
					log_failure("gen::def_operator: operator%s may not have more than one parameter - param count: %d"
						, to_str(op)
						, params_code->NumEntries
					);
					return OpValidateResult::Fail;
				}
			}
			break;

		case BNot:
		{
			// Some compilers let you do this...
		#if 0
			if ( ! ret_type.is_equal( t_bool) )
			{
				log_failure( "gen::def_operator: return type is not a boolean - %s", params_code.debug_str() );
				return OpValidateResult::Fail;
			}
		#endif

			if ( ! params_code )
				is_member_symbol = true;

			else
			{
				if ( params_code->Type != ECode::Parameters )
				{
					log_failure( "gen::def_operator: params is not of Parameters type - %s", params_code.debug_str() );
					return OpValidateResult::Fail;
				}

				if ( params_code->NumEntries > 1 )
				{
					log_failure(
					    "gen::def_operator: operator%s may not have more than one parameter - param count: %d",
					    to_str( op ),
					    params_code->NumEntries
					);
					return OpValidateResult::Fail;
				}
			}
			break;
		}

		case Add:
		case Subtract:
		case Multiply:
		case Divide:
		case Modulo:
		case BAnd:
		case BOr:
		case BXOr:
		case LShift:
		case RShift:
			check_params();

			switch ( params_code->NumEntries )
			{
				case 1:
					is_member_symbol = true;
					break;

				case 2:
					if ( ! params_code->ValueType.is_equal( ret_type ) )
					{
						log_failure("gen::def_operator: "
							"operator%s is non-member symbol yet first paramter does not equal return type\n"
							"param type: %s\n"
							"return type: %s\n"
							, params_code.debug_str()
							, ret_type.debug_str()
						);
						return OpValidateResult::Fail;
					}
					break;

				default:
					log_failure("gen::def_operator: operator%s recieved unexpected number of paramters recived %d instead of 0-2"
						, to_str(op)
						, params_code->NumEntries
					);
					return OpValidateResult::Fail;
			}
			break;

		case UnaryNot:
			if ( ! params_code )
				is_member_symbol = true;

			else
			{
				if ( params_code->Type != ECode::Parameters )
				{
					log_failure("gen::def_operator: params is not of Parameters type - %s", params_code.debug_str());
					return OpValidateResult::Fail;
				}

				if ( params_code->NumEntries != 1 )
				{
					log_failure("gen::def_operator: operator%s recieved unexpected number of paramters recived %d instead of 0-1"
						, to_str(op)
						, params_code->NumEntries
					);
					return OpValidateResult::Fail;
				}
			}

			if ( ! ret_type.is_equal( t_bool ))
			{
				log_failure("gen::def_operator: operator%s return type must be of type bool - %s"
					, to_str(op)
					, ret_type.debug_str()
				);
				return OpValidateResult::Fail;
			}
			break;

		case LAnd:
		case LOr:
		case LEqual:
		case LNot:
		case Lesser:
		case Greater:
		case LesserEqual:
		case GreaterEqual:
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
					return OpValidateResult::Fail;
			}
			break;

		case Indirection:
		case AddressOf:
		case MemberOfPointer:
			if ( params_code && params_code->NumEntries > 1)
			{
				log_failure("gen::def_operator: operator%s recieved unexpected number of paramters recived %d instead of 0-1"
					, to_str(op)
					, params_code->NumEntries
				);
				return OpValidateResult::Fail;
			}
			else
			{
				is_member_symbol = true;
			}
			break;

		case PtrToMemOfPtr:
			if ( params_code )
			{
				log_failure("gen::def_operator: operator%s expects no paramters - %s", to_str(op), params_code.debug_str());
				return OpValidateResult::Fail;
			}
			break;

		case Subscript:
		case FunctionCall:
		case Comma:
			check_params();
			break;

		case New:
		case Delete:
			// This library doesn't support validating new and delete yet.
			break;
#	undef specs
	}

	return is_member_symbol ? OpValidateResult::Member : OpValidateResult::Global;
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
		return CodeInvalid;                                                                           \
	}                                                                                                 \
																									  \
	if ( Name_.Ptr == nullptr )                                                                       \
	{                                                                                                 \
		log_failure( "gen::" stringize(Context_) ": name is null" );                                  \
		return CodeInvalid;                                                                           \
	}                                                                                                 \
}

#define null_check( Context_, Code_ )                                                         \
	if ( ! Code_ )                                                                            \
	{                                                                                         \
		log_failure( "gen::" stringize(Context_) ": " stringize(Code_) " provided is null" ); \
		return CodeInvalid;                                                                   \
	}

#define null_or_invalid_check( Context_, Code_ )                                                \
{                                                                                               \
	if ( ! Code_ )                                                                              \
	{                                                                                           \
		log_failure( "gen::" stringize(Context_) ": " stringize(Code_) " provided is null" );   \
		return CodeInvalid;                                                                     \
	}                                                                                           \
																								\
	if ( Code_->is_invalid() )                                                                  \
	{                                                                                           \
		log_failure("gen::" stringize(Context_) ": " stringize(Code_) " provided is invalid" ); \
		return CodeInvalid;                                                                     \
	}                                                                                           \
}

#define not_implemented( Context_ )                             \
	log_failure( "gen::%s: This function is not implemented" ); \
	return CodeInvalid;
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
		return CodeInvalid;
	}

	Code
	result          = make_code();
	result->Type    = ECode::PlatformAttributes;
	result->Name    = get_cached_string( content );
	result->Content = result->Name;

	return (CodeAttributes) result;
}

CodeComment def_comment( StrC content )
{
	if ( content.Len <= 0 || content.Ptr == nullptr )
	{
		log_failure( "gen::def_comment: Invalid comment provided:" );
		return CodeInvalid;
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
		append_fmt(cmt_formatted, "//%.*s", length, line );
		mem_set( line, 0, MaxCommentLineLength );

		scanner += length;
	}
	while ( scanner <= end );

	if ( back(cmt_formatted) != '\n' )
		append( cmt_formatted, "\n" );

	Code
	result          = make_code();
	result->Type    = ECode::Comment;
	result->Name    = get_cached_string( cmt_formatted );
	result->Content = result->Name;

	free(cmt_formatted);

	return (CodeComment) result;
}

CodeConstructor def_constructor( CodeParam params, Code initializer_list, Code body )
{
	using namespace ECode;

	if ( params && params->Type != Parameters )
	{
		log_failure("gen::def_constructor: params must be of Parameters type - %s", params.debug_str());
		return CodeInvalid;
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
			case Function_Body:
			case Untyped:
			break;

			default:
				log_failure("gen::def_constructor: body must be either of Function_Body or Untyped type - %s", body.debug_str());
				return CodeInvalid;
		}

		result->Type = Constructor;
		result->Body = body;
	}
	else
	{
		result->Type = Constructor_Fwd;
	}

	return result;
}

CodeClass def_class( StrC name
	, Code           body
	, CodeType       parent, AccessSpec parent_access
	, CodeAttributes attributes
	, ModuleFlag     mflags
	, CodeType*      interfaces, s32 num_interfaces	)
{
	using namespace ECode;

	name_check( def_class, name );

	if ( attributes && attributes->Type != PlatformAttributes )
	{
		log_failure( "gen::def_class: attributes was not a 'PlatformAttributes' type: %s", attributes.debug_str() );
		return CodeInvalid;
	}

	if ( parent && ( parent->Type != Class && parent->Type != Struct && parent->Type != Typename && parent->Type != Untyped ) )
	{
		log_failure( "gen::def_class: parent provided is not type 'Class', 'Struct', 'Typeanme', or 'Untyped': %s", parent.debug_str() );
		return CodeInvalid;
	}

	CodeClass
	result              = (CodeClass) make_code();
	result->Name        = get_cached_string( name );
	result->ModuleFlags = mflags;

	if ( body )
	{
		switch ( body->Type )
		{
			case Class_Body:
			case Untyped:
			break;

			default:
				log_failure("gen::def_class: body must be either of Class_Body or Untyped type - %s", body.debug_str());
				return CodeInvalid;
		}

		result->Type = Class;
		result->Body = body;
		result->Body->Parent = result; // TODO(Ed): Review this?
	}
	else
	{
		result->Type = Class_Fwd;
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
			result.add_interface( interfaces[idx] );
		}
	}

	return result;
}

CodeDefine def_define( StrC name, StrC content )
{
	using namespace ECode;

	name_check( def_define, name );

	// Defines can be empty definitions
#if 0
	if ( content.Len <= 0 || content.Ptr == nullptr )
	{
		log_failure( "gen::def_define: Invalid value provided" );
		return CodeInvalid;
	}
#endif

	CodeDefine
	result          = (CodeDefine) make_code();
	result->Type    = Preprocess_Define;
	result->Name    = get_cached_string( name );
	if ( content.Len <= 0 || content.Ptr == nullptr )
	{
		result->Content = get_cached_string( txt("") );
	}
	else
		result->Content = get_cached_string( content );

	return result;
}

CodeDestructor def_destructor( Code body, CodeSpecifiers specifiers )
{
	using namespace ECode;

	if ( specifiers && specifiers->Type != Specifiers )
	{
		log_failure( "gen::def_destructor: specifiers was not a 'Specifiers' type: %s", specifiers.debug_str() );
		return CodeInvalid;
	}

	CodeDestructor result = (CodeDestructor) make_code();

	if ( specifiers )
		result->Specs = specifiers;

	if ( body )
	{
		switch ( body->Type )
		{
			case Function_Body:
			case Untyped:
			break;

			default:
				log_failure("gen::def_destructor: body must be either of Function_Body or Untyped type - %s", body.debug_str());
				return CodeInvalid;
		}

		result->Type = Destructor;
		result->Body = body;
	}
	else
	{
		result->Type = Destructor_Fwd;
	}

	return result;
}

CodeEnum def_enum( StrC name
	, Code       body,      CodeType       type
	, EnumT      specifier, CodeAttributes attributes
	, ModuleFlag mflags )
{
	using namespace ECode;

	name_check( def_enum, name );

	if ( type && type->Type != Typename )
	{
		log_failure( "gen::def_enum: enum underlying type provided was not of type Typename: %s", type.debug_str() );
		return CodeInvalid;
	}

	if ( attributes && attributes->Type != PlatformAttributes )
	{
		log_failure( "gen::def_enum: attributes was not a 'PlatformAttributes' type: %s", attributes.debug_str() );
		return CodeInvalid;
	}

	CodeEnum
	result              = (CodeEnum) make_code();
	result->Name        = get_cached_string( name );
	result->ModuleFlags = mflags;

	if ( body )
	{
		switch ( body->Type )
		{
			case Enum_Body:
			case Untyped:
			break;

			default:
				log_failure( "gen::def_enum: body must be of Enum_Body or Untyped type %s", body.debug_str());
				return CodeInvalid;
		}

		result->Type = specifier == EnumDecl_Class ?
			Enum_Class : Enum;

		result->Body = body;
	}
	else
	{
		result->Type = specifier == EnumDecl_Class ?
			Enum_Class_Fwd : Enum_Fwd;
	}

	if ( attributes )
		result->Attributes = attributes;

	if ( type )
	{
		result->UnderlyingType = type;
	}
	else if ( result->Type != Enum_Class_Fwd && result->Type != Enum_Fwd )
	{
		log_failure( "gen::def_enum: enum forward declaration must have an underlying type" );
		return CodeInvalid;
	}

	return result;
}

CodeExec def_execution( StrC content )
{
	if ( content.Len <= 0 || content.Ptr == nullptr )
	{
		log_failure( "gen::def_execution: Invalid execution provided" );
		return CodeInvalid;
	}

	Code
	result          = make_code();
	result->Type    = ECode::Execution;
	result->Name    = get_cached_string( content );
	result->Content = result->Name;

	return (CodeExec) result;
}

CodeExtern def_extern_link( StrC name, Code body )
{
	using namespace ECode;

	name_check( def_extern_linkage, name );
	null_check( def_extern_linkage, body );

	if ( body->Type != Extern_Linkage_Body && body->Type != Untyped )
	{
		log_failure("gen::def_extern_linkage: body is not of extern_linkage or untyped type %s", body->debug_str());
		return CodeInvalid;
	}

	CodeExtern
	result        = (CodeExtern)make_code();
	result->Type  = Extern_Linkage;
	result->Name  = get_cached_string( name );
	result->Body  = body;

	return (CodeExtern) result;
}

CodeFriend def_friend( Code declaration )
{
	using namespace ECode;

	null_check( def_friend, declaration );

	switch ( declaration->Type )
	{
		case Class_Fwd:
		case Function_Fwd:
		case Operator_Fwd:
		case Struct_Fwd:
		case Class:
		case Function:
		case Operator:
		case Struct:
		break;

		default:
			log_failure("gen::def_friend: requires declartion to have class, function, operator, or struct - %s", declaration->debug_str());
			return CodeInvalid;
	}

	CodeFriend
	result       = (CodeFriend) make_code();
	result->Type = Friend;

	result->Declaration = declaration;

	return result;
}

CodeFn def_function( StrC name
	, CodeParam      params ,    CodeType       ret_type, Code body
	, CodeSpecifiers specifiers, CodeAttributes attributes
	, ModuleFlag     mflags )
{
	using namespace ECode;

	name_check( def_function, name );

	if ( params && params->Type != Parameters )
	{
		log_failure( "gen::def_function: params was not a `Parameters` type: %s", params.debug_str() );
		return CodeInvalid;
	}

	if ( ret_type && ret_type->Type != Typename )
	{
		log_failure( "gen::def_function: ret_type was not a Typename: %s", ret_type.debug_str() );
		return CodeInvalid;
	}

	if ( specifiers && specifiers->Type != Specifiers )
	{
		log_failure( "gen::def_function: specifiers was not a `Specifiers` type: %s", specifiers.debug_str() );
		return CodeInvalid;
	}

	if ( attributes && attributes->Type != PlatformAttributes )
	{
		log_failure( "gen::def_function: attributes was not a `PlatformAttributes` type: %s", attributes.debug_str() );
		return CodeInvalid;
	}

	CodeFn
	result              = (CodeFn) make_code();
	result->Name        = get_cached_string( name );
	result->ModuleFlags = mflags;

	if ( body )
	{
		switch ( body->Type )
		{
			case Function_Body:
			case Execution:
			case Untyped:
				break;

			default:
			{
				log_failure("gen::def_function: body must be either of Function_Body, Execution, or Untyped type. %s", body->debug_str());
				return CodeInvalid;
			}
		}

		result->Type = Function;
		result->Body = body;
	}
	else
	{
		result->Type = Function_Fwd;
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

CodeInclude def_include( StrC path, bool foreign )
{
	if ( path.Len <= 0 || path.Ptr == nullptr )
	{
		log_failure( "gen::def_include: Invalid path provided - %d" );
		return CodeInvalid;
	}

	StrC content = foreign ?
			to_str( str_fmt_buf( "<%.*s>", path.Len, path.Ptr ))
		:	to_str( str_fmt_buf( "\"%.*s\"", path.Len, path.Ptr ));

	Code
	result          = make_code();
	result->Type    = ECode::Preprocess_Include;
	result->Name    = get_cached_string( content );
	result->Content = result->Name;

	return (CodeInclude) result;
}

CodeModule def_module( StrC name, ModuleFlag mflags )
{
	name_check( def_module, name );

	Code
	result              = make_code();
	result->Type        = ECode::Module;
	result->Name        = get_cached_string( name );
	result->Content     = result->Name;
	result->ModuleFlags = mflags;

	return (CodeModule) result;
}

CodeNS def_namespace( StrC name, Code body, ModuleFlag mflags )
{
	using namespace ECode;

	name_check( def_namespace, name );
	null_check( def_namespace, body );

	if ( body->Type != Namespace_Body && body->Type != Untyped )
	{
		log_failure("gen::def_namespace: body is not of namespace or untyped type %s", body.debug_str());
		return CodeInvalid;
	}

	CodeNS
	result              = (CodeNS) make_code();
	result->Type        = Namespace;
	result->Name        = get_cached_string( name );
	result->ModuleFlags = mflags;
	result->Body        = body;

	return result;
}

CodeOperator def_operator( OperatorT op, StrC nspace
	, CodeParam      params_code, CodeType       ret_type, Code body
	, CodeSpecifiers specifiers,  CodeAttributes attributes
	, ModuleFlag     mflags )
{
	using namespace ECode;

	if ( attributes && attributes->Type != PlatformAttributes )
	{
		log_failure( "gen::def_operator: PlatformAttributes was provided but its not of attributes type: %s", attributes.debug_str() );
		return CodeInvalid;
	}

	if ( specifiers && specifiers->Type != Specifiers )
	{
		log_failure( "gen::def_operator: Specifiers was provided but its not of specifiers type: %s", specifiers.debug_str() );
		return CodeInvalid;
	}

	OpValidateResult check_result = operator__validate( op, params_code, ret_type, specifiers );

	if ( check_result == OpValidateResult::Fail )
	{
		return CodeInvalid;
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
			case Function_Body:
			case Execution:
			case Untyped:
				break;

			default:
			{
				log_failure("gen::def_operator: body must be either of Function_Body, Execution, or Untyped type. %s", body->debug_str());
				return CodeInvalid;
			}
		}

		result->Type = check_result == OpValidateResult::Global ?
			Operator : Operator_Member;

		result->Body = body;
	}
	else
	{
		result->Type = check_result == OpValidateResult::Global ?
			Operator_Fwd : Operator_Member_Fwd;
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

CodeOpCast def_operator_cast( CodeType type, Code body, CodeSpecifiers const_spec )
{
	using namespace ECode;
	null_check( def_operator_cast, type );

	if ( type->Type != Typename )
	{
		log_failure( "gen::def_operator_cast: type is not a typename - %s", type.debug_str() );
		return CodeInvalid;
	}

	CodeOpCast result = (CodeOpCast) make_code();

	if (body)
	{
		result->Type = Operator_Cast;

		if ( body->Type != Function_Body && body->Type != Execution )
		{
			log_failure( "gen::def_operator_cast: body is not of function body or execution type - %s", body.debug_str() );
			return CodeInvalid;
		}

		result->Body = body;
	}
	else
	{
		result->Type = Operator_Cast_Fwd;
	}

	if ( const_spec )
	{
		result->Specs = const_spec;
	}

	result->ValueType = type;
	return result;
}

CodeParam def_param( CodeType type, StrC name, Code value )
{
	using namespace ECode;

	name_check( def_param, name );
	null_check( def_param, type );

	if ( type->Type != Typename )
	{
		log_failure( "gen::def_param: type is not a typename - %s", type.debug_str() );
		return CodeInvalid;
	}

	if ( value && value->Type != Untyped )
	{
		log_failure( "gen::def_param: value is not untyped - %s", value.debug_str() );
		return CodeInvalid;
	}

	CodeParam
	result       = (CodeParam) make_code();
	result->Type = Parameters;
	result->Name = get_cached_string( name );

	result->ValueType = type;

	if ( value )
		result->Value = value;

	result->NumEntries++;

	return result;
}

CodePragma def_pragma( StrC directive )
{
	using namespace ECode;

	if ( directive.Len <= 0 || directive.Ptr == nullptr )
	{
		log_failure( "gen::def_comment: Invalid comment provided:" );
		return CodeInvalid;
	}

	CodePragma
	result          = (CodePragma) make_code();
	result->Type    = Preprocess_Pragma;
	result->Content = get_cached_string( directive );

	return result;
}

CodePreprocessCond def_preprocess_cond( EPreprocessCond type, StrC expr )
{
	using namespace ECode;

	if ( expr.Len <= 0 || expr.Ptr == nullptr )
	{
		log_failure( "gen::def_comment: Invalid comment provided:" );
		return CodeInvalid;
	}

	CodePreprocessCond
	result          = (CodePreprocessCond) make_code();
	result->Content = get_cached_string( expr );

	switch (type)
	{
		case PreprocessCond_If:
			result->Type = Preprocess_If;
		break;
		case PreprocessCond_IfDef:
			result->Type = Preprocess_IfDef;
		break;
		case PreprocessCond_IfNotDef:
			result->Type = Preprocess_IfNotDef;
		break;
		case PreprocessCond_ElIf:
			result->Type = Preprocess_ElIf;
		break;
	}

	return result;
}

CodeSpecifiers def_specifier( SpecifierT spec )
{
	CodeSpecifiers
	result       = (CodeSpecifiers) make_code();
	result->Type = ECode::Specifiers;
	result.append( spec );

	return result;
}

CodeStruct def_struct( StrC name
	, Code           body
	, CodeType       parent, AccessSpec parent_access
	, CodeAttributes attributes
	, ModuleFlag     mflags
	, CodeType*      interfaces, s32 num_interfaces )
{
	using namespace ECode;

	if ( attributes && attributes->Type != PlatformAttributes )
	{
		log_failure( "gen::def_struct: attributes was not a `PlatformAttributes` type - %s", attributes.debug_str() );
		return CodeInvalid;
	}

	if ( parent && parent->Type != Typename )
	{
		log_failure( "gen::def_struct: parent was not a `Struct` type - %s", parent.debug_str() );
		return CodeInvalid;
	}

	if ( body && body->Type != Struct_Body )
	{
		log_failure( "gen::def_struct: body was not a Struct_Body type - %s", body.debug_str() );
		return CodeInvalid;
	}

	CodeStruct
	result              = (CodeStruct) make_code();
	result->ModuleFlags = mflags;

	if ( name )
		result->Name = get_cached_string( name );

	if ( body )
	{
		result->Type = Struct;
		result->Body = body;
	}
	else
	{
		result->Type = Struct_Fwd;
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
			result.add_interface( interfaces[idx] );
		}
	}

	return result;
}

CodeTemplate def_template( CodeParam params, Code declaration, ModuleFlag mflags )
{
	null_check( def_template, declaration );

	if ( params && params->Type != ECode::Parameters )
	{
		log_failure( "gen::def_template: params is not of parameters type - %s", params.debug_str() );
		return CodeInvalid;
	}

	switch (declaration->Type )
	{
		case ECode::Class:
		case ECode::Function:
		case ECode::Struct:
		case ECode::Variable:
		case ECode::Using:
		break;

		default:
			log_failure( "gen::def_template: declaration is not of class, function, struct, variable, or using type - %s", declaration.debug_str() );
	}

	CodeTemplate
	result              = (CodeTemplate) make_code();
	result->Type        = ECode::Template;
	result->ModuleFlags = mflags;
	result->Params      = params;
	result->Declaration = declaration;

	return result;
}

CodeType def_type( StrC name, Code arrayexpr, CodeSpecifiers specifiers, CodeAttributes attributes )
{
	name_check( def_type, name );

	if ( attributes && attributes->Type != ECode::PlatformAttributes )
	{
		log_failure( "gen::def_type: attributes is not of attributes type - %s", attributes.debug_str() );
		return CodeInvalid;
	}

	if ( specifiers && specifiers->Type != ECode::Specifiers )
	{
		log_failure( "gen::def_type: specifiers is not of specifiers type - %s", specifiers.debug_str() );
		return CodeInvalid;
	}

	if ( arrayexpr && arrayexpr->Type != ECode::Untyped )
	{
		log_failure( "gen::def_type: arrayexpr is not of untyped type - %s", arrayexpr->debug_str() );
		return CodeInvalid;
	}

	CodeType
	result       = (CodeType) make_code();
	result->Name = get_cached_string( name );
	result->Type = ECode::Typename;

	if ( attributes )
		result->Attributes = attributes;

	if ( specifiers )
		result->Specs = specifiers;

	if ( arrayexpr )
		result->ArrExpr = arrayexpr;

	return result;
}

CodeTypedef def_typedef( StrC name, Code type, CodeAttributes attributes, ModuleFlag mflags )
{
	using namespace ECode;

	null_check( def_typedef, type );

	switch ( type->Type )
	{
		case Class:
		case Class_Fwd:
		case Enum:
		case Enum_Fwd:
		case Enum_Class:
		case Enum_Class_Fwd:
		case Function_Fwd:
		case Struct:
		case Struct_Fwd:
		case Union:
		case Typename:
			break;
		default:
			log_failure( "gen::def_typedef: type was not a Class, Enum, Function Forward, Struct, Typename, or Union - %s", type.debug_str() );
			return CodeInvalid;
	}

	if ( attributes && attributes->Type != ECode::PlatformAttributes )
	{
		log_failure( "gen::def_typedef: attributes was not a PlatformAttributes - %s", attributes.debug_str() );
		return CodeInvalid;
	}

	// Registering the type.
	Code registered_type = def_type( name );

	if ( ! registered_type )
	{
		log_failure( "gen::def_typedef: failed to register type" );
		return CodeInvalid;
	}

	CodeTypedef
	result              = (CodeTypedef) make_code();
	result->Type        = ECode::Typedef;
	result->ModuleFlags = mflags;

	result->UnderlyingType = type;

	if ( name.Len <= 0  )
	{
		if (type->Type != Untyped)
		{
			log_failure( "gen::def_typedef: name was empty and type was not untyped (indicating its a function typedef) - %s", type.debug_str() );
			return CodeInvalid;
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

CodeUnion def_union( StrC name, Code body, CodeAttributes attributes, ModuleFlag mflags )
{
	null_check( def_union, body );

	if ( body->Type != ECode::Union_Body )
	{
		log_failure( "gen::def_union: body was not a Union_Body type - %s", body.debug_str() );
		return CodeInvalid;
	}

	if ( attributes && attributes->Type != ECode::PlatformAttributes )
	{
		log_failure( "gen::def_union: attributes was not a PlatformAttributes type - %s", attributes.debug_str() );
		return CodeInvalid;
	}

	CodeUnion
	result              = (CodeUnion) make_code();
	result->ModuleFlags = mflags;
	result->Type        = ECode::Union;

	if ( name.Ptr )
		result->Name = get_cached_string( name );

	result->Body = body;

	if ( attributes )
		result->Attributes = attributes;

	return result;
}

CodeUsing def_using( StrC name, CodeType type
	, CodeAttributes attributes
	, ModuleFlag     mflags )
{
	name_check( def_using, name );
	null_check( def_using, type );

	Code register_type = def_type( name );

	if ( ! register_type )
	{
		log_failure( "gen::def_using: failed to register type" );
		return CodeInvalid;
	}

	if ( attributes && attributes->Type != ECode::PlatformAttributes )
	{
		log_failure( "gen::def_using: attributes was not a PlatformAttributes type - %s", attributes.debug_str() );
		return CodeInvalid;
	}

	CodeUsing
	result              = (CodeUsing) make_code();
	result->Name        = get_cached_string( name );
	result->ModuleFlags = mflags;
	result->Type = ECode::Using;

	result->UnderlyingType = type;

	if ( attributes )
		result->Attributes = attributes;

	return result;
}

CodeUsing def_using_namespace( StrC name )
{
	name_check( def_using_namespace, name );

	Code
	result          = make_code();
	result->Name    = get_cached_string( name );
	result->Content = result->Name;
	result->Type    = ECode::Using_Namespace;

	return (CodeUsing) result;
}

CodeVar def_variable( CodeType type, StrC name, Code value
	, CodeSpecifiers specifiers, CodeAttributes attributes
	, ModuleFlag     mflags )
{
	name_check( def_variable, name );
	null_check( def_variable, type );

	if ( attributes && attributes->Type != ECode::PlatformAttributes )
	{
		log_failure( "gen::def_variable: attributes was not a `PlatformAttributes` type - %s", attributes.debug_str() );
		return CodeInvalid;
	}

	if ( specifiers && specifiers->Type != ECode::Specifiers )
	{
		log_failure( "gen::def_variable: specifiers was not a `Specifiers` type - %s", specifiers.debug_str() );
		return CodeInvalid;
	}

	if ( type->Type != ECode::Typename )
	{
		log_failure( "gen::def_variable: type was not a Typename - %s", type.debug_str() );
		return CodeInvalid;
	}

	if ( value && value->Type != ECode::Untyped )
	{
		log_failure( "gen::def_variable: value was not a `Untyped` type - %s", value.debug_str() );
		return CodeInvalid;
	}

	CodeVar
	result              = (CodeVar) make_code();
	result->Name        = get_cached_string( name );
	result->Type        = ECode::Variable;
	result->ModuleFlags = mflags;

	result->ValueType = type;

	if ( attributes )
		result->Attributes = attributes;

	if ( specifiers )
		result->Specs = specifiers;

	if ( value )
		result->Value = value;

	return result;
}

#pragma region Helper Macros for def_**_body functions
#define def_body_start( Name_ )                                               \
using namespace ECode;                                                        \
																			  \
if ( num <= 0 )                                                               \
{                                                                             \
	log_failure("gen::" stringize(Name_) ": num cannot be zero or negative"); \
	return CodeInvalid;                                                       \
}

#define def_body_code_array_start( Name_ )                                     \
using namespace ECode;                                                         \
																			   \
if ( num <= 0 )                                                                \
{                                                                              \
	log_failure("gen::" stringize(Name_) ": num cannot be zero or negative");  \
	return CodeInvalid;                                                        \
}                                                                              \
																			   \
if ( codes == nullptr )                                                        \
{                                                                              \
	log_failure("gen::" stringize(Name_)" : Provided a null array of codes");  \
	return CodeInvalid;                                                        \
}

#pragma endregion Helper Macros for def_**_body functions

CodeBody def_class_body( s32 num, ... )
{
	def_body_start( def_class_body );

	CodeBody result = ( CodeBody )make_code();
	result->Type    = Class_Body;

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
			return CodeInvalid;
		}

		switch (entry->Type)
		{
			GEN_AST_BODY_CLASS_UNALLOWED_TYPES
				log_failure("gen::" "def_class_body" ": Entry type is not allowed: %s", entry.debug_str());
				return CodeInvalid;

			default:
			break;
		}

		result.append(entry);
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
	result->Type = Function_Body;

	do
	{
		Code entry = *codes;
		codes++;

		if (!entry)
		{
			log_failure("gen::" "def_class_body" ": Provided an null entry");
			return CodeInvalid;
		}

		switch (entry->Type)
		{
			GEN_AST_BODY_CLASS_UNALLOWED_TYPES
				log_failure("gen::" "def_class_body" ": Entry type is not allowed: %s", entry.debug_str());
				return CodeInvalid;

			default:
			break;
		}

		result.append(entry);
	}
	while (num--, num > 0);

	return result;
}

CodeBody def_enum_body( s32 num, ... )
{
	def_body_start( def_enum_body );

	CodeBody
	result       = (CodeBody) make_code();
	result->Type = Enum_Body;

	va_list va;
	va_start(va, num);
	do
	{
		Code_POD pod   = va_arg(va, Code_POD);
		Code     entry = pcast(Code, pod);

		if ( ! entry )
		{
			log_failure("gen::def_enum_body: Provided a null entry");
			return CodeInvalid;
		}

		if ( entry->Type != Untyped && entry->Type != Comment )
		{
			log_failure("gen::def_enum_body: Entry type is not allowed - %s. Must be of untyped or comment type.", entry.debug_str() );
			return CodeInvalid;
		}

		result.append( entry );
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
	result->Type = Enum_Body;

	do
	{
		Code entry = *codes;

		if ( ! entry )
		{
			log_failure("gen::def_enum_body: Provided a null entry");
			return CodeInvalid;
		}

		if ( entry->Type != Untyped && entry->Type != Comment )
		{
			log_failure("gen::def_enum_body: Entry type is not allowed: %s", entry.debug_str() );
			return CodeInvalid;
		}

		result.append( entry );
	}
	while ( codes++, num--, num > 0 );

	return result;
}

CodeBody def_export_body( s32 num, ... )
{
	def_body_start( def_export_body );

	CodeBody
	result       = (CodeBody) make_code();
	result->Type = Export_Body;

	va_list va;
	va_start(va, num);
	do
	{
		Code_POD pod = va_arg(va, Code_POD);
		Code     entry = pcast(Code, pod);

		if (!entry)
		{
			log_failure("gen::" "def_export_body" ": Provided an null entry");
			return CodeInvalid;
		}

		switch (entry->Type)
		{
			GEN_AST_BODY_EXPORT_UNALLOWED_TYPES
				log_failure("gen::" "def_export_body" ": Entry type is not allowed: %s", entry.debug_str());
				return CodeInvalid;

			default:
			break;
		}

		result.append(entry);
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
	result->Type = Export_Body;

	do
	{
		Code entry = *codes;
		codes++;

		if (!entry)
		{
			log_failure("gen::" "def_export_body" ": Provided an null entry");
			return CodeInvalid;
		}

		switch (entry->Type)
		{
			GEN_AST_BODY_EXPORT_UNALLOWED_TYPES
				log_failure("gen::" "def_export_body" ": Entry type is not allowed: %s", entry.debug_str());
				return CodeInvalid;

			default:
			break;
		}

		result.append(entry);
	}
	while (num--, num > 0);

	return result;
}

CodeBody def_extern_link_body( s32 num, ... )
{
	def_body_start( def_extern_linkage_body );

	CodeBody
	result       = (CodeBody) make_code();
	result->Type = Extern_Linkage_Body;

	va_list va;
	va_start(va, num);
	do
	{
		Code_POD pod = va_arg(va, Code_POD);
		Code entry = pcast(Code, pod);

		if (!entry)
		{
			log_failure("gen::" "def_extern_linkage_body" ": Provided an null entry");
			return CodeInvalid;
		}

		switch (entry->Type)
		{
			GEN_AST_BODY_EXTERN_LINKAGE_UNALLOWED_TYPES
				log_failure("gen::" "def_extern_linkage_body" ": Entry type is not allowed: %s", entry.debug_str());
				return CodeInvalid;

			default:
			break;
		}

		result.append(entry);
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
	result->Type = Extern_Linkage_Body;

	do
	{
		Code entry = *codes;
		codes++;

		if (!entry)
		{
			log_failure("gen::" "def_extern_linkage_body" ": Provided an null entry");
			return CodeInvalid;
		}

		switch (entry->Type)
		{
			GEN_AST_BODY_EXTERN_LINKAGE_UNALLOWED_TYPES
				log_failure("gen::" "def_extern_linkage_body" ": Entry type is not allowed: %s", entry.debug_str());
				return CodeInvalid;

			default:
			break;
		}

		result.append(entry);

	}
	while (num--, num > 0);

	return result;
}

CodeBody def_function_body( s32 num, ... )
{
	def_body_start( def_function_body );

	CodeBody
	result       = (CodeBody) make_code();
	result->Type = Function_Body;

	va_list va;
	va_start(va, num);
	do
	{
		Code_POD pod   = va_arg(va, Code_POD);
		Code     entry = pcast(Code, pod);

		if (!entry)
		{
			log_failure("gen::" stringize(def_function_body) ": Provided an null entry");
			return CodeInvalid;
		}

		switch (entry->Type)
		{

			GEN_AST_BODY_FUNCTION_UNALLOWED_TYPES
				log_failure("gen::" stringize(def_function_body) ": Entry type is not allowed: %s", entry.debug_str());
				return CodeInvalid;

			default:
			break;
		}

		result.append(entry);
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
	result->Type = Function_Body;

	do
	{
		Code entry = *codes;
		codes++;

		if (!entry)
		{
			log_failure("gen::" "def_function_body" ": Provided an null entry");
			return CodeInvalid;
		}

		switch (entry->Type)
		{
			GEN_AST_BODY_FUNCTION_UNALLOWED_TYPES
				log_failure("gen::" "def_function_body" ": Entry type is not allowed: %s", entry.debug_str());
				return CodeInvalid;

			default:
			break;
		}
		result.append(entry);
	}
	while (num--, num > 0);

	return result;
}

CodeBody def_global_body( s32 num, ... )
{
	def_body_start( def_global_body );

	CodeBody
	result       = (CodeBody) make_code();
	result->Type = Global_Body;

	va_list va;
	va_start(va, num);
	do
	{
		Code_POD pod   = va_arg(va, Code_POD);
		Code     entry = pcast(Code, pod);

		if (!entry)
		{
			log_failure("gen::" "def_global_body" ": Provided an null entry");
			return CodeInvalid;
		}

		switch (entry->Type)
		{
			case Global_Body:
				result.append( entry.code_cast<CodeBody>() ) ;
				continue;

			GEN_AST_BODY_GLOBAL_UNALLOWED_TYPES
				log_failure("gen::" "def_global_body" ": Entry type is not allowed: %s", entry.debug_str());
				return (*Code::Invalid.ast);

			default:
			break;
		}

		result.append(entry);
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
	result->Type = Global_Body;

	do
	{
		Code entry = *codes;
		codes++;

		if (!entry)
		{
			log_failure("gen::" "def_global_body" ": Provided an null entry");
			return CodeInvalid;
		}

		switch (entry->Type)
		{
			case Global_Body:
				result.append( entry.code_cast<CodeBody>() ) ;
				continue;

			GEN_AST_BODY_GLOBAL_UNALLOWED_TYPES
				log_failure("gen::" "def_global_body" ": Entry type is not allowed: %s", entry.debug_str());
				return CodeInvalid;

			default:
			break;
		}

		result.append(entry);
	}
	while (num--, num > 0);

	return result;
}

CodeBody def_namespace_body( s32 num, ... )
{
	def_body_start( def_namespace_body );

	CodeBody
	result       = (CodeBody) make_code();
	result->Type = Namespace_Body;

	va_list va;
	va_start(va, num);
	do
	{
		Code_POD pod   = va_arg(va, Code_POD);
		Code     entry = pcast(Code, pod);

		if (!entry)
		{
			log_failure("gen::" "def_namespace_body" ": Provided an null entry");
			return CodeInvalid;
		}

		switch (entry->Type)
		{
			GEN_AST_BODY_NAMESPACE_UNALLOWED_TYPES
				log_failure("gen::" "def_namespace_body" ": Entry type is not allowed: %s", entry.debug_str());
				return CodeInvalid;

			default:
			break;
		}

		result.append(entry);
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
	result->Type = Global_Body;

	do
	{
		Code entry = *codes;
		codes++;

		if (!entry)
		{
			log_failure("gen::" "def_namespace_body" ": Provided an null entry");
			return CodeInvalid;
		}

		switch (entry->Type)
		{
			GEN_AST_BODY_NAMESPACE_UNALLOWED_TYPES
				log_failure("gen::" "def_namespace_body" ": Entry type is not allowed: %s", entry.debug_str() );
				return CodeInvalid;

			default: break;
		}

		result.append(entry);
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

	if ( param->Type != Parameters )
	{
		log_failure( "gen::def_params: param %d is not a Parameters", num - num + 1 );
		return CodeInvalid;
	}

	CodeParam result = (CodeParam) param.duplicate();

	while ( -- num )
	{
		pod   = va_arg(va, Code_POD);
		param = pcast( CodeParam, pod );

		if ( param->Type != Parameters )
		{
			log_failure( "gen::def_params: param %d is not a Parameters", num - num + 1 );
			return CodeInvalid;
		}

		result.append( param );
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
		return CodeInvalid;                                                                                     \
	}                                                                                                           \
																												\
	if (current->Type != Parameters )                                                                           \
	{                                                                                                           \
		log_failure("gen::def_params: Code in coes array is not of paramter type - %s", current.debug_str() );  \
		return CodeInvalid;                                                                                     \
	}

	CodeParam current = (CodeParam) codes->duplicate();
	check_current();

	CodeParam
	result            = (CodeParam) make_code();
	result->Name      = current->Name;
	result->Type      = current->Type;
	result->ValueType = current->ValueType;

	while( codes++, current = * codes, num--, num > 0 )
	{
		check_current();
		result.append( current );
	}
#	undef check_current

	return result;
}

CodeSpecifiers def_specifiers( s32 num, ... )
{
	if ( num <= 0 )
	{
		log_failure("gen::def_specifiers: num cannot be zero or less");
		return CodeInvalid;
	}

	if ( num > AST::ArrSpecs_Cap )
	{
		log_failure("gen::def_specifiers: num of speciifers to define AST larger than AST specicifier capacity - %d", num);
		return CodeInvalid;
	}

	CodeSpecifiers
	result       = (CodeSpecifiers) make_code();
	result->Type = ECode::Specifiers;

	va_list va;
	va_start(va, num);
	do
	{
		SpecifierT type = (SpecifierT)va_arg(va, int);

		result.append( type );
	}
	while ( --num, num );
	va_end(va);

	return result;
}

CodeSpecifiers def_specifiers( s32 num, SpecifierT* specs )
{
	if ( num <= 0 )
	{
		log_failure("gen::def_specifiers: num cannot be zero or less");
		return CodeInvalid;
	}

	if ( num > AST::ArrSpecs_Cap )
	{
		log_failure("gen::def_specifiers: num of speciifers to define AST larger than AST specicifier capacity - %d", num);
		return CodeInvalid;
	}

	CodeSpecifiers
	result       = (CodeSpecifiers) make_code();
	result->Type = ECode::Specifiers;

	s32 idx = 0;
	do
	{
		result.append( specs[idx] );
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
	result->Type = Struct_Body;

	va_list va;
	va_start(va, num);
	do
	{
		Code_POD pod   = va_arg(va, Code_POD);
		Code     entry = pcast(Code, pod);

		if (!entry)
		{
			log_failure("gen::" "def_struct_body" ": Provided an null entry");
			return CodeInvalid;
		}

		switch (entry->Type)
		{
			GEN_AST_BODY_STRUCT_UNALLOWED_TYPES
				log_failure("gen::" "def_struct_body" ": Entry type is not allowed: %s", entry.debug_str());
				return CodeInvalid;

			default:
			break;
		}

		result.append(entry);
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
	result->Type = Struct_Body;

	do
	{
		Code entry = *codes;
		codes++;

		if (!entry)
		{
			log_failure("gen::" "def_struct_body" ": Provided an null entry");
			return CodeInvalid;
		}

		switch (entry->Type)
		{
			GEN_AST_BODY_STRUCT_UNALLOWED_TYPES
				log_failure("gen::" "def_struct_body" ": Entry type is not allowed: %s", entry.debug_str() );
				return CodeInvalid;

			default:
			break;
		}

		result.append(entry);
	}
	while (num--, num > 0);

	return result;
}

CodeBody def_union_body( s32 num, ... )
{
	def_body_start( def_union_body );

	CodeBody
	result       = (CodeBody) make_code();
	result->Type = Union_Body;

	va_list va;
	va_start(va, num);
	do
	{
		Code_POD pod   = va_arg(va, Code_POD);
		Code     entry = pcast( Code, pod );

		if ( ! entry )
		{
			log_failure("gen::def_union_body: Provided a null entry");
			return CodeInvalid;
		}

		if ( entry->Type != Untyped && entry->Type != Comment )
		{
			log_failure("gen::def_union_body: Entry type is not allowed - %s. Must be of untyped or comment type.", entry.debug_str() );
			return CodeInvalid;
		}

		result.append( entry );
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
	result->Type = Union_Body;

	do
	{
		Code entry = *codes;

		if ( ! entry )
		{
			log_failure("gen::def_union_body: Provided a null entry");
			return CodeInvalid;
		}

		if ( entry->Type != Untyped && entry->Type != Comment )
		{
			log_failure("gen::def_union_body: Entry type is not allowed: %s", entry.debug_str() );
			return CodeInvalid;
		}

		result.append( entry );
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
