#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#	include "strbuilder_ops.hpp"
#endif

#pragma region Printing

typedef struct FileInfo FileInfo;

#ifndef GEN_PRINTF_MAXLEN
#	define GEN_PRINTF_MAXLEN kilobytes(128)
#endif
typedef char PrintF_Buffer[GEN_PRINTF_MAXLEN];

// NOTE: A locally persisting buffer is used internally
GEN_API char*  c_str_fmt_buf       ( char const* fmt, ... );
GEN_API char*  c_str_fmt_buf_va    ( char const* fmt, va_list va );
GEN_API ssize  c_str_fmt           ( char* str, ssize n, char const* fmt, ... );
GEN_API ssize  c_str_fmt_va        ( char* str, ssize n, char const* fmt, va_list va );
GEN_API ssize  c_str_fmt_out_va    ( char const* fmt, va_list va );
GEN_API ssize  c_str_fmt_out_err   ( char const* fmt, ... );
GEN_API ssize  c_str_fmt_out_err_va( char const* fmt, va_list va );
GEN_API ssize  c_str_fmt_file      ( FileInfo* f, char const* fmt, ... );
GEN_API ssize  c_str_fmt_file_va   ( FileInfo* f, char const* fmt, va_list va );

constexpr
char const* Msg_Invalid_Value = "INVALID VALUE PROVIDED";

inline
ssize log_fmt(char const* fmt, ...)
{
	ssize res;
	va_list va;

	va_start(va, fmt);
	res = c_str_fmt_out_va(fmt, va);
	va_end(va);

	return res;
}

#pragma endregion Printing
