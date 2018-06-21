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
    std::pair<bool, NetworkMsgPtr> m_expectingMessage;
    std::vector<NetworkMsgPtr> m_inputMsgs;
    std::vector<NetworkMsgPtr> m_outputMsgs;
    
    SocketState(): m_isPolling(false){}
  };  
}