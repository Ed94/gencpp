// Testing to make sure backend of library is operating properly.

#ifdef gen_time
#define GEN_FEATURE_PARSING
#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#define GEN_EXPOSE_BACKEND
#define GEN_BENCHMARK
#include "gen.hpp"

void check_sanity()
{
	using namespace gen;
	gen::init();
	log_fmt("\ncheck_sanity:\n");

	// Test string caching:
	CodeType t_int_dupe = def_type( name(int) );

	if ( t_int_dupe->Name != t_int->Name )
		fatal("check_sanity: String caching failed!");


	// Purposefully use an excessive amount of memory to make so the the memory backend doesn't break.
	// This has been tested with num_iterations set to 15000000 (generates 15 million lines of code), the Global_BlockSize, along with CodePool_NumBlocks, and SizePer_StringArena
	// must be adjusted to gigabytes(2), kilobytes(512), and gigabyte(1) for good performance without crashing.
	/*
		Typical usage (megabytes(10), kilobytes(4), megabytes(1), for 650000 (the limit of 10 meg partition buckets in global arena) )
		Memory after builder:
		Num Global Arenas       : 14 TotalSize: 146800640 !
		Num Code Pools          : 794 TotalSize: 416284672 !
		Num String Cache Arenas : 60 TotalSize: 62914560 !
		Num String Cache        : 1300007

		Memory usage to expect at 15 mil file:
		Num Global Arenas       : 2 TotalSize: 4294967296 !
		Num Code Pools          : 144 TotalSize: 9663676416 !
		Num String Cache Arenas : 2 TotalSize: 2147483648 !
		Num String Cache        : 30000025
	*/
	constexpr
	s32 num_iterations = 650000;

	Array<CodeTypedef> typedefs = Array<CodeTypedef>::init_reserve( Memory::GlobalAllocator, num_iterations * 2 );

	s32 idx = num_iterations;
	while( --idx )
	{
		// Stress testing string allocation
		String type_name = String::fmt_buf( Memory::GlobalAllocator, "type_%d", idx );
		String typedef_name = String::fmt_buf( Memory::GlobalAllocator, "typedef_%d", idx );

		CodeTypedef type_as_int = def_typedef( type_name, t_int );
		CodeType    type        = def_type( type_name );
		CodeTypedef type_def    = def_typedef( typedef_name, type );

		typedefs.append( type_as_int );
		typedefs.append( type_def );
	}

	log_fmt("\nMemory before builder:\n");
	log_fmt("Num Global Arenas       : %llu TotalSize: %llu !\n", Memory::Global_AllocatorBuckets.num(), Memory::Global_AllocatorBuckets.num() * Memory::Global_BucketSize);
	log_fmt("Num Code Pools          : %llu TotalSize: %llu !\n", StaticData::CodePools.num(), StaticData::CodePools.num() * CodePool_NumBlocks * StaticData::CodePools.back().BlockSize);
	log_fmt("Num String Cache Arenas : %llu TotalSize: %llu !\n", StaticData::StringArenas.num(), StaticData::StringArenas.num() * SizePer_StringArena);
	log_fmt("Num String Cache        : %llu\n", StaticData::StringCache.Entries.num(), StaticData::StringCache);

	Builder builder;
	builder.open( "sanity.gen.hpp" );

	idx = num_iterations;
#ifdef GEN_BENCHMARK
	u64 time_start     = time_rel_ms();
#endif
	while( --idx )
	{
		builder.print( typedefs[idx] );
	}

	builder.write();
#ifdef GEN_BENCHMARK
	log_fmt("\n\nBuilder finished writting. Time taken: %llu ms\n", time_rel_ms() - time_start);
#endif

	log_fmt("\nMemory after builder:\n");
	log_fmt("Num Global Arenas       : %llu TotalSize: %llu !\n", Memory::Global_AllocatorBuckets.num(), Memory::Global_AllocatorBuckets.num() * Memory::Global_BucketSize);
	log_fmt("Num Code Pools          : %llu TotalSize: %llu !\n", StaticData::CodePools.num(), StaticData::CodePools.num() * CodePool_NumBlocks * StaticData::CodePools.back().BlockSize);
	log_fmt("Num String Cache Arenas : %llu TotalSize: %llu !\n", StaticData::StringArenas.num(), StaticData::StringArenas.num() * SizePer_StringArena);
	log_fmt("Num String Cache        : %llu\n", StaticData::StringCache.Entries.num(), StaticData::StringCache);

	log_fmt("\nSanity passed!\n");
	gen::deinit();
}
#endif
