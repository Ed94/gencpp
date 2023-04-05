#include "gen.h"

#define Table( Type_ ) Table_##Type_

typedef u64(*)(void*) HashingFn;

#if gen_time
#	define gen_table( Type_, HashingFn_ ) gen_request_table( #Type_, sizeof(Type_), HashingFn_ ) 

	u64 table_default_hash_fn( void* address )
	{
		return crc32( address, 4 );
	}

	Code gen_table_code( char const* type_str, sw type_size, HashingFn hash_fn )
	{
		Code table;

		return table;
	}

	struct TableRequest
	{
		char const* Type;
		sw          Size;
		HashingFn   HashFn;
	};

	array(TableRequest) TableRequests;

	void gen_request_table( const char* type_str, sw type_size, HashingFn hash_fn )
	{
		TableRequest request = { type_str, type_size, hash_fn };

		array_append( TableRequests, request );
	}

	u32 gen_table_file()
	{
		gen_table( u32 );
		gen_table( char const* );

		array(Code) array_asts;
		array_init( array_asts, g_allocator );

		sw left  = array_count( TableRequests );
		sw index = 0;
		while( left -- )
		{
			ArrayRequest request = TableRequests[index];

			Code result = gen_table_code( request.Name, request.Size, request.HashFn );

			array_append( array_asts, result );
		}

		Builder
		arraygen;
		arraygen.open( "table.gen.h" );

		left  = array_count( array_asts );
		index = 0;

		while( left-- )
		{
			Code code = array_asts[index];

			arraygen.print( code );
		}

		arraygen.write();
		return 0;
	}
#endif

#ifndef gen_time
#	include "table.gen.h"
#endif

