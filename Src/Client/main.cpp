#define ASIO_STANDALONE 
#define ASIO_HAS_STD_ADDRESSOF
#define ASIO_HAS_STD_ARRAY
#define ASIO_HAS_CSTDINT
#define ASIO_HAS_STD_SHARED_PTR
#define ASIO_HAS_STD_TYPE_TRAITS

#include <asio-1.12.1/include/asio.hpp>

#include <windows.h>
#include "Client/ClientControl.h"

int main(int argc, const char * argv[] ) 
{
  
  /*
  Network::ClientControl l_client("localhost", 50000);
  l_client.Connect();
  l_client.StartCommunication();
  
  for(;;)
  {
    l_client.Update();
  }
  */
  
  
  system("pause");
  return 0;

}