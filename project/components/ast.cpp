#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "static_data.cpp"
#endif

Code Code::Global;
Code Code::Invalid;

// This serializes all the data-members in a "debug" format, where each member is printed with its associated value.
char const* debug_str(AST* self)
{
	GEN_ASSERT(self != nullptr);
	String  result_stack = string_make_reserve( GlobalAllocator, kilobytes(1) );
	String* result       = & result_stack;

	if ( self->Parent )
		append_fmt( result, "\n\tParent       : %S %S", self->Parent->type_str(), self->Name ? self->Name : "" );
	else
		append_fmt( result, "\n\tParent       : %S", "Null" );

	append_fmt( result, "\n\tName         : %S", self->Name ? self->Name : "Null" );
	append_fmt( result, "\n\tType         : %S", type_str(self) );
	append_fmt( result, "\n\tModule Flags : %S", to_str( self->ModuleFlags ) );

	switch ( self->Type )
	{
		using namespace ECode;

		case Invalid:
		case NewLine:
		case Access_Private:
		case Access_Protected:
		case Access_Public:
			if ( self->Prev )
				append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
		break;

		case Untyped:
		case Execution:
		case Comment:
		case PlatformAttributes:
		case Preprocess_Define:
		case Preprocess_Include:
		case Preprocess_Pragma:
		case Preprocess_If:
		case Preprocess_ElIf:
		case Preprocess_Else:
		case Preprocess_IfDef:
		case Preprocess_IfNotDef:
			if ( self->Prev )
				append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			append_fmt( result, "\n\tContent: %S", self->Content );
		break;

		case Class:
		case Struct:
			if ( self->Prev )
				append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			append_fmt( result, "\n\tInlineCmt   : %S", self->InlineCmt  ? self->InlineCmt->Content     : "Null" );
			append_fmt( result, "\n\tAttributes  : %S", self->Attributes ? to_string(self->Attributes)  : "Null" );
			append_fmt( result, "\n\tParentAccess: %s", self->ParentType ? to_str( self->ParentAccess ) : "No Parent" );
			append_fmt( result, "\n\tParentType  : %s", self->ParentType ? type_str(self->ParentType)   : "Null" );
			append_fmt( result, "\n\tBody        : %S", self->Body       ? debug_str(self->Body)        : "Null" );
		break;

		case Class_Fwd:
		case Struct_Fwd:
			if ( self->Prev )
				append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			append_fmt( result, "\n\tInlineCmt   : %S", self->InlineCmt  ? self->InlineCmt->Content     : "Null" );
			append_fmt( result, "\n\tAttributes  : %S", self->Attributes ? to_string(self->Attributes)  : "Null" );
			append_fmt( result, "\n\tParentAccess: %s", self->ParentType ? to_str( self->ParentAccess ) : "No Parent" );
			append_fmt( result, "\n\tParentType  : %s", self->ParentType ? type_str(self->ParentType)   : "Null" );
		break;

		case Constructor:
			if ( self->Prev )
				append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			append_fmt( result, "\n\tInlineCmt      : %S", self->InlineCmt       ? self->InlineCmt->Content         : "Null" );
			append_fmt( result, "\n\tSpecs          : %S", self->Specs           ? to_string(self->Specs)           : "Null" );
			append_fmt( result, "\n\tInitializerList: %S", self->InitializerList ? to_string(self->InitializerList) : "Null" );
			append_fmt( result, "\n\tParams         : %S", self->Params          ? to_string(self->Params)          : "Null" );
			append_fmt( result, "\n\tBody           : %S", self->Body            ? debug_str(self->Body)            : "Null" );
		break;

		case Constructor_Fwd:
			if ( self->Prev )
				append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			append_fmt( result, "\n\tInlineCmt      : %S", self->InlineCmt       ? self->InlineCmt->Content         : "Null" );
			append_fmt( result, "\n\tSpecs          : %S", self->Specs           ? to_string(self->Specs)           : "Null" );
			append_fmt( result, "\n\tInitializerList: %S", self->InitializerList ? to_string(self->InitializerList) : "Null" );
			append_fmt( result, "\n\tParams         : %S", self->Params          ? to_string(self->Params)          : "Null" );
		break;

		case Destructor:
			if ( self->Prev )
				append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			append_fmt( result, "\n\tInlineCmt      : %S", self->InlineCmt       ? self->InlineCmt->Content         : "Null" );
			append_fmt( result, "\n\tSpecs          : %S", self->Specs           ? to_string(self->Specs)           : "Null" );
			append_fmt( result, "\n\tBody           : %S", self->Body            ? debug_str(self->Body)            : "Null" );
		break;

		case Destructor_Fwd:
		break;

		case Enum:
		case Enum_Class:
			if ( self->Prev )
				append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			append_fmt( result, "\n\tInlineCmt       : %S", self->InlineCmt      ? self->InlineCmt->Content        : "Null" );
			append_fmt( result, "\n\tAttributes      : %S", self->Attributes     ? to_string(self->Attributes)     : "Null" );
			append_fmt( result, "\n\tUnderlying Type : %S", self->UnderlyingType ? to_string(self->UnderlyingType) : "Null" );
			append_fmt( result, "\n\tBody            : %S", self->Body           ? debug_str(self->Body)           : "Null" );
		break;

		case Enum_Fwd:
		case Enum_Class_Fwd:
			if ( self->Prev )
				append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			append_fmt( result, "\n\tInlineCmt       : %S", self->InlineCmt      ? self->InlineCmt->Content        : "Null" );
			append_fmt( result, "\n\tAttributes      : %S", self->Attributes     ? to_string(self->Attributes)     : "Null" );
			append_fmt( result, "\n\tUnderlying Type : %S", self->UnderlyingType ? to_string(self->UnderlyingType) : "Null" );
		break;

		case Extern_Linkage:
		case Namespace:
			if ( self->Prev )
				append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			append_fmt( result, "\n\tBody: %S", self->Body ? debug_str(self->Body) : "Null" );
		break;

		case Friend:
			if ( self->Prev )
				append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			append_fmt( result, "\n\tInlineCmt  : %S", self->InlineCmt   ? self->InlineCmt->Content     : "Null" );
			append_fmt( result, "\n\tDeclaration: %S", self->Declaration ? to_string(self->Declaration) : "Null" );
		break;

		case Function:
			if ( self->Prev )
				append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			append_fmt( result, "\n\tInlineCmt : %S", self->InlineCmt  ? self->InlineCmt->Content    : "Null" );
			append_fmt( result, "\n\tAttributes: %S", self->Attributes ? to_string(self->Attributes) : "Null" );
			append_fmt( result, "\n\tSpecs     : %S", self->Specs      ? to_string(self->Specs)      : "Null" );
			append_fmt( result, "\n\tReturnType: %S", self->ReturnType ? to_string(self->ReturnType) : "Null" );
			append_fmt( result, "\n\tParams    : %S", self->Params     ? to_string(self->Params)     : "Null" );
			append_fmt( result, "\n\tBody      : %S", self->Body       ? debug_str(self->Body)       : "Null" );
		break;

		case Function_Fwd:
			if ( self->Prev )
				append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			append_fmt( result, "\n\tInlineCmt : %S", self->InlineCmt  ? self->InlineCmt->Content    : "Null" );
			append_fmt( result, "\n\tAttributes: %S", self->Attributes ? to_string(self->Attributes) : "Null" );
			append_fmt( result, "\n\tSpecs     : %S", self->Specs      ? to_string(self->Specs)      : "Null" );
			append_fmt( result, "\n\tReturnType: %S", self->ReturnType ? to_string(self->ReturnType) : "Null" );
			append_fmt( result, "\n\tParams    : %S", self->Params     ? to_string(self->Params)     : "Null" );
		break;

		case Module:
			if ( self->Prev )
				append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
		break;

		case Operator:
		case Operator_Member:
			if ( self->Prev )
				append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			append_fmt( result, "\n\tInlineCmt : %S", self->InlineCmt  ? self->InlineCmt->Content    : "Null" );
			append_fmt( result, "\n\tAttributes: %S", self->Attributes ? to_string(self->Attributes) : "Null" );
			append_fmt( result, "\n\tSpecs     : %S", self->Specs      ? to_string(self->Specs)      : "Null" );
			append_fmt( result, "\n\tReturnType: %S", self->ReturnType ? to_string(self->ReturnType) : "Null" );
			append_fmt( result, "\n\tParams    : %S", self->Params     ? to_string(self->Params)     : "Null" );
			append_fmt( result, "\n\tBody      : %S", self->Body       ? debug_str(self->Body)       : "Null" );
			append_fmt( result, "\n\tOp        : %S", to_str( self->Op ) );
		break;

		case Operator_Fwd:
		case Operator_Member_Fwd:
			if ( self->Prev )
				append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			append_fmt( result, "\n\tInlineCmt : %S", self->InlineCmt  ? self->InlineCmt->Content    : "Null" );
			append_fmt( result, "\n\tAttributes: %S", self->Attributes ? to_string(self->Attributes) : "Null" );
			append_fmt( result, "\n\tSpecs     : %S", self->Specs      ? to_string(self->Specs)      : "Null" );
			append_fmt( result, "\n\tReturnType: %S", self->ReturnType ? to_string(self->ReturnType) : "Null" );
			append_fmt( result, "\n\tParams    : %S", self->Params     ? to_string(self->Params)     : "Null" );
			append_fmt( result, "\n\tOp        : %S", to_str( self->Op ) );
		break;

		case Operator_Cast:
			if ( self->Prev )
				append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			append_fmt( result, "\n\tInlineCmt : %S", self->InlineCmt  ? self->InlineCmt->Content   : "Null" );
			append_fmt( result, "\n\tSpecs     : %S", self->Specs      ? to_string(self->Specs)     : "Null" );
			append_fmt( result, "\n\tValueType : %S", self->ValueType  ? to_string(self->ValueType) : "Null" );
			append_fmt( result, "\n\tBody      : %S", self->Body       ? debug_str(self->Body)      : "Null" );
		break;

		case Operator_Cast_Fwd:
			if ( self->Prev )
				append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			append_fmt( result, "\n\tInlineCmt : %S", self->InlineCmt  ? self->InlineCmt->Content   : "Null" );
			append_fmt( result, "\n\tSpecs     : %S", self->Specs      ? to_string(self->Specs)     : "Null" );
			append_fmt( result, "\n\tValueType : %S", self->ValueType  ? to_string(self->ValueType) : "Null" );
		break;

		case Parameters:
			append_fmt( result, "\n\tNumEntries: %d", self->NumEntries );
			append_fmt( result, "\n\tLast      : %S", self->Last->Name );
			append_fmt( result, "\n\tNext      : %S", self->Next->Name );
			append_fmt( result, "\n\tValueType : %S", self->ValueType ? to_string(self->ValueType) : "Null" );
			append_fmt( result, "\n\tValue     : %S", self->Value     ? to_string(self->Value)     : "Null" );
		break;

		case Specifiers:
		{
			append_fmt( result, "\n\tNumEntries: %d", self->NumEntries );
			GEN_NS append( result, "\n\tArrSpecs: " );

			s32 idx  = 0;
			s32 left = self->NumEntries;
			while ( left-- )
			{
				StrC spec = ESpecifier::to_str( self->ArrSpecs[idx] );
				append_fmt( result, "%.*s, ", spec.Len, spec.Ptr );
				idx++;
			}
			append_fmt( result, "\n\tNextSpecs: %S", self->NextSpecs ? debug_str(self->NextSpecs) : "Null" );
		}
		break;

		case Template:
			if ( self->Prev )
				append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			append_fmt( result, "\n\tParams     : %S", self->Params      ? to_string(self->Params)      : "Null" );
			append_fmt( result, "\n\tDeclaration: %S", self->Declaration ? to_string(self->Declaration) : "Null" );
		break;

		case Typedef:
			if ( self->Prev )
				append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			append_fmt( result, "\n\tInlineCmt     : %S", self->InlineCmt      ? self->InlineCmt->Content        : "Null" );
			append_fmt( result, "\n\tUnderlyingType: %S", self->UnderlyingType ? to_string(self->UnderlyingType) : "Null" );
		break;

		case Typename:
			append_fmt( result, "\n\tAttributes     : %S", self->Attributes ? to_string(self->Attributes) : "Null" );
			append_fmt( result, "\n\tSpecs          : %S", self->Specs      ? to_string(self->Specs)      : "Null" );
			append_fmt( result, "\n\tReturnType     : %S", self->ReturnType ? to_string(self->ReturnType) : "Null" );
			append_fmt( result, "\n\tParams         : %S", self->Params     ? to_string(self->Params)     : "Null" );
			append_fmt( result, "\n\tArrExpr        : %S", self->ArrExpr    ? to_string(self->ArrExpr)    : "Null" );
		break;

		case Union:
			if ( self->Prev )
				append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			append_fmt( result, "\n\tAttributes: %S", self->Attributes ? to_string(self->Attributes) : "Null" );
			append_fmt( result, "\n\tBody      : %S", self->Body       ? debug_str(self->Body)       : "Null" );
		break;

		case Using:
			if ( self->Prev )
				append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			append_fmt( result, "\n\tInlineCmt     : %S", self->InlineCmt      ? self->InlineCmt->Content        : "Null" );
			append_fmt( result, "\n\tAttributes    : %S", self->Attributes     ? to_string(self->Attributes)     : "Null" );
			append_fmt( result, "\n\tUnderlyingType: %S", self->UnderlyingType ? to_string(self->UnderlyingType) : "Null" );
		break;

		case Variable:

			if ( self->Parent && self->Parent->Type == Variable )
			{
				// Its a NextVar
				append_fmt( result, "\n\tSpecs       : %S", self->Specs        ? to_string(self->Specs)        : "Null" );
				append_fmt( result, "\n\tValue       : %S", self->Value        ? to_string(self->Value)        : "Null" );
				append_fmt( result, "\n\tBitfieldSize: %S", self->BitfieldSize ? to_string(self->BitfieldSize) : "Null" );
				append_fmt( result, "\n\tNextVar     : %S", self->NextVar      ? debug_str(self->NextVar)      : "Null" );
				break;
			}

			if ( self->Prev )
				append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			append_fmt( result, "\n\tInlineCmt   : %S", self->InlineCmt    ? self->InlineCmt->Content      : "Null" );
			append_fmt( result, "\n\tAttributes  : %S", self->Attributes   ? to_string(self->Attributes)   : "Null" );
			append_fmt( result, "\n\tSpecs       : %S", self->Specs        ? to_string(self->Specs)        : "Null" );
			append_fmt( result, "\n\tValueType   : %S", self->ValueType    ? to_string(self->ValueType)    : "Null" );
			append_fmt( result, "\n\tBitfieldSize: %S", self->BitfieldSize ? to_string(self->BitfieldSize) : "Null" );
			append_fmt( result, "\n\tValue       : %S", self->Value        ? to_string(self->Value)        : "Null" );
			append_fmt( result, "\n\tNextVar     : %S", self->NextVar      ? debug_str(self->NextVar)      : "Null" );
		break;
	}

	return * result;
}

