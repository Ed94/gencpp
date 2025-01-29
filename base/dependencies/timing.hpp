#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#	include "filesystem.hpp"
#endif

#pragma region Timing

#ifdef GEN_BENCHMARK
//! Return CPU timestamp.
GEN_API u64 read_cpu_time_stamp_counter( void );

//! Return relative time (in seconds) since the application start.
GEN_API f64 time_rel( void );

//! Return relative time since the application start.
GEN_API u64 time_rel_ms( void );
#endif

#pragma endregion Timing
