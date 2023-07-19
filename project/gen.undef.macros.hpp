#if GEN_TIME
// This undefines the macros used by the gen library but are not necessary for the user.

#undef GEN_ARCH_64_BIT
#undef GEN_ARCH_32_BIT

#undef GEN_SYSTEM_ANDROID
#undef GEN_SYSTEM_CYGWIN
#undef GEN_SYSTEM_EMSCRIPTEN
#undef GEN_SYSTEM_FREEBSD
#undef GEN_SYSTEM_IOS
#undef GEN_SYSTEM_LINUX
#undef GEN_SYSTEM_MACOS
#undef GEN_SYSTEM_OPENBSD
#undef GEN_SYSTEM_OSX
#undef GEN_SYSTEM_UNIX
#undef GEN_SYSTEM_WINDOWS

#undef GEN_COMPILER_CLANG
#undef GEN_COMPILER_GCC
#undef GEN_COMPILER_MINGW
#undef GEN_COMPILER_MSVC

#undef forceinline
#undef neverinline

#undef zpl_cast

#undef global
#undef internal
#undef local_persist

#undef kilobytes
#undef megabytes
#undef gigabytes
#undef terabytes

#undef zero_item
#undef zero_array

#undef alloc_item
#undef alloc_array

#undef malloc
#undef mfree

#undef count_of
#undef is_between
#undef min
#undef size_of
#undef swap

#undef bit
#undef bitfield_is_equal
#undef ccast
#undef scast
#undef rcast
#undef pcast
#undef do_once
#undef do_once_start
#undef do_once_end
#undef num_args
#undef num_args_impl
#undef stringize
#undef stringize
#undef stringize_va
#undef txt_StrC

#undef GEN_TIME
#undef gen_main
#undef __
#undef name
#undef code
#undef args
#undef code_str
#undef code_fmt
#undef token_fmt

// GEN_TIME
#endif
