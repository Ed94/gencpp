#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "static_data.cpp"
#endif

global Code Code_Global;
global Code Code_Invalid;

// This serializes all the data-members in a "debug" format, where each member is printed with its associated value.
char const* debug_str(Code self)
{
	GEN_ASSERT(self != nullptr);
	String  result_stack = string_make_reserve( GlobalAllocator, kilobytes(1) );
	String* result       = & result_stack;

	if ( self->Parent )
		string_append_fmt( result, "\n\tParent       : %S %S", type_str(self->Parent), self->Name ? self->Name : "" );
	else
		string_append_fmt( result, "\n\tParent       : %S", "Null" );

	string_append_fmt( result, "\n\tName         : %S", self->Name ? self->Name : "Null" );
	string_append_fmt( result, "\n\tType         : %S", type_str(self) );
	string_append_fmt( result, "\n\tModule Flags : %S", to_str( self->ModuleFlags ) );

	switch ( self->Type )
	{
		case CT_Invalid:
		case CT_NewLine:
		case CT_Access_Private:
		case CT_Access_Protected:
		case CT_Access_Public:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
		break;

		case CT_Untyped:
		case CT_Execution:
		case CT_Comment:
		case CT_PlatformAttributes:
		case CT_Preprocess_Define:
		case CT_Preprocess_Include:
		case CT_Preprocess_Pragma:
		case CT_Preprocess_If:
		case CT_Preprocess_ElIf:
		case CT_Preprocess_Else:
		case CT_Preprocess_IfDef:
		case CT_Preprocess_IfNotDef:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			string_append_fmt( result, "\n\tContent: %S", self->Content );
		break;

		case CT_Class:
		case CT_Struct:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			string_append_fmt( result, "\n\tInlineCmt   : %S", self->InlineCmt  ? self->InlineCmt->Content     : "Null" );
			string_append_fmt( result, "\n\tAttributes  : %S", self->Attributes ? to_string(self->Attributes)  : "Null" );
			string_append_fmt( result, "\n\tParentAccess: %s", self->ParentType ? to_str( self->ParentAccess ) : "No Parent" );
			string_append_fmt( result, "\n\tParentType  : %s", self->ParentType ? type_str(self->ParentType)   : "Null" );
			string_append_fmt( result, "\n\tBody        : %S", self->Body       ? debug_str(self->Body)        : "Null" );
		break;

		case CT_Class_Fwd:
		case CT_Struct_Fwd:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			string_append_fmt( result, "\n\tInlineCmt   : %S", self->InlineCmt  ? self->InlineCmt->Content     : "Null" );
			string_append_fmt( result, "\n\tAttributes  : %S", self->Attributes ? to_string(self->Attributes)  : "Null" );
			string_append_fmt( result, "\n\tParentAccess: %s", self->ParentType ? to_str( self->ParentAccess ) : "No Parent" );
			string_append_fmt( result, "\n\tParentType  : %s", self->ParentType ? type_str(self->ParentType)   : "Null" );
		break;

		case CT_Constructor:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			string_append_fmt( result, "\n\tInlineCmt      : %S", self->InlineCmt       ? self->InlineCmt->Content         : "Null" );
			string_append_fmt( result, "\n\tSpecs          : %S", self->Specs           ? to_string(self->Specs)           : "Null" );
			string_append_fmt( result, "\n\tInitializerList: %S", self->InitializerList ? to_string(self->InitializerList) : "Null" );
			string_append_fmt( result, "\n\tParams         : %S", self->Params          ? to_string(self->Params)          : "Null" );
			string_append_fmt( result, "\n\tBody           : %S", self->Body            ? debug_str(self->Body)            : "Null" );
		break;

		case CT_Constructor_Fwd:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			string_append_fmt( result, "\n\tInlineCmt      : %S", self->InlineCmt       ? self->InlineCmt->Content         : "Null" );
			string_append_fmt( result, "\n\tSpecs          : %S", self->Specs           ? to_string(self->Specs)           : "Null" );
			string_append_fmt( result, "\n\tInitializerList: %S", self->InitializerList ? to_string(self->InitializerList) : "Null" );
			string_append_fmt( result, "\n\tParams         : %S", self->Params          ? to_string(self->Params)          : "Null" );
		break;

		case CT_Destructor:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			string_append_fmt( result, "\n\tInlineCmt      : %S", self->InlineCmt       ? self->InlineCmt->Content         : "Null" );
			string_append_fmt( result, "\n\tSpecs          : %S", self->Specs           ? to_string(self->Specs)           : "Null" );
			string_append_fmt( result, "\n\tBody           : %S", self->Body            ? debug_str(self->Body)            : "Null" );
		break;

		case CT_Destructor_Fwd:
		break;

		case CT_Enum:
		case CT_Enum_Class:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			string_append_fmt( result, "\n\tInlineCmt       : %S", self->InlineCmt      ? self->InlineCmt->Content        : "Null" );
			string_append_fmt( result, "\n\tAttributes      : %S", self->Attributes     ? to_string(self->Attributes)     : "Null" );
			string_append_fmt( result, "\n\tUnderlying Type : %S", self->UnderlyingType ? to_string(self->UnderlyingType) : "Null" );
			string_append_fmt( result, "\n\tBody            : %S", self->Body           ? debug_str(self->Body)           : "Null" );
		break;

		case CT_Enum_Fwd:
		case CT_Enum_Class_Fwd:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			string_append_fmt( result, "\n\tInlineCmt       : %S", self->InlineCmt      ? self->InlineCmt->Content        : "Null" );
			string_append_fmt( result, "\n\tAttributes      : %S", self->Attributes     ? to_string(self->Attributes)     : "Null" );
			string_append_fmt( result, "\n\tUnderlying Type : %S", self->UnderlyingType ? to_string(self->UnderlyingType) : "Null" );
		break;

		case CT_Extern_Linkage:
		case CT_Namespace:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			string_append_fmt( result, "\n\tBody: %S", self->Body ? debug_str(self->Body) : "Null" );
		break;

		case CT_Friend:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			string_append_fmt( result, "\n\tInlineCmt  : %S", self->InlineCmt   ? self->InlineCmt->Content     : "Null" );
			string_append_fmt( result, "\n\tDeclaration: %S", self->Declaration ? to_string(self->Declaration) : "Null" );
		break;

		case CT_Function:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			string_append_fmt( result, "\n\tInlineCmt : %S", self->InlineCmt  ? self->InlineCmt->Content    : "Null" );
			string_append_fmt( result, "\n\tAttributes: %S", self->Attributes ? to_string(self->Attributes) : "Null" );
			string_append_fmt( result, "\n\tSpecs     : %S", self->Specs      ? to_string(self->Specs)      : "Null" );
			string_append_fmt( result, "\n\tReturnType: %S", self->ReturnType ? to_string(self->ReturnType) : "Null" );
			string_append_fmt( result, "\n\tParams    : %S", self->Params     ? to_string(self->Params)     : "Null" );
			string_append_fmt( result, "\n\tBody      : %S", self->Body       ? debug_str(self->Body)       : "Null" );
		break;

		case CT_Function_Fwd:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			string_append_fmt( result, "\n\tInlineCmt : %S", self->InlineCmt  ? self->InlineCmt->Content    : "Null" );
			string_append_fmt( result, "\n\tAttributes: %S", self->Attributes ? to_string(self->Attributes) : "Null" );
			string_append_fmt( result, "\n\tSpecs     : %S", self->Specs      ? to_string(self->Specs)      : "Null" );
			string_append_fmt( result, "\n\tReturnType: %S", self->ReturnType ? to_string(self->ReturnType) : "Null" );
			string_append_fmt( result, "\n\tParams    : %S", self->Params     ? to_string(self->Params)     : "Null" );
		break;

		case CT_Module:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
		break;

		case CT_Operator:
		case CT_Operator_Member:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			string_append_fmt( result, "\n\tInlineCmt : %S", self->InlineCmt  ? self->InlineCmt->Content    : "Null" );
			string_append_fmt( result, "\n\tAttributes: %S", self->Attributes ? to_string(self->Attributes) : "Null" );
			string_append_fmt( result, "\n\tSpecs     : %S", self->Specs      ? to_string(self->Specs)      : "Null" );
			string_append_fmt( result, "\n\tReturnType: %S", self->ReturnType ? to_string(self->ReturnType) : "Null" );
			string_append_fmt( result, "\n\tParams    : %S", self->Params     ? to_string(self->Params)     : "Null" );
			string_append_fmt( result, "\n\tBody      : %S", self->Body       ? debug_str(self->Body)       : "Null" );
			string_append_fmt( result, "\n\tOp        : %S", to_str( self->Op ) );
		break;

		case CT_Operator_Fwd:
		case CT_Operator_Member_Fwd:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			string_append_fmt( result, "\n\tInlineCmt : %S", self->InlineCmt  ? self->InlineCmt->Content    : "Null" );
			string_append_fmt( result, "\n\tAttributes: %S", self->Attributes ? to_string(self->Attributes) : "Null" );
			string_append_fmt( result, "\n\tSpecs     : %S", self->Specs      ? to_string(self->Specs)      : "Null" );
			string_append_fmt( result, "\n\tReturnType: %S", self->ReturnType ? to_string(self->ReturnType) : "Null" );
			string_append_fmt( result, "\n\tParams    : %S", self->Params     ? to_string(self->Params)     : "Null" );
			string_append_fmt( result, "\n\tOp        : %S", to_str( self->Op ) );
		break;

		case CT_Operator_Cast:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			string_append_fmt( result, "\n\tInlineCmt : %S", self->InlineCmt  ? self->InlineCmt->Content   : "Null" );
			string_append_fmt( result, "\n\tSpecs     : %S", self->Specs      ? to_string(self->Specs)     : "Null" );
			string_append_fmt( result, "\n\tValueType : %S", self->ValueType  ? to_string(self->ValueType) : "Null" );
			string_append_fmt( result, "\n\tBody      : %S", self->Body       ? debug_str(self->Body)      : "Null" );
		break;

		case CT_Operator_Cast_Fwd:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			string_append_fmt( result, "\n\tInlineCmt : %S", self->InlineCmt  ? self->InlineCmt->Content   : "Null" );
			string_append_fmt( result, "\n\tSpecs     : %S", self->Specs      ? to_string(self->Specs)     : "Null" );
			string_append_fmt( result, "\n\tValueType : %S", self->ValueType  ? to_string(self->ValueType) : "Null" );
		break;

		case CT_Parameters:
			string_append_fmt( result, "\n\tNumEntries: %d", self->NumEntries );
			string_append_fmt( result, "\n\tLast      : %S", self->Last->Name );
			string_append_fmt( result, "\n\tNext      : %S", self->Next->Name );
			string_append_fmt( result, "\n\tValueType : %S", self->ValueType ? to_string(self->ValueType) : "Null" );
			string_append_fmt( result, "\n\tValue     : %S", self->Value     ? to_string(self->Value)     : "Null" );
		break;

		case CT_Specifiers:
		{
			string_append_fmt( result, "\n\tNumEntries: %d", self->NumEntries );
			string_append_c_str( result, "\n\tArrSpecs: " );

			s32 idx  = 0;
			s32 left = self->NumEntries;
			while ( left-- )
			{
				StrC spec = to_str( self->ArrSpecs[idx] );
				string_append_fmt( result, "%.*s, ", spec.Len, spec.Ptr );
				idx++;
			}
			string_append_fmt( result, "\n\tNextSpecs: %S", self->NextSpecs ? debug_str(self->NextSpecs) : "Null" );
		}
		break;

		case CT_Template:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			string_append_fmt( result, "\n\tParams     : %S", self->Params      ? to_string(self->Params)      : "Null" );
			string_append_fmt( result, "\n\tDeclaration: %S", self->Declaration ? to_string(self->Declaration) : "Null" );
		break;

		case CT_Typedef:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			string_append_fmt( result, "\n\tInlineCmt     : %S", self->InlineCmt      ? self->InlineCmt->Content        : "Null" );
			string_append_fmt( result, "\n\tUnderlyingType: %S", self->UnderlyingType ? to_string(self->UnderlyingType) : "Null" );
		break;

		case CT_Typename:
			string_append_fmt( result, "\n\tAttributes     : %S", self->Attributes ? to_string(self->Attributes) : "Null" );
			string_append_fmt( result, "\n\tSpecs          : %S", self->Specs      ? to_string(self->Specs)      : "Null" );
			string_append_fmt( result, "\n\tReturnType     : %S", self->ReturnType ? to_string(self->ReturnType) : "Null" );
			string_append_fmt( result, "\n\tParams         : %S", self->Params     ? to_string(self->Params)     : "Null" );
			string_append_fmt( result, "\n\tArrExpr        : %S", self->ArrExpr    ? to_string(self->ArrExpr)    : "Null" );
		break;

		case CT_Union:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			string_append_fmt( result, "\n\tAttributes: %S", self->Attributes ? to_string(self->Attributes) : "Null" );
			string_append_fmt( result, "\n\tBody      : %S", self->Body       ? debug_str(self->Body)       : "Null" );
		break;

		case CT_Using:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			string_append_fmt( result, "\n\tInlineCmt     : %S", self->InlineCmt      ? self->InlineCmt->Content        : "Null" );
			string_append_fmt( result, "\n\tAttributes    : %S", self->Attributes     ? to_string(self->Attributes)     : "Null" );
			string_append_fmt( result, "\n\tUnderlyingType: %S", self->UnderlyingType ? to_string(self->UnderlyingType) : "Null" );
		break;

		case CT_Variable:

			if ( self->Parent && self->Parent->Type == CT_Variable )
			{
				// Its a NextVar
				string_append_fmt( result, "\n\tSpecs       : %S", self->Specs        ? to_string(self->Specs)        : "Null" );
				string_append_fmt( result, "\n\tValue       : %S", self->Value        ? to_string(self->Value)        : "Null" );
				string_append_fmt( result, "\n\tBitfieldSize: %S", self->BitfieldSize ? to_string(self->BitfieldSize) : "Null" );
				string_append_fmt( result, "\n\tNextVar     : %S", self->NextVar      ? debug_str(self->NextVar)      : "Null" );
				break;
			}

			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %S %S", type_str(self->Prev), self->Prev->Name ? self->Prev->Name : "Null" );

			string_append_fmt( result, "\n\tInlineCmt   : %S", self->InlineCmt    ? self->InlineCmt->Content      : "Null" );
			string_append_fmt( result, "\n\tAttributes  : %S", self->Attributes   ? to_string(self->Attributes)   : "Null" );
			string_append_fmt( result, "\n\tSpecs       : %S", self->Specs        ? to_string(self->Specs)        : "Null" );
			string_append_fmt( result, "\n\tValueType   : %S", self->ValueType    ? to_string(self->ValueType)    : "Null" );
			string_append_fmt( result, "\n\tBitfieldSize: %S", self->BitfieldSize ? to_string(self->BitfieldSize) : "Null" );
			string_append_fmt( result, "\n\tValue       : %S", self->Value        ? to_string(self->Value)        : "Null" );
			string_append_fmt( result, "\n\tNextVar     : %S", self->NextVar      ? debug_str(self->NextVar)      : "Null" );
		break;
	}

	return * result;
}

