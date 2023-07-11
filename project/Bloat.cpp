#define BLOAT_IMPL
#include "Bloat.hpp"


namespace gen
{
#pragma region Memory

	struct _heap_stats
	{
		u32 magic;
		sw  used_memory;
		sw  alloc_count;
	};

	global _heap_stats _heap_stats_info;

	void heap_stats_init( void )
	{
		zero_item( &_heap_stats_info );
		_heap_stats_info.magic = ZPL_HEAP_STATS_MAGIC;
	}

	sw heap_stats_used_memory( void )
	{
		ZPL_ASSERT_MSG( _heap_stats_info.magic == ZPL_HEAP_STATS_MAGIC, "heap_stats is not initialised yet, call heap_stats_init first!" );
		return _heap_stats_info.used_memory;
	}

	sw heap_stats_alloc_count( void )
	{
		ZPL_ASSERT_MSG( _heap_stats_info.magic == ZPL_HEAP_STATS_MAGIC, "heap_stats is not initialised yet, call heap_stats_init first!" );
		return _heap_stats_info.alloc_count;
	}

	void heap_stats_check( void )
	{
		ZPL_ASSERT_MSG( _heap_stats_info.magic == ZPL_HEAP_STATS_MAGIC, "heap_stats is not initialised yet, call heap_stats_init first!" );
		ZPL_ASSERT( _heap_stats_info.used_memory == 0 );
		ZPL_ASSERT( _heap_stats_info.alloc_count == 0 );
	}

	struct _heap_alloc_info
	{
		sw    size;
		void* physical_start;
	};

	void* heap_allocator_proc( void* allocator_data, AllocType type, sw size, sw alignment, void* old_memory, sw old_size, u64 flags )
	{
		return zpl::heap_allocator_proc( allocator_data, (zpl::AllocType)type, size, alignment, old_memory, old_size, flags );

	// 	void* ptr = NULL;
	// 	// unused( allocator_data );
	// 	// unused( old_size );
	// 	if ( ! alignment )
	// 		alignment = ZPL_DEFAULT_MEMORY_ALIGNMENT;

	// #ifdef ZPL_HEAP_ANALYSIS
	// 	sw alloc_info_size      = size_of( _heap_alloc_info );
	// 	sw alloc_info_remainder = ( alloc_info_size % alignment );
	// 	sw track_size           = max( alloc_info_size, alignment ) + alloc_info_remainder;
	// 	switch ( type )
	// 	{
	// 		case EAllocation_FREE :
	// 			{
	// 				if ( ! old_memory )
	// 					break;
	// 				_heap_alloc_info* alloc_info  = zpl_cast( _heap_alloc_info* ) old_memory - 1;
	// 				_heap_stats_info.used_memory -= alloc_info->size;
	// 				_heap_stats_info.alloc_count--;
	// 				old_memory = alloc_info->physical_start;
	// 			}
	// 			break;
	// 		case EAllocation_ALLOC :
	// 			{
	// 				size += track_size;
	// 			}
	// 			break;
	// 		default :
	// 			break;
	// 	}
	// #endif

	// 	switch ( type )
	// 	{
	// #if defined( ZPL_COMPILER_MSVC ) || ( defined( ZPL_COMPILER_GCC ) && defined( ZPL_SYSTEM_WINDOWS ) ) || ( defined( ZPL_COMPILER_TINYC ) && defined( ZPL_SYSTEM_WINDOWS ) )
	// 		case EAllocation_ALLOC :
	// 			ptr = _aligned_malloc( size, alignment );
	// 			if ( flags & ALLOCATOR_FLAG_CLEAR_TO_ZERO )
	// 				zero_size( ptr, size );
	// 			break;
	// 		case EAllocation_FREE :
	// 			_aligned_free( old_memory );
	// 			break;
	// 		case EAllocation_RESIZE :
	// 			{
	// 				AllocatorInfo a = heap();
	// 				ptr             = default_resize_align( a, old_memory, old_size, size, alignment );
	// 			}
	// 			break;

	// #elif defined( ZPL_SYSTEM_LINUX ) && ! defined( ZPL_CPU_ARM ) && ! defined( ZPL_COMPILER_TINYC )
	// 		case EAllocation_ALLOC :
	// 			{
	// 				ptr = aligned_alloc( alignment, ( size + alignment - 1 ) & ~( alignment - 1 ) );

	// 				if ( flags & ZPL_ALLOCATOR_FLAG_CLEAR_TO_ZERO )
	// 				{
	// 					zero_size( ptr, size );
	// 				}
	// 			}
	// 			break;

	// 		case EAllocation_FREE :
	// 			{
	// 				free( old_memory );
	// 			}
	// 			break;

	// 		case EAllocation_RESIZE :
	// 			{
	// 				AllocatorInfo a = heap();
	// 				ptr             = default_resize_align( a, old_memory, old_size, size, alignment );
	// 			}
	// 			break;
	// #else
	// 		case EAllocation_ALLOC :
	// 			{
	// 				posix_memalign( &ptr, alignment, size );

	// 				if ( flags & ZPL_ALLOCATOR_FLAG_CLEAR_TO_ZERO )
	// 				{
	// 					zero_size( ptr, size );
	// 				}
	// 			}
	// 			break;

	// 		case EAllocation_FREE :
	// 			{
	// 				free( old_memory );
	// 			}
	// 			break;

	// 		case EAllocation_RESIZE :
	// 			{
	// 				AllocatorInfo a = heap();
	// 				ptr             = default_resize_align( a, old_memory, old_size, size, alignment );
	// 			}
	// 			break;
	// #endif

	// 		case EAllocation_FREE_ALL :
	// 			break;
	// 	}

	// #ifdef ZPL_HEAP_ANALYSIS
	// 	if ( type == EAllocation_ALLOC )
	// 	{
	// 		_heap_alloc_info* alloc_info = zpl_cast( _heap_alloc_info* )( zpl_cast( char* ) ptr + alloc_info_remainder );
	// 		zero_item( alloc_info );
	// 		alloc_info->size              = size - track_size;
	// 		alloc_info->physical_start    = ptr;
	// 		ptr                           = zpl_cast( void* )( alloc_info + 1 );
	// 		_heap_stats_info.used_memory += alloc_info->size;
	// 		_heap_stats_info.alloc_count++;
	// 	}
	// #endif

	// 	return ptr;
	}

