#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#	include "string_ops.hpp"
#endif

#pragma region Printing

GEN_API_C_BEGIN

struct FileInfo;

#ifndef GEN_PRINTF_MAXLEN
#	define GEN_PRINTF_MAXLEN kilobytes(128)
#endif

// NOTE: A locally persisting buffer is used internally
char*  str_fmt_buf       ( char const* fmt, ... );
char*  str_fmt_buf_va    ( char const* fmt, va_list va );
ssize  str_fmt           ( char* str, ssize n, char const* fmt, ... );
ssize  str_fmt_va        ( char* str, ssize n, char const* fmt, va_list va );
ssize  str_fmt_out_va    ( char const* fmt, va_list va );
ssize  str_fmt_out_err   ( char const* fmt, ... );
ssize  str_fmt_out_err_va( char const* fmt, va_list va );
ssize  str_fmt_file      ( FileInfo* f, char const* fmt, ... );
ssize  str_fmt_file_va   ( FileInfo* f, char const* fmt, va_list va );

constexpr
char const* Msg_Invalid_Value = "INVALID VALUE PROVIDED";

inline
ssize log_fmt(char const* fmt, ...)
{
	ssize res;
	va_list va;

	va_start(va, fmt);
	res = str_fmt_out_va(fmt, va);
	va_end(va);

	return res;
}

GEN_API_C_END

#pragma endregion Printing