Code duplicate(Code self)
{
	Code result = make_code();

	mem_copy( result.ast, self.ast, sizeof( AST ) );

	result->Parent = { nullptr };
	return result;
}

String to_string(Code self)
{
	String result = string_make_strc( GlobalAllocator, txt("") );
	GEN_NS to_string( self, & result );
	return result;
}

void to_string( Code self, String* result )
{
	GEN_ASSERT(self != nullptr);
	local_persist thread_local
	char SerializationLevel = 0;

	switch ( self->Type )
	{
		case CT_Invalid:
		#ifdef GEN_DONT_ALLOW_INVALID_CODE
			log_failure("Attempted to serialize invalid code! - %S", Parent ? Parent->debug_str() : Name );
		#else
			append_fmt( result, "Invalid Code!" );
		#endif
		break;

		case CT_NewLine:
			string_append_strc( result, txt("\n"));
		break;

		case CT_Untyped:
		case CT_Execution:
		case CT_Comment:
		case CT_PlatformAttributes:
			string_append_strc( result, self->Content );
		break;

		case CT_Access_Private:
		case CT_Access_Protected:
		case CT_Access_Public:
			string_append_strc( result, self->Name );
		break;

		case CT_Class:
			to_string_def(cast(CodeClass, self), result );
		break;

		case CT_Class_Fwd:
			to_string_fwd(cast(CodeClass, self), result );
		break;

		case CT_Constructor:
			to_string_def(cast(CodeConstructor, self), result );
		break;

		case CT_Constructor_Fwd:
			to_string_fwd(cast(CodeConstructor, self), result );
		break;

		case CT_Destructor:
			to_string_def(cast(CodeDestructor, self), result );
		break;

		case CT_Destructor_Fwd:
			to_string_fwd(cast(CodeDestructor, self), result );
		break;

		case CT_Enum:
			to_string_def(cast(CodeEnum, self), result );
		break;

		case CT_Enum_Fwd:
			to_string_fwd(cast(CodeEnum, self), result );
		break;

		case CT_Enum_Class:
			to_string_class_def(cast(CodeEnum, self), result );
		break;

		case CT_Enum_Class_Fwd:
			to_string_class_fwd(cast(CodeEnum, self), result );
		break;

		case CT_Export_Body:
			to_string_export(cast(CodeBody, self), result );
		break;

		case CT_Extern_Linkage:
			to_string(cast(CodeExtern, self), result );
		break;

		case CT_Friend:
			to_string(cast(CodeFriend, self), result );
		break;

		case CT_Function:
			to_string_def(cast(CodeFn, self), result );
		break;

		case CT_Function_Fwd:
			to_string_fwd(cast(CodeFn, self), result );
		break;

		case CT_Module:
			to_string(cast(CodeModule, self), result );
		break;

		case CT_Namespace:
			to_string(cast(CodeNS, self), result );
		break;

		case CT_Operator:
		case CT_Operator_Member:
			to_string_def(cast(CodeOperator, self), result );
		break;

		case CT_Operator_Fwd:
		case CT_Operator_Member_Fwd:
			to_string_fwd(cast(CodeOperator, self), result );
		break;

		case CT_Operator_Cast:
			to_string_def(cast(CodeOpCast, self), result );
		break;

		case CT_Operator_Cast_Fwd:
			to_string_fwd(cast(CodeOpCast, self), result );
		break;

		case CT_Parameters:
			to_string(cast(CodeParam, self), result );
		break;

		case CT_Preprocess_Define:
			to_string(cast(CodeDefine, self), result );
		break;

		case CT_Preprocess_If:
			to_string_if(cast(CodePreprocessCond, self), result );
		break;

		case CT_Preprocess_IfDef:
			to_string_ifdef(cast(CodePreprocessCond, self), result );
		break;

		case CT_Preprocess_IfNotDef:
			to_string_ifndef(cast(CodePreprocessCond, self), result );
		break;

		case CT_Preprocess_Include:
			to_string(cast(CodeInclude, self), result );
		break;

		case CT_Preprocess_ElIf:
			to_string_elif(cast(CodePreprocessCond, self), result );
		break;

		case CT_Preprocess_Else:
			to_string_else(cast(CodePreprocessCond, self), result );
		break;

		case CT_Preprocess_EndIf:
			to_string_endif(cast(CodePreprocessCond, self), result );
		break;

		case CT_Preprocess_Pragma:
			to_string(cast(CodePragma, self), result );
		break;

		case CT_Specifiers:
			to_string(cast(CodeSpecifiers, self), result );
		break;

		case CT_Struct:
			to_string_def(cast(CodeStruct, self), result );
		break;

		case CT_Struct_Fwd:
			to_string_fwd(cast(CodeStruct, self), result );
		break;

		case CT_Template:
			to_string(cast(CodeTemplate, self), result );
		break;

		case CT_Typedef:
			to_string(cast(CodeTypedef, self), result );
		break;

		case CT_Typename:
			to_string(cast(CodeTypename, self), result );
		break;

		case CT_Union:
			to_string( cast(CodeUnion, self), result );
		break;

		case CT_Using:
			to_string(cast(CodeUsing, self), result );
		break;

		case CT_Using_Namespace:
			to_string_ns(cast(CodeUsing, self), result );
		break;

		case CT_Variable:
			to_string(cast(CodeVar, self), result );
		break;

		case CT_Enum_Body:
		case CT_Class_Body:
		case CT_Extern_Linkage_Body:
		case CT_Function_Body:
		case CT_Global_Body:
		case CT_Namespace_Body:
		case CT_Struct_Body:
		case CT_Union_Body:
			to_string( cast(CodeBody, self), result );
		break;
	}
}

