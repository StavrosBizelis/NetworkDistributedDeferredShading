/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: ServerControl.cpp
 *   DATE: Mon Jun 18 21:16:43 2018
 *  DESCR: 
 ***********************************************************************/
#include "Server/ServerControl.h"
#include "Common/Core/MyUtilities.h"
namespace Network
{

  /***********************************************************************
   *  Method: ServerControl::ServerControl
   *  Params: const std::string &a_hostname, const unsigned int &a_hostPort
   * Effects: 
   ***********************************************************************/
  ServerControl::ServerControl(const unsigned int &a_hostPort)
  : m_hostPort(a_hostPort), m_io(), m_acceptor( m_io, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), m_hostPort) ), 
    m_acceptingNewConnections(false), m_waitsForNewConnections(false), m_nextID(0), m_communicatesWithClients(false)
  {
    m_sizeMsgOut = std::make_shared<NetworkMsg>();
    m_sizeMsgOut->CreateSizeMsg(0);
    
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
    IFDBG( std::cout << "Start Accepting connections" << std::endl );
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
    
    IFDBG( std::cout << "Stop Accepting connections" << std::endl );
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
    IFDBG( std::cout << "Start Client Communication" << std::endl );
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
    {
      IFDBG( std::cout << "Failed to register Socket. It already exists." << std::endl );
      return false;
    }
    NetworkMsgPtr l_msg = std::make_shared<NetworkMsg>();
    l_msg->CreateSizeMsg(0);
    m_sizeMsgIn.push_back( l_msg );
    
    m_sockets[a_socket];
    IFDBG( std::cout << "Register Socket: " << a_socket << std::endl );
    return true;
  }


  /***********************************************************************
   *  Method: ServerControl::RegisterMessage
   *  Params: const unsigned int& a_id, const std::list< NetworkMsgPtr >::iterator& a_message
   * Returns: void
   * Effects: 
   ***********************************************************************/
  void ServerControl::RegisterMessage(const std::shared_ptr<asio::ip::tcp::socket>& a_socket, const std::list< NetworkMsgPtr >::iterator& a_message)
  { 
    // allow the socket to poll again
    SocketState& l_state = m_sockets[a_socket];
    l_state.m_isPolling = false;
    
    // check if it is size message
    if( (*a_message)->GetType() == MsgType::MSG_SIZE )
    {
      IFDBG( std::cout << "Input Msg Message" << (**a_message) << std::endl );
      // if it is a size message - then we should expect another message after it with the given size
      uint32_t l_size;
      (*a_message)->DeserializeSizeMsg( l_size );
      NetworkMsgPtr l_msg = std::make_shared<NetworkMsg>();
      l_msg->SetSize(l_size);
      
      std::lock_guard<std::mutex> guard(m_expectingMessageMut);
      l_state.m_expectingMessage.first = true;
      l_state.m_expectingMessage.second = l_msg;
      
      m_preInMsgMut.lock();
      m_sizeMsgIn.push_front(*a_message);
      m_preInMsgMut.unlock();
    }
    else
    {
      IFDBG( std::cout << "Input Message" << (**a_message) << std::endl );
      // add the message to m_inputMsgs
      m_inMsgMut.lock();
      l_state.m_inputMsgs.push_back( *a_message );
      m_inMsgMut.unlock();
    }

    // remove message from pre input messages
    std::lock_guard<std::mutex> guard(m_preInMsgMut);
    m_preInputMsgs.erase(a_message);
  }

  /***********************************************************************
   *  Method: ServerControl::PushMsg
   *  Params: const unsigned int &a_id, const NetworkMsgPtr &a_msg
   * Returns: void
   * Effects: 
   ***********************************************************************/
  void
  ServerControl::PushMsg(const std::shared_ptr<asio::ip::tcp::socket>& a_socket, const NetworkMsgPtr &a_msg)
  {
    std::lock_guard<std::mutex> guard(m_outMsgMut);
    m_sockets[a_socket].m_outputMsgs.push_back( std::move(a_msg) );
    IFDBG( std::cout << "Push message" << *a_msg << std::endl );
  }

  /***********************************************************************
   *  Method: ServerControl::GetMsgs
   *  Params: 
   * Returns: std::map< unsigned int, std::vector<NetworkMsgPtr>>
   * Effects: 
   ***********************************************************************/
  std::map< std::shared_ptr<asio::ip::tcp::socket>, std::vector<NetworkMsgPtr>>
  ServerControl::GetMsgs()
  {
    std::map< std::shared_ptr<asio::ip::tcp::socket>, std::vector<NetworkMsgPtr>> l_tmp;
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
        IFDBG( std::cout << "Wait for new connections" << std::endl );
        std::shared_ptr<asio::ip::tcp::socket> l_socket = std::make_shared<asio::ip::tcp::socket>( m_io );
        asio::ip::tcp::socket l_sock{m_io};
        m_acceptor.async_accept
        (
          *l_socket,
          [l_me, l_socket](const std::error_code& a_error)
          {
            
            if (!a_error)
              l_me->RegisterSocket(l_socket);
          }
        );
        // IFDBG( std::cout << "accepted 1" << std::endl );
        
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
        std::vector<NetworkMsgPtr> l_toSend;
        m_outMsgMut.lock();
        l_state.m_outputMsgs.swap(l_toSend);
        m_outMsgMut.unlock();
        for( std::vector<NetworkMsgPtr>::iterator l_message = l_toSend.begin(); l_message != l_toSend.end(); ++l_message )
        {
          m_sizeMsgOut->CreateSizeMsg( (uint32_t)(*l_message)->GetSize() );
          uint32_t l_ttt;
          m_sizeMsgOut->DeserializeSizeMsg(l_ttt);
          std::cout << "size message deseriaized size " << l_ttt << std::endl;
          
          std::cout << "actual size " << (uint32_t)(*l_message)->GetSize() << std::endl;
          
          l_iter->first->send( asio::buffer(m_sizeMsgOut->GetData(), m_sizeMsgOut->GetSize() ) );
          l_iter->first->send( asio::buffer((*l_message)->GetData(), (*l_message)->GetSize() ) );
        }
        
        // receive messages
        if( !l_state.m_isPolling )
        {
          l_state.m_isPolling = true;
          
           
          if( !l_state.m_expectingMessage.first )  // if there is no expected message - create a size message to expect
          {
            m_preInMsgMut.lock();
            m_preInputMsgs.push_front( m_sizeMsgIn.front() );
            m_sizeMsgIn.pop_front();
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
          std::list< NetworkMsgPtr >::iterator l_preInputMessage = m_preInputMsgs.begin();
          m_preInMsgMut.unlock();
          
          std::shared_ptr<asio::ip::tcp::socket> l_socket = l_iter->first;
          l_socket->async_receive( asio::buffer( (*l_preInputMessage)->GetData(), (*l_preInputMessage)->GetSize() ),
                                        [ l_me, l_socket, l_preInputMessage](const std::error_code& a_error, std::size_t length )
                                        {
                                          if (!a_error)
                                            l_me->RegisterMessage(l_socket, l_preInputMessage);
                                        }
                                      );
        }
      }

      
    }
    m_io.run();
    
    
  }
}