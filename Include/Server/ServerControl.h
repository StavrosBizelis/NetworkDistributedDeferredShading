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
#include <atomic>
#include <mutex>
#include <memory>

#include "Common/Network/NetworkMsg.h"




namespace Network
{
  struct SocketState
  {
    std::atomic_bool m_isPolling;
    std::pair<bool, NetworkMsg> m_expectingMessage;
    std::vector<NetworkMsg> m_inputMsgs;
    std::vector<NetworkMsg> m_outputMsgs;
    
    SocketState(): m_isPolling(false){}
  };

  class ServerControl
  {
  private:
    unsigned int m_hostPort;
    
    asio::io_context m_io;
    asio::ip::tcp::acceptor m_acceptor;
    

    
    std::list< NetworkMsg > m_preInputMsgs;
    std::map< std::shared_ptr<asio::ip::tcp::socket> , SocketState > m_sockets;
    
    
    
    
    std::mutex m_outMsgMut;
    std::mutex m_preInMsgMut;
    std::mutex m_inMsgMut;
    std::mutex m_socketsMut;
    std::mutex m_expectingMessageMut;
    bool m_acceptingNewConnections;
    bool m_waitsForNewConnections;
    bool m_communicatesWithClients;
    unsigned int m_nextID;
    
    
  public:
    ServerControl(const unsigned int &a_hostPort);
    ~ServerControl();
     
    /**
    *   @brief Starts accepting connections phase
    */
    void AcceptConnections();
    
    /**
    *   @brief Ends accepting connections phase
    *   @return a vector of current connected clients
    */
    std::vector<std::shared_ptr<asio::ip::tcp::socket> > StopAcceptingConnections();
    
    /**
    *   @brief Starts communication with the clients for sending - receiving messages
    */
    void StartClientCommunication();
    
    bool RegisterSocket( const std::shared_ptr<asio::ip::tcp::socket>& a_socket);
    void RegisterMessage(const std::shared_ptr<asio::ip::tcp::socket>& a_socket, const std::list< NetworkMsg >::iterator&  a_message);
    
    /**
    *   @brief send message to client with given id
    *   @param a_id: the id of the client
    *   @param a_msg: the message to send
    */
    void PushMsg(const std::shared_ptr<asio::ip::tcp::socket>& a_socket, const NetworkMsg& a_msg);
    
    /**
    *   @brief return a map of the messages
    */
    std::map< std::shared_ptr<asio::ip::tcp::socket>, std::vector<NetworkMsg>> GetMsgs();
    
    
    
    void Update();

    unsigned int GetHostPort(){return m_hostPort;}

  };

}