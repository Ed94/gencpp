#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "containers.hpp"
#endif

#pragma region Hashing

GEN_API_C_BEGIN

u32 crc32( void const* data, ssize len );
u64 crc64( void const* data, ssize len );

GEN_API_C_END

#pragma endregion Hashing
