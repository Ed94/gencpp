#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "code_serialization.cpp"
#endif

namespace parser {
internal void init();
internal void deinit();
}

internal
void* Global_Allocator_Proc( void* allocator_data, AllocType type, ssize size, ssize alignment, void* old_memory, ssize old_size, u64 flags )
{
	Arena* last = back(& Global_AllocatorBuckets);

	switch ( type )
	{
		case EAllocation_ALLOC:
		{
			if ( ( last->TotalUsed + size ) > last->TotalSize )
			{
				Arena bucket = arena_init_from_allocator( heap(), Global_BucketSize );

				if ( bucket.PhysicalStart == nullptr )
					GEN_FATAL( "Failed to create bucket for Global_AllocatorBuckets");

				if ( ! append( & Global_AllocatorBuckets, bucket ) )
					GEN_FATAL( "Failed to append bucket to Global_AllocatorBuckets");

				last = back(& Global_AllocatorBuckets);
			}

			return alloc_align( allocator_info(last), size, alignment );
		}
		case EAllocation_FREE:
		{
			// Doesn't recycle.
		}
		break;
		case EAllocation_FREE_ALL:
		{
			// Memory::cleanup instead.
		}
		break;
		case EAllocation_RESIZE:
		{
			if ( last->TotalUsed + size > last->TotalSize )
			{
				Arena bucket = arena_init_from_allocator( heap(), Global_BucketSize );

				if ( bucket.PhysicalStart == nullptr )
					GEN_FATAL( "Failed to create bucket for Global_AllocatorBuckets");

				if ( ! append( & Global_AllocatorBuckets, bucket ) )
					GEN_FATAL( "Failed to append bucket to Global_AllocatorBuckets");

				last = back(& Global_AllocatorBuckets);
			}

			void* result = alloc_align( last->Backing, size, alignment );

			if ( result != nullptr && old_memory != nullptr )
			{
				mem_copy( result, old_memory, old_size );
			}

			return result;
		}
	}

	return nullptr;
}