	void* Arena::allocator_proc( void* allocator_data, AllocType type, sw size, sw alignment, void* old_memory, sw old_size, u64 flags )
	{
		Arena* arena = rcast(Arena*, allocator_data);
		void*      ptr   = NULL;

		// unused( old_size );

		switch ( type )
		{
			case EAllocation_ALLOC :
				{
					void* end        = pointer_add( arena->PhysicalStart, arena->TotalUsed );
					sw    total_size = align_forward_i64( size, alignment );

					// NOTE: Out of memory
					if ( arena->TotalUsed + total_size > (sw) arena->TotalSize )
					{
						// zpl__printf_err("%s", "Arena out of memory\n");
						return nullptr;
					}

					ptr              = align_forward( end, alignment );
					arena->TotalUsed += total_size;

					if ( flags & ALLOCATOR_FLAG_CLEAR_TO_ZERO )
						zero_size( ptr, size );
				}
				break;

			case EAllocation_FREE :
				// NOTE: Free all at once
				// Use Temp_Arena_Memory if you want to free a block
				break;

			case EAllocation_FREE_ALL :
				arena->TotalUsed = 0;
				break;

			case EAllocation_RESIZE :
				{
					// TODO : Check if ptr is on top of stack and just extend
					AllocatorInfo a = arena->Backing;
					ptr             = default_resize_align( a, old_memory, old_size, size, alignment );
				}
				break;
		}
		return ptr;
	}

