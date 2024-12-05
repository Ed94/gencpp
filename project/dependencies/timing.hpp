#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#	include "filesystem.hpp"
#endif

#pragma region Timing

GEN_API_C_BEGIN

#ifdef GEN_BENCHMARK
//! Return CPU timestamp.
u64 read_cpu_time_stamp_counter( void );

//! Return relative time (in seconds) since the application start.
f64 time_rel( void );

//! Return relative time since the application start.
u64 time_rel_ms( void );
#endif

GEN_API_C_END

#pragma endregion Timing
