/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: ServerControl.cpp
 *   DATE: Mon Jun 18 21:16:43 2018
 *  DESCR: 
 ***********************************************************************/
#include "Server/ServerControl.h"
namespace Network
{

  /***********************************************************************
   *  Method: ServerControl::ServerControl
   *  Params: const std::string &a_hostname, const unsigned int &a_hostPort
   * Effects: 
   ***********************************************************************/
  ServerControl::ServerControl(const unsigned int &a_hostPort)
  : m_hostPort(a_hostPort), m_io(), m_acceptor( m_io ), m_acceptingNewConnections(false), m_waitsForNewConnections(false), m_nextID(0), m_communicatesWithClients(false)
  {
    
    asio::ip::tcp::endpoint l_endpoint(asio::ip::tcp::v4(), m_hostPort);
    m_acceptor.open( l_endpoint.protocol() );
    m_acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    m_acceptor.bind(l_endpoint);
  }


  /***********************************************************************
   *  Method: ServerControl::~ServerControl
   *  Params: 
   * Effects: 
   ***********************************************************************/
  ServerControl::~ServerControl()
  {
  }


  /***********************************************************************
   *  Method: ServerControl::AcceptConnections
   *  Params: 
   * Returns: void
   * Effects: 
   ***********************************************************************/
  void
  ServerControl::AcceptConnections()
  {
    std::lock_guard<std::mutex> guard(m_socketsMut);
    m_acceptingNewConnections = true;
    m_waitsForNewConnections = false;
  }


  /***********************************************************************
   *  Method: ServerControl::StopAcceptingConnections
   *  Params: 
   * Returns: std::vector<unsigned int>
   * Effects: 
   ***********************************************************************/
  std::vector<std::shared_ptr<asio::ip::tcp::socket> >
  ServerControl::StopAcceptingConnections()
  {
    m_acceptor.cancel();
    std::lock_guard<std::mutex> guard(m_socketsMut);
    m_acceptingNewConnections = false;
    m_waitsForNewConnections = false;
    
    std::vector<std::shared_ptr<asio::ip::tcp::socket> > l_toReturn;
    l_toReturn.reserve(m_sockets.size());
    
    for( std::map< std::shared_ptr<asio::ip::tcp::socket> , SocketState  >::const_iterator l_socket = m_sockets.cbegin(); l_socket != m_sockets.cend(); ++l_socket )
      l_toReturn.push_back(l_socket->first);
    
    return l_toReturn;
  }

  /***********************************************************************
   *  Method: ServerControl::StartClientCommunication
   *  Params: 
   * Returns: void
   * Effects: 
   ***********************************************************************/
  void ServerControl::StartClientCommunication()
  {
    m_communicatesWithClients = true;
  }

  /***********************************************************************
   *  Method: ServerControl::RegisterSocket
   *  Params: const asio::ip::tcp::socket& a_socket
   * Returns: bool
   * Effects: 
   ***********************************************************************/
  bool ServerControl::RegisterSocket( const std::shared_ptr<asio::ip::tcp::socket>& a_socket)
  {
    std::lock_guard<std::mutex> guard(m_socketsMut);
    
    if( m_sockets.find(a_socket) != m_sockets.end() )
      return false;
    

    m_sockets[a_socket];
    return true;
  }


  /***********************************************************************
   *  Method: ServerControl::RegisterMessage
   *  Params: const unsigned int& a_id, const std::list< NetworkMsg >::iterator& a_message
   * Returns: void
   * Effects: 
   ***********************************************************************/
  void ServerControl::RegisterMessage(const std::shared_ptr<asio::ip::tcp::socket>& a_socket, const std::list< NetworkMsg >::iterator& a_message)
  { 
    // allow the socket to poll again
    SocketState& l_state = m_sockets[a_socket];
    l_state.m_isPolling = false;
    
    // check if it is size message
    if( a_message->GetType() == MsgType::MSG_SIZE )
    {
      // if it is a size message - then we should expect another message after it with the given size
      uint32_t l_size;
      a_message->DeserializeSizeMsg( l_size );
      NetworkMsg l_msg;
      l_msg.SetSize(l_size);
      
      std::lock_guard<std::mutex> guard(m_expectingMessageMut);
      l_state.m_expectingMessage.first = true;
      l_state.m_expectingMessage.second = l_msg;
    }
    else
    {
      // add the message to m_inputMsgs
      m_inMsgMut.lock();
      l_state.m_inputMsgs.push_back( std::move(*a_message) );
      m_inMsgMut.unlock();
    }

    // remove message from pre input messages
    m_preInMsgMut.lock();
    m_preInputMsgs.erase(a_message);
    m_preInMsgMut.unlock();
  }

