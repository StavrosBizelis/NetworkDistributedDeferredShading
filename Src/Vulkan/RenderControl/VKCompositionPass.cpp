/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: VKCompositionPass.cpp
 *   DATE: Tue Jul 17 20:15:15 2018
 *  DESCR: 
 ***********************************************************************/
#include "Vulkan/RenderControl/VKCompositionPass.h"
#include <iostream>
#include "Vulkan/RenderControl/Pipelines/VKCompositionPipeline.h"

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
    m_logicalDevice(a_device), m_physicalDevice(a_physicalDevice), m_memory(a_memory), m_graphicsQueue(a_graphicsQueue), m_presentQueue(a_presentQueue), m_indices(a_indices)
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
  
  CreateCommandPool();
  CreateSemaphores();
  CreateRenderPass();
  CreateFramebuffer();
  CreatePipelines();
  CreateDescriptorPool();
  CreateCommandBuffers();
  
  m_camera->SetOrthographicProjectionMatrix((int)m_resolution.x, (int)m_resolution.y);

  m_uboMemBuffer = m_memory->CreateUniformBuffer( sizeof(VertexViewProjMatrices) );
  
  return true;
}

void RenderControl::VKCompositionPass::Render()
{
  // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
  // glDrawBuffer(GL_COLOR_ATTACHMENT0 );
  // glViewport(0, 0, (GLsizei)m_resolution.x, (GLsizei)m_resolution.y);

  // glutil::MatrixStack l_matrixStack;
  // l_matrixStack.SetIdentity();
  
  // glDisable(GL_DEPTH_TEST);
  
  // m_material->UseProgram();
  // m_material->SetUniform("matrices.projMatrix", m_camera->GetOrthographicProjectionMatrix() );
  
  // for( unsigned int i =0; i < m_subpartRects.size(); ++i)
    // m_subpartRects[i]->Render(l_matrixStack);
  
  // glEnable(GL_DEPTH_TEST);
}

