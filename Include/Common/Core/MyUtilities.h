#pragma once
#include <cstdint>

#ifdef DEBUG
#  define IFDBG(x) x
#else
#  define IFDBG(x)
#endif




// following code taken from https://www.ibm.com/developerworks/aix/library/au-endianc/index.html
////////////////////////////////////////////////////////////////////////////////////////////////
const int g_varToCheckForEndianness = 1;
#define is_bigendian() ( (*(char*)&g_varToCheckForEndianness) == 0 )


////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t ConsistentCharArrToInt32(char* a_arr);
float ConsistentCharArrToFloat(char* a_arr);
void ConsistentInt32ToCharArray(const uint32_t& a_num, char* a_arr);
void ConsistentFloatToCharArray(const float& a_num, char* a_arr);


