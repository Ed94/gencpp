#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "code_serialization.cpp"
#endif

GEN_NS_PARSER_BEGIN
internal void parser_init();
internal void parser_deinit();
GEN_NS_PARSER_END

internal
void* fallback_allocator_proc( void* allocator_data, AllocType type, ssize size, ssize alignment, void* old_memory, ssize old_size, u64 flags )
{
	GEN_ASSERT(_ctx);
	GEN_ASSERT(_ctx->Fallback_AllocatorBuckets);
	Arena* last = array_back(_ctx->Fallback_AllocatorBuckets);

	switch ( type )
	{
		case EAllocation_ALLOC:
		{
			if ( ( last->TotalUsed + size ) > last->TotalSize )
			{
				Arena bucket = arena_init_from_allocator( heap(), _ctx->InitSize_Fallback_Allocator_Bucket_Size );

				if ( bucket.PhysicalStart == nullptr )
					GEN_FATAL( "Failed to create bucket for Fallback_AllocatorBuckets");

				if ( ! array_append( _ctx->Fallback_AllocatorBuckets, bucket ) )
					GEN_FATAL( "Failed to append bucket to Fallback_AllocatorBuckets");

				last = array_back(_ctx->Fallback_AllocatorBuckets);
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
				Arena bucket = arena_init_from_allocator( heap(), _ctx->InitSize_Fallback_Allocator_Bucket_Size );

				if ( bucket.PhysicalStart == nullptr )
					GEN_FATAL( "Failed to create bucket for Fallback_AllocatorBuckets");

				if ( ! array_append( _ctx->Fallback_AllocatorBuckets, bucket ) )
					GEN_FATAL( "Failed to append bucket to Fallback_AllocatorBuckets");

				last = array_back( _ctx->Fallback_AllocatorBuckets);
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
	Code_Global->Name    = cache_str( txt("Global Code") );
	Code_Global->Content = Code_Global->Name;

	Code_Invalid = make_code();
	code_set_global(Code_Invalid);

	t_empty       = (CodeTypename) make_code();
	t_empty->Type = CT_Typename;
	t_empty->Name = cache_str( txt("") );
	code_set_global(cast(Code, t_empty));

	access_private       = make_code();
	access_private->Type = CT_Access_Private;
	access_private->Name = cache_str( txt("private:\n") );
	code_set_global(cast(Code, access_private));

	access_protected       = make_code();
	access_protected->Type = CT_Access_Protected;
	access_protected->Name = cache_str( txt("protected:\n") );
	code_set_global(access_protected);

	access_public       = make_code();
	access_public->Type = CT_Access_Public;
	access_public->Name = cache_str( txt("public:\n") );
	code_set_global(access_public);

	Str api_export_str = code(GEN_API_Export_Code);
	attrib_api_export = def_attributes( api_export_str );
	code_set_global(cast(Code, attrib_api_export));

	Str api_import_str = code(GEN_API_Import_Code);
	attrib_api_import = def_attributes( api_import_str );
	code_set_global(cast(Code, attrib_api_import));

	module_global_fragment          = make_code();
	module_global_fragment->Type    = CT_Untyped;
	module_global_fragment->Name    = cache_str( txt("module;") );
	module_global_fragment->Content = module_global_fragment->Name;
	code_set_global(cast(Code, module_global_fragment));

	module_private_fragment          = make_code();
	module_private_fragment->Type    = CT_Untyped;
	module_private_fragment->Name    = cache_str( txt("module : private;") );
	module_private_fragment->Content = module_private_fragment->Name;
	code_set_global(cast(Code, module_private_fragment));

	fmt_newline = make_code();
	fmt_newline->Type = CT_NewLine;
	code_set_global((Code)fmt_newline);

	pragma_once          = (CodePragma) make_code();
	pragma_once->Type    = CT_Preprocess_Pragma;
	pragma_once->Name    = cache_str( txt("once") );
	pragma_once->Content = pragma_once->Name;
	code_set_global((Code)pragma_once);

	param_varadic            = (CodeParams) make_code();
	param_varadic->Type      = CT_Parameters;
	param_varadic->Name      = cache_str( txt("...") );
	param_varadic->ValueType = t_empty;
	code_set_global((Code)param_varadic);

	preprocess_else = (CodePreprocessCond) make_code();
	preprocess_else->Type = CT_Preprocess_Else;
	code_set_global((Code)preprocess_else);

	preprocess_endif = (CodePreprocessCond) make_code();
	preprocess_endif->Type = CT_Preprocess_EndIf;
	code_set_global((Code)preprocess_endif);

	Str auto_str     = txt("auto");     t_auto     = def_type( auto_str );     code_set_global( t_auto );
	Str void_str     = txt("void");     t_void     = def_type( void_str );     code_set_global( t_void );
	Str int_str      = txt("int");      t_int      = def_type( int_str );      code_set_global( t_int  );
	Str bool_str     = txt("bool");     t_bool     = def_type( bool_str );     code_set_global( t_bool );
	Str char_str     = txt("char");     t_char     = def_type( char_str );     code_set_global( t_char );
	Str wchar_str    = txt("wchar_t");  t_wchar_t  = def_type( wchar_str );    code_set_global( t_wchar_t );
	Str class_str    = txt("class");    t_class    = def_type( class_str );    code_set_global( t_class );
	Str typename_str = txt("typename"); t_typename = def_type( typename_str ); code_set_global( t_typename );

#ifdef GEN_DEFINE_LIBRARY_CODE_CONSTANTS
	t_b32 = def_type( name(b32) ); code_set_global( t_b32 );

	Str s8_str  = txt("s8");  t_s8  = def_type( s8_str ); code_set_global( t_s8 );
	Str s16_str = txt("s16"); t_s16 = def_type( s16_str ); code_set_global( t_s16 );
	Str s32_str = txt("s32"); t_s32 = def_type( s32_str ); code_set_global( t_s32 );
	Str s64_str = txt("s64"); t_s64 = def_type( s64_str ); code_set_global( t_s64 );

	Str u8_str  = txt("u8");  t_u8  = def_type( u8_str );  code_set_global( t_u8 );
	Str u16_str = txt("u16"); t_u16 = def_type( u16_str ); code_set_global( t_u16 );
	Str u32_str = txt("u32"); t_u32 = def_type( u32_str ); code_set_global( t_u32 );
	Str u64_str = txt("u64"); t_u64 = def_type( u64_str ); code_set_global( t_u64 );

	Str ssize_str = txt("ssize"); t_ssize = def_type( ssize_str ); code_set_global( t_ssize );
	Str usize_str = txt("usize"); t_usize = def_type( usize_str ); code_set_global( t_usize );

	Str f32_str = txt("f32"); t_f32 = def_type( f32_str ); code_set_global( t_f32 );
	Str f64_str = txt("f64"); t_f64 = def_type( f64_str ); code_set_global( t_f64 );
#endif

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

	if (enum_underlying_sig.Len == 0) {
		enum_underlying_sig = txt("enum_underlying(");
	}
	array_append( _ctx->PreprocessorDefines, enum_underlying_sig);

#	undef def_constant_spec
}

void init(Context* ctx)
{
	AllocatorInfo fallback_allocator = { & fallback_allocator_proc, nullptr };
	
	b32 using_fallback_allocator = false;
	if (ctx->Allocator_DyanmicContainers.Proc == nullptr) {
		ctx->Allocator_DyanmicContainers = fallback_allocator;
		using_fallback_allocator = true;
	}
	if (ctx->Allocator_Pool.Proc == nullptr ) {
		ctx->Allocator_Pool = fallback_allocator;
		using_fallback_allocator = true;
	}
	if (ctx->Allocator_StrCache.Proc == nullptr) {
		ctx->Allocator_StrCache = fallback_allocator;
		using_fallback_allocator = true;
	}
	if (ctx->Allocator_Temp.Proc == nullptr) {
		ctx->Allocator_Temp = fallback_allocator;
		using_fallback_allocator = true;
	}
	// Setup fallback allocator
	if (using_fallback_allocator)
	{
		ctx->Fallback_AllocatorBuckets = array_init_reserve(Arena, heap(), 128 );
		if ( ctx->Fallback_AllocatorBuckets == nullptr )
			GEN_FATAL( "Failed to reserve memory for Fallback_AllocatorBuckets");

		Arena bucket = arena_init_from_allocator( heap(), ctx->InitSize_Fallback_Allocator_Bucket_Size );
		if ( bucket.PhysicalStart == nullptr )
			GEN_FATAL( "Failed to create first bucket for Fallback_AllocatorBuckets");

		array_append( ctx->Fallback_AllocatorBuckets, bucket );
	}

	if (ctx->Max_CommentLineLength == 0) {
		ctx->Max_CommentLineLength = 1024;
	}
	if (ctx->Max_StrCacheLength == 0) {
		ctx->Max_StrCacheLength = kilobytes(512);
	}

	if (ctx->InitSize_BuilderBuffer == 0) {
		ctx->InitSize_BuilderBuffer = megabytes(2);
	}
	if (ctx->InitSize_CodePoolsArray == 0) { 
		ctx->InitSize_CodePoolsArray = 16; 
	}
	if (ctx->InitSize_StringArenasArray == 0) {
		ctx->InitSize_StringArenasArray = 16; 
	}
	if (ctx->CodePool_NumBlocks == 0) {
		ctx->CodePool_NumBlocks = kilobytes(16);
	}

	if (ctx->InitSize_LexArena == 0 ) {
		ctx->InitSize_LexArena = megabytes(4);
	}
	if (ctx->SizePer_StringArena == 0) {
		ctx->SizePer_StringArena = megabytes(1);
	}

	if (ctx->InitSize_Fallback_Allocator_Bucket_Size == 0) { 
		ctx->InitSize_Fallback_Allocator_Bucket_Size = megabytes(8);
	}

	// Override the current context (user has to put it back if unwanted).
	_ctx = ctx;

	// Setup the arrays
	{
		ctx->CodePools = array_init_reserve(Pool, ctx->Allocator_DyanmicContainers, ctx->InitSize_CodePoolsArray );
		if ( ctx->CodePools == nullptr )
			GEN_FATAL( "gen::init: Failed to initialize the CodePools array" );

		ctx->StringArenas = array_init_reserve(Arena, ctx->Allocator_DyanmicContainers, ctx->InitSize_StringArenasArray );
		if ( ctx->StringArenas == nullptr )
			GEN_FATAL( "gen::init: Failed to initialize the StringArenas array" );
	}
	// Setup the code pool and code entries arena.
	{
		Pool code_pool = pool_init( ctx->Allocator_Pool, ctx->CodePool_NumBlocks, sizeof(AST) );
		if ( code_pool.PhysicalStart == nullptr )
			GEN_FATAL( "gen::init: Failed to initialize the code pool" );
		array_append( ctx->CodePools, code_pool );

		// TODO(Ed): This is going to be phased out most likely.
		ctx->LexArena = arena_init_from_allocator( ctx->Allocator_DyanmicContainers, ctx->InitSize_LexArena );

		// TODO(Ed): Eventually the string arenas needs to be phased out for a dedicated string slab allocator
		Arena strbuilder_arena = arena_init_from_allocator( ctx->Allocator_StrCache, ctx->SizePer_StringArena );
		if ( strbuilder_arena.PhysicalStart == nullptr )
			GEN_FATAL( "gen::init: Failed to initialize the string arena" );
		array_append( ctx->StringArenas, strbuilder_arena );
	}

	// Setup the hash tables
	{
		ctx->StrCache = hashtable_init(StrCached, ctx->Allocator_DyanmicContainers);
		if ( ctx->StrCache.Entries == nullptr )
			GEN_FATAL( "gen::init: Failed to initialize the StringCache");
	}

	// Preprocessor Defines
	ctx->PreprocessorDefines = array_init_reserve(StrCached, ctx->Allocator_DyanmicContainers, kilobytes(1) );

	define_constants();
	GEN_NS_PARSER parser_init();
}

void deinit(Context* ctx)
{
	usize index = 0;
	usize left  = array_num(ctx->CodePools);
	do
	{
		Pool* code_pool = & ctx->CodePools[index];
		pool_free(code_pool);
		index++;
	}
	while ( left--, left );

	index = 0;
	left  = array_num(ctx->StringArenas);
	do
	{
		Arena* strbuilder_arena = & ctx->StringArenas[index];
		arena_free(strbuilder_arena);
		index++;
	}
	while ( left--, left );

	hashtable_destroy(ctx->StrCache);

	array_free( ctx->CodePools);
	array_free( ctx->StringArenas);

	arena_free(& ctx->LexArena);

	array_free(ctx->PreprocessorDefines);

	index = 0;
	left  = array_num( ctx->Fallback_AllocatorBuckets);
	do
	{
		Arena* bucket = & ctx->Fallback_AllocatorBuckets[ index ];
		arena_free(bucket);
		index++;
	}
	while ( left--, left );

	array_free( ctx->Fallback_AllocatorBuckets);
	GEN_NS_PARSER parser_deinit();

	if (_ctx == ctx) 
		_ctx = nullptr;
}

void reset(Context* ctx)
{
	s32 index = 0;
	s32 left  = array_num(ctx->CodePools);
	do
	{
		Pool* code_pool = & ctx->CodePools[index];
		pool_clear(code_pool);
		index++;
	}
	while ( left--, left );

	index = 0;
	left  = array_num(ctx->StringArenas);
	do
	{
		Arena* strbuilder_arena = & ctx->StringArenas[index];
		strbuilder_arena->TotalUsed = 0;;
		index++;
	}
	while ( left--, left );

	hashtable_clear(ctx->StrCache);
	define_constants();
}

void set_context(Context* new_ctx) {
	GEN_ASSERT(new_ctx);
	_ctx = new_ctx;
}

AllocatorInfo get_cached_str_allocator( s32 str_length )
{
	Arena* last     = array_back(_ctx->StringArenas);
	usize  size_req = str_length + sizeof(StrBuilderHeader) + sizeof(char*);
	if ( last->TotalUsed + scast(ssize, size_req) > last->TotalSize )
	{
		Arena new_arena = arena_init_from_allocator( _ctx->Allocator_StrCache, _ctx->SizePer_StringArena );
		if ( ! array_append( _ctx->StringArenas, new_arena ) )
			GEN_FATAL( "gen::get_cached_str_allocator: Failed to allocate a new string arena" );

		last = array_back( _ctx->StringArenas);
	}

	return arena_allocator_info(last);
}

// Will either make or retrive a code string.
StrCached cache_str( Str str )
{
	if (str.Len > _ctx->Max_StrCacheLength) {
		// Do not cache the string, just shove into the arena and and return it.
		Str result = strbuilder_to_str( strbuilder_make_str( get_cached_str_allocator( str.Len ), str ));
		return result;
	}
	u64 key = crc32( str.Ptr, str.Len ); {
		StrCached* result = hashtable_get( _ctx->StrCache, key );
		if ( result )
			return * result;
	}
	Str result = strbuilder_to_str( strbuilder_make_str( get_cached_str_allocator( str.Len ), str ));
	hashtable_set( _ctx->StrCache, key, result );
	return result;
}

// Used internally to retireve a Code object form the CodePool.
Code make_code()
{
	Pool* allocator = array_back( _ctx->CodePools);
	if ( allocator->FreeList == nullptr )
	{
		Pool code_pool = pool_init( _ctx->Allocator_Pool, _ctx->CodePool_NumBlocks, sizeof(AST) );

		if ( code_pool.PhysicalStart == nullptr )
			GEN_FATAL( "gen::make_code: Failed to allocate a new code pool - CodePool allcoator returned nullptr." );

		if ( ! array_append( _ctx->CodePools, code_pool ) )
			GEN_FATAL( "gen::make_code: Failed to allocate a new code pool - CodePools failed to append new pool." );

		allocator = array_back( _ctx->CodePools);
	}

	Code result = { rcast( AST*, alloc( pool_allocator_info(allocator), sizeof(AST) )) };
	mem_set( rcast(void*, cast(AST*, result)), 0, sizeof(AST) );
	return result;
}

void set_preprocess_define( Str id, b32 is_functional ) {
	StrBuilder builder = strbuilder_make_str( _ctx->Allocator_Temp, id );
	if (is_functional) {
		strbuilder_append_char( & builder, '(' );
	}
	array_append( _ctx->PreprocessorDefines, cache_str(builder) ); 
}
