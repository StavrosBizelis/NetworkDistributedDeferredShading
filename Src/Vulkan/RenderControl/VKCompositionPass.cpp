/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: VKCompositionPass.cpp
 *   DATE: Tue Jul 17 20:15:15 2018
 *  DESCR: 
 ***********************************************************************/
#include "Vulkan/RenderControl/VKCompositionPass.h"
#include <iostream>
#include "Vulkan/RenderControl/Pipelines/VKCompositionPipeline.h"
#include "Vulkan/Textures/VKTexture.h"
#include "Common/SceneControl/TexturedSceneNode.h"
#include "Vulkan/SceneControl/VKMeshSceneNode.h"

#include "Common/Core/MyUtilities.h"
#include "Vulkan/Core/VulkanUtilities.h"

/***********************************************************************
 *  Method: RenderControl::VKCompositionPass::VKCompositionPass
 *  Params: const glm::vec2 &a_resolution, SceneControl::SceneManager *a_scnManager, SceneControl::IShapeFactory *a_shapeFactory, const unsigned int &a_subparts
 * Effects: 
 ***********************************************************************/
RenderControl::VKCompositionPass::VKCompositionPass(const std::shared_ptr<VulkanLogicalDeviceManager>& a_device, const VkPhysicalDevice& a_physicalDevice, const std::shared_ptr<VulkanMemory>& a_memory,
                                                    const VkQueue& a_graphicsQueue, const VkQueue& a_presentQueue, const QueueFamilyIndices& a_indices,
                                                    const glm::vec2 &a_resolution, SceneControl::SceneManager *a_scnManager, IShapeFactory *a_shapeFactory, 
                                                    ITextureFactory* a_textFactory, const unsigned int &a_subparts)
  :ACompositionPass(a_resolution, a_scnManager, a_shapeFactory,a_textFactory, a_subparts),
    m_logicalDevice(a_device), m_physicalDevice(a_physicalDevice), m_memory(a_memory), m_graphicsQueue(a_graphicsQueue), m_presentQueue(a_presentQueue), m_indices(a_indices), m_currentFrame(0)
{
  
}


/***********************************************************************
 *  Method: RenderControl::VKCompositionPass::~VKCompositionPass
 *  Params: 
 * Effects: 
 ***********************************************************************/
RenderControl::VKCompositionPass::~VKCompositionPass()
{
  Clear();
}


/***********************************************************************
 *  Method: RenderControl::VKCompositionPass::SetMaterialManager
 *  Params: MaterialControl::IMaterialManager *a_materialManager
 * Returns: void
 * Effects: 
 ***********************************************************************/
void
RenderControl::VKCompositionPass::SetMaterialManager(MaterialControl::IMaterialManager *a_materialManager)
{
  ACompositionPass::SetMaterialManager(a_materialManager);
  
  // initiate materials
  m_material =
    a_materialManager->GetSingleTextureMaterial(
      "..\\Assets\\GLSL_shaders\\HUDImage.vert",
      "..\\Assets\\GLSL_shaders\\HUDImage.frag"
    );
  for( unsigned int i = 0; i < m_subpartRects.size(); ++i)
    m_subpartRects[i]->SetMaterial(m_material);
}


