/**
  zpl - Pushing the boundaries of simplicity.

Usage:
#  define ZPL_IMPLEMENTATION exactly in ONE source file right BEFORE including the library, like:

#  define ZPL_IMPLEMENTATION
#  include "zpl.h"

  You can also use a lightweight version of zpl by using ZPL_NANO, like:

#  define ZPL_IMPLEMENTATION
#  define ZPL_NANO
#  include "zpl.h"

  There is also a distribution that provides only the essential modules, you can enable it by defining ZPL_PICO.
  Currently, the distro offers: preprocessor helpers, debug module, memory API (except vm) and collections.
  Some of these modules used to depend on zpl_printf, but they use the standard library if the distro is enabled now.

#  define ZPL_IMPLEMENTATION
#  define ZPL_PICO
#  include "zpl.h"

Options:
  ZPL_EXPOSE_TYPES - exposes all zpl defined types to the global namespace. This means type such as `zpl_u32` is now available as `u32` globally.
  ZPL_DEFINE_NULL_MACRO - to let zpl define what NULL stands for in case it is undefined.
  ZPL_NO_MATH_H - disables the use of math.h library and replaces it with custom routines or SIMD.
  ZPL_HEAP_ANALYSIS - enables heap allocator analysis tools
  ZPL_PARSER_DISABLE_ANALYSIS - disables the extra parsing logic that would collect more information about node's formatting and structure.
                                this is useful in scenarios where a raw parsing performance is preferred over a more complex analysis.
                                It is not recommended to serialise data back since we lack the extra information about the original source document.

GitHub:
  https://github.com/zpl-c/zpl

Version History:
  19.0.0  - Check all results of zpl_alloc() when using JSON parser/writer (rheatley-pervasid)

  18.1.5  - set parent to parsed JSON nodes (fixed)
  		- fix zpl_json/csv_write_string off-by-one issue
  18.1.4  - fix zpl_random_gen_isize/zpl_random_range_isize 32bit overflow
  18.1.3  - set parent to parsed JSON nodes
  18.1.2  - fix zpl sort procs
  18.1.1  - updated table _clear method
  18.1.0  - added table _clear method
  18.0.4  - fix memory arena alignment & added tests
  18.0.3  - fix emscripten support
  18.0.2  - fix global-buffer-overflow in print module
          - raise ZPL_PRINTF_MAXLEN to 64kb
  18.0.1  - fix ADT parser wrongly assuming that an IP address is a real number
  18.0.0  - removed coroutines module
          - removed timer module
          - rename zpl_adt_get -> zpl_adt_query

  17.0.0  - ADT API changes
              zpl_adt_inset_* -> zpl_adt_append_*
              zpl_adt_node now holds a parent field, methods no longer require a pointer to the parent
              methods are now documented
          - add zpl_thread_init_nowait (gaopeng)

  16.1.1  - fix scientific notation parsing
  16.1.0  - introduce ZPL_PARSER_DISABLE_ANALYSIS that disables extra parsing capabilities to offer better raw performance
              at a cost of lack of node metadata.
  16.0.0  - introduce a new zpl_adt_query method for flexible data retrieval
              "a/b/c" navigates through objects "a" and "b" to get to "c"
              "arr/[foo=123]/bar" iterates over "arr" to find any object with param "foo" that matches the value "123", then gets its field called "bar"
              "arr/3" retrieves the 4th element in "arr"
              "arr/[apple]" retrieves the first element of value "apple" in "arr"
          - fix memory leak when parsing a json array (gaopeng)
          - add zpl_strntok (gaopeng)
          - add zpl_semaphore_trywait (gaopeng)

  15.0.3  - fix zpl_sign call in math failing to compile
              on macos devices
  15.0.2  - zpl_sign0 was introduced
  15.0.1  - hashtable performance improvements
  		- zpl_sign(0) returns 0
  15.0.0  - Rework zpl ring buffer
  		- various code improvements

  14.1.7  - fix zpl_random_range_i64
  		- set thread's is_running before we start a thread
  14.1.6  - remove windows.h dependency for header part
  14.1.5  - fix array append_at
  14.1.4  - Fix win32 missing CRITICAL_SECTION definition if
  		- ZPL_NO_WINDOWS_H is defined
  14.1.0  - add hashtable map_mut method
  14.0.1  - fix zpl_array_remove_at boundary bug
  14.0.0  - heap memory allocator analysis

  13.4.1  - adt optimizations
  13.4.0  - new adt manipulation methods
  13.3.3  - fix zpl_str_skip_literal bug
  13.3.2  - escape strings in parser output
  13.3.1  - number parsing improvements
  13.3.0  - csv parse numbers
  13.2.0  - hashtable _map function
  13.1.5  - ZPL_DEBUG_TRAP for tcc
  13.1.4  - potential csv ub fix
  13.1.3  - tcc support improvements
  13.1.2  - fix ast -> adt filename
  13.1.1  - fix emscripten support
  13.1.0  - abstract data tree naming update
  13.0.0  - text parsers refactor

  12.8.0  - zpl_opts improvements
  12.7.0  - math improvements
  12.6.2  - remove register usage (BeastLe9enD)
  12.6.1  - improve tar null padding code
  12.6.0  - introduce zpl_align_forward_u64/i64
  12.5.1  - small type casting fixes
  12.5.0  - add zpl_asprintf
  12.4.0  - zpl_printf improvements
  12.3.2  - allow zpl_path_dirlist to include symlinks, but don't enter them
  12.3.1  - avoid symbolic link cycle in zpl_path_dirlist
  12.3.0  - add TAR archiving support
  12.2.1  - fix zpl_random_gen_f64
  12.2.0  - Add zpl_array_fill and zpl_array_appendv_at
  12.1.0  - Add rectangle partitioning
  12.0.1  - Optimize zpl_strlen
  12.0.0  - JSON API revamp + improvements

  11.3.0  - JSON zpl_json_str_to_flt + cleanup
  11.2.5  - fix small atomics typo
  11.2.4  - JSON rewrite core parser
  11.2.2  - JSON rewrite comment handling
  11.2.1  - JSON zero-initialise node
  11.2.0  - JSON API improvements
  11.1.2  - Improve atomics
  11.1.1  - Fix zpl_json_write_string providing incorrect length
  11.1.0  - Introduce new ZPL_PICO distro
  11.0.11 - remove stdatomic.h include
  11.0.10 - get rid of c11 atomics lib
  11.0.9  - stringlib uses ZPL_PRINTF_MAXLEN now
          - zpl_printf family is now thread-safe
  11.0.7  - Add ZPL_PRINTF_MAXLEN
  11.0.6  - Fix zpl_printf left padding bug
  11.0.4  - Disable ZPL_NO_MATH_H on TinyC
  11.0.3  - Added support for TinyC compiler
  11.0.2  - Fixes for Apple M1 chip
  11.0.0  - New jobs system
          - Rewrite the timer module
          - zpl_ring rework

  10.13.0 - Initial ARM threading support
  10.12.1 - Fix missing zpL_alloc_str
  10.12.0 - Add zpl_crc64
  10.11.1 - Fix zpl_time_utc_ms on 32-bit OSes
  10.11.0 - Added zpl_file_stream_buf
  10.10.3 - Math type-punning fixes
  10.10.1 - Fix memory writing issue + new write-only in-situ flag
  10.10.0 - Implement memory streaming API
  10.9.1  - Support ARMv6, ARMv7 and ARMv8-a builds
  10.9.0  - Improve the time API
  10.8.3  - zpl_file_close tempfile Windows fixes
  10.8.2  - zpl_file_temp disallow some operations
  10.8.1  - zpl_file_temp Windows fixes
  10.8.0  - Implemented zpl_json_write_string
  10.7.1  - Fix zpl_file_temp platform bug
  10.7.0  - Add zpl_file_write_contents
  10.6.6  - Fix type mismatch in Jobs system
  10.6.0  - Remove event system
  10.5.8  - Remove zpl__memcpy_4byte
  10.5.7  - zpl_file_new is now OS-agnostic constructor
  10.5.6  - Fix coroutine creation
  10.5.5  - Jobs system uses zpl_f32 for priority setting
  10.5.4  - zpl_buffer_free no longer takes the 2nd argument (allocator)
  10.5.3  - Removed crc64 and annotated some hashing methods
  10.5.2  - Don't expose ZPL types anymore
  10.5.1  - Fixed zpl_rdtsc for Emscripten
  10.5.0  - Changed casts to memcopy in random methods, added embed cmd
  10.4.1  - Jobs system now enqueues jobs with def priority of 1.0
  10.4.0  - [META] version bump
  10.3.0  - Pool allocator now supports zpl_free_all
  10.2.0  - [META] version bump
  10.1.0  - Additional math methods (thanks to funZX and msmshazan)
  10.0.15 - WIP Emscripten fixes
  10.0.14 - FreeBSD support
  10.0.13 - OpenBSD support
  10.0.12 - Cygwin fixes
  10.0.11 - Tweak module dependencies
  10.0.10 - Fix zero-allocation regression in filesystem module
  10.0.9 - Fix multi-compilation unit builds
  10.0.8 - Fix zpl_printf "%0d" format specifier
  10.0.4 - Flush tester output to fix ordering
  10.0.3 - Fix ZPL_STATIC_ASSERT under MSVC
  10.0.0 - Major overhaul of the library

  9.8.10 - JSON fix array-based documents with objects
  9.8.9 - JSON document structured as array now properly recognizes the root object as array.
  9.8.8 - Fixed an incorrect parsing of empty array nodes.
  9.8.7 - Improve FreeBSD support
  9.8.6 - WIP: Handle inlined methods properly
  9.8.5 - Fix incorrect usage of EOF and opts dependency on JSON5 module's methods
  9.8.4 - Fix MSVC ZPL_NO_MATH_H code branch using incorrect methods internally
  9.8.3 - Fix MinGW GCC related issue with zpl_printf %lld format
  9.8.2 - Fix VS C4190 issue
  9.8.1 - Fix several C++ type casting quirks
  9.8.0 - Incorporated OpenGL into ZPL core as an optional module
  9.7.0 - Added co-routine module
  9.6.0 - Added process module for creation and manipulation
  9.5.2 - zpl_printf family now prints (null) on NULL string arguments
  9.5.1 - Fixed JSON5 real number export support + indentation fixes
  9.5.0 - Added base64 encode/decode methods
  9.4.10- Small enum style changes
  9.4.9 - Remove #undef for cast and hard_cast (sorry)
  9.4.8 - Fix quote-less JSON node name resolution
  9.4.7 - Additional change to the code
  9.4.6 - Fix issue where zpl_json_find would have false match on substrings
  9.4.5 - Mistakes were made, fixed compilation errors
  9.4.3 - Fix old API shenanigans
  9.4.2 - Fix small API typos
  9.4.1 - Reordered JSON5 constants to integrate better with conditions
  9.4.0 - JSON5 API changes made to zpl_json_find
  9.3.0 - Change how zpl uses basic types internally
  9.2.0 - Directory listing was added. Check dirlist_api.c test for more info
  9.1.1 - Fix WIN32_LEAN_AND_MEAN redefinition properly
  9.1.0 - get_env rework and fixes
  9.0.3 - Small fixes and removals
  9.0.0 - New documentation format, removed deprecated code, changed styles

  8.14.1 - Fix string library
  8.14.0 - Added zpl_re_match_all
  8.13.0 - Update system command API
  8.12.6 - Fix warning in CLI options parser
  8.12.5 - Support parametric options preceding positionals
  8.12.4 - Fixed opts positionals ordering
  8.12.3 - Fixed incorrect handling of flags preceding positionals
  8.12.2 - JSON parsing remark added
  8.12.1 - Fixed a lot of important stuff
  8.12.0 - Added helper constructors for containers
  8.11.2 - Fix bug in opts module
  8.11.1 - Small code improvements
  8.11.0 - Ported regex processor from https://github.com/gingerBill/gb/ and applied fixes on top of it
  8.10.2 - Fix zpl_strtok
  8.10.1 - Replace zpl_strchr by zpl_char_last_occurence
  8.10.0 - Added zpl_strchr
  8.9.0  - API improvements for JSON5 parser
  8.8.4  - Add support for SJSON formatting http://bitsquid.blogspot.com/2009/10/simplified-json-notation.html

  6.8.3  - JSON5 exp fix
  6.8.2  - Bugfixes applied from gb
  6.8.1  - Performance improvements for JSON5 parser
  6.8.0  - zpl.h is now generated by build.py
  6.7.0  - Several fixes and added switches
  6.6.0  - Several significant changes made to the repository
  6.5.0  - Ported platform layer from https://github.com/gingerBill/gb/
  6.4.1  - Use zpl_strlen in zpl_strdup
  6.4.0  - Deprecated zpl_buffer_free and added zpl_array_end, zpl_buffer_end
  6.3.0  - Added zpl_strdup
  6.2.1  - Remove math redundancies
  6.2.0  - Integrated zpl_math.h into zpl.h
  6.1.1  - Added direct.h include for win c++ dir methods
  6.1.0  - Added zpl_path_mkdir, zpl_path_rmdir, and few new zplFileErrors
  6.0.4  - More MSVC(++) satisfaction by fixing warnings
  6.0.3  - Satisfy MSVC by fixing a warning
  6.0.2  - Fixed warnings for json5 i64 printfs
  6.0.1  - Fixed warnings for particual win compiler in dirlist method
  6.0.0  - New build, include/ was renamed to code/

  5.8.3  - Naming fixes
  5.8.2  - Job system now supports prioritized tasks
  5.8.1  - Renames zpl_pad to zpl_ring
  5.8.0  - Added instantiated scratch pad (circular buffer)
  5.7.2  - Added Windows support for zpl_path_dirlist
  5.7.1  - Fixed few things in job system + macOS support for zpl_path_dirlist
  5.7.0  - Added a job system (zpl_thread_pool)
  5.6.5  - Fixes extra error cases for zpl_opts when input is:
          - missing a value for an option,
          - having an extra value for a flag (e.g. --enable-log shouldn't get a value.)
  5.6.4  - Several tweaks to the zpl_opts API
  5.6.3  - Added support for flags without values
  5.6.2  - Improve error handling for zpl_opts
  5.6.1  - Added support for strings with spaces in zpl_opts
  5.6.0  - Added zpl_opts for CLI argument parsing
  5.5.1  - Fixed time method for win
  5.5.0  - Integrate JSON5 writer into the core
  5.4.0  - Improved storage support for numbers in JSON5 parser
  5.3.0  - Integrated zpl_json into ZPL
  5.2.0  - Added zpl_string_sprintf
  5.1.1  - Added zpl_system_command_nores for output-less execution
  5.1.0  - Added event handler
  5.0.4  - Fix alias for zpl_list
  5.0.3  - Finalizing syntax changes
  5.0.2  - Fix segfault when using zpl_stack_memory
  5.0.1  - Small code improvements
  5.0.0  - Project structure changes

  4.7.2  - Got rid of size arg for zpl_str_split_lines
  4.7.1  - Added an example
  4.7.0  - Added zpl_path_dirlist
  4.6.1  - zpl_memcopy x86 patch from upstream
  4.6.0  - Added few string-related functions
  4.5.9  - Error fixes
  4.5.8  - Warning fixes
  4.5.7  - Fixed timer loops. zpl_time* related functions work with seconds now
  4.5.6  - Fixed zpl_time_now() for Windows and Linux
  4.5.5  - Small cosmetic changes
  4.5.4  - Fixed issue when zpl_list_add would break the links
          - when adding a new item between nodes
  4.5.3  - Fixed malformed enum values
  4.5.1  - Fixed some warnings
  4.5.0  - Added zpl_array_append_at
  4.4.0  - Added zpl_array_back, zpl_array_front
  4.3.0  - Added zpl_list
  4.2.0  - Added zpl_system_command_str
  4.1.2  - GG, fixed small compilation error
  4.1.1  - Fixed possible security issue in zpl_system_command
  4.1.0  - Added zpl_string_make_reserve and small fixes
  4.0.2  - Warning fix for _LARGEFILE64_SOURCE
  4.0.1  - include stdlib.h for getenv (temp)
  4.0.0  - ARM support, coding style changes and various improvements

  3.4.1  - zpl_memcopy now uses memcpy for ARM arch-family
  3.4.0  - Removed obsolete code
  3.3.4  - Added Travis CI config
  3.3.3  - Small macro formatting changes + ZPL_SYSTEM_IOS
  3.3.2  - Fixes for android arm
  3.3.1  - Fixed some type cast warnings
  3.3.0  - Added Android support
  3.1.5  - Renamed userptr to user_data in timer
  3.1.4  - Fix for zpl_buffer not allocating correctly
  3.1.2  - Small fix in zpl_memcompare
  3.1.1  - Added char* conversion for data field in zpl_array_header
  3.1.0  - Added data field to zpl_array_header
  3.0.7  - Added timer userptr as argument to callback
  3.0.6  - Small changes
  3.0.5  - Fixed compilation for emscripten
  3.0.4  - Small fixes for tiny cpp warnings
  3.0.3  - Small fixes for various cpp warnings and errors
  3.0.2  - Fixed linux part, and removed trailing spaces
  3.0.1  - Small bugfix in zpl_file_open
  3.0.0  - Added several fixes and features

  2.4.0  - Added remove to hash table
  2.3.3  - Removed redundant code
  2.3.2  - Eliminated extra warnings
  2.3.1  - Warning hunt
  2.3.0  - Added the ability to copy array/buffer and fixed bug in hash table.
  2.2.1  - Used tmpfile() for Windows
  2.2.0  - Added zpl_file_temp
  2.1.1  - Very small fix (forgive me)
  2.1.0  - Added the ability to resize bitstream
  2.0.8  - Small adjustments
  2.0.7  - MinGW related fixes
  2.0.0  - New NPM based version

  1.2.2  - Small fix
  1.2.1  - Macro fixes
  1.2.0  - Added zpl_async macro
  1.1.0  - Added timer feature
  1.0.0  - Initial version


License:
  This Software is dual licensed under the following licenses:

  Unlicense
  This is free and unencumbered software released into the public domain.

  Anyone is free to copy, modify, publish, use, compile, sell, or
  distribute this software, either in source code form or as a compiled
  binary, for any purpose, commercial or non-commercial, and by any
  means.

  In jurisdictions that recognize copyright laws, the author or authors
  of this software dedicate any and all copyright interest in the
  software to the public domain. We make this dedication for the benefit
  of the public at large and to the detriment of our heirs and
  successors. We intend this dedication to be an overt act of
  relinquishment in perpetuity of all present and future rights to this
  software under copyright law.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
  OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
  OTHER DEALINGS IN THE SOFTWARE.

  For more information, please refer to <http://unlicense.org/>

  BSD 3-Clause
  Copyright (c) 2016-2021 Dominik Madarász. All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
  3. Neither the name of the copyright holder nor the names of its contributors
  may be used to endorse or promote products derived from this software without
  specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef ZPL_H
#	define ZPL_H

#	define ZPL_VERSION_MAJOR 19
#	define ZPL_VERSION_MINOR 0
#	define ZPL_VERSION_PATCH 0
#	define ZPL_VERSION_PRE   ""

// file: zpl_hedley.h

/* Hedley - https://nemequ.github.io/hedley
 * Created by Evan Nemerson <evan@nemerson.com>
 *
 * To the extent possible under law, the author(s) have dedicated all
 * copyright and related and neighboring rights to this software to
 * the public domain worldwide. This software is distributed without
 * any warranty.
 *
 * For details, see <http://creativecommons.org/publicdomain/zero/1.0/>.
 * SPDX-License-Identifier: CC0-1.0
 */

#if ! defined( ZPL_HEDLEY_VERSION ) || ( ZPL_HEDLEY_VERSION < 12 )
#	if defined( ZPL_HEDLEY_VERSION )
#		undef ZPL_HEDLEY_VERSION
#	endif
#	define ZPL_HEDLEY_VERSION 12

#	if defined( ZPL_STRINGIFY_EX )
#		undef ZPL_STRINGIFY_EX
#	endif
#	define ZPL_STRINGIFY_EX( x ) #x

#	if defined( ZPL_STRINGIFY )
#		undef ZPL_STRINGIFY
#	endif
#	define ZPL_STRINGIFY( x ) ZPL_STRINGIFY_EX( x )

#	if defined( ZPL_CONCAT_EX )
#		undef ZPL_CONCAT_EX
#	endif
#	define ZPL_CONCAT_EX( a, b ) a##b

#	if defined( ZPL_CONCAT )
#		undef ZPL_CONCAT
#	endif
#	define ZPL_CONCAT( a, b ) ZPL_CONCAT_EX( a, b )

#	if defined( ZPL_VERSION_ENCODE )
#		undef ZPL_VERSION_ENCODE
#	endif
#	define ZPL_VERSION_ENCODE( major, minor, patch ) ( ( ( major )*1000000 ) + ( ( minor )*1000 ) + ( patch ) )

#	if defined( ZPL_VERSION_DECODE_MAJOR )
#		undef ZPL_VERSION_DECODE_MAJOR
#	endif
#	define ZPL_VERSION_DECODE_MAJOR( version ) ( ( version ) / 1000000 )

#	if defined( ZPL_VERSION_DECODE_MINOR )
#		undef ZPL_VERSION_DECODE_MINOR
#	endif
#	define ZPL_VERSION_DECODE_MINOR( version ) ( ( ( version ) % 1000000 ) / 1000 )

#	if defined( ZPL_VERSION_DECODE_PATCH )
#		undef ZPL_VERSION_DECODE_PATCH
#	endif
#	define ZPL_VERSION_DECODE_PATCH( version ) ( ( version ) % 1000 )

#	if defined( ZPL_VERSION_CHECK )
#		undef ZPL_VERSION_CHECK
#	endif
#	define ZPL_VERSION_CHECK( major, minor, patch ) ( ZPL_VERSION_ENCODE( major, minor, patch ) <= ZPL_VERSION )

#	if defined( ZPL_GNUC_VERSION )
#		undef ZPL_GNUC_VERSION
#	endif
#	if defined( __GNUC__ ) && defined( __GNUC_PATCHLEVEL__ )
#		define ZPL_GNUC_VERSION ZPL_VERSION_ENCODE( __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__ )
#	elif defined( __GNUC__ )
#		define ZPL_GNUC_VERSION ZPL_VERSION_ENCODE( __GNUC__, __GNUC_MINOR__, 0 )
#	endif

#	if defined( ZPL_GNUC_VERSION_CHECK )
#		undef ZPL_GNUC_VERSION_CHECK
#	endif
#	if defined( ZPL_GNUC_VERSION )
#		define ZPL_GNUC_VERSION_CHECK( major, minor, patch ) ( ZPL_GNUC_VERSION >= ZPL_VERSION_ENCODE( major, minor, patch ) )
#	else
#		define ZPL_GNUC_VERSION_CHECK( major, minor, patch ) ( 0 )
#	endif

#	if defined( ZPL_MSVC_VERSION )
#		undef ZPL_MSVC_VERSION
#	endif
#	if defined( _MSC_FULL_VER ) && ( _MSC_FULL_VER >= 140000000 )
#		define ZPL_MSVC_VERSION ZPL_VERSION_ENCODE( _MSC_FULL_VER / 10000000, ( _MSC_FULL_VER % 10000000 ) / 100000, ( _MSC_FULL_VER % 100000 ) / 100 )
#	elif defined( _MSC_FULL_VER )
#		define ZPL_MSVC_VERSION ZPL_VERSION_ENCODE( _MSC_FULL_VER / 1000000, ( _MSC_FULL_VER % 1000000 ) / 10000, ( _MSC_FULL_VER % 10000 ) / 10 )
#	elif defined( _MSC_VER )
#		define ZPL_MSVC_VERSION ZPL_VERSION_ENCODE( _MSC_VER / 100, _MSC_VER % 100, 0 )
#	endif

#	if defined( ZPL_MSVC_VERSION_CHECK )
#		undef ZPL_MSVC_VERSION_CHECK
#	endif
#	if ! defined( _MSC_VER )
#		define ZPL_MSVC_VERSION_CHECK( major, minor, patch ) ( 0 )
#	elif defined( _MSC_VER ) && ( _MSC_VER >= 1400 )
#		define ZPL_MSVC_VERSION_CHECK( major, minor, patch ) ( _MSC_FULL_VER >= ( ( major * 10000000 ) + ( minor * 100000 ) + ( patch ) ) )
#	elif defined( _MSC_VER ) && ( _MSC_VER >= 1200 )
#		define ZPL_MSVC_VERSION_CHECK( major, minor, patch ) ( _MSC_FULL_VER >= ( ( major * 1000000 ) + ( minor * 10000 ) + ( patch ) ) )
#	else
#		define ZPL_MSVC_VERSION_CHECK( major, minor, patch ) ( _MSC_VER >= ( ( major * 100 ) + ( minor ) ) )
#	endif

#	if defined( ZPL_INTEL_VERSION )
#		undef ZPL_INTEL_VERSION
#	endif
#	if defined( __INTEL_COMPILER ) && defined( __INTEL_COMPILER_UPDATE )
#		define ZPL_INTEL_VERSION ZPL_VERSION_ENCODE( __INTEL_COMPILER / 100, __INTEL_COMPILER % 100, __INTEL_COMPILER_UPDATE )
#	elif defined( __INTEL_COMPILER )
#		define ZPL_INTEL_VERSION ZPL_VERSION_ENCODE( __INTEL_COMPILER / 100, __INTEL_COMPILER % 100, 0 )
#	endif

#	if defined( ZPL_INTEL_VERSION_CHECK )
#		undef ZPL_INTEL_VERSION_CHECK
#	endif
#	if defined( ZPL_INTEL_VERSION )
#		define ZPL_INTEL_VERSION_CHECK( major, minor, patch ) ( ZPL_INTEL_VERSION >= ZPL_VERSION_ENCODE( major, minor, patch ) )
#	else
#		define ZPL_INTEL_VERSION_CHECK( major, minor, patch ) ( 0 )
#	endif

#	if defined( ZPL_PGI_VERSION )
#		undef ZPL_PGI_VERSION
#	endif
#	if defined( __PGI ) && defined( __PGIC__ ) && defined( __PGIC_MINOR__ ) && defined( __PGIC_PATCHLEVEL__ )
#		define ZPL_PGI_VERSION ZPL_VERSION_ENCODE( __PGIC__, __PGIC_MINOR__, __PGIC_PATCHLEVEL__ )
#	endif

#	if defined( ZPL_PGI_VERSION_CHECK )
#		undef ZPL_PGI_VERSION_CHECK
#	endif
#	if defined( ZPL_PGI_VERSION )
#		define ZPL_PGI_VERSION_CHECK( major, minor, patch ) ( ZPL_PGI_VERSION >= ZPL_VERSION_ENCODE( major, minor, patch ) )
#	else
#		define ZPL_PGI_VERSION_CHECK( major, minor, patch ) ( 0 )
#	endif

#	if defined( ZPL_SUNPRO_VERSION )
#		undef ZPL_SUNPRO_VERSION
#	endif
#	if defined( __SUNPRO_C ) && ( __SUNPRO_C > 0x1000 )
#		define ZPL_SUNPRO_VERSION                                                        \
			ZPL_VERSION_ENCODE(                                                           \
			    ( ( ( __SUNPRO_C >> 16 ) & 0xf ) * 10 ) + ( ( __SUNPRO_C >> 12 ) & 0xf ), \
			    ( ( ( __SUNPRO_C >> 8 ) & 0xf ) * 10 ) + ( ( __SUNPRO_C >> 4 ) & 0xf ),   \
			    ( __SUNPRO_C & 0xf ) * 10                                                 \
			)
#	elif defined( __SUNPRO_C )
#		define ZPL_SUNPRO_VERSION ZPL_VERSION_ENCODE( ( __SUNPRO_C >> 8 ) & 0xf, ( __SUNPRO_C >> 4 ) & 0xf, ( __SUNPRO_C )&0xf )
#	elif defined( __SUNPRO_CC ) && ( __SUNPRO_CC > 0x1000 )
#		define ZPL_SUNPRO_VERSION                                                          \
			ZPL_VERSION_ENCODE(                                                             \
			    ( ( ( __SUNPRO_CC >> 16 ) & 0xf ) * 10 ) + ( ( __SUNPRO_CC >> 12 ) & 0xf ), \
			    ( ( ( __SUNPRO_CC >> 8 ) & 0xf ) * 10 ) + ( ( __SUNPRO_CC >> 4 ) & 0xf ),   \
			    ( __SUNPRO_CC & 0xf ) * 10                                                  \
			)
#	elif defined( __SUNPRO_CC )
#		define ZPL_SUNPRO_VERSION ZPL_VERSION_ENCODE( ( __SUNPRO_CC >> 8 ) & 0xf, ( __SUNPRO_CC >> 4 ) & 0xf, ( __SUNPRO_CC )&0xf )
#	endif

#	if defined( ZPL_SUNPRO_VERSION_CHECK )
#		undef ZPL_SUNPRO_VERSION_CHECK
#	endif
#	if defined( ZPL_SUNPRO_VERSION )
#		define ZPL_SUNPRO_VERSION_CHECK( major, minor, patch ) ( ZPL_SUNPRO_VERSION >= ZPL_VERSION_ENCODE( major, minor, patch ) )
#	else
#		define ZPL_SUNPRO_VERSION_CHECK( major, minor, patch ) ( 0 )
#	endif

#	if defined( ZPL_EMSCRIPTEN_VERSION )
#		undef ZPL_EMSCRIPTEN_VERSION
#	endif
#	if defined( __EMSCRIPTEN__ )
#		define ZPL_EMSCRIPTEN_VERSION ZPL_VERSION_ENCODE( __EMSCRIPTEN_major__, __EMSCRIPTEN_minor__, __EMSCRIPTEN_tiny__ )
#	endif

#	if defined( ZPL_EMSCRIPTEN_VERSION_CHECK )
#		undef ZPL_EMSCRIPTEN_VERSION_CHECK
#	endif
#	if defined( ZPL_EMSCRIPTEN_VERSION )
#		define ZPL_EMSCRIPTEN_VERSION_CHECK( major, minor, patch ) ( ZPL_EMSCRIPTEN_VERSION >= ZPL_VERSION_ENCODE( major, minor, patch ) )
#	else
#		define ZPL_EMSCRIPTEN_VERSION_CHECK( major, minor, patch ) ( 0 )
#	endif

#	if defined( ZPL_ARM_VERSION )
#		undef ZPL_ARM_VERSION
#	endif
#	if defined( __CC_ARM ) && defined( __ARMCOMPILER_VERSION )
#		define ZPL_ARM_VERSION ZPL_VERSION_ENCODE( __ARMCOMPILER_VERSION / 1000000, ( __ARMCOMPILER_VERSION % 1000000 ) / 10000, ( __ARMCOMPILER_VERSION % 10000 ) / 100 )
#	elif defined( __CC_ARM ) && defined( __ARMCC_VERSION )
#		define ZPL_ARM_VERSION ZPL_VERSION_ENCODE( __ARMCC_VERSION / 1000000, ( __ARMCC_VERSION % 1000000 ) / 10000, ( __ARMCC_VERSION % 10000 ) / 100 )
#	endif

#	if defined( ZPL_ARM_VERSION_CHECK )
#		undef ZPL_ARM_VERSION_CHECK
#	endif
#	if defined( ZPL_ARM_VERSION )
#		define ZPL_ARM_VERSION_CHECK( major, minor, patch ) ( ZPL_ARM_VERSION >= ZPL_VERSION_ENCODE( major, minor, patch ) )
#	else
#		define ZPL_ARM_VERSION_CHECK( major, minor, patch ) ( 0 )
#	endif

#	if defined( ZPL_IBM_VERSION )
#		undef ZPL_IBM_VERSION
#	endif
#	if defined( __ibmxl__ )
#		define ZPL_IBM_VERSION ZPL_VERSION_ENCODE( __ibmxl_version__, __ibmxl_release__, __ibmxl_modification__ )
#	elif defined( __xlC__ ) && defined( __xlC_ver__ )
#		define ZPL_IBM_VERSION ZPL_VERSION_ENCODE( __xlC__ >> 8, __xlC__ & 0xff, ( __xlC_ver__ >> 8 ) & 0xff )
#	elif defined( __xlC__ )
#		define ZPL_IBM_VERSION ZPL_VERSION_ENCODE( __xlC__ >> 8, __xlC__ & 0xff, 0 )
#	endif

#	if defined( ZPL_IBM_VERSION_CHECK )
#		undef ZPL_IBM_VERSION_CHECK
#	endif
#	if defined( ZPL_IBM_VERSION )
#		define ZPL_IBM_VERSION_CHECK( major, minor, patch ) ( ZPL_IBM_VERSION >= ZPL_VERSION_ENCODE( major, minor, patch ) )
#	else
#		define ZPL_IBM_VERSION_CHECK( major, minor, patch ) ( 0 )
#	endif

#	if defined( ZPL_TI_VERSION )
#		undef ZPL_TI_VERSION
#	endif
#	if defined( __TI_COMPILER_VERSION__ ) && ( defined( __TMS470__ ) || defined( __TI_ARM__ ) || defined( __MSP430__ ) || defined( __TMS320C2000__ ) )
#		if ( __TI_COMPILER_VERSION__ >= 16000000 )
#			define ZPL_TI_VERSION ZPL_VERSION_ENCODE( __TI_COMPILER_VERSION__ / 1000000, ( __TI_COMPILER_VERSION__ % 1000000 ) / 1000, ( __TI_COMPILER_VERSION__ % 1000 ) )
#		endif
#	endif

#	if defined( ZPL_TI_VERSION_CHECK )
#		undef ZPL_TI_VERSION_CHECK
#	endif
#	if defined( ZPL_TI_VERSION )
#		define ZPL_TI_VERSION_CHECK( major, minor, patch ) ( ZPL_TI_VERSION >= ZPL_VERSION_ENCODE( major, minor, patch ) )
#	else
#		define ZPL_TI_VERSION_CHECK( major, minor, patch ) ( 0 )
#	endif

#	if defined( ZPL_TI_CL2000_VERSION )
#		undef ZPL_TI_CL2000_VERSION
#	endif
#	if defined( __TI_COMPILER_VERSION__ ) && defined( __TMS320C2000__ )
#		define ZPL_TI_CL2000_VERSION ZPL_VERSION_ENCODE( __TI_COMPILER_VERSION__ / 1000000, ( __TI_COMPILER_VERSION__ % 1000000 ) / 1000, ( __TI_COMPILER_VERSION__ % 1000 ) )
#	endif

#	if defined( ZPL_TI_CL2000_VERSION_CHECK )
#		undef ZPL_TI_CL2000_VERSION_CHECK
#	endif
#	if defined( ZPL_TI_CL2000_VERSION )
#		define ZPL_TI_CL2000_VERSION_CHECK( major, minor, patch ) ( ZPL_TI_CL2000_VERSION >= ZPL_VERSION_ENCODE( major, minor, patch ) )
#	else
#		define ZPL_TI_CL2000_VERSION_CHECK( major, minor, patch ) ( 0 )
#	endif

#	if defined( ZPL_TI_CL430_VERSION )
#		undef ZPL_TI_CL430_VERSION
#	endif
#	if defined( __TI_COMPILER_VERSION__ ) && defined( __MSP430__ )
#		define ZPL_TI_CL430_VERSION ZPL_VERSION_ENCODE( __TI_COMPILER_VERSION__ / 1000000, ( __TI_COMPILER_VERSION__ % 1000000 ) / 1000, ( __TI_COMPILER_VERSION__ % 1000 ) )
#	endif

#	if defined( ZPL_TI_CL430_VERSION_CHECK )
#		undef ZPL_TI_CL430_VERSION_CHECK
#	endif
#	if defined( ZPL_TI_CL430_VERSION )
#		define ZPL_TI_CL430_VERSION_CHECK( major, minor, patch ) ( ZPL_TI_CL430_VERSION >= ZPL_VERSION_ENCODE( major, minor, patch ) )
#	else
#		define ZPL_TI_CL430_VERSION_CHECK( major, minor, patch ) ( 0 )
#	endif

#	if defined( ZPL_TI_ARMCL_VERSION )
#		undef ZPL_TI_ARMCL_VERSION
#	endif
#	if defined( __TI_COMPILER_VERSION__ ) && ( defined( __TMS470__ ) || defined( __TI_ARM__ ) )
#		define ZPL_TI_ARMCL_VERSION ZPL_VERSION_ENCODE( __TI_COMPILER_VERSION__ / 1000000, ( __TI_COMPILER_VERSION__ % 1000000 ) / 1000, ( __TI_COMPILER_VERSION__ % 1000 ) )
#	endif

#	if defined( ZPL_TI_ARMCL_VERSION_CHECK )
#		undef ZPL_TI_ARMCL_VERSION_CHECK
#	endif
#	if defined( ZPL_TI_ARMCL_VERSION )
#		define ZPL_TI_ARMCL_VERSION_CHECK( major, minor, patch ) ( ZPL_TI_ARMCL_VERSION >= ZPL_VERSION_ENCODE( major, minor, patch ) )
#	else
#		define ZPL_TI_ARMCL_VERSION_CHECK( major, minor, patch ) ( 0 )
#	endif

#	if defined( ZPL_TI_CL6X_VERSION )
#		undef ZPL_TI_CL6X_VERSION
#	endif
#	if defined( __TI_COMPILER_VERSION__ ) && defined( __TMS320C6X__ )
#		define ZPL_TI_CL6X_VERSION ZPL_VERSION_ENCODE( __TI_COMPILER_VERSION__ / 1000000, ( __TI_COMPILER_VERSION__ % 1000000 ) / 1000, ( __TI_COMPILER_VERSION__ % 1000 ) )
#	endif

#	if defined( ZPL_TI_CL6X_VERSION_CHECK )
#		undef ZPL_TI_CL6X_VERSION_CHECK
#	endif
#	if defined( ZPL_TI_CL6X_VERSION )
#		define ZPL_TI_CL6X_VERSION_CHECK( major, minor, patch ) ( ZPL_TI_CL6X_VERSION >= ZPL_VERSION_ENCODE( major, minor, patch ) )
#	else
#		define ZPL_TI_CL6X_VERSION_CHECK( major, minor, patch ) ( 0 )
#	endif

#	if defined( ZPL_TI_CL7X_VERSION )
#		undef ZPL_TI_CL7X_VERSION
#	endif
#	if defined( __TI_COMPILER_VERSION__ ) && defined( __C7000__ )
#		define ZPL_TI_CL7X_VERSION ZPL_VERSION_ENCODE( __TI_COMPILER_VERSION__ / 1000000, ( __TI_COMPILER_VERSION__ % 1000000 ) / 1000, ( __TI_COMPILER_VERSION__ % 1000 ) )
#	endif

#	if defined( ZPL_TI_CL7X_VERSION_CHECK )
#		undef ZPL_TI_CL7X_VERSION_CHECK
#	endif
#	if defined( ZPL_TI_CL7X_VERSION )
#		define ZPL_TI_CL7X_VERSION_CHECK( major, minor, patch ) ( ZPL_TI_CL7X_VERSION >= ZPL_VERSION_ENCODE( major, minor, patch ) )
#	else
#		define ZPL_TI_CL7X_VERSION_CHECK( major, minor, patch ) ( 0 )
#	endif

#	if defined( ZPL_TI_CLPRU_VERSION )
#		undef ZPL_TI_CLPRU_VERSION
#	endif
#	if defined( __TI_COMPILER_VERSION__ ) && defined( __PRU__ )
#		define ZPL_TI_CLPRU_VERSION ZPL_VERSION_ENCODE( __TI_COMPILER_VERSION__ / 1000000, ( __TI_COMPILER_VERSION__ % 1000000 ) / 1000, ( __TI_COMPILER_VERSION__ % 1000 ) )
#	endif

#	if defined( ZPL_TI_CLPRU_VERSION_CHECK )
#		undef ZPL_TI_CLPRU_VERSION_CHECK
#	endif
#	if defined( ZPL_TI_CLPRU_VERSION )
#		define ZPL_TI_CLPRU_VERSION_CHECK( major, minor, patch ) ( ZPL_TI_CLPRU_VERSION >= ZPL_VERSION_ENCODE( major, minor, patch ) )
#	else
#		define ZPL_TI_CLPRU_VERSION_CHECK( major, minor, patch ) ( 0 )
#	endif

#	if defined( ZPL_CRAY_VERSION )
#		undef ZPL_CRAY_VERSION
#	endif
#	if defined( _CRAYC )
#		if defined( _RELEASE_PATCHLEVEL )
#			define ZPL_CRAY_VERSION ZPL_VERSION_ENCODE( _RELEASE_MAJOR, _RELEASE_MINOR, _RELEASE_PATCHLEVEL )
#		else
#			define ZPL_CRAY_VERSION ZPL_VERSION_ENCODE( _RELEASE_MAJOR, _RELEASE_MINOR, 0 )
#		endif
#	endif

#	if defined( ZPL_CRAY_VERSION_CHECK )
#		undef ZPL_CRAY_VERSION_CHECK
#	endif
#	if defined( ZPL_CRAY_VERSION )
#		define ZPL_CRAY_VERSION_CHECK( major, minor, patch ) ( ZPL_CRAY_VERSION >= ZPL_VERSION_ENCODE( major, minor, patch ) )
#	else
#		define ZPL_CRAY_VERSION_CHECK( major, minor, patch ) ( 0 )
#	endif

#	if defined( ZPL_IAR_VERSION )
#		undef ZPL_IAR_VERSION
#	endif
#	if defined( __IAR_SYSTEMS_ICC__ )
#		if __VER__ > 1000
#			define ZPL_IAR_VERSION ZPL_VERSION_ENCODE( ( __VER__ / 1000000 ), ( ( __VER__ / 1000 ) % 1000 ), ( __VER__ % 1000 ) )
#		else
#			define ZPL_IAR_VERSION ZPL_VERSION_ENCODE( VER / 100, __VER__ % 100, 0 )
#		endif
#	endif

#	if defined( ZPL_IAR_VERSION_CHECK )
#		undef ZPL_IAR_VERSION_CHECK
#	endif
#	if defined( ZPL_IAR_VERSION )
#		define ZPL_IAR_VERSION_CHECK( major, minor, patch ) ( ZPL_IAR_VERSION >= ZPL_VERSION_ENCODE( major, minor, patch ) )
#	else
#		define ZPL_IAR_VERSION_CHECK( major, minor, patch ) ( 0 )
#	endif

#	if defined( ZPL_TINYC_VERSION )
#		undef ZPL_TINYC_VERSION
#	endif
#	if defined( __TINYC__ )
#		define ZPL_TINYC_VERSION ZPL_VERSION_ENCODE( __TINYC__ / 1000, ( __TINYC__ / 100 ) % 10, __TINYC__ % 100 )
#	endif

#	if defined( ZPL_TINYC_VERSION_CHECK )
#		undef ZPL_TINYC_VERSION_CHECK
#	endif
#	if defined( ZPL_TINYC_VERSION )
#		define ZPL_TINYC_VERSION_CHECK( major, minor, patch ) ( ZPL_TINYC_VERSION >= ZPL_VERSION_ENCODE( major, minor, patch ) )
#	else
#		define ZPL_TINYC_VERSION_CHECK( major, minor, patch ) ( 0 )
#	endif

#	if defined( ZPL_DMC_VERSION )
#		undef ZPL_DMC_VERSION
#	endif
#	if defined( __DMC__ )
#		define ZPL_DMC_VERSION ZPL_VERSION_ENCODE( __DMC__ >> 8, ( __DMC__ >> 4 ) & 0xf, __DMC__ & 0xf )
#	endif

#	if defined( ZPL_DMC_VERSION_CHECK )
#		undef ZPL_DMC_VERSION_CHECK
#	endif
#	if defined( ZPL_DMC_VERSION )
#		define ZPL_DMC_VERSION_CHECK( major, minor, patch ) ( ZPL_DMC_VERSION >= ZPL_VERSION_ENCODE( major, minor, patch ) )
#	else
#		define ZPL_DMC_VERSION_CHECK( major, minor, patch ) ( 0 )
#	endif

#	if defined( ZPL_COMPCERT_VERSION )
#		undef ZPL_COMPCERT_VERSION
#	endif
#	if defined( __COMPCERT_VERSION__ )
#		define ZPL_COMPCERT_VERSION ZPL_VERSION_ENCODE( __COMPCERT_VERSION__ / 10000, ( __COMPCERT_VERSION__ / 100 ) % 100, __COMPCERT_VERSION__ % 100 )
#	endif

#	if defined( ZPL_COMPCERT_VERSION_CHECK )
#		undef ZPL_COMPCERT_VERSION_CHECK
#	endif
#	if defined( ZPL_COMPCERT_VERSION )
#		define ZPL_COMPCERT_VERSION_CHECK( major, minor, patch ) ( ZPL_COMPCERT_VERSION >= ZPL_VERSION_ENCODE( major, minor, patch ) )
#	else
#		define ZPL_COMPCERT_VERSION_CHECK( major, minor, patch ) ( 0 )
#	endif

#	if defined( ZPL_PELLES_VERSION )
#		undef ZPL_PELLES_VERSION
#	endif
#	if defined( __POCC__ )
#		define ZPL_PELLES_VERSION ZPL_VERSION_ENCODE( __POCC__ / 100, __POCC__ % 100, 0 )
#	endif

#	if defined( ZPL_PELLES_VERSION_CHECK )
#		undef ZPL_PELLES_VERSION_CHECK
#	endif
#	if defined( ZPL_PELLES_VERSION )
#		define ZPL_PELLES_VERSION_CHECK( major, minor, patch ) ( ZPL_PELLES_VERSION >= ZPL_VERSION_ENCODE( major, minor, patch ) )
#	else
#		define ZPL_PELLES_VERSION_CHECK( major, minor, patch ) ( 0 )
#	endif

#	if defined( ZPL_GCC_VERSION )
#		undef ZPL_GCC_VERSION
#	endif
#	if defined( ZPL_GNUC_VERSION ) && ! defined( __clang__ ) && ! defined( ZPL_INTEL_VERSION ) && ! defined( ZPL_PGI_VERSION ) && ! defined( ZPL_ARM_VERSION ) \
	    && ! defined( ZPL_TI_VERSION ) && ! defined( ZPL_TI_ARMCL_VERSION ) && ! defined( ZPL_TI_CL430_VERSION ) && ! defined( ZPL_TI_CL2000_VERSION )          \
	    && ! defined( ZPL_TI_CL6X_VERSION ) && ! defined( ZPL_TI_CL7X_VERSION ) && ! defined( ZPL_TI_CLPRU_VERSION ) && ! defined( __COMPCERT__ )
#		define ZPL_GCC_VERSION ZPL_GNUC_VERSION
#	endif

#	if defined( ZPL_GCC_VERSION_CHECK )
#		undef ZPL_GCC_VERSION_CHECK
#	endif
#	if defined( ZPL_GCC_VERSION )
#		define ZPL_GCC_VERSION_CHECK( major, minor, patch ) ( ZPL_GCC_VERSION >= ZPL_VERSION_ENCODE( major, minor, patch ) )
#	else
#		define ZPL_GCC_VERSION_CHECK( major, minor, patch ) ( 0 )
#	endif

#	if defined( ZPL_HAS_ATTRIBUTE )
#		undef ZPL_HAS_ATTRIBUTE
#	endif
#	if defined( __has_attribute )
#		define ZPL_HAS_ATTRIBUTE( attribute ) __has_attribute( attribute )
#	else
#		define ZPL_HAS_ATTRIBUTE( attribute ) ( 0 )
#	endif

#	if defined( ZPL_GNUC_HAS_ATTRIBUTE )
#		undef ZPL_GNUC_HAS_ATTRIBUTE
#	endif
#	if defined( __has_attribute )
#		define ZPL_GNUC_HAS_ATTRIBUTE( attribute, major, minor, patch ) __has_attribute( attribute )
#	else
#		define ZPL_GNUC_HAS_ATTRIBUTE( attribute, major, minor, patch ) ZPL_GNUC_VERSION_CHECK( major, minor, patch )
#	endif

#	if defined( ZPL_GCC_HAS_ATTRIBUTE )
#		undef ZPL_GCC_HAS_ATTRIBUTE
#	endif
#	if defined( __has_attribute )
#		define ZPL_GCC_HAS_ATTRIBUTE( attribute, major, minor, patch ) __has_attribute( attribute )
#	else
#		define ZPL_GCC_HAS_ATTRIBUTE( attribute, major, minor, patch ) ZPL_GCC_VERSION_CHECK( major, minor, patch )
#	endif

#	if defined( ZPL_HAS_CPP_ATTRIBUTE )
#		undef ZPL_HAS_CPP_ATTRIBUTE
#	endif
#	if defined( __has_cpp_attribute ) && defined( __cplusplus ) && ( ! defined( ZPL_SUNPRO_VERSION ) || ZPL_SUNPRO_VERSION_CHECK( 5, 15, 0 ) )
#		define ZPL_HAS_CPP_ATTRIBUTE( attribute ) __has_cpp_attribute( attribute )
#	else
#		define ZPL_HAS_CPP_ATTRIBUTE( attribute ) ( 0 )
#	endif

#	if defined( ZPL_HAS_CPP_ATTRIBUTE_NS )
#		undef ZPL_HAS_CPP_ATTRIBUTE_NS
#	endif
#	if ! defined( __cplusplus ) || ! defined( __has_cpp_attribute )
#		define ZPL_HAS_CPP_ATTRIBUTE_NS( ns, attribute ) ( 0 )
#	elif ! defined( ZPL_PGI_VERSION ) && ! defined( ZPL_IAR_VERSION ) && ( ! defined( ZPL_SUNPRO_VERSION ) || ZPL_SUNPRO_VERSION_CHECK( 5, 15, 0 ) ) \
	    && ( ! defined( ZPL_MSVC_VERSION ) || ZPL_MSVC_VERSION_CHECK( 19, 20, 0 ) )
#		define ZPL_HAS_CPP_ATTRIBUTE_NS( ns, attribute ) ZPL_HAS_CPP_ATTRIBUTE( ns::attribute )
#	else
#		define ZPL_HAS_CPP_ATTRIBUTE_NS( ns, attribute ) ( 0 )
#	endif

#	if defined( ZPL_GNUC_HAS_CPP_ATTRIBUTE )
#		undef ZPL_GNUC_HAS_CPP_ATTRIBUTE
#	endif
#	if defined( __has_cpp_attribute ) && defined( __cplusplus )
#		define ZPL_GNUC_HAS_CPP_ATTRIBUTE( attribute, major, minor, patch ) __has_cpp_attribute( attribute )
#	else
#		define ZPL_GNUC_HAS_CPP_ATTRIBUTE( attribute, major, minor, patch ) ZPL_GNUC_VERSION_CHECK( major, minor, patch )
#	endif

#	if defined( ZPL_GCC_HAS_CPP_ATTRIBUTE )
#		undef ZPL_GCC_HAS_CPP_ATTRIBUTE
#	endif
#	if defined( __has_cpp_attribute ) && defined( __cplusplus )
#		define ZPL_GCC_HAS_CPP_ATTRIBUTE( attribute, major, minor, patch ) __has_cpp_attribute( attribute )
#	else
#		define ZPL_GCC_HAS_CPP_ATTRIBUTE( attribute, major, minor, patch ) ZPL_GCC_VERSION_CHECK( major, minor, patch )
#	endif

#	if defined( ZPL_HAS_BUILTIN )
#		undef ZPL_HAS_BUILTIN
#	endif
#	if defined( __has_builtin )
#		define ZPL_HAS_BUILTIN( builtin ) __has_builtin( builtin )
#	else
#		define ZPL_HAS_BUILTIN( builtin ) ( 0 )
#	endif

#	if defined( ZPL_GNUC_HAS_BUILTIN )
#		undef ZPL_GNUC_HAS_BUILTIN
#	endif
#	if defined( __has_builtin )
#		define ZPL_GNUC_HAS_BUILTIN( builtin, major, minor, patch ) __has_builtin( builtin )
#	else
#		define ZPL_GNUC_HAS_BUILTIN( builtin, major, minor, patch ) ZPL_GNUC_VERSION_CHECK( major, minor, patch )
#	endif

#	if defined( ZPL_GCC_HAS_BUILTIN )
#		undef ZPL_GCC_HAS_BUILTIN
#	endif
#	if defined( __has_builtin )
#		define ZPL_GCC_HAS_BUILTIN( builtin, major, minor, patch ) __has_builtin( builtin )
#	else
#		define ZPL_GCC_HAS_BUILTIN( builtin, major, minor, patch ) ZPL_GCC_VERSION_CHECK( major, minor, patch )
#	endif

#	if defined( ZPL_HAS_FEATURE )
#		undef ZPL_HAS_FEATURE
#	endif
#	if defined( __has_feature )
#		define ZPL_HAS_FEATURE( feature ) __has_feature( feature )
#	else
#		define ZPL_HAS_FEATURE( feature ) ( 0 )
#	endif

#	if defined( ZPL_GNUC_HAS_FEATURE )
#		undef ZPL_GNUC_HAS_FEATURE
#	endif
#	if defined( __has_feature )
#		define ZPL_GNUC_HAS_FEATURE( feature, major, minor, patch ) __has_feature( feature )
#	else
#		define ZPL_GNUC_HAS_FEATURE( feature, major, minor, patch ) ZPL_GNUC_VERSION_CHECK( major, minor, patch )
#	endif

#	if defined( ZPL_GCC_HAS_FEATURE )
#		undef ZPL_GCC_HAS_FEATURE
#	endif
#	if defined( __has_feature )
#		define ZPL_GCC_HAS_FEATURE( feature, major, minor, patch ) __has_feature( feature )
#	else
#		define ZPL_GCC_HAS_FEATURE( feature, major, minor, patch ) ZPL_GCC_VERSION_CHECK( major, minor, patch )
#	endif

#	if defined( ZPL_HAS_EXTENSION )
#		undef ZPL_HAS_EXTENSION
#	endif
#	if defined( __has_extension )
#		define ZPL_HAS_EXTENSION( extension ) __has_extension( extension )
#	else
#		define ZPL_HAS_EXTENSION( extension ) ( 0 )
#	endif

#	if defined( ZPL_GNUC_HAS_EXTENSION )
#		undef ZPL_GNUC_HAS_EXTENSION
#	endif
#	if defined( __has_extension )
#		define ZPL_GNUC_HAS_EXTENSION( extension, major, minor, patch ) __has_extension( extension )
#	else
#		define ZPL_GNUC_HAS_EXTENSION( extension, major, minor, patch ) ZPL_GNUC_VERSION_CHECK( major, minor, patch )
#	endif

#	if defined( ZPL_GCC_HAS_EXTENSION )
#		undef ZPL_GCC_HAS_EXTENSION
#	endif
#	if defined( __has_extension )
#		define ZPL_GCC_HAS_EXTENSION( extension, major, minor, patch ) __has_extension( extension )
#	else
#		define ZPL_GCC_HAS_EXTENSION( extension, major, minor, patch ) ZPL_GCC_VERSION_CHECK( major, minor, patch )
#	endif

#	if defined( ZPL_HAS_DECLSPEC_ATTRIBUTE )
#		undef ZPL_HAS_DECLSPEC_ATTRIBUTE
#	endif
#	if defined( __has_declspec_attribute )
#		define ZPL_HAS_DECLSPEC_ATTRIBUTE( attribute ) __has_declspec_attribute( attribute )
#	else
#		define ZPL_HAS_DECLSPEC_ATTRIBUTE( attribute ) ( 0 )
#	endif

#	if defined( ZPL_GNUC_HAS_DECLSPEC_ATTRIBUTE )
#		undef ZPL_GNUC_HAS_DECLSPEC_ATTRIBUTE
#	endif
#	if defined( __has_declspec_attribute )
#		define ZPL_GNUC_HAS_DECLSPEC_ATTRIBUTE( attribute, major, minor, patch ) __has_declspec_attribute( attribute )
#	else
#		define ZPL_GNUC_HAS_DECLSPEC_ATTRIBUTE( attribute, major, minor, patch ) ZPL_GNUC_VERSION_CHECK( major, minor, patch )
#	endif

#	if defined( ZPL_GCC_HAS_DECLSPEC_ATTRIBUTE )
#		undef ZPL_GCC_HAS_DECLSPEC_ATTRIBUTE
#	endif
#	if defined( __has_declspec_attribute )
#		define ZPL_GCC_HAS_DECLSPEC_ATTRIBUTE( attribute, major, minor, patch ) __has_declspec_attribute( attribute )
#	else
#		define ZPL_GCC_HAS_DECLSPEC_ATTRIBUTE( attribute, major, minor, patch ) ZPL_GCC_VERSION_CHECK( major, minor, patch )
#	endif

#	if defined( ZPL_HAS_WARNING )
#		undef ZPL_HAS_WARNING
#	endif
#	if defined( __has_warning )
#		define ZPL_HAS_WARNING( warning ) __has_warning( warning )
#	else
#		define ZPL_HAS_WARNING( warning ) ( 0 )
#	endif

#	if defined( ZPL_GNUC_HAS_WARNING )
#		undef ZPL_GNUC_HAS_WARNING
#	endif
#	if defined( __has_warning )
#		define ZPL_GNUC_HAS_WARNING( warning, major, minor, patch ) __has_warning( warning )
#	else
#		define ZPL_GNUC_HAS_WARNING( warning, major, minor, patch ) ZPL_GNUC_VERSION_CHECK( major, minor, patch )
#	endif

#	if defined( ZPL_GCC_HAS_WARNING )
#		undef ZPL_GCC_HAS_WARNING
#	endif
#	if defined( __has_warning )
#		define ZPL_GCC_HAS_WARNING( warning, major, minor, patch ) __has_warning( warning )
#	else
#		define ZPL_GCC_HAS_WARNING( warning, major, minor, patch ) ZPL_GCC_VERSION_CHECK( major, minor, patch )
#	endif

/* ZPL_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_ is for
   ZPL INTERNAL USE ONLY.  API subject to change without notice. */
#	if defined( ZPL_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_ )
#		undef ZPL_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_
#	endif
#	if defined( __cplusplus )
#		if ZPL_HAS_WARNING( "-Wc++98-compat" )
#			if ZPL_HAS_WARNING( "-Wc++17-extensions" )
#				define ZPL_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_( xpr ) \
					ZPL_DIAGNOSTIC_PUSH                                  \
					_Pragma( "clang diagnostic ignored \"-Wc++98-compat\"" ) _Pragma( "clang diagnostic ignored \"-Wc++17-extensions\"" ) xpr ZPL_DIAGNOSTIC_POP
#			else
#				define ZPL_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_( xpr ) \
					ZPL_DIAGNOSTIC_PUSH                                  \
					_Pragma( "clang diagnostic ignored \"-Wc++98-compat\"" ) xpr ZPL_DIAGNOSTIC_POP
#			endif
#		endif
#	endif
#	if ! defined( ZPL_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_ )
#		define ZPL_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_( x ) x
#	endif

#	if defined( ZPL_CONST_CAST )
#		undef ZPL_CONST_CAST
#	endif
#	if defined( __cplusplus )
#		define ZPL_CONST_CAST( T, expr ) ( const_cast< T >( expr ) )
#	elif ZPL_HAS_WARNING( "-Wcast-qual" ) || ZPL_GCC_VERSION_CHECK( 4, 6, 0 ) || ZPL_INTEL_VERSION_CHECK( 13, 0, 0 )
#		define ZPL_CONST_CAST( T, expr )                          \
			( __extension__( {                                     \
				ZPL_DIAGNOSTIC_PUSH                                \
				ZPL_DIAGNOSTIC_DISABLE_CAST_QUAL( ( T )( expr ) ); \
				ZPL_DIAGNOSTIC_POP                                 \
			} ) )
#	else
#		define ZPL_CONST_CAST( T, expr ) ( ( T )( expr ) )
#	endif

#	if defined( ZPL_REINTERPRET_CAST )
#		undef ZPL_REINTERPRET_CAST
#	endif
#	if defined( __cplusplus )
#		define ZPL_REINTERPRET_CAST( T, expr ) ( reinterpret_cast< T >( expr ) )
#	else
#		define ZPL_REINTERPRET_CAST( T, expr ) ( ( T )( expr ) )
#	endif

#	if defined( ZPL_STATIC_CAST )
#		undef ZPL_STATIC_CAST
#	endif
#	if defined( __cplusplus )
#		define ZPL_STATIC_CAST( T, expr ) ( static_cast< T >( expr ) )
#	else
#		define ZPL_STATIC_CAST( T, expr ) ( ( T )( expr ) )
#	endif

#	if defined( ZPL_CPP_CAST )
#		undef ZPL_CPP_CAST
#	endif
#	if defined( __cplusplus )
#		if ZPL_HAS_WARNING( "-Wold-style-zpl_cast" )
#			define ZPL_CPP_CAST( T, expr ) \
				ZPL_DIAGNOSTIC_PUSH         \
				_Pragma( "clang diagnostic ignored \"-Wold-style-zpl_cast\"" )( ( T )( expr ) ) ZPL_DIAGNOSTIC_POP
#		elif ZPL_IAR_VERSION_CHECK( 8, 3, 0 )
#			define ZPL_CPP_CAST( T, expr ) \
				ZPL_DIAGNOSTIC_PUSH         \
				_Pragma( "diag_suppress=Pe137" ) ZPL_DIAGNOSTIC_POP #else
#			define ZPL_CPP_CAST( T, expr ) ( ( T )( expr ) )
#		endif
#	else
#		define ZPL_CPP_CAST( T, expr ) ( expr )
#	endif

#	if ( defined( __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 199901L ) ) || defined( __clang__ ) || ZPL_GCC_VERSION_CHECK( 3, 0, 0 ) || ZPL_INTEL_VERSION_CHECK( 13, 0, 0 ) \
	    || ZPL_IAR_VERSION_CHECK( 8, 0, 0 ) || ZPL_PGI_VERSION_CHECK( 18, 4, 0 ) || ZPL_ARM_VERSION_CHECK( 4, 1, 0 ) || ZPL_TI_VERSION_CHECK( 15, 12, 0 )                     \
	    || ZPL_TI_ARMCL_VERSION_CHECK( 4, 7, 0 ) || ZPL_TI_CL430_VERSION_CHECK( 2, 0, 1 ) || ZPL_TI_CL2000_VERSION_CHECK( 6, 1, 0 ) || ZPL_TI_CL6X_VERSION_CHECK( 7, 0, 0 )   \
	    || ZPL_TI_CL7X_VERSION_CHECK( 1, 2, 0 ) || ZPL_TI_CLPRU_VERSION_CHECK( 2, 1, 0 ) || ZPL_CRAY_VERSION_CHECK( 5, 0, 0 ) || ZPL_TINYC_VERSION_CHECK( 0, 9, 17 )          \
	    || ZPL_SUNPRO_VERSION_CHECK( 8, 0, 0 ) || ( ZPL_IBM_VERSION_CHECK( 10, 1, 0 ) && defined( __C99_PRAGMA_OPERATOR ) )
#		define ZPL_PRAGMA( value ) _Pragma( #value )
#	elif ZPL_MSVC_VERSION_CHECK( 15, 0, 0 )
#		define ZPL_PRAGMA( value ) __pragma( value )
#	else
#		define ZPL_PRAGMA( value )
#	endif

#	if defined( ZPL_DIAGNOSTIC_PUSH )
#		undef ZPL_DIAGNOSTIC_PUSH
#	endif
#	if defined( ZPL_DIAGNOSTIC_PUSH_WARNLEVEL )
#		undef ZPL_DIAGNOSTIC_PUSH_WARNLEVEL
#	endif
#	if defined( ZPL_DIAGNOSTIC_POP )
#		undef ZPL_DIAGNOSTIC_POP
#	endif
#	if defined( __clang__ )
#		define ZPL_DIAGNOSTIC_PUSH _Pragma( "clang diagnostic push" )
#		define ZPL_DIAGNOSTIC_PUSH_WARNLEVEL( x )
#		define ZPL_DIAGNOSTIC_POP _Pragma( "clang diagnostic pop" )
#	elif ZPL_INTEL_VERSION_CHECK( 13, 0, 0 )
#		define ZPL_DIAGNOSTIC_PUSH _Pragma( "warning(push)" )
#		define ZPL_DIAGNOSTIC_PUSH_WARNLEVEL( x )
#		define ZPL_DIAGNOSTIC_POP _Pragma( "warning(pop)" )
#	elif ZPL_GCC_VERSION_CHECK( 4, 6, 0 )
#		define ZPL_DIAGNOSTIC_PUSH _Pragma( "GCC diagnostic push" )
#		define ZPL_DIAGNOSTIC_PUSH_WARNLEVEL( x )
#		define ZPL_DIAGNOSTIC_POP _Pragma( "GCC diagnostic pop" )
#	elif ZPL_MSVC_VERSION_CHECK( 15, 0, 0 )
#		define ZPL_DIAGNOSTIC_PUSH                __pragma( warning( push ) )
#		define ZPL_DIAGNOSTIC_PUSH_WARNLEVEL( x ) __pragma( warning( push, x ) )
#		define ZPL_DIAGNOSTIC_POP                 __pragma( warning( pop ) )
#	elif ZPL_ARM_VERSION_CHECK( 5, 6, 0 )
#		define ZPL_DIAGNOSTIC_PUSH _Pragma( "push" )
#		define ZPL_DIAGNOSTIC_POP  _Pragma( "pop" )
#	elif ZPL_TI_VERSION_CHECK( 15, 12, 0 ) || ZPL_TI_ARMCL_VERSION_CHECK( 5, 2, 0 ) || ZPL_TI_CL430_VERSION_CHECK( 4, 4, 0 ) || ZPL_TI_CL6X_VERSION_CHECK( 8, 1, 0 ) \
	    || ZPL_TI_CL7X_VERSION_CHECK( 1, 2, 0 ) || ZPL_TI_CLPRU_VERSION_CHECK( 2, 1, 0 )
#		define ZPL_DIAGNOSTIC_PUSH _Pragma( "diag_push" )
#		define ZPL_DIAGNOSTIC_POP  _Pragma( "diag_pop" )
#	elif ZPL_PELLES_VERSION_CHECK( 2, 90, 0 )
#		define ZPL_DIAGNOSTIC_PUSH _Pragma( "warning(push)" )
#		define ZPL_DIAGNOSTIC_POP  _Pragma( "warning(pop)" )
#	else
#		define ZPL_DIAGNOSTIC_PUSH
#		define ZPL_DIAGNOSTIC_POP
#	endif

#	if defined( ZPL_DIAGNOSTIC_DISABLE_DEPRECATED )
#		undef ZPL_DIAGNOSTIC_DISABLE_DEPRECATED
#	endif
#	if ZPL_HAS_WARNING( "-Wdeprecated-declarations" )
#		define ZPL_DIAGNOSTIC_DISABLE_DEPRECATED _Pragma( "clang diagnostic ignored \"-Wdeprecated-declarations\"" )
#	elif ZPL_INTEL_VERSION_CHECK( 13, 0, 0 )
#		define ZPL_DIAGNOSTIC_DISABLE_DEPRECATED _Pragma( "warning(disable:1478 1786)" )
#	elif ZPL_PGI_VERSION_CHECK( 17, 10, 0 )
#		define ZPL_DIAGNOSTIC_DISABLE_DEPRECATED _Pragma( "diag_suppress 1215,1444" )
#	elif ZPL_GCC_VERSION_CHECK( 4, 3, 0 )
#		define ZPL_DIAGNOSTIC_DISABLE_DEPRECATED _Pragma( "GCC diagnostic ignored \"-Wdeprecated-declarations\"" )
#	elif ZPL_MSVC_VERSION_CHECK( 15, 0, 0 )
#		define ZPL_DIAGNOSTIC_DISABLE_DEPRECATED __pragma( warning( disable : 4996 ) )
#	elif ZPL_TI_VERSION_CHECK( 15, 12, 0 ) || ( ZPL_TI_ARMCL_VERSION_CHECK( 4, 8, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_ARMCL_VERSION_CHECK( 5, 2, 0 )   \
	    || ( ZPL_TI_CL2000_VERSION_CHECK( 6, 0, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_CL2000_VERSION_CHECK( 6, 4, 0 )                                     \
	    || ( ZPL_TI_CL430_VERSION_CHECK( 4, 0, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_CL430_VERSION_CHECK( 4, 3, 0 )                                       \
	    || ( ZPL_TI_CL6X_VERSION_CHECK( 7, 2, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_CL6X_VERSION_CHECK( 7, 5, 0 ) || ZPL_TI_CL7X_VERSION_CHECK( 1, 2, 0 ) \
	    || ZPL_TI_CLPRU_VERSION_CHECK( 2, 1, 0 )
#		define ZPL_DIAGNOSTIC_DISABLE_DEPRECATED _Pragma( "diag_suppress 1291,1718" )
#	elif ZPL_SUNPRO_VERSION_CHECK( 5, 13, 0 ) && ! defined( __cplusplus )
#		define ZPL_DIAGNOSTIC_DISABLE_DEPRECATED _Pragma( "error_messages(off,E_DEPRECATED_ATT,E_DEPRECATED_ATT_MESS)" )
#	elif ZPL_SUNPRO_VERSION_CHECK( 5, 13, 0 ) && defined( __cplusplus )
#		define ZPL_DIAGNOSTIC_DISABLE_DEPRECATED _Pragma( "error_messages(off,symdeprecated,symdeprecated2)" )
#	elif ZPL_IAR_VERSION_CHECK( 8, 0, 0 )
#		define ZPL_DIAGNOSTIC_DISABLE_DEPRECATED _Pragma( "diag_suppress=Pe1444,Pe1215" )
#	elif ZPL_PELLES_VERSION_CHECK( 2, 90, 0 )
#		define ZPL_DIAGNOSTIC_DISABLE_DEPRECATED _Pragma( "warn(disable:2241)" )
#	else
#		define ZPL_DIAGNOSTIC_DISABLE_DEPRECATED
#	endif

#	if defined( ZPL_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS )
#		undef ZPL_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS
#	endif
#	if ZPL_HAS_WARNING( "-Wunknown-pragmas" )
#		define ZPL_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS _Pragma( "clang diagnostic ignored \"-Wunknown-pragmas\"" )
#	elif ZPL_INTEL_VERSION_CHECK( 13, 0, 0 )
#		define ZPL_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS _Pragma( "warning(disable:161)" )
#	elif ZPL_PGI_VERSION_CHECK( 17, 10, 0 )
#		define ZPL_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS _Pragma( "diag_suppress 1675" )
#	elif ZPL_GCC_VERSION_CHECK( 4, 3, 0 )
#		define ZPL_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS _Pragma( "GCC diagnostic ignored \"-Wunknown-pragmas\"" )
#	elif ZPL_MSVC_VERSION_CHECK( 15, 0, 0 )
#		define ZPL_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS __pragma( warning( disable : 4068 ) )
#	elif ZPL_TI_VERSION_CHECK( 16, 9, 0 ) || ZPL_TI_CL6X_VERSION_CHECK( 8, 0, 0 ) || ZPL_TI_CL7X_VERSION_CHECK( 1, 2, 0 ) || ZPL_TI_CLPRU_VERSION_CHECK( 2, 3, 0 )
#		define ZPL_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS _Pragma( "diag_suppress 163" )
#	elif ZPL_TI_CL6X_VERSION_CHECK( 8, 0, 0 )
#		define ZPL_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS _Pragma( "diag_suppress 163" )
#	elif ZPL_IAR_VERSION_CHECK( 8, 0, 0 )
#		define ZPL_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS _Pragma( "diag_suppress=Pe161" )
#	else
#		define ZPL_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS
#	endif

#	if defined( ZPL_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES )
#		undef ZPL_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES
#	endif
#	if ZPL_HAS_WARNING( "-Wunknown-attributes" )
#		define ZPL_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES _Pragma( "clang diagnostic ignored \"-Wunknown-attributes\"" )
#	elif ZPL_GCC_VERSION_CHECK( 4, 6, 0 )
#		define ZPL_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES _Pragma( "GCC diagnostic ignored \"-Wdeprecated-declarations\"" )
#	elif ZPL_INTEL_VERSION_CHECK( 17, 0, 0 )
#		define ZPL_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES _Pragma( "warning(disable:1292)" )
#	elif ZPL_MSVC_VERSION_CHECK( 19, 0, 0 )
#		define ZPL_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES __pragma( warning( disable : 5030 ) )
#	elif ZPL_PGI_VERSION_CHECK( 17, 10, 0 )
#		define ZPL_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES _Pragma( "diag_suppress 1097" )
#	elif ZPL_SUNPRO_VERSION_CHECK( 5, 14, 0 ) && defined( __cplusplus )
#		define ZPL_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES _Pragma( "error_messages(off,attrskipunsup)" )
#	elif ZPL_TI_VERSION_CHECK( 18, 1, 0 ) || ZPL_TI_CL6X_VERSION_CHECK( 8, 3, 0 ) || ZPL_TI_CL7X_VERSION_CHECK( 1, 2, 0 )
#		define ZPL_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES _Pragma( "diag_suppress 1173" )
#	elif ZPL_IAR_VERSION_CHECK( 8, 0, 0 )
#		define ZPL_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES _Pragma( "diag_suppress=Pe1097" )
#	else
#		define ZPL_DIAGNOSTIC_DISABLE_UNKNOWN_CPP_ATTRIBUTES
#	endif

#	if defined( ZPL_DIAGNOSTIC_DISABLE_CAST_QUAL )
#		undef ZPL_DIAGNOSTIC_DISABLE_CAST_QUAL
#	endif
#	if ZPL_HAS_WARNING( "-Wcast-qual" )
#		define ZPL_DIAGNOSTIC_DISABLE_CAST_QUAL _Pragma( "clang diagnostic ignored \"-Wcast-qual\"" )
#	elif ZPL_INTEL_VERSION_CHECK( 13, 0, 0 )
#		define ZPL_DIAGNOSTIC_DISABLE_CAST_QUAL _Pragma( "warning(disable:2203 2331)" )
#	elif ZPL_GCC_VERSION_CHECK( 3, 0, 0 )
#		define ZPL_DIAGNOSTIC_DISABLE_CAST_QUAL _Pragma( "GCC diagnostic ignored \"-Wcast-qual\"" )
#	else
#		define ZPL_DIAGNOSTIC_DISABLE_CAST_QUAL
#	endif

#	if defined( ZPL_DEPRECATED )
#		undef ZPL_DEPRECATED
#	endif
#	if defined( ZPL_DEPRECATED_FOR )
#		undef ZPL_DEPRECATED_FOR
#	endif
#	if defined( __cplusplus ) && ( __cplusplus >= 201402L )
#		define ZPL_DEPRECATED( since )                  ZPL_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_( [[deprecated( "Since " #since )]] )
#		define ZPL_DEPRECATED_FOR( since, replacement ) ZPL_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_( [[deprecated( "Since " #since "; use " #replacement )]] )
#	elif ZPL_HAS_EXTENSION( attribute_deprecated_with_message ) || ZPL_GCC_VERSION_CHECK( 4, 5, 0 ) || ZPL_INTEL_VERSION_CHECK( 13, 0, 0 ) || ZPL_ARM_VERSION_CHECK( 5, 6, 0 ) \
	    || ZPL_SUNPRO_VERSION_CHECK( 5, 13, 0 ) || ZPL_PGI_VERSION_CHECK( 17, 10, 0 ) || ZPL_TI_VERSION_CHECK( 18, 1, 0 ) || ZPL_TI_ARMCL_VERSION_CHECK( 18, 1, 0 )             \
	    || ZPL_TI_CL6X_VERSION_CHECK( 8, 3, 0 ) || ZPL_TI_CL7X_VERSION_CHECK( 1, 2, 0 ) || ZPL_TI_CLPRU_VERSION_CHECK( 2, 3, 0 )
#		define ZPL_DEPRECATED( since )                  __attribute__( ( __deprecated__( "Since " #since ) ) )
#		define ZPL_DEPRECATED_FOR( since, replacement ) __attribute__( ( __deprecated__( "Since " #since "; use " #replacement ) ) )
#	elif ZPL_HAS_ATTRIBUTE( deprecated ) || ZPL_GCC_VERSION_CHECK( 3, 1, 0 ) || ZPL_ARM_VERSION_CHECK( 4, 1, 0 ) || ZPL_TI_VERSION_CHECK( 15, 12, 0 )                         \
	    || ( ZPL_TI_ARMCL_VERSION_CHECK( 4, 8, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_ARMCL_VERSION_CHECK( 5, 2, 0 )                                       \
	    || ( ZPL_TI_CL2000_VERSION_CHECK( 6, 0, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_CL2000_VERSION_CHECK( 6, 4, 0 )                                     \
	    || ( ZPL_TI_CL430_VERSION_CHECK( 4, 0, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_CL430_VERSION_CHECK( 4, 3, 0 )                                       \
	    || ( ZPL_TI_CL6X_VERSION_CHECK( 7, 2, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_CL6X_VERSION_CHECK( 7, 5, 0 ) || ZPL_TI_CL7X_VERSION_CHECK( 1, 2, 0 ) \
	    || ZPL_TI_CLPRU_VERSION_CHECK( 2, 1, 0 )
#		define ZPL_DEPRECATED( since )                  __attribute__( ( __deprecated__ ) )
#		define ZPL_DEPRECATED_FOR( since, replacement ) __attribute__( ( __deprecated__ ) )
#	elif ZPL_MSVC_VERSION_CHECK( 14, 0, 0 )
#		define ZPL_DEPRECATED( since )                  __declspec( deprecated( "Since " #since ) )
#		define ZPL_DEPRECATED_FOR( since, replacement ) __declspec( deprecated( "Since " #since "; use " #replacement ) )
#	elif ZPL_MSVC_VERSION_CHECK( 13, 10, 0 ) || ZPL_PELLES_VERSION_CHECK( 6, 50, 0 )
#		define ZPL_DEPRECATED( since )                  __declspec( deprecated )
#		define ZPL_DEPRECATED_FOR( since, replacement ) __declspec( deprecated )
#	elif ZPL_IAR_VERSION_CHECK( 8, 0, 0 )
#		define ZPL_DEPRECATED( since )                  _Pragma( "deprecated" )
#		define ZPL_DEPRECATED_FOR( since, replacement ) _Pragma( "deprecated" )
#	else
#		define ZPL_DEPRECATED( since )
#		define ZPL_DEPRECATED_FOR( since, replacement )
#	endif

#	if defined( ZPL_UNAVAILABLE )
#		undef ZPL_UNAVAILABLE
#	endif
#	if ZPL_HAS_ATTRIBUTE( warning ) || ZPL_GCC_VERSION_CHECK( 4, 3, 0 ) || ZPL_INTEL_VERSION_CHECK( 13, 0, 0 )
#		define ZPL_UNAVAILABLE( available_since ) __attribute__( ( __warning__( "Not available until " #available_since ) ) )
#	else
#		define ZPL_UNAVAILABLE( available_since )
#	endif

#	if defined( ZPL_WARN_UNUSED_RESULT )
#		undef ZPL_WARN_UNUSED_RESULT
#	endif
#	if defined( ZPL_WARN_UNUSED_RESULT_MSG )
#		undef ZPL_WARN_UNUSED_RESULT_MSG
#	endif
#	if ( ZPL_HAS_CPP_ATTRIBUTE( nodiscard ) >= 201907L )
#		define ZPL_WARN_UNUSED_RESULT            ZPL_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_( [[nodiscard]] )
#		define ZPL_WARN_UNUSED_RESULT_MSG( msg ) ZPL_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_( [[nodiscard( msg )]] )
#	elif ZPL_HAS_CPP_ATTRIBUTE( nodiscard )
#		define ZPL_WARN_UNUSED_RESULT            ZPL_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_( [[nodiscard]] )
#		define ZPL_WARN_UNUSED_RESULT_MSG( msg ) ZPL_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_( [[nodiscard]] )
#	elif ZPL_HAS_ATTRIBUTE( warn_unused_result ) || ZPL_GCC_VERSION_CHECK( 3, 4, 0 ) || ZPL_INTEL_VERSION_CHECK( 13, 0, 0 ) || ZPL_TI_VERSION_CHECK( 15, 12, 0 )              \
	    || ( ZPL_TI_ARMCL_VERSION_CHECK( 4, 8, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_ARMCL_VERSION_CHECK( 5, 2, 0 )                                       \
	    || ( ZPL_TI_CL2000_VERSION_CHECK( 6, 0, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_CL2000_VERSION_CHECK( 6, 4, 0 )                                     \
	    || ( ZPL_TI_CL430_VERSION_CHECK( 4, 0, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_CL430_VERSION_CHECK( 4, 3, 0 )                                       \
	    || ( ZPL_TI_CL6X_VERSION_CHECK( 7, 2, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_CL6X_VERSION_CHECK( 7, 5, 0 ) || ZPL_TI_CL7X_VERSION_CHECK( 1, 2, 0 ) \
	    || ZPL_TI_CLPRU_VERSION_CHECK( 2, 1, 0 ) || ( ZPL_SUNPRO_VERSION_CHECK( 5, 15, 0 ) && defined( __cplusplus ) ) || ZPL_PGI_VERSION_CHECK( 17, 10, 0 )
#		define ZPL_WARN_UNUSED_RESULT            __attribute__( ( __warn_unused_result__ ) )
#		define ZPL_WARN_UNUSED_RESULT_MSG( msg ) __attribute__( ( __warn_unused_result__ ) )
#	elif defined( _Check_return_ ) /* SAL */
#		define ZPL_WARN_UNUSED_RESULT            _Check_return_
#		define ZPL_WARN_UNUSED_RESULT_MSG( msg ) _Check_return_
#	else
#		define ZPL_WARN_UNUSED_RESULT
#		define ZPL_WARN_UNUSED_RESULT_MSG( msg )
#	endif

#	if defined( ZPL_SENTINEL )
#		undef ZPL_SENTINEL
#	endif
#	if ZPL_HAS_ATTRIBUTE( sentinel ) || ZPL_GCC_VERSION_CHECK( 4, 0, 0 ) || ZPL_INTEL_VERSION_CHECK( 13, 0, 0 ) || ZPL_ARM_VERSION_CHECK( 5, 4, 0 )
#		define ZPL_SENTINEL( position ) __attribute__( ( __sentinel__( position ) ) )
#	else
#		define ZPL_SENTINEL( position )
#	endif

#	if defined( ZPL_NO_RETURN )
#		undef ZPL_NO_RETURN
#	endif
#	if ZPL_IAR_VERSION_CHECK( 8, 0, 0 )
#		define ZPL_NO_RETURN __noreturn
#	elif ZPL_INTEL_VERSION_CHECK( 13, 0, 0 )
#		define ZPL_NO_RETURN __attribute__( ( __noreturn__ ) )
#	elif defined( __STDC_VERSION__ ) && __STDC_VERSION__ >= 201112L
#		define ZPL_NO_RETURN _Noreturn
#	elif defined( __cplusplus ) && ( __cplusplus >= 201103L )
#		define ZPL_NO_RETURN ZPL_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_( [[noreturn]] )
#	elif ZPL_HAS_ATTRIBUTE( noreturn ) || ZPL_GCC_VERSION_CHECK( 3, 2, 0 ) || ZPL_SUNPRO_VERSION_CHECK( 5, 11, 0 ) || ZPL_ARM_VERSION_CHECK( 4, 1, 0 )                            \
	    || ZPL_IBM_VERSION_CHECK( 10, 1, 0 ) || ZPL_TI_VERSION_CHECK( 15, 12, 0 ) || ( ZPL_TI_ARMCL_VERSION_CHECK( 4, 8, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) )          \
	    || ZPL_TI_ARMCL_VERSION_CHECK( 5, 2, 0 ) || ( ZPL_TI_CL2000_VERSION_CHECK( 6, 0, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) )                                          \
	    || ZPL_TI_CL2000_VERSION_CHECK( 6, 4, 0 ) || ( ZPL_TI_CL430_VERSION_CHECK( 4, 0, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_CL430_VERSION_CHECK( 4, 3, 0 ) \
	    || ( ZPL_TI_CL6X_VERSION_CHECK( 7, 2, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_CL6X_VERSION_CHECK( 7, 5, 0 ) || ZPL_TI_CL7X_VERSION_CHECK( 1, 2, 0 )     \
	    || ZPL_TI_CLPRU_VERSION_CHECK( 2, 1, 0 )
#		define ZPL_NO_RETURN __attribute__( ( __noreturn__ ) )
#	elif ZPL_SUNPRO_VERSION_CHECK( 5, 10, 0 )
#		define ZPL_NO_RETURN _Pragma( "does_not_return" )
#	elif ZPL_MSVC_VERSION_CHECK( 13, 10, 0 )
#		define ZPL_NO_RETURN __declspec( noreturn )
#	elif ZPL_TI_CL6X_VERSION_CHECK( 6, 0, 0 ) && defined( __cplusplus )
#		define ZPL_NO_RETURN _Pragma( "FUNC_NEVER_RETURNS;" )
#	elif ZPL_COMPCERT_VERSION_CHECK( 3, 2, 0 )
#		define ZPL_NO_RETURN __attribute( ( noreturn ) )
#	elif ZPL_PELLES_VERSION_CHECK( 9, 0, 0 )
#		define ZPL_NO_RETURN __declspec( noreturn )
#	else
#		define ZPL_NO_RETURN
#	endif

#	if defined( ZPL_NO_ESCAPE )
#		undef ZPL_NO_ESCAPE
#	endif
#	if ZPL_HAS_ATTRIBUTE( noescape )
#		define ZPL_NO_ESCAPE __attribute__( ( __noescape__ ) )
#	else
#		define ZPL_NO_ESCAPE
#	endif

#	if defined( ZPL_UNREACHABLE )
#		undef ZPL_UNREACHABLE
#	endif
#	if defined( ZPL_UNREACHABLE_RETURN )
#		undef ZPL_UNREACHABLE_RETURN
#	endif
#	if defined( ZPL_ASSUME )
#		undef ZPL_ASSUME
#	endif
#	if ZPL_MSVC_VERSION_CHECK( 13, 10, 0 ) || ZPL_INTEL_VERSION_CHECK( 13, 0, 0 )
#		define ZPL_ASSUME( expr ) __assume( expr )
#	elif ZPL_HAS_BUILTIN( __builtin_assume )
#		define ZPL_ASSUME( expr ) __builtin_assume( expr )
#	elif ZPL_TI_CL2000_VERSION_CHECK( 6, 2, 0 ) || ZPL_TI_CL6X_VERSION_CHECK( 4, 0, 0 )
#		if defined( __cplusplus )
#			define ZPL_ASSUME( expr ) std::_nassert( expr )
#		else
#			define ZPL_ASSUME( expr ) _nassert( expr )
#		endif
#	endif
#	if ( ZPL_HAS_BUILTIN( __builtin_unreachable ) && ( ! defined( ZPL_ARM_VERSION ) ) ) || ZPL_GCC_VERSION_CHECK( 4, 5, 0 ) || ZPL_PGI_VERSION_CHECK( 18, 10, 0 ) \
	    || ZPL_INTEL_VERSION_CHECK( 13, 0, 0 ) || ZPL_IBM_VERSION_CHECK( 13, 1, 5 )
#		define ZPL_UNREACHABLE() __builtin_unreachable()
#	elif defined( ZPL_ASSUME )
#		define ZPL_UNREACHABLE() ZPL_ASSUME( 0 )
#	endif
#	if ! defined( ZPL_ASSUME )
#		if defined( ZPL_UNREACHABLE )
#			define ZPL_ASSUME( expr ) ZPL_STATIC_CAST( void, ( ( expr ) ? 1 : ( ZPL_UNREACHABLE(), 1 ) ) )
#		else
#			define ZPL_ASSUME( expr ) ZPL_STATIC_CAST( void, expr )
#		endif
#	endif
#	if defined( ZPL_UNREACHABLE )
#		if ZPL_TI_CL2000_VERSION_CHECK( 6, 2, 0 ) || ZPL_TI_CL6X_VERSION_CHECK( 4, 0, 0 )
#			define ZPL_UNREACHABLE_RETURN( value ) return ( ZPL_STATIC_CAST( void, ZPL_ASSUME( 0 ) ), ( value ) )
#		else
#			define ZPL_UNREACHABLE_RETURN( value ) ZPL_UNREACHABLE()
#		endif
#	else
#		define ZPL_UNREACHABLE_RETURN( value ) return ( value )
#	endif
#	if ! defined( ZPL_UNREACHABLE )
#		define ZPL_UNREACHABLE() ZPL_ASSUME( 0 )
#	endif

ZPL_DIAGNOSTIC_PUSH
#	if ZPL_HAS_WARNING( "-Wpedantic" )
#		pragma clang diagnostic ignored "-Wpedantic"
#	endif
#	if ZPL_HAS_WARNING( "-Wc++98-compat-pedantic" ) && defined( __cplusplus )
#		pragma clang diagnostic ignored "-Wc++98-compat-pedantic"
#	endif
#	if ZPL_GCC_HAS_WARNING( "-Wvariadic-macros", 4, 0, 0 )
#		if defined( __clang__ )
#			pragma clang diagnostic ignored "-Wvariadic-macros"
#		elif defined( ZPL_GCC_VERSION )
#			pragma GCC diagnostic ignored "-Wvariadic-macros"
#		endif
#	endif
#	if defined( ZPL_NON_NULL )
#		undef ZPL_NON_NULL
#	endif
#	if ZPL_HAS_ATTRIBUTE( nonnull ) || ZPL_GCC_VERSION_CHECK( 3, 3, 0 ) || ZPL_INTEL_VERSION_CHECK( 13, 0, 0 ) || ZPL_ARM_VERSION_CHECK( 4, 1, 0 )
#		define ZPL_NON_NULL( ... ) __attribute__( ( __nonnull__( __VA_ARGS__ ) ) )
#	else
#		define ZPL_NON_NULL( ... )
#	endif
ZPL_DIAGNOSTIC_POP

#	if defined( ZPL_PRINTF_FORMAT )
#		undef ZPL_PRINTF_FORMAT
#	endif
#	if defined( __MINGW32__ ) && ZPL_GCC_HAS_ATTRIBUTE( format, 4, 4, 0 ) && ! defined( __USE_MINGW_ANSI_STDIO )
#		define ZPL_PRINTF_FORMAT( string_idx, first_to_check ) __attribute__( ( __format__( ms_printf, string_idx, first_to_check ) ) )
#	elif defined( __MINGW32__ ) && ZPL_GCC_HAS_ATTRIBUTE( format, 4, 4, 0 ) && defined( __USE_MINGW_ANSI_STDIO )
#		define ZPL_PRINTF_FORMAT( string_idx, first_to_check ) __attribute__( ( __format__( gnu_printf, string_idx, first_to_check ) ) )
#	elif ZPL_HAS_ATTRIBUTE( format ) || ZPL_GCC_VERSION_CHECK( 3, 1, 0 ) || ZPL_INTEL_VERSION_CHECK( 13, 0, 0 ) || ZPL_ARM_VERSION_CHECK( 5, 6, 0 )                               \
	    || ZPL_IBM_VERSION_CHECK( 10, 1, 0 ) || ZPL_TI_VERSION_CHECK( 15, 12, 0 ) || ( ZPL_TI_ARMCL_VERSION_CHECK( 4, 8, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) )          \
	    || ZPL_TI_ARMCL_VERSION_CHECK( 5, 2, 0 ) || ( ZPL_TI_CL2000_VERSION_CHECK( 6, 0, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) )                                          \
	    || ZPL_TI_CL2000_VERSION_CHECK( 6, 4, 0 ) || ( ZPL_TI_CL430_VERSION_CHECK( 4, 0, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_CL430_VERSION_CHECK( 4, 3, 0 ) \
	    || ( ZPL_TI_CL6X_VERSION_CHECK( 7, 2, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_CL6X_VERSION_CHECK( 7, 5, 0 ) || ZPL_TI_CL7X_VERSION_CHECK( 1, 2, 0 )     \
	    || ZPL_TI_CLPRU_VERSION_CHECK( 2, 1, 0 )
#		define ZPL_PRINTF_FORMAT( string_idx, first_to_check ) __attribute__( ( __format__( __printf__, string_idx, first_to_check ) ) )
#	elif ZPL_PELLES_VERSION_CHECK( 6, 0, 0 )
#		define ZPL_PRINTF_FORMAT( string_idx, first_to_check ) __declspec( vaformat( printf, string_idx, first_to_check ) )
#	else
#		define ZPL_PRINTF_FORMAT( string_idx, first_to_check )
#	endif

#	if defined( ZPL_CONSTEXPR )
#		undef ZPL_CONSTEXPR
#	endif
#	if defined( __cplusplus )
#		if __cplusplus >= 201103L
#			define ZPL_CONSTEXPR ZPL_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_( constexpr )
#		endif
#	endif
#	if ! defined( ZPL_CONSTEXPR )
#		define ZPL_CONSTEXPR
#	endif

#	if defined( ZPL_PREDICT )
#		undef ZPL_PREDICT
#	endif
#	if defined( ZPL_LIKELY )
#		undef ZPL_LIKELY
#	endif
#	if defined( ZPL_UNLIKELY )
#		undef ZPL_UNLIKELY
#	endif
#	if defined( ZPL_UNPREDICTABLE )
#		undef ZPL_UNPREDICTABLE
#	endif
#	if ZPL_HAS_BUILTIN( __builtin_unpredictable )
#		define ZPL_UNPREDICTABLE( expr ) __builtin_unpredictable( ( expr ) )
#	endif
#	if ZPL_HAS_BUILTIN( __builtin_expect_with_probability ) || ZPL_GCC_VERSION_CHECK( 9, 0, 0 )
#		define ZPL_PREDICT( expr, value, probability ) __builtin_expect_with_probability( ( expr ), ( value ), ( probability ) )
#		define ZPL_PREDICT_TRUE( expr, probability )   __builtin_expect_with_probability( ! ! ( expr ), 1, ( probability ) )
#		define ZPL_PREDICT_FALSE( expr, probability )  __builtin_expect_with_probability( ! ! ( expr ), 0, ( probability ) )
#		define ZPL_LIKELY( expr )                      __builtin_expect( ! ! ( expr ), 1 )
#		define ZPL_UNLIKELY( expr )                    __builtin_expect( ! ! ( expr ), 0 )
#	elif ZPL_HAS_BUILTIN( __builtin_expect ) || ZPL_GCC_VERSION_CHECK( 3, 0, 0 ) || ZPL_INTEL_VERSION_CHECK( 13, 0, 0 )                                                 \
	    || ( ZPL_SUNPRO_VERSION_CHECK( 5, 15, 0 ) && defined( __cplusplus ) ) || ZPL_ARM_VERSION_CHECK( 4, 1, 0 ) || ZPL_IBM_VERSION_CHECK( 10, 1, 0 )                   \
	    || ZPL_TI_VERSION_CHECK( 15, 12, 0 ) || ZPL_TI_ARMCL_VERSION_CHECK( 4, 7, 0 ) || ZPL_TI_CL430_VERSION_CHECK( 3, 1, 0 ) || ZPL_TI_CL2000_VERSION_CHECK( 6, 1, 0 ) \
	    || ZPL_TI_CL6X_VERSION_CHECK( 6, 1, 0 ) || ZPL_TI_CL7X_VERSION_CHECK( 1, 2, 0 ) || ZPL_TI_CLPRU_VERSION_CHECK( 2, 1, 0 ) || ZPL_TINYC_VERSION_CHECK( 0, 9, 27 )  \
	    || ZPL_CRAY_VERSION_CHECK( 8, 1, 0 )
#		define ZPL_PREDICT( expr, expected, probability ) \
			( ( ( probability ) >= 0.9 ) ? __builtin_expect( ( expr ), ( expected ) ) : ( ZPL_STATIC_CAST( void, expected ), ( expr ) ) )
#		define ZPL_PREDICT_TRUE( expr, probability )                                                                                                                 \
			( __extension__( {                                                                                                                                        \
				double probability_ = ( probability );                                                                                                                \
				( ( probability_ >= 0.9 ) ? __builtin_expect( ! ! ( expr ), 1 ) : ( ( probability_ <= 0.1 ) ? __builtin_expect( ! ! ( expr ), 0 ) : ! ! ( expr ) ) ); \
			} ) )
#		define ZPL_PREDICT_FALSE( expr, probability )                                                                                                                \
			( __extension__( {                                                                                                                                        \
				double probability_ = ( probability );                                                                                                                \
				( ( probability_ >= 0.9 ) ? __builtin_expect( ! ! ( expr ), 0 ) : ( ( probability_ <= 0.1 ) ? __builtin_expect( ! ! ( expr ), 1 ) : ! ! ( expr ) ) ); \
			} ) )
#		define ZPL_LIKELY( expr )   __builtin_expect( ! ! ( expr ), 1 )
#		define ZPL_UNLIKELY( expr ) __builtin_expect( ! ! ( expr ), 0 )
#	else
#		define ZPL_PREDICT( expr, expected, probability ) ( ZPL_STATIC_CAST( void, expected ), ( expr ) )
#		define ZPL_PREDICT_TRUE( expr, probability )      ( ! ! ( expr ) )
#		define ZPL_PREDICT_FALSE( expr, probability )     ( ! ! ( expr ) )
#		define ZPL_LIKELY( expr )                         ( ! ! ( expr ) )
#		define ZPL_UNLIKELY( expr )                       ( ! ! ( expr ) )
#	endif
#	if ! defined( ZPL_UNPREDICTABLE )
#		define ZPL_UNPREDICTABLE( expr ) ZPL_PREDICT( expr, 1, 0.5 )
#	endif

#	if defined( ZPL_MALLOC )
#		undef ZPL_MALLOC
#	endif
#	if ZPL_HAS_ATTRIBUTE( malloc ) || ZPL_GCC_VERSION_CHECK( 3, 1, 0 ) || ZPL_INTEL_VERSION_CHECK( 13, 0, 0 ) || ZPL_SUNPRO_VERSION_CHECK( 5, 11, 0 )                         \
	    || ZPL_ARM_VERSION_CHECK( 4, 1, 0 ) || ZPL_IBM_VERSION_CHECK( 12, 1, 0 ) || ZPL_TI_VERSION_CHECK( 15, 12, 0 )                                                          \
	    || ( ZPL_TI_ARMCL_VERSION_CHECK( 4, 8, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_ARMCL_VERSION_CHECK( 5, 2, 0 )                                       \
	    || ( ZPL_TI_CL2000_VERSION_CHECK( 6, 0, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_CL2000_VERSION_CHECK( 6, 4, 0 )                                     \
	    || ( ZPL_TI_CL430_VERSION_CHECK( 4, 0, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_CL430_VERSION_CHECK( 4, 3, 0 )                                       \
	    || ( ZPL_TI_CL6X_VERSION_CHECK( 7, 2, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_CL6X_VERSION_CHECK( 7, 5, 0 ) || ZPL_TI_CL7X_VERSION_CHECK( 1, 2, 0 ) \
	    || ZPL_TI_CLPRU_VERSION_CHECK( 2, 1, 0 )
#		define ZPL_MALLOC __attribute__( ( __malloc__ ) )
#	elif ZPL_SUNPRO_VERSION_CHECK( 5, 10, 0 )
#		define ZPL_MALLOC _Pragma( "returns_new_memory" )
#	elif ZPL_MSVC_VERSION_CHECK( 14, 0, 0 )
#		define ZPL_MALLOC __declspec( restrict )
#	else
#		define ZPL_MALLOC
#	endif

#	if defined( ZPL_PURE )
#		undef ZPL_PURE
#	endif
#	if ZPL_HAS_ATTRIBUTE( pure ) || ZPL_GCC_VERSION_CHECK( 2, 96, 0 ) || ZPL_INTEL_VERSION_CHECK( 13, 0, 0 ) || ZPL_SUNPRO_VERSION_CHECK( 5, 11, 0 )                          \
	    || ZPL_ARM_VERSION_CHECK( 4, 1, 0 ) || ZPL_IBM_VERSION_CHECK( 10, 1, 0 ) || ZPL_TI_VERSION_CHECK( 15, 12, 0 )                                                          \
	    || ( ZPL_TI_ARMCL_VERSION_CHECK( 4, 8, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_ARMCL_VERSION_CHECK( 5, 2, 0 )                                       \
	    || ( ZPL_TI_CL2000_VERSION_CHECK( 6, 0, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_CL2000_VERSION_CHECK( 6, 4, 0 )                                     \
	    || ( ZPL_TI_CL430_VERSION_CHECK( 4, 0, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_CL430_VERSION_CHECK( 4, 3, 0 )                                       \
	    || ( ZPL_TI_CL6X_VERSION_CHECK( 7, 2, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_CL6X_VERSION_CHECK( 7, 5, 0 ) || ZPL_TI_CL7X_VERSION_CHECK( 1, 2, 0 ) \
	    || ZPL_TI_CLPRU_VERSION_CHECK( 2, 1, 0 ) || ZPL_PGI_VERSION_CHECK( 17, 10, 0 )
#		define ZPL_PURE __attribute__( ( __pure__ ) )
#	elif ZPL_SUNPRO_VERSION_CHECK( 5, 10, 0 )
#		define ZPL_PURE _Pragma( "does_not_write_global_data" )
#	elif defined( __cplusplus ) && ( ZPL_TI_CL430_VERSION_CHECK( 2, 0, 1 ) || ZPL_TI_CL6X_VERSION_CHECK( 4, 0, 0 ) || ZPL_TI_CL7X_VERSION_CHECK( 1, 2, 0 ) )
#		define ZPL_PURE _Pragma( "FUNC_IS_PURE;" )
#	else
#		define ZPL_PURE
#	endif

#	if defined( ZPL_CONST )
#		undef ZPL_CONST
#	endif
#	if ZPL_HAS_ATTRIBUTE( const ) || ZPL_GCC_VERSION_CHECK( 2, 5, 0 ) || ZPL_INTEL_VERSION_CHECK( 13, 0, 0 ) || ZPL_SUNPRO_VERSION_CHECK( 5, 11, 0 )                          \
	    || ZPL_ARM_VERSION_CHECK( 4, 1, 0 ) || ZPL_IBM_VERSION_CHECK( 10, 1, 0 ) || ZPL_TI_VERSION_CHECK( 15, 12, 0 )                                                          \
	    || ( ZPL_TI_ARMCL_VERSION_CHECK( 4, 8, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_ARMCL_VERSION_CHECK( 5, 2, 0 )                                       \
	    || ( ZPL_TI_CL2000_VERSION_CHECK( 6, 0, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_CL2000_VERSION_CHECK( 6, 4, 0 )                                     \
	    || ( ZPL_TI_CL430_VERSION_CHECK( 4, 0, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_CL430_VERSION_CHECK( 4, 3, 0 )                                       \
	    || ( ZPL_TI_CL6X_VERSION_CHECK( 7, 2, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_CL6X_VERSION_CHECK( 7, 5, 0 ) || ZPL_TI_CL7X_VERSION_CHECK( 1, 2, 0 ) \
	    || ZPL_TI_CLPRU_VERSION_CHECK( 2, 1, 0 ) || ZPL_PGI_VERSION_CHECK( 17, 10, 0 )
#		define ZPL_CONST __attribute__( ( __const__ ) )
#	elif ZPL_SUNPRO_VERSION_CHECK( 5, 10, 0 )
#		define ZPL_CONST _Pragma( "no_side_effect" )
#	else
#		define ZPL_CONST ZPL_PURE
#	endif

#	if defined( ZPL_RESTRICT )
#		undef ZPL_RESTRICT
#	endif
#	if defined( __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 199901L ) && ! defined( __cplusplus )
#		define ZPL_RESTRICT restrict
#	elif ZPL_GCC_VERSION_CHECK( 3, 1, 0 ) || ZPL_MSVC_VERSION_CHECK( 14, 0, 0 ) || ZPL_INTEL_VERSION_CHECK( 13, 0, 0 ) || ZPL_ARM_VERSION_CHECK( 4, 1, 0 )           \
	    || ZPL_IBM_VERSION_CHECK( 10, 1, 0 ) || ZPL_PGI_VERSION_CHECK( 17, 10, 0 ) || ZPL_TI_CL430_VERSION_CHECK( 4, 3, 0 ) || ZPL_TI_CL2000_VERSION_CHECK( 6, 2, 4 ) \
	    || ZPL_TI_CL6X_VERSION_CHECK( 8, 1, 0 ) || ZPL_TI_CL7X_VERSION_CHECK( 1, 2, 0 ) || ( ZPL_SUNPRO_VERSION_CHECK( 5, 14, 0 ) && defined( __cplusplus ) )         \
	    || ZPL_IAR_VERSION_CHECK( 8, 0, 0 ) || defined( __clang__ )
#		define ZPL_RESTRICT __restrict
#	elif ZPL_SUNPRO_VERSION_CHECK( 5, 3, 0 ) && ! defined( __cplusplus )
#		define ZPL_RESTRICT _Restrict
#	else
#		define ZPL_RESTRICT
#	endif

#	if defined( ZPL_INLINE )
#		undef ZPL_INLINE
#	endif
#	if ( defined( __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 199901L ) ) || ( defined( __cplusplus ) && ( __cplusplus >= 199711L ) )
#		define ZPL_INLINE inline
#	elif defined( ZPL_GCC_VERSION ) || ZPL_ARM_VERSION_CHECK( 6, 2, 0 )
#		define ZPL_INLINE __inline__
#	elif ZPL_MSVC_VERSION_CHECK( 12, 0, 0 ) || ZPL_ARM_VERSION_CHECK( 4, 1, 0 ) || ZPL_TI_ARMCL_VERSION_CHECK( 5, 1, 0 ) || ZPL_TI_CL430_VERSION_CHECK( 3, 1, 0 ) \
	    || ZPL_TI_CL2000_VERSION_CHECK( 6, 2, 0 ) || ZPL_TI_CL6X_VERSION_CHECK( 8, 0, 0 ) || ZPL_TI_CL7X_VERSION_CHECK( 1, 2, 0 ) || ZPL_TI_CLPRU_VERSION_CHECK( 2, 1, 0 )
#		define ZPL_INLINE __inline
#	else
#		define ZPL_INLINE
#	endif

#	if defined( ZPL_ALWAYS_INLINE )
#		undef ZPL_ALWAYS_INLINE
#	endif
#	if ZPL_HAS_ATTRIBUTE( always_inline ) || ZPL_GCC_VERSION_CHECK( 4, 0, 0 ) || ZPL_INTEL_VERSION_CHECK( 13, 0, 0 ) || ZPL_SUNPRO_VERSION_CHECK( 5, 11, 0 )                  \
	    || ZPL_ARM_VERSION_CHECK( 4, 1, 0 ) || ZPL_IBM_VERSION_CHECK( 10, 1, 0 ) || ZPL_TI_VERSION_CHECK( 15, 12, 0 )                                                          \
	    || ( ZPL_TI_ARMCL_VERSION_CHECK( 4, 8, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_ARMCL_VERSION_CHECK( 5, 2, 0 )                                       \
	    || ( ZPL_TI_CL2000_VERSION_CHECK( 6, 0, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_CL2000_VERSION_CHECK( 6, 4, 0 )                                     \
	    || ( ZPL_TI_CL430_VERSION_CHECK( 4, 0, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_CL430_VERSION_CHECK( 4, 3, 0 )                                       \
	    || ( ZPL_TI_CL6X_VERSION_CHECK( 7, 2, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_CL6X_VERSION_CHECK( 7, 5, 0 ) || ZPL_TI_CL7X_VERSION_CHECK( 1, 2, 0 ) \
	    || ZPL_TI_CLPRU_VERSION_CHECK( 2, 1, 0 )
#		define ZPL_ALWAYS_INLINE __attribute__( ( __always_inline__ ) ) ZPL_INLINE
#	elif ZPL_MSVC_VERSION_CHECK( 12, 0, 0 )
#		define ZPL_ALWAYS_INLINE __forceinline
#	elif defined( __cplusplus )                                                                                                                                              \
	    && ( ZPL_TI_ARMCL_VERSION_CHECK( 5, 2, 0 ) || ZPL_TI_CL430_VERSION_CHECK( 4, 3, 0 ) || ZPL_TI_CL2000_VERSION_CHECK( 6, 4, 0 ) || ZPL_TI_CL6X_VERSION_CHECK( 6, 1, 0 ) \
	         || ZPL_TI_CL7X_VERSION_CHECK( 1, 2, 0 ) || ZPL_TI_CLPRU_VERSION_CHECK( 2, 1, 0 ) )
#		define ZPL_ALWAYS_INLINE _Pragma( "FUNC_ALWAYS_INLINE;" )
#	elif ZPL_IAR_VERSION_CHECK( 8, 0, 0 )
#		define ZPL_ALWAYS_INLINE _Pragma( "inline=forced" )
#	else
#		define ZPL_ALWAYS_INLINE ZPL_INLINE
#	endif

#	undef ZPL_ALWAYS_INLINE
#	define ZPL_ALWAYS_INLINE ZPL_INLINE

#	if defined( ZPL_NEVER_INLINE )
#		undef ZPL_NEVER_INLINE
#	endif
#	if ZPL_HAS_ATTRIBUTE( noinline ) || ZPL_GCC_VERSION_CHECK( 4, 0, 0 ) || ZPL_INTEL_VERSION_CHECK( 13, 0, 0 ) || ZPL_SUNPRO_VERSION_CHECK( 5, 11, 0 )                       \
	    || ZPL_ARM_VERSION_CHECK( 4, 1, 0 ) || ZPL_IBM_VERSION_CHECK( 10, 1, 0 ) || ZPL_TI_VERSION_CHECK( 15, 12, 0 )                                                          \
	    || ( ZPL_TI_ARMCL_VERSION_CHECK( 4, 8, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_ARMCL_VERSION_CHECK( 5, 2, 0 )                                       \
	    || ( ZPL_TI_CL2000_VERSION_CHECK( 6, 0, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_CL2000_VERSION_CHECK( 6, 4, 0 )                                     \
	    || ( ZPL_TI_CL430_VERSION_CHECK( 4, 0, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_CL430_VERSION_CHECK( 4, 3, 0 )                                       \
	    || ( ZPL_TI_CL6X_VERSION_CHECK( 7, 2, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_CL6X_VERSION_CHECK( 7, 5, 0 ) || ZPL_TI_CL7X_VERSION_CHECK( 1, 2, 0 ) \
	    || ZPL_TI_CLPRU_VERSION_CHECK( 2, 1, 0 )
#		define ZPL_NEVER_INLINE __attribute__( ( __noinline__ ) )
#	elif ZPL_MSVC_VERSION_CHECK( 13, 10, 0 )
#		define ZPL_NEVER_INLINE __declspec( noinline )
#	elif ZPL_PGI_VERSION_CHECK( 10, 2, 0 )
#		define ZPL_NEVER_INLINE _Pragma( "noinline" )
#	elif ZPL_TI_CL6X_VERSION_CHECK( 6, 0, 0 ) && defined( __cplusplus )
#		define ZPL_NEVER_INLINE _Pragma( "FUNC_CANNOT_INLINE;" )
#	elif ZPL_IAR_VERSION_CHECK( 8, 0, 0 )
#		define ZPL_NEVER_INLINE _Pragma( "inline=never" )
#	elif ZPL_COMPCERT_VERSION_CHECK( 3, 2, 0 )
#		define ZPL_NEVER_INLINE __attribute( ( noinline ) )
#	elif ZPL_PELLES_VERSION_CHECK( 9, 0, 0 )
#		define ZPL_NEVER_INLINE __declspec( noinline )
#	else
#		define ZPL_NEVER_INLINE
#	endif

#	if defined( ZPL_PRIVATE )
#		undef ZPL_PRIVATE
#	endif
#	if defined( ZPL_PUBLIC )
#		undef ZPL_PUBLIC
#	endif
#	if defined( ZPL_IMPORT )
#		undef ZPL_IMPORT
#	endif
#	if defined( _WIN32 ) || defined( __CYGWIN__ )
#		define ZPL_PRIVATE
#		define ZPL_PUBLIC __declspec( dllexport )
#		define ZPL_IMPORT __declspec( dllimport )
#	else
#		if ZPL_HAS_ATTRIBUTE( visibility ) || ZPL_GCC_VERSION_CHECK( 3, 3, 0 ) || ZPL_SUNPRO_VERSION_CHECK( 5, 11, 0 ) || ZPL_INTEL_VERSION_CHECK( 13, 0, 0 ) \
		    || ZPL_ARM_VERSION_CHECK( 4, 1, 0 ) || ZPL_IBM_VERSION_CHECK( 13, 1, 0 )                                                                           \
		    || ( defined( __TI_EABI__ ) && ( ( ZPL_TI_CL6X_VERSION_CHECK( 7, 2, 0 ) && defined( __TI_GNU_ATTRIBUTE_SUPPORT__ ) ) || ZPL_TI_CL6X_VERSION_CHECK( 7, 5, 0 ) ) )
#			define ZPL_PRIVATE __attribute__( ( __visibility__( "hidden" ) ) )
#			define ZPL_PUBLIC  __attribute__( ( __visibility__( "default" ) ) )
#		else
#			define ZPL_PRIVATE
#			define ZPL_PUBLIC
#		endif
#		define ZPL_IMPORT extern
#	endif

#	if defined( ZPL_NO_THROW )
#		undef ZPL_NO_THROW
#	endif
#	if ZPL_HAS_ATTRIBUTE( nothrow ) || ZPL_GCC_VERSION_CHECK( 3, 3, 0 ) || ZPL_INTEL_VERSION_CHECK( 13, 0, 0 )
#		define ZPL_NO_THROW __attribute__( ( __nothrow__ ) )
#	elif ZPL_MSVC_VERSION_CHECK( 13, 1, 0 ) || ZPL_ARM_VERSION_CHECK( 4, 1, 0 )
#		define ZPL_NO_THROW __declspec( nothrow )
#	else
#		define ZPL_NO_THROW
#	endif

#	if defined( ZPL_FALL_THROUGH )
#		undef ZPL_FALL_THROUGH
#	endif
#	if ZPL_GNUC_HAS_ATTRIBUTE( fallthrough, 7, 0, 0 ) && ! defined( ZPL_PGI_VERSION )
#		define ZPL_FALL_THROUGH __attribute__( ( __fallthrough__ ) )
#	elif ZPL_HAS_CPP_ATTRIBUTE_NS( clang, fallthrough )
#		define ZPL_FALL_THROUGH ZPL_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_( [[clang::fallthrough]] )
#	elif ZPL_HAS_CPP_ATTRIBUTE( fallthrough )
#		define ZPL_FALL_THROUGH ZPL_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_( [[fallthrough]] )
#	elif defined( __fallthrough ) /* SAL */
#		define ZPL_FALL_THROUGH __fallthrough
#	else
#		define ZPL_FALL_THROUGH
#	endif

#	if defined( ZPL_RETURNS_NON_NULL )
#		undef ZPL_RETURNS_NON_NULL
#	endif
#	if ZPL_HAS_ATTRIBUTE( returns_nonnull ) || ZPL_GCC_VERSION_CHECK( 4, 9, 0 )
#		define ZPL_RETURNS_NON_NULL __attribute__( ( __returns_nonnull__ ) )
#	elif defined( _Ret_notnull_ ) /* SAL */
#		define ZPL_RETURNS_NON_NULL _Ret_notnull_
#	else
#		define ZPL_RETURNS_NON_NULL
#	endif

#	if defined( ZPL_ARRAY_PARAM )
#		undef ZPL_ARRAY_PARAM
#	endif
#	if defined( __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 199901L ) && ! defined( __STDC_NO_VLA__ ) && ! defined( __cplusplus ) && ! defined( ZPL_PGI_VERSION ) \
	    && ! defined( ZPL_TINYC_VERSION )
#		define ZPL_ARRAY_PARAM( name ) ( name )
#	else
#		define ZPL_ARRAY_PARAM( name )
#	endif

#	if defined( ZPL_IS_CONSTANT )
#		undef ZPL_IS_CONSTANT
#	endif
#	if defined( ZPL_REQUIRE_CONSTEXPR )
#		undef ZPL_REQUIRE_CONSTEXPR
#	endif
/* ZPL_IS_CONSTEXPR_ is for
   ZPL INTERNAL USE ONLY.  API subject to change without notice. */
#	if defined( ZPL_IS_CONSTEXPR_ )
#		undef ZPL_IS_CONSTEXPR_
#	endif
#	if ZPL_HAS_BUILTIN( __builtin_constant_p ) || ZPL_GCC_VERSION_CHECK( 3, 4, 0 ) || ZPL_INTEL_VERSION_CHECK( 13, 0, 0 ) || ZPL_TINYC_VERSION_CHECK( 0, 9, 19 ) \
	    || ZPL_ARM_VERSION_CHECK( 4, 1, 0 ) || ZPL_IBM_VERSION_CHECK( 13, 1, 0 ) || ZPL_TI_CL6X_VERSION_CHECK( 6, 1, 0 )                                          \
	    || ( ZPL_SUNPRO_VERSION_CHECK( 5, 10, 0 ) && ! defined( __cplusplus ) ) || ZPL_CRAY_VERSION_CHECK( 8, 1, 0 )
#		define ZPL_IS_CONSTANT( expr ) __builtin_constant_p( expr )
#	endif
#	if ! defined( __cplusplus )
#		if ZPL_HAS_BUILTIN( __builtin_types_compatible_p ) || ZPL_GCC_VERSION_CHECK( 3, 4, 0 ) || ZPL_INTEL_VERSION_CHECK( 13, 0, 0 ) || ZPL_IBM_VERSION_CHECK( 13, 1, 0 ) \
		    || ZPL_CRAY_VERSION_CHECK( 8, 1, 0 ) || ZPL_ARM_VERSION_CHECK( 5, 4, 0 ) || ZPL_TINYC_VERSION_CHECK( 0, 9, 24 )
#			if defined( __INTPTR_TYPE__ )
#				define ZPL_IS_CONSTEXPR_( expr ) __builtin_types_compatible_p( __typeof__( ( 1 ? ( void* )( ( __INTPTR_TYPE__ )( ( expr )*0 ) ) : ( int* )0 ) ), int* )
#			else
#				include <stdint.h>
#				define ZPL_IS_CONSTEXPR_( expr ) __builtin_types_compatible_p( __typeof__( ( 1 ? ( void* )( ( intptr_t )( ( expr )*0 ) ) : ( int* )0 ) ), int* )
#			endif
#		elif (                                                                                                                                                               \
		    defined( __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 201112L ) && ! defined( ZPL_SUNPRO_VERSION ) && ! defined( ZPL_PGI_VERSION ) && ! defined( ZPL_IAR_VERSION ) \
		) || ZPL_HAS_EXTENSION( c_generic_selections )                                                                                                                        \
		    || ZPL_GCC_VERSION_CHECK( 4, 9, 0 ) || ZPL_INTEL_VERSION_CHECK( 17, 0, 0 ) || ZPL_IBM_VERSION_CHECK( 12, 1, 0 ) || ZPL_ARM_VERSION_CHECK( 5, 3, 0 )
#			if defined( __INTPTR_TYPE__ )
#				define ZPL_IS_CONSTEXPR_( expr ) _Generic( ( 1 ? ( void* )( ( __INTPTR_TYPE__ )( ( expr )*0 ) ) : ( int* )0 ), int*: 1, void*: 0 )
#			else
#				include <stdint.h>
#				define ZPL_IS_CONSTEXPR_( expr ) _Generic( ( 1 ? ( void* )( ( intptr_t )*0 ) : ( int* )0 ), int*: 1, void*: 0 )
#			endif
#		elif defined( ZPL_GCC_VERSION ) || defined( ZPL_INTEL_VERSION ) || defined( ZPL_TINYC_VERSION ) || defined( ZPL_TI_ARMCL_VERSION )                    \
		    || ZPL_TI_CL430_VERSION_CHECK( 18, 12, 0 ) || defined( ZPL_TI_CL2000_VERSION ) || defined( ZPL_TI_CL6X_VERSION ) || defined( ZPL_TI_CL7X_VERSION ) \
		    || defined( ZPL_TI_CLPRU_VERSION ) || defined( __clang__ )
#			define ZPL_IS_CONSTEXPR_( expr ) ( sizeof( void ) != sizeof( *( 1 ? ( ( void* )( ( expr )*0L ) ) : ( ( struct { char v[ sizeof( void ) * 2 ]; }* )1 ) ) ) )
#		endif
#	endif
#	if defined( ZPL_IS_CONSTEXPR_ )
#		if ! defined( ZPL_IS_CONSTANT )
#			define ZPL_IS_CONSTANT( expr ) ZPL_IS_CONSTEXPR_( expr )
#		endif
#		define ZPL_REQUIRE_CONSTEXPR( expr ) ( ZPL_IS_CONSTEXPR_( expr ) ? ( expr ) : ( -1 ) )
#	else
#		if ! defined( ZPL_IS_CONSTANT )
#			define ZPL_IS_CONSTANT( expr ) ( 0 )
#		endif
#		define ZPL_REQUIRE_CONSTEXPR( expr ) ( expr )
#	endif

#	if defined( ZPL_BEGIN_C_DECLS )
#		undef ZPL_BEGIN_C_DECLS
#	endif
#	if defined( ZPL_END_C_DECLS )
#		undef ZPL_END_C_DECLS
#	endif
#	if defined( ZPL_C_DECL )
#		undef ZPL_C_DECL
#	endif
#	if defined( __cplusplus ) && ! defined( ZPL_DISABLE_C_DECLS )
#		define ZPL_BEGIN_C_DECLS \
			extern "C"            \
			{
#		define ZPL_END_C_DECLS }
#		define ZPL_C_DECL      extern "C"
#	else
#		define ZPL_BEGIN_C_DECLS
#		define ZPL_END_C_DECLS
#		define ZPL_C_DECL
#	endif

#	if defined( ZPL_STATIC_ASSERT )
#		undef ZPL_STATIC_ASSERT
#	endif
#	if ! defined( __cplusplus )                                                                                                                          \
	    && ( ( defined( __STDC_VERSION__ ) && ( __STDC_VERSION__ >= 201112L ) ) || ZPL_HAS_FEATURE( c_static_assert ) || ZPL_GCC_VERSION_CHECK( 6, 0, 0 ) \
	         || ZPL_INTEL_VERSION_CHECK( 13, 0, 0 ) || defined( _Static_assert ) )
#		define ZPL_STATIC_ASSERT( expr, message ) _Static_assert( expr, message )
#	elif ( defined( __cplusplus ) && ( __cplusplus >= 201103L ) ) || ZPL_MSVC_VERSION_CHECK( 16, 0, 0 )
#		define ZPL_STATIC_ASSERT( expr, message ) ZPL_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_( static_assert( expr, message ) )
#	else
#		define ZPL_STATIC_ASSERT3( cond, msg )   typedef char static_assertion_##msg[ ( ! ! ( cond ) ) * 2 - 1 ]
#		define ZPL_STATIC_ASSERT2( cond, line )  ZPL_STATIC_ASSERT3( cond, static_assertion_at_line_##line )
#		define ZPL_STATIC_ASSERT1( cond, line )  ZPL_STATIC_ASSERT2( cond, line )
#		define ZPL_STATIC_ASSERT( cond, unused ) ZPL_STATIC_ASSERT1( cond, __LINE__ )
#	endif

#	if defined( ZPL_NULL )
#		undef ZPL_NULL
#	endif
#	if defined( __cplusplus )
#		if __cplusplus >= 201103L
#			define ZPL_NULL ZPL_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_( nullptr )
#		elif defined( NULL )
#			define ZPL_NULL NULL
#		else
#			define ZPL_NULL ZPL_STATIC_CAST( void*, 0 )
#		endif
#	elif defined( NULL )
#		define ZPL_NULL NULL
#	else
#		define ZPL_NULL ( ( void* )0 )
#	endif

#	if defined( ZPL_MESSAGE )
#		undef ZPL_MESSAGE
#	endif
#	if ZPL_HAS_WARNING( "-Wunknown-pragmas" )
#		define ZPL_MESSAGE( msg )                 \
			ZPL_DIAGNOSTIC_PUSH                    \
			ZPL_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS \
			ZPL_PRAGMA( message msg )              \
			ZPL_DIAGNOSTIC_POP
#	elif ZPL_GCC_VERSION_CHECK( 4, 4, 0 ) || ZPL_INTEL_VERSION_CHECK( 13, 0, 0 )
#		define ZPL_MESSAGE( msg ) ZPL_PRAGMA( message msg )
#	elif ZPL_CRAY_VERSION_CHECK( 5, 0, 0 )
#		define ZPL_MESSAGE( msg ) ZPL_PRAGMA( _CRI message msg )
#	elif ZPL_IAR_VERSION_CHECK( 8, 0, 0 )
#		define ZPL_MESSAGE( msg ) ZPL_PRAGMA( message( msg ) )
#	elif ZPL_PELLES_VERSION_CHECK( 2, 0, 0 )
#		define ZPL_MESSAGE( msg ) ZPL_PRAGMA( message( msg ) )
#	else
#		define ZPL_MESSAGE( msg )
#	endif

#	if defined( ZPL_WARNING )
#		undef ZPL_WARNING
#	endif
#	if ZPL_HAS_WARNING( "-Wunknown-pragmas" )
#		define ZPL_WARNING( msg )                 \
			ZPL_DIAGNOSTIC_PUSH                    \
			ZPL_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS \
			ZPL_PRAGMA( clang warning msg )        \
			ZPL_DIAGNOSTIC_POP
#	elif ZPL_GCC_VERSION_CHECK( 4, 8, 0 ) || ZPL_PGI_VERSION_CHECK( 18, 4, 0 ) || ZPL_INTEL_VERSION_CHECK( 13, 0, 0 )
#		define ZPL_WARNING( msg ) ZPL_PRAGMA( GCC warning msg )
#	elif ZPL_MSVC_VERSION_CHECK( 15, 0, 0 )
#		define ZPL_WARNING( msg ) ZPL_PRAGMA( message( msg ) )
#	else
#		define ZPL_WARNING( msg ) ZPL_MESSAGE( msg )
#	endif

#	if defined( ZPL_REQUIRE )
#		undef ZPL_REQUIRE
#	endif
#	if defined( ZPL_REQUIRE_MSG )
#		undef ZPL_REQUIRE_MSG
#	endif
#	if ZPL_HAS_ATTRIBUTE( diagnose_if )
#		if ZPL_HAS_WARNING( "-Wgcc-compat" )
#			define ZPL_REQUIRE( expr ) \
				ZPL_DIAGNOSTIC_PUSH     \
				_Pragma( "clang diagnostic ignored \"-Wgcc-compat\"" ) __attribute__( ( diagnose_if( ! ( expr ), #expr, "error" ) ) ) ZPL_DIAGNOSTIC_POP
#			define ZPL_REQUIRE_MSG( expr, msg ) \
				ZPL_DIAGNOSTIC_PUSH              \
				_Pragma( "clang diagnostic ignored \"-Wgcc-compat\"" ) __attribute__( ( diagnose_if( ! ( expr ), msg, "error" ) ) ) ZPL_DIAGNOSTIC_POP
#		else
#			define ZPL_REQUIRE( expr )          __attribute__( ( diagnose_if( ! ( expr ), #expr, "error" ) ) )
#			define ZPL_REQUIRE_MSG( expr, msg ) __attribute__( ( diagnose_if( ! ( expr ), msg, "error" ) ) )
#		endif
#	else
#		define ZPL_REQUIRE( expr )
#		define ZPL_REQUIRE_MSG( expr, msg )
#	endif

#	if defined( ZPL_FLAGS )
#		undef ZPL_FLAGS
#	endif
#	if ZPL_HAS_ATTRIBUTE( flag_enum )
#		define ZPL_FLAGS __attribute__( ( __flag_enum__ ) )
#	endif

#	if defined( ZPL_FLAGS_CAST )
#		undef ZPL_FLAGS_CAST
#	endif
#	if ZPL_INTEL_VERSION_CHECK( 19, 0, 0 )
#		define ZPL_FLAGS_CAST( T, expr )                           \
			( __extension__( {                                      \
				ZPL_DIAGNOSTIC_PUSH                                 \
				_Pragma( "warning(disable:188)" )( ( T )( expr ) ); \
				ZPL_DIAGNOSTIC_POP                                  \
			} ) )
#	else
#		define ZPL_FLAGS_CAST( T, expr ) ZPL_STATIC_CAST( T, expr )
#	endif

#	if defined( ZPL_EMPTY_BASES )
#		undef ZPL_EMPTY_BASES
#	endif
#	if ZPL_MSVC_VERSION_CHECK( 19, 0, 23918 ) && ! ZPL_MSVC_VERSION_CHECK( 20, 0, 0 )
#		define ZPL_EMPTY_BASES __declspec( empty_bases )
#	else
#		define ZPL_EMPTY_BASES
#	endif

/* Remaining macros are deprecated. */

#	if defined( ZPL_GCC_NOT_CLANG_VERSION_CHECK )
#		undef ZPL_GCC_NOT_CLANG_VERSION_CHECK
#	endif
#	if defined( __clang__ )
#		define ZPL_GCC_NOT_CLANG_VERSION_CHECK( major, minor, patch ) ( 0 )
#	else
#		define ZPL_GCC_NOT_CLANG_VERSION_CHECK( major, minor, patch ) ZPL_GCC_VERSION_CHECK( major, minor, patch )
#	endif

#	if defined( ZPL_CLANG_HAS_ATTRIBUTE )
#		undef ZPL_CLANG_HAS_ATTRIBUTE
#	endif
#	define ZPL_CLANG_HAS_ATTRIBUTE( attribute ) ZPL_HAS_ATTRIBUTE( attribute )

#	if defined( ZPL_CLANG_HAS_CPP_ATTRIBUTE )
#		undef ZPL_CLANG_HAS_CPP_ATTRIBUTE
#	endif
#	define ZPL_CLANG_HAS_CPP_ATTRIBUTE( attribute ) ZPL_HAS_CPP_ATTRIBUTE( attribute )

#	if defined( ZPL_CLANG_HAS_BUILTIN )
#		undef ZPL_CLANG_HAS_BUILTIN
#	endif
#	define ZPL_CLANG_HAS_BUILTIN( builtin ) ZPL_HAS_BUILTIN( builtin )

#	if defined( ZPL_CLANG_HAS_FEATURE )
#		undef ZPL_CLANG_HAS_FEATURE
#	endif
#	define ZPL_CLANG_HAS_FEATURE( feature ) ZPL_HAS_FEATURE( feature )

#	if defined( ZPL_CLANG_HAS_EXTENSION )
#		undef ZPL_CLANG_HAS_EXTENSION
#	endif
#	define ZPL_CLANG_HAS_EXTENSION( extension ) ZPL_HAS_EXTENSION( extension )

#	if defined( ZPL_CLANG_HAS_DECLSPEC_DECLSPEC_ATTRIBUTE )
#		undef ZPL_CLANG_HAS_DECLSPEC_DECLSPEC_ATTRIBUTE
#	endif
#	define ZPL_CLANG_HAS_DECLSPEC_ATTRIBUTE( attribute ) ZPL_HAS_DECLSPEC_ATTRIBUTE( attribute )

#	if defined( ZPL_CLANG_HAS_WARNING )
#		undef ZPL_CLANG_HAS_WARNING
#	endif
#	define ZPL_CLANG_HAS_WARNING( warning ) ZPL_HAS_WARNING( warning )

#endif /* !defined(ZPL_HEDLEY_VERSION) || (ZPL_HEDLEY_VERSION < X) */


#	define ZPL_VERSION ZPL_VERSION_ENCODE( ZPL_VERSION_MAJOR, ZPL_VERSION_MINOR, ZPL_VERSION_PATCH )

#	ifdef ZPL_IMPL
#		ifndef ZPL_IMPLEMENTATION
#			define ZPL_IMPLEMENTATION
#		endif
#	endif

#	if defined( __cplusplus ) && defined( ZPL_WRAP_IN_NAMESPACE )
#		define ZPL_BEGIN_NAMESPACE \
			namespace zpl           \
			{
#		define ZPL_END_NAMESPACE    }
#		define ZPL_NS( Identifier ) zpl::##Identifier    // Used in macros, properly exposes symbol anywhere
#	else
#		define ZPL_BEGIN_NAMESPACE
#		define ZPL_END_NAMESPACE
#		define ZPL_NS( Identifier ) Identifier    // Used in macros, properly exposes symbol anywhere
#	endif

#	if defined( __cplusplus ) && ! defined( ZPL_EXTERN ) && ! defined( ZPL_DISABLE_C_DECLS )
#		define ZPL_EXTERN extern "C"
#	else
#		define ZPL_EXTERN extern
#	endif

#	ifndef ZPL_DEF
#		if defined( ZPL_SHARED_LIB )
#			ifdef ZPL_IMPLEMENTATION
#				define ZPL_DEF ZPL_PUBLIC
#			else
#				define ZPL_DEF ZPL_IMPORT
#			endif
#		elif defined( ZPL_STATIC_LIB )
#			ifdef ZPL_IMPLEMENTATION
#				define ZPL_DEF
#			else
#				define ZPL_DEF ZPL_EXTERN
#			endif
#		elif defined( ZPL_STATIC )
#			define ZPL_DEF static
#		else
#			define ZPL_DEF ZPL_EXTERN
#		endif
#	endif

#	ifndef ZPL_DEF_INLINE
#		if defined( ZPL_STATIC )
#			define ZPL_DEF_INLINE
#			define ZPL_IMPL_INLINE
#		else
#			define ZPL_DEF_INLINE  static
#			define ZPL_IMPL_INLINE static inline
#		endif
#	endif

/* builtin overrides */
#	if defined( __TINYC__ ) || defined( __EMSCRIPTEN__ )
#		if defined( ZPL_ENFORCE_THREADING )
#			define ZPL_ENABLE_THREADING
#		else
#			define ZPL_DISABLE_THREADING
#		endif
#	endif

/* Distributions */
#	ifndef ZPL_CUSTOM_MODULES
/* default distribution */
#		define ZPL_MODULE_ESSENTIALS
#		define ZPL_MODULE_CORE
#		define ZPL_MODULE_TIMER
#		define ZPL_MODULE_HASHING
#		define ZPL_MODULE_REGEX
#		define ZPL_MODULE_EVENT
#		define ZPL_MODULE_DLL
#		define ZPL_MODULE_OPTS
#		define ZPL_MODULE_PROCESS
#		define ZPL_MODULE_MATH
#		define ZPL_MODULE_THREADING
#		define ZPL_MODULE_JOBS
#		define ZPL_MODULE_PARSER

/* zpl nano distribution */
#		if defined( ZPL_NANO ) || defined( ZPL_PICO )
#			undef ZPL_MODULE_TIMER
#			undef ZPL_MODULE_HASHING
#			undef ZPL_MODULE_REGEX
#			undef ZPL_MODULE_EVENT
#			undef ZPL_MODULE_DLL
#			undef ZPL_MODULE_OPTS
#			undef ZPL_MODULE_PROCESS
#			undef ZPL_MODULE_MATH
#			undef ZPL_MODULE_THREADING
#			undef ZPL_MODULE_JOBS
#			undef ZPL_MODULE_PARSER
#		endif

#		if defined( ZPL_PICO )
#			undef ZPL_MODULE_CORE
#		endif

/* module enabling overrides */
#		if defined( ZPL_ENABLE_CORE ) && ! defined( ZPL_MODULE_CORE )
#			define ZPL_MODULE_CORE
#		endif
#		if defined( ZPL_ENABLE_HASHING ) && ! defined( ZPL_MODULE_HASHING )
#			define ZPL_MODULE_HASHING
#		endif
#		if defined( ZPL_ENABLE_REGEX ) && ! defined( ZPL_MODULE_REGEX )
#			define ZPL_MODULE_REGEX
#		endif
#		if defined( ZPL_ENABLE_DLL ) && ! defined( ZPL_MODULE_DLL )
#			define ZPL_MODULE_DLL
#		endif
#		if defined( ZPL_ENABLE_OPTS ) && ! defined( ZPL_MODULE_OPTS )
#			define ZPL_MODULE_OPTS
#		endif
#		if defined( ZPL_ENABLE_PROCESS ) && ! defined( ZPL_MODULE_PROCESS )
#			define ZPL_MODULE_PROCESS
#		endif
#		if defined( ZPL_ENABLE_MATH ) && ! defined( ZPL_MODULE_MATH )
#			define ZPL_MODULE_MATH
#		endif
#		if defined( ZPL_ENABLE_THREADING ) && ! defined( ZPL_MODULE_THREADING )
#			define ZPL_MODULE_THREADING
#		endif
#		if defined( ZPL_ENABLE_JOBS ) && ! defined( ZPL_MODULE_JOBS )
#			ifndef ZPL_MODULE_THREADING
#				define ZPL_MODULE_THREADING /* dependency */
#			endif
#			define ZPL_MODULE_JOBS
#		endif
#		if defined( ZPL_ENABLE_PARSER ) && ! defined( ZPL_MODULE_PARSER )
#			define ZPL_MODULE_PARSER
#		endif

/* module disabling overrides */
#		if defined( ZPL_DISABLE_CORE ) && defined( ZPL_MODULE_CORE )
#			undef ZPL_MODULE_CORE
#		endif
#		if defined( ZPL_DISABLE_HASHING ) && defined( ZPL_MODULE_HASHING )
#			undef ZPL_MODULE_HASHING
#		endif
#		if defined( ZPL_DISABLE_REGEX ) && defined( ZPL_MODULE_REGEX )
#			undef ZPL_MODULE_REGEX
#		endif
#		if defined( ZPL_DISABLE_DLL ) && defined( ZPL_MODULE_DLL )
#			undef ZPL_MODULE_DLL
#		endif
#		if defined( ZPL_DISABLE_OPTS ) && defined( ZPL_MODULE_OPTS )
#			undef ZPL_MODULE_OPTS
#		endif
#		if defined( ZPL_DISABLE_PROCESS ) && defined( ZPL_MODULE_PROCESS )
#			undef ZPL_MODULE_PROCESS
#		endif
#		if defined( ZPL_DISABLE_MATH ) && defined( ZPL_MODULE_MATH )
#			undef ZPL_MODULE_MATH
#		endif
#		if defined( ZPL_DISABLE_THREADING ) && defined( ZPL_MODULE_THREADING )
#			ifdef ZPL_MODULE_JOBS
#				undef ZPL_MODULE_JOBS /* user */
#			endif
#			undef ZPL_MODULE_THREADING
#		endif
#		if defined( ZPL_DISABLE_JOBS ) && defined( ZPL_MODULE_JOBS )
#			undef ZPL_MODULE_JOBS
#		endif
#		if defined( ZPL_DISABLE_PARSER ) && defined( ZPL_MODULE_PARSER )
#			undef ZPL_MODULE_PARSER
#		endif
#	endif

#	if defined( __GCC__ ) || defined( __GNUC__ ) || defined( __clang__ )
#		pragma GCC diagnostic push
#		pragma GCC diagnostic ignored "-Wunused-function"
#		pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#		pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#	endif

#	if defined( _MSC_VER )
#		pragma warning( push )
#		pragma warning( disable : 4201 )
#		pragma warning( disable : 4127 )    // Conditional expression is constant
#	endif

/* general purpose includes */

// file: header/core/system.h


ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

/* Platform architecture */

#if defined( _WIN64 ) || defined( __x86_64__ ) || defined( _M_X64 ) || defined( __64BIT__ ) || defined( __powerpc64__ ) || defined( __ppc64__ ) || defined( __aarch64__ )
#	ifndef ZPL_ARCH_64_BIT
#		define ZPL_ARCH_64_BIT 1
#	endif
#else
#	ifndef ZPL_ARCH_32_BIT
#		define ZPL_ARCH_32_BIT 1
#	endif
#endif

/* Platform endiannes */

#ifndef ZPL_ENDIAN_ORDER
#	define ZPL_ENDIAN_ORDER
#	define ZPL_IS_BIG_ENDIAN    ( ! *( u8* )&( u16 ) { 1 } )
#	define ZPL_IS_LITTLE_ENDIAN ( ! ZPL_IS_BIG_ENDIAN )
#endif

/* Platform OS */

#if defined( _WIN32 ) || defined( _WIN64 )
#	ifndef ZPL_SYSTEM_WINDOWS
#		define ZPL_SYSTEM_WINDOWS 1
#	endif
#elif defined( __APPLE__ ) && defined( __MACH__ )
#	ifndef ZPL_SYSTEM_OSX
#		define ZPL_SYSTEM_OSX 1
#	endif
#	ifndef ZPL_SYSTEM_MACOS
#		define ZPL_SYSTEM_MACOS 1
#	endif
#	include <TargetConditionals.h>
#	if TARGET_IPHONE_SIMULATOR == 1 || TARGET_OS_IPHONE == 1
#		ifndef ZPL_SYSTEM_IOS
#			define ZPL_SYSTEM_IOS 1
#		endif
#	endif
#elif defined( __unix__ )
#	ifndef ZPL_SYSTEM_UNIX
#		define ZPL_SYSTEM_UNIX 1
#	endif
#	if defined( ANDROID ) || defined( __ANDROID__ )
#		ifndef ZPL_SYSTEM_ANDROID
#			define ZPL_SYSTEM_ANDROID 1
#		endif
#		ifndef ZPL_SYSTEM_LINUX
#			define ZPL_SYSTEM_LINUX 1
#		endif
#	elif defined( __linux__ )
#		ifndef ZPL_SYSTEM_LINUX
#			define ZPL_SYSTEM_LINUX 1
#		endif
#	elif defined( __FreeBSD__ ) || defined( __FreeBSD_kernel__ )
#		ifndef ZPL_SYSTEM_FREEBSD
#			define ZPL_SYSTEM_FREEBSD 1
#		endif
#	elif defined( __OpenBSD__ )
#		ifndef ZPL_SYSTEM_OPENBSD
#			define ZPL_SYSTEM_OPENBSD 1
#		endif
#	elif defined( __EMSCRIPTEN__ )
#		ifndef ZPL_SYSTEM_EMSCRIPTEN
#			define ZPL_SYSTEM_EMSCRIPTEN 1
#		endif
#	elif defined( __CYGWIN__ )
#		ifndef ZPL_SYSTEM_CYGWIN
#			define ZPL_SYSTEM_CYGWIN 1
#		endif
#	else
#		error This UNIX operating system is not supported
#	endif
#else
#	error This operating system is not supported
#endif

/* Platform compiler */

#if defined( _MSC_VER )
#	define ZPL_COMPILER_MSVC 1
#elif defined( __GNUC__ )
#	define ZPL_COMPILER_GCC 1
#elif defined( __clang__ )
#	define ZPL_COMPILER_CLANG 1
#elif defined( __MINGW32__ )
#	define ZPL_COMPILER_MINGW 1
#elif defined( __TINYC__ )
#	define ZPL_COMPILER_TINYC 1
#else
#	error Unknown compiler
#endif

/* Platform CPU */

#if defined( __arm__ ) || defined( __aarch64__ ) || defined( __ARM_ARCH )
#	ifndef ZPL_CPU_ARM
#		define ZPL_CPU_ARM 1
#	endif
#	ifndef ZPL_CACHE_LINE_SIZE
#		define ZPL_CACHE_LINE_SIZE 64
#	endif
#elif defined( _M_IX86 ) || defined( _M_X64 ) || defined( __i386__ ) || defined( __x86_64__ ) || defined( ZPL_SYSTEM_EMSCRIPTEN )
#	ifndef ZPL_CPU_X86
#		define ZPL_CPU_X86 1
#	endif
#	ifndef ZPL_CACHE_LINE_SIZE
#		define ZPL_CACHE_LINE_SIZE 64
#	endif
#elif defined( _M_PPC ) || defined( __powerpc__ ) || defined( __powerpc64__ )
#	ifndef ZPL_CPU_PPC
#		define ZPL_CPU_PPC 1
#	endif
#	ifndef ZPL_CACHE_LINE_SIZE
#		define ZPL_CACHE_LINE_SIZE 128
#	endif
#elif defined( __MIPSEL__ ) || defined( __mips_isa_rev )
#	ifndef ZPL_CPU_MIPS
#		define ZPL_CPU_MIPS 1
#	endif
#	ifndef ZPL_CACHE_LINE_SIZE
#		define ZPL_CACHE_LINE_SIZE 64
#	endif
#else
#	error Unknown CPU Type
#endif

// TODO(ZaKlaus): Find a better way to get this flag in MinGW.
#if ( defined( ZPL_COMPILER_GCC ) && ! defined( WC_ERR_INVALID_CHARS ) ) || defined( ZPL_COMPILER_TINYC )
#	define WC_ERR_INVALID_CHARS 0x0080
#endif

#if defined( ZPL_COMPILER_GCC ) && defined( ZPL_SYSTEM_WINDOWS )
#	ifndef ZPL_COMPILER_MINGW
#		define ZPL_COMPILER_MINGW    // assume we use mingw as a compiler
#	endif
#endif

#if defined( ZPL_SYSTEM_UNIX )
#	ifndef _GNU_SOURCE
#		define _GNU_SOURCE
#	endif

#	ifndef _LARGEFILE64_SOURCE
#		define _LARGEFILE64_SOURCE
#	endif
#endif

#if ZPL_GNUC_VERSION_CHECK( 3, 3, 0 )
#	define ZPL_INFINITY ( __builtin_inff() )
#	define ZPL_NAN      ( __builtin_nanf( "" ) )
#elif defined( ZPL_COMPILER_MSVC )

#	if ! defined( ZPL__HACK_INFINITY )
typedef union _msvc_inf_hack
{
	unsigned __int8 bytes[ 4 ];
	float           value;
} _msvc_inf_hack;
static union _msvc_inf_hack ZPL__INFINITY_HACK = {
	{0x00, 0x00, 0x80, 0x7F}
};
#		define ZPL__HACK_INFINITY ( ZPL_NS( ZPL__INFINITY_HACK ).value )
#	endif

#	define ZPL_INFINITY ( ZPL__HACK_INFINITY )
#	define ZPL_NAN      ( 0 )
#else
#	define ZPL_INFINITY ( 1e10000f )
#	define ZPL_NAN      ( 0.0f / 0.0f )
#endif

ZPL_END_C_DECLS
ZPL_END_NAMESPACE


#	include <stdarg.h>
#	include <stddef.h>

#	if defined( ZPL_SYSTEM_WINDOWS )
#		include <intrin.h>
#	endif

// file: header/essentials/helpers.h

/* Various macro based helpers */

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

#ifndef zpl_cast
#	define zpl_cast( Type ) ( Type )
#endif

#ifndef size_of
#	define size_of( x ) ( ZPL_NS( sw ) )( sizeof( x ) )
#endif

#ifndef count_of
#	define count_of( x ) ( ( size_of( x ) / size_of( 0 [ x ] ) ) / ( ( ZPL_NS( sw ) )( ! ( size_of( x ) % size_of( 0 [ x ] ) ) ) ) )
#endif

#ifndef offset_of
#	if defined( _MSC_VER ) || defined( ZPL_COMPILER_TINYC )
#		define offset_of( Type, element ) ( ( ZPL_NS( sw ) ) & ( ( ( Type* )0 )->element ) )
#	else
#		define offset_of( Type, element ) __builtin_offsetof( Type, element )
#	endif
#endif

#if defined( __cplusplus )
#	ifndef align_of
#		if __cplusplus >= 201103L
#			define align_of( Type ) ( ZPL_NS( sw ) )alignof( Type )
#		else
extern "C++"
{
	template< typename T >
	struct alignment_trick
	{
		char c;
		T    member;
	};
}
#			define align_of( Type ) offset_of( ZPL_NS( alignment_trick ) < Type >, member )
#		endif
#	endif
#else
#	ifndef align_of
#		define align_of( Type ) \
			ZPL_NS( offset_of(   \
			    struct {         \
				    char c;      \
				    Type member; \
			    },               \
			    member           \
			) )
#	endif
#endif

#ifndef swap
#	define swap( Type, a, b ) \
		do                     \
		{                      \
			Type tmp = ( a );  \
			( a )    = ( b );  \
			( b )    = tmp;    \
		} while ( 0 )
#endif



#ifndef global
#	define global static    // Global variables
#endif

#ifndef internal
#	define internal static    // Internal linkage
#endif

#ifndef local_persist
#	define local_persist static    // Local Persisting variables
#endif

#ifndef unused
#	if defined( _MSC_VER )
#		define unused( x ) ( __pragma( warning( suppress : 4100 ) )( x ) )
#	elif defined( __GCC__ )
#		define unused( x ) __attribute__( ( __unused__ ) ) ( x )
#	else
#		define unused( x ) ( ( void )( size_of( x ) ) )
#	endif
#endif


#ifndef ZPL_JOIN_MACROS
#	define ZPL_JOIN_MACROS

#	define ZPL_JOIN2               ZPL_CONCAT
#	define ZPL_JOIN3( a, b, c )    ZPL_JOIN2( ZPL_JOIN2( a, b ), c )
#	define ZPL_JOIN4( a, b, c, d ) ZPL_JOIN2( ZPL_JOIN2( ZPL_JOIN2( a, b ), c ), d )
#endif

#ifndef ZPL_BIT
#	define ZPL_BIT( x ) ( 1 << ( x ) )
#endif

#ifndef min
#	define min( a, b ) ( ( a ) < ( b ) ? ( a ) : ( b ) )
#endif

#ifndef max
#	define max( a, b ) ( ( a ) > ( b ) ? ( a ) : ( b ) )
#endif

#ifndef min3
#	define min3( a, b, c ) min( min( a, b ), c )
#endif

#ifndef max3
#	define max3( a, b, c ) max( max( a, b ), c )
#endif

#ifndef clamp
#	define clamp( x, lower, upper ) min( max( ( x ), ( lower ) ), ( upper ) )
#endif

#ifndef clamp01
#	define clamp01( x ) clamp( ( x ), 0, 1 )
#endif

#ifndef is_between
#	define is_between( x, lower, upper ) ( ( ( lower ) <= ( x ) ) && ( ( x ) <= ( upper ) ) )
#endif

#ifndef is_between_limit
#	define is_between_limit( x, lower, upper ) ( ( ( lower ) <= ( x ) ) && ( ( x ) < ( upper ) ) )
#endif

#ifndef step
#	define step( x, y ) ( ( ( x ) / ( y ) ) * ( y ) )
#endif

#ifndef abs
#	define abs( x ) ( ( x ) < 0 ? -( x ) : ( x ) )
#endif

#ifndef ZPL_MASK_SET
#	define ZPL_MASK_SET( var, set, mask ) \
		do                                 \
		{                                  \
			if ( set )                     \
				( var ) |= ( mask );       \
			else                           \
				( var ) &= ~( mask );      \
		} while ( 0 )
#endif

// Multiline string literals in C99!
#ifndef ZPL_MULTILINE
#	define ZPL_MULTILINE( ... ) #__VA_ARGS__
#endif

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: header/essentials/types.h



/* Basic types */

#if defined( ZPL_COMPILER_MSVC )
ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS
#	if _MSC_VER < 1300
typedef unsigned char  u8;
typedef signed char    s8;
typedef unsigned short u16;
typedef signed short   s16;
typedef unsigned int   u32;
typedef signed int     s32;
#    else
typedef unsigned __int8  u8;
typedef signed __int8    s8;
typedef unsigned __int16 u16;
typedef signed __int16   s16;
typedef unsigned __int32 u32;
typedef signed __int32   s32;
#    endif
typedef unsigned __int64 u64;
typedef signed __int64   s64;
ZPL_END_C_DECLS
ZPL_END_NAMESPACE
#else
#	include <stdint.h>

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS
typedef uint8_t  u8;
typedef int8_t   s8;
typedef uint16_t u16;
typedef int16_t  s16;
typedef uint32_t u32;
typedef int32_t  s32;
typedef uint64_t u64;
typedef int64_t  s64;
ZPL_END_C_DECLS
ZPL_END_NAMESPACE
#endif

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS
ZPL_STATIC_ASSERT( sizeof( u8 ) == sizeof( s8 ), "sizeof(u8) != sizeof(s8)" );
ZPL_STATIC_ASSERT( sizeof( u16 ) == sizeof( s16 ), "sizeof(u16) != sizeof(s16)" );
ZPL_STATIC_ASSERT( sizeof( u32 ) == sizeof( s32 ), "sizeof(u32) != sizeof(s32)" );
ZPL_STATIC_ASSERT( sizeof( u64 ) == sizeof( s64 ), "sizeof(u64) != sizeof(s64)" );

ZPL_STATIC_ASSERT( sizeof( u8 ) == 1, "sizeof(u8) != 1" );
ZPL_STATIC_ASSERT( sizeof( u16 ) == 2, "sizeof(u16) != 2" );
ZPL_STATIC_ASSERT( sizeof( u32 ) == 4, "sizeof(u32) != 4" );
ZPL_STATIC_ASSERT( sizeof( u64 ) == 8, "sizeof(u64) != 8" );

typedef size_t    uw;
typedef ptrdiff_t sw;

ZPL_STATIC_ASSERT( sizeof( uw ) == sizeof( sw ), "sizeof(uw) != sizeof(sw)" );

// NOTE: (u)zpl_intptr is only here for semantic reasons really as this library will only support 32/64 bit OSes.
#if defined( _WIN64 )
typedef signed __int64   sptr;
typedef unsigned __int64 uptr;
#elif defined( _WIN32 )
// NOTE; To mark types changing their size, e.g. zpl_intptr
#	ifndef _W64
#		if ! defined( __midl ) && ( defined( _X86_ ) || defined( _M_IX86 ) ) && _MSC_VER >= 1300
#			define _W64 __w64
#		else
#			define _W64
#		endif
#	endif
typedef _W64 signed int   sptr;
typedef _W64 unsigned int uptr;
#else
typedef uintptr_t uptr;
typedef intptr_t  sptr;
#endif

ZPL_STATIC_ASSERT( sizeof( uptr ) == sizeof( sptr ), "sizeof(uptr) != sizeof(sptr)" );

typedef float  f32;
typedef double f64;

ZPL_STATIC_ASSERT( sizeof( f32 ) == 4, "sizeof(f32) != 4" );
ZPL_STATIC_ASSERT( sizeof( f64 ) == 8, "sizeof(f64) != 8" );

typedef s32 rune;    // NOTE: Unicode codepoint
typedef s32 char32;
#define ZPL_RUNE_INVALID zpl_cast( ZPL_NS( rune ) )( 0xfffd )
#define ZPL_RUNE_MAX     zpl_cast( ZPL_NS( rune ) )( 0x0010ffff )
#define ZPL_RUNE_BOM     zpl_cast( ZPL_NS( rune ) )( 0xfeff )
#define ZPL_RUNE_EOF     zpl_cast( ZPL_NS( rune ) )( -1 )

typedef s8  b8;
typedef s16 b16;
typedef s32 b32;
ZPL_END_C_DECLS
ZPL_END_NAMESPACE

#if ! defined( __cplusplus )
#	if ( defined( _MSC_VER ) && _MSC_VER < 1800 ) || ( ! defined( _MSC_VER ) && ! defined( __STDC_VERSION__ ) )
#		ifndef true
#			define true( 0 == 0 )
#		endif
#		ifndef false
#			define false( 0 != 0 )
#		endif
ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS
typedef b8 bool;
ZPL_END_C_DECLS
ZPL_END_NAMESPACE
#	else
#		include <stdbool.h>
#	endif
#endif

#ifndef ZPL_U8_MIN
#	define ZPL_U8_MIN 0u
#	define ZPL_U8_MAX 0xffu
#	define ZPL_I8_MIN ( -0x7f - 1 )
#	define ZPL_I8_MAX 0x7f

#	define ZPL_U16_MIN 0u
#	define ZPL_U16_MAX 0xffffu
#	define ZPL_I16_MIN ( -0x7fff - 1 )
#	define ZPL_I16_MAX 0x7fff

#	define ZPL_U32_MIN 0u
#	define ZPL_U32_MAX 0xffffffffu
#	define ZPL_I32_MIN ( -0x7fffffff - 1 )
#	define ZPL_I32_MAX 0x7fffffff

#	define ZPL_U64_MIN 0ull
#	define ZPL_U64_MAX 0xffffffffffffffffull
#	define ZPL_I64_MIN ( -0x7fffffffffffffffll - 1 )
#	define ZPL_I64_MAX 0x7fffffffffffffffll

#	if defined( ZPL_ARCH_32_BIT )
#		define ZPL_USIZE_MIN ZPL_U32_MIN
#		define ZPL_USIZE_MAX ZPL_U32_MAX
#		define ZPL_ISIZE_MIN ZPL_S32_MIN
#		define ZPL_ISIZE_MAX ZPL_S32_MAX
#	elif defined( ZPL_ARCH_64_BIT )
#		define ZPL_USIZE_MIN ZPL_U64_MIN
#		define ZPL_USIZE_MAX ZPL_U64_MAX
#		define ZPL_ISIZE_MIN ZPL_I64_MIN
#		define ZPL_ISIZE_MAX ZPL_I64_MAX
#	else
#		error Unknown architecture size. This library only supports 32 bit and 64 bit architectures.
#	endif

#	define ZPL_F32_MIN 1.17549435e-38f
#	define ZPL_F32_MAX 3.40282347e+38f

#	define ZPL_F64_MIN 2.2250738585072014e-308
#	define ZPL_F64_MAX 1.7976931348623157e+308
#endif

#ifdef ZPL_DEFINE_NULL_MACRO
#	ifndef NULL
#		define NULL ZPL_NULL
#	endif
#endif


#	if defined( ZPL_MODULE_ESSENTIALS )
// file: header/essentials/collections/array.h

////////////////////////////////////////////////////////////////
//
// Dynamic Array (POD Types)
//
// zpl_array(Type) works like zpl_string or zpl_buffer where the actual type is just a pointer to the first
// element.
//
// Available Procedures for zpl_array(Type)
// zpl_array_init
// zpl_array_free
// zpl_array_set_capacity
// zpl_array_grow
// zpl_array_append
// zpl_array_appendv
// zpl_array_pop
// zpl_array_clear
// zpl_array_back
// zpl_array_front
// zpl_array_resize
// zpl_array_reserve
//

#if 0    // Example
void foo(void) {
    sw i;
    int test_values[] = {4, 2, 1, 7};
    AllocatorInfo a = heap_allocator();
    Array(int) items;

    array_init(items, a);

    array_append(items, 1);
    array_append(items, 4);
    array_append(items, 9);
    array_append(items, 16);

    items[1] = 3; // Manually set value
    // NOTE: No array bounds checking

    for (i = 0; i < items.count; i++)
        printf("%d\n", items[i]);
    // 1
    // 3
    // 9
    // 16

    array_clear(items);

    array_appendv(items, test_values, count_of(test_values));
    for (i = 0; i < items.count; i++)
        printf("%d\n", items[i]);
    // 4
    // 2
    // 1
    // 7

    array_free(items);
}
#endif


ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

typedef struct ArrayHeader
{
	sw            elem_size;
	sw            count;
	sw            capacity;
	AllocatorInfo allocator;
} ArrayHeader;

#define Array( Type ) Type*

#define array_make( Type, Name, allocator ) \
	Type* Name;                             \
	ZPL_NS( array_init )( Name, allocator )

#ifndef ZPL_ARRAY_GROW_FORMULA
#	define ZPL_ARRAY_GROW_FORMULA( x ) ( 2 * ( x ) + 8 )
#endif

ZPL_STATIC_ASSERT( ZPL_ARRAY_GROW_FORMULA( 0 ) > 0, "ZPL_ARRAY_GROW_FORMULA(0) <= 0" );

#define ZPL_ARRAY_HEADER( x ) ( zpl_cast( ZPL_NS( ArrayHeader )* )( x ) - 1 )
#define array_allocator( x )  ( ZPL_ARRAY_HEADER( x )->allocator )
#define array_elem_size( x )  ( ZPL_ARRAY_HEADER( x )->elem_size )
#define array_count( x )      ( ZPL_ARRAY_HEADER( x )->count )
#define array_capacity( x )   ( ZPL_ARRAY_HEADER( x )->capacity )
#define array_end( x )        ( x + ( array_count( x ) - 1 ) )

ZPL_IMPL_INLINE b8 _array_init_reserve( void** _array_, AllocatorInfo allocator_, sw elem_size, sw cap )
{
	ArrayHeader* _ah = zpl_cast( ArrayHeader* ) alloc( allocator_, size_of( ArrayHeader ) + elem_size * cap );
	if ( ! _ah )
		return false;
	_ah->allocator = allocator_;
	_ah->elem_size = elem_size;
	_ah->count     = 0;
	_ah->capacity  = cap;
	*_array_       = zpl_cast( void* )( _ah + 1 );
	return true;
}

#define array_init_reserve( x, allocator_, cap ) ZPL_NS( _array_init_reserve )( zpl_cast( void** ) & ( x ), allocator_, size_of( *( x ) ), ( cap ) )

// NOTE: Give it an initial default capacity
#define array_init( x, allocator ) array_init_reserve( x, allocator, ZPL_ARRAY_GROW_FORMULA( 0 ) )

#define array_free( x )                                         \
	do                                                          \
	{                                                           \
		if ( x )                                                \
		{                                                       \
			ZPL_NS( ArrayHeader )* _ah = ZPL_ARRAY_HEADER( x ); \
			ZPL_NS( free )( _ah->allocator, _ah );              \
		}                                                       \
	} while ( 0 )

ZPL_IMPL_INLINE b8 _array_set_capacity( void** array, sw capacity )
{
	ArrayHeader* h = ZPL_ARRAY_HEADER( *array );
	if ( capacity == h->capacity )
		return true;
	if ( capacity < h->count )
		h->count = capacity;
	sw           size = size_of( ArrayHeader ) + h->elem_size * capacity;
	ArrayHeader* nh   = zpl_cast( ArrayHeader* ) alloc( h->allocator, size );
	if ( ! nh )
		return false;
	memmove( nh, h, size_of( ArrayHeader ) + h->elem_size * h->count );
	nh->allocator = h->allocator;
	nh->elem_size = h->elem_size;
	nh->count     = h->count;
	nh->capacity  = capacity;
	free( h->allocator, h );
	*array = nh + 1;
	return true;
}

#define array_set_capacity( x, capacity ) ZPL_NS( _array_set_capacity )( zpl_cast( void** ) & ( x ), ( capacity ) )

ZPL_IMPL_INLINE b8 _array_grow( void** x, sw min_capacity )
{
	sw new_capacity = ZPL_ARRAY_GROW_FORMULA( array_capacity( *x ) );
	if ( new_capacity < min_capacity )
		new_capacity = min_capacity;
	return _array_set_capacity( x, new_capacity );
}

#define array_grow( x, min_capacity ) ZPL_NS( _array_grow )( zpl_cast( void** ) & ( x ), ( min_capacity ) )

ZPL_IMPL_INLINE b8 _array_append_helper( void** x )
{
	if ( array_capacity( *x ) < array_count( *x ) + 1 )
	{
		if ( ! _array_grow( x, 0 ) )
			return false;
	}
	return true;
}

#define array_append( x, item ) ( ZPL_NS( _array_append_helper )( zpl_cast( void** ) & ( x ) ) && ( ( ( x )[ array_count( x )++ ] = ( item ) ), true ) )

ZPL_IMPL_INLINE b8 _array_append_at_helper( void** x, sw ind )
{
	if ( ind >= array_count( *x ) )
		ind = array_count( *x ) - 1;
	if ( ind < 0 )
		ind = 0;
	if ( array_capacity( *x ) < array_count( *x ) + 1 )
	{
		if ( ! _array_grow( x, 0 ) )
			return false;
	}
	s8* s = ( zpl_cast( s8* ) * x ) + ind * array_elem_size( *x );
	memmove( s + array_elem_size( *x ), s, array_elem_size( *x ) * ( array_count( *x ) - ind ) );
	return true;
}

#define array_append_at( x, item, ind ) ( ZPL_NS( _array_append_at_helper )( zpl_cast( void** ) & ( x ), ( ind ) ) && ( ( ( x )[ ind ] = ( item ) ), array_count( x )++, true ) )

ZPL_IMPL_INLINE b8 _array_appendv( void** x, void* items, sw item_size, sw item_count )
{
	ZPL_ASSERT( item_size == array_elem_size( *x ) );
	if ( array_capacity( *x ) < array_count( *x ) + item_count )
	{
		if ( ! _array_grow( x, array_count( *x ) + item_count ) )
			return false;
	}
	memcopy( ( zpl_cast( s8* ) * x ) + array_count( *x ) * array_elem_size( *x ), items, array_elem_size( *x ) * item_count );
	array_count( *x ) += item_count;
	return true;
}

#define array_appendv( x, items, item_count ) ZPL_NS( _array_appendv )( zpl_cast( void** ) & ( x ), ( items ), size_of( ( items )[ 0 ] ), ( item_count ) )

ZPL_IMPL_INLINE b8 _array_appendv_at( void** x, void* items, sw item_size, sw item_count, sw ind )
{
	if ( ind >= array_count( *x ) )
		return _array_appendv( x, items, item_size, item_count );
	ZPL_ASSERT( item_size == array_elem_size( *x ) );
	if ( array_capacity( *x ) < array_count( *x ) + item_count )
	{
		if ( ! _array_grow( x, array_count( *x ) + item_count ) )
			return false;
	}
	memmove(
	    ( zpl_cast( s8* ) * x ) + ( ind + item_count ) * array_elem_size( *x ),
	    ( zpl_cast( s8* ) * x ) + ind * array_elem_size( *x ),
	    array_elem_size( *x ) * ( array_count( *x ) - ind )
	);
	memcopy( ( zpl_cast( s8* ) * x ) + ind * array_elem_size( *x ), items, array_elem_size( *x ) * item_count );
	array_count( *x ) += item_count;
	return true;
}

#define array_appendv_at( x, items, item_count, ind ) ZPL_NS( _array_appendv_at )( zpl_cast( void** ) & ( x ), ( items ), size_of( ( items )[ 0 ] ), ( item_count ), ( ind ) )

#define array_fill( x, begin, end, value )                          \
	do                                                              \
	{                                                               \
		ZPL_ASSERT( ( begin ) >= 0 && ( end ) < array_count( x ) ); \
		ZPL_ASSERT( size_of( value ) == size_of( ( x )[ 0 ] ) );    \
		for ( ZPL_NS( sw ) i = ( begin ); i < ( end ); i++ )        \
		{                                                           \
			x[ i ] = value;                                         \
		}                                                           \
	} while ( 0 )

#define array_remove_at( x, index )                                                                    \
	do                                                                                                 \
	{                                                                                                  \
		ZPL_NS( ArrayHeader )* _ah = ZPL_ARRAY_HEADER( x );                                            \
		ZPL_ASSERT( index < _ah->count );                                                              \
		ZPL_NS( memmove )( x + index, x + index + 1, size_of( x[ 0 ] ) * ( _ah->count - index - 1 ) ); \
		--_ah->count;                                                                                  \
	} while ( 0 )

ZPL_IMPL_INLINE b8 _array_copy_init( void** y, void** x )
{
	if ( ! _array_init_reserve( y, array_allocator( *x ), array_elem_size( *x ), array_capacity( *x ) ) )
		return false;
	memcopy( *y, *x, array_capacity( *x ) * array_elem_size( *x ) );
	array_count( *y ) = array_count( *x );
	return true;
}

#define array_copy_init( y, x ) ZPL_NS( _array_copy_init )( zpl_cast( void** ) & ( y ), zpl_cast( void** ) & ( x ) )

#define array_pop( x )                                  \
	do                                                  \
	{                                                   \
		ZPL_ASSERT( ZPL_ARRAY_HEADER( x )->count > 0 ); \
		ZPL_ARRAY_HEADER( x )->count--;                 \
	} while ( 0 )
#define array_back( x )  x[ ZPL_ARRAY_HEADER( x )->count - 1 ]
#define array_front( x ) x[ 0 ]
#define array_clear( x )                  \
	do                                    \
	{                                     \
		ZPL_ARRAY_HEADER( x )->count = 0; \
	} while ( 0 )

ZPL_IMPL_INLINE b8 _array_resize( void** x, sw new_count )
{
	if ( ZPL_ARRAY_HEADER( *x )->capacity < new_count )
	{
		if ( ! _array_grow( x, new_count ) )
			return false;
	}
	ZPL_ARRAY_HEADER( *x )->count = new_count;
	return true;
}

#define array_resize( x, new_count ) ZPL_NS( _array_resize )( zpl_cast( void** ) & ( x ), ( new_count ) )

ZPL_IMPL_INLINE b8 _array_reserve( void** x, sw new_capacity )
{
	if ( ZPL_ARRAY_HEADER( *x )->capacity < new_capacity )
		return _array_set_capacity( x, new_capacity );
	return true;
}

#define array_reserve( x, new_capacity ) ZPL_NS( _array_reserve )( zpl_cast( void** ) & ( x ), ( new_capacity ) )

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: header/essentials/collections/buffer.h

////////////////////////////////////////////////////////////////
//
// Fixed Capacity Buffer (POD Types)
//
//
// zpl_buffer(Type) works like zpl_string or zpl_array where the actual type is just a pointer to the first
// element.
//
// Available Procedures for zpl_buffer(Type)
// zpl_buffer_init
// zpl_buffer_free
// zpl_buffer_append
// zpl_buffer_appendv
// zpl_buffer_pop
// zpl_buffer_clear


ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

typedef struct buffer_header
{
	AllocatorInfo backing;
	sw            count;
	sw            capacity;
} buffer_header;

#define buffer( Type ) Type*

#define buffer_make( Type, Name, allocator, cap ) \
	Type* Name;                                   \
	ZPL_NS( buffer_init )( Name, allocator, cap )

#define ZPL_BUFFER_HEADER( x ) ( zpl_cast( ZPL_NS( buffer_header )* )( x ) - 1 )
#define buffer_count( x )      ( ZPL_BUFFER_HEADER( x )->count )
#define buffer_capacity( x )   ( ZPL_BUFFER_HEADER( x )->capacity )
#define buffer_end( x )        ( x + ( buffer_count( x ) - 1 ) )

#define buffer_init( x, allocator, cap )                                                                                                                           \
	do                                                                                                                                                             \
	{                                                                                                                                                              \
		void** nx                    = zpl_cast( void** ) & ( x );                                                                                                 \
		ZPL_NS( buffer_header )* _bh = zpl_cast( ZPL_NS( buffer_header )* ) alloc( ( allocator ), sizeof( ZPL_NS( buffer_header ) ) + ( cap )*size_of( *( x ) ) ); \
		_bh->backing                 = allocator;                                                                                                                  \
		_bh->count                   = 0;                                                                                                                          \
		_bh->capacity                = cap;                                                                                                                        \
		*nx                          = zpl_cast( void* )( _bh + 1 );                                                                                               \
	} while ( 0 )

#define buffer_free( x ) ( ZPL_NS( free )( ZPL_BUFFER_HEADER( x )->backing, ZPL_BUFFER_HEADER( x ) ) )

#define buffer_append( x, item )                 \
	do                                           \
	{                                            \
		( x )[ buffer_count( x )++ ] = ( item ); \
	} while ( 0 )

#define buffer_appendv( x, items, item_count )                                                           \
	do                                                                                                   \
	{                                                                                                    \
		ZPL_ASSERT( size_of( *( items ) ) == size_of( *( x ) ) );                                        \
		ZPL_ASSERT( buffer_count( x ) + item_count <= buffer_capacity( x ) );                            \
		ZPL_NS( memcopy )( &( x )[ buffer_count( x ) ], ( items ), size_of( *( x ) ) * ( item_count ) ); \
		buffer_count( x ) += ( item_count );                                                             \
	} while ( 0 )

#define buffer_copy_init( y, x )                                                                   \
	do                                                                                             \
	{                                                                                              \
		ZPL_NS( buffer_init_reserve )( y, ZPL_NS( buffer_allocator )( x ), buffer_capacity( x ) ); \
		ZPL_NS( memcopy )( y, x, buffer_capacity( x ) * size_of( *x ) );                           \
		buffer_count( y ) = buffer_count( x );                                                     \
	} while ( 0 )

#define buffer_pop( x )                      \
	do                                       \
	{                                        \
		ZPL_ASSERT( buffer_count( x ) > 0 ); \
		buffer_count( x )--;                 \
	} while ( 0 )
#define buffer_clear( x )      \
	do                         \
	{                          \
		buffer_count( x ) = 0; \
	} while ( 0 )

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: header/essentials/collections/hashtable.h

/** @file hashtable.c
@brief Instantiated hash table
@defgroup hashtable Instantiated hash table


 This is an attempt to implement a templated hash table
 NOTE: The key is always a zpl_u64 for simplicity and you will _probably_ _never_ need anything bigger.

 Hash table type and function declaration, call: ZPL_TABLE_DECLARE(PREFIX, NAME, FUNC, VALUE)
 Hash table function definitions, call: ZPL_TABLE_DEFINE(NAME, FUNC, VALUE)

     PREFIX  - a prefix for function prototypes e.g. extern, static, etc.
     NAME    - Name of the Hash Table
     FUNC    - the name will prefix function names
     VALUE   - the type of the value to be stored

    tablename_init(NAME * h, zpl_allocator a);
    tablename_destroy(NAME * h);
    tablename_get(NAME * h, zpl_u64 key);
    tablename_set(NAME * h, zpl_u64 key, VALUE value);
    tablename_grow(NAME * h);
    tablename_map(NAME * h, void (*map_proc)(zpl_u64 key, VALUE value))
    tablename_map_mut(NAME * h, void (*map_proc)(zpl_u64 key, VALUE * value))
    tablename_rehash(NAME * h, zpl_isize new_count);
    tablename_remove(NAME * h, zpl_u64 key);

 @{
*/

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

typedef struct hash_table_find_result
{
	sw hash_index;
	sw entry_prev;
	sw entry_index;
} hash_table_find_result;

/**
 * Combined macro for a quick delcaration + definition
 */

#define ZPL_TABLE( PREFIX, NAME, FUNC, VALUE )      \
	ZPL_TABLE_DECLARE( PREFIX, NAME, FUNC, VALUE ); \
	ZPL_TABLE_DEFINE( NAME, FUNC, VALUE );

/**
 * Table delcaration macro that generates the interface
 */

#define ZPL_TABLE_DECLARE( PREFIX, NAME, FUNC, VALUE )                                                            \
	typedef struct ZPL_JOIN2( NAME, Entry )                                                                       \
	{                                                                                                             \
		ZPL_NS( u64 ) key;                                                                                        \
		ZPL_NS( sw ) next;                                                                                        \
		VALUE value;                                                                                              \
	} ZPL_JOIN2( NAME, Entry );                                                                                   \
                                                                                                                  \
	typedef struct NAME                                                                                           \
	{                                                                                                             \
		Array( ZPL_NS( sw ) ) hashes;                                                                             \
		Array( ZPL_JOIN2( NAME, Entry ) ) entries;                                                                \
	} NAME;                                                                                                       \
                                                                                                                  \
	PREFIX void   ZPL_JOIN2( FUNC, init )( NAME * h, ZPL_NS( AllocatorInfo ) a );                                 \
	PREFIX void   ZPL_JOIN2( FUNC, destroy )( NAME * h );                                                         \
	PREFIX void   ZPL_JOIN2( FUNC, clear )( NAME * h );                                                           \
	PREFIX VALUE* ZPL_JOIN2( FUNC, get )( NAME * h, ZPL_NS( u64 ) key );                                          \
	PREFIX        ZPL_NS( sw ) ZPL_JOIN2( FUNC, slot )( NAME * h, ZPL_NS( u64 ) key );                            \
	PREFIX void   ZPL_JOIN2( FUNC, set )( NAME * h, ZPL_NS( u64 ) key, VALUE value );                             \
	PREFIX void   ZPL_JOIN2( FUNC, grow )( NAME * h );                                                            \
	PREFIX void   ZPL_JOIN2( FUNC, rehash )( NAME * h, ZPL_NS( sw ) new_count );                                  \
	PREFIX void   ZPL_JOIN2( FUNC, rehash_fast )( NAME * h );                                                     \
	PREFIX void   ZPL_JOIN2( FUNC, map )( NAME * h, void ( *map_proc )( ZPL_NS( u64 ) key, VALUE value ) );       \
	PREFIX void   ZPL_JOIN2( FUNC, map_mut )( NAME * h, void ( *map_proc )( ZPL_NS( u64 ) key, VALUE * value ) ); \
	PREFIX void   ZPL_JOIN2( FUNC, remove )( NAME * h, ZPL_NS( u64 ) key );                                       \
	PREFIX void   ZPL_JOIN2( FUNC, remove_entry )( NAME * h, ZPL_NS( sw ) idx );

/**
 * Table definition interfaces that generates the implementation
 */

#define ZPL_TABLE_DEFINE( NAME, FUNC, VALUE )                                                           \
	void ZPL_JOIN2( FUNC, init )( NAME * h, ZPL_NS( AllocatorInfo ) a )                                 \
	{                                                                                                   \
		array_init( h->hashes, a );                                                                     \
		array_init( h->entries, a );                                                                    \
	}                                                                                                   \
                                                                                                        \
	void ZPL_JOIN2( FUNC, destroy )( NAME * h )                                                         \
	{                                                                                                   \
		if ( h->entries )                                                                               \
			array_free( h->entries );                                                                   \
		if ( h->hashes )                                                                                \
			array_free( h->hashes );                                                                    \
	}                                                                                                   \
                                                                                                        \
	void ZPL_JOIN2( FUNC, clear )( NAME * h )                                                           \
	{                                                                                                   \
		for ( int i = 0; i < array_count( h->hashes ); i++ )                                            \
			h->hashes[ i ] = -1;                                                                        \
		array_clear( h->entries );                                                                      \
	}                                                                                                   \
                                                                                                        \
	ZPL_NS( sw ) ZPL_JOIN2( FUNC, slot )( NAME * h, ZPL_NS( u64 ) key )                                 \
	{                                                                                                   \
		for ( ZPL_NS( sw ) i = 0; i < array_count( h->entries ); i++ )                                  \
		{                                                                                               \
			if ( h->entries[ i ].key == key )                                                           \
			{                                                                                           \
				return i;                                                                               \
			}                                                                                           \
		}                                                                                               \
		return -1;                                                                                      \
	}                                                                                                   \
                                                                                                        \
	internal ZPL_NS( sw ) ZPL_JOIN2( FUNC, _add_entry )( NAME * h, ZPL_NS( u64 ) key )                  \
	{                                                                                                   \
		ZPL_NS( sw ) index;                                                                             \
		ZPL_JOIN2( NAME, Entry ) e = { 0 };                                                             \
		e.key                      = key;                                                               \
		e.next                     = -1;                                                                \
		index                      = array_count( h->entries );                                         \
		array_append( h->entries, e );                                                                  \
		return index;                                                                                   \
	}                                                                                                   \
                                                                                                        \
	internal ZPL_NS( hash_table_find_result ) ZPL_JOIN2( FUNC, _find )( NAME * h, ZPL_NS( u64 ) key )   \
	{                                                                                                   \
		ZPL_NS( hash_table_find_result ) r = { -1, -1, -1 };                                            \
		if ( array_count( h->hashes ) > 0 )                                                             \
		{                                                                                               \
			r.hash_index  = key % array_count( h->hashes );                                             \
			r.entry_index = h->hashes[ r.hash_index ];                                                  \
			while ( r.entry_index >= 0 )                                                                \
			{                                                                                           \
				if ( h->entries[ r.entry_index ].key == key )                                           \
					return r;                                                                           \
				r.entry_prev  = r.entry_index;                                                          \
				r.entry_index = h->entries[ r.entry_index ].next;                                       \
			}                                                                                           \
		}                                                                                               \
		return r;                                                                                       \
	}                                                                                                   \
                                                                                                        \
	internal ZPL_NS( b32 ) ZPL_JOIN2( FUNC, _full )( NAME * h )                                         \
	{                                                                                                   \
		return 0.75f * array_count( h->hashes ) < array_count( h->entries );                            \
	}                                                                                                   \
                                                                                                        \
	void ZPL_JOIN2( FUNC, grow )( NAME * h )                                                            \
	{                                                                                                   \
		ZPL_NS( sw ) new_count = ZPL_ARRAY_GROW_FORMULA( array_count( h->entries ) );                   \
		ZPL_JOIN2( FUNC, rehash )( h, new_count );                                                      \
	}                                                                                                   \
                                                                                                        \
	void ZPL_JOIN2( FUNC, rehash )( NAME * h, ZPL_NS( sw ) new_count )                                  \
	{                                                                                                   \
		ZPL_NS( sw ) i, j;                                                                              \
		NAME nh = { 0 };                                                                                \
		ZPL_JOIN2( FUNC, init )( &nh, array_allocator( h->hashes ) );                                   \
		array_resize( nh.hashes, new_count );                                                           \
		array_reserve( nh.entries, array_count( h->entries ) );                                         \
		for ( i = 0; i < new_count; i++ )                                                               \
			nh.hashes[ i ] = -1;                                                                        \
		for ( i = 0; i < array_count( h->entries ); i++ )                                               \
		{                                                                                               \
			ZPL_JOIN2( NAME, Entry ) * e;                                                               \
			ZPL_NS( hash_table_find_result ) fr;                                                        \
			if ( array_count( nh.hashes ) == 0 )                                                        \
				ZPL_JOIN2( FUNC, grow )( &nh );                                                         \
			e  = &h->entries[ i ];                                                                      \
			fr = ZPL_JOIN2( FUNC, _find )( &nh, e->key );                                               \
			j  = ZPL_JOIN2( FUNC, _add_entry )( &nh, e->key );                                          \
			if ( fr.entry_prev < 0 )                                                                    \
				nh.hashes[ fr.hash_index ] = j;                                                         \
			else                                                                                        \
				nh.entries[ fr.entry_prev ].next = j;                                                   \
			nh.entries[ j ].next  = fr.entry_index;                                                     \
			nh.entries[ j ].value = e->value;                                                           \
		}                                                                                               \
		ZPL_JOIN2( FUNC, destroy )( h );                                                                \
		h->hashes  = nh.hashes;                                                                         \
		h->entries = nh.entries;                                                                        \
	}                                                                                                   \
                                                                                                        \
	void ZPL_JOIN2( FUNC, rehash_fast )( NAME * h )                                                     \
	{                                                                                                   \
		ZPL_NS( sw ) i;                                                                                 \
		for ( i = 0; i < array_count( h->entries ); i++ )                                               \
			h->entries[ i ].next = -1;                                                                  \
		for ( i = 0; i < array_count( h->hashes ); i++ )                                                \
			h->hashes[ i ] = -1;                                                                        \
		for ( i = 0; i < array_count( h->entries ); i++ )                                               \
		{                                                                                               \
			ZPL_JOIN2( NAME, Entry ) * e;                                                               \
			ZPL_NS( hash_table_find_result ) fr;                                                        \
			e  = &h->entries[ i ];                                                                      \
			fr = ZPL_JOIN2( FUNC, _find )( h, e->key );                                                 \
			if ( fr.entry_prev < 0 )                                                                    \
				h->hashes[ fr.hash_index ] = i;                                                         \
			else                                                                                        \
				h->entries[ fr.entry_prev ].next = i;                                                   \
		}                                                                                               \
	}                                                                                                   \
                                                                                                        \
	VALUE* ZPL_JOIN2( FUNC, get )( NAME * h, ZPL_NS( u64 ) key )                                        \
	{                                                                                                   \
		ZPL_NS( sw ) index = ZPL_JOIN2( FUNC, _find )( h, key ).entry_index;                            \
		if ( index >= 0 )                                                                               \
			return &h->entries[ index ].value;                                                          \
		return NULL;                                                                                    \
	}                                                                                                   \
                                                                                                        \
	void ZPL_JOIN2( FUNC, remove )( NAME * h, ZPL_NS( u64 ) key )                                       \
	{                                                                                                   \
		ZPL_NS( hash_table_find_result ) fr = ZPL_JOIN2( FUNC, _find )( h, key );                       \
		if ( fr.entry_index >= 0 )                                                                      \
		{                                                                                               \
			array_remove_at( h->entries, fr.entry_index );                                              \
			ZPL_JOIN2( FUNC, rehash_fast )( h );                                                        \
		}                                                                                               \
	}                                                                                                   \
                                                                                                        \
	void ZPL_JOIN2( FUNC, remove_entry )( NAME * h, ZPL_NS( sw ) idx )                                  \
	{                                                                                                   \
		array_remove_at( h->entries, idx );                                                             \
	}                                                                                                   \
                                                                                                        \
	void ZPL_JOIN2( FUNC, map )( NAME * h, void ( *map_proc )( ZPL_NS( u64 ) key, VALUE value ) )       \
	{                                                                                                   \
		ZPL_ASSERT_NOT_NULL( h );                                                                       \
		ZPL_ASSERT_NOT_NULL( map_proc );                                                                \
		for ( ZPL_NS( sw ) i = 0; i < array_count( h->entries ); ++i )                                  \
		{                                                                                               \
			map_proc( h->entries[ i ].key, h->entries[ i ].value );                                     \
		}                                                                                               \
	}                                                                                                   \
                                                                                                        \
	void ZPL_JOIN2( FUNC, map_mut )( NAME * h, void ( *map_proc )( ZPL_NS( u64 ) key, VALUE * value ) ) \
	{                                                                                                   \
		ZPL_ASSERT_NOT_NULL( h );                                                                       \
		ZPL_ASSERT_NOT_NULL( map_proc );                                                                \
		for ( ZPL_NS( sw ) i = 0; i < array_count( h->entries ); ++i )                                  \
		{                                                                                               \
			map_proc( h->entries[ i ].key, &h->entries[ i ].value );                                    \
		}                                                                                               \
	}                                                                                                   \
                                                                                                        \
	void ZPL_JOIN2( FUNC, set )( NAME * h, ZPL_NS( u64 ) key, VALUE value )                             \
	{                                                                                                   \
		ZPL_NS( sw ) index;                                                                             \
		ZPL_NS( hash_table_find_result ) fr;                                                            \
		if ( array_count( h->hashes ) == 0 )                                                            \
			ZPL_JOIN2( FUNC, grow )( h );                                                               \
		fr = ZPL_JOIN2( FUNC, _find )( h, key );                                                        \
		if ( fr.entry_index >= 0 )                                                                      \
		{                                                                                               \
			index = fr.entry_index;                                                                     \
		}                                                                                               \
		else                                                                                            \
		{                                                                                               \
			index = ZPL_JOIN2( FUNC, _add_entry )( h, key );                                            \
			if ( fr.entry_prev >= 0 )                                                                   \
			{                                                                                           \
				h->entries[ fr.entry_prev ].next = index;                                               \
			}                                                                                           \
			else                                                                                        \
			{                                                                                           \
				h->hashes[ fr.hash_index ] = index;                                                     \
			}                                                                                           \
		}                                                                                               \
		h->entries[ index ].value = value;                                                              \
		if ( ZPL_JOIN2( FUNC, _full )( h ) )                                                            \
			ZPL_JOIN2( FUNC, grow )( h );                                                               \
	}

//! @}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: header/essentials/collections/list.h

////////////////////////////////////////////////////////////////
//
// Linked List
//
// zpl_list encapsulates pointer to data and points to the next and the previous element in the list.
//
// Available Procedures for zpl_list
// zpl_list_init
// zpl_list_add
// zpl_list_remove


ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

#if 0
#	define ZPL_IMPLEMENTATION
#	include "zpl.h"
int main(void)
{
    List s, *head, *cursor;
    list_init(&s, "it is optional to call init: ");
    head = cursor = &s;

    // since we can construct an element implicitly this way
    // the second field gets overwritten once we add it to a list.
    List a = {"hello"};
    cursor = list_add(cursor, &a);

    List b = {"world"};
    cursor = list_add(cursor, &b);

    List c = {"!!! OK"};
    cursor = list_add(cursor, &c);

    for (List *l=head; l; l=l->next) {
        printf("%s ", zpl_cast(char *)l->ptr);
    }
    printf("\n");

    return 0;
}
#endif


typedef struct _list
{
	void const*   ptr;
	struct _list *next, *prev;
} List;

ZPL_DEF_INLINE void  list_init( List* list, void const* ptr );
ZPL_DEF_INLINE List* list_add( List* list, List* item );

// NOTE(zaklaus): Returns a pointer to the next node (or NULL if the removed node has no trailing node.)
ZPL_DEF_INLINE List* list_remove( List* list );

ZPL_IMPL_INLINE void list_init( List* list, void const* ptr )
{
	List list_ = { 0 };
	*list      = list_;
	list->ptr  = ptr;
}

ZPL_IMPL_INLINE List* list_add( List* list, List* item )
{
	item->next = NULL;

	if ( list->next )
	{
		item->next = list->next;
	}

	list->next = item;
	item->prev = list;
	return item;
}

ZPL_IMPL_INLINE List* list_remove( List* list )
{
	if ( list->prev )
	{
		list->prev->next = list->next;
	}

	return list->next;
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: header/essentials/collections/ring.h

////////////////////////////////////////////////////////////////
//
// Instantiated Circular buffer
//

/*
Buffer type and function declaration, call: ZPL_RING_DECLARE(PREFIX, FUNC, VALUE)
Buffer function definitions, call: ZPL_RING_DEFINE(PREFIX, FUNC, VALUE)

PREFIX  - a prefix for function prototypes e.g. extern, static, etc.
     FUNC    - the name will prefix function names
     VALUE   - the type of the value to be stored

funcname_init(VALUE * pad, zpl_allocator a, zpl_isize max_size)
 funcname_free(VALUE * pad)
 funcname_full(VALUE * pad)
 funcname_empty(VALUE * pad)
 funcname_append(VALUE * pad, type data)
 funcname_append_array(VALUE * pad, zpl_array(type) data)
 funcname_get(VALUE * pad)
funcname_get_array(VALUE * pad, zpl_usize max_size, zpl_allocator a)
*/
ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

#define ZPL_RING( PREFIX, FUNC, VALUE )      \
	ZPL_RING_DECLARE( PREFIX, FUNC, VALUE ); \
	ZPL_RING_DEFINE( FUNC, VALUE );

#define ZPL_RING_DECLARE( prefix, func, type )                                                                               \
	typedef struct                                                                                                           \
	{                                                                                                                        \
		ZPL_NS( AllocatorInfo ) backing;                                                                                     \
		buffer( type ) buf;                                                                                                  \
		ZPL_NS( uw ) head, tail;                                                                                             \
		ZPL_NS( uw ) capacity;                                                                                               \
	} ZPL_JOIN2( func, type );                                                                                               \
                                                                                                                             \
	prefix void  ZPL_JOIN2( func, init )( ZPL_JOIN2( func, type ) * pad, ZPL_NS( AllocatorInfo ) a, ZPL_NS( sw ) max_size ); \
	prefix void  ZPL_JOIN2( func, free )( ZPL_JOIN2( func, type ) * pad );                                                   \
	prefix       ZPL_NS( b32 ) ZPL_JOIN2( func, full )( ZPL_JOIN2( func, type ) * pad );                                     \
	prefix       ZPL_NS( b32 ) ZPL_JOIN2( func, empty )( ZPL_JOIN2( func, type ) * pad );                                    \
	prefix void  ZPL_JOIN2( func, append )( ZPL_JOIN2( func, type ) * pad, type data );                                      \
	prefix void  ZPL_JOIN2( func, append_array )( ZPL_JOIN2( func, type ) * pad, Array( type ) data );                       \
	prefix type* ZPL_JOIN2( func, get )( ZPL_JOIN2( func, type ) * pad );                                                    \
	prefix       Array( type ) ZPL_JOIN2( func, get_array )( ZPL_JOIN2( func, type ) * pad, ZPL_NS( uw ) max_size, ZPL_NS( AllocatorInfo ) a );

#define ZPL_RING_DEFINE( func, type )                                                                                             \
	void ZPL_JOIN2( func, init )( ZPL_JOIN2( func, type ) * pad, ZPL_NS( AllocatorInfo ) a, ZPL_NS( sw ) max_size )               \
	{                                                                                                                             \
		ZPL_JOIN2( func, type ) pad_ = { 0 };                                                                                     \
		*pad                         = pad_;                                                                                      \
                                                                                                                                  \
		pad->backing = a;                                                                                                         \
		buffer_init( pad->buf, a, max_size + 1 );                                                                                 \
		pad->capacity = max_size + 1;                                                                                             \
		pad->head = pad->tail = 0;                                                                                                \
	}                                                                                                                             \
	void ZPL_JOIN2( func, free )( ZPL_JOIN2( func, type ) * pad )                                                                 \
	{                                                                                                                             \
		buffer_free( pad->buf );                                                                                                  \
	}                                                                                                                             \
                                                                                                                                  \
	b32 ZPL_JOIN2( func, full )( ZPL_JOIN2( func, type ) * pad )                                                                  \
	{                                                                                                                             \
		return ( ( pad->head + 1 ) % pad->capacity ) == pad->tail;                                                                \
	}                                                                                                                             \
                                                                                                                                  \
	b32 ZPL_JOIN2( func, empty )( ZPL_JOIN2( func, type ) * pad )                                                                 \
	{                                                                                                                             \
		return pad->head == pad->tail;                                                                                            \
	}                                                                                                                             \
                                                                                                                                  \
	void ZPL_JOIN2( func, append )( ZPL_JOIN2( func, type ) * pad, type data )                                                    \
	{                                                                                                                             \
		pad->buf[ pad->head ] = data;                                                                                             \
		pad->head             = ( pad->head + 1 ) % pad->capacity;                                                                \
                                                                                                                                  \
		if ( pad->head == pad->tail )                                                                                             \
		{                                                                                                                         \
			pad->tail = ( pad->tail + 1 ) % pad->capacity;                                                                        \
		}                                                                                                                         \
	}                                                                                                                             \
                                                                                                                                  \
	void ZPL_JOIN2( func, append_array )( ZPL_JOIN2( func, type ) * pad, Array( type ) data )                                     \
	{                                                                                                                             \
		ZPL_NS( uw ) c = array_count( data );                                                                                     \
		for ( uw i = 0; i < c; ++i )                                                                                              \
		{                                                                                                                         \
			ZPL_JOIN2( func, append )( pad, data[ i ] );                                                                          \
		}                                                                                                                         \
	}                                                                                                                             \
                                                                                                                                  \
	type* ZPL_JOIN2( func, get )( ZPL_JOIN2( func, type ) * pad )                                                                 \
	{                                                                                                                             \
		if ( ZPL_JOIN2( func, empty )( pad ) )                                                                                    \
		{                                                                                                                         \
			return NULL;                                                                                                          \
		}                                                                                                                         \
                                                                                                                                  \
		type* data = &pad->buf[ pad->tail ];                                                                                      \
		pad->tail  = ( pad->tail + 1 ) % pad->capacity;                                                                           \
                                                                                                                                  \
		return data;                                                                                                              \
	}                                                                                                                             \
                                                                                                                                  \
	Array( type ) ZPL_JOIN2( func, get_array )( ZPL_JOIN2( func, type ) * pad, ZPL_NS( uw ) max_size, ZPL_NS( AllocatorInfo ) a ) \
	{                                                                                                                             \
		Array( type ) vals = 0;                                                                                                   \
		array_init( vals, a );                                                                                                    \
		while ( --max_size && ! ZPL_JOIN2( func, empty )( pad ) )                                                                 \
		{                                                                                                                         \
			array_append( vals, *ZPL_JOIN2( func, get )( pad ) );                                                                 \
		}                                                                                                                         \
		return vals;                                                                                                              \
	}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: header/essentials/debug.h

/* Debugging stuff */

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

#ifndef ZPL_DEBUG_TRAP
#	if defined( _MSC_VER )
#		if _MSC_VER < 1300
#			define ZPL_DEBUG_TRAP() __asm int 3 /* Trap to debugger! */
#		else
#			define ZPL_DEBUG_TRAP() __debugbreak()
#		endif
#	elif defined( ZPL_COMPILER_TINYC )
#		define ZPL_DEBUG_TRAP() exit( 1 )
#	else
#		define ZPL_DEBUG_TRAP() __builtin_trap()
#	endif
#endif

#ifndef ZPL_ASSERT_MSG
#	define ZPL_ASSERT_MSG( cond, msg, ... )                                                                         \
		do                                                                                                           \
		{                                                                                                            \
			if ( ! ( cond ) )                                                                                        \
			{                                                                                                        \
				ZPL_NS( assert_handler )( #cond, __FILE__, zpl_cast( ZPL_NS( s64 ) ) __LINE__, msg, ##__VA_ARGS__ ); \
				ZPL_DEBUG_TRAP();                                                                                    \
			}                                                                                                        \
		} while ( 0 )
#endif

#ifndef ZPL_ASSERT
#	define ZPL_ASSERT( cond ) ZPL_ASSERT_MSG( cond, NULL )
#endif

#ifndef ZPL_ASSERT_NOT_NULL
#	define ZPL_ASSERT_NOT_NULL( ptr ) ZPL_ASSERT_MSG( ( ptr ) != NULL, #ptr " must not be NULL" )
#endif

// NOTE: Things that shouldn't happen with a message!
#ifndef ZPL_PANIC
#	define ZPL_PANIC( msg, ... ) ZPL_ASSERT_MSG( 0, msg, ##__VA_ARGS__ )
#endif

#ifndef ZPL_NOT_IMPLEMENTED
#	define ZPL_NOT_IMPLEMENTED ZPL_PANIC( "not implemented" )
#endif

/* Functions */

ZPL_DEF void assert_handler( char const* condition, char const* file, s32 line, char const* msg, ... );
ZPL_DEF s32  assert_crash( char const* condition );
ZPL_DEF void exit( u32 code );

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: header/essentials/memory.h

/** @file mem.c
@brief Memory manipulation and helpers.
@defgroup memman Memory management

 Consists of pointer arithmetic methods, virtual memory management and custom memory allocators.

 @{
 */

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

//! Checks if value is power of 2.
ZPL_DEF_INLINE b32 is_power_of_two( sw x );

//! Aligns address to specified alignment.
ZPL_DEF_INLINE void* align_forward( void* ptr, sw alignment );

//! Aligns value to a specified alignment.
ZPL_DEF_INLINE s64 align_forward_i64( s64 value, sw alignment );

//! Aligns value to a specified alignment.
ZPL_DEF_INLINE u64 align_forward_u64( u64 value, uw alignment );

//! Moves pointer forward by bytes.
ZPL_DEF_INLINE void* pointer_add( void* ptr, sw bytes );

//! Moves pointer backward by bytes.
ZPL_DEF_INLINE void* pointer_sub( void* ptr, sw bytes );

//! Moves pointer forward by bytes.
ZPL_DEF_INLINE void const* pointer_add_const( void const* ptr, sw bytes );

//! Moves pointer backward by bytes.
ZPL_DEF_INLINE void const* pointer_sub_const( void const* ptr, sw bytes );

//! Calculates difference between two addresses.
ZPL_DEF_INLINE sw pointer_diff( void const* begin, void const* end );

#define ptr_add       ZPL_NS( pointer_add )
#define ptr_sub       ZPL_NS( pointer_sub )
#define ptr_add_const ZPL_NS( pointer_add_const )
#define ptr_sub_const ZPL_NS( pointer_sub_const )
#define ptr_diff      ZPL_NS( pointer_diff )

//! Clears up memory at location by specified size.

//! @param ptr Memory location to clear up.
//! @param size The size to clear up with.
ZPL_DEF_INLINE void zero_size( void* ptr, sw size );

#ifndef zero_item
//! Clears up an item.
#	define zero_item( t ) zero_size( ( t ), size_of( *( t ) ) )    // NOTE: Pass pointer of struct

//! Clears up an array.
#	define zero_array( a, count ) zero_size( ( a ), size_of( *( a ) ) * count )
#endif

//! Copy memory from source to destination.
ZPL_DEF_INLINE void* memmove( void* dest, void const* source, sw size );

//! Set constant value at memory location with specified size.
ZPL_DEF_INLINE void* memset( void* data, u8 byte_value, sw size );

//! Compare two memory locations with specified size.
ZPL_DEF_INLINE s32 memcompare( void const* s1, void const* s2, sw size );

//! Swap memory contents between 2 locations with size.
ZPL_DEF void memswap( void* i, void* j, sw size );

//! Search for a constant value within the size limit at memory location.
ZPL_DEF void const* memchr( void const* data, u8 byte_value, sw size );

//! Search for a constant value within the size limit at memory location in backwards.
ZPL_DEF void const* memrchr( void const* data, u8 byte_value, sw size );

//! Copy non-overlapping memory from source to destination.
ZPL_DEF void* memcopy( void* dest, void const* source, sw size );

#ifndef memcopy_array

//! Copy non-overlapping array.
#	define memcopy_array( dst, src, count ) ZPL_NS( memcopy )( ( dst ), ( src ), size_of( *( dst ) ) * ( count ) )
#endif

//! Copy an array.
#ifndef memmove_array
#	define memmove_array( dst, src, count ) ZPL_NS( memmove )( ( dst ), ( src ), size_of( *( dst ) ) * ( count ) )
#endif

#ifndef ZPL_BIT_CAST
#	define ZPL_BIT_CAST( dest, source )                                                                            \
		do                                                                                                          \
		{                                                                                                           \
			ZPL_STATIC_ASSERT( size_of( *( dest ) ) <= size_of( source ), "size_of(*(dest)) !<= size_of(source)" ); \
			ZPL_NS( memcopy )( ( dest ), &( source ), size_of( *dest ) );                                           \
		} while ( 0 )
#endif

#ifndef kilobytes
#	define kilobytes( x ) ( ( x ) * ( ZPL_NS( s64 ) )( 1024 ) )
#	define megabytes( x ) ( kilobytes( x ) * ( ZPL_NS( s64 ) )( 1024 ) )
#	define gigabytes( x ) ( megabytes( x ) * ( ZPL_NS( s64 ) )( 1024 ) )
#	define terabytes( x ) ( gigabytes( x ) * ( ZPL_NS( s64 ) )( 1024 ) )
#endif


/* inlines */

#define ZPL__ONES          ( zpl_cast( ZPL_NS( uw ) ) - 1 / ZPL_U8_MAX )
#define ZPL__HIGHS         ( ZPL__ONES * ( ZPL_U8_MAX / 2 + 1 ) )
#define ZPL__HAS_ZERO( x ) ( ( ( x )-ZPL__ONES ) & ~( x )&ZPL__HIGHS )

ZPL_IMPL_INLINE void* align_forward( void* ptr, sw alignment )
{
	uptr p;

	ZPL_ASSERT( is_power_of_two( alignment ) );

	p = zpl_cast( uptr ) ptr;
	return zpl_cast( void* )( ( p + ( alignment - 1 ) ) & ~( alignment - 1 ) );
}

ZPL_IMPL_INLINE s64 align_forward_i64( s64 value, sw alignment )
{
	return value + ( alignment - value % alignment ) % alignment;
}

ZPL_IMPL_INLINE u64 align_forward_u64( u64 value, uw alignment )
{
	return value + ( alignment - value % alignment ) % alignment;
}

ZPL_IMPL_INLINE void* pointer_add( void* ptr, sw bytes )
{
	return zpl_cast( void* )( zpl_cast( u8* ) ptr + bytes );
}

ZPL_IMPL_INLINE void* pointer_sub( void* ptr, sw bytes )
{
	return zpl_cast( void* )( zpl_cast( u8* ) ptr - bytes );
}

ZPL_IMPL_INLINE void const* pointer_add_const( void const* ptr, sw bytes )
{
	return zpl_cast( void const* )( zpl_cast( u8 const* ) ptr + bytes );
}

ZPL_IMPL_INLINE void const* pointer_sub_const( void const* ptr, sw bytes )
{
	return zpl_cast( void const* )( zpl_cast( u8 const* ) ptr - bytes );
}

ZPL_IMPL_INLINE sw pointer_diff( void const* begin, void const* end )
{
	return zpl_cast( sw )( zpl_cast( u8 const* ) end - zpl_cast( u8 const* ) begin );
}

ZPL_IMPL_INLINE void zero_size( void* ptr, sw size )
{
	memset( ptr, 0, size );
}

#if defined( _MSC_VER ) && ! defined( __clang__ )
#	pragma intrinsic( __movsb )
#endif

ZPL_IMPL_INLINE void* memmove( void* dest, void const* source, sw n )
{
	if ( dest == NULL )
	{
		return NULL;
	}

	u8*       d = zpl_cast( u8* ) dest;
	u8 const* s = zpl_cast( u8 const* ) source;

	if ( d == s )
		return d;
	if ( s + n <= d || d + n <= s )    // NOTE: Non-overlapping
		return memcopy( d, s, n );

	if ( d < s )
	{
		if ( zpl_cast( uptr ) s % size_of( sw ) == zpl_cast( uptr ) d % size_of( sw ) )
		{
			while ( zpl_cast( uptr ) d % size_of( sw ) )
			{
				if ( ! n-- )
					return dest;
				*d++ = *s++;
			}
			while ( n >= size_of( sw ) )
			{
				*zpl_cast( sw* ) d  = *zpl_cast( sw* ) s;
				n                  -= size_of( sw );
				d                  += size_of( sw );
				s                  += size_of( sw );
			}
		}
		for ( ; n; n-- )
			*d++ = *s++;
	}
	else
	{
		if ( ( zpl_cast( uptr ) s % size_of( sw ) ) == ( zpl_cast( uptr ) d % size_of( sw ) ) )
		{
			while ( zpl_cast( uptr )( d + n ) % size_of( sw ) )
			{
				if ( ! n-- )
					return dest;
				d[ n ] = s[ n ];
			}
			while ( n >= size_of( sw ) )
			{
				n                         -= size_of( sw );
				*zpl_cast( sw* )( d + n )  = *zpl_cast( sw* )( s + n );
			}
		}
		while ( n )
			n--, d[ n ] = s[ n ];
	}

	return dest;
}

ZPL_IMPL_INLINE void* memset( void* dest, u8 c, sw n )
{
	if ( dest == NULL )
	{
		return NULL;
	}

	u8* s = zpl_cast( u8* ) dest;
	sw  k;
	u32 c32 = ( ( u32 )-1 ) / 255 * c;

	if ( n == 0 )
		return dest;
	s[ 0 ] = s[ n - 1 ] = c;
	if ( n < 3 )
		return dest;
	s[ 1 ] = s[ n - 2 ] = c;
	s[ 2 ] = s[ n - 3 ] = c;
	if ( n < 7 )
		return dest;
	s[ 3 ] = s[ n - 4 ] = c;
	if ( n < 9 )
		return dest;

	k  = -zpl_cast( sptr ) s & 3;
	s += k;
	n -= k;
	n &= -4;

	*zpl_cast( u32* )( s + 0 )     = c32;
	*zpl_cast( u32* )( s + n - 4 ) = c32;
	if ( n < 9 )
		return dest;
	*zpl_cast( u32* )( s + 4 )      = c32;
	*zpl_cast( u32* )( s + 8 )      = c32;
	*zpl_cast( u32* )( s + n - 12 ) = c32;
	*zpl_cast( u32* )( s + n - 8 )  = c32;
	if ( n < 25 )
		return dest;
	*zpl_cast( u32* )( s + 12 )     = c32;
	*zpl_cast( u32* )( s + 16 )     = c32;
	*zpl_cast( u32* )( s + 20 )     = c32;
	*zpl_cast( u32* )( s + 24 )     = c32;
	*zpl_cast( u32* )( s + n - 28 ) = c32;
	*zpl_cast( u32* )( s + n - 24 ) = c32;
	*zpl_cast( u32* )( s + n - 20 ) = c32;
	*zpl_cast( u32* )( s + n - 16 ) = c32;

	k  = 24 + ( zpl_cast( uptr ) s & 4 );
	s += k;
	n -= k;

	{
		u64 c64 = ( zpl_cast( u64 ) c32 << 32 ) | c32;
		while ( n > 31 )
		{
			*zpl_cast( u64* )( s + 0 )  = c64;
			*zpl_cast( u64* )( s + 8 )  = c64;
			*zpl_cast( u64* )( s + 16 ) = c64;
			*zpl_cast( u64* )( s + 24 ) = c64;

			n -= 32;
			s += 32;
		}
	}

	return dest;
}

ZPL_IMPL_INLINE s32 memcompare( void const* s1, void const* s2, sw size )
{
	u8 const* s1p8 = zpl_cast( u8 const* ) s1;
	u8 const* s2p8 = zpl_cast( u8 const* ) s2;

	if ( s1 == NULL || s2 == NULL )
	{
		return 0;
	}

	while ( size-- )
	{
		sw d;
		if ( ( d = ( *s1p8++ - *s2p8++ ) ) != 0 )
			return zpl_cast( s32 ) d;
	}
	return 0;
}

ZPL_IMPL_INLINE b32 is_power_of_two( sw x )
{
	if ( x <= 0 )
		return false;
	return ! ( x & ( x - 1 ) );
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: header/essentials/memory_custom.h

////////////////////////////////////////////////////////////////
//
// Custom Allocation
//
//

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

typedef enum alloc_type
{
	ZPL_ALLOCATION_ALLOC,
	ZPL_ALLOCATION_FREE,
	ZPL_ALLOCATION_FREE_ALL,
	ZPL_ALLOCATION_RESIZE,
} alloc_type;

// NOTE: This is useful so you can define an allocator of the same type and parameters
#define ZPL_ALLOCATOR_PROC( name ) \
	void* name( void* allocator_data, ZPL_NS( alloc_type ) type, ZPL_NS( sw ) size, ZPL_NS( sw ) alignment, void* old_memory, ZPL_NS( sw ) old_size, ZPL_NS( u64 ) flags )
typedef ZPL_ALLOCATOR_PROC( allocator_proc );

typedef struct AllocatorInfo
{
	allocator_proc* proc;
	void*           data;
} AllocatorInfo;

typedef enum alloc_flag
{
	ZPL_ALLOCATOR_FLAG_CLEAR_TO_ZERO = ZPL_BIT( 0 ),
} alloc_flag;

#ifndef ZPL_DEFAULT_MEMORY_ALIGNMENT
#	define ZPL_DEFAULT_MEMORY_ALIGNMENT ( 2 * size_of( void* ) )
#endif

#ifndef ZPL_DEFAULT_ALLOCATOR_FLAGS
#	define ZPL_DEFAULT_ALLOCATOR_FLAGS ( ZPL_NS( ZPL_ALLOCATOR_FLAG_CLEAR_TO_ZERO ) )
#endif

//! Allocate memory with specified alignment.
ZPL_DEF_INLINE void* alloc_align( AllocatorInfo a, sw size, sw alignment );

//! Allocate memory with default alignment.
ZPL_DEF_INLINE void* alloc( AllocatorInfo a, sw size );

//! Free allocated memory.
ZPL_DEF_INLINE void free( AllocatorInfo a, void* ptr );

//! Free all memory allocated by an allocator.
ZPL_DEF_INLINE void free_all( AllocatorInfo a );

//! Resize an allocated memory.
ZPL_DEF_INLINE void* resize( AllocatorInfo a, void* ptr, sw old_size, sw new_size );

//! Resize an allocated memory with specified alignment.
ZPL_DEF_INLINE void* resize_align( AllocatorInfo a, void* ptr, sw old_size, sw new_size, sw alignment );

//! Allocate memory and copy data into it.
ZPL_DEF_INLINE void* alloc_copy( AllocatorInfo a, void const* src, sw size );

//! Allocate memory with specified alignment and copy data into it.
ZPL_DEF_INLINE void* alloc_copy_align( AllocatorInfo a, void const* src, sw size, sw alignment );

//! Allocate memory for null-terminated C-String.
ZPL_DEF char* alloc_str( AllocatorInfo a, char const* str );

//! Allocate memory for C-String with specified size.
ZPL_DEF_INLINE char* alloc_str_len( AllocatorInfo a, char const* str, sw len );

#ifndef alloc_item

//! Allocate memory for an item.
#	define alloc_item( allocator_, Type ) ( Type* )ZPL_NS( alloc )( allocator_, size_of( Type ) )

//! Allocate memory for an array of items.
#	define alloc_array( allocator_, Type, count ) ( Type* )ZPL_NS( alloc )( allocator_, size_of( Type ) * ( count ) )
#endif

/* heap memory analysis tools */
/* define ZPL_HEAP_ANALYSIS to enable this feature */
/* call zpl_heap_stats_init at the beginning of the entry point */
/* you can call zpl_heap_stats_check near the end of the execution to validate any possible leaks */
ZPL_DEF void heap_stats_init( void );
ZPL_DEF sw   heap_stats_used_memory( void );
ZPL_DEF sw   heap_stats_alloc_count( void );
ZPL_DEF void heap_stats_check( void );

//! Allocate/Resize memory using default options.

//! Use this if you don't need a "fancy" resize allocation
ZPL_DEF_INLINE void* default_resize_align( AllocatorInfo a, void* ptr, sw old_size, sw new_size, sw alignment );

//! The heap allocator backed by operating system's memory manager.
ZPL_DEF_INLINE AllocatorInfo heap_allocator( void );
ZPL_DEF                      ZPL_ALLOCATOR_PROC( heap_allocator_proc );

#ifndef malloc

//! Helper to allocate memory using heap allocator.
#	define malloc( sz ) ZPL_NS( alloc )( ZPL_NS( heap_allocator )(), sz )

//! Helper to free memory allocated by heap allocator.
#	define mfree( ptr ) ZPL_NS( free )( ZPL_NS( heap_allocator )(), ptr )

//! Alias to heap allocator.
#	define heap ZPL_NS( heap_allocator )
#endif

//
// Arena Allocator
//

typedef struct Arena
{
	AllocatorInfo backing;
	void*         physical_start;
	sw            total_size;
	sw            total_allocated;
	sw            temp_count;
} Arena;

//! Initialize memory arena from existing memory region.
ZPL_DEF_INLINE void arena_init_from_memory( Arena* arena, void* start, sw size );

//! Initialize memory arena using existing memory allocator.
ZPL_DEF_INLINE void arena_init_from_allocator( Arena* arena, AllocatorInfo backing, sw size );

//! Initialize memory arena within an existing parent memory arena.
ZPL_DEF_INLINE void arena_init_sub( Arena* arena, Arena* parent_arena, sw size );

//! Release the memory used by memory arena.
ZPL_DEF_INLINE void arena_free( Arena* arena );


//! Retrieve memory arena's aligned allocation address.
ZPL_DEF_INLINE sw arena_alignment_of( Arena* arena, sw alignment );

//! Retrieve memory arena's remaining size.
ZPL_DEF_INLINE sw arena_size_remaining( Arena* arena, sw alignment );

//! Check whether memory arena has any temporary snapshots.
ZPL_DEF_INLINE void arena_check( Arena* arena );

//! Allocation Types: alloc, free_all, resize
ZPL_DEF_INLINE AllocatorInfo arena_allocator( Arena* arena );
ZPL_DEF                      ZPL_ALLOCATOR_PROC( arena_allocator_proc );

typedef struct arena_snapshot
{
	Arena* arena;
	sw     original_count;
} arena_snapshot;

//! Capture a snapshot of used memory in a memory arena.
ZPL_DEF_INLINE arena_snapshot arena_snapshot_begin( Arena* arena );

//! Reset memory arena's usage by a captured snapshot.
ZPL_DEF_INLINE void arena_snapshot_end( arena_snapshot tmp_mem );

//
// Pool Allocator
//


typedef struct Pool
{
	AllocatorInfo backing;
	void*         physical_start;
	void*         free_list;
	sw            block_size;
	sw            block_align;
	sw            total_size;
	sw            num_blocks;
} Pool;

//! Initialize pool allocator.
ZPL_DEF_INLINE void pool_init( Pool* pool, AllocatorInfo backing, sw num_blocks, sw block_size );

//! Initialize pool allocator with specific block alignment.
ZPL_DEF void pool_init_align( Pool* pool, AllocatorInfo backing, sw num_blocks, sw block_size, sw block_align );

//! Release the resources used by pool allocator.
ZPL_DEF_INLINE void pool_free( Pool* pool );

//! Allocation Types: alloc, free
ZPL_DEF_INLINE AllocatorInfo pool_allocator( Pool* pool );
ZPL_DEF                      ZPL_ALLOCATOR_PROC( pool_allocator_proc );

//
// Scratch Memory Allocator - Ring Buffer Based Arena
//

typedef struct allocation_header_ev
{
	sw size;
} allocation_header_ev;

ZPL_DEF_INLINE allocation_header_ev* allocation_header( void* data );
ZPL_DEF_INLINE void                  allocation_header_fill( allocation_header_ev* header, void* data, sw size );

#if defined( ZPL_ARCH_32_BIT )
#	define ZPL_ISIZE_HIGH_BIT 0x80000000
#elif defined( ZPL_ARCH_64_BIT )
#	define ZPL_ISIZE_HIGH_BIT 0x8000000000000000ll
#else
#	error
#endif

typedef struct scratch_memory
{
	void* physical_start;
	sw    total_size;
	void* alloc_point;
	void* free_point;
} scratch_memory;

//! Initialize ring buffer arena.
ZPL_DEF void scratch_memory_init( scratch_memory* s, void* start, sw size );

//! Check whether ring buffer arena is in use.
ZPL_DEF b32 scratch_memory_is_in_use( scratch_memory* s, void* ptr );

//! Allocation Types: alloc, free, free_all, resize
ZPL_DEF AllocatorInfo scratch_allocator( scratch_memory* s );
ZPL_DEF               ZPL_ALLOCATOR_PROC( scratch_allocator_proc );

//
// Stack Memory Allocator
//


typedef struct stack_memory
{
	AllocatorInfo backing;

	void* physical_start;
	uw    total_size;
	uw    allocated;
} stack_memory;

//! Initialize stack allocator from existing memory.
ZPL_DEF_INLINE void stack_memory_init_from_memory( stack_memory* s, void* start, sw size );

//! Initialize stack allocator using existing memory allocator.
ZPL_DEF_INLINE void stack_memory_init( stack_memory* s, AllocatorInfo backing, sw size );

//! Check whether stack allocator is in use.
ZPL_DEF_INLINE b32 stack_memory_is_in_use( stack_memory* s, void* ptr );

//! Release the resources used by stack allocator.
ZPL_DEF_INLINE void stack_memory_free( stack_memory* s );

//! Allocation Types: alloc, free, free_all
ZPL_DEF_INLINE AllocatorInfo stack_allocator( stack_memory* s );
ZPL_DEF                      ZPL_ALLOCATOR_PROC( stack_allocator_proc );

/* inlines */

ZPL_IMPL_INLINE void* alloc_align( AllocatorInfo a, sw size, sw alignment )
{
	return a.proc( a.data, ZPL_ALLOCATION_ALLOC, size, alignment, NULL, 0, ZPL_DEFAULT_ALLOCATOR_FLAGS );
}

ZPL_IMPL_INLINE void* alloc( AllocatorInfo a, sw size )
{
	return alloc_align( a, size, ZPL_DEFAULT_MEMORY_ALIGNMENT );
}

ZPL_IMPL_INLINE void free( AllocatorInfo a, void* ptr )
{
	if ( ptr != NULL )
		a.proc( a.data, ZPL_ALLOCATION_FREE, 0, 0, ptr, 0, ZPL_DEFAULT_ALLOCATOR_FLAGS );
}

ZPL_IMPL_INLINE void free_all( AllocatorInfo a )
{
	a.proc( a.data, ZPL_ALLOCATION_FREE_ALL, 0, 0, NULL, 0, ZPL_DEFAULT_ALLOCATOR_FLAGS );
}

ZPL_IMPL_INLINE void* resize( AllocatorInfo a, void* ptr, sw old_size, sw new_size )
{
	return resize_align( a, ptr, old_size, new_size, ZPL_DEFAULT_MEMORY_ALIGNMENT );
}

ZPL_IMPL_INLINE void* resize_align( AllocatorInfo a, void* ptr, sw old_size, sw new_size, sw alignment )
{
	return a.proc( a.data, ZPL_ALLOCATION_RESIZE, new_size, alignment, ptr, old_size, ZPL_DEFAULT_ALLOCATOR_FLAGS );
}

ZPL_IMPL_INLINE void* alloc_copy( AllocatorInfo a, void const* src, sw size )
{
	return memcopy( alloc( a, size ), src, size );
}

ZPL_IMPL_INLINE void* alloc_copy_align( AllocatorInfo a, void const* src, sw size, sw alignment )
{
	return memcopy( alloc_align( a, size, alignment ), src, size );
}

ZPL_IMPL_INLINE char* alloc_str_len( AllocatorInfo a, char const* str, sw len )
{
	char* result;
	result = zpl_cast( char* ) alloc( a, len + 1 );
	memmove( result, str, len );
	result[ len ] = '\0';
	return result;
}

ZPL_IMPL_INLINE void* default_resize_align( AllocatorInfo a, void* old_memory, sw old_size, sw new_size, sw alignment )
{
	if ( ! old_memory )
		return alloc_align( a, new_size, alignment );

	if ( new_size == 0 )
	{
		free( a, old_memory );
		return NULL;
	}

	if ( new_size < old_size )
		new_size = old_size;

	if ( old_size == new_size )
	{
		return old_memory;
	}
	else
	{
		void* new_memory = alloc_align( a, new_size, alignment );
		if ( ! new_memory )
			return NULL;
		memmove( new_memory, old_memory, min( new_size, old_size ) );
		free( a, old_memory );
		return new_memory;
	}
}

//
// Heap Allocator
//

ZPL_IMPL_INLINE AllocatorInfo heap_allocator( void )
{
	AllocatorInfo a;
	a.proc = heap_allocator_proc;
	a.data = NULL;
	return a;
}

//
// Arena Allocator
//

ZPL_IMPL_INLINE void arena_init_from_memory( Arena* arena, void* start, sw size )
{
	arena->backing.proc    = NULL;
	arena->backing.data    = NULL;
	arena->physical_start  = start;
	arena->total_size      = size;
	arena->total_allocated = 0;
	arena->temp_count      = 0;
}

ZPL_IMPL_INLINE void arena_init_from_allocator( Arena* arena, AllocatorInfo backing, sw size )
{
	arena->backing         = backing;
	arena->physical_start  = alloc( backing, size );    // NOTE: Uses default alignment
	arena->total_size      = size;
	arena->total_allocated = 0;
	arena->temp_count      = 0;
}

ZPL_IMPL_INLINE void arena_init_sub( Arena* arena, Arena* parent_arena, sw size )
{
	arena_init_from_allocator( arena, arena_allocator( parent_arena ), size );
}

ZPL_IMPL_INLINE void arena_free( Arena* arena )
{
	if ( arena->backing.proc )
	{
		free( arena->backing, arena->physical_start );
		arena->physical_start = NULL;
	}
}

ZPL_IMPL_INLINE sw arena_alignment_of( Arena* arena, sw alignment )
{
	sw alignment_offset, result_pointer, mask;
	ZPL_ASSERT( is_power_of_two( alignment ) );

	alignment_offset = 0;
	result_pointer   = zpl_cast( sw ) arena->physical_start + arena->total_allocated;
	mask             = alignment - 1;
	if ( result_pointer & mask )
		alignment_offset = alignment - ( result_pointer & mask );

	return alignment_offset;
}

ZPL_IMPL_INLINE sw arena_size_remaining( Arena* arena, sw alignment )
{
	sw result = arena->total_size - ( arena->total_allocated + arena_alignment_of( arena, alignment ) );
	return result;
}

ZPL_IMPL_INLINE void arena_check( Arena* arena )
{
	ZPL_ASSERT( arena->temp_count == 0 );
}

ZPL_IMPL_INLINE AllocatorInfo arena_allocator( Arena* arena )
{
	AllocatorInfo allocator;
	allocator.proc = arena_allocator_proc;
	allocator.data = arena;
	return allocator;
}

ZPL_IMPL_INLINE arena_snapshot arena_snapshot_begin( Arena* arena )
{
	arena_snapshot tmp;
	tmp.arena          = arena;
	tmp.original_count = arena->total_allocated;
	arena->temp_count++;
	return tmp;
}

ZPL_IMPL_INLINE void arena_snapshot_end( arena_snapshot tmp )
{
	ZPL_ASSERT( tmp.arena->total_allocated >= tmp.original_count );
	ZPL_ASSERT( tmp.arena->temp_count > 0 );
	tmp.arena->total_allocated = tmp.original_count;
	tmp.arena->temp_count--;
}

//
// Pool Allocator
//

ZPL_IMPL_INLINE void pool_init( Pool* pool, AllocatorInfo backing, sw num_blocks, sw block_size )
{
	pool_init_align( pool, backing, num_blocks, block_size, ZPL_DEFAULT_MEMORY_ALIGNMENT );
}

ZPL_IMPL_INLINE void pool_free( Pool* pool )
{
	if ( pool->backing.proc )
	{
		free( pool->backing, pool->physical_start );
	}
}

ZPL_IMPL_INLINE AllocatorInfo pool_allocator( Pool* pool )
{
	AllocatorInfo allocator;
	allocator.proc = pool_allocator_proc;
	allocator.data = pool;
	return allocator;
}

ZPL_IMPL_INLINE allocation_header_ev* allocation_header( void* data )
{
	sw* p = zpl_cast( sw* ) data;
	while ( p[ -1 ] == zpl_cast( sw )( -1 ) )
		p--;
	return zpl_cast( allocation_header_ev* ) p - 1;
}

ZPL_IMPL_INLINE void allocation_header_fill( allocation_header_ev* header, void* data, sw size )
{
	sw* ptr;
	header->size = size;
	ptr          = zpl_cast( sw* )( header + 1 );
	while ( zpl_cast( void* ) ptr < data )
		*ptr++ = zpl_cast( sw )( -1 );
}

//
// Stack Memory Allocator
//

#define ZPL_STACK_ALLOC_OFFSET sizeof( ZPL_NS( u64 ) )
ZPL_STATIC_ASSERT( ZPL_STACK_ALLOC_OFFSET == 8, "ZPL_STACK_ALLOC_OFFSET != 8" );

ZPL_IMPL_INLINE void stack_memory_init_from_memory( stack_memory* s, void* start, sw size )
{
	s->physical_start = start;
	s->total_size     = size;
	s->allocated      = 0;
}

ZPL_IMPL_INLINE void stack_memory_init( stack_memory* s, AllocatorInfo backing, sw size )
{
	s->backing        = backing;
	s->physical_start = alloc( backing, size );
	s->total_size     = size;
	s->allocated      = 0;
}

ZPL_IMPL_INLINE b32 stack_memory_is_in_use( stack_memory* s, void* ptr )
{
	if ( s->allocated == 0 )
		return false;

	if ( ptr > s->physical_start && ptr < pointer_add( s->physical_start, s->total_size ) )
	{
		return true;
	}

	return false;
}

ZPL_IMPL_INLINE void stack_memory_free( stack_memory* s )
{
	if ( s->backing.proc )
	{
		free( s->backing, s->physical_start );
		s->physical_start = NULL;
	}
}

ZPL_IMPL_INLINE AllocatorInfo stack_allocator( stack_memory* s )
{
	AllocatorInfo a;
	a.proc = stack_allocator_proc;
	a.data = s;
	return a;
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

#		if defined( ZPL_MODULE_CORE )
// file: header/core/file.h

/** @file file.c
@brief File handling
@defgroup fileio File handling

File I/O operations as well as path and folder structure manipulation methods. With threading enabled, it also offers async read/write methods.

@{
*/

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

typedef u32 file_mode;

typedef enum file_mode_flag
{
	EFileMode_READ   = ZPL_BIT( 0 ),
	EFileMode_WRITE  = ZPL_BIT( 1 ),
	EFileMode_APPEND = ZPL_BIT( 2 ),
	EFileMode_RW     = ZPL_BIT( 3 ),
	ZPL_FILE_MODES   = EFileMode_READ | EFileMode_WRITE | EFileMode_APPEND | EFileMode_RW,
} file_mode_flag;

// NOTE: Only used internally and for the file operations
typedef enum seek_whence_type
{
	ZPL_SEEK_WHENCE_BEGIN   = 0,
	ZPL_SEEK_WHENCE_CURRENT = 1,
	ZPL_SEEK_WHENCE_END     = 2,
} seek_whence_type;

typedef enum FileError
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
} FileError;

typedef union file_descriptor
{
	void* p;
	sptr  i;
	uptr  u;
} file_descriptor;

typedef struct file_operations file_operations;

#define ZPL_FILE_OPEN_PROC( name )     FileError name( file_descriptor* fd, file_operations* ops, file_mode mode, char const* filename )
#define ZPL_FILE_READ_AT_PROC( name )  b32 name( file_descriptor fd, void* buffer, sw size, s64 offset, sw* bytes_read, b32 stop_at_newline )
#define ZPL_FILE_WRITE_AT_PROC( name ) b32 name( file_descriptor fd, void const* buffer, sw size, s64 offset, sw* bytes_written )
#define ZPL_FILE_SEEK_PROC( name )     b32 name( file_descriptor fd, s64 offset, seek_whence_type whence, s64* new_offset )
#define ZPL_FILE_CLOSE_PROC( name )    void name( file_descriptor fd )

typedef ZPL_FILE_OPEN_PROC( file_open_proc );
typedef ZPL_FILE_READ_AT_PROC( file_read_proc );
typedef ZPL_FILE_WRITE_AT_PROC( file_write_proc );
typedef ZPL_FILE_SEEK_PROC( file_seek_proc );
typedef ZPL_FILE_CLOSE_PROC( file_close_proc );

struct file_operations
{
	file_read_proc*  read_at;
	file_write_proc* write_at;
	file_seek_proc*  seek;
	file_close_proc* close;
};

extern file_operations const default_file_operations;

typedef u64 file_time;

typedef enum dir_type
{
	ZPL_DIR_TYPE_FILE,
	ZPL_DIR_TYPE_FOLDER,
	ZPL_DIR_TYPE_UNKNOWN,
} dir_type;

struct dir_info;

typedef struct dir_entry
{
	char const*      filename;
	struct dir_info* dir_info;
	u8               type;
} dir_entry;

typedef struct dir_info
{
	char const* fullpath;
	dir_entry*  entries;    // zpl_array

	// Internals
	char** filenames;    // zpl_array
	String buf;
} dir_info;

typedef struct FileInfo
{
	file_operations ops;
	file_descriptor fd;
	b32             is_temp;

	char const* filename;
	file_time   last_write_time;
	dir_entry*  dir;
} FileInfo;

typedef enum file_standard_type
{
	ZPL_FILE_STANDARD_INPUT,
	ZPL_FILE_STANDARD_OUTPUT,
	ZPL_FILE_STANDARD_ERROR,

	ZPL_FILE_STANDARD_COUNT,
} file_standard_type;

/**
 * Get standard file I/O.
 * @param  std Check zpl_file_standard_type
 * @return     File handle to standard I/O
 */
ZPL_DEF FileInfo* file_get_standard( file_standard_type std );

/**
 * Connects a system handle to a zpl file.
 * @param  file   Pointer to zpl file
 * @param  handle Low-level OS handle to connect
 */
ZPL_DEF void file_connect_handle( FileInfo* file, void* handle );

/**
 * Creates a new file
 * @param  file
 * @param  filename
 */
ZPL_DEF FileError file_create( FileInfo* file, char const* filename );

/**
 * Opens a file
 * @param  file
 * @param  filename
 */
ZPL_DEF FileError file_open( FileInfo* file, char const* filename );

/**
 * Opens a file using a specified mode
 * @param  file
 * @param  mode     Access mode to use
 * @param  filename
 */
ZPL_DEF FileError file_open_mode( FileInfo* file, file_mode mode, char const* filename );

/**
 * Constructs a new file from data
 * @param  file
 * @param  fd       Low-level file descriptor to use
 * @param  ops      File operations to rely upon
 * @param  filename
 */
ZPL_DEF FileError file_new( FileInfo* file, file_descriptor fd, file_operations ops, char const* filename );

/**
 * Returns a size of the file
 * @param  file
 * @return      File size
 */
ZPL_DEF s64 file_size( FileInfo* file );

/**
 * Returns the currently opened file's name
 * @param  file
 */
ZPL_DEF char const* file_name( FileInfo* file );

/**
 * Truncates the file by a specified size
 * @param  file
 * @param  size Size to truncate
 */
ZPL_DEF FileError file_truncate( FileInfo* file, s64 size );

/**
 * Checks whether a file's been changed since the last check
 * @param  file
 */
ZPL_DEF b32 file_has_changed( FileInfo* file );

/**
 * Retrieves a directory listing relative to the file
 * @param file
 */
ZPL_DEF void file_dirinfo_refresh( FileInfo* file );

/**
 * Creates a temporary file
 * @param  file
 */
FileError file_temp( FileInfo* file );

/**
 * Closes the file
 * @param  file
 */
ZPL_DEF FileError file_close( FileInfo* file );

/**
 * Reads file safely
 * @param  file
 * @param  buffer     Buffer to read to
 * @param  size       Size to read
 * @param  offset     Offset to read from
 * @param  bytes_read How much data we've actually read
 */
ZPL_DEF_INLINE b32 file_read_at_check( FileInfo* file, void* buffer, sw size, s64 offset, sw* bytes_read );

/**
 * Writes to file safely
 * @param  file
 * @param  buffer        Buffer to read from
 * @param  size          Size to write
 * @param  offset        Offset to write to
 * @param  bytes_written How much data we've actually written
 */
ZPL_DEF_INLINE b32 file_write_at_check( FileInfo* file, void const* buffer, sw size, s64 offset, sw* bytes_written );


/**
 * Reads file at a specific offset
 * @param  file
 * @param  buffer     Buffer to read to
 * @param  size       Size to read
 * @param  offset     Offset to read from
 * @param  bytes_read How much data we've actually read
 */
ZPL_DEF_INLINE b32 file_read_at( FileInfo* file, void* buffer, sw size, s64 offset );

/**
 * Writes to file at a specific offset
 * @param  file
 * @param  buffer        Buffer to read from
 * @param  size          Size to write
 * @param  offset        Offset to write to
 * @param  bytes_written How much data we've actually written
 */
ZPL_DEF_INLINE b32 file_write_at( FileInfo* file, void const* buffer, sw size, s64 offset );

/**
 * Seeks the file cursor from the beginning of file to a specific position
 * @param  file
 * @param  offset Offset to seek to
 */
ZPL_DEF_INLINE s64 file_seek( FileInfo* file, s64 offset );

/**
 * Seeks the file cursor to the end of the file
 * @param  file
 */
ZPL_DEF_INLINE s64 file_seek_to_end( FileInfo* file );

/**
 * Skips N bytes at the current position
 * @param  file
 * @param  bytes Bytes to skip
 */
ZPL_DEF_INLINE s64 file_skip( FileInfo* file, s64 bytes );    // NOTE: Skips a certain amount of bytes

/**
 * Returns the length from the beginning of the file we've read so far
 * @param  file
 * @return      Our current position in file
 */
ZPL_DEF_INLINE s64 file_tell( FileInfo* file );

/**
 * Reads from a file
 * @param  file
 * @param  buffer Buffer to read to
 * @param  size   Size to read
 */
ZPL_DEF_INLINE b32 file_read( FileInfo* file, void* buffer, sw size );

/**
 * Writes to a file
 * @param  file
 * @param  buffer Buffer to read from
 * @param  size   Size to read
 */
ZPL_DEF_INLINE b32 file_write( FileInfo* file, void const* buffer, sw size );

typedef struct file_contents
{
	AllocatorInfo allocator;
	void*         data;
	sw            size;
} file_contents;

/**
 * Reads the whole file contents
 * @param  a              Allocator to use
 * @param  zero_terminate End the read data with null terminator
 * @param  filepath       Path to the file
 * @return                File contents data
 */
ZPL_DEF file_contents file_read_contents( AllocatorInfo a, b32 zero_terminate, char const* filepath );

/**
 * Frees the file content data previously read
 * @param  fc
 */
ZPL_DEF void file_free_contents( file_contents* fc );

/**
 * Writes content to a file
 */
ZPL_DEF b32 file_write_contents( char const* filepath, void const* buffer, sw size, FileError* err );

/**
 * Reads the file as array of lines
 *
 * Make sure you free both the returned buffer and the lines (zpl_array)
 * @param  alloc            Allocator to use
 * @param  lines            Reference to zpl_array container we store lines to
 * @param  filename         Path to the file
 * @param  strip_whitespace Strip whitespace when we split to lines?
 * @return                  File content we've read itself
 */
ZPL_DEF char* file_read_lines( AllocatorInfo alloc, Array( char* ) * lines, char const* filename, b32 strip_whitespace );

//! @}

/* inlines */


ZPL_IMPL_INLINE b32 file_read_at_check( FileInfo* f, void* buffer, sw size, s64 offset, sw* bytes_read )
{
	if ( ! f->ops.read_at )
		f->ops = default_file_operations;
	return f->ops.read_at( f->fd, buffer, size, offset, bytes_read, false );
}

ZPL_IMPL_INLINE b32 file_write_at_check( FileInfo* f, void const* buffer, sw size, s64 offset, sw* bytes_written )
{
	if ( ! f->ops.read_at )
		f->ops = default_file_operations;
	return f->ops.write_at( f->fd, buffer, size, offset, bytes_written );
}

ZPL_IMPL_INLINE b32 file_read_at( FileInfo* f, void* buffer, sw size, s64 offset )
{
	return file_read_at_check( f, buffer, size, offset, NULL );
}

ZPL_IMPL_INLINE b32 file_write_at( FileInfo* f, void const* buffer, sw size, s64 offset )
{
	return file_write_at_check( f, buffer, size, offset, NULL );
}

ZPL_IMPL_INLINE s64 file_seek( FileInfo* f, s64 offset )
{
	s64 new_offset = 0;
	if ( ! f->ops.read_at )
		f->ops = default_file_operations;
	f->ops.seek( f->fd, offset, ZPL_SEEK_WHENCE_BEGIN, &new_offset );
	return new_offset;
}

ZPL_IMPL_INLINE s64 file_seek_to_end( FileInfo* f )
{
	s64 new_offset = 0;
	if ( ! f->ops.read_at )
		f->ops = default_file_operations;
	f->ops.seek( f->fd, 0, ZPL_SEEK_WHENCE_END, &new_offset );
	return new_offset;
}

// NOTE: Skips a certain amount of bytes
ZPL_IMPL_INLINE s64 file_skip( FileInfo* f, s64 bytes )
{
	s64 new_offset = 0;
	if ( ! f->ops.read_at )
		f->ops = default_file_operations;
	f->ops.seek( f->fd, bytes, ZPL_SEEK_WHENCE_CURRENT, &new_offset );
	return new_offset;
}

ZPL_IMPL_INLINE s64 file_tell( FileInfo* f )
{
	s64 new_offset = 0;
	if ( ! f->ops.read_at )
		f->ops = default_file_operations;
	f->ops.seek( f->fd, 0, ZPL_SEEK_WHENCE_CURRENT, &new_offset );
	return new_offset;
}

ZPL_IMPL_INLINE b32 file_read( FileInfo* f, void* buffer, sw size )
{
	s64 cur_offset = file_tell( f );
	b32 result     = file_read_at( f, buffer, size, file_tell( f ) );
	file_seek( f, cur_offset + size );
	return result;
}

ZPL_IMPL_INLINE b32 file_write( FileInfo* f, void const* buffer, sw size )
{
	s64 cur_offset = file_tell( f );
	b32 result     = file_write_at( f, buffer, size, file_tell( f ) );
	file_seek( f, cur_offset + size );
	return result;
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: header/core/file_misc.h


ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

#ifndef ZPL_PATH_SEPARATOR
#	if defined( ZPL_SYSTEM_WINDOWS )
#		define ZPL_PATH_SEPARATOR '\\'
#	else
#		define ZPL_PATH_SEPARATOR '/'
#	endif
#endif

#ifndef ZPL_MAX_PATH
#	if defined( ZPL_SYSTEM_WINDOWS )
#		define ZPL_MAX_PATH MAX_PATH
#	elif defined( ZPL_SYSTEM_UNIX ) && ! defined( ZPL_SYSTEM_EMSCRIPTEN )
#		define ZPL_MAX_PATH PATH_MAX
#	else
#		define ZPL_MAX_PATH 4096
#	endif
#endif

/**
 * Checks if file/directory exists
 * @param  filepath
 */
ZPL_DEF b32 fs_exists( char const* filepath );

/**
 * Retrieves node's type (file, folder, ...)
 * @param  path
 */
ZPL_DEF u8 fs_get_type( char const* path );

/**
 * Retrieves file's last write time
 * @param  filepath
 */
ZPL_DEF file_time fs_last_write_time( char const* filepath );

/**
 * Copies the file to a directory
 * @param  existing_filename
 * @param  new_filename
 * @param  fail_if_exists
 */
ZPL_DEF b32 fs_copy( char const* existing_filename, char const* new_filename, b32 fail_if_exists );

/**
 * Moves the file to a directory
 * @param  existing_filename
 * @param  new_filename
 */
ZPL_DEF b32 fs_move( char const* existing_filename, char const* new_filename );

/**
 * Removes a file from a directory
 * @param  filename
 */
ZPL_DEF b32 fs_remove( char const* filename );

ZPL_DEF_INLINE b32 path_is_absolute( char const* path );
ZPL_DEF_INLINE b32 path_is_relative( char const* path );
ZPL_DEF_INLINE b32 path_is_root( char const* path );

ZPL_DEF_INLINE char const* path_base_name( char const* path );
ZPL_DEF_INLINE char const* path_extension( char const* path );

ZPL_DEF void path_fix_slashes( char* path );

ZPL_DEF FileError path_mkdir( char const* path, s32 mode );
ZPL_DEF sw        path_mkdir_recursive( char const* path, s32 mode );
ZPL_DEF FileError path_rmdir( char const* path );

ZPL_DEF char* path_get_full_name( AllocatorInfo a, char const* path );

/**
 * Returns file paths terminated by newline (\n)
 * @param  alloc   [description]
 * @param  dirname [description]
 * @param  recurse [description]
 * @return         [description]
 */
ZPL_DEF /*zpl_string*/ char* path_dirlist( AllocatorInfo alloc, char const* dirname, b32 recurse );

/**
 * Initialize dirinfo from specified path
 * @param dir  [description]
 * @param path [description]
 */
ZPL_DEF void dirinfo_init( dir_info* dir, char const* path );
ZPL_DEF void dirinfo_free( dir_info* dir );

/**
 * Analyze the entry's dirinfo
 * @param dir_entry [description]
 */
ZPL_DEF void dirinfo_step( dir_entry* dir_entry );

/* inlines */

ZPL_IMPL_INLINE b32 path_is_absolute( char const* path )
{
	b32 result = false;
	ZPL_ASSERT_NOT_NULL( path );
#if defined( ZPL_SYSTEM_WINDOWS )
	result = ( strlen( path ) > 2 ) && char_is_alpha( path[ 0 ] ) && ( path[ 1 ] == ':' && path[ 2 ] == ZPL_PATH_SEPARATOR );
#else
	result = ( strlen( path ) > 0 && path[ 0 ] == ZPL_PATH_SEPARATOR );
#endif
	return result;
}

ZPL_IMPL_INLINE b32 path_is_relative( char const* path )
{
	return ! path_is_absolute( path );
}

ZPL_IMPL_INLINE b32 path_is_root( char const* path )
{
	b32 result = false;
	ZPL_ASSERT_NOT_NULL( path );
#if defined( ZPL_SYSTEM_WINDOWS )
	result = path_is_absolute( path ) && ( strlen( path ) == 3 );
#else
	result = path_is_absolute( path ) && ( strlen( path ) == 1 );
#endif
	return result;
}

ZPL_IMPL_INLINE char const* path_base_name( char const* path )
{
	char const* ls;
	ZPL_ASSERT_NOT_NULL( path );
	path_fix_slashes( ( char* )path );
	ls = char_last_occurence( path, ZPL_PATH_SEPARATOR );
	return ( ls == NULL ) ? path : ls + 1;
}

ZPL_IMPL_INLINE char const* path_extension( char const* path )
{
	char const* ld;
	ZPL_ASSERT_NOT_NULL( path );
	ld = char_last_occurence( path, '.' );
	return ( ld == NULL ) ? NULL : ld + 1;
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: header/core/file_stream.h

/** @file file_stream.c
@brief File stream
@defgroup fileio File stream

File streaming operations on memory.

@{
*/

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

typedef enum
{
	/* Allows us to write to the buffer directly. Beware: you can not append a new data! */
	ZPL_FILE_STREAM_WRITABLE = ZPL_BIT( 0 ),

	/* Clones the input buffer so you can write (zpl_file_write*) data into it. */
	/* Since we work with a clone, the buffer size can dynamically grow as well. */
	ZPL_FILE_STREAM_CLONE_WRITABLE = ZPL_BIT( 1 ),
} file_stream_flags;

/**
 * Opens a new memory stream
 * @param file
 * @param allocator
 */
ZPL_DEF b8 file_stream_new( FileInfo* file, AllocatorInfo allocator );

/**
 * Opens a memory stream over an existing buffer
 * @param  file
 * @param  allocator
 * @param  buffer   Memory to create stream from
 * @param  size     Buffer's size
 * @param  flags
 */
ZPL_DEF b8 file_stream_open( FileInfo* file, AllocatorInfo allocator, u8* buffer, sw size, file_stream_flags flags );

/**
 * Retrieves the stream's underlying buffer and buffer size.
 * @param file memory stream
 * @param size (Optional) buffer size
 */
ZPL_DEF u8* file_stream_buf( FileInfo* file, sw* size );

extern file_operations const memory_file_operations;

//! @}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: header/core/file_tar.h

/** @file file_tar.c
@brief Tar archiving module
@defgroup fileio Tar module

Allows to easily pack/unpack files.
Based on: https://github.com/rxi/microtar/

Disclaimer: The pack method does not support file permissions nor GID/UID information. Only regular files are supported.
Use zpl_tar_pack_dir to pack an entire directory recursively. Empty folders are ignored.

@{
*/


ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

typedef enum
{
	ZPL_TAR_ERROR_NONE,
	ZPL_TAR_ERROR_INTERRUPTED,
	ZPL_TAR_ERROR_IO_ERROR,
	ZPL_TAR_ERROR_BAD_CHECKSUM,
	ZPL_TAR_ERROR_FILE_NOT_FOUND,
	ZPL_TAR_ERROR_INVALID_INPUT,
} tar_errors;

typedef enum
{
	ZPL_TAR_TYPE_REGULAR = '0',
	ZPL_TAR_TYPE_LINK    = '1',
	ZPL_TAR_TYPE_SYMBOL  = '2',
	ZPL_TAR_TYPE_CHR     = '3',
	ZPL_TAR_TYPE_BLK     = '4',
	ZPL_TAR_TYPE_DIR     = '5',
	ZPL_TAR_TYPE_FIFO    = '6'
} tar_file_type;

typedef struct
{
	char  type;
	char* path;
	s64   offset;
	s64   length;
	sw    error;
} tar_record;

#define ZPL_TAR_UNPACK_PROC( name ) ZPL_NS( sw ) name( ZPL_NS( FileInfo ) * archive, ZPL_NS( tar_record ) * file, void* user_data )
typedef ZPL_TAR_UNPACK_PROC( tar_unpack_proc );

/**
 * @brief Packs a list of files
 * Packs a list of provided files. Note that this method only supports regular files
 * and does not provide extended info such as GID/UID or permissions.
 * @param archive archive we pack files into
 * @param paths list of files
 * @param paths_len number of files provided
 * @return error
 */
ZPL_DEF sw tar_pack( FileInfo* archive, char const** paths, sw paths_len );

/**
 * @brief Packs an entire directory
 * Packs an entire directory of files recursively.
 * @param archive archive we pack files to
 * @param path folder to pack
 * @param alloc memory allocator to use (ex. zpl_heap())
 * @return error
 */
ZPL_DEF sw tar_pack_dir( FileInfo* archive, char const* path, AllocatorInfo alloc );

/**
 * @brief Unpacks an existing archive
 * Unpacks an existing archive. Users provide a callback in which information about file is provided.
 * Library does not unpack files to the filesystem nor reads any file data.
 * @param archive archive we unpack files from
 * @param unpack_proc callback we call per each file parsed
 * @param user_data user provided data
 * @return error
 */
ZPL_DEF sw tar_unpack( FileInfo* archive, tar_unpack_proc* unpack_proc, void* user_data );

/**
 * @brief Unpacks an existing archive into directory
 * Unpacks an existing archive into directory. The folder structure will be re-created automatically.
 * @param archive archive we unpack files from
 * @param dest directory to unpack files to
 * @return error
 */
ZPL_DEF_INLINE sw tar_unpack_dir( FileInfo* archive, char const* dest );

ZPL_DEF ZPL_TAR_UNPACK_PROC( tar_default_list_file );
ZPL_DEF ZPL_TAR_UNPACK_PROC( tar_default_unpack_file );

//! @}

ZPL_IMPL_INLINE sw tar_unpack_dir( FileInfo* archive, char const* dest )
{
	return tar_unpack( archive, tar_default_unpack_file, zpl_cast( void* ) dest );
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: header/core/memory_virtual.h


////////////////////////////////////////////////////////////////
//
// Virtual Memory
//
//

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

typedef struct virtual_memory
{
	void* data;
	sw    size;
} virtual_memory;

//! Initialize virtual memory from existing data.
ZPL_DEF virtual_memory vm( void* data, sw size );

//! Allocate virtual memory at address with size.

//! @param addr The starting address of the region to reserve. If NULL, it lets operating system to decide where to allocate it.
//! @param size The size to serve.
ZPL_DEF virtual_memory vm_alloc( void* addr, sw size );

//! Release the virtual memory.
ZPL_DEF b32 vm_free( virtual_memory vm );

//! Trim virtual memory.
ZPL_DEF virtual_memory vm_trim( virtual_memory vm, sw lead_size, sw size );

//! Purge virtual memory.
ZPL_DEF b32 vm_purge( virtual_memory vm );

//! Retrieve VM's page size and alignment.
ZPL_DEF sw virtual_memory_page_size( sw* alignment_out );

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: header/core/misc.h

/** @file misc.c
@brief Various other stuff
@defgroup misc Various other stuff

 Methods that don't belong anywhere but are still very useful in many occasions.

 @{
 */

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

ZPL_DEF void yield( void );

//! Returns allocated buffer
ZPL_DEF const char* get_env( const char* name );
ZPL_DEF const char* get_env_buf( const char* name );
ZPL_DEF String      get_env_str( const char* name );
ZPL_DEF void        set_env( const char* name, const char* value );
ZPL_DEF void        unset_env( const char* name );

ZPL_DEF u32    system_command( const char* command, uw buffer_len, char* buffer );
ZPL_DEF String system_command_str( const char* command, AllocatorInfo backing );

ZPL_DEF_INLINE u16 endian_swap16( u16 i );
ZPL_DEF_INLINE u32 endian_swap32( u32 i );
ZPL_DEF_INLINE u64 endian_swap64( u64 i );

ZPL_DEF_INLINE sw count_set_bits( u64 mask );

//! @}
//$$

ZPL_IMPL_INLINE u16 endian_swap16( u16 i )
{
	return ( i >> 8 ) | ( i << 8 );
}

ZPL_IMPL_INLINE u32 endian_swap32( u32 i )
{
	return ( i >> 24 ) | ( i << 24 ) | ( ( i & 0x00ff0000u ) >> 8 ) | ( ( i & 0x0000ff00u ) << 8 );
}

ZPL_IMPL_INLINE u64 endian_swap64( u64 i )
{
	return ( i >> 56 ) | ( i << 56 ) | ( ( i & 0x00ff000000000000ull ) >> 40 ) | ( ( i & 0x000000000000ff00ull ) << 40 ) | ( ( i & 0x0000ff0000000000ull ) >> 24 )
	    | ( ( i & 0x0000000000ff0000ull ) << 24 ) | ( ( i & 0x000000ff00000000ull ) >> 8 ) | ( ( i & 0x00000000ff000000ull ) << 8 );
}

ZPL_IMPL_INLINE s32 next_pow2( s32 x )
{
	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return x + 1;
}

ZPL_IMPL_INLINE void bit_set( u32* x, u32 bit )
{
	*x = *x | ( 1 << bit );
}

ZPL_IMPL_INLINE b8 bit_get( u32 x, u32 bit )
{
	return ( x & ( 1 << bit ) );
}

ZPL_IMPL_INLINE void bit_reset( u32* x, u32 bit )
{
	*x = *x & ~( 1 << bit );
}

ZPL_IMPL_INLINE sw count_set_bits( u64 mask )
{
	sw count = 0;
	while ( mask )
	{
		count  += ( mask & 1 );
		mask  >>= 1;
	}
	return count;
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: header/core/print.h

/** @file print.c
@brief Printing methods
@defgroup print Printing methods

Various printing methods.
@{
*/

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

#ifndef ZPL_PRINTF_MAXLEN
#	define ZPL_PRINTF_MAXLEN 65536
#endif

ZPL_DEF sw printf( char const* fmt, ... );
ZPL_DEF sw printf_va( char const* fmt, va_list va );
ZPL_DEF sw printf_err( char const* fmt, ... );
ZPL_DEF sw printf_err_va( char const* fmt, va_list va );
ZPL_DEF sw fprintf( FileInfo* f, char const* fmt, ... );
ZPL_DEF sw fprintf_va( FileInfo* f, char const* fmt, va_list va );

// NOTE: A locally persisting buffer is used internally
ZPL_DEF char* bprintf( char const* fmt, ... );

// NOTE: A locally persisting buffer is used internally
ZPL_DEF char* bprintf_va( char const* fmt, va_list va );

ZPL_DEF sw asprintf( AllocatorInfo allocator, char** buffer, char const* fmt, ... );
ZPL_DEF sw asprintf_va( AllocatorInfo allocator, char** buffer, char const* fmt, va_list va );

ZPL_DEF sw snprintf( char* str, sw n, char const* fmt, ... );
ZPL_DEF sw snprintf_va( char* str, sw n, char const* fmt, va_list va );

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: header/core/random.h


ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

typedef struct random
{
	u32 offsets[ 8 ];
	u32 value;
} random;

// NOTE: Generates from numerous sources to produce a decent pseudo-random seed
ZPL_DEF void random_init( random* r );
ZPL_DEF u32  random_gen_u32( random* r );
ZPL_DEF u32  random_gen_u32_unique( random* r );
ZPL_DEF u64  random_gen_u64( random* r );    // NOTE: (zpl_random_gen_u32() << 32) | zpl_random_gen_u32()
ZPL_DEF sw   random_gen_isize( random* r );
ZPL_DEF s64  random_range_i64( random* r, s64 lower_inc, s64 higher_inc );
ZPL_DEF sw   random_range_isize( random* r, sw lower_inc, sw higher_inc );
ZPL_DEF f64  random_range_f64( random* r, f64 lower_inc, f64 higher_inc );

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: header/core/sort.h

/** @file sort.c
@brief Sorting and searching methods.
@defgroup sort Sorting and searching

Methods for sorting arrays using either Quick/Merge-sort combo or Radix sort. It also contains simple implementation of binary search, as well as an easy to use API to define your
own comparators.

@{
*/

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

#define ZPL_COMPARE_PROC( name ) int name( void const* a, void const* b )
typedef ZPL_COMPARE_PROC( compare_proc );

#define ZPL_COMPARE_PROC_PTR( def ) ZPL_COMPARE_PROC( ( *def ) )

// Procedure pointers
// NOTE: The offset parameter specifies the offset in the structure
// e.g. zpl_i32_cmp(zpl_offset_of(Thing, value))
// Use 0 if it's just the type instead.

ZPL_DEF ZPL_COMPARE_PROC_PTR( i16_cmp( sw offset ) );
ZPL_DEF ZPL_COMPARE_PROC_PTR( u8_cmp( sw offset ) );
ZPL_DEF ZPL_COMPARE_PROC_PTR( i32_cmp( sw offset ) );
ZPL_DEF ZPL_COMPARE_PROC_PTR( i64_cmp( sw offset ) );
ZPL_DEF ZPL_COMPARE_PROC_PTR( isize_cmp( sw offset ) );
ZPL_DEF ZPL_COMPARE_PROC_PTR( str_cmp( sw offset ) );
ZPL_DEF ZPL_COMPARE_PROC_PTR( f32_cmp( sw offset ) );
ZPL_DEF ZPL_COMPARE_PROC_PTR( f64_cmp( sw offset ) );

// TODO: Better sorting algorithms

//! Sorts an array.

//! Uses quick sort for large arrays but insertion sort for small ones.
#define sort_array( array, count, compare_proc ) ZPL_NS( sort )( array, count, size_of( *( array ) ), compare_proc )

//! Perform sorting operation on a memory location with a specified item count and size.
ZPL_DEF void sort( void* base, sw count, sw size, compare_proc compare_proc );

// NOTE: the count of temp == count of items
#define radix_sort( Type )          radix_sort_##Type
#define ZPL_RADIX_SORT_PROC( Type ) void radix_sort( Type )( Type * items, Type * temp, sw count )

ZPL_DEF ZPL_RADIX_SORT_PROC( u8 );
ZPL_DEF ZPL_RADIX_SORT_PROC( u16 );
ZPL_DEF ZPL_RADIX_SORT_PROC( u32 );
ZPL_DEF ZPL_RADIX_SORT_PROC( u64 );

//! Performs binary search on an array.

//! Returns index or -1 if not found
#define binary_search_array( array, count, key, compare_proc ) ZPL_NS( binary_search )( array, count, size_of( *( array ) ), key, compare_proc )

//! Performs binary search on a memory location with specified item count and size.
ZPL_DEF_INLINE sw binary_search( void const* base, sw count, sw size, void const* key, compare_proc compare_proc );

#define shuffle_array( array, count ) ZPL_NS( shuffle )( array, count, size_of( *( array ) ) )

//! Shuffles a memory.
ZPL_DEF void shuffle( void* base, sw count, sw size );

#define reverse_array( array, count ) ZPL_NS( reverse )( array, count, size_of( *( array ) ) )

//! Reverses memory's contents
ZPL_DEF void reverse( void* base, sw count, sw size );

//! @}


ZPL_IMPL_INLINE sw binary_search( void const* base, sw count, sw size, void const* key, compare_proc compare_proc )
{
	sw start = 0;
	sw end   = count;

	while ( start < end )
	{
		sw mid    = start + ( end - start ) / 2;
		sw result = compare_proc( key, zpl_cast( u8* ) base + mid * size );
		if ( result < 0 )
			end = mid;
		else if ( result > 0 )
			start = mid + 1;
		else
			return mid;
	}

	return -1;
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: header/core/string.h

/** @file string.c
@brief String operations and library
@defgroup string String library

Offers methods for c-string manipulation, but also a string library based on gb_string, which is c-string friendly.

@{
*/

////////////////////////////////////////////////////////////////
//
// Char Functions
//
//


ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

ZPL_DEF_INLINE char char_to_lower( char c );
ZPL_DEF_INLINE char char_to_upper( char c );
ZPL_DEF_INLINE b32  char_is_space( char c );
ZPL_DEF_INLINE b32  char_is_digit( char c );
ZPL_DEF_INLINE b32  char_is_hex_digit( char c );
ZPL_DEF_INLINE b32  char_is_alpha( char c );
ZPL_DEF_INLINE b32  char_is_alphanumeric( char c );
ZPL_DEF_INLINE s32  digit_to_int( char c );
ZPL_DEF_INLINE s32  hex_digit_to_int( char c );
ZPL_DEF_INLINE u8   char_to_hex_digit( char c );
ZPL_DEF_INLINE b32  char_is_control( char c );

// NOTE: ASCII only
ZPL_DEF_INLINE void str_to_lower( char* str );
ZPL_DEF_INLINE void str_to_upper( char* str );

ZPL_DEF_INLINE char const* str_trim( char const* str, b32 catch_newline );
ZPL_DEF_INLINE char const* str_skip( char const* str, char c );
ZPL_DEF_INLINE char const* str_skip_any( char const* str, char const* char_list );
ZPL_DEF_INLINE char const* str_skip_literal( char const* str, char c );
ZPL_DEF_INLINE char const* str_control_skip( char const* str, char c );

ZPL_DEF_INLINE sw          strlen( const char* str );
ZPL_DEF_INLINE sw          strnlen( const char* str, sw max_len );
ZPL_DEF_INLINE s32         str_compare( const char* s1, const char* s2 );
ZPL_DEF_INLINE s32         str_compare( const char* s1, const char* s2, sw len );
ZPL_DEF_INLINE char*       strcpy( char* dest, const char* source );
ZPL_DEF_INLINE char*       strcat( char* dest, const char* source );
ZPL_DEF_INLINE char*       strncpy( char* dest, const char* source, sw len );
ZPL_DEF_INLINE sw          strlcpy( char* dest, const char* source, sw len );
ZPL_DEF_INLINE char*       strrev( char* str );    // NOTE: ASCII only
ZPL_DEF_INLINE const char* strtok( char* output, const char* src, const char* delimit );
ZPL_DEF_INLINE const char* strntok( char* output, sw len, const char* src, const char* delimit );

ZPL_DEF_INLINE char*  strdup( AllocatorInfo a, char* src, sw max_len );
ZPL_DEF_INLINE char** str_split_lines( AllocatorInfo alloc, char* source, b32 strip_whitespace );

#define str_expand( str ) str, ZPL_NS( strlen )( str )
#define str_advance_while( str, cond ) \
	do                                 \
	{                                  \
		++str;                         \
	} while ( ( cond ) );

ZPL_DEF_INLINE b32 str_has_prefix( const char* str, const char* prefix );
ZPL_DEF_INLINE b32 str_has_suffix( const char* str, const char* suffix );

ZPL_DEF_INLINE const char* char_first_occurence( const char* str, char c );
ZPL_DEF_INLINE const char* char_last_occurence( const char* str, char c );
#define strchr char_first_occurence

ZPL_DEF_INLINE void str_concat( char* dest, sw dest_len, const char* src_a, sw src_a_len, const char* src_b, sw src_b_len );

ZPL_DEF u64  str_to_u64( const char* str, char** end_ptr, s32 base );    // TODO: Support more than just decimal and hexadecimal
ZPL_DEF s64  str_to_i64( const char* str, char** end_ptr, s32 base );    // TODO: Support more than just decimal and hexadecimal
ZPL_DEF f64  str_to_f64( const char* str, char** end_ptr );
ZPL_DEF void i64_to_str( s64 value, char* string, s32 base );
ZPL_DEF void u64_to_str( u64 value, char* string, s32 base );

ZPL_DEF_INLINE f32 str_to_f32( const char* str, char** end_ptr );

////////////////////////////////////////////////////////////////
//
// UTF-8 Handling
//
//

// NOTE: Does not check if utf-8 string is valid
ZPL_IMPL_INLINE sw utf8_strlen( u8 const* str );
ZPL_IMPL_INLINE sw utf8_strnlen( u8 const* str, sw max_len );

// NOTE: Windows doesn't handle 8 bit filenames well
ZPL_DEF u16* utf8_to_ucs2( u16* buffer, sw len, u8 const* str );
ZPL_DEF u8*  ucs2_to_utf8( u8* buffer, sw len, u16 const* str );
ZPL_DEF u16* utf8_to_ucs2_buf( u8 const* str );     // NOTE: Uses locally persisting buffer
ZPL_DEF u8*  ucs2_to_utf8_buf( u16 const* str );    // NOTE: Uses locally persisting buffer

// NOTE: Returns size of codepoint in bytes
ZPL_DEF sw utf8_decode( u8 const* str, sw str_len, rune* codepoint );
ZPL_DEF sw utf8_codepoint_size( u8 const* str, sw str_len );
ZPL_DEF sw utf8_encode_rune( u8 buf[ 4 ], rune r );

/* inlines */

ZPL_IMPL_INLINE char char_to_lower( char c )
{
	if ( c >= 'A' && c <= 'Z' )
		return 'a' + ( c - 'A' );
	return c;
}

ZPL_IMPL_INLINE char char_to_upper( char c )
{
	if ( c >= 'a' && c <= 'z' )
		return 'A' + ( c - 'a' );
	return c;
}

ZPL_IMPL_INLINE b32 char_is_space( char c )
{
	if ( c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v' )
		return true;
	return false;
}

ZPL_IMPL_INLINE b32 char_is_digit( char c )
{
	if ( c >= '0' && c <= '9' )
		return true;
	return false;
}

ZPL_IMPL_INLINE b32 char_is_hex_digit( char c )
{
	if ( char_is_digit( c ) || ( c >= 'a' && c <= 'f' ) || ( c >= 'A' && c <= 'F' ) )
		return true;
	return false;
}

ZPL_IMPL_INLINE b32 char_is_alpha( char c )
{
	if ( ( c >= 'A' && c <= 'Z' ) || ( c >= 'a' && c <= 'z' ) )
		return true;
	return false;
}

ZPL_IMPL_INLINE b32 char_is_alphanumeric( char c )
{
	return char_is_alpha( c ) || char_is_digit( c );
}

ZPL_IMPL_INLINE s32 digit_to_int( char c )
{
	return char_is_digit( c ) ? c - '0' : c - 'W';
}

ZPL_IMPL_INLINE s32 hex_digit_to_int( char c )
{
	if ( char_is_digit( c ) )
		return digit_to_int( c );
	else if ( is_between( c, 'a', 'f' ) )
		return c - 'a' + 10;
	else if ( is_between( c, 'A', 'F' ) )
		return c - 'A' + 10;
	return -1;
}

ZPL_IMPL_INLINE u8 char_to_hex_digit( char c )
{
	if ( c >= '0' && c <= '9' )
		return ( u8 )( c - '0' );
	if ( c >= 'a' && c <= 'f' )
		return ( u8 )( c - 'a' );
	if ( c >= 'A' && c <= 'F' )
		return ( u8 )( c - 'A' );
	return 0;
}

ZPL_IMPL_INLINE void str_to_lower( char* str )
{
	if ( ! str )
		return;
	while ( *str )
	{
		*str = char_to_lower( *str );
		str++;
	}
}

ZPL_IMPL_INLINE void str_to_upper( char* str )
{
	if ( ! str )
		return;
	while ( *str )
	{
		*str = char_to_upper( *str );
		str++;
	}
}

ZPL_IMPL_INLINE sw strlen( const char* str )
{
	if ( str == NULL )
	{
		return 0;
	}
	const char* p = str;
	while ( *str )
		str++;
	return str - p;
}

ZPL_IMPL_INLINE sw strnlen( const char* str, sw max_len )
{
	const char* end = zpl_cast( const char* ) memchr( str, 0, max_len );
	if ( end )
		return end - str;
	return max_len;
}

ZPL_IMPL_INLINE sw utf8_strlen( u8 const* str )
{
	sw count = 0;
	for ( ; *str; count++ )
	{
		u8 c   = *str;
		sw inc = 0;
		if ( c < 0x80 )
			inc = 1;
		else if ( ( c & 0xe0 ) == 0xc0 )
			inc = 2;
		else if ( ( c & 0xf0 ) == 0xe0 )
			inc = 3;
		else if ( ( c & 0xf8 ) == 0xf0 )
			inc = 4;
		else
			return -1;

		str += inc;
	}
	return count;
}

ZPL_IMPL_INLINE sw utf8_strnlen( u8 const* str, sw max_len )
{
	sw count = 0;
	for ( ; *str && max_len > 0; count++ )
	{
		u8 c   = *str;
		sw inc = 0;
		if ( c < 0x80 )
			inc = 1;
		else if ( ( c & 0xe0 ) == 0xc0 )
			inc = 2;
		else if ( ( c & 0xf0 ) == 0xe0 )
			inc = 3;
		else if ( ( c & 0xf8 ) == 0xf0 )
			inc = 4;
		else
			return -1;

		str     += inc;
		max_len -= inc;
	}
	return count;
}

ZPL_IMPL_INLINE s32 str_compare( const char* s1, const char* s2 )
{
	while ( *s1 && ( *s1 == *s2 ) )
	{
		s1++, s2++;
	}
	return *( u8* )s1 - *( u8* )s2;
}

ZPL_IMPL_INLINE char* strcpy( char* dest, const char* source )
{
	ZPL_ASSERT_NOT_NULL( dest );
	if ( source )
	{
		char* str = dest;
		while ( *source )
			*str++ = *source++;
	}
	return dest;
}

ZPL_IMPL_INLINE char* strcat( char* dest, const char* source )
{
	ZPL_ASSERT_NOT_NULL( dest );
	if ( source )
	{
		char* str = dest;
		while ( *str )
			++str;
		while ( *source )
			*str++ = *source++;
	}
	return dest;
}

ZPL_IMPL_INLINE char* strncpy( char* dest, const char* source, sw len )
{
	ZPL_ASSERT_NOT_NULL( dest );
	if ( source )
	{
		char* str = dest;
		while ( len > 0 && *source )
		{
			*str++ = *source++;
			len--;
		}
		while ( len > 0 )
		{
			*str++ = '\0';
			len--;
		}
	}
	return dest;
}

ZPL_IMPL_INLINE sw strlcpy( char* dest, const char* source, sw len )
{
	sw result = 0;
	ZPL_ASSERT_NOT_NULL( dest );
	if ( source )
	{
		const char* source_start = source;
		char*       str          = dest;
		while ( len > 0 && *source )
		{
			*str++ = *source++;
			len--;
		}
		while ( len > 0 )
		{
			*str++ = '\0';
			len--;
		}

		result = source - source_start;
	}
	return result;
}

ZPL_IMPL_INLINE char* strrev( char* str )
{
	sw    len  = strlen( str );
	char* a    = str + 0;
	char* b    = str + len - 1;
	len       /= 2;
	while ( len-- )
	{
		swap( char, *a, *b );
		a++, b--;
	}
	return str;
}

ZPL_IMPL_INLINE s32 str_compare( const char* s1, const char* s2, sw len )
{
	for ( ; len > 0; s1++, s2++, len-- )
	{
		if ( *s1 != *s2 )
			return ( ( s1 < s2 ) ? -1 : +1 );
		else if ( *s1 == '\0' )
			return 0;
	}
	return 0;
}

ZPL_IMPL_INLINE const char* strtok( char* output, const char* src, const char* delimit )
{
	while ( *src && char_first_occurence( delimit, *src ) == NULL )
		*output++ = *src++;

	*output = 0;
	return *src ? src + 1 : src;
}

ZPL_IMPL_INLINE const char* strntok( char* output, sw len, const char* src, const char* delimit )
{
	ZPL_ASSERT( len > 0 );
	*( output + len - 1 ) = 0;
	while ( *src && char_first_occurence( delimit, *src ) == NULL && len > 0 )
	{
		*output++ = *src++;
		len--;
	}

	if ( len > 0 )
		*output = 0;
	return *src ? src + 1 : src;
}

ZPL_IMPL_INLINE b32 char_is_control( char c )
{
	return ! ! strchr( "\"\\/bfnrt", c );
}

ZPL_IMPL_INLINE b32 _is_special_char( char c )
{
	return ! ! strchr( "<>:/", c );
}

ZPL_IMPL_INLINE b32 _is_assign_char( char c )
{
	return ! ! strchr( ":=|", c );
}

ZPL_IMPL_INLINE b32 _is_delim_char( char c )
{
	return ! ! strchr( ",|\n", c );
}

ZPL_IMPL_INLINE char const* str_control_skip( char const* str, char c )
{
	while ( ( *str && *str != c ) || ( *( str - 1 ) == '\\' && *str == c && char_is_control( c ) ) )
	{
		++str;
	}

	return str;
}

ZPL_IMPL_INLINE b32 str_has_prefix( const char* str, const char* prefix )
{
	while ( *prefix )
	{
		if ( *str++ != *prefix++ )
			return false;
	}
	return true;
}

ZPL_IMPL_INLINE b32 str_has_suffix( const char* str, const char* suffix )
{
	sw i = strlen( str );
	sw j = strlen( suffix );
	if ( j <= i )
		return str_compare( str + i - j, suffix ) == 0;
	return false;
}

ZPL_IMPL_INLINE const char* char_first_occurence( const char* s, char c )
{
	char ch = c;
	for ( ; *s != ch; s++ )
	{
		if ( *s == '\0' )
			return NULL;
	}
	return s;
}

ZPL_IMPL_INLINE const char* char_last_occurence( const char* s, char c )
{
	char* result = ( char* )NULL;
	do
	{
		if ( *s == c )
			result = ( char* )s;
	} while ( *s++ );

	return result;
}

ZPL_IMPL_INLINE char const* str_trim( char const* str, b32 catch_newline )
{
	while ( *str && char_is_space( *str ) && ( ! catch_newline || ( catch_newline && *str != '\n' ) ) )
	{
		++str;
	}
	return str;
}

ZPL_IMPL_INLINE char const* str_skip( char const* str, char c )
{
	while ( *str && *str != c )
	{
		++str;
	}
	return str;
}

ZPL_IMPL_INLINE char const* str_skip_any( char const* str, char const* char_list )
{
	char const* closest_ptr     = zpl_cast( char const* ) ptr_add( ( void* )str, strlen( str ) );
	sw          char_list_count = strlen( char_list );
	for ( sw i = 0; i < char_list_count; i++ )
	{
		char const* p = str_skip( str, char_list[ i ] );
		closest_ptr   = min( closest_ptr, p );
	}
	return closest_ptr;
}

ZPL_IMPL_INLINE char const* str_skip_literal( char const* str, char c )
{
	while ( ( *str && *str != c ) || ( *str == c && *( str - 1 ) == '\\' ) )
	{
		++str;
	}
	return str;
}

ZPL_IMPL_INLINE void str_concat( char* dest, sw dest_len, const char* src_a, sw src_a_len, const char* src_b, sw src_b_len )
{
	ZPL_ASSERT( dest_len >= src_a_len + src_b_len + 1 );
	if ( dest )
	{
		memcopy( dest, src_a, src_a_len );
		memcopy( dest + src_a_len, src_b, src_b_len );
		dest[ src_a_len + src_b_len ] = '\0';
	}
}

ZPL_IMPL_INLINE f32 str_to_f32( const char* str, char** end_ptr )
{
	f64 f = str_to_f64( str, end_ptr );
	f32 r = zpl_cast( f32 ) f;
	return r;
}

ZPL_IMPL_INLINE char* strdup( AllocatorInfo a, char* src, sw max_len )
{
	ZPL_ASSERT_NOT_NULL( src );
	sw    len  = strlen( src );
	char* dest = zpl_cast( char* ) alloc( a, max_len );
	memset( dest + len, 0, max_len - len );
	strncpy( dest, src, max_len );

	return dest;
}

ZPL_IMPL_INLINE char** str_split_lines( AllocatorInfo alloc, char* source, b32 strip_whitespace )
{
	char **lines = NULL, *p = source, *pd = p;
	array_init( lines, alloc );

	while ( *p )
	{
		if ( *pd == '\n' )
		{
			*pd = 0;
			if ( *( pd - 1 ) == '\r' )
				*( pd - 1 ) = 0;
			if ( strip_whitespace && ( pd - p ) == 0 )
			{
				p = pd + 1;
				continue;
			}
			array_append( lines, p );
			p = pd + 1;
		}
		++pd;
	}
	return lines;
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: header/core/stringlib.h


ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

typedef char* String;

typedef struct StringHeader
{
	AllocatorInfo allocator;
	sw            length;
	sw            capacity;
} StringHeader;

#define ZPL_STRING_HEADER( str ) ( zpl_cast( ZPL_NS( StringHeader )* )( str ) - 1 )

ZPL_DEF String string_make_reserve( AllocatorInfo a, sw capacity );
ZPL_DEF String string_make_length( AllocatorInfo a, void const* str, sw num_bytes );
ZPL_DEF String string_sprintf( AllocatorInfo a, char* buf, sw num_bytes, const char* fmt, ... );
ZPL_DEF String string_sprintf_buf( AllocatorInfo a, const char* fmt, ... );    // NOTE: Uses locally persistent buffer
ZPL_DEF String string_append_length( String str, void const* other, sw num_bytes );
ZPL_DEF String string_appendc( String str, const char* other );
ZPL_DEF String string_join( AllocatorInfo a, const char** parts, sw count, const char* glue );
ZPL_DEF String string_set( String str, const char* cstr );
ZPL_DEF String string_make_space_for( String str, sw add_len );
ZPL_DEF sw     string_allocation_size( String const str );
ZPL_DEF b32    string_are_equal( String const lhs, String const rhs );
ZPL_DEF String string_trim( String str, const char* cut_set );
ZPL_DEF String string_append_rune( String str, rune r );
ZPL_DEF String string_append_fmt( String str, const char* fmt, ... );

ZPL_DEF_INLINE String string_make( AllocatorInfo a, const char* str );
ZPL_DEF_INLINE void   string_free( String str );
ZPL_DEF_INLINE void   string_clear( String str );
ZPL_DEF_INLINE String string_duplicate( AllocatorInfo a, String const str );
ZPL_DEF_INLINE sw     string_length( String const str );
ZPL_DEF_INLINE sw     string_capacity( String const str );
ZPL_DEF_INLINE sw     string_available_space( String const str );
ZPL_DEF_INLINE String string_append( String str, String const other );
ZPL_DEF_INLINE String string_trim_space( String str );    // Whitespace ` \t\r\n\v\f`
ZPL_DEF_INLINE void   _set_string_length( String str, sw len );
ZPL_DEF_INLINE void   _set_string_capacity( String str, sw cap );

ZPL_IMPL_INLINE void _set_string_length( String str, sw len )
{
	ZPL_STRING_HEADER( str )->length = len;
}

ZPL_IMPL_INLINE void _set_string_capacity( String str, sw cap )
{
	ZPL_STRING_HEADER( str )->capacity = cap;
}

ZPL_IMPL_INLINE String string_make( AllocatorInfo a, const char* str )
{
	sw len = str ? strlen( str ) : 0;
	return string_make_length( a, str, len );
}

ZPL_IMPL_INLINE void string_free( String str )
{
	if ( str )
	{
		StringHeader* header = ZPL_STRING_HEADER( str );
		free( header->allocator, header );
	}
}

ZPL_IMPL_INLINE String string_duplicate( AllocatorInfo a, String const str )
{
	return string_make_length( a, str, string_length( str ) );
}

ZPL_IMPL_INLINE sw string_length( String const str )
{
	return ZPL_STRING_HEADER( str )->length;
}

ZPL_IMPL_INLINE sw string_capacity( String const str )
{
	return ZPL_STRING_HEADER( str )->capacity;
}

ZPL_IMPL_INLINE sw string_available_space( String const str )
{
	StringHeader* h = ZPL_STRING_HEADER( str );
	if ( h->capacity > h->length )
		return h->capacity - h->length;
	return 0;
}

ZPL_IMPL_INLINE void string_clear( String str )
{
	_set_string_length( str, 0 );
	str[ 0 ] = '\0';
}

ZPL_IMPL_INLINE String string_append( String str, String const other )
{
	return string_append_length( str, other, string_length( other ) );
}

ZPL_IMPL_INLINE String string_trim_space( String str )
{
	return string_trim( str, " \t\r\n\v\f" );
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: header/core/time.h

/** @file time.c
@brief Time helper methods.
@defgroup time Time helpers

 Helper methods for retrieving the current time in many forms under different precisions. It also offers a simple to use timer library.

 @{
 */


ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

//! Return CPU timestamp.
ZPL_DEF u64 rdtsc( void );

//! Return relative time (in seconds) since the application start.
ZPL_DEF f64 time_rel( void );

//! Return relative time since the application start.
ZPL_DEF u64 time_rel_ms( void );

//! Return time (in seconds) since 1601-01-01 UTC.
ZPL_DEF f64 time_utc( void );

//! Return time since 1601-01-01 UTC.
ZPL_DEF u64 time_utc_ms( void );

//! Return local system time since 1601-01-01
ZPL_DEF u64 time_tz_ms( void );

//! Return local system time in seconds since 1601-01-01
ZPL_DEF f64 time_tz( void );

//! Convert Win32 epoch (1601-01-01 UTC) to UNIX (1970-01-01 UTC)
ZPL_DEF_INLINE u64 time_win32_to_unix( u64 ms );

//! Convert UNIX (1970-01-01 UTC) to Win32 epoch (1601-01-01 UTC)
ZPL_DEF_INLINE u64 time_unix_to_win32( u64 ms );

//! Sleep for specified number of milliseconds.
ZPL_DEF void sleep_ms( u32 ms );

//! Sleep for specified number of seconds.
ZPL_DEF_INLINE void sleep( f32 s );

// Deprecated methods
ZPL_DEPRECATED_FOR( 10.9.0, time_rel )
ZPL_DEF_INLINE f64 time_now( void );

ZPL_DEPRECATED_FOR( 10.9.0, time_utc )
ZPL_DEF_INLINE f64 utc_time_now( void );


#ifndef ZPL__UNIX_TO_WIN32_EPOCH
#	define ZPL__UNIX_TO_WIN32_EPOCH 11644473600000ull
#endif

ZPL_IMPL_INLINE u64 time_win32_to_unix( u64 ms )
{
	return ms - ZPL__UNIX_TO_WIN32_EPOCH;
}

ZPL_IMPL_INLINE u64 time_unix_to_win32( u64 ms )
{
	return ms + ZPL__UNIX_TO_WIN32_EPOCH;
}

ZPL_IMPL_INLINE void sleep( f32 s )
{
	sleep_ms( ( u32 )( s * 1000 ) );
}

ZPL_IMPL_INLINE f64 time_now()
{
	return time_rel();
}

ZPL_IMPL_INLINE f64 utc_time_now()
{
	return time_utc();
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

#		endif
#	endif

#	if defined( ZPL_MODULE_HASHING )
// file: header/hashing.h

/** @file hashing.c
@brief Hashing and Checksum Functions
@defgroup hashing Hashing and Checksum Functions

Several hashing methods used by zpl internally but possibly useful outside of it. Contains: adler32, crc32/64, fnv32/64/a and murmur32/64

@{
*/

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

ZPL_DEF u32 adler32( void const* data, sw len );

ZPL_DEF u32 crc32( void const* data, sw len );
ZPL_DEF u64 crc64( void const* data, sw len );

// These use FNV-1 algorithm
ZPL_DEF u32 fnv32( void const* data, sw len );
ZPL_DEF u64 fnv64( void const* data, sw len );
ZPL_DEF u32 fnv32a( void const* data, sw len );
ZPL_DEF u64 fnv64a( void const* data, sw len );

ZPL_DEF u8* base64_encode( AllocatorInfo a, void const* data, sw len );
ZPL_DEF u8* base64_decode( AllocatorInfo a, void const* data, sw len );

//! Based on MurmurHash3
ZPL_DEF u32 murmur32_seed( void const* data, sw len, u32 seed );

//! Based on MurmurHash2
ZPL_DEF u64 murmur64_seed( void const* data, sw len, u64 seed );

//! Default seed of 0x9747b28c
ZPL_DEF_INLINE u32 murmur32( void const* data, sw len );

//! Default seed of 0x9747b28c
ZPL_DEF_INLINE u64 murmur64( void const* data, sw len );

//! @}

ZPL_IMPL_INLINE u32 murmur32( void const* data, sw len )
{
	return murmur32_seed( data, len, 0x9747b28c );
}

ZPL_IMPL_INLINE u64 murmur64( void const* data, sw len )
{
	return murmur64_seed( data, len, 0x9747b28c );
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

#	endif

#	if defined( ZPL_MODULE_REGEX )
// file: header/regex.h

/** @file regex.c
@brief Regular expressions parser.
@defgroup regex Regex processor

Port of gb_regex with several bugfixes applied. This is a simple regex library and is fast to perform.

Supported Matching:
    @n ^       - Beginning of string
    @n $       - End of string
    @n .       - Match one (anything)
    @n |       - Branch (or)
    @n ()      - Capturing group
    @n []      - Any character included in set
    @n [^]     - Any character excluded from set
    @n +       - One or more  (greedy)
    @n +?      - One or more  (non-greedy)
    @n *       - Zero or more (greedy)
    @n *?      - Zero or more (non-greedy)
    @n ?       - Zero or once
    @n [BACKSLASH]XX     - Hex decimal digit (must be 2 digits)
    @n [BACKSLASH]meta   - Meta character
    @n [BACKSLASH]s      - Whitespace
    @n [BACKSLASH]S      - Not whitespace
    @n [BACKSLASH]d      - Digit
    @n [BACKSLASH]D      - Not digit
    @n [BACKSLASH]a      - Alphabetic character
    @n [BACKSLASH]l      - Lower case letter
    @n [BACKSLASH]u      - Upper case letter
    @n [BACKSLASH]w      - Word
    @n [BACKSLASH]W      - Not word
    @n [BACKSLASH]x      - Hex Digit
    @n [BACKSLASH]p      - Printable ASCII character
    @n --Whitespace--
    @n [BACKSLASH]t      - Tab
    @n [BACKSLASH]n      - New line
    @n [BACKSLASH]r      - Return carriage
    @n [BACKSLASH]v      - Vertical Tab
    @n [BACKSLASH]f      - Form feed

    @{
*/

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

typedef struct re
{
	AllocatorInfo backing;
	sw            capture_count;
	char*         buf;
	sw            buf_len, buf_cap;
	b32           can_realloc;
} re;

typedef struct re_capture
{
	char const* str;
	sw          len;
} re_capture;

#define zplRegexError ZPL_NS( regex_error )

typedef enum regex_error
{
	ZPL_RE_ERROR_NONE,
	ZPL_RE_ERROR_NO_MATCH,
	ZPL_RE_ERROR_TOO_LONG,
	ZPL_RE_ERROR_MISMATCHED_CAPTURES,
	ZPL_RE_ERROR_MISMATCHED_BLOCKS,
	ZPL_RE_ERROR_BRANCH_FAILURE,
	ZPL_RE_ERROR_INVALID_QUANTIFIER,
	ZPL_RE_ERROR_INTERNAL_FAILURE,
} regex_error;

//! Compile regex pattern.
ZPL_DEF regex_error re_compile( re* re, AllocatorInfo backing, char const* pattern, sw pattern_len );

//! Compile regex pattern using a buffer.
ZPL_DEF regex_error re_compile_from_buffer( re* re, char const* pattern, sw pattern_len, void* buffer, sw buffer_len );

//! Destroy regex object.
ZPL_DEF void re_destroy( re* re );

//! Retrieve number of retrievable captures.
ZPL_DEF sw re_capture_count( re* re );

//! Match input string and output captures of the occurence.
ZPL_DEF b32 re_match( re* re, char const* str, sw str_len, re_capture* captures, sw max_capture_count, sw* offset );

//! Match all occurences in an input string and output them into captures. Array of captures is allocated on the heap and needs to be freed afterwards.
ZPL_DEF b32 re_match_all( re* re, char const* str, sw str_len, sw max_capture_count, re_capture** out_captures );

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

#	endif

#	if defined( ZPL_MODULE_DLL )
// file: header/dll.h

/** @file dll.c
@brief DLL Handling
@defgroup dll DLL handling

@{
*/

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

typedef void* dll_handle;
typedef void ( *dll_proc )( void );

ZPL_DEF dll_handle dll_load( char const* filepath );
ZPL_DEF void       dll_unload( dll_handle dll );
ZPL_DEF dll_proc   dll_proc_address( dll_handle dll, char const* proc_name );

//! @}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

#	endif

#	if defined( ZPL_MODULE_OPTS )
// file: header/opts.h

/** @file opts.c
@brief CLI options processor
@defgroup cli CLI options processor

 Opts is a CLI options parser, it can parse flags, switches and arguments from command line
 and offers an easy way to express input errors as well as the ability to display help screen.

@{
 */

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

typedef enum
{
	ZPL_OPTS_STRING,
	ZPL_OPTS_FLOAT,
	ZPL_OPTS_FLAG,
	ZPL_OPTS_INT,
} opts_types;

typedef struct
{
	char const *name, *lname, *desc;
	u8          type;
	b32         met, pos;

	//! values
	union
	{
		String text;
		s64    integer;
		f64    real;
	};
} opts_entry;

typedef enum
{
	ZPL_OPTS_ERR_VALUE,
	ZPL_OPTS_ERR_OPTION,
	ZPL_OPTS_ERR_EXTRA_VALUE,
	ZPL_OPTS_ERR_MISSING_VALUE,
} opts_err_type;

typedef struct
{
	char* val;
	u8    type;
} opts_err;

typedef struct
{
	AllocatorInfo alloc;
	opts_entry*   entries;       ///< zpl_array
	opts_err*     errors;        ///< zpl_array
	opts_entry**  positioned;    ///< zpl_array
	char const*   appname;
} Opts;

//! Initializes options parser.

//! Initializes CLI options parser using specified memory allocator and provided application name.
//! @param opts Options parser to initialize.
//! @param allocator Memory allocator to use. (ex. zpl_heap())
//! @param app Application name displayed in help screen.
ZPL_DEF void opts_init( Opts* opts, AllocatorInfo allocator, char const* app );

//! Releases the resources used by options parser.
ZPL_DEF void opts_free( Opts* opts );

//! Registers an option.

//! Registers an option with its short and long name, specifies option's type and its description.
//! @param opts Options parser to add to.
//! @param lname Shorter name of option. (ex. "f")
//! @param name Full name of option. (ex. "foo") Note that rest of the module uses longer names to manipulate opts.
//! @param desc Description shown in the help screen.
//! @param type Option's type (see zpl_opts_types)
//! @see zpl_opts_types
ZPL_DEF void opts_add( Opts* opts, char const* name, char const* lname, const char* desc, u8 type );

//! Registers option as positional.

//! Registers added option as positional, so that we can pass it anonymously. Arguments are expected on the command input in the same order they were registered as.
//! @param opts
//! @param name Name of already registered option.
ZPL_DEF void opts_positional_add( Opts* opts, char const* name );

//! Compiles CLI arguments.

// This method takes CLI arguments as input and processes them based on rules that were set up.
//! @param opts
//! @param argc Argument count in an array.
//! @param argv Array of arguments.
ZPL_DEF b32 opts_compile( Opts* opts, int argc, char** argv );

//! Prints out help screen.

//! Prints out help screen with example usage of application as well as with all the flags available.
ZPL_DEF void opts_print_help( Opts* opts );

//! Prints out parsing errors.

//! Prints out possible errors caused by CLI input.
ZPL_DEF void opts_print_errors( Opts* opts );

//! Fetches a string from an option.

//! @param opts
//! @param name Name of an option.
//! @param fallback Fallback string we return if option wasn't found.
ZPL_DEF String opts_string( Opts* opts, char const* name, char const* fallback );

//! Fetches a real number from an option.

//! @param opts
//! @param name Name of an option.
//! @param fallback Fallback real number we return if option was not found.
ZPL_DEF f64 opts_real( Opts* opts, char const* name, f64 fallback );

//! Fetches an integer number from an option.

//! @param opts
//! @param name Name of an option.
//! @param fallback Fallback integer number we return if option was not found.
ZPL_DEF s64 opts_integer( Opts* opts, char const* name, s64 fallback );

//! Checks whether an option was used.

//! @param opts
//! @param name Name of an option.
ZPL_DEF b32 opts_has_arg( Opts* opts, char const* name );

//! Checks whether all positionals have been passed in.
ZPL_DEF b32 opts_positionals_filled( Opts* opts );

//! @}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

#	endif

#	if defined( ZPL_MODULE_PROCESS )
// file: header/process.h

/** @file process.c
@brief Process creation and manipulation methods
@defgroup process Process creation and manipulation methods

Gives you the ability to create a new process, wait for it to end or terminate it.
It also exposes standard I/O with configurable options.

@{
*/

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

// TODO(zaklaus): Add Linux support

typedef enum
{
	ZPL_PR_OPTS_COMBINE_STD_OUTPUT = ZPL_BIT( 1 ),
	ZPL_PR_OPTS_INHERIT_ENV        = ZPL_BIT( 2 ),
	ZPL_PR_OPTS_CUSTOM_ENV         = ZPL_BIT( 3 ),
} pr_opts;

typedef struct
{
	FileInfo in, out, err;
	void *   f_stdin, *f_stdout, *f_stderr;
#ifdef ZPL_SYSTEM_WINDOWS
	void* win32_handle;
#else
	// todo
#endif
} pr;

typedef struct
{
	char* con_title;
	char* workdir;

	sw     env_count;
	char** env;    // format: "var=name"

	u32 posx, posy;
	u32 resx, resy;
	u32 bufx, bufy;
	u32 fill_attr;
	u32 flags;
	b32 show_window;
} pr_si;

ZPL_DEF s32  pr_create( pr* process, const char** args, sw argc, pr_si si, pr_opts options );
ZPL_DEF void pr_destroy( pr* process );
ZPL_DEF void pr_terminate( pr* process, s32 err_code );
ZPL_DEF s32  pr_join( pr* process );

//! @}
ZPL_END_C_DECLS
ZPL_END_NAMESPACE

#	endif

#	if defined( ZPL_MODULE_MATH )
// file: header/math.h

/** @file math.c
@brief Math operations
@defgroup math Math operations

OpenGL gamedev friendly library for math.

@{
*/

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

typedef union vec2
{
	struct
	{
		f32 x, y;
	};

	struct
	{
		f32 s, t;
	};

	f32 e[ 2 ];
} vec2;

typedef union vec3
{
	struct
	{
		f32 x, y, z;
	};

	struct
	{
		f32 r, g, b;
	};

	struct
	{
		f32 s, t, p;
	};

	vec2 xy;
	vec2 st;
	f32  e[ 3 ];
} vec3;

typedef union vec4
{
	struct
	{
		f32 x, y, z, w;
	};

	struct
	{
		f32 r, g, b, a;
	};

	struct
	{
		f32 s, t, p, q;
	};

	struct
	{
		vec2 xy, zw;
	};

	struct
	{
		vec2 st, pq;
	};

	vec3 xyz;
	vec3 rgb;
	f32  e[ 4 ];
} vec4;

typedef union mat2
{
	struct
	{
		vec2 x, y;
	};

	vec2 col[ 2 ];
	f32  e[ 4 ];
} mat2;

typedef union mat3
{
	struct
	{
		vec3 x, y, z;
	};

	vec3 col[ 3 ];
	f32  e[ 9 ];
} mat3;

typedef union mat4
{
	struct
	{
		vec4 x, y, z, w;
	};

	vec4 col[ 4 ];
	f32  e[ 16 ];
} mat4;

typedef union quat
{
	struct
	{
		f32 x, y, z, w;
	};

	vec4 xyzw;
	vec3 xyz;
	f32  e[ 4 ];
} quat;

typedef union plane
{
	struct
	{
		f32 a, b, c, d;
	};

	vec4 xyzw;
	vec3 n;
	f32  e[ 4 ];
} plane;

typedef struct frustum
{
	plane x1;
	plane x2;
	plane y1;
	plane y2;
	plane z1;
	plane z2;
} frustum;

typedef f32 float2[ 2 ];
typedef f32 float3[ 3 ];
typedef f32 float4[ 4 ];

typedef struct rect2
{
	vec2 pos, dim;
} rect2;

typedef struct rect3
{
	vec3 pos, dim;
} rect3;

typedef struct aabb2
{
	vec2 min, max;
} aabb2;

typedef struct aabb3
{
	vec3 min, max;
} aabb3;

typedef short half;

#ifndef ZPL_CONSTANTS
#	define ZPL_CONSTANTS
#	define ZPL_EPSILON    1.19209290e-7f
#	define ZPL_ZERO       0.0f
#	define ZPL_ONE        1.0f
#	define ZPL_TWO_THIRDS 0.666666666666666666666666666666666666667f

#	define ZPL_TAU          6.28318530717958647692528676655900576f
#	define ZPL_PI           3.14159265358979323846264338327950288f
#	define ZPL_ONE_OVER_TAU 0.636619772367581343075535053490057448f
#	define ZPL_ONE_OVER_PI  0.159154943091895335768883763372514362f

#	define ZPL_TAU_OVER_2 3.14159265358979323846264338327950288f
#	define ZPL_TAU_OVER_4 1.570796326794896619231321691639751442f
#	define ZPL_TAU_OVER_8 0.785398163397448309615660845819875721f

#	define ZPL_E          2.71828182845904523536f
#	define ZPL_SQRT_TWO   1.41421356237309504880168872420969808f
#	define ZPL_SQRT_THREE 1.73205080756887729352744634150587236f
#	define ZPL_SQRT_FIVE  2.23606797749978969640917366873127623f

#	define ZPL_LOG_TWO 0.693147180559945309417232121458176568f
#	define ZPL_LOG_TEN 2.30258509299404568401799145468436421f
#endif    // ZPL_CONSTANTS

#ifndef square
#	define square( x ) ( ( x ) * ( x ) )
#endif

#ifndef cube
#	define cube( x ) ( ( x ) * ( x ) * ( x ) )
#endif

#ifndef sign
#	define sign( x ) ( ( x ) >= 0.0f ? 1.0f : -1.0f )
#endif

#ifndef sign0
#	define sign0( x ) ( ( x == 0.0f ) ? 0.0f : ( ( x ) >= 0.0f ? 1.0f : -1.0f ) )
#endif

ZPL_DEF f32 to_radians( f32 degrees );
ZPL_DEF f32 to_degrees( f32 radians );

/* NOTE: Because to interpolate angles */
ZPL_DEF f32 angle_diff( f32 radians_a, f32 radians_b );

ZPL_DEF f32 copy_sign( f32 x, f32 y );
ZPL_DEF f32 remainder( f32 x, f32 y );
ZPL_DEF f32 mod( f32 x, f32 y );
ZPL_DEF f64 copy_sign64( f64 x, f64 y );
ZPL_DEF f64 floor64( f64 x );
ZPL_DEF f64 ceil64( f64 x );
ZPL_DEF f64 round64( f64 x );
ZPL_DEF f64 remainder64( f64 x, f64 y );
ZPL_DEF f64 abs64( f64 x );
ZPL_DEF f64 sign64( f64 x );
ZPL_DEF f64 mod64( f64 x, f64 y );
ZPL_DEF f32 sqrt( f32 a );
ZPL_DEF f32 rsqrt( f32 a );
ZPL_DEF f32 quake_rsqrt( f32 a ); /* NOTE: It's probably better to use 1.0f/zpl_sqrt(a)
                                   * And for simd, there is usually isqrt functions too!
                                   */
ZPL_DEF f32 sin( f32 radians );
ZPL_DEF f32 cos( f32 radians );
ZPL_DEF f32 tan( f32 radians );
ZPL_DEF f32 arcsin( f32 a );
ZPL_DEF f32 arccos( f32 a );
ZPL_DEF f32 arctan( f32 a );
ZPL_DEF f32 arctan2( f32 y, f32 x );

ZPL_DEF f32 exp( f32 x );
ZPL_DEF f32 exp2( f32 x );
ZPL_DEF f32 log( f32 x );
ZPL_DEF f32 log2( f32 x );
ZPL_DEF f32 fast_exp( f32 x );   /* NOTE: Only valid from -1 <= x <= +1 */
ZPL_DEF f32 fast_exp2( f32 x );  /* NOTE: Only valid from -1 <= x <= +1 */
ZPL_DEF f32 pow( f32 x, f32 y ); /* x^y */

ZPL_DEF f32 round( f32 x );
ZPL_DEF f32 floor( f32 x );
ZPL_DEF f32 ceil( f32 x );

ZPL_DEF f32  half_to_float( half value );
ZPL_DEF half float_to_half( f32 value );

ZPL_DEF vec2 vec2f_zero( void );
ZPL_DEF vec2 vec2f( f32 x, f32 y );
ZPL_DEF vec2 vec2fv( f32 x[ 2 ] );

ZPL_DEF vec3 vec3f_zero( void );
ZPL_DEF vec3 vec3f( f32 x, f32 y, f32 z );
ZPL_DEF vec3 vec3fv( f32 x[ 3 ] );

ZPL_DEF vec4 vec4f_zero( void );
ZPL_DEF vec4 vec4f( f32 x, f32 y, f32 z, f32 w );
ZPL_DEF vec4 vec4fv( f32 x[ 4 ] );

ZPL_DEF f32  vec2_max( vec2 v );
ZPL_DEF f32  vec2_side( vec2 p, vec2 q, vec2 r );
ZPL_DEF void vec2_add( vec2* d, vec2 v0, vec2 v1 );
ZPL_DEF void vec2_sub( vec2* d, vec2 v0, vec2 v1 );
ZPL_DEF void vec2_mul( vec2* d, vec2 v, f32 s );
ZPL_DEF void vec2_div( vec2* d, vec2 v, f32 s );

ZPL_DEF f32  vec3_max( vec3 v );
ZPL_DEF void vec3_add( vec3* d, vec3 v0, vec3 v1 );
ZPL_DEF void vec3_sub( vec3* d, vec3 v0, vec3 v1 );
ZPL_DEF void vec3_mul( vec3* d, vec3 v, f32 s );
ZPL_DEF void vec3_div( vec3* d, vec3 v, f32 s );

ZPL_DEF void vec4_add( vec4* d, vec4 v0, vec4 v1 );
ZPL_DEF void vec4_sub( vec4* d, vec4 v0, vec4 v1 );
ZPL_DEF void vec4_mul( vec4* d, vec4 v, f32 s );
ZPL_DEF void vec4_div( vec4* d, vec4 v, f32 s );

ZPL_DEF void vec2_addeq( vec2* d, vec2 v );
ZPL_DEF void vec2_subeq( vec2* d, vec2 v );
ZPL_DEF void vec2_muleq( vec2* d, f32 s );
ZPL_DEF void vec2_diveq( vec2* d, f32 s );

ZPL_DEF void vec3_addeq( vec3* d, vec3 v );
ZPL_DEF void vec3_subeq( vec3* d, vec3 v );
ZPL_DEF void vec3_muleq( vec3* d, f32 s );
ZPL_DEF void vec3_diveq( vec3* d, f32 s );

ZPL_DEF void vec4_addeq( vec4* d, vec4 v );
ZPL_DEF void vec4_subeq( vec4* d, vec4 v );
ZPL_DEF void vec4_muleq( vec4* d, f32 s );
ZPL_DEF void vec4_diveq( vec4* d, f32 s );

ZPL_DEF f32 vec2_dot( vec2 v0, vec2 v1 );
ZPL_DEF f32 vec3_dot( vec3 v0, vec3 v1 );
ZPL_DEF f32 vec4_dot( vec4 v0, vec4 v1 );

ZPL_DEF void vec2_cross( f32* d, vec2 v0, vec2 v1 );
ZPL_DEF void vec3_cross( vec3* d, vec3 v0, vec3 v1 );

ZPL_DEF f32 vec2_mag2( vec2 v );
ZPL_DEF f32 vec3_mag2( vec3 v );
ZPL_DEF f32 vec4_mag2( vec4 v );

ZPL_DEF f32 vec2_mag( vec2 v );
ZPL_DEF f32 vec3_mag( vec3 v );
ZPL_DEF f32 vec4_mag( vec4 v );

ZPL_DEF void vec2_norm( vec2* d, vec2 v );
ZPL_DEF void vec3_norm( vec3* d, vec3 v );
ZPL_DEF void vec4_norm( vec4* d, vec4 v );

ZPL_DEF void vec2_norm0( vec2* d, vec2 v );
ZPL_DEF void vec3_norm0( vec3* d, vec3 v );
ZPL_DEF void vec4_norm0( vec4* d, vec4 v );

ZPL_DEF void vec2_reflect( vec2* d, vec2 i, vec2 n );
ZPL_DEF void vec3_reflect( vec3* d, vec3 i, vec3 n );
ZPL_DEF void vec2_refract( vec2* d, vec2 i, vec2 n, f32 eta );
ZPL_DEF void vec3_refract( vec3* d, vec3 i, vec3 n, f32 eta );

ZPL_DEF f32 vec2_aspect_ratio( vec2 v );

ZPL_DEF void mat2_identity( mat2* m );
ZPL_DEF void float22_identity( f32 m[ 2 ][ 2 ] );

ZPL_DEF void mat2_transpose( mat2* m );
ZPL_DEF void mat2_mul( mat2* out, mat2* m1, mat2* m2 );
ZPL_DEF void mat2_mul_vec2( vec2* out, mat2* m, vec2 in );
ZPL_DEF void mat2_inverse( mat2* out, mat2* in );
ZPL_DEF f32  mat2_determinate( mat2* m );

ZPL_DEF mat2*   mat2_v( vec2 m[ 2 ] );
ZPL_DEF mat2*   mat2_f( f32 m[ 2 ][ 2 ] );
ZPL_DEF float2* float22_m( mat2* m );
ZPL_DEF float2* float22_v( vec2 m[ 2 ] );
ZPL_DEF float2* float22_4( f32 m[ 4 ] );

ZPL_DEF void float22_transpose( f32 ( *vec )[ 2 ] );
ZPL_DEF void float22_mul( f32 ( *out )[ 2 ], f32 ( *mat1 )[ 2 ], f32 ( *mat2 )[ 2 ] );
ZPL_DEF void float22_mul_vec2( vec2* out, f32 m[ 2 ][ 2 ], vec2 in );

ZPL_DEF void mat3_identity( mat3* m );
ZPL_DEF void float33_identity( f32 m[ 3 ][ 3 ] );

ZPL_DEF void mat3_transpose( mat3* m );
ZPL_DEF void mat3_mul( mat3* out, mat3* m1, mat3* m2 );
ZPL_DEF void mat3_mul_vec3( vec3* out, mat3* m, vec3 in );
ZPL_DEF void mat3_inverse( mat3* out, mat3* in );
ZPL_DEF f32  mat3_determinate( mat3* m );

ZPL_DEF mat3* mat3_v( vec3 m[ 3 ] );
ZPL_DEF mat3* mat3_f( f32 m[ 3 ][ 3 ] );

ZPL_DEF float3* float33_m( mat3* m );
ZPL_DEF float3* float33_v( vec3 m[ 3 ] );
ZPL_DEF float3* float33_9( f32 m[ 9 ] );

ZPL_DEF void float33_transpose( f32 ( *vec )[ 3 ] );
ZPL_DEF void float33_mul( f32 ( *out )[ 3 ], f32 ( *mat1 )[ 3 ], f32 ( *mat2 )[ 3 ] );
ZPL_DEF void float33_mul_vec3( vec3* out, f32 m[ 3 ][ 3 ], vec3 in );

ZPL_DEF void mat4_identity( mat4* m );
ZPL_DEF void float44_identity( f32 m[ 4 ][ 4 ] );
ZPL_DEF void mat4_copy( mat4* out, mat4* m );

ZPL_DEF void mat4_transpose( mat4* m );
ZPL_DEF void mat4_mul( mat4* out, mat4* m1, mat4* m2 );
ZPL_DEF void mat4_mul_vec4( vec4* out, mat4* m, vec4 in );
ZPL_DEF void mat4_inverse( mat4* out, mat4* in );

ZPL_DEF mat4* mat4_v( vec4 m[ 4 ] );
ZPL_DEF mat4* mat4_f( f32 m[ 4 ][ 4 ] );

ZPL_DEF float4* float44_m( mat4* m );
ZPL_DEF float4* float44_v( vec4 m[ 4 ] );
ZPL_DEF float4* float44_16( f32 m[ 16 ] );

ZPL_DEF void float44_transpose( f32 ( *vec )[ 4 ] );
ZPL_DEF void float44_mul( f32 ( *out )[ 4 ], f32 ( *mat1 )[ 4 ], f32 ( *mat2 )[ 4 ] );
ZPL_DEF void float44_mul_vec4( vec4* out, f32 m[ 4 ][ 4 ], vec4 in );

ZPL_DEF void mat4_axis_angle( mat4* out, vec3 v, f32 angle_radians );
ZPL_DEF void mat4_to_translate( mat4* out, vec3 v );
ZPL_DEF void mat4_to_rotate( mat4* out, vec3 v, f32 angle_radians );
ZPL_DEF void mat4_to_scale( mat4* out, vec3 v );
ZPL_DEF void mat4_to_scalef( mat4* out, f32 s );
ZPL_DEF void mat4_translate( mat4* out, vec3 v );
ZPL_DEF void mat4_rotate( mat4* out, vec3 v, f32 angle_radians );
ZPL_DEF void mat4_scale( mat4* out, vec3 v );
ZPL_DEF void mat4_scalef( mat4* out, f32 s );
ZPL_DEF void mat4_ortho2d( mat4* out, f32 left, f32 right, f32 bottom, f32 top );
ZPL_DEF void mat4_ortho3d( mat4* out, f32 left, f32 right, f32 bottom, f32 top, f32 z_near, f32 z_far );
ZPL_DEF void mat4_perspective( mat4* out, f32 fovy, f32 aspect, f32 z_near, f32 z_far );
ZPL_DEF void mat4_infinite_perspective( mat4* out, f32 fovy, f32 aspect, f32 z_near );

ZPL_DEF void mat4_ortho2d_dx( mat4* out, f32 left, f32 right, f32 bottom, f32 top );
ZPL_DEF void mat4_ortho3d_dx( mat4* out, f32 left, f32 right, f32 bottom, f32 top, f32 z_near, f32 z_far );
ZPL_DEF void mat4_perspective_dx( mat4* out, f32 fovy, f32 aspect, f32 z_near, f32 z_far );
ZPL_DEF void mat4_infinite_perspective_dx( mat4* out, f32 fovy, f32 aspect, f32 z_near );

ZPL_DEF void mat4_look_at( mat4* out, vec3 eye, vec3 centre, vec3 up );

ZPL_DEF void mat4_look_at_lh( mat4* out, vec3 eye, vec3 centre, vec3 up );

ZPL_DEF quat quatf( f32 x, f32 y, f32 z, f32 w );
ZPL_DEF quat quatfv( f32 e[ 4 ] );
ZPL_DEF quat quat_axis_angle( vec3 axis, f32 angle_radians );
ZPL_DEF quat quat_euler_angles( f32 pitch, f32 yaw, f32 roll );
ZPL_DEF quat quat_identity( void );

ZPL_DEF void quat_add( quat* d, quat q0, quat q1 );
ZPL_DEF void quat_sub( quat* d, quat q0, quat q1 );
ZPL_DEF void quat_mul( quat* d, quat q0, quat q1 );
ZPL_DEF void quat_div( quat* d, quat q0, quat q1 );

ZPL_DEF void quat_mulf( quat* d, quat q, f32 s );
ZPL_DEF void quat_divf( quat* d, quat q, f32 s );

ZPL_DEF void quat_addeq( quat* d, quat q );
ZPL_DEF void quat_subeq( quat* d, quat q );
ZPL_DEF void quat_muleq( quat* d, quat q );
ZPL_DEF void quat_diveq( quat* d, quat q );

ZPL_DEF void quat_muleqf( quat* d, f32 s );
ZPL_DEF void quat_diveqf( quat* d, f32 s );

ZPL_DEF f32 quat_dot( quat q0, quat q1 );
ZPL_DEF f32 quat_mag( quat q );

ZPL_DEF void quat_norm( quat* d, quat q );
ZPL_DEF void quat_conj( quat* d, quat q );
ZPL_DEF void quat_inverse( quat* d, quat q );

ZPL_DEF void quat_axis( vec3* axis, quat q );
ZPL_DEF f32  quat_angle( quat q );

ZPL_DEF f32 quat_pitch( quat q );
ZPL_DEF f32 quat_yaw( quat q );
ZPL_DEF f32 quat_roll( quat q );

/* NOTE: Rotate v by q */
ZPL_DEF void quat_rotate_vec3( vec3* d, quat q, vec3 v );
ZPL_DEF void mat4_from_quat( mat4* out, quat q );
ZPL_DEF void quat_from_mat4( quat* out, mat4* m );

/* Plane math. */
ZPL_DEF f32 plane_distance( plane* p, vec3 v );

/* Frustum culling. */
ZPL_DEF void frustum_create( frustum* out, mat4* camera, mat4* proj );
ZPL_DEF b8   frustum_sphere_inside( frustum* frustum, vec3 center, f32 radius );
ZPL_DEF b8   frustum_point_inside( frustum* frustum, vec3 point );
ZPL_DEF b8   frustum_box_inside( frustum* frustum, aabb3 box );

/* Interpolations */
ZPL_DEF f32 lerp( f32 a, f32 b, f32 t );
ZPL_DEF f32 unlerp( f32 t, f32 a, f32 b );
ZPL_DEF f32 smooth_step( f32 a, f32 b, f32 t );
ZPL_DEF f32 smoother_step( f32 a, f32 b, f32 t );

ZPL_DEF void vec2_lerp( vec2* d, vec2 a, vec2 b, f32 t );
ZPL_DEF void vec3_lerp( vec3* d, vec3 a, vec3 b, f32 t );
ZPL_DEF void vec4_lerp( vec4* d, vec4 a, vec4 b, f32 t );

ZPL_DEF void vec2_cslerp( vec2* d, vec2 a, vec2 v0, vec2 b, vec2 v1, f32 t );
ZPL_DEF void vec3_cslerp( vec3* d, vec3 a, vec3 v0, vec3 b, vec3 v1, f32 t );
ZPL_DEF void vec2_dcslerp( vec2* d, vec2 a, vec2 v0, vec2 b, vec2 v1, f32 t );
ZPL_DEF void vec3_dcslerp( vec3* d, vec3 a, vec3 v0, vec3 b, vec3 v1, f32 t );

ZPL_DEF void quat_lerp( quat* d, quat a, quat b, f32 t );
ZPL_DEF void quat_nlerp( quat* d, quat a, quat b, f32 t );
ZPL_DEF void quat_slerp( quat* d, quat a, quat b, f32 t );
ZPL_DEF void quat_nquad( quat* d, quat p, quat a, quat b, quat q, f32 t );
ZPL_DEF void quat_squad( quat* d, quat p, quat a, quat b, quat q, f32 t );
ZPL_DEF void quat_slerp_approx( quat* d, quat a, quat b, f32 t );
ZPL_DEF void quat_squad_approx( quat* d, quat p, quat a, quat b, quat q, f32 t );

/* rects */
ZPL_DEF rect2 rect2f( vec2 pos, vec2 dim );
ZPL_DEF rect3 rect3f( vec3 pos, vec3 dim );

ZPL_DEF aabb2 aabb2f( f32 minx, f32 miny, f32 maxx, f32 maxy );
ZPL_DEF aabb3 aabb3f( f32 minx, f32 miny, f32 minz, f32 maxx, f32 maxy, f32 maxz );

ZPL_DEF aabb2 aabb2_rect2( rect2 a );
ZPL_DEF aabb3 aabb3_rect3( rect3 a );
ZPL_DEF rect2 rect2_aabb2( aabb2 a );
ZPL_DEF rect3 rect3_aabb3( aabb3 a );

ZPL_DEF int rect2_contains( rect2 a, f32 x, f32 y );
ZPL_DEF int rect2_contains_vec2( rect2 a, vec2 p );
ZPL_DEF int rect2_intersects( rect2 a, rect2 b );
ZPL_DEF int rect2_intersection_result( rect2 a, rect2 b, rect2* intersection );
ZPL_DEF int aabb2_contains( aabb2 a, f32 x, f32 y );
ZPL_DEF int aabb3_contains( aabb3 a, f32 x, f32 y, f32 z );

/* rectangle partitioning: based on https://halt.software/dead-simple-layouts/ */
ZPL_DEF aabb2 aabb2_cut_left( aabb2* a, f32 b );
ZPL_DEF aabb2 aabb2_cut_right( aabb2* a, f32 b );
ZPL_DEF aabb2 aabb2_cut_top( aabb2* a, f32 b );
ZPL_DEF aabb2 aabb2_cut_bottom( aabb2* a, f32 b );

ZPL_DEF aabb2 aabb2_get_left( const aabb2* a, f32 b );
ZPL_DEF aabb2 aabb2_get_right( const aabb2* a, f32 b );
ZPL_DEF aabb2 aabb2_get_top( const aabb2* a, f32 b );
ZPL_DEF aabb2 aabb2_get_bottom( const aabb2* a, f32 b );

ZPL_DEF aabb2 aabb2_add_left( const aabb2* a, f32 b );
ZPL_DEF aabb2 aabb2_add_right( const aabb2* a, f32 b );
ZPL_DEF aabb2 aabb2_add_top( const aabb2* a, f32 b );
ZPL_DEF aabb2 aabb2_add_bottom( const aabb2* a, f32 b );

ZPL_DEF aabb2 aabb2_contract( const aabb2* a, f32 b );
ZPL_DEF aabb2 aabb2_expand( const aabb2* a, f32 b );

//! @}
ZPL_END_C_DECLS
#if defined( __cplusplus )
ZPL_INLINE bool operator==( vec2 a, vec2 b )
{
	return ( a.x == b.x ) && ( a.y == b.y );
}

ZPL_INLINE bool operator!=( vec2 a, vec2 b )
{
	return ! operator==( a, b );
}

ZPL_INLINE vec2 operator+( vec2 a )
{
	return a;
}

ZPL_INLINE vec2 operator-( vec2 a )
{
	vec2 r = { -a.x, -a.y };
	return r;
}

ZPL_INLINE vec2 operator+( vec2 a, vec2 b )
{
	vec2 r;
	vec2_add( &r, a, b );
	return r;
}

ZPL_INLINE vec2 operator-( vec2 a, vec2 b )
{
	vec2 r;
	vec2_sub( &r, a, b );
	return r;
}

ZPL_INLINE vec2 operator*( vec2 a, float scalar )
{
	vec2 r;
	vec2_mul( &r, a, scalar );
	return r;
}

ZPL_INLINE vec2 operator*( float scalar, vec2 a )
{
	return operator*( a, scalar );
}

ZPL_INLINE vec2 operator/( vec2 a, float scalar )
{
	return operator*( a, 1.0f / scalar );
}

/* Hadamard Product */
ZPL_INLINE vec2 operator*( vec2 a, vec2 b )
{
	vec2 r = { a.x * b.x, a.y * b.y };
	return r;
}

ZPL_INLINE vec2 operator/( vec2 a, vec2 b )
{
	vec2 r = { a.x / b.x, a.y / b.y };
	return r;
}

ZPL_INLINE vec2& operator+=( vec2& a, vec2 b )
{
	return ( a = a + b );
}

ZPL_INLINE vec2& operator-=( vec2& a, vec2 b )
{
	return ( a = a - b );
}

ZPL_INLINE vec2& operator*=( vec2& a, float scalar )
{
	return ( a = a * scalar );
}

ZPL_INLINE vec2& operator/=( vec2& a, float scalar )
{
	return ( a = a / scalar );
}

ZPL_INLINE bool operator==( vec3 a, vec3 b )
{
	return ( a.x == b.x ) && ( a.y == b.y ) && ( a.z == b.z );
}

ZPL_INLINE bool operator!=( vec3 a, vec3 b )
{
	return ! operator==( a, b );
}

ZPL_INLINE vec3 operator+( vec3 a )
{
	return a;
}

ZPL_INLINE vec3 operator-( vec3 a )
{
	vec3 r = { -a.x, -a.y, -a.z };
	return r;
}

ZPL_INLINE vec3 operator+( vec3 a, vec3 b )
{
	vec3 r;
	vec3_add( &r, a, b );
	return r;
}

ZPL_INLINE vec3 operator-( vec3 a, vec3 b )
{
	vec3 r;
	vec3_sub( &r, a, b );
	return r;
}

ZPL_INLINE vec3 operator*( vec3 a, float scalar )
{
	vec3 r;
	vec3_mul( &r, a, scalar );
	return r;
}

ZPL_INLINE vec3 operator*( float scalar, vec3 a )
{
	return operator*( a, scalar );
}

ZPL_INLINE vec3 operator/( vec3 a, float scalar )
{
	return operator*( a, 1.0f / scalar );
}

/* Hadamard Product */
ZPL_INLINE vec3 operator*( vec3 a, vec3 b )
{
	vec3 r = { a.x * b.x, a.y * b.y, a.z * b.z };
	return r;
}

ZPL_INLINE vec3 operator/( vec3 a, vec3 b )
{
	vec3 r = { a.x / b.x, a.y / b.y, a.z / b.z };
	return r;
}

ZPL_INLINE vec3& operator+=( vec3& a, vec3 b )
{
	return ( a = a + b );
}

ZPL_INLINE vec3& operator-=( vec3& a, vec3 b )
{
	return ( a = a - b );
}

ZPL_INLINE vec3& operator*=( vec3& a, float scalar )
{
	return ( a = a * scalar );
}

ZPL_INLINE vec3& operator/=( vec3& a, float scalar )
{
	return ( a = a / scalar );
}

ZPL_INLINE bool operator==( vec4 a, vec4 b )
{
	return ( a.x == b.x ) && ( a.y == b.y ) && ( a.z == b.z ) && ( a.w == b.w );
}

ZPL_INLINE bool operator!=( vec4 a, vec4 b )
{
	return ! operator==( a, b );
}

ZPL_INLINE vec4 operator+( vec4 a )
{
	return a;
}

ZPL_INLINE vec4 operator-( vec4 a )
{
	vec4 r = { -a.x, -a.y, -a.z, -a.w };
	return r;
}

ZPL_INLINE vec4 operator+( vec4 a, vec4 b )
{
	vec4 r;
	vec4_add( &r, a, b );
	return r;
}

ZPL_INLINE vec4 operator-( vec4 a, vec4 b )
{
	vec4 r;
	vec4_sub( &r, a, b );
	return r;
}

ZPL_INLINE vec4 operator*( vec4 a, float scalar )
{
	vec4 r;
	vec4_mul( &r, a, scalar );
	return r;
}

ZPL_INLINE vec4 operator*( float scalar, vec4 a )
{
	return operator*( a, scalar );
}

ZPL_INLINE vec4 operator/( vec4 a, float scalar )
{
	return operator*( a, 1.0f / scalar );
}

/* Hadamard Product */
ZPL_INLINE vec4 operator*( vec4 a, vec4 b )
{
	vec4 r = { a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w };
	return r;
}

ZPL_INLINE vec4 operator/( vec4 a, vec4 b )
{
	vec4 r = { a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w };
	return r;
}

ZPL_INLINE vec4& operator+=( vec4& a, vec4 b )
{
	return ( a = a + b );
}

ZPL_INLINE vec4& operator-=( vec4& a, vec4 b )
{
	return ( a = a - b );
}

ZPL_INLINE vec4& operator*=( vec4& a, float scalar )
{
	return ( a = a * scalar );
}

ZPL_INLINE vec4& operator/=( vec4& a, float scalar )
{
	return ( a = a / scalar );
}

ZPL_INLINE mat2 operator+( mat2 const& a, mat2 const& b )
{
	int  i, j;
	mat2 r = { 0 };
	for ( j = 0; j < 2; j++ )
	{
		for ( i = 0; i < 2; i++ )
			r.e[ 2 * j + i ] = a.e[ 2 * j + i ] + b.e[ 2 * j + i ];
	}
	return r;
}

ZPL_INLINE mat2 operator-( mat2 const& a, mat2 const& b )
{
	int  i, j;
	mat2 r = { 0 };
	for ( j = 0; j < 2; j++ )
	{
		for ( i = 0; i < 2; i++ )
			r.e[ 2 * j + i ] = a.e[ 2 * j + i ] - b.e[ 2 * j + i ];
	}
	return r;
}

ZPL_INLINE mat2 operator*( mat2 const& a, mat2 const& b )
{
	mat2 r;
	mat2_mul( &r, ( mat2* )&a, ( mat2* )&b );
	return r;
}

ZPL_INLINE vec2 operator*( mat2 const& a, vec2 v )
{
	vec2 r;
	mat2_mul_vec2( &r, ( mat2* )&a, v );
	return r;
}

ZPL_INLINE mat2 operator*( mat2 const& a, float scalar )
{
	mat2 r = { 0 };
	int  i;
	for ( i = 0; i < 2 * 2; i++ )
		r.e[ i ] = a.e[ i ] * scalar;
	return r;
}

ZPL_INLINE mat2 operator*( float scalar, mat2 const& a )
{
	return operator*( a, scalar );
}

ZPL_INLINE mat2 operator/( mat2 const& a, float scalar )
{
	return operator*( a, 1.0f / scalar );
}

ZPL_INLINE mat2& operator+=( mat2& a, mat2 const& b )
{
	return ( a = a + b );
}

ZPL_INLINE mat2& operator-=( mat2& a, mat2 const& b )
{
	return ( a = a - b );
}

ZPL_INLINE mat2& operator*=( mat2& a, mat2 const& b )
{
	return ( a = a * b );
}

ZPL_INLINE mat3 operator+( mat3 const& a, mat3 const& b )
{
	int  i, j;
	mat3 r = { 0 };
	for ( j = 0; j < 3; j++ )
	{
		for ( i = 0; i < 3; i++ )
			r.e[ 3 * j + i ] = a.e[ 3 * j + i ] + b.e[ 3 * j + i ];
	}
	return r;
}

ZPL_INLINE mat3 operator-( mat3 const& a, mat3 const& b )
{
	int  i, j;
	mat3 r = { 0 };
	for ( j = 0; j < 3; j++ )
	{
		for ( i = 0; i < 3; i++ )
			r.e[ 3 * j + i ] = a.e[ 3 * j + i ] - b.e[ 3 * j + i ];
	}
	return r;
}

ZPL_INLINE mat3 operator*( mat3 const& a, mat3 const& b )
{
	mat3 r;
	mat3_mul( &r, ( mat3* )&a, ( mat3* )&b );
	return r;
}

ZPL_INLINE vec3 operator*( mat3 const& a, vec3 v )
{
	vec3 r;
	mat3_mul_vec3( &r, ( mat3* )&a, v );
	return r;
}

ZPL_INLINE mat3 operator*( mat3 const& a, float scalar )
{
	mat3 r = { 0 };
	int  i;
	for ( i = 0; i < 3 * 3; i++ )
		r.e[ i ] = a.e[ i ] * scalar;
	return r;
}

ZPL_INLINE mat3 operator*( float scalar, mat3 const& a )
{
	return operator*( a, scalar );
}

ZPL_INLINE mat3 operator/( mat3 const& a, float scalar )
{
	return operator*( a, 1.0f / scalar );
}

ZPL_INLINE mat3& operator+=( mat3& a, mat3 const& b )
{
	return ( a = a + b );
}

ZPL_INLINE mat3& operator-=( mat3& a, mat3 const& b )
{
	return ( a = a - b );
}

ZPL_INLINE mat3& operator*=( mat3& a, mat3 const& b )
{
	return ( a = a * b );
}

ZPL_INLINE mat4 operator+( mat4 const& a, mat4 const& b )
{
	int  i, j;
	mat4 r = { 0 };
	for ( j = 0; j < 4; j++ )
	{
		for ( i = 0; i < 4; i++ )
			r.e[ 4 * j + i ] = a.e[ 4 * j + i ] + b.e[ 4 * j + i ];
	}
	return r;
}

ZPL_INLINE mat4 operator-( mat4 const& a, mat4 const& b )
{
	int  i, j;
	mat4 r = { 0 };
	for ( j = 0; j < 4; j++ )
	{
		for ( i = 0; i < 4; i++ )
			r.e[ 4 * j + i ] = a.e[ 4 * j + i ] - b.e[ 4 * j + i ];
	}
	return r;
}

ZPL_INLINE mat4 operator*( mat4 const& a, mat4 const& b )
{
	mat4 r;
	mat4_mul( &r, ( mat4* )&a, ( mat4* )&b );
	return r;
}

ZPL_INLINE vec4 operator*( mat4 const& a, vec4 v )
{
	vec4 r;
	mat4_mul_vec4( &r, ( mat4* )&a, v );
	return r;
}

ZPL_INLINE mat4 operator*( mat4 const& a, float scalar )
{
	mat4 r = { 0 };
	int  i;
	for ( i = 0; i < 4 * 4; i++ )
		r.e[ i ] = a.e[ i ] * scalar;
	return r;
}

ZPL_INLINE mat4 operator*( float scalar, mat4 const& a )
{
	return operator*( a, scalar );
}

ZPL_INLINE mat4 operator/( mat4 const& a, float scalar )
{
	return operator*( a, 1.0f / scalar );
}

ZPL_INLINE mat4& operator+=( mat4& a, mat4 const& b )
{
	return ( a = a + b );
}

ZPL_INLINE mat4& operator-=( mat4& a, mat4 const& b )
{
	return ( a = a - b );
}

ZPL_INLINE mat4& operator*=( mat4& a, mat4 const& b )
{
	return ( a = a * b );
}

ZPL_INLINE bool operator==( quat a, quat b )
{
	return a.xyzw == b.xyzw;
}

ZPL_INLINE bool operator!=( quat a, quat b )
{
	return ! operator==( a, b );
}

ZPL_INLINE quat operator+( quat q )
{
	return q;
}

ZPL_INLINE quat operator-( quat q )
{
	return quatf( -q.x, -q.y, -q.z, -q.w );
}

ZPL_INLINE quat operator+( quat a, quat b )
{
	quat r;
	quat_add( &r, a, b );
	return r;
}

ZPL_INLINE quat operator-( quat a, quat b )
{
	quat r;
	quat_sub( &r, a, b );
	return r;
}

ZPL_INLINE quat operator*( quat a, quat b )
{
	quat r;
	quat_mul( &r, a, b );
	return r;
}

ZPL_INLINE quat operator*( quat q, float s )
{
	quat r;
	quat_mulf( &r, q, s );
	return r;
}

ZPL_INLINE quat operator*( float s, quat q )
{
	return operator*( q, s );
}

ZPL_INLINE quat operator/( quat q, float s )
{
	quat r;
	quat_divf( &r, q, s );
	return r;
}

ZPL_INLINE quat& operator+=( quat& a, quat b )
{
	quat_addeq( &a, b );
	return a;
}

ZPL_INLINE quat& operator-=( quat& a, quat b )
{
	quat_subeq( &a, b );
	return a;
}

ZPL_INLINE quat& operator*=( quat& a, quat b )
{
	quat_muleq( &a, b );
	return a;
}

ZPL_INLINE quat& operator/=( quat& a, quat b )
{
	quat_diveq( &a, b );
	return a;
}

ZPL_INLINE quat& operator*=( quat& a, float b )
{
	quat_muleqf( &a, b );
	return a;
}

ZPL_INLINE quat& operator/=( quat& a, float b )
{
	quat_diveqf( &a, b );
	return a;
}

/* Rotate v by a */
ZPL_INLINE vec3 operator*( quat q, vec3 v )
{
	vec3 r;
	quat_rotate_vec3( &r, q, v );
	return r;
}
#endif

ZPL_END_NAMESPACE

#	endif

#	if defined( ZPL_MODULE_PARSER )
// file: header/adt.h

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

typedef enum adt_type
{
	ZPL_ADT_TYPE_UNINITIALISED, /* node was not initialised, this is a programming error! */
	ZPL_ADT_TYPE_ARRAY,
	ZPL_ADT_TYPE_OBJECT,
	ZPL_ADT_TYPE_STRING,
	ZPL_ADT_TYPE_MULTISTRING,
	ZPL_ADT_TYPE_INTEGER,
	ZPL_ADT_TYPE_REAL,
} adt_type;

typedef enum adt_props
{
	ZPL_ADT_PROPS_NONE,
	ZPL_ADT_PROPS_NAN,
	ZPL_ADT_PROPS_NAN_NEG,
	ZPL_ADT_PROPS_INFINITY,
	ZPL_ADT_PROPS_INFINITY_NEG,
	ZPL_ADT_PROPS_FALSE,
	ZPL_ADT_PROPS_TRUE,
	ZPL_ADT_PROPS_NULL,
	ZPL_ADT_PROPS_IS_EXP,
	ZPL_ADT_PROPS_IS_HEX,

	// Used internally so that people can fill in real numbers they plan to write.
	ZPL_ADT_PROPS_IS_PARSED_REAL,
} adt_props;

typedef enum adt_naming_style
{
	ZPL_ADT_NAME_STYLE_DOUBLE_QUOTE,
	ZPL_ADT_NAME_STYLE_SINGLE_QUOTE,
	ZPL_ADT_NAME_STYLE_NO_QUOTES,
} adt_naming_style;

typedef enum adt_assign_style
{
	ZPL_ADT_ASSIGN_STYLE_COLON,
	ZPL_ADT_ASSIGN_STYLE_EQUALS,
	ZPL_ADT_ASSIGN_STYLE_LINE,
} adt_assign_style;

typedef enum adt_delim_style
{
	ZPL_ADT_DELIM_STYLE_COMMA,
	ZPL_ADT_DELIM_STYLE_LINE,
	ZPL_ADT_DELIM_STYLE_NEWLINE,
} adt_delim_style;

typedef enum adt_error
{
	ZPL_ADT_ERROR_NONE,
	ZPL_ADT_ERROR_INTERNAL,
	ZPL_ADT_ERROR_ALREADY_CONVERTED,
	ZPL_ADT_ERROR_INVALID_TYPE,
	ZPL_ADT_ERROR_OUT_OF_MEMORY,
} adt_error;

typedef struct adt_node
{
	char const*      name;
	struct adt_node* parent;

	/* properties */
	u8 type  : 4;
	u8 props : 4;
#ifndef ZPL_PARSER_DISABLE_ANALYSIS
	u8 cfg_mode          : 1;
	u8 name_style        : 2;
	u8 assign_style      : 2;
	u8 delim_style       : 2;
	u8 delim_line_width  : 4;
	u8 assign_line_width : 4;
#endif

	/* adt data */
	union
	{
		char const*      string;
		struct adt_node* nodes;    ///< zpl_array

		struct
		{
			union
			{
				f64 real;
				s64 integer;
			};

#ifndef ZPL_PARSER_DISABLE_ANALYSIS
			/* number analysis */
			s32 base;
			s32 base2;
			u8  base2_offset : 4;
			s8  exp          : 4;
			u8  neg_zero     : 1;
			u8  lead_digit   : 1;
#endif
		};
	};
} adt_node;

/* ADT NODE LIMITS
 * delimiter and assignment segment width is limited to 128 whitespace symbols each.
 * real number limits decimal position to 128 places.
 * real number exponent is limited to 64 digits.
 */

/**
 * @brief Initialise an ADT object or array
 *
 * @param node
 * @param backing Memory allocator used for descendants
 * @param name Node's name
 * @param is_array
 * @return error code
 */
ZPL_DEF u8 adt_make_branch( adt_node* node, AllocatorInfo backing, char const* name, b32 is_array );

/**
 * @brief Destroy an ADT branch and its descendants
 *
 * @param node
 * @return error code
 */
ZPL_DEF u8 adt_destroy_branch( adt_node* node );

/**
 * @brief Initialise an ADT leaf
 *
 * @param node
 * @param name Node's name
 * @param type Node's type (use zpl_adt_make_branch for container nodes)
 * @return error code
 */
ZPL_DEF u8 adt_make_leaf( adt_node* node, char const* name, u8 type );


/**
 * @brief Fetch a node using provided URI string.
 *
 * This method uses a basic syntax to fetch a node from the ADT. The following features are available
 * to retrieve the data:
 *
 * - "a/b/c" navigates through objects "a" and "b" to get to "c"
 * - "arr/[foo=123]/bar" iterates over "arr" to find any object with param "foo" that matches the value "123", then gets its field called "bar"
 * - "arr/3" retrieves the 4th element in "arr"
 * - "arr/[apple]" retrieves the first element of value "apple" in "arr"
 *
 * @param node ADT node
 * @param uri Locator string as described above
 * @return zpl_adt_node*
 *
 * @see code/apps/examples/json_get.c
 */
ZPL_DEF adt_node* adt_query( adt_node* node, char const* uri );

/**
 * @brief Find a field node within an object by the given name.
 *
 * @param node
 * @param name
 * @param deep_search Perform search recursively
 * @return zpl_adt_node * node
 */
ZPL_DEF adt_node* adt_find( adt_node* node, char const* name, b32 deep_search );

/**
 * @brief Allocate an unitialised node within a container at a specified index.
 *
 * @param parent
 * @param index
 * @return zpl_adt_node * node
 */
ZPL_DEF adt_node* adt_alloc_at( adt_node* parent, sw index );

/**
 * @brief Allocate an unitialised node within a container.
 *
 * @param parent
 * @return zpl_adt_node * node
 */
ZPL_DEF adt_node* adt_alloc( adt_node* parent );

/**
 * @brief Move an existing node to a new container at a specified index.
 *
 * @param node
 * @param new_parent
 * @param index
 * @return zpl_adt_node * node
 */
ZPL_DEF adt_node* adt_move_node_at( adt_node* node, adt_node* new_parent, sw index );

/**
 * @brief Move an existing node to a new container.
 *
 * @param node
 * @param new_parent
 * @return zpl_adt_node * node
 */
ZPL_DEF adt_node* adt_move_node( adt_node* node, adt_node* new_parent );

/**
 * @brief Swap two nodes.
 *
 * @param node
 * @param other_node
 * @return
 */
ZPL_DEF void adt_swap_nodes( adt_node* node, adt_node* other_node );

/**
 * @brief Remove node from container.
 *
 * @param node
 * @return
 */
ZPL_DEF void adt_remove_node( adt_node* node );

/**
 * @brief Initialise a node as an object
 *
 * @param obj
 * @param name
 * @param backing
 * @return
 */
ZPL_DEF b8 adt_set_obj( adt_node* obj, char const* name, AllocatorInfo backing );

/**
 * @brief Initialise a node as an array
 *
 * @param obj
 * @param name
 * @param backing
 * @return
 */
ZPL_DEF b8 adt_set_arr( adt_node* obj, char const* name, AllocatorInfo backing );

/**
 * @brief Initialise a node as a string
 *
 * @param obj
 * @param name
 * @param value
 * @return
 */
ZPL_DEF b8 adt_set_str( adt_node* obj, char const* name, char const* value );

/**
 * @brief Initialise a node as a float
 *
 * @param obj
 * @param name
 * @param value
 * @return
 */
ZPL_DEF b8 adt_set_flt( adt_node* obj, char const* name, f64 value );

/**
 * @brief Initialise a node as a signed integer
 *
 * @param obj
 * @param name
 * @param value
 * @return
 */
ZPL_DEF b8 adt_set_int( adt_node* obj, char const* name, s64 value );

/**
 * @brief Append a new node to a container as an object
 *
 * @param parent
 * @param name
 * @return*
 */
ZPL_DEF adt_node* adt_append_obj( adt_node* parent, char const* name );

/**
 * @brief Append a new node to a container as an array
 *
 * @param parent
 * @param name
 * @return*
 */
ZPL_DEF adt_node* adt_append_arr( adt_node* parent, char const* name );

/**
 * @brief Append a new node to a container as a string
 *
 * @param parent
 * @param name
 * @param value
 * @return*
 */
ZPL_DEF adt_node* adt_append_str( adt_node* parent, char const* name, char const* value );

/**
 * @brief Append a new node to a container as a float
 *
 * @param parent
 * @param name
 * @param value
 * @return*
 */
ZPL_DEF adt_node* adt_append_flt( adt_node* parent, char const* name, f64 value );

/**
 * @brief Append a new node to a container as a signed integer
 *
 * @param parent
 * @param name
 * @param value
 * @return*
 */
ZPL_DEF adt_node* adt_append_int( adt_node* parent, char const* name, s64 value );

/* parser helpers */

/**
 * @brief Parses a text and stores the result into an unitialised node.
 *
 * @param node
 * @param base
 * @return*
 */
ZPL_DEF char* adt_parse_number( adt_node* node, char* base );

/**
 * @brief Parses and converts an existing string node into a number.
 *
 * @param node
 * @return
 */
ZPL_DEF adt_error adt_str_to_number( adt_node* node );

/**
 * @brief Prints a number into a file stream.
 *
 * The provided file handle can also be a memory mapped stream.
 *
 * @see zpl_file_stream_new
 * @param file
 * @param node
 * @return
 */
ZPL_DEF adt_error adt_print_number( FileInfo* file, adt_node* node );

/**
 * @brief Prints a string into a file stream.
 *
 * The provided file handle can also be a memory mapped stream.
 *
 * @see zpl_file_stream_new
 * @param file
 * @param node
 * @param escaped_chars
 * @param escape_symbol
 * @return
 */
ZPL_DEF adt_error adt_print_string( FileInfo* file, adt_node* node, char const* escaped_chars, char const* escape_symbol );

/* extensions */

#if defined( __STDC_VERSION__ ) && __STDC_VERSION__ >= 201112L
#	define adt_append( parent, name, value ) \
		_Generic( ( value ), char*: adt_append_str, char const*: adt_append_str, f64: adt_append_flt, default: adt_append_int )( parent, name, value )
#	define adt_set( obj, name, value ) _Generic( ( value ), char*: adt_set_str, char const*: adt_set_str, f64: adt_set_flt, default: adt_set_int )( obj, name, value )
#endif

/* deprecated */

ZPL_DEPRECATED_FOR( 18.0.0, adt_query )

ZPL_IMPL_INLINE adt_node* adt_get( adt_node* node, char const* uri )
{
	return adt_query( node, uri );
}

ZPL_DEPRECATED_FOR( 13.3.0, adt_str_to_number )

ZPL_IMPL_INLINE void adt_str_to_flt( adt_node* node )
{
	( void )adt_str_to_number( node );
}

ZPL_DEPRECATED_FOR( 17.0.0, adt_append_obj )

ZPL_IMPL_INLINE adt_node* adt_inset_obj( adt_node* parent, char const* name )
{
	return adt_append_obj( parent, name );
}

ZPL_DEPRECATED_FOR( 17.0.0, adt_append_arr )

ZPL_IMPL_INLINE adt_node* adt_inset_arr( adt_node* parent, char const* name )
{
	return adt_append_arr( parent, name );
}

ZPL_DEPRECATED_FOR( 17.0.0, adt_append_str )

ZPL_IMPL_INLINE adt_node* adt_inset_str( adt_node* parent, char const* name, char const* value )
{
	return adt_append_str( parent, name, value );
}

ZPL_DEPRECATED_FOR( 17.0.0, adt_append_flt )

ZPL_IMPL_INLINE adt_node* adt_inset_flt( adt_node* parent, char const* name, f64 value )
{
	return adt_append_flt( parent, name, value );
}

ZPL_DEPRECATED_FOR( 17.0.0, adt_append_int )

ZPL_IMPL_INLINE adt_node* adt_inset_int( adt_node* parent, char const* name, s64 value )
{
	return adt_append_int( parent, name, value );
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE


/* parsers */
// file: header/parsers/csv.h


ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

typedef enum csv_error
{
	ZPL_CSV_ERROR_NONE,
	ZPL_CSV_ERROR_INTERNAL,
	ZPL_CSV_ERROR_UNEXPECTED_END_OF_INPUT,
	ZPL_CSV_ERROR_MISMATCHED_ROWS,
} csv_error;

typedef adt_node csv_object;

ZPL_DEF_INLINE u8 csv_parse( csv_object* root, char* text, AllocatorInfo allocator, b32 has_header );
ZPL_DEF u8        csv_parse_delimiter( csv_object* root, char* text, AllocatorInfo allocator, b32 has_header, char delim );
ZPL_DEF void      csv_free( csv_object* obj );

ZPL_DEF_INLINE void   csv_write( FileInfo* file, csv_object* obj );
ZPL_DEF_INLINE String csv_write_string( AllocatorInfo a, csv_object* obj );
ZPL_DEF void          csv_write_delimiter( FileInfo* file, csv_object* obj, char delim );
ZPL_DEF String        csv_write_string_delimiter( AllocatorInfo a, csv_object* obj, char delim );

/* inline */

ZPL_IMPL_INLINE u8 csv_parse( csv_object* root, char* text, AllocatorInfo allocator, b32 has_header )
{
	return csv_parse_delimiter( root, text, allocator, has_header, ',' );
}

ZPL_IMPL_INLINE void csv_write( FileInfo* file, csv_object* obj )
{
	csv_write_delimiter( file, obj, ',' );
}

ZPL_IMPL_INLINE String csv_write_string( AllocatorInfo a, csv_object* obj )
{
	return csv_write_string_delimiter( a, obj, ',' );
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: header/parsers/json.h


ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

typedef enum json_error
{
	ZPL_JSON_ERROR_NONE,
	ZPL_JSON_ERROR_INTERNAL,
	ZPL_JSON_ERROR_INVALID_NAME,
	ZPL_JSON_ERROR_INVALID_VALUE,
	ZPL_JSON_ERROR_INVALID_ASSIGNMENT,
	ZPL_JSON_ERROR_UNKNOWN_KEYWORD,
	ZPL_JSON_ERROR_ARRAY_LEFT_OPEN,
	ZPL_JSON_ERROR_OBJECT_END_PAIR_MISMATCHED,
	ZPL_JSON_ERROR_OUT_OF_MEMORY,
} json_error;

typedef adt_node json_object;

ZPL_DEF u8     json_parse( json_object* root, char* text, AllocatorInfo allocator );
ZPL_DEF void   json_free( json_object* obj );
ZPL_DEF b8     json_write( FileInfo* file, json_object* obj, sw indent );
ZPL_DEF String json_write_string( AllocatorInfo a, json_object* obj, sw indent );

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

#	endif

#	if defined( ZPL_MODULE_THREADING )
#		if defined( ZPL_SYSTEM_UNIX ) || defined( ZPL_SYSTEM_MACOS )
#			include <pthread.h>
#		endif

#		if ! defined( thread_local )
#			if defined( __cplusplus ) && __cplusplus >= 201103L
#				define thread_local thread_local
#			elif defined( _MSC_VER ) && _MSC_VER >= 1300
#				define thread_local __declspec( thread )
#			elif defined( __GNUC__ )
#				define thread_local __thread
#			elif defined( __TINYC__ )
#				define thread_local
#			else
#				define thread_local thread_local
#			endif
#		endif

// file: header/threading/affinity.h


ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

#if defined( ZPL_SYSTEM_WINDOWS ) || defined( ZPL_SYSTEM_CYGWIN )

typedef struct affinity
{
	b32 is_accurate;
	sw  core_count;
	sw  thread_count;

#	define ZPL_WIN32_MAX_THREADS ( 8 * size_of( ZPL_NS( uw ) ) )
	uw core_masks[ ZPL_WIN32_MAX_THREADS ];
} affinity;

#elif defined( ZPL_SYSTEM_OSX )

typedef struct affinity
{
	b32 is_accurate;
	sw  core_count;
	sw  thread_count;
	sw  threads_per_core;
} affinity;

#elif defined( ZPL_SYSTEM_LINUX ) || defined( ZPL_SYSTEM_FREEBSD ) || defined( ZPL_SYSTEM_EMSCRIPTEN ) || defined( ZPL_SYSTEM_OPENBSD )

typedef struct affinity
{
	b32 is_accurate;
	sw  core_count;
	sw  thread_count;
	sw  threads_per_core;
} affinity;

#else
#	error TODO: Unknown system
#endif

ZPL_DEF void affinity_init( affinity* a );
ZPL_DEF void affinity_destroy( affinity* a );
ZPL_DEF b32  affinity_set( affinity* a, sw core, sw thread );
ZPL_DEF sw   affinity_thread_count_for_core( affinity* a, sw core );

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: header/threading/atomic.h

// Atomics

// TODO: Be specific with memory order?
// e.g. relaxed, acquire, release, acquire_release

#if ! defined( __STDC_NO_ATOMICS__ ) && ! defined( __cplusplus ) && ! defined( ZPL_COMPILER_MSVC ) && ! defined( ZPL_COMPILER_TINYC )
#	define atomic( X ) volatile _Atomic( X )
#else
// TODO: Fix once C++ guys bother to add C atomics to std.
// # include <atomic>
#	define atomic( X ) volatile X /*std::atomic<X>*/
#endif

#if defined( __STDC_NO_ATOMICS__ ) || defined( __cplusplus ) || defined( ZPL_COMPILER_MSVC )
#	define atomicarg( X ) volatile X
#else
#	define atomicarg( X ) X
#endif

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

#if defined( ZPL_COMPILER_MSVC )
typedef struct atomic32
{
	atomic( s32 ) value;
} atomic32;

typedef struct atomic64
{
	atomic( s64 ) value;
} atomic64;

typedef struct atomic_ptr
{
	atomic( void* ) value;
} atomic_ptr;
#else
#	if defined( ZPL_ARCH_32_BIT )
#		define ZPL_ATOMIC_PTR_ALIGNMENT 4
#	elif defined( ZPL_ARCH_64_BIT )
#		define ZPL_ATOMIC_PTR_ALIGNMENT 8
#	else
#		error Unknown architecture
#	endif

typedef struct atomic32
{
	atomic( s32 ) value;
} __attribute__( ( aligned( 4 ) ) ) atomic32;

typedef struct atomic64
{
	atomic( s64 ) value;
} __attribute__( ( aligned( 8 ) ) ) atomic64;

typedef struct atomic_ptr
{
	atomic( void* ) value;
} __attribute__( ( aligned( ZPL_ATOMIC_PTR_ALIGNMENT ) ) ) atomic_ptr;
#endif

ZPL_DEF s32  atomic32_load( atomic32 const* a );
ZPL_DEF void atomic32_store( atomic32* a, atomicarg( s32 ) value );
ZPL_DEF s32  atomic32_compare_exchange( atomic32* a, atomicarg( s32 ) expected, atomicarg( s32 ) desired );
ZPL_DEF s32  atomic32_exchange( atomic32* a, atomicarg( s32 ) desired );
ZPL_DEF s32  atomic32_fetch_add( atomic32* a, atomicarg( s32 ) operand );
ZPL_DEF s32  atomic32_fetch_and( atomic32* a, atomicarg( s32 ) operand );
ZPL_DEF s32  atomic32_fetch_or( atomic32* a, atomicarg( s32 ) operand );
ZPL_DEF b32  atomic32_spin_lock( atomic32* a, sw time_out );    // NOTE: time_out = -1 as default
ZPL_DEF void atomic32_spin_unlock( atomic32* a );
ZPL_DEF b32  atomic32_try_acquire_lock( atomic32* a );


ZPL_DEF s64  atomic64_load( atomic64 const* a );
ZPL_DEF void atomic64_store( atomic64* a, atomicarg( s64 ) value );
ZPL_DEF s64  atomic64_compare_exchange( atomic64* a, atomicarg( s64 ) expected, atomicarg( s64 ) desired );
ZPL_DEF s64  atomic64_exchange( atomic64* a, atomicarg( s64 ) desired );
ZPL_DEF s64  atomic64_fetch_add( atomic64* a, atomicarg( s64 ) operand );
ZPL_DEF s64  atomic64_fetch_and( atomic64* a, atomicarg( s64 ) operand );
ZPL_DEF s64  atomic64_fetch_or( atomic64* a, atomicarg( s64 ) operand );
ZPL_DEF b32  atomic64_spin_lock( atomic64* a, sw time_out );    // NOTE: time_out = -1 as default
ZPL_DEF void atomic64_spin_unlock( atomic64* a );
ZPL_DEF b32  atomic64_try_acquire_lock( atomic64* a );


ZPL_DEF void* atomic_ptr_load( atomic_ptr const* a );
ZPL_DEF void  atomic_ptr_store( atomic_ptr* a, atomicarg( void* ) value );
ZPL_DEF void* atomic_ptr_compare_exchange( atomic_ptr* a, atomicarg( void* ) expected, atomicarg( void* ) desired );
ZPL_DEF void* atomic_ptr_exchange( atomic_ptr* a, atomicarg( void* ) desired );
ZPL_DEF void* atomic_ptr_fetch_add( atomic_ptr* a, atomicarg( void* ) operand );
ZPL_DEF void* atomic_ptr_fetch_and( atomic_ptr* a, atomicarg( void* ) operand );
ZPL_DEF void* atomic_ptr_fetch_or( atomic_ptr* a, atomicarg( void* ) operand );
ZPL_DEF b32   atomic_ptr_spin_lock( atomic_ptr* a, sw time_out );    // NOTE: time_out = -1 as default
ZPL_DEF void  atomic_ptr_spin_unlock( atomic_ptr* a );
ZPL_DEF b32   atomic_ptr_try_acquire_lock( atomic_ptr* a );

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: header/threading/fence.h

// Fences

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

ZPL_DEF void yield_thread( void );
ZPL_DEF void mfence( void );
ZPL_DEF void sfence( void );
ZPL_DEF void lfence( void );

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: header/threading/mutex.h


ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

typedef struct mutex
{
#if defined( ZPL_SYSTEM_WINDOWS )
	u64 win32_critical_section[ sizeof( uw ) / 2 + 1 ];
#else
	pthread_mutex_t pthread_mutex;
#endif
} mutex;

ZPL_DEF void mutex_init( mutex* m );
ZPL_DEF void mutex_destroy( mutex* m );
ZPL_DEF void mutex_lock( mutex* m );
ZPL_DEF b32  mutex_try_lock( mutex* m );
ZPL_DEF void mutex_unlock( mutex* m );

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: header/threading/sem.h


ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

#if defined( ZPL_SYSTEM_MACOS )
#	include <mach/thread_act.h>
#elif defined( ZPL_SYSTEM_UNIX )
#	include <semaphore.h>
#endif

#if defined( ZPL_SYSTEM_WINDOWS )
typedef struct semaphore
{
	void* win32_handle;
} semaphore;
#elif defined( ZPL_SYSTEM_MACOS )
typedef struct semaphore
{
	semaphore_t osx_handle;
} semaphore;
#elif defined( ZPL_SYSTEM_UNIX )
typedef struct semaphore
{
	sem_t unix_handle;
} semaphore;
#else
#	error
#endif

ZPL_DEF void semaphore_init( semaphore* s );
ZPL_DEF void semaphore_destroy( semaphore* s );
ZPL_DEF void semaphore_post( semaphore* s, s32 count );
ZPL_DEF void semaphore_release( semaphore* s );    // NOTE: zpl_semaphore_post(s, 1)
ZPL_DEF void semaphore_wait( semaphore* s );
ZPL_DEF s32  semaphore_trywait( semaphore* s );

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: header/threading/sync.h

// NOTE: Thread Merge Operation
// Based on Sean Barrett's stb_sync

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

typedef struct sync
{
	s32 target;     // Target Number of threads
	s32 current;    // Threads to hit
	s32 waiting;    // Threads waiting

	mutex     start;
	mutex     mutex;
	semaphore release;
} sync;

ZPL_DEF void sync_init( sync* s );
ZPL_DEF void sync_destroy( sync* s );
ZPL_DEF void sync_set_target( sync* s, s32 count );
ZPL_DEF void sync_release( sync* s );
ZPL_DEF s32  sync_reach( sync* s );
ZPL_DEF void sync_reach_and_wait( sync* s );

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: header/threading/thread.h

#ifdef ZPL_EDITOR
#	include <zpl.h>
#else
struct thread;
#endif

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

typedef sw ( *thread_proc )( struct thread* thread );

typedef struct thread
{
#if defined( ZPL_SYSTEM_WINDOWS )
	void* win32_handle;
#else
	pthread_t posix_handle;
#endif

	thread_proc proc;
	void*       user_data;
	sw          user_index;
	sw          return_value;

	semaphore semaphore;
	sw        stack_size;
	b32       is_running;
	b32       nowait;
} thread;

ZPL_DEF void thread_init( thread* t );
ZPL_DEF void thread_init_nowait( thread* t );
ZPL_DEF void thread_destroy( thread* t );
ZPL_DEF void thread_start( thread* t, thread_proc proc, void* data );
ZPL_DEF void thread_start_with_stack( thread* t, thread_proc proc, void* data, sw stack_size );
ZPL_DEF void thread_join( thread* t );
ZPL_DEF b32  thread_is_running( thread const* t );
ZPL_DEF u32  thread_current_id( void );
ZPL_DEF void thread_set_name( thread* t, char const* name );

ZPL_END_C_DECLS
ZPL_END_NAMESPACE


#		if defined( ZPL_MODULE_JOBS )
// file: header/jobs.h

/** @file threadpool.c
@brief Job system
@defgroup jobs Job system

 This job system follows thread pool pattern to minimize the costs of thread initialization.
 It reuses fixed number of threads to process variable number of jobs.

 @{
 */

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

typedef void ( *jobs_proc )( void* data );

#define ZPL_INVALID_JOB ZPL_U32_MAX

#ifndef ZPL_JOBS_MAX_QUEUE
#	define ZPL_JOBS_MAX_QUEUE 100
#endif

#ifdef ZPL_JOBS_ENABLE_DEBUG
#	define ZPL_JOBS_DEBUG
#endif

typedef enum
{
	ZPL_JOBS_STATUS_READY,
	ZPL_JOBS_STATUS_BUSY,
	ZPL_JOBS_STATUS_WAITING,
	ZPL_JOBS_STATUS_TERM,
} jobs_status;

typedef enum
{
	ZPL_JOBS_PRIORITY_REALTIME,
	ZPL_JOBS_PRIORITY_HIGH,
	ZPL_JOBS_PRIORITY_NORMAL,
	ZPL_JOBS_PRIORITY_LOW,
	ZPL_JOBS_PRIORITY_IDLE,
	ZPL_JOBS_MAX_PRIORITIES,
} jobs_priority;

typedef struct
{
	jobs_proc proc;
	void*     data;
} thread_job;

ZPL_RING_DECLARE( extern, _jobs_ring_, thread_job );

typedef struct
{
	thread     thread;
	atomic32   status;
	thread_job job;
#ifdef ZPL_JOBS_DEBUG
	u32 hits;
	u32 idle;
#endif
} thread_worker;

typedef struct
{
	_jobs_ring_zpl_thread_job jobs;    ///< zpl_ring
	u32                       chance;
#ifdef ZPL_JOBS_DEBUG
	u32 hits;
#endif
} thread_queue;

typedef struct
{
	AllocatorInfo  alloc;
	u32            max_threads, max_jobs, counter;
	thread_worker* workers;    ///< zpl_buffer
	thread_queue   queues[ ZPL_JOBS_MAX_PRIORITIES ];
} jobs_system;

//! Initialize thread pool with specified amount of fixed threads.
ZPL_DEF void jobs_init( jobs_system* pool, AllocatorInfo a, u32 max_threads );

//! Initialize thread pool with specified amount of fixed threads and custom job limit.
ZPL_DEF void jobs_init_with_limit( jobs_system* pool, AllocatorInfo a, u32 max_threads, u32 max_jobs );

//! Release the resources use by thread pool.
ZPL_DEF void jobs_free( jobs_system* pool );

//! Enqueue a job with specified data and custom priority.
ZPL_DEF b32 jobs_enqueue_with_priority( jobs_system* pool, jobs_proc proc, void* data, jobs_priority priority );

//! Enqueue a job with specified data.
ZPL_DEF b32 jobs_enqueue( jobs_system* pool, jobs_proc proc, void* data );

//! Check if the work queue is empty.
ZPL_DEF b32 jobs_empty( jobs_system* pool, jobs_priority priority );

ZPL_DEF b32 jobs_empty_all( jobs_system* pool );
ZPL_DEF b32 jobs_full_all( jobs_system* pool );

//! Check if the work queue is full.
ZPL_DEF b32 jobs_full( jobs_system* pool, jobs_priority priority );

//! Check if all workers are done.
ZPL_DEF b32 jobs_done( jobs_system* pool );

//! Process all jobs and check all threads. Should be called by Main Thread in a tight loop.
ZPL_DEF b32 jobs_process( jobs_system* pool );

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

#		endif
#	else
#		if ! defined( thread_local )
#			if defined( __cplusplus ) && __cplusplus >= 201103L
#				define thread_local thread_local
#			else
#				define thread_local
#		endif
#	endif

#	if defined( ZPL_COMPILER_MSVC )
#		pragma warning( pop )
#	endif

#	if defined( __GCC__ ) || defined( __GNUC__ ) || defined( __clang__ )
#		pragma GCC diagnostic pop
#	endif

#	if defined( ZPL_IMPLEMENTATION ) && ! defined( ZPL_IMPLEMENTATION_DONE )
#		define ZPL_IMPLEMENTATION_DONE

#		if defined( __GCC__ ) || defined( __GNUC__ ) || defined( __clang__ )
#			pragma GCC diagnostic push
#			pragma GCC diagnostic ignored "-Wattributes"
#			pragma GCC diagnostic ignored "-Wunused-value"
#			pragma GCC diagnostic ignored "-Wunused-function"
#			pragma GCC diagnostic ignored "-Wwrite-strings"
#			pragma GCC diagnostic ignored "-Wunused-parameter"
#			pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#			pragma GCC diagnostic ignored "-Wmissing-braces"
#			pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#			pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#			pragma GCC diagnostic ignored "-Wignored-qualifiers"
#		endif

#		if defined( _MSC_VER )
#			pragma warning( push )
#			pragma warning( disable : 4201 )
#			pragma warning( disable : 4996 )    // Disable deprecated POSIX functions warning
#			pragma warning( disable : 4127 )    // Conditional expression is constant
#			pragma warning( disable : 4204 )    // non-constant field initializer
#			pragma warning( disable : 4756 )    // -INFINITY
#			pragma warning( disable : 4056 )    // -INFINITY
#			pragma warning( disable : 4702 )    // unreachable code
#		endif

/* general purpose includes */

#		include <stdio.h>

// NOTE: Ensure we use standard methods for these calls if we use ZPL_PICO
#		if ! defined( ZPL_PICO_CUSTOM_ROUTINES )
#			if ! defined( ZPL_MODULE_CORE )
#				define _strlen                   strlen
#				define _printf_err( fmt, ... )   fprintf( stderr, fmt, __VA_ARGS__ )
#				define _printf_err_va( fmt, va ) vfprintf( stderr, fmt, va )
#			else
#				define _strlen                   strlen
#				define _printf_err( fmt, ... )   printf_err( fmt, __VA_ARGS__ )
#				define _printf_err_va( fmt, va ) printf_err_va( fmt, va )
#			endif
#		endif

#		include <errno.h>

#		if defined( ZPL_SYSTEM_UNIX ) || defined( ZPL_SYSTEM_MACOS )
#			include <unistd.h>
#		elif defined( ZPL_SYSTEM_WINDOWS )
#			if ! defined( ZPL_NO_WINDOWS_H )
#				ifndef WIN32_LEAN_AND_MEAN
#					ifndef NOMINMAX
#						define NOMINMAX
#					endif

#					define WIN32_LEAN_AND_MEAN
#					define WIN32_MEAN_AND_LEAN
#					define VC_EXTRALEAN
#				endif
#				include <windows.h>
#				undef NOMINMAX
#				undef WIN32_LEAN_AND_MEAN
#				undef WIN32_MEAN_AND_LEAN
#				undef VC_EXTRALEAN
#			endif
#		endif

#		if defined( ZPL_MODULE_ESSENTIALS )
// file: source/essentials/debug.c


ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

void assert_handler( char const* condition, char const* file, s32 line, char const* msg, ... )
{
	_printf_err( "%s:(%d): Assert Failure: ", file, line );

	if ( condition )
		_printf_err( "`%s` ", condition );

	if ( msg )
	{
		va_list va;
		va_start( va, msg );
		_printf_err_va( msg, va );
		va_end( va );
	}

	_printf_err( "%s", "\n" );
}

s32 assert_crash( char const* condition )
{
	ZPL_PANIC( condition );
	return 0;
}

#if defined( ZPL_SYSTEM_UNIX ) || defined( ZPL_SYSTEM_MACOS )
#	include <sched.h>
#endif

#if defined( ZPL_SYSTEM_WINDOWS )
void exit( u32 code )
{
	ExitProcess( code );
}
#else
#	include <stdlib.h>

void exit( u32 code )
{
	exit( code );
}
#endif

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: source/essentials/memory.c


#include <string.h>

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

void memswap( void* i, void* j, sw size )
{
	if ( i == j )
		return;

	if ( size == 4 )
	{
		swap( u32, *zpl_cast( u32* ) i, *zpl_cast( u32* ) j );
	}
	else if ( size == 8 )
	{
		swap( u64, *zpl_cast( u64* ) i, *zpl_cast( u64* ) j );
	}
	else if ( size < 8 )
	{
		u8* a = zpl_cast( u8* ) i;
		u8* b = zpl_cast( u8* ) j;
		if ( a != b )
		{
			while ( size-- )
			{
				swap( u8, *a++, *b++ );
			}
		}
	}
	else
	{
		char buffer[ 256 ];

		while ( size > size_of( buffer ) )
		{
			memswap( i, j, size_of( buffer ) );
			i     = pointer_add( i, size_of( buffer ) );
			j     = pointer_add( j, size_of( buffer ) );
			size -= size_of( buffer );
		}

		memcopy( buffer, i, size );
		memcopy( i, j, size );
		memcopy( j, buffer, size );
	}
}

void const* memchr( void const* data, u8 c, sw n )
{
	u8 const* s = zpl_cast( u8 const* ) data;
	while ( ( zpl_cast( uptr ) s & ( sizeof( uw ) - 1 ) ) && n && *s != c )
	{
		s++;
		n--;
	}
	if ( n && *s != c )
	{
		sw const* w;
		sw        k = ZPL__ONES * c;
		w           = zpl_cast( sw const* ) s;
		while ( n >= size_of( sw ) && ! ZPL__HAS_ZERO( *w ^ k ) )
		{
			w++;
			n -= size_of( sw );
		}
		s = zpl_cast( u8 const* ) w;
		while ( n && *s != c )
		{
			s++;
			n--;
		}
	}

	return n ? zpl_cast( void const* ) s : NULL;
}

void const* memrchr( void const* data, u8 c, sw n )
{
	u8 const* s = zpl_cast( u8 const* ) data;
	while ( n-- )
	{
		if ( s[ n ] == c )
			return zpl_cast( void const* )( s + n );
	}
	return NULL;
}

void* memcopy( void* dest, void const* source, sw n )
{
	if ( dest == NULL )
	{
		return NULL;
	}

	return memcpy( dest, source, n );

	// TODO: Re-work the whole method
#if 0
#	if defined( _MSC_VER )
    __movsb(zpl_cast(u8 *) dest, zpl_cast(u8 *) source, n);
#	elif defined( ZPL_CPU_X86 ) && ! defined( ZPL_SYSTEM_EMSCRIPTEN )
    u8 *__dest8 = zpl_cast(u8 *) dest;
    u8 *__source8 = zpl_cast(u8 *) source;
    __asm__ __volatile__("rep movsb" : "+D"(__dest8), "+S"(__source8), "+c"(n) : : "memory");
#	elif defined( ZPL_CPU_ARM )
    return memcpy(dest, source, n);
#	else
    u8 *d = zpl_cast(u8 *) dest;
    u8 const *s = zpl_cast(u8 const *) source;
    u32 w, x;

    for (; zpl_cast(uptr) s % 4 && n; n--) *d++ = *s++;

    if (zpl_cast(uptr) d % 4 == 0) {
        for (; n >= 16; s += 16, d += 16, n -= 16) {
            *zpl_cast(u32 *)(d + 0) = *zpl_cast(u32 *)(s + 0);
            *zpl_cast(u32 *)(d + 4) = *zpl_cast(u32 *)(s + 4);
            *zpl_cast(u32 *)(d + 8) = *zpl_cast(u32 *)(s + 8);
            *zpl_cast(u32 *)(d + 12) = *zpl_cast(u32 *)(s + 12);
        }
        if (n & 8) {
            *zpl_cast(u32 *)(d + 0) = *zpl_cast(u32 *)(s + 0);
            *zpl_cast(u32 *)(d + 4) = *zpl_cast(u32 *)(s + 4);
            d += 8;
            s += 8;
        }
        if (n & 4) {
            *zpl_cast(u32 *)(d + 0) = *zpl_cast(u32 *)(s + 0);
            d += 4;
            s += 4;
        }
        if (n & 2) {
            *d++ = *s++;
            *d++ = *s++;
        }
        if (n & 1) { *d = *s; }
        return dest;
    }

    if (n >= 32) {
#		if __BYTE_ORDER == __BIG_ENDIAN
#			define LS <<
#			define RS >>
#		else
#			define LS >>
#			define RS <<
#		endif
        switch (zpl_cast(uptr) d % 4) {
            case 1: {
                w = *zpl_cast(u32 *) s;
                *d++ = *s++;
                *d++ = *s++;
                *d++ = *s++;
                n -= 3;
                while (n > 16) {
                    x = *zpl_cast(u32 *)(s + 1);
                    *zpl_cast(u32 *)(d + 0) = (w LS 24) | (x RS 8);
                    w = *zpl_cast(u32 *)(s + 5);
                    *zpl_cast(u32 *)(d + 4) = (x LS 24) | (w RS 8);
                    x = *zpl_cast(u32 *)(s + 9);
                    *zpl_cast(u32 *)(d + 8) = (w LS 24) | (x RS 8);
                    w = *zpl_cast(u32 *)(s + 13);
                    *zpl_cast(u32 *)(d + 12) = (x LS 24) | (w RS 8);

                    s += 16;
                    d += 16;
                    n -= 16;
                }
            } break;
            case 2: {
                w = *zpl_cast(u32 *) s;
                *d++ = *s++;
                *d++ = *s++;
                n -= 2;
                while (n > 17) {
                    x = *zpl_cast(u32 *)(s + 2);
                    *zpl_cast(u32 *)(d + 0) = (w LS 16) | (x RS 16);
                    w = *zpl_cast(u32 *)(s + 6);
                    *zpl_cast(u32 *)(d + 4) = (x LS 16) | (w RS 16);
                    x = *zpl_cast(u32 *)(s + 10);
                    *zpl_cast(u32 *)(d + 8) = (w LS 16) | (x RS 16);
                    w = *zpl_cast(u32 *)(s + 14);
                    *zpl_cast(u32 *)(d + 12) = (x LS 16) | (w RS 16);

                    s += 16;
                    d += 16;
                    n -= 16;
                }
            } break;
            case 3: {
                w = *zpl_cast(u32 *) s;
                *d++ = *s++;
                n -= 1;
                while (n > 18) {
                    x = *zpl_cast(u32 *)(s + 3);
                    *zpl_cast(u32 *)(d + 0) = (w LS 8) | (x RS 24);
                    w = *zpl_cast(u32 *)(s + 7);
                    *zpl_cast(u32 *)(d + 4) = (x LS 8) | (w RS 24);
                    x = *zpl_cast(u32 *)(s + 11);
                    *zpl_cast(u32 *)(d + 8) = (w LS 8) | (x RS 24);
                    w = *zpl_cast(u32 *)(s + 15);
                    *zpl_cast(u32 *)(d + 12) = (x LS 8) | (w RS 24);

                    s += 16;
                    d += 16;
                    n -= 16;
                }
            } break;
            default: break; // NOTE: Do nowt!
        }
#		undef LS
#		undef RS
        if (n & 16) {
            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;
        }
        if (n & 8) {
            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;
        }
        if (n & 4) {
            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;
        }
        if (n & 2) {
            *d++ = *s++;
            *d++ = *s++;
        }
        if (n & 1) { *d = *s; }
    }

#	endif
#endif

	return dest;
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: source/essentials/memory_custom.c


#ifndef _IOSC11_SOURCE
#	define _IOSC11_SOURCE
#endif

#include <stdlib.h>

#if defined( ZPL_SYSTEM_WINDOWS )
#	include <malloc.h>
#endif

// include errno.h for MinGW
#if defined( ZPL_COMPILER_GCC ) || ( defined( ZPL_COMPILER_TINYC ) && defined( ZPL_SYSTEM_WINDOWS ) )
#	include <errno.h>
#endif

#if defined( ZPL_COMPILER_MINGW )
#	ifdef __MINGW32__
#		define _aligned_malloc __mingw_aligned_malloc
#		define _aligned_free   __mingw_aligned_free
#	endif    // MINGW
#endif

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

char* alloc_str( AllocatorInfo a, char const* str )
{
	return alloc_str_len( a, str, _strlen( str ) );
}

////////////////////////////////////////////////////////////////
//
// Custom Allocation
//
//

//
// Heap Allocator
//

#define ZPL_HEAP_STATS_MAGIC 0xDEADC0DE

typedef struct _heap_stats
{
	u32 magic;
	sw  used_memory;
	sw  alloc_count;
} _heap_stats;

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

typedef struct _heap_alloc_info
{
	sw    size;
	void* physical_start;
} _heap_alloc_info;

ZPL_ALLOCATOR_PROC( heap_allocator_proc )
{
	void* ptr = NULL;
	unused( allocator_data );
	unused( old_size );
	if ( ! alignment )
		alignment = ZPL_DEFAULT_MEMORY_ALIGNMENT;

#ifdef ZPL_HEAP_ANALYSIS
	sw alloc_info_size      = size_of( _heap_alloc_info );
	sw alloc_info_remainder = ( alloc_info_size % alignment );
	sw track_size           = max( alloc_info_size, alignment ) + alloc_info_remainder;
	switch ( type )
	{
		case ZPL_ALLOCATION_FREE :
			{
				if ( ! old_memory )
					break;
				_heap_alloc_info* alloc_info  = zpl_cast( _heap_alloc_info* ) old_memory - 1;
				_heap_stats_info.used_memory -= alloc_info->size;
				_heap_stats_info.alloc_count--;
				old_memory = alloc_info->physical_start;
			}
			break;
		case ZPL_ALLOCATION_ALLOC :
			{
				size += track_size;
			}
			break;
		default :
			break;
	}
#endif

	switch ( type )
	{
#if defined( ZPL_COMPILER_MSVC ) || ( defined( ZPL_COMPILER_GCC ) && defined( ZPL_SYSTEM_WINDOWS ) ) || ( defined( ZPL_COMPILER_TINYC ) && defined( ZPL_SYSTEM_WINDOWS ) )
		case ZPL_ALLOCATION_ALLOC :
			ptr = _aligned_malloc( size, alignment );
			if ( flags & ZPL_ALLOCATOR_FLAG_CLEAR_TO_ZERO )
				zero_size( ptr, size );
			break;
		case ZPL_ALLOCATION_FREE :
			_aligned_free( old_memory );
			break;
		case ZPL_ALLOCATION_RESIZE :
			{
				AllocatorInfo a = heap_allocator();
				ptr             = default_resize_align( a, old_memory, old_size, size, alignment );
			}
			break;

#elif defined( ZPL_SYSTEM_LINUX ) && ! defined( ZPL_CPU_ARM ) && ! defined( ZPL_COMPILER_TINYC )
		case ZPL_ALLOCATION_ALLOC :
			{
				ptr = aligned_alloc( alignment, ( size + alignment - 1 ) & ~( alignment - 1 ) );

				if ( flags & ZPL_ALLOCATOR_FLAG_CLEAR_TO_ZERO )
				{
					zero_size( ptr, size );
				}
			}
			break;

		case ZPL_ALLOCATION_FREE :
			{
				free( old_memory );
			}
			break;

		case ZPL_ALLOCATION_RESIZE :
			{
				AllocatorInfo a = heap_allocator();
				ptr             = default_resize_align( a, old_memory, old_size, size, alignment );
			}
			break;
#else
		case ZPL_ALLOCATION_ALLOC :
			{
				posix_memalign( &ptr, alignment, size );

				if ( flags & ZPL_ALLOCATOR_FLAG_CLEAR_TO_ZERO )
				{
					zero_size( ptr, size );
				}
			}
			break;

		case ZPL_ALLOCATION_FREE :
			{
				free( old_memory );
			}
			break;

		case ZPL_ALLOCATION_RESIZE :
			{
				AllocatorInfo a = heap_allocator();
				ptr             = default_resize_align( a, old_memory, old_size, size, alignment );
			}
			break;
#endif

		case ZPL_ALLOCATION_FREE_ALL :
			break;
	}

#ifdef ZPL_HEAP_ANALYSIS
	if ( type == ZPL_ALLOCATION_ALLOC )
	{
		_heap_alloc_info* alloc_info = zpl_cast( _heap_alloc_info* )( zpl_cast( char* ) ptr + alloc_info_remainder );
		zero_item( alloc_info );
		alloc_info->size              = size - track_size;
		alloc_info->physical_start    = ptr;
		ptr                           = zpl_cast( void* )( alloc_info + 1 );
		_heap_stats_info.used_memory += alloc_info->size;
		_heap_stats_info.alloc_count++;
	}
#endif

	return ptr;
}

//
// Arena Allocator
//

ZPL_ALLOCATOR_PROC( arena_allocator_proc )
{
	Arena* arena = zpl_cast( Arena* ) allocator_data;
	void*  ptr   = NULL;

	unused( old_size );

	switch ( type )
	{
		case ZPL_ALLOCATION_ALLOC :
			{
				void* end        = pointer_add( arena->physical_start, arena->total_allocated );
				sw    total_size = align_forward_i64( size, alignment );

				// NOTE: Out of memory
				if ( arena->total_allocated + total_size > zpl_cast( sw ) arena->total_size )
				{
					// zpl__printf_err("%s", "Arena out of memory\n");
					return NULL;
				}

				ptr                     = align_forward( end, alignment );
				arena->total_allocated += total_size;
				if ( flags & ZPL_ALLOCATOR_FLAG_CLEAR_TO_ZERO )
					zero_size( ptr, size );
			}
			break;

		case ZPL_ALLOCATION_FREE :
			// NOTE: Free all at once
			// Use Temp_Arena_Memory if you want to free a block
			break;

		case ZPL_ALLOCATION_FREE_ALL :
			arena->total_allocated = 0;
			break;

		case ZPL_ALLOCATION_RESIZE :
			{
				// TODO: Check if ptr is on top of stack and just extend
				AllocatorInfo a = arena_allocator( arena );
				ptr             = default_resize_align( a, old_memory, old_size, size, alignment );
			}
			break;
	}
	return ptr;
}

//
// Pool Allocator
//

void pool_init_align( Pool* pool, AllocatorInfo backing, sw num_blocks, sw block_size, sw block_align )
{
	sw    actual_block_size, pool_size, block_index;
	void *data, *curr;
	uptr* end;

	zero_item( pool );

	pool->backing     = backing;
	pool->block_size  = block_size;
	pool->block_align = block_align;
	pool->num_blocks  = num_blocks;

	actual_block_size = block_size + block_align;
	pool_size         = num_blocks * actual_block_size;

	data = alloc_align( backing, pool_size, block_align );

	// NOTE: Init intrusive freelist
	curr = data;
	for ( block_index = 0; block_index < num_blocks - 1; block_index++ )
	{
		uptr* next = zpl_cast( uptr* ) curr;
		*next      = zpl_cast( uptr ) curr + actual_block_size;
		curr       = pointer_add( curr, actual_block_size );
	}

	end  = zpl_cast( uptr* ) curr;
	*end = zpl_cast( uptr ) NULL;

	pool->physical_start = data;
	pool->free_list      = data;
}

ZPL_ALLOCATOR_PROC( pool_allocator_proc )
{
	Pool* pool = zpl_cast( Pool* ) allocator_data;
	void* ptr  = NULL;

	unused( old_size );

	switch ( type )
	{
		case ZPL_ALLOCATION_ALLOC :
			{
				uptr next_free;
				ZPL_ASSERT( size == pool->block_size );
				ZPL_ASSERT( alignment == pool->block_align );
				ZPL_ASSERT( pool->free_list != NULL );

				next_free         = *zpl_cast( uptr* ) pool->free_list;
				ptr               = pool->free_list;
				pool->free_list   = zpl_cast( void* ) next_free;
				pool->total_size += pool->block_size;
				if ( flags & ZPL_ALLOCATOR_FLAG_CLEAR_TO_ZERO )
					zero_size( ptr, size );
			}
			break;

		case ZPL_ALLOCATION_FREE :
			{
				uptr* next;
				if ( old_memory == NULL )
					return NULL;

				next              = zpl_cast( uptr* ) old_memory;
				*next             = zpl_cast( uptr ) pool->free_list;
				pool->free_list   = old_memory;
				pool->total_size -= pool->block_size;
			}
			break;

		case ZPL_ALLOCATION_FREE_ALL :
			{
				sw    actual_block_size, block_index;
				void* curr;
				uptr* end;

				actual_block_size = pool->block_size + pool->block_align;
				pool->total_size  = 0;

				// NOTE: Init intrusive freelist
				curr = pool->physical_start;
				for ( block_index = 0; block_index < pool->num_blocks - 1; block_index++ )
				{
					uptr* next = zpl_cast( uptr* ) curr;
					*next      = zpl_cast( uptr ) curr + actual_block_size;
					curr       = pointer_add( curr, actual_block_size );
				}

				end             = zpl_cast( uptr* ) curr;
				*end            = zpl_cast( uptr ) NULL;
				pool->free_list = pool->physical_start;
			}
			break;

		case ZPL_ALLOCATION_RESIZE :
			// NOTE: Cannot resize
			ZPL_PANIC( "You cannot resize something allocated by with a pool." );
			break;
	}

	return ptr;
}

//
// Scratch Memory Allocator
//

void scratch_memory_init( scratch_memory* s, void* start, sw size )
{
	s->physical_start = start;
	s->total_size     = size;
	s->alloc_point    = start;
	s->free_point     = start;
}

b32 scratch_memory_is_in_use( scratch_memory* s, void* ptr )
{
	if ( s->free_point == s->alloc_point )
		return false;
	if ( s->alloc_point > s->free_point )
		return ptr >= s->free_point && ptr < s->alloc_point;
	return ptr >= s->free_point || ptr < s->alloc_point;
}

AllocatorInfo scratch_allocator( scratch_memory* s )
{
	AllocatorInfo a;
	a.proc = scratch_allocator_proc;
	a.data = s;
	return a;
}

ZPL_ALLOCATOR_PROC( scratch_allocator_proc )
{
	scratch_memory* s   = zpl_cast( scratch_memory* ) allocator_data;
	void*           ptr = NULL;
	ZPL_ASSERT_NOT_NULL( s );

	switch ( type )
	{
		case ZPL_ALLOCATION_ALLOC :
			{
				void*                 pt     = s->alloc_point;
				allocation_header_ev* header = zpl_cast( allocation_header_ev* ) pt;
				void*                 data   = align_forward( header + 1, alignment );
				void*                 end    = pointer_add( s->physical_start, s->total_size );

				ZPL_ASSERT( alignment % 4 == 0 );
				size = ( ( size + 3 ) / 4 ) * 4;
				pt   = pointer_add( pt, size );

				// NOTE: Wrap around
				if ( pt > end )
				{
					header->size = pointer_diff( header, end ) | ZPL_ISIZE_HIGH_BIT;
					pt           = s->physical_start;
					header       = zpl_cast( allocation_header_ev* ) pt;
					data         = align_forward( header + 1, alignment );
					pt           = pointer_add( pt, size );
				}

				if ( ! scratch_memory_is_in_use( s, pt ) )
				{
					allocation_header_fill( header, pt, pointer_diff( header, pt ) );
					s->alloc_point = zpl_cast( u8* ) pt;
					ptr            = data;
				}

				if ( flags & ZPL_ALLOCATOR_FLAG_CLEAR_TO_ZERO )
					zero_size( ptr, size );
			}
			break;

		case ZPL_ALLOCATION_FREE :
			{
				if ( old_memory )
				{
					void* end = pointer_add( s->physical_start, s->total_size );
					if ( old_memory < s->physical_start || old_memory >= end )
					{
						ZPL_ASSERT( false );
					}
					else
					{
						// NOTE: Mark as free
						allocation_header_ev* h = allocation_header( old_memory );
						ZPL_ASSERT( ( h->size & ZPL_ISIZE_HIGH_BIT ) == 0 );
						h->size = h->size | ZPL_ISIZE_HIGH_BIT;

						while ( s->free_point != s->alloc_point )
						{
							allocation_header_ev* header = zpl_cast( allocation_header_ev* ) s->free_point;
							if ( ( header->size & ZPL_ISIZE_HIGH_BIT ) == 0 )
								break;

							s->free_point = pointer_add( s->free_point, h->size & ( ~ZPL_ISIZE_HIGH_BIT ) );
							if ( s->free_point == end )
								s->free_point = s->physical_start;
						}
					}
				}
			}
			break;

		case ZPL_ALLOCATION_FREE_ALL :
			s->alloc_point = s->physical_start;
			s->free_point  = s->physical_start;
			break;

		case ZPL_ALLOCATION_RESIZE :
			ptr = default_resize_align( scratch_allocator( s ), old_memory, old_size, size, alignment );
			break;
	}

	return ptr;
}

//
// Stack Memory Allocator
//
ZPL_ALLOCATOR_PROC( stack_allocator_proc )
{
	stack_memory* s   = zpl_cast( stack_memory* ) allocator_data;
	void*         ptr = NULL;
	ZPL_ASSERT_NOT_NULL( s );
	unused( old_size );
	unused( flags );

	switch ( type )
	{
		case ZPL_ALLOCATION_ALLOC :
			{
				size             += ZPL_STACK_ALLOC_OFFSET;
				u64 alloc_offset  = s->allocated;

				void* curr = zpl_cast( u64* ) align_forward( zpl_cast( u64* ) pointer_add( s->physical_start, s->allocated ), alignment );

				if ( zpl_cast( u64* ) pointer_add( curr, size ) > zpl_cast( u64* ) pointer_add( s->physical_start, s->total_size ) )
				{
					if ( s->backing.proc )
					{
						void* old_start   = s->physical_start;
						s->physical_start = resize_align( s->backing, s->physical_start, s->total_size, s->total_size + size, alignment );
						curr              = zpl_cast( u64* ) align_forward( zpl_cast( u64* ) pointer_add( s->physical_start, s->allocated ), alignment );
						s->total_size     = pointer_diff( old_start, s->physical_start );
					}
					else
					{
						ZPL_PANIC( "Can not resize stack's memory! Allocator not defined!" );
					}
				}

				s->allocated = pointer_diff( s->physical_start, curr ) + size;

				*( u64* )curr = alloc_offset;
				curr          = pointer_add( curr, ZPL_STACK_ALLOC_OFFSET );

				ptr = curr;
			}
			break;

		case ZPL_ALLOCATION_FREE :
			{
				if ( old_memory )
				{
					void* curr = old_memory;
					curr       = pointer_sub( curr, ZPL_STACK_ALLOC_OFFSET );

					u64 alloc_offset = *( u64* )curr;
					s->allocated     = ( uw )alloc_offset;
				}
			}
			break;

		case ZPL_ALLOCATION_FREE_ALL :
			{
				s->allocated = 0;
			}
			break;

		case ZPL_ALLOCATION_RESIZE :
			{
				ZPL_PANIC( "You cannot resize something allocated by a stack." );
			}
			break;
	}
	return ptr;
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

#			if defined( ZPL_MODULE_CORE )
// file: source/core/file.c


////////////////////////////////////////////////////////////////
//
// File Handling
//
//
#include <sys/stat.h>

#ifdef ZPL_SYSTEM_MACOS
#	include <copyfile.h>
#endif

#ifdef ZPL_SYSTEM_CYGWIN
#	include <windows.h>
#endif

#if defined( ZPL_SYSTEM_WINDOWS ) && ! defined( ZPL_COMPILER_GCC )
#	include <io.h>
#endif

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

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
	len = strlen( text );
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
	unused( stop_at_newline );
	b32 result = false;
	_win32_file_seek( fd, offset, ZPL_SEEK_WHENCE_BEGIN, NULL );
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
	_win32_file_seek( fd, offset, ZPL_SEEK_WHENCE_BEGIN, NULL );
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

file_operations const default_file_operations = { _win32_file_read, _win32_file_write, _win32_file_seek, _win32_file_close };

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

	w_text = _alloc_utf8_to_ucs2( heap_allocator(), filename, NULL );
	handle = CreateFileW( w_text, desired_access, FILE_SHARE_READ | FILE_SHARE_DELETE, NULL, creation_disposition, FILE_ATTRIBUTE_NORMAL, NULL );

	free( heap_allocator(), w_text );

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
		LARGE_INTEGER offset = { 0 };
		if ( ! SetFilePointerEx( handle, offset, NULL, ZPL_SEEK_WHENCE_END ) )
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
	_posix_file_seek( fd, 0, ZPL_SEEK_WHENCE_CURRENT, &curr_offset );
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

file_operations const default_file_operations = { _posix_file_read, _posix_file_write, _posix_file_seek, _posix_file_close };

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

#endif

FileError file_new( FileInfo* f, file_descriptor fd, file_operations ops, char const* filename )
{
	FileError err = EFileError_NONE;
	sw        len = strlen( filename );

	f->ops             = ops;
	f->fd              = fd;
	f->dir             = NULL;
	f->last_write_time = 0;
	f->filename        = alloc_array( heap_allocator(), char, len + 1 );
	memcopy( zpl_cast( char* ) f->filename, zpl_cast( char* ) filename, len + 1 );

	return err;
}

FileError file_open_mode( FileInfo* f, file_mode mode, char const* filename )
{
	FileInfo file_ = { 0 };
	*f             = file_;
	FileError err;
#if defined( ZPL_SYSTEM_WINDOWS ) || defined( ZPL_SYSTEM_CYGWIN )
	err = _win32_file_open( &f->fd, &f->ops, mode, filename );
#else
	err = _posix_file_open( &f->fd, &f->ops, mode, filename );
#endif
	if ( err == EFileError_NONE )
		return file_new( f, f->fd, f->ops, filename );
	return err;
}

internal void _dirinfo_free_entry( dir_entry* entry );

FileError file_close( FileInfo* f )
{
	if ( ! f )
		return EFileError_INVALID;

	if ( f->filename )
		free( heap_allocator(), zpl_cast( char* ) f->filename );

#if defined( ZPL_SYSTEM_WINDOWS )
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

	if ( f->dir )
	{
		_dirinfo_free_entry( f->dir );
		mfree( f->dir );
		f->dir = NULL;
	}

	return EFileError_NONE;
}

FileError file_create( FileInfo* f, char const* filename )
{
	return file_open_mode( f, EFileMode_WRITE | EFileMode_RW, filename );
}

FileError file_open( FileInfo* f, char const* filename )
{
	return file_open_mode( f, EFileMode_READ, filename );
}

char const* file_name( FileInfo* f )
{
	return f->filename ? f->filename : "";
}

b32 file_has_changed( FileInfo* f )
{
	if ( f->is_temp )
		return false;
	b32       result          = false;
	file_time last_write_time = fs_last_write_time( f->filename );
	if ( f->last_write_time != last_write_time )
	{
		result             = true;
		f->last_write_time = last_write_time;
	}
	return result;
}

// TODO: Is this a bad idea?
global b32      _std_file_set                         = false;
global FileInfo _std_files[ ZPL_FILE_STANDARD_COUNT ] = { { 0 } };

#if defined( ZPL_SYSTEM_WINDOWS ) || defined( ZPL_SYSTEM_CYGWIN )

FileInfo* file_get_standard( file_standard_type std )
{
	if ( ! _std_file_set )
	{
#	define ZPL__SET_STD_FILE( type, v ) \
		_std_files[ type ].fd.p = v;     \
		_std_files[ type ].ops  = default_file_operations
		ZPL__SET_STD_FILE( ZPL_FILE_STANDARD_INPUT, GetStdHandle( STD_INPUT_HANDLE ) );
		ZPL__SET_STD_FILE( ZPL_FILE_STANDARD_OUTPUT, GetStdHandle( STD_OUTPUT_HANDLE ) );
		ZPL__SET_STD_FILE( ZPL_FILE_STANDARD_ERROR, GetStdHandle( STD_ERROR_HANDLE ) );
#	undef ZPL__SET_STD_FILE
		_std_file_set = true;
	}
	return &_std_files[ std ];
}

void file_connect_handle( FileInfo* file, void* handle )
{
	ZPL_ASSERT_NOT_NULL( file );
	ZPL_ASSERT_NOT_NULL( handle );

	if ( file->is_temp )
		return;

	zero_item( file );

	file->fd.p = handle;
	file->ops  = default_file_operations;
}

FileError file_truncate( FileInfo* f, s64 size )
{
	FileError err         = EFileError_NONE;
	s64       prev_offset = file_tell( f );
	file_seek( f, size );
	if ( ! SetEndOfFile( f ) )
		err = EFileError_TRUNCATION_FAILURE;
	file_seek( f, prev_offset );
	return err;
}

b32 fs_exists( char const* name )
{
	WIN32_FIND_DATAW data;
	wchar_t*         w_text;
	void*            handle;
	b32              found = false;
	AllocatorInfo    a     = heap_allocator();

	w_text = _alloc_utf8_to_ucs2( a, name, NULL );
	if ( w_text == NULL )
	{
		return false;
	}
	handle = FindFirstFileW( w_text, &data );
	free( a, w_text );
	found = handle != INVALID_HANDLE_VALUE;
	if ( found )
		FindClose( handle );
	return found;
}

#else    // POSIX

FileInfo* file_get_standard( file_standard_type std )
{
	if ( ! _std_file_set )
	{
#	define ZPL__SET_STD_FILE( type, v ) \
		_std_files[ type ].fd.i = v;     \
		_std_files[ type ].ops  = default_file_operations
		ZPL__SET_STD_FILE( ZPL_FILE_STANDARD_INPUT, 0 );
		ZPL__SET_STD_FILE( ZPL_FILE_STANDARD_OUTPUT, 1 );
		ZPL__SET_STD_FILE( ZPL_FILE_STANDARD_ERROR, 2 );
#	undef ZPL__SET_STD_FILE
		_std_file_set = true;
	}
	return &_std_files[ std ];
}

FileError file_truncate( FileInfo* f, s64 size )
{
	FileError err = EFileError_NONE;
	int       i   = ftruncate( f->fd.i, size );
	if ( i != 0 )
		err = EFileError_TRUNCATION_FAILURE;
	return err;
}

b32 fs_exists( char const* name )
{
	return access( name, F_OK ) != -1;
}

#endif

s64 file_size( FileInfo* f )
{
	s64 size        = 0;
	s64 prev_offset = file_tell( f );
	file_seek_to_end( f );
	size = file_tell( f );
	file_seek( f, prev_offset );
	return size;
}

FileError file_temp( FileInfo* file )
{
	zero_item( file );
	FILE* fd = NULL;

#if ( defined( ZPL_SYSTEM_WINDOWS ) && ! defined( ZPL_SYSTEM_TINYC ) ) && ! defined( ZPL_COMPILER_GCC )
	errno_t errcode = tmpfile_s( &fd );

	if ( errcode != 0 )
	{
		fd = NULL;
	}
#else
	fd         = tmpfile();
#endif

	if ( fd == NULL )
	{
		return EFileError_INVALID;
	}

#if defined( ZPL_SYSTEM_WINDOWS ) && ! defined( ZPL_COMPILER_GCC )
	file->fd.i = _get_osfhandle( _fileno( fd ) );
#else
	file->fd.i = fileno( fd );
#endif
	file->ops     = default_file_operations;
	file->is_temp = true;
	return EFileError_NONE;
}

file_contents file_read_contents( AllocatorInfo a, b32 zero_terminate, char const* filepath )
{
	file_contents result = { 0 };
	FileInfo      file   = { 0 };

	result.allocator = a;

	if ( file_open( &file, filepath ) == EFileError_NONE )
	{
		sw file_size = zpl_cast( sw ) file_size( &file );
		if ( file_size > 0 )
		{
			result.data = alloc( a, zero_terminate ? file_size + 1 : file_size );
			result.size = file_size;
			file_read_at( &file, result.data, result.size, 0 );
			if ( zero_terminate )
			{
				u8* str          = zpl_cast( u8* ) result.data;
				str[ file_size ] = '\0';
			}
		}
		file_close( &file );
	}

	return result;
}

void file_free_contents( file_contents* fc )
{
	ZPL_ASSERT_NOT_NULL( fc->data );
	free( fc->allocator, fc->data );
	fc->data = NULL;
	fc->size = 0;
}

b32 file_write_contents( char const* filepath, void const* buffer, sw size, FileError* err )
{
	FileInfo  f = { 0 };
	FileError open_err;
	b32       write_ok;
	open_err = file_open_mode( &f, EFileMode_WRITE, filepath );

	if ( open_err != EFileError_NONE )
	{
		if ( err )
			*err = open_err;

		return false;
	}

	write_ok = file_write( &f, buffer, size );
	file_close( &f );
	return write_ok;
}

char* file_read_lines( AllocatorInfo alloc, Array( char* ) * lines, char const* filename, b32 strip_whitespace )
{
	FileInfo f = { 0 };
	file_open( &f, filename );
	sw fsize = ( sw )file_size( &f );

	char* contents = ( char* )alloc( alloc, fsize + 1 );
	file_read( &f, contents, fsize );
	contents[ fsize ] = 0;
	*lines            = str_split_lines( alloc, contents, strip_whitespace );
	file_close( &f );

	return contents;
}

#if ! defined( _WINDOWS_ ) && defined( ZPL_SYSTEM_WINDOWS )
ZPL_IMPORT DWORD WINAPI GetFullPathNameA( char const* lpFileName, DWORD nBufferLength, char* lpBuffer, char** lpFilePart );
ZPL_IMPORT DWORD WINAPI GetFullPathNameW( wchar_t const* lpFileName, DWORD nBufferLength, wchar_t* lpBuffer, wchar_t** lpFilePart );
#endif

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: source/core/file_misc.c


#if defined( ZPL_SYSTEM_UNIX ) || defined( ZPL_SYSTEM_MACOS )
#	include <dirent.h>
#endif

#if defined( ZPL_SYSTEM_UNIX ) && ! defined( ZPL_SYSTEM_FREEBSD ) && ! defined( ZPL_SYSTEM_OPENBSD ) && ! defined( ZPL_SYSTEM_CYGWIN ) && ! defined( ZPL_SYSTEM_EMSCRIPTEN )
#	include <sys/sendfile.h>
#endif

#if defined( ZPL_SYSTEM_WINDOWS )
#	include <direct.h>
#	include <io.h>
#endif

#if defined( ZPL_SYSTEM_CYGWIN )
#	include <dirent.h>
#	include <io.h>
#	include <windows.h>
#endif

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS


#if defined( ZPL_SYSTEM_WINDOWS ) || defined( ZPL_SYSTEM_CYGWIN )
file_time fs_last_write_time( char const* filepath )
{
	ULARGE_INTEGER            li              = { 0 };
	FILETIME                  last_write_time = { 0 };
	WIN32_FILE_ATTRIBUTE_DATA data            = { 0 };
	AllocatorInfo             a               = heap_allocator();

	wchar_t* w_text = _alloc_utf8_to_ucs2( a, filepath, NULL );
	if ( w_text == NULL )
	{
		return 0;
	}
	if ( GetFileAttributesExW( w_text, GetFileExInfoStandard, &data ) )
		last_write_time = data.ftLastWriteTime;

	free( a, w_text );

	li.LowPart  = last_write_time.dwLowDateTime;
	li.HighPart = last_write_time.dwHighDateTime;
	return zpl_cast( file_time ) li.QuadPart;
}

b32 fs_copy( char const* existing_filename, char const* new_filename, b32 fail_if_exists )
{
	b32           result = false;
	AllocatorInfo a      = heap_allocator();

	wchar_t* w_old = _alloc_utf8_to_ucs2( a, existing_filename, NULL );
	if ( w_old == NULL )
	{
		return false;
	}

	wchar_t* w_new = _alloc_utf8_to_ucs2( a, new_filename, NULL );
	if ( w_new != NULL )
	{
		result = CopyFileW( w_old, w_new, fail_if_exists );
	}

	free( a, w_old );
	free( a, w_new );
	return result;
}

b32 fs_move( char const* existing_filename, char const* new_filename )
{
	b32           result = false;
	AllocatorInfo a      = heap_allocator();

	wchar_t* w_old = _alloc_utf8_to_ucs2( a, existing_filename, NULL );
	if ( w_old == NULL )
	{
		return false;
	}

	wchar_t* w_new = _alloc_utf8_to_ucs2( a, new_filename, NULL );
	if ( w_new != NULL )
	{
		result = MoveFileW( w_old, w_new );
	}

	free( a, w_old );
	free( a, w_new );
	return result;
}

b32 fs_remove( char const* filename )
{
	b32           result = false;
	AllocatorInfo a      = heap_allocator();

	wchar_t* w_filename = _alloc_utf8_to_ucs2( a, filename, NULL );
	if ( w_filename == NULL )
	{
		return false;
	}

	result = DeleteFileW( w_filename );

	free( a, w_filename );
	return result;
}

#else

file_time fs_last_write_time( char const* filepath )
{
	time_t      result = 0;
	struct stat file_stat;

	if ( stat( filepath, &file_stat ) )
		result = file_stat.st_mtime;

	return zpl_cast( file_time ) result;
}

#	if defined( ZPL_SYSTEM_FREEBSD )
#		include <sys/socket.h>
#		include <sys/types.h>
#		include <sys/uio.h>
#	endif


b32 fs_copy( char const* existing_filename, char const* new_filename, b32 fail_if_exists )
{
	unused( fail_if_exists );
#	if defined( ZPL_SYSTEM_OSX )
	return copyfile( existing_filename, new_filename, NULL, COPYFILE_DATA ) == 0;
#	elif defined( ZPL_SYSTEM_OPENBSD )
	ZPL_NOT_IMPLEMENTED;
	return 0;
#	elif defined( ZPL_SYSTEM_EMSCRIPTEN )
	ZPL_NOT_IMPLEMENTED;
	return 0;
#	else
	int         existing_fd = open( existing_filename, O_RDONLY, 0 );
	struct stat stat_existing;
	fstat( existing_fd, &stat_existing );

	sw  size;
	int new_fd = open( new_filename, O_WRONLY | O_CREAT, stat_existing.st_mode );

#		if defined( ZPL_SYSTEM_FREEBSD )
	size       = sendfile( new_fd, existing_fd, 0, stat_existing.st_size, NULL, 0, 0 );
#		else
	size = sendfile( new_fd, existing_fd, 0, stat_existing.st_size );
#		endif

	close( new_fd );
	close( existing_fd );

	return size == stat_existing.st_size;
#	endif
}

b32 fs_move( char const* existing_filename, char const* new_filename )
{
	if ( link( existing_filename, new_filename ) == 0 )
	{
		return ( unlink( existing_filename ) != -1 );
	}
	return false;
}

b32 fs_remove( char const* filename )
{
#	if defined( ZPL_SYSTEM_OSX ) || defined( ZPL_SYSTEM_EMSCRIPTEN )
	return ( unlink( filename ) != -1 );
#	else
	return ( remove( filename ) == 0 );
#	endif
}

#endif

char* path_get_full_name( AllocatorInfo a, char const* path )
{
#if defined( ZPL_SYSTEM_WINDOWS )
	wchar_t* w_path     = NULL;
	wchar_t* w_fullpath = NULL;
	sw       w_len      = 0;
	sw       new_len    = 0;
	sw       new_len1   = 0;
	char*    new_path   = 0;

	w_path = _alloc_utf8_to_ucs2( heap_allocator(), path, NULL );
	if ( w_path == NULL )
	{
		return NULL;
	}

	w_len = GetFullPathNameW( w_path, 0, NULL, NULL );
	if ( w_len == 0 )
	{
		return NULL;
	}

	w_fullpath = alloc_array( heap_allocator(), wchar_t, w_len + 1 );
	GetFullPathNameW( w_path, zpl_cast( int ) w_len, w_fullpath, NULL );
	w_fullpath[ w_len ] = 0;

	free( heap_allocator(), w_path );

	new_len = WideCharToMultiByte( CP_UTF8, WC_ERR_INVALID_CHARS, w_fullpath, zpl_cast( int ) w_len, NULL, 0, NULL, NULL );

	if ( new_len == 0 )
	{
		free( heap_allocator(), w_fullpath );
		return NULL;
	}

	new_path = alloc_array( a, char, new_len );
	new_len1 = WideCharToMultiByte( CP_UTF8, WC_ERR_INVALID_CHARS, w_fullpath, zpl_cast( int ) w_len, new_path, zpl_cast( int ) new_len, NULL, NULL );

	if ( new_len1 == 0 )
	{
		free( heap_allocator(), w_fullpath );
		free( a, new_path );
		return NULL;
	}

	new_path[ new_len ] = 0;
	return new_path;
#else
	char *p, *result, *fullpath = NULL;
	sw    len;
	p        = realpath( path, NULL );
	fullpath = p;
	if ( p == NULL )
	{
		// NOTE(bill): File does not exist
		fullpath = zpl_cast( char* ) path;
	}

	len = strlen( fullpath );

	result = alloc_array( a, char, len + 1 );
	memmove( result, fullpath, len );
	result[ len ] = 0;
	free( a, p );

	return result;
#endif
}

FileError path_mkdir( char const* path, s32 mode )
{
	s32 error = 0;
#if defined( ZPL_SYSTEM_WINDOWS )
	error = _wmkdir( ( const wchar_t* )utf8_to_ucs2_buf( ( const u8* )path ) );
#else
	error                      = mkdir( path, ( mode_t )mode );
#endif

	if ( error == 0 )
	{
		return EFileError_NONE;
	}

	switch ( errno )
	{
		case EPERM :
		case EACCES :
			return EFileError_PERMISSION;
		case EEXIST :
			return EFileError_EXISTS;
		case ENAMETOOLONG :
			return EFileError_NAME_TOO_LONG;
	}

	return EFileError_UNKNOWN;
}

sw path_mkdir_recursive( char const* path, s32 mode )
{
	char  tmp[ ZPL_MAX_PATH ] = { 0 };
	char* p                   = 0;
	sw    len                 = strlen( path );

	if ( len > size_of( tmp ) - 1 )
	{
		return -1;
	}
	strcpy( tmp, path );
	path_fix_slashes( tmp );
	for ( p = tmp + 1; *p; p++ )
	{
		if ( *p == ZPL_PATH_SEPARATOR )
		{
			*p = 0;
			path_mkdir( tmp, mode );
			*p = ZPL_PATH_SEPARATOR;
		}
	}
	path_mkdir( tmp, mode );
	return 0;
}

FileError path_rmdir( char const* path )
{
	s32 error = 0;
#if defined( ZPL_SYSTEM_WINDOWS )
	error = _wrmdir( ( const wchar_t* )utf8_to_ucs2_buf( ( const u8* )path ) );
#else
	error                      = rmdir( path );
#endif

	if ( error == 0 )
	{
		return EFileError_NONE;
	}

	switch ( errno )
	{
		case EPERM :
		case EACCES :
			return EFileError_PERMISSION;
		case ENOENT :
			return EFileError_NOT_EXISTS;
		case ENOTEMPTY :
			return EFileError_NOT_EMPTY;
		case ENAMETOOLONG :
			return EFileError_NAME_TOO_LONG;
	}

	return EFileError_UNKNOWN;
}

void _file_direntry( AllocatorInfo alloc, char const* dirname, String* output, b32 recurse )
{
#if defined( ZPL_SYSTEM_UNIX ) || defined( ZPL_SYSTEM_OSX )
	DIR *          d, *cd;
	struct dirent* dir;
	d = opendir( dirname );

	if ( d )
	{
		while ( ( dir = readdir( d ) ) )
		{
			if ( dir == 0 )
				break;
			if ( ! str_compare( dir->d_name, "..", 2 ) )
				continue;
			if ( dir->d_name[ 0 ] == '.' && dir->d_name[ 1 ] == 0 )
				continue;

			String dirpath = string_make( alloc, dirname );
			dirpath        = string_appendc( dirpath, "/" );
			dirpath        = string_appendc( dirpath, dir->d_name );

			*output = string_appendc( *output, dirpath );
			*output = string_appendc( *output, "\n" );

			if ( recurse && ( cd = opendir( dirpath ) ) != NULL && dir->d_type == DT_DIR )
			{
				_file_direntry( alloc, dirpath, output, recurse );
			}
			string_free( dirpath );
		}
	}
#elif defined( ZPL_SYSTEM_WINDOWS )
	uw                  length = strlen( dirname );
	struct _wfinddata_t data;
	sptr                findhandle;

	char directory[ MAX_PATH ] = { 0 };
	strncpy( directory, dirname, length );

	// keeping it native
	for ( uw i = 0; i < length; i++ )
	{
		if ( directory[ i ] == '/' )
			directory[ i ] = '\\';
	}

	// remove trailing slashses
	if ( directory[ length - 1 ] == '\\' )
	{
		directory[ length - 1 ] = '\0';
	}

	// attach search pattern
	String findpath = string_make( alloc, directory );
	findpath        = string_appendc( findpath, "\\" );
	findpath        = string_appendc( findpath, "*" );

	findhandle = _wfindfirst( ( const wchar_t* )utf8_to_ucs2_buf( ( const u8* )findpath ), &data );
	string_free( findpath );

	if ( findhandle != -1 )
	{
		do
		{
			char* filename = ( char* )ucs2_to_utf8_buf( ( const u16* )data.name );
			if ( ! str_compare( filename, "..", 2 ) )
				continue;
			if ( filename[ 0 ] == '.' && filename[ 1 ] == 0 )
				continue;

			String dirpath = string_make( alloc, directory );
			dirpath        = string_appendc( dirpath, "\\" );
			dirpath        = string_appendc( dirpath, filename );
			DWORD attrs    = GetFileAttributesW( ( const wchar_t* )utf8_to_ucs2_buf( ( const u8* )dirpath ) );

			*output = string_appendc( *output, dirpath );
			*output = string_appendc( *output, "\n" );

			if ( recurse && ( data.attrib & _A_SUBDIR ) && ! ( attrs & FILE_ATTRIBUTE_REPARSE_POINT ) )
			{
				_file_direntry( alloc, dirpath, output, recurse );
			}

			string_free( dirpath );
		} while ( _wfindnext( findhandle, &data ) != -1 );
		_findclose( findhandle );
	}
#else
	// TODO: Implement other OSes
#endif
}

String path_dirlist( AllocatorInfo alloc, char const* dirname, b32 recurse )
{
	String buf = string_make_reserve( alloc, 4 );
	_file_direntry( alloc, dirname, &buf, recurse );
	return buf;
}

void dirinfo_init( dir_info* dir, char const* path )
{
	ZPL_ASSERT_NOT_NULL( dir );

	dir_info dir_ = { 0 };
	*dir          = dir_;
	dir->fullpath = ( char const* )malloc( strlen( path ) );
	strcpy( ( char* )dir->fullpath, path );


	String dirlist = path_dirlist( heap(), path, false );
	char** files   = str_split_lines( heap(), dirlist, false );
	dir->filenames = files;
	dir->buf       = dirlist;

	array_init( dir->entries, heap() );

	for ( s32 i = 0; i < array_count( files ); ++i )
	{
		dir_entry entry = { 0 };
		entry.filename  = files[ i ];
		entry.type      = fs_get_type( entry.filename );

		array_append( dir->entries, entry );
	}
}

internal void _dirinfo_free_entry( dir_entry* entry )
{
	if ( entry->dir_info )
	{
		dirinfo_free( entry->dir_info );
		mfree( entry->dir_info );
		entry->dir_info = NULL;
	}
}

void dirinfo_free( dir_info* dir )
{
	ZPL_ASSERT_NOT_NULL( dir );

	for ( sw i = 0; i < array_count( dir->entries ); ++i )
	{
		_dirinfo_free_entry( dir->entries + i );
	}

	array_free( dir->entries );
	array_free( dir->filenames );
	string_free( dir->buf );
	mfree( ( void* )dir->fullpath );
}

u8 fs_get_type( char const* path )
{
#ifdef ZPL_SYSTEM_WINDOWS
	DWORD attrs = GetFileAttributesW( ( const wchar_t* )utf8_to_ucs2_buf( ( const u8* )path ) );

	if ( attrs == INVALID_FILE_ATTRIBUTES )
	{
		return ZPL_DIR_TYPE_UNKNOWN;
	}

	if ( attrs & FILE_ATTRIBUTE_DIRECTORY )
		return ZPL_DIR_TYPE_FOLDER;
	else
		return ZPL_DIR_TYPE_FILE;

#else
	struct stat s;
	if ( stat( path, &s ) == 0 )
	{
		if ( s.st_mode & S_IFDIR )
			return ZPL_DIR_TYPE_FOLDER;
		else
			return ZPL_DIR_TYPE_FILE;
	}
#endif

	return ZPL_DIR_TYPE_UNKNOWN;
}

void dirinfo_step( dir_entry* entry )
{
	if ( entry->dir_info )
	{
		_dirinfo_free_entry( entry );
	}

	entry->dir_info  = ( dir_info* )malloc( sizeof( dir_info ) );
	dir_info dir_    = { 0 };
	*entry->dir_info = dir_;

	local_persist char buf[ 128 ] = { 0 };
	char const*        path       = entry->filename;

	if ( entry->type != ZPL_DIR_TYPE_FOLDER )
	{
		path_fix_slashes( ( char* )path );
		char const* slash = char_last_occurence( path, ZPL_PATH_SEPARATOR );
		strncpy( buf, path, slash - path );
		path = buf;
	}

	dirinfo_init( entry->dir_info, path );
}

void file_dirinfo_refresh( FileInfo* file )
{
	if ( file->is_temp )
		return;

	if ( file->dir )
	{
		_dirinfo_free_entry( file->dir );
		mfree( file->dir );
		file->dir = NULL;
	}

	file->dir           = ( dir_entry* )malloc( sizeof( dir_entry ) );
	dir_entry dir_      = { 0 };
	*file->dir          = dir_;
	file->dir->filename = file->filename;
	file->dir->type     = ZPL_DIR_TYPE_FILE;

	dirinfo_step( file->dir );
}

void path_fix_slashes( char* path )
{
#ifdef ZPL_SYSTEM_WINDOWS
	char* p = path;

	while ( *p != '\0' )
	{
		if ( *p == '/' )
			*p = '\\';

		++p;
	}
#endif
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: source/core/file_stream.c


////////////////////////////////////////////////////////////////
//
// Memory streaming
//
//

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

typedef struct
{
	u8            magic;
	u8*           buf;    //< zpl_array OR plain buffer if we can't write
	sw            cursor;
	AllocatorInfo alloc;

	file_stream_flags flags;
	sw                cap;
} _memory_fd;

#define ZPL__FILE_STREAM_FD_MAGIC 37

ZPL_DEF_INLINE file_descriptor _file_stream_fd_make( _memory_fd* d );
ZPL_DEF_INLINE _memory_fd*     _file_stream_from_fd( file_descriptor fd );

ZPL_IMPL_INLINE file_descriptor _file_stream_fd_make( _memory_fd* d )
{
	file_descriptor fd = { 0 };
	fd.p               = ( void* )d;
	return fd;
}

ZPL_IMPL_INLINE _memory_fd* _file_stream_from_fd( file_descriptor fd )
{
	_memory_fd* d = ( _memory_fd* )fd.p;
	ZPL_ASSERT( d->magic == ZPL__FILE_STREAM_FD_MAGIC );
	return d;
}

b8 file_stream_new( FileInfo* file, AllocatorInfo allocator )
{
	ZPL_ASSERT_NOT_NULL( file );
	_memory_fd* d = ( _memory_fd* )alloc( allocator, size_of( _memory_fd ) );
	if ( ! d )
		return false;
	zero_item( file );
	d->magic = ZPL__FILE_STREAM_FD_MAGIC;
	d->alloc = allocator;
	d->flags = ZPL_FILE_STREAM_CLONE_WRITABLE;
	d->cap   = 0;
	if ( ! array_init( d->buf, allocator ) )
		return false;
	file->ops             = memory_file_operations;
	file->fd              = _file_stream_fd_make( d );
	file->dir             = NULL;
	file->last_write_time = 0;
	file->filename        = NULL;
	file->is_temp         = true;
	return true;
}

b8 file_stream_open( FileInfo* file, AllocatorInfo allocator, u8* buffer, sw size, file_stream_flags flags )
{
	ZPL_ASSERT_NOT_NULL( file );
	_memory_fd* d = ( _memory_fd* )alloc( allocator, size_of( _memory_fd ) );
	if ( ! d )
		return false;
	zero_item( file );
	d->magic = ZPL__FILE_STREAM_FD_MAGIC;
	d->alloc = allocator;
	d->flags = flags;
	if ( d->flags & ZPL_FILE_STREAM_CLONE_WRITABLE )
	{
		if ( ! array_init_reserve( d->buf, allocator, size ) )
			return false;
		memcopy( d->buf, buffer, size );
		d->cap = array_count( d->buf ) = size;
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
	ZPL_ASSERT_NOT_NULL( file );
	_memory_fd* d = _file_stream_from_fd( file->fd );
	if ( size )
		*size = d->cap;
	return d->buf;
}

internal ZPL_FILE_SEEK_PROC( _memory_file_seek )
{
	_memory_fd* d      = _file_stream_from_fd( fd );
	sw          buflen = d->cap;

	if ( whence == ZPL_SEEK_WHENCE_BEGIN )
		d->cursor = 0;
	else if ( whence == ZPL_SEEK_WHENCE_END )
		d->cursor = buflen;

	d->cursor = max( 0, clamp( d->cursor + offset, 0, buflen ) );
	if ( new_offset )
		*new_offset = d->cursor;
	return true;
}

internal ZPL_FILE_READ_AT_PROC( _memory_file_read )
{
	unused( stop_at_newline );
	_memory_fd* d = _file_stream_from_fd( fd );
	memcopy( buffer, d->buf + offset, size );
	if ( bytes_read )
		*bytes_read = size;
	return true;
}

internal ZPL_FILE_WRITE_AT_PROC( _memory_file_write )
{
	_memory_fd* d = _file_stream_from_fd( fd );
	if ( ! ( d->flags & ( ZPL_FILE_STREAM_CLONE_WRITABLE | ZPL_FILE_STREAM_WRITABLE ) ) )
		return false;
	sw buflen   = d->cap;
	sw extralen = max( 0, size - ( buflen - offset ) );
	sw rwlen    = size - extralen;
	sw new_cap  = buflen + extralen;
	if ( d->flags & ZPL_FILE_STREAM_CLONE_WRITABLE )
	{
		if ( array_capacity( d->buf ) < new_cap )
		{
			if ( ! array_grow( d->buf, ( s64 )( new_cap ) ) )
				return false;
		}
	}
	memcopy( d->buf + offset, buffer, rwlen );

	if ( ( d->flags & ZPL_FILE_STREAM_CLONE_WRITABLE ) && extralen > 0 )
	{
		memcopy( d->buf + offset + rwlen, ptr_add_const( buffer, rwlen ), extralen );
		d->cap = array_count( d->buf ) = new_cap;
	}
	else
	{
		extralen = 0;
	}

	if ( bytes_written )
		*bytes_written = ( rwlen + extralen );
	return true;
}

internal ZPL_FILE_CLOSE_PROC( _memory_file_close )
{
	_memory_fd*   d     = _file_stream_from_fd( fd );
	AllocatorInfo alloc = d->alloc;
	if ( d->flags & ZPL_FILE_STREAM_CLONE_WRITABLE )
		array_free( d->buf );
	free( alloc, d );
}

file_operations const memory_file_operations = { _memory_file_read, _memory_file_write, _memory_file_seek, _memory_file_close };

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: source/core/file_tar.c



ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

typedef struct
{
	char name[ 100 ];
	char mode[ 8 ];
	char owner[ 8 ];
	char group[ 8 ];
	char size[ 12 ];
	char mtime[ 12 ];
	char checksum[ 8 ];
	char type;
	char linkname[ 100 ];
	char _padding[ 255 ];
} _tar_header;

internal uw _tar_checksum( _tar_header* hr )
{
	uw  i;
	uw  res = 256;
	u8* p   = zpl_cast( u8* )( hr );
	for ( i = 0; i < zpl_cast( uw ) offset_of( _tar_header, checksum ); i++ )
		res += p[ i ];
	for ( i = zpl_cast( uw ) offset_of( _tar_header, type ); i < zpl_cast( uw ) size_of( _tar_header ); i++ )
		res += p[ i ];
	return res;
}

internal b32 _tar_write_null( FileInfo* archive, sw cnt )
{
	char* out = bprintf( "%*r", cnt, '\0' );
	if ( ! file_write( archive, out, cnt ) )
		return 0;
	return 1;
}

sw tar_pack( FileInfo* archive, char const** paths, sw paths_len )
{
	ZPL_ASSERT_NOT_NULL( archive );
	ZPL_ASSERT_NOT_NULL( paths );

	for ( sw i = 0; i < paths_len; i++ )
	{
		ZPL_ASSERT_NOT_NULL( paths[ i ] );
		_tar_header hr = { 0 };
		FileInfo    file;
		FileError   ferr = file_open_mode( &file, EFileMode_READ, paths[ i ] );
		if ( ferr == EFileError_NOT_EXISTS )
		{
			return -( ZPL_TAR_ERROR_FILE_NOT_FOUND );
		}
		else if ( ferr != EFileError_NONE )
		{
			return -( ZPL_TAR_ERROR_IO_ERROR );
		}

		s64 file_size = file_size( &file );
		snprintf( hr.name, 12, "%s", paths[ i ] );
		snprintf( hr.size, 12, "%o", file_size );
		snprintf( hr.mode, 8, "%o", 0664 );
		snprintf( hr.mtime, 12, "%o", fs_last_write_time( paths[ i ] ) );
		hr.type = ZPL_TAR_TYPE_REGULAR;
		snprintf( hr.checksum, 8, "%o", _tar_checksum( &hr ) );

		file_write( archive, zpl_cast( void* )( &hr ), size_of( _tar_header ) );

		// write data
		{
			s64  remaining_data = file_size;
			s64  total_data     = align_forward_i64( remaining_data, 512 );
			s64  padding        = ( total_data - file_size );
			char buf[ 4096 ]    = { 0 };
			s64  pos            = 0;
			sw   bytes_read     = 0;
			do
			{
				if ( ! file_read_at_check( &file, buf, 4096, pos, &bytes_read ) )
				{
					file_close( &file );
					return -( ZPL_TAR_ERROR_IO_ERROR );
				}
				else if ( bytes_read == 0 )
				{
					break;
				}

				file_write( archive, buf, bytes_read );
				pos            += bytes_read;
				remaining_data -= bytes_read;
			} while ( remaining_data > 0 );

			if ( padding > 0 )
			{
				if ( ! _tar_write_null( archive, padding ) )
				{
					file_close( &file );
					return -( ZPL_TAR_ERROR_IO_ERROR );
				}
			}
		}

		file_close( &file );
	}

	if ( ! _tar_write_null( archive, size_of( _tar_header ) * 2 ) )
	{
		return -( ZPL_TAR_ERROR_IO_ERROR );
	}

	return 0;
}

sw tar_pack_dir( FileInfo* archive, char const* path, AllocatorInfo alloc )
{
	String       filelst = path_dirlist( alloc, path, true );
	char const** files   = zpl_cast( char const** ) str_split_lines( alloc, filelst, false );
	sw           err     = tar_pack( archive, files, array_count( files ) );
	string_free( filelst );
	array_free( files );
	return err;
}

sw tar_unpack( FileInfo* archive, tar_unpack_proc* unpack_proc, void* user_data )
{
	ZPL_ASSERT_NOT_NULL( archive );
	ZPL_ASSERT_NOT_NULL( unpack_proc );

	s64         pos = file_tell( archive );
	_tar_header hr  = { 0 };
	sw          err = ZPL_TAR_ERROR_NONE;

	do
	{
		if ( ! file_read( archive, zpl_cast( void* ) & hr, size_of( hr ) ) )
		{
			err = ZPL_TAR_ERROR_IO_ERROR;
			break;
		}
		else if ( *hr.checksum == 0 )
		{
			break;
		}
		pos = file_tell( archive );

		tar_record rec = { 0 };
		rec.type       = hr.type;
		rec.path       = hr.name;
		rec.offset     = pos;
		rec.length     = str_to_i64( hr.size, 0, 8 );
		rec.error      = ZPL_TAR_ERROR_NONE;

		uw checksum1 = zpl_cast( uw )( str_to_i64( hr.checksum, 0, 8 ) );
		uw checksum2 = _tar_checksum( &hr );
		rec.error    = ( checksum1 != checksum2 ) ? zpl_cast( sw ) ZPL_TAR_ERROR_BAD_CHECKSUM : rec.error;

		rec.error = unpack_proc( archive, &rec, user_data );

		if ( rec.error > 0 )
		{
			err = ZPL_TAR_ERROR_INTERRUPTED;
			break;
		}

		/* tar rounds files to 512 byte boundary */
		file_seek( archive, pos + align_forward_i64( rec.length, 512 ) );
	} while ( err == ZPL_TAR_ERROR_NONE );

	return -( err );
}

ZPL_TAR_UNPACK_PROC( tar_default_list_file )
{
	( void )archive;
	( void )user_data;
	if ( file->error != ZPL_TAR_ERROR_NONE )
		return 0; /* skip file */

	if ( file->type != ZPL_TAR_TYPE_REGULAR )
		return 0; /* we only care about regular files */

	/* proceed as usual */
	printf( "name: %s, offset: %d, length: %d\n", file->path, file->offset, file->length );
	return 0;
}

ZPL_TAR_UNPACK_PROC( tar_default_unpack_file )
{
	if ( file->error != ZPL_TAR_ERROR_NONE )
		return 0; /* skip file */

	if ( file->type != ZPL_TAR_TYPE_REGULAR )
		return 0; /* we only care about regular files */

	if ( ! str_compare( file->path, "..", 2 ) )
		return 0;

	char  tmp[ ZPL_MAX_PATH ] = { 0 };
	char* base_path           = zpl_cast( char* ) user_data;
	sw    base_len            = strlen( base_path );
	sw    len                 = strlen( file->path );
	ZPL_ASSERT( base_len + len - 2 < ZPL_MAX_PATH ); /* todo: account for missing leading path sep */

	strcpy( tmp, base_path );
	path_fix_slashes( tmp ); /* todo: need to do twice as base_path is checked before concat */

	if ( *tmp && tmp[ base_len - 1 ] != ZPL_PATH_SEPARATOR )
	{
		char sep[ 2 ] = { ZPL_PATH_SEPARATOR, 0 };
		strcat( tmp, sep );
	}
	strcat( tmp, file->path );
	path_fix_slashes( tmp );

	const char* last_slash = char_last_occurence( tmp, ZPL_PATH_SEPARATOR );

	if ( last_slash )
	{
		sw i     = zpl_cast( sw )( last_slash - tmp );
		tmp[ i ] = 0;
		path_mkdir_recursive( tmp, 0755 );
		tmp[ i ] = ZPL_PATH_SEPARATOR;
	}

	FileInfo f;
	file_create( &f, tmp );
	{
		char buf[ 4096 ]    = { 0 };
		sw   remaining_data = file->length;
		sw   bytes_read     = 0;
		s64  pos            = file->offset;
		do
		{
			if ( ! file_read_at_check( archive, buf, min( 4096, remaining_data ), pos, &bytes_read ) )
			{
				file_close( &f );
				return 1;
			}
			else if ( bytes_read == 0 )
			{
				break;
			}

			file_write( &f, buf, bytes_read );
			pos            += bytes_read;
			remaining_data -= bytes_read;
		} while ( remaining_data > 0 );
	}
	file_close( &f );
	return 0;
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: source/core/memory_virtual.c

////////////////////////////////////////////////////////////////
//
// Virtual Memory
//
//

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

virtual_memory vm( void* data, sw size )
{
	virtual_memory vm;
	vm.data = data;
	vm.size = size;
	return vm;
}

#if defined( ZPL_SYSTEM_WINDOWS )
virtual_memory vm_alloc( void* addr, sw size )
{
	virtual_memory vm;
	ZPL_ASSERT( size > 0 );
	vm.data = VirtualAlloc( addr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
	vm.size = size;
	return vm;
}

b32 vm_free( virtual_memory vm )
{
	MEMORY_BASIC_INFORMATION info;
	while ( vm.size > 0 )
	{
		if ( VirtualQuery( vm.data, &info, size_of( info ) ) == 0 )
			return false;
		if ( info.BaseAddress != vm.data || info.AllocationBase != vm.data || info.State != MEM_COMMIT || info.RegionSize > zpl_cast( uw ) vm.size )
		{
			return false;
		}
		if ( VirtualFree( vm.data, 0, MEM_RELEASE ) == 0 )
			return false;
		vm.data  = pointer_add( vm.data, info.RegionSize );
		vm.size -= info.RegionSize;
	}
	return true;
}

virtual_memory vm_trim( virtual_memory vm, sw lead_size, sw size )
{
	virtual_memory new_vm = { 0 };
	void*          ptr;
	ZPL_ASSERT( vm.size >= lead_size + size );

	ptr = pointer_add( vm.data, lead_size );

	vm_free( vm );
	new_vm = vm_alloc( ptr, size );
	if ( new_vm.data == ptr )
		return new_vm;
	if ( new_vm.data )
		vm_free( new_vm );
	return new_vm;
}

b32 vm_purge( virtual_memory vm )
{
	VirtualAlloc( vm.data, vm.size, MEM_RESET, PAGE_READWRITE );
	// NOTE: Can this really fail?
	return true;
}

sw virtual_memory_page_size( sw* alignment_out )
{
	SYSTEM_INFO info;
	GetSystemInfo( &info );
	if ( alignment_out )
		*alignment_out = info.dwAllocationGranularity;
	return info.dwPageSize;
}

#else
#	include <sys/mman.h>

#	ifndef MAP_ANONYMOUS
#		define MAP_ANONYMOUS MAP_ANON
#	endif

virtual_memory vm_alloc( void* addr, sw size )
{
	virtual_memory vm;
	ZPL_ASSERT( size > 0 );
	vm.data = mmap( addr, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0 );
	vm.size = size;
	return vm;
}

b32 vm_free( virtual_memory vm )
{
	munmap( vm.data, vm.size );
	return true;
}

virtual_memory vm_trim( virtual_memory vm, sw lead_size, sw size )
{
	void* ptr;
	sw    trail_size;
	ZPL_ASSERT( vm.size >= lead_size + size );

	ptr        = pointer_add( vm.data, lead_size );
	trail_size = vm.size - lead_size - size;

	if ( lead_size != 0 )
		vm_free( vm( vm.data, lead_size ) );
	if ( trail_size != 0 )
		vm_free( vm( ptr, trail_size ) );
	return vm( ptr, size );
}

b32 vm_purge( virtual_memory vm )
{
	int err = madvise( vm.data, vm.size, MADV_DONTNEED );
	return err != 0;
}

sw virtual_memory_page_size( sw* alignment_out )
{
	// TODO: Is this always true?
	sw result = zpl_cast( sw ) sysconf( _SC_PAGE_SIZE );
	if ( alignment_out )
		*alignment_out = result;
	return result;
}

#endif

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: source/core/misc.c


ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

void yield( void )
{
#if defined( ZPL_SYSTEM_WINDOWS )
	Sleep( 0 );
#else
	sched_yield();
#endif
}

const char* get_env( const char* name )
{
	char*       buffer = NULL;
	const char* ptr    = get_env_buf( name );

	if ( ptr == NULL )
	{
		return NULL;
	}

	sw ptr_size = strlen( ptr );
	buffer      = ( char* )malloc( ptr_size * sizeof( char ) + 1 );
	memcopy( ( char* )buffer, ptr, ptr_size + 1 );
	return buffer;
}

const char* get_env_buf( const char* name )
{
#ifdef ZPL_SYSTEM_WINDOWS
	local_persist wchar_t wbuffer[ 32767 ] = { 0 };
	local_persist char    buffer[ 32767 ]  = { 0 };

	if ( ! GetEnvironmentVariableW( zpl_cast( LPCWSTR ) utf8_to_ucs2_buf( zpl_cast( const u8* ) name ), zpl_cast( LPWSTR ) wbuffer, 32767 ) )
	{
		return NULL;
	}

	ucs2_to_utf8( zpl_cast( u8* ) buffer, 32767, zpl_cast( const u16* ) wbuffer );

	return ( const char* )buffer;
#else
	return ( const char* )getenv( name );
#endif
}

String get_env_str( const char* name )
{
	const char* buf = get_env_buf( name );

	if ( buf == NULL )
	{
		return NULL;
	}

	String str = string_make( heap(), buf );
	return str;
}

void set_env( const char* name, const char* value )
{
#if defined( ZPL_SYSTEM_WINDOWS )
	SetEnvironmentVariableA( name, value );
#else
	setenv( name, value, 1 );
#endif
}

void unset_env( const char* name )
{
#if defined( ZPL_SYSTEM_WINDOWS )
	SetEnvironmentVariableA( name, NULL );
#else
	unsetenv( name );
#endif
}

#if ! defined( ZPL_SYSTEM_WINDOWS )
extern char** environ;
#endif

u32 system_command( const char* command, uw buffer_len, char* buffer )
{
#if defined( ZPL_SYSTEM_EMSCRIPTEN )
	ZPL_PANIC( "system_command not supported" );
#else

#	if defined( ZPL_SYSTEM_WINDOWS )
	FILE* handle = _popen( command, "r" );
#	else
	FILE* handle = popen( command, "r" );
#	endif

	if ( ! handle )
		return 0;

	int c;
	uw  i = 0;
	while ( ( c = getc( handle ) ) != EOF && i++ < buffer_len )
	{
		*buffer++ = c;
	}

#	if defined( ZPL_SYSTEM_WINDOWS )
	_pclose( handle );
#	else
	pclose( handle );
#	endif

#endif

	return 1;
}

String system_command_str( const char* command, AllocatorInfo backing )
{
#if defined( ZPL_SYSTEM_EMSCRIPTEN )
	ZPL_PANIC( "system_command not supported" );
#else

#	if defined( ZPL_SYSTEM_WINDOWS )
	FILE* handle = _popen( command, "r" );
#	else
	FILE* handle = popen( command, "r" );
#	endif

	if ( ! handle )
		return NULL;

	String output = string_make_reserve( backing, 4 );

	int c;
	while ( ( c = getc( handle ) ) != EOF )
	{
		char ins[ 2 ] = { ( char )c, 0 };
		output        = string_appendc( output, ins );
	}

#	if defined( ZPL_SYSTEM_WINDOWS )
	_pclose( handle );
#	else
	pclose( handle );
#	endif
	return output;
#endif
	return NULL;
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: source/core/print.c


ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

sw printf_va( char const* fmt, va_list va )
{
	return fprintf_va( file_get_standard( ZPL_FILE_STANDARD_OUTPUT ), fmt, va );
}

sw printf_err_va( char const* fmt, va_list va )
{
	return fprintf_va( file_get_standard( ZPL_FILE_STANDARD_ERROR ), fmt, va );
}

sw fprintf_va( struct FileInfo* f, char const* fmt, va_list va )
{
	local_persist thread_local char buf[ ZPL_PRINTF_MAXLEN ];
	sw                              len = snprintf_va( buf, size_of( buf ), fmt, va );
	b32                             res = file_write( f, buf, len - 1 );    // NOTE: prevent extra whitespace
	return res ? len : -1;
}

char* bprintf_va( char const* fmt, va_list va )
{
	local_persist thread_local char buffer[ ZPL_PRINTF_MAXLEN ];
	snprintf_va( buffer, size_of( buffer ), fmt, va );
	return buffer;
}

sw asprintf_va( AllocatorInfo allocator, char** buffer, char const* fmt, va_list va )
{
	local_persist thread_local char tmp[ ZPL_PRINTF_MAXLEN ];
	ZPL_ASSERT_NOT_NULL( buffer );
	sw res;
	res     = snprintf_va( tmp, size_of( tmp ), fmt, va );
	*buffer = alloc_str( allocator, tmp );
	return res;
}

sw printf( char const* fmt, ... )
{
	sw      res;
	va_list va;
	va_start( va, fmt );
	res = printf_va( fmt, va );
	va_end( va );
	return res;
}

sw printf_err( char const* fmt, ... )
{
	sw      res;
	va_list va;
	va_start( va, fmt );
	res = printf_err_va( fmt, va );
	va_end( va );
	return res;
}

sw fprintf( struct FileInfo* f, char const* fmt, ... )
{
	sw      res;
	va_list va;
	va_start( va, fmt );
	res = fprintf_va( f, fmt, va );
	va_end( va );
	return res;
}

char* bprintf( char const* fmt, ... )
{
	va_list va;
	char*   str;
	va_start( va, fmt );
	str = bprintf_va( fmt, va );
	va_end( va );
	return str;
}

sw asprintf( AllocatorInfo allocator, char** buffer, char const* fmt, ... )
{
	sw      res;
	va_list va;
	va_start( va, fmt );
	res = asprintf_va( allocator, buffer, fmt, va );
	va_end( va );
	return res;
}

sw snprintf( char* str, sw n, char const* fmt, ... )
{
	sw      res;
	va_list va;
	va_start( va, fmt );
	res = snprintf_va( str, n, fmt, va );
	va_end( va );
	return res;
}

enum
{
	ZPL_FMT_MINUS = ZPL_BIT( 0 ),
	ZPL_FMT_PLUS  = ZPL_BIT( 1 ),
	ZPL_FMT_ALT   = ZPL_BIT( 2 ),
	ZPL_FMT_SPACE = ZPL_BIT( 3 ),
	ZPL_FMT_ZERO  = ZPL_BIT( 4 ),

	ZPL_FMT_CHAR   = ZPL_BIT( 5 ),
	ZPL_FMT_SHORT  = ZPL_BIT( 6 ),
	ZPL_FMT_INT    = ZPL_BIT( 7 ),
	ZPL_FMT_LONG   = ZPL_BIT( 8 ),
	ZPL_FMT_LLONG  = ZPL_BIT( 9 ),
	ZPL_FMT_SIZE   = ZPL_BIT( 10 ),
	ZPL_FMT_INTPTR = ZPL_BIT( 11 ),

	ZPL_FMT_UNSIGNED = ZPL_BIT( 12 ),
	ZPL_FMT_LOWER    = ZPL_BIT( 13 ),
	ZPL_FMT_UPPER    = ZPL_BIT( 14 ),
	ZPL_FMT_WIDTH    = ZPL_BIT( 15 ),

	ZPL_FMT_DONE = ZPL_BIT( 30 ),

	ZPL_FMT_INTS = ZPL_FMT_CHAR | ZPL_FMT_SHORT | ZPL_FMT_INT | ZPL_FMT_LONG | ZPL_FMT_LLONG | ZPL_FMT_SIZE | ZPL_FMT_INTPTR
};

typedef struct
{
	s32 base;
	s32 flags;
	s32 width;
	s32 precision;
} _format_info;

internal sw _print_string( char* text, sw max_len, _format_info* info, char const* str )
{
	sw    res = 0, len = 0;
	sw    remaining = max_len;
	char* begin     = text;

	if ( str == NULL && max_len >= 6 )
	{
		res += strlcpy( text, "(null)", 6 );
		return res;
	}

	if ( info && info->precision >= 0 )
		len = strnlen( str, info->precision );
	else
		len = strlen( str );

	if ( info && ( info->width == 0 && info->flags & ZPL_FMT_WIDTH ) )
	{
		return res;
	}

	if ( info && ( info->width == 0 || info->flags & ZPL_FMT_MINUS ) )
	{
		if ( info->precision > 0 )
			len = info->precision < len ? info->precision : len;
		if ( res + len > max_len )
			return res;
		res  += strlcpy( text, str, len );
		text += res;

		if ( info->width > res )
		{
			sw padding = info->width - len;

			char pad = ( info->flags & ZPL_FMT_ZERO ) ? '0' : ' ';
			while ( padding-- > 0 && remaining-- > 0 )
				*text++ = pad, res++;
		}
	}
	else
	{
		if ( info && ( info->width > res ) )
		{
			sw   padding = info->width - len;
			char pad     = ( info->flags & ZPL_FMT_ZERO ) ? '0' : ' ';
			while ( padding-- > 0 && remaining-- > 0 )
				*text++ = pad, res++;
		}

		if ( res + len > max_len )
			return res;
		res += strlcpy( text, str, len );
	}

	if ( info )
	{
		if ( info->flags & ZPL_FMT_UPPER )
			str_to_upper( begin );
		else if ( info->flags & ZPL_FMT_LOWER )
			str_to_lower( begin );
	}

	return res;
}

internal sw _print_char( char* text, sw max_len, _format_info* info, char arg )
{
	char str[ 2 ] = "";
	str[ 0 ]      = arg;
	return _print_string( text, max_len, info, str );
}

internal sw _print_repeated_char( char* text, sw max_len, _format_info* info, char arg )
{
	sw  res = 0;
	s32 rem = ( info ) ? ( info->width > 0 ) ? info->width : 1 : 1;
	res     = rem;
	while ( rem-- > 0 )
		*text++ = arg;

	return res;
}

internal sw _print_i64( char* text, sw max_len, _format_info* info, s64 value )
{
	char num[ 130 ];
	i64_to_str( value, num, info ? info->base : 10 );
	return _print_string( text, max_len, info, num );
}

internal sw _print_u64( char* text, sw max_len, _format_info* info, u64 value )
{
	char num[ 130 ];
	u64_to_str( value, num, info ? info->base : 10 );
	return _print_string( text, max_len, info, num );
}

internal sw _print_f64( char* text, sw max_len, _format_info* info, b32 is_hexadecimal, f64 arg )
{
	// TODO: Handle exponent notation
	sw    width, len, remaining = max_len;
	char* text_begin = text;

	if ( arg )
	{
		u64 value;
		if ( arg < 0 )
		{
			if ( remaining > 1 )
				*text = '-', remaining--;
			text++;
			arg = -arg;
		}
		else if ( info->flags & ZPL_FMT_MINUS )
		{
			if ( remaining > 1 )
				*text = '+', remaining--;
			text++;
		}

		value  = zpl_cast( u64 ) arg;
		len    = _print_u64( text, remaining, NULL, value );
		text  += len;

		if ( len >= remaining )
			remaining = min( remaining, 1 );
		else
			remaining -= len;
		arg -= value;

		if ( info->precision < 0 )
			info->precision = 6;

		if ( ( info->flags & ZPL_FMT_ALT ) || info->precision > 0 )
		{
			s64 mult = 10;
			if ( remaining > 1 )
				*text = '.', remaining--;
			text++;
			while ( info->precision-- > 0 )
			{
				value  = zpl_cast( u64 )( arg * mult );
				len    = _print_u64( text, remaining, NULL, value );
				text  += len;
				if ( len >= remaining )
					remaining = min( remaining, 1 );
				else
					remaining -= len;
				arg  -= zpl_cast( f64 ) value / mult;
				mult *= 10;
			}
		}
	}
	else
	{
		if ( remaining > 1 )
			*text = '0', remaining--;
		text++;
		if ( info->flags & ZPL_FMT_ALT )
		{
			if ( remaining > 1 )
				*text = '.', remaining--;
			text++;
		}
	}

	width = info->width - ( text - text_begin );
	if ( width > 0 )
	{
		char  fill = ( info->flags & ZPL_FMT_ZERO ) ? '0' : ' ';
		char* end  = text + remaining - 1;
		len        = ( text - text_begin );

		for ( len = ( text - text_begin ); len--; )
		{
			if ( ( text_begin + len + width ) < end )
				*( text_begin + len + width ) = *( text_begin + len );
		}

		len   = width;
		text += len;
		if ( len >= remaining )
			remaining = min( remaining, 1 );
		else
			remaining -= len;

		while ( len-- )
		{
			if ( text_begin + len < end )
				text_begin[ len ] = fill;
		}
	}

	return ( text - text_begin );
}

ZPL_NEVER_INLINE sw snprintf_va( char* text, sw max_len, char const* fmt, va_list va )
{
	char const* text_begin = text;
	sw          remaining  = max_len, res;

	while ( *fmt )
	{
		_format_info info = { 0 };
		sw           len  = 0;
		info.precision    = -1;

		while ( *fmt && *fmt != '%' && remaining )
			*text++ = *fmt++;

		if ( *fmt == '%' )
		{
			do
			{
				switch ( *++fmt )
				{
					case '-' :
						{
							info.flags |= ZPL_FMT_MINUS;
							break;
						}
					case '+' :
						{
							info.flags |= ZPL_FMT_PLUS;
							break;
						}
					case '#' :
						{
							info.flags |= ZPL_FMT_ALT;
							break;
						}
					case ' ' :
						{
							info.flags |= ZPL_FMT_SPACE;
							break;
						}
					case '0' :
						{
							info.flags |= ( ZPL_FMT_ZERO | ZPL_FMT_WIDTH );
							break;
						}
					default :
						{
							info.flags |= ZPL_FMT_DONE;
							break;
						}
				}
			} while ( ! ( info.flags & ZPL_FMT_DONE ) );
		}

		// NOTE: Optional Width
		if ( *fmt == '*' )
		{
			int width = va_arg( va, int );
			if ( width < 0 )
			{
				info.flags |= ZPL_FMT_MINUS;
				info.width  = -width;
			}
			else
			{
				info.width = width;
			}
			info.flags |= ZPL_FMT_WIDTH;
			fmt++;
		}
		else
		{
			info.width = zpl_cast( s32 ) str_to_i64( fmt, zpl_cast( char** ) & fmt, 10 );
			if ( info.width != 0 )
			{
				info.flags |= ZPL_FMT_WIDTH;
			}
		}

		// NOTE: Optional Precision
		if ( *fmt == '.' )
		{
			fmt++;
			if ( *fmt == '*' )
			{
				info.precision = va_arg( va, int );
				fmt++;
			}
			else
			{
				info.precision = zpl_cast( s32 ) str_to_i64( fmt, zpl_cast( char** ) & fmt, 10 );
			}
			info.flags &= ~ZPL_FMT_ZERO;
		}

		switch ( *fmt++ )
		{
			case 'h' :
				if ( *fmt == 'h' )
				{    // hh => char
					info.flags |= ZPL_FMT_CHAR;
					fmt++;
				}
				else
				{    // h => short
					info.flags |= ZPL_FMT_SHORT;
				}
				break;

			case 'l' :
				if ( *fmt == 'l' )
				{    // ll => long long
					info.flags |= ZPL_FMT_LLONG;
					fmt++;
				}
				else
				{    // l => long
					info.flags |= ZPL_FMT_LONG;
				}
				break;

				break;

			case 'z' :    // NOTE: zpl_usize
				info.flags |= ZPL_FMT_UNSIGNED;
				// fallthrough
			case 't' :    // NOTE: zpl_isize
				info.flags |= ZPL_FMT_SIZE;
				break;

			default :
				fmt--;
				break;
		}

		switch ( *fmt )
		{
			case 'u' :
				info.flags |= ZPL_FMT_UNSIGNED;
				// fallthrough
			case 'd' :
			case 'i' :
				info.base = 10;
				break;

			case 'o' :
				info.base = 8;
				break;

			case 'x' :
				info.base   = 16;
				info.flags |= ( ZPL_FMT_UNSIGNED | ZPL_FMT_LOWER );
				break;

			case 'X' :
				info.base   = 16;
				info.flags |= ( ZPL_FMT_UNSIGNED | ZPL_FMT_UPPER );
				break;

			case 'f' :
			case 'F' :
			case 'g' :
			case 'G' :
				len = _print_f64( text, remaining, &info, 0, va_arg( va, f64 ) );
				break;

			case 'a' :
			case 'A' :
				len = _print_f64( text, remaining, &info, 1, va_arg( va, f64 ) );
				break;

			case 'c' :
				len = _print_char( text, remaining, &info, zpl_cast( char ) va_arg( va, int ) );
				break;

			case 's' :
				len = _print_string( text, remaining, &info, va_arg( va, char* ) );
				break;

			case 'r' :
				len = _print_repeated_char( text, remaining, &info, va_arg( va, int ) );
				break;

			case 'p' :
				info.base   = 16;
				info.flags |= ( ZPL_FMT_LOWER | ZPL_FMT_UNSIGNED | ZPL_FMT_ALT | ZPL_FMT_INTPTR );
				break;

			case '%' :
				len = _print_char( text, remaining, &info, '%' );
				break;

			default :
				fmt--;
				break;
		}

		fmt++;

		if ( info.base != 0 )
		{
			if ( info.flags & ZPL_FMT_UNSIGNED )
			{
				u64 value = 0;
				switch ( info.flags & ZPL_FMT_INTS )
				{
					case ZPL_FMT_CHAR :
						value = zpl_cast( u64 ) zpl_cast( u8 ) va_arg( va, int );
						break;
					case ZPL_FMT_SHORT :
						value = zpl_cast( u64 ) zpl_cast( u16 ) va_arg( va, int );
						break;
					case ZPL_FMT_LONG :
						value = zpl_cast( u64 ) va_arg( va, unsigned long );
						break;
					case ZPL_FMT_LLONG :
						value = zpl_cast( u64 ) va_arg( va, unsigned long long );
						break;
					case ZPL_FMT_SIZE :
						value = zpl_cast( u64 ) va_arg( va, uw );
						break;
					case ZPL_FMT_INTPTR :
						value = zpl_cast( u64 ) va_arg( va, uptr );
						break;
					default :
						value = zpl_cast( u64 ) va_arg( va, unsigned int );
						break;
				}

				len = _print_u64( text, remaining, &info, value );
			}
			else
			{
				s64 value = 0;
				switch ( info.flags & ZPL_FMT_INTS )
				{
					case ZPL_FMT_CHAR :
						value = zpl_cast( s64 ) zpl_cast( s8 ) va_arg( va, int );
						break;
					case ZPL_FMT_SHORT :
						value = zpl_cast( s64 ) zpl_cast( s16 ) va_arg( va, int );
						break;
					case ZPL_FMT_LONG :
						value = zpl_cast( s64 ) va_arg( va, long );
						break;
					case ZPL_FMT_LLONG :
						value = zpl_cast( s64 ) va_arg( va, long long );
						break;
					case ZPL_FMT_SIZE :
						value = zpl_cast( s64 ) va_arg( va, uw );
						break;
					case ZPL_FMT_INTPTR :
						value = zpl_cast( s64 ) va_arg( va, uptr );
						break;
					default :
						value = zpl_cast( s64 ) va_arg( va, int );
						break;
				}

				len = _print_i64( text, remaining, &info, value );
			}
		}

		text += len;
		if ( len >= remaining )
			remaining = min( remaining, 1 );
		else
			remaining -= len;
	}

	*text++ = '\0';
	res     = ( text - text_begin );
	return ( res >= max_len || res < 0 ) ? -1 : res;
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: source/core/random.c


ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

#if defined( ZPL_MODULE_THREADING )
global atomic32 _random_shared_counter = { 0 };
#else
global s32 _random_shared_counter = 0;
#endif

internal u32 _get_noise_from_time( void )
{
	u32 accum = 0;
	f64 start, remaining, end, curr = 0;
	u64 interval = 100000ll;

	start     = time_rel();
	remaining = ( interval - zpl_cast( u64 )( interval * start ) % interval ) / zpl_cast( f64 ) interval;
	end       = start + remaining;

	do
	{
		curr   = time_rel();
		accum += zpl_cast( u32 ) curr;
	} while ( curr >= end );
	return accum;
}

// NOTE: Partly from http://preshing.com/20121224/how-to-generate-a-sequence-of-unique-random-integers/
// But the generation is even more random-er-est

internal ZPL_ALWAYS_INLINE u32 _permute_qpr( u32 x )
{
	local_persist u32 const prime = 4294967291;    // 2^32 - 5
	if ( x >= prime )
	{
		return x;
	}
	else
	{
		u32 residue = zpl_cast( u32 )( zpl_cast( u64 ) x * x ) % prime;
		if ( x <= prime / 2 )
			return residue;
		else
			return prime - residue;
	}
}

internal ZPL_ALWAYS_INLINE u32 _permute_with_offset( u32 x, u32 offset )
{
	return ( _permute_qpr( x ) + offset ) ^ 0x5bf03635;
}

void random_init( random* r )
{
	u64 time, tick;
	sw  i, j;
	u32 x    = 0;
	r->value = 0;

	r->offsets[ 0 ] = _get_noise_from_time();
#ifdef ZPL_MODULE_THREADING
	r->offsets[ 1 ] = atomic32_fetch_add( &_random_shared_counter, 1 );
	r->offsets[ 2 ] = thread_current_id();
	r->offsets[ 3 ] = thread_current_id() * 3 + 1;
#else
	r->offsets[ 1 ] = _random_shared_counter++;
	r->offsets[ 2 ] = 0;
	r->offsets[ 3 ] = 1;
#endif
	time            = time_tz_ms();
	r->offsets[ 4 ] = zpl_cast( u32 )( time >> 32 );
	r->offsets[ 5 ] = zpl_cast( u32 ) time;
	r->offsets[ 6 ] = _get_noise_from_time();
	tick            = rdtsc();
	r->offsets[ 7 ] = zpl_cast( u32 )( tick ^ ( tick >> 32 ) );

	for ( j = 0; j < 4; j++ )
	{
		for ( i = 0; i < count_of( r->offsets ); i++ )
		{
			r->offsets[ i ] = x = _permute_with_offset( x, r->offsets[ i ] );
		}
	}
}

u32 random_gen_u32( random* r )
{
	u32 x     = r->value;
	u32 carry = 1;
	sw  i;
	for ( i = 0; i < count_of( r->offsets ); i++ )
	{
		x = _permute_with_offset( x, r->offsets[ i ] );
		if ( carry > 0 )
		{
			carry = ++r->offsets[ i ] ? 0 : 1;
		}
	}

	r->value = x;
	return x;
}

u32 random_gen_u32_unique( random* r )
{
	u32 x = r->value;
	sw  i;
	r->value++;
	for ( i = 0; i < count_of( r->offsets ); i++ )
	{
		x = _permute_with_offset( x, r->offsets[ i ] );
	}

	return x;
}

u64 random_gen_u64( random* r )
{
	return ( ( zpl_cast( u64 ) random_gen_u32( r ) ) << 32 ) | random_gen_u32( r );
}

sw random_gen_isize( random* r )
{
#if defined( ZPL_ARCH_32_BIT )
	u32 u = random_gen_u32( r );
#else
	u64 u           = random_gen_u64( r );
#endif
	sw i;
	memcopy( &i, &u, size_of( u ) );
	return i;
}

s64 random_range_i64( random* r, s64 lower_inc, s64 higher_inc )
{
	u64 u     = random_gen_u64( r );
	s64 diff  = higher_inc - lower_inc + 1;
	u        %= diff;
	s64 i;
	memcopy( &i, &u, size_of( u ) );
	i += lower_inc;
	return i;
}

sw random_range_isize( random* r, sw lower_inc, sw higher_inc )
{
#if defined( ZPL_ARCH_32_BIT )
	u32 u = random_gen_u32( r );
#else
	u64 u           = random_gen_u64( r );
#endif
	sw diff  = higher_inc - lower_inc + 1;
	u       %= diff;
	sw i;
	memcopy( &i, &u, size_of( u ) );
	i += lower_inc;
	return i;
}

ZPL_ALWAYS_INLINE f64 _random_copy_sign64( f64 x, f64 y )
{
	s64 ix = 0, iy = 0;
	memcopy( &ix, &x, size_of( s64 ) );
	memcopy( &iy, &y, size_of( s64 ) );

	ix &= 0x7fffffffffffffff;
	ix |= iy & 0x8000000000000000;

	f64 r = 0.0;
	memcopy( &r, &ix, size_of( f64 ) );
	return r;
}

f64 random_range_f64( random* r, f64 lower_inc, f64 higher_inc )
{
	f64 f    = zpl_cast( f64 ) random_gen_u64( r ) / zpl_cast( f64 ) ZPL_U64_MAX;
	f64 diff = higher_inc - lower_inc;

	f *= diff;
	f += lower_inc;
	return f;
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: source/core/sort.c


ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

#define ZPL__COMPARE_PROC( Type )                                                           \
	global sw Type##__cmp_offset;                                                           \
	ZPL_COMPARE_PROC( Type##__cmp )                                                         \
	{                                                                                       \
		Type const p = *zpl_cast( Type const* ) pointer_add_const( a, Type##__cmp_offset ); \
		Type const q = *zpl_cast( Type const* ) pointer_add_const( b, Type##__cmp_offset ); \
		return p < q ? -1 : p > q;                                                          \
	}                                                                                       \
	ZPL_COMPARE_PROC_PTR( Type##_cmp( sw offset ) )                                         \
	{                                                                                       \
		Type##__cmp_offset = offset;                                                        \
		return &Type##__cmp;                                                                \
	}

ZPL__COMPARE_PROC( u8 );
ZPL__COMPARE_PROC( s16 );
ZPL__COMPARE_PROC( s32 );
ZPL__COMPARE_PROC( s64 );
ZPL__COMPARE_PROC( sw );
ZPL__COMPARE_PROC( f32 );
ZPL__COMPARE_PROC( f64 );

// NOTE: str_cmp is special as it requires a funny type and funny comparison
global sw _str_cmp_offset;

ZPL_COMPARE_PROC( _str_cmp )
{
	char const* p = *zpl_cast( char const** ) pointer_add_const( a, _str_cmp_offset );
	char const* q = *zpl_cast( char const** ) pointer_add_const( b, _str_cmp_offset );
	return str_compare( p, q );
}

ZPL_COMPARE_PROC_PTR( str_cmp( sw offset ) )
{
	_str_cmp_offset = offset;
	return &_str_cmp;
}

#undef ZPL__COMPARE_PROC

// TODO: Make user definable?
#define ZPL__SORT_STACK_SIZE       64
#define _SORT_INSERT_SORT_TRESHOLD 8

#define ZPL__SORT_PUSH( _base, _limit ) \
	do                                  \
	{                                   \
		stack_ptr[ 0 ]  = ( _base );    \
		stack_ptr[ 1 ]  = ( _limit );   \
		stack_ptr      += 2;            \
	} while ( 0 )

#define ZPL__SORT_POP( _base, _limit ) \
	do                                 \
	{                                  \
		stack_ptr  -= 2;               \
		( _base )   = stack_ptr[ 0 ];  \
		( _limit )  = stack_ptr[ 1 ];  \
	} while ( 0 )

void sort( void* base_, sw count, sw size, compare_proc cmp )
{
	u8 *i, *j;
	u8* base      = zpl_cast( u8* ) base_;
	u8* limit     = base + count * size;
	sw  threshold = _SORT_INSERT_SORT_TRESHOLD * size;

	// NOTE: Prepare the stack
	u8*  stack[ ZPL__SORT_STACK_SIZE ] = { 0 };
	u8** stack_ptr                     = stack;

	for ( ;; )
	{
		if ( ( limit - base ) > threshold )
		{
			// NOTE: Quick sort
			i = base + size;
			j = limit - size;

			memswap( ( ( limit - base ) / size / 2 ) * size + base, base, size );
			if ( cmp( i, j ) > 0 )
				memswap( i, j, size );
			if ( cmp( base, j ) > 0 )
				memswap( base, j, size );
			if ( cmp( i, base ) > 0 )
				memswap( i, base, size );

			for ( ;; )
			{
				do
					i += size;
				while ( cmp( i, base ) < 0 );
				do
					j -= size;
				while ( cmp( j, base ) > 0 );
				if ( i > j )
					break;
				memswap( i, j, size );
			}

			memswap( base, j, size );

			if ( j - base > limit - i )
			{
				ZPL__SORT_PUSH( base, j );
				base = i;
			}
			else
			{
				ZPL__SORT_PUSH( i, limit );
				limit = j;
			}
		}
		else
		{
			// NOTE: Insertion sort
			for ( j = base, i = j + size; i < limit; j = i, i += size )
			{
				for ( ; cmp( j, j + size ) > 0; j -= size )
				{
					memswap( j, j + size, size );
					if ( j == base )
						break;
				}
			}

			if ( stack_ptr == stack )
				break;    // NOTE: Sorting is done!
			ZPL__SORT_POP( base, limit );
		}
	}
}

#undef ZPL__SORT_PUSH
#undef ZPL__SORT_POP

#define ZPL_RADIX_SORT_PROC_GEN( Type )                                                  \
	ZPL_RADIX_SORT_PROC( Type )                                                          \
	{                                                                                    \
		Type* source = items;                                                            \
		Type* dest   = temp;                                                             \
		sw    byte_index, i, byte_max = 8 * size_of( Type );                             \
		for ( byte_index = 0; byte_index < byte_max; byte_index += 8 )                   \
		{                                                                                \
			sw offsets[ 256 ] = { 0 };                                                   \
			sw total          = 0;                                                       \
			/* NOTE: First pass - count how many of each key */                          \
			for ( i = 0; i < count; i++ )                                                \
			{                                                                            \
				Type radix_value = source[ i ];                                          \
				Type radix_piece = ( radix_value >> byte_index ) & 0xff;                 \
				offsets[ radix_piece ]++;                                                \
			}                                                                            \
			/* NOTE: Change counts to offsets */                                         \
			for ( i = 0; i < count_of( offsets ); i++ )                                  \
			{                                                                            \
				sw skcount    = offsets[ i ];                                            \
				offsets[ i ]  = total;                                                   \
				total        += skcount;                                                 \
			}                                                                            \
			/* NOTE: Second pass - place elements into the right location */             \
			for ( i = 0; i < count; i++ )                                                \
			{                                                                            \
				Type radix_value                 = source[ i ];                          \
				Type radix_piece                 = ( radix_value >> byte_index ) & 0xff; \
				dest[ offsets[ radix_piece ]++ ] = source[ i ];                          \
			}                                                                            \
			swap( Type*, source, dest );                                                 \
		}                                                                                \
	}

ZPL_RADIX_SORT_PROC_GEN( u8 );
ZPL_RADIX_SORT_PROC_GEN( u16 );
ZPL_RADIX_SORT_PROC_GEN( u32 );
ZPL_RADIX_SORT_PROC_GEN( u64 );

void shuffle( void* base, sw count, sw size )
{
	u8*    a;
	sw     i, j;
	random random;
	random_init( &random );

	a = zpl_cast( u8* ) base + ( count - 1 ) * size;
	for ( i = count; i > 1; i-- )
	{
		j = random_gen_isize( &random ) % i;
		memswap( a, zpl_cast( u8* ) base + j * size, size );
		a -= size;
	}
}

void reverse( void* base, sw count, sw size )
{
	sw i, j = count - 1;
	for ( i = 0; i < j; i++, j++ )
		memswap( zpl_cast( u8* ) base + i * size, zpl_cast( u8* ) base + j * size, size );
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: source/core/string.c

////////////////////////////////////////////////////////////////
//
// Char things
//
//

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

internal sw _scan_zpl_i64( const char* text, s32 base, s64* value )
{
	const char* text_begin = text;
	s64         result     = 0;
	b32         negative   = false;

	if ( *text == '-' )
	{
		negative = true;
		text++;
	}

	if ( base == 16 && str_compare( text, "0x", 2 ) == 0 )
		text += 2;

	for ( ;; )
	{
		s64 v;
		if ( char_is_digit( *text ) )
			v = *text - '0';
		else if ( base == 16 && char_is_hex_digit( *text ) )
			v = hex_digit_to_int( *text );
		else
			break;

		result *= base;
		result += v;
		text++;
	}

	if ( value )
	{
		if ( negative )
			result = -result;
		*value = result;
	}

	return ( text - text_begin );
}

internal sw _scan_zpl_u64( const char* text, s32 base, u64* value )
{
	const char* text_begin = text;
	u64         result     = 0;

	if ( base == 16 && str_compare( text, "0x", 2 ) == 0 )
		text += 2;

	for ( ;; )
	{
		u64 v;
		if ( char_is_digit( *text ) )
			v = *text - '0';
		else if ( base == 16 && char_is_hex_digit( *text ) )
			v = hex_digit_to_int( *text );
		else
		{
			break;
		}

		result *= base;
		result += v;
		text++;
	}

	if ( value )
		*value = result;

	return ( text - text_begin );
}

// TODO: Make better
u64 str_to_u64( const char* str, char** end_ptr, s32 base )
{
	sw  len;
	u64 value = 0;

	if ( ! base )
	{
		if ( ( strlen( str ) > 2 ) && ( str_compare( str, "0x", 2 ) == 0 ) )
			base = 16;
		else
			base = 10;
	}

	len = _scan_zpl_u64( str, base, &value );
	if ( end_ptr )
		*end_ptr = ( char* )str + len;
	return value;
}

s64 str_to_i64( const char* str, char** end_ptr, s32 base )
{
	sw  len;
	s64 value;

	if ( ! base )
	{
		if ( ( strlen( str ) > 2 ) && ( str_compare( str, "0x", 2 ) == 0 ) )
			base = 16;
		else
			base = 10;
	}

	len = _scan_zpl_i64( str, base, &value );
	if ( end_ptr )
		*end_ptr = ( char* )str + len;
	return value;
}

// TODO: Are these good enough for characters?
global const char _num_to_char_table[] =
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "@$";

void i64_to_str( s64 value, char* string, s32 base )
{
	char* buf      = string;
	b32   negative = false;
	u64   v;

	if ( value < 0 )
	{
		negative = true;
		value    = -value;
	}

	v = zpl_cast( u64 ) value;
	if ( v != 0 )
	{
		while ( v > 0 )
		{
			*buf++  = _num_to_char_table[ v % base ];
			v      /= base;
		}
	}
	else
	{
		*buf++ = '0';
	}
	if ( negative )
		*buf++ = '-';
	*buf = '\0';
	strrev( string );
}

void u64_to_str( u64 value, char* string, s32 base )
{
	char* buf = string;

	if ( value )
	{
		while ( value > 0 )
		{
			*buf++  = _num_to_char_table[ value % base ];
			value  /= base;
		}
	}
	else
	{
		*buf++ = '0';
	}
	*buf = '\0';

	strrev( string );
}

f64 str_to_f64( const char* str, char** end_ptr )
{
	f64 result, value, sign, scale;
	s32 frac;

	while ( char_is_space( *str ) )
	{
		str++;
	}

	sign = 1.0;
	if ( *str == '-' )
	{
		sign = -1.0;
		str++;
	}
	else if ( *str == '+' )
	{
		str++;
	}

	for ( value = 0.0; char_is_digit( *str ); str++ )
	{
		value = value * 10.0 + ( *str - '0' );
	}

	if ( *str == '.' )
	{
		f64 pow10 = 10.0;
		str++;
		while ( char_is_digit( *str ) )
		{
			value += ( *str - '0' ) / pow10;
			pow10 *= 10.0;
			str++;
		}
	}

	frac  = 0;
	scale = 1.0;
	if ( ( *str == 'e' ) || ( *str == 'E' ) )
	{
		u32 exp;

		str++;
		if ( *str == '-' )
		{
			frac = 1;
			str++;
		}
		else if ( *str == '+' )
		{
			str++;
		}

		for ( exp = 0; char_is_digit( *str ); str++ )
		{
			exp = exp * 10 + ( *str - '0' );
		}
		if ( exp > 308 )
			exp = 308;

		while ( exp >= 50 )
		{
			scale *= 1e50;
			exp   -= 50;
		}
		while ( exp >= 8 )
		{
			scale *= 1e8;
			exp   -= 8;
		}
		while ( exp > 0 )
		{
			scale *= 10.0;
			exp   -= 1;
		}
	}

	result = sign * ( frac ? ( value / scale ) : ( value * scale ) );

	if ( end_ptr )
		*end_ptr = zpl_cast( char* ) str;

	return result;
}

////////////////////////////////////////////////////////////////
//
// Windows UTF-8 Handling
//
//

u16* utf8_to_ucs2( u16* buffer, sw len, u8 const* str )
{
	rune c;
	sw   i = 0;
	len--;
	while ( *str )
	{
		if ( i >= len )
			return NULL;
		if ( ! ( *str & 0x80 ) )
		{
			buffer[ i++ ] = *str++;
		}
		else if ( ( *str & 0xe0 ) == 0xc0 )
		{
			if ( *str < 0xc2 )
				return NULL;
			c = ( *str++ & 0x1f ) << 6;
			if ( ( *str & 0xc0 ) != 0x80 )
				return NULL;
			buffer[ i++ ] = zpl_cast( u16 )( c + ( *str++ & 0x3f ) );
		}
		else if ( ( *str & 0xf0 ) == 0xe0 )
		{
			if ( *str == 0xe0 && ( str[ 1 ] < 0xa0 || str[ 1 ] > 0xbf ) )
				return NULL;
			if ( *str == 0xed && str[ 1 ] > 0x9f )    // str[1] < 0x80 is checked below
				return NULL;
			c = ( *str++ & 0x0f ) << 12;
			if ( ( *str & 0xc0 ) != 0x80 )
				return NULL;
			c += ( *str++ & 0x3f ) << 6;
			if ( ( *str & 0xc0 ) != 0x80 )
				return NULL;
			buffer[ i++ ] = zpl_cast( u16 )( c + ( *str++ & 0x3f ) );
		}
		else if ( ( *str & 0xf8 ) == 0xf0 )
		{
			if ( *str > 0xf4 )
				return NULL;
			if ( *str == 0xf0 && ( str[ 1 ] < 0x90 || str[ 1 ] > 0xbf ) )
				return NULL;
			if ( *str == 0xf4 && str[ 1 ] > 0x8f )    // str[1] < 0x80 is checked below
				return NULL;
			c = ( *str++ & 0x07 ) << 18;
			if ( ( *str & 0xc0 ) != 0x80 )
				return NULL;
			c += ( *str++ & 0x3f ) << 12;
			if ( ( *str & 0xc0 ) != 0x80 )
				return NULL;
			c += ( *str++ & 0x3f ) << 6;
			if ( ( *str & 0xc0 ) != 0x80 )
				return NULL;
			c += ( *str++ & 0x3f );
			// UTF-8 encodings of values used in surrogate pairs are invalid
			if ( ( c & 0xfffff800 ) == 0xd800 )
				return NULL;
			if ( c >= 0x10000 )
			{
				c -= 0x10000;
				if ( i + 2 > len )
					return NULL;
				buffer[ i++ ] = 0xd800 | ( 0x3ff & ( c >> 10 ) );
				buffer[ i++ ] = 0xdc00 | ( 0x3ff & ( c ) );
			}
		}
		else
		{
			return NULL;
		}
	}
	buffer[ i ] = 0;
	return buffer;
}

u8* ucs2_to_utf8( u8* buffer, sw len, u16 const* str )
{
	sw i = 0;
	len--;
	while ( *str )
	{
		if ( *str < 0x80 )
		{
			if ( i + 1 > len )
				return NULL;
			buffer[ i++ ] = ( char )*str++;
		}
		else if ( *str < 0x800 )
		{
			if ( i + 2 > len )
				return NULL;
			buffer[ i++ ]  = zpl_cast( char )( 0xc0 + ( *str >> 6 ) );
			buffer[ i++ ]  = zpl_cast( char )( 0x80 + ( *str & 0x3f ) );
			str           += 1;
		}
		else if ( *str >= 0xd800 && *str < 0xdc00 )
		{
			rune c;
			if ( i + 4 > len )
				return NULL;
			c              = ( ( str[ 0 ] - 0xd800 ) << 10 ) + ( ( str[ 1 ] ) - 0xdc00 ) + 0x10000;
			buffer[ i++ ]  = zpl_cast( char )( 0xf0 + ( c >> 18 ) );
			buffer[ i++ ]  = zpl_cast( char )( 0x80 + ( ( c >> 12 ) & 0x3f ) );
			buffer[ i++ ]  = zpl_cast( char )( 0x80 + ( ( c >> 6 ) & 0x3f ) );
			buffer[ i++ ]  = zpl_cast( char )( 0x80 + ( ( c )&0x3f ) );
			str           += 2;
		}
		else if ( *str >= 0xdc00 && *str < 0xe000 )
		{
			return NULL;
		}
		else
		{
			if ( i + 3 > len )
				return NULL;
			buffer[ i++ ]  = 0xe0 + ( *str >> 12 );
			buffer[ i++ ]  = 0x80 + ( ( *str >> 6 ) & 0x3f );
			buffer[ i++ ]  = 0x80 + ( ( *str ) & 0x3f );
			str           += 1;
		}
	}
	buffer[ i ] = 0;
	return buffer;
}

u16* utf8_to_ucs2_buf( u8 const* str )
{    // NOTE: Uses locally persisting buffer
	local_persist u16 buf[ 4096 ];
	return utf8_to_ucs2( buf, count_of( buf ), str );
}

u8* ucs2_to_utf8_buf( u16 const* str )
{    // NOTE: Uses locally persisting buffer
	local_persist u8 buf[ 4096 ];
	return ucs2_to_utf8( buf, count_of( buf ), str );
}

global u8 const _utf8_first[ 256 ] = {
	0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,    // 0x00-0x0F
	0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,    // 0x10-0x1F
	0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,    // 0x20-0x2F
	0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,    // 0x30-0x3F
	0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,    // 0x40-0x4F
	0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,    // 0x50-0x5F
	0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,    // 0x60-0x6F
	0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,    // 0x70-0x7F
	0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1,    // 0x80-0x8F
	0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1,    // 0x90-0x9F
	0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1,    // 0xA0-0xAF
	0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1,    // 0xB0-0xBF
	0xf1, 0xf1, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,    // 0xC0-0xCF
	0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,    // 0xD0-0xDF
	0x13, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x23, 0x03, 0x03,    // 0xE0-0xEF
	0x34, 0x04, 0x04, 0x04, 0x44, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1,    // 0xF0-0xFF
};

typedef struct utf8_accept_range
{
	u8 lo, hi;
} utf8_accept_range;

global utf8_accept_range const _utf8_accept_ranges[] = {
	{0x80, 0xbf},
    {0xa0, 0xbf},
    {0x80, 0x9f},
    {0x90, 0xbf},
    {0x80, 0x8f},
};

sw utf8_decode( u8 const* str, sw str_len, rune* codepoint_out )
{

	sw   width     = 0;
	rune codepoint = ZPL_RUNE_INVALID;

	if ( str_len > 0 )
	{
		u8                s0 = str[ 0 ];
		u8                x  = _utf8_first[ s0 ], sz;
		u8                b1, b2, b3;
		utf8_accept_range accept;
		if ( x >= 0xf0 )
		{
			rune mask = ( zpl_cast( rune ) x << 31 ) >> 31;
			codepoint = ( zpl_cast( rune ) s0 & ( ~mask ) ) | ( ZPL_RUNE_INVALID & mask );
			width     = 1;
			goto end;
		}
		if ( s0 < 0x80 )
		{
			codepoint = s0;
			width     = 1;
			goto end;
		}

		sz     = x & 7;
		accept = _utf8_accept_ranges[ x >> 4 ];
		if ( str_len < sz )
			goto invalid_codepoint;

		b1 = str[ 1 ];
		if ( b1 < accept.lo || accept.hi < b1 )
			goto invalid_codepoint;

		if ( sz == 2 )
		{
			codepoint = ( zpl_cast( rune ) s0 & 0x1f ) << 6 | ( zpl_cast( rune ) b1 & 0x3f );
			width     = 2;
			goto end;
		}

		b2 = str[ 2 ];
		if ( ! is_between( b2, 0x80, 0xbf ) )
			goto invalid_codepoint;

		if ( sz == 3 )
		{
			codepoint = ( zpl_cast( rune ) s0 & 0x1f ) << 12 | ( zpl_cast( rune ) b1 & 0x3f ) << 6 | ( zpl_cast( rune ) b2 & 0x3f );
			width     = 3;
			goto end;
		}

		b3 = str[ 3 ];
		if ( ! is_between( b3, 0x80, 0xbf ) )
			goto invalid_codepoint;

		codepoint = ( zpl_cast( rune ) s0 & 0x07 ) << 18 | ( zpl_cast( rune ) b1 & 0x3f ) << 12 | ( zpl_cast( rune ) b2 & 0x3f ) << 6 | ( zpl_cast( rune ) b3 & 0x3f );
		width     = 4;
		goto end;

	invalid_codepoint:
		codepoint = ZPL_RUNE_INVALID;
		width     = 1;
	}

end:
	if ( codepoint_out )
		*codepoint_out = codepoint;
	return width;
}

sw utf8_codepoint_size( u8 const* str, sw str_len )
{
	sw i = 0;
	for ( ; i < str_len && str[ i ]; i++ )
	{
		if ( ( str[ i ] & 0xc0 ) != 0x80 )
			break;
	}
	return i + 1;
}

sw utf8_encode_rune( u8 buf[ 4 ], rune r )
{
	u32 i    = zpl_cast( u32 ) r;
	u8  mask = 0x3f;
	if ( i <= ( 1 << 7 ) - 1 )
	{
		buf[ 0 ] = zpl_cast( u8 ) r;
		return 1;
	}
	if ( i <= ( 1 << 11 ) - 1 )
	{
		buf[ 0 ] = 0xc0 | zpl_cast( u8 )( r >> 6 );
		buf[ 1 ] = 0x80 | ( zpl_cast( u8 )( r ) & mask );
		return 2;
	}

	// Invalid or Surrogate range
	if ( i > ZPL_RUNE_MAX || is_between( i, 0xd800, 0xdfff ) )
	{
		r = ZPL_RUNE_INVALID;

		buf[ 0 ] = 0xe0 | zpl_cast( u8 )( r >> 12 );
		buf[ 1 ] = 0x80 | ( zpl_cast( u8 )( r >> 6 ) & mask );
		buf[ 2 ] = 0x80 | ( zpl_cast( u8 )( r ) & mask );
		return 3;
	}

	if ( i <= ( 1 << 16 ) - 1 )
	{
		buf[ 0 ] = 0xe0 | zpl_cast( u8 )( r >> 12 );
		buf[ 1 ] = 0x80 | ( zpl_cast( u8 )( r >> 6 ) & mask );
		buf[ 2 ] = 0x80 | ( zpl_cast( u8 )( r ) & mask );
		return 3;
	}

	buf[ 0 ] = 0xf0 | zpl_cast( u8 )( r >> 18 );
	buf[ 1 ] = 0x80 | ( zpl_cast( u8 )( r >> 12 ) & mask );
	buf[ 2 ] = 0x80 | ( zpl_cast( u8 )( r >> 6 ) & mask );
	buf[ 3 ] = 0x80 | ( zpl_cast( u8 )( r ) & mask );
	return 4;
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: source/core/stringlib.c


ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

String string_make_reserve( AllocatorInfo a, sw capacity )
{
	sw    header_size = size_of( StringHeader );
	void* ptr         = alloc( a, header_size + capacity + 1 );

	String        str;
	StringHeader* header;

	if ( ptr == NULL )
		return NULL;
	zero_size( ptr, header_size + capacity + 1 );

	str               = zpl_cast( char* ) ptr + header_size;
	header            = ZPL_STRING_HEADER( str );
	header->allocator = a;
	header->length    = 0;
	header->capacity  = capacity;
	str[ capacity ]   = '\0';

	return str;
}

String string_make_length( AllocatorInfo a, void const* init_str, sw num_bytes )
{
	sw    header_size = size_of( StringHeader );
	void* ptr         = alloc( a, header_size + num_bytes + 1 );

	String        str;
	StringHeader* header;

	if ( ptr == NULL )
		return NULL;
	if ( ! init_str )
		zero_size( ptr, header_size + num_bytes + 1 );

	str               = zpl_cast( char* ) ptr + header_size;
	header            = ZPL_STRING_HEADER( str );
	header->allocator = a;
	header->length    = num_bytes;
	header->capacity  = num_bytes;
	if ( num_bytes && init_str )
		memcopy( str, init_str, num_bytes );
	str[ num_bytes ] = '\0';

	return str;
}

String string_sprintf_buf( AllocatorInfo a, const char* fmt, ... )
{
	local_persist thread_local char buf[ ZPL_PRINTF_MAXLEN ] = { 0 };
	va_list                         va;
	va_start( va, fmt );
	snprintf_va( buf, ZPL_PRINTF_MAXLEN, fmt, va );
	va_end( va );

	return string_make( a, buf );
}

String string_sprintf( AllocatorInfo a, char* buf, sw num_bytes, const char* fmt, ... )
{
	va_list va;
	va_start( va, fmt );
	snprintf_va( buf, num_bytes, fmt, va );
	va_end( va );

	return string_make( a, buf );
}

String string_append_length( String str, void const* other, sw other_len )
{
	if ( other_len > 0 )
	{
		sw curr_len = string_length( str );

		str = string_make_space_for( str, other_len );
		if ( str == NULL )
			return NULL;

		memcopy( str + curr_len, other, other_len );
		str[ curr_len + other_len ] = '\0';
		_set_string_length( str, curr_len + other_len );
	}
	return str;
}

ZPL_ALWAYS_INLINE String string_appendc( String str, const char* other )
{
	return string_append_length( str, other, strlen( other ) );
}

ZPL_ALWAYS_INLINE String string_join( AllocatorInfo a, const char** parts, sw count, const char* glue )
{
	String ret;
	sw     i;

	ret = string_make( a, NULL );

	for ( i = 0; i < count; ++i )
	{
		ret = string_appendc( ret, parts[ i ] );

		if ( ( i + 1 ) < count )
		{
			ret = string_appendc( ret, glue );
		}
	}

	return ret;
}

String string_set( String str, const char* cstr )
{
	sw len = strlen( cstr );
	if ( string_capacity( str ) < len )
	{
		str = string_make_space_for( str, len - string_length( str ) );
		if ( str == NULL )
			return NULL;
	}

	memcopy( str, cstr, len );
	str[ len ] = '\0';
	_set_string_length( str, len );

	return str;
}

String string_make_space_for( String str, sw add_len )
{
	sw available = string_available_space( str );

	// NOTE: Return if there is enough space left
	if ( available >= add_len )
	{
		return str;
	}
	else
	{
		sw            new_len, old_size, new_size;
		void *        ptr, *new_ptr;
		AllocatorInfo a = ZPL_STRING_HEADER( str )->allocator;
		StringHeader* header;

		new_len  = string_length( str ) + add_len;
		ptr      = ZPL_STRING_HEADER( str );
		old_size = size_of( StringHeader ) + string_length( str ) + 1;
		new_size = size_of( StringHeader ) + new_len + 1;

		new_ptr = resize( a, ptr, old_size, new_size );
		if ( new_ptr == NULL )
			return NULL;

		header            = zpl_cast( StringHeader* ) new_ptr;
		header->allocator = a;

		str = zpl_cast( String )( header + 1 );
		_set_string_capacity( str, new_len );

		return str;
	}
}

sw string_allocation_size( String const str )
{
	sw cap = string_capacity( str );
	return size_of( StringHeader ) + cap;
}

b32 string_are_equal( String const lhs, String const rhs )
{
	sw lhs_len, rhs_len, i;
	lhs_len = string_length( lhs );
	rhs_len = string_length( rhs );
	if ( lhs_len != rhs_len )
		return false;

	for ( i = 0; i < lhs_len; i++ )
	{
		if ( lhs[ i ] != rhs[ i ] )
			return false;
	}

	return true;
}

String string_trim( String str, const char* cut_set )
{
	char *start, *end, *start_pos, *end_pos;
	sw    len;

	start_pos = start = str;
	end_pos = end = str + string_length( str ) - 1;

	while ( start_pos <= end && char_first_occurence( cut_set, *start_pos ) )
		start_pos++;
	while ( end_pos > start_pos && char_first_occurence( cut_set, *end_pos ) )
		end_pos--;

	len = zpl_cast( sw )( ( start_pos > end_pos ) ? 0 : ( ( end_pos - start_pos ) + 1 ) );

	if ( str != start_pos )
		memmove( str, start_pos, len );
	str[ len ] = '\0';

	_set_string_length( str, len );

	return str;
}

String string_append_rune( String str, rune r )
{
	if ( r >= 0 )
	{
		u8 buf[ 8 ] = { 0 };
		sw len      = utf8_encode_rune( buf, r );
		return string_append_length( str, buf, len );
	}

	return str;
}

String string_append_fmt( String str, const char* fmt, ... )
{
	sw      res;
	char    buf[ ZPL_PRINTF_MAXLEN ] = { 0 };
	va_list va;
	va_start( va, fmt );
	res = snprintf_va( buf, count_of( buf ) - 1, fmt, va ) - 1;
	va_end( va );
	return string_append_length( str, buf, res );
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: source/core/time.c


#if defined( ZPL_SYSTEM_MACOS ) || ZPL_SYSTEM_UNIX
#	include <sys/time.h>
#	include <time.h>
#endif

#if defined( ZPL_SYSTEM_MACOS )
#	include <mach/clock.h>
#	include <mach/mach.h>
#	include <mach/mach_time.h>
#endif

#if defined( ZPL_SYSTEM_EMSCRIPTEN )
#	include <emscripten.h>
#endif

#if defined( ZPL_SYSTEM_WINDOWS )
#	include <timezoneapi.h>
#endif

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

//! @}
//$$
////////////////////////////////////////////////////////////////
//
// Time
//
//

#if defined( ZPL_COMPILER_MSVC ) && ! defined( __clang__ )
u64 rdtsc( void )
{
	return __rdtsc();
}
#elif defined( __i386__ )
u64 rdtsc( void )
{
	u64 x;
	__asm__ volatile( ".byte 0x0f, 0x31" : "=A"( x ) );
	return x;
}
#elif defined( __x86_64__ )
u64 rdtsc( void )
{
	u32 hi, lo;
	__asm__ __volatile__( "rdtsc" : "=a"( lo ), "=d"( hi ) );
	return ( zpl_cast( u64 ) lo ) | ( ( zpl_cast( u64 ) hi ) << 32 );
}
#elif defined( __powerpc__ )
u64 rdtsc( void )
{
	u64 result = 0;
	u32 upper, lower, tmp;
	__asm__ volatile(
	    "0:                   \n"
	    "\tmftbu   %0         \n"
	    "\tmftb    %1         \n"
	    "\tmftbu   %2         \n"
	    "\tcmpw    %2,%0      \n"
	    "\tbne     0b         \n"
	    : "=r"( upper ), "=r"( lower ), "=r"( tmp )
	);
	result = upper;
	result = result << 32;
	result = result | lower;

	return result;
}
#elif defined( ZPL_SYSTEM_EMSCRIPTEN )
u64 rdtsc( void )
{
	return ( u64 )( emscripten_get_now() * 1e+6 );
}
#elif defined( ZPL_CPU_ARM ) && ! defined( ZPL_COMPILER_TINYC )
u64 rdtsc( void )
{
#	if defined( __aarch64__ )
	int64_t r = 0;
	asm volatile( "mrs %0, cntvct_el0" : "=r"( r ) );
#	elif ( __ARM_ARCH >= 6 )
	uint32_t r = 0;
	uint32_t pmccntr;
	uint32_t pmuseren;
	uint32_t pmcntenset;

	// Read the user mode perf monitor counter access permissions.
	asm volatile( "mrc p15, 0, %0, c9, c14, 0" : "=r"( pmuseren ) );
	if ( pmuseren & 1 )
	{    // Allows reading perfmon counters for user mode code.
		asm volatile( "mrc p15, 0, %0, c9, c12, 1" : "=r"( pmcntenset ) );
		if ( pmcntenset & 0x80000000ul )
		{    // Is it counting?
			asm volatile( "mrc p15, 0, %0, c9, c13, 0" : "=r"( pmccntr ) );
			// The counter is set up to count every 64th cycle
			return ( ( int64_t )pmccntr ) * 64;    // Should optimize to << 6
		}
	}
#	else
#		error "No suitable method for rdtsc for this cpu type"
#	endif

	return r;
}
#else
u64 rdtsc( void )
{
	ZPL_PANIC( "rdtsc is not supported on this particular setup" );
	return -0;
}
#endif

#if defined( ZPL_SYSTEM_WINDOWS ) || defined( ZPL_SYSTEM_CYGWIN )

u64 time_rel_ms( void )
{
	local_persist LARGE_INTEGER win32_perf_count_freq = { 0 };
	u64                         result;
	LARGE_INTEGER               counter;
	local_persist LARGE_INTEGER win32_perf_counter = { 0 };
	if ( ! win32_perf_count_freq.QuadPart )
	{
		QueryPerformanceFrequency( &win32_perf_count_freq );
		ZPL_ASSERT( win32_perf_count_freq.QuadPart != 0 );
		QueryPerformanceCounter( &win32_perf_counter );
	}

	QueryPerformanceCounter( &counter );

	result = ( counter.QuadPart - win32_perf_counter.QuadPart ) * 1000 / ( win32_perf_count_freq.QuadPart );
	return result;
}

u64 time_utc_ms( void )
{
	FILETIME       ft;
	ULARGE_INTEGER li;

	GetSystemTimeAsFileTime( &ft );
	li.LowPart  = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;

	return li.QuadPart / 1000;
}

u64 time_tz_ms( void )
{
	FILETIME       ft;
	SYSTEMTIME     st, lst;
	ULARGE_INTEGER li;

	GetSystemTime( &st );
	SystemTimeToTzSpecificLocalTime( NULL, &st, &lst );
	SystemTimeToFileTime( &lst, &ft );
	li.LowPart  = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;

	return li.QuadPart / 1000;
}

void sleep_ms( u32 ms )
{
	Sleep( ms );
}

#else

#	if defined( ZPL_SYSTEM_LINUX ) || defined( ZPL_SYSTEM_FREEBSD ) || defined( ZPL_SYSTEM_OPENBSD ) || defined( ZPL_SYSTEM_EMSCRIPTEN )
u64 _unix_gettime( void )
{
	struct timespec t;
	u64             result;

	clock_gettime( 1 /*CLOCK_MONOTONIC*/, &t );
	result = 1000 * t.tv_sec + 1.0e-6 * t.tv_nsec;
	return result;
}
#	endif

u64 time_rel_ms( void )
{
#	if defined( ZPL_SYSTEM_OSX )
	u64 result;

	local_persist u64 timebase  = 0;
	local_persist u64 timestart = 0;

	if ( ! timestart )
	{
		mach_timebase_info_data_t tb = { 0 };
		mach_timebase_info( &tb );
		timebase   = tb.numer;
		timebase  /= tb.denom;
		timestart  = mach_absolute_time();
	}

	// NOTE: mach_absolute_time() returns things in nanoseconds
	result = 1.0e-6 * ( mach_absolute_time() - timestart ) * timebase;
	return result;
#	else
	local_persist u64 unix_timestart = 0.0;

	if ( ! unix_timestart )
	{
		unix_timestart = _unix_gettime();
	}

	u64 now = _unix_gettime();

	return ( now - unix_timestart );
#	endif
}

u64 time_utc_ms( void )
{
	struct timespec t;
#	if defined( ZPL_SYSTEM_OSX )
	clock_serv_t    cclock;
	mach_timespec_t mts;
	host_get_clock_service( mach_host_self(), CALENDAR_CLOCK, &cclock );
	clock_get_time( cclock, &mts );
	mach_port_deallocate( mach_task_self(), cclock );
	t.tv_sec  = mts.tv_sec;
	t.tv_nsec = mts.tv_nsec;
#	else
	clock_gettime( 0 /*CLOCK_REALTIME*/, &t );
#	endif
	return ( ( u64 )t.tv_sec * 1000 + t.tv_nsec * 1e-6 + ZPL__UNIX_TO_WIN32_EPOCH );
}

void sleep_ms( u32 ms )
{
	struct timespec req = { zpl_cast( time_t )( ms * 1e-3 ), zpl_cast( long )( ( ms % 1000 ) * 1e6 ) };
	struct timespec rem = { 0, 0 };
	nanosleep( &req, &rem );
}

u64 time_tz_ms( void )
{
	struct tm t;
	u64       result  = time_utc_ms() - ZPL__UNIX_TO_WIN32_EPOCH;
	u16       ms      = result % 1000;
	result           *= 1e-3;
	localtime_r( ( const time_t* )&result, &t );
	result = ( u64 )mktime( &t );
	return ( result - timezone + t.tm_isdst * 3600 ) * 1000 + ms + ZPL__UNIX_TO_WIN32_EPOCH;
}
#endif

f64 time_rel( void )
{
	return ( f64 )( time_rel_ms() * 1e-3 );
}

f64 time_utc( void )
{
	return ( f64 )( time_utc_ms() * 1e-3 );
}

f64 time_tz( void )
{
	return ( f64 )( time_tz_ms() * 1e-3 );
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

#			endif
#		endif

#		if defined( ZPL_MODULE_HASHING )
// file: source/hashing.c

////////////////////////////////////////////////////////////////
//
// Hashing functions
//
//

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

u32 adler32( void const* data, sw len )
{
	u32 const MOD_ALDER = 65521;
	u32       a = 1, b = 0;
	sw        i, block_len;
	u8 const* bytes = zpl_cast( u8 const* ) data;

	block_len = len % 5552;

	while ( len )
	{
		for ( i = 0; i + 7 < block_len; i += 8 )
		{
			a += bytes[ 0 ], b += a;
			a += bytes[ 1 ], b += a;
			a += bytes[ 2 ], b += a;
			a += bytes[ 3 ], b += a;
			a += bytes[ 4 ], b += a;
			a += bytes[ 5 ], b += a;
			a += bytes[ 6 ], b += a;
			a += bytes[ 7 ], b += a;

			bytes += 8;
		}
		for ( ; i < block_len; i++ )
			a += *bytes++, b += a;

		a %= MOD_ALDER, b %= MOD_ALDER;
		len       -= block_len;
		block_len  = 5552;
	}

	return ( b << 16 ) | a;
}

global u32 const _crc32_table[ 256 ] = {
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd,
	0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7, 0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
	0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
	0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59, 0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
	0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f,
	0x9fbfe4a5, 0xe8b8d433, 0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65, 0x4db26158, 0x3ab551ce,
	0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
	0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f, 0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
	0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
	0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1, 0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0,
	0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 0xcc0c7795, 0xbb0b4703,
	0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d, 0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
	0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
	0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777, 0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
	0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5,
	0x47b2cf7f, 0x30b5ffe9, 0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d,
};

global u64 const _crc64_table[ 256 ] = {
	0x0000000000000000ull, 0x7ad870c830358979ull, 0xf5b0e190606b12f2ull, 0x8f689158505e9b8bull, 0xc038e5739841b68full, 0xbae095bba8743ff6ull, 0x358804e3f82aa47dull,
	0x4f50742bc81f2d04ull, 0xab28ecb46814fe75ull, 0xd1f09c7c5821770cull, 0x5e980d24087fec87ull, 0x24407dec384a65feull, 0x6b1009c7f05548faull, 0x11c8790fc060c183ull,
	0x9ea0e857903e5a08ull, 0xe478989fa00bd371ull, 0x7d08ff3b88be6f81ull, 0x07d08ff3b88be6f8ull, 0x88b81eabe8d57d73ull, 0xf2606e63d8e0f40aull, 0xbd301a4810ffd90eull,
	0xc7e86a8020ca5077ull, 0x4880fbd87094cbfcull, 0x32588b1040a14285ull, 0xd620138fe0aa91f4ull, 0xacf86347d09f188dull, 0x2390f21f80c18306ull, 0x594882d7b0f40a7full,
	0x1618f6fc78eb277bull, 0x6cc0863448deae02ull, 0xe3a8176c18803589ull, 0x997067a428b5bcf0ull, 0xfa11fe77117cdf02ull, 0x80c98ebf2149567bull, 0x0fa11fe77117cdf0ull,
	0x75796f2f41224489ull, 0x3a291b04893d698dull, 0x40f16bccb908e0f4ull, 0xcf99fa94e9567b7full, 0xb5418a5cd963f206ull, 0x513912c379682177ull, 0x2be1620b495da80eull,
	0xa489f35319033385ull, 0xde51839b2936bafcull, 0x9101f7b0e12997f8ull, 0xebd98778d11c1e81ull, 0x64b116208142850aull, 0x1e6966e8b1770c73ull, 0x8719014c99c2b083ull,
	0xfdc17184a9f739faull, 0x72a9e0dcf9a9a271ull, 0x08719014c99c2b08ull, 0x4721e43f0183060cull, 0x3df994f731b68f75ull, 0xb29105af61e814feull, 0xc849756751dd9d87ull,
	0x2c31edf8f1d64ef6ull, 0x56e99d30c1e3c78full, 0xd9810c6891bd5c04ull, 0xa3597ca0a188d57dull, 0xec09088b6997f879ull, 0x96d1784359a27100ull, 0x19b9e91b09fcea8bull,
	0x636199d339c963f2ull, 0xdf7adabd7a6e2d6full, 0xa5a2aa754a5ba416ull, 0x2aca3b2d1a053f9dull, 0x50124be52a30b6e4ull, 0x1f423fcee22f9be0ull, 0x659a4f06d21a1299ull,
	0xeaf2de5e82448912ull, 0x902aae96b271006bull, 0x74523609127ad31aull, 0x0e8a46c1224f5a63ull, 0x81e2d7997211c1e8ull, 0xfb3aa75142244891ull, 0xb46ad37a8a3b6595ull,
	0xceb2a3b2ba0eececull, 0x41da32eaea507767ull, 0x3b024222da65fe1eull, 0xa2722586f2d042eeull, 0xd8aa554ec2e5cb97ull, 0x57c2c41692bb501cull, 0x2d1ab4dea28ed965ull,
	0x624ac0f56a91f461ull, 0x1892b03d5aa47d18ull, 0x97fa21650afae693ull, 0xed2251ad3acf6feaull, 0x095ac9329ac4bc9bull, 0x7382b9faaaf135e2ull, 0xfcea28a2faafae69ull,
	0x8632586aca9a2710ull, 0xc9622c4102850a14ull, 0xb3ba5c8932b0836dull, 0x3cd2cdd162ee18e6ull, 0x460abd1952db919full, 0x256b24ca6b12f26dull, 0x5fb354025b277b14ull,
	0xd0dbc55a0b79e09full, 0xaa03b5923b4c69e6ull, 0xe553c1b9f35344e2ull, 0x9f8bb171c366cd9bull, 0x10e3202993385610ull, 0x6a3b50e1a30ddf69ull, 0x8e43c87e03060c18ull,
	0xf49bb8b633338561ull, 0x7bf329ee636d1eeaull, 0x012b592653589793ull, 0x4e7b2d0d9b47ba97ull, 0x34a35dc5ab7233eeull, 0xbbcbcc9dfb2ca865ull, 0xc113bc55cb19211cull,
	0x5863dbf1e3ac9decull, 0x22bbab39d3991495ull, 0xadd33a6183c78f1eull, 0xd70b4aa9b3f20667ull, 0x985b3e827bed2b63ull, 0xe2834e4a4bd8a21aull, 0x6debdf121b863991ull,
	0x1733afda2bb3b0e8ull, 0xf34b37458bb86399ull, 0x8993478dbb8deae0ull, 0x06fbd6d5ebd3716bull, 0x7c23a61ddbe6f812ull, 0x3373d23613f9d516ull, 0x49aba2fe23cc5c6full,
	0xc6c333a67392c7e4ull, 0xbc1b436e43a74e9dull, 0x95ac9329ac4bc9b5ull, 0xef74e3e19c7e40ccull, 0x601c72b9cc20db47ull, 0x1ac40271fc15523eull, 0x5594765a340a7f3aull,
	0x2f4c0692043ff643ull, 0xa02497ca54616dc8ull, 0xdafce7026454e4b1ull, 0x3e847f9dc45f37c0ull, 0x445c0f55f46abeb9ull, 0xcb349e0da4342532ull, 0xb1eceec59401ac4bull,
	0xfebc9aee5c1e814full, 0x8464ea266c2b0836ull, 0x0b0c7b7e3c7593bdull, 0x71d40bb60c401ac4ull, 0xe8a46c1224f5a634ull, 0x927c1cda14c02f4dull, 0x1d148d82449eb4c6ull,
	0x67ccfd4a74ab3dbfull, 0x289c8961bcb410bbull, 0x5244f9a98c8199c2ull, 0xdd2c68f1dcdf0249ull, 0xa7f41839ecea8b30ull, 0x438c80a64ce15841ull, 0x3954f06e7cd4d138ull,
	0xb63c61362c8a4ab3ull, 0xcce411fe1cbfc3caull, 0x83b465d5d4a0eeceull, 0xf96c151de49567b7ull, 0x76048445b4cbfc3cull, 0x0cdcf48d84fe7545ull, 0x6fbd6d5ebd3716b7ull,
	0x15651d968d029fceull, 0x9a0d8ccedd5c0445ull, 0xe0d5fc06ed698d3cull, 0xaf85882d2576a038ull, 0xd55df8e515432941ull, 0x5a3569bd451db2caull, 0x20ed197575283bb3ull,
	0xc49581ead523e8c2ull, 0xbe4df122e51661bbull, 0x3125607ab548fa30ull, 0x4bfd10b2857d7349ull, 0x04ad64994d625e4dull, 0x7e7514517d57d734ull, 0xf11d85092d094cbfull,
	0x8bc5f5c11d3cc5c6ull, 0x12b5926535897936ull, 0x686de2ad05bcf04full, 0xe70573f555e26bc4ull, 0x9ddd033d65d7e2bdull, 0xd28d7716adc8cfb9ull, 0xa85507de9dfd46c0ull,
	0x273d9686cda3dd4bull, 0x5de5e64efd965432ull, 0xb99d7ed15d9d8743ull, 0xc3450e196da80e3aull, 0x4c2d9f413df695b1ull, 0x36f5ef890dc31cc8ull, 0x79a59ba2c5dc31ccull,
	0x037deb6af5e9b8b5ull, 0x8c157a32a5b7233eull, 0xf6cd0afa9582aa47ull, 0x4ad64994d625e4daull, 0x300e395ce6106da3ull, 0xbf66a804b64ef628ull, 0xc5bed8cc867b7f51ull,
	0x8aeeace74e645255ull, 0xf036dc2f7e51db2cull, 0x7f5e4d772e0f40a7ull, 0x05863dbf1e3ac9deull, 0xe1fea520be311aafull, 0x9b26d5e88e0493d6ull, 0x144e44b0de5a085dull,
	0x6e963478ee6f8124ull, 0x21c640532670ac20ull, 0x5b1e309b16452559ull, 0xd476a1c3461bbed2ull, 0xaeaed10b762e37abull, 0x37deb6af5e9b8b5bull, 0x4d06c6676eae0222ull,
	0xc26e573f3ef099a9ull, 0xb8b627f70ec510d0ull, 0xf7e653dcc6da3dd4ull, 0x8d3e2314f6efb4adull, 0x0256b24ca6b12f26ull, 0x788ec2849684a65full, 0x9cf65a1b368f752eull,
	0xe62e2ad306bafc57ull, 0x6946bb8b56e467dcull, 0x139ecb4366d1eea5ull, 0x5ccebf68aecec3a1ull, 0x2616cfa09efb4ad8ull, 0xa97e5ef8cea5d153ull, 0xd3a62e30fe90582aull,
	0xb0c7b7e3c7593bd8ull, 0xca1fc72bf76cb2a1ull, 0x45775673a732292aull, 0x3faf26bb9707a053ull, 0x70ff52905f188d57ull, 0x0a2722586f2d042eull, 0x854fb3003f739fa5ull,
	0xff97c3c80f4616dcull, 0x1bef5b57af4dc5adull, 0x61372b9f9f784cd4ull, 0xee5fbac7cf26d75full, 0x9487ca0fff135e26ull, 0xdbd7be24370c7322ull, 0xa10fceec0739fa5bull,
	0x2e675fb4576761d0ull, 0x54bf2f7c6752e8a9ull, 0xcdcf48d84fe75459ull, 0xb71738107fd2dd20ull, 0x387fa9482f8c46abull, 0x42a7d9801fb9cfd2ull, 0x0df7adabd7a6e2d6ull,
	0x772fdd63e7936bafull, 0xf8474c3bb7cdf024ull, 0x829f3cf387f8795dull, 0x66e7a46c27f3aa2cull, 0x1c3fd4a417c62355ull, 0x935745fc4798b8deull, 0xe98f353477ad31a7ull,
	0xa6df411fbfb21ca3ull, 0xdc0731d78f8795daull, 0x536fa08fdfd90e51ull, 0x29b7d047efec8728ull,
};

u32 crc32( void const* data, sw len )
{
	sw        remaining;
	u32       result = ~( zpl_cast( u32 ) 0 );
	u8 const* c      = zpl_cast( u8 const* ) data;
	for ( remaining = len; remaining--; c++ )
		result = ( result >> 8 ) ^ ( _crc32_table[ ( result ^ *c ) & 0xff ] );
	return ~result;
}

u64 crc64( void const* data, sw len )
{
	sw        remaining;
	u64       result = ( zpl_cast( u64 ) 0 );
	u8 const* c      = zpl_cast( u8 const* ) data;
	for ( remaining = len; remaining--; c++ )
		result = ( result >> 8 ) ^ ( _crc64_table[ ( result ^ *c ) & 0xff ] );
	return result;
}

u32 fnv32( void const* data, sw len )
{
	sw        i;
	u32       h = 0x811c9dc5;
	u8 const* c = zpl_cast( u8 const* ) data;

	for ( i = 0; i < len; i++ )
		h = ( h * 0x01000193 ) ^ c[ i ];

	return h;
}

u64 fnv64( void const* data, sw len )
{
	sw        i;
	u64       h = 0xcbf29ce484222325ull;
	u8 const* c = zpl_cast( u8 const* ) data;

	for ( i = 0; i < len; i++ )
		h = ( h * 0x100000001b3ll ) ^ c[ i ];

	return h;
}

u32 fnv32a( void const* data, sw len )
{
	sw        i;
	u32       h = 0x811c9dc5;
	u8 const* c = zpl_cast( u8 const* ) data;

	for ( i = 0; i < len; i++ )
		h = ( h ^ c[ i ] ) * 0x01000193;

	return h;
}

u64 fnv64a( void const* data, sw len )
{
	sw        i;
	u64       h = 0xcbf29ce484222325ull;
	u8 const* c = zpl_cast( u8 const* ) data;

	for ( i = 0; i < len; i++ )
		h = ( h ^ c[ i ] ) * 0x100000001b3ll;

	return h;
}

// base64 implementation based on https://nachtimwald.com/2017/11/18/base64-encode-and-decode-in-c/
//
global u8 _base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


/* generated table based on: */
#if 0
void _base64_decode_table() {
    s32 inv[80];
    sw i;

    memset(inv, -1, size_of(inv));

    for (i=0; i < size_of(_base64_chars)-1; i++) {
        inv[_base64_chars[i]-43] = i;
    }
}
#endif
/* === */
global s32 _base64_dec_table[] = { 62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1, -1, 0,  1,  2,  3,  4,
	                               5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, -1,
	                               26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51 };

sw _base64_encoded_size( sw len )
{
	sw ret = len;

	if ( len % 3 != 0 )
	{
		ret += 3 - ( len % 3 );
	}

	ret /= 3;
	ret *= 4;

	return ret;
}

sw _base64_decoded_size( void const* data )
{
	sw        len, ret, i;
	const u8* s = zpl_cast( const u8* ) data;

	if ( s == NULL )
	{
		return 0;
	}

	len = strlen( zpl_cast( const char* ) s );
	ret = len / 4 * 3;

	for ( i = len; i-- > 0; )
	{
		if ( s[ i ] == '=' )
		{
			ret--;
		}
		else
		{
			break;
		}
	}

	return ret;
}

b32 _base64_valid_char( u8 c )
{
	if ( c >= '0' && c <= '9' )
		return true;
	if ( c >= 'A' && c <= 'Z' )
		return true;
	if ( c >= 'a' && c <= 'z' )
		return true;
	if ( c == '+' || c == '/' || c == '=' )
		return true;

	return false;
}

u8* base64_encode( AllocatorInfo a, void const* data, sw len )
{
	const u8* s   = zpl_cast( const u8* ) data;
	u8*       ret = NULL;
	sw        enc_len, i, j, v;

	if ( data == NULL || len == 0 )
	{
		return NULL;
	}

	enc_len        = _base64_encoded_size( len );
	ret            = zpl_cast( u8* ) alloc( a, enc_len + 1 );
	ret[ enc_len ] = 0;

	for ( i = 0, j = 0; i < len; i += 3, j += 4 )
	{
		v = s[ i ];
		v = ( i + 1 < len ) ? ( v << 8 | s[ i + 1 ] ) : ( v << 8 );
		v = ( i + 2 < len ) ? ( v << 8 | s[ i + 2 ] ) : ( v << 8 );

		ret[ j ]     = _base64_chars[ ( v >> 18 ) & 0x3F ];
		ret[ j + 1 ] = _base64_chars[ ( v >> 12 ) & 0x3F ];

		if ( i + 1 < len )
			ret[ j + 2 ] = _base64_chars[ ( v >> 6 ) & 0x3F ];

		else
			ret[ j + 2 ] = '=';

		if ( i + 2 < len )
			ret[ j + 3 ] = _base64_chars[ v & 0x3F ];

		else
			ret[ j + 3 ] = '=';
	}

	return ret;
}

u8* base64_decode( AllocatorInfo a, void const* data, sw len )
{
	const u8* s   = zpl_cast( const u8* ) data;
	u8*       ret = NULL;
	sw        alen, i, j, v;

	if ( data == NULL )
	{
		return NULL;
	}

	alen = _base64_decoded_size( s );
	ret  = zpl_cast( u8* ) alloc( a, alen + 1 );

	ZPL_ASSERT_NOT_NULL( ret );

	ret[ alen ] = 0;

	for ( i = 0; i < len; i++ )
	{
		if ( ! _base64_valid_char( s[ i ] ) )
			return NULL;
	}

	for ( i = 0, j = 0; i < len; i += 4, j += 3 )
	{
		v = _base64_dec_table[ s[ i ] - 43 ];
		v = ( v << 6 ) | _base64_dec_table[ s[ i + 1 ] - 43 ];
		v = ( s[ i + 2 ] == '=' ) ? ( v << 6 ) : ( ( v << 6 ) | _base64_dec_table[ s[ i + 2 ] - 43 ] );
		v = ( s[ i + 3 ] == '=' ) ? ( v << 6 ) : ( ( v << 6 ) | _base64_dec_table[ s[ i + 3 ] - 43 ] );

		ret[ j ] = ( v >> 16 ) & 0xFF;

		if ( s[ i + 2 ] != '=' )
			ret[ j + 1 ] = ( v >> 8 ) & 0xFF;

		if ( s[ i + 3 ] != '=' )
			ret[ j + 2 ] = v & 0xFF;
	}

	return ret;
}

u32 murmur32_seed( void const* data, sw len, u32 seed )
{
	u32 const c1 = 0xcc9e2d51;
	u32 const c2 = 0x1b873593;
	u32 const r1 = 15;
	u32 const r2 = 13;
	u32 const m  = 5;
	u32 const n  = 0xe6546b64;

	sw         i, nblocks = len / 4;
	u32        hash = seed, k1 = 0;
	u32 const* blocks = zpl_cast( u32 const* ) data;
	u8 const*  tail   = zpl_cast( u8 const* )( data ) + nblocks * 4;

	for ( i = 0; i < nblocks; i++ )
	{
		u32 k  = blocks[ i ];
		k     *= c1;
		k      = ( k << r1 ) | ( k >> ( 32 - r1 ) );
		k     *= c2;

		hash ^= k;
		hash  = ( ( hash << r2 ) | ( hash >> ( 32 - r2 ) ) ) * m + n;
	}

	switch ( len & 3 )
	{
		case 3 :
			k1 ^= tail[ 2 ] << 16;
		case 2 :
			k1 ^= tail[ 1 ] << 8;
		case 1 :
			k1 ^= tail[ 0 ];

			k1   *= c1;
			k1    = ( k1 << r1 ) | ( k1 >> ( 32 - r1 ) );
			k1   *= c2;
			hash ^= k1;
	}

	hash ^= len;
	hash ^= ( hash >> 16 );
	hash *= 0x85ebca6b;
	hash ^= ( hash >> 13 );
	hash *= 0xc2b2ae35;
	hash ^= ( hash >> 16 );

	return hash;
}

u64 murmur64_seed( void const* data_, sw len, u64 seed )
{
	u64 const m = 0xc6a4a7935bd1e995ULL;
	s32 const r = 47;

	u64 h = seed ^ ( len * m );

	u64 const* data  = zpl_cast( u64 const* ) data_;
	u8 const*  data2 = zpl_cast( u8 const* ) data_;
	u64 const* end   = data + ( len / 8 );

	while ( data != end )
	{
		u64 k = *data++;

		k *= m;
		k ^= k >> r;
		k *= m;

		h ^= k;
		h *= m;
	}

	switch ( len & 7 )
	{
		case 7 :
			h ^= zpl_cast( u64 )( data2[ 6 ] ) << 48;
		case 6 :
			h ^= zpl_cast( u64 )( data2[ 5 ] ) << 40;
		case 5 :
			h ^= zpl_cast( u64 )( data2[ 4 ] ) << 32;
		case 4 :
			h ^= zpl_cast( u64 )( data2[ 3 ] ) << 24;
		case 3 :
			h ^= zpl_cast( u64 )( data2[ 2 ] ) << 16;
		case 2 :
			h ^= zpl_cast( u64 )( data2[ 1 ] ) << 8;
		case 1 :
			h ^= zpl_cast( u64 )( data2[ 0 ] );
			h *= m;
	};

	h ^= h >> r;
	h *= m;
	h ^= h >> r;

	return h;
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

#		endif

#		if defined( ZPL_MODULE_REGEX )
// file: source/regex.c


ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

typedef enum zplreOp
{
	ZPL_RE_OP_BEGIN_CAPTURE,
	ZPL_RE_OP_END_CAPTURE,

	ZPL_RE_OP_BEGINNING_OF_LINE,
	ZPL_RE_OP_END_OF_LINE,

	ZPL_RE_OP_EXACT_MATCH,
	ZPL_RE_OP_META_MATCH,

	ZPL_RE_OP_ANY,
	ZPL_RE_OP_ANY_OF,
	ZPL_RE_OP_ANY_BUT,

	ZPL_RE_OP_ZERO_OR_MORE,
	ZPL_RE_OP_ONE_OR_MORE,
	ZPL_RE_OP_ZERO_OR_MORE_SHORTEST,
	ZPL_RE_OP_ONE_OR_MORE_SHORTEST,
	ZPL_RE_OP_ZERO_OR_ONE,

	ZPL_RE_OP_BRANCH_START,
	ZPL_RE_OP_BRANCH_END
} zplreOp;

typedef enum zplreCode
{
	ZPL_RE_CODE_NULL           = 0x0000,
	ZPL_RE_CODE_WHITESPACE     = 0x0100,
	ZPL_RE_CODE_NOT_WHITESPACE = 0x0200,
	ZPL_RE_CODE_DIGIT          = 0x0300,
	ZPL_RE_CODE_NOT_DIGIT      = 0x0400,
	ZPL_RE_CODE_ALPHA          = 0x0500,
	ZPL_RE_CODE_LOWER          = 0x0600,
	ZPL_RE_CODE_UPPER          = 0x0700,
	ZPL_RE_CODE_WORD           = 0x0800,
	ZPL_RE_CODE_NOT_WORD       = 0x0900,

	ZPL_RE_CODE_XDIGIT    = 0x0a00,
	ZPL_RE_CODE_PRINTABLE = 0x0b00,
} zplreCode;

typedef struct
{
	sw op, offset;
} re_ctx;

enum
{
	ZPL_RE__NO_MATCH         = -1,
	ZPL_RE__INTERNAL_FAILURE = -2,
};

static char const ZPL_RE__META_CHARS[] = "^$()[].*+?|\\";
static char const ZPL_RE__WHITESPACE[] = " \r\t\n\v\f";
#define ZPL_RE__LITERAL( str ) ( str ), size_of( str ) - 1

static re_ctx re__exec_single( re* re, sw op, char const* str, sw str_len, sw offset, re_capture* captures, sw max_capture_count );
static re_ctx re__exec( re* re, sw op, char const* str, sw str_len, sw offset, re_capture* captures, sw max_capture_count );

static re_ctx re__ctx_no_match( sw op )
{
	re_ctx c;
	c.op     = op;
	c.offset = ZPL_RE__NO_MATCH;
	return c;
}

static re_ctx re__ctx_internal_failure( sw op )
{
	re_ctx c;
	c.op     = op;
	c.offset = ZPL_RE__INTERNAL_FAILURE;
	return c;
}

static u8 re__hex( char const* s )
{
	return ( ( char_to_hex_digit( *s ) << 4 ) & 0xf0 ) | ( char_to_hex_digit( *( s + 1 ) ) & 0x0f );
}

static sw re__strfind( char const* s, sw len, char c, sw offset )
{
	if ( offset < len )
	{
		char const* found = ( char const* )memchr( s + offset, c, len - offset );
		if ( found )
			return found - s;
	}

	return -1;
}

static b32 re__match_escape( char c, int code )
{
	switch ( code )
	{
		case ZPL_RE_CODE_NULL :
			return c == 0;
		case ZPL_RE_CODE_WHITESPACE :
			return re__strfind( ZPL_RE__LITERAL( ZPL_RE__WHITESPACE ), c, 0 ) >= 0;
		case ZPL_RE_CODE_NOT_WHITESPACE :
			return re__strfind( ZPL_RE__LITERAL( ZPL_RE__WHITESPACE ), c, 0 ) < 0;
		case ZPL_RE_CODE_DIGIT :
			return ( c >= '0' && c <= '9' );
		case ZPL_RE_CODE_NOT_DIGIT :
			return ! ( c >= '0' && c <= '9' );
		case ZPL_RE_CODE_ALPHA :
			return ( c >= 'A' && c <= 'Z' ) || ( c >= 'a' && c <= 'z' );
		case ZPL_RE_CODE_LOWER :
			return ( c >= 'a' && c <= 'z' );
		case ZPL_RE_CODE_UPPER :
			return ( c >= 'A' && c <= 'Z' );

		/* TODO(bill): Make better? */
		case ZPL_RE_CODE_WORD :
			return ( c >= 'A' && c <= 'Z' ) || ( c >= 'a' && c <= 'z' ) || ( c >= '0' && c <= '9' ) || c == '_';
		case ZPL_RE_CODE_NOT_WORD :
			return ! ( ( c >= 'A' && c <= 'Z' ) || ( c >= 'a' && c <= 'z' ) || ( c >= '0' && c <= '9' ) || c == '_' );

		/* TODO(bill): Maybe replace with between tests? */
		case ZPL_RE_CODE_XDIGIT :
			return re__strfind( ZPL_RE__LITERAL( "0123456789ABCDEFabcdef" ), c, 0 ) >= 0;
		case ZPL_RE_CODE_PRINTABLE :
			return c >= 0x20 && c <= 0x7e;
		default :
			break;
	}

	return 0;
}

static re_ctx re__consume( re* re, sw op, char const* str, sw str_len, sw offset, re_capture* captures, sw max_capture_count, b32 is_greedy )
{
	re_ctx c, best_c, next_c;

	c.op     = op;
	c.offset = offset;

	best_c.op     = ZPL_RE__NO_MATCH;
	best_c.offset = offset;

	for ( ;; )
	{
		c = re__exec_single( re, op, str, str_len, c.offset, 0, 0 );
		if ( c.offset > str_len || c.offset == -1 )
			break;
		if ( c.op >= re->buf_len )
			return c;

		next_c = re__exec( re, c.op, str, str_len, c.offset, captures, max_capture_count );
		if ( next_c.offset <= str_len )
		{
			if ( captures )
				re__exec( re, c.op, str, str_len, c.offset, captures, max_capture_count );

			best_c = next_c;
			if ( ! is_greedy )
				break;
		}

		if ( best_c.op > re->buf_len )
			best_c.op = c.op;
	}

	return best_c;
}

static re_ctx re__exec_single( re* re, sw op, char const* str, sw str_len, sw offset, re_capture* captures, sw max_capture_count )
{
	re_ctx ctx;
	sw     buffer_len;
	sw     match_len;
	sw     next_op;
	sw     skip;

	switch ( re->buf[ op++ ] )
	{
		case ZPL_RE_OP_BEGIN_CAPTURE :
			{
				u8 capture = re->buf[ op++ ];
				if ( captures && ( capture < max_capture_count ) )
					captures[ capture ].str = str + offset;
			}
			break;

		case ZPL_RE_OP_END_CAPTURE :
			{
				u8 capture = re->buf[ op++ ];
				if ( captures && ( capture < max_capture_count ) )
					captures[ capture ].len = ( str + offset ) - captures[ capture ].str;
			}
			break;

		case ZPL_RE_OP_BEGINNING_OF_LINE :
			{
				if ( offset != 0 )
					return re__ctx_no_match( op );
			}
			break;

		case ZPL_RE_OP_END_OF_LINE :
			{
				if ( offset != str_len )
					return re__ctx_no_match( op );
			}
			break;

		case ZPL_RE_OP_BRANCH_START :
			{
				skip = re->buf[ op++ ];
				ctx  = re__exec( re, op, str, str_len, offset, captures, max_capture_count );
				if ( ctx.offset <= str_len )
				{
					offset = ctx.offset;
					op     = ctx.op;
				}
				else
				{
					ctx    = re__exec( re, op + skip, str, str_len, offset, captures, max_capture_count );
					offset = ctx.offset;
					op     = ctx.op;
				}
			}
			break;

		case ZPL_RE_OP_BRANCH_END :
			{
				skip  = re->buf[ op++ ];
				op   += skip;
			}
			break;

		case ZPL_RE_OP_ANY :
			{
				if ( offset < str_len )
				{
					offset++;
					break;
				}
				return re__ctx_no_match( op );
			}
			break;

		case ZPL_RE_OP_ANY_OF :
			{
				sw   i;
				char cin   = str[ offset ];
				buffer_len = re->buf[ op++ ];

				if ( offset >= str_len )
					return re__ctx_no_match( op + buffer_len );

				for ( i = 0; i < buffer_len; i++ )
				{
					char cmatch = ( char )re->buf[ op + i ];
					if ( ! cmatch )
					{
						i++;
						if ( re__match_escape( cin, re->buf[ op + i ] << 8 ) )
							break;
					}
					else if ( cin == cmatch )
					{
						break;
					}
				}

				if ( i == buffer_len )
					return re__ctx_no_match( op + buffer_len );

				offset++;
				op += buffer_len;
			}
			break;

		case ZPL_RE_OP_ANY_BUT :
			{
				sw   i;
				char cin   = str[ offset ];
				buffer_len = re->buf[ op++ ];

				if ( offset >= str_len )
					return re__ctx_no_match( op + buffer_len );

				for ( i = 0; i < buffer_len; i++ )
				{
					char cmatch = ( char )re->buf[ op + i ];
					if ( ! cmatch )
					{
						i++;
						if ( re__match_escape( cin, re->buf[ op + i ] << 8 ) )
							return re__ctx_no_match( op + buffer_len );
					}
					else if ( cin == cmatch )
					{
						return re__ctx_no_match( op + buffer_len );
					}
				}

				offset++;
				op += buffer_len;
			}
			break;

		case ZPL_RE_OP_EXACT_MATCH :
			{
				match_len = re->buf[ op++ ];

				if ( ( match_len > ( str_len - offset ) ) || str_compare( str + offset, ( const char* )re->buf + op, match_len ) != 0 )
					return re__ctx_no_match( op + match_len );

				op     += match_len;
				offset += match_len;
			}
			break;

		case ZPL_RE_OP_META_MATCH :
			{
				char cin    = ( char )re->buf[ op++ ];
				char cmatch = str[ offset++ ];

				if ( ! cin )
				{
					if ( re__match_escape( cmatch, re->buf[ op++ ] << 8 ) )
						break;
				}
				else if ( cin == cmatch )
					break;

				return re__ctx_no_match( op );
			}
			break;

		case ZPL_RE_OP_ZERO_OR_MORE :
			{
				ctx    = re__consume( re, op, str, str_len, offset, captures, max_capture_count, 1 );
				offset = ctx.offset;
				op     = ctx.op;
			}
			break;

		case ZPL_RE_OP_ONE_OR_MORE :
			{
				ctx = re__exec_single( re, op, str, str_len, offset, captures, max_capture_count );

				if ( ctx.offset > str_len )
					return ctx;

				ctx    = re__consume( re, op, str, str_len, offset, captures, max_capture_count, 1 );
				offset = ctx.offset;
				op     = ctx.op;
			}
			break;

		case ZPL_RE_OP_ZERO_OR_MORE_SHORTEST :
			{
				ctx    = re__consume( re, op, str, str_len, offset, captures, max_capture_count, 0 );
				offset = ctx.offset;
				op     = ctx.op;
			}
			break;

		case ZPL_RE_OP_ONE_OR_MORE_SHORTEST :
			{
				ctx = re__exec_single( re, op, str, str_len, offset, captures, max_capture_count );

				if ( ctx.offset > str_len )
					return ctx;

				ctx    = re__consume( re, op, str, str_len, offset, captures, max_capture_count, 0 );
				offset = ctx.offset;
				op     = ctx.op;
			}
			break;

		case ZPL_RE_OP_ZERO_OR_ONE :
			{
				ctx = re__exec_single( re, op, str, str_len, offset, captures, max_capture_count );

				if ( ctx.offset <= str_len )
				{
					re_ctx possible_ctx = re__exec( re, ctx.op, str, str_len, ctx.offset, captures, max_capture_count );

					if ( possible_ctx.offset <= str_len )
					{
						op     = possible_ctx.op;
						offset = possible_ctx.offset;
						break;
					}
				}

				next_op = ctx.op;
				ctx     = re__exec( re, next_op, str, str_len, offset, captures, max_capture_count );

				if ( ctx.offset <= str_len )
				{
					op     = ctx.op;
					offset = ctx.offset;
					break;
				}
				return re__ctx_no_match( op );
			}
			break;

		default :
			{
				return re__ctx_internal_failure( op );
			}
			break;
	}

	ctx.op     = op;
	ctx.offset = offset;

	return ctx;
}

static re_ctx re__exec( re* re, sw op, char const* str, sw str_len, sw offset, re_capture* captures, sw max_capture_count )
{
	re_ctx c;
	c.op     = op;
	c.offset = offset;

	while ( c.op < re->buf_len )
	{
		c = re__exec_single( re, c.op, str, str_len, c.offset, captures, max_capture_count );

		if ( c.offset > str_len || c.offset == -1 )
			break;
	}

	return c;
}

static regex_error re__emit_ops( re* re, sw op_count, ... )
{
	va_list va;

	if ( re->buf_len + op_count > re->buf_cap )
	{
		if ( ! re->can_realloc )
		{
			return ZPL_RE_ERROR_TOO_LONG;
		}
		else
		{
			sw new_cap  = ( re->buf_cap * 2 ) + op_count;
			re->buf     = ( char* )resize( re->backing, re->buf, re->buf_cap, new_cap );
			re->buf_cap = new_cap;
		}
	}

	va_start( va, op_count );
	for ( sw i = 0; i < op_count; i++ )
	{
		s32 v = va_arg( va, s32 );
		if ( v > 256 )
			return ZPL_RE_ERROR_TOO_LONG;
		re->buf[ re->buf_len++ ] = ( char )v;
	}
	va_end( va );

	return ZPL_RE_ERROR_NONE;
}

static regex_error re__emit_ops_buffer( re* re, sw op_count, char const* buffer )
{
	if ( re->buf_len + op_count > re->buf_cap )
	{
		if ( ! re->can_realloc )
		{
			return ZPL_RE_ERROR_TOO_LONG;
		}
		else
		{
			sw new_cap  = ( re->buf_cap * 2 ) + op_count;
			re->buf     = ( char* )resize( re->backing, re->buf, re->buf_cap, new_cap );
			re->buf_cap = new_cap;
		}
	}

	for ( sw i = 0; i < op_count; i++ )
	{
		re->buf[ re->buf_len++ ] = buffer[ i ];
	}

	return ZPL_RE_ERROR_NONE;
}

static int re__encode_escape( char code )
{
	switch ( code )
	{
		default :
			break; /* NOTE(bill): It's a normal character */

		/* TODO(bill): Are there anymore? */
		case 't' :
			return '\t';
		case 'n' :
			return '\n';
		case 'r' :
			return '\r';
		case 'f' :
			return '\f';
		case 'v' :
			return '\v';

		case '0' :
			return ZPL_RE_CODE_NULL;

		case 's' :
			return ZPL_RE_CODE_WHITESPACE;
		case 'S' :
			return ZPL_RE_CODE_NOT_WHITESPACE;

		case 'd' :
			return ZPL_RE_CODE_DIGIT;
		case 'D' :
			return ZPL_RE_CODE_NOT_DIGIT;

		case 'a' :
			return ZPL_RE_CODE_ALPHA;
		case 'l' :
			return ZPL_RE_CODE_LOWER;
		case 'u' :
			return ZPL_RE_CODE_UPPER;

		case 'w' :
			return ZPL_RE_CODE_WORD;
		case 'W' :
			return ZPL_RE_CODE_NOT_WORD;

		case 'x' :
			return ZPL_RE_CODE_XDIGIT;
		case 'p' :
			return ZPL_RE_CODE_PRINTABLE;
	}
	return code;
}

static regex_error re__parse_group( re* re, char const* pattern, sw len, sw offset, sw* new_offset )
{
	regex_error err           = ZPL_RE_ERROR_NONE;
	char        buffer[ 256 ] = { 0 };
	sw          buffer_len = 0, buffer_cap = size_of( buffer );
	b32         closed = 0;
	zplreOp     op     = ZPL_RE_OP_ANY_OF;

	if ( pattern[ offset ] == '^' )
	{
		offset++;
		op = ZPL_RE_OP_ANY_BUT;
	}

	while ( ! closed && err == ZPL_RE_ERROR_NONE && offset < len )
	{
		if ( pattern[ offset ] == ']' )
		{
			err = re__emit_ops( re, 2, ( s32 )op, ( s32 )buffer_len );
			if ( err )
				break;

			err = re__emit_ops_buffer( re, buffer_len, ( const char* )buffer );
			if ( err )
				break;
			offset++;
			closed = 1;
			break;
		}

		if ( buffer_len >= buffer_cap )
			return ZPL_RE_ERROR_TOO_LONG;

		if ( pattern[ offset ] == '\\' )
		{
			offset++;

			if ( ( offset + 1 < len ) && char_is_hex_digit( *( pattern + offset ) ) )
			{
				buffer[ buffer_len++ ] = re__hex( ( pattern + offset ) );
				offset++;
			}
			else if ( offset < len )
			{
				s32 code = re__encode_escape( pattern[ offset ] );

				if ( ! code || code > 0xff )
				{
					buffer[ buffer_len++ ] = 0;

					if ( buffer_len >= buffer_cap )
						return ZPL_RE_ERROR_TOO_LONG;

					buffer[ buffer_len++ ] = ( code >> 8 ) & 0xff;
				}
				else
				{
					buffer[ buffer_len++ ] = code & 0xff;
				}
			}
		}
		else
		{
			buffer[ buffer_len++ ] = ( unsigned char )pattern[ offset ];
		}

		offset++;
	}

	if ( err )
		return err;
	if ( ! closed )
		return ZPL_RE_ERROR_MISMATCHED_BLOCKS;
	if ( new_offset )
		*new_offset = offset;
	return ZPL_RE_ERROR_NONE;
}

static regex_error re__compile_quantifier( re* re, sw last_buf_len, unsigned char quantifier )
{
	regex_error err;
	sw          move_size;

	if ( ( re->buf[ last_buf_len ] == ZPL_RE_OP_EXACT_MATCH ) && ( re->buf[ last_buf_len + 1 ] > 1 ) )
	{
		unsigned char last_char = re->buf[ re->buf_len - 1 ];

		re->buf[ last_buf_len + 1 ]--;
		re->buf_len--;
		err = re__emit_ops( re, 4, ( s32 )quantifier, ( s32 )ZPL_RE_OP_EXACT_MATCH, 1, ( s32 )last_char );
		if ( err )
			return err;
		return ZPL_RE_ERROR_NONE;
	}

	move_size = re->buf_len - last_buf_len + 1;

	err = re__emit_ops( re, 1, 0 );
	if ( err )
		return err;

	memmove( re->buf + last_buf_len + 1, re->buf + last_buf_len, move_size );
	re->buf[ last_buf_len ] = quantifier;

	return ZPL_RE_ERROR_NONE;
}

static regex_error re__parse( re* re, char const* pattern, sw len, sw offset, sw level, sw* new_offset )
{
	regex_error err          = ZPL_RE_ERROR_NONE;
	sw          last_buf_len = re->buf_len;
	sw          branch_begin = re->buf_len;
	sw          branch_op    = -1;

	while ( offset < len )
	{
		switch ( pattern[ offset++ ] )
		{
			case '^' :
				{
					err = re__emit_ops( re, 1, ZPL_RE_OP_BEGINNING_OF_LINE );
					if ( err )
						return err;
				}
				break;

			case '$' :
				{
					err = re__emit_ops( re, 1, ZPL_RE_OP_END_OF_LINE );
					if ( err )
						return err;
				}
				break;

			case '(' :
				{
					sw capture   = re->capture_count++;
					last_buf_len = re->buf_len;
					err          = re__emit_ops( re, 2, ZPL_RE_OP_BEGIN_CAPTURE, ( s32 )capture );
					if ( err )
						return err;

					err = re__parse( re, pattern, len, offset, level + 1, &offset );

					if ( ( offset > len ) || ( pattern[ offset - 1 ] != ')' ) )
						return ZPL_RE_ERROR_MISMATCHED_CAPTURES;

					err = re__emit_ops( re, 2, ZPL_RE_OP_END_CAPTURE, ( s32 )capture );
					if ( err )
						return err;
				}
				break;

			case ')' :
				{
					if ( branch_op != -1 )
						re->buf[ branch_op + 1 ] = ( unsigned char )( re->buf_len - ( branch_op + 2 ) );

					if ( level == 0 )
						return ZPL_RE_ERROR_MISMATCHED_CAPTURES;

					if ( new_offset )
						*new_offset = offset;
					return ZPL_RE_ERROR_NONE;
				}
				break;

			case '[' :
				{
					last_buf_len = re->buf_len;
					err          = re__parse_group( re, pattern, len, offset, &offset );
					if ( offset > len )
						return err;
				}
				break;

			/* NOTE(bill): Branching magic! */
			case '|' :
				{
					if ( branch_begin >= re->buf_len )
					{
						return ZPL_RE_ERROR_BRANCH_FAILURE;
					}
					else
					{
						sw size = re->buf_len - branch_begin;
						err     = re__emit_ops( re, 4, 0, 0, ZPL_RE_OP_BRANCH_END, 0 );
						if ( err )
							return err;

						memmove( re->buf + branch_begin + 2, re->buf + branch_begin, size );
						re->buf[ branch_begin ]     = ZPL_RE_OP_BRANCH_START;
						re->buf[ branch_begin + 1 ] = ( size + 2 ) & 0xff;
						branch_op                   = re->buf_len - 2;
					}
				}
				break;

			case '.' :
				{
					last_buf_len = re->buf_len;
					err          = re__emit_ops( re, 1, ZPL_RE_OP_ANY );
					if ( err )
						return err;
				}
				break;

			case '*' :
			case '+' :
				{
					unsigned char quantifier = ZPL_RE_OP_ONE_OR_MORE;
					if ( pattern[ offset - 1 ] == '*' )
						quantifier = ZPL_RE_OP_ZERO_OR_MORE;

					if ( last_buf_len >= re->buf_len )
						return ZPL_RE_ERROR_INVALID_QUANTIFIER;
					if ( ( re->buf[ last_buf_len ] < ZPL_RE_OP_EXACT_MATCH ) || ( re->buf[ last_buf_len ] > ZPL_RE_OP_ANY_BUT ) )
						return ZPL_RE_ERROR_INVALID_QUANTIFIER;

					if ( ( offset < len ) && ( pattern[ offset ] == '?' ) )
					{
						quantifier = ZPL_RE_OP_ONE_OR_MORE_SHORTEST;
						offset++;
					}

					err = re__compile_quantifier( re, last_buf_len, quantifier );
					if ( err )
						return err;
				}
				break;

			case '?' :
				{
					if ( last_buf_len >= re->buf_len )
						return ZPL_RE_ERROR_INVALID_QUANTIFIER;
					if ( ( re->buf[ last_buf_len ] < ZPL_RE_OP_EXACT_MATCH ) || ( re->buf[ last_buf_len ] > ZPL_RE_OP_ANY_BUT ) )
						return ZPL_RE_ERROR_INVALID_QUANTIFIER;

					err = re__compile_quantifier( re, last_buf_len, ( unsigned char )ZPL_RE_OP_ZERO_OR_ONE );
					if ( err )
						return err;
				}
				break;

			case '\\' :
				{
					last_buf_len = re->buf_len;
					if ( ( offset + 1 < len ) && char_is_hex_digit( *( pattern + offset ) ) )
					{
						unsigned char hex_value  = re__hex( ( pattern + offset ) );
						offset                  += 2;
						err                      = re__emit_ops( re, 2, ZPL_RE_OP_META_MATCH, ( int )hex_value );
						if ( err )
							return err;
					}
					else if ( offset < len )
					{
						int code = re__encode_escape( pattern[ offset++ ] );
						if ( ! code || ( code > 0xff ) )
						{
							err = re__emit_ops( re, 3, ZPL_RE_OP_META_MATCH, 0, ( int )( ( code >> 8 ) & 0xff ) );
							if ( err )
								return err;
						}
						else
						{
							err = re__emit_ops( re, 2, ZPL_RE_OP_META_MATCH, ( int )code );
							if ( err )
								return err;
						}
					}
				}
				break;

			/* NOTE(bill): Exact match */
			default :
				{
					char const* match_start;
					sw          size = 0;
					offset--;
					match_start = pattern + offset;
					while ( ( offset < len ) && ( re__strfind( ZPL_RE__LITERAL( ZPL_RE__META_CHARS ), pattern[ offset ], 0 ) < 0 ) )
					{
						size++, offset++;
					}

					last_buf_len = re->buf_len;
					err          = re__emit_ops( re, 2, ZPL_RE_OP_EXACT_MATCH, ( int )size );
					if ( err )
						return err;
					err = re__emit_ops_buffer( re, size, ( char const* )match_start );
					if ( err )
						return err;
				}
				break;
		}
	}

	if ( new_offset )
		*new_offset = offset;
	return ZPL_RE_ERROR_NONE;
}

regex_error re_compile_from_buffer( re* re, char const* pattern, sw pattern_len, void* buffer, sw buffer_len )
{
	regex_error err;
	re->capture_count = 0;
	re->buf           = ( char* )buffer;
	re->buf_len       = 0;
	re->buf_cap       = re->buf_len;
	re->can_realloc   = 0;

	err = re__parse( re, pattern, pattern_len, 0, 0, 0 );
	return err;
}

regex_error re_compile( re* re, AllocatorInfo backing, char const* pattern, sw pattern_len )
{
	regex_error err;
	sw          cap    = pattern_len + 128;
	sw          offset = 0;

	re->backing       = backing;
	re->capture_count = 0;
	re->buf           = ( char* )alloc( backing, cap );
	re->buf_len       = 0;
	re->buf_cap       = cap;
	re->can_realloc   = 1;

	err = re__parse( re, pattern, pattern_len, 0, 0, &offset );

	if ( offset != pattern_len )
		free( backing, re->buf );

	return err;
}

sw re_capture_count( re* re )
{
	return re->capture_count;
}

b32 re_match( re* re, char const* str, sw len, re_capture* captures, sw max_capture_count, sw* offset )
{
	if ( re && re->buf_len > 0 )
	{
		if ( re->buf[ 0 ] == ZPL_RE_OP_BEGINNING_OF_LINE )
		{
			re_ctx c = re__exec( re, 0, str, len, 0, captures, max_capture_count );
			if ( c.offset >= 0 && c.offset <= len )
			{
				if ( offset )
					*offset = c.offset;
				return 1;
			};
			if ( c.offset == ZPL_RE__INTERNAL_FAILURE )
				return 0;
		}
		else
		{
			sw i;
			for ( i = 0; i < len; i++ )
			{
				re_ctx c = re__exec( re, 0, str, len, i, captures, max_capture_count );
				if ( c.offset >= 0 && c.offset <= len )
				{
					if ( offset )
						*offset = c.offset;
					return 1;
				};
				if ( c.offset == ZPL_RE__INTERNAL_FAILURE )
					return 0;
			}
		}
		return 0;
	}
	return 1;
}

b32 re_match_all( re* re, char const* str, sw str_len, sw max_capture_count, re_capture** out_captures )
{
	char* end = ( char* )str + str_len;
	char* p   = ( char* )str;

	buffer_make( re_capture, cps, heap(), max_capture_count );

	sw offset = 0;

	while ( p < end )
	{
		b32 ok = re_match( re, p, end - p, cps, max_capture_count, &offset );
		if ( ! ok )
		{
			buffer_free( cps );
			return false;
		}

		p += offset;

		for ( sw i = 0; i < max_capture_count; i++ )
		{
			array_append( *out_captures, cps[ i ] );
		}
	}

	buffer_free( cps );

	return true;
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

#		endif

#		if defined( ZPL_MODULE_DLL )
// file: source/dll.c


#if defined( ZPL_SYSTEM_UNIX ) || defined( ZPL_SYSTEM_MACOS )
#	include <dlfcn.h>
#endif

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

////////////////////////////////////////////////////////////////
//
// DLL Handling
//
//

#if defined( ZPL_SYSTEM_WINDOWS )
dll_handle dll_load( char const* filepath )
{
	return zpl_cast( dll_handle ) LoadLibraryA( filepath );
}

void dll_unload( dll_handle dll )
{
	FreeLibrary( zpl_cast( HMODULE ) dll );
}

dll_proc dll_proc_address( dll_handle dll, char const* proc_name )
{
	return zpl_cast( dll_proc ) GetProcAddress( zpl_cast( HMODULE ) dll, proc_name );
}

#else    // POSIX

dll_handle dll_load( char const* filepath )
{
	return zpl_cast( dll_handle ) dlopen( filepath, RTLD_LAZY | RTLD_GLOBAL );
}

void dll_unload( dll_handle dll )
{
	dlclose( dll );
}

dll_proc dll_proc_address( dll_handle dll, char const* proc_name )
{
	return zpl_cast( dll_proc ) dlsym( dll, proc_name );
}

#endif

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

#		endif

#		if defined( ZPL_MODULE_OPTS )
// file: source/opts.c

////////////////////////////////////////////////////////////////
//
// CLI Options
//
//

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

void opts_init( Opts* opts, AllocatorInfo a, char const* app )
{
	Opts opts_    = { 0 };
	*opts         = opts_;
	opts->alloc   = a;
	opts->appname = app;

	array_init( opts->entries, a );
	array_init( opts->positioned, a );
	array_init( opts->errors, a );
}

void opts_free( Opts* opts )
{
	for ( s32 i = 0; i < array_count( opts->entries ); ++i )
	{
		opts_entry* e = opts->entries + i;
		if ( e->type == ZPL_OPTS_STRING )
		{
			string_free( e->text );
		}
	}

	array_free( opts->entries );
	array_free( opts->positioned );
	array_free( opts->errors );
}

void opts_add( Opts* opts, char const* name, char const* lname, const char* desc, u8 type )
{
	opts_entry e = { 0 };

	e.name  = name;
	e.lname = lname;
	e.desc  = desc;
	e.type  = type;
	e.met   = false;
	e.pos   = false;

	array_append( opts->entries, e );
}

opts_entry* _opts_find( Opts* opts, char const* name, uw len, b32 longname )
{
	opts_entry* e = 0;

	for ( int i = 0; i < array_count( opts->entries ); ++i )
	{
		e             = opts->entries + i;
		char const* n = ( longname ? e->lname : e->name );
		if ( ! n )
			continue;

		if ( strnlen( name, len ) == strlen( n ) && ! str_compare( n, name, len ) )
		{
			return e;
		}
	}

	return NULL;
}

void opts_positional_add( Opts* opts, char const* name )
{
	opts_entry* e = _opts_find( opts, name, strlen( name ), true );

	if ( e )
	{
		e->pos = true;
		array_append_at( opts->positioned, e, 0 );
	}
}

b32 opts_positionals_filled( Opts* opts )
{
	return array_count( opts->positioned ) == 0;
}

String opts_string( Opts* opts, char const* name, char const* fallback )
{
	opts_entry* e = _opts_find( opts, name, strlen( name ), true );

	return ( char* )( ( e && e->met ) ? e->text : fallback );
}

f64 opts_real( Opts* opts, char const* name, f64 fallback )
{
	opts_entry* e = _opts_find( opts, name, strlen( name ), true );

	return ( e && e->met ) ? e->real : fallback;
}

s64 opts_integer( Opts* opts, char const* name, s64 fallback )
{
	opts_entry* e = _opts_find( opts, name, strlen( name ), true );

	return ( e && e->met ) ? e->integer : fallback;
}

void _opts_set_value( Opts* opts, opts_entry* t, char* b )
{
	t->met = true;

	switch ( t->type )
	{
		case ZPL_OPTS_STRING :
			{
				t->text = string_make( opts->alloc, b );
			}
			break;

		case ZPL_OPTS_FLOAT :
			{
				t->real = str_to_f64( b, NULL );
			}
			break;

		case ZPL_OPTS_INT :
			{
				t->integer = str_to_i64( b, NULL, 10 );
			}
			break;
	}

	for ( sw i = 0; i < array_count( opts->positioned ); i++ )
	{
		if ( ! str_compare( opts->positioned[ i ]->lname, t->lname ) )
		{
			array_remove_at( opts->positioned, i );
			break;
		}
	}
}

b32 opts_has_arg( Opts* opts, char const* name )
{
	opts_entry* e = _opts_find( opts, name, strlen( name ), true );

	if ( e )
	{
		return e->met;
	}

	return false;
}

void opts_print_help( Opts* opts )
{
	printf( "USAGE: %s", opts->appname );

	for ( sw i = array_count( opts->entries ); i >= 0; --i )
	{
		opts_entry* e = opts->entries + i;

		if ( e->pos == ( b32 ) true )
		{
			printf( " [%s]", e->lname );
		}
	}

	printf( "\nOPTIONS:\n" );

	for ( sw i = 0; i < array_count( opts->entries ); ++i )
	{
		opts_entry* e = opts->entries + i;

		if ( e->name )
		{
			if ( e->lname )
			{
				printf( "\t-%s, --%s: %s\n", e->name, e->lname, e->desc );
			}
			else
			{
				printf( "\t-%s: %s\n", e->name, e->desc );
			}
		}
		else
		{
			printf( "\t--%s: %s\n", e->lname, e->desc );
		}
	}
}

void opts_print_errors( Opts* opts )
{
	for ( int i = 0; i < array_count( opts->errors ); ++i )
	{
		opts_err* err = ( opts->errors + i );

		printf( "ERROR: " );

		switch ( err->type )
		{
			case ZPL_OPTS_ERR_OPTION :
				printf( "Invalid option \"%s\"", err->val );
				break;

			case ZPL_OPTS_ERR_VALUE :
				printf( "Invalid value \"%s\"", err->val );
				break;

			case ZPL_OPTS_ERR_MISSING_VALUE :
				printf( "Missing value for option \"%s\"", err->val );
				break;

			case ZPL_OPTS_ERR_EXTRA_VALUE :
				printf( "Extra value for option \"%s\"", err->val );
				break;
		}

		printf( "\n" );
	}
}

void _opts_push_error( Opts* opts, char* b, u8 errtype )
{
	opts_err err = { 0 };
	err.val      = b;
	err.type     = errtype;
	array_append( opts->errors, err );
}

b32 opts_compile( Opts* opts, int argc, char** argv )
{
	b32 had_errors = false;
	for ( int i = 1; i < argc; ++i )
	{
		char* p = argv[ i ];

		if ( *p )
		{
			p = zpl_cast( char* ) str_trim( p, false );
			if ( *p == '-' )
			{
				opts_entry* t       = 0;
				b32         checkln = false;
				if ( *( p + 1 ) == '-' )
				{
					checkln = true;
					++p;
				}

				char *b = p + 1, *e = b;

				while ( char_is_alphanumeric( *e ) || *e == '-' || *e == '_' )
				{
					++e;
				}

				t = _opts_find( opts, b, ( e - b ), checkln );

				if ( t )
				{
					char* ob = b;
					b        = e;

					/**/ if ( *e == '=' )
					{
						if ( t->type == ZPL_OPTS_FLAG )
						{
							*e = '\0';
							_opts_push_error( opts, ob, ZPL_OPTS_ERR_EXTRA_VALUE );
							had_errors = true;
							continue;
						}

						b = e = e + 1;
					}
					else if ( *e == '\0' )
					{
						char* sp = argv[ i + 1 ];

						if ( sp && *sp != '-' && ( array_count( opts->positioned ) < 1 || t->type != ZPL_OPTS_FLAG ) )
						{
							if ( t->type == ZPL_OPTS_FLAG )
							{
								_opts_push_error( opts, b, ZPL_OPTS_ERR_EXTRA_VALUE );
								had_errors = true;
								continue;
							}

							p = sp;
							b = e = sp;
							++i;
						}
						else
						{
							if ( t->type != ZPL_OPTS_FLAG )
							{
								_opts_push_error( opts, ob, ZPL_OPTS_ERR_MISSING_VALUE );
								had_errors = true;
								continue;
							}
							t->met = true;
							continue;
						}
					}

					e = zpl_cast( char* ) str_control_skip( e, '\0' );
					_opts_set_value( opts, t, b );
				}
				else
				{
					_opts_push_error( opts, b, ZPL_OPTS_ERR_OPTION );
					had_errors = true;
				}
			}
			else if ( array_count( opts->positioned ) )
			{
				opts_entry* l = array_back( opts->positioned );
				array_pop( opts->positioned );
				_opts_set_value( opts, l, p );
			}
			else
			{
				_opts_push_error( opts, p, ZPL_OPTS_ERR_VALUE );
				had_errors = true;
			}
		}
	}
	return ! had_errors;
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

#		endif

#		if defined( ZPL_MODULE_PROCESS )
// file: source/process.c

////////////////////////////////////////////////////////////////
//
// Process creation and manipulation methods
//
//

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

static ZPL_ALWAYS_INLINE void _pr_close_file_handle( FileInfo* f )
{
	ZPL_ASSERT_NOT_NULL( f );
	f->fd.p = NULL;
}

static ZPL_ALWAYS_INLINE void _pr_close_file_handles( pr* process )
{
	ZPL_ASSERT_NOT_NULL( process );

	_pr_close_file_handle( &process->in );
	_pr_close_file_handle( &process->out );
	_pr_close_file_handle( &process->err );

	process->f_stdin = process->f_stdout = process->f_stderr = NULL;

#ifdef ZPL_SYSTEM_WINDOWS
	process->win32_handle = NULL;
#else
	ZPL_NOT_IMPLEMENTED;
#endif
}

enum
{
	ZPL_PR_HANDLE_MODE_READ,
	ZPL_PR_HANDLE_MODE_WRITE,
	ZPL_PR_HANDLE_MODES,
};

void* _pr_open_handle( u8 type, const char* mode, void** handle )
{
#ifdef ZPL_SYSTEM_WINDOWS
	void* pipes[ ZPL_PR_HANDLE_MODES ];
	s32   fd;

	const u32           flag_inherit = 0x00000001;
	SECURITY_ATTRIBUTES sa           = { size_of( sa ), 0, 1 };

	if ( ! CreatePipe( &pipes[ 0 ], &pipes[ 1 ], zpl_cast( LPSECURITY_ATTRIBUTES ) & sa, 0 ) )
	{
		return NULL;
	}

	if ( ! SetHandleInformation( pipes[ type ], flag_inherit, 0 ) )
	{
		return NULL;
	}

	fd = _open_osfhandle( zpl_cast( sptr ) pipes[ type ], 0 );

	if ( fd != -1 )
	{
		*handle = pipes[ 1 - type ];
		return _fdopen( fd, mode );
	}

	return NULL;
#else
	ZPL_NOT_IMPLEMENTED;
	return NULL;
#endif
}

s32 pr_create( pr* process, const char** args, sw argc, pr_si si, pr_opts options )
{
	ZPL_ASSERT_NOT_NULL( process );
	zero_item( process );

#ifdef ZPL_SYSTEM_WINDOWS
	String              cli, env;
	b32                 c_env           = false;
	STARTUPINFOW        psi             = { 0 };
	PROCESS_INFORMATION pi              = { 0 };
	s32                 err_code        = 0;
	AllocatorInfo       a               = heap();
	const u32           use_std_handles = 0x00000100;

	psi.cb      = size_of( psi );
	psi.dwFlags = use_std_handles | si.flags;

	if ( options & ZPL_PR_OPTS_CUSTOM_ENV )
	{
		env   = string_join( heap(), zpl_cast( const char** ) si.env, si.env_count, "\0\0" );
		env   = string_appendc( env, "\0" );
		c_env = true;
	}
	else if ( ! ( options & ZPL_PR_OPTS_INHERIT_ENV ) )
	{
		env = ( String ) "\0\0\0\0";
	}
	else
	{
		env = ( String )NULL;
	}

	process->f_stdin  = _pr_open_handle( ZPL_PR_HANDLE_MODE_WRITE, "wb", &psi.hStdInput );
	process->f_stdout = _pr_open_handle( ZPL_PR_HANDLE_MODE_READ, "rb", &psi.hStdOutput );

	if ( options & ZPL_PR_OPTS_COMBINE_STD_OUTPUT )
	{
		process->f_stderr = process->f_stdout;
		psi.hStdError     = psi.hStdOutput;
	}
	else
	{
		process->f_stderr = _pr_open_handle( ZPL_PR_HANDLE_MODE_READ, "rb", &psi.hStdError );
	}

	cli = string_join( heap(), args, argc, " " );

	psi.dwX             = si.posx;
	psi.dwY             = si.posy;
	psi.dwXSize         = si.resx;
	psi.dwYSize         = si.resy;
	psi.dwXCountChars   = si.bufx;
	psi.dwYCountChars   = si.bufy;
	psi.dwFillAttribute = si.fill_attr;
	psi.wShowWindow     = si.show_window;

	wchar_t* w_cli     = _alloc_utf8_to_ucs2( a, cli, NULL );
	wchar_t* w_workdir = _alloc_utf8_to_ucs2( a, si.workdir, NULL );

	if ( ! CreateProcessW( NULL, w_cli, NULL, NULL, 1, 0, env, w_workdir, zpl_cast( LPSTARTUPINFOW ) & psi, zpl_cast( LPPROCESS_INFORMATION ) & pi ) )
	{
		err_code = -1;
		goto pr_free_data;
	}

	process->win32_handle = pi.hProcess;
	CloseHandle( pi.hThread );

	file_connect_handle( &process->in, process->f_stdin );
	file_connect_handle( &process->out, process->f_stdout );
	file_connect_handle( &process->err, process->f_stderr );

pr_free_data:
	string_free( cli );
	free( a, w_cli );
	free( a, w_workdir );

	if ( c_env )
		string_free( env );

	return err_code;

#else
	ZPL_NOT_IMPLEMENTED;
	return -1;
#endif
}

s32 pr_join( pr* process )
{
	s32 ret_code;

	ZPL_ASSERT_NOT_NULL( process );

#ifdef ZPL_SYSTEM_WINDOWS
	if ( process->f_stdin )
	{
		fclose( zpl_cast( FILE* ) process->f_stdin );
	}

	WaitForSingleObject( process->win32_handle, INFINITE );

	if ( ! GetExitCodeProcess( process->win32_handle, zpl_cast( LPDWORD ) & ret_code ) )
	{
		pr_destroy( process );
		return -1;
	}

	pr_destroy( process );

	return ret_code;
#else
	ZPL_NOT_IMPLEMENTED;
	ret_code = -1;
	return ret_code;
#endif
}

void pr_destroy( pr* process )
{
	ZPL_ASSERT_NOT_NULL( process );

#ifdef ZPL_SYSTEM_WINDOWS
	if ( process->f_stdin )
	{
		fclose( zpl_cast( FILE* ) process->f_stdin );
	}

	fclose( zpl_cast( FILE* ) process->f_stdout );

	if ( process->f_stderr != process->f_stdout )
	{
		fclose( zpl_cast( FILE* ) process->f_stderr );
	}

	CloseHandle( process->win32_handle );

	_pr_close_file_handles( process );
#else
	ZPL_NOT_IMPLEMENTED;
#endif
}

void pr_terminate( pr* process, s32 err_code )
{
	ZPL_ASSERT_NOT_NULL( process );

#ifdef ZPL_SYSTEM_WINDOWS
	TerminateProcess( process->win32_handle, zpl_cast( UINT ) err_code );
	pr_destroy( process );
#else
	ZPL_NOT_IMPLEMENTED;
#endif
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

#		endif

#		if defined( ZPL_MODULE_MATH )
// file: source/math.c


#if defined( ZPL_COMPILER_TINYC ) && defined( ZPL_NO_MATH_H )
#	undef ZPL_NO_MATH_H
#endif

#if ! defined( ZPL_NO_MATH_H )
#	include <math.h>
#endif

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

////////////////////////////////////////////////////////////////
//
// Math
//

f32 to_radians( f32 degrees )
{
	return degrees * ZPL_TAU / 360.0f;
}

f32 to_degrees( f32 radians )
{
	return radians * 360.0f / ZPL_TAU;
}

f32 angle_diff( f32 radians_a, f32 radians_b )
{
	f32 delta  = mod( radians_b - radians_a, ZPL_TAU );
	delta      = mod( delta + 1.5f * ZPL_TAU, ZPL_TAU );
	delta     -= 0.5f * ZPL_TAU;
	return delta;
}

f32 copy_sign( f32 x, f32 y )
{
	s32 ix, iy;
	f32 r;
	memcopy( &ix, &x, size_of( x ) );
	memcopy( &iy, &y, size_of( y ) );

	ix &= 0x7fffffff;
	ix |= iy & 0x80000000;
	memcopy( &r, &ix, size_of( ix ) );
	return r;
}

f32 remainder( f32 x, f32 y )
{
	return x - ( round( x / y ) * y );
}

f32 mod( f32 x, f32 y )
{
	f32 result;
	y      = abs( y );
	result = remainder( abs( x ), y );
	if ( sign( result ) > 0.0f )
		result += y;
	return copy_sign( result, x );
}

f64 copy_sign64( f64 x, f64 y )
{
	s64 ix, iy;
	f64 r;
	memcopy( &ix, &x, size_of( x ) );
	memcopy( &iy, &y, size_of( y ) );

	ix &= 0x7fffffffffffffff;
	ix |= iy & 0x8000000000000000;
	memcopy( &r, &ix, size_of( ix ) );
	return r;
}

f64 floor64( f64 x )
{
	return zpl_cast( f64 )( ( x >= 0.0 ) ? zpl_cast( s64 ) x : zpl_cast( s64 )( x - 0.9999999999999999 ) );
}

f64 ceil64( f64 x )
{
	return zpl_cast( f64 )( ( x < 0 ) ? zpl_cast( s64 ) x : ( zpl_cast( s64 ) x ) + 1 );
}

f64 round64( f64 x )
{
	return zpl_cast( f64 )( ( x >= 0.0 ) ? floor64( x + 0.5 ) : ceil64( x - 0.5 ) );
}

f64 remainder64( f64 x, f64 y )
{
	return x - ( round64( x / y ) * y );
}

f64 abs64( f64 x )
{
	return x < 0 ? -x : x;
}

f64 sign64( f64 x )
{
	return x < 0 ? -1.0 : +1.0;
}

f64 mod64( f64 x, f64 y )
{
	f64 result;
	y      = abs64( y );
	result = remainder64( abs64( x ), y );
	if ( sign64( result ) )
		result += y;
	return copy_sign64( result, x );
}

f32 quake_rsqrt( f32 a )
{
	union
	{
		int i;
		f32 f;
	} t;

	f32       x2;
	f32 const three_halfs = 1.5f;

	x2  = a * 0.5f;
	t.f = a;
	t.i = 0x5f375a86 - ( t.i >> 1 );                  /* What the fuck? */
	t.f = t.f * ( three_halfs - ( x2 * t.f * t.f ) ); /* 1st iteration */
	t.f = t.f * ( three_halfs - ( x2 * t.f * t.f ) ); /* 2nd iteration, this can be removed */

	return t.f;
}

#if defined( ZPL_NO_MATH_H )
#	if defined( _MSC_VER )

f32 rsqrt( f32 a )
{
	return _mm_cvtss_f32( _mm_rsqrt_ss( _mm_set_ss( a ) ) );
}

f32 sqrt( f32 a )
{
	return _mm_cvtss_f32( _mm_sqrt_ss( _mm_set_ss( a ) ) );
};

f32 sin( f32 a )
{
	static f32 const a0 = +1.91059300966915117e-31f;
	static f32 const a1 = +1.00086760103908896f;
	static f32 const a2 = -1.21276126894734565e-2f;
	static f32 const a3 = -1.38078780785773762e-1f;
	static f32 const a4 = -2.67353392911981221e-2f;
	static f32 const a5 = +2.08026600266304389e-2f;
	static f32 const a6 = -3.03996055049204407e-3f;
	static f32 const a7 = +1.38235642404333740e-4f;
	return a0 + a * ( a1 + a * ( a2 + a * ( a3 + a * ( a4 + a * ( a5 + a * ( a6 + a * a7 ) ) ) ) ) );
}

f32 cos( f32 a )
{
	static f32 const a0 = +1.00238601909309722f;
	static f32 const a1 = -3.81919947353040024e-2f;
	static f32 const a2 = -3.94382342128062756e-1f;
	static f32 const a3 = -1.18134036025221444e-1f;
	static f32 const a4 = +1.07123798512170878e-1f;
	static f32 const a5 = -1.86637164165180873e-2f;
	static f32 const a6 = +9.90140908664079833e-4f;
	static f32 const a7 = -5.23022132118824778e-14f;
	return a0 + a * ( a1 + a * ( a2 + a * ( a3 + a * ( a4 + a * ( a5 + a * ( a6 + a * a7 ) ) ) ) ) );
}

f32 tan( f32 radians )
{
	f32 rr  = radians * radians;
	f32 a   = 9.5168091e-03f;
	a      *= rr;
	a      += 2.900525e-03f;
	a      *= rr;
	a      += 2.45650893e-02f;
	a      *= rr;
	a      += 5.33740603e-02f;
	a      *= rr;
	a      += 1.333923995e-01f;
	a      *= rr;
	a      += 3.333314036e-01f;
	a      *= rr;
	a      += 1.0f;
	a      *= radians;
	return a;
}

f32 arcsin( f32 a )
{
	return arctan2( a, sqrt( ( 1.0f + a ) * ( 1.0f - a ) ) );
}

f32 arccos( f32 a )
{
	return arctan2( sqrt( ( 1.0f + a ) * ( 1.0f - a ) ), a );
}

f32 arctan( f32 a )
{
	f32 u  = a * a;
	f32 u2 = u * u;
	f32 u3 = u2 * u;
	f32 u4 = u3 * u;
	f32 f  = 1.0f + 0.33288950512027f * u - 0.08467922817644f * u2 + 0.03252232640125f * u3 - 0.00749305860992f * u4;
	return a / f;
}

f32 arctan2( f32 y, f32 x )
{
	if ( abs( x ) > abs( y ) )
	{
		f32 a = arctan( y / x );
		if ( x > 0.0f )
			return a;
		else
			return y > 0.0f ? a + ZPL_TAU_OVER_2 : a - ZPL_TAU_OVER_2;
	}
	else
	{
		f32 a = arctan( x / y );
		if ( x > 0.0f )
			return y > 0.0f ? ZPL_TAU_OVER_4 - a : -ZPL_TAU_OVER_4 - a;
		else
			return y > 0.0f ? ZPL_TAU_OVER_4 + a : -ZPL_TAU_OVER_4 + a;
	}
}

f32 exp( f32 a )
{
	union
	{
		f32 f;
		int i;
	} u, v;

	u.i = ( int )( 6051102 * a + 1056478197 );
	v.i = ( int )( 1056478197 - 6051102 * a );
	return u.f / v.f;
}

f32 log( f32 a )
{
	union
	{
		f32 f;
		int i;
	} u = { a };

	return ( u.i - 1064866805 ) * 8.262958405176314e-8f; /* 1 / 12102203.0; */
}

f32 pow( f32 a, f32 b )
{
	int flipped = 0, e;
	f32 f, r = 1.0f;
	if ( b < 0 )
	{
		flipped = 1;
		b       = -b;
	}

	e = ( int )b;
	f = exp( b - e );

	while ( e )
	{
		if ( e & 1 )
			r *= a;
		a  *= a;
		e >>= 1;
	}

	r *= f;
	return flipped ? 1.0f / r : r;
}

#	else

f32 rsqrt( f32 a )
{
	return 1.0f / __builtin_sqrt( a );
}

f32 sqrt( f32 a )
{
	return __builtin_sqrt( a );
}

f32 sin( f32 radians )
{
	return __builtin_sinf( radians );
}

f32 cos( f32 radians )
{
	return __builtin_cosf( radians );
}

f32 tan( f32 radians )
{
	return __builtin_tanf( radians );
}

f32 arcsin( f32 a )
{
	return __builtin_asinf( a );
}

f32 arccos( f32 a )
{
	return __builtin_acosf( a );
}

f32 arctan( f32 a )
{
	return __builtin_atanf( a );
}

f32 arctan2( f32 y, f32 x )
{
	return __builtin_atan2f( y, x );
}

f32 exp( f32 x )
{
	return __builtin_expf( x );
}

f32 log( f32 x )
{
	return __builtin_logf( x );
}

// TODO: Should this be zpl_exp(y * zpl_log(x)) ???
f32 pow( f32 x, f32 y )
{
	return __builtin_powf( x, y );
}

#	endif
#else
f32 rsqrt( f32 a )
{
	return 1.0f / sqrtf( a );
}

f32 sqrt( f32 a )
{
	return sqrtf( a );
};

f32 sin( f32 radians )
{
	return sinf( radians );
};

f32 cos( f32 radians )
{
	return cosf( radians );
};

f32 tan( f32 radians )
{
	return tanf( radians );
};

f32 arcsin( f32 a )
{
	return asinf( a );
};

f32 arccos( f32 a )
{
	return acosf( a );
};

f32 arctan( f32 a )
{
	return atanf( a );
};

f32 arctan2( f32 y, f32 x )
{
	return atan2f( y, x );
};

f32 exp( f32 x )
{
	return expf( x );
}

f32 log( f32 x )
{
	return logf( x );
}

f32 pow( f32 x, f32 y )
{
	return powf( x, y );
}
#endif

f32 exp2( f32 x )
{
	return exp( ZPL_LOG_TWO * x );
}

f32 log2( f32 x )
{
	return log( x ) / ZPL_LOG_TWO;
}

f32 fast_exp( f32 x )
{
	/* NOTE: Only works in the range -1 <= x <= +1 */
	f32 e = 1.0f + x * ( 1.0f + x * 0.5f * ( 1.0f + x * 0.3333333333f * ( 1.0f + x * 0.25f * ( 1.0f + x * 0.2f ) ) ) );
	return e;
}

f32 fast_exp2( f32 x )
{
	return fast_exp( ZPL_LOG_TWO * x );
}

f32 round( f32 x )
{
	return ( float )( ( x >= 0.0f ) ? floor( x + 0.5f ) : ceil( x - 0.5f ) );
}

f32 floor( f32 x )
{
	return ( float )( ( x >= 0.0f ) ? ( int )x : ( int )( x - 0.9999999999999999f ) );
}

f32 ceil( f32 x )
{
	return ( float )( ( x < 0.0f ) ? ( int )x : ( ( int )x ) + 1 );
}

f32 half_to_float( half value )
{
	union
	{
		unsigned int i;
		f32          f;
	} result;

	int s = ( value >> 15 ) & 0x001;
	int e = ( value >> 10 ) & 0x01f;
	int m = value & 0x3ff;

	if ( e == 0 )
	{
		if ( m == 0 )
		{
			/* Plus or minus zero */
			result.i = ( unsigned int )( s << 31 );
			return result.f;
		}
		else
		{
			/* Denormalized number */
			while ( ! ( m & 0x00000400 ) )
			{
				m <<= 1;
				e  -= 1;
			}

			e += 1;
			m &= ~0x00000400;
		}
	}
	else if ( e == 31 )
	{
		if ( m == 0 )
		{
			/* Positive or negative infinity */
			result.i = ( unsigned int )( ( s << 31 ) | 0x7f800000 );
			return result.f;
		}
		else
		{
			/* Nan */
			result.i = ( unsigned int )( ( s << 31 ) | 0x7f800000 | ( m << 13 ) );
			return result.f;
		}
	}

	e = e + ( 127 - 15 );
	m = m << 13;

	result.i = ( unsigned int )( ( s << 31 ) | ( e << 23 ) | m );
	return result.f;
}

half float_to_half( f32 value )
{
	union
	{
		unsigned int i;
		f32          f;
	} v;

	int i, s, e, m;

	v.f = value;
	i   = ( int )v.i;

	s = ( i >> 16 ) & 0x00008000;
	e = ( ( i >> 23 ) & 0x000000ff ) - ( 127 - 15 );
	m = i & 0x007fffff;

	if ( e <= 0 )
	{
		if ( e < -10 )
			return ( half )s;
		m = ( m | 0x00800000 ) >> ( 1 - e );

		if ( m & 0x00001000 )
			m += 0x00002000;

		return ( half )( s | ( m >> 13 ) );
	}
	else if ( e == 0xff - ( 127 - 15 ) )
	{
		if ( m == 0 )
		{
			return ( half )( s | 0x7c00 ); /* NOTE: infinity */
		}
		else
		{
			/* NOTE: NAN */
			m >>= 13;
			return ( half )( s | 0x7c00 | m | ( m == 0 ) );
		}
	}
	else
	{
		if ( m & 0x00001000 )
		{
			m += 0x00002000;
			if ( m & 0x00800000 )
			{
				m  = 0;
				e += 1;
			}
		}

		if ( e > 30 )
		{
			f32 volatile f = 1e12f;
			int j;
			for ( j = 0; j < 10; j++ )
				f *= f; /* NOTE: Cause overflow */

			return ( half )( s | 0x7c00 );
		}

		return ( half )( s | ( e << 10 ) | ( m >> 13 ) );
	}
}

#define ZPL_VEC2_2OP( a, c, post ) \
	a->x = c.x post;               \
	a->y = c.y post;

#define ZPL_VEC2_3OP( a, b, op, c, post ) \
	a->x = b.x op c.x post;               \
	a->y = b.y op c.y post;

#define ZPL_VEC3_2OP( a, c, post ) \
	a->x = c.x post;               \
	a->y = c.y post;               \
	a->z = c.z post;

#define ZPL_VEC3_3OP( a, b, op, c, post ) \
	a->x = b.x op c.x post;               \
	a->y = b.y op c.y post;               \
	a->z = b.z op c.z post;

#define ZPL_VEC4_2OP( a, c, post ) \
	a->x = c.x post;               \
	a->y = c.y post;               \
	a->z = c.z post;               \
	a->w = c.w post;

#define ZPL_VEC4_3OP( a, b, op, c, post ) \
	a->x = b.x op c.x post;               \
	a->y = b.y op c.y post;               \
	a->z = b.z op c.z post;               \
	a->w = b.w op c.w post;

vec2 vec2f_zero( void )
{
	vec2 v = { 0, 0 };
	return v;
}

vec2 vec2f( f32 x, f32 y )
{
	vec2 v;
	v.x = x;
	v.y = y;
	return v;
}

vec2 vec2fv( f32 x[ 2 ] )
{
	vec2 v;
	v.x = x[ 0 ];
	v.y = x[ 1 ];
	return v;
}

vec3 vec3f_zero( void )
{
	vec3 v = { 0, 0, 0 };
	return v;
}

vec3 vec3f( f32 x, f32 y, f32 z )
{
	vec3 v;
	v.x = x;
	v.y = y;
	v.z = z;
	return v;
}

vec3 vec3fv( f32 x[ 3 ] )
{
	vec3 v;
	v.x = x[ 0 ];
	v.y = x[ 1 ];
	v.z = x[ 2 ];
	return v;
}

vec4 vec4f_zero( void )
{
	vec4 v = { 0, 0, 0, 0 };
	return v;
}

vec4 vec4f( f32 x, f32 y, f32 z, f32 w )
{
	vec4 v;
	v.x = x;
	v.y = y;
	v.z = z;
	v.w = w;
	return v;
}

vec4 vec4fv( f32 x[ 4 ] )
{
	vec4 v;
	v.x = x[ 0 ];
	v.y = x[ 1 ];
	v.z = x[ 2 ];
	v.w = x[ 3 ];
	return v;
}

f32 vec2_max( vec2 v )
{
	return max( v.x, v.y );
}

f32 vec2_side( vec2 p, vec2 q, vec2 r )
{
	return ( ( q.x - p.x ) * ( r.y - p.y ) - ( r.x - p.x ) * ( q.y - p.y ) );
}

void vec2_add( vec2* d, vec2 v0, vec2 v1 )
{
	ZPL_VEC2_3OP( d, v0, +, v1, +0 );
}

void vec2_sub( vec2* d, vec2 v0, vec2 v1 )
{
	ZPL_VEC2_3OP( d, v0, -, v1, +0 );
}

void vec2_mul( vec2* d, vec2 v, f32 s )
{
	ZPL_VEC2_2OP( d, v, *s );
}

void vec2_div( vec2* d, vec2 v, f32 s )
{
	ZPL_VEC2_2OP( d, v, / s );
}

f32 vec3_max( vec3 v )
{
	return max3( v.x, v.y, v.z );
}

void vec3_add( vec3* d, vec3 v0, vec3 v1 )
{
	ZPL_VEC3_3OP( d, v0, +, v1, +0 );
}

void vec3_sub( vec3* d, vec3 v0, vec3 v1 )
{
	ZPL_VEC3_3OP( d, v0, -, v1, +0 );
}

void vec3_mul( vec3* d, vec3 v, f32 s )
{
	ZPL_VEC3_2OP( d, v, *s );
}

void vec3_div( vec3* d, vec3 v, f32 s )
{
	ZPL_VEC3_2OP( d, v, / s );
}

void vec4_add( vec4* d, vec4 v0, vec4 v1 )
{
	ZPL_VEC4_3OP( d, v0, +, v1, +0 );
}

void vec4_sub( vec4* d, vec4 v0, vec4 v1 )
{
	ZPL_VEC4_3OP( d, v0, -, v1, +0 );
}

void vec4_mul( vec4* d, vec4 v, f32 s )
{
	ZPL_VEC4_2OP( d, v, *s );
}

void vec4_div( vec4* d, vec4 v, f32 s )
{
	ZPL_VEC4_2OP( d, v, / s );
}

void vec2_addeq( vec2* d, vec2 v )
{
	ZPL_VEC2_3OP( d, ( *d ), +, v, +0 );
}

void vec2_subeq( vec2* d, vec2 v )
{
	ZPL_VEC2_3OP( d, ( *d ), -, v, +0 );
}

void vec2_muleq( vec2* d, f32 s )
{
	ZPL_VEC2_2OP( d, ( *d ), *s );
}

void vec2_diveq( vec2* d, f32 s )
{
	ZPL_VEC2_2OP( d, ( *d ), / s );
}

void vec3_addeq( vec3* d, vec3 v )
{
	ZPL_VEC3_3OP( d, ( *d ), +, v, +0 );
}

void vec3_subeq( vec3* d, vec3 v )
{
	ZPL_VEC3_3OP( d, ( *d ), -, v, +0 );
}

void vec3_muleq( vec3* d, f32 s )
{
	ZPL_VEC3_2OP( d, ( *d ), *s );
}

void vec3_diveq( vec3* d, f32 s )
{
	ZPL_VEC3_2OP( d, ( *d ), / s );
}

void vec4_addeq( vec4* d, vec4 v )
{
	ZPL_VEC4_3OP( d, ( *d ), +, v, +0 );
}

void vec4_subeq( vec4* d, vec4 v )
{
	ZPL_VEC4_3OP( d, ( *d ), -, v, +0 );
}

void vec4_muleq( vec4* d, f32 s )
{
	ZPL_VEC4_2OP( d, ( *d ), *s );
}

void vec4_diveq( vec4* d, f32 s )
{
	ZPL_VEC4_2OP( d, ( *d ), / s );
}

#undef ZPL_VEC2_2OP
#undef ZPL_VEC2_3OP
#undef ZPL_VEC3_3OP
#undef ZPL_VEC3_2OP
#undef ZPL_VEC4_2OP
#undef ZPL_VEC4_3OP

f32 vec2_dot( vec2 v0, vec2 v1 )
{
	return v0.x * v1.x + v0.y * v1.y;
}

f32 vec3_dot( vec3 v0, vec3 v1 )
{
	return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z;
}

f32 vec4_dot( vec4 v0, vec4 v1 )
{
	return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z + v0.w * v1.w;
}

void vec2_cross( f32* d, vec2 v0, vec2 v1 )
{
	*d = v0.x * v1.y - v1.x * v0.y;
}

void vec3_cross( vec3* d, vec3 v0, vec3 v1 )
{
	d->x = v0.y * v1.z - v0.z * v1.y;
	d->y = v0.z * v1.x - v0.x * v1.z;
	d->z = v0.x * v1.y - v0.y * v1.x;
}

f32 vec2_mag2( vec2 v )
{
	return vec2_dot( v, v );
}

f32 vec3_mag2( vec3 v )
{
	return vec3_dot( v, v );
}

f32 vec4_mag2( vec4 v )
{
	return vec4_dot( v, v );
}

/* TODO: Create custom sqrt function */
f32 vec2_mag( vec2 v )
{
	return sqrt( vec2_dot( v, v ) );
}

f32 vec3_mag( vec3 v )
{
	return sqrt( vec3_dot( v, v ) );
}

f32 vec4_mag( vec4 v )
{
	return sqrt( vec4_dot( v, v ) );
}

void vec2_norm( vec2* d, vec2 v )
{
	f32 inv_mag = rsqrt( vec2_dot( v, v ) );
	vec2_mul( d, v, inv_mag );
}

void vec3_norm( vec3* d, vec3 v )
{
	f32 inv_mag = rsqrt( vec3_dot( v, v ) );
	vec3_mul( d, v, inv_mag );
}

void vec4_norm( vec4* d, vec4 v )
{
	f32 inv_mag = rsqrt( vec4_dot( v, v ) );
	vec4_mul( d, v, inv_mag );
}

void vec2_norm0( vec2* d, vec2 v )
{
	f32 mag = vec2_mag( v );
	if ( mag > 0 )
		vec2_div( d, v, mag );
	else
		*d = vec2f_zero();
}

void vec3_norm0( vec3* d, vec3 v )
{
	f32 mag = vec3_mag( v );
	if ( mag > 0 )
		vec3_div( d, v, mag );
	else
		*d = vec3f_zero();
}

void vec4_norm0( vec4* d, vec4 v )
{
	f32 mag = vec4_mag( v );
	if ( mag > 0 )
		vec4_div( d, v, mag );
	else
		*d = vec4f_zero();
}

void vec2_reflect( vec2* d, vec2 i, vec2 n )
{
	vec2 b = n;
	vec2_muleq( &b, 2.0f * vec2_dot( n, i ) );
	vec2_sub( d, i, b );
}

void vec3_reflect( vec3* d, vec3 i, vec3 n )
{
	vec3 b = n;
	vec3_muleq( &b, 2.0f * vec3_dot( n, i ) );
	vec3_sub( d, i, b );
}

void vec2_refract( vec2* d, vec2 i, vec2 n, f32 eta )
{
	vec2 a, b;
	f32  dv, k;

	dv = vec2_dot( n, i );
	k  = 1.0f - eta * eta * ( 1.0f - dv * dv );
	vec2_mul( &a, i, eta );
	vec2_mul( &b, n, eta * dv * sqrt( k ) );
	vec2_sub( d, a, b );
	vec2_muleq( d, ( float )( k >= 0.0f ) );
}

void vec3_refract( vec3* d, vec3 i, vec3 n, f32 eta )
{
	vec3 a, b;
	f32  dv, k;

	dv = vec3_dot( n, i );
	k  = 1.0f - eta * eta * ( 1.0f - dv * dv );
	vec3_mul( &a, i, eta );
	vec3_mul( &b, n, eta * dv * sqrt( k ) );
	vec3_sub( d, a, b );
	vec3_muleq( d, ( float )( k >= 0.0f ) );
}

f32 vec2_aspect_ratio( vec2 v )
{
	return ( v.y < 0.0001f ) ? 0.0f : v.x / v.y;
}

void mat2_transpose( mat2* m )
{
	float22_transpose( float22_m( m ) );
}

void mat2_identity( mat2* m )
{
	float22_identity( float22_m( m ) );
}

void mat2_mul( mat2* out, mat2* m1, mat2* m2 )
{
	float22_mul( float22_m( out ), float22_m( m1 ), float22_m( m2 ) );
}

void float22_identity( f32 m[ 2 ][ 2 ] )
{
	m[ 0 ][ 0 ] = 1;
	m[ 0 ][ 1 ] = 0;
	m[ 1 ][ 0 ] = 0;
	m[ 1 ][ 1 ] = 1;
}

void mat2_copy( mat2* out, mat2* m )
{
	memcopy( out, m, sizeof( mat3 ) );
}

void mat2_mul_vec2( vec2* out, mat2* m, vec2 in )
{
	float22_mul_vec2( out, float22_m( m ), in );
}

mat2* mat2_v( vec2 m[ 2 ] )
{
	return ( mat2* )m;
}

mat2* mat2_f( f32 m[ 2 ][ 2 ] )
{
	return ( mat2* )m;
}

float2* float22_m( mat2* m )
{
	return ( float2* )m;
}

float2* float22_v( vec2 m[ 2 ] )
{
	return ( float2* )m;
}

float2* float22_4( f32 m[ 4 ] )
{
	return ( float2* )m;
}

void float22_transpose( f32 ( *vec )[ 2 ] )
{
	int i, j;
	for ( j = 0; j < 2; j++ )
	{
		for ( i = j + 1; i < 2; i++ )
		{
			f32 t         = vec[ i ][ j ];
			vec[ i ][ j ] = vec[ j ][ i ];
			vec[ j ][ i ] = t;
		}
	}
}

void float22_mul( f32 ( *out )[ 2 ], f32 ( *mat1 )[ 2 ], f32 ( *mat2 )[ 2 ] )
{
	int i, j;
	f32 temp1[ 2 ][ 2 ], temp2[ 2 ][ 2 ];
	if ( mat1 == out )
	{
		memcopy( temp1, mat1, sizeof( temp1 ) );
		mat1 = temp1;
	}
	if ( mat2 == out )
	{
		memcopy( temp2, mat2, sizeof( temp2 ) );
		mat2 = temp2;
	}
	for ( j = 0; j < 2; j++ )
	{
		for ( i = 0; i < 2; i++ )
		{
			out[ j ][ i ] = mat1[ 0 ][ i ] * mat2[ j ][ 0 ] + mat1[ 1 ][ i ] * mat2[ j ][ 1 ];
		}
	}
}

void float22_mul_vec2( vec2* out, f32 m[ 2 ][ 2 ], vec2 v )
{
	out->x = m[ 0 ][ 0 ] * v.x + m[ 0 ][ 1 ] * v.y;
	out->y = m[ 1 ][ 0 ] * v.x + m[ 1 ][ 1 ] * v.y;
}

f32 mat2_determinate( mat2* m )
{
	float2* e = float22_m( m );
	return e[ 0 ][ 0 ] * e[ 1 ][ 1 ] - e[ 1 ][ 0 ] * e[ 0 ][ 1 ];
}

void mat2_inverse( mat2* out, mat2* in )
{
	float2* o = float22_m( out );
	float2* i = float22_m( in );

	f32 ood = 1.0f / mat2_determinate( in );

	o[ 0 ][ 0 ] = +i[ 1 ][ 1 ] * ood;
	o[ 0 ][ 1 ] = -i[ 0 ][ 1 ] * ood;
	o[ 1 ][ 0 ] = -i[ 1 ][ 0 ] * ood;
	o[ 1 ][ 1 ] = +i[ 0 ][ 0 ] * ood;
}

void mat3_transpose( mat3* m )
{
	float33_transpose( float33_m( m ) );
}

void mat3_identity( mat3* m )
{
	float33_identity( float33_m( m ) );
}

void mat3_copy( mat3* out, mat3* m )
{
	memcopy( out, m, sizeof( mat3 ) );
}

void mat3_mul( mat3* out, mat3* m1, mat3* m2 )
{
	float33_mul( float33_m( out ), float33_m( m1 ), float33_m( m2 ) );
}

void float33_identity( f32 m[ 3 ][ 3 ] )
{
	m[ 0 ][ 0 ] = 1;
	m[ 0 ][ 1 ] = 0;
	m[ 0 ][ 2 ] = 0;
	m[ 1 ][ 0 ] = 0;
	m[ 1 ][ 1 ] = 1;
	m[ 1 ][ 2 ] = 0;
	m[ 2 ][ 0 ] = 0;
	m[ 2 ][ 1 ] = 0;
	m[ 2 ][ 2 ] = 1;
}

void mat3_mul_vec3( vec3* out, mat3* m, vec3 in )
{
	float33_mul_vec3( out, float33_m( m ), in );
}

mat3* mat3_v( vec3 m[ 3 ] )
{
	return ( mat3* )m;
}

mat3* mat3_f( f32 m[ 3 ][ 3 ] )
{
	return ( mat3* )m;
}

float3* float33_m( mat3* m )
{
	return ( float3* )m;
}

float3* float33_v( vec3 m[ 3 ] )
{
	return ( float3* )m;
}

float3* float33_9( f32 m[ 9 ] )
{
	return ( float3* )m;
}

void float33_transpose( f32 ( *vec )[ 3 ] )
{
	int i, j;
	for ( j = 0; j < 3; j++ )
	{
		for ( i = j + 1; i < 3; i++ )
		{
			f32 t         = vec[ i ][ j ];
			vec[ i ][ j ] = vec[ j ][ i ];
			vec[ j ][ i ] = t;
		}
	}
}

void float33_mul( f32 ( *out )[ 3 ], f32 ( *mat1 )[ 3 ], f32 ( *mat2 )[ 3 ] )
{
	int i, j;
	f32 temp1[ 3 ][ 3 ], temp2[ 3 ][ 3 ];
	if ( mat1 == out )
	{
		memcopy( temp1, mat1, sizeof( temp1 ) );
		mat1 = temp1;
	}
	if ( mat2 == out )
	{
		memcopy( temp2, mat2, sizeof( temp2 ) );
		mat2 = temp2;
	}
	for ( j = 0; j < 3; j++ )
	{
		for ( i = 0; i < 3; i++ )
		{
			out[ j ][ i ] = mat1[ 0 ][ i ] * mat2[ j ][ 0 ] + mat1[ 1 ][ i ] * mat2[ j ][ 1 ] + mat1[ 2 ][ i ] * mat2[ j ][ 2 ];
		}
	}
}

void float33_mul_vec3( vec3* out, f32 m[ 3 ][ 3 ], vec3 v )
{
	out->x = m[ 0 ][ 0 ] * v.x + m[ 0 ][ 1 ] * v.y + m[ 0 ][ 2 ] * v.z;
	out->y = m[ 1 ][ 0 ] * v.x + m[ 1 ][ 1 ] * v.y + m[ 1 ][ 2 ] * v.z;
	out->z = m[ 2 ][ 0 ] * v.x + m[ 2 ][ 1 ] * v.y + m[ 2 ][ 2 ] * v.z;
}

f32 mat3_determinate( mat3* m )
{
	float3* e = float33_m( m );
	f32     d = +e[ 0 ][ 0 ] * ( e[ 1 ][ 1 ] * e[ 2 ][ 2 ] - e[ 1 ][ 2 ] * e[ 2 ][ 1 ] ) - e[ 0 ][ 1 ] * ( e[ 1 ][ 0 ] * e[ 2 ][ 2 ] - e[ 1 ][ 2 ] * e[ 2 ][ 0 ] )
	    + e[ 0 ][ 2 ] * ( e[ 1 ][ 0 ] * e[ 2 ][ 1 ] - e[ 1 ][ 1 ] * e[ 2 ][ 0 ] );
	return d;
}

void mat3_inverse( mat3* out, mat3* in )
{
	float3* o = float33_m( out );
	float3* i = float33_m( in );

	f32 ood = 1.0f / mat3_determinate( in );

	o[ 0 ][ 0 ] = +( i[ 1 ][ 1 ] * i[ 2 ][ 2 ] - i[ 2 ][ 1 ] * i[ 1 ][ 2 ] ) * ood;
	o[ 0 ][ 1 ] = -( i[ 1 ][ 0 ] * i[ 2 ][ 2 ] - i[ 2 ][ 0 ] * i[ 1 ][ 2 ] ) * ood;
	o[ 0 ][ 2 ] = +( i[ 1 ][ 0 ] * i[ 2 ][ 1 ] - i[ 2 ][ 0 ] * i[ 1 ][ 1 ] ) * ood;
	o[ 1 ][ 0 ] = -( i[ 0 ][ 1 ] * i[ 2 ][ 2 ] - i[ 2 ][ 1 ] * i[ 0 ][ 2 ] ) * ood;
	o[ 1 ][ 1 ] = +( i[ 0 ][ 0 ] * i[ 2 ][ 2 ] - i[ 2 ][ 0 ] * i[ 0 ][ 2 ] ) * ood;
	o[ 1 ][ 2 ] = -( i[ 0 ][ 0 ] * i[ 2 ][ 1 ] - i[ 2 ][ 0 ] * i[ 0 ][ 1 ] ) * ood;
	o[ 2 ][ 0 ] = +( i[ 0 ][ 1 ] * i[ 1 ][ 2 ] - i[ 1 ][ 1 ] * i[ 0 ][ 2 ] ) * ood;
	o[ 2 ][ 1 ] = -( i[ 0 ][ 0 ] * i[ 1 ][ 2 ] - i[ 1 ][ 0 ] * i[ 0 ][ 2 ] ) * ood;
	o[ 2 ][ 2 ] = +( i[ 0 ][ 0 ] * i[ 1 ][ 1 ] - i[ 1 ][ 0 ] * i[ 0 ][ 1 ] ) * ood;
}

void mat4_transpose( mat4* m )
{
	float44_transpose( float44_m( m ) );
}

void mat4_identity( mat4* m )
{
	float44_identity( float44_m( m ) );
}

void mat4_copy( mat4* out, mat4* m )
{
	memcopy( out, m, sizeof( mat4 ) );
}

void mat4_mul( mat4* out, mat4* m1, mat4* m2 )
{
	float44_mul( float44_m( out ), float44_m( m1 ), float44_m( m2 ) );
}

void float44_identity( f32 m[ 4 ][ 4 ] )
{
	m[ 0 ][ 0 ] = 1;
	m[ 0 ][ 1 ] = 0;
	m[ 0 ][ 2 ] = 0;
	m[ 0 ][ 3 ] = 0;
	m[ 1 ][ 0 ] = 0;
	m[ 1 ][ 1 ] = 1;
	m[ 1 ][ 2 ] = 0;
	m[ 1 ][ 3 ] = 0;
	m[ 2 ][ 0 ] = 0;
	m[ 2 ][ 1 ] = 0;
	m[ 2 ][ 2 ] = 1;
	m[ 2 ][ 3 ] = 0;
	m[ 3 ][ 0 ] = 0;
	m[ 3 ][ 1 ] = 0;
	m[ 3 ][ 2 ] = 0;
	m[ 3 ][ 3 ] = 1;
}

void mat4_mul_vec4( vec4* out, mat4* m, vec4 in )
{
	float44_mul_vec4( out, float44_m( m ), in );
}

mat4* mat4_v( vec4 m[ 4 ] )
{
	return ( mat4* )m;
}

mat4* mat4_f( f32 m[ 4 ][ 4 ] )
{
	return ( mat4* )m;
}

float4* float44_m( mat4* m )
{
	return ( float4* )m;
}

float4* float44_v( vec4 m[ 4 ] )
{
	return ( float4* )m;
}

float4* float44_16( f32 m[ 16 ] )
{
	return ( float4* )m;
}

void float44_transpose( f32 ( *vec )[ 4 ] )
{
	f32 tmp;
	tmp           = vec[ 1 ][ 0 ];
	vec[ 1 ][ 0 ] = vec[ 0 ][ 1 ];
	vec[ 0 ][ 1 ] = tmp;
	tmp           = vec[ 2 ][ 0 ];
	vec[ 2 ][ 0 ] = vec[ 0 ][ 2 ];
	vec[ 0 ][ 2 ] = tmp;
	tmp           = vec[ 3 ][ 0 ];
	vec[ 3 ][ 0 ] = vec[ 0 ][ 3 ];
	vec[ 0 ][ 3 ] = tmp;
	tmp           = vec[ 2 ][ 1 ];
	vec[ 2 ][ 1 ] = vec[ 1 ][ 2 ];
	vec[ 1 ][ 2 ] = tmp;
	tmp           = vec[ 3 ][ 1 ];
	vec[ 3 ][ 1 ] = vec[ 1 ][ 3 ];
	vec[ 1 ][ 3 ] = tmp;
	tmp           = vec[ 3 ][ 2 ];
	vec[ 3 ][ 2 ] = vec[ 2 ][ 3 ];
	vec[ 2 ][ 3 ] = tmp;
}

void float44_mul( f32 ( *out )[ 4 ], f32 ( *mat1 )[ 4 ], f32 ( *mat2 )[ 4 ] )
{
	int i, j;
	f32 temp1[ 4 ][ 4 ], temp2[ 4 ][ 4 ];
	if ( mat1 == out )
	{
		memcopy( temp1, mat1, sizeof( temp1 ) );
		mat1 = temp1;
	}
	if ( mat2 == out )
	{
		memcopy( temp2, mat2, sizeof( temp2 ) );
		mat2 = temp2;
	}
	for ( j = 0; j < 4; j++ )
	{
		for ( i = 0; i < 4; i++ )
		{
			out[ j ][ i ] = mat1[ 0 ][ i ] * mat2[ j ][ 0 ] + mat1[ 1 ][ i ] * mat2[ j ][ 1 ] + mat1[ 2 ][ i ] * mat2[ j ][ 2 ] + mat1[ 3 ][ i ] * mat2[ j ][ 3 ];
		}
	}
}

void float44_mul_vec4( vec4* out, f32 m[ 4 ][ 4 ], vec4 v )
{
	out->x = m[ 0 ][ 0 ] * v.x + m[ 1 ][ 0 ] * v.y + m[ 2 ][ 0 ] * v.z + m[ 3 ][ 0 ] * v.w;
	out->y = m[ 0 ][ 1 ] * v.x + m[ 1 ][ 1 ] * v.y + m[ 2 ][ 1 ] * v.z + m[ 3 ][ 1 ] * v.w;
	out->z = m[ 0 ][ 2 ] * v.x + m[ 1 ][ 2 ] * v.y + m[ 2 ][ 2 ] * v.z + m[ 3 ][ 2 ] * v.w;
	out->w = m[ 0 ][ 3 ] * v.x + m[ 1 ][ 3 ] * v.y + m[ 2 ][ 3 ] * v.z + m[ 3 ][ 3 ] * v.w;
}

void mat4_inverse( mat4* out, mat4* in )
{
	float4* o = float44_m( out );
	float4* m = float44_m( in );

	f32 ood;

	f32 sf00 = m[ 2 ][ 2 ] * m[ 3 ][ 3 ] - m[ 3 ][ 2 ] * m[ 2 ][ 3 ];
	f32 sf01 = m[ 2 ][ 1 ] * m[ 3 ][ 3 ] - m[ 3 ][ 1 ] * m[ 2 ][ 3 ];
	f32 sf02 = m[ 2 ][ 1 ] * m[ 3 ][ 2 ] - m[ 3 ][ 1 ] * m[ 2 ][ 2 ];
	f32 sf03 = m[ 2 ][ 0 ] * m[ 3 ][ 3 ] - m[ 3 ][ 0 ] * m[ 2 ][ 3 ];
	f32 sf04 = m[ 2 ][ 0 ] * m[ 3 ][ 2 ] - m[ 3 ][ 0 ] * m[ 2 ][ 2 ];
	f32 sf05 = m[ 2 ][ 0 ] * m[ 3 ][ 1 ] - m[ 3 ][ 0 ] * m[ 2 ][ 1 ];
	f32 sf06 = m[ 1 ][ 2 ] * m[ 3 ][ 3 ] - m[ 3 ][ 2 ] * m[ 1 ][ 3 ];
	f32 sf07 = m[ 1 ][ 1 ] * m[ 3 ][ 3 ] - m[ 3 ][ 1 ] * m[ 1 ][ 3 ];
	f32 sf08 = m[ 1 ][ 1 ] * m[ 3 ][ 2 ] - m[ 3 ][ 1 ] * m[ 1 ][ 2 ];
	f32 sf09 = m[ 1 ][ 0 ] * m[ 3 ][ 3 ] - m[ 3 ][ 0 ] * m[ 1 ][ 3 ];
	f32 sf10 = m[ 1 ][ 0 ] * m[ 3 ][ 2 ] - m[ 3 ][ 0 ] * m[ 1 ][ 2 ];
	f32 sf11 = m[ 1 ][ 1 ] * m[ 3 ][ 3 ] - m[ 3 ][ 1 ] * m[ 1 ][ 3 ];
	f32 sf12 = m[ 1 ][ 0 ] * m[ 3 ][ 1 ] - m[ 3 ][ 0 ] * m[ 1 ][ 1 ];
	f32 sf13 = m[ 1 ][ 2 ] * m[ 2 ][ 3 ] - m[ 2 ][ 2 ] * m[ 1 ][ 3 ];
	f32 sf14 = m[ 1 ][ 1 ] * m[ 2 ][ 3 ] - m[ 2 ][ 1 ] * m[ 1 ][ 3 ];
	f32 sf15 = m[ 1 ][ 1 ] * m[ 2 ][ 2 ] - m[ 2 ][ 1 ] * m[ 1 ][ 2 ];
	f32 sf16 = m[ 1 ][ 0 ] * m[ 2 ][ 3 ] - m[ 2 ][ 0 ] * m[ 1 ][ 3 ];
	f32 sf17 = m[ 1 ][ 0 ] * m[ 2 ][ 2 ] - m[ 2 ][ 0 ] * m[ 1 ][ 2 ];
	f32 sf18 = m[ 1 ][ 0 ] * m[ 2 ][ 1 ] - m[ 2 ][ 0 ] * m[ 1 ][ 1 ];

	o[ 0 ][ 0 ] = +( m[ 1 ][ 1 ] * sf00 - m[ 1 ][ 2 ] * sf01 + m[ 1 ][ 3 ] * sf02 );
	o[ 1 ][ 0 ] = -( m[ 1 ][ 0 ] * sf00 - m[ 1 ][ 2 ] * sf03 + m[ 1 ][ 3 ] * sf04 );
	o[ 2 ][ 0 ] = +( m[ 1 ][ 0 ] * sf01 - m[ 1 ][ 1 ] * sf03 + m[ 1 ][ 3 ] * sf05 );
	o[ 3 ][ 0 ] = -( m[ 1 ][ 0 ] * sf02 - m[ 1 ][ 1 ] * sf04 + m[ 1 ][ 2 ] * sf05 );

	o[ 0 ][ 1 ] = -( m[ 0 ][ 1 ] * sf00 - m[ 0 ][ 2 ] * sf01 + m[ 0 ][ 3 ] * sf02 );
	o[ 1 ][ 1 ] = +( m[ 0 ][ 0 ] * sf00 - m[ 0 ][ 2 ] * sf03 + m[ 0 ][ 3 ] * sf04 );
	o[ 2 ][ 1 ] = -( m[ 0 ][ 0 ] * sf01 - m[ 0 ][ 1 ] * sf03 + m[ 0 ][ 3 ] * sf05 );
	o[ 3 ][ 1 ] = +( m[ 0 ][ 0 ] * sf02 - m[ 0 ][ 1 ] * sf04 + m[ 0 ][ 2 ] * sf05 );

	o[ 0 ][ 2 ] = +( m[ 0 ][ 1 ] * sf06 - m[ 0 ][ 2 ] * sf07 + m[ 0 ][ 3 ] * sf08 );
	o[ 1 ][ 2 ] = -( m[ 0 ][ 0 ] * sf06 - m[ 0 ][ 2 ] * sf09 + m[ 0 ][ 3 ] * sf10 );
	o[ 2 ][ 2 ] = +( m[ 0 ][ 0 ] * sf11 - m[ 0 ][ 1 ] * sf09 + m[ 0 ][ 3 ] * sf12 );
	o[ 3 ][ 2 ] = -( m[ 0 ][ 0 ] * sf08 - m[ 0 ][ 1 ] * sf10 + m[ 0 ][ 2 ] * sf12 );

	o[ 0 ][ 3 ] = -( m[ 0 ][ 1 ] * sf13 - m[ 0 ][ 2 ] * sf14 + m[ 0 ][ 3 ] * sf15 );
	o[ 1 ][ 3 ] = +( m[ 0 ][ 0 ] * sf13 - m[ 0 ][ 2 ] * sf16 + m[ 0 ][ 3 ] * sf17 );
	o[ 2 ][ 3 ] = -( m[ 0 ][ 0 ] * sf14 - m[ 0 ][ 1 ] * sf16 + m[ 0 ][ 3 ] * sf18 );
	o[ 3 ][ 3 ] = +( m[ 0 ][ 0 ] * sf15 - m[ 0 ][ 1 ] * sf17 + m[ 0 ][ 2 ] * sf18 );

	ood = 1.0f / ( m[ 0 ][ 0 ] * o[ 0 ][ 0 ] + m[ 0 ][ 1 ] * o[ 1 ][ 0 ] + m[ 0 ][ 2 ] * o[ 2 ][ 0 ] + m[ 0 ][ 3 ] * o[ 3 ][ 0 ] );

	o[ 0 ][ 0 ] *= ood;
	o[ 1 ][ 0 ] *= ood;
	o[ 2 ][ 0 ] *= ood;
	o[ 3 ][ 0 ] *= ood;
	o[ 0 ][ 1 ] *= ood;
	o[ 1 ][ 1 ] *= ood;
	o[ 2 ][ 1 ] *= ood;
	o[ 3 ][ 1 ] *= ood;
	o[ 0 ][ 2 ] *= ood;
	o[ 1 ][ 2 ] *= ood;
	o[ 2 ][ 2 ] *= ood;
	o[ 3 ][ 2 ] *= ood;
	o[ 0 ][ 3 ] *= ood;
	o[ 1 ][ 3 ] *= ood;
	o[ 2 ][ 3 ] *= ood;
	o[ 3 ][ 3 ] *= ood;
}

void mat4_axis_angle( mat4* out, vec3 v, f32 angle_radians )
{
	f32     c, s;
	vec3    axis, t;
	float4* rot;

	c = cos( angle_radians );
	s = sin( angle_radians );

	vec3_norm( &axis, v );
	vec3_mul( &t, axis, 1.0f - c );

	mat4_identity( out );
	rot = float44_m( out );

	rot[ 0 ][ 0 ] = c + t.x * axis.x;
	rot[ 0 ][ 1 ] = 0 + t.x * axis.y + s * axis.z;
	rot[ 0 ][ 2 ] = 0 + t.x * axis.z - s * axis.y;
	rot[ 0 ][ 3 ] = 0;

	rot[ 1 ][ 0 ] = 0 + t.y * axis.x - s * axis.z;
	rot[ 1 ][ 1 ] = c + t.y * axis.y;
	rot[ 1 ][ 2 ] = 0 + t.y * axis.z + s * axis.x;
	rot[ 1 ][ 3 ] = 0;

	rot[ 2 ][ 0 ] = 0 + t.z * axis.x + s * axis.y;
	rot[ 2 ][ 1 ] = 0 + t.z * axis.y - s * axis.x;
	rot[ 2 ][ 2 ] = c + t.z * axis.z;
	rot[ 2 ][ 3 ] = 0;
}

void mat4_to_translate( mat4* out, vec3 v )
{
	mat4_identity( out );
	out->col[ 3 ].xyz = v;
}

void mat4_to_rotate( mat4* out, vec3 v, f32 angle_radians )
{
	mat4_axis_angle( out, v, angle_radians );
}

void mat4_to_scale( mat4* out, vec3 v )
{
	mat4_identity( out );
	out->col[ 0 ].x = v.x;
	out->col[ 1 ].y = v.y;
	out->col[ 2 ].z = v.z;
}

void mat4_to_scalef( mat4* out, f32 s )
{
	mat4_identity( out );
	out->col[ 0 ].x = s;
	out->col[ 1 ].y = s;
	out->col[ 2 ].z = s;
}

void mat4_translate( mat4* m, vec3 v )
{
	mat4 mm;
	mat4_to_translate( &mm, v );
	mat4_mul( m, m, &mm );
}

void mat4_rotate( mat4* m, vec3 v, f32 angle_radians )
{
	mat4 mm;
	mat4_axis_angle( &mm, v, angle_radians );
	mat4_mul( m, m, &mm );
}

void mat4_scale( mat4* m, vec3 v )
{
	mat4 mm;
	mat4_to_scale( &mm, v );
	mat4_mul( m, m, &mm );
}

void mat4_scalef( mat4* m, f32 s )
{
	mat4 mm;
	mat4_to_scalef( &mm, s );
	mat4_mul( m, m, &mm );
}

void mat4_ortho2d( mat4* out, f32 left, f32 right, f32 bottom, f32 top )
{
	float4* m;
	mat4_identity( out );
	m = float44_m( out );

	m[ 0 ][ 0 ] = 2.0f / ( right - left );
	m[ 1 ][ 1 ] = 2.0f / ( top - bottom );
	m[ 2 ][ 2 ] = -1.0f;
	m[ 3 ][ 0 ] = -( right + left ) / ( right - left );
	m[ 3 ][ 1 ] = -( top + bottom ) / ( top - bottom );
}

void mat4_ortho3d( mat4* out, f32 left, f32 right, f32 bottom, f32 top, f32 z_near, f32 z_far )
{
	float4* m;
	mat4_identity( out );
	m = float44_m( out );

	m[ 0 ][ 0 ] = +2.0f / ( right - left );
	m[ 1 ][ 1 ] = +2.0f / ( top - bottom );
	m[ 2 ][ 2 ] = -2.0f / ( z_far - z_near );
	m[ 3 ][ 0 ] = -( right + left ) / ( right - left );
	m[ 3 ][ 1 ] = -( top + bottom ) / ( top - bottom );
	m[ 3 ][ 2 ] = -( z_far + z_near ) / ( z_far - z_near );
}

void mat4_perspective( mat4* out, f32 fovy, f32 aspect, f32 z_near, f32 z_far )
{
	f32     tan_half_fovy = tan( 0.5f * fovy );
	mat4    zero_mat      = { 0 };
	float4* m             = float44_m( out );
	*out                  = zero_mat;

	m[ 0 ][ 0 ] = 1.0f / ( aspect * tan_half_fovy );
	m[ 1 ][ 1 ] = 1.0f / ( tan_half_fovy );
	m[ 2 ][ 2 ] = -( z_far + z_near ) / ( z_far - z_near );
	m[ 2 ][ 3 ] = -1.0f;
	m[ 3 ][ 2 ] = -2.0f * z_far * z_near / ( z_far - z_near );
}

void mat4_infinite_perspective( mat4* out, f32 fovy, f32 aspect, f32 z_near )
{
	f32     range    = tan( 0.5f * fovy ) * z_near;
	f32     left     = -range * aspect;
	f32     right    = range * aspect;
	f32     bottom   = -range;
	f32     top      = range;
	mat4    zero_mat = { 0 };
	float4* m        = float44_m( out );
	*out             = zero_mat;

	m[ 0 ][ 0 ] = ( 2.0f * z_near ) / ( right - left );
	m[ 1 ][ 1 ] = ( 2.0f * z_near ) / ( top - bottom );
	m[ 2 ][ 2 ] = -1.0f;
	m[ 2 ][ 3 ] = -1.0f;
	m[ 3 ][ 2 ] = -2.0f * z_near;
}

void mat4_ortho2d_dx( mat4* out, f32 left, f32 right, f32 bottom, f32 top )
{
	float4* m;
	mat4_identity( out );
	m = float44_m( out );

	m[ 0 ][ 0 ] = 2.0f / ( right - left );
	m[ 1 ][ 1 ] = 2.0f / ( top - bottom );
	m[ 2 ][ 2 ] = -1.0f;
	m[ 3 ][ 0 ] = -( right + left ) / ( right - left );
	m[ 3 ][ 1 ] = -( top + bottom ) / ( top - bottom );
}

void mat4_ortho3d_dx( mat4* out, f32 left, f32 right, f32 bottom, f32 top, f32 z_near, f32 z_far )
{
	float4* m;
	mat4_identity( out );
	m = float44_m( out );

	m[ 0 ][ 0 ] = +2.0f / ( right - left );
	m[ 1 ][ 1 ] = +2.0f / ( top - bottom );
	m[ 2 ][ 2 ] = -1.0f / ( z_far - z_near );
	m[ 3 ][ 0 ] = -( right + left ) / ( right - left );
	m[ 3 ][ 1 ] = -( top + bottom ) / ( top - bottom );
	m[ 3 ][ 2 ] = -( z_near ) / ( z_far - z_near );
}

void mat4_perspective_dx( mat4* out, f32 fovy, f32 aspect, f32 z_near, f32 z_far )
{
	f32     tan_half_fovy = tan( 0.5f * fovy );
	mat4    zero_mat      = { 0 };
	float4* m             = float44_m( out );
	*out                  = zero_mat;

	m[ 0 ][ 0 ] = 1.0f / ( aspect * tan_half_fovy );
	m[ 1 ][ 1 ] = 1.0f / ( tan_half_fovy );
	m[ 2 ][ 2 ] = -( z_far ) / ( z_far - z_near );
	m[ 2 ][ 3 ] = -1.0f;
	m[ 3 ][ 2 ] = -z_near / ( z_far - z_near );
}

void mat4_infinite_perspective_dx( mat4* out, f32 fovy, f32 aspect, f32 z_near )
{
	f32     tan_half_fovy = tan( 0.5f * fovy );
	mat4    zero_mat      = { 0 };
	float4* m             = float44_m( out );
	*out                  = zero_mat;

	m[ 0 ][ 0 ] = 1.0f / ( aspect * tan_half_fovy );
	m[ 1 ][ 1 ] = 1.0f / ( tan_half_fovy );
	m[ 2 ][ 2 ] = -1.0f;
	m[ 2 ][ 3 ] = -1.0f;
	m[ 3 ][ 2 ] = -z_near;
}

void mat4_look_at( mat4* out, vec3 eye, vec3 centre, vec3 up )
{
	vec3    f, s, u;
	float4* m;

	vec3_sub( &f, centre, eye );
	vec3_norm( &f, f );

	vec3_cross( &s, f, up );
	vec3_norm( &s, s );

	vec3_cross( &u, s, f );

	mat4_identity( out );
	m = float44_m( out );

	m[ 0 ][ 0 ] = +s.x;
	m[ 1 ][ 0 ] = +s.y;
	m[ 2 ][ 0 ] = +s.z;

	m[ 0 ][ 1 ] = +u.x;
	m[ 1 ][ 1 ] = +u.y;
	m[ 2 ][ 1 ] = +u.z;

	m[ 0 ][ 2 ] = -f.x;
	m[ 1 ][ 2 ] = -f.y;
	m[ 2 ][ 2 ] = -f.z;

	m[ 3 ][ 0 ] = -vec3_dot( s, eye );
	m[ 3 ][ 1 ] = -vec3_dot( u, eye );
	m[ 3 ][ 2 ] = +vec3_dot( f, eye );
}

void mat4_look_at_lh( mat4* out, vec3 eye, vec3 centre, vec3 up )
{
	vec3    f, s, u;
	float4* m;

	vec3_sub( &f, centre, eye );
	vec3_norm( &f, f );

	vec3_cross( &s, up, f );
	vec3_norm( &s, s );

	vec3_cross( &u, f, s );

	mat4_identity( out );
	m = float44_m( out );

	m[ 0 ][ 0 ] = +s.x;
	m[ 1 ][ 0 ] = +s.y;
	m[ 2 ][ 0 ] = +s.z;

	m[ 0 ][ 1 ] = +u.x;
	m[ 1 ][ 1 ] = +u.y;
	m[ 2 ][ 1 ] = +u.z;

	m[ 0 ][ 2 ] = +f.x;
	m[ 1 ][ 2 ] = +f.y;
	m[ 2 ][ 2 ] = +f.z;

	m[ 3 ][ 0 ] = -vec3_dot( s, eye );
	m[ 3 ][ 1 ] = -vec3_dot( u, eye );
	m[ 3 ][ 2 ] = -vec3_dot( f, eye );
}

quat quatf( f32 x, f32 y, f32 z, f32 w )
{
	quat q;
	q.x = x;
	q.y = y;
	q.z = z;
	q.w = w;
	return q;
}

quat quatfv( f32 e[ 4 ] )
{
	quat q;
	q.x = e[ 0 ];
	q.y = e[ 1 ];
	q.z = e[ 2 ];
	q.w = e[ 3 ];
	return q;
}

quat quat_axis_angle( vec3 axis, f32 angle_radians )
{
	quat q;
	vec3_norm( &q.xyz, axis );
	vec3_muleq( &q.xyz, sin( 0.5f * angle_radians ) );
	q.w = cos( 0.5f * angle_radians );
	return q;
}

quat quat_euler_angles( f32 pitch, f32 yaw, f32 roll )
{
	/* TODO: Do without multiplication, i.e. make it faster */
	quat q, p, y, r;
	p = quat_axis_angle( vec3f( 1, 0, 0 ), pitch );
	y = quat_axis_angle( vec3f( 0, 1, 0 ), yaw );
	r = quat_axis_angle( vec3f( 0, 0, 1 ), roll );

	quat_mul( &q, y, p );
	quat_muleq( &q, r );

	return q;
}

quat quat_identity( void )
{
	quat q = { 0, 0, 0, 1 };
	return q;
}

void quat_add( quat* d, quat q0, quat q1 )
{
	vec4_add( &d->xyzw, q0.xyzw, q1.xyzw );
}

void quat_sub( quat* d, quat q0, quat q1 )
{
	vec4_sub( &d->xyzw, q0.xyzw, q1.xyzw );
}

void quat_mul( quat* d, quat q0, quat q1 )
{
	d->x = q0.w * q1.x + q0.x * q1.w + q0.y * q1.z - q0.z * q1.y;
	d->y = q0.w * q1.y - q0.x * q1.z + q0.y * q1.w + q0.z * q1.x;
	d->z = q0.w * q1.z + q0.x * q1.y - q0.y * q1.x + q0.z * q1.w;
	d->w = q0.w * q1.w - q0.x * q1.x - q0.y * q1.y - q0.z * q1.z;
}

void quat_div( quat* d, quat q0, quat q1 )
{
	quat iq1;
	quat_inverse( &iq1, q1 );
	quat_mul( d, q0, iq1 );
}

void quat_mulf( quat* d, quat q0, f32 s )
{
	vec4_mul( &d->xyzw, q0.xyzw, s );
}

void quat_divf( quat* d, quat q0, f32 s )
{
	vec4_div( &d->xyzw, q0.xyzw, s );
}

void quat_addeq( quat* d, quat q )
{
	vec4_addeq( &d->xyzw, q.xyzw );
}

void quat_subeq( quat* d, quat q )
{
	vec4_subeq( &d->xyzw, q.xyzw );
}

void quat_muleq( quat* d, quat q )
{
	quat_mul( d, *d, q );
}

void quat_diveq( quat* d, quat q )
{
	quat_div( d, *d, q );
}

void quat_muleqf( quat* d, f32 s )
{
	vec4_muleq( &d->xyzw, s );
}

void quat_diveqf( quat* d, f32 s )
{
	vec4_diveq( &d->xyzw, s );
}

f32 quat_dot( quat q0, quat q1 )
{
	f32 r = vec3_dot( q0.xyz, q1.xyz ) + q0.w * q1.w;
	return r;
}

f32 quat_mag( quat q )
{
	f32 r = sqrt( quat_dot( q, q ) );
	return r;
}

void quat_norm( quat* d, quat q )
{
	quat_divf( d, q, quat_mag( q ) );
}

void quat_conj( quat* d, quat q )
{
	d->xyz = vec3f( -q.x, -q.y, -q.z );
	d->w   = q.w;
}

void quat_inverse( quat* d, quat q )
{
	quat_conj( d, q );
	quat_diveqf( d, quat_dot( q, q ) );
}

void quat_axis( vec3* axis, quat q )
{
	quat n;
	quat_norm( &n, q );
	vec3_div( axis, n.xyz, sin( arccos( q.w ) ) );
}

f32 quat_angle( quat q )
{
	f32 mag   = quat_mag( q );
	f32 c     = q.w * ( 1.0f / mag );
	f32 angle = 2.0f * arccos( c );
	return angle;
}

f32 quat_roll( quat q )
{
	return arctan2( 2.0f * q.x * q.y + q.z * q.w, q.x * q.x + q.w * q.w - q.y * q.y - q.z * q.z );
}

f32 quat_pitch( quat q )
{
	return arctan2( 2.0f * q.y * q.z + q.w * q.x, q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z );
}

f32 quat_yaw( quat q )
{
	return arcsin( -2.0f * ( q.x * q.z - q.w * q.y ) );
}

void quat_rotate_vec3( vec3* d, quat q, vec3 v )
{
	/* zpl_vec3 t = 2.0f * cross(q.xyz, v);
	 * *d = q.w*t + v + cross(q.xyz, t);
	 */
	vec3 t, p;
	vec3_cross( &t, q.xyz, v );
	vec3_muleq( &t, 2.0f );

	vec3_cross( &p, q.xyz, t );

	vec3_mul( d, t, q.w );
	vec3_addeq( d, v );
	vec3_addeq( d, p );
}

void mat4_from_quat( mat4* out, quat q )
{
	float4* m;
	quat    a;
	f32     xx, yy, zz, xy, xz, yz, wx, wy, wz;

	quat_norm( &a, q );
	xx = a.x * a.x;
	yy = a.y * a.y;
	zz = a.z * a.z;
	xy = a.x * a.y;
	xz = a.x * a.z;
	yz = a.y * a.z;
	wx = a.w * a.x;
	wy = a.w * a.y;
	wz = a.w * a.z;

	mat4_identity( out );
	m = float44_m( out );

	m[ 0 ][ 0 ] = 1.0f - 2.0f * ( yy + zz );
	m[ 0 ][ 1 ] = 2.0f * ( xy + wz );
	m[ 0 ][ 2 ] = 2.0f * ( xz - wy );

	m[ 1 ][ 0 ] = 2.0f * ( xy - wz );
	m[ 1 ][ 1 ] = 1.0f - 2.0f * ( xx + zz );
	m[ 1 ][ 2 ] = 2.0f * ( yz + wx );

	m[ 2 ][ 0 ] = 2.0f * ( xz + wy );
	m[ 2 ][ 1 ] = 2.0f * ( yz - wx );
	m[ 2 ][ 2 ] = 1.0f - 2.0f * ( xx + yy );
}

void quat_from_mat4( quat* out, mat4* mat )
{
	float4* m;
	f32     four_x_squared_minus_1, four_y_squared_minus_1, four_z_squared_minus_1, four_w_squared_minus_1, four_biggest_squared_minus_1;
	int     biggest_index = 0;
	f32     biggest_value, mult;

	m = float44_m( mat );

	four_x_squared_minus_1 = m[ 0 ][ 0 ] - m[ 1 ][ 1 ] - m[ 2 ][ 2 ];
	four_y_squared_minus_1 = m[ 1 ][ 1 ] - m[ 0 ][ 0 ] - m[ 2 ][ 2 ];
	four_z_squared_minus_1 = m[ 2 ][ 2 ] - m[ 0 ][ 0 ] - m[ 1 ][ 1 ];
	four_w_squared_minus_1 = m[ 0 ][ 0 ] + m[ 1 ][ 1 ] + m[ 2 ][ 2 ];

	four_biggest_squared_minus_1 = four_w_squared_minus_1;
	if ( four_x_squared_minus_1 > four_biggest_squared_minus_1 )
	{
		four_biggest_squared_minus_1 = four_x_squared_minus_1;
		biggest_index                = 1;
	}
	if ( four_y_squared_minus_1 > four_biggest_squared_minus_1 )
	{
		four_biggest_squared_minus_1 = four_y_squared_minus_1;
		biggest_index                = 2;
	}
	if ( four_z_squared_minus_1 > four_biggest_squared_minus_1 )
	{
		four_biggest_squared_minus_1 = four_z_squared_minus_1;
		biggest_index                = 3;
	}

	biggest_value = sqrt( four_biggest_squared_minus_1 + 1.0f ) * 0.5f;
	mult          = 0.25f / biggest_value;

	switch ( biggest_index )
	{
		case 0 :
			out->w = biggest_value;
			out->x = ( m[ 1 ][ 2 ] - m[ 2 ][ 1 ] ) * mult;
			out->y = ( m[ 2 ][ 0 ] - m[ 0 ][ 2 ] ) * mult;
			out->z = ( m[ 0 ][ 1 ] - m[ 1 ][ 0 ] ) * mult;
			break;
		case 1 :
			out->w = ( m[ 1 ][ 2 ] - m[ 2 ][ 1 ] ) * mult;
			out->x = biggest_value;
			out->y = ( m[ 0 ][ 1 ] + m[ 1 ][ 0 ] ) * mult;
			out->z = ( m[ 2 ][ 0 ] + m[ 0 ][ 2 ] ) * mult;
			break;
		case 2 :
			out->w = ( m[ 2 ][ 0 ] - m[ 0 ][ 2 ] ) * mult;
			out->x = ( m[ 0 ][ 1 ] + m[ 1 ][ 0 ] ) * mult;
			out->y = biggest_value;
			out->z = ( m[ 1 ][ 2 ] + m[ 2 ][ 1 ] ) * mult;
			break;
		case 3 :
			out->w = ( m[ 0 ][ 1 ] - m[ 1 ][ 0 ] ) * mult;
			out->x = ( m[ 2 ][ 0 ] + m[ 0 ][ 2 ] ) * mult;
			out->y = ( m[ 1 ][ 2 ] + m[ 2 ][ 1 ] ) * mult;
			out->z = biggest_value;
			break;
	}
}

f32 plane_distance( plane* p, vec3 v )
{
	return ( p->a * v.x + p->b * v.y + p->c * v.z + p->d );
}

void frustum_create( frustum* out, mat4* camera, mat4* proj )
{
	mat4 pv;

	mat4_mul( &pv, camera, proj );

	plane* fp = 0;
	f32    rmag;

	fp    = &out->x1;
	fp->a = pv.x.w + pv.x.x;
	fp->b = pv.y.w + pv.x.y;
	fp->c = pv.z.w + pv.x.z;
	fp->d = pv.w.w + pv.x.w;

	rmag = rsqrt( square( fp->a ) + square( fp->b ) + square( fp->c ) );

	fp->a *= rmag;
	fp->b *= rmag;
	fp->c *= rmag;
	fp->d *= rmag;

	fp = &out->x2;

	fp->a = pv.x.w - pv.x.x;
	fp->b = pv.y.w - pv.x.y;
	fp->c = pv.z.w - pv.x.z;
	fp->d = pv.w.w - pv.x.w;

	rmag = rsqrt( square( fp->a ) + square( fp->b ) + square( fp->c ) );

	fp->a *= rmag;
	fp->b *= rmag;
	fp->c *= rmag;
	fp->d *= rmag;

	fp = &out->y1;

	fp->a = pv.x.w - pv.y.x;
	fp->b = pv.y.w - pv.y.y;
	fp->c = pv.z.w - pv.y.w;
	fp->d = pv.w.w - pv.y.z;

	rmag = rsqrt( square( fp->a ) + square( fp->b ) + square( fp->c ) );

	fp->a *= rmag;
	fp->b *= rmag;
	fp->c *= rmag;
	fp->d *= rmag;

	fp = &out->y2;

	fp->a = pv.x.w + pv.y.x;
	fp->b = pv.y.w + pv.y.y;
	fp->c = pv.z.w + pv.y.z;
	fp->d = pv.w.w + pv.y.w;

	rmag = rsqrt( square( fp->a ) + square( fp->b ) + square( fp->c ) );

	fp->a *= rmag;
	fp->b *= rmag;
	fp->c *= rmag;
	fp->d *= rmag;
	;

	fp = &out->z1;

	fp->a = pv.x.w + pv.z.x;
	fp->b = pv.y.w + pv.z.y;
	fp->c = pv.z.w + pv.z.z;
	fp->d = pv.w.w + pv.z.w;

	rmag = rsqrt( square( fp->a ) + square( fp->b ) + square( fp->c ) );

	fp->a *= rmag;
	fp->b *= rmag;
	fp->c *= rmag;
	fp->d *= rmag;

	fp = &out->z2;

	fp->a = pv.x.w - pv.z.x;
	fp->b = pv.y.w - pv.z.y;
	fp->c = pv.z.w - pv.z.z;
	fp->d = pv.w.w - pv.z.w;

	rmag = rsqrt( square( fp->a ) + square( fp->b ) + square( fp->c ) );

	fp->a *= rmag;
	fp->b *= rmag;
	fp->c *= rmag;
	fp->d *= rmag;
}

b8 frustum_sphere_inside( frustum* frustum, vec3 center, f32 radius )
{
	if ( plane_distance( &frustum->x1, center ) <= -radius )
		return 0;
	if ( plane_distance( &frustum->x2, center ) <= -radius )
		return 0;
	if ( plane_distance( &frustum->y1, center ) <= -radius )
		return 0;
	if ( plane_distance( &frustum->y2, center ) <= -radius )
		return 0;
	if ( plane_distance( &frustum->z1, center ) <= -radius )
		return 0;
	if ( plane_distance( &frustum->z2, center ) <= -radius )
		return 0;

	return 1;
}

b8 frustum_point_inside( frustum* frustum, vec3 point )
{
	return frustum_sphere_inside( frustum, point, 0.0f );
}

b8 frustum_box_inside( frustum* frustum, aabb3 aabb )
{
	vec3 box, center;
	vec3 v, b;
	vec3_sub( &box, aabb.max, aabb.min );
	vec3_diveq( &box, 2.0f );
	vec3_add( &center, aabb.min, box );

	b = vec3f( -box.x, -box.y, -box.z );
	vec3_add( &v, b, center );

	if ( frustum_point_inside( frustum, v ) )
		return 1;

	b = vec3f( +box.x, -box.y, -box.z );
	vec3_add( &v, b, center );

	if ( frustum_point_inside( frustum, v ) )
		return 1;

	b = vec3f( -box.x, +box.y, -box.z );
	vec3_add( &v, b, center );

	if ( frustum_point_inside( frustum, v ) )
		return 1;

	b = vec3f( +box.x, +box.y, -box.z );
	vec3_add( &v, b, center );

	if ( frustum_point_inside( frustum, v ) )
		return 1;

	b = vec3f( +box.x, +box.y, +box.z );
	vec3_add( &v, b, center );

	if ( frustum_point_inside( frustum, v ) )
		return 1;

	b = vec3f( -box.x, +box.y, +box.z );
	vec3_add( &v, b, center );

	if ( frustum_point_inside( frustum, v ) )
		return 1;

	b = vec3f( -box.x, -box.y, +box.z );
	vec3_add( &v, b, center );

	if ( frustum_point_inside( frustum, v ) )
		return 1;

	b = vec3f( +box.x, -box.y, +box.z );
	vec3_add( &v, b, center );

	if ( frustum_point_inside( frustum, v ) )
		return 1;

	return 0;
}

f32 lerp( f32 a, f32 b, f32 t )
{
	return a * ( 1.0f - t ) + b * t;
}

f32 unlerp( f32 t, f32 a, f32 b )
{
	return ( t - a ) / ( b - a );
}

f32 smooth_step( f32 a, f32 b, f32 t )
{
	f32 x = ( t - a ) / ( b - a );
	return x * x * ( 3.0f - 2.0f * x );
}

f32 smoother_step( f32 a, f32 b, f32 t )
{
	f32 x = ( t - a ) / ( b - a );
	return x * x * x * ( x * ( 6.0f * x - 15.0f ) + 10.0f );
}

#define ZPL_VEC_LERPN( N, d, a, b, t ) \
	vec##N db;                         \
	vec##N##_sub( &db, b, a );         \
	vec##N##_muleq( &db, t );          \
	vec##N##_add( d, a, db )

void vec2_lerp( vec2* d, vec2 a, vec2 b, f32 t )
{
	ZPL_VEC_LERPN( 2, d, a, b, t );
}

void vec3_lerp( vec3* d, vec3 a, vec3 b, f32 t )
{
	ZPL_VEC_LERPN( 3, d, a, b, t );
}

void vec4_lerp( vec4* d, vec4 a, vec4 b, f32 t )
{
	ZPL_VEC_LERPN( 4, d, a, b, t );
}

#undef ZPL_VEC_LERPN

void vec2_cslerp( vec2* d, vec2 a, vec2 v0, vec2 b, vec2 v1, f32 t )
{
	f32 t2  = t * t;
	f32 ti  = ( t - 1 );
	f32 ti2 = ti * ti;

	f32 h00 = ( 1 + 2 * t ) * ti2;
	f32 h10 = t * ti2;
	f32 h01 = t2 * ( 3 - 2 * t );
	f32 h11 = t2 * ti;

	d->x = h00 * a.x + h10 * v0.x + h01 * b.x + h11 * v1.x;
	d->y = h00 * a.y + h10 * v0.y + h01 * b.y + h11 * v1.y;
}

void vec3_cslerp( vec3* d, vec3 a, vec3 v0, vec3 b, vec3 v1, f32 t )
{
	f32 t2  = t * t;
	f32 ti  = ( t - 1 );
	f32 ti2 = ti * ti;

	f32 h00 = ( 1 + 2 * t ) * ti2;
	f32 h10 = t * ti2;
	f32 h01 = t2 * ( 3 - 2 * t );
	f32 h11 = t2 * ti;

	d->x = h00 * a.x + h10 * v0.x + h01 * b.x + h11 * v1.x;
	d->y = h00 * a.y + h10 * v0.y + h01 * b.y + h11 * v1.y;
	d->z = h00 * a.z + h10 * v0.z + h01 * b.z + h11 * v1.z;
}

void vec2_dcslerp( vec2* d, vec2 a, vec2 v0, vec2 b, vec2 v1, f32 t )
{
	f32 t2 = t * t;

	f32 dh00 = 6 * t2 - 6 * t;
	f32 dh10 = 3 * t2 - 4 * t + 1;
	f32 dh01 = -6 * t2 + 6 * t;
	f32 dh11 = 3 * t2 - 2 * t;

	d->x = dh00 * a.x + dh10 * v0.x + dh01 * b.x + dh11 * v1.x;
	d->y = dh00 * a.y + dh10 * v0.y + dh01 * b.y + dh11 * v1.y;
}

void vec3_dcslerp( vec3* d, vec3 a, vec3 v0, vec3 b, vec3 v1, f32 t )
{
	f32 t2 = t * t;

	f32 dh00 = 6 * t2 - 6 * t;
	f32 dh10 = 3 * t2 - 4 * t + 1;
	f32 dh01 = -6 * t2 + 6 * t;
	f32 dh11 = 3 * t2 - 2 * t;

	d->x = dh00 * a.x + dh10 * v0.x + dh01 * b.x + dh11 * v1.x;
	d->y = dh00 * a.y + dh10 * v0.y + dh01 * b.y + dh11 * v1.y;
	d->z = dh00 * a.z + dh10 * v0.z + dh01 * b.z + dh11 * v1.z;
}

void quat_lerp( quat* d, quat a, quat b, f32 t )
{
	vec4_lerp( &d->xyzw, a.xyzw, b.xyzw, t );
}

void quat_nlerp( quat* d, quat a, quat b, f32 t )
{
	quat_lerp( d, a, b, t );
	quat_norm( d, *d );
}

void quat_slerp( quat* d, quat a, quat b, f32 t )
{
	quat x, y, z;
	f32  cos_theta, angle;
	f32  s1, s0, is;

	z         = b;
	cos_theta = quat_dot( a, b );

	if ( cos_theta < 0.0f )
	{
		z         = quatf( -b.x, -b.y, -b.z, -b.w );
		cos_theta = -cos_theta;
	}

	if ( cos_theta > 1.0f )
	{
		/* NOTE: Use lerp not nlerp as it's not a real angle or they are not normalized */
		quat_lerp( d, a, b, t );
	}

	angle = arccos( cos_theta );

	s1 = sin( ( 1.0f - t ) * angle );
	s0 = sin( t * angle );
	is = 1.0f / sin( angle );
	quat_mulf( &x, a, s1 );
	quat_mulf( &y, z, s0 );
	quat_add( d, x, y );
	quat_muleqf( d, is );
}

void quat_slerp_approx( quat* d, quat a, quat b, f32 t )
{
	/* NOTE: Derived by taylor expanding the geometric interpolation equation
	 *             Even works okay for nearly anti-parallel versors!!!
	 */
	/* NOTE: Extra interations cannot be used as they require angle^4 which is not worth it to approximate */
	f32 tp = t + ( 1.0f - quat_dot( a, b ) ) / 3.0f * t * ( -2.0f * t * t + 3.0f * t - 1.0f );
	quat_nlerp( d, a, b, tp );
}

void quat_nquad( quat* d, quat p, quat a, quat b, quat q, f32 t )
{
	quat x, y;
	quat_nlerp( &x, p, q, t );
	quat_nlerp( &y, a, b, t );
	quat_nlerp( d, x, y, 2.0f * t * ( 1.0f - t ) );
}

void quat_squad( quat* d, quat p, quat a, quat b, quat q, f32 t )
{
	quat x, y;
	quat_slerp( &x, p, q, t );
	quat_slerp( &y, a, b, t );
	quat_slerp( d, x, y, 2.0f * t * ( 1.0f - t ) );
}

void quat_squad_approx( quat* d, quat p, quat a, quat b, quat q, f32 t )
{
	quat x, y;
	quat_slerp_approx( &x, p, q, t );
	quat_slerp_approx( &y, a, b, t );
	quat_slerp_approx( d, x, y, 2.0f * t * ( 1.0f - t ) );
}

rect2 rect2f( vec2 pos, vec2 dim )
{
	rect2 r;
	r.pos = pos;
	r.dim = dim;
	return r;
}

rect3 rect3f( vec3 pos, vec3 dim )
{
	rect3 r;
	r.pos = pos;
	r.dim = dim;
	return r;
}

aabb2 aabb2f( f32 minx, f32 miny, f32 maxx, f32 maxy )
{
	aabb2 r;
	r.min = vec2f( minx, miny );
	r.max = vec2f( maxx, maxy );
	return r;
}

aabb3 aabb3f( f32 minx, f32 miny, f32 minz, f32 maxx, f32 maxy, f32 maxz )
{
	aabb3 r;
	r.min = vec3f( minx, miny, minz );
	r.max = vec3f( maxx, maxy, maxz );
	return r;
}

aabb2 aabb2_rect2( rect2 a )
{
	aabb2 r;
	r.min = a.pos;
	vec2_add( &r.max, a.pos, a.dim );
	return r;
}

aabb3 aabb3_rect3( rect3 a )
{
	aabb3 r;
	r.min = a.pos;
	vec3_add( &r.max, a.pos, a.dim );
	return r;
}

rect2 rect2_aabb2( aabb2 a )
{
	rect2 r;
	r.pos = a.min;
	vec2_sub( &r.dim, a.max, a.min );
	return r;
}

rect3 rect3_aabb3( aabb3 a )
{
	rect3 r;
	r.pos = a.min;
	vec3_sub( &r.dim, a.max, a.min );
	return r;
}

int rect2_contains( rect2 a, f32 x, f32 y )
{
	f32 min_x  = min( a.pos.x, a.pos.x + a.dim.x );
	f32 max_x  = max( a.pos.x, a.pos.x + a.dim.x );
	f32 min_y  = min( a.pos.y, a.pos.y + a.dim.y );
	f32 max_y  = max( a.pos.y, a.pos.y + a.dim.y );
	int result = ( x >= min_x ) & ( x < max_x ) & ( y >= min_y ) & ( y < max_y );
	return result;
}

int rect2_contains_vec2( rect2 a, vec2 p )
{
	return rect2_contains( a, p.x, p.y );
}

int rect2_intersects( rect2 a, rect2 b )
{
	rect2 r = { 0 };
	return rect2_intersection_result( a, b, &r );
}

int rect2_intersection_result( rect2 a, rect2 b, rect2* intersection )
{
	f32 a_min_x = min( a.pos.x, a.pos.x + a.dim.x );
	f32 a_max_x = max( a.pos.x, a.pos.x + a.dim.x );
	f32 a_min_y = min( a.pos.y, a.pos.y + a.dim.y );
	f32 a_max_y = max( a.pos.y, a.pos.y + a.dim.y );

	f32 b_min_x = min( b.pos.x, b.pos.x + b.dim.x );
	f32 b_max_x = max( b.pos.x, b.pos.x + b.dim.x );
	f32 b_min_y = min( b.pos.y, b.pos.y + b.dim.y );
	f32 b_max_y = max( b.pos.y, b.pos.y + b.dim.y );

	f32 x0 = max( a_min_x, b_min_x );
	f32 y0 = max( a_min_y, b_min_y );
	f32 x1 = min( a_max_x, b_max_x );
	f32 y1 = min( a_max_y, b_max_y );

	if ( ( x0 < x1 ) && ( y0 < y1 ) )
	{
		rect2 r       = rect2f( vec2f( x0, y0 ), vec2f( x1 - x0, y1 - y0 ) );
		*intersection = r;
		return 1;
	}
	else
	{
		rect2 r       = { 0 };
		*intersection = r;
		return 0;
	}
}

int aabb2_contains( aabb2 a, f32 x, f32 y )
{
	return ( is_between_limit( x, a.min.x, a.max.x ) && is_between_limit( y, a.min.y, a.max.y ) );
}

int aabb3_contains( aabb3 a, f32 x, f32 y, f32 z )
{
	return ( is_between_limit( x, a.min.x, a.max.x ) && is_between_limit( y, a.min.y, a.max.y ) && is_between_limit( z, a.min.z, a.max.z ) );
}

aabb2 aabb2_cut_left( aabb2* a, f32 b )
{
	f32 minx = a->min.x;
	a->min.x = min( a->max.x, a->min.x + b );
	return aabb2f( minx, a->min.y, a->min.x, a->max.y );
}

aabb2 aabb2_cut_right( aabb2* a, f32 b )
{
	f32 maxx = a->max.x;
	a->max.x = max( a->min.x, a->max.x - b );
	return aabb2f( a->max.x, a->min.y, maxx, a->max.y );
}

aabb2 aabb2_cut_top( aabb2* a, f32 b )
{
	f32 miny = a->min.y;
	a->min.y = min( a->max.y, a->min.y + b );
	return aabb2f( a->min.x, miny, a->max.x, a->min.y );
}

aabb2 aabb2_cut_bottom( aabb2* a, f32 b )
{
	f32 maxy = a->max.y;
	a->max.y = max( a->min.y, a->max.y - b );
	return aabb2f( a->min.x, a->max.y, a->max.x, maxy );
}

aabb2 aabb2_get_left( const aabb2* a, f32 b )
{
	f32 minx  = a->min.x;
	f32 aminx = min( a->max.x, a->min.x + b );
	return aabb2f( minx, a->min.y, aminx, a->max.y );
}

aabb2 aabb2_get_right( const aabb2* a, f32 b )
{
	f32 maxx  = a->max.x;
	f32 amaxx = max( a->min.x, a->max.x - b );
	return aabb2f( amaxx, a->min.y, maxx, a->max.y );
}

aabb2 aabb2_get_top( const aabb2* a, f32 b )
{
	f32 miny  = a->min.y;
	f32 aminy = min( a->max.y, a->min.y + b );
	return aabb2f( a->min.x, miny, a->max.x, aminy );
}

aabb2 aabb2_get_bottom( const aabb2* a, f32 b )
{
	f32 maxy  = a->max.y;
	f32 amaxy = max( a->min.y, a->max.y - b );
	return aabb2f( a->min.x, amaxy, a->max.x, maxy );
}

aabb2 aabb2_add_left( const aabb2* a, f32 b )
{
	return aabb2f( a->min.x - b, a->min.y, a->min.x, a->max.y );
}

aabb2 aabb2_add_right( const aabb2* a, f32 b )
{
	return aabb2f( a->max.x, a->min.y, a->max.x + b, a->max.y );
}

aabb2 aabb2_add_top( const aabb2* a, f32 b )
{
	return aabb2f( a->min.x, a->min.y - b, a->max.x, a->min.y );
}

aabb2 aabb2_add_bottom( const aabb2* a, f32 b )
{
	return aabb2f( a->min.x, a->max.y, a->max.x, a->max.y + b );
}

aabb2 aabb2_contract( const aabb2* a, f32 b )
{
	aabb2 r  = *a;
	vec2  vb = vec2f( b, b );
	vec2_addeq( &r.min, vb );
	vec2_subeq( &r.max, vb );

	if ( vec2_mag2( r.min ) > vec2_mag2( r.max ) )
	{
		return aabb2f( 0, 0, 0, 0 );
	}
	return r;
}

aabb2 aabb2_expand( const aabb2* a, f32 b )
{
	return aabb2_contract( a, -b );
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

#		endif

#		if defined( ZPL_MODULE_THREADING )
// file: source/threading/affinity.c


#if defined( ZPL_SYSTEM_MACOS )
#	include <sys/sysctl.h>
#endif

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

#if defined( ZPL_SYSTEM_WINDOWS ) || defined( ZPL_SYSTEM_CYGWIN )

void affinity_init( affinity* a )
{
	SYSTEM_LOGICAL_PROCESSOR_INFORMATION* start_processor_info = NULL;
	DWORD                                 length               = 0;
	b32                                   result               = GetLogicalProcessorInformation( NULL, &length );

	zero_item( a );

	if ( ! result && GetLastError() == 122l /*ERROR_INSUFFICIENT_BUFFER*/ && length > 0 )
	{
		start_processor_info = zpl_cast( SYSTEM_LOGICAL_PROCESSOR_INFORMATION* ) alloc( heap_allocator(), length );
		result               = GetLogicalProcessorInformation( start_processor_info, &length );
		if ( result )
		{
			SYSTEM_LOGICAL_PROCESSOR_INFORMATION *end_processor_info, *processor_info;

			a->is_accurate     = true;
			a->core_count      = 0;
			a->thread_count    = 0;
			end_processor_info = zpl_cast( SYSTEM_LOGICAL_PROCESSOR_INFORMATION* ) pointer_add( start_processor_info, length );

			for ( processor_info = start_processor_info; processor_info < end_processor_info; processor_info++ )
			{
				if ( processor_info->Relationship == RelationProcessorCore )
				{
					sw thread = count_set_bits( processor_info->ProcessorMask );
					if ( thread == 0 )
					{
						a->is_accurate = false;
					}
					else if ( a->thread_count + thread > ZPL_WIN32_MAX_THREADS )
					{
						a->is_accurate = false;
					}
					else
					{
						ZPL_ASSERT( a->core_count <= a->thread_count && a->thread_count < ZPL_WIN32_MAX_THREADS );
						a->core_masks[ a->core_count++ ]  = processor_info->ProcessorMask;
						a->thread_count                  += thread;
					}
				}
			}
		}

		free( heap_allocator(), start_processor_info );
	}

	ZPL_ASSERT( a->core_count <= a->thread_count );
	if ( a->thread_count == 0 )
	{
		a->is_accurate     = false;
		a->core_count      = 1;
		a->thread_count    = 1;
		a->core_masks[ 0 ] = 1;
	}
}

void affinity_destroy( affinity* a )
{
	unused( a );
}

b32 affinity_set( affinity* a, sw core, sw thread )
{
	uw available_mask, check_mask = 1;
	ZPL_ASSERT( thread < affinity_thread_count_for_core( a, core ) );

	available_mask = a->core_masks[ core ];
	for ( ;; )
	{
		if ( ( available_mask & check_mask ) != 0 )
		{
			if ( thread-- == 0 )
			{
				uw result = SetThreadAffinityMask( GetCurrentThread(), check_mask );
				return result != 0;
			}
		}
		check_mask <<= 1;    // NOTE: Onto the next bit
	}
}

sw affinity_thread_count_for_core( affinity* a, sw core )
{
	ZPL_ASSERT( core >= 0 && core < a->core_count );
	return count_set_bits( a->core_masks[ core ] );
}

#elif defined( ZPL_SYSTEM_MACOS )
void affinity_init( affinity* a )
{
	uw count, count_size = size_of( count );

	a->is_accurate      = false;
	a->thread_count     = 1;
	a->core_count       = 1;
	a->threads_per_core = 1;

	if ( sysctlbyname( "hw.logicalcpu", &count, &count_size, NULL, 0 ) == 0 )
	{
		if ( count > 0 )
		{
			a->thread_count = count;
			// Get # of physical cores
			if ( sysctlbyname( "hw.physicalcpu", &count, &count_size, NULL, 0 ) == 0 )
			{
				if ( count > 0 )
				{
					a->core_count       = count;
					a->threads_per_core = a->thread_count / count;
					if ( a->threads_per_core < 1 )
						a->threads_per_core = 1;
					else
						a->is_accurate = true;
				}
			}
		}
	}
}

void affinity_destroy( affinity* a )
{
	unused( a );
}

b32 affinity_set( affinity* a, sw core, sw thread_index )
{
	sw                            index;
	thread_t                      thread;
	thread_affinity_policy_data_t info;
	kern_return_t                 result;

	ZPL_ASSERT( core < a->core_count );
	ZPL_ASSERT( thread_index < a->threads_per_core );

	index             = core * a->threads_per_core + thread_index;
	thread            = mach_thread_self();
	info.affinity_tag = zpl_cast( integer_t ) index;
	result            = thread_policy_set( thread, THREAD_AFFINITY_POLICY, zpl_cast( thread_policy_t ) & info, THREAD_AFFINITY_POLICY_COUNT );
	return result == KERN_SUCCESS;
}

sw affinity_thread_count_for_core( affinity* a, sw core )
{
	ZPL_ASSERT( core >= 0 && core < a->core_count );
	return a->threads_per_core;
}

#elif defined( ZPL_SYSTEM_LINUX ) || defined( ZPL_SYSTEM_FREEBSD ) || defined( ZPL_SYSTEM_OPENBSD )
void affinity_init( affinity* a )
{
	a->core_count       = sysconf( _SC_NPROCESSORS_ONLN );
	a->threads_per_core = 1;

	a->is_accurate  = a->core_count > 0;
	a->core_count   = a->is_accurate ? a->core_count : 1;
	a->thread_count = a->core_count;
}

void affinity_destroy( affinity* a )
{
	unused( a );
}

b32 affinity_set( affinity* a, sw core, sw thread_index )
{
	unused( a );
	unused( core );
	unused( thread_index );
	return true;
}

sw affinity_thread_count_for_core( affinity* a, sw core )
{
	ZPL_ASSERT( 0 <= core && core < a->core_count );
	return a->threads_per_core;
}

#elif defined( ZPL_SYSTEM_EMSCRIPTEN )
#	error No affinity implementation for Emscripten
#else
#	error TODO: Unknown system
#endif

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: source/threading/atomic.c


ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

////////////////////////////////////////////////////////////////
//
// Concurrency
//
//
// IMPORTANT TODO: Use compiler intrinsics for the atomics

#if defined( ZPL_COMPILER_MSVC ) && ! defined( ZPL_COMPILER_CLANG )
s32 atomic32_load( atomic32 const* a )
{
	return a->value;
}

void atomic32_store( atomic32* a, atomicarg( s32 ) value )
{
	a->value = value;
}

s32 atomic32_compare_exchange( atomic32* a, atomicarg( s32 ) expected, atomicarg( s32 ) desired )
{
	return _InterlockedCompareExchange( zpl_cast( long* ) a, desired, expected );
}

s32 atomic32_exchange( atomic32* a, atomicarg( s32 ) desired )
{
	return _InterlockedExchange( zpl_cast( long* ) a, desired );
}

s32 atomic32_fetch_add( atomic32* a, atomicarg( s32 ) operand )
{
	return _InterlockedExchangeAdd( zpl_cast( long* ) a, operand );
}

s32 atomic32_fetch_and( atomic32* a, atomicarg( s32 ) operand )
{
	return _InterlockedAnd( zpl_cast( long* ) a, operand );
}

s32 atomic32_fetch_or( atomic32* a, atomicarg( s32 ) operand )
{
	return _InterlockedOr( zpl_cast( long* ) a, operand );
}

s64 atomic64_load( atomic64 const* a )
{
#	if defined( ZPL_ARCH_64_BIT )
	return a->value;
#	elif ZPL_CPU_X86
	// NOTE: The most compatible way to get an atomic 64-bit load on x86 is with cmpxchg8b
	atomicarg( s64 ) result;
	__asm {
                mov esi, a;
                mov ebx, eax;
                mov ecx, edx;
                lock cmpxchg8b [esi];
                mov dword ptr result, eax;
                mov dword ptr result[4], edx;
	}
	return result;
#	else
#		error TODO: atomics for this CPU
#	endif
}

void atomic64_store( atomic64* a, atomicarg( s64 ) value )
{
#	if defined( ZPL_ARCH_64_BIT )
	a->value = value;
#	elif ZPL_CPU_X86
	// NOTE: The most compatible way to get an atomic 64-bit store on x86 is with cmpxchg8b
	__asm {
                mov esi, a;
                mov ebx, dword ptr value;
                mov ecx, dword ptr value[4];
                retry:
                cmpxchg8b [esi];
                jne retry;
	}
#	else
#		error TODO: atomics for this CPU
#	endif
}

s64 atomic64_compare_exchange( atomic64* a, atomicarg( s64 ) expected, atomicarg( s64 ) desired )
{
	return _InterlockedCompareExchange64( zpl_cast( atomicarg( s64 )* ) a, desired, expected );
}

s64 atomic64_exchange( atomic64* a, atomicarg( s64 ) desired )
{
#	if defined( ZPL_ARCH_64_BIT )
	return _InterlockedExchange64( zpl_cast( atomicarg( s64 )* ) a, desired );
#	elif ZPL_CPU_X86
	atomicarg( s64 ) expected = a->value;
	for ( ;; )
	{
		atomicarg( s64 ) original = _InterlockedCompareExchange64( zpl_cast( atomicarg( s64 )* ) a, desired, expected );
		if ( original == expected )
			return original;
		expected = original;
	}
#	else
#		error TODO: atomics for this CPU
#	endif
}

s64 atomic64_fetch_add( atomic64* a, atomicarg( s64 ) operand )
{
#	if defined( ZPL_ARCH_64_BIT )
	return _InterlockedExchangeAdd64( zpl_cast( atomicarg( s64 )* ) a, operand );
#	elif ZPL_CPU_X86
	atomicarg( s64 ) expected = a->value;
	for ( ;; )
	{
		atomicarg( s64 ) original = _InterlockedCompareExchange64( zpl_cast( atomicarg( s64 )* ) a, expected + operand, expected );
		if ( original == expected )
			return original;
		expected = original;
	}
#	else
#		error TODO: atomics for this CPU
#	endif
}

s64 atomic64_fetch_and( atomic64* a, atomicarg( s64 ) operand )
{
#	if defined( ZPL_ARCH_64_BIT )
	return _InterlockedAnd64( zpl_cast( atomicarg( s64 )* ) a, operand );
#	elif ZPL_CPU_X86
	atomicarg( s64 ) expected = a->value;
	for ( ;; )
	{
		atomicarg( s64 ) original = _InterlockedCompareExchange64( zpl_cast( atomicarg( s64 )* ) a, expected & operand, expected );
		if ( original == expected )
			return original;
		expected = original;
	}
#	else
#		error TODO: atomics for this CPU
#	endif
}

s64 atomic64_fetch_or( atomic64* a, atomicarg( s64 ) operand )
{
#	if defined( ZPL_ARCH_64_BIT )
	return _InterlockedOr64( zpl_cast( atomicarg( s64 )* ) a, operand );
#	elif ZPL_CPU_X86
	atomicarg( s64 ) expected = a->value;
	for ( ;; )
	{
		atomicarg( s64 ) original = _InterlockedCompareExchange64( zpl_cast( atomicarg( s64 )* ) a, expected | operand, expected );
		if ( original == expected )
			return original;
		expected = original;
	}
#	else
#		error TODO: atomics for this CPU
#	endif
}

#elif defined( ZPL_CPU_X86 )

s32 atomic32_load( atomic32 const* a )
{
	return a->value;
}

void atomic32_store( atomic32* a, atomicarg( s32 ) value )
{
	a->value = value;
}

s32 atomic32_compare_exchange( atomic32* a, atomicarg( s32 ) expected, atomicarg( s32 ) desired )
{
	atomicarg( s32 ) original;
	__asm__( "lock; cmpxchgl %2, %1" : "=a"( original ), "+m"( a->value ) : "q"( desired ), "0"( expected ) );
	return original;
}

s32 atomic32_exchange( atomic32* a, atomicarg( s32 ) desired )
{
	// NOTE: No lock prefix is necessary for xchgl
	atomicarg( s32 ) original;
	__asm__( "xchgl %0, %1" : "=r"( original ), "+m"( a->value ) : "0"( desired ) );
	return original;
}

s32 atomic32_fetch_add( atomic32* a, atomicarg( s32 ) operand )
{
	atomicarg( s32 ) original;
	__asm__( "lock; xaddl %0, %1" : "=r"( original ), "+m"( a->value ) : "0"( operand ) );
	return original;
}

s32 atomic32_fetch_and( atomic32* a, atomicarg( s32 ) operand )
{
	atomicarg( s32 ) original;
	atomicarg( s32 ) tmp;
	__asm__(
	    "1:     movl    %1, %0\n"
	    "       movl    %0, %2\n"
	    "       andl    %3, %2\n"
	    "       lock; cmpxchgl %2, %1\n"
	    "       jne     1b"
	    : "=&a"( original ), "+m"( a->value ), "=&r"( tmp )
	    : "r"( operand )
	);
	return original;
}

s32 atomic32_fetch_or( atomic32* a, atomicarg( s32 ) operand )
{
	atomicarg( s32 ) original;
	atomicarg( s32 ) temp;
	__asm__(
	    "1:     movl    %1, %0\n"
	    "       movl    %0, %2\n"
	    "       orl     %3, %2\n"
	    "       lock; cmpxchgl %2, %1\n"
	    "       jne     1b"
	    : "=&a"( original ), "+m"( a->value ), "=&r"( temp )
	    : "r"( operand )
	);
	return original;
}

s64 atomic64_load( atomic64 const* a )
{
#	if defined( ZPL_ARCH_64_BIT )
	return a->value;
#	else
	atomicarg( s64 ) original;
	__asm__(
	    "movl %%ebx, %%eax\n"
	    "movl %%ecx, %%edx\n"
	    "lock; cmpxchg8b %1"
	    : "=&A"( original )
	    : "m"( a->value )
	);
	return original;
#	endif
}

void atomic64_store( atomic64* a, atomicarg( s64 ) value )
{
#	if defined( ZPL_ARCH_64_BIT )
	a->value = value;
#	else
	atomicarg( s64 ) expected = a->value;
	__asm__(
	    "1:    cmpxchg8b %0\n"
	    "      jne 1b"
	    : "=m"( a->value )
	    : "b"( ( atomicarg( s32 ) )value ), "c"( ( atomicarg( s32 ) )( value >> 32 ) ), "A"( expected )
	);
#	endif
}

s64 atomic64_compare_exchange( atomic64* a, atomicarg( s64 ) expected, atomicarg( s64 ) desired )
{
#	if defined( ZPL_ARCH_64_BIT )
	atomicarg( s64 ) original;
	__asm__( "lock; cmpxchgq %2, %1" : "=a"( original ), "+m"( a->value ) : "q"( desired ), "0"( expected ) );
	return original;
#	else
	atomicarg( s64 ) original;
	__asm__( "lock; cmpxchg8b %1" : "=A"( original ), "+m"( a->value ) : "b"( ( atomicarg( s32 ) )desired ), "c"( ( atomicarg( s32 ) )( desired >> 32 ) ), "0"( expected ) );
	return original;
#	endif
}

s64 atomic64_exchange( atomic64* a, atomicarg( s64 ) desired )
{
#	if defined( ZPL_ARCH_64_BIT )
	atomicarg( s64 ) original;
	__asm__( "xchgq %0, %1" : "=r"( original ), "+m"( a->value ) : "0"( desired ) );
	return original;
#	else
	atomicarg( s64 ) original = a->value;
	for ( ;; )
	{
		atomicarg( s64 ) previous = atomic64_compare_exchange( a, original, desired );
		if ( original == previous )
			return original;
		original = previous;
	}
#	endif
}

s64 atomic64_fetch_add( atomic64* a, atomicarg( s64 ) operand )
{
#	if defined( ZPL_ARCH_64_BIT )
	atomicarg( s64 ) original;
	__asm__( "lock; xaddq %0, %1" : "=r"( original ), "+m"( a->value ) : "0"( operand ) );
	return original;
#	else
	for ( ;; )
	{
		atomicarg( s64 ) original = a->value;
		if ( atomic64_compare_exchange( a, original, original + operand ) == original )
			return original;
	}
#	endif
}

s64 atomic64_fetch_and( atomic64* a, atomicarg( s64 ) operand )
{
#	if defined( ZPL_ARCH_64_BIT )
	atomicarg( s64 ) original;
	atomicarg( s64 ) tmp;
	__asm__(
	    "1:     movq    %1, %0\n"
	    "       movq    %0, %2\n"
	    "       andq    %3, %2\n"
	    "       lock; cmpxchgq %2, %1\n"
	    "       jne     1b"
	    : "=&a"( original ), "+m"( a->value ), "=&r"( tmp )
	    : "r"( operand )
	);
	return original;
#	else
	for ( ;; )
	{
		atomicarg( s64 ) original = a->value;
		if ( atomic64_compare_exchange( a, original, original & operand ) == original )
			return original;
	}
#	endif
}

s64 atomic64_fetch_or( atomic64* a, atomicarg( s64 ) operand )
{
#	if defined( ZPL_ARCH_64_BIT )
	atomicarg( s64 ) original;
	atomicarg( s64 ) temp;
	__asm__(
	    "1:     movq    %1, %0\n"
	    "       movq    %0, %2\n"
	    "       orq     %3, %2\n"
	    "       lock; cmpxchgq %2, %1\n"
	    "       jne     1b"
	    : "=&a"( original ), "+m"( a->value ), "=&r"( temp )
	    : "r"( operand )
	);
	return original;
#	else
	for ( ;; )
	{
		atomicarg( s64 ) original = a->value;
		if ( atomic64_compare_exchange( a, original, original | operand ) == original )
			return original;
	}
#	endif
}

#elif ! defined( ZPL_COMPILER_MSVC )
s32 atomic32_load( atomic32 const* a )
{
	return __atomic_load_n( ( s32* )&a->value, __ATOMIC_SEQ_CST );
}

void atomic32_store( atomic32* a, atomicarg( s32 ) value )
{
	__atomic_store( ( s32* )&a->value, ( s32* )&value, __ATOMIC_SEQ_CST );
}

s32 atomic32_compare_exchange( atomic32* a, atomicarg( s32 ) expected, atomicarg( s32 ) desired )
{
	return __atomic_compare_exchange_n( ( s32* )&a->value, ( s32* )&expected, desired, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST );
}

s32 atomic32_exchange( atomic32* a, atomicarg( s32 ) desired )
{
	return __atomic_exchange_n( ( s32* )&a->value, desired, __ATOMIC_SEQ_CST );
}

s32 atomic32_fetch_add( atomic32* a, atomicarg( s32 ) operand )
{
	return __atomic_fetch_add( ( s32* )&a->value, operand, __ATOMIC_SEQ_CST );
}

s32 atomic32_fetch_and( atomic32* a, atomicarg( s32 ) operand )
{
	return __atomic_fetch_and( ( s32* )&a->value, operand, __ATOMIC_SEQ_CST );
}

s32 atomic32_fetch_or( atomic32* a, atomicarg( s32 ) operand )
{
	return __atomic_fetch_or( ( s32* )&a->value, operand, __ATOMIC_SEQ_CST );
}

s64 atomic64_load( atomic64 const* a )
{
	return __atomic_load_n( ( s64* )&a->value, __ATOMIC_SEQ_CST );
}

void atomic64_store( atomic64* a, atomicarg( s64 ) value )
{
	__atomic_store( ( s64* )&a->value, ( s64* )&value, __ATOMIC_SEQ_CST );
}

s64 atomic64_compare_exchange( atomic64* a, atomicarg( s64 ) expected, atomicarg( s64 ) desired )
{
	return __atomic_compare_exchange_n( ( s64* )&a->value, ( s64* )&expected, desired, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST );
}

s64 atomic64_exchange( atomic64* a, atomicarg( s64 ) desired )
{
	return __atomic_exchange_n( ( s64* )&a->value, desired, __ATOMIC_SEQ_CST );
}

s64 atomic64_fetch_add( atomic64* a, atomicarg( s64 ) operand )
{
	return __atomic_fetch_add( ( s64* )&a->value, operand, __ATOMIC_SEQ_CST );
}

s64 atomic64_fetch_and( atomic64* a, atomicarg( s64 ) operand )
{
	return __atomic_fetch_and( ( s64* )&a->value, operand, __ATOMIC_SEQ_CST );
}

s64 atomic64_fetch_or( atomic64* a, atomicarg( s64 ) operand )
{
	return __atomic_fetch_or( ( s64* )&a->value, operand, __ATOMIC_SEQ_CST );
}

#else
#	error TODO: Implement Atomics for this CPU
#endif



b32 atomic32_spin_lock( atomic32* a, sw time_out )
{
	atomicarg( s32 ) old_value = atomic32_compare_exchange( a, 1, 0 );
	s32 counter                = 0;
	while ( old_value != 0 && ( time_out < 0 || counter++ < time_out ) )
	{
		yield_thread();
		old_value = atomic32_compare_exchange( a, 1, 0 );
		mfence();
	}
	return old_value == 0;
}

void atomic32_spin_unlock( atomic32* a )
{
	atomic32_store( a, 0 );
	mfence();
}

b32 atomic64_spin_lock( atomic64* a, sw time_out )
{
	atomicarg( s64 ) old_value = atomic64_compare_exchange( a, 1, 0 );
	atomicarg( s64 ) counter   = 0;
	while ( old_value != 0 && ( time_out < 0 || counter++ < time_out ) )
	{
		yield_thread();
		old_value = atomic64_compare_exchange( a, 1, 0 );
		mfence();
	}
	return old_value == 0;
}

void atomic64_spin_unlock( atomic64* a )
{
	atomic64_store( a, 0 );
	mfence();
}

b32 atomic32_try_acquire_lock( atomic32* a )
{
	atomicarg( s32 ) old_value;
	yield_thread();
	old_value = atomic32_compare_exchange( a, 1, 0 );
	mfence();
	return old_value == 0;
}

b32 atomic64_try_acquire_lock( atomic64* a )
{
	atomicarg( s64 ) old_value;
	yield_thread();
	old_value = atomic64_compare_exchange( a, 1, 0 );
	mfence();
	return old_value == 0;
}



#if defined( ZPL_ARCH_32_BIT )

void* atomic_ptr_load( atomic_ptr const* a )
{
	return ( void* )zpl_cast( sptr ) atomic32_load( zpl_cast( atomic32 const* ) a );
}

void atomic_ptr_store( atomic_ptr* a, atomicarg( void* ) value )
{
	atomic32_store( zpl_cast( atomic32* ) a, zpl_cast( atomicarg( s32 ) ) zpl_cast( sptr ) value );
}

void* atomic_ptr_compare_exchange( atomic_ptr* a, atomicarg( void* ) expected, atomicarg( void* ) desired )
{
	return ( void* )zpl_cast( sptr )
	    atomic32_compare_exchange( zpl_cast( atomic32* ) a, zpl_cast( atomicarg( s32 ) ) zpl_cast( sptr ) expected, zpl_cast( atomicarg( s32 ) ) zpl_cast( sptr ) desired );
}

void* atomic_ptr_exchange( atomic_ptr* a, atomicarg( void* ) desired )
{
	return ( void* )zpl_cast( sptr ) atomic32_exchange( zpl_cast( atomic32* ) a, zpl_cast( atomicarg( s32 ) ) zpl_cast( sptr ) desired );
}

void* atomic_ptr_fetch_add( atomic_ptr* a, atomicarg( void* ) operand )
{
	return ( void* )zpl_cast( sptr ) atomic32_fetch_add( zpl_cast( atomic32* ) a, zpl_cast( atomicarg( s32 ) ) zpl_cast( sptr ) operand );
}

void* atomic_ptr_fetch_and( atomic_ptr* a, atomicarg( void* ) operand )
{
	return ( void* )zpl_cast( sptr ) atomic32_fetch_and( zpl_cast( atomic32* ) a, zpl_cast( atomicarg( s32 ) ) zpl_cast( sptr ) operand );
}

void* atomic_ptr_fetch_or( atomic_ptr* a, atomicarg( void* ) operand )
{
	return ( void* )zpl_cast( sptr ) atomic32_fetch_or( zpl_cast( atomic32* ) a, zpl_cast( atomicarg( s32 ) ) zpl_cast( sptr ) operand );
}

b32 atomic_ptr_spin_lock( atomic_ptr* a, sw time_out )
{
	return atomic32_spin_lock( zpl_cast( atomic32* ) a, time_out );
}

void atomic_ptr_spin_unlock( atomic_ptr* a )
{
	atomic32_spin_unlock( zpl_cast( atomic32* ) a );
}

b32 atomic_ptr_try_acquire_lock( atomic_ptr* a )
{
	return atomic32_try_acquire_lock( zpl_cast( atomic32* ) a );
}

#elif defined( ZPL_ARCH_64_BIT )

void* atomic_ptr_load( atomic_ptr const* a )
{
	return ( void* )zpl_cast( sptr ) atomic64_load( zpl_cast( atomic64 const* ) a );
}

void atomic_ptr_store( atomic_ptr* a, atomicarg( void* ) value )
{
	atomic64_store( zpl_cast( atomic64* ) a, zpl_cast( s64 ) zpl_cast( sptr ) value );
}

void* atomic_ptr_compare_exchange( atomic_ptr* a, atomicarg( void* ) expected, atomicarg( void* ) desired )
{
	return ( void* )zpl_cast( sptr ) atomic64_compare_exchange( zpl_cast( atomic64* ) a, zpl_cast( s64 ) zpl_cast( sptr ) expected, zpl_cast( s64 ) zpl_cast( sptr ) desired );
}

void* atomic_ptr_exchange( atomic_ptr* a, atomicarg( void* ) desired )
{
	return ( void* )zpl_cast( sptr ) atomic64_exchange( zpl_cast( atomic64* ) a, zpl_cast( s64 ) zpl_cast( sptr ) desired );
}

void* atomic_ptr_fetch_add( atomic_ptr* a, atomicarg( void* ) operand )
{
	return ( void* )zpl_cast( sptr ) atomic64_fetch_add( zpl_cast( atomic64* ) a, zpl_cast( s64 ) zpl_cast( sptr ) operand );
}

void* atomic_ptr_fetch_and( atomic_ptr* a, atomicarg( void* ) operand )
{
	return ( void* )zpl_cast( sptr ) atomic64_fetch_and( zpl_cast( atomic64* ) a, zpl_cast( s64 ) zpl_cast( sptr ) operand );
}

void* atomic_ptr_fetch_or( atomic_ptr* a, atomicarg( void* ) operand )
{
	return ( void* )zpl_cast( sptr ) atomic64_fetch_or( zpl_cast( atomic64* ) a, zpl_cast( s64 ) zpl_cast( sptr ) operand );
}

b32 atomic_ptr_spin_lock( atomic_ptr* a, sw time_out )
{
	return atomic64_spin_lock( zpl_cast( atomic64* ) a, time_out );
}

void atomic_ptr_spin_unlock( atomic_ptr* a )
{
	atomic64_spin_unlock( zpl_cast( atomic64* ) a );
}

b32 atomic_ptr_try_acquire_lock( atomic_ptr* a )
{
	return atomic64_try_acquire_lock( zpl_cast( atomic64* ) a );
}

#endif

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: source/threading/fence.c


ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

#if defined( _MSC_VER )
/* Microsoft C/C++-compatible compiler */
#	include <intrin.h>
#elif defined( __GNUC__ ) && ( defined( __x86_64__ ) || defined( __i386__ ) )
/* GCC-compatible compiler, targeting x86/x86-64 */
#	include <x86intrin.h>
#elif defined( __GNUC__ ) && defined( __ARM_NEON__ )
/* GCC-compatible compiler, targeting ARM with NEON */
#	include <arm_neon.h>
#elif defined( __GNUC__ ) && defined( __IWMMXT__ )
/* GCC-compatible compiler, targeting ARM with WMMX */
#	include <mmintrin.h>
#elif ( defined( __GNUC__ ) || defined( __xlC__ ) ) && ( defined( __VEC__ ) || defined( __ALTIVEC__ ) )
/* XLC or GCC-compatible compiler, targeting PowerPC with VMX/VSX */
#	include <altivec.h>
#elif defined( __GNUC__ ) && defined( __SPE__ )
/* GCC-compatible compiler, targeting PowerPC with SPE */
#	include <spe.h>
#endif

void yield_thread( void )
{
#if defined( ZPL_SYSTEM_WINDOWS )
	_mm_pause();
#elif defined( ZPL_SYSTEM_OSX ) || defined( ZPL_COMPILER_TINYC )
	__asm__ volatile( "" : : : "memory" );
#elif defined( ZPL_CPU_X86 )
	_mm_pause();
#endif
}

void mfence( void )
{
#if defined( ZPL_SYSTEM_WINDOWS )
	_ReadWriteBarrier();
#elif defined( ZPL_COMPILER_TINYC )
	__asm__ volatile( "" : : : "memory" );
#elif defined( ZPL_SYSTEM_OSX )
	__sync_synchronize();
#elif defined( ZPL_CPU_X86 )
	_mm_mfence();
#endif
}

void sfence( void )
{
#if defined( ZPL_SYSTEM_WINDOWS )
	_WriteBarrier();
#elif defined( ZPL_SYSTEM_OSX ) || defined( ZPL_COMPILER_TINYC )
	__asm__ volatile( "" : : : "memory" );
#elif defined( ZPL_CPU_X86 )
	_mm_sfence();
#endif
}

void lfence( void )
{
#if defined( ZPL_SYSTEM_WINDOWS )
	_ReadBarrier();
#elif defined( ZPL_SYSTEM_OSX ) || defined( ZPL_COMPILER_TINYC )
	__asm__ volatile( "" : : : "memory" );
#elif defined( ZPL_CPU_X86 )
	_mm_lfence();
#endif
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: source/threading/mutex.c


ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

void mutex_init( mutex* m )
{
#if defined( ZPL_SYSTEM_WINDOWS )
	InitializeCriticalSection( ( CRITICAL_SECTION* )m->win32_critical_section );
#else
	pthread_mutex_init( &m->pthread_mutex, NULL );
#endif
}

void mutex_destroy( mutex* m )
{
#if defined( ZPL_SYSTEM_WINDOWS )
	DeleteCriticalSection( ( CRITICAL_SECTION* )m->win32_critical_section );
#else
	pthread_mutex_destroy( &m->pthread_mutex );
#endif
}

void mutex_lock( mutex* m )
{
#if defined( ZPL_SYSTEM_WINDOWS )
	EnterCriticalSection( ( CRITICAL_SECTION* )m->win32_critical_section );
#else
	pthread_mutex_lock( &m->pthread_mutex );
#endif
}

b32 mutex_try_lock( mutex* m )
{
#if defined( ZPL_SYSTEM_WINDOWS )
	return TryEnterCriticalSection( ( CRITICAL_SECTION* )m->win32_critical_section );
#else
	return pthread_mutex_trylock( &m->pthread_mutex );
#endif
}

void mutex_unlock( mutex* m )
{
#if defined( ZPL_SYSTEM_WINDOWS )
	LeaveCriticalSection( ( CRITICAL_SECTION* )m->win32_critical_section );
#else
	pthread_mutex_unlock( &m->pthread_mutex );
#endif
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: source/threading/sem.c


ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

void semaphore_release( semaphore* s )
{
	semaphore_post( s, 1 );
}

#if defined( ZPL_SYSTEM_WINDOWS )

void semaphore_init( semaphore* s )
{
	s->win32_handle = CreateSemaphoreA( NULL, 0, ZPL_I32_MAX, NULL );
}

void semaphore_destroy( semaphore* s )
{
	CloseHandle( s->win32_handle );
}

void semaphore_post( semaphore* s, s32 count )
{
	ReleaseSemaphore( s->win32_handle, count, NULL );
}

void semaphore_wait( semaphore* s )
{
	WaitForSingleObject( s->win32_handle, INFINITE );
}

s32 semaphore_trywait( semaphore* s )
{
	int r = WaitForSingleObject( s->win32_handle, 0 );
	return r;
}

#elif defined( ZPL_SYSTEM_OSX )

void semaphore_init( semaphore* s )
{
	semaphore_create( mach_task_self(), &s->osx_handle, SYNC_POLICY_FIFO, 0 );
}

void semaphore_destroy( semaphore* s )
{
	semaphore_destroy( mach_task_self(), s->osx_handle );
}

void semaphore_post( semaphore* s, s32 count )
{
	while ( count-- > 0 )
		semaphore_signal( s->osx_handle );
}

void semaphore_wait( semaphore* s )
{
	semaphore_wait( s->osx_handle );
}

s32 semaphore_trywait( semaphore* s )
{
	mach_timespec_t t;
	t.tv_sec = t.tv_nsec = 0;
	kern_return_t r      = semaphore_timedwait( s->osx_handle, t );
	return r;
}

#elif defined( ZPL_SYSTEM_UNIX )

void semaphore_init( semaphore* s )
{
	sem_init( &s->unix_handle, 0, 0 );
}

void semaphore_destroy( semaphore* s )
{
	sem_destroy( &s->unix_handle );
}

void semaphore_post( semaphore* s, s32 count )
{
	while ( count-- > 0 )
		sem_post( &s->unix_handle );
}

void semaphore_wait( semaphore* s )
{
	int i;
	do
	{
		i = sem_wait( &s->unix_handle );
	} while ( i == -1 && errno == EINTR );
}

s32 semaphore_trywait( semaphore* s )
{
	int r = sem_trywait( &s->unix_handle );
	return r;
}

#else
#	error Semaphores for this OS are not implemented
#endif

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: source/threading/sync.c


ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

void sync_init( sync* s )
{
	zero_item( s );
	mutex_init( &s->mutex );
	mutex_init( &s->start );
	semaphore_init( &s->release );
}

void sync_destroy( sync* s )
{
	if ( s->waiting )
	{
		ZPL_PANIC( "Cannot destroy while threads are waiting!" );
	}

	mutex_destroy( &s->mutex );
	mutex_destroy( &s->start );
	semaphore_destroy( &s->release );
}

void sync_set_target( sync* s, s32 count )
{
	mutex_lock( &s->start );

	mutex_lock( &s->mutex );
	ZPL_ASSERT( s->target == 0 );
	s->target  = count;
	s->current = 0;
	s->waiting = 0;
	mutex_unlock( &s->mutex );
}

void sync_release( sync* s )
{
	if ( s->waiting )
	{
		semaphore_release( &s->release );
	}
	else
	{
		s->target = 0;
		mutex_unlock( &s->start );
	}
}

s32 sync_reach( sync* s )
{
	s32 n;
	mutex_lock( &s->mutex );
	ZPL_ASSERT( s->current < s->target );
	n = ++s->current;    // NOTE: Record this value to avoid possible race if `return s->current` was done
	if ( s->current == s->target )
		sync_release( s );
	mutex_unlock( &s->mutex );
	return n;
}

void sync_reach_and_wait( sync* s )
{
	mutex_lock( &s->mutex );
	ZPL_ASSERT( s->current < s->target );
	s->current++;
	if ( s->current == s->target )
	{
		sync_release( s );
		mutex_unlock( &s->mutex );
	}
	else
	{
		s->waiting++;                     // NOTE: Waiting, so one more waiter
		mutex_unlock( &s->mutex );        // NOTE: Release the mutex to other threads
		semaphore_wait( &s->release );    // NOTE: Wait for merge completion
		mutex_lock( &s->mutex );          // NOTE: On merge completion, lock mutex
		s->waiting--;                     // NOTE: Done waiting
		sync_release( s );                // NOTE: Restart the next waiter
		mutex_unlock( &s->mutex );
	}
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: source/threading/thread.c


ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

b32 thread_is_running( thread const* t )
{
	return t->is_running != 0;
}

void thread_init_nowait( thread* t )
{
	zero_item( t );

#if defined( ZPL_SYSTEM_WINDOWS )
	t->win32_handle = INVALID_HANDLE_VALUE;
#endif

	t->nowait = true;
}

void thread_init( thread* t )
{
	thread_init_nowait( t );

	t->nowait = false;
	semaphore_init( &t->semaphore );
}

void thread_destroy( thread* t )
{
#if defined( ZPL_SYSTEM_WINDOWS )
	if ( t->win32_handle != INVALID_HANDLE_VALUE )
		thread_join( t );
#else
	if ( t->posix_handle )
		thread_join( t );
#endif
	if ( ! t->nowait )
		semaphore_destroy( &t->semaphore );
}

static void _thread_run( thread* t )
{
	if ( ! t->nowait )
		semaphore_release( &t->semaphore );
	t->return_value = t->proc( t );
}

#if defined( ZPL_SYSTEM_WINDOWS )
static DWORD __stdcall _thread_proc( void* arg )
{
	thread* t     = zpl_cast( thread* ) arg;
	t->is_running = true;
	_thread_run( t );
	t->is_running = false;
	return 0;
}
#else
static void* _thread_proc( void* arg )
{
	thread* t     = zpl_cast( thread* ) arg;
	t->is_running = true;
	_thread_run( t );
	t->is_running = false;
	return NULL;
}
#endif

void thread_start( thread* t, thread_proc proc, void* user_data )
{
	thread_start_with_stack( t, proc, user_data, 0 );
}

void thread_start_with_stack( thread* t, thread_proc proc, void* user_data, sw stack_size )
{
	ZPL_ASSERT( ! t->is_running );
	ZPL_ASSERT( proc != NULL );
	t->proc       = proc;
	t->user_data  = user_data;
	t->stack_size = stack_size;

#if defined( ZPL_SYSTEM_WINDOWS )
	t->win32_handle = CreateThread( NULL, stack_size, _thread_proc, t, 0, NULL );
	ZPL_ASSERT_MSG( t->win32_handle != NULL, "CreateThread: GetLastError" );
#else
	{
		pthread_attr_t attr;
		pthread_attr_init( &attr );
		pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );
		if ( stack_size != 0 )
			pthread_attr_setstacksize( &attr, stack_size );
		pthread_create( &t->posix_handle, &attr, _thread_proc, t );
		pthread_attr_destroy( &attr );
	}
#endif
	if ( ! t->nowait )
		semaphore_wait( &t->semaphore );
}

void thread_join( thread* t )
{
#if defined( ZPL_SYSTEM_WINDOWS )
	WaitForSingleObject( t->win32_handle, INFINITE );
	CloseHandle( t->win32_handle );
	t->win32_handle = INVALID_HANDLE_VALUE;
#else
	pthread_join( t->posix_handle, NULL );
	t->posix_handle = 0;
#endif
}

u32 thread_current_id( void )
{
	u32 thread_id;
#if defined( ZPL_SYSTEM_WINDOWS )
#	if defined( ZPL_ARCH_32_BIT ) && defined( ZPL_CPU_X86 )
	thread_id = ( zpl_cast( u32* ) __readfsdword( 24 ) )[ 9 ];
#	elif defined( ZPL_ARCH_64_BIT ) && defined( ZPL_CPU_X86 )
	thread_id = ( zpl_cast( u32* ) __readgsqword( 48 ) )[ 18 ];
#	else
	thread_id = GetCurrentThreadId();
#	endif

#elif defined( ZPL_SYSTEM_OSX ) && defined( ZPL_ARCH_64_BIT )
	thread_id       = pthread_mach_thread_np( pthread_self() );
#elif defined( ZPL_ARCH_32_BIT ) && defined( ZPL_CPU_X86 )
	__asm__( "mov %%gs:0x08,%0" : "=r"( thread_id ) );
#elif defined( ZPL_ARCH_64_BIT ) && defined( ZPL_CPU_X86 )
	__asm__( "mov %%fs:0x10,%0" : "=r"( thread_id ) );
#elif defined( __ARM_ARCH )
	thread_id = pthread_self();
#else
#	error Unsupported architecture for thread_current_id()
#endif

	return thread_id;
}

void thread_set_name( thread* t, char const* name )
{
#if defined( ZPL_COMPILER_MSVC )
#	pragma pack( push, 8 )

	typedef struct
	{
		DWORD       type;
		char const* name;
		DWORD       id;
		DWORD       flags;
	} zplprivThreadName;

#	pragma pack( pop )

	zplprivThreadName tn;
	tn.type  = 0x1000;
	tn.name  = name;
	tn.id    = GetThreadId( zpl_cast( HANDLE ) t->win32_handle );
	tn.flags = 0;

	__try
	{
		RaiseException( 0x406d1388, 0, size_of( tn ) / 4, zpl_cast( ULONG_PTR* ) & tn );
	}
	__except ( 1 /*EXCEPTION_EXECUTE_HANDLER*/ )
	{
	}

#elif defined( ZPL_SYSTEM_WINDOWS ) && ! defined( ZPL_COMPILER_MSVC )
	unused( t );
	unused( name );
	// IMPORTANT TODO: Set thread name for GCC/Clang on windows
	return;
#elif defined( ZPL_SYSTEM_OSX )
	// TODO: Test if this works
	pthread_setname_np( name );
#else
	unused( t );
	unused( name );
	// TODO: Test if this works
	//        pthread_set_name_np(t->posix_handle, name);
#endif
}

ZPL_END_C_DECLS
ZPL_BEGIN_NAMESPACE


#			if defined( ZPL_MODULE_JOBS )
// file: source/jobs.c

///////////////////////////////////////////////////////////////
//
// Thread Pool
//

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

ZPL_RING_DEFINE( _jobs_ring_, thread_job );

global const u32 _jobs_chances[ ZPL_JOBS_MAX_PRIORITIES ] = { 2, 3, 5, 7, 11 };

sw _jobs_entry( struct thread* thread )
{
	thread_worker* tw = ( thread_worker* )thread->user_data;

	for ( ;; )
	{
		u32 status = atomic32_load( &tw->status );

		switch ( status )
		{
			case ZPL_JOBS_STATUS_READY :
				{
					atomic32_store( &tw->status, ZPL_JOBS_STATUS_BUSY );
					tw->job.proc( tw->job.data );
					atomic32_compare_exchange( &tw->status, ZPL_JOBS_STATUS_BUSY, ZPL_JOBS_STATUS_WAITING );

#ifdef ZPL_JOBS_DEBUG
					++tw->hits;
#endif
				}
				break;

			case ZPL_JOBS_STATUS_WAITING :
				{
#ifdef ZPL_JOBS_DEBUG
					++tw->idle;
#endif
					yield();
				}
				break;

			case ZPL_JOBS_STATUS_TERM :
				{
					return 0;
				}
				break;
		}
	}

	return 0;
}

void jobs_init( jobs_system* pool, AllocatorInfo a, u32 max_threads )
{
	jobs_init_with_limit( pool, a, max_threads, ZPL_JOBS_MAX_QUEUE );
}

void jobs_init_with_limit( jobs_system* pool, AllocatorInfo a, u32 max_threads, u32 max_jobs )
{
	jobs_system pool_ = { 0 };
	*pool             = pool_;

	pool->alloc       = a;
	pool->max_threads = max_threads;
	pool->max_jobs    = max_jobs;
	pool->counter     = 0;

	buffer_init( pool->workers, a, max_threads );

	for ( uw i = 0; i < ZPL_JOBS_MAX_PRIORITIES; ++i )
	{
		thread_queue* q = &pool->queues[ i ];
		_jobs_ring_init( &q->jobs, a, max_jobs );
		q->chance = _jobs_chances[ i ];
	}

	for ( uw i = 0; i < max_threads; ++i )
	{
		thread_worker  worker_ = { 0 };
		thread_worker* tw      = pool->workers + i;
		*tw                    = worker_;

		thread_init( &tw->thread );
		atomic32_store( &tw->status, ZPL_JOBS_STATUS_WAITING );
		thread_start( &tw->thread, _jobs_entry, ( void* )tw );
	}
}

void jobs_free( jobs_system* pool )
{
	for ( uw i = 0; i < pool->max_threads; ++i )
	{
		thread_worker* tw = pool->workers + i;

		atomic32_store( &tw->status, ZPL_JOBS_STATUS_TERM );
		thread_destroy( &tw->thread );
	}

	buffer_free( pool->workers );

	for ( uw i = 0; i < ZPL_JOBS_MAX_PRIORITIES; ++i )
	{
		thread_queue* q = &pool->queues[ i ];
		_jobs_ring_free( &q->jobs );
	}
}

b32 jobs_enqueue_with_priority( jobs_system* pool, jobs_proc proc, void* data, jobs_priority priority )
{
	ZPL_ASSERT( priority >= 0 && priority < ZPL_JOBS_MAX_PRIORITIES );
	ZPL_ASSERT_NOT_NULL( proc );
	thread_job job = { 0 };
	job.proc       = proc;
	job.data       = data;

	if ( ! jobs_full( pool, priority ) )
	{
		_jobs_ring_append( &pool->queues[ priority ].jobs, job );
		return true;
	}
	return false;
}

b32 jobs_enqueue( jobs_system* pool, jobs_proc proc, void* data )
{
	return jobs_enqueue_with_priority( pool, proc, data, ZPL_JOBS_PRIORITY_NORMAL );
}

b32 jobs_empty( jobs_system* pool, jobs_priority priority )
{
	ZPL_ASSERT( priority >= 0 && priority < ZPL_JOBS_MAX_PRIORITIES );
	return _jobs_ring_empty( &pool->queues[ priority ].jobs );
}

b32 jobs_full( jobs_system* pool, jobs_priority priority )
{
	ZPL_ASSERT( priority >= 0 && priority < ZPL_JOBS_MAX_PRIORITIES );
	return _jobs_ring_full( &pool->queues[ priority ].jobs );
}

b32 jobs_done( jobs_system* pool )
{
	for ( uw i = 0; i < pool->max_threads; ++i )
	{
		thread_worker* tw = pool->workers + i;
		if ( atomic32_load( &tw->status ) != ZPL_JOBS_STATUS_WAITING )
		{
			return false;
		}
	}

	return jobs_empty_all( pool );
}

b32 jobs_empty_all( jobs_system* pool )
{
	for ( uw i = 0; i < ZPL_JOBS_MAX_PRIORITIES; ++i )
	{
		if ( ! jobs_empty( pool, ( jobs_priority )i ) )
		{
			return false;
		}
	}
	return true;
}

b32 jobs_full_all( jobs_system* pool )
{
	for ( uw i = 0; i < ZPL_JOBS_MAX_PRIORITIES; ++i )
	{
		if ( ! jobs_full( pool, ( jobs_priority )i ) )
		{
			return false;
		}
	}
	return true;
}

b32 jobs_process( jobs_system* pool )
{
	if ( jobs_empty_all( pool ) )
	{
		return false;
	}
	// NOTE: Process the jobs
	for ( uw i = 0; i < pool->max_threads; ++i )
	{
		thread_worker* tw         = pool->workers + i;
		u32            status     = atomic32_load( &tw->status );
		b32            last_empty = false;

		if ( status == ZPL_JOBS_STATUS_WAITING )
		{
			for ( uw j = 0; j < ZPL_JOBS_MAX_PRIORITIES; ++j )
			{
				thread_queue* q = &pool->queues[ j ];
				if ( jobs_empty( pool, ( jobs_priority )j ) )
				{
					last_empty = ( j + 1 == ZPL_JOBS_MAX_PRIORITIES );
					continue;
				}
				if ( ! last_empty && ( ( pool->counter++ % q->chance ) != 0 ) )
				{
					continue;
				}

				last_empty = false;
				tw->job    = *_jobs_ring_get( &q->jobs );
				atomic32_store( &tw->status, ZPL_JOBS_STATUS_READY );
#ifdef ZPL_JOBS_DEBUG
				++q->hits;
#endif
				break;
			}
		}
	}

	return true;
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

#			endif
#		endif

#		if defined( ZPL_MODULE_PARSER )
// file: source/adt.c


ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

#define _adt_fprintf( s_, fmt_, ... )                 \
	do                                                \
	{                                                 \
		if ( fprintf( s_, fmt_, ##__VA_ARGS__ ) < 0 ) \
			return ZPL_ADT_ERROR_OUT_OF_MEMORY;       \
	} while ( 0 )

u8 adt_make_branch( adt_node* node, AllocatorInfo backing, char const* name, b32 is_array )
{
	u8 type = ZPL_ADT_TYPE_OBJECT;
	if ( is_array )
	{
		type = ZPL_ADT_TYPE_ARRAY;
	}
	adt_node* parent = node->parent;
	zero_item( node );
	node->type   = type;
	node->name   = name;
	node->parent = parent;
	if ( ! array_init( node->nodes, backing ) )
		return ZPL_ADT_ERROR_OUT_OF_MEMORY;
	return 0;
}

u8 adt_destroy_branch( adt_node* node )
{
	ZPL_ASSERT_NOT_NULL( node );
	if ( ( node->type == ZPL_ADT_TYPE_OBJECT || node->type == ZPL_ADT_TYPE_ARRAY ) && node->nodes )
	{
		for ( sw i = 0; i < array_count( node->nodes ); ++i )
		{
			adt_destroy_branch( node->nodes + i );
		}

		array_free( node->nodes );
	}
	return 0;
}

u8 adt_make_leaf( adt_node* node, char const* name, u8 type )
{
	ZPL_ASSERT( type != ZPL_ADT_TYPE_OBJECT && type != ZPL_ADT_TYPE_ARRAY );
	adt_node* parent = node->parent;
	zero_item( node );
	node->type   = type;
	node->name   = name;
	node->parent = parent;
	return 0;
}

adt_node* adt_find( adt_node* node, char const* name, b32 deep_search )
{
	if ( node->type != ZPL_ADT_TYPE_OBJECT )
	{
		return NULL;
	}

	for ( sw i = 0; i < array_count( node->nodes ); i++ )
	{
		if ( ! str_compare( node->nodes[ i ].name, name ) )
		{
			return ( node->nodes + i );
		}
	}

	if ( deep_search )
	{
		for ( sw i = 0; i < array_count( node->nodes ); i++ )
		{
			adt_node* res = adt_find( node->nodes + i, name, deep_search );

			if ( res != NULL )
				return res;
		}
	}

	return NULL;
}

internal adt_node* _adt_get_value( adt_node* node, char const* value )
{
	switch ( node->type )
	{
		case ZPL_ADT_TYPE_MULTISTRING :
		case ZPL_ADT_TYPE_STRING :
			{
				if ( node->string && ! str_compare( node->string, value ) )
				{
					return node;
				}
			}
			break;
		case ZPL_ADT_TYPE_INTEGER :
		case ZPL_ADT_TYPE_REAL :
			{
				char     back[ 4096 ] = { 0 };
				FileInfo tmp;

				/* allocate a file descriptor for a memory-mapped number to string conversion, input source buffer is not cloned, however. */
				file_stream_open( &tmp, heap(), ( u8* )back, size_of( back ), ZPL_FILE_STREAM_WRITABLE );
				adt_print_number( &tmp, node );

				sw  fsize = 0;
				u8* buf   = file_stream_buf( &tmp, &fsize );

				if ( ! str_compare( ( char const* )buf, value ) )
				{
					file_close( &tmp );
					return node;
				}

				file_close( &tmp );
			}
			break;
		default :
			break; /* node doesn't support value based lookup */
	}

	return NULL;
}

internal adt_node* _adt_get_field( adt_node* node, char* name, char* value )
{
	for ( sw i = 0; i < array_count( node->nodes ); i++ )
	{
		if ( ! str_compare( node->nodes[ i ].name, name ) )
		{
			adt_node* child = &node->nodes[ i ];
			if ( _adt_get_value( child, value ) )
			{
				return node; /* this object does contain a field of a specified value! */
			}
		}
	}

	return NULL;
}

adt_node* adt_query( adt_node* node, char const* uri )
{
	ZPL_ASSERT_NOT_NULL( uri );

	if ( *uri == '/' )
	{
		uri++;
	}

	if ( *uri == 0 )
	{
		return node;
	}

	if ( ! node || ( node->type != ZPL_ADT_TYPE_OBJECT && node->type != ZPL_ADT_TYPE_ARRAY ) )
	{
		return NULL;
	}

#if defined ZPL_ADT_URI_DEBUG || 0
	printf( "uri: %s\n", uri );
#endif

	char *    p = ( char* )uri, *b = p, *e = p;
	adt_node* found_node = NULL;

	b = p;
	p = e     = ( char* )str_skip( p, '/' );
	char* buf = bprintf( "%.*s", ( int )( e - b ), b );

	/* handle field value lookup */
	if ( *b == '[' )
	{
		char *l_p = buf + 1, *l_b = l_p, *l_e = l_p, *l_b2 = l_p, *l_e2 = l_p;
		l_e  = ( char* )str_skip( l_p, '=' );
		l_e2 = ( char* )str_skip( l_p, ']' );

		if ( ( ! *l_e && node->type != ZPL_ADT_TYPE_ARRAY ) || ! *l_e2 )
		{
			ZPL_ASSERT_MSG( 0, "Invalid field value lookup" );
			return NULL;
		}

		*l_e2 = 0;

		/* [field=value] */
		if ( *l_e )
		{
			*l_e = 0;
			l_b2 = l_e + 1;

			/* run a value comparison against our own fields */
			if ( node->type == ZPL_ADT_TYPE_OBJECT )
			{
				found_node = _adt_get_field( node, l_b, l_b2 );
			}

			/* run a value comparison against any child that is an object node */
			else if ( node->type == ZPL_ADT_TYPE_ARRAY )
			{
				for ( sw i = 0; i < array_count( node->nodes ); i++ )
				{
					adt_node* child = &node->nodes[ i ];
					if ( child->type != ZPL_ADT_TYPE_OBJECT )
					{
						continue;
					}

					found_node = _adt_get_field( child, l_b, l_b2 );

					if ( found_node )
						break;
				}
			}
		}
		/* [value] */
		else
		{
			for ( sw i = 0; i < array_count( node->nodes ); i++ )
			{
				adt_node* child = &node->nodes[ i ];
				if ( _adt_get_value( child, l_b2 ) )
				{
					found_node = child;
					break; /* we found a matching value in array, ignore the rest of it */
				}
			}
		}

		/* go deeper if uri continues */
		if ( *e )
		{
			return adt_query( found_node, e + 1 );
		}
	}
	/* handle field name lookup */
	else if ( node->type == ZPL_ADT_TYPE_OBJECT )
	{
		found_node = adt_find( node, buf, false );

		/* go deeper if uri continues */
		if ( *e )
		{
			return adt_query( found_node, e + 1 );
		}
	}
	/* handle array index lookup */
	else
	{
		sw idx = ( sw )str_to_i64( buf, NULL, 10 );
		if ( idx >= 0 && idx < array_count( node->nodes ) )
		{
			found_node = &node->nodes[ idx ];

			/* go deeper if uri continues */
			if ( *e )
			{
				return adt_query( found_node, e + 1 );
			}
		}
	}

	return found_node;
}

adt_node* adt_alloc_at( adt_node* parent, sw index )
{
	if ( ! parent || ( parent->type != ZPL_ADT_TYPE_OBJECT && parent->type != ZPL_ADT_TYPE_ARRAY ) )
	{
		return NULL;
	}

	if ( ! parent->nodes )
		return NULL;

	if ( index < 0 || index > array_count( parent->nodes ) )
		return NULL;

	adt_node o = { 0 };
	o.parent   = parent;
	if ( ! array_append_at( parent->nodes, o, index ) )
		return NULL;

	return parent->nodes + index;
}

adt_node* adt_alloc( adt_node* parent )
{
	if ( ! parent || ( parent->type != ZPL_ADT_TYPE_OBJECT && parent->type != ZPL_ADT_TYPE_ARRAY ) )
	{
		return NULL;
	}

	if ( ! parent->nodes )
		return NULL;

	return adt_alloc_at( parent, array_count( parent->nodes ) );
}

b8 adt_set_obj( adt_node* obj, char const* name, AllocatorInfo backing )
{
	return adt_make_branch( obj, backing, name, 0 );
}

b8 adt_set_arr( adt_node* obj, char const* name, AllocatorInfo backing )
{
	return adt_make_branch( obj, backing, name, 1 );
}

b8 adt_set_str( adt_node* obj, char const* name, char const* value )
{
	adt_make_leaf( obj, name, ZPL_ADT_TYPE_STRING );
	obj->string = value;
	return true;
}

b8 adt_set_flt( adt_node* obj, char const* name, f64 value )
{
	adt_make_leaf( obj, name, ZPL_ADT_TYPE_REAL );
	obj->real = value;
	return true;
}

b8 adt_set_int( adt_node* obj, char const* name, s64 value )
{
	adt_make_leaf( obj, name, ZPL_ADT_TYPE_INTEGER );
	obj->integer = value;
	return true;
}

adt_node* adt_move_node_at( adt_node* node, adt_node* new_parent, sw index )
{
	ZPL_ASSERT_NOT_NULL( node );
	ZPL_ASSERT_NOT_NULL( new_parent );
	adt_node* old_parent = node->parent;
	adt_node* new_node   = adt_alloc_at( new_parent, index );
	*new_node            = *node;
	new_node->parent     = new_parent;
	if ( old_parent )
	{
		adt_remove_node( node );
	}
	return new_node;
}

adt_node* adt_move_node( adt_node* node, adt_node* new_parent )
{
	ZPL_ASSERT_NOT_NULL( node );
	ZPL_ASSERT_NOT_NULL( new_parent );
	ZPL_ASSERT( new_parent->type == ZPL_ADT_TYPE_ARRAY || new_parent->type == ZPL_ADT_TYPE_OBJECT );
	return adt_move_node_at( node, new_parent, array_count( new_parent->nodes ) );
}

void adt_swap_nodes( adt_node* node, adt_node* other_node )
{
	ZPL_ASSERT_NOT_NULL( node );
	ZPL_ASSERT_NOT_NULL( other_node );
	adt_node* parent                     = node->parent;
	adt_node* other_parent               = other_node->parent;
	sw        index                      = ( pointer_diff( parent->nodes, node ) / size_of( adt_node ) );
	sw        index2                     = ( pointer_diff( other_parent->nodes, other_node ) / size_of( adt_node ) );
	adt_node  temp                       = parent->nodes[ index ];
	temp.parent                          = other_parent;
	other_parent->nodes[ index2 ].parent = parent;
	parent->nodes[ index ]               = other_parent->nodes[ index2 ];
	other_parent->nodes[ index2 ]        = temp;
}

void adt_remove_node( adt_node* node )
{
	ZPL_ASSERT_NOT_NULL( node );
	ZPL_ASSERT_NOT_NULL( node->parent );
	adt_node* parent = node->parent;
	sw        index  = ( pointer_diff( parent->nodes, node ) / size_of( adt_node ) );
	array_remove_at( parent->nodes, index );
}

adt_node* adt_append_obj( adt_node* parent, char const* name )
{
	adt_node* o = adt_alloc( parent );
	if ( ! o )
		return NULL;
	if ( adt_set_obj( o, name, ZPL_ARRAY_HEADER( parent->nodes )->allocator ) )
	{
		adt_remove_node( o );
		return NULL;
	}
	return o;
}

adt_node* adt_append_arr( adt_node* parent, char const* name )
{
	adt_node* o = adt_alloc( parent );
	if ( ! o )
		return NULL;
	if ( adt_set_arr( o, name, ZPL_ARRAY_HEADER( parent->nodes )->allocator ) )
	{
		adt_remove_node( o );
		return NULL;
	}
	return o;
}

adt_node* adt_append_str( adt_node* parent, char const* name, char const* value )
{
	adt_node* o = adt_alloc( parent );
	if ( ! o )
		return NULL;
	adt_set_str( o, name, value );
	return o;
}

adt_node* adt_append_flt( adt_node* parent, char const* name, f64 value )
{
	adt_node* o = adt_alloc( parent );
	if ( ! o )
		return NULL;
	adt_set_flt( o, name, value );
	return o;
}

adt_node* adt_append_int( adt_node* parent, char const* name, s64 value )
{
	adt_node* o = adt_alloc( parent );
	if ( ! o )
		return NULL;
	adt_set_int( o, name, value );
	return o;
}

/* parser helpers */

char* adt_parse_number( adt_node* node, char* base_str )
{
	ZPL_ASSERT_NOT_NULL( node );
	ZPL_ASSERT_NOT_NULL( base_str );
	char *p = base_str, *e = p;

	s32 base         = 0;
	s32 base2        = 0;
	u8  base2_offset = 0;
	s8  exp = 0, orig_exp = 0;
	u8  neg_zero   = 0;
	u8  lead_digit = 0;
	u8  node_type  = 0;
	u8  node_props = 0;

	/* skip false positives and special cases */
	if ( ! ! strchr( "eE", *p ) || ( ! ! strchr( ".+-", *p ) && ! char_is_hex_digit( *( p + 1 ) ) && *( p + 1 ) != '.' ) )
	{
		return ++base_str;
	}

	node_type = ZPL_ADT_TYPE_INTEGER;
	neg_zero  = false;

	sw   ib        = 0;
	char buf[ 48 ] = { 0 };

	if ( *e == '+' )
		++e;
	else if ( *e == '-' )
	{
		buf[ ib++ ] = *e++;
	}

	if ( *e == '.' )
	{
		node_type   = ZPL_ADT_TYPE_REAL;
		node_props  = ZPL_ADT_PROPS_IS_PARSED_REAL;
		lead_digit  = false;
		buf[ ib++ ] = '0';
		do
		{
			buf[ ib++ ] = *e;
		} while ( char_is_digit( *++e ) );
	}
	else
	{
		if ( ! str_compare( e, "0x", 2 ) || ! str_compare( e, "0X", 2 ) )
		{
			node_props = ZPL_ADT_PROPS_IS_HEX;
		}
		while ( char_is_hex_digit( *e ) || char_to_lower( *e ) == 'x' )
		{
			buf[ ib++ ] = *e++;
		}

		if ( *e == '.' )
		{
			node_type  = ZPL_ADT_TYPE_REAL;
			lead_digit = true;
			u32 step   = 0;

			do
			{
				buf[ ib++ ] = *e;
				++step;
			} while ( char_is_digit( *++e ) );

			if ( step < 2 )
			{
				buf[ ib++ ] = '0';
			}
		}
	}

	/* check if we have a dot here, this is a false positive (IP address, ...) */
	if ( *e == '.' )
	{
		return ++base_str;
	}

	f32  eb          = 10;
	char expbuf[ 6 ] = { 0 };
	sw   expi        = 0;

	if ( *e && ! ! strchr( "eE", *e ) )
	{
		++e;
		if ( *e == '+' || *e == '-' || char_is_digit( *e ) )
		{
			if ( *e == '-' )
			{
				eb = 0.1f;
			}
			if ( ! char_is_digit( *e ) )
			{
				++e;
			}
			while ( char_is_digit( *e ) )
			{
				expbuf[ expi++ ] = *e++;
			}
		}

		orig_exp = exp = ( u8 )str_to_i64( expbuf, NULL, 10 );
	}

	if ( node_type == ZPL_ADT_TYPE_INTEGER )
	{
		node->integer = str_to_i64( buf, 0, 0 );
#ifndef ZPL_PARSER_DISABLE_ANALYSIS
		/* special case: negative zero */
		if ( node->integer == 0 && buf[ 0 ] == '-' )
		{
			neg_zero = true;
		}
#endif
		while ( orig_exp-- > 0 )
		{
			node->integer *= ( s64 )eb;
		}
	}
	else
	{
		node->real = str_to_f64( buf, 0 );

#ifndef ZPL_PARSER_DISABLE_ANALYSIS
		char *q = buf, *base_string = q, *base_string2 = q;
		base_string           = zpl_cast( char* ) str_skip( base_string, '.' );
		*base_string          = '\0';
		base_string2          = base_string + 1;
		char* base_string_off = base_string2;
		while ( *base_string_off++ == '0' )
			base2_offset++;

		base  = ( s32 )str_to_i64( q, 0, 0 );
		base2 = ( s32 )str_to_i64( base_string2, 0, 0 );
		if ( exp )
		{
			exp        = exp * ( ! ( eb == 10.0f ) ? -1 : 1 );
			node_props = ZPL_ADT_PROPS_IS_EXP;
		}

		/* special case: negative zero */
		if ( base == 0 && buf[ 0 ] == '-' )
		{
			neg_zero = true;
		}
#endif
		while ( orig_exp-- > 0 )
		{
			node->real *= eb;
		}
	}

	node->type  = node_type;
	node->props = node_props;

#ifndef ZPL_PARSER_DISABLE_ANALYSIS
	node->base         = base;
	node->base2        = base2;
	node->base2_offset = base2_offset;
	node->exp          = exp;
	node->neg_zero     = neg_zero;
	node->lead_digit   = lead_digit;
#else
	unused( base );
	unused( base2 );
	unused( base2_offset );
	unused( exp );
	unused( neg_zero );
	unused( lead_digit );
#endif
	return e;
}

adt_error adt_print_number( FileInfo* file, adt_node* node )
{
	ZPL_ASSERT_NOT_NULL( file );
	ZPL_ASSERT_NOT_NULL( node );
	if ( node->type != ZPL_ADT_TYPE_INTEGER && node->type != ZPL_ADT_TYPE_REAL )
	{
		return ZPL_ADT_ERROR_INVALID_TYPE;
	}

#ifndef ZPL_PARSER_DISABLE_ANALYSIS
	if ( node->neg_zero )
	{
		_adt_fprintf( file, "-" );
	}
#endif

	switch ( node->type )
	{
		case ZPL_ADT_TYPE_INTEGER :
			{
				if ( node->props == ZPL_ADT_PROPS_IS_HEX )
				{
					_adt_fprintf( file, "0x%llx", ( long long )node->integer );
				}
				else
				{
					_adt_fprintf( file, "%lld", ( long long )node->integer );
				}
			}
			break;

		case ZPL_ADT_TYPE_REAL :
			{
				if ( node->props == ZPL_ADT_PROPS_NAN )
				{
					_adt_fprintf( file, "NaN" );
				}
				else if ( node->props == ZPL_ADT_PROPS_NAN_NEG )
				{
					_adt_fprintf( file, "-NaN" );
				}
				else if ( node->props == ZPL_ADT_PROPS_INFINITY )
				{
					_adt_fprintf( file, "Infinity" );
				}
				else if ( node->props == ZPL_ADT_PROPS_INFINITY_NEG )
				{
					_adt_fprintf( file, "-Infinity" );
				}
				else if ( node->props == ZPL_ADT_PROPS_TRUE )
				{
					_adt_fprintf( file, "true" );
				}
				else if ( node->props == ZPL_ADT_PROPS_FALSE )
				{
					_adt_fprintf( file, "false" );
				}
				else if ( node->props == ZPL_ADT_PROPS_NULL )
				{
					_adt_fprintf( file, "null" );
#ifndef ZPL_PARSER_DISABLE_ANALYSIS
				}
				else if ( node->props == ZPL_ADT_PROPS_IS_EXP )
				{
					_adt_fprintf( file, "%lld.%0*d%llde%lld", ( long long )node->base, node->base2_offset, 0, ( long long )node->base2, ( long long )node->exp );
				}
				else if ( node->props == ZPL_ADT_PROPS_IS_PARSED_REAL )
				{
					if ( ! node->lead_digit )
						_adt_fprintf( file, ".%0*d%lld", node->base2_offset, 0, ( long long )node->base2 );
					else
						_adt_fprintf( file, "%lld.%0*d%lld", ( long long int )node->base2_offset, 0, ( int )node->base, ( long long )node->base2 );
#endif
				}
				else
				{
					_adt_fprintf( file, "%f", node->real );
				}
			}
			break;
	}

	return ZPL_ADT_ERROR_NONE;
}

adt_error adt_print_string( FileInfo* file, adt_node* node, char const* escaped_chars, char const* escape_symbol )
{
	ZPL_ASSERT_NOT_NULL( file );
	ZPL_ASSERT_NOT_NULL( node );
	ZPL_ASSERT_NOT_NULL( escaped_chars );
	if ( node->type != ZPL_ADT_TYPE_STRING && node->type != ZPL_ADT_TYPE_MULTISTRING )
	{
		return ZPL_ADT_ERROR_INVALID_TYPE;
	}

	/* escape string */
	char const *p = node->string, *b = p;
	do
	{
		p = str_skip_any( p, escaped_chars );
		_adt_fprintf( file, "%.*s", ptr_diff( b, p ), b );
		if ( *p && ! ! strchr( escaped_chars, *p ) )
		{
			_adt_fprintf( file, "%s%c", escape_symbol, *p );
			p++;
		}
		b = p;
	} while ( *p );

	return ZPL_ADT_ERROR_NONE;
}

adt_error adt_str_to_number( adt_node* node )
{
	ZPL_ASSERT( node );

	if ( node->type == ZPL_ADT_TYPE_REAL || node->type == ZPL_ADT_TYPE_INTEGER )
		return ZPL_ADT_ERROR_ALREADY_CONVERTED; /* this is already converted/parsed */
	if ( node->type != ZPL_ADT_TYPE_STRING && node->type != ZPL_ADT_TYPE_MULTISTRING )
	{
		return ZPL_ADT_ERROR_INVALID_TYPE;
	}

	adt_parse_number( node, ( char* )node->string );

	return ZPL_ADT_ERROR_NONE;
}

#undef _adt_fprintf

ZPL_END_C_DECLS
ZPL_END_NAMESPACE


/* parsers */
// file: source/parsers/csv.c


#ifdef ZPL_CSV_DEBUG
#	define ZPL_CSV_ASSERT( msg ) ZPL_PANIC( msg )
#else
#	define ZPL_CSV_ASSERT( msg )
#endif


ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

u8 csv_parse_delimiter( csv_object* root, char* text, AllocatorInfo allocator, b32 has_header, char delim )
{
	csv_error err = ZPL_CSV_ERROR_NONE;
	ZPL_ASSERT_NOT_NULL( root );
	ZPL_ASSERT_NOT_NULL( text );
	zero_item( root );
	adt_make_branch( root, allocator, NULL, has_header ? false : true );
	char *p = text, *b = p, *e = p;
	sw    colc = 0, total_colc = 0;

	do
	{
		char d = 0;
		p      = zpl_cast( char* ) str_trim( p, false );
		if ( *p == 0 )
			break;
		adt_node row_item = { 0 };
		row_item.type     = ZPL_ADT_TYPE_STRING;
#ifndef ZPL_PARSER_DISABLE_ANALYSIS
		row_item.name_style = ZPL_ADT_NAME_STYLE_NO_QUOTES;
#endif

		/* handle string literals */
		if ( *p == '"' )
		{
			p = b = e       = p + 1;
			row_item.string = b;
#ifndef ZPL_PARSER_DISABLE_ANALYSIS
			row_item.name_style = ZPL_ADT_NAME_STYLE_DOUBLE_QUOTE;
#endif
			do
			{
				e = zpl_cast( char* ) str_skip( e, '"' );
				if ( *e && *( e + 1 ) == '"' )
				{
					e += 2;
				}
				else
					break;
			} while ( *e );
			if ( *e == 0 )
			{
				ZPL_CSV_ASSERT( "unmatched quoted string" );
				err = ZPL_CSV_ERROR_UNEXPECTED_END_OF_INPUT;
				return err;
			}
			*e = 0;
			p  = zpl_cast( char* ) str_trim( e + 1, true );
			d  = *p;

			/* unescape escaped quotes (so that unescaped text escapes :) */
			{
				char* ep = b;
				do
				{
					if ( *ep == '"' && *( ep + 1 ) == '"' )
					{
						memmove( ep, ep + 1, strlen( ep ) );
					}
					ep++;
				} while ( *ep );
			}
		}
		else if ( *p == delim )
		{
			d               = *p;
			row_item.string = "";
		}
		else if ( *p )
		{
			/* regular data */
			b = e           = p;
			row_item.string = b;
			do
			{
				e++;
			} while ( *e && *e != delim && *e != '\n' );
			if ( *e )
			{
				p = zpl_cast( char* ) str_trim( e, true );
				while ( char_is_space( *( e - 1 ) ) )
				{
					e--;
				}
				d  = *p;
				*e = 0;
			}
			else
			{
				d = 0;
				p = e;
			}

			/* check if number and process if so */
			b32   skip_number = false;
			char* num_p       = b;
			do
			{
				if ( ! char_is_hex_digit( *num_p ) && ( ! strchr( "+-.eExX", *num_p ) ) )
				{
					skip_number = true;
					break;
				}
			} while ( *num_p++ );

			if ( ! skip_number )
			{
				adt_str_to_number( &row_item );
			}
		}

		if ( colc >= array_count( root->nodes ) )
		{
			adt_append_arr( root, NULL );
		}

		array_append( root->nodes[ colc ].nodes, row_item );

		if ( d == delim )
		{
			colc++;
			p++;
		}
		else if ( d == '\n' || d == 0 )
		{
			/* check if number of rows is not mismatched */
			if ( total_colc < colc )
				total_colc = colc;
			else if ( total_colc != colc )
			{
				ZPL_CSV_ASSERT( "mismatched rows" );
				err = ZPL_CSV_ERROR_MISMATCHED_ROWS;
				return err;
			}
			colc = 0;
			if ( d != 0 )
				p++;
		}
	} while ( *p );

	if ( array_count( root->nodes ) == 0 )
	{
		ZPL_CSV_ASSERT( "unexpected end of input. stream is empty." );
		err = ZPL_CSV_ERROR_UNEXPECTED_END_OF_INPUT;
		return err;
	}

	/* consider first row as a header. */
	if ( has_header )
	{
		for ( sw i = 0; i < array_count( root->nodes ); i++ )
		{
			csv_object* col = root->nodes + i;
			csv_object* hdr = col->nodes;
			col->name       = hdr->string;
			array_remove_at( col->nodes, 0 );
		}
	}

	return err;
}

void csv_free( csv_object* obj )
{
	adt_destroy_branch( obj );
}

void _csv_write_record( FileInfo* file, csv_object* node )
{
	switch ( node->type )
	{
		case ZPL_ADT_TYPE_STRING :
			{
#ifndef ZPL_PARSER_DISABLE_ANALYSIS
				switch ( node->name_style )
				{
					case ZPL_ADT_NAME_STYLE_DOUBLE_QUOTE :
						{
							fprintf( file, "\"" );
							adt_print_string( file, node, "\"", "\"" );
							fprintf( file, "\"" );
						}
						break;

					case ZPL_ADT_NAME_STYLE_NO_QUOTES :
						{
#endif
							fprintf( file, "%s", node->string );
#ifndef ZPL_PARSER_DISABLE_ANALYSIS
						}
						break;
				}
#endif
			}
			break;

		case ZPL_ADT_TYPE_REAL :
		case ZPL_ADT_TYPE_INTEGER :
			{
				adt_print_number( file, node );
			}
			break;
	}
}

void _csv_write_header( FileInfo* file, csv_object* header )
{
	csv_object temp = *header;
	temp.string     = temp.name;
	temp.type       = ZPL_ADT_TYPE_STRING;
	_csv_write_record( file, &temp );
}

void csv_write_delimiter( FileInfo* file, csv_object* obj, char delimiter )
{
	ZPL_ASSERT_NOT_NULL( file );
	ZPL_ASSERT_NOT_NULL( obj );
	ZPL_ASSERT( obj->nodes );
	sw cols = array_count( obj->nodes );
	if ( cols == 0 )
		return;

	sw rows = array_count( obj->nodes[ 0 ].nodes );
	if ( rows == 0 )
		return;

	b32 has_headers = obj->nodes[ 0 ].name != NULL;

	if ( has_headers )
	{
		for ( sw i = 0; i < cols; i++ )
		{
			_csv_write_header( file, &obj->nodes[ i ] );
			if ( i + 1 != cols )
			{
				fprintf( file, "%c", delimiter );
			}
		}
		fprintf( file, "\n" );
	}

	for ( sw r = 0; r < rows; r++ )
	{
		for ( sw i = 0; i < cols; i++ )
		{
			_csv_write_record( file, &obj->nodes[ i ].nodes[ r ] );
			if ( i + 1 != cols )
			{
				fprintf( file, "%c", delimiter );
			}
		}
		fprintf( file, "\n" );
	}
}

String csv_write_string_delimiter( AllocatorInfo a, csv_object* obj, char delimiter )
{
	FileInfo tmp;
	file_stream_new( &tmp, a );
	csv_write_delimiter( &tmp, obj, delimiter );
	sw     fsize;
	u8*    buf    = file_stream_buf( &tmp, &fsize );
	String output = string_make_length( a, ( char* )buf, fsize );
	file_close( &tmp );
	return output;
}

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

// file: source/parsers/json.c

////////////////////////////////////////////////////////////////
//
// JSON5 Parser
//
//


#ifdef ZPL_JSON_DEBUG
#	define ZPL_JSON_ASSERT( msg ) ZPL_PANIC( msg )
#else
#	define ZPL_JSON_ASSERT( msg )
#endif

ZPL_BEGIN_NAMESPACE
ZPL_BEGIN_C_DECLS

char* _json_parse_object( adt_node* obj, char* base, AllocatorInfo a, u8* err_code );
char* _json_parse_array( adt_node* obj, char* base, AllocatorInfo a, u8* err_code );
char* _json_parse_value( adt_node* obj, char* base, AllocatorInfo a, u8* err_code );
char* _json_parse_name( adt_node* obj, char* base, u8* err_code );
char* _json_trim( char* base, b32 catch_newline );
b8    _json_write_value( FileInfo* f, adt_node* o, adt_node* t, sw indent, b32 is_inline, b32 is_last );

#define _json_fprintf( s_, fmt_, ... )                \
	do                                                \
	{                                                 \
		if ( fprintf( s_, fmt_, ##__VA_ARGS__ ) < 0 ) \
			return false;                             \
	} while ( 0 )

#define __ind( x ) \
	if ( x > 0 )   \
		_json_fprintf( f, "%*r", x, ' ' );

u8 json_parse( adt_node* root, char* text, AllocatorInfo a )
{
	u8 err_code = ZPL_JSON_ERROR_NONE;
	ZPL_ASSERT( root );
	ZPL_ASSERT( text );
	zero_item( root );
	text = _json_trim( text, true );

#ifndef ZPL_PARSER_DISABLE_ANALYSIS
	if ( ! strchr( "{[", *text ) )
	{
		root->cfg_mode = true;
	}
#endif

	_json_parse_object( root, text, a, &err_code );
	return err_code;
}

void json_free( adt_node* obj )
{
	adt_destroy_branch( obj );
}

String json_write_string( AllocatorInfo a, adt_node* obj, sw indent )
{
	FileInfo tmp;
	if ( ! file_stream_new( &tmp, a ) )
		return NULL;
	if ( ! json_write( &tmp, obj, indent ) )
		return NULL;
	sw     fsize;
	u8*    buf    = file_stream_buf( &tmp, &fsize );
	String output = string_make_length( a, ( char* )buf, fsize );
	file_close( &tmp );
	return output;
}

/* private */

#define _json_append_node( x, item )                                               \
	do                                                                             \
	{                                                                              \
		if ( ! array_append( x, item ) )                                           \
		{                                                                          \
			*err_code = ZPL_JSON_ERROR_OUT_OF_MEMORY;                              \
			return NULL;                                                           \
		}                                                                          \
		if ( item.type == ZPL_ADT_TYPE_OBJECT || item.type == ZPL_ADT_TYPE_ARRAY ) \
		{                                                                          \
			for ( sw i = 0; i < array_count( item.nodes ); i++ )                   \
				item.nodes[ i ].parent = array_end( x );                           \
		}                                                                          \
	} while ( 0 );

static ZPL_ALWAYS_INLINE b32 _json_is_assign_char( char c )
{
	return ! ! strchr( ":=|", c );
}

static ZPL_ALWAYS_INLINE b32 _json_is_delim_char( char c )
{
	return ! ! strchr( ",|\n", c );
}

ZPL_DEF_INLINE b32 _json_validate_name( char const* str, char* err );

#define jx( x ) ! char_is_hex_digit( str[ x ] )

ZPL_IMPL_INLINE b32 _json_validate_name( char const* str, char* err )
{
	while ( *str )
	{
		/* todo: refactor name validation. */
		if ( ( str[ 0 ] == '\\' && ! char_is_control( str[ 1 ] ) ) && ( str[ 0 ] == '\\' && jx( 1 ) && jx( 2 ) && jx( 3 ) && jx( 4 ) ) )
		{
			if ( err )
				*err = *str;
			return false;
		}

		++str;
	}

	return true;
}

#undef jx

char* _json_parse_array( adt_node* obj, char* base, AllocatorInfo a, u8* err_code )
{
	ZPL_ASSERT( obj && base );
	char* p = base;

	obj->type = ZPL_ADT_TYPE_ARRAY;
	if ( ! array_init( obj->nodes, a ) )
	{
		*err_code = ZPL_JSON_ERROR_OUT_OF_MEMORY;
		return NULL;
	}

	while ( *p )
	{
		p = _json_trim( p, false );

		if ( *p == ']' )
		{
			return p;
		}

		adt_node elem = { 0 };
		p             = _json_parse_value( &elem, p, a, err_code );

		if ( *err_code != ZPL_JSON_ERROR_NONE )
		{
			return NULL;
		}

		_json_append_node( obj->nodes, elem );

		p = _json_trim( p, false );

		if ( *p == ',' )
		{
			++p;
			continue;
		}
		else
		{
			if ( *p != ']' )
			{
				ZPL_JSON_ASSERT( "end of array unfulfilled" );
				*err_code = ZPL_JSON_ERROR_ARRAY_LEFT_OPEN;
				return NULL;
			}
			return p;
		}
	}

	*err_code = ZPL_JSON_ERROR_INTERNAL;
	return NULL;
}

char* _json_parse_value( adt_node* obj, char* base, AllocatorInfo a, u8* err_code )
{
	ZPL_ASSERT( obj && base );
	char *p = base, *b = p, *e = p;

	/* handle quoted strings */
	if ( ! ! strchr( "`\"'", *p ) )
	{
		char c    = *p;
		obj->type = ( c == '`' ) ? ZPL_ADT_TYPE_MULTISTRING : ZPL_ADT_TYPE_STRING;
		b = e       = p + 1;
		obj->string = b;
		e           = zpl_cast( char* ) str_skip_literal( e, c );
		*e = '\0', p = e + 1;
	}
	else if ( char_is_alpha( *p ) || ( *p == '-' && ! char_is_digit( *( p + 1 ) ) ) )
	{
		/* handle constants */
		if ( str_has_prefix( p, "true" ) )
		{
			obj->type   = ZPL_ADT_TYPE_REAL;
			obj->props  = ZPL_ADT_PROPS_TRUE;
			obj->real   = 1;
			p          += 4;
		}
		else if ( str_has_prefix( p, "false" ) )
		{
			obj->type   = ZPL_ADT_TYPE_REAL;
			obj->props  = ZPL_ADT_PROPS_FALSE;
			obj->real   = 0;
			p          += 5;
		}
		else if ( str_has_prefix( p, "null" ) )
		{
			obj->type   = ZPL_ADT_TYPE_REAL;
			obj->props  = ZPL_ADT_PROPS_NULL;
			obj->real   = 0;
			p          += 4;
		}
		else if ( str_has_prefix( p, "Infinity" ) )
		{
			obj->type   = ZPL_ADT_TYPE_REAL;
			obj->real   = ZPL_INFINITY;
			obj->props  = ZPL_ADT_PROPS_INFINITY;
			p          += 8;
		}
		else if ( str_has_prefix( p, "-Infinity" ) )
		{
			obj->type   = ZPL_ADT_TYPE_REAL;
			obj->real   = -ZPL_INFINITY;
			obj->props  = ZPL_ADT_PROPS_INFINITY_NEG;
			p          += 9;
		}
		else if ( str_has_prefix( p, "NaN" ) )
		{
			obj->type   = ZPL_ADT_TYPE_REAL;
			obj->real   = ZPL_NAN;
			obj->props  = ZPL_ADT_PROPS_NAN;
			p          += 3;
		}
		else if ( str_has_prefix( p, "-NaN" ) )
		{
			obj->type   = ZPL_ADT_TYPE_REAL;
			obj->real   = -ZPL_NAN;
			obj->props  = ZPL_ADT_PROPS_NAN_NEG;
			p          += 4;
		}
		else
		{
			ZPL_JSON_ASSERT( "unknown keyword" );
			*err_code = ZPL_JSON_ERROR_UNKNOWN_KEYWORD;
			return NULL;
		}
	}
	else if ( char_is_digit( *p ) || *p == '+' || *p == '-' || *p == '.' )
	{
		/* handle numbers */
		/* defer operation to our helper method. */
		p = adt_parse_number( obj, p );
	}
	else if ( ! ! strchr( "[{", *p ) )
	{
		/* handle compound objects */
		p = _json_parse_object( obj, p, a, err_code );
		++p;
	}

	return p;
}

char* _json_parse_object( adt_node* obj, char* base, AllocatorInfo a, u8* err_code )
{
	ZPL_ASSERT( obj && base );
	char* p = base;

	p = _json_trim( p, false );
	/**/ if ( *p == '{' )
	{
		++p;
	}
	else if ( *p == '[' )
	{ /* special case for when we call this func on an array. */
		++p;
		obj->type = ZPL_ADT_TYPE_ARRAY;
		return _json_parse_array( obj, p, a, err_code );
	}

	if ( ! array_init( obj->nodes, a ) )
	{
		*err_code = ZPL_JSON_ERROR_OUT_OF_MEMORY;
		return NULL;
	}
	obj->type = ZPL_ADT_TYPE_OBJECT;

	do
	{
		adt_node node = { 0 };
		p             = _json_trim( p, false );
		if ( *p == '}' && obj->type == ZPL_ADT_TYPE_OBJECT )
			return p;
		else if ( *p == ']' && obj->type == ZPL_ADT_TYPE_ARRAY )
			return p;
		else if ( ! ! strchr( "}]", *p ) )
		{
			ZPL_JSON_ASSERT( "mismatched end pair" );
			*err_code = ZPL_JSON_ERROR_OBJECT_END_PAIR_MISMATCHED;
			return NULL;
		}

		/* First, we parse the key, then we proceed to the value itself. */
		p = _json_parse_name( &node, p, err_code );
		if ( err_code && *err_code != ZPL_JSON_ERROR_NONE )
		{
			return NULL;
		}
		p = _json_trim( p + 1, false );
		p = _json_parse_value( &node, p, a, err_code );
		if ( err_code && *err_code != ZPL_JSON_ERROR_NONE )
		{
			return NULL;
		}

		_json_append_node( obj->nodes, node );

		char* end_p = p;
		unused( end_p );
		p = _json_trim( p, true );

		/* this code analyses the keyvalue pair delimiter used in the packet. */
		if ( _json_is_delim_char( *p ) )
		{
#ifndef ZPL_PARSER_DISABLE_ANALYSIS
			adt_node* n    = array_end( obj->nodes );
			n->delim_style = ZPL_ADT_DELIM_STYLE_COMMA;

			if ( *p == '\n' )
				n->delim_style = ZPL_ADT_DELIM_STYLE_NEWLINE;
			else if ( *p == '|' )
			{
				n->delim_style      = ZPL_ADT_DELIM_STYLE_LINE;
				n->delim_line_width = zpl_cast( u8 )( p - end_p );
			}
#endif
			++p;
		}
		p = _json_trim( p, false );
	} while ( *p );
	return p;
}

char* _json_parse_name( adt_node* node, char* base, u8* err_code )
{
	char *p = base, *b = p, *e = p;
	u8    name_style = 0;

	if ( *p == '"' || *p == '\'' || char_is_alpha( *p ) || *p == '_' || *p == '$' )
	{
		if ( *p == '"' || *p == '\'' )
		{
#ifndef ZPL_PARSER_DISABLE_ANALYSIS
			if ( *p == '"' )
			{
				node->name_style = ZPL_ADT_NAME_STYLE_DOUBLE_QUOTE;
			}
			else if ( *p == '\'' )
			{
				node->name_style = ZPL_ADT_NAME_STYLE_SINGLE_QUOTE;
			}
#endif
			char c     = *p;
			b          = ++p;
			e          = zpl_cast( char* ) str_control_skip( b, c );
			node->name = b;

			/* we can safely null-terminate here, since "e" points to the quote pair end. */
			*e++ = '\0';
		}
		else
		{
			b = e = p;
			str_advance_while( e, *e && ( char_is_alphanumeric( *e ) || *e == '_' ) && ! char_is_space( *e ) && ! _json_is_assign_char( *e ) );
			node->name = b;
			name_style = ZPL_ADT_NAME_STYLE_NO_QUOTES;
			/* we defer null-termination as it can potentially wipe our assign char as well. */
		}

		char* assign_p = e;
		unused( assign_p );
		p = _json_trim( e, false );
#ifndef ZPL_PARSER_DISABLE_ANALYSIS
		node->assign_line_width = zpl_cast( u8 )( p - assign_p );
#endif

		if ( *p && ! _json_is_assign_char( *p ) )
		{
			ZPL_JSON_ASSERT( "invalid assignment" );
			*err_code = ZPL_JSON_ERROR_INVALID_ASSIGNMENT;
			return NULL;
		}
		else
		{
#ifndef ZPL_PARSER_DISABLE_ANALYSIS
			if ( *p == '=' )
				node->assign_style = ZPL_ADT_ASSIGN_STYLE_EQUALS;
			else if ( *p == '|' )
				node->assign_style = ZPL_ADT_ASSIGN_STYLE_LINE;
			else
				node->assign_style = ZPL_ADT_ASSIGN_STYLE_COLON;
#endif
		}

		/* since we already know the assign style, we can cut it here for unquoted names */
		if ( name_style == ZPL_ADT_NAME_STYLE_NO_QUOTES && *e )
		{
			*e = '\0';
#ifndef ZPL_PARSER_DISABLE_ANALYSIS
			node->name_style = name_style;
#endif
		}
	}

	if ( node->name && ! _json_validate_name( node->name, NULL ) )
	{
		ZPL_JSON_ASSERT( "invalid name" );
		*err_code = ZPL_JSON_ERROR_INVALID_NAME;
		return NULL;
	}

	return p;
}

char* _json_trim( char* base, b32 catch_newline )
{
	ZPL_ASSERT_NOT_NULL( base );
	char* p = base;
	do
	{
		if ( str_has_prefix( p, "//" ) )
		{
			const char* e  = str_skip( p, '\n' );
			p             += ( e - p );
		}
		else if ( str_has_prefix( p, "/*" ) )
		{
			const char* e = zpl_str_skip( p + 2, '*' );
			if ( *e && *( e + 1 ) == '/' )
			{
				e += 2; /* advance past end comment block */
				p += ( e - p );
			}
		}
		else if ( *p == '\n' && catch_newline )
		{
			return p;
		}
		else if ( ! char_is_space( *p ) )
		{
			return p;
		}
	} while ( *p++ );
	return NULL;
}

b8 json_write( FileInfo* f, adt_node* o, sw indent )
{
	if ( ! o )
		return true;

	ZPL_ASSERT( o->type == ZPL_ADT_TYPE_OBJECT || o->type == ZPL_ADT_TYPE_ARRAY );

	__ind( indent - 4 );
#ifndef ZPL_PARSER_DISABLE_ANALYSIS
	if ( ! o->cfg_mode )
#else
	if ( 1 )
#endif
		_json_fprintf( f, "%c\n", o->type == ZPL_ADT_TYPE_OBJECT ? '{' : '[' );
	else
	{
		indent -= 4;
	}

	if ( o->nodes )
	{
		sw cnt = array_count( o->nodes );

		for ( int i = 0; i < cnt; ++i )
		{
			if ( ! _json_write_value( f, o->nodes + i, o, indent, false, ! ( i < cnt - 1 ) ) )
				return false;
		}
	}

	__ind( indent );

	if ( indent > 0 )
	{
		_json_fprintf( f, "%c", o->type == ZPL_ADT_TYPE_OBJECT ? '}' : ']' );
	}
	else
	{
#ifndef ZPL_PARSER_DISABLE_ANALYSIS
		if ( ! o->cfg_mode )
#endif
			_json_fprintf( f, "%c\n", o->type == ZPL_ADT_TYPE_OBJECT ? '}' : ']' );
	}

	return true;
}

b8 _json_write_value( FileInfo* f, adt_node* o, adt_node* t, sw indent, b32 is_inline, b32 is_last )
{
	adt_node* node  = o;
	indent         += 4;

	if ( ! is_inline )
	{
		__ind( indent );

		if ( t->type != ZPL_ADT_TYPE_ARRAY )
		{
#ifndef ZPL_PARSER_DISABLE_ANALYSIS
			switch ( node->name_style )
			{
				case ZPL_ADT_NAME_STYLE_DOUBLE_QUOTE :
					{
						_json_fprintf( f, "\"%s\"", node->name );
					}
					break;

				case ZPL_ADT_NAME_STYLE_SINGLE_QUOTE :
					{
						_json_fprintf( f, "\'%s\'", node->name );
					}
					break;

				case ZPL_ADT_NAME_STYLE_NO_QUOTES :
					{
						_json_fprintf( f, "%s", node->name );
					}
					break;
			}

			if ( o->assign_style == ZPL_ADT_ASSIGN_STYLE_COLON )
				_json_fprintf( f, ": " );
			else
			{
				__ind( max( o->assign_line_width, 1 ) );

				if ( o->assign_style == ZPL_ADT_ASSIGN_STYLE_EQUALS )
					_json_fprintf( f, "= " );
				else if ( o->assign_style == ZPL_ADT_ASSIGN_STYLE_LINE )
				{
					_json_fprintf( f, "| " );
				}
			}
#else
			_json_fprintf( f, "\"%s\": ", node->name );
#endif
		}
	}

	switch ( node->type )
	{
		case ZPL_ADT_TYPE_STRING :
			{
				_json_fprintf( f, "\"" );
				if ( adt_print_string( f, node, "\"", "\\" ) )
					return false;
				_json_fprintf( f, "\"" );
			}
			break;

		case ZPL_ADT_TYPE_MULTISTRING :
			{
				_json_fprintf( f, "`" );
				if ( adt_print_string( f, node, "`", "\\" ) )
					return false;
				_json_fprintf( f, "`" );
			}
			break;

		case ZPL_ADT_TYPE_ARRAY :
			{
				_json_fprintf( f, "[" );
				sw elemn = array_count( node->nodes );
				for ( int j = 0; j < elemn; ++j )
				{
					sw ind = ( ( node->nodes + j )->type == ZPL_ADT_TYPE_OBJECT || ( node->nodes + j )->type == ZPL_ADT_TYPE_ARRAY ) ? 0 : -4;
					if ( ! _json_write_value( f, node->nodes + j, o, ind, true, true ) )
						return false;

					if ( j < elemn - 1 )
					{
						_json_fprintf( f, ", " );
					}
				}
				_json_fprintf( f, "]" );
			}
			break;

		case ZPL_ADT_TYPE_REAL :
		case ZPL_ADT_TYPE_INTEGER :
			{
				if ( adt_print_number( f, node ) )
					return false;
			}
			break;

		case ZPL_ADT_TYPE_OBJECT :
			{
				if ( ! json_write( f, node, indent ) )
					return false;
			}
			break;
	}

	if ( ! is_inline )
	{
#ifndef ZPL_PARSER_DISABLE_ANALYSIS
		if ( o->delim_style != ZPL_ADT_DELIM_STYLE_COMMA )
		{
			if ( o->delim_style == ZPL_ADT_DELIM_STYLE_NEWLINE )
				_json_fprintf( f, "\n" );
			else if ( o->delim_style == ZPL_ADT_DELIM_STYLE_LINE )
			{
				__ind( o->delim_line_width );
				_json_fprintf( f, "|\n" );
			}
		}
		else
		{
			if ( ! is_last )
			{
				_json_fprintf( f, ",\n" );
			}
			else
			{
				_json_fprintf( f, "\n" );
			}
		}
#else
		if ( ! is_last )
		{
			_json_fprintf( f, ",\n" );
		}
		else
		{
			_json_fprintf( f, "\n" );
		}
#endif
	}

	return true;
}

#undef _json_fprintf
#undef __ind
#undef _json_append_node

ZPL_END_C_DECLS
ZPL_END_NAMESPACE

#		endif

#		if defined( ZPL_COMPILER_MSVC )
#			pragma warning( pop )
#		endif

#		if defined( __GCC__ ) || defined( __GNUC__ ) || defined( __clang__ )
#			pragma GCC diagnostic pop
#		endif

#	endif    // ZPL_IMPLEMENTATION

#	if ! defined( ZPL_PICO_CUSTOM_ROUTINES )
#		undef _printf_err
#		undef _printf_err_va
#		undef _strlen
#	endif

#	if defined( ZPL_EXPOSE_TYPES )
typedef u8   u8;
typedef s8   i8;
typedef u16  u16;
typedef s16  i16;
typedef u32  u32;
typedef s32  i32;
typedef u64  u64;
typedef s64  i64;
typedef b8   b8;
typedef b16  b16;
typedef b32  b32;
typedef f32  f32;
typedef f64  f64;
typedef rune rune;
typedef uw   usize;
typedef sw   isize;
typedef uptr uintptr;
typedef sptr intptr;
#	endif    // ZPL_EXPOSE_TYPES

#endif        // ZPL_H

// TOC:
// zpl.h
// zpl_hedley.h
// header/opts.h
// header/essentials/helpers.h
// header/essentials/memory.h
// header/essentials/memory_custom.h
// header/essentials/types.h
// header/essentials/collections/buffer.h
// header/essentials/collections/list.h
// header/essentials/collections/hashtable.h
// header/essentials/collections/ring.h
// header/essentials/collections/array.h
// header/essentials/debug.h
// header/process.h
// header/threading/fence.h
// header/threading/mutex.h
// header/threading/sync.h
// header/threading/affinity.h
// header/threading/atomic.h
// header/threading/thread.h
// header/threading/sem.h
// header/math.h
// header/jobs.h
// header/parsers/json.h
// header/parsers/csv.h
// header/dll.h
// header/adt.h
// header/core/file_tar.h
// header/core/memory_virtual.h
// header/core/random.h
// header/core/file_stream.h
// header/core/string.h
// header/core/misc.h
// header/core/file.h
// header/core/stringlib.h
// header/core/sort.h
// header/core/print.h
// header/core/system.h
// header/core/file_misc.h
// header/core/time.h
// header/hashing.h
// header/regex.h
// source/hashing.c
// source/adt.c
// source/process.c
// source/essentials/debug.c
// source/essentials/memory_custom.c
// source/essentials/memory.c
// source/dll.c
// source/regex.c
// source/threading/mutex.c
// source/threading/affinity.c
// source/threading/atomic.c
// source/threading/sync.c
// source/threading/thread.c
// source/threading/fence.c
// source/threading/sem.c
// source/parsers/csv.c
// source/parsers/json.c
// source/jobs.c
// source/core/file_stream.c
// source/core/stringlib.c
// source/core/misc.c
// source/core/file_misc.c
// source/core/file.c
// source/core/memory_virtual.c
// source/core/print.c
// source/core/time.c
// source/core/string.c
// source/core/random.c
// source/core/sort.c
// source/core/file_tar.c
// source/opts.c
// source/math.c