bool is_equal( Code self, Code other )
{
/*
	AST values are either some u32 value, a cached string, or a pointer to another AST.

	u32 values are compared by value.
	Cached strings are compared by pointer.
	AST nodes are compared with AST::is_equal.
*/
	if ( other == nullptr )
	{
		log_fmt( "AST::is_equal: other is null\nAST: %S", debug_str(self) );
		return false;
	}

	if ( self->Type != other->Type )
	{
		log_fmt("AST::is_equal: Type check failure with other\nAST: %S\nOther: %S"
			, debug_str(self)
			,debug_str(other)
		);

		return false;
	}

	switch ( self->Type )
	{
	#define check_member_val( val )                           \
	if ( self->val != other->val )                            \
	{                                                         \
		log_fmt("\nAST::is_equal: Member - " #val " failed\n" \
		        "AST  : %S\n"                                 \
		        "Other: %S\n"                                 \
		    , debug_str(self)                                 \
		    ,debug_str(other)                                 \
		);                                                    \
                                                              \
		return false;                                         \
	}

	#define check_member_str( str )                                 \
	if ( self->str != other->str )                                  \
	{                                                               \
		log_fmt("\nAST::is_equal: Member string - "#str " failed\n" \
				"AST  : %S\n"                                       \
				"Other: %S\n"                                       \
			, debug_str(self)                                       \
			,debug_str(other)                                       \
		);                                                          \
	                                                                \
		return false;                                               \
	}

	#define check_member_content( content )                                \
	if ( self->content != other->content )                                 \
	{                                                                      \
		log_fmt("\nAST::is_equal: Member content - "#content " failed\n"   \
				"AST  : %S\n"                                              \
				"Other: %S\n"                                              \
			, debug_str(self)                                              \
			, debug_str(other)                                             \
		);                                                                 \
                                                                           \
		log_fmt("Content cannot be trusted to be unique with this check "  \
			"so it must be verified by eye for now\n"                      \
			"AST   Content:\n%S\n"                                         \
			"Other Content:\n%S\n"                                         \
			, strc_visualize_whitespace(self->content, GlobalAllocator)    \
			, strc_visualize_whitespace(other->content, GlobalAllocator)   \
		);                                                                 \
	}

	#define check_member_ast( ast )                                                                \
	if ( self->ast )                                                                               \
	{                                                                                              \
		if ( other->ast == nullptr )                                                               \
		{                                                                                          \
			log_fmt("\nAST::is_equal: Failed for member " #ast " other equivalent param is null\n" \
					"AST  : %s\n"                                                                  \
					"Other: %s\n"                                                                  \
					"For ast member: %s\n"                                                         \
				, debug_str(self)                                                                  \
				, debug_str(other)                                                                 \
				, debug_str(self->ast)                                                             \
			);                                                                                     \
                                                                                                   \
			return false;                                                                          \
		}                                                                                          \
                                                                                                   \
		if ( ! is_equal(self->ast, other->ast ) )                                                  \
		{                                                                                          \
			log_fmt( "\nAST::is_equal: Failed for " #ast"\n"                                       \
			         "AST  : %S\n"                                                                 \
			         "Other: %S\n"                                                                 \
			         "For     ast member: %S\n"                                                    \
			         "other's ast member: %S\n"                                                    \
				, debug_str(self)                                                                  \
				, debug_str(other)                                                                 \
				, debug_str(self->ast)                                                             \
				, debug_str(other->ast)                                                            \
			);                                                                                     \
		                                                                                           \
			return false;                                                                          \
		}                                                                                          \
	}

		case CT_NewLine:
		case CT_Access_Public:
		case CT_Access_Protected:
		case CT_Access_Private:
		case CT_Preprocess_Else:
		case CT_Preprocess_EndIf:
			return true;


		// Comments are not validated.
		case CT_Comment:
			return true;

		case CT_Execution:
		case CT_PlatformAttributes:
		case CT_Untyped:
		{
			check_member_content( Content );
			return true;
		}

		case CT_Class_Fwd:
		case CT_Struct_Fwd:
		{
			check_member_str( Name );
			check_member_ast( ParentType );
			check_member_val( ParentAccess );
			check_member_ast( Attributes );

			return true;
		}

		case CT_Class:
		case CT_Struct:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( ParentType );
			check_member_val( ParentAccess );
			check_member_ast( Attributes );
			check_member_ast( Body );

			return true;
		}

		case CT_Constructor:
		{
			check_member_ast( InitializerList );
			check_member_ast( Params );
			check_member_ast( Body );

			return true;
		}

		case CT_Constructor_Fwd:
		{
			check_member_ast( InitializerList );
			check_member_ast( Params );

			return true;
		}

		case CT_Destructor:
		{
			check_member_ast( Specs );
			check_member_ast( Body );

			return true;
		}

		case CT_Destructor_Fwd:
		{
			check_member_ast( Specs );

			return true;
		}

		case CT_Enum:
		case CT_Enum_Class:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( Attributes );
			check_member_ast( UnderlyingType );
			check_member_ast( Body );

			return true;
		}

		case CT_Enum_Fwd:
		case CT_Enum_Class_Fwd:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( Attributes );
			check_member_ast( UnderlyingType );

			return true;
		}

		case CT_Extern_Linkage:
		{
			check_member_str( Name );
			check_member_ast( Body );

			return true;
		}

		case CT_Friend:
		{
			check_member_str( Name );
			check_member_ast( Declaration );

			return true;
		}

		case CT_Function:
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

		case CT_Function_Fwd:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( ReturnType );
			check_member_ast( Attributes );
			check_member_ast( Specs );
			check_member_ast( Params );

			return true;
		}

		case CT_Module:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );

			return true;
		}

		case CT_Namespace:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( Body );

			return true;
		}

		case CT_Operator:
		case CT_Operator_Member:
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

		case CT_Operator_Fwd:
		case CT_Operator_Member_Fwd:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( ReturnType );
			check_member_ast( Attributes );
			check_member_ast( Specs );
			check_member_ast( Params );

			return true;
		}

		case CT_Operator_Cast:
		{
			check_member_str( Name );
			check_member_ast( Specs );
			check_member_ast( ValueType );
			check_member_ast( Body );

			return true;
		}

		case CT_Operator_Cast_Fwd:
		{
			check_member_str( Name );
			check_member_ast( Specs );
			check_member_ast( ValueType );

			return true;
		}

		case CT_Parameters:
		{
			if ( self->NumEntries > 1 )
			{
				Code curr       = self;
				Code curr_other = other;
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
							    , debug_str(curr)
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
								, debug_str(self)
								, debug_str(other)
								, debug_str(curr)
								, debug_str(curr_other)
							);
							return false;
						}

						if ( curr->ValueType && ! is_equal(curr->ValueType, curr_other->ValueType) )
						{
							log_fmt( "\nAST::is_equal: Failed for parameter value type check\n"
									"AST  : %S\n"
									"Other: %S\n"
									"For     ast member: %S\n"
									"other's ast member: %S\n"
								, debug_str(self)
								, debug_str(other)
								, debug_str(curr)
								, debug_str(curr_other)
							);
							return false;
						}

						if ( curr->Value && ! is_equal(curr->Value, curr_other->Value) )
						{
							log_fmt( "\nAST::is_equal: Failed for parameter value check\n"
									"AST  : %S\n"
									"Other: %S\n"
									"For     ast member: %S\n"
									"other's ast member: %S\n"
								, debug_str(self)
								, debug_str(other)
								, debug_str(curr)
								, debug_str(curr_other)
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

		case CT_Preprocess_Define:
		{
			check_member_str( Name );
			check_member_content( Content );

			return true;
		}

		case CT_Preprocess_If:
		case CT_Preprocess_IfDef:
		case CT_Preprocess_IfNotDef:
		case CT_Preprocess_ElIf:
		{
			check_member_content( Content );

			return true;
		}

		case CT_Preprocess_Include:
		case CT_Preprocess_Pragma:
		{
			check_member_content( Content );

			return true;
		}

		case CT_Specifiers:
		{
			check_member_val( NumEntries );
			check_member_str( Name );
			for ( s32 idx = 0; idx < self->NumEntries; ++idx )
			{
				check_member_val( ArrSpecs[ idx ] );
			}
			return true;
		}

		case CT_Template:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( Params );
			check_member_ast( Declaration );

			return true;
		}

		case CT_Typedef:
		{
			check_member_val( IsFunction );
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( Specs );
			check_member_ast( UnderlyingType );

			return true;
		}
		case CT_Typename:
		{
			check_member_val( IsParamPack );
			check_member_str( Name );
			check_member_ast( Specs );
			check_member_ast( ArrExpr );

			return true;
		}

		case CT_Union:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( Attributes );
			check_member_ast( Body );

			return true;
		}

		case CT_Using:
		case CT_Using_Namespace:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( UnderlyingType );
			check_member_ast( Attributes );

			return true;
		}

		case CT_Variable:
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

		case CT_Class_Body:
		case CT_Enum_Body:
		case CT_Export_Body:
		case CT_Global_Body:
		case CT_Namespace_Body:
		case CT_Struct_Body:
		case CT_Union_Body:
		{
			check_member_ast( Front );
			check_member_ast( Back );

			Code curr       = self->Front;
			Code curr_other = other->Front;
			while ( curr != nullptr )
			{
				if ( curr_other == nullptr )
				{
					log_fmt("\nAST::is_equal: Failed for body, other equivalent param is null\n"
					        "AST  : %S\n"
					        "Other: %S\n"
					        "For ast member: %S\n"
					    , debug_str(curr)
					);

					return false;
				}

				if ( ! is_equal( curr, curr_other ) )
				{
					log_fmt( "\nAST::is_equal: Failed for body\n"
							"AST  : %S\n"
							"Other: %S\n"
							"For     ast member: %S\n"
							"other's ast member: %S\n"
						, debug_str(self)
						, debug_str(other)
						, debug_str(curr)
						, debug_str(curr_other)
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

bool validate_body(Code self)
{
#define CheckEntries( Unallowed_Types )                                                                     \
	do                                                                                                      \
	{                                                                                                       \
		for ( Code entry : cast(CodeBody, self) )                                                           \
		{                                                                                                   \
			switch ( entry->Type )                                                                          \
			{                                                                                               \
				Unallowed_Types                                                                             \
					log_failure( "AST::validate_body: Invalid entry in body %s", GEN_NS debug_str(entry) ); \
					return false;                                                                           \
			}                                                                                               \
		}                                                                                                   \
	}                                                                                                       \
	while (0);

	switch ( self->Type )
	{
		case CT_Class_Body:
			CheckEntries( GEN_AST_BODY_CLASS_UNALLOWED_TYPES );
		break;
		case CT_Enum_Body:
			for ( Code entry : cast(CodeBody, self) )
			{
				if ( entry->Type != CT_Untyped )
				{
					log_failure( "AST::validate_body: Invalid entry in enum body (needs to be untyped or comment) %s", GEN_NS debug_str(entry) );
					return false;
				}
			}
		break;
		case CT_Export_Body:
			CheckEntries( GEN_AST_BODY_CLASS_UNALLOWED_TYPES );
		break;
		case CT_Extern_Linkage:
			CheckEntries( GEN_AST_BODY_EXTERN_LINKAGE_UNALLOWED_TYPES );
		break;
		case CT_Function_Body:
			CheckEntries( GEN_AST_BODY_FUNCTION_UNALLOWED_TYPES );
		break;
		case CT_Global_Body:
			for (Code entry : cast(CodeBody, self))
			{
				switch (entry->Type)
				{
					case CT_Access_Public:
					case CT_Access_Protected:
					case CT_Access_Private:
					case CT_PlatformAttributes:
					case CT_Class_Body:
					case CT_Enum_Body:
					case CT_Execution:
					case CT_Friend:
					case CT_Function_Body:
					case CT_Global_Body:
					case CT_Namespace_Body:
					case CT_Operator_Member:
					case CT_Operator_Member_Fwd:
					case CT_Parameters:
					case CT_Specifiers:
					case CT_Struct_Body:
					case CT_Typename:
						log_failure("AST::validate_body: Invalid entry in body %s", GEN_NS debug_str(entry));
					return false;
				}
			}
		break;
		case CT_Namespace_Body:
			CheckEntries( GEN_AST_BODY_NAMESPACE_UNALLOWED_TYPES );
		break;
		case CT_Struct_Body:
			CheckEntries( GEN_AST_BODY_STRUCT_UNALLOWED_TYPES );
		break;
		case CT_Union_Body:
			for ( Code entry : cast(CodeBody, {self->Body}) )
			{
				if ( entry->Type != CT_Untyped )
				{
					log_failure( "AST::validate_body: Invalid entry in union body (needs to be untyped or comment) %s", GEN_NS debug_str(entry) );
					return false;
				}
			}
		break;

		default:
			log_failure( "AST::validate_body: Invalid this AST does not have a body %s", debug_str(self) );
			return false;
	}

	return false;

#undef CheckEntries
}
