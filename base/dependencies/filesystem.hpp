#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#	include "strings.hpp"
#endif

#pragma region File Handling

enum FileModeFlag
{
	EFileMode_READ   = bit( 0 ),
	EFileMode_WRITE  = bit( 1 ),
	EFileMode_APPEND = bit( 2 ),
	EFileMode_RW     = bit( 3 ),
	GEN_FILE_MODES   = EFileMode_READ | EFileMode_WRITE | EFileMode_APPEND | EFileMode_RW,
};

// NOTE: Only used internally and for the file operations
enum SeekWhenceType
{
	ESeekWhence_BEGIN   = 0,
	ESeekWhence_CURRENT = 1,
	ESeekWhence_END     = 2,
};

enum FileError
{
	EFileError_NONE,
	EFileError_INVALID,
	EFileError_INVALID_FILENAME,
	EFileError_EXISTS,
	EFileError_NOT_EXISTS,
	EFileError_PERMISSION,
	EFileError_TRUNCATION_FAILURE,
	EFileError_NOT_EMPTY,
	EFileError_NAME_TOO_LONG,
	EFileError_UNKNOWN,
};

union FileDescriptor
{
	void* p;
	sptr  i;
	uptr  u;
};

typedef u32                   FileMode;
typedef struct FileOperations FileOperations;

#define GEN_FILE_OPEN_PROC( name )     FileError name( FileDescriptor* fd, FileOperations* ops, FileMode mode, char const* filename )
#define GEN_FILE_READ_AT_PROC( name )  b32 name( FileDescriptor fd, void* buffer, ssize size, s64 offset, ssize* bytes_read, b32 stop_at_newline )
#define GEN_FILE_WRITE_AT_PROC( name ) b32 name( FileDescriptor fd, mem_ptr_const buffer, ssize size, s64 offset, ssize* bytes_written )
#define GEN_FILE_SEEK_PROC( name )     b32 name( FileDescriptor fd, s64 offset, SeekWhenceType whence, s64* new_offset )
#define GEN_FILE_CLOSE_PROC( name )    void name( FileDescriptor fd )

typedef GEN_FILE_OPEN_PROC( file_open_proc );
typedef GEN_FILE_READ_AT_PROC( FileReadProc );
typedef GEN_FILE_WRITE_AT_PROC( FileWriteProc );
typedef GEN_FILE_SEEK_PROC( FileSeekProc );
typedef GEN_FILE_CLOSE_PROC( FileCloseProc );

struct FileOperations
{
	FileReadProc*  read_at;
	FileWriteProc* write_at;
	FileSeekProc*  seek;
	FileCloseProc* close;
};

extern FileOperations const default_file_operations;

typedef u64 FileTime;

enum DirType
{
	GEN_DIR_TYPE_FILE,
	GEN_DIR_TYPE_FOLDER,
	GEN_DIR_TYPE_UNKNOWN,
};

struct DirInfo;

struct DirEntry
{
	char const* filename;
	DirInfo*    dir_info;
	u8          type;
};

struct DirInfo
{
	char const* fullpath;
	DirEntry*   entries;    // zpl_array

	// Internals
	char** filenames;    // zpl_array
	StrBuilder buf;
};

struct FileInfo
{
	FileOperations ops;
	FileDescriptor fd;
	b32            is_temp;

	char const* filename;
	FileTime    last_write_time;
	DirEntry*   dir;
};

enum FileStandardType
{
	EFileStandard_INPUT,
	EFileStandard_OUTPUT,
	EFileStandard_ERROR,

	EFileStandard_COUNT,
};

/**
	* Get standard file I/O.
	* @param  std Check zpl_file_standard_type
	* @return     File handle to standard I/O
	*/
GEN_API FileInfo* file_get_standard( FileStandardType std );

/**
	* Closes the file
	* @param  file
	*/
GEN_API FileError file_close( FileInfo* file );

/**
	* Returns the currently opened file's name
	* @param  file
	*/
inline
char const* file_name( FileInfo* file )
{
	return file->filename ? file->filename : "";
}

/**
	* Opens a file
	* @param  file
	* @param  filename
	*/
GEN_API FileError file_open( FileInfo* file, char const* filename );

/**
	* Opens a file using a specified mode
	* @param  file
	* @param  mode     Access mode to use
	* @param  filename
	*/
GEN_API FileError file_open_mode( FileInfo* file, FileMode mode, char const* filename );

/**
	* Reads from a file
	* @param  file
	* @param  buffer Buffer to read to
	* @param  size   Size to read
	*/
b32 file_read( FileInfo* file, void* buffer, ssize size );

/**
	* Reads file at a specific offset
	* @param  file
	* @param  buffer     Buffer to read to
	* @param  size       Size to read
	* @param  offset     Offset to read from
	* @param  bytes_read How much data we've actually read
	*/
b32 file_read_at( FileInfo* file, void* buffer, ssize size, s64 offset );

/**
	* Reads file safely
	* @param  file
	* @param  buffer     Buffer to read to
	* @param  size       Size to read
	* @param  offset     Offset to read from
	* @param  bytes_read How much data we've actually read
	*/
b32 file_read_at_check( FileInfo* file, void* buffer, ssize size, s64 offset, ssize* bytes_read );

typedef struct FileContents FileContents;
struct FileContents
{
	AllocatorInfo allocator;
	void*         data;
	ssize            size;
};

constexpr b32 file_zero_terminate    = true;
constexpr b32 file_no_zero_terminate = false;

