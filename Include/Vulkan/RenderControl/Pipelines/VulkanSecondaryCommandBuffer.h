#pragma once

#include <Vulkan/Vulkan.hpp>

/**
*   Structure that holds a secondary buffer and provides an interface to easily add commands for use in a VulkanPipeline
*/
struct VulkanSecondaryCommandBuffer
{
  private:
  VkDevice m_logicalDevice;
  VkCommandPool m_cmdPool;
  VkDescriptorSet m_descriptorSet;
  VkPipelineLayout m_pipelineLayout; 
  
  VkCommandBuffer m_secondaryCmdBuffer;
  
  VkRenderPass m_renderPass;
  unsigned int m_subpassIndex;
  
  bool m_ready;
  
  
  /// key: id, pair< first: vertexBuffer, second: indexBuffer>
  // std::map<unsigned int, VulkanIndexedMesh > m_objectsToDraw;
  
  public:
  VulkanSecondaryCommandBuffer(const VkDevice& a_logicalDevice, const VkCommandPool& a_cmdPool, 
                               const VkDescriptorSet& a_descriptorSet, const VkPipelineLayout& a_pipelineLayout,
                               const VkRenderPass& a_renderPass, const unsigned int& a_subpassIndex );
  ~VulkanSecondaryCommandBuffer();
  
  bool operator==(const VulkanSecondaryCommandBuffer &a_other) const;
  bool operator!=(const VulkanSecondaryCommandBuffer &a_other) const;
  
  void Init();
  
  
  bool IsReady();   ///< returns true if it is ready for execution
  bool RecordCommands(const VkPipeline& a_pipeline);  ///< records commands - return IsReady().
  
  
  
  // void AddIndexedMesh(const unsigned int& a_id, const VulkanIndexedMesh& a_indexedMesh);
  void RemoveIndexedMesh(const unsigned int& a_id);
  
  
  VkCommandBuffer GetCommandBufferHandle();
  
  void Clear();
};