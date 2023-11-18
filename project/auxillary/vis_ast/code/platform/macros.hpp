#pragma once
#if INTELLISENSE_DIRECTIVES
#include "vendor/compiler.hpp"
#endif

#define global        static // Global variables
#define internal      static // Internal linkage
#define local_persist static // Local Persisting variables

#define api_c extern "C"

#define ccast( type, value ) ( const_cast< type >( (value) ) )
#define pcast( type, value ) ( * reinterpret_cast< type* >( & ( value ) ) )
#define rcast( type, value ) reinterpret_cast< type >( value )
#define scast( type, value ) static_cast< type >( value )

#define do_once()   for ( local_persist b32 once = true; once; once = false )
#define stmt( ... ) do { __VA_ARGS__; } while ( 0 )

#define array_count( array ) ( sizeof( array ) / sizeof( ( array )[0] ) )

#define kilobytes( x ) ( ( x ) * ( s64 )( 1024 ) )
#define megabytes( x ) ( kilobytes( x ) * ( s64 )( 1024 ) )
#define gigabytes( x ) ( megabytes( x ) * ( s64 )( 1024 ) )
#define terabytes( x ) ( gigabytes( x ) * ( s64 )( 1024 ) )
