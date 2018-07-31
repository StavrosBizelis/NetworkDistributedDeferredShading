/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: VKDeferredShadingPass.cpp
 *   DATE: Tue Jul 17 20:15:15 2018
 *  DESCR: 
 ***********************************************************************/
#include "Vulkan/RenderControl/VKDeferredShadingPass.h"
#include <iostream>
#include "Vulkan/RenderControl/Pipelines/VKCompositionPipeline.h"
#include "Vulkan/RenderControl/Pipelines/VKDeferredPipelines.h"
#include "Vulkan/Textures/VKTexture.h"
#include "Common/SceneControl/TexturedSceneNode.h"
#include "Vulkan/SceneControl/VKMeshSceneNode.h"

#include "Common/Core/MyUtilities.h"
#include "Vulkan/Core/VulkanUtilities.h"
#include "freeimage/include/FreeImage.h"
/***********************************************************************
 *  Method: RenderControl::VKDeferredShadingPass::VKDeferredShadingPass
 *  Params: const glm::vec2 &a_resolution, SceneControl::SceneManager *a_scnManager, SceneControl::IShapeFactory *a_shapeFactory, const unsigned int &a_subparts
 * Effects: 
 ***********************************************************************/
RenderControl::VKDeferredShadingPass::VKDeferredShadingPass(const std::shared_ptr<VulkanLogicalDeviceManager>& a_device, const VkPhysicalDevice& a_physicalDevice, const std::shared_ptr<VulkanMemory>& a_memory,
                                                    const VkQueue& a_graphicsQueue, const VkQueue& a_presentQueue, const QueueFamilyIndices& a_indices,
                                                    const glm::vec2 &a_resolution, SceneControl::SceneManager *a_scnManager, IShapeFactory *a_shapeFactory, 
                                                    ITextureFactory* a_textFactory, const unsigned int &a_subparts)
  :ADeferredShadingPass(a_resolution, a_resolution, glm::vec4(0,0,a_resolution.x,a_resolution.y) ),
    m_logicalDevice(a_device), m_physicalDevice(a_physicalDevice), m_memory(a_memory), m_graphicsQueue(a_graphicsQueue), m_presentQueue(a_presentQueue), m_indices(a_indices), m_currentFrame(0), 
    m_scnManager(a_scnManager)
{
  
  
  std::cout<< a_resolution.x << " " << a_resolution.y << " " << a_subparts << std::endl;
  
  std::shared_ptr<IMesh> l_rec = a_shapeFactory->GetOpenAssetImportMesh("../Assets/Models/Asteroid/asteroid.obj");
  for( unsigned int i =0; i < a_subparts; ++i)
  {
    std::shared_ptr<ITexture> l_text = a_textFactory->GetTexture("../Assets/Models/Asteroid/diffuse.png");
    std::shared_ptr<ITexture> l_text2 = a_textFactory->GetTexture("../Assets/Models/Asteroid/normal.png");
    // std::shared_ptr<ITexture> l_text = a_textFactory->GetTexture("..\\Assets\\Skybox\\spacebox\\DX+.jpg");
    m_subpartRects.push_back( m_scnManager->AddMeshSceneNode(l_rec) );
    m_subpartRects.back()->SetTexture(0,l_text);
    m_subpartRects.back()->SetTexture(1,l_text2);
    m_subpartRects.back()->SetTexture(2,l_text);
    m_subpartRects.back()->SetTexture(3,l_text);
    m_subpartRects.back()->SetTexture(4,l_text);
    
    m_subpartRects.push_back( m_scnManager->AddMeshSceneNode(l_rec) );
    m_subpartRects.back()->SetTexture(0,l_text);
    m_subpartRects.back()->SetTexture(1,l_text2);
    m_subpartRects.back()->SetTexture(2,l_text);
    m_subpartRects.back()->SetTexture(3,l_text);
    m_subpartRects.back()->SetTexture(4,l_text);
    
  }
  
  m_subpartRects[0]->SetPos( glm::vec3( -3, 0, -10 ) );
  m_subpartRects[0]->SetScale( glm::vec3( 2, 2, 1 ) );
  m_subpartRects[0]->SetEulerAngles( glm::vec3( 0, 0, 0 ) );
  
  m_subpartRects[1]->SetPos( glm::vec3( 0, 0, -10 ) );
  m_subpartRects[1]->SetScale( glm::vec3( 2, 2, 1 ) );
  m_subpartRects[1]->SetEulerAngles( glm::vec3( 0, 0, 0 ) );
}


