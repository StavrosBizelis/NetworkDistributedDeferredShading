#pragma once

#include <vulkan/vulkan.hpp>
#include "conddecl.h"

#include "Vulkan/Core/VulkanMemory.h"

#include <memory> // shared_ptr
#include <vector>
#include <queue>
#include <map>


struct VulkanIndexedMesh
{
  std::shared_ptr<VulkanMemoryBuffer> m_vertexBuffer;
  std::shared_ptr<VulkanMemoryBuffer> m_indexBuffer;
  unsigned int m_indicesCount;
  
  VulkanIndexedMesh();
  VulkanIndexedMesh(const std::shared_ptr<VulkanMemoryBuffer>& a_vertexBuffer, const std::shared_ptr<VulkanMemoryBuffer>& a_indexBuffer, const unsigned int& a_indicesCount);

  VulkanIndexedMesh(const VulkanIndexedMesh& a_other) = default;
  VulkanIndexedMesh& operator=(const VulkanIndexedMesh& a_other) = default;
  
  VulkanIndexedMesh(VulkanIndexedMesh&& a_other) = default;   ///< move constructor
  VulkanIndexedMesh& operator=(VulkanIndexedMesh&&) = default;    ///< move assignment operator
};

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
  std::map<unsigned int, VulkanIndexedMesh > m_objectsToDraw;
  
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
  
  
  
  void AddIndexedMesh(const unsigned int& a_id, const VulkanIndexedMesh& a_indexedMesh);
  void RemoveIndexedMesh(const unsigned int& a_id);
  
  
  VkCommandBuffer GetCommandBufferHandle();
  
  void Clear();
};













struct ZRShaderVariable
{
  VkDescriptorSetLayoutBinding m_inputAttribute;
  VkDescriptorPoolSize m_poolSize;
  std::pair< std::shared_ptr<VulkanMemoryBuffer>, unsigned int> m_uniformBuffer;
  
  ZRShaderVariable() = default;
};



struct VulkanPipelineOptions
{
  /**
  *   If not set then shaders do not have any variables so none will be passed
  
  *   @brief Hold the description for the variables used in the shaders of this pipeline
  *   @brief each ZRShaderVariable corresponds to one variable used in the 
  */
  std::shared_ptr<std::vector<ZRShaderVariable> > m_shadersVars;
  
  // blending
  bool m_blendingEnabled; ///< default false
  unsigned int m_sourceBlendingFactor;    ///< VkBlendFactor
  unsigned int m_sourceAlphaBlendingFactor;    ///< VkBlendFactor
  unsigned int m_destBlendingFactor;    ///< VkBlendFactor
  unsigned int m_destAlphaBlendingFactor;    ///< VkBlendFactor
  unsigned int m_blendingOp;  ///< VkBlendOp
  unsigned int m_blendingAlphaOp;  ///< VkBlendOp
  
  // depth tests
  bool m_depthTestEnabled;  ///< default true
  bool m_depthTestWrite;    ///< default true
  unsigned int m_depthTestOperation;  ///< VkCompareOp - default VK_COMPARE_OP_LESS
 
  // stencil tests
  bool m_stencilTestEnabled;    ///< default false
  
  // stencil front face
  unsigned int m_frontStencilFunc; ///<  is VkCompareOp
  unsigned int m_frontStencilReference; ///< uint32_t
  unsigned int m_frontStencilMask; ///< uint32_t
  
  unsigned int m_frontOnStencilFail;
  unsigned int m_frontOnDepthFail;
  unsigned int m_frontOnStencilDepthPass;
  // stencil back face
  unsigned int m_backStencilFunc; ///<  is VkCompareOp
  unsigned int m_backStencilReference; ///< uint32_t
  unsigned int m_backStencilMask; ///< uint32_t
  unsigned int m_backOnStencilFail;
  unsigned int m_backOnDepthFail;
  unsigned int m_backOnStencilDepthPass;    
  
  // rasterizer
  unsigned int m_cullingMode; ///< VkCullModeFlags - default: VK_CULL_MODE_BACK_BIT 
  unsigned int m_polygonMode; ///< VkPolygonMode - default VK_POLYGON_MODE_FILL
  
  
  VulkanPipelineOptions();
  VulkanPipelineOptions(const VulkanPipelineOptions& a_other);
  VulkanPipelineOptions& operator=(const VulkanPipelineOptions& a_other);
  VulkanPipelineOptions(VulkanPipelineOptions&& a_other) = default;   ///< move constructor
  VulkanPipelineOptions& operator=(VulkanPipelineOptions&&) = default;    ///< move assignment operator
  
  
};

/**
*   Structure that holds secondary buffers and pipeline information for use in a VulkanPrimaryCommandBuffer
*/
struct VulkanPipeline
{
  private:
  VkDevice m_logicalDevice;
  VkCommandPool m_cmdPool;
  std::shared_ptr< std::vector<char> > m_vertexCode;
  std::shared_ptr< std::vector<char> > m_fragCode;
  
  VkExtent2D m_extend;
  VkRenderPass m_renderPass;
  
  VkVertexInputBindingDescription m_bindingDesc;
  std::shared_ptr< std::vector<VkVertexInputAttributeDescription> > m_attributeDesc;
  