	void* Pool::allocator_proc( void* allocator_data, AllocType type, sw size, sw alignment, void* old_memory, sw old_size, u64 flags )
	{
		Pool* pool = zpl_cast( Pool* ) allocator_data;
		void* ptr  = NULL;

		// unused( old_size );

		switch ( type )
		{
			case EAllocation_ALLOC :
				{
					uptr next_free;

					ZPL_ASSERT( size == pool->BlockSize );
					ZPL_ASSERT( alignment == pool->BlockAlign );
					ZPL_ASSERT( pool->FreeList != NULL );

					next_free        = *zpl_cast( uptr* ) pool->FreeList;
					ptr              = pool->FreeList;
					pool->FreeList   = zpl_cast( void* ) next_free;
					pool->TotalSize += pool->BlockSize;

					if ( flags & ALLOCATOR_FLAG_CLEAR_TO_ZERO )
						zero_size( ptr, size );
				}
				break;

			case EAllocation_FREE :
				{
					uptr* next;
					if ( old_memory == NULL )
						return NULL;

					next             = zpl_cast( uptr* ) old_memory;
					*next            = zpl_cast( uptr ) pool->FreeList;
					pool->FreeList   = old_memory;
					pool->TotalSize -= pool->BlockSize;
				}
				break;

			case EAllocation_FREE_ALL :
				{
					sw    actual_block_size, block_index;
					void* curr;
					uptr* end;

					actual_block_size = pool->BlockSize + pool->BlockAlign;
					pool->TotalSize   = 0;

					// NOTE: Init intrusive freelist
					curr = pool->PhysicalStart;
					for ( block_index = 0; block_index < pool->NumBlocks - 1; block_index++ )
					{
						uptr* next = zpl_cast( uptr* ) curr;
						*next      = zpl_cast( uptr ) curr + actual_block_size;
						curr       = pointer_add( curr, actual_block_size );
					}

					end            = zpl_cast( uptr* ) curr;
					*end           = zpl_cast( uptr ) NULL;
					pool->FreeList = pool->PhysicalStart;
				}
				break;

			case EAllocation_RESIZE :
				// NOTE: Cannot resize
				ZPL_PANIC( "You cannot resize something allocated by with a pool." );
				break;
		}

		return ptr;
	}

	Pool Pool::init_align( AllocatorInfo backing, sw num_blocks, sw block_size, sw block_align )
	{
		Pool pool = {};

		sw    actual_block_size, pool_size, block_index;
		void *data, *curr;
		uptr* end;

		pool.Backing     = backing;
		pool.BlockSize   = block_size;
		pool.BlockAlign  = block_align;
		pool.NumBlocks   = num_blocks;

		actual_block_size = block_size + block_align;
		pool_size         = num_blocks * actual_block_size;

		data = alloc_align( backing, pool_size, block_align );

		// NOTE: Init intrusive freelist
		curr = data;
		for ( block_index = 0; block_index < num_blocks - 1; block_index++ )
		{
			uptr* next = ( uptr* ) curr;
			*next      = ( uptr  ) curr + actual_block_size;
			curr       = pointer_add( curr, actual_block_size );
		}

		end  =  ( uptr* ) curr;
		*end =  ( uptr )  0;

		pool.PhysicalStart = data;
		pool.FreeList      = data;

		return pool;
	}

#pragma endregion Memory

#pragma region File Handling

	#if defined( ZPL_SYSTEM_WINDOWS ) || defined( ZPL_SYSTEM_CYGWIN )

	internal wchar_t* _alloc_utf8_to_ucs2( AllocatorInfo a, char const* text, sw* w_len_ )
	{
		wchar_t* w_text = NULL;
		sw       len = 0, w_len = 0, w_len1 = 0;
		if ( text == NULL )
		{
			if ( w_len_ )
				*w_len_ = w_len;
			return NULL;
		}
		len = str_len( text );
		if ( len == 0 )
		{
			if ( w_len_ )
				*w_len_ = w_len;
			return NULL;
		}
		w_len = MultiByteToWideChar( CP_UTF8, MB_ERR_INVALID_CHARS, text, zpl_cast( int ) len, NULL, 0 );
		if ( w_len == 0 )
		{
			if ( w_len_ )
				*w_len_ = w_len;
			return NULL;
		}
		w_text = alloc_array( a, wchar_t, w_len + 1 );
		w_len1 = MultiByteToWideChar( CP_UTF8, MB_ERR_INVALID_CHARS, text, zpl_cast( int ) len, w_text, zpl_cast( int ) w_len );
		if ( w_len1 == 0 )
		{
			free( a, w_text );
			if ( w_len_ )
				*w_len_ = 0;
			return NULL;
		}
		w_text[ w_len ] = 0;
		if ( w_len_ )
			*w_len_ = w_len;
		return w_text;
	}

