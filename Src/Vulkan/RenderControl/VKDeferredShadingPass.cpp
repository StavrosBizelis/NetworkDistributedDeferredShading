/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: VKDeferredShadingPass.cpp
 *   DATE: Tue Jul 17 20:15:15 2018
 *  DESCR: 
 ***********************************************************************/
#include "Vulkan/RenderControl/VKDeferredShadingPass.h"
#include "Common/Network/NetworkMsg.h"
#include "Common/Core/MyUtilities.h"
#include "Vulkan/Core/VulkanUtilities.h"
#include "Vulkan/RenderControl/Pipelines/VKDeferredPipelines.h"
#include "Vulkan/RenderControl/Pipelines/VKCompositionPipeline.h"
#include "Vulkan/Shapes/VKShapeFactory.h"
#include <Vulkan/Textures/VKTexture.h>
#include <Vulkan/Textures/VKCubemap.h>
#include <Vulkan/RenderControl/VulkanRenderable.h>

#include "Common/SceneControl/TexturedSceneNode.h"

#include <iostream>



/*
 *  Method: RenderControl::VKDeferredShadingPass::VKDeferredShadingPass
 *  Params: const glm::vec2 &a_resolution
 * Effects: 
 */
RenderControl::VKDeferredShadingPass::VKDeferredShadingPass(const std::shared_ptr<VulkanLogicalDeviceManager>& a_device, const VkPhysicalDevice& a_physicalDevice, const std::shared_ptr<VulkanMemory>& a_memory,
                                                            const VkQueue& a_graphicsQueue, const VkQueue& a_presentQueue, const QueueFamilyIndices& a_indices,
                                                            const glm::vec2 &a_resolution, const glm::vec2 &a_partialResolution, const glm::vec4& a_viewportSettings)
  : ADeferredShadingPass(a_resolution, a_partialResolution, a_viewportSettings),
    m_logicalDevice(a_device), m_physicalDevice(a_physicalDevice), m_memory(a_memory), m_graphicsQueue(a_graphicsQueue), m_presentQueue(a_presentQueue), m_indices(a_indices),
    m_descriptorPool(NULL)
{
  m_renderPass = NULL;
  // m_pbos[0] = 0;
  // m_pbos[1] = 0;
  m_outputTextures.resize(5, 0);
  m_outputSamplers.resize(5, 0);
  m_currentFrame = 0;
}


/*
 *  Method: RenderControl::VKDeferredShadingPass::~VKDeferredShadingPass
 *  Params: 
 * Effects: 
 */
RenderControl::VKDeferredShadingPass::~VKDeferredShadingPass()
{
  Clear();
}


bool RenderControl::VKDeferredShadingPass::Init()
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


  
  // // init camera
  std::shared_ptr<CCamera> l_cam = GetCamera();

  m_camera->SetOrthographicProjectionMatrix((int)m_resolution.x, (int)m_resolution.y);
  m_camera->SetPerspectiveProjectionMatrix(45.0f, m_resolution.x / m_resolution.y, 0.5f, 7000.0f);

  // init buffers for global ubos
  m_uboMemBuffers.resize(2);  
  m_uboMemBuffers[0] = m_memory->CreateUniformBuffer( sizeof(FragLightGlobalVars) );
  m_uboMemBuffers[1] = m_memory->CreateUniformBuffer( sizeof(VertexViewProjMatrices) );
  
  return true;
}

void RenderControl::VKDeferredShadingPass::Render()
{

  uint32_t l_imageIndex;
  VkResult result = vkAcquireNextImageKHR(m_logicalDevice->GetDevice(), m_logicalDevice->GetSwapChain(), std::numeric_limits<uint64_t>::max(), m_imageAvailableSemaphore[m_currentFrame], VK_NULL_HANDLE, &l_imageIndex);
  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
       std::cout << "VKDeferredShadingPass::Render() - requires reinitialization\n";
      // ReInit();
      // return;
  }
  else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("VKDeferredShadingPass::Render() - failed to acquire swap chain image!");
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

  std::cout << "FUCK ME1\n" << l_imageIndex;
  VkResult l_res = vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
  if ( l_res != VK_SUCCESS)
  {
    std::cout << "FUCK ME2\n" << l_res;
    throw std::runtime_error("VKDeferredShadingPass::Render() - failed to submit draw command buffer!");
  }
  std::cout << "FUCK ME3\n" << l_res;

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
    std::cout << "VKDeferredShadingPass::Render() - requires reinitialization\n";
  } 
  else if (result != VK_SUCCESS) 
  {
    throw std::runtime_error("VKDeferredShadingPass::Render() - failed to present swap chain image!");
  }
  
  // vkQueueWaitIdle(m_presentQueue);
  m_currentFrame = (m_currentFrame + 1) % m_maxFramesInFlight;
  
  
  
  // VkSubmitInfo submitInfo = {};
  // submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  
  // // Wait for swap chain presentation to finish
	// submitInfo.waitSemaphoreCount = 0;
  // //submitInfo.pWaitSemaphores = &m_renderFinishedSemaphore;
  // // Signal ready with offscreen semaphore
  // submitInfo.signalSemaphoreCount = 0;
  // //submitInfo.pSignalSemaphores = &m_renderFinishedSemaphore;
  
  

  // m_primaryCmdBuffer->Update();
  // VkCommandBuffer l_cmdBuffer = m_primaryCmdBuffer->GetNextCommandBufferHandle();
  // submitInfo.commandBufferCount = 1;
  // submitInfo.pCommandBuffers = &l_cmdBuffer;
	// if( vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS )
  // {
    // throw std::runtime_error("failed to submit draw command buffer!");
  // }
  // // wait for the graphics queue to finish
  vkQueueWaitIdle(m_graphicsQueue); 
  vkQueueWaitIdle(m_presentQueue); 
}