  // descriptor vars -- create these using VulkanPipelineOptions::m_shadersVars
  VkDescriptorSetLayout m_descriptorSetLayout;
  VkDescriptorPool m_descriptorPool;
  VkDescriptorSet m_descriptorSet;
  
  
  VkPipelineLayout m_pipelineLayout;
  VkPipeline m_graphicsPipeline;
  unsigned int m_subpassIndex;
  std::shared_ptr<std::vector< std::shared_ptr<VulkanSecondaryCommandBuffer> > > m_commandBuffers;
  

  // helpers
  bool m_dirty;
  VkShaderModule CreateShaderModule(const std::vector<char>& a_code, const VkDevice& a_device);
  void CreateDescriptorSetLayout();    ///< create descriptor set layout based on VulkanPipelineOptions::m_shadersVars
  void CreateDescriptorPool();    ///< create descriptor pool based on VulkanPipelineOptions::m_shadersVars
  void CreateDescriptorSet();
  
  // use this secondary cmd buffer in order to bind the pipeline on recording
  VkCommandBuffer m_secondaryCmdBuffer;
  
  VulkanPipelineOptions m_options;

  
  public:
  VulkanPipeline(const VkDevice& a_logicalDevice, const VkCommandPool& a_cmdPool, const VkRenderPass& a_renderPass, const unsigned int& a_subpassIndex, 
                 const VkVertexInputBindingDescription& a_bindingDesc, const std::vector<VkVertexInputAttributeDescription>& a_attributeDesc,
                 const std::vector<char>& a_vertexCode, const std::vector<char>& a_fragCode, const VkExtent2D& a_extend );
  ~VulkanPipeline();
  bool operator==(const VulkanPipeline &a_other) const;
  bool operator!=(const VulkanPipeline &a_other) const;
  
  
  void Init();
  void ReInit();    ///< reinitialization - does not delete the internal objects - just reinitializes them
  
  
  bool IsDirty(bool a_clean); ///< return if changes have been made. a_clean - if true, the pipeline changes are supposed to be taken into consideration
  
  void AddSecondaryBuffer(const std::shared_ptr<VulkanSecondaryCommandBuffer>& a_buffer);
  
  std::shared_ptr<std::vector< std::shared_ptr<VulkanSecondaryCommandBuffer> > > GetSecondaryCommandBuffers();
  std::shared_ptr<std::vector< VkCommandBuffer> > GetSecondaryCommandBuffersHandles();    ///< returns the secondary command buffers ( +the first one is a plus - and is the one that binds the pipeline to the render pass)
  VkPipeline GetPipelineHandle();
  VkPipelineLayout GetPipelineLayout();
  VkDescriptorSetLayout GetDescriptorSetLayout();
  VkDescriptorPool GetDescriptorPool();
  VkDescriptorSet GetDescriptorSet();
  
  void SetOptions(const VulkanPipelineOptions& a_options); 
  VulkanPipelineOptions GetOptions(); 
  
  
  
  void Clear();
};
















/**
*   Structure that holds a number of primary command buffers(the same thing-different swapchain framebuffer) and clears it when out of scope
*/
struct VulkanPrimaryCommandBuffer
{
  private:
  VkDevice m_logicalDevice;
  VkCommandPool m_cmdPool;
  std::shared_ptr< std::vector<VkFramebuffer> > m_swapChainFramebuffers;
  VkRenderPass m_renderPass;
  VkExtent2D m_swapChainExtent;
  
  
  unsigned int m_index;
  std::shared_ptr< std::vector<VkCommandBuffer> > m_cmdBuffers;
  
  public:
  std::queue< std::shared_ptr< VulkanPipeline > > m_pipelines;
  
  std::array<VkClearValue, 2> m_clearValues;
  
  // helpers
  void RecordCommands();
  void RecordPipeline(VkCommandBuffer a_cmdBuffer, const std::shared_ptr< VulkanPipeline >& a_pipeline);
  
  bool m_dirty;
  public:
  VulkanPrimaryCommandBuffer();
  VulkanPrimaryCommandBuffer(const VkDevice& a_logicalDevice, const VkCommandPool& a_cmdPool,
                             const std::shared_ptr< std::vector<VkFramebuffer> >& a_swapChainFramebuffers, 
                             const VkRenderPass& a_renderPass, 
                             const VkExtent2D& a_swapChainExtent  );
  
  ~VulkanPrimaryCommandBuffer();
  
  void Init();
  void ReInit();    ///< reinitialization - does not delete the internal objects - just reinitializes them
  
  void AddPipeline(const std::shared_ptr< VulkanPipeline >& a_pipeline);
  void RemovePipeline(const std::shared_ptr< VulkanPipeline >& a_pipeline);
  
  void Update();    ///< called periodically - if pipelines are added of deleted - and if anything is not ready it reinitializes them
  
  
  VkCommandBuffer GetNextCommandBufferHandle();   ///< returns the next command buffer (many command buffers - each one for each swapchain framebuffer)
  
  std::queue< std::shared_ptr< VulkanPipeline > > GetPipelines();
  
  void Clear();
};
