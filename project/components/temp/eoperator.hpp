// This is the non-bootstraped version of the EOperator. This will be obsolete once bootstrap is stress tested.

namespace EOperator
{
#	define Define_Operators       \
	Entry( Invalid,         INVALID ) \
	Entry( Assign,          =  )  \
	Entry( Assign_Add,      += )  \
	Entry( Assign_Subtract, -= )  \
	Entry( Assign_Multiply, *= )  \
	Entry( Assign_Divide,   /= )  \
	Entry( Assign_Modulo,   %= )  \
	Entry( Assign_BAnd,     &= )  \
	Entry( Assign_BOr,      |= )  \
	Entry( Assign_BXOr,     ^= )  \
	Entry( Assign_LShift,   <<= ) \
	Entry( Assign_RShift,   >>= ) \
	Entry( Increment,       ++ )  \
	Entry( Decrement,       -- )  \
	Entry( Unary_Plus,      + )   \
	Entry( Unary_Minus,     - )   \
	Entry( UnaryNot,        ! )   \
	Entry( Add,             + )   \
	Entry( Subtract,        - )   \
	Entry( Multiply,        * )   \
	Entry( Divide,          / )   \
	Entry( Modulo,          % )   \
	Entry( BNot,            ~ )   \
	Entry( BAnd,            & )   \
	Entry( BOr,             | )   \
	Entry( BXOr,            ^ )   \
	Entry( LShift,          << )  \
	Entry( RShift,          >> )  \
	Entry( LAnd,            && )  \
	Entry( LOr,             || )  \
	Entry( LEqual,          == )  \
	Entry( LNot,            != )  \
	Entry( Lesser,          < )   \
	Entry( Greater,         > )   \
	Entry( LesserEqual,     <= )  \
	Entry( GreaterEqual,    >= )  \
	Entry( Subscript,       [] )  \
	Entry( Indirection,     * )   \
	Entry( AddressOf,       & )   \
	Entry( MemberOfPointer, -> )  \
	Entry( PtrToMemOfPtr,   ->* ) \
	Entry( FunctionCall,    () )

	enum Type : u32
	{
	#	define Entry( Type_, Token_ ) Type_,
		Define_Operators
	#	undef Entry
		Comma,

		Num_Ops,
	};

	inline
	char const* to_str( Type op )
	{
		local_persist
		char const* lookup[ Num_Ops ] = {
		#	define Entry( Type_, Token_ ) stringize(Token_),
			Define_Operators
		#	undef Entry
			","
		};

		return lookup[ op ];
	}

#	undef Define_Operators
}
using OperatorT = EOperator::Type;