	internal ZPL_FILE_SEEK_PROC( _win32_file_seek )
	{
		LARGE_INTEGER li_offset;
		li_offset.QuadPart = offset;
		if ( ! SetFilePointerEx( fd.p, li_offset, &li_offset, whence ) )
		{
			return false;
		}

		if ( new_offset )
			*new_offset = li_offset.QuadPart;
		return true;
	}

	internal ZPL_FILE_READ_AT_PROC( _win32_file_read )
	{
		// unused( stop_at_newline );
		b32 result = false;
		_win32_file_seek( fd, offset, ESeekWhence_BEGIN, NULL );
		DWORD size_ = zpl_cast( DWORD )( size > ZPL_I32_MAX ? ZPL_I32_MAX : size );
		DWORD bytes_read_;
		if ( ReadFile( fd.p, buffer, size_, &bytes_read_, NULL ) )
		{
			if ( bytes_read )
				*bytes_read = bytes_read_;
			result = true;
		}

		return result;
	}

	internal ZPL_FILE_WRITE_AT_PROC( _win32_file_write )
	{
		DWORD size_ = zpl_cast( DWORD )( size > ZPL_I32_MAX ? ZPL_I32_MAX : size );
		DWORD bytes_written_;
		_win32_file_seek( fd, offset, ESeekWhence_BEGIN, NULL );
		if ( WriteFile( fd.p, buffer, size_, &bytes_written_, NULL ) )
		{
			if ( bytes_written )
				*bytes_written = bytes_written_;
			return true;
		}
		return false;
	}

	internal ZPL_FILE_CLOSE_PROC( _win32_file_close )
	{
		CloseHandle( fd.p );
	}

	FileOperations const default_file_operations = { _win32_file_read, _win32_file_write, _win32_file_seek, _win32_file_close };

	ZPL_NEVER_INLINE ZPL_FILE_OPEN_PROC( _win32_file_open )
	{
		DWORD    desired_access;
		DWORD    creation_disposition;
		void*    handle;
		wchar_t* w_text;

		switch ( mode & ZPL_FILE_MODES )
		{
			case EFileMode_READ :
				desired_access       = GENERIC_READ;
				creation_disposition = OPEN_EXISTING;
				break;
			case EFileMode_WRITE :
				desired_access       = GENERIC_WRITE;
				creation_disposition = CREATE_ALWAYS;
				break;
			case EFileMode_APPEND :
				desired_access       = GENERIC_WRITE;
				creation_disposition = OPEN_ALWAYS;
				break;
			case EFileMode_READ | EFileMode_RW :
				desired_access       = GENERIC_READ | GENERIC_WRITE;
				creation_disposition = OPEN_EXISTING;
				break;
			case EFileMode_WRITE | EFileMode_RW :
				desired_access       = GENERIC_READ | GENERIC_WRITE;
				creation_disposition = CREATE_ALWAYS;
				break;
			case EFileMode_APPEND | EFileMode_RW :
				desired_access       = GENERIC_READ | GENERIC_WRITE;
				creation_disposition = OPEN_ALWAYS;
				break;
			default :
				ZPL_PANIC( "Invalid file mode" );
				return EFileError_INVALID;
		}

		w_text = _alloc_utf8_to_ucs2( heap(), filename, NULL );
		handle = CreateFileW( w_text, desired_access, FILE_SHARE_READ | FILE_SHARE_DELETE, NULL, creation_disposition, FILE_ATTRIBUTE_NORMAL, NULL );

		free( heap(), w_text );

		if ( handle == INVALID_HANDLE_VALUE )
		{
			DWORD err = GetLastError();
			switch ( err )
			{
				case ERROR_FILE_NOT_FOUND :
					return EFileError_NOT_EXISTS;
				case ERROR_FILE_EXISTS :
					return EFileError_EXISTS;
				case ERROR_ALREADY_EXISTS :
					return EFileError_EXISTS;
				case ERROR_ACCESS_DENIED :
					return EFileError_PERMISSION;
			}
			return EFileError_INVALID;
		}

		if ( mode & EFileMode_APPEND )
		{
			LARGE_INTEGER offset = { { 0 } };
			if ( ! SetFilePointerEx( handle, offset, NULL, ESeekWhence_END ) )
			{
				CloseHandle( handle );
				return EFileError_INVALID;
			}
		}

		fd->p = handle;
		*ops  = default_file_operations;
		return EFileError_NONE;
	}