AST* duplicate(AST* self)
{
	using namespace ECode;

	AST* result = make_code().ast;

	mem_copy( result, self, sizeof( AST ) );

	result->Parent = nullptr;
	return result;
}

String AST::to_string()
{
	String result = string_make( GlobalAllocator, "" );
	to_string( result );
	return result;
}

void AST::to_string( String& result )
{
	local_persist thread_local
	char SerializationLevel = 0;

	switch ( Type )
	{
		using namespace ECode;

		case Invalid:
		#ifdef GEN_DONT_ALLOW_INVALID_CODE
			log_failure("Attempted to serialize invalid code! - %S", Parent ? Parent->debug_str() : Name );
		#else
			GEN_NS append_fmt( & result, "Invalid Code!" );
		#endif
		break;

		case NewLine:
			GEN_NS append( & result,"\n");
		break;

		case Untyped:
		case Execution:
		case Comment:
		case PlatformAttributes:
			GEN_NS append( & result, Content );
		break;

		case Access_Private:
		case Access_Protected:
		case Access_Public:
			GEN_NS append( & result, Name );
		break;

		case Class:
			code_cast<CodeClass>().to_string_def( result );
		break;

		case Class_Fwd:
			code_cast<CodeClass>().to_string_fwd( result );
		break;

		case Constructor:
			code_cast<CodeConstructor>().to_string_def( result );
		break;

		case Constructor_Fwd:
			code_cast<CodeConstructor>().to_string_fwd( result );
		break;

		case Destructor:
			code_cast<CodeDestructor>().to_string_def( result );
		break;

		case Destructor_Fwd:
			code_cast<CodeDestructor>().to_string_fwd( result );
		break;

		case Enum:
			code_cast<CodeEnum>().to_string_def( result );
		break;

		case Enum_Fwd:
			code_cast<CodeEnum>().to_string_fwd( result );
		break;

		case Enum_Class:
			code_cast<CodeEnum>().to_string_class_def( result );
		break;

		case Enum_Class_Fwd:
			code_cast<CodeEnum>().to_string_class_fwd( result );
		break;

		case Export_Body:
			code_cast<CodeBody>().to_string_export( result );
		break;

		case Extern_Linkage:
			code_cast<CodeExtern>().to_string( result );
		break;

		case Friend:
			code_cast<CodeFriend>().to_string( result );
		break;

		case Function:
			code_cast<CodeFn>().to_string_def( result );
		break;

		case Function_Fwd:
			code_cast<CodeFn>().to_string_fwd( result );
		break;

		case Module:
			code_cast<CodeModule>().to_string( result );
		break;

		case Namespace:
			code_cast<CodeNS>().to_string( result );
		break;

		case Operator:
		case Operator_Member:
			code_cast<CodeOperator>().to_string_def( result );
		break;

		case Operator_Fwd:
		case Operator_Member_Fwd:
			code_cast<CodeOperator>().to_string_fwd( result );
		break;

		case Operator_Cast:
			code_cast<CodeOpCast>().to_string_def( result );
		break;

		case Operator_Cast_Fwd:
			code_cast<CodeOpCast>().to_string_fwd( result );
		break;

		case Parameters:
			code_cast<CodeParam>().to_string( result );
		break;

		case Preprocess_Define:
			code_cast<CodeDefine>().to_string( result );
		break;

		case Preprocess_If:
			code_cast<CodePreprocessCond>().to_string_if( result );
		break;

		case Preprocess_IfDef:
			code_cast<CodePreprocessCond>().to_string_ifdef( result );
		break;

		case Preprocess_IfNotDef:
			code_cast<CodePreprocessCond>().to_string_ifndef( result );
		break;

		case Preprocess_Include:
			code_cast<CodeInclude>().to_string( result );
		break;

		case Preprocess_ElIf:
			code_cast<CodePreprocessCond>().to_string_elif( result );
		break;

		case Preprocess_Else:
			code_cast<CodePreprocessCond>().to_string_else( result );
		break;

		case Preprocess_EndIf:
			code_cast<CodePreprocessCond>().to_string_endif( result );
		break;

		case Preprocess_Pragma:
			code_cast<CodePragma>().to_string( result );
		break;

		case Specifiers:
			code_cast<CodeSpecifiers>().to_string( result );
		break;

		case Struct:
			code_cast<CodeStruct>().to_string_def( result );
		break;

		case Struct_Fwd:
			code_cast<CodeStruct>().to_string_fwd( result );
		break;

		case Template:
			code_cast<CodeTemplate>().to_string( result );
		break;

		case Typedef:
			code_cast<CodeTypedef>().to_string( result );
		break;

		case Typename:
			code_cast<CodeType>().to_string( result );
		break;

		case Union:
			code_cast<CodeUnion>().to_string( result );
		break;

		case Using:
			code_cast<CodeUsing>().to_string( result );
		break;

		case Using_Namespace:
			code_cast<CodeUsing>().to_string_ns( result );
		break;

		case Variable:
			code_cast<CodeVar>().to_string( result );
		break;

		case Enum_Body:
		case Class_Body:
		case Extern_Linkage_Body:
		case Function_Body:
		case Global_Body:
		case Namespace_Body:
		case Struct_Body:
		case Union_Body:
			code_cast<CodeBody>().to_string( result );
		break;
	}
}

