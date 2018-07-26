#include "Vulkan/RenderControl/Pipelines/VulkanSecondaryCommandBuffer.h"

  VulkanSecondaryCommandBuffer::VulkanSecondaryCommandBuffer(const VkDevice& a_logicalDevice, const VkCommandPool& a_cmdPool, 
                                                             const VkPipelineLayout& a_pipelineLayout,
                                                             const VkRenderPass& a_renderPass, const unsigned int& a_subpassIndex)
    : m_logicalDevice(a_logicalDevice), m_cmdPool(a_cmdPool), m_pipelineLayout(a_pipelineLayout), m_ready(false),
      m_renderPass(a_renderPass), m_subpassIndex(a_subpassIndex) {}
      
  VulkanSecondaryCommandBuffer::~VulkanSecondaryCommandBuffer(){}
  
  
  bool VulkanSecondaryCommandBuffer::operator==(const VulkanSecondaryCommandBuffer &a_other) const
  {
    if ( m_logicalDevice != a_other.m_logicalDevice) return false;
    if ( m_cmdPool != a_other.m_cmdPool) return false;
    if ( m_pipelineLayout != a_other.m_pipelineLayout) return false;
    if ( m_secondaryCmdBuffer != a_other.m_secondaryCmdBuffer) return false;
    if ( m_objectsToDraw.size() != a_other.m_objectsToDraw.size()) return false;
    
    return true;
  }
  bool VulkanSecondaryCommandBuffer::operator!=(const VulkanSecondaryCommandBuffer &a_other) const { return !(*this == a_other); }
  
  void VulkanSecondaryCommandBuffer::Init()
  {
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_cmdPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    allocInfo.commandBufferCount = (uint32_t)1;

    
    if (vkAllocateCommandBuffers(m_logicalDevice, &allocInfo, &m_secondaryCmdBuffer) != VK_SUCCESS) 
      throw std::runtime_error("failed to allocate command buffer!");  
    
    m_ready = false;
  }
  
  bool VulkanSecondaryCommandBuffer::IsReady(){ return m_ready; }
  
  bool VulkanSecondaryCommandBuffer::RecordCommands(const VkPipeline& a_pipeline)
  {
    
    VkCommandBufferInheritanceInfo l_inheritanceInfo;
    l_inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
    l_inheritanceInfo.pNext = NULL;
    l_inheritanceInfo.renderPass = m_renderPass;
    l_inheritanceInfo.subpass = m_subpassIndex;
    // this is null although we could set a framebuffer - but that would require to record a secondary command buffer for each framebuffer
    l_inheritanceInfo.framebuffer = VK_NULL_HANDLE;   
    l_inheritanceInfo.occlusionQueryEnable = VK_TRUE;
    l_inheritanceInfo.queryFlags = VK_QUERY_CONTROL_PRECISE_BIT;
    l_inheritanceInfo.pipelineStatistics = VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT;
    
    
    m_ready = false;
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
    beginInfo.pInheritanceInfo = &l_inheritanceInfo;
    
    vkBeginCommandBuffer(m_secondaryCmdBuffer, &beginInfo);
  
    vkCmdBindPipeline(m_secondaryCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, a_pipeline );
    // actual recording happening here
    for (std::set< VulkanRenderable* >::iterator it=m_objectsToDraw.begin(); it!=m_objectsToDraw.end(); ++it)
    {
      // render each object's meshes
      VKShape* l_shape = reinterpret_cast<VKShape*>( (*it)->GetShape()->GetExtra() );
      if( l_shape )
      {
        unsigned int l_meshes = l_shape->GetMeshesCount();
        for( unsigned int i = 0; i < l_meshes; ++i)
        {

          VkBuffer l_vertBuff = l_shape->GetVertices(i)->m_buffer->m_buffer;
          VkDeviceSize l_vertBuffOffset = l_shape->GetVertices(i)->GetBufferOffset();
          VkBuffer l_indexBuff = l_shape->GetIndices(i)->m_buffer->m_buffer;
          VkDeviceSize l_indexBuffOffset = l_shape->GetIndices(i)->GetBufferOffset();
          
          unsigned int l_indexCount = l_shape->GetIndicesCount(i);
          VkDescriptorSet* l_descSet = (*it)->GetDesciptorSet();
          
          vkCmdBindVertexBuffers(m_secondaryCmdBuffer, 0, 1, &l_vertBuff, &l_vertBuffOffset );
          vkCmdBindIndexBuffer(m_secondaryCmdBuffer, l_indexBuff, l_indexBuffOffset, VK_INDEX_TYPE_UINT16);
          
          
          vkCmdBindDescriptorSets(m_secondaryCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, l_descSet, 0, nullptr);
        
          vkCmdDrawIndexed(m_secondaryCmdBuffer, static_cast<uint32_t>(l_indexCount), 1, 0, 0, 0);
        }
      }
    }
    
    if (vkEndCommandBuffer(m_secondaryCmdBuffer) == VK_SUCCESS)
      m_ready = true;
    
    return m_ready;
    
  }
  
  
  void VulkanSecondaryCommandBuffer::AddMesh(VulkanRenderable* a_indexedMesh)
  {
      m_objectsToDraw.insert( a_indexedMesh );
      m_ready = false;
  }
  
  void VulkanSecondaryCommandBuffer::RemoveMesh(VulkanRenderable* a_indexedMesh)
  {
      m_objectsToDraw.erase(a_indexedMesh);
      m_ready = false;
  }
  
  
  VkCommandBuffer VulkanSecondaryCommandBuffer::GetCommandBufferHandle(){ return m_secondaryCmdBuffer; }

  void VulkanSecondaryCommandBuffer::Clear()
  {
    vkFreeCommandBuffers(m_logicalDevice, m_cmdPool, static_cast<uint32_t>(1), &m_secondaryCmdBuffer);
  }
  