/**
	* Reads the whole file contents
	* @param  a              Allocator to use
	* @param  zero_terminate End the read data with null terminator
	* @param  filepath       Path to the file
	* @return                File contents data
	*/
GEN_API FileContents file_read_contents( AllocatorInfo a, b32 zero_terminate, char const* filepath );

/**
	* Returns a size of the file
	* @param  file
	* @return      File size
	*/
GEN_API s64 file_size( FileInfo* file );

/**
	* Seeks the file cursor from the beginning of file to a specific position
	* @param  file
	* @param  offset Offset to seek to
	*/
s64 file_seek( FileInfo* file, s64 offset );

/**
	* Seeks the file cursor to the end of the file
	* @param  file
	*/
s64 file_seek_to_end( FileInfo* file );

/**
	* Returns the length from the beginning of the file we've read so far
	* @param  file
	* @return      Our current position in file
	*/
s64 file_tell( FileInfo* file );

/**
	* Writes to a file
	* @param  file
	* @param  buffer Buffer to read from
	* @param  size   Size to read
	*/
b32 file_write( FileInfo* file, void const* buffer, ssize size );

/**
	* Writes to file at a specific offset
	* @param  file
	* @param  buffer        Buffer to read from
	* @param  size          Size to write
	* @param  offset        Offset to write to
	* @param  bytes_written How much data we've actually written
	*/
b32 file_write_at( FileInfo* file, void const* buffer, ssize size, s64 offset );

/**
	* Writes to file safely
	* @param  file
	* @param  buffer        Buffer to read from
	* @param  size          Size to write
	* @param  offset        Offset to write to
	* @param  bytes_written How much data we've actually written
	*/
b32 file_write_at_check( FileInfo* file, void const* buffer, ssize size, s64 offset, ssize* bytes_written );

enum FileStreamFlags : u32
{
	/* Allows us to write to the buffer directly. Beware: you can not append a new data! */
	EFileStream_WRITABLE = bit( 0 ),

	/* Clones the input buffer so you can write (zpl_file_write*) data into it. */
	/* Since we work with a clone, the buffer size can dynamically grow as well. */
	EFileStream_CLONE_WRITABLE = bit( 1 ),

	EFileStream_UNDERLYING = GEN_U32_MAX,
};

/**
	* Opens a new memory stream
	* @param file
	* @param allocator
	*/
GEN_API b8 file_stream_new( FileInfo* file, AllocatorInfo allocator );

/**
	* Opens a memory stream over an existing buffer
	* @param  file
	* @param  allocator
	* @param  buffer   Memory to create stream from
	* @param  size     Buffer's size
	* @param  flags
	*/
GEN_API b8 file_stream_open( FileInfo* file, AllocatorInfo allocator, u8* buffer, ssize size, FileStreamFlags flags );

/**
	* Retrieves the stream's underlying buffer and buffer size.
	* @param file memory stream
	* @param size (Optional) buffer size
	*/
GEN_API u8* file_stream_buf( FileInfo* file, ssize* size );

extern FileOperations const memory_file_operations;

inline
s64 file_seek( FileInfo* f, s64 offset )
{
	s64 new_offset = 0;

	if ( ! f->ops.read_at )
		f->ops = default_file_operations;

	f->ops.seek( f->fd, offset, ESeekWhence_BEGIN, &new_offset );

	return new_offset;
}

inline
s64 file_seek_to_end( FileInfo* f )
{
	s64 new_offset = 0;

	if ( ! f->ops.read_at )
		f->ops = default_file_operations;

	f->ops.seek( f->fd, 0, ESeekWhence_END, &new_offset );

	return new_offset;
}

inline
s64 file_tell( FileInfo* f )
{
	s64 new_offset = 0;

	if ( ! f->ops.read_at )
		f->ops = default_file_operations;

	f->ops.seek( f->fd, 0, ESeekWhence_CURRENT, &new_offset );

	return new_offset;
}

inline
b32 file_read( FileInfo* f, void* buffer, ssize size )
{
	s64 cur_offset = file_tell( f );
	b32 result     = file_read_at( f, buffer, size, file_tell( f ) );
	file_seek( f, cur_offset + size );
	return result;
}

inline
b32 file_read_at( FileInfo* f, void* buffer, ssize size, s64 offset )
{
	return file_read_at_check( f, buffer, size, offset, NULL );
}

inline
b32 file_read_at_check( FileInfo* f, void* buffer, ssize size, s64 offset, ssize* bytes_read )
{
	if ( ! f->ops.read_at )
		f->ops = default_file_operations;
	return f->ops.read_at( f->fd, buffer, size, offset, bytes_read, false );
}

inline
b32 file_write( FileInfo* f, void const* buffer, ssize size )
{
	s64 cur_offset = file_tell( f );
	b32 result     = file_write_at( f, buffer, size, file_tell( f ) );

	file_seek( f, cur_offset + size );

	return result;
}

inline
b32 file_write_at( FileInfo* f, void const* buffer, ssize size, s64 offset )
{
	return file_write_at_check( f, buffer, size, offset, NULL );
}

inline
b32 file_write_at_check( FileInfo* f, void const* buffer, ssize size, s64 offset, ssize* bytes_written )
{
	if ( ! f->ops.read_at )
		f->ops = default_file_operations;

	return f->ops.write_at( f->fd, buffer, size, offset, bytes_written );
}

#pragma endregion File Handling
