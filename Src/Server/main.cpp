#define ASIO_STANDALONE 
#define ASIO_HAS_STD_ADDRESSOF
#define ASIO_HAS_STD_ARRAY
#define ASIO_HAS_CSTDINT
#define ASIO_HAS_STD_SHARED_PTR
#define ASIO_HAS_STD_TYPE_TRAITS

#include <asio-1.12.1/include/asio.hpp>


#include <windows.h>

#include "Server/ServerApp.h"

ServerApp g_serverApp( glm::vec2(680, 420) );

LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	return g_serverApp.ProcessEvents(window, message, w_param, l_param);
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, PSTR, int) 
{
  // create a console to print program output
  AllocConsole();
  freopen("CONOUT$", "w", stdout);
  
  g_serverApp.SetHinstance(hinstance); 
  return g_serverApp.Execute();
}