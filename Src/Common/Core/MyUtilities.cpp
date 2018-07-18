#include "Common/Core/MyUtilities.h"
#include <cstring>
#include <fstream>      // std::ifstream



uint32_t ConsistentCharArrToUInt32(char* a_arr)
{
  if ( is_bigendian() )
  {
    char l_tmp[4];
    l_tmp[0] = a_arr[3];
    l_tmp[1] = a_arr[2];
    l_tmp[2] = a_arr[1];
    l_tmp[3] = a_arr[0];
    return *(uint32_t*)&l_tmp[0];
  }
  return *(uint32_t*)&a_arr[0];
}
int32_t ConsistentCharArrToInt32(char* a_arr)
{
  if ( is_bigendian() )
  {
    char l_tmp[4];
    l_tmp[0] = a_arr[3];
    l_tmp[1] = a_arr[2];
    l_tmp[2] = a_arr[1];
    l_tmp[3] = a_arr[0];
    return *(int32_t*)&l_tmp[0];
  }
  return *(int32_t*)&a_arr[0];
}

float ConsistentCharArrToFloat(char* a_arr)
{
    if ( is_bigendian() )
  {
    char l_tmp[4];
    l_tmp[0] = a_arr[3];
    l_tmp[1] = a_arr[2];
    l_tmp[2] = a_arr[1];
    l_tmp[3] = a_arr[0];
    return *(uint32_t*)&l_tmp[0];
  }
  return *(float*)&a_arr[0];
}

void ConsistentUInt32ToCharArray(const uint32_t& a_num, char* a_arr)
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
void ConsistentInt32ToCharArray(const int32_t& a_num, char* a_arr)
{
  if( is_bigendian() )
  {
    *(a_arr+3) = ((char *)(&a_num))[0];
    *(a_arr+2) = ((char *)(&a_num))[1];
    *(a_arr+1) = ((char *)(&a_num))[2];
    *(a_arr) = ((char *)(&a_num))[3];
  }
  else
    memcpy(a_arr, &a_num, sizeof(int32_t) );   
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


std::vector<char> ReadFile(const std::string& filename)
{
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("failed to open file!");
  }

  size_t fileSize = (size_t) file.tellg();
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);

  file.close();

  return buffer;
}