void RenderControl::VKDeferredShadingPass::OutputOnScreen()
{}

void RenderControl::VKDeferredShadingPass::UpdateViewportSettings(const glm::vec2& a_resolutionPart, const glm::vec4& a_viewportSettings)
{
  Clear();
  m_resolutionPart = a_resolutionPart;
  m_viewPortSetting = a_viewportSettings;
  Init();
}

void RenderControl::VKDeferredShadingPass::GeometryPass()
{}

void RenderControl::VKDeferredShadingPass::LightPass()
{}

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

void RenderControl::VKDeferredShadingPass::SetMaterialManager(MaterialControl::IMaterialManager* a_materialManager){}


bool RenderControl::VKDeferredShadingPass::AddRenderable(RenderControl::IRenderable* a_renderable, const RenderControl::GeometryPassMaterialFlags& a_geometryMaterialFlags)
{
  if (!Exists(a_renderable))
		{
      
      
      // // simple geometry 
      // m_pipelines[0] = std::make_shared<VKGeometryPassPipeline>(m_logicalDevice, m_renderPass, CreatePipelineShaderCreateInfo(l_geomVert, l_geomFrag), 0, m_resolutionPart, m_viewPortSetting, 0 );
      // // emissive
      // m_pipelines[1] = std::make_shared<VKGeometryPassPipeline>(m_logicalDevice, m_renderPass, CreatePipelineShaderCreateInfo(l_geomVert, l_geomEmissiveFrag), 1, m_resolutionPart, m_viewPortSetting, 1 );
      // // colour
      // m_pipelines[2] = std::make_shared<VKGeometryPassPipeline>(m_logicalDevice, m_renderPass, CreatePipelineShaderCreateInfo(l_geomVert, l_geomColourFrag), 2, m_resolutionPart, m_viewPortSetting, 1 );
      // // colour normal
      // m_pipelines[3] = std::make_shared<VKGeometryPassPipeline>(m_logicalDevice, m_renderPass, CreatePipelineShaderCreateInfo(l_geomVert, l_geomColourNormalFrag), 3, m_resolutionPart, m_viewPortSetting, 2 );
      // // colour normal spec
      // m_pipelines[4] = std::make_shared<VKGeometryPassPipeline>(m_logicalDevice, m_renderPass, CreatePipelineShaderCreateInfo(l_geomVert, l_geomColourNormalSpecFrag), 4, m_resolutionPart, m_viewPortSetting, 3 );
      // // colour normal spec hard
      // m_pipelines[5] = std::make_shared<VKGeometryPassPipeline>(m_logicalDevice, m_renderPass, CreatePipelineShaderCreateInfo(l_geomVert, l_geomColourNormalSpecHardnessFrag), 5, m_resolutionPart, m_viewPortSetting, 4 );
      // // colour normal spec hard emissive
      // m_pipelines[6] = std::make_shared<VKGeometryPassPipeline>(m_logicalDevice, m_renderPass, CreatePipelineShaderCreateInfo(l_geomVert, l_geomColourNormalSpecHardnessEmmisiveFrag), 6, m_resolutionPart, m_viewPortSetting, 5 );
      // // sky box
      // m_pipelines[7] = std::make_shared<VKSkyboxPassPipeline>(m_logicalDevice, m_renderPass, CreatePipelineShaderCreateInfo(l_skyboxVert, l_skyboxFrag), 7, m_resolutionPart, m_viewPortSetting );
      // // light pass
      // m_pipelines[8] = std::make_shared<VKLightPassPipeline>(m_logicalDevice, m_renderPass, CreatePipelineShaderCreateInfo(l_pointVert, l_pointFrag), 7, m_resolutionPart, m_viewPortSetting );
      // // directional light pass
      // m_pipelines[9] = std::make_shared<VKDirLightPassPipeline>(m_logicalDevice, m_renderPass, CreatePipelineShaderCreateInfo(l_directionalVert, l_directionalFrag), 8, m_resolutionPart, m_viewPortSetting );
      
			// std::shared_ptr<IShaderProgram> l_selectedMat;
			// get the appropriate material func
      
      std::vector< std::shared_ptr<VulkanSecondaryCommandBuffer> > l_cmdBuffers;
			// simple color
			if ((a_geometryMaterialFlags & (NORMAL_MAP | DIFFUSE_MAP | SPECULAR_MAP | HARDNESS_MAP | EMISSION_MAP)) == (NORMAL_MAP | DIFFUSE_MAP | SPECULAR_MAP | HARDNESS_MAP | EMISSION_MAP))
      {
        CreateGeometryDescriptorSet(m_pipelines[6], a_renderable);
        l_cmdBuffers = m_pipelines[6]->GetSecondaryCommandBuffers();
      }
			else if ((a_geometryMaterialFlags & (NORMAL_MAP | DIFFUSE_MAP | SPECULAR_MAP | HARDNESS_MAP )) == (NORMAL_MAP | DIFFUSE_MAP | SPECULAR_MAP | HARDNESS_MAP))
			{
        CreateGeometryDescriptorSet(m_pipelines[5], a_renderable);
        l_cmdBuffers = m_pipelines[5]->GetSecondaryCommandBuffers();
      }
      else if ((a_geometryMaterialFlags & (NORMAL_MAP | DIFFUSE_MAP | SPECULAR_MAP)) == (NORMAL_MAP | DIFFUSE_MAP | SPECULAR_MAP))
			{
        CreateGeometryDescriptorSet(m_pipelines[4], a_renderable);
        l_cmdBuffers = m_pipelines[4]->GetSecondaryCommandBuffers();
      }
      else if ((a_geometryMaterialFlags & (NORMAL_MAP | DIFFUSE_MAP)) == (NORMAL_MAP | DIFFUSE_MAP))
			{
        CreateGeometryDescriptorSet(m_pipelines[3], a_renderable);
        l_cmdBuffers = m_pipelines[3]->GetSecondaryCommandBuffers();
      }
      else if ((a_geometryMaterialFlags & (DIFFUSE_MAP)) == (DIFFUSE_MAP))
			{
        CreateGeometryDescriptorSet(m_pipelines[2], a_renderable);
      }
      else if ((a_geometryMaterialFlags & (EMISSION_MAP)) == (EMISSION_MAP))
			{
        CreateGeometryDescriptorSet(m_pipelines[1], a_renderable);
        l_cmdBuffers = m_pipelines[1]->GetSecondaryCommandBuffers();
      }
      else if ((a_geometryMaterialFlags & (SKYBOX)) == (SKYBOX))
			{
        CreateGeometryDescriptorSet(m_pipelines[7], a_renderable);
        l_cmdBuffers = m_pipelines[7]->GetSecondaryCommandBuffers();
      }
      else 
			{
        CreateGeometryDescriptorSet(m_pipelines[0], a_renderable);
        l_cmdBuffers = m_pipelines[0]->GetSecondaryCommandBuffers();
      }
      
      VulkanRenderable* l_renderable = reinterpret_cast<VulkanRenderable*>( a_renderable->GetExtra() );
      if( l_cmdBuffers.size() > 0 && l_renderable)
      {
        std::cout << "renderable is added to secondary command buffer \n\n\n";
        l_cmdBuffers[0]->AddMesh(l_renderable);
      }
      
			// a_renderable->SetMaterial(l_selectedMat);
			// m_toRender[l_selectedMat].push_back(a_renderable);
			return true;
		} 
		return false;
}

