#pragma once

#include <memory>
#include <vector>
#include <deque>
#include <mutex>
#include <thread>
#include <future>
#include <vulkan/vulkan.hpp>
#include <Vulkan/Core/VulkanLogicalDeviceManager.h>
#include <Vulkan/Core/VulkanMemory.h>
#include <Common/Network/NetworkMsg.h>


class VulkanTexturePacker
{
  private:
  std::shared_ptr<VulkanLogicalDeviceManager> m_device;
  std::shared_ptr<VulkanMemory> m_memory;
  std::vector< std::shared_ptr<VulkanImageMemoryChunk> > m_imagesToPack;
  std::vector< VkFence > m_fences;
  
  
  std::vector<Network::NetworkMsgPtr> m_messages;
  
  std::vector< std::shared_ptr< std::thread > > m_threads;
  
  void PackThread(const unsigned int& a_index, bool a_compress = false);
  
  public:
  VulkanTexturePacker(const std::shared_ptr<VulkanLogicalDeviceManager>& a_device, const std::shared_ptr<VulkanMemory>& a_memory, 
                      const std::vector< std::shared_ptr<VulkanImageMemoryChunk> >& a_imagesToPack, const std::vector< VkFence >& a_fences);
                      
  void Pack(const unsigned int& a_index, bool a_compress);
  void BlockTillPacked(const unsigned int& a_index);
  void Get(const unsigned int& a_index, Network::NetworkMsgPtr& a_msg);
};