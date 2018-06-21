#include "Common/Core/MyUtilities.h"
#include <cstring>


uint32_t ConsistentCharArrToInt32(char* a_arr)
{
  if ( is_bigendian() )
    return ((uint32_t)(*a_arr) << 24) | ((uint32_t)(a_arr[1]) << 16) | ((uint32_t)(a_arr[2]) << 8) | (uint32_t)(a_arr[3]);  
  return ((uint32_t)(a_arr[3]) << 24) | ((uint32_t)(a_arr[2]) << 16) | ((uint32_t)(a_arr[1]) << 8) | (uint32_t)(*a_arr);
}

float ConsistentCharArrToFloat(char* a_arr)
{
  return ConsistentCharArrToInt32(a_arr);
}

void ConsistentInt32ToCharArray(const uint32_t& a_num, char* a_arr)
{
  if( is_bigendian() )
  {
    *(a_arr+3) = ((char *)(&a_num))[0];
    *(a_arr+2) = ((char *)(&a_num))[1];
    *(a_arr+1) = ((char *)(&a_num))[2];
    *(a_arr) = ((char *)(&a_num))[3];
  }
  else
    memcpy(a_arr, &a_num, sizeof(uint32_t) );   
}

void ConsistentFloatToCharArray(const float& a_num, char* a_arr)
{
  if( is_bigendian() )
  {
    *(a_arr+3) = ((char *)(&a_num))[0];
    *(a_arr+2) = ((char *)(&a_num))[1];
    *(a_arr+1) = ((char *)(&a_num))[2];
    *(a_arr) = ((char *)(&a_num))[3];
  }
  else
    memcpy(a_arr, &a_num, sizeof(float) );
  
    
}