	#else    // POSIX
	#	include <fcntl.h>

	internal ZPL_FILE_SEEK_PROC( _posix_file_seek )
	{
	#	if defined( ZPL_SYSTEM_OSX )
		s64 res = lseek( fd.i, offset, whence );
	#	else    // TODO(ZaKlaus): @fixme lseek64
		s64 res = lseek( fd.i, offset, whence );
	#	endif
		if ( res < 0 )
			return false;
		if ( new_offset )
			*new_offset = res;
		return true;
	}

	internal ZPL_FILE_READ_AT_PROC( _posix_file_read )
	{
		unused( stop_at_newline );
		sw res = pread( fd.i, buffer, size, offset );
		if ( res < 0 )
			return false;
		if ( bytes_read )
			*bytes_read = res;
		return true;
	}

	internal ZPL_FILE_WRITE_AT_PROC( _posix_file_write )
	{
		sw  res;
		s64 curr_offset = 0;
		_posix_file_seek( fd, 0, ESeekWhence_CURRENT, &curr_offset );
		if ( curr_offset == offset )
		{
			// NOTE: Writing to stdout et al. doesn't like pwrite for numerous reasons
			res = write( zpl_cast( int ) fd.i, buffer, size );
		}
		else
		{
			res = pwrite( zpl_cast( int ) fd.i, buffer, size, offset );
		}
		if ( res < 0 )
			return false;
		if ( bytes_written )
			*bytes_written = res;
		return true;
	}

	internal ZPL_FILE_CLOSE_PROC( _posix_file_close )
	{
		close( fd.i );
	}

	FileOperations const default_file_operations = { _posix_file_read, _posix_file_write, _posix_file_seek, _posix_file_close };

	ZPL_NEVER_INLINE ZPL_FILE_OPEN_PROC( _posix_file_open )
	{
		s32 os_mode;
		switch ( mode & ZPL_FILE_MODES )
		{
			case EFileMode_READ :
				os_mode = O_RDONLY;
				break;
			case EFileMode_WRITE :
				os_mode = O_WRONLY | O_CREAT | O_TRUNC;
				break;
			case EFileMode_APPEND :
				os_mode = O_WRONLY | O_APPEND | O_CREAT;
				break;
			case EFileMode_READ | EFileMode_RW :
				os_mode = O_RDWR;
				break;
			case EFileMode_WRITE | EFileMode_RW :
				os_mode = O_RDWR | O_CREAT | O_TRUNC;
				break;
			case EFileMode_APPEND | EFileMode_RW :
				os_mode = O_RDWR | O_APPEND | O_CREAT;
				break;
			default :
				ZPL_PANIC( "Invalid file mode" );
				return EFileError_INVALID;
		}

		fd->i = open( filename, os_mode, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH );
		if ( fd->i < 0 )
		{
			// TODO: More file errors
			return EFileError_INVALID;
		}

		*ops = default_file_operations;
		return EFileError_NONE;
	}

	// POSIX
	#endif

	internal void _dirinfo_free_entry( DirEntry* entry );

	FileError file_close( FileInfo* f )
	{
		if ( ! f )
			return EFileError_INVALID;

		if ( f->Filename )
			free( heap(), zpl_cast( char* ) f->Filename );

	#if defined( ZPL_SYSTEM_WINDOWS )
		if ( f->FD.p == INVALID_HANDLE_VALUE )
			return EFileError_INVALID;
	#else
		if ( f->fd.i < 0 )
			return EFileError_INVALID;
	#endif

		if ( f->IsTemp )
		{
			f->Ops.close( f->FD );
			return EFileError_NONE;
		}

		if ( ! f->Ops.read_at )
			f->Ops = default_file_operations;
		f->Ops.close( f->FD );

		if ( f->Dir )
		{
			_dirinfo_free_entry( f->Dir );
			mfree( f->Dir );
			f->Dir = NULL;
		}

		return EFileError_NONE;
	}

	FileError file_new( FileInfo* f, FileDescriptor fd, FileOperations ops, char const* filename )
	{
		FileError err = EFileError_NONE;
		sw        len = str_len( filename );

		f->Ops             = ops;
		f->FD              = fd;
		f->Dir             = nullptr;
		f->LastWriteTime   = 0;
		f->Filename        = alloc_array( heap(), char, len + 1 );
		mem_copy( zpl_cast( char* ) f->Filename, zpl_cast( char* ) filename, len + 1 );

		return err;
	}

