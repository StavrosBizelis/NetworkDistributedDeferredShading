#pragma once

#include <atomic>
#include <utility>
#include <vector>
#include "Common/Network/NetworkMsg.h"

namespace Network
{
  struct SocketState
  {
    std::atomic_bool m_isPolling;
    std::pair<bool, NetworkMsgPtr> m_expectingMessage;    /// bool if true we are aexpecting a message. NetworkMsgPtr preallocated message
    std::pair<unsigned int,unsigned int> m_expectedMessageCompleteness;  /// 1) received bytes count so far, 2) received bytes to complete
    std::vector<NetworkMsgPtr> m_inputMsgs;
    std::vector<NetworkMsgPtr> m_outputMsgs;
    
    SocketState(): m_isPolling(false){}
  };  
}