bool RenderControl::VKCompositionPass::Init()
{
  vkAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR) vkGetDeviceProcAddr(m_logicalDevice->GetDevice(), "vkAcquireNextImageKHR");
  vkQueuePresentKHR = (PFN_vkQueuePresentKHR) vkGetDeviceProcAddr(m_logicalDevice->GetDevice(), "vkQueuePresentKHR");
  if( !vkAcquireNextImageKHR || !vkQueuePresentKHR)
    throw std::runtime_error("VKCompositionPass::Init - failed to load presentation functions!");

  CreateCommandPool();
  CreateSemaphores();
  CreateRenderPass();
  CreateFramebuffer();
  CreatePipelines();
  CreateDescriptorPool();
  CreateCommandBuffers();
  
  m_camera->SetOrthographicProjectionMatrix((int)m_resolution.x, (int)m_resolution.y);

  m_uboMemBuffer = m_memory->CreateUniformBuffer( sizeof(VertexViewProjMatrices) );
  
  std::cout << "m_subpartRects.size() " << m_subpartRects.size() << std::endl;
  // add the rectancles to the appropriate pipelines
  for( unsigned int i = 0; i < m_subpartRects.size(); ++i)
  {    
    std::cout << "RECORD RECT" << reinterpret_cast<VKShape*>( reinterpret_cast<VulkanRenderable*>( m_subpartRects[i]->GetExtra() )->GetShape()->GetExtra() ) << "\n";
    CreateDescriptorSet(m_pipelines[0], m_subpartRects[i]);
    std::vector< std::shared_ptr<VulkanSecondaryCommandBuffer> > l_cmdBuffers;
    l_cmdBuffers = m_pipelines[0]->GetSecondaryCommandBuffers();
    
    l_cmdBuffers[0]->AddMesh( reinterpret_cast<VulkanRenderable*>( m_subpartRects[i]->GetExtra() )  );
  }
  
  return true;
}

void RenderControl::VKCompositionPass::Render()
{
  uint32_t l_imageIndex;
  VkResult result = vkAcquireNextImageKHR(m_logicalDevice->GetDevice(), m_logicalDevice->GetSwapChain(), std::numeric_limits<uint64_t>::max(), m_imageAvailableSemaphore[m_currentFrame], VK_NULL_HANDLE, &l_imageIndex);
  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
       std::cout << "VKCompositionPass::Render() - requires reinitialization\n";
      // ReInit();
      // return;
  }
  else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("VKCompositionPass::Render() - failed to acquire swap chain image!");
  }

  
  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphore[m_currentFrame] };
  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;


  m_primaryCmdBuffer->Update();
  
  submitInfo.commandBufferCount = 1;
  VkCommandBuffer l_cmdBuffer[] = { m_primaryCmdBuffer->GetNextCommandBufferHandle() };
  submitInfo.pCommandBuffers = l_cmdBuffer;

  VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphore[m_currentFrame] };
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
    throw std::runtime_error("VKCompositionPass::Render() - failed to submit draw command buffer!");

  VkPresentInfoKHR presentInfo = {};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {m_logicalDevice->GetSwapChain()};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;

  presentInfo.pImageIndices = &l_imageIndex;

  result = vkQueuePresentKHR(m_presentQueue, &presentInfo);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) 
  {
      //ReInit();
    std::cout << "VKCompositionPass::Render() - requires reinitialization\n";
  } 
  else if (result != VK_SUCCESS) 
    throw std::runtime_error("VKCompositionPass::Render() - failed to present swap chain image!");
  
  
  // vkQueueWaitIdle(m_presentQueue);
  m_currentFrame = (m_currentFrame + 1) % m_maxFramesInFlight;
}

void RenderControl::VKCompositionPass::OutputOnScreen(){}


void RenderControl::VKCompositionPass::Clear()
{
if( m_descriptorPool )
    vkDestroyDescriptorPool(m_logicalDevice->GetDevice(), m_descriptorPool, nullptr);
  
  for( unsigned int i = 0; i <m_frameBuffers.size(); ++i)
    if(m_frameBuffers[i])
      vkDestroyFramebuffer(m_logicalDevice->GetDevice(), m_frameBuffers[i], nullptr);
  
  if(m_renderPass)
    vkDestroyRenderPass(m_logicalDevice->GetDevice(), m_renderPass, nullptr);
}

void RenderControl::VKCompositionPass::VulkanUpdate( char* a_mappedBuffer )
{
  // copy global camera matrices and screen resolution for lights fragments shaders
  m_globalsUbo.projMatrix = *m_camera->GetOrthographicProjectionMatrix();
  m_globalsUbo.viewMatrix = glm::mat4(); // always identity matrix
  
  memcpy(a_mappedBuffer+m_uboMemBuffer->GetMemoryOffset(), &m_globalsUbo, sizeof(VertexViewProjMatrices) );
}


