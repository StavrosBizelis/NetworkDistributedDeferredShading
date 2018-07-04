/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: ClientControl.cpp
 *   DATE: Wed Jun 20 20:04:02 2018
 *  DESCR: 
 ***********************************************************************/
#include "Client/ClientControl.h"
#include "Common/Core/MyUtilities.h"
namespace Network
{
  /***********************************************************************
   *  Method: ClientControl::ClientControl
   *  Params: const std::string &a_hostName, const unsigned int &a_hostPort
   * Effects: 
   ***********************************************************************/
  ClientControl::ClientControl(const std::string &a_hostName, const unsigned int &a_hostPort)
  : m_hostName(a_hostName), m_hostPort(a_hostPort), m_resolver{m_io}, m_connected(false), m_communicatesWithServer(false)
  {
    
    m_sizeMsgOut = std::make_shared<NetworkMsg>();
    m_sizeMsgOut->CreateSizeMsg(0);
    m_sizeMsgIn = std::make_shared<NetworkMsg>();
    m_sizeMsgIn->CreateSizeMsg(0);
  }


  /***********************************************************************
   *  Method: ClientControl::~ClientControl
   *  Params: 
   * Effects: 
   ***********************************************************************/
  ClientControl::~ClientControl()
  {
  }


  /***********************************************************************
   *  Method: ClientControl::Connect
   *  Params: 
   * Returns: void
   * Effects: 
   ***********************************************************************/
  void ClientControl::Connect()
  {
    asio::ip::tcp::resolver::query l_query(m_hostName, std::to_string(m_hostPort) );
    asio::ip::tcp::resolver::results_type l_endpoints = m_resolver.resolve(l_query);
    m_io.run();
    std::cout << "connnecting to:" << m_hostName << "/" << std::to_string(m_hostPort) << std::endl;
    m_socket = std::make_shared<asio::ip::tcp::socket>(m_io);
    asio::connect(*m_socket, l_endpoints);
    m_connected = true;
    std::cout << "Connected to " << m_hostName  << "/" << std::to_string(m_hostPort) << std::endl << std::endl; 
    m_socket->set_option( asio::ip::tcp::no_delay(true) );
  }


  /***********************************************************************
   *  Method: ClientControl::StartCommunication
   *  Params: 
   * Returns: bool
   * Effects: 
   ***********************************************************************/
  bool
  ClientControl::StartCommunication()
  {
    if( !m_connected )
      return false;
    m_communicatesWithServer = true;
    std::cout << "Start Communication" << std::endl << std::endl;
    return true;
  }

  /***********************************************************************
   *  Method: ClientControl::RegisterMessage
   *  Params: const std::list< NetworkMsgPtr >::iterator& a_message
   * Returns: void
   * Effects: 
   ***********************************************************************/
  void 
  ClientControl::RegisterMessage( const std::list< NetworkMsgPtr >::iterator& a_message, const std::size_t& a_size )
  {
    // allow the socket to poll again
    SocketState& l_state = m_socketState;
    l_state.m_isPolling = false;
    
    // check if it is size message
    if( (*a_message)->GetType() == MsgType::MSG_SIZE )
    {
      // IFDBG( std::cout << "Input Msg Message" << (**a_message) << std::endl );
      // if it is a size message - then we should expect another message after it with the given size
      uint32_t l_size;
      (*a_message)->DeserializeSizeMsg( l_size );
      NetworkMsgPtr l_msg = std::make_shared<NetworkMsg>();
      l_msg->SetSize(l_size);
      
      std::lock_guard<std::mutex> guard(m_expectingMessageMut);
      l_state.m_expectingMessage.first = true;
      l_state.m_expectingMessage.second = l_msg; 
      l_state.m_expectedMessageCompleteness.first = 0;  // zero completeness for the next message
      l_state.m_expectedMessageCompleteness.second = l_size;  // this is the goal
    }
    else
    {
      // IFDBG( std::cout << "Input Message" << (**a_message) << std::endl );
      // add the message to m_inputMsgs
      l_state.m_expectedMessageCompleteness.first += a_size;
      if(l_state.m_expectedMessageCompleteness.first == l_state.m_expectedMessageCompleteness.second)
      {
        std::lock_guard<std::mutex> guard(m_inMsgMut);
        l_state.m_inputMsgs.push_back( *a_message );
        m_socketState.m_expectingMessage.first = false;
        l_state.m_expectedMessageCompleteness.first = 0;
      }
    }

    // remove message from pre input messages
    std::lock_guard<std::mutex> guard(m_preInMsgMut);
    m_preInputMsgs.erase(a_message);
  }