internal
void define_constants()
{
	Code_Global                         = make_code();
	scast(String, Code_Global->Name)    = get_cached_string( txt("Global Code") );
	scast(String, Code_Global->Content) = Code_Global->Name;

	Code_Invalid = make_code();
	set_global(Code_Invalid);

	t_empty       = (CodeType) make_code();
	t_empty->Type = ECode::Typename;
	t_empty->Name = get_cached_string( txt("") );
	set_global(t_empty);

	access_private       = make_code();
	access_private->Type = ECode::Access_Private;
	access_private->Name = get_cached_string( txt("private:\n") );
	set_global(access_private);

	access_protected       = make_code();
	access_protected->Type = ECode::Access_Protected;
	access_protected->Name = get_cached_string( txt("protected:\n") );
	set_global(access_protected);

	access_public       = make_code();
	access_public->Type = ECode::Access_Public;
	access_public->Name = get_cached_string( txt("public:\n") );
	set_global(access_public);

	attrib_api_export = def_attributes( code(GEN_API_Export_Code));
	set_global(attrib_api_export);

	attrib_api_import = def_attributes( code(GEN_API_Import_Code));
	set_global(attrib_api_import);

	module_global_fragment          = make_code();
	module_global_fragment->Type    = ECode::Untyped;
	module_global_fragment->Name    = get_cached_string( txt("module;") );
	module_global_fragment->Content = module_global_fragment->Name;
	set_global(module_global_fragment);

	module_private_fragment          = make_code();
	module_private_fragment->Type    = ECode::Untyped;
	module_private_fragment->Name    = get_cached_string( txt("module : private;") );
	module_private_fragment->Content = module_private_fragment->Name;
	set_global(module_private_fragment);

	fmt_newline = make_code();
	fmt_newline->Type = ECode::NewLine;
	set_global(fmt_newline);

	pragma_once          = (CodePragma) make_code();
	pragma_once->Type    = ECode::Preprocess_Pragma;
	pragma_once->Name    = get_cached_string( txt("once") );
	pragma_once->Content = pragma_once->Name;
	set_global(pragma_once);

	param_varadic            = (CodeType) make_code();
	param_varadic->Type      = ECode::Parameters;
	param_varadic->Name      = get_cached_string( txt("...") );
	param_varadic->ValueType = t_empty;
	set_global(param_varadic);

	preprocess_else = (CodePreprocessCond) make_code();
	preprocess_else->Type = ECode::Preprocess_Else;
	set_global(preprocess_else);

	preprocess_endif = (CodePreprocessCond) make_code();
	preprocess_endif->Type = ECode::Preprocess_EndIf;
	set_global(preprocess_endif);

#	define def_constant_code_type( Type_ )   \
		t_##Type_ = def_type( name(Type_) ); \
		set_global(t_##Type_);

	def_constant_code_type( auto );
	def_constant_code_type( void );
	def_constant_code_type( int );
	def_constant_code_type( bool );
	def_constant_code_type( char );
	def_constant_code_type( wchar_t );
	def_constant_code_type( class );
	def_constant_code_type( typename );

#ifdef GEN_DEFINE_LIBRARY_CODE_CONSTANTS
	t_b32 = def_type( name(b32) );

	def_constant_code_type( s8 );
	def_constant_code_type( s16 );
	def_constant_code_type( s32 );
	def_constant_code_type( s64 );

	def_constant_code_type( u8 );
	def_constant_code_type( u16 );
	def_constant_code_type( u32 );
	def_constant_code_type( u64 );

	def_constant_code_type( ssize );
	def_constant_code_type( usize );

	def_constant_code_type( f32 );
	def_constant_code_type( f64 );
#endif
#	undef def_constant_code_type


#	define def_constant_spec( Type_, ... )                                  \
		spec_##Type_ = def_specifiers( num_args(__VA_ARGS__), __VA_ARGS__); \
		set_global(spec_##Type_);

#	pragma push_macro("forceinline")
#	pragma push_macro("global")
#	pragma push_macro("internal")
#	pragma push_macro("local_persist")
#	pragma push_macro("neverinline")
#	undef forceinline
#	undef global
#	undef internal
#	undef local_persist
#	undef neverinline
	def_constant_spec( const,            ESpecifier::Const );
	def_constant_spec( consteval,        ESpecifier::Consteval );
	def_constant_spec( constexpr,        ESpecifier::Constexpr );
	def_constant_spec( constinit,        ESpecifier::Constinit );
	def_constant_spec( extern_linkage,   ESpecifier::External_Linkage );
	def_constant_spec( final, 		     ESpecifier::Final );
	def_constant_spec( forceinline,      ESpecifier::ForceInline );
	def_constant_spec( global,           ESpecifier::Global );
	def_constant_spec( inline,           ESpecifier::Inline );
	def_constant_spec( internal_linkage, ESpecifier::Internal_Linkage );
	def_constant_spec( local_persist,    ESpecifier::Local_Persist );
	def_constant_spec( mutable,          ESpecifier::Mutable );
	def_constant_spec( neverinline,      ESpecifier::NeverInline );
	def_constant_spec( noexcept,         ESpecifier::NoExceptions );
	def_constant_spec( override,         ESpecifier::Override );
	def_constant_spec( ptr,              ESpecifier::Ptr );
	def_constant_spec( pure,             ESpecifier::Pure )
	def_constant_spec( ref,              ESpecifier::Ref );
	def_constant_spec( register,         ESpecifier::Register );
	def_constant_spec( rvalue,           ESpecifier::RValue );
	def_constant_spec( static_member,    ESpecifier::Static );
	def_constant_spec( thread_local,     ESpecifier::Thread_Local );
	def_constant_spec( virtual, 		 ESpecifier::Virtual );
	def_constant_spec( volatile, 	     ESpecifier::Volatile)

	spec_local_persist = def_specifiers( 1, ESpecifier::Local_Persist );
	set_global(spec_local_persist);

#	pragma pop_macro("forceinline")
#	pragma pop_macro("global")
#	pragma pop_macro("internal")
#	pragma pop_macro("local_persist")
#	pragma pop_macro("neverinline")

#	pragma push_macro("enum_underlying")

#	pragma pop_macro("enum_underlying")

#	undef def_constant_spec
}

void init()
{
	// Setup global allocator
	{
		GlobalAllocator = AllocatorInfo { & Global_Allocator_Proc, nullptr };

		Global_AllocatorBuckets = array_init_reserve<Arena>( heap(), 128 );

		if ( Global_AllocatorBuckets == nullptr )
			GEN_FATAL( "Failed to reserve memory for Global_AllocatorBuckets");

		Arena bucket = arena_init_from_allocator( heap(), Global_BucketSize );

		if ( bucket.PhysicalStart == nullptr )
			GEN_FATAL( "Failed to create first bucket for Global_AllocatorBuckets");

		append( & Global_AllocatorBuckets, bucket );
	}

	// Setup the arrays
	{
		CodePools = array_init_reserve<Pool>( Allocator_DataArrays, InitSize_DataArrays );

		if ( CodePools == nullptr )
			GEN_FATAL( "gen::init: Failed to initialize the CodePools array" );

		StringArenas = array_init_reserve<Arena>( Allocator_DataArrays, InitSize_DataArrays );

		if ( StringArenas == nullptr )
			GEN_FATAL( "gen::init: Failed to initialize the StringArenas array" );
	}

	// Setup the code pool and code entries arena.
	{
		Pool code_pool = pool_init( Allocator_CodePool, CodePool_NumBlocks, sizeof(AST) );

		if ( code_pool.PhysicalStart == nullptr )
			GEN_FATAL( "gen::init: Failed to initialize the code pool" );

		append( & CodePools, code_pool );

		LexArena = arena_init_from_allocator( Allocator_Lexer, LexAllocator_Size );

		Arena string_arena = arena_init_from_allocator( Allocator_StringArena, SizePer_StringArena );

		if ( string_arena.PhysicalStart == nullptr )
			GEN_FATAL( "gen::init: Failed to initialize the string arena" );

		append( & StringArenas, string_arena );
	}

	// Setup the hash tables
	{
		StringCache = hashtable_init<StringCached>(Allocator_StringTable);

		if ( StringCache.Entries == nullptr )
			GEN_FATAL( "gen::init: Failed to initialize the StringCache");
	}

	// Preprocessor Defines
	PreprocessorDefines = array_init_reserve<StringCached>( GlobalAllocator, kilobytes(1) );

	define_constants();
	parser::init();
}

void deinit()
{
	usize index = 0;
	usize left  = num(CodePools);
	do
	{
		Pool* code_pool = & CodePools[index];
		free(code_pool);
		index++;
	}
	while ( left--, left );

	index = 0;
	left  = num(StringArenas);
	do
	{
		Arena* string_arena = & StringArenas[index];
		free(string_arena);
		index++;
	}
	while ( left--, left );

	destroy(& StringCache);

	free( & CodePools);
	free( & StringArenas);

	free(& LexArena);

	free(& PreprocessorDefines);

	index = 0;
	left  = num(Global_AllocatorBuckets);
	do
	{
		Arena* bucket = & Global_AllocatorBuckets[ index ];
		free(bucket);
		index++;
	}
	while ( left--, left );

	free(Global_AllocatorBuckets);
	parser::deinit();
}

void reset()
{
	s32 index = 0;
	s32 left  = num(CodePools);
	do
	{
		Pool* code_pool = & CodePools[index];
		clear(* code_pool);
		index++;
	}
	while ( left--, left );

	index = 0;
	left  = num(StringArenas);
	do
	{
		Arena* string_arena = & StringArenas[index];
		string_arena->TotalUsed = 0;;
		index++;
	}
	while ( left--, left );

	clear(StringCache);

	define_constants();
}

AllocatorInfo get_string_allocator( s32 str_length )
{
	Arena* last = back(& StringArenas);

	usize size_req = str_length + sizeof(StringHeader) + sizeof(char*);

	if ( last->TotalUsed + ssize(size_req) > last->TotalSize )
	{
		Arena new_arena = arena_init_from_allocator( Allocator_StringArena, SizePer_StringArena );

		if ( ! append( & StringArenas, new_arena ) )
			GEN_FATAL( "gen::get_string_allocator: Failed to allocate a new string arena" );

		last = back(& StringArenas);
	}

	return allocator_info(last);
}

// Will either make or retrive a code string.
StringCached get_cached_string( StrC str )
{
	s32 hash_length = str.Len > kilobytes(1) ? kilobytes(1) : str.Len;
	u64 key         = crc32( str.Ptr, hash_length );
	{
		StringCached* result = get(StringCache, key );

		if ( result )
			return * result;
	}

	String result = string_make( get_string_allocator( str.Len ), str );
	set<StringCached>(& StringCache, key, result );

	return result;
}

// Used internally to retireve a Code object form the CodePool.
Code make_code()
{
	Pool* allocator = back( & CodePools);
	if ( allocator->FreeList == nullptr )
	{
		Pool code_pool = pool_init( Allocator_CodePool, CodePool_NumBlocks, sizeof(AST) );

		if ( code_pool.PhysicalStart == nullptr )
			GEN_FATAL( "gen::make_code: Failed to allocate a new code pool - CodePool allcoator returned nullptr." );

		if ( ! append( & CodePools, code_pool ) )
			GEN_FATAL( "gen::make_code: Failed to allocate a new code pool - CodePools failed to append new pool." );

		allocator = back( & CodePools);
	}

	Code result { rcast( AST*, alloc( allocator_info(allocator), sizeof(AST) )) };
	mem_set( result.ast, 0, sizeof(AST) );
	// result->Type = ECode::Invalid;

	// result->Content     = { nullptr };
	// result->Prev        = { nullptr };
	// result->Next        = { nullptr };
	// result->Token		= nullptr;
	// result->Parent      = { nullptr };
	// result->Name        = { nullptr };
	// result->Type        = ECode::Invalid;
	// result->ModuleFlags = ModuleFlag::Invalid;
	// result->NumEntries  = 0;

	return result;
}

void set_allocator_data_arrays( AllocatorInfo allocator )
{
	Allocator_DataArrays = allocator;
}

void set_allocator_code_pool( AllocatorInfo allocator )
{
	Allocator_CodePool = allocator;
}

void set_allocator_lexer( AllocatorInfo allocator )
{
	Allocator_Lexer = allocator;
}

void set_allocator_string_arena( AllocatorInfo allocator )
{
	Allocator_StringArena = allocator;
}

void set_allocator_string_table( AllocatorInfo allocator )
{
	Allocator_StringArena = allocator;
}
