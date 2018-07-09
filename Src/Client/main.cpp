#define ASIO_STANDALONE 
#define ASIO_HAS_STD_ADDRESSOF
#define ASIO_HAS_STD_ARRAY
#define ASIO_HAS_CSTDINT
#define ASIO_HAS_STD_SHARED_PTR
#define ASIO_HAS_STD_TYPE_TRAITS

#include <asio-1.12.1/include/asio.hpp>

#include <windows.h>
#include "Client/ClientApp.h"
#include "Common/Core/Gamewindow.h"
// int main(int argc, const char * argv[] ) 
// {


  // ClientApp l_app("localhost", 50000);
  // l_app.Initialise();
  
  // for(;;)
  // {
    // l_app.Update();
  // }
 
  
  // l_gameWindow.Deinit();
  // system("pause");
  // return 0;

// }



ClientApp g_app("192.168.1.3", 50001);

LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
  return g_app.ProcessEvents(window, message, w_param, l_param);
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, PSTR, int) 
{
  // create a console to print program output
  AllocConsole();
  freopen("CONOUT$", "w", stdout);
  
  g_app.SetHinstance(hinstance); 
  return g_app.Execute();
}