void RenderControl::VKCompositionPass::OutputOnScreen()
{
  // unsigned int l_actualAttachment = GL_COLOR_ATTACHMENT0;
  // unsigned int l_actualBit = GL_COLOR_BUFFER_BIT;


  // /* We are going to blit into the window (default framebuffer)                     */
  // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  // glDrawBuffer(GL_BACK);              /* Use backbuffer as color dst.         */
  // //glClearColor(0.f, 0.f, 0.f, 0.0f);
  // //glClearDepth(1.0f);
  // //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  // /* Read from your FBO */
  // glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
  // glReadBuffer(l_actualAttachment); /* Use Color Attachment n as color src. */

                    // /* Copy the color and depth buffer from your FBO to the default framebuffer       */
  // glBlitFramebuffer(0, 0, (GLint)m_resolution.x, (GLint)m_resolution.y,
    // 0, 0, (GLint)m_resolution.x, (GLint)m_resolution.y,
    // l_actualBit, GL_LINEAR);
  // // return to default 
  // glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}


void RenderControl::VKCompositionPass::Clear()
{
if( m_descriptorPool )
    vkDestroyDescriptorPool(m_logicalDevice->GetDevice(), m_descriptorPool, nullptr);
  
  if(m_frameBuffer)
    vkDestroyFramebuffer(m_logicalDevice->GetDevice(), m_frameBuffer, nullptr);
  
  if(m_renderPass)
    vkDestroyRenderPass(m_logicalDevice->GetDevice(), m_renderPass, nullptr);
}

void RenderControl::VKCompositionPass::VulkanUpdate( char* a_mappedBuffer )
{
  
  m_primaryCmdBuffer->Update();
  
  // copy global camera matrices and screen resolution for lights fragments shaders
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

  if (vkCreateSemaphore(m_logicalDevice->GetDevice(), &semaphoreInfo, nullptr, &m_imageAvailableSemaphore) != VK_SUCCESS ||
      vkCreateSemaphore(m_logicalDevice->GetDevice(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphore) != VK_SUCCESS) 
  {
    throw std::runtime_error("failed to create semaphores!");
  }
}

void RenderControl::VKCompositionPass::CreateRenderPass()
{
  // init attachments
  // create the attachments image
  m_swapChainImages = m_logicalDevice->GetSwapChainImages();
  m_swapChainImageViews = m_logicalDevice->GetSwapChainImageViews();
  if( m_swapChainImages.size() != m_swapChainImageViews.size() )
      throw std::runtime_error("swap chain images and image views not of the same size");

  
  // create attachments
  std::vector< VkAttachmentDescription > l_attachments(m_swapChainImages.size());
  for( unsigned int i = 0; i < m_swapChainImages.size(); ++i)
  {
    l_attachments[i].format = m_logicalDevice->GetSwapChainImageFormat();
    l_attachments[i].samples = VK_SAMPLE_COUNT_1_BIT;
    l_attachments[i].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    l_attachments[i].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    l_attachments[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    l_attachments[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    
    l_attachments[i].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    l_attachments[i].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // dont care since they are not going to be used for anything else
  }

  // create attachment references 
  // geometry sub pass references
  std::vector <VkAttachmentReference> l_colourAttachmentRefs(1);
  for( unsigned int i = 0; i < l_colourAttachmentRefs.size(); ++i)
  {
    l_colourAttachmentRefs[i].attachment = i;
    l_colourAttachmentRefs[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  }

  
  // create subpasses
  VkSubpassDescription l_subpass;
  l_subpass.inputAttachmentCount = 0;
  l_subpass.colorAttachmentCount = l_colourAttachmentRefs.size();
  l_subpass.pColorAttachments = l_colourAttachmentRefs.data();
  l_subpass.pDepthStencilAttachment = (VkAttachmentReference*)VK_ATTACHMENT_UNUSED;


  std::vector< VkSubpassDependency> l_dependencies(2);
  
  
  l_dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
  l_dependencies[0].dstSubpass = 0;
  l_dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  l_dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  l_dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  l_dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  l_dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
  
  l_dependencies[1].srcSubpass = 0;
  l_dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
  l_dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  l_dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  l_dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  l_dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  l_dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

  

  
  VkRenderPassCreateInfo l_renderPassInfo = {};
  l_renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  l_renderPassInfo.attachmentCount = l_attachments.size();
  l_renderPassInfo.pAttachments = l_attachments.data();
  l_renderPassInfo.subpassCount = 1;
  l_renderPassInfo.pSubpasses = &l_subpass;
  l_renderPassInfo.dependencyCount = l_dependencies.size();
  l_renderPassInfo.pDependencies = l_dependencies.data();
  
  if (vkCreateRenderPass(m_logicalDevice->GetDevice(), &l_renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) 
    throw std::runtime_error("VKCompositionPass::Init() - failed to create render pass!");
}

void RenderControl::VKCompositionPass::CreateFramebuffer()
{
    // create frame buffer
    m_swapChainImageViews = m_logicalDevice->GetSwapChainImageViews();
    
    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = m_renderPass;
    framebufferInfo.attachmentCount = m_swapChainImageViews.size();
    framebufferInfo.pAttachments = m_swapChainImageViews.data();
    framebufferInfo.width = m_resolution.x;
    framebufferInfo.height = m_resolution.y;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(m_logicalDevice->GetDevice(), &framebufferInfo, nullptr, &m_frameBuffer ) != VK_SUCCESS)
      throw std::runtime_error("VKCompositionPass::Init() - failed to create framebuffer!");
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
  l_poolInfo.pPoolSizes = &l_poolSizes[0];
  l_poolInfo.maxSets = 128;

  if (vkCreateDescriptorPool(m_logicalDevice->GetDevice(), &l_poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
    throw std::runtime_error("VKCompositionPass::CreateDescriptorPool() - failed to create descriptor pool!");
  }
}

void RenderControl::VKCompositionPass::CreateCommandBuffers()
{
  m_primaryCmdBuffer = std::shared_ptr<VulkanPrimaryCommandBuffer>( new VulkanPrimaryCommandBuffer(m_logicalDevice->GetDevice(), m_commandPool, {m_frameBuffer}, m_renderPass, m_resolution) );
  m_primaryCmdBuffer->Init();
  for( auto l_pipeline : m_pipelines )
    m_primaryCmdBuffer->AddPipeline(l_pipeline);
}

void RenderControl::VKCompositionPass::CreateDescriptorSet(const std::shared_ptr<VKPipeline>& a_pipeline, VulkanRenderable* a_renderable)
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
  a_renderable->Init(l_descSet, l_uboMemBuffer[0], l_uboMemBuffer[1] );
  
  
  std::vector< VkWriteDescriptorSet > l_descriptorSetWrites;
  // get appropriate size of ubo from pipeline - 
  // and also need to know if this pipeline requires any type of global data and what type that is ( ex. VertexSingleMat4, FragDirLightGlobalVars )
  // also need to know what kind of samplers does this pipeline requires and what type( input attachments or combined image samplers)
  unsigned int l_uboIndex = 0;
  unsigned int l_imagesIndex = 0;
  unsigned int l_inputAttachmentsIndex = 0;
  std::vector<SceneGlobalDataType> l_globals = a_pipeline->GetGlobalDataTypes();
  unsigned int l_globalsToGo = a_pipeline->GetGlobalDataTypes().size();
  // descriptor layout bindings are always global first, then object bindings
  std::vector<VkDescriptorSetLayoutBinding> l_bindings = a_pipeline->GetDescriptorSetLayoutBindings();
  for( unsigned int i =0; i < l_bindings.size(); ++i)
  {
    if( l_bindings[i].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER )
    {
      // first handle global uniform buffers then the object ones
      VkDescriptorBufferInfo l_bufferInfo = {};
      if( l_globalsToGo == 0)
      {
        l_bufferInfo.buffer = l_uboMemBuffer[l_uboIndex]->m_buffer->m_buffer;
        l_bufferInfo.offset = l_uboMemBuffer[l_uboIndex]->GetBufferOffset();
        l_bufferInfo.range = l_uboMemBuffer[l_uboIndex]->m_size;
        ++l_uboIndex;
      }
      else
      {
        --l_globalsToGo;
        if( l_globals[l_globalsToGo] == GLOBAL_PROJ_VIEW_MATRIX )
        {
          l_bufferInfo.buffer = m_uboMemBuffer->m_buffer->m_buffer;
          l_bufferInfo.offset = m_uboMemBuffer->GetBufferOffset();
          l_bufferInfo.range = m_uboMemBuffer->m_size;
        }
      }
      VkWriteDescriptorSet descriptorWrite = {};
      descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrite.dstSet = l_descSet;
      descriptorWrite.dstBinding = l_bindings[i].binding;
      descriptorWrite.dstArrayElement = 0;
      descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      descriptorWrite.descriptorCount = 1;
      descriptorWrite.pBufferInfo = &l_bufferInfo;
      
      l_descriptorSetWrites.push_back(descriptorWrite);
    }
    else if( l_bindings[i].descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER )
    {
      std::shared_ptr<VKATexture> l_texture = a_renderable->GetVKTexture(l_imagesIndex);
      
      VkDescriptorImageInfo l_imageInfo = {};
      if( l_texture )
      {
        l_imageInfo.sampler = l_texture->GetSampler()->m_sampler; // VkSampler                      
        l_imageInfo.imageView = l_texture->GetImage()->m_imageView;  // VkImageView
        l_imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;   // VkImageLayout
      }
      else
        continue;
        

      VkWriteDescriptorSet descriptorWrite = {};
      descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrite.dstSet = l_descSet;
      descriptorWrite.dstBinding = l_bindings[i].binding;
      descriptorWrite.dstArrayElement = 0;
      descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      descriptorWrite.descriptorCount = 1;
      descriptorWrite.pBufferInfo = nullptr;
      descriptorWrite.pImageInfo = &l_imageInfo;
      
      l_descriptorSetWrites.push_back(descriptorWrite);
      l_imagesIndex++;
    }
    
  }
  
  vkUpdateDescriptorSets(m_logicalDevice->GetDevice(), l_descriptorSetWrites.size(), l_descriptorSetWrites.data(), 0, nullptr);
}
