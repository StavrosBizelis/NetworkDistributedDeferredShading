#pragma once

#include <vulkan/vulkan.hpp>

#include <memory> // shared_ptr
#include <vector>
#include <array>
#include <deque>
#include <map>
/**
*   Structure that holds a number of primary command buffers(the same thing-different swapchain framebuffer) and clears it when out of scope
*/
struct VulkanPrimaryCommandBuffer
{
  private:
  VkDevice m_logicalDevice;
  VkCommandPool m_cmdPool;
  std::vector<VkFramebuffer> m_swapChainFramebuffers;
  VkRenderPass m_renderPass;
  glm::vec2 m_resolution;
  
  
  unsigned int m_index;
  std::shared_ptr< std::vector<VkCommandBuffer> > m_cmdBuffers;
  
  public:
  std::deque< std::shared_ptr< VulkanPipeline > > m_pipelines;
  
  std::array<VkClearValue, 2> m_clearValues;
  
  // helpers
  void RecordCommands();
  void RecordPipeline(VkCommandBuffer a_cmdBuffer, const std::shared_ptr< VulkanPipeline >& a_pipeline);
  
  bool m_dirty;
  public:
  VulkanPrimaryCommandBuffer();
  VulkanPrimaryCommandBuffer(const VkDevice& a_logicalDevice, const VkCommandPool& a_cmdPool,
                             const std::vector<VkFramebuffer>& a_swapChainFramebuffers, 
                             const VkRenderPass& a_renderPass, 
                             const glm::vec2& m_resolution  );
  
  ~VulkanPrimaryCommandBuffer();
  
  void Init();
  void ReInit();    ///< reinitialization - does not delete the internal objects - just reinitializes them
  
  void AddPipeline(const std::shared_ptr< VulkanPipeline >& a_pipeline);
  void RemovePipeline(const std::shared_ptr< VulkanPipeline >& a_pipeline);
  
  void Update();    ///< called periodically - if pipelines are added of deleted - and if anything is not ready it reinitializes them
  
  
  VkCommandBuffer GetNextCommandBufferHandle();   ///< returns the next command buffer (many command buffers - each one for each swapchain framebuffer)
  
  std::deque< std::shared_ptr< VulkanPipeline > > GetPipelines();
  
  void Clear();
};