#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "ast.cpp"
#endif

internal void init_parser();
internal void deinit_parser();

internal
void* Global_Allocator_Proc( void* allocator_data, AllocType type, sw size, sw alignment, void* old_memory, sw old_size, u64 flags )
{
	Arena* last = & Global_AllocatorBuckets.back();

	switch ( type )
	{
		case EAllocation_ALLOC:
		{
			if ( ( last->TotalUsed + size ) > last->TotalSize )
			{
				Arena bucket = Arena::init_from_allocator( heap(), Global_BucketSize );

				if ( bucket.PhysicalStart == nullptr )
					GEN_FATAL( "Failed to create bucket for Global_AllocatorBuckets");

				if ( ! Global_AllocatorBuckets.append( bucket ) )
					GEN_FATAL( "Failed to append bucket to Global_AllocatorBuckets");

				last = & Global_AllocatorBuckets.back();
			}

			return alloc_align( * last, size, alignment );
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
				Arena bucket = Arena::init_from_allocator( heap(), Global_BucketSize );

				if ( bucket.PhysicalStart == nullptr )
					GEN_FATAL( "Failed to create bucket for Global_AllocatorBuckets");

				if ( ! Global_AllocatorBuckets.append( bucket ) )
					GEN_FATAL( "Failed to append bucket to Global_AllocatorBuckets");

				last = & Global_AllocatorBuckets.back();
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
	Code::Global          = make_code();
	Code::Global->Name    = get_cached_string( txt("Global Code") );
	Code::Global->Content = Code::Global->Name;

	Code::Invalid = make_code();
	Code::Invalid.set_global();

	t_empty          = (CodeType) make_code();
	t_empty->Type    = ECode::Typename;
	t_empty->Name    = get_cached_string( txt("") );
	t_empty.set_global();

	access_private       = make_code();
	access_private->Type = ECode::Access_Private;
	access_private->Name = get_cached_string( txt("private:") );
	access_private.set_global();

	access_protected       = make_code();
	access_protected->Type = ECode::Access_Protected;
	access_protected->Name = get_cached_string( txt("protected:") );
	access_protected.set_global();

	access_public       = make_code();
	access_public->Type = ECode::Access_Public;
	access_public->Name = get_cached_string( txt("public:") );
	access_public.set_global();

	attrib_api_export = def_attributes( code(GEN_API_Export_Code));
	attrib_api_export.set_global();

	attrib_api_import = def_attributes( code(GEN_API_Import_Code));
	attrib_api_import.set_global();

	module_global_fragment          = make_code();
	module_global_fragment->Type    = ECode::Untyped;
	module_global_fragment->Name    = get_cached_string( txt("module;") );
	module_global_fragment->Content = module_global_fragment->Name;
	module_global_fragment.set_global();

	module_private_fragment          = make_code();
	module_private_fragment->Type    = ECode::Untyped;
	module_private_fragment->Name    = get_cached_string( txt("module : private;") );
	module_private_fragment->Content = module_private_fragment->Name;
	module_private_fragment.set_global();

	fmt_newline = make_code();
	fmt_newline->Type = ECode::NewLine;
	fmt_newline.set_global();

	pragma_once          = (CodePragma) make_code();
	pragma_once->Type    = ECode::Preprocess_Pragma;
	pragma_once->Name    = get_cached_string( txt("once") );
	pragma_once->Content = pragma_once->Name;
	pragma_once.set_global();

	param_varadic            = (CodeType) make_code();
	param_varadic->Type      = ECode::Parameters;
	param_varadic->Name      = get_cached_string( txt("...") );
	param_varadic->ValueType = t_empty;
	param_varadic.set_global();

	preprocess_else = (CodePreprocessCond) make_code();
	preprocess_else->Type = ECode::Preprocess_Else;
	preprocess_else.set_global();

	preprocess_endif = (CodePreprocessCond) make_code();
	preprocess_endif->Type = ECode::Preprocess_EndIf;
	preprocess_endif.set_global();

#	define def_constant_code_type( Type_ )   \
		t_##Type_ = def_type( name(Type_) ); \
		t_##Type_.set_global();

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

	def_constant_code_type( sw );
	def_constant_code_type( uw );

	def_constant_code_type( f32 );
	def_constant_code_type( f64 );
#endif
#	undef def_constant_code_type

#	pragma push_macro( "forceinline" )
#	pragma push_macro( "global" )
#	pragma push_macro( "internal" )
#	pragma push_macro( "local_persist" )
#	pragma push_macro( "neverinline" )
#	undef forceinline
#	undef global
#	undef internal
#	undef local_persist
#	undef neverinline

#	define def_constant_spec( Type_, ... )                                  \
		spec_##Type_ = def_specifiers( num_args(__VA_ARGS__), __VA_ARGS__); \
		spec_##Type_.set_global();

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
	spec_local_persist.set_global();

#	pragma pop_macro( "forceinline" )
#	pragma pop_macro( "global" )
#	pragma pop_macro( "internal" )
#	pragma pop_macro( "local_persist" )
#	pragma pop_macro( "neverinline" )

#	undef def_constant_spec
}

void init()
{
	// Setup global allocator
	{
		GlobalAllocator = AllocatorInfo { & Global_Allocator_Proc, nullptr };

		Global_AllocatorBuckets = Array<Arena>::init_reserve( heap(), 128 );

		if ( Global_AllocatorBuckets == nullptr )
			GEN_FATAL( "Failed to reserve memory for Global_AllocatorBuckets");

		Arena bucket = Arena::init_from_allocator( heap(), Global_BucketSize );

		if ( bucket.PhysicalStart == nullptr )
			GEN_FATAL( "Failed to create first bucket for Global_AllocatorBuckets");

		Global_AllocatorBuckets.append( bucket );

	}

	// Setup the arrays
	{
		CodePools = Array<Pool>::init_reserve( Allocator_DataArrays, InitSize_DataArrays );

		if ( CodePools == nullptr )
			GEN_FATAL( "gen::init: Failed to initialize the CodePools array" );

		StringArenas = Array<Arena>::init_reserve( Allocator_DataArrays, InitSize_DataArrays );

		if ( StringArenas == nullptr )
			GEN_FATAL( "gen::init: Failed to initialize the StringArenas array" );
	}

	// Setup the code pool and code entries arena.
	{
		Pool code_pool = Pool::init( Allocator_CodePool, CodePool_NumBlocks, sizeof(AST) );

		if ( code_pool.PhysicalStart == nullptr )
			GEN_FATAL( "gen::init: Failed to initialize the code pool" );

		CodePools.append( code_pool );

		LexArena = Arena::init_from_allocator( Allocator_Lexer, LexAllocator_Size );

		Arena string_arena = Arena::init_from_allocator( Allocator_StringArena, SizePer_StringArena );

		if ( string_arena.PhysicalStart == nullptr )
			GEN_FATAL( "gen::init: Failed to initialize the string arena" );

		StringArenas.append( string_arena );
	}

	// Setup the hash tables
	{
		StringCache = StringTable::init( Allocator_StringTable );

		if ( StringCache.Entries == nullptr )
			GEN_FATAL( "gen::init: Failed to initialize the StringCache");
	}

	define_constants();
	init_parser();
}

void deinit()
{
	uw index = 0;
	uw left  = CodePools.num();
	do
	{
		Pool* code_pool = & CodePools[index];
		code_pool->free();
		index++;
	}
	while ( left--, left );

	index = 0;
	left  = StringArenas.num();
	do
	{
		Arena* string_arena = & StringArenas[index];
		string_arena->free();
		index++;
	}
	while ( left--, left );

	StringCache.destroy();

	CodePools.free();
	StringArenas.free();

	LexArena.free();

	index = 0;
	left  = Global_AllocatorBuckets.num();
	do
	{
		Arena* bucket = & Global_AllocatorBuckets[ index ];
		bucket->free();
		index++;
	}
	while ( left--, left );

	Global_AllocatorBuckets.free();
	deinit_parser();
}

void reset()
{
	s32 index = 0;
	s32 left  = CodePools.num();
	do
	{
		Pool* code_pool = & CodePools[index];
		code_pool->clear();
		index++;
	}
	while ( left--, left );

	index = 0;
	left  = StringArenas.num();
	do
	{
		Arena* string_arena = & StringArenas[index];
		string_arena->TotalUsed = 0;;
		index++;
	}
	while ( left--, left );

	StringCache.clear();

	define_constants();
}

AllocatorInfo get_string_allocator( s32 str_length )
{
	Arena* last = & StringArenas.back();

	uw size_req = str_length + sizeof(String::Header) + sizeof(char*);

	if ( last->TotalUsed + size_req > last->TotalSize )
	{
		Arena new_arena = Arena::init_from_allocator( Allocator_StringArena, SizePer_StringArena );

		if ( ! StringArenas.append( new_arena ) )
			GEN_FATAL( "gen::get_string_allocator: Failed to allocate a new string arena" );

		last = & StringArenas.back();
	}

	return * last;
}

// Will either make or retrive a code string.
StringCached get_cached_string( StrC str )
{
	s32 hash_length = str.Len > kilobytes(1) ? kilobytes(1) : str.Len;
	u64 key         = crc32( str.Ptr, hash_length );
	{
		StringCached* result = StringCache.get( key );

		if ( result )
			return * result;
	}

	String result = String::make( get_string_allocator( str.Len ), str );
	StringCache.set( key, result );

	return result;
}

// Used internally to retireve a Code object form the CodePool.
Code make_code()
{
	Pool* allocator = & CodePools.back();
	if ( allocator->FreeList == nullptr )
	{
		Pool code_pool = Pool::init( Allocator_CodePool, CodePool_NumBlocks, sizeof(AST) );

		if ( code_pool.PhysicalStart == nullptr )
			GEN_FATAL( "gen::make_code: Failed to allocate a new code pool - CodePool allcoator returned nullptr." );

		if ( ! CodePools.append( code_pool ) )
			GEN_FATAL( "gen::make_code: Failed to allocate a new code pool - CodePools failed to append new pool." );

		allocator = & CodePools.back();
	}

	Code result { rcast( AST*, alloc( * allocator, sizeof(AST) )) };
	// mem_set( result.ast, 0, sizeof(AST) );
	result->Type = ECode::Invalid;

	result->Content         = { nullptr };
	result->Prev            = { nullptr };
	result->Next			= { nullptr };
	result->Parent          = { nullptr };
	result->Name            = { nullptr };
	result->Type            = ECode::Invalid;
	result->ModuleFlags     = ModuleFlag::Invalid;
	result->NumEntries      = 0;
	result->Token		    = -1;

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