  /***********************************************************************
   *  Method: ClientControl::PushMsg
   *  Params: const NetworkMsgPtr &a_msg
   * Returns: void
   * Effects: 
   ***********************************************************************/
  void
  ClientControl::PushMsg(const NetworkMsgPtr &a_msg)
  {
    std::lock_guard<std::mutex> guard(m_outMsgMut);
    m_socketState.m_outputMsgs.push_back(a_msg);
    // IFDBG( std::cout << "Push message" << *a_msg << std::endl );
  }

    /***********************************************************************
   *  Method: ClientControl::PushMsg
   *  Params: const NetworkMsgPtr &a_msg
   * Returns: void
   * Effects: 
   ***********************************************************************/
  std::vector<NetworkMsgPtr> 
  ClientControl::GetMsgs()
  {
    std::lock_guard<std::mutex> guard(m_inMsgMut);
    std::vector<NetworkMsgPtr> l_toReturn; 
    m_socketState.m_inputMsgs.swap(l_toReturn);
    return l_toReturn;
  }

  /***********************************************************************
   *  Method: ClientControl::Update
   *  Params: 
   * Returns: void
   * Effects: 
   ***********************************************************************/
  void
  ClientControl::Update()
  {
    ClientControl* l_me = this;
    if( m_communicatesWithServer && m_connected)
    {

      // send messages      
      m_outMsgMut.lock();
      for( std::vector<NetworkMsgPtr>::iterator l_message = m_socketState.m_outputMsgs.begin(); l_message != m_socketState.m_outputMsgs.end(); ++l_message )
      {
        // IFDBG( std::cout << "Message to send size: " << (*l_message)->GetSize() << " "; );
        m_sizeMsgOut->CreateSizeMsg( (uint32_t)(*l_message)->GetSize() );
        m_socket->async_send( asio::buffer(m_sizeMsgOut->GetData(), m_sizeMsgOut->GetSize() ), [](const std::error_code& a_error, std::size_t bytes_transferred){} );
        m_socket->async_send( asio::buffer((*l_message)->GetData(), (*l_message)->GetSize() ), [](const std::error_code& a_error, std::size_t bytes_transferred){} );
        // IFDBG( std::cout << "Message sent: " << (*l_message)->GetSize() << " "; );
      }
      m_socketState.m_outputMsgs.clear();
      m_outMsgMut.unlock();
      
      // receive message
      if( !m_socketState.m_isPolling )
      {
        m_socketState.m_isPolling = true;
        if( !m_socketState.m_expectingMessage.first )  // if there is no expected message - create a size message to expect
        {

          m_preInMsgMut.lock();
          m_preInputMsgs.push_front(m_sizeMsgIn);
          m_preInMsgMut.unlock();
        }
        else // if there is an expected message - copy it to the preInputMessages
        {
          std::lock_guard<std::mutex> guard(m_expectingMessageMut);
          
          m_preInMsgMut.lock();
          m_preInputMsgs.push_front( m_socketState.m_expectingMessage.second );
          m_preInMsgMut.unlock();
        }
        
        m_preInMsgMut.lock();
        std::list< NetworkMsgPtr >::iterator l_preInputMessage = m_preInputMsgs.begin();
        m_preInMsgMut.unlock();
        m_socket->async_receive( asio::buffer((*l_preInputMessage)->GetData()+m_socketState.m_expectedMessageCompleteness.first, (*l_preInputMessage)->GetSize()-m_socketState.m_expectedMessageCompleteness.first ),
                                 [ l_me, l_preInputMessage](const std::error_code& a_error, std::size_t length )
                                 {
                                   if (!a_error)
                                   {
                                     // IFDBG( std::cout << "ACTUALLY RECEIVED LENGTH " << length << std::endl<< std::endl; );
                                     l_me->RegisterMessage(l_preInputMessage, length);
                                   }
                                 }
                               );
        
      }
    }
    m_io.run();
  }
  
  
  
  
  

}