/***********************************************************************
 *  Method: RenderControl::VKDeferredShadingPass::~VKDeferredShadingPass
 *  Params: 
 * Effects: 
 ***********************************************************************/
RenderControl::VKDeferredShadingPass::~VKDeferredShadingPass()
{
  Clear();
}


/***********************************************************************
 *  Method: RenderControl::VKDeferredShadingPass::SetMaterialManager
 *  Params: MaterialControl::IMaterialManager *a_materialManager
 * Returns: void
 * Effects: 
 ***********************************************************************/
void
RenderControl::VKDeferredShadingPass::SetMaterialManager(MaterialControl::IMaterialManager *a_materialManager)
{
  ADeferredShadingPass::SetMaterialManager(a_materialManager);

}


bool RenderControl::VKDeferredShadingPass::Init()
{
  vkAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR) vkGetDeviceProcAddr(m_logicalDevice->GetDevice(), "vkAcquireNextImageKHR");
  vkQueuePresentKHR = (PFN_vkQueuePresentKHR) vkGetDeviceProcAddr(m_logicalDevice->GetDevice(), "vkQueuePresentKHR");
  if( !vkAcquireNextImageKHR || !vkQueuePresentKHR)
    throw std::runtime_error("VKDeferredShadingPass::Init - failed to load presentation functions!");

  CreateCommandPool();
  CreateSemaphores();
  CreateRenderPass();
  CreateFramebuffer();
  CreatePipelines();
  CreateDescriptorPool();
  CreateCommandBuffers();
  
  m_camera->SetOrthographicProjectionMatrix((int)m_resolution.x, (int)m_resolution.y);
  m_camera->SetPerspectiveProjectionMatrix(45.0f, m_resolution.x / m_resolution.y, 0.5f, 7000.0f);;

  m_uboMemBuffer = m_memory->CreateUniformBuffer( sizeof(VertexViewProjMatrices) );
  
  std::cout << "m_subpartRects.size() " << m_subpartRects.size() << std::endl;
  std::cout << "m_pipelines.size() " << m_pipelines.size() << std::endl;
  // add the rectancles to the appropriate pipelines
  
  std::vector< std::shared_ptr<VulkanSecondaryCommandBuffer> > l_cmdBuffers;
  
  CreateDescriptorSet(m_pipelines[6], m_subpartRects[0]);
  l_cmdBuffers = m_pipelines[6]->GetSecondaryCommandBuffers();
  l_cmdBuffers[0]->AddMesh( reinterpret_cast<VulkanRenderable*>( m_subpartRects[0]->GetExtra() )  );
  m_subpartRects[0]->SetPersistentUniform(0,"UEmissive", glm::vec4(1) );
  
  
  CreateDescriptorSet(m_pipelines[0], m_subpartRects[1]);
  l_cmdBuffers = m_pipelines[0]->GetSecondaryCommandBuffers();
  l_cmdBuffers[0]->AddMesh( reinterpret_cast<VulkanRenderable*>( m_subpartRects[1]->GetExtra() )  );
  
  return true;
}