	FileError file_open_mode( FileInfo* f, FileMode mode, char const* filename )
	{
		FileInfo file_ =
		{
			{ nullptr, nullptr, nullptr, nullptr },
			{ nullptr },
			0,
			nullptr,
			0,
			nullptr
		 };

		*f = file_;
		FileError err;

	#if defined( ZPL_SYSTEM_WINDOWS ) || defined( ZPL_SYSTEM_CYGWIN )
		err = _win32_file_open( &f->FD, &f->Ops, mode, filename );
	#else
		err = _posix_file_open( &f->fd, &f->ops, mode, filename );
	#endif

		if ( err == EFileError_NONE )
			return file_new( f, f->FD, f->Ops, filename );

		return err;
	}

	internal void _dirinfo_free_entry( DirEntry* entry )
	{
		if ( entry->Info )
		{
			dirinfo_free( entry->Info );
			mfree( entry->Info );
			entry->Info = nullptr;
		}
	}

	void dirinfo_free( DirInfo* dir )
	{
		ZPL_ASSERT_NOT_NULL( dir );

		for ( sw i = 0; i < array_count( dir->Entries ); ++i )
		{
			_dirinfo_free_entry( dir->Entries + i );
		}

		array_free( dir->Entries );
		array_free( dir->Filenames );
		// string_free( dir->Buffer );
		dir->Buffer.free();
		mfree( ( void* )dir->FullPath );
	}

#pragma endreigon File Handling

	namespace Memory
	{
		global AllocatorInfo GlobalAllocator;

		global Array<Arena> Global_AllocatorBuckets;

		void* Global_Allocator_Proc( void* allocator_data, AllocType type, sw size, sw alignment, void* old_memory, sw old_size, u64 flags )
		{
			Arena& last = Global_AllocatorBuckets.back();

			switch ( type )
			{
				case EAllocation_ALLOC:
				{
					if ( last.TotalUsed + size > last.TotalSize )
					{
						Arena bucket = Arena::init_from_allocator( heap(), BucketSize );

						if ( bucket.PhysicalStart == nullptr )
							fatal( "Failed to create bucket for Global_AllocatorBuckets");

						if ( ! Global_AllocatorBuckets.append( bucket ) )
							fatal( "Failed to append bucket to Global_AllocatorBuckets");

						last = Global_AllocatorBuckets.back();
					}

					return alloc_align( last, size, alignment );
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
					if ( last.TotalUsed + size > last.TotalSize )
					{
						Arena bucket = Arena::init_from_allocator( heap(), BucketSize );

						if ( bucket.PhysicalStart == nullptr )
							fatal( "Failed to create bucket for Global_AllocatorBuckets");

						if ( ! Global_AllocatorBuckets.append( bucket ) )
							fatal( "Failed to append bucket to Global_AllocatorBuckets");

						last = Global_AllocatorBuckets.back();
					}

					void* result = alloc_align( last.Backing, size, alignment );

					if ( result != nullptr && old_memory != nullptr )
					{
						mem_copy( result, old_memory, old_size );
					}

					return result;
				}
			}

			return nullptr;
		}

		void setup()
		{
			GlobalAllocator = AllocatorInfo { & Global_Allocator_Proc, nullptr };

			Global_AllocatorBuckets = Array<Arena>::init_reserve( heap(), 128 );

			if ( Global_AllocatorBuckets == nullptr )
				fatal( "Failed to reserve memory for Global_AllocatorBuckets");

			Arena bucket = Arena::init_from_allocator( heap(), BucketSize );

			if ( bucket.PhysicalStart == nullptr )
				fatal( "Failed to create first bucket for Global_AllocatorBuckets");

			Global_AllocatorBuckets.append( bucket );
		}

		void cleanup()
		{
			s32 index = 0;
			s32 left  = Global_AllocatorBuckets.num();
			do
			{
				Arena* bucket = & Global_AllocatorBuckets[ index ];
				bucket->free();
				index++;
			}
			while ( left--, left );

			Global_AllocatorBuckets.free();
		}

	// namespace Memory
	}

// namespace gen
}