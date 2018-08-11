#pragma once

#include <Vulkan/Core/VulkanTexturePacker.h>



VulkanTexturePacker::VulkanTexturePacker(const std::shared_ptr<VulkanLogicalDeviceManager>& a_device, const std::shared_ptr<VulkanMemory>& a_memory, 
                    const std::vector< std::shared_ptr<VulkanImageMemoryChunk> >& a_imagesToPack, const std::vector< VkFence >& a_fences)
  : m_device(a_device), m_memory(a_memory), m_imagesToPack(a_imagesToPack), m_fences(a_fences)
{
  m_messages.resize( m_imagesToPack.size(), std::make_shared<Network::NetworkMsg>() );
  m_threads.resize(m_imagesToPack.size(), nullptr );
}                    
void VulkanTexturePacker::Pack(const unsigned int& a_index)
{
  m_threads[a_index] = std::make_shared<std::thread>(&VulkanTexturePacker::PackThread, this, a_index);  
}
void VulkanTexturePacker::Get(const unsigned int& a_index, Network::NetworkMsgPtr& a_msg)
{
  if( m_threads[a_index] )
  {
    if( m_threads[a_index]->joinable() )
      m_threads[a_index]->join();
    a_msg = std::make_shared<Network::NetworkMsg>( *m_messages[a_index] );
  }
  else
  {
    char* l_tmp = new char[4];
    l_tmp[0] = 0;
    l_tmp[1] = 0;
    l_tmp[2] = 0;
    l_tmp[3] = 0;
    a_msg->CreateRenderResultMsg( l_tmp, glm::vec2(1), 0, 8, false);
    delete l_tmp;
  }
}


void VulkanTexturePacker::BlockTillPacked(const unsigned int& a_index)
{
  if( !m_threads[a_index] )
    return;
  
  if( m_threads[a_index]->joinable() )
    m_threads[a_index]->join();
  
}
void VulkanTexturePacker::PackThread(const unsigned int& a_index)
{
  vkWaitForFences(m_device->GetDevice(), 1, &m_fences[a_index], VK_TRUE, std::numeric_limits<uint64_t>::max());
  
  std::shared_ptr<VulkanMemoryChunk> l_buf = m_memory->CreateBufferFromImage( m_imagesToPack[a_index] );
  void* data;
  vkMapMemory(m_device->GetDevice(), l_buf->m_memorySpace, l_buf->GetMemoryOffset(), l_buf->m_size, 0, &data);
  
  
  m_messages[a_index]->CreateRenderResultMsg( (char*)data, glm::vec2(m_imagesToPack[a_index]->m_width, m_imagesToPack[a_index]->m_height), 0, 8, false);


  vkUnmapMemory(m_device->GetDevice(), l_buf->m_memorySpace);
  l_buf->Free();
  
}