void RenderControl::VKDeferredShadingPass::Render()
{
  // uint32_t l_imageIndex;
  // VkResult result = vkAcquireNextImageKHR(m_logicalDevice->GetDevice(), m_logicalDevice->GetSwapChain(), std::numeric_limits<uint64_t>::max(), m_imageAvailableSemaphore[m_currentFrame], VK_NULL_HANDLE, &l_imageIndex);
  // if (result == VK_ERROR_OUT_OF_DATE_KHR) {
       // std::cout << "VKDeferredShadingPass::Render() - requires reinitialization\n";
      // // ReInit();
      // // return;
  // }
  // else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    // throw std::runtime_error("VKDeferredShadingPass::Render() - failed to acquire swap chain image!");
  // }
  
  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  // VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphore[m_currentFrame] };
  // VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  // submitInfo.waitSemaphoreCount = 1;
  // submitInfo.pWaitSemaphores = waitSemaphores;
  // submitInfo.pWaitDstStageMask = waitStages;


  m_primaryCmdBuffer->Update();
  
  submitInfo.commandBufferCount = 1;
  VkCommandBuffer l_cmdBuffer[] = { m_primaryCmdBuffer->GetNextCommandBufferHandle() };
  submitInfo.pCommandBuffers = l_cmdBuffer;

  // VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphore[m_currentFrame] };
  // submitInfo.signalSemaphoreCount = 1;
  // submitInfo.pSignalSemaphores = signalSemaphores;

  VkResult l_res = vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
  if ( l_res != VK_SUCCESS )
  {  
    std::cout << "Test fail " << l_res << std::endl;
    throw std::runtime_error("VKDeferredShadingPass::Render() - failed to submit draw command buffer!");
  }
  
  // VkPresentInfoKHR presentInfo = {};
  // presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  // presentInfo.waitSemaphoreCount = 1;
  // presentInfo.pWaitSemaphores = signalSemaphores;

  // VkSwapchainKHR swapChains[] = {m_logicalDevice->GetSwapChain()};
  // presentInfo.swapchainCount = 1;
  // presentInfo.pSwapchains = swapChains;

  // presentInfo.pImageIndices = &l_imageIndex;

  // result = vkQueuePresentKHR(m_presentQueue, &presentInfo);

  // if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) 
  // {
      // //ReInit();
    // std::cout << "VKDeferredShadingPass::Render() - requires reinitialization\n";
  // } 
  // else if (result != VK_SUCCESS) 
    // throw std::runtime_error("VKDeferredShadingPass::Render() - failed to present swap chain image!");
  
  
  // vkQueueWaitIdle(m_presentQueue);
  vkQueueWaitIdle(m_graphicsQueue);
  m_currentFrame = (m_currentFrame + 1) % m_maxFramesInFlight;
  
  
  
  
  // write attachment to file
  unsigned int l_indexToWrite = 3;
  std::shared_ptr<VulkanMemoryChunk> l_buf = m_memory->CreateBufferFromImage( m_attachmentImages[l_indexToWrite] );
  FIBITMAP* bitmap = FreeImage_Allocate(m_attachmentImages[l_indexToWrite]->m_width, m_attachmentImages[l_indexToWrite]->m_height, 24);
  RGBQUAD color;
  
  
  void* data;
  vkMapMemory(m_logicalDevice->GetDevice(), l_buf->m_memorySpace, l_buf->GetMemoryOffset(), l_buf->m_size, 0, &data);
      char* l_curr = (char*)data;
  
  for( int i = 0; i < m_attachmentImages[l_indexToWrite]->m_height; ++i)
    for( int j = 0; j < m_attachmentImages[l_indexToWrite]->m_width; ++j)
    {
      color.rgbRed = *l_curr; l_curr++;
      color.rgbGreen = *l_curr; l_curr++;
      color.rgbBlue = *l_curr; l_curr++;
      color.rgbReserved = 0; l_curr++;
      FreeImage_SetPixelColor(bitmap, j,i, &color);
    }
  
  vkUnmapMemory(m_logicalDevice->GetDevice(), l_buf->m_memorySpace);
  l_buf->Free();
  
  if( FreeImage_Save(FIF_PNG, bitmap, "test.png", 0 ) )
    std::cout << "SUCCEED IN SAVING A SCREENSHOT\n";
  
  
}

