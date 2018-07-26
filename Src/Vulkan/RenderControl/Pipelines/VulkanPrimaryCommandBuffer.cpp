#include "Vulkan/RenderControl/Pipelines/VulkanPrimaryCommandBuffer.h"



void VulkanPrimaryCommandBuffer::RecordCommands()
{
  for (size_t i = 0; i < m_cmdBuffers.size(); i++) 
  {
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    vkBeginCommandBuffer(m_cmdBuffers[i], &beginInfo);

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_renderPass;
    renderPassInfo.framebuffer = m_swapChainFramebuffers[i];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = {(uint32_t)m_resolution.x, (uint32_t)m_resolution.y};

    
    renderPassInfo.clearValueCount = static_cast<uint32_t>(m_clearValues.size());
    renderPassInfo.pClearValues = m_clearValues.data();
    //begin render pass
    vkCmdBeginRenderPass(m_cmdBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

    
    // actual recording happening here
    std::deque<std::shared_ptr< VKPipeline > > l_newPipelines;
    unsigned int l_counter = m_pipelines.size();
    while( !m_pipelines.empty() )
    {
      l_counter--;
      std::shared_ptr< VKPipeline >  l_currPipeline = m_pipelines.front();
      m_pipelines.pop_front();
      
      
      
      this->RecordPipeline( m_cmdBuffers[i], l_currPipeline );
      l_newPipelines.push_back(l_currPipeline);
      // go to next subpass - in order to attach the next pipeline
      if( l_counter > 0 )
        vkCmdNextSubpass(m_cmdBuffers[i], VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
    }
    m_pipelines = std::move( l_newPipelines );
    
    
    // end render pass
    vkCmdEndRenderPass(m_cmdBuffers[i] );

    if (vkEndCommandBuffer(m_cmdBuffers[i] ) != VK_SUCCESS)
      throw std::runtime_error("VulkanPrimaryCommandBuffer - failed to record primary command buffer!");
  }
}

void VulkanPrimaryCommandBuffer::RecordPipeline(VkCommandBuffer a_cmdBuffer, const std::shared_ptr< VKPipeline >& a_pipeline)
{
    // record pipelines(each pipeline is responsible to provide a recorded secondary command buffer that binds the pipeline to the renderpass)
    std::vector< VkCommandBuffer> l_secondaryCmdBuffs = a_pipeline->GetSecondaryCommandBuffersHandles();
    if( !l_secondaryCmdBuffs.empty() )
    {
      // executing the recorded commands from the secondary command buffers
      // vkCmdExecuteCommands(a_cmdBuffer, (uint32_t)1, l_secondaryCmdBuffs.data() );
      vkCmdExecuteCommands(a_cmdBuffer, (uint32_t)l_secondaryCmdBuffs.size(), l_secondaryCmdBuffs.data() );
    }
}
  
VulkanPrimaryCommandBuffer::VulkanPrimaryCommandBuffer(){}

VulkanPrimaryCommandBuffer::VulkanPrimaryCommandBuffer(const VkDevice& a_logicalDevice, const VkCommandPool& a_cmdPool,
                                                       const std::vector<VkFramebuffer>& a_swapChainFramebuffers, 
                                                       const VkRenderPass& a_renderPass, 
                                                       const glm::vec2& a_resolution  )
  : m_logicalDevice(a_logicalDevice), m_cmdPool(a_cmdPool), m_swapChainFramebuffers(a_swapChainFramebuffers), m_renderPass(a_renderPass), m_resolution(a_resolution), 
    m_clearValues({}), m_index(0), m_dirty(true)
{
  m_clearValues[0].color = {1.0f, 0.0f, 0.0f, 0.0f};
  m_clearValues[1].depthStencil = {1.0f, 0};
}
                            
VulkanPrimaryCommandBuffer::~VulkanPrimaryCommandBuffer() {}


void VulkanPrimaryCommandBuffer::Init()
{
  m_cmdBuffers.resize( m_swapChainFramebuffers.size() );

  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = m_cmdPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = (uint32_t) m_cmdBuffers.size();

  if (vkAllocateCommandBuffers(m_logicalDevice, &allocInfo, m_cmdBuffers.data()) != VK_SUCCESS)
      throw std::runtime_error("VulkanPrimaryCommandBuffer - failed to allocate command buffers! - vkAllocateCommandBuffers failed");
  m_dirty = true;
}

void VulkanPrimaryCommandBuffer::ReInit()
{
  // // // // // // // // // // // initialize primary command buffer
  // // // // // // // // // // Init();
  
  // // // // // // // // // // // reinitialize secondary command buffers
  // // // // // // // // // // std::deque<std::shared_ptr< VKPipeline > > l_newPipelines;

  
  // // // // // // // // // // // reinitialize pipelines
  // // // // // // // // // // while( !m_pipelines.empty() )
  // // // // // // // // // // {
    // // // // // // // // // // std::shared_ptr< VKPipeline >  l_currPipeline = m_pipelines.front();

    // // // // // // // // // // // reinitialize the current pipeline
    // // // // // // // // // // l_currPipeline->ReInit();
    // // // // // // // // // // m_pipelines.pop();
    // // // // // // // // // // l_newPipelines.push(l_currPipeline);
  // // // // // // // // // // }
  // // // // // // // // // // m_pipelines = std::move( l_newPipelines );
  // // // // // // // // // // m_dirty = true;
}

void VulkanPrimaryCommandBuffer::AddPipeline(const std::shared_ptr< VKPipeline >& a_pipeline) { m_pipelines.push_front(a_pipeline); m_dirty = true;}
  
void VulkanPrimaryCommandBuffer::RemovePipeline(const std::shared_ptr< VKPipeline >& a_pipeline)
{
  std::deque<std::shared_ptr< VKPipeline > > l_newPipelines;
  
  while( !m_pipelines.empty() )
  {
    std::shared_ptr< VKPipeline >  l_currPipeline = m_pipelines.front();
    m_pipelines.pop_front();
    
    // push it in the temporary queue only if it is not the given pipeline
    if( !(a_pipeline == l_currPipeline) ) 
      l_newPipelines.push_back(l_currPipeline);
  } 
  m_pipelines = std::move( l_newPipelines );
  m_dirty = true;
}
  

void VulkanPrimaryCommandBuffer::Update()
{
  std::deque<std::shared_ptr< VKPipeline > >::iterator l_it = m_pipelines.begin();
  while( l_it != m_pipelines.end() )
  {    
    // check for anything changed
    if( (*l_it)->IsDirty(true) )
      m_dirty = true;
    
    ++l_it;
  }
  
  
  if( m_dirty )
  {
    RecordCommands();
  }
  
  m_dirty = false;
}


VkCommandBuffer VulkanPrimaryCommandBuffer::GetNextCommandBufferHandle()
{
  // increment command buffer index and check if out of bounds
  m_index++;
  if( m_index >= m_cmdBuffers.size() )
    m_index = 0;
  
  // get the next command buffer
  VkCommandBuffer l_nextCmdBuffer = m_cmdBuffers[m_index];
  return l_nextCmdBuffer;
}

std::deque<std::shared_ptr< VKPipeline > > VulkanPrimaryCommandBuffer::GetPipelines(){ return m_pipelines; }

void VulkanPrimaryCommandBuffer::Clear(){ vkFreeCommandBuffers(m_logicalDevice, m_cmdPool, static_cast<uint32_t>(m_cmdBuffers.size()), m_cmdBuffers.data() );  }

