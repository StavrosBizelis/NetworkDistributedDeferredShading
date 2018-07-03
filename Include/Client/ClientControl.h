#pragma once

#define ASIO_STANDALONE 
#define ASIO_HAS_STD_ADDRESSOF
#define ASIO_HAS_STD_ARRAY
#define ASIO_HAS_CSTDINT
#define ASIO_HAS_STD_SHARED_PTR
#define ASIO_HAS_STD_TYPE_TRAITS

#include <asio-1.12.1/include/asio.hpp>


#include <windows.h>
#include <string>
#include <map>
#include <list>

#include <mutex>
#include <memory>

#include "Common/Network/NetworkMsg.h"
#include "Common/Network/SocketState.h"




namespace Network
{


  
  class ClientControl
  {
  private:
    std::mutex m_outMsgMut;
    std::mutex m_preInMsgMut;
    std::mutex m_inMsgMut;
    std::mutex m_expectingMessageMut;
    
    
    std::string m_hostName; 
    unsigned int m_hostPort;
    
    asio::io_context m_io;
    asio::ip::tcp::resolver m_resolver;
    
    

    
    std::shared_ptr<asio::ip::tcp::socket> m_socket;
    SocketState m_socketState;
    
    std::list< NetworkMsgPtr > m_preInputMsgs;
    NetworkMsgPtr m_sizeMsgOut; ///< a size msg use for sending messages
    NetworkMsgPtr m_sizeMsgIn; ///< a size msg use for receiving messages
    
    
    bool m_connected;
    bool m_communicatesWithServer;
    
    
    
  public:
    ClientControl(const std::string &a_hostName, const unsigned int &a_hostPort);
    ~ClientControl();
     
    /**
    *   @brief try to connect to the server
    */
    void Connect();
    
    /**
    *   @brief Starts communication with the server for sending - receiving messages
    *   @return success
    */
    bool StartCommunication();
    
    void RegisterMessage( const std::list< NetworkMsgPtr >::iterator&  a_message, const std::size_t& a_size ); ///< do not use - used by internal lambdas
    
    /**
    *   @brief send message to client with given id
    *   @param a_id: the id of the client
    *   @param a_msg: the message to send
    */
    void PushMsg(const NetworkMsgPtr& a_msg);
    
    /**
    *   @brief return a vector of the messages
    */
    std::vector<NetworkMsgPtr> GetMsgs();
    
    
    
    void Update();

    unsigned int GetHostPort(){return m_hostPort;}
    std::string GetHostName(){return m_hostName;}

  };

}