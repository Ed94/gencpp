#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "static_data.cpp"
#endif

global Code Code_Global;
global Code Code_Invalid;

// This serializes all the data-members in a "debug" format, where each member is printed with its associated value.
StrC code_debug_str(Code self)
{
	GEN_ASSERT(self != nullptr);
	String  result_stack = string_make_reserve( GlobalAllocator, kilobytes(1) );
	String* result       = & result_stack;

	if ( self->Parent )
		string_append_fmt( result, "\n\tParent       : %SC %SC", code_type_str(self->Parent), self->Name.Len ? self->Name : txt("Null") );
	else
		string_append_fmt( result, "\n\tParent       : %SC", txt("Null") );

	string_append_fmt( result, "\n\tName         : %SC", self->Name.Len ? self->Name : txt("Null") );
	string_append_fmt( result, "\n\tType         : %SC", code_type_str(self) );
	string_append_fmt( result, "\n\tModule Flags : %SC", module_flag_to_str( self->ModuleFlags ) );

	switch ( self->Type )
	{
		case CT_Invalid:
		case CT_NewLine:
		case CT_Access_Private:
		case CT_Access_Protected:
		case CT_Access_Public:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
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
				string_append_fmt( result, "\n\tPrev: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			string_append_fmt( result, "\n\tContent: %SC", self->Content );
		break;

		case CT_Class:
		case CT_Struct:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			string_append_fmt( result, "\n\tInlineCmt   : %SC", self->InlineCmt  ? self->InlineCmt->Content                           : txt("Null") );
			string_append_fmt( result, "\n\tAttributes  : %SC", self->Attributes ? string_to_strc( code_to_string(self->Attributes) ) : txt("Null") );
			string_append_fmt( result, "\n\tParentAccess: %SC", self->ParentType ? access_spec_to_str( self->ParentAccess )           : txt("No Parent") );
			string_append_fmt( result, "\n\tParentType  : %SC", self->ParentType ? code_type_str(self->ParentType)                    : txt("Null") );
			string_append_fmt( result, "\n\tBody        : %SC", self->Body       ? code_debug_str(self->Body)                         : txt("Null") );
		break;

		case CT_Class_Fwd:
		case CT_Struct_Fwd:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			string_append_fmt( result, "\n\tInlineCmt   : %SC", self->InlineCmt  ? self->InlineCmt->Content                           : txt("Null") );
			string_append_fmt( result, "\n\tAttributes  : %SC", self->Attributes ? string_to_strc( code_to_string(self->Attributes) ) : txt("Null") );
			string_append_fmt( result, "\n\tParentAccess: %SC", self->ParentType ? access_spec_to_str( self->ParentAccess )           : txt("No Parent") );
			string_append_fmt( result, "\n\tParentType  : %SC", self->ParentType ? code_type_str(self->ParentType)                    : txt("Null") );
		break;

		case CT_Constructor:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			string_append_fmt( result, "\n\tInlineCmt      : %SC", self->InlineCmt       ? self->InlineCmt->Content                                : txt("Null") );
			string_append_fmt( result, "\n\tSpecs          : %SC", self->Specs           ? string_to_strc( code_to_string(self->Specs) )           : txt("Null") );
			string_append_fmt( result, "\n\tInitializerList: %SC", self->InitializerList ? string_to_strc( code_to_string(self->InitializerList) ) : txt("Null") );
			string_append_fmt( result, "\n\tParams         : %SC", self->Params          ? string_to_strc( code_to_string(self->Params) )          : txt("Null") );
			string_append_fmt( result, "\n\tBody           : %SC", self->Body            ? code_debug_str(self->Body)                              : txt("Null") );
		break;

		case CT_Constructor_Fwd:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			string_append_fmt( result, "\n\tInlineCmt      : %SC", self->InlineCmt       ? self->InlineCmt->Content                                : txt("Null") );
			string_append_fmt( result, "\n\tSpecs          : %SC", self->Specs           ? string_to_strc( code_to_string(self->Specs) )           : txt("Null") );
			string_append_fmt( result, "\n\tInitializerList: %SC", self->InitializerList ? string_to_strc( code_to_string(self->InitializerList) ) : txt("Null") );
			string_append_fmt( result, "\n\tParams         : %SC", self->Params          ? string_to_strc( code_to_string(self->Params) )          : txt("Null") );
		break;

		case CT_Destructor:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			string_append_fmt( result, "\n\tInlineCmt      : %SC", self->InlineCmt       ? self->InlineCmt->Content                      : txt("Null") );
			string_append_fmt( result, "\n\tSpecs          : %SC", self->Specs           ? string_to_strc( code_to_string(self->Specs) ) : txt("Null") );
			string_append_fmt( result, "\n\tBody           : %SC", self->Body            ? code_debug_str(self->Body)                    : txt("Null") );
		break;

		case CT_Destructor_Fwd:
		break;

		case CT_Enum:
		case CT_Enum_Class:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			string_append_fmt( result, "\n\tInlineCmt       : %SC", self->InlineCmt      ? self->InlineCmt->Content                              : txt("Null") );
			string_append_fmt( result, "\n\tAttributes      : %SC", self->Attributes     ? string_to_strc( code_to_string(self->Attributes) )    : txt("Null") );
			string_append_fmt( result, "\n\tUnderlying Type : %SC", self->UnderlyingType ? string_to_strc( code_to_string(self->UnderlyingType)) : txt("Null") );
			string_append_fmt( result, "\n\tBody            : %SC", self->Body           ? code_debug_str(self->Body)                            : txt("Null") );
		break;

		case CT_Enum_Fwd:
		case CT_Enum_Class_Fwd:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			string_append_fmt( result, "\n\tInlineCmt       : %SC", self->InlineCmt      ? self->InlineCmt->Content                              : txt("Null") );
			string_append_fmt( result, "\n\tAttributes      : %SC", self->Attributes     ? string_to_strc( code_to_string(self->Attributes) )    : txt("Null") );
			string_append_fmt( result, "\n\tUnderlying Type : %SC", self->UnderlyingType ? string_to_strc( code_to_string(self->UnderlyingType)) : txt("Null") );
		break;

		case CT_Extern_Linkage:
		case CT_Namespace:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			string_append_fmt( result, "\n\tBody: %SC", self->Body ? code_debug_str(self->Body) : txt("Null") );
		break;

		case CT_Friend:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			string_append_fmt( result, "\n\tInlineCmt  : %SC", self->InlineCmt   ? self->InlineCmt->Content                           : txt("Null") );
			string_append_fmt( result, "\n\tDeclaration: %SC", self->Declaration ? string_to_strc( code_to_string(self->Declaration)) : txt("Null") );
		break;

		case CT_Function:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			string_append_fmt( result, "\n\tInlineCmt : %SC", self->InlineCmt  ? self->InlineCmt->Content                           : txt("Null") );
			string_append_fmt( result, "\n\tAttributes: %SC", self->Attributes ? string_to_strc( code_to_string(self->Attributes) ) : txt("Null") );
			string_append_fmt( result, "\n\tSpecs     : %SC", self->Specs      ? string_to_strc( code_to_string(self->Specs))       : txt("Null") );
			string_append_fmt( result, "\n\tReturnType: %SC", self->ReturnType ? string_to_strc( code_to_string(self->ReturnType))  : txt("Null") );
			string_append_fmt( result, "\n\tParams    : %SC", self->Params     ? string_to_strc( code_to_string(self->Params))      : txt("Null") );
			string_append_fmt( result, "\n\tBody      : %SC", self->Body       ? code_debug_str(self->Body)                         : txt("Null") );
		break;

		case CT_Function_Fwd:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			string_append_fmt( result, "\n\tInlineCmt : %SC", self->InlineCmt  ? self->InlineCmt->Content                           : txt("Null") );
			string_append_fmt( result, "\n\tAttributes: %SC", self->Attributes ? string_to_strc( code_to_string(self->Attributes) ) : txt("Null") );
			string_append_fmt( result, "\n\tSpecs     : %SC", self->Specs      ? string_to_strc( code_to_string(self->Specs))       : txt("Null") );
			string_append_fmt( result, "\n\tReturnType: %SC", self->ReturnType ? string_to_strc( code_to_string(self->ReturnType))  : txt("Null") );
			string_append_fmt( result, "\n\tParams    : %SC", self->Params     ? string_to_strc( code_to_string(self->Params))      : txt("Null") );
		break;

		case CT_Module:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
		break;

		case CT_Operator:
		case CT_Operator_Member:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			string_append_fmt( result, "\n\tInlineCmt : %SC", self->InlineCmt  ? self->InlineCmt->Content                           : txt("Null") );
			string_append_fmt( result, "\n\tAttributes: %SC", self->Attributes ? string_to_strc( code_to_string(self->Attributes) ) : txt("Null") );
			string_append_fmt( result, "\n\tSpecs     : %SC", self->Specs      ? string_to_strc( code_to_string(self->Specs))       : txt("Null") );
			string_append_fmt( result, "\n\tReturnType: %SC", self->ReturnType ? string_to_strc( code_to_string(self->ReturnType))  : txt("Null") );
			string_append_fmt( result, "\n\tParams    : %SC", self->Params     ? string_to_strc( code_to_string(self->Params))      : txt("Null") );
			string_append_fmt( result, "\n\tBody      : %SC", self->Body       ? code_debug_str(self->Body)                         : txt("Null") );
			string_append_fmt( result, "\n\tOp        : %SC", operator_to_str( self->Op ) );
		break;

		case CT_Operator_Fwd:
		case CT_Operator_Member_Fwd:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			string_append_fmt( result, "\n\tInlineCmt : %SC", self->InlineCmt  ? self->InlineCmt->Content                           : txt("Null") );
			string_append_fmt( result, "\n\tAttributes: %SC", self->Attributes ? string_to_strc( code_to_string(self->Attributes) ) : txt("Null") );
			string_append_fmt( result, "\n\tSpecs     : %SC", self->Specs      ? string_to_strc( code_to_string(self->Specs) )      : txt("Null") );
			string_append_fmt( result, "\n\tReturnType: %SC", self->ReturnType ? string_to_strc( code_to_string(self->ReturnType) ) : txt("Null") );
			string_append_fmt( result, "\n\tParams    : %SC", self->Params     ? string_to_strc( code_to_string(self->Params) )     : txt("Null") );
			string_append_fmt( result, "\n\tOp        : %SC", operator_to_str( self->Op ) );
		break;

		case CT_Operator_Cast:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			string_append_fmt( result, "\n\tInlineCmt : %SC", self->InlineCmt  ? self->InlineCmt->Content                         : txt("Null") );
			string_append_fmt( result, "\n\tSpecs     : %SC", self->Specs      ? string_to_strc( code_to_string(self->Specs))     : txt("Null") );
			string_append_fmt( result, "\n\tValueType : %SC", self->ValueType  ? string_to_strc( code_to_string(self->ValueType)) : txt("Null") );
			string_append_fmt( result, "\n\tBody      : %SC", self->Body       ? code_debug_str(self->Body)                       : txt("Null") );
		break;

		case CT_Operator_Cast_Fwd:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			string_append_fmt( result, "\n\tInlineCmt : %SC", self->InlineCmt  ? self->InlineCmt->Content                         : txt("Null") );
			string_append_fmt( result, "\n\tSpecs     : %SC", self->Specs      ? string_to_strc( code_to_string(self->Specs))     : txt("Null") );
			string_append_fmt( result, "\n\tValueType : %SC", self->ValueType  ? string_to_strc( code_to_string(self->ValueType)) : txt("Null") );
		break;

		case CT_Parameters:
			string_append_fmt( result, "\n\tNumEntries: %d", self->NumEntries );
			string_append_fmt( result, "\n\tLast      : %SC", self->Last->Name );
			string_append_fmt( result, "\n\tNext      : %SC", self->Next->Name );
			string_append_fmt( result, "\n\tValueType : %SC", self->ValueType ? string_to_strc( code_to_string(self->ValueType)) : txt("Null") );
			string_append_fmt( result, "\n\tValue     : %SC", self->Value     ? string_to_strc( code_to_string(self->Value))     : txt("Null") );
		break;

		case CT_Specifiers:
		{
			string_append_fmt( result, "\n\tNumEntries: %d", self->NumEntries );
			string_append_strc( result, txt("\n\tArrSpecs: ") );

			s32 idx  = 0;
			s32 left = self->NumEntries;
			while ( left-- )
			{
				StrC spec = spec_to_str( self->ArrSpecs[idx] );
				string_append_fmt( result, "%.*s, ", spec.Len, spec.Ptr );
				idx++;
			}
			string_append_fmt( result, "\n\tNextSpecs: %SC", self->NextSpecs ? code_debug_str(self->NextSpecs) : txt("Null") );
		}
		break;

		case CT_Template:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			string_append_fmt( result, "\n\tParams     : %SC", self->Params      ? string_to_strc( code_to_string(self->Params))      : txt("Null") );
			string_append_fmt( result, "\n\tDeclaration: %SC", self->Declaration ? string_to_strc( code_to_string(self->Declaration)) : txt("Null") );
		break;

		case CT_Typedef:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			string_append_fmt( result, "\n\tInlineCmt     : %SC", self->InlineCmt      ? self->InlineCmt->Content                              : txt("Null") );
			string_append_fmt( result, "\n\tUnderlyingType: %SC", self->UnderlyingType ? string_to_strc( code_to_string(self->UnderlyingType)) : txt("Null") );
		break;

		case CT_Typename:
			string_append_fmt( result, "\n\tAttributes     : %SC", self->Attributes ? string_to_strc( code_to_string(self->Attributes) ) : txt("Null") );
			string_append_fmt( result, "\n\tSpecs          : %SC", self->Specs      ? string_to_strc( code_to_string(self->Specs))       : txt("Null") );
			string_append_fmt( result, "\n\tReturnType     : %SC", self->ReturnType ? string_to_strc( code_to_string(self->ReturnType))  : txt("Null") );
			string_append_fmt( result, "\n\tParams         : %SC", self->Params     ? string_to_strc( code_to_string(self->Params))      : txt("Null") );
			string_append_fmt( result, "\n\tArrExpr        : %SC", self->ArrExpr    ? string_to_strc( code_to_string(self->ArrExpr))     : txt("Null") );
		break;

		case CT_Union:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			string_append_fmt( result, "\n\tAttributes: %SC", self->Attributes ? string_to_strc( code_to_string(self->Attributes) ) : txt("Null") );
			string_append_fmt( result, "\n\tBody      : %SC", self->Body       ? code_debug_str(self->Body)       : txt("Null") );
		break;

		case CT_Using:
			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			string_append_fmt( result, "\n\tInlineCmt     : %SC", self->InlineCmt      ? self->InlineCmt->Content                               : txt("Null") );
			string_append_fmt( result, "\n\tAttributes    : %SC", self->Attributes     ? string_to_strc( code_to_string(self->Attributes) )     : txt("Null") );
			string_append_fmt( result, "\n\tUnderlyingType: %SC", self->UnderlyingType ? string_to_strc( code_to_string(self->UnderlyingType))  : txt("Null") );
		break;

		case CT_Variable:

			if ( self->Parent && self->Parent->Type == CT_Variable )
			{
				// Its a NextVar
				string_append_fmt( result, "\n\tSpecs       : %SC", self->Specs        ? string_to_strc( code_to_string(self->Specs))        : txt("Null") );
				string_append_fmt( result, "\n\tValue       : %SC", self->Value        ? string_to_strc( code_to_string(self->Value))        : txt("Null") );
				string_append_fmt( result, "\n\tBitfieldSize: %SC", self->BitfieldSize ? string_to_strc( code_to_string(self->BitfieldSize)) : txt("Null") );
				string_append_fmt( result, "\n\tNextVar     : %SC", self->NextVar      ? code_debug_str(self->NextVar)                       : txt("Null") );
				break;
			}

			if ( self->Prev )
				string_append_fmt( result, "\n\tPrev: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );
			if ( self->Next )
				string_append_fmt( result, "\n\tNext: %SC %SC", code_type_str(self->Prev), self->Prev->Name.Len ? self->Prev->Name : txt("Null") );

			string_append_fmt( result, "\n\tInlineCmt   : %SC", self->InlineCmt    ? self->InlineCmt->Content                             : txt("Null") );
			string_append_fmt( result, "\n\tAttributes  : %SC", self->Attributes   ? string_to_strc( code_to_string(self->Attributes) )   : txt("Null") );
			string_append_fmt( result, "\n\tSpecs       : %SC", self->Specs        ? string_to_strc( code_to_string(self->Specs))         : txt("Null") );
			string_append_fmt( result, "\n\tValueType   : %SC", self->ValueType    ? string_to_strc( code_to_string(self->ValueType))     : txt("Null") );
			string_append_fmt( result, "\n\tBitfieldSize: %SC", self->BitfieldSize ? string_to_strc( code_to_string(self->BitfieldSize))  : txt("Null") );
			string_append_fmt( result, "\n\tValue       : %SC", self->Value        ? string_to_strc( code_to_string(self->Value))         : txt("Null") );
			string_append_fmt( result, "\n\tNextVar     : %SC", self->NextVar      ? code_debug_str(self->NextVar)                        : txt("Null") );
		break;
	}

	return string_to_strc( * result );
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

String code_to_string(Code self)
{
	String result = string_make_strc( GlobalAllocator, txt("") );
	code_to_string_ptr( self, & result );
	return result;
}

void code_to_string_ptr( Code self, String* result )
{
	GEN_ASSERT(self != nullptr);
	local_persist thread_local
	char SerializationLevel = 0;

	switch ( self->Type )
	{
		case CT_Invalid:
		#ifdef GEN_DONT_ALLOW_INVALID_CODE
			log_failure("Attempted to serialize invalid code! - %SC", Parent ? Parent->code_debug_str() : Name );
		#else
			string_append_fmt( result, "Invalid Code!" );
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
			class_to_string_def(cast(CodeClass, self), result );
		break;

		case CT_Class_Fwd:
			class_to_string_fwd(cast(CodeClass, self), result );
		break;

		case CT_Constructor:
			constructor_to_string_def(cast(CodeConstructor, self), result );
		break;

		case CT_Constructor_Fwd:
			constructor_to_string_fwd(cast(CodeConstructor, self), result );
		break;

		case CT_Destructor:
			destructor_to_string_def(cast(CodeDestructor, self), result );
		break;

		case CT_Destructor_Fwd:
			destructor_to_string_fwd(cast(CodeDestructor, self), result );
		break;

		case CT_Enum:
			enum_to_string_def(cast(CodeEnum, self), result );
		break;

		case CT_Enum_Fwd:
			enum_to_string_fwd(cast(CodeEnum, self), result );
		break;

		case CT_Enum_Class:
			enum_to_string_class_def(cast(CodeEnum, self), result );
		break;

		case CT_Enum_Class_Fwd:
			enum_to_string_class_fwd(cast(CodeEnum, self), result );
		break;

		case CT_Export_Body:
			body_to_string_export(cast(CodeBody, self), result );
		break;

		case CT_Extern_Linkage:
			extern_to_string(cast(CodeExtern, self), result );
		break;

		case CT_Friend:
			friend_to_string_ref(cast(CodeFriend, self), result );
		break;

		case CT_Function:
			fn_to_string_def(cast(CodeFn, self), result );
		break;

		case CT_Function_Fwd:
			fn_to_string_fwd(cast(CodeFn, self), result );
		break;

		case CT_Module:
			module_to_string_ref(cast(CodeModule, self), result );
		break;

		case CT_Namespace:
			namespace_to_string_ref(cast(CodeNS, self), result );
		break;

		case CT_Operator:
		case CT_Operator_Member:
			code_op_to_string_def(cast(CodeOperator, self), result );
		break;

		case CT_Operator_Fwd:
		case CT_Operator_Member_Fwd:
			code_op_to_string_fwd(cast(CodeOperator, self), result );
		break;

		case CT_Operator_Cast:
			opcast_to_string_def(cast(CodeOpCast, self), result );
		break;

		case CT_Operator_Cast_Fwd:
			opcast_to_string_fwd(cast(CodeOpCast, self), result );
		break;

		case CT_Parameters:
			params_to_string_ref(cast(CodeParam, self), result );
		break;

		case CT_Preprocess_Define:
			define_to_string_ref(cast(CodeDefine, self), result );
		break;

		case CT_Preprocess_If:
			preprocess_to_string_if(cast(CodePreprocessCond, self), result );
		break;

		case CT_Preprocess_IfDef:
			preprocess_to_string_ifdef(cast(CodePreprocessCond, self), result );
		break;

		case CT_Preprocess_IfNotDef:
			preprocess_to_string_ifndef(cast(CodePreprocessCond, self), result );
		break;

		case CT_Preprocess_Include:
			include_to_string_ref(cast(CodeInclude, self), result );
		break;

		case CT_Preprocess_ElIf:
			preprocess_to_string_elif(cast(CodePreprocessCond, self), result );
		break;

		case CT_Preprocess_Else:
			preprocess_to_string_else(cast(CodePreprocessCond, self), result );
		break;

		case CT_Preprocess_EndIf:
			preprocess_to_string_endif(cast(CodePreprocessCond, self), result );
		break;

		case CT_Preprocess_Pragma:
			pragma_to_string_ref(cast(CodePragma, self), result );
		break;

		case CT_Specifiers:
			specifiers_to_string_ref(cast(CodeSpecifiers, self), result );
		break;

		case CT_Struct:
			struct_to_string_def(cast(CodeStruct, self), result );
		break;

		case CT_Struct_Fwd:
			struct_to_string_fwd(cast(CodeStruct, self), result );
		break;

		case CT_Template:
			template_to_string_ref(cast(CodeTemplate, self), result );
		break;

		case CT_Typedef:
			typedef_to_string_ref(cast(CodeTypedef, self), result );
		break;

		case CT_Typename:
			typename_to_string_ref(cast(CodeTypename, self), result );
		break;

		case CT_Union:
			union_to_string_def( cast(CodeUnion, self), result );
		break;

		case CT_Union_Fwd:
			union_to_string_fwd( cast(CodeUnion, self), result );
		break;

		case CT_Using:
			using_to_string_ref(cast(CodeUsing, self), result );
		break;

		case CT_Using_Namespace:
			using_to_string_ns(cast(CodeUsing, self), result );
		break;

		case CT_Variable:
			var_to_string_ref(cast(CodeVar, self), result );
		break;

		case CT_Enum_Body:
		case CT_Class_Body:
		case CT_Extern_Linkage_Body:
		case CT_Function_Body:
		case CT_Global_Body:
		case CT_Namespace_Body:
		case CT_Struct_Body:
		case CT_Union_Body:
			body_to_string_ref( cast(CodeBody, self), result );
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
		log_fmt( "AST::is_equal: other is null\nAST: %SC", code_debug_str(self) );
		return false;
	}

	if ( self->Type != other->Type )
	{
		log_fmt("AST::is_equal: Type check failure with other\nAST: %SC\nOther: %SC"
			, code_debug_str(self)
			,code_debug_str(other)
		);

		return false;
	}

	switch ( self->Type )
	{
	#define check_member_val( val )                           \
	if ( self->val != other->val )                            \
	{                                                         \
		log_fmt("\nAST::is_equal: Member - " #val " failed\n" \
		        "AST  : %SC\n"                                \
		        "Other: %SC\n"                                \
		    , code_debug_str(self)                            \
		    ,code_debug_str(other)                            \
		);                                                    \
                                                              \
		return false;                                         \
	}

	#define check_member_str( str )                                 \
	if ( ! strc_are_equal( self->str, other->str ) )                \
	{                                                               \
		log_fmt("\nAST::is_equal: Member string - "#str " failed\n" \
				"AST  : %SC\n"                                      \
				"Other: %SC\n"                                      \
			, code_debug_str(self)                                  \
			,code_debug_str(other)                                  \
		);                                                          \
	                                                                \
		return false;                                               \
	}

	#define check_member_content( content )                                \
	if ( ! strc_are_equal( self->content, other->content ))                \
	{                                                                      \
		log_fmt("\nAST::is_equal: Member content - "#content " failed\n"   \
				"AST  : %SC\n"                                             \
				"Other: %SC\n"                                             \
			, code_debug_str(self)                                         \
			, code_debug_str(other)                                        \
		);                                                                 \
                                                                           \
		log_fmt("Content cannot be trusted to be unique with this check "  \
			"so it must be verified by eye for now\n"                      \
			"AST   Content:\n%SC\n"                                        \
			"Other Content:\n%SC\n"                                        \
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
					"AST  : %SC\n"                                                                 \
					"Other: %SC\n"                                                                 \
					"For ast member: %SC\n"                                                        \
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
					"AST  : %SC\n"                                                                 \
					"Other: %SC\n"                                                                 \
					"For     ast member: %SC\n"                                                    \
					"other's ast member: %SC\n"                                                    \
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
									"AST  : %SC\n"
									"Other: %SC\n"
									"For ast member: %SC\n"
								, code_debug_str(curr)
							);

							return false;
						}

						if ( strc_are_equal(curr->Name, curr_other->Name) )
						{
							log_fmt( "\nAST::is_equal: Failed for parameter name check\n"
									"AST  : %SC\n"
									"Other: %SC\n"
									"For     ast member: %SC\n"
									"other's ast member: %SC\n"
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
									"AST  : %SC\n"
									"Other: %SC\n"
									"For     ast member: %SC\n"
									"other's ast member: %SC\n"
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
									"AST  : %SC\n"
									"Other: %SC\n"
									"For     ast member: %SC\n"
									"other's ast member: %SC\n"
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
							"AST  : %SC\n"
							"Other: %SC\n"
						, code_debug_str(curr)
						, code_debug_str(other)
					);

					return false;
				}

				if ( ! code_is_equal( curr, curr_other ) )
				{
					log_fmt( "\nAST::is_equal: Failed for body\n"
							"AST  : %SC\n"
							"Other: %SC\n"
							"For     ast member: %SC\n"
							"other's ast member: %SC\n"
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
#define CheckEntries( Unallowed_Types )                                                                                   \
	do                                                                                                                    \
	{                                                                                                                     \
		CodeBody body = cast(CodeBody, self);                                                                             \
		for ( Code code_entry = begin_CodeBody(body); code_entry != end_CodeBody(body); next_CodeBody(body, code_entry) ) \
		{                                                                                                                 \
			switch ( code_entry->Type )                                                                                   \
			{                                                                                                             \
				Unallowed_Types                                                                                           \
					log_failure( "AST::validate_body: Invalid entry in body %SC", code_debug_str(code_entry) );           \
					return false;                                                                                         \
			}                                                                                                             \
		}                                                                                                                 \
	}                                                                                                                     \
	while (0);

	switch ( self->Type )
	{
		case CT_Class_Body:
		{
			CheckEntries( GEN_AST_BODY_CLASS_UNALLOWED_TYPES );
		}
		break;
		case CT_Enum_Body:
		{
			CodeBody body = cast(CodeBody, self);
			for ( Code entry = begin_CodeBody(body); entry != end_CodeBody(body); next_CodeBody(body, entry) )
			{
				if ( entry->Type != CT_Untyped )
				{
					log_failure( "AST::validate_body: Invalid entry in enum body (needs to be untyped or comment) %SC", code_debug_str(entry) );
					return false;
				}
			}
		}
		break;
		case CT_Export_Body:
		{
			CheckEntries( GEN_AST_BODY_CLASS_UNALLOWED_TYPES );
		}
		break;
		case CT_Extern_Linkage:
		{
			CheckEntries( GEN_AST_BODY_EXTERN_LINKAGE_UNALLOWED_TYPES );
		}
		break;
		case CT_Function_Body:
		{
			CheckEntries( GEN_AST_BODY_FUNCTION_UNALLOWED_TYPES );
		}
		break;
		case CT_Global_Body:
		{
			CodeBody body = cast(CodeBody, self);
			for ( Code entry = begin_CodeBody(body); entry != end_CodeBody(body); next_CodeBody(body, entry) )
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
						log_failure("AST::validate_body: Invalid entry in body %SC", code_debug_str(entry));
					return false;
				}
			}
		}
		break;
		case CT_Namespace_Body:
		{
			CheckEntries( GEN_AST_BODY_NAMESPACE_UNALLOWED_TYPES );
		}
		break;
		case CT_Struct_Body:
		{
			CheckEntries( GEN_AST_BODY_STRUCT_UNALLOWED_TYPES );
		}
		break;
		case CT_Union_Body:
		{
			CodeBody body = cast(CodeBody, self);
			for ( Code entry = begin_CodeBody(body); entry != end_CodeBody(body); next_CodeBody(body, entry) )
			{
				if ( entry->Type != CT_Untyped )
				{
					log_failure( "AST::validate_body: Invalid entry in union body (needs to be untyped or comment) %SC", code_debug_str(entry) );
					return false;
				}
			}
		}
		break;

		default:
			log_failure( "AST::validate_body: Invalid this AST does not have a body %SC", code_debug_str(self) );
			return false;
	}

	return false;

#undef CheckEntries
}