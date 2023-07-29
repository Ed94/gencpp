#pragma region Printing

struct FileInfo;

#ifndef GEN_PRINTF_MAXLEN
#	define GEN_PRINTF_MAXLEN kilobytes(128)
#endif

// NOTE: A locally persisting buffer is used internally
char* str_fmt_buf       ( char const* fmt, ... );
char* str_fmt_buf_va    ( char const* fmt, va_list va );
sw    str_fmt_va        ( char* str, sw n, char const* fmt, va_list va );
sw    str_fmt_out_va    ( char const* fmt, va_list va );
sw    str_fmt_out_err   ( char const* fmt, ... );
sw    str_fmt_out_err_va( char const* fmt, va_list va );

// TODO : Move these to file handling.

sw    str_fmt_file      ( FileInfo* f, char const* fmt, ... );
sw    str_fmt_file_va   ( FileInfo* f, char const* fmt, va_list va );

constexpr
char const* Msg_Invalid_Value = "INVALID VALUE PROVIDED";

inline
sw log_fmt(char const* fmt, ...)
{
	sw res;
	va_list va;

	va_start(va, fmt);
	res = str_fmt_out_va(fmt, va);
	va_end(va);

	return res;
}

inline
sw fatal(char const* fmt, ...)
{
	local_persist thread_local
	char buf[GEN_PRINTF_MAXLEN] = { 0 };

	va_list va;

#if Build_Debug
	va_start(va, fmt);
	str_fmt_va(buf, GEN_PRINTF_MAXLEN, fmt, va);
	va_end(va);

	assert_crash(buf);
	return -1;
#else
	va_start(va, fmt);
	str_fmt_out_err_va( fmt, va);
	va_end(va);

	exit(1);
	return -1;
#endif
}

#pragma endregion Printing
