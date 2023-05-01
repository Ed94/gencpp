// Standard Allocation
#define new             static_assert( false, "Banned keyword used: " new )
#define delete          static_assert( false, "Banned keyword used: " delete )

// Standard Coroutines
#define co_await        static_assert( false, "Banned keyword used: " co_await )
#define co_return       static_assert( false, "Banned keyword used: " co_return )
#define co_yield        static_assert( false, "Banned keyword used: " co_yield )

// Standard Exceptions
#define atomic_cancel   static_assert( false, "Banned keyword used: " atomic_cancel )
#define atomic_commit   static_assert( false, "Banned keyword used: " atomic_commit )
#define atomic_noexcept static_assert( false, "Banned keyword used: " atomic_noexcept )
#define catch           static_assert( false, "Banned keyword used: " catch )
#define noexcept        static_assert( false, "Banned keyword used: " noexcept )
#define throw           static_assert( false, "Banned keyword used: " throw )
#define try	            static_assert( false, "Banned keyword used: " try )

// Standard RTTI
#define decltype        static_assert( false, "Banned keyword used: " decltype )
#define reflexpr        static_assert( false, "Banned keyword used: " reflexpr )
#define typeid          static_assert( false, "Banned keyword used: " typeid )

// Object-Oriented Dynamic Dispatch
#define final           static_assert( false, "Banned keyword used: " final )
#define override        static_assert( false, "Banned keyword used: " override )
#define virtual         static_assert( false, "Banned keyword used: " virtual )

// Private Access Specifier
#define private         static_assert( false, "Banned keyword used: " private )

// Template Meta-programming
#define concept         static_assert( false, "Banned keyword used: " concept )
#define requires        static_assert( false, "Banned keyword used: " requires )
#define template        static_assert( false, "Banned keyword used: " template )