bool AST::is_equal( AST* other )
{
/*
	AST values are either some u32 value, a cached string, or a pointer to another AST.

	u32 values are compared by value.
	Cached strings are compared by pointer.
	AST nodes are compared with AST::is_equal.
*/
	if ( other == nullptr )
	{
		log_fmt( "AST::is_equal: other is null\nAST: %S", debug_str() );
		return false;
	}

	if ( Type != other->Type )
	{
		log_fmt("AST::is_equal: Type check failure with other\nAST: %S\nOther: %S"
			, debug_str()
			, other->debug_str()
		);

		return false;
	}

	switch ( Type )
	{
		using namespace ECode;

	#define check_member_val( val )                           \
	if ( val != other->val )                                  \
	{                                                         \
		log_fmt("\nAST::is_equal: Member - " #val " failed\n" \
		        "AST  : %S\n"                                 \
		        "Other: %S\n"                                 \
		    , debug_str()                                     \
		    , other->debug_str()                              \
		);                                                    \
	                                                          \
		return false;                                         \
	}

	#define check_member_str( str )                                 \
	if ( str != other->str )                                        \
	{                                                               \
		log_fmt("\nAST::is_equal: Member string - "#str " failed\n" \
				"AST  : %S\n"                                       \
				"Other: %S\n"                                       \
			, debug_str()                                           \
			, other->debug_str()                                    \
		);                                                          \
	                                                                \
		return false;                                               \
	}

	#define check_member_content( content )                               \
	if ( content != other->content )                                      \
	{                                                                     \
		log_fmt("\nAST::is_equal: Member content - "#content " failed\n"  \
				"AST  : %S\n"                                             \
				"Other: %S\n"                                             \
			, debug_str()                                                 \
			, other->debug_str()                                          \
		);                                                                \
                                                                          \
		log_fmt("Content cannot be trusted to be unique with this check " \
			"so it must be verified by eye for now\n"                     \
			"AST   Content:\n%S\n"                                        \
			"Other Content:\n%S\n"                                        \
			, visualize_whitespace(content)                               \
			, visualize_whitespace(other->content)                        \
		);                                                                \
	}

	#define check_member_ast( ast )                                                                \
	if ( ast )                                                                                     \
	{                                                                                              \
		if ( other->ast == nullptr )                                                               \
		{                                                                                          \
			log_fmt("\nAST::is_equal: Failed for member " #ast " other equivalent param is null\n" \
					"AST  : %s\n"                                                                  \
					"Other: %s\n"                                                                  \
					"For ast member: %s\n"                                                         \
				, debug_str()                                                                      \
				, other->debug_str()                                                               \
				, ast->debug_str()                                                                 \
			);                                                                                     \
                                                                                                   \
			return false;                                                                          \
		}                                                                                          \
                                                                                                   \
		if ( ! ast->is_equal( other->ast ) )                                                       \
		{                                                                                          \
			log_fmt( "\nAST::is_equal: Failed for " #ast"\n"                                       \
			         "AST  : %S\n"                                                                 \
			         "Other: %S\n"                                                                 \
			         "For     ast member: %S\n"                                                    \
			         "other's ast member: %S\n"                                                    \
				, debug_str()                                                                      \
				, other->debug_str()                                                               \
				, ast->debug_str()                                                                 \
				, other->ast->debug_str()                                                          \
			);                                                                                     \
		                                                                                           \
			return false;                                                                          \
		}                                                                                          \
	}

		case NewLine:
		case Access_Public:
		case Access_Protected:
		case Access_Private:
		case Preprocess_Else:
		case Preprocess_EndIf:
			return true;


		// Comments are not validated.
		case Comment:
			return true;

		case Execution:
		case PlatformAttributes:
		case Untyped:
		{
			check_member_content( Content );

			return true;
		}

		case Class_Fwd:
		case Struct_Fwd:
		{
			check_member_str( Name );
			check_member_ast( ParentType );
			check_member_val( ParentAccess );
			check_member_ast( Attributes );

			return true;
		}

		case Class:
		case Struct:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( ParentType );
			check_member_val( ParentAccess );
			check_member_ast( Attributes );
			check_member_ast( Body );

			return true;
		}

		case Constructor:
		{
			check_member_ast( InitializerList );
			check_member_ast( Params );
			check_member_ast( Body );

			return true;
		}

		case Constructor_Fwd:
		{
			check_member_ast( InitializerList );
			check_member_ast( Params );

			return true;
		}

		case Destructor:
		{
			check_member_ast( Specs );
			check_member_ast( Body );

			return true;
		}

		case Destructor_Fwd:
		{
			check_member_ast( Specs );

			return true;
		}

		case Enum:
		case Enum_Class:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( Attributes );
			check_member_ast( UnderlyingType );
			check_member_ast( Body );

			return true;
		}

		case Enum_Fwd:
		case Enum_Class_Fwd:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( Attributes );
			check_member_ast( UnderlyingType );

			return true;
		}

		case Extern_Linkage:
		{
			check_member_str( Name );
			check_member_ast( Body );

			return true;
		}

		case Friend:
		{
			check_member_str( Name );
			check_member_ast( Declaration );

			return true;
		}

		case Function:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( ReturnType );
			check_member_ast( Attributes );
			check_member_ast( Specs );
			check_member_ast( Params );
			check_member_ast( Body );

			return true;
		}

		case Function_Fwd:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( ReturnType );
			check_member_ast( Attributes );
			check_member_ast( Specs );
			check_member_ast( Params );

			return true;
		}

		case Module:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );

			return true;
		}

		case Namespace:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( Body );

			return true;
		}

		case Operator:
		case Operator_Member:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( ReturnType );
			check_member_ast( Attributes );
			check_member_ast( Specs );
			check_member_ast( Params );
			check_member_ast( Body );

			return true;
		}

		case Operator_Fwd:
		case Operator_Member_Fwd:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( ReturnType );
			check_member_ast( Attributes );
			check_member_ast( Specs );
			check_member_ast( Params );

			return true;
		}

		case Operator_Cast:
		{
			check_member_str( Name );
			check_member_ast( Specs );
			check_member_ast( ValueType );
			check_member_ast( Body );

			return true;
		}

		case Operator_Cast_Fwd:
		{
			check_member_str( Name );
			check_member_ast( Specs );
			check_member_ast( ValueType );

			return true;
		}

		case Parameters:
		{
			if ( NumEntries > 1 )
			{
				AST* curr       = this;
				AST* curr_other = other;
				while ( curr != nullptr  )
				{
					if ( curr )
					{
						if ( curr_other == nullptr )
						{
							log_fmt("\nAST::is_equal: Failed for parameter, other equivalent param is null\n"
							        "AST  : %S\n"
							        "Other: %S\n"
							        "For ast member: %S\n"
							    , curr->debug_str()
							);

							return false;
						}

						if ( curr->Name != curr_other->Name )
						{
							log_fmt( "\nAST::is_equal: Failed for parameter name check\n"
									"AST  : %S\n"
									"Other: %S\n"
									"For     ast member: %S\n"
									"other's ast member: %S\n"
								, debug_str()
								, other->debug_str()
								, curr->debug_str()
								, curr_other->debug_str()
							);
							return false;
						}

						if ( curr->ValueType && ! curr->ValueType->is_equal(curr_other->ValueType) )
						{
							log_fmt( "\nAST::is_equal: Failed for parameter value type check\n"
									"AST  : %S\n"
									"Other: %S\n"
									"For     ast member: %S\n"
									"other's ast member: %S\n"
								, debug_str()
								, other->debug_str()
								, curr->debug_str()
								, curr_other->debug_str()
							);
							return false;
						}

						if ( curr->Value && ! curr->Value->is_equal(curr_other->Value) )
						{
							log_fmt( "\nAST::is_equal: Failed for parameter value check\n"
									"AST  : %S\n"
									"Other: %S\n"
									"For     ast member: %S\n"
									"other's ast member: %S\n"
								, debug_str()
								, other->debug_str()
								, curr->debug_str()
								, curr_other->debug_str()
							);
							return false;
						}
					}

					curr       = curr->Next;
					curr_other = curr_other->Next;
				}

				check_member_val( NumEntries );

				return true;
			}

			check_member_str( Name );
			check_member_ast( ValueType );
			check_member_ast( Value );
			check_member_ast( ArrExpr );

			return true;
		}

		case Preprocess_Define:
		{
			check_member_str( Name );
			check_member_content( Content );

			return true;
		}

		case Preprocess_If:
		case Preprocess_IfDef:
		case Preprocess_IfNotDef:
		case Preprocess_ElIf:
		{
			check_member_content( Content );

			return true;
		}

		case Preprocess_Include:
		case Preprocess_Pragma:
		{
			check_member_content( Content );

			return true;
		}

		case Specifiers:
		{
			check_member_val( NumEntries );
			check_member_str( Name );
			for ( s32 idx = 0; idx < NumEntries; ++idx )
			{
				check_member_val( ArrSpecs[ idx ] );
			}
			return true;
		}

		case Template:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( Params );
			check_member_ast( Declaration );

			return true;
		}

		case Typedef:
		{
			check_member_val( IsFunction );
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( Specs );
			check_member_ast( UnderlyingType );

			return true;
		}
		case Typename:
		{
			check_member_val( IsParamPack );
			check_member_str( Name );
			check_member_ast( Specs );
			check_member_ast( ArrExpr );

			return true;
		}

		case Union:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( Attributes );
			check_member_ast( Body );

			return true;
		}

		case Using:
		case Using_Namespace:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( UnderlyingType );
			check_member_ast( Attributes );

			return true;
		}

		case Variable:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( ValueType );
			check_member_ast( BitfieldSize );
			check_member_ast( Value );
			check_member_ast( Attributes );
			check_member_ast( Specs );
			check_member_ast( NextVar );

			return true;
		}

		case Class_Body:
		case Enum_Body:
		case Export_Body:
		case Global_Body:
		case Namespace_Body:
		case Struct_Body:
		case Union_Body:
		{
			check_member_ast( Front );
			check_member_ast( Back );

			AST* curr       = Front;
			AST* curr_other = other->Front;
			while ( curr != nullptr )
			{
				if ( curr_other == nullptr )
				{
					log_fmt("\nAST::is_equal: Failed for body, other equivalent param is null\n"
					        "AST  : %S\n"
					        "Other: %S\n"
					        "For ast member: %S\n"
					    , curr->debug_str()
					);

					return false;
				}

				if ( ! curr->is_equal( curr_other ) )
				{
					log_fmt( "\nAST::is_equal: Failed for body\n"
							"AST  : %S\n"
							"Other: %S\n"
							"For     ast member: %S\n"
							"other's ast member: %S\n"
						, debug_str()
						, other->debug_str()
						, curr->debug_str()
						, curr_other->debug_str()
					);

					return false;
				}

				curr       = curr->Next;
				curr_other = curr_other->Next;
			}

			check_member_val( NumEntries );

			return true;
		}

	#undef check_member_val
	#undef check_member_str
	#undef check_member_ast
	}

	return true;
}

