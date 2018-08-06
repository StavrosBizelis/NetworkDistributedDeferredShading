#define ASIO_STANDALONE 
#define ASIO_HAS_STD_ADDRESSOF
#define ASIO_HAS_STD_ARRAY
#define ASIO_HAS_CSTDINT
#define ASIO_HAS_STD_SHARED_PTR
#define ASIO_HAS_STD_TYPE_TRAITS

#include <asio-1.12.1/include/asio.hpp>


#include <windows.h>

#include "Server/ServerApp.h"
#include "Common/Core/MyUtilities.h"
#include <memory>

std::shared_ptr<ServerApp> g_serverApp; //( glm::vec2(680, 420), ImplTech::OPENGL , 2);

LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	return g_serverApp->ProcessEvents(window, message, w_param, l_param);
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, PSTR, int) 
{
  // create a console to print program output
  AllocConsole();
  freopen("CONOUT$", "w", stdout);
  
  
  
  std::vector<std::string> l_setup = ReadFileLines("../Assets/ServerConfiguration.config");
  std::string l_ip;
  unsigned int l_port;
  unsigned int l_numberOfClients;
  ImplTech l_implTech;
  glm::vec2 l_resolution;
  unsigned int l_testIndex;
  DecodeConfigFile(l_setup, l_ip, l_port, l_numberOfClients, l_implTech, l_resolution, l_testIndex);
  
  
  
  g_serverApp = std::make_shared<ServerApp>( l_resolution, l_port, l_implTech, l_numberOfClients, l_testIndex);
  g_serverApp->SetHinstance(hinstance); 
  return g_serverApp->Execute();
}