void RenderControl::VKCompositionPass::CreateCommandPool() 
{
  VkCommandPoolCreateInfo poolInfo = {};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.queueFamilyIndex = m_indices.m_graphicsFamily;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

  if (vkCreateCommandPool(m_logicalDevice->GetDevice(), &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
    throw std::runtime_error("failed to create command pool!");
}

void RenderControl::VKCompositionPass::CreateSemaphores()
{
  VkSemaphoreCreateInfo semaphoreInfo = {};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  m_imageAvailableSemaphore.resize(m_maxFramesInFlight);
  m_renderFinishedSemaphore.resize(m_maxFramesInFlight);
  for( unsigned int i = 0; i < m_maxFramesInFlight; ++i)
  {
    vkCreateSemaphore(m_logicalDevice->GetDevice(), &semaphoreInfo, nullptr, &m_imageAvailableSemaphore[i]);
    vkCreateSemaphore(m_logicalDevice->GetDevice(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphore[i]);
  }
  
}

void RenderControl::VKCompositionPass::CreateRenderPass()
{
  VkAttachmentDescription colorAttachment = {};
  colorAttachment.format = m_logicalDevice->GetSwapChainImageFormat();
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  
  
  VkAttachmentReference colorAttachmentRef = {};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  
  VkSubpassDescription subpass = {};
  subpass.inputAttachmentCount = 0;
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;

  
  VkSubpassDependency l_dependency1;
  l_dependency1.srcSubpass = VK_SUBPASS_EXTERNAL;
  l_dependency1.dstSubpass = 0;
  l_dependency1.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  l_dependency1.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  l_dependency1.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  l_dependency1.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  l_dependency1.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
  
  VkSubpassDependency l_dependency2;
  l_dependency2.srcSubpass = 0;
  l_dependency2.dstSubpass = VK_SUBPASS_EXTERNAL;
  l_dependency2.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  l_dependency2.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  l_dependency2.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  l_dependency2.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  l_dependency2.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

  
  std::vector< VkSubpassDependency> l_dependencies = {l_dependency1, l_dependency2};
  
  std::vector<VkAttachmentDescription> attachments = {colorAttachment};
  VkRenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = attachments.size();
  renderPassInfo.pAttachments = attachments.data();
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = l_dependencies.size();
  renderPassInfo.pDependencies = l_dependencies.data();
  
  
  if (vkCreateRenderPass(m_logicalDevice->GetDevice(), &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
      throw std::runtime_error("failed to create render pass!");
  }
}

void RenderControl::VKCompositionPass::CreateFramebuffer()
{
  
  // create frame buffer
  m_frameBuffers.resize(m_logicalDevice->GetSwapChainImageViews().size());
  m_swapChainImageViews = m_logicalDevice->GetSwapChainImageViews();

  size_t l_count = m_swapChainImageViews.size();
  for (size_t i = 0; i < l_count; i++) 
  {
    VkImageView attachments[] = {
        m_swapChainImageViews[i]
    };

    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = m_renderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = m_resolution.x;
    framebufferInfo.height = m_resolution.y;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(m_logicalDevice->GetDevice(), &framebufferInfo, nullptr, &m_frameBuffers[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer!");
    }
  }
}

void RenderControl::VKCompositionPass::CreatePipelines()
{

  // geometry shader shaders
  VkShaderModule l_vertex = CreateShaderModule(ReadFile("..\\Assets\\SPV_shaders\\HUDImage.vert.spv"), m_logicalDevice->GetDevice());
  VkShaderModule l_frag = CreateShaderModule(ReadFile("..\\Assets\\SPV_shaders\\HUDImage.frag.spv"), m_logicalDevice->GetDevice());
  
  m_pipelines = std::vector< std::shared_ptr<VKPipeline> >(1);
  // simple geometry 
  m_pipelines[0] = std::make_shared<VKCompositionPipeline>(m_logicalDevice, m_renderPass, CreatePipelineShaderCreateInfo(l_vertex, l_frag), 0, m_resolution, glm::vec4(0,0,m_resolution.x,m_resolution.y), 0 );
  m_pipelines[0]->Init();
  
  // destroy shader modules
  vkDestroyShaderModule(m_logicalDevice->GetDevice(), l_vertex, nullptr);
  vkDestroyShaderModule(m_logicalDevice->GetDevice(), l_frag, nullptr);
  
  
  std::shared_ptr<VulkanSecondaryCommandBuffer> l_secondaryCmdBufferTmp;
  for( unsigned int i = 0; i < m_pipelines.size(); ++i)
  {
    l_secondaryCmdBufferTmp = std::make_shared<VulkanSecondaryCommandBuffer>(m_logicalDevice->GetDevice(), m_commandPool, m_pipelines[i]->GetPipelineLayout(), m_renderPass, i);
    l_secondaryCmdBufferTmp->Init();
    m_pipelines[i]->AddSecondaryBuffer(l_secondaryCmdBufferTmp);
  }
}

void RenderControl::VKCompositionPass::CreateDescriptorPool()
{
  VkDescriptorPoolSize l_poolSize1 = {};
  l_poolSize1.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  l_poolSize1.descriptorCount = 256;

  VkDescriptorPoolSize l_poolSize2 = {};
  l_poolSize2.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  l_poolSize2.descriptorCount = 256;

  std::vector<VkDescriptorPoolSize> l_poolSizes = {l_poolSize1, l_poolSize2};
  
  VkDescriptorPoolCreateInfo l_poolInfo = {};
  l_poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  l_poolInfo.poolSizeCount = l_poolSizes.size();
  l_poolInfo.pPoolSizes = l_poolSizes.data();
  l_poolInfo.maxSets = 128;

  if (vkCreateDescriptorPool(m_logicalDevice->GetDevice(), &l_poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
    throw std::runtime_error("VKCompositionPass::CreateDescriptorPool() - failed to create descriptor pool!");
  }
}

void RenderControl::VKCompositionPass::CreateCommandBuffers()
{
  m_primaryCmdBuffer = std::shared_ptr<VulkanPrimaryCommandBuffer>( new VulkanPrimaryCommandBuffer(m_logicalDevice->GetDevice(), m_commandPool, m_frameBuffers, m_renderPass, m_resolution) );
  m_primaryCmdBuffer->Init();
  for( auto l_pipeline : m_pipelines )
    m_primaryCmdBuffer->AddPipeline(l_pipeline);
}

void RenderControl::VKCompositionPass::CreateDescriptorSet(const std::shared_ptr<VKPipeline>& a_pipeline, IRenderable* a_renderable)
{
  VkDescriptorSetLayout l_layout = a_pipeline->GetDescriptorSetLayout();
  VkDescriptorSetAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = m_descriptorPool;
  allocInfo.descriptorSetCount = 1;
  allocInfo.pSetLayouts = &l_layout;

  VkDescriptorSet l_descSet;
  if (vkAllocateDescriptorSets(m_logicalDevice->GetDevice(), &allocInfo, &l_descSet ) != VK_SUCCESS) {
    throw std::runtime_error("VKCompositionPass::CreateDescriptorSet() - failed to allocate descriptor sets!");
  }
  
  // personal ubos here
  std::vector<size_t> l_uboSizes = a_pipeline->GetObjUboSizes();
  std::vector< std::shared_ptr<VulkanMemoryChunk> > l_uboMemBuffer = {nullptr,nullptr};
  if( l_uboSizes.size() > 0 )
    l_uboMemBuffer[0] = m_memory->CreateUniformBuffer( l_uboSizes[0] );
  if( l_uboSizes.size() > 1 )
    l_uboMemBuffer[1] = m_memory->CreateUniformBuffer( l_uboSizes[1] );
  
  reinterpret_cast<VulkanRenderable*>( a_renderable->GetExtra() )->Init(l_descSet, l_uboMemBuffer[0], l_uboMemBuffer[1] );
  
  
  // get appropriate size of ubo from pipeline - 
  // and also need to know if this pipeline requires any type of global data and what type that is ( ex. VertexSingleMat4, FragDirLightGlobalVars )
  // also need to know what kind of samplers does this pipeline requires and what type( input attachments or combined image samplers)
  unsigned int l_uboIndex = 0;
  unsigned int l_imagesIndex = 0;
  unsigned int l_inputAttachmentsIndex = 0;
  std::vector<SceneGlobalDataType> l_globals = a_pipeline->GetGlobalDataTypes();
  unsigned int l_globalsToGo = a_pipeline->GetGlobalDataTypes().size();
  
  std::vector<VkDescriptorSetLayoutBinding> l_bindings = a_pipeline->GetDescriptorSetLayoutBindings();
  
  // first the global 
  VkDescriptorBufferInfo l_bufferInfo = {};
  l_bufferInfo.buffer = m_uboMemBuffer->m_buffer->m_buffer;
  l_bufferInfo.offset = m_uboMemBuffer->GetBufferOffset();
  l_bufferInfo.range = m_uboMemBuffer->m_size;
  
  
  VkWriteDescriptorSet descriptorWrite = {};
  descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrite.dstSet = l_descSet;
  descriptorWrite.dstBinding = l_bindings[0].binding;
  descriptorWrite.dstArrayElement = 0;
  descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptorWrite.descriptorCount = 1;
  descriptorWrite.pBufferInfo = &l_bufferInfo;
  
  

  
  VkDescriptorBufferInfo l_bufferInfo2 = {};
  l_bufferInfo2.buffer = l_uboMemBuffer[0]->m_buffer->m_buffer;
  l_bufferInfo2.offset = l_uboMemBuffer[0]->GetBufferOffset();
  l_bufferInfo2.range = l_uboMemBuffer[0]->m_size;
  
  
  VkWriteDescriptorSet descriptorWrite2 = {};
  descriptorWrite2.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrite2.dstSet = l_descSet;
  descriptorWrite2.dstBinding = l_bindings[1].binding;
  descriptorWrite2.dstArrayElement = 0;
  descriptorWrite2.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptorWrite2.descriptorCount = 1;
  descriptorWrite2.pBufferInfo = &l_bufferInfo2;
  
  
  std::vector< VkWriteDescriptorSet > l_descriptorSetWrites = {descriptorWrite, descriptorWrite2 };
  

  SceneControl::TexturedSceneNode* l_texturedSceneNode = dynamic_cast<SceneControl::TexturedSceneNode*>(a_renderable);
  std::shared_ptr<ITexture> l_texture = l_texturedSceneNode->GetTexture(0);
  
  VkDescriptorImageInfo l_imageInfo = {};
  VkWriteDescriptorSet descriptorWrite3 = {};
  if( l_texture )
  {
    VKATexture* l_tempTexture = reinterpret_cast<VKATexture*>( l_texture->GetExtra() );
    if( l_tempTexture )
    {      
      l_imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;   // VkImageLayout
      l_imageInfo.sampler = l_tempTexture->GetSampler()->m_sampler; // VkSampler                      
      l_imageInfo.imageView = l_tempTexture->GetImage()->m_imageView;  // VkImageView
      
      
      descriptorWrite3.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrite3.dstSet = l_descSet;
      descriptorWrite3.dstBinding = l_bindings[2].binding;
      descriptorWrite3.dstArrayElement = 0;
      descriptorWrite3.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      descriptorWrite3.descriptorCount = 1;
      descriptorWrite3.pImageInfo = &l_imageInfo;
      l_descriptorSetWrites.push_back(descriptorWrite3);
    }
  }
 
  
  vkUpdateDescriptorSets(m_logicalDevice->GetDevice(), l_descriptorSetWrites.size(), l_descriptorSetWrites.data(), 0, nullptr);
}