bool AST::validate_body()
{
	using namespace ECode;

#define CheckEntries( Unallowed_Types )                                                               \
	do                                                                                                \
	{                                                                                                 \
		for ( Code entry : code_cast<CodeBody>() )                                                         \
		{                                                                                             \
			switch ( entry->Type )                                                                    \
			{                                                                                         \
				Unallowed_Types                                                                       \
					log_failure( "AST::validate_body: Invalid entry in body %s", entry.debug_str() ); \
					return false;                                                                     \
			}                                                                                         \
		}                                                                                             \
	}                                                                                                 \
	while (0);

	switch ( Type )
	{
		case Class_Body:
			CheckEntries( GEN_AST_BODY_CLASS_UNALLOWED_TYPES );
		break;
		case Enum_Body:
			for ( Code entry : code_cast<CodeBody>() )
			{
				if ( entry->Type != Untyped )
				{
					log_failure( "AST::validate_body: Invalid entry in enum body (needs to be untyped or comment) %s", entry.debug_str() );
					return false;
				}
			}
		break;
		case Export_Body:
			CheckEntries( GEN_AST_BODY_CLASS_UNALLOWED_TYPES );
		break;
		case Extern_Linkage:
			CheckEntries( GEN_AST_BODY_EXTERN_LINKAGE_UNALLOWED_TYPES );
		break;
		case Function_Body:
			CheckEntries( GEN_AST_BODY_FUNCTION_UNALLOWED_TYPES );
		break;
		case Global_Body:
			for (Code entry : code_cast<CodeBody>())
			{
				switch (entry->Type)
				{
					case Access_Public:
					case Access_Protected:
					case Access_Private:
					case PlatformAttributes:
					case Class_Body:
					case Enum_Body:
					case Execution:
					case Friend:
					case Function_Body:
					case Global_Body:
					case Namespace_Body:
					case Operator_Member:
					case Operator_Member_Fwd:
					case Parameters:
					case Specifiers:
					case Struct_Body:
					case Typename:
						log_failure("AST::validate_body: Invalid entry in body %s", entry.debug_str());
					return false;
				}
			}
		break;
		case Namespace_Body:
			CheckEntries( GEN_AST_BODY_NAMESPACE_UNALLOWED_TYPES );
		break;
		case Struct_Body:
			CheckEntries( GEN_AST_BODY_STRUCT_UNALLOWED_TYPES );
		break;
		case Union_Body:
			for ( Code entry : Body->code_cast<CodeBody>() )
			{
				if ( entry->Type != Untyped )
				{
					log_failure( "AST::validate_body: Invalid entry in union body (needs to be untyped or comment) %s", entry.debug_str() );
					return false;
				}
			}
		break;

		default:
			log_failure( "AST::validate_body: Invalid this AST does not have a body %s", debug_str() );
			return false;
	}

	return false;

#undef CheckEntries
}
