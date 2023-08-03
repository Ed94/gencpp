// This is the non-bootstraped version of the Common AST Implementation. This will be obsolete once bootstrap is stress tested.

#pragma region AST Common

#define Define_CodeImpl( Typename )                                                  \
char const* Typename::debug_str()                                                    \
{                                                                                    \
	if ( ast == nullptr )                                                            \
		return "Code::debug_str: AST is null!";                                      \
	                                                                                 \
	return rcast(AST*, ast)->debug_str();                                            \
}                                                                                    \
Code Typename::duplicate()														     \
{                                                                                    \
	if ( ast == nullptr )                                                            \
	{                                                                                \
		log_failure("Code::duplicate: Cannot duplicate code, AST is null!");         \
		return Code::Invalid;                                                        \
	}                                                                                \
	                                                                                 \
	return { rcast(AST*, ast)->duplicate() };                                        \
}                                                                                    \
bool Typename::is_equal( Code other )                                                \
{                                                                                    \
	if ( ast == nullptr || other.ast == nullptr )                                    \
	{                                                                                \
		log_failure("Code::is_equal: Cannot compare code, AST is null!");            \
		return false;                                                                \
	}                                                                                \
                                                                                     \
	return rcast(AST*, ast)->is_equal( other.ast );                                  \
}                                                                                    \
bool Typename::is_valid()                                                            \
{                                                                                    \
	return (AST*) ast != nullptr && rcast( AST*, ast)->Type != CodeT::Invalid;       \
}                                                                                    \
void Typename::set_global()                                                          \
{                                                                                    \
	if ( ast == nullptr )                                                            \
	{                                                                                \
		log_failure("Code::set_global: Cannot set code as global, AST is null!");    \
		return;                                                                      \
	}                                                                                \
	                                                                                 \
	rcast(AST*, ast)->Parent = Code::Global.ast;                                     \
}                                                                                    \
String Typename::to_string()													     \
{                                                                                    \
	if ( ast == nullptr )                                                            \
	{                                                                                \
	log_failure("Code::to_string: Cannot convert code to string, AST is null!");     \
	return { nullptr };                                                              \
	}                                                                                \
	                                                                                 \
	return rcast(AST*, ast)->to_string();                                            \
}                                                                                    \
Typename& Typename::operator =( Code other )                                         \
{                                                                                    \
	if ( other.ast && other->Parent )                                                \
	{                                                                                \
		ast = rcast( decltype(ast), other.ast->duplicate() );                        \
		rcast( AST*, ast)->Parent = nullptr;                                         \
	}                                                                                \
                                                                                     \
	ast = rcast( decltype(ast), other.ast );                                         \
	return *this;                                                                    \
}                                                                                    \
bool Typename::operator ==( Code other )                                             \
{                                                                                    \
	return (AST*) ast == other.ast;                                                  \
}                                                                                    \
bool Typename::operator !=( Code other )                                             \
{                                                                                    \
	return (AST*) ast != other.ast;                                                  \
}                                                                                    \
Typename::operator bool()                                                            \
{                                                                                    \
	return ast != nullptr;                                                           \
}

Define_CodeImpl( Code );
Define_CodeImpl( CodeBody );
Define_CodeImpl( CodeAttributes );
Define_CodeImpl( CodeComment );
Define_CodeImpl( CodeClass );
Define_CodeImpl( CodeDefine );
Define_CodeImpl( CodeEnum );
Define_CodeImpl( CodeExec );
Define_CodeImpl( CodeExtern );
Define_CodeImpl( CodeInclude );
Define_CodeImpl( CodeFriend );
Define_CodeImpl( CodeFn );
Define_CodeImpl( CodeModule );
Define_CodeImpl( CodeNamespace );
Define_CodeImpl( CodeOperator );
Define_CodeImpl( CodeOpCast );
Define_CodeImpl( CodeParam );
Define_CodeImpl( CodePragma );
Define_CodeImpl( CodePreprocessCond );
Define_CodeImpl( CodeSpecifiers );
Define_CodeImpl( CodeStruct );
Define_CodeImpl( CodeTemplate );
Define_CodeImpl( CodeType );
Define_CodeImpl( CodeTypedef );
Define_CodeImpl( CodeUnion );
Define_CodeImpl( CodeUsing );
Define_CodeImpl( CodeVar );
#undef Define_CodeImpl

#define Define_AST_Cast( typename )              \
AST::operator Code ## typename()                 \
{                                                \
	return { rcast( AST_ ## typename*, this ) }; \
}

Define_AST_Cast( Body );
Define_AST_Cast( Attributes );
Define_AST_Cast( Comment );
Define_AST_Cast( Class );
Define_AST_Cast( Define );
Define_AST_Cast( Enum );
Define_AST_Cast( Exec );
Define_AST_Cast( Extern );
Define_AST_Cast( Include );
Define_AST_Cast( Friend );
Define_AST_Cast( Fn );
Define_AST_Cast( Module );
Define_AST_Cast( Namespace );
Define_AST_Cast( Operator );
Define_AST_Cast( OpCast );
Define_AST_Cast( Param );
Define_AST_Cast( Pragma );
Define_AST_Cast( PreprocessCond );
Define_AST_Cast( Struct );
Define_AST_Cast( Specifiers );
Define_AST_Cast( Template );
Define_AST_Cast( Type );
Define_AST_Cast( Typedef );
Define_AST_Cast( Union );
Define_AST_Cast( Using );
Define_AST_Cast( Var );
#undef Define_AST_Cast

#define Define_CodeCast( type )     \
Code::operator Code ## type() const \
{                                   \
	return { (AST_ ## type*) ast }; \
}

Define_CodeCast( Attributes );
Define_CodeCast( Comment );
Define_CodeCast( Class );
Define_CodeCast( Define );
Define_CodeCast( Exec );
Define_CodeCast( Enum );
Define_CodeCast( Extern );
Define_CodeCast( Include );
Define_CodeCast( Friend );
Define_CodeCast( Fn );
Define_CodeCast( Module );
Define_CodeCast( Namespace );
Define_CodeCast( Operator );
Define_CodeCast( OpCast );
Define_CodeCast( Param );
Define_CodeCast( Pragma );
Define_CodeCast( PreprocessCond );
Define_CodeCast( Specifiers );
Define_CodeCast( Struct );
Define_CodeCast( Template );
Define_CodeCast( Type );
Define_CodeCast( Typedef );
Define_CodeCast( Union );
Define_CodeCast( Using );
Define_CodeCast( Var );
Define_CodeCast( Body);
#undef Define_CodeCast

#pragma endregion AST Common

