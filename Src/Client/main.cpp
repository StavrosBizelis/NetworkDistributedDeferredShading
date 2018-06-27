#define ASIO_STANDALONE 
#define ASIO_HAS_STD_ADDRESSOF
#define ASIO_HAS_STD_ARRAY
#define ASIO_HAS_CSTDINT
#define ASIO_HAS_STD_SHARED_PTR
#define ASIO_HAS_STD_TYPE_TRAITS

#include <asio-1.12.1/include/asio.hpp>

#include <windows.h>
#include "Client/ClientApp.h"

int main(int argc, const char * argv[] ) 
{
  
  
  ClientApp l_app("localhost", 50000);
  l_app.Initialise();
  
  for(;;)
  {
    l_app.Update();
  }
 
  
  
  system("pause");
  return 0;

}