void RenderControl::VKDeferredShadingPass::AddLight(RenderControl::IRenderable* a_light, const RenderControl::LightTypeFlags& a_lightType)
{
  VulkanRenderable* l_renderable =reinterpret_cast<VulkanRenderable*>( a_light->GetExtra() );
  
  switch (a_lightType)
		{
		case LightTypeFlags::POINT_LIGHT:
    {
      CreateDescriptorSet(m_pipelines[8], a_light);
    }
		break;
		case LightTypeFlags::SPOT_LIGHT:
		{
      CreateDescriptorSet(m_pipelines[8], a_light);
    }
		break;
		case LightTypeFlags::DIRECTIONAL_LIGHT:
		{
      CreateDescriptorSet(m_pipelines[9], a_light);
    }
			break;
		}

		m_lights.insert(a_light);
}

bool RenderControl::VKDeferredShadingPass::PackTexture( Network::NetworkMsgPtr& a_outMsg)
{
  return true;
}

void RenderControl::VKDeferredShadingPass::VulkanUpdate( char* a_mappedBuffer )
{

  // m_globalsUbo1.UInverseViewProjectionMatrix
  // m_globalsUbo1.UCamPos
  // m_globalsUbo1.UScreenResDiv =
  
  // copy global camera matrices and screen resolution for lights fragments shaders
  m_globalsUbo2.projMatrix = *m_camera->GetPerspectiveProjectionMatrix();
  
  glutil::MatrixStack modelViewMatrixStack;
  modelViewMatrixStack.SetIdentity();
  modelViewMatrixStack.LookAt(m_camera->GetPosition(), m_camera->GetView(), m_camera->GetUpVector());
  m_globalsUbo2.viewMatrix = modelViewMatrixStack.Top(); // always identity matrix
  
  // m_globalsUbo2.viewMatrix = m_camera->GetViewMatrix(); // always identity matrix
  
  // copy global camera matrices and screen resolution for lights fragments shaders
  memcpy(a_mappedBuffer+m_uboMemBuffers[0]->GetMemoryOffset(), &m_globalsUbo1, sizeof(FragLightGlobalVars) );
  // copy global camera matrices for objects
  memcpy(a_mappedBuffer+m_uboMemBuffers[1]->GetMemoryOffset(), &m_globalsUbo2, sizeof(VertexViewProjMatrices) ); 
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
  
  // init attachments
  // no swapchain attachments for deferred shading pass
  // create the attachments image
  m_attachmentImages.reserve(5);
  try{
    //diffuse attachment
    m_attachmentImages.push_back( m_memory->CreateAttachmentTexture(m_resolutionPart.x, m_resolutionPart.y, VK_FORMAT_R8G8B8A8_UNORM ) );
    //normals attachment
    m_attachmentImages.push_back( m_memory->CreateAttachmentTexture(m_resolutionPart.x, m_resolutionPart.y, VK_FORMAT_R16G16B16A16_SFLOAT ) );
    //specular
    m_attachmentImages.push_back( m_memory->CreateAttachmentTexture(m_resolutionPart.x, m_resolutionPart.y, VK_FORMAT_R8G8B8A8_UNORM ) );
    //emissive - final image
    m_attachmentImages.push_back( m_memory->CreateAttachmentToDownloadTexture(m_resolutionPart.x, m_resolutionPart.y, VK_FORMAT_R8G8B8A8_UNORM ) );
    //stencil depth
    m_attachmentImages.push_back( m_memory->CreateStencilDepthAttachmentTexture(m_resolutionPart.x, m_resolutionPart.y ) );
  }
  catch(std::runtime_error& e)
  {
    std::cout<< "EXCEPTION: " << e.what();
  }

  std::vector< VkAttachmentDescription > l_attachments( m_attachmentImages.size() );
  // create attachments
  for( unsigned int i = 0; i < l_attachments.size(); ++i)
  {
    l_attachments[i].format = VkFormat(m_attachmentImages[i]->m_format);
    l_attachments[i].samples = VK_SAMPLE_COUNT_1_BIT;
    l_attachments[i].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    l_attachments[i].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    l_attachments[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    l_attachments[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    
    l_attachments[i].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    l_attachments[i].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    if( i == 4 )  //if it is the depth stencil attachment
    {
      l_attachments[i].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
      l_attachments[i].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
    }
    
    if( i == 3 )  // temporary 3rd swapchain attachment
    {
      l_attachments[i].format = m_logicalDevice->GetSwapChainImageFormat();
    }
    
  }
  
  std::vector<VkSubpassDescription> l_subpasses(10);
   
  // geometry sub pass references
  std::vector <VkAttachmentReference> l_colourAttachmentRefs(4);
  for( unsigned int i = 0; i < 4; ++i)
  {
    l_colourAttachmentRefs[i].attachment = i;
    l_colourAttachmentRefs[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  }
  
  VkAttachmentReference l_depthAttachmentRef = {};
  l_depthAttachmentRef.attachment = 4;
  l_depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  
  
  for( unsigned int i = 0; i < 8; ++i )
  {
    l_subpasses[i].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    l_subpasses[i].inputAttachmentCount = 0;
    l_subpasses[i].colorAttachmentCount = l_colourAttachmentRefs.size();
    l_subpasses[i].pColorAttachments = l_colourAttachmentRefs.data();
    l_subpasses[i].pDepthStencilAttachment = &l_depthAttachmentRef;
  }
  
  
  
  
  // light pass references
  std::vector <VkAttachmentReference> l_lightInputAttachmentRefs(4);
  for( unsigned int i = 0; i < 3; ++i)
  {
    l_lightInputAttachmentRefs[i].attachment = i;
    l_lightInputAttachmentRefs[i].layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  }  
  l_lightInputAttachmentRefs[3].attachment = 4;
  l_lightInputAttachmentRefs[3].layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
  
  
  VkAttachmentReference l_lightColourAttachment = {};
  l_lightColourAttachment.attachment = 3;
  l_lightColourAttachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  
  for( unsigned int i= 8; i < 10; ++i)
  {
    l_subpasses[i].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    l_subpasses[i].inputAttachmentCount = l_lightInputAttachmentRefs.size();
    l_subpasses[i].pInputAttachments = l_lightInputAttachmentRefs.data();
    l_subpasses[i].colorAttachmentCount = 1;
    l_subpasses[i].pColorAttachments = &l_lightColourAttachment;
    // l_subpasses[i].pDepthStencilAttachment = &l_depthAttachmentRef;
  }
  

  // dependencies - two major types of dependencies
  // geometry passes depend on the external
  std::vector< VkSubpassDependency> l_dependencies( 2*8 );

  // light passes depend on the geometry ones
  for( unsigned int i = 8; i < 10; ++i)
  {
    for( unsigned int j = 0; j < 8; ++j )
    {
      l_dependencies[i].srcSubpass = j;
      l_dependencies[i].dstSubpass = i;
      l_dependencies[i].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
      l_dependencies[i].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
      l_dependencies[i].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
      l_dependencies[i].dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
      l_dependencies[i].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
    }
  }
  
  
  VkRenderPassCreateInfo l_renderPassInfo = {};
  l_renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  l_renderPassInfo.attachmentCount = l_attachments.size();
  l_renderPassInfo.pAttachments = l_attachments.data();
  l_renderPassInfo.subpassCount = l_subpasses.size();
  l_renderPassInfo.pSubpasses = l_subpasses.data();
  l_renderPassInfo.dependencyCount = l_dependencies.size();
  l_renderPassInfo.pDependencies = l_dependencies.data();
  
  if (vkCreateRenderPass(m_logicalDevice->GetDevice(), &l_renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) 
    throw std::runtime_error("VKDeferredShadingPass::Init() - failed to create render pass!");
  
}

void RenderControl::VKDeferredShadingPass::CreateFramebuffer()
{
  // create frame buffer
  m_frameBuffers.resize(m_logicalDevice->GetSwapChainImageViews().size());
  m_swapChainImageViews = m_logicalDevice->GetSwapChainImageViews();

  size_t l_count = m_swapChainImageViews.size();
  for (size_t i = 0; i < l_count; ++i) 
  {
    VkImageView attachments[] = {
      m_attachmentImages[0]->m_imageView,
      m_attachmentImages[1]->m_imageView,
      m_attachmentImages[2]->m_imageView,
      m_swapChainImageViews[i],
      m_attachmentImages[4]->m_imageView
    };

    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = m_renderPass;
    framebufferInfo.attachmentCount = 5;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = m_resolutionPart.x;
    framebufferInfo.height = m_resolutionPart.y;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(m_logicalDevice->GetDevice(), &framebufferInfo, nullptr, &m_frameBuffers[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer!");
    }
  }
}

void RenderControl::VKDeferredShadingPass::CreatePipelines()
{
  // geometry shader shaders
  VkShaderModule l_geomVert = CreateShaderModule(ReadFile("..\\Assets\\SPV_shaders\\GeometryShader.vert.spv"), m_logicalDevice->GetDevice());
  VkShaderModule l_geomFrag = CreateShaderModule(ReadFile("..\\Assets\\SPV_shaders\\GeometryShader.frag.spv"), m_logicalDevice->GetDevice());
  VkShaderModule l_geomEmissiveFrag = CreateShaderModule(ReadFile("..\\Assets\\SPV_shaders\\GeometryEmissiveShader.frag.spv"), m_logicalDevice->GetDevice());
  VkShaderModule l_geomColourFrag = CreateShaderModule(ReadFile("..\\Assets\\SPV_shaders\\GeometryColourShader.frag.spv"), m_logicalDevice->GetDevice());
  VkShaderModule l_geomColourNormalFrag = CreateShaderModule(ReadFile("..\\Assets\\SPV_shaders\\GeometryColourNormalShader.frag.spv"), m_logicalDevice->GetDevice());
  VkShaderModule l_geomColourNormalSpecFrag = CreateShaderModule(ReadFile("..\\Assets\\SPV_shaders\\GeometryColourNormalSpecShader.frag.spv"), m_logicalDevice->GetDevice());
  VkShaderModule l_geomColourNormalSpecHardnessFrag = CreateShaderModule(ReadFile("..\\Assets\\SPV_shaders\\GeometryColourNormalSpecHardnessShader.frag.spv"), m_logicalDevice->GetDevice());
  VkShaderModule l_geomColourNormalSpecHardnessEmmisiveFrag = CreateShaderModule(ReadFile("..\\Assets\\SPV_shaders\\GeometryColourNormalSpecHardnessEmissiveShader.frag.spv"), m_logicalDevice->GetDevice());
  
  VkShaderModule l_skyboxVert = CreateShaderModule(ReadFile("..\\Assets\\SPV_shaders\\GeometrySkyBoxShader.vert.spv"), m_logicalDevice->GetDevice());
  VkShaderModule l_skyboxFrag = CreateShaderModule(ReadFile("..\\Assets\\SPV_shaders\\GeometrySkyBoxShader.frag.spv"), m_logicalDevice->GetDevice());
  
  // light pass shaders
  // VkShaderModule l_stencilVert = CreateShaderModule(ReadFile("..\\Assets\\SPV_shaders\\NullShader.vert.spv"), m_logicalDevice->GetDevice());
  // VkShaderModule l_stencilFrag = CreateShaderModule(ReadFile("..\\Assets\\SPV_shaders\\NullShader.frag.spv"), m_logicalDevice->GetDevice());
  
  VkShaderModule l_pointVert = CreateShaderModule(ReadFile("..\\Assets\\SPV_shaders\\PointLightShader.vert.spv"), m_logicalDevice->GetDevice());
  VkShaderModule l_pointFrag = CreateShaderModule(ReadFile("..\\Assets\\SPV_shaders\\PointLightShader.frag.spv"), m_logicalDevice->GetDevice());
  
  // VkShaderModule l_spotVert = CreateShaderModule(ReadFile("..\\Assets\\SPV_shaders\\SpotLightShader.vert.spv"), m_logicalDevice->GetDevice());
  // VkShaderModule l_spotFrag = CreateShaderModule(ReadFile("..\\Assets\\SPV_shaders\\SpotLightShader.frag.spv"), m_logicalDevice->GetDevice());
  
  VkShaderModule l_directionalVert = CreateShaderModule(ReadFile("..\\Assets\\SPV_shaders\\DirectionalLightShader.vert.spv"), m_logicalDevice->GetDevice());
  VkShaderModule l_directionalFrag = CreateShaderModule(ReadFile("..\\Assets\\SPV_shaders\\DirectionalLightShader.frag.spv"), m_logicalDevice->GetDevice());
  
  

  m_pipelines = std::vector< std::shared_ptr<VKPipeline> >(10);
  // simple geometry 

  
  m_pipelines[0] = std::make_shared<VKGeometryPassPipeline>(m_logicalDevice, m_renderPass, CreatePipelineShaderCreateInfo(l_geomVert, l_geomFrag), 0, m_resolutionPart, m_viewPortSetting, 0 );
  // emissive
  m_pipelines[1] = std::make_shared<VKGeometryPassPipeline>(m_logicalDevice, m_renderPass, CreatePipelineShaderCreateInfo(l_geomVert, l_geomEmissiveFrag), 1, m_resolutionPart, m_viewPortSetting, 1 );
  // colour
  m_pipelines[2] = std::make_shared<VKGeometryPassPipeline>(m_logicalDevice, m_renderPass, CreatePipelineShaderCreateInfo(l_geomVert, l_geomColourFrag), 2, m_resolutionPart, m_viewPortSetting, 1 );
  // colour normal
  m_pipelines[3] = std::make_shared<VKGeometryPassPipeline>(m_logicalDevice, m_renderPass, CreatePipelineShaderCreateInfo(l_geomVert, l_geomColourNormalFrag), 3, m_resolutionPart, m_viewPortSetting, 2 );
  // colour normal spec
  m_pipelines[4] = std::make_shared<VKGeometryPassPipeline>(m_logicalDevice, m_renderPass, CreatePipelineShaderCreateInfo(l_geomVert, l_geomColourNormalSpecFrag), 4, m_resolutionPart, m_viewPortSetting, 3 );
  // colour normal spec hard
  m_pipelines[5] = std::make_shared<VKGeometryPassPipeline>(m_logicalDevice, m_renderPass, CreatePipelineShaderCreateInfo(l_geomVert, l_geomColourNormalSpecHardnessFrag), 5, m_resolutionPart, m_viewPortSetting, 4 );
  // colour normal spec hard emissive
  m_pipelines[6] = std::make_shared<VKGeometryPassPipeline>(m_logicalDevice, m_renderPass, CreatePipelineShaderCreateInfo(l_geomVert, l_geomColourNormalSpecHardnessEmmisiveFrag), 6, m_resolutionPart, m_viewPortSetting, 5 );
  // sky box
  m_pipelines[7] = std::make_shared<VKSkyboxPassPipeline>(m_logicalDevice, m_renderPass, CreatePipelineShaderCreateInfo(l_skyboxVert, l_skyboxFrag), 7, m_resolutionPart, m_viewPortSetting );
  // light pass
  m_pipelines[8] = std::make_shared<VKLightPassPipeline>(m_logicalDevice, m_renderPass, CreatePipelineShaderCreateInfo(l_pointVert, l_pointFrag), 8, m_resolutionPart, m_viewPortSetting );
  // directional light pass
  m_pipelines[9] = std::make_shared<VKDirLightPassPipeline>(m_logicalDevice, m_renderPass, CreatePipelineShaderCreateInfo(l_directionalVert, l_directionalFrag), 9, m_resolutionPart, m_viewPortSetting );
  
  for( auto l_pipeline : m_pipelines)
  {
    l_pipeline->Init();
  }
  // leaving stencil pass and spotlights out for the moment 
  // may implement tiled deferred shading with point lights
  // CreatePipeline(CreatePipelineShaderCreateInfo(l_stencilVert, l_stencilFrag), 3, 1,  );
  // CreatePipeline(CreatePipelineShaderCreateInfo(l_spotVert, l_spotFrag), 4, 1, 9 );

  
  
  // destroy shader modules
  vkDestroyShaderModule(m_logicalDevice->GetDevice(), l_geomVert, nullptr);
  vkDestroyShaderModule(m_logicalDevice->GetDevice(), l_geomFrag, nullptr);
  vkDestroyShaderModule(m_logicalDevice->GetDevice(), l_geomEmissiveFrag, nullptr);
  vkDestroyShaderModule(m_logicalDevice->GetDevice(), l_geomColourFrag, nullptr);
  vkDestroyShaderModule(m_logicalDevice->GetDevice(), l_geomColourNormalFrag, nullptr);
  vkDestroyShaderModule(m_logicalDevice->GetDevice(), l_geomColourNormalSpecFrag, nullptr);
  vkDestroyShaderModule(m_logicalDevice->GetDevice(), l_geomColourNormalSpecHardnessFrag, nullptr);
  vkDestroyShaderModule(m_logicalDevice->GetDevice(), l_geomColourNormalSpecHardnessEmmisiveFrag, nullptr);
  vkDestroyShaderModule(m_logicalDevice->GetDevice(), l_skyboxVert, nullptr);
  vkDestroyShaderModule(m_logicalDevice->GetDevice(), l_skyboxFrag, nullptr);
  // vkDestroyShaderModule(m_logicalDevice->GetDevice(), l_stencilVert, nullptr);
  // vkDestroyShaderModule(m_logicalDevice->GetDevice(), l_stencilFrag, nullptr);
  vkDestroyShaderModule(m_logicalDevice->GetDevice(), l_pointVert, nullptr);
  vkDestroyShaderModule(m_logicalDevice->GetDevice(), l_pointFrag, nullptr);
  // vkDestroyShaderModule(m_logicalDevice->GetDevice(), l_spotVert, nullptr);
  // vkDestroyShaderModule(m_logicalDevice->GetDevice(), l_spotFrag, nullptr);
  vkDestroyShaderModule(m_logicalDevice->GetDevice(), l_directionalVert, nullptr);
  vkDestroyShaderModule(m_logicalDevice->GetDevice(), l_directionalFrag, nullptr);
  
  std::shared_ptr<VulkanSecondaryCommandBuffer> l_secondaryCmdBufferTmp;
  for( unsigned int i = 0; i < m_pipelines.size(); ++i)
  {
    l_secondaryCmdBufferTmp = std::make_shared<VulkanSecondaryCommandBuffer>(m_logicalDevice->GetDevice(), m_commandPool, m_pipelines[i]->GetPipelineLayout(), m_renderPass, i);
    l_secondaryCmdBufferTmp->Init();
    m_pipelines[i]->AddSecondaryBuffer(l_secondaryCmdBufferTmp);
  }  
}

void RenderControl::VKDeferredShadingPass::CreateDescriptorPool()
{
  VkDescriptorPoolSize l_poolSize1 = {};
  l_poolSize1.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  l_poolSize1.descriptorCount = 2000;

  VkDescriptorPoolSize l_poolSize2 = {};
  l_poolSize2.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  l_poolSize2.descriptorCount = 2000;

  VkDescriptorPoolSize l_poolSize3 = {};
  l_poolSize3.type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
  l_poolSize3.descriptorCount = 2000;
  
  std::vector<VkDescriptorPoolSize> l_poolSizes = {l_poolSize1, l_poolSize2, l_poolSize3};
  
  VkDescriptorPoolCreateInfo l_poolInfo = {};
  l_poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  l_poolInfo.poolSizeCount = l_poolSizes.size();
  l_poolInfo.pPoolSizes = &l_poolSizes[0];
  l_poolInfo.maxSets = 1000;

  if (vkCreateDescriptorPool(m_logicalDevice->GetDevice(), &l_poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
    throw std::runtime_error("VKDeferredShadingPass::CreateDescriptorPool() - failed to create descriptor pool!");
  }
}

void RenderControl::VKDeferredShadingPass::CreateCommandBuffers()
{
  m_primaryCmdBuffer = std::shared_ptr<VulkanPrimaryCommandBuffer>( new VulkanPrimaryCommandBuffer(m_logicalDevice->GetDevice(), m_commandPool, m_frameBuffers, m_renderPass, m_resolutionPart, 5) );
  m_primaryCmdBuffer->Init();
  for( auto l_pipeline : m_pipelines )
    m_primaryCmdBuffer->AddPipeline(l_pipeline);
}


void RenderControl::VKDeferredShadingPass::CreateDescriptorSet(const std::shared_ptr<VKPipeline>& a_pipeline, IRenderable* a_renderable)
{
  VkDescriptorSetLayout l_layout = a_pipeline->GetDescriptorSetLayout();
  VkDescriptorSetAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = m_descriptorPool;
  allocInfo.descriptorSetCount = 1;
  allocInfo.pSetLayouts = &l_layout;

  VkDescriptorSet l_descSet;
  if (vkAllocateDescriptorSets(m_logicalDevice->GetDevice(), &allocInfo, &l_descSet ) != VK_SUCCESS) {
    throw std::runtime_error("VKDeferredShadingPass::CreateDescriptorSet() - failed to allocate descriptor sets!");
  }
  
  // personal ubos here
  std::vector<size_t> l_uboSizes = a_pipeline->GetObjUboSizes();
  std::vector< std::shared_ptr<VulkanMemoryChunk> > l_uboMemBuffer = {nullptr,nullptr};
  if( l_uboSizes.size() > 0 )
    l_uboMemBuffer[0] = m_memory->CreateUniformBuffer( l_uboSizes[0] );
  if( l_uboSizes.size() > 1 )
    l_uboMemBuffer[1] = m_memory->CreateUniformBuffer( l_uboSizes[1] );
  reinterpret_cast<VulkanRenderable*>( a_renderable->GetExtra() )->Init(l_descSet, l_uboMemBuffer[0], l_uboMemBuffer[1] );
  
  
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
  
  // temporary containers for VkDescriptorBufferInfo and VkDescriptorImageInfo
  std::vector<VkDescriptorBufferInfo> m_tmpbufferInfo;
  std::vector<VkDescriptorImageInfo> m_tmpImageInfo;
  
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
          l_bufferInfo.buffer = m_uboMemBuffers[1]->m_buffer->m_buffer;
          l_bufferInfo.offset = m_uboMemBuffers[1]->GetBufferOffset();
          l_bufferInfo.range = m_uboMemBuffers[1]->m_size;
        }
        else if( l_globals[l_globalsToGo] == GLOBAL_LIGHT_FRAG_DATA )
        {
          l_bufferInfo.buffer = m_uboMemBuffers[0]->m_buffer->m_buffer;
          l_bufferInfo.offset = m_uboMemBuffers[0]->GetBufferOffset();
          l_bufferInfo.range = m_uboMemBuffers[0]->m_size;
        }
      }
      
      m_tmpbufferInfo.push_back(l_bufferInfo);
      
      VkWriteDescriptorSet descriptorWrite = {};
      descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrite.dstSet = l_descSet;
      descriptorWrite.dstBinding = l_bindings[i].binding;
      descriptorWrite.dstArrayElement = 0;
      descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      descriptorWrite.descriptorCount = 1;
      descriptorWrite.pBufferInfo = &m_tmpbufferInfo.back();
      
      l_descriptorSetWrites.push_back(descriptorWrite);
    }
    else if( l_bindings[i].descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER )
    {
      SceneControl::TexturedSceneNode* l_texturedSceneNode = reinterpret_cast<SceneControl::TexturedSceneNode*>(a_renderable);
      std::shared_ptr<ITexture> l_texture = l_texturedSceneNode->GetTexture(l_imagesIndex);
      if( l_texture )
      {
        VKATexture* l_tempTexture = reinterpret_cast<VKATexture*>( l_texture->GetExtra() );
        if( l_tempTexture )
        {
          
          VkDescriptorImageInfo l_imageInfo = {};
          
          l_imageInfo.sampler = l_tempTexture->GetSampler()->m_sampler; // VkSampler                      
          l_imageInfo.imageView = l_tempTexture->GetImage()->m_imageView;  // VkImageView
          l_imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;   // VkImageLayout
          
          m_tmpImageInfo.push_back(l_imageInfo);
          
          VkWriteDescriptorSet descriptorWrite = {};
          descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
          descriptorWrite.dstSet = l_descSet;
          descriptorWrite.dstBinding = l_bindings[i].binding;
          descriptorWrite.dstArrayElement = 0;
          descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
          descriptorWrite.descriptorCount = 1;
          descriptorWrite.pBufferInfo = nullptr;
          descriptorWrite.pImageInfo = &m_tmpImageInfo.back();

          l_descriptorSetWrites.push_back(descriptorWrite);
        }
        l_imagesIndex++;
      }  
    }
    else if( l_bindings[i].descriptorType == VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT )
    {
      // this is the order of the geometry pass attachments
      // diffuse attachment
      // normals attachment
      // specular
      // emissive - final image
      // stencil depth
      
      VkDescriptorImageInfo l_imageInfo = {};
      l_imageInfo.sampler = NULL; // VkSampler                      
      l_imageInfo.imageView = m_attachmentImages[l_inputAttachmentsIndex]->m_imageView;  // VkImageView
      l_imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;   // VkImageLayout
      
      m_tmpImageInfo.push_back(l_imageInfo);
      
      VkWriteDescriptorSet descriptorWrite = {};
      descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrite.dstSet = l_descSet;
      descriptorWrite.dstBinding = l_bindings[i].binding;
      descriptorWrite.dstArrayElement = 0;
      descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
      descriptorWrite.descriptorCount = 1;
      descriptorWrite.pBufferInfo = nullptr;
      descriptorWrite.pImageInfo = &m_tmpImageInfo.back();
      
      l_descriptorSetWrites.push_back(descriptorWrite);
      
      l_inputAttachmentsIndex++;
      // skip the emissive - final image attachment - we need only the other four
      if(l_inputAttachmentsIndex == 3)
      l_inputAttachmentsIndex = 4;
        
    }
  }
  vkUpdateDescriptorSets(m_logicalDevice->GetDevice(), l_descriptorSetWrites.size(), l_descriptorSetWrites.data(), 0, nullptr);

}



void RenderControl::VKDeferredShadingPass::CreateGeometryDescriptorSet(const std::shared_ptr<VKPipeline>& a_pipeline, IRenderable* a_renderable)
{
  VkDescriptorSetLayout l_layout = a_pipeline->GetDescriptorSetLayout();
  VkDescriptorSetAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = m_descriptorPool;
  allocInfo.descriptorSetCount = 1;
  allocInfo.pSetLayouts = &l_layout;

  VkDescriptorSet l_descSet;
  if (vkAllocateDescriptorSets(m_logicalDevice->GetDevice(), &allocInfo, &l_descSet ) != VK_SUCCESS) {
    throw std::runtime_error("VKDeferredShadingPass::CreateGeometryDescriptorSet() - failed to allocate descriptor sets!");
  }
  
  // personal ubos here
  std::vector<size_t> l_uboSizes = a_pipeline->GetObjUboSizes();
  std::vector< std::shared_ptr<VulkanMemoryChunk> > l_uboMemBuffer = {nullptr,nullptr};
  if( l_uboSizes.size() > 0 )
    l_uboMemBuffer[0] = m_memory->CreateUniformBuffer( l_uboSizes[0] );
  if( l_uboSizes.size() > 1 )
    l_uboMemBuffer[1] = m_memory->CreateUniformBuffer( l_uboSizes[1] );
  reinterpret_cast<VulkanRenderable*>( a_renderable->GetExtra() )->Init(l_descSet, l_uboMemBuffer[0], l_uboMemBuffer[1] );
  
  


  // descriptor layout bindings are always global first, then object bindings
  std::vector<VkDescriptorSetLayoutBinding> l_bindings = a_pipeline->GetDescriptorSetLayoutBindings();

  
  // m_descLayoutBindings = {l_vertexBindingGlobal, l_vertexBindingObject, l_fragBindingObject};
  // the others are samplers
  // vertex global 
  VkDescriptorBufferInfo l_bufferInfo0 = {};
  l_bufferInfo0.buffer = m_uboMemBuffers[1]->m_buffer->m_buffer;
  l_bufferInfo0.offset = m_uboMemBuffers[1]->GetBufferOffset();
  l_bufferInfo0.range = m_uboMemBuffers[1]->m_size;
  
  VkWriteDescriptorSet descriptorWrite0 = {};
  descriptorWrite0.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrite0.dstSet = l_descSet;
  descriptorWrite0.dstBinding = l_bindings[0].binding;
  descriptorWrite0.dstArrayElement = 0;
  descriptorWrite0.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptorWrite0.descriptorCount = 1;
  descriptorWrite0.pBufferInfo = &l_bufferInfo0;

  // vertex object data 
  VkDescriptorBufferInfo l_bufferInfo1 = {};
  l_bufferInfo1.buffer = l_uboMemBuffer[0]->m_buffer->m_buffer;
  l_bufferInfo1.offset = l_uboMemBuffer[0]->GetBufferOffset();
  l_bufferInfo1.range = l_uboMemBuffer[0]->m_size;
  
  VkWriteDescriptorSet descriptorWrite1 = {};
  descriptorWrite1.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrite1.dstSet = l_descSet;
  descriptorWrite1.dstBinding = l_bindings[1].binding;
  descriptorWrite1.dstArrayElement = 0;
  descriptorWrite1.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptorWrite1.descriptorCount = 1;
  descriptorWrite1.pBufferInfo = &l_bufferInfo1;
  
  // fragment object data 
  VkDescriptorBufferInfo l_bufferInfo2 = {};
  l_bufferInfo2.buffer = l_uboMemBuffer[1]->m_buffer->m_buffer;
  l_bufferInfo2.offset = l_uboMemBuffer[1]->GetBufferOffset();
  l_bufferInfo2.range = l_uboMemBuffer[1]->m_size;
  
  VkWriteDescriptorSet descriptorWrite2 = {};
  descriptorWrite2.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrite2.dstSet = l_descSet;
  descriptorWrite2.dstBinding = l_bindings[2].binding;
  descriptorWrite2.dstArrayElement = 0;
  descriptorWrite2.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptorWrite2.descriptorCount = 1;
  descriptorWrite2.pBufferInfo = &l_bufferInfo2;
  
  std::vector< VkWriteDescriptorSet > l_descriptorSetWrites = {descriptorWrite1, descriptorWrite2 };
  std::vector< VkDescriptorImageInfo > l_imageDescriptors( l_bindings.size() - 3 );
  
  // then samplers
  SceneControl::TexturedSceneNode* l_texturedSceneNode = reinterpret_cast<SceneControl::TexturedSceneNode*>(a_renderable);
  for( int i = 0; i < l_imageDescriptors.size(); ++i)
  {
    std::shared_ptr<ITexture> l_texture = l_texturedSceneNode->GetTexture(i);
    if( l_texture )
    {
      
      VKATexture* l_tempTexture = reinterpret_cast<VKATexture*>( l_texture->GetExtra() );
      if( l_tempTexture )
      {
        l_imageDescriptors[i].sampler = l_tempTexture->GetSampler()->m_sampler; // VkSampler                      
        l_imageDescriptors[i].imageView = l_tempTexture->GetImage()->m_imageView;  // VkImageView
        l_imageDescriptors[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;   // VkImageLayout
      }
    }
  }

  
  for( int i = 3; i < l_bindings.size(); ++i)
  {
    VkWriteDescriptorSet descriptorWrite = {};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = l_descSet;
    descriptorWrite.dstBinding = l_bindings[i].binding;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = nullptr;
    descriptorWrite.pImageInfo = &l_imageDescriptors[i-3];

    l_descriptorSetWrites.push_back(descriptorWrite);
  }
  
  
  vkUpdateDescriptorSets(m_logicalDevice->GetDevice(), l_descriptorSetWrites.size(), l_descriptorSetWrites.data(), 0, nullptr);
  printf("vkUpdateDescriptorSets - done\n");

}















