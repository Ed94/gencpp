#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "static_data.cpp"
#endif

// This serializes all the data-members in a "debug" format, where each member is printed with its associated value.
Str code_debug_str(Code self)
{
	GEN_ASSERT(self != nullptr);
	StrBuilder  result_stack = strbuilder_make_reserve( _ctx->Allocator_Temp, kilobytes(1) );
	StrBuilder* result       = & result_stack;

	if ( self->Parent )
		strbuilder_append_fmt( result, "\n\tParent       : %S %S", code_type_str(self->Parent), self->Name.Len ? self->Name : txt("Null") );
	else
		strbuilder_append_fmt( result, "\n\tParent       : %S", txt("Null") );

	strbuilder_append_fmt( result, "\n\tName         : %S", self->Name.Len ? self->Name : txt("Null") );
	strbuilder_append_fmt( result, "\n\tType         : %S", code_type_str(self) );
	strbuilder_append_fmt( result, "\n\tModule Flags : %S", module_flag_to_str( self->ModuleFlags ) );

	switch ( self->Type )
	{
		case CT_Invalid:
		case CT_NewLine:
		case CT_Access_Private:
		case CT_Access_Protected:
		case CT_Access_Public:
			if ( self->Prev )
				strbuilder_append_fmt( result, "\n\tPrev: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				strbuilder_append_fmt( result, "\n\tNext: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
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
				strbuilder_append_fmt( result, "\n\tPrev: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				strbuilder_append_fmt( result, "\n\tNext: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			strbuilder_append_fmt( result, "\n\tContent: %S", self->Content );
		break;

		case CT_Class:
		case CT_Struct:
			if ( self->Prev )
				strbuilder_append_fmt( result, "\n\tPrev: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				strbuilder_append_fmt( result, "\n\tNext: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			strbuilder_append_fmt( result, "\n\tInlineCmt   : %S", self->InlineCmt  ? self->InlineCmt->Content                                  : txt("Null") );
			strbuilder_append_fmt( result, "\n\tAttributes  : %S", self->Attributes ? strbuilder_to_str( code_to_strbuilder(self->Attributes) ) : txt("Null") );
			strbuilder_append_fmt( result, "\n\tParentAccess: %S", self->ParentType ? access_spec_to_str( self->ParentAccess )                  : txt("No Parent") );
			strbuilder_append_fmt( result, "\n\tParentType  : %S", self->ParentType ? code_type_str(self->ParentType)                           : txt("Null") );
			strbuilder_append_fmt( result, "\n\tBody        : %S", self->Body       ? code_debug_str(self->Body)                                : txt("Null") );
		break;

		case CT_Class_Fwd:
		case CT_Struct_Fwd:
			if ( self->Prev )
				strbuilder_append_fmt( result, "\n\tPrev: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				strbuilder_append_fmt( result, "\n\tNext: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			strbuilder_append_fmt( result, "\n\tInlineCmt   : %S", self->InlineCmt  ? self->InlineCmt->Content                                  : txt("Null") );
			strbuilder_append_fmt( result, "\n\tAttributes  : %S", self->Attributes ? strbuilder_to_str( code_to_strbuilder(self->Attributes) ) : txt("Null") );
			strbuilder_append_fmt( result, "\n\tParentAccess: %S", self->ParentType ? access_spec_to_str( self->ParentAccess )                  : txt("No Parent") );
			strbuilder_append_fmt( result, "\n\tParentType  : %S", self->ParentType ? code_type_str(self->ParentType)                           : txt("Null") );
		break;

		case CT_Constructor:
			if ( self->Prev )
				strbuilder_append_fmt( result, "\n\tPrev: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				strbuilder_append_fmt( result, "\n\tNext: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			strbuilder_append_fmt( result, "\n\tInlineCmt      : %S", self->InlineCmt       ? self->InlineCmt->Content                                       : txt("Null") );
			strbuilder_append_fmt( result, "\n\tSpecs          : %S", self->Specs           ? strbuilder_to_str( code_to_strbuilder(self->Specs) )           : txt("Null") );
			strbuilder_append_fmt( result, "\n\tInitializerList: %S", self->InitializerList ? strbuilder_to_str( code_to_strbuilder(self->InitializerList) ) : txt("Null") );
			strbuilder_append_fmt( result, "\n\tParams         : %S", self->Params          ? strbuilder_to_str( code_to_strbuilder(self->Params) )          : txt("Null") );
			strbuilder_append_fmt( result, "\n\tBody           : %S", self->Body            ? code_debug_str(self->Body)                                     : txt("Null") );
		break;

		case CT_Constructor_Fwd:
			if ( self->Prev )
				strbuilder_append_fmt( result, "\n\tPrev: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				strbuilder_append_fmt( result, "\n\tNext: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			strbuilder_append_fmt( result, "\n\tInlineCmt      : %S", self->InlineCmt       ? self->InlineCmt->Content                                       : txt("Null") );
			strbuilder_append_fmt( result, "\n\tSpecs          : %S", self->Specs           ? strbuilder_to_str( code_to_strbuilder(self->Specs) )           : txt("Null") );
			strbuilder_append_fmt( result, "\n\tInitializerList: %S", self->InitializerList ? strbuilder_to_str( code_to_strbuilder(self->InitializerList) ) : txt("Null") );
			strbuilder_append_fmt( result, "\n\tParams         : %S", self->Params          ? strbuilder_to_str( code_to_strbuilder(self->Params) )          : txt("Null") );
		break;

		case CT_Destructor:
			if ( self->Prev )
				strbuilder_append_fmt( result, "\n\tPrev: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				strbuilder_append_fmt( result, "\n\tNext: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			strbuilder_append_fmt( result, "\n\tInlineCmt      : %S", self->InlineCmt       ? self->InlineCmt->Content                             : txt("Null") );
			strbuilder_append_fmt( result, "\n\tSpecs          : %S", self->Specs           ? strbuilder_to_str( code_to_strbuilder(self->Specs) ) : txt("Null") );
			strbuilder_append_fmt( result, "\n\tBody           : %S", self->Body            ? code_debug_str(self->Body)                           : txt("Null") );
		break;

		case CT_Destructor_Fwd:
		break;

		case CT_Enum:
		case CT_Enum_Class:
			if ( self->Prev )
				strbuilder_append_fmt( result, "\n\tPrev: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				strbuilder_append_fmt( result, "\n\tNext: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			strbuilder_append_fmt( result, "\n\tInlineCmt       : %S", self->InlineCmt      ? self->InlineCmt->Content                                     : txt("Null") );
			strbuilder_append_fmt( result, "\n\tAttributes      : %S", self->Attributes     ? strbuilder_to_str( code_to_strbuilder(self->Attributes) )    : txt("Null") );
			strbuilder_append_fmt( result, "\n\tUnderlying Type : %S", self->UnderlyingType ? strbuilder_to_str( code_to_strbuilder(self->UnderlyingType)) : txt("Null") );
			strbuilder_append_fmt( result, "\n\tBody            : %S", self->Body           ? code_debug_str(self->Body)                                   : txt("Null") );
		break;

		case CT_Enum_Fwd:
		case CT_Enum_Class_Fwd:
			if ( self->Prev )
				strbuilder_append_fmt( result, "\n\tPrev: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				strbuilder_append_fmt( result, "\n\tNext: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			strbuilder_append_fmt( result, "\n\tInlineCmt       : %S", self->InlineCmt      ? self->InlineCmt->Content                                     : txt("Null") );
			strbuilder_append_fmt( result, "\n\tAttributes      : %S", self->Attributes     ? strbuilder_to_str( code_to_strbuilder(self->Attributes) )    : txt("Null") );
			strbuilder_append_fmt( result, "\n\tUnderlying Type : %S", self->UnderlyingType ? strbuilder_to_str( code_to_strbuilder(self->UnderlyingType)) : txt("Null") );
		break;

		case CT_Extern_Linkage:
		case CT_Namespace:
			if ( self->Prev )
				strbuilder_append_fmt( result, "\n\tPrev: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				strbuilder_append_fmt( result, "\n\tNext: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			strbuilder_append_fmt( result, "\n\tBody: %S", self->Body ? code_debug_str(self->Body) : txt("Null") );
		break;

		case CT_Friend:
			if ( self->Prev )
				strbuilder_append_fmt( result, "\n\tPrev: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				strbuilder_append_fmt( result, "\n\tNext: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			strbuilder_append_fmt( result, "\n\tInlineCmt  : %S", self->InlineCmt   ? self->InlineCmt->Content                                  : txt("Null") );
			strbuilder_append_fmt( result, "\n\tDeclaration: %S", self->Declaration ? strbuilder_to_str( code_to_strbuilder(self->Declaration)) : txt("Null") );
		break;

		case CT_Function:
			if ( self->Prev )
				strbuilder_append_fmt( result, "\n\tPrev: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				strbuilder_append_fmt( result, "\n\tNext: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			strbuilder_append_fmt( result, "\n\tInlineCmt : %S", self->InlineCmt  ? self->InlineCmt->Content                                  : txt("Null") );
			strbuilder_append_fmt( result, "\n\tAttributes: %S", self->Attributes ? strbuilder_to_str( code_to_strbuilder(self->Attributes) ) : txt("Null") );
			strbuilder_append_fmt( result, "\n\tSpecs     : %S", self->Specs      ? strbuilder_to_str( code_to_strbuilder(self->Specs))       : txt("Null") );
			strbuilder_append_fmt( result, "\n\tReturnType: %S", self->ReturnType ? strbuilder_to_str( code_to_strbuilder(self->ReturnType))  : txt("Null") );
			strbuilder_append_fmt( result, "\n\tParams    : %S", self->Params     ? strbuilder_to_str( code_to_strbuilder(self->Params))      : txt("Null") );
			strbuilder_append_fmt( result, "\n\tBody      : %S", self->Body       ? code_debug_str(self->Body)                                : txt("Null") );
		break;

		case CT_Function_Fwd:
			if ( self->Prev )
				strbuilder_append_fmt( result, "\n\tPrev: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				strbuilder_append_fmt( result, "\n\tNext: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			strbuilder_append_fmt( result, "\n\tInlineCmt : %S", self->InlineCmt  ? self->InlineCmt->Content                                  : txt("Null") );
			strbuilder_append_fmt( result, "\n\tAttributes: %S", self->Attributes ? strbuilder_to_str( code_to_strbuilder(self->Attributes) ) : txt("Null") );
			strbuilder_append_fmt( result, "\n\tSpecs     : %S", self->Specs      ? strbuilder_to_str( code_to_strbuilder(self->Specs))       : txt("Null") );
			strbuilder_append_fmt( result, "\n\tReturnType: %S", self->ReturnType ? strbuilder_to_str( code_to_strbuilder(self->ReturnType))  : txt("Null") );
			strbuilder_append_fmt( result, "\n\tParams    : %S", self->Params     ? strbuilder_to_str( code_to_strbuilder(self->Params))      : txt("Null") );
		break;

		case CT_Module:
			if ( self->Prev )
				strbuilder_append_fmt( result, "\n\tPrev: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				strbuilder_append_fmt( result, "\n\tNext: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
		break;

		case CT_Operator:
		case CT_Operator_Member:
			if ( self->Prev )
				strbuilder_append_fmt( result, "\n\tPrev: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				strbuilder_append_fmt( result, "\n\tNext: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			strbuilder_append_fmt( result, "\n\tInlineCmt : %S", self->InlineCmt  ? self->InlineCmt->Content                                  : txt("Null") );
			strbuilder_append_fmt( result, "\n\tAttributes: %S", self->Attributes ? strbuilder_to_str( code_to_strbuilder(self->Attributes) ) : txt("Null") );
			strbuilder_append_fmt( result, "\n\tSpecs     : %S", self->Specs      ? strbuilder_to_str( code_to_strbuilder(self->Specs))       : txt("Null") );
			strbuilder_append_fmt( result, "\n\tReturnType: %S", self->ReturnType ? strbuilder_to_str( code_to_strbuilder(self->ReturnType))  : txt("Null") );
			strbuilder_append_fmt( result, "\n\tParams    : %S", self->Params     ? strbuilder_to_str( code_to_strbuilder(self->Params))      : txt("Null") );
			strbuilder_append_fmt( result, "\n\tBody      : %S", self->Body       ? code_debug_str(self->Body)                                : txt("Null") );
			strbuilder_append_fmt( result, "\n\tOp        : %S", operator_to_str( self->Op ) );
		break;

		case CT_Operator_Fwd:
		case CT_Operator_Member_Fwd:
			if ( self->Prev )
				strbuilder_append_fmt( result, "\n\tPrev: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				strbuilder_append_fmt( result, "\n\tNext: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			strbuilder_append_fmt( result, "\n\tInlineCmt : %S", self->InlineCmt  ? self->InlineCmt->Content                                  : txt("Null") );
			strbuilder_append_fmt( result, "\n\tAttributes: %S", self->Attributes ? strbuilder_to_str( code_to_strbuilder(self->Attributes) ) : txt("Null") );
			strbuilder_append_fmt( result, "\n\tSpecs     : %S", self->Specs      ? strbuilder_to_str( code_to_strbuilder(self->Specs) )      : txt("Null") );
			strbuilder_append_fmt( result, "\n\tReturnType: %S", self->ReturnType ? strbuilder_to_str( code_to_strbuilder(self->ReturnType) ) : txt("Null") );
			strbuilder_append_fmt( result, "\n\tParams    : %S", self->Params     ? strbuilder_to_str( code_to_strbuilder(self->Params) )     : txt("Null") );
			strbuilder_append_fmt( result, "\n\tOp        : %S", operator_to_str( self->Op ) );
		break;

		case CT_Operator_Cast:
			if ( self->Prev )
				strbuilder_append_fmt( result, "\n\tPrev: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				strbuilder_append_fmt( result, "\n\tNext: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			strbuilder_append_fmt( result, "\n\tInlineCmt : %S", self->InlineCmt  ? self->InlineCmt->Content                                : txt("Null") );
			strbuilder_append_fmt( result, "\n\tSpecs     : %S", self->Specs      ? strbuilder_to_str( code_to_strbuilder(self->Specs))     : txt("Null") );
			strbuilder_append_fmt( result, "\n\tValueType : %S", self->ValueType  ? strbuilder_to_str( code_to_strbuilder(self->ValueType)) : txt("Null") );
			strbuilder_append_fmt( result, "\n\tBody      : %S", self->Body       ? code_debug_str(self->Body)                              : txt("Null") );
		break;

		case CT_Operator_Cast_Fwd:
			if ( self->Prev )
				strbuilder_append_fmt( result, "\n\tPrev: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				strbuilder_append_fmt( result, "\n\tNext: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			strbuilder_append_fmt( result, "\n\tInlineCmt : %S", self->InlineCmt  ? self->InlineCmt->Content                                : txt("Null") );
			strbuilder_append_fmt( result, "\n\tSpecs     : %S", self->Specs      ? strbuilder_to_str( code_to_strbuilder(self->Specs))     : txt("Null") );
			strbuilder_append_fmt( result, "\n\tValueType : %S", self->ValueType  ? strbuilder_to_str( code_to_strbuilder(self->ValueType)) : txt("Null") );
		break;

		case CT_Parameters:
			strbuilder_append_fmt( result, "\n\tNumEntries: %d", self->NumEntries );
			strbuilder_append_fmt( result, "\n\tLast      : %S", self->Last->Name );
			strbuilder_append_fmt( result, "\n\tNext      : %S", self->Next->Name );
			strbuilder_append_fmt( result, "\n\tValueType : %S", self->ValueType ? strbuilder_to_str( code_to_strbuilder(self->ValueType)) : txt("Null") );
			strbuilder_append_fmt( result, "\n\tValue     : %S", self->Value     ? strbuilder_to_str( code_to_strbuilder(self->Value))     : txt("Null") );
		break;

		case CT_Specifiers:
		{
			strbuilder_append_fmt( result, "\n\tNumEntries: %d", self->NumEntries );
			strbuilder_append_str( result, txt("\n\tArrSpecs: ") );

			s32 idx  = 0;
			s32 left = self->NumEntries;
			while ( left-- )
			{
				Str spec = spec_to_str( self->ArrSpecs[idx] );
				strbuilder_append_fmt( result, "%.*s, ", spec.Len, spec.Ptr );
				idx++;
			}
			strbuilder_append_fmt( result, "\n\tNextSpecs: %S", self->NextSpecs ? code_debug_str(self->NextSpecs) : txt("Null") );
		}
		break;

		case CT_Template:
			if ( self->Prev )
				strbuilder_append_fmt( result, "\n\tPrev: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				strbuilder_append_fmt( result, "\n\tNext: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			strbuilder_append_fmt( result, "\n\tParams     : %S", self->Params      ? strbuilder_to_str( code_to_strbuilder(self->Params))      : txt("Null") );
			strbuilder_append_fmt( result, "\n\tDeclaration: %S", self->Declaration ? strbuilder_to_str( code_to_strbuilder(self->Declaration)) : txt("Null") );
		break;

		case CT_Typedef:
			if ( self->Prev )
				strbuilder_append_fmt( result, "\n\tPrev: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				strbuilder_append_fmt( result, "\n\tNext: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			strbuilder_append_fmt( result, "\n\tInlineCmt     : %S", self->InlineCmt      ? self->InlineCmt->Content                                     : txt("Null") );
			strbuilder_append_fmt( result, "\n\tUnderlyingType: %S", self->UnderlyingType ? strbuilder_to_str( code_to_strbuilder(self->UnderlyingType)) : txt("Null") );
		break;

		case CT_Typename:
			strbuilder_append_fmt( result, "\n\tAttributes     : %S", self->Attributes ? strbuilder_to_str( code_to_strbuilder(self->Attributes) ) : txt("Null") );
			strbuilder_append_fmt( result, "\n\tSpecs          : %S", self->Specs      ? strbuilder_to_str( code_to_strbuilder(self->Specs))       : txt("Null") );
			strbuilder_append_fmt( result, "\n\tReturnType     : %S", self->ReturnType ? strbuilder_to_str( code_to_strbuilder(self->ReturnType))  : txt("Null") );
			strbuilder_append_fmt( result, "\n\tParams         : %S", self->Params     ? strbuilder_to_str( code_to_strbuilder(self->Params))      : txt("Null") );
			strbuilder_append_fmt( result, "\n\tArrExpr        : %S", self->ArrExpr    ? strbuilder_to_str( code_to_strbuilder(self->ArrExpr))     : txt("Null") );
		break;

		case CT_Union:
			if ( self->Prev )
				strbuilder_append_fmt( result, "\n\tPrev: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				strbuilder_append_fmt( result, "\n\tNext: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			strbuilder_append_fmt( result, "\n\tAttributes: %S", self->Attributes ? strbuilder_to_str( code_to_strbuilder(self->Attributes) ) : txt("Null") );
			strbuilder_append_fmt( result, "\n\tBody      : %S", self->Body       ? code_debug_str(self->Body)                                : txt("Null") );
		break;

		case CT_Using:
			if ( self->Prev )
				strbuilder_append_fmt( result, "\n\tPrev: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				strbuilder_append_fmt( result, "\n\tNext: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			strbuilder_append_fmt( result, "\n\tInlineCmt     : %S", self->InlineCmt      ? self->InlineCmt->Content                                      : txt("Null") );
			strbuilder_append_fmt( result, "\n\tAttributes    : %S", self->Attributes     ? strbuilder_to_str( code_to_strbuilder(self->Attributes) )     : txt("Null") );
			strbuilder_append_fmt( result, "\n\tUnderlyingType: %S", self->UnderlyingType ? strbuilder_to_str( code_to_strbuilder(self->UnderlyingType))  : txt("Null") );
		break;

		case CT_Variable:

			if ( self->Parent && self->Parent->Type == CT_Variable )
			{
				// Its a NextVar
				strbuilder_append_fmt( result, "\n\tSpecs       : %S", self->Specs        ? strbuilder_to_str( code_to_strbuilder(self->Specs))        : txt("Null") );
				strbuilder_append_fmt( result, "\n\tValue       : %S", self->Value        ? strbuilder_to_str( code_to_strbuilder(self->Value))        : txt("Null") );
				strbuilder_append_fmt( result, "\n\tBitfieldSize: %S", self->BitfieldSize ? strbuilder_to_str( code_to_strbuilder(self->BitfieldSize)) : txt("Null") );
				strbuilder_append_fmt( result, "\n\tNextVar     : %S", self->NextVar      ? code_debug_str(self->NextVar)                              : txt("Null") );
				break;
			}

			if ( self->Prev )
				strbuilder_append_fmt( result, "\n\tPrev: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				strbuilder_append_fmt( result, "\n\tNext: %S %S", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			strbuilder_append_fmt( result, "\n\tInlineCmt   : %S", self->InlineCmt    ? self->InlineCmt->Content                                    : txt("Null") );
			strbuilder_append_fmt( result, "\n\tAttributes  : %S", self->Attributes   ? strbuilder_to_str( code_to_strbuilder(self->Attributes) )   : txt("Null") );
			strbuilder_append_fmt( result, "\n\tSpecs       : %S", self->Specs        ? strbuilder_to_str( code_to_strbuilder(self->Specs))         : txt("Null") );
			strbuilder_append_fmt( result, "\n\tValueType   : %S", self->ValueType    ? strbuilder_to_str( code_to_strbuilder(self->ValueType))     : txt("Null") );
			strbuilder_append_fmt( result, "\n\tBitfieldSize: %S", self->BitfieldSize ? strbuilder_to_str( code_to_strbuilder(self->BitfieldSize))  : txt("Null") );
			strbuilder_append_fmt( result, "\n\tValue       : %S", self->Value        ? strbuilder_to_str( code_to_strbuilder(self->Value))         : txt("Null") );
			strbuilder_append_fmt( result, "\n\tNextVar     : %S", self->NextVar      ? code_debug_str(self->NextVar)                               : txt("Null") );
		break;
	}

	return strbuilder_to_str( * result );
}

Code code_duplicate(Code self)
{
	Code result = make_code();

	void* mem_result = rcast(void*, cast(AST*, result));
	void* mem_self   = rcast(void*, cast(AST*, self));
	mem_copy( mem_result, mem_self, sizeof( AST ) );

	result->Parent = NullCode;
	return result;
}

StrBuilder code_to_strbuilder(Code self)
{
	StrBuilder result = strbuilder_make_str( _ctx->Allocator_Temp, txt("") );
	code_to_strbuilder_ptr( self, & result );
	return result;
}

void code_to_strbuilder_ptr( Code self, StrBuilder* result )
{
	GEN_ASSERT(self != nullptr);
	local_persist thread_local
	char SerializationLevel = 0;

	switch ( self->Type )
	{
		case CT_Invalid:
		#ifdef GEN_DONT_ALLOW_INVALID_CODE
			log_failure("Attempted to serialize invalid code! - %S", Parent ? Parent->code_debug_str() : Name );
		#else
			strbuilder_append_fmt( result, "Invalid Code!" );
		#endif
		break;

		case CT_NewLine:
			strbuilder_append_str( result, txt("\n"));
		break;

		case CT_Untyped:
		case CT_Execution:
		case CT_Comment:
		case CT_PlatformAttributes:
			strbuilder_append_str( result, self->Content );
		break;

		case CT_Access_Private:
		case CT_Access_Protected:
		case CT_Access_Public:
			strbuilder_append_str( result, self->Name );
		break;

		case CT_Class:
			class_to_strbuilder_def(cast(CodeClass, self), result );
		break;

		case CT_Class_Fwd:
			class_to_strbuilder_fwd(cast(CodeClass, self), result );
		break;

		case CT_Constructor:
			constructor_to_strbuilder_def(cast(CodeConstructor, self), result );
		break;

		case CT_Constructor_Fwd:
			constructor_to_strbuilder_fwd(cast(CodeConstructor, self), result );
		break;

		case CT_Destructor:
			destructor_to_strbuilder_def(cast(CodeDestructor, self), result );
		break;

		case CT_Destructor_Fwd:
			destructor_to_strbuilder_fwd(cast(CodeDestructor, self), result );
		break;

		case CT_Enum:
			enum_to_strbuilder_def(cast(CodeEnum, self), result );
		break;

		case CT_Enum_Fwd:
			enum_to_strbuilder_fwd(cast(CodeEnum, self), result );
		break;

		case CT_Enum_Class:
			enum_to_strbuilder_class_def(cast(CodeEnum, self), result );
		break;

		case CT_Enum_Class_Fwd:
			enum_to_strbuilder_class_fwd(cast(CodeEnum, self), result );
		break;

		case CT_Export_Body:
			body_to_strbuilder_export(cast(CodeBody, self), result );
		break;

		case CT_Extern_Linkage:
			extern_to_strbuilder(cast(CodeExtern, self), result );
		break;

		case CT_Friend:
			friend_to_strbuilder_ref(cast(CodeFriend, self), result );
		break;

		case CT_Function:
			fn_to_strbuilder_def(cast(CodeFn, self), result );
		break;

		case CT_Function_Fwd:
			fn_to_strbuilder_fwd(cast(CodeFn, self), result );
		break;

		case CT_Module:
			module_to_strbuilder_ref(cast(CodeModule, self), result );
		break;

		case CT_Namespace:
			namespace_to_strbuilder_ref(cast(CodeNS, self), result );
		break;

		case CT_Operator:
		case CT_Operator_Member:
			code_op_to_strbuilder_def(cast(CodeOperator, self), result );
		break;

		case CT_Operator_Fwd:
		case CT_Operator_Member_Fwd:
			code_op_to_strbuilder_fwd(cast(CodeOperator, self), result );
		break;

		case CT_Operator_Cast:
			opcast_to_strbuilder_def(cast(CodeOpCast, self), result );
		break;

		case CT_Operator_Cast_Fwd:
			opcast_to_strbuilder_fwd(cast(CodeOpCast, self), result );
		break;

		case CT_Parameters:
			params_to_strbuilder_ref(cast(CodeParams, self), result );
		break;

		case CT_Preprocess_Define:
			define_to_strbuilder_ref(cast(CodeDefine, self), result );
		break;

		case CT_Preprocess_If:
			preprocess_to_strbuilder_if(cast(CodePreprocessCond, self), result );
		break;

		case CT_Preprocess_IfDef:
			preprocess_to_strbuilder_ifdef(cast(CodePreprocessCond, self), result );
		break;

		case CT_Preprocess_IfNotDef:
			preprocess_to_strbuilder_ifndef(cast(CodePreprocessCond, self), result );
		break;

		case CT_Preprocess_Include:
			include_to_strbuilder_ref(cast(CodeInclude, self), result );
		break;

		case CT_Preprocess_ElIf:
			preprocess_to_strbuilder_elif(cast(CodePreprocessCond, self), result );
		break;

		case CT_Preprocess_Else:
			preprocess_to_strbuilder_else(cast(CodePreprocessCond, self), result );
		break;

		case CT_Preprocess_EndIf:
			preprocess_to_strbuilder_endif(cast(CodePreprocessCond, self), result );
		break;

		case CT_Preprocess_Pragma:
			pragma_to_strbuilder_ref(cast(CodePragma, self), result );
		break;

		case CT_Specifiers:
			specifiers_to_strbuilder_ref(cast(CodeSpecifiers, self), result );
		break;

		case CT_Struct:
			struct_to_strbuilder_def(cast(CodeStruct, self), result );
		break;

		case CT_Struct_Fwd:
			struct_to_strbuilder_fwd(cast(CodeStruct, self), result );
		break;

		case CT_Template:
			template_to_strbuilder_ref(cast(CodeTemplate, self), result );
		break;

		case CT_Typedef:
			typedef_to_strbuilder_ref(cast(CodeTypedef, self), result );
		break;

		case CT_Typename:
			typename_to_strbuilder_ref(cast(CodeTypename, self), result );
		break;

		case CT_Union:
			union_to_strbuilder_def( cast(CodeUnion, self), result );
		break;

		case CT_Union_Fwd:
			union_to_strbuilder_fwd( cast(CodeUnion, self), result );
		break;

		case CT_Using:
			using_to_strbuilder_ref(cast(CodeUsing, self), result );
		break;

		case CT_Using_Namespace:
			using_to_strbuilder_ns(cast(CodeUsing, self), result );
		break;

		case CT_Variable:
			var_to_strbuilder_ref(cast(CodeVar, self), result );
		break;

		case CT_Enum_Body:
		case CT_Class_Body:
		case CT_Extern_Linkage_Body:
		case CT_Function_Body:
		case CT_Global_Body:
		case CT_Namespace_Body:
		case CT_Struct_Body:
		case CT_Union_Body:
			body_to_strbuilder_ref( cast(CodeBody, self), result );
		break;
	}
}

bool code_is_equal( Code self, Code other )
{
/*
	AST values are either some u32 value, a cached string, or a pointer to another AST.

	u32 values are compared by value.
	Cached strings are compared by pointer.
	AST nodes are compared with AST::is_equal.
*/
	if ( other == nullptr )
	{
		log_fmt( "AST::is_equal: other is null\nAST: %S", code_debug_str(self) );
		return false;
	}

	if ( self->Type != other->Type )
	{
		log_fmt("AST::is_equal: Type check failure with other\nAST: %S\nOther: %S"
			, code_debug_str(self)
			, code_debug_str(other)
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
		    , code_debug_str(self)                            \
		    ,code_debug_str(other)                            \
		);                                                    \
                                                              \
		return false;                                         \
	}

	#define check_member_str( str )                                 \
	if ( ! str_are_equal( self->str, other->str ) )                 \
	{                                                               \
		log_fmt("\nAST::is_equal: Member string - "#str " failed\n" \
				"AST  : %S\n"                                       \
				"Other: %S\n"                                       \
			, code_debug_str(self)                                  \
			,code_debug_str(other)                                  \
		);                                                          \
	                                                                \
		return false;                                               \
	}

	#define check_member_content( content )                                  \
	if ( ! str_are_equal( self->content, other->content ))                   \
	{                                                                        \
		log_fmt("\nAST::is_equal: Member content - "#content " failed\n"     \
				"AST  : %S\n"                                                \
				"Other: %S\n"                                                \
			, code_debug_str(self)                                           \
			, code_debug_str(other)                                          \
		);                                                                   \
                                                                             \
		log_fmt("Content cannot be trusted to be unique with this check "    \
			"so it must be verified by eye for now\n"                        \
			"AST   Content:\n%S\n"                                           \
			"Other Content:\n%S\n"                                           \
			, str_visualize_whitespace(self->content, _ctx->Allocator_Temp)  \
			, str_visualize_whitespace(other->content, _ctx->Allocator_Temp) \
		);                                                                   \
	}

	#define check_member_ast( ast )                                                                \
	if ( self->ast )                                                                               \
	{                                                                                              \
		if ( other->ast == nullptr )                                                               \
		{                                                                                          \
			log_fmt("\nAST::is_equal: Failed for member " #ast " other equivalent param is null\n" \
					"AST  : %S\n"                                                                  \
					"Other: %S\n"                                                                  \
					"For ast member: %S\n"                                                         \
				, code_debug_str(self)                                                             \
				, code_debug_str(other)                                                            \
				, code_debug_str(self->ast)                                                        \
			);                                                                                     \
                                                                                                   \
			return false;                                                                          \
		}                                                                                          \
                                                                                                   \
		if ( ! code_is_equal(self->ast, other->ast ) )                                             \
		{                                                                                          \
			log_fmt( "\nAST::is_equal: Failed for " #ast"\n"                                       \
					"AST  : %S\n"                                                                  \
					"Other: %S\n"                                                                  \
					"For     ast member: %S\n"                                                     \
					"other's ast member: %S\n"                                                     \
				, code_debug_str(self)                                                             \
				, code_debug_str(other)                                                            \
				, code_debug_str(self->ast)                                                        \
				, code_debug_str(other->ast)                                                       \
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
			check_member_ast( UnderlyingTypeMacro );

			return true;
		}

		case CT_Enum_Fwd:
		case CT_Enum_Class_Fwd:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( Attributes );
			check_member_ast( UnderlyingType );
			check_member_ast( UnderlyingTypeMacro );

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
								, code_debug_str(curr)
							);

							return false;
						}

						if ( str_are_equal(curr->Name, curr_other->Name) )
						{
							log_fmt( "\nAST::is_equal: Failed for parameter name check\n"
									"AST  : %S\n"
									"Other: %S\n"
									"For     ast member: %S\n"
									"other's ast member: %S\n"
								, code_debug_str(self)
								, code_debug_str(other)
								, code_debug_str(curr)
								, code_debug_str(curr_other)
							);
							return false;
						}

						if ( curr->ValueType && ! code_is_equal(curr->ValueType, curr_other->ValueType) )
						{
							log_fmt( "\nAST::is_equal: Failed for parameter value type check\n"
									"AST  : %S\n"
									"Other: %S\n"
									"For     ast member: %S\n"
									"other's ast member: %S\n"
								, code_debug_str(self)
								, code_debug_str(other)
								, code_debug_str(curr)
								, code_debug_str(curr_other)
							);
							return false;
						}

						if ( curr->Value && ! code_is_equal(curr->Value, curr_other->Value) )
						{
							log_fmt( "\nAST::is_equal: Failed for parameter value check\n"
									"AST  : %S\n"
									"Other: %S\n"
									"For     ast member: %S\n"
									"other's ast member: %S\n"
								, code_debug_str(self)
								, code_debug_str(other)
								, code_debug_str(curr)
								, code_debug_str(curr_other)
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

		case CT_Union_Fwd:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( Attributes );
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
						, code_debug_str(curr)
						, code_debug_str(other)
					);

					return false;
				}

				if ( ! code_is_equal( curr, curr_other ) )
				{
					log_fmt( "\nAST::is_equal: Failed for body\n"
							"AST  : %S\n"
							"Other: %S\n"
							"For     ast member: %S\n"
							"other's ast member: %S\n"
						, code_debug_str(self)
						, code_debug_str(other)
						, code_debug_str(curr)
						, code_debug_str(curr_other)
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

bool code_validate_body(Code self)
{
	switch ( self->Type )
	{
		case CT_Class_Body:
		{
			CodeBody body = cast(CodeBody, self);
			for (Code code_entry = begin_CodeBody(body); code_entry != end_CodeBody(body); next_CodeBody(body, code_entry)) switch (code_entry->Type)
			{
				GEN_AST_BODY_CLASS_UNALLOWED_TYPES:
					log_failure("AST::validate_body: Invalid entry in body %S", code_debug_str(code_entry));
				return false;

				default:
				continue;
			}
		}
		break;
		case CT_Enum_Body:
		{
			CodeBody body = cast(CodeBody, self);
			for ( Code entry = begin_CodeBody(body); entry != end_CodeBody(body); next_CodeBody(body, entry) )
			{
				if ( entry->Type != CT_Untyped )
				{
					log_failure( "AST::validate_body: Invalid entry in enum body (needs to be untyped or comment) %S", code_debug_str(entry) );
					return false;
				}
			}
		}
		break;
		case CT_Export_Body:
		{
			CodeBody body = cast(CodeBody, self);
			for (Code code_entry = begin_CodeBody(body); code_entry != end_CodeBody(body); next_CodeBody(body, code_entry)) switch (code_entry->Type)
			{
				GEN_AST_BODY_EXPORT_UNALLOWED_TYPES:
					log_failure("AST::validate_body: Invalid entry in body %S", code_debug_str(code_entry));
				return false;

				default:
				continue;
			}
		}
		break;
		case CT_Extern_Linkage:
		{
			CodeBody body = cast(CodeBody, self);
			for (Code code_entry = begin_CodeBody(body); code_entry != end_CodeBody(body); next_CodeBody(body, code_entry)) switch (code_entry->Type)
			{
				GEN_AST_BODY_EXTERN_LINKAGE_UNALLOWED_TYPES:
					log_failure("AST::validate_body: Invalid entry in body %S", code_debug_str(code_entry));
				return false;

				default:
				continue;
			}
		}
		break;
		case CT_Function_Body:
		{
			CodeBody body = cast(CodeBody, self);
			for (Code code_entry = begin_CodeBody(body); code_entry != end_CodeBody(body); next_CodeBody(body, code_entry)) switch (code_entry->Type)
			{
				GEN_AST_BODY_FUNCTION_UNALLOWED_TYPES:
					log_failure("AST::validate_body: Invalid entry in body %S", code_debug_str(code_entry));
				return false;

				default:
				continue;
			}
		}
		break;
		case CT_Global_Body:
		{
			CodeBody body = cast(CodeBody, self);
			for ( Code entry = begin_CodeBody(body); entry != end_CodeBody(body); next_CodeBody(body, entry) )switch (entry->Type)
			{
				GEN_AST_BODY_GLOBAL_UNALLOWED_TYPES:
					log_failure("AST::validate_body: Invalid entry in body %S", code_debug_str(entry));
				return false;
			}
		}
		break;
		case CT_Namespace_Body:
		{
			CodeBody body = cast(CodeBody, self);
			for ( Code entry = begin_CodeBody(body); entry != end_CodeBody(body); next_CodeBody(body, entry) ) switch (entry->Type)
			{
				GEN_AST_BODY_NAMESPACE_UNALLOWED_TYPES:
					log_failure("AST::validate_body: Invalid entry in body %S", code_debug_str(entry));
				return false;
			}
		}
		break;
		case CT_Struct_Body:
		{
			CodeBody body = cast(CodeBody, self);
			for ( Code entry = begin_CodeBody(body); entry != end_CodeBody(body); next_CodeBody(body, entry) ) switch (entry->Type)
			{
				GEN_AST_BODY_STRUCT_UNALLOWED_TYPES:
					log_failure("AST::validate_body: Invalid entry in body %S", code_debug_str(entry));
				return false;
			}
		}
		break;
		case CT_Union_Body:
		{
			CodeBody body = cast(CodeBody, self);
			for ( Code entry = begin_CodeBody(body); entry != end_CodeBody(body); next_CodeBody(body, entry) )
			{
				if ( entry->Type != CT_Untyped )
				{
					log_failure( "AST::validate_body: Invalid entry in union body (needs to be untyped or comment) %S", code_debug_str(entry) );
					return false;
				}
			}
		}
		break;

		default:
			log_failure( "AST::validate_body: Invalid this AST does not have a body %S", code_debug_str(self) );
			return false;
	}
	return false;
}
