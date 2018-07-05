#pragma once

#include <atomic>
#include <utility>
#include <vector>
#include <set>
#include "Common/Network/NetworkMsg.h"

namespace Network
{
  struct SocketState
  {
    std::atomic_bool m_isPolling;
    std::pair<bool, NetworkMsgPtr> m_expectingMessage;    ///< bool if true we are aexpecting a message. NetworkMsgPtr preallocated message
    std::pair<unsigned int,unsigned int> m_expectedMessageCompleteness;  ///< 1) received bytes count so far, 2) received bytes to complete
    std::vector<NetworkMsgPtr> m_inputMsgs;         ///< messages to receive
    std::vector<NetworkMsgPtr> m_outputMsgs;        ///< messages to send
    std::multiset<NetworkMsgPtr> m_pendingOutMsgs;    ///< messages that are sent but not finished sending
    
    SocketState(): m_isPolling(false){}
  };  
}