void RenderControl::VKDeferredShadingPass::OutputOnScreen(){}


void RenderControl::VKDeferredShadingPass::Clear()
{
if( m_descriptorPool )
    vkDestroyDescriptorPool(m_logicalDevice->GetDevice(), m_descriptorPool, nullptr);
  
  for( unsigned int i = 0; i <m_frameBuffers.size(); ++i)
    if(m_frameBuffers[i])
      vkDestroyFramebuffer(m_logicalDevice->GetDevice(), m_frameBuffers[i], nullptr);
  
  if(m_renderPass)
    vkDestroyRenderPass(m_logicalDevice->GetDevice(), m_renderPass, nullptr);
}

void RenderControl::VKDeferredShadingPass::VulkanUpdate( char* a_mappedBuffer )
{
  // copy global camera matrices and screen resolution for lights fragments shaders
  // m_globalsUbo.projMatrix = *m_camera->GetOrthographicProjectionMatrix();
  m_globalsUbo.projMatrix = *m_camera->GetPerspectiveProjectionMatrix();
  m_globalsUbo.viewMatrix = glm::mat4(); // always identity matrix
  
  memcpy(a_mappedBuffer+m_uboMemBuffer->GetMemoryOffset(), &m_globalsUbo, sizeof(VertexViewProjMatrices) );
}