  /***********************************************************************
   *  Method: ServerControl::PushMsg
   *  Params: const unsigned int &a_id, const NetworkMsg &a_msg
   * Returns: void
   * Effects: 
   ***********************************************************************/
  void
  ServerControl::PushMsg(const std::shared_ptr<asio::ip::tcp::socket>& a_socket, const NetworkMsg &a_msg)
  {
    std::lock_guard<std::mutex> guard(m_outMsgMut);
    m_sockets[a_socket].m_outputMsgs.push_back(a_msg);
  }

  /***********************************************************************
   *  Method: ServerControl::GetMsgs
   *  Params: 
   * Returns: std::map< unsigned int, std::vector<NetworkMsg>>
   * Effects: 
   ***********************************************************************/
  std::map< std::shared_ptr<asio::ip::tcp::socket>, std::vector<NetworkMsg>>
  ServerControl::GetMsgs()
  {
    std::map< std::shared_ptr<asio::ip::tcp::socket>, std::vector<NetworkMsg>> l_tmp;
    m_inMsgMut.lock();
    for( std::map< std::shared_ptr<asio::ip::tcp::socket> , SocketState >::iterator l_iter = m_sockets.begin();l_iter != m_sockets.end(); ++l_iter)
    {
      l_tmp[l_iter->first] = std::move( l_iter->second.m_inputMsgs );
      l_iter->second.m_inputMsgs.clear();
    }
    m_inMsgMut.unlock();
    
    return l_tmp;
  }


  /***********************************************************************
   *  Method: ServerControl::Update
   *  Params: 
   * Returns: void
   * Effects: 
   ***********************************************************************/
  void ServerControl::Update()
  {
    ServerControl* l_me = this;
    // check if accepting connections
    if( m_socketsMut.try_lock() )
    {
      if( m_acceptingNewConnections && !m_waitsForNewConnections )
      {  
        
        std::shared_ptr<asio::ip::tcp::socket> l_socket = std::make_shared<asio::ip::tcp::socket>( m_io );
        m_acceptor.async_accept
        (
          *l_socket, 
          [l_me, l_socket](const std::error_code& a_error)
          {
            if (!a_error)
              l_me->RegisterSocket(l_socket);
          }
        );
        m_waitsForNewConnections = true;
      }
      m_socketsMut.unlock();
    }
    
    
    if( m_communicatesWithClients )
    {
      // for each socket
      for( std::map< std::shared_ptr<asio::ip::tcp::socket> , SocketState >::iterator l_iter = m_sockets.begin(); l_iter != m_sockets.end(); ++l_iter )
      {
        
        SocketState& l_state = l_iter->second;
        // send messages        
        for( std::vector<NetworkMsg>::iterator l_message = l_state.m_outputMsgs.begin(); l_message != l_state.m_outputMsgs.end(); ++l_message )
          l_iter->first->send( asio::buffer(l_message->GetData(), l_message->GetSize() ) );
        l_state.m_outputMsgs.clear();
        
        // receive messages
        if( !l_state.m_isPolling )
        {
          l_state.m_isPolling = true;
          
           
          if( !l_state.m_expectingMessage.first )  // if there is no expected message - create a size message to expect
          {
            NetworkMsg l_message;
            l_message.CreateSizeMsg(0);
            m_preInMsgMut.lock();
            m_preInputMsgs.push_front(l_message);
            m_preInMsgMut.unlock();
          }
          else // if there is an expected message - copy it to the preInputMessages
          {
            std::lock_guard<std::mutex> guard(m_expectingMessageMut);
            
            m_preInMsgMut.lock();
            m_preInputMsgs.push_front( std::move(l_state.m_expectingMessage.second) );
            m_preInMsgMut.unlock();
            
            l_state.m_expectingMessage.first = false;
          }
            
          
          m_preInMsgMut.lock();
          std::list< NetworkMsg >::iterator l_preInputMessage = m_preInputMsgs.begin();
          m_preInMsgMut.unlock();
          std::shared_ptr<asio::ip::tcp::socket> l_socket = l_iter->first;
          l_socket->async_receive( asio::buffer(l_preInputMessage->GetData(), l_preInputMessage->GetSize() ),
                                        [ l_me, l_socket, l_preInputMessage](const std::error_code& a_error, std::size_t length )
                                        {
                                          if (!a_error)
                                            l_me->RegisterMessage(l_socket, l_preInputMessage);
                                        }
                                      );
        }
      }
      
    }
    
    
    
  }
}