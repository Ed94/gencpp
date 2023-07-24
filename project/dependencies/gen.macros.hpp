#define zpl_cast( Type ) ( Type )

// Keywords

#define global        static    // Global variables
#define internal      static    // Internal linkage
#define local_persist static    // Local Persisting variables

// Bits

#define bit( Value )                             ( 1 << Value )
#define bitfield_is_equal( Type, Field, Mask ) ( (Type(Mask) & Type(Field)) == Type(Mask) )

// Casting
#define ccast( Type, Value ) ( * const_cast< Type* >( & (Value) ) )
#define pcast( Type, Value ) ( * reinterpret_cast< Type* >( & ( Value ) ) )
#define rcast( Type, Value ) reinterpret_cast< Type >( Value )
#define scast( Type, Value ) static_cast< Type >( Value )

// Num Arguments (Varadics)
#if defined(__GNUC__) || defined(__clang__)
// Supports 0-10 arguments
#define num_args_impl( _0,                                \
		_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10,  \
		_11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
		N, ...                                            \
	) N
	// _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
	// _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
	// _41, _42, _43, _44, _45, _46, _47, _48, _49, _50,

	// ## deletes preceding comma if _VA_ARGS__ is empty (GCC, Clang)
#define num_args(...)                           \
	num_args_impl(_, ## __VA_ARGS__,            \
		20, 19, 18, 17, 16, 15, 14, 13, 12, 11, \
		10,  9,  8,  7,  6,  5,  4,  3,  2,  1, \
		0                                       \
	)
	// 50, 49, 48, 47, 46, 45, 44, 43, 42, 41,              \
	// 40, 39, 38, 37, 36, 35, 34, 33, 32, 31,              \
	// 30, 29, 28, 27, 26, 25, 24, 23, 22, 21,
#else
// Supports 1-10 arguments
#define num_args_impl(                                    \
		_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10,  \
		_11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
		N, ...                                            \
	) N

#define num_args(...)                           \
	num_args_impl( __VA_ARGS__,                 \
		20, 19, 18, 17, 16, 15, 14, 13, 12, 11, \
		10,  9,  8,  7,  6,  5,  4,  3,  2,  1  \
	)
#endif

// Stringizing
#define stringize_va( ... ) #__VA_ARGS__
#define stringize( ... )    stringize_va( __VA_ARGS__ )

// Function do once

#define do_once()          \
	do                     \
	{                      \
		static             \
		bool Done = false; \
		if ( Done )        \
			return;        \
		Done = true;       \
	}                      \
	while(0)

#define do_once_start      \
	do                     \
	{                      \
		static             \
		bool Done = false; \
		if ( Done )        \
			break;         \
		Done = true;

#define do_once_end        \
	}                      \
	while(0);

#define clamp( x, lower, upper )      min( max( ( x ), ( lower ) ), ( upper ) )
#define count_of( x )                 ( ( size_of( x ) / size_of( 0 [ x ] ) ) / ( ( sw )( ! ( size_of( x ) % size_of( 0 [ x ] ) ) ) ) )
#define is_between( x, lower, upper ) ( ( ( lower ) <= ( x ) ) && ( ( x ) <= ( upper ) ) )
#define max( a, b )                   ( ( a ) > ( b ) ? ( a ) : ( b ) )
#define min( a, b )                   ( ( a ) < ( b ) ? ( a ) : ( b ) )
#define size_of( x )                  ( sw )( sizeof( x ) )

template< class Type >
void swap( Type a, Type b )
{
	Type tmp = a;
	a = b;
	b = tmp;
}
