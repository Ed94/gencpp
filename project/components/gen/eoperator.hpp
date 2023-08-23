#pragma once

// This file was generated automatially by gencpp's bootstrap.cpp (See: https://github.com/Ed94/gencpp)

namespace EOperator
{
	enum Type : u32
	{
		Invalid,
		Assign,
		Assign_Add,
		Assign_Subtract,
		Assign_Multiply,
		Assign_Divide,
		Assign_Modulo,
		Assign_BAnd,
		Assign_BOr,
		Assign_BXOr,
		Assign_LShift,
		Assign_RShift,
		Increment,
		Decrement,
		Unary_Plus,
		Unary_Minus,
		UnaryNot,
		Add,
		Subtract,
		Multiply,
		Divide,
		Modulo,
		BNot,
		BAnd,
		BOr,
		BXOr,
		LShift,
		RShift,
		LAnd,
		LOr,
		LEqual,
		LNot,
		Lesser,
		Greater,
		LesserEqual,
		GreaterEqual,
		Subscript,
		Indirection,
		AddressOf,
		MemberOfPointer,
		PtrToMemOfPtr,
		FunctionCall,
		Comma,
		NumOps
	};

	StrC to_str( Type op )
	{
		local_persist StrC lookup[] {
			{sizeof( "INVALID" ), "INVALID"},
			{ sizeof( "=" ),      "="      },
			{ sizeof( "+=" ),     "+="     },
			{ sizeof( "-=" ),     "-="     },
			{ sizeof( "*=" ),     "*="     },
			{ sizeof( "/=" ),     "/="     },
			{ sizeof( "%=" ),     "%="     },
			{ sizeof( "&=" ),     "&="     },
			{ sizeof( "|=" ),     "|="     },
			{ sizeof( "^=" ),     "^="     },
			{ sizeof( "<<=" ),    "<<="    },
			{ sizeof( ">>=" ),    ">>="    },
			{ sizeof( "++" ),     "++"     },
			{ sizeof( "--" ),     "--"     },
			{ sizeof( "+" ),      "+"      },
			{ sizeof( "-" ),      "-"      },
			{ sizeof( "!" ),      "!"      },
			{ sizeof( "+" ),      "+"      },
			{ sizeof( "-" ),      "-"      },
			{ sizeof( "*" ),      "*"      },
			{ sizeof( "/" ),      "/"      },
			{ sizeof( "%" ),      "%"      },
			{ sizeof( "~" ),      "~"      },
			{ sizeof( "&" ),      "&"      },
			{ sizeof( "|" ),      "|"      },
			{ sizeof( "^" ),      "^"      },
			{ sizeof( "<<" ),     "<<"     },
			{ sizeof( ">>" ),     ">>"     },
			{ sizeof( "&&" ),     "&&"     },
			{ sizeof( "||" ),     "||"     },
			{ sizeof( "==" ),     "=="     },
			{ sizeof( "!=" ),     "!="     },
			{ sizeof( "<" ),      "<"      },
			{ sizeof( ">" ),      ">"      },
			{ sizeof( "<=" ),     "<="     },
			{ sizeof( ">=" ),     ">="     },
			{ sizeof( "[]" ),     "[]"     },
			{ sizeof( "*" ),      "*"      },
			{ sizeof( "&" ),      "&"      },
			{ sizeof( "->" ),     "->"     },
			{ sizeof( "->*" ),    "->*"    },
			{ sizeof( "()" ),     "()"     },
			{ sizeof( "," ),      ","      },
		};
		return lookup[ op ];
	}

}    // namespace EOperator

using OperatorT = EOperator::Type;
