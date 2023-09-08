#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "containers.hpp"
#endif

#pragma region Hashing

u32 crc32( void const* data, sw len );
u64 crc64( void const* data, sw len );

#pragma endregion Hashing
