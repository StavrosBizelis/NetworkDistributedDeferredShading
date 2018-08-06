#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <glm/glm.hpp>

#ifdef DEBUG
#  define IFDBG(x) x
#else
#  define IFDBG(x)
#endif

enum class ImplTech
{
  OPENGL = 0,
  VULKAN
};


// following code taken from https://www.ibm.com/developerworks/aix/library/au-endianc/index.html
////////////////////////////////////////////////////////////////////////////////////////////////
const int g_varToCheckForEndianness = 1;
#define is_bigendian() ( (*(char*)&g_varToCheckForEndianness) == 0 )


////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t ConsistentCharArrToUInt32(char* a_arr);
int32_t ConsistentCharArrToInt32(char* a_arr);
float ConsistentCharArrToFloat(char* a_arr);

void ConsistentUInt32ToCharArray(const uint32_t& a_num, char* a_arr);
void ConsistentInt32ToCharArray(const int32_t& a_num, char* a_arr);
void ConsistentFloatToCharArray(const float& a_num, char* a_arr);


// temporary file reader
std::vector<char> ReadFile(const std::string& filename);
std::vector<std::string> ReadFileLines(const std::string& filename);


void DecodeConfigFile(const std::vector<std::string>& a_fileLines, std::string& a_ip, unsigned int& a_port, unsigned int& a_numberOfClients, ImplTech& a_implTech, glm::vec2& a_resolution, unsigned int& a_testIndex);