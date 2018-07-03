#define ASIO_STANDALONE 
#define ASIO_HAS_STD_ADDRESSOF
#define ASIO_HAS_STD_ARRAY
#define ASIO_HAS_CSTDINT
#define ASIO_HAS_STD_SHARED_PTR
#define ASIO_HAS_STD_TYPE_TRAITS

#include <asio-1.12.1/include/asio.hpp>
#include <windows.h>
#include "Server/ServerControl.h"
#include "Client/ClientControl.h"
#include "Common/Core/MyUtilities.h"
#include "Common/Network/NetworkMsg.h"

int main(int argc, const char * argv[] ) 
{

  if( argc == 1 )
  {
    Network::ServerControl l_ctrl(50000);
    l_ctrl.AcceptConnections();
    while( l_ctrl.GetConnectedClientsCount() < 1 )
      l_ctrl.Update();
    IFDBG( std::cout << "Stop accepting connections " << std::endl << std::endl; );
    std::vector<std::shared_ptr<asio::ip::tcp::socket> > l_socks = l_ctrl.StopAcceptingConnections();

    IFDBG( std::cout << "Start Client Communication " << std::endl; );
    l_ctrl.StartClientCommunication();
    
    unsigned int l_counter = 0;
    for(;;)
    {
      Network::NetworkMsgPtr l_msg = std::make_shared<Network::NetworkMsg>();
      char l_arr[760000];
      
      // l_msg->CreateRenderResultMsg( (char*)(&l_counter), 4, glm::vec2(2) );
      l_msg->CreateRenderResultMsg( l_arr, 760000, glm::vec2(2) );
      l_ctrl.PushMsg(l_socks[0], l_msg);
      // l_counter++;
      l_ctrl.Update();
      
      // std::map< std::shared_ptr<asio::ip::tcp::socket>, std::vector<Network::NetworkMsgPtr> > l_msgs = l_ctrl.GetMsgs();
      // for(std::map< std::shared_ptr<asio::ip::tcp::socket>, std::vector<Network::NetworkMsgPtr> >::iterator i = l_msgs.begin(); i != l_msgs.end(); ++i)
      // for(   std::vector<Network::NetworkMsgPtr>::iterator l_iter = i->second.begin(); l_iter != i->second.end(); ++l_iter)
      // {
        // // IFDBG( std::cout << "received message from: " << i->first  << ", " << *(*l_iter) << std::endl; );
          // IFDBG( std::cout << "received message from: " << i->first  << std::endl; );
          // if( (*l_iter)->GetType() != Network::MsgType::CLNT_RENDER_RESULT )
          // {  
            // IFDBG( std::cout << "wrong type of message: " << std::endl; );
          // }
          // else
          // {
            // // system("pause");
          // }
      // }
    }
    
  }
  else
  {
    Network::ClientControl l_ctrl("localhost", 50000);
    l_ctrl.Connect();
    
    l_ctrl.StartCommunication();

    unsigned int l_counter = 0;
    for(;;)
    {
      // Network::NetworkMsgPtr l_msg = std::make_shared<Network::NetworkMsg>();
      // char l_arr[760000];
      
      // // l_msg->CreateRenderResultMsg( (char*)(&l_counter), 4, glm::vec2(2) );
      // l_msg->CreateRenderResultMsg( l_arr, 760000, glm::vec2(2) );
      // l_ctrl.PushMsg(l_msg);
        // IFDBG( std::cout << "sent message" << std::endl; );
      l_ctrl.Update(); 
      // l_counter++;
      std::vector<Network::NetworkMsgPtr> l_msgs = l_ctrl.GetMsgs();
      for(unsigned int i =0; i < l_msgs.size(); ++i)
        IFDBG( std::cout << "received message" << *(l_msgs[i]) << std::endl; );
    }
    
  }
  
 
  
  system("pause");
  return 0;

}
