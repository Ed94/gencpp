#define GENC_IMPLEMENTATION
#include "genc.h"
#include "table.h"


struct Test
{
	u64 A;
	u64 B;
};



#if gen_time


u64 hash_struct( void* test )
{
	return crc32( ((Test)test).A, sizeof(u64) );
}


int gen_main()
{
	gen_table( Test, & hash_struct )

	gen_table_file();
}	
#endif



#if runtime
int main()
{
	Table(Test) test_table;

	
}
#endif