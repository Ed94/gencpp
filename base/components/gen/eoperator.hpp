#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "components/types.hpp"
#endif

// This file was generated automatially by gencpp's bootstrap.cpp (See: https://github.com/Ed94/gencpp)

enum Operator : u32
{
	Op_Invalid,
	Op_Assign,
	Op_Assign_Add,
	Op_Assign_Subtract,
	Op_Assign_Multiply,
	Op_Assign_Divide,
	Op_Assign_Modulo,
	Op_Assign_BAnd,
	Op_Assign_BOr,
	Op_Assign_BXOr,
	Op_Assign_LShift,
	Op_Assign_RShift,
	Op_Increment,
	Op_Decrement,
	Op_Unary_Plus,
	Op_Unary_Minus,
	Op_UnaryNot,
	Op_Add,
	Op_Subtract,
	Op_Multiply,
	Op_Divide,
	Op_Modulo,
	Op_BNot,
	Op_BAnd,
	Op_BOr,
	Op_BXOr,
	Op_LShift,
	Op_RShift,
	Op_LAnd,
	Op_LOr,
	Op_LEqual,
	Op_LNot,
	Op_Lesser,
	Op_Greater,
	Op_LesserEqual,
	Op_GreaterEqual,
	Op_Subscript,
	Op_Indirection,
	Op_AddressOf,
	Op_MemberOfPointer,
	Op_PtrToMemOfPtr,
	Op_FunctionCall,
	Op_Comma,
	Op_New,
	Op_NewArray,
	Op_Delete,
	Op_DeleteArray,
	Op_NumOps,
	Op_UnderlyingType = 0xffffffffu
};

inline Str operator_to_str( Operator op )
{
	local_persist Str lookup[47] = {
		{ "INVALID",  sizeof( "INVALID" )  },
		{ "=",        sizeof( "=" )        },
		{ "+=",       sizeof( "+=" )       },
		{ "-=",       sizeof( "-=" )       },
		{ "*=",       sizeof( "*=" )       },
		{ "/=",       sizeof( "/=" )       },
		{ "%=",       sizeof( "%=" )       },
		{ "&=",       sizeof( "&=" )       },
		{ "|=",       sizeof( "|=" )       },
		{ "^=",       sizeof( "^=" )       },
		{ "<<=",      sizeof( "<<=" )      },
		{ ">>=",      sizeof( ">>=" )      },
		{ "++",       sizeof( "++" )       },
		{ "--",       sizeof( "--" )       },
		{ "+",        sizeof( "+" )        },
		{ "-",        sizeof( "-" )        },
		{ "!",        sizeof( "!" )        },
		{ "+",        sizeof( "+" )        },
		{ "-",        sizeof( "-" )        },
		{ "*",        sizeof( "*" )        },
		{ "/",        sizeof( "/" )        },
		{ "%",        sizeof( "%" )        },
		{ "~",        sizeof( "~" )        },
		{ "&",        sizeof( "&" )        },
		{ "|",        sizeof( "|" )        },
		{ "^",        sizeof( "^" )        },
		{ "<<",       sizeof( "<<" )       },
		{ ">>",       sizeof( ">>" )       },
		{ "&&",       sizeof( "&&" )       },
		{ "||",       sizeof( "||" )       },
		{ "==",       sizeof( "==" )       },
		{ "!=",       sizeof( "!=" )       },
		{ "<",        sizeof( "<" )        },
		{ ">",        sizeof( ">" )        },
		{ "<=",       sizeof( "<=" )       },
		{ ">=",       sizeof( ">=" )       },
		{ "[]",       sizeof( "[]" )       },
		{ "*",        sizeof( "*" )        },
		{ "&",        sizeof( "&" )        },
		{ "->",       sizeof( "->" )       },
		{ "->*",      sizeof( "->*" )      },
		{ "()",       sizeof( "()" )       },
		{ ",",        sizeof( "," )        },
		{ "new",      sizeof( "new" )      },
		{ "new[]",    sizeof( "new[]" )    },
		{ "delete",   sizeof( "delete" )   },
		{ "delete[]", sizeof( "delete[]" ) },
	};
	return lookup[op];
}

forceinline Str to_str( Operator op )
{
	return operator_to_str( op );
}
