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

std::vector<std::string> ReadFileLines(const std::string& filename)
{
  std::vector<std::string> l_toReturn;
  std::ifstream l_file(filename);
  if (l_file.is_open())
  {
    std::string l_line;
    while ( std::getline (l_file,l_line) )
    {
      l_toReturn.push_back(l_line);
    }
    l_file.close();
  }
  return l_toReturn;
}


void DecodeConfigFile(const std::vector<std::string>& a_fileLines, std::string& a_ip, unsigned int& a_port, unsigned int& a_numberOfClients, ImplTech& a_implTech, glm::vec2& a_resolution, unsigned int& a_testIndex, std::string& a_fileName,
                      unsigned int& a_numberOfLights, unsigned int& a_compressImage)
{
  a_ip = "localhost";
  a_port = 50001;
  a_numberOfClients = 2;
  a_implTech = ImplTech::VULKAN;
  a_resolution = glm::vec2(680, 420);
  a_testIndex = 0;
  a_fileName = "test1.txt";
  a_numberOfLights = 10;
  a_compressImage = 0;
  
  unsigned int l_currLine = 0;
  // load ip
  bool l_valueSet = false;
  while( !l_valueSet && l_currLine < a_fileLines.size() )  
  {
    if( a_fileLines[l_currLine].size() > 0 )
      if( a_fileLines[l_currLine].find("#") == std::string::npos )
      {
        a_ip = a_fileLines[l_currLine];
        l_valueSet = true;
      }
    ++l_currLine;
  }
  // port
  l_valueSet = false;
  while( !l_valueSet && l_currLine < a_fileLines.size() )  
  {
    if( a_fileLines[l_currLine].size() > 0 )
      if( a_fileLines[l_currLine].find("#") == std::string::npos )
      {
        a_port = std::stoi( a_fileLines[l_currLine] );  
        l_valueSet = true;
      }      
    ++l_currLine;
  }
  
  // a_numberOfClients
  l_valueSet = false;
  while( !l_valueSet && l_currLine < a_fileLines.size() )  
  {
    if( a_fileLines[l_currLine].size() > 0 )
      if( a_fileLines[l_currLine].find("#") == std::string::npos )
      {
        a_numberOfClients = std::stoi( a_fileLines[l_currLine] );  
        l_valueSet = true;
      }      
    ++l_currLine;
  }
  
  // a_implTech
  l_valueSet = false;
  while( !l_valueSet && l_currLine < a_fileLines.size() )  
  {
    if( a_fileLines[l_currLine].size() > 0 )
      if( a_fileLines[l_currLine].find("#") == std::string::npos )
      {
        if( a_fileLines[l_currLine].find("VULKAN") != std::string::npos )
          a_implTech = ImplTech::VULKAN;
        else if( a_fileLines[l_currLine].find("OPENGL") != std::string::npos )
          a_implTech = ImplTech::OPENGL;
        
        l_valueSet = true;
      }      
    ++l_currLine;
  }
  
  // a_resolution
  l_valueSet = false;
  while( !l_valueSet && l_currLine < a_fileLines.size() )  
  {
    if( a_fileLines[l_currLine].size() > 0 )
      if( a_fileLines[l_currLine].find("#") == std::string::npos )
      {
        std::size_t l_spacePos = a_fileLines[l_currLine].find(" ");
        if( l_spacePos != std::string::npos )
        {
          std::string l_width = a_fileLines[l_currLine].substr(0,l_spacePos);
          std::string l_height = a_fileLines[l_currLine].substr(l_spacePos);
          
          a_resolution = glm::vec2(std::stoi( l_width ), std::stoi( l_height ) );
        }
        
        l_valueSet = true;
      }      
    ++l_currLine;
  }
  
  
  // a_testIndex
  l_valueSet = false;
  while( !l_valueSet && l_currLine < a_fileLines.size() )  
  {
    if( a_fileLines[l_currLine].size() > 0 )
      if( a_fileLines[l_currLine].find("#") == std::string::npos )
      {
        a_testIndex = std::stoi( a_fileLines[l_currLine] );  
        l_valueSet = true;
      }      
    ++l_currLine;
  }

  // file type
  l_valueSet = false;
  while( !l_valueSet && l_currLine < a_fileLines.size() )  
  {
    if( a_fileLines[l_currLine].size() > 0 )
      if( a_fileLines[l_currLine].find("#") == std::string::npos )
      {
        a_fileName = a_fileLines[l_currLine];
        l_valueSet = true;
      }
    ++l_currLine;
  }
  
  // lights number
  l_valueSet = false;
  while( !l_valueSet && l_currLine < a_fileLines.size() )  
  {
    if( a_fileLines[l_currLine].size() > 0 )
      if( a_fileLines[l_currLine].find("#") == std::string::npos )
      {
        a_numberOfLights = std::stoi( a_fileLines[l_currLine] );  
        l_valueSet = true;
      }      
    ++l_currLine;
  }
  
  // compress image
  l_valueSet = false;
  while( !l_valueSet && l_currLine < a_fileLines.size() )  
  {
    if( a_fileLines[l_currLine].size() > 0 )
      if( a_fileLines[l_currLine].find("#") == std::string::npos )
      {
        a_compressImage = std::stoi( a_fileLines[l_currLine] );  
        l_valueSet = true;
      }      
    ++l_currLine;
  }
  
}

















