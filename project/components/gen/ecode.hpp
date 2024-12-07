#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "components/types.hpp"
#endif

// This file was generated automatially by gencpp's bootstrap.cpp (See: https://github.com/Ed94/gencpp)

enum CodeType : u32
{
CT_Invalid,CT_Untyped,CT_NewLine,CT_Comment,CT_Access_Private,CT_Access_Protected,CT_Access_Public,CT_PlatformAttributes,CT_Class,CT_Class_Fwd,CT_Class_Body,CT_Constructor,CT_Constructor_Fwd,CT_Destructor,CT_Destructor_Fwd,CT_Enum,CT_Enum_Fwd,CT_Enum_Body,CT_Enum_Class,CT_Enum_Class_Fwd,CT_Execution,CT_Export_Body,CT_Extern_Linkage,CT_Extern_Linkage_Body,CT_Friend,CT_Function,CT_Function_Fwd,CT_Function_Body,CT_Global_Body,CT_Module,CT_Namespace,CT_Namespace_Body,CT_Operator,CT_Operator_Fwd,CT_Operator_Member,CT_Operator_Member_Fwd,CT_Operator_Cast,CT_Operator_Cast_Fwd,CT_Parameters,CT_Preprocess_Define,CT_Preprocess_Include,CT_Preprocess_If,CT_Preprocess_IfDef,CT_Preprocess_IfNotDef,CT_Preprocess_ElIf,CT_Preprocess_Else,CT_Preprocess_EndIf,CT_Preprocess_Pragma,CT_Specifiers,CT_Struct,CT_Struct_Fwd,CT_Struct_Body,CT_Template,CT_Typedef,CT_Typename,CT_Union,CT_Union_Fwd,CT_Union_Body,CT_Using,CT_Using_Namespace,CT_Variable,CT_NumTypes
};
typedef enum CodeType CodeType;
 inline
StrC codetype_to_str( CodeType type)
{
local_persist StrC lookup[61] = { { sizeof("Invalid"), "Invalid" },
{ sizeof("Untyped"), "Untyped" },
{ sizeof("NewLine"), "NewLine" },
{ sizeof("Comment"), "Comment" },
{ sizeof("Access_Private"), "Access_Private" },
{ sizeof("Access_Protected"), "Access_Protected" },
{ sizeof("Access_Public"), "Access_Public" },
{ sizeof("PlatformAttributes"), "PlatformAttributes" },
{ sizeof("Class"), "Class" },
{ sizeof("Class_Fwd"), "Class_Fwd" },
{ sizeof("Class_Body"), "Class_Body" },
{ sizeof("Constructor"), "Constructor" },
{ sizeof("Constructor_Fwd"), "Constructor_Fwd" },
{ sizeof("Destructor"), "Destructor" },
{ sizeof("Destructor_Fwd"), "Destructor_Fwd" },
{ sizeof("Enum"), "Enum" },
{ sizeof("Enum_Fwd"), "Enum_Fwd" },
{ sizeof("Enum_Body"), "Enum_Body" },
{ sizeof("Enum_Class"), "Enum_Class" },
{ sizeof("Enum_Class_Fwd"), "Enum_Class_Fwd" },
{ sizeof("Execution"), "Execution" },
{ sizeof("Export_Body"), "Export_Body" },
{ sizeof("Extern_Linkage"), "Extern_Linkage" },
{ sizeof("Extern_Linkage_Body"), "Extern_Linkage_Body" },
{ sizeof("Friend"), "Friend" },
{ sizeof("Function"), "Function" },
{ sizeof("Function_Fwd"), "Function_Fwd" },
{ sizeof("Function_Body"), "Function_Body" },
{ sizeof("Global_Body"), "Global_Body" },
{ sizeof("Module"), "Module" },
{ sizeof("Namespace"), "Namespace" },
{ sizeof("Namespace_Body"), "Namespace_Body" },
{ sizeof("Operator"), "Operator" },
{ sizeof("Operator_Fwd"), "Operator_Fwd" },
{ sizeof("Operator_Member"), "Operator_Member" },
{ sizeof("Operator_Member_Fwd"), "Operator_Member_Fwd" },
{ sizeof("Operator_Cast"), "Operator_Cast" },
{ sizeof("Operator_Cast_Fwd"), "Operator_Cast_Fwd" },
{ sizeof("Parameters"), "Parameters" },
{ sizeof("Preprocess_Define"), "Preprocess_Define" },
{ sizeof("Preprocess_Include"), "Preprocess_Include" },
{ sizeof("Preprocess_If"), "Preprocess_If" },
{ sizeof("Preprocess_IfDef"), "Preprocess_IfDef" },
{ sizeof("Preprocess_IfNotDef"), "Preprocess_IfNotDef" },
{ sizeof("Preprocess_ElIf"), "Preprocess_ElIf" },
{ sizeof("Preprocess_Else"), "Preprocess_Else" },
{ sizeof("Preprocess_EndIf"), "Preprocess_EndIf" },
{ sizeof("Preprocess_Pragma"), "Preprocess_Pragma" },
{ sizeof("Specifiers"), "Specifiers" },
{ sizeof("Struct"), "Struct" },
{ sizeof("Struct_Fwd"), "Struct_Fwd" },
{ sizeof("Struct_Body"), "Struct_Body" },
{ sizeof("Template"), "Template" },
{ sizeof("Typedef"), "Typedef" },
{ sizeof("Typename"), "Typename" },
{ sizeof("Union"), "Union" },
{ sizeof("Union_Fwd"), "Union_Fwd" },
{ sizeof("Union_Body"), "Union_Body" },
{ sizeof("Using"), "Using" },
{ sizeof("Using_Namespace"), "Using_Namespace" },
{ sizeof("Variable"), "Variable" },
 }; return lookup[ type ];
}