void RenderControl::VKDeferredShadingPass::CreateCommandPool() 
{
  VkCommandPoolCreateInfo poolInfo = {};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.queueFamilyIndex = m_indices.m_graphicsFamily;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

  if (vkCreateCommandPool(m_logicalDevice->GetDevice(), &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
    throw std::runtime_error("failed to create command pool!");
}

void RenderControl::VKDeferredShadingPass::CreateSemaphores()
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

void RenderControl::VKDeferredShadingPass::CreateRenderPass()
{

    // first colour attachment
  m_attachmentImages.push_back( m_memory->CreateAttachmentTexture(m_resolutionPart.x, m_resolutionPart.y, VK_FORMAT_R8G8B8A8_UNORM ) );
  VkAttachmentDescription colorAttachment = {};
  // colorAttachment.format = m_logicalDevice->GetSwapChainImageFormat();
  colorAttachment.format = m_attachmentImages.back()->m_format;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  // colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
  
  // second colour attachment
  m_attachmentImages.push_back( m_memory->CreateAttachmentTexture(m_resolutionPart.x, m_resolutionPart.y, VK_FORMAT_R8G8B8A8_UNORM ) );
  VkAttachmentDescription colorAttachment2 = {};
  colorAttachment2.format = m_attachmentImages.back()->m_format;
  colorAttachment2.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment2.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment2.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment2.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment2.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment2.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  colorAttachment2.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
  
  // thid colour attachment
  m_attachmentImages.push_back( m_memory->CreateAttachmentTexture(m_resolutionPart.x, m_resolutionPart.y, VK_FORMAT_R8G8B8A8_UNORM ) );
  VkAttachmentDescription colorAttachment3 = {};
  colorAttachment3.format = m_attachmentImages.back()->m_format;
  colorAttachment3.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment3.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment3.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment3.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment3.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment3.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  colorAttachment3.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
  
  // fourth colour attachment
  m_attachmentImages.push_back( m_memory->CreateAttachmentTexture(m_resolutionPart.x, m_resolutionPart.y, VK_FORMAT_R8G8B8A8_UNORM ) );
  VkAttachmentDescription colorAttachment4 = {};
  colorAttachment4.format = m_attachmentImages.back()->m_format;
  colorAttachment4.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment4.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment4.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment4.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment4.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment4.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  colorAttachment4.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
  
  // stencil depth attachment
  m_attachmentImages.push_back( m_memory->CreateStencilDepthAttachmentTexture(m_resolutionPart.x, m_resolutionPart.y ) );
  VkAttachmentDescription depthAttachment = {};
  depthAttachment.format = m_attachmentImages.back()->m_format;
  depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

  VkAttachmentReference colorAttachmentRef = {};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  
  VkAttachmentReference colorAttachmentRef2 = {};
  colorAttachmentRef2.attachment = 1;
  colorAttachmentRef2.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  
  VkAttachmentReference colorAttachmentRef3 = {};
  colorAttachmentRef3.attachment = 2;
  colorAttachmentRef3.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  
  VkAttachmentReference colorAttachmentRef4 = {};
  colorAttachmentRef4.attachment = 3;
  colorAttachmentRef4.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthAttachmentRef = {};
  depthAttachmentRef.attachment = 4;
  depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  std::vector<VkAttachmentReference> l_colourAttachmentRefs = { colorAttachmentRef, colorAttachmentRef2, colorAttachmentRef3, colorAttachmentRef4};
  
  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = static_cast<uint32_t>( l_colourAttachmentRefs.size() );
  subpass.pColorAttachments = l_colourAttachmentRefs.data();
  subpass.pDepthStencilAttachment = &depthAttachmentRef;
  
  
  std::vector< VkSubpassDescription > l_subpasses = { subpass, subpass, subpass, subpass, subpass, subpass, subpass};
  std::vector< VkSubpassDependency > l_dependencies = {};

  for(unsigned int i = 0; i < l_subpasses.size(); ++i)
  {
    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    VkSubpassDependency dependency2 = {};
    dependency2.srcSubpass = 0;
    dependency2.dstSubpass = VK_SUBPASS_EXTERNAL;
    dependency2.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency2.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependency2.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency2.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependency2.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
    
    l_dependencies.push_back(dependency);
    l_dependencies.push_back(dependency2);
  }
  
  
  std::vector<VkAttachmentDescription > attachments = { colorAttachment, colorAttachment2, colorAttachment3, colorAttachment4, depthAttachment};
  
  VkRenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  renderPassInfo.pAttachments = attachments.data();
  renderPassInfo.subpassCount = l_subpasses.size();
  renderPassInfo.pSubpasses = l_subpasses.data();
  renderPassInfo.dependencyCount = l_dependencies.size();
  renderPassInfo.pDependencies = l_dependencies.data();

  if (vkCreateRenderPass(m_logicalDevice->GetDevice(), &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
      throw std::runtime_error("failed to create render pass!");
  }
}

void RenderControl::VKDeferredShadingPass::CreateFramebuffer()
{ 
  // create frame buffer
  m_frameBuffers.resize(m_logicalDevice->GetSwapChainImageViews().size());
  m_swapChainImageViews = m_logicalDevice->GetSwapChainImageViews();

  size_t l_count = m_swapChainImageViews.size();
  for (size_t i = 0; i < l_count; i++) 
  {
    std::vector<VkImageView> attachments = {
      // m_swapChainImageViews[i],
      m_attachmentImages[0]->m_imageView,
      m_attachmentImages[1]->m_imageView,
      m_attachmentImages[2]->m_imageView,
      m_attachmentImages[3]->m_imageView,
      m_attachmentImages[4]->m_imageView
    };

    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = m_renderPass;
    framebufferInfo.attachmentCount = attachments.size();
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = m_resolution.x;
    framebufferInfo.height = m_resolution.y;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(m_logicalDevice->GetDevice(), &framebufferInfo, nullptr, &m_frameBuffers[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer!");
    }
  }
}

void RenderControl::VKDeferredShadingPass::CreatePipelines()
{
  m_pipelines = std::vector< std::shared_ptr<VKPipeline> >(7);

  // geometry pass shaders
  CreateSingleGeometryPassPipeline(0, 0, "..\\Assets\\SPV_shaders\\GeometryShader.vert.spv", "..\\Assets\\SPV_shaders\\GeometryShader.frag.spv");
  CreateSingleGeometryPassPipeline(1, 1, "..\\Assets\\SPV_shaders\\GeometryShader.vert.spv", "..\\Assets\\SPV_shaders\\GeometryEmissiveShader.frag.spv");
  CreateSingleGeometryPassPipeline(2, 1, "..\\Assets\\SPV_shaders\\GeometryShader.vert.spv", "..\\Assets\\SPV_shaders\\GeometryColourShader.frag.spv");
  CreateSingleGeometryPassPipeline(3, 2, "..\\Assets\\SPV_shaders\\GeometryShader.vert.spv", "..\\Assets\\SPV_shaders\\GeometryColourNormalShader.frag.spv");
  CreateSingleGeometryPassPipeline(4, 3, "..\\Assets\\SPV_shaders\\GeometryShader.vert.spv", "..\\Assets\\SPV_shaders\\GeometryColourNormalSpecShader.frag.spv");
  CreateSingleGeometryPassPipeline(5, 4, "..\\Assets\\SPV_shaders\\GeometryShader.vert.spv", "..\\Assets\\SPV_shaders\\GeometryColourNormalSpecHardnessShader.frag.spv");
  CreateSingleGeometryPassPipeline(6, 5, "..\\Assets\\SPV_shaders\\GeometryShader.vert.spv", "..\\Assets\\SPV_shaders\\GeometryColourNormalSpecHardnessEmissiveShader.frag.spv");
  
}

void RenderControl::VKDeferredShadingPass::CreateSingleGeometryPassPipeline(const unsigned int& a_index, const unsigned int& a_inputSamplers, const std::string& a_vertPath, const std::string& a_fragPath)
{
  // geometry shader shaders
  VkShaderModule l_vertex = CreateShaderModule(ReadFile( a_vertPath.c_str() ), m_logicalDevice->GetDevice());
  VkShaderModule l_frag = CreateShaderModule(ReadFile( a_fragPath.c_str() ), m_logicalDevice->GetDevice());
  
  // simple geometry 
  m_pipelines[a_index] = std::make_shared<VKGeometryPassPipeline>(m_logicalDevice, m_renderPass, CreatePipelineShaderCreateInfo(l_vertex, l_frag), a_index, 
                                                                  m_resolution, glm::vec4(0,0,m_resolution.x,m_resolution.y), a_inputSamplers );
  m_pipelines[a_index]->Init();
  
  // destroy shader modules
  vkDestroyShaderModule(m_logicalDevice->GetDevice(), l_vertex, nullptr);
  vkDestroyShaderModule(m_logicalDevice->GetDevice(), l_frag, nullptr);
  
  std::shared_ptr<VulkanSecondaryCommandBuffer> l_secondaryCmdBufferTmp =
    std::make_shared<VulkanSecondaryCommandBuffer>(m_logicalDevice->GetDevice(), m_commandPool, m_pipelines[a_index]->GetPipelineLayout(), m_renderPass, a_index);
  l_secondaryCmdBufferTmp->Init();
  m_pipelines[a_index]->AddSecondaryBuffer(l_secondaryCmdBufferTmp);

}


void RenderControl::VKDeferredShadingPass::CreateDescriptorPool()
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
    throw std::runtime_error("VKDeferredShadingPass::CreateDescriptorPool() - failed to create descriptor pool!");
  }
}

void RenderControl::VKDeferredShadingPass::CreateCommandBuffers()
{
  m_primaryCmdBuffer = std::shared_ptr<VulkanPrimaryCommandBuffer>( new VulkanPrimaryCommandBuffer(m_logicalDevice->GetDevice(), m_commandPool, m_frameBuffers, m_renderPass, m_resolution, 5) );
  m_primaryCmdBuffer->Init();
  for( auto l_pipeline : m_pipelines )
    m_primaryCmdBuffer->AddPipeline(l_pipeline);
}

void RenderControl::VKDeferredShadingPass::CreateDescriptorSet(const std::shared_ptr<VKPipeline>& a_pipeline, IRenderable* a_renderable)
{
    std::cout << "Prioiiiiiits\n";
  VkDescriptorSetLayout l_layout = a_pipeline->GetDescriptorSetLayout();
  VkDescriptorSetAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = m_descriptorPool;
  allocInfo.descriptorSetCount = 1;
  allocInfo.pSetLayouts = &l_layout;

  VkDescriptorSet l_descSet;
    std::cout << "Prioiiiiiits2\n";
  if (vkAllocateDescriptorSets(m_logicalDevice->GetDevice(), &allocInfo, &l_descSet ) != VK_SUCCESS) {
    throw std::runtime_error("VKDeferredShadingPass::CreateDescriptorSet() - failed to allocate descriptor sets!");
  }
  
  // personal ubos here
  std::vector<size_t> l_uboSizes = a_pipeline->GetObjUboSizes();
  std::vector< std::shared_ptr<VulkanMemoryChunk> > l_uboMemBuffer = {nullptr,nullptr};
  if( l_uboSizes.size() > 0 )
  {
    l_uboMemBuffer[0] = m_memory->CreateUniformBuffer( l_uboSizes[0] );
  }
  if( l_uboSizes.size() > 1 )
  {
    l_uboMemBuffer[1] = m_memory->CreateUniformBuffer( l_uboSizes[1] );
  }
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
  
  

  // first object data
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
  
  
  // second object data
  VkDescriptorBufferInfo l_bufferInfo3 = {};
  l_bufferInfo3.buffer = l_uboMemBuffer[1]->m_buffer->m_buffer;
  l_bufferInfo3.offset = l_uboMemBuffer[1]->GetBufferOffset();
  l_bufferInfo3.range = l_uboMemBuffer[1]->m_size;
  
  
  VkWriteDescriptorSet descriptorWrite3 = {};
  descriptorWrite3.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrite3.dstSet = l_descSet;
  descriptorWrite3.dstBinding = l_bindings[2].binding;
  descriptorWrite3.dstArrayElement = 0;
  descriptorWrite3.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptorWrite3.descriptorCount = 1;
  descriptorWrite3.pBufferInfo = &l_bufferInfo3;
  
  std::vector< VkWriteDescriptorSet > l_descriptorSetWrites = {descriptorWrite, descriptorWrite2, descriptorWrite3 };
  

  unsigned int l_textureCount = l_bindings.size() - 3;
  std::vector<VkDescriptorImageInfo> l_imageInfo(l_textureCount, {});
  std::vector<VkWriteDescriptorSet> extraDescriptorWrites(l_textureCount, {});
  
  for( unsigned int i = 0; i < l_textureCount; ++i)
  {  
    SceneControl::TexturedSceneNode* l_texturedSceneNode = dynamic_cast<SceneControl::TexturedSceneNode*>(a_renderable);
    std::shared_ptr<ITexture> l_texture = l_texturedSceneNode->GetTexture(i);
    if( l_texture )
    {
      VKATexture* l_tempTexture = reinterpret_cast<VKATexture*>( l_texture->GetExtra() );
      if( l_tempTexture )
      {      
        l_imageInfo[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;   // VkImageLayout
        l_imageInfo[i].sampler = l_tempTexture->GetSampler()->m_sampler; // VkSampler                      
        l_imageInfo[i].imageView = l_tempTexture->GetImage()->m_imageView;  // VkImageView
        
        
        extraDescriptorWrites[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        extraDescriptorWrites[i].dstSet = l_descSet;
        extraDescriptorWrites[i].dstBinding = l_bindings[i+3].binding;
        extraDescriptorWrites[i].dstArrayElement = 0;
        extraDescriptorWrites[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        extraDescriptorWrites[i].descriptorCount = 1;
        extraDescriptorWrites[i].pImageInfo = &l_imageInfo[i];
        l_descriptorSetWrites.push_back(extraDescriptorWrites[i]);
      }
    }
  }

  vkUpdateDescriptorSets(m_logicalDevice->GetDevice(), l_descriptorSetWrites.size(), l_descriptorSetWrites.data(), 0, nullptr);
}
