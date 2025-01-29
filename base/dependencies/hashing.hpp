#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "containers.hpp"
#endif

#pragma region Hashing

GEN_API u32 crc32( void const* data, ssize len );
GEN_API u64 crc64( void const* data, ssize len );

#pragma endregion Hashing
