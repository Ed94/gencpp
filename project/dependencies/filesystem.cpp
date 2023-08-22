#pragma once
// #include "strings.cpp"

#pragma region File Handling

#if defined( GEN_SYSTEM_WINDOWS ) || defined( GEN_SYSTEM_CYGWIN )

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

internal GEN_FILE_SEEK_PROC( _win32_file_seek )
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

internal GEN_FILE_READ_AT_PROC( _win32_file_read )
{
	// unused( stop_at_newline );
	b32 result = false;
	_win32_file_seek( fd, offset, ESeekWhence_BEGIN, NULL );
	DWORD size_ = zpl_cast( DWORD )( size > GEN_I32_MAX ? GEN_I32_MAX : size );
	DWORD bytes_read_;
	if ( ReadFile( fd.p, buffer, size_, &bytes_read_, NULL ) )
	{
		if ( bytes_read )
			*bytes_read = bytes_read_;
		result = true;
	}

	return result;
}

internal GEN_FILE_WRITE_AT_PROC( _win32_file_write )
{
	DWORD size_ = zpl_cast( DWORD )( size > GEN_I32_MAX ? GEN_I32_MAX : size );
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

internal GEN_FILE_CLOSE_PROC( _win32_file_close )
{
	CloseHandle( fd.p );
}

FileOperations const default_file_operations = { _win32_file_read, _win32_file_write, _win32_file_seek, _win32_file_close };

neverinline GEN_FILE_OPEN_PROC( _win32_file_open )
{
	DWORD    desired_access;
	DWORD    creation_disposition;
	void*    handle;
	wchar_t* w_text;

	switch ( mode & GEN_FILE_MODES )
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
			GEN_PANIC( "Invalid file mode" );
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

internal GEN_FILE_SEEK_PROC( _posix_file_seek )
{
#	if defined( GEN_SYSTEM_OSX )
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

internal GEN_FILE_READ_AT_PROC( _posix_file_read )
{
	unused( stop_at_newline );
	sw res = pread( fd.i, buffer, size, offset );
	if ( res < 0 )
		return false;
	if ( bytes_read )
		*bytes_read = res;
	return true;
}

internal GEN_FILE_WRITE_AT_PROC( _posix_file_write )
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

internal GEN_FILE_CLOSE_PROC( _posix_file_close )
{
	close( fd.i );
}

FileOperations const default_file_operations = { _posix_file_read, _posix_file_write, _posix_file_seek, _posix_file_close };

neverinline GEN_FILE_OPEN_PROC( _posix_file_open )
{
	s32 os_mode;
	switch ( mode & GEN_FILE_MODES )
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
			GEN_PANIC( "Invalid file mode" );
			return EFileError_INVALID;
	}

	fd->i = open( filename, os_mode, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH );
	if ( fd->i < 0 )
	{
		// TODO : More file errors
		return EFileError_INVALID;
	}

	*ops = default_file_operations;
	return EFileError_NONE;
}

// POSIX
#endif

internal void _dirinfo_free_entry( DirEntry* entry );

// TODO : Is this a bad idea?
global b32      _std_file_set                     = false;
global FileInfo _std_files[ EFileStandard_COUNT ] = {
{
	{ nullptr, nullptr, nullptr, nullptr },
	{ nullptr },
	0,
	nullptr,
	0,
	nullptr
} };

#if defined( GEN_SYSTEM_WINDOWS ) || defined( GEN_SYSTEM_CYGWIN )

FileInfo* file_get_standard( FileStandardType std )
{
	if ( ! _std_file_set )
	{
#	define GEN__SET_STD_FILE( type, v ) \
		_std_files[ type ].fd.p = v;     \
		_std_files[ type ].ops  = default_file_operations
		GEN__SET_STD_FILE( EFileStandard_INPUT, GetStdHandle( STD_INPUT_HANDLE ) );
		GEN__SET_STD_FILE( EFileStandard_OUTPUT, GetStdHandle( STD_OUTPUT_HANDLE ) );
		GEN__SET_STD_FILE( EFileStandard_ERROR, GetStdHandle( STD_ERROR_HANDLE ) );
#	undef GEN__SET_STD_FILE
		_std_file_set = true;
	}
	return &_std_files[ std ];
}

#else    // POSIX

FileInfo* file_get_standard( FileStandardType std )
{
	if ( ! _std_file_set )
	{
#	define GEN__SET_STD_FILE( type, v ) \
		_std_files[ type ].fd.i = v;     \
		_std_files[ type ].ops  = default_file_operations
		GEN__SET_STD_FILE( EFileStandard_INPUT, 0 );
		GEN__SET_STD_FILE( EFileStandard_OUTPUT, 1 );
		GEN__SET_STD_FILE( EFileStandard_ERROR, 2 );
#	undef GEN__SET_STD_FILE
		_std_file_set = true;
	}
	return &_std_files[ std ];
}

#endif

FileError file_close( FileInfo* f )
{
	if ( ! f )
		return EFileError_INVALID;

	if ( f->filename )
		free( heap(), zpl_cast( char* ) f->filename );

#if defined( GEN_SYSTEM_WINDOWS )
	if ( f->fd.p == INVALID_HANDLE_VALUE )
		return EFileError_INVALID;
#else
	if ( f->fd.i < 0 )
		return EFileError_INVALID;
#endif

	if ( f->is_temp )
	{
		f->ops.close( f->fd );
		return EFileError_NONE;
	}

	if ( ! f->ops.read_at )
		f->ops = default_file_operations;
	f->ops.close( f->fd );

#if 0
	if ( f->Dir )
	{
		_dirinfo_free_entry( f->Dir );
		mfree( f->Dir );
		f->Dir = NULL;
	}
#endif

	return EFileError_NONE;
}

FileError file_new( FileInfo* f, FileDescriptor fd, FileOperations ops, char const* filename )
{
	FileError err = EFileError_NONE;
	sw        len = str_len( filename );

	f->ops             = ops;
	f->fd              = fd;
	f->dir             = nullptr;
	f->last_write_time = 0;
	f->filename        = alloc_array( heap(), char, len + 1 );
	mem_copy( zpl_cast( char* ) f->filename, zpl_cast( char* ) filename, len + 1 );

	return err;
}

FileError file_open( FileInfo* f, char const* filename )
{
	return file_open_mode( f, EFileMode_READ, filename );
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

#if defined( GEN_SYSTEM_WINDOWS ) || defined( GEN_SYSTEM_CYGWIN )
	err = _win32_file_open( &f->fd, &f->ops, mode, filename );
#else
	err = _posix_file_open( &f->fd, &f->ops, mode, filename );
#endif

	if ( err == EFileError_NONE )
		return file_new( f, f->fd, f->ops, filename );

	return err;
}

s64 file_size( FileInfo* f )
{
	s64 size        = 0;
	s64 prev_offset = file_tell( f );

	file_seek_to_end( f );
	size = file_tell( f );

	file_seek( f, prev_offset );

	return size;
}

FileContents file_read_contents( AllocatorInfo a, b32 zero_terminate, char const* filepath )
{
	FileContents result;
	FileInfo     file  ;

	result.allocator = a;

	if ( file_open( &file, filepath ) == EFileError_NONE )
	{
		sw fsize = zpl_cast( sw ) file_size( &file );
		if ( fsize > 0 )
		{
			result.data = alloc( a, zero_terminate ? fsize + 1 : fsize );
			result.size = fsize;
			file_read_at( &file, result.data, result.size, 0 );
			if ( zero_terminate )
			{
				u8* str      = zpl_cast( u8* ) result.data;
				str[ fsize ] = '\0';
			}
		}
		file_close( &file );
	}

	return result;
}

struct _memory_fd
{
	u8            magic;
	u8*           buf;    //< zpl_array OR plain buffer if we can't write
	sw            cursor;
	AllocatorInfo allocator;

	FileStreamFlags flags;
	sw              cap;
};

#define GEN__FILE_STREAM_FD_MAGIC 37

GEN_DEF_INLINE FileDescriptor _file_stream_fd_make( _memory_fd* d );
GEN_DEF_INLINE _memory_fd*    _file_stream_from_fd( FileDescriptor fd );

GEN_IMPL_INLINE FileDescriptor _file_stream_fd_make( _memory_fd* d )
{
	FileDescriptor fd = { 0 };
	fd.p              = ( void* )d;
	return fd;
}

GEN_IMPL_INLINE _memory_fd* _file_stream_from_fd( FileDescriptor fd )
{
	_memory_fd* d = ( _memory_fd* )fd.p;
	GEN_ASSERT( d->magic == GEN__FILE_STREAM_FD_MAGIC );
	return d;
}

b8 file_stream_new( FileInfo* file, AllocatorInfo allocator )
{
	GEN_ASSERT_NOT_NULL( file );

	_memory_fd* d = ( _memory_fd* )alloc( allocator, size_of( _memory_fd ) );

	if ( ! d )
		return false;

	zero_item( file );
	d->magic     = GEN__FILE_STREAM_FD_MAGIC;
	d->allocator = allocator;
	d->flags     = EFileStream_CLONE_WRITABLE;
	d->cap       = 0;
	d->buf       = Array<u8>::init( allocator );

	if ( ! d->buf )
		return false;

	file->ops             = memory_file_operations;
	file->fd              = _file_stream_fd_make( d );
	file->dir             = NULL;
	file->last_write_time = 0;
	file->filename        = NULL;
	file->is_temp         = true;
	return true;
}

b8 file_stream_open( FileInfo* file, AllocatorInfo allocator, u8* buffer, sw size, FileStreamFlags flags )
{
	GEN_ASSERT_NOT_NULL( file );
	_memory_fd* d = ( _memory_fd* )alloc( allocator, size_of( _memory_fd ) );
	if ( ! d )
		return false;
	zero_item( file );
	d->magic     = GEN__FILE_STREAM_FD_MAGIC;
	d->allocator = allocator;
	d->flags     = flags;
	if ( d->flags & EFileStream_CLONE_WRITABLE )
	{
		Array<u8> arr = Array<u8>::init_reserve( allocator, size );
		d->buf = arr;

		if ( ! d->buf )
			return false;

		mem_copy( d->buf, buffer, size );
		d->cap = size;

		arr.get_header()->Num = size;
	}
	else
	{
		d->buf = buffer;
		d->cap = size;
	}
	file->ops             = memory_file_operations;
	file->fd              = _file_stream_fd_make( d );
	file->dir             = NULL;
	file->last_write_time = 0;
	file->filename        = NULL;
	file->is_temp         = true;
	return true;
}

u8* file_stream_buf( FileInfo* file, sw* size )
{
	GEN_ASSERT_NOT_NULL( file );
	_memory_fd* d = _file_stream_from_fd( file->fd );
	if ( size )
		*size = d->cap;
	return d->buf;
}

internal GEN_FILE_SEEK_PROC( _memory_file_seek )
{
	_memory_fd* d      = _file_stream_from_fd( fd );
	sw          buflen = d->cap;

	if ( whence == ESeekWhence_BEGIN )
		d->cursor = 0;
	else if ( whence == ESeekWhence_END )
		d->cursor = buflen;

	d->cursor = max( 0, clamp( d->cursor + offset, 0, buflen ) );
	if ( new_offset )
		*new_offset = d->cursor;
	return true;
}

internal GEN_FILE_READ_AT_PROC( _memory_file_read )
{
	// unused( stop_at_newline );
	_memory_fd* d = _file_stream_from_fd( fd );
	mem_copy( buffer, d->buf + offset, size );
	if ( bytes_read )
		*bytes_read = size;
	return true;
}

internal GEN_FILE_WRITE_AT_PROC( _memory_file_write )
{
	_memory_fd* d = _file_stream_from_fd( fd );

	if ( ! ( d->flags & ( EFileStream_CLONE_WRITABLE | EFileStream_WRITABLE ) ) )
		return false;

	sw buflen   = d->cap;
	sw extralen = max( 0, size - ( buflen - offset ) );
	sw rwlen    = size - extralen;
	sw new_cap  = buflen + extralen;

	if ( d->flags & EFileStream_CLONE_WRITABLE )
	{
		Array<u8> arr = { d->buf };

		if ( arr.get_header()->Capacity < new_cap )
		{
			if ( ! arr.grow( ( s64 )( new_cap ) ) )
				return false;
			d->buf = arr;
		}
	}

	mem_copy( d->buf + offset, buffer, rwlen );

	if ( ( d->flags & EFileStream_CLONE_WRITABLE ) && extralen > 0 )
	{
		Array<u8> arr = { d->buf };

		mem_copy( d->buf + offset + rwlen, pointer_add_const( buffer, rwlen ), extralen );
		d->cap = new_cap;
		arr.get_header()->Capacity = new_cap;
	}
	else
	{
		extralen = 0;
	}

	if ( bytes_written )
		*bytes_written = ( rwlen + extralen );
	return true;
}

internal GEN_FILE_CLOSE_PROC( _memory_file_close )
{
	_memory_fd*   d         = _file_stream_from_fd( fd );
	AllocatorInfo allocator = d->allocator;

	if ( d->flags & EFileStream_CLONE_WRITABLE )
	{
		Array<u8> arr = { d->buf };
		arr.free();
	}

	free( allocator, d );
}

FileOperations const memory_file_operations = { _memory_file_read, _memory_file_write, _memory_file_seek, _memory_file_close };

#pragma endregion File Handling
