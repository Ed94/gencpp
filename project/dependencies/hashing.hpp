#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "containers.hpp"
#endif

#pragma region Hashing

u32 crc32( void const* data, ssize len );
u64 crc64( void const* data, ssize len );

#pragma endregion Hashing
