#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "code_serialization.cpp"
#endif

GEN_NS_PARSER_BEGIN
internal void parser_init();
internal void parser_deinit();
GEN_NS_PARSER_END

internal
void* Global_Allocator_Proc( void* allocator_data, AllocType type, ssize size, ssize alignment, void* old_memory, ssize old_size, u64 flags )
{
	Arena* last = array_back(Global_AllocatorBuckets);

	switch ( type )
	{
		case EAllocation_ALLOC:
		{
			if ( ( last->TotalUsed + size ) > last->TotalSize )
			{
				Arena bucket = arena_init_from_allocator( heap(), Global_BucketSize );

				if ( bucket.PhysicalStart == nullptr )
					GEN_FATAL( "Failed to create bucket for Global_AllocatorBuckets");

				if ( ! array_append( Global_AllocatorBuckets, bucket ) )
					GEN_FATAL( "Failed to append bucket to Global_AllocatorBuckets");

				last = array_back(Global_AllocatorBuckets);
			}

			return alloc_align( arena_allocator_info(last), size, alignment );
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

				if ( ! array_append( Global_AllocatorBuckets, bucket ) )
					GEN_FATAL( "Failed to append bucket to Global_AllocatorBuckets");

				last = array_back(Global_AllocatorBuckets);
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
	Code_Global          = make_code();
	Code_Global->Name    = get_cached_string( txt("Global Code") );
	Code_Global->Content = Code_Global->Name;

	Code_Invalid = make_code();
	code_set_global(Code_Invalid);

	t_empty       = (CodeTypename) make_code();
	t_empty->Type = CT_Typename;
	t_empty->Name = get_cached_string( txt("") );
	code_set_global(cast(Code, t_empty));

	access_private       = make_code();
	access_private->Type = CT_Access_Private;
	access_private->Name = get_cached_string( txt("private:\n") );
	code_set_global(cast(Code, access_private));

	access_protected       = make_code();
	access_protected->Type = CT_Access_Protected;
	access_protected->Name = get_cached_string( txt("protected:\n") );
	code_set_global(access_protected);

	access_public       = make_code();
	access_public->Type = CT_Access_Public;
	access_public->Name = get_cached_string( txt("public:\n") );
	code_set_global(access_public);

	StrC api_export_str = code(GEN_API_Export_Code);
	attrib_api_export = def_attributes( api_export_str );
	code_set_global(cast(Code, attrib_api_export));

	StrC api_import_str = code(GEN_API_Import_Code);
	attrib_api_import = def_attributes( api_import_str );
	code_set_global(cast(Code, attrib_api_import));

	module_global_fragment          = make_code();
	module_global_fragment->Type    = CT_Untyped;
	module_global_fragment->Name    = get_cached_string( txt("module;") );
	module_global_fragment->Content = module_global_fragment->Name;
	code_set_global(cast(Code, module_global_fragment));

	module_private_fragment          = make_code();
	module_private_fragment->Type    = CT_Untyped;
	module_private_fragment->Name    = get_cached_string( txt("module : private;") );
	module_private_fragment->Content = module_private_fragment->Name;
	code_set_global(cast(Code, module_private_fragment));

	fmt_newline = make_code();
	fmt_newline->Type = CT_NewLine;
	code_set_global((Code)fmt_newline);

	pragma_once          = (CodePragma) make_code();
	pragma_once->Type    = CT_Preprocess_Pragma;
	pragma_once->Name    = get_cached_string( txt("once") );
	pragma_once->Content = pragma_once->Name;
	code_set_global((Code)pragma_once);

	param_varadic            = (CodeParam) make_code();
	param_varadic->Type      = CT_Parameters;
	param_varadic->Name      = get_cached_string( txt("...") );
	param_varadic->ValueType = t_empty;
	code_set_global((Code)param_varadic);

	preprocess_else = (CodePreprocessCond) make_code();
	preprocess_else->Type = CT_Preprocess_Else;
	code_set_global((Code)preprocess_else);

	preprocess_endif = (CodePreprocessCond) make_code();
	preprocess_endif->Type = CT_Preprocess_EndIf;
	code_set_global((Code)preprocess_endif);

#	define def_constant_code_type( Type_ )           \
		do                                           \
		{                                            \
			StrC name_str = name(Type_);             \
			t_##Type_ = def_type( name_str );        \
			code_set_global( cast(Code, t_##Type_)); \
		} while(0)

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


	spec_const            = def_specifier( Spec_Const);            code_set_global( cast(Code, spec_const ));
	spec_consteval        = def_specifier( Spec_Consteval);        code_set_global( cast(Code, spec_consteval ));;
	spec_constexpr        = def_specifier( Spec_Constexpr);        code_set_global( cast(Code, spec_constexpr ));;
	spec_constinit        = def_specifier( Spec_Constinit);        code_set_global( cast(Code, spec_constinit ));;
	spec_extern_linkage   = def_specifier( Spec_External_Linkage); code_set_global( cast(Code, spec_extern_linkage ));;
	spec_final            = def_specifier( Spec_Final);            code_set_global( cast(Code, spec_final ));;
	spec_forceinline      = def_specifier( Spec_ForceInline);      code_set_global( cast(Code, spec_forceinline ));;
	spec_global           = def_specifier( Spec_Global);           code_set_global( cast(Code, spec_global ));;
	spec_inline           = def_specifier( Spec_Inline);           code_set_global( cast(Code, spec_inline ));;
	spec_internal_linkage = def_specifier( Spec_Internal_Linkage); code_set_global( cast(Code, spec_internal_linkage ));;
	spec_local_persist    = def_specifier( Spec_Local_Persist);    code_set_global( cast(Code, spec_local_persist ));;
	spec_mutable          = def_specifier( Spec_Mutable);          code_set_global( cast(Code, spec_mutable ));;
	spec_neverinline      = def_specifier( Spec_NeverInline);      code_set_global( cast(Code, spec_neverinline ));;
	spec_noexcept         = def_specifier( Spec_NoExceptions);     code_set_global( cast(Code, spec_noexcept ));;
	spec_override         = def_specifier( Spec_Override);         code_set_global( cast(Code, spec_override ));;
	spec_ptr              = def_specifier( Spec_Ptr);              code_set_global( cast(Code, spec_ptr ));;
	spec_pure             = def_specifier( Spec_Pure);             code_set_global( cast(Code, spec_pure ));
	spec_ref              = def_specifier( Spec_Ref);              code_set_global( cast(Code, spec_ref ));;
	spec_register         = def_specifier( Spec_Register);         code_set_global( cast(Code, spec_register ));;
	spec_rvalue           = def_specifier( Spec_RValue);           code_set_global( cast(Code, spec_rvalue ));;
	spec_static_member    = def_specifier( Spec_Static);           code_set_global( cast(Code, spec_static_member ));;
	spec_thread_local     = def_specifier( Spec_Thread_Local);     code_set_global( cast(Code, spec_thread_local ));;
	spec_virtual          = def_specifier( Spec_Virtual);          code_set_global( cast(Code, spec_virtual ));;
	spec_volatile         = def_specifier( Spec_Volatile);         code_set_global( cast(Code, spec_volatile ));

	spec_local_persist = def_specifiers( 1, Spec_Local_Persist );
	code_set_global(cast(Code, spec_local_persist));

#	pragma push_macro("enum_underlying")
	array_append(PreprocessorDefines, txt("enum_underlying("));
#	pragma pop_macro("enum_underlying")

#	undef def_constant_spec
}

void init()
{
	// Setup global allocator
	{
		AllocatorInfo becasue_C = { & Global_Allocator_Proc, nullptr };
		GlobalAllocator = becasue_C;

		Global_AllocatorBuckets = array_init_reserve(Arena, heap(), 128 );

		if ( Global_AllocatorBuckets == nullptr )
			GEN_FATAL( "Failed to reserve memory for Global_AllocatorBuckets");

		Arena bucket = arena_init_from_allocator( heap(), Global_BucketSize );

		if ( bucket.PhysicalStart == nullptr )
			GEN_FATAL( "Failed to create first bucket for Global_AllocatorBuckets");

		array_append( Global_AllocatorBuckets, bucket );
	}

	if (Allocator_DataArrays.Proc == nullptr) {
		Allocator_DataArrays = heap();
	}
	if (Allocator_CodePool.Proc == nullptr ) {
		Allocator_CodePool = heap();
	}
	if (Allocator_Lexer.Proc == nullptr) {
		Allocator_Lexer = heap();
	}
	if (Allocator_StringArena.Proc == nullptr) {
		Allocator_StringArena = heap();
	}
	if (Allocator_StringTable.Proc == nullptr) {
		Allocator_StringTable = heap();
	}
	if (Allocator_TypeTable.Proc == nullptr) {
		Allocator_TypeTable = heap();
	}

	// Setup the arrays
	{
		CodePools = array_init_reserve(Pool, Allocator_DataArrays, InitSize_DataArrays );

		if ( CodePools == nullptr )
			GEN_FATAL( "gen::init: Failed to initialize the CodePools array" );

		StringArenas = array_init_reserve(Arena, Allocator_DataArrays, InitSize_DataArrays );

		if ( StringArenas == nullptr )
			GEN_FATAL( "gen::init: Failed to initialize the StringArenas array" );
	}

	// Setup the code pool and code entries arena.
	{
		Pool code_pool = pool_init( Allocator_CodePool, CodePool_NumBlocks, sizeof(AST) );

		if ( code_pool.PhysicalStart == nullptr )
			GEN_FATAL( "gen::init: Failed to initialize the code pool" );

		array_append( CodePools, code_pool );

		LexArena = arena_init_from_allocator( Allocator_Lexer, LexAllocator_Size );

		Arena string_arena = arena_init_from_allocator( Allocator_StringArena, SizePer_StringArena );

		if ( string_arena.PhysicalStart == nullptr )
			GEN_FATAL( "gen::init: Failed to initialize the string arena" );

		array_append( StringArenas, string_arena );
	}

	// Setup the hash tables
	{
		StringCache = hashtable_init(StringCached, Allocator_StringTable);

		if ( StringCache.Entries == nullptr )
			GEN_FATAL( "gen::init: Failed to initialize the StringCache");
	}

	// Preprocessor Defines
	PreprocessorDefines = array_init_reserve(StringCached, GlobalAllocator, kilobytes(1) );

	define_constants();
	GEN_NS_PARSER parser_init();
}

void deinit()
{
	usize index = 0;
	usize left  = array_num(CodePools);
	do
	{
		Pool* code_pool = & CodePools[index];
		pool_free(code_pool);
		index++;
	}
	while ( left--, left );

	index = 0;
	left  = array_num(StringArenas);
	do
	{
		Arena* string_arena = & StringArenas[index];
		arena_free(string_arena);
		index++;
	}
	while ( left--, left );

	hashtable_destroy(StringCache);

	array_free( CodePools);
	array_free( StringArenas);

	arena_free(& LexArena);

	array_free(PreprocessorDefines);

	index = 0;
	left  = array_num(Global_AllocatorBuckets);
	do
	{
		Arena* bucket = & Global_AllocatorBuckets[ index ];
		arena_free(bucket);
		index++;
	}
	while ( left--, left );

	array_free(Global_AllocatorBuckets);
	GEN_NS_PARSER parser_deinit();
}

void reset()
{
	s32 index = 0;
	s32 left  = array_num(CodePools);
	do
	{
		Pool* code_pool = & CodePools[index];
		pool_clear(code_pool);
		index++;
	}
	while ( left--, left );

	index = 0;
	left  = array_num(StringArenas);
	do
	{
		Arena* string_arena = & StringArenas[index];
		string_arena->TotalUsed = 0;;
		index++;
	}
	while ( left--, left );

	hashtable_clear(StringCache);

	define_constants();
}

AllocatorInfo get_string_allocator( s32 str_length )
{
	Arena* last = array_back(StringArenas);

	usize size_req = str_length + sizeof(StringHeader) + sizeof(char*);

	if ( last->TotalUsed + scast(ssize, size_req) > last->TotalSize )
	{
		Arena new_arena = arena_init_from_allocator( Allocator_StringArena, SizePer_StringArena );

		if ( ! array_append( StringArenas, new_arena ) )
			GEN_FATAL( "gen::get_string_allocator: Failed to allocate a new string arena" );

		last = array_back(StringArenas);
	}

	return arena_allocator_info(last);
}

// Will either make or retrive a code string.
StringCached get_cached_string( StrC str )
{
	s32 hash_length = str.Len > kilobytes(1) ? kilobytes(1) : str.Len;
	u64 key         = crc32( str.Ptr, hash_length );
	{
		StringCached* result = hashtable_get(StringCache, key );

		if ( result )
			return * result;
	}

	StrC result = string_to_strc( string_make_strc( get_string_allocator( str.Len ), str ));
	hashtable_set(StringCache, key, result );

	return result;
}

// Used internally to retireve a Code object form the CodePool.
Code make_code()
{
	Pool* allocator = array_back( CodePools);
	if ( allocator->FreeList == nullptr )
	{
		Pool code_pool = pool_init( Allocator_CodePool, CodePool_NumBlocks, sizeof(AST) );

		if ( code_pool.PhysicalStart == nullptr )
			GEN_FATAL( "gen::make_code: Failed to allocate a new code pool - CodePool allcoator returned nullptr." );

		if ( ! array_append( CodePools, code_pool ) )
			GEN_FATAL( "gen::make_code: Failed to allocate a new code pool - CodePools failed to append new pool." );

		allocator = array_back( CodePools);
	}

	Code result = { rcast( AST*, alloc( pool_allocator_info(allocator), sizeof(AST) )) };
	mem_set( rcast(void*, cast(AST*, result)), 0, sizeof(AST) );
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
