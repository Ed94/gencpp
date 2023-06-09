#ifdef gen_time
#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_FEATURE_PARSING
#include "Parsed\Array.Parsed.hpp"
#include "Parsed\Buffer.Parsed.hpp"
#include "Parsed\HashTable.Parsed.hpp"
#include "Parsed\Ring.Parsed.hpp"
#include "Parsed\Sanity.Parsed.hpp"
#include "SOA.hpp"
#include "gen.cpp"

using namespace gen;

// TODO : Rewrite this to include both upfront and parsed testing.

int gen_main()
{
	Memory::setup();
	gen::init();

	gen_sanity();

	gen_array( u8 );
	gen_array( sw );

	gen_buffer( u8 );

	gen_hashtable( u32 );

	gen_ring( s16 );
	gen_ring( uw );

	gen_array_file();
	gen_buffer_file();
	gen_hashtable_file();
	gen_ring_file();

	Builder soa_test; soa_test.open( "SOA.gen.hpp" );

	soa_test.print( parse_using( code(
		using u16 = unsigned short;
	)));

	soa_test.print( def_include( txt_StrC("Bloat.hpp")));

	soa_test.print( def_using_namespace( name(gen) ) );

	soa_test.print( gen_SOA(
		parse_struct( code(
			struct TestStruct
			{
				u8  A;
				u16 B;
				u32 C;
				u64 D;
			};
		)),
		false
	));

	soa_test.write();

	gen::deinit();
	Memory::cleanup();
	return 0;
}
#endif


#ifdef runtime
int main()
{
	return 0;
}
#endif
