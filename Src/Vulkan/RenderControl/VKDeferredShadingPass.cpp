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
#include "Vulkan/Shapes/VKShapeFactory.h"
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
    m_logicalDevice(a_device), m_physicalDevice(a_physicalDevice), m_memory(a_memory), m_graphicsQueue(a_graphicsQueue), m_presentQueue(a_presentQueue), m_indices(a_indices)
{
  m_renderPass = NULL;
  m_frameBuffer = NULL;
  // m_pbos[0] = 0;
  // m_pbos[1] = 0;
  m_outputTextures.resize(5, 0);
  m_outputSamplers.resize(5, 0);
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
  CreateCommandPool();
  CreateSemaphores();
  CreateRenderPass();
  CreateFramebuffer();
  CreateCommandBuffers();
  
  // // init camera
  std::shared_ptr<CCamera> l_cam = GetCamera();

  l_cam->SetOrthographicProjectionMatrix((int)m_resolution.x, (int)m_resolution.y);
  l_cam->SetPerspectiveProjectionMatrix(45.0f, m_resolution.x / m_resolution.y, 0.5f, 7000.0f);

    
    // // init pbos
    // glGenBuffers(2, &m_pbos[0]);
    // glBindBufferARB(GL_PIXEL_PACK_BUFFER_EXT, m_pbos[0]);
    // glBufferDataARB(GL_PIXEL_PACK_BUFFER_EXT, 
                    // (m_resolutionPart.x * m_resolutionPart.y * 3 ), 
                    // NULL, 
                    // GL_STREAM_READ);

    // glBindBufferARB(GL_PIXEL_PACK_BUFFER_EXT, m_pbos[1]);
    // glBufferDataARB(GL_PIXEL_PACK_BUFFER_EXT, 
                    // (m_resolutionPart.x * m_resolutionPart.y * 3) , 
                    // NULL, 
                    // GL_STREAM_READ);
  
    // glBindBufferARB(GL_PIXEL_PACK_BUFFER_EXT, 0);
  
  		// // init camera
		// std::shared_ptr<CCamera> l_cam = GetCamera();

		// l_cam->SetOrthographicProjectionMatrix((int)m_resolution.x, (int)m_resolution.y);
		// l_cam->SetPerspectiveProjectionMatrix(45.0f, m_resolution.x / m_resolution.y, 0.5f, 7000.0f);



		// // create framebuffer textures
		// glGenTextures(m_outputTextures.size(), &m_outputTextures[0]);
		// glGenSamplers(m_outputSamplers.size(), &m_outputSamplers[0]);

		// // create default samplers
		// for (unsigned int i = 0; i<m_outputSamplers.size(); ++i)
		// {
			// glSamplerParameterf(m_outputSamplers[i], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			// glSamplerParameterf(m_outputSamplers[i], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			// glSamplerParameterf(m_outputSamplers[i], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			// glSamplerParameterf(m_outputSamplers[i], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// }


		// glm::vec2 l_res = m_resolutionPart;
		// // setup geometry frame buffer object and its textures
		// glGenFramebuffers(1, &m_fbo);
		// glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

		// // diffuse image 
		// glBindTexture(GL_TEXTURE_2D, m_outputTextures[0]);
		// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, (GLsizei)l_res.x, (GLsizei)l_res.y, 0, GL_RGB, GL_FLOAT, NULL);
		// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, (GLuint)m_outputTextures[0], 0);

		// // normals image
		// glBindTexture(GL_TEXTURE_2D, m_outputTextures[1]);
		// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, (GLsizei)l_res.x, (GLsizei)l_res.y, 0, GL_RGB, GL_FLOAT, NULL);
		// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, (GLuint)m_outputTextures[1], 0);

		// // specular
		// glBindTexture(GL_TEXTURE_2D, m_outputTextures[2]);
		// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, (GLsizei)l_res.x, (GLsizei)l_res.y, 0, GL_RGB, GL_FLOAT, NULL);
		// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, (GLuint)m_outputTextures[2], 0);

		// // final image
		// glBindTexture(GL_TEXTURE_2D, m_outputTextures[3]);
		// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, (GLsizei)l_res.x, (GLsizei)l_res.y, 0, GL_RGB, GL_FLOAT, NULL);
		// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, (GLuint)m_outputTextures[3], 0);


		// // stencil depth
		// glBindTexture(GL_TEXTURE_2D, m_outputTextures[4]);
		// glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, (GLsizei)l_res.x, (GLsizei)l_res.y, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, NULL);
		// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, (GLuint)m_outputTextures[4], 0/*mipmap level*/);

		


		// GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		// if (Status != GL_FRAMEBUFFER_COMPLETE)
		// {
			// printf("FB error, status: 0x%x\n", Status);
			// return false;
		// }

		
		// // restore default FBO
		// glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		return true;
}

void RenderControl::VKDeferredShadingPass::Render()
{
  GeometryPass();
		
  LightPass();
}

void RenderControl::VKDeferredShadingPass::OutputOnScreen()
{
  // unsigned int l_actualAttachment;
  // unsigned int l_actualBit = GL_COLOR_BUFFER_BIT;
  // (m_attachmentIndex == 4) ? (l_actualAttachment = GL_DEPTH_STENCIL_ATTACHMENT, l_actualBit = GL_DEPTH_BUFFER_BIT) : l_actualAttachment = GL_COLOR_ATTACHMENT0 + m_attachmentIndex;



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

void RenderControl::VKDeferredShadingPass::UpdateViewportSettings(const glm::vec2& a_resolutionPart, const glm::vec4& a_viewportSettings)
{
  Clear();
  m_resolutionPart = a_resolutionPart;
  m_viewPortSetting = a_viewportSettings;
  Init();
}

void RenderControl::VKDeferredShadingPass::GeometryPass()
{
  // glEnable(GL_CULL_FACE);


  // // bind and clear the other attachments
  // glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
  // GLenum bufs[4];
  // bufs[0] = GL_COLOR_ATTACHMENT0;	// diffuse
  // bufs[1] = GL_COLOR_ATTACHMENT1;	// normal
  // bufs[2] = GL_COLOR_ATTACHMENT2;	// spec
  // bufs[3] = GL_COLOR_ATTACHMENT3;	// emissive - final	
  // glDrawBuffers(4, &bufs[0]);



  // glm::vec2 l_resolution = GetResolution();
  // glViewport( m_viewPortSetting.x, m_viewPortSetting.y, 
              // m_viewPortSetting.z, m_viewPortSetting.w);



  // glClearColor(1, 0, 0, 1);
  // glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  // glDepthMask(GL_TRUE);
  // glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  
  

  // // setup camera matrix
  // glutil::MatrixStack modelViewMatrixStack;
  // modelViewMatrixStack.SetIdentity();
  // modelViewMatrixStack.LookAt(m_camera->GetPosition(), m_camera->GetView(), m_camera->GetUpVector());


  // ////////////////////////////////////////////////////////////////////////////////////
  // // RENDER OBJECTS HERE /////////////////////////////////////////////////////////////
  // ////////////////////////////////////////////////////////////////////////////////////
  // typedef std::unordered_map< std::shared_ptr<IShaderProgram>, std::vector<IRenderable*> >::iterator Iter;
  // typedef std::vector<IRenderable*>::iterator VectorIter;
  // for (Iter l_iter = m_toRender.begin(); l_iter != m_toRender.end(); ++l_iter)
  // {
    // l_iter->first->UseProgram();
    // l_iter->first->SetUniform("matrices.projMatrix", m_camera->GetPerspectiveProjectionMatrix() );
    // // used for frustum culling in geometry culling
    // l_iter->first->SetUniform("UCamFr.UCamPos", m_camera->GetPosition() );
    // l_iter->first->SetUniform("UCamFr.UFOV", m_camera->GetFov() );
    // l_iter->first->SetUniform("UCamFr.URatio", m_camera->GetAspectRatio() );
    // l_iter->first->SetUniform("UCamFr.UNear", m_camera->GetNearClippingPlane() );
    // l_iter->first->SetUniform("UCamFr.UFar", m_camera->GetFarClippingPlane() );
    // l_iter->first->SetUniform("UCamFr.UViewDir", m_camera->GetView());
    // l_iter->first->SetUniform("UCamFr.UUpVec", m_camera->GetUpVector());
    // FrustumData l_temp = m_camera->GetFrustumData();
    // l_iter->first->SetUniform("UCamFr.m_x", l_temp.m_x);
    // l_iter->first->SetUniform("UCamFr.m_y", l_temp.m_y);
    // l_iter->first->SetUniform("UCamFr.m_z", l_temp.m_z);
    // l_iter->first->SetUniform("UCamFr.m_tangent", l_temp.m_tangent);
    // l_iter->first->SetUniform("UCamFr.m_width", l_temp.m_width);
    // l_iter->first->SetUniform("UCamFr.m_height", l_temp.m_height);



    // std::vector<IRenderable*>& l_tmp = l_iter->second;
    // for (VectorIter l_iter2 = l_tmp.begin(); l_iter2 != l_tmp.end(); ++l_iter2)
      // (*l_iter2)->Render(modelViewMatrixStack);
      
  // }
}
void RenderControl::VKDeferredShadingPass::LightPass()
{
    // // setup camera matrix
  // glutil::MatrixStack projModelViewMatrixStack;
  // projModelViewMatrixStack.SetIdentity();
  // projModelViewMatrixStack.ApplyMatrix(*m_camera->GetPerspectiveProjectionMatrix());
  // projModelViewMatrixStack.LookAt(m_camera->GetPosition(), m_camera->GetView(), m_camera->GetUpVector());
  

  // // bind the textures
  // for (unsigned int i = 0; i < m_outputTextures.size() - 2; ++i)
  // {
    // glActiveTexture(GL_TEXTURE0 + i);
    // glBindTexture(GL_TEXTURE_2D, m_outputTextures[i]);
    // glBindSampler(i, m_outputSamplers[i]);
  // }		
  // // depth
  // glActiveTexture(GL_TEXTURE0 + 3);
  // glBindTexture(GL_TEXTURE_2D, m_outputTextures[4]);
  // glBindSampler(3, m_outputSamplers[4]);
  

  // glEnable(GL_DEPTH_CLAMP);

  // ////////////////////////////////////////////////////////////////////////////////////
  // // RENDER OBJECTS HERE /////////////////////////////////////////////////////////////
  // ////////////////////////////////////////////////////////////////////////////////////
  
  // typedef std::set<IRenderable*>::iterator iterator;
  // for (iterator l_iter = m_lights.begin(); l_iter != m_lights.end(); ++l_iter)
  // {
    // std::shared_ptr<IShaderProgram> l_lightMaterial = (*l_iter)->GetMaterial(0);
    // std::shared_ptr<IShaderProgram> l_nullMaterial = (*l_iter)->GetMaterial(1);
    // glm::mat4 l_invProjViewMat = glm::inverse(projModelViewMatrixStack.Top() );

    // if (l_lightMaterial)
    // {
      // glm::vec2 l_resDiv = glm::vec2(1.0f) / GetResolution();
      // glm::vec3 l_camPos = m_camera->GetPosition();
      // l_lightMaterial->SetOnUse("LightPassUse",
        // [l_resDiv, l_camPos, l_invProjViewMat](IShaderProgram* l_me)
        // {
          // l_me->SetUniform("UScreenResDiv", l_resDiv);
          // l_me->SetUniform("UCamPos", l_camPos);
          // l_me->SetUniform("UInverseViewProjectionMatrix", l_invProjViewMat);
        // }
      // );


    // }
    // if (l_nullMaterial)
    // {
      // l_nullMaterial->SetOnUse("LightPassUse",
        // [l_invProjViewMat](IShaderProgram* l_me)
        // {
          // l_me->SetUniform("UInverseViewProjectionMatrix", l_invProjViewMat);
        // }
      // );


    // }

    
    // (*l_iter)->Render(projModelViewMatrixStack);
  // }
  // glDisable(GL_DEPTH_CLAMP);
}

void RenderControl::VKDeferredShadingPass::Clear()
{
  if(m_frameBuffer)
    vkDestroyFramebuffer(m_logicalDevice->GetDevice(), m_frameBuffer, nullptr);
  
  if(m_renderPass)
    vkDestroyRenderPass(m_logicalDevice->GetDevice(), m_renderPass, nullptr);
  
  // if (m_fbo != 0)
  // {
    // glDeleteFramebuffers(1, &m_fbo);
    // m_fbo = 0;
  // }
  // for( unsigned int i = 0; i < 2; ++i)
  // {
    // if( m_pbos[i] != 0 )
    // {
      // glDeleteBuffers(1,&m_pbos[i]);
      // m_pbos[i] = 0;
    // }
  // }
  
  // for (int i = 0; i < 5; ++i)
  // {
    // if (m_outputTextures[i])
    // {
      // glDeleteTextures(1, &m_outputTextures[i]);
      // m_outputTextures[i] = 0;
    // }
    // if (m_outputSamplers[i])
    // {
      // glDeleteSamplers(1, &m_outputSamplers[i]);
      // m_outputSamplers[i] = 0;
    // }
  // }
}

void RenderControl::VKDeferredShadingPass::SetMaterialManager(MaterialControl::IMaterialManager* a_materialManager)
{
  ADeferredShadingPass::SetMaterialManager(a_materialManager);
  // initiate materials
  std::shared_ptr<IShaderProgram> GeometryMaterial =
    a_materialManager->GetGeometryPassMaterial(
      "..\\Assets\\GLSL_shaders\\GeometryShader.vert",
      "..\\Assets\\GLSL_shaders\\GeometryShader.frag"
    );

  std::shared_ptr<IShaderProgram> GeometryMaterialEmissiveMap =
    GetMaterialManager()->GetEmissiveGeometryPassMaterial(
      "..\\Assets\\GLSL_shaders\\GeometryShader.vert",
      "..\\Assets\\GLSL_shaders\\GeometryEmissiveShader.frag"
    );

  std::shared_ptr<IShaderProgram> GeometryMaterialDiffuseMap =
    a_materialManager->GetColourGeometryPassMaterial(
      "..\\Assets\\GLSL_shaders\\GeometryShader.vert",
      "..\\Assets\\GLSL_shaders\\GeometryColourShader.frag"
    );

  std::shared_ptr<IShaderProgram> GeometryMaterialDiffuseNormalMap =
    a_materialManager->GetColourNormalGeometryPassMaterial(
      "..\\Assets\\GLSL_shaders\\GeometryShader.vert",
      "..\\Assets\\GLSL_shaders\\GeometryColourNormalShader.frag"
    );

  std::shared_ptr<IShaderProgram> GeometryMaterialDiffuseNormalSpecMap =
    a_materialManager->GetColourNormalSpecGeometryPassMaterial(
      "..\\Assets\\GLSL_shaders\\GeometryShader.vert",
      "..\\Assets\\GLSL_shaders\\GeometryColourNormalSpecShader.frag"
    );

  std::shared_ptr<IShaderProgram> GeometryMaterialDiffuseNormalSpecHardnessMap =
    a_materialManager->GetColourNormalSpecHardnessGeometryPassMaterial(
      "..\\Assets\\GLSL_shaders\\GeometryShader.vert",
      "..\\Assets\\GLSL_shaders\\GeometryColourNormalSpecHardnessShader.frag"
    );

  std::shared_ptr<IShaderProgram> GeometryMaterialDiffuseNormalSpecHardnessEmissionMap =
    a_materialManager->GetColourNormalSpecHardnessEmissionGeometryPassMaterial(
      "..\\Assets\\GLSL_shaders\\GeometryShader.vert",
      "..\\Assets\\GLSL_shaders\\GeometryColourNormalSpecHardnessEmissiveShader.frag"
    );
  

  std::shared_ptr<IShaderProgram> GeometrySkybox =
    a_materialManager->GetSkyCubeMaterial(
      "..\\Assets\\GLSL_shaders\\GeometrySkyBoxShader.vert",
      "..\\Assets\\GLSL_shaders\\GeometrySkyBoxShader.frag"
    );

  // create appropriate keys
  m_toRender[GeometryMaterialDiffuseNormalSpecHardnessEmissionMap];
  m_toRender[GeometryMaterialDiffuseNormalSpecHardnessMap];
  m_toRender[GeometryMaterialDiffuseNormalSpecMap];
  m_toRender[GeometryMaterialDiffuseNormalMap];
  m_toRender[GeometryMaterialDiffuseMap];
  m_toRender[GeometryMaterialEmissiveMap];
  m_toRender[GeometryMaterial];
  m_toRender[GeometrySkybox];


  std::shared_ptr<IShaderProgram> l_nullMaterial =
    a_materialManager->GetStencilLightPassMaterial(
      "..\\Assets\\GLSL_shaders\\NullShader.vert",
      "..\\Assets\\GLSL_shaders\\NullShader.frag"
    );

  std::shared_ptr<IShaderProgram> l_pointLightMaterial =
    a_materialManager->GetPointLightPassMaterial(
      "..\\Assets\\GLSL_shaders\\PointLightShader.vert",
      "..\\Assets\\GLSL_shaders\\PointLightShader.frag"
    );

  std::shared_ptr<IShaderProgram> l_spotLightMaterial =
    a_materialManager->GetSpotLightPassMaterial(
      "..\\Assets\\GLSL_shaders\\SpotLightShader.vert",
      "..\\Assets\\GLSL_shaders\\SpotLightShader.frag"
    );

  std::shared_ptr<IShaderProgram> l_directionalLightMaterial =
    a_materialManager->GetFullScreenLightPassMaterial(
      "..\\Assets\\GLSL_shaders\\DirectionalLightShader.vert",
      "..\\Assets\\GLSL_shaders\\DirectionalLightShader.frag"
    );
  
}

bool RenderControl::VKDeferredShadingPass::PackTexture( Network::NetworkMsgPtr& a_outMsg)
{
  // m_pboIndex = !m_pboIndex;
  // bool l_nextIndex = !m_pboIndex;
  
  // glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
  // glReadBuffer(GL_COLOR_ATTACHMENT0 + m_attachmentIndex);
  
  
  // glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pbos[m_pboIndex] );
  // glReadPixels(0, 0, (GLsizei)m_resolutionPart.x, (GLsizei)m_resolutionPart.y, GL_BGR, GL_UNSIGNED_BYTE, 0);  // returns immediately
  

  // glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pbos[l_nextIndex]);
  // char* l_ptr = (char*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
  
  // if(l_ptr)
  // {
    // // IFDBG( std::cout << "Packed Actual Texture " << std::endl; );
    // a_outMsg->CreateRenderResultMsg(l_ptr, m_resolutionPart, 0, 8 );
    // glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
  // }
  // else
  // {
    // IFDBG( std::cout << "Packed pseudo Texture " << std::endl; );
    // char* l_tmp = new char[3];
    // l_tmp[0] = 0;
    // l_tmp[1] = 0;
    // l_tmp[2] = 0;
    // a_outMsg->CreateRenderResultMsg(l_tmp, m_resolutionPart );
    // delete l_tmp;
  // }
  // // // back to conventional pixel operation
  // glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
  
  // return l_ptr != nullptr;
  return false;
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

  if (vkCreateSemaphore(m_logicalDevice->GetDevice(), &semaphoreInfo, nullptr, &m_imageAvailableSemaphore) != VK_SUCCESS ||
      vkCreateSemaphore(m_logicalDevice->GetDevice(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphore) != VK_SUCCESS) 
  {
    throw std::runtime_error("failed to create semaphores!");
  }
}

void RenderControl::VKDeferredShadingPass::CreateRenderPass()
{
    // init attachments
  // no swapchain attachments for deferred shading pass
  // create the attachments image
  m_attachmentImages.reserve(5);
  try{
    // diffuse attachment
    m_attachmentImages.push_back( m_memory->CreateAttachmentTexture(m_resolutionPart.x, m_resolutionPart.y, VK_FORMAT_R8G8B8A8_UNORM ) );
    // normals attachment
    m_attachmentImages.push_back( m_memory->CreateAttachmentTexture(m_resolutionPart.x, m_resolutionPart.y, VK_FORMAT_R16G16B16A16_SFLOAT ) );
    // specular
    m_attachmentImages.push_back( m_memory->CreateAttachmentTexture(m_resolutionPart.x, m_resolutionPart.y, VK_FORMAT_R8G8B8A8_UNORM ) );
    // emissive - final image
    // BREAKS HERE
    m_attachmentImages.push_back( m_memory->CreateAttachmentToDownloadTexture(m_resolutionPart.x, m_resolutionPart.y, VK_FORMAT_R8G8B8A8_UNORM ) );
    // stencil depth
    m_attachmentImages.push_back( m_memory->CreateStencilDepthAttachmentTexture(m_resolutionPart.x, m_resolutionPart.y ) );
  }
  catch(std::runtime_error& e)
  {
    std::cout<< e.what();
  }

  
  std::vector< VkAttachmentDescription > l_attachments(5);
  // create attachments
  for( unsigned int i = 0; i < 5; ++i)
  {
    l_attachments[i].format = VkFormat(m_attachmentImages[i]->m_format);
    l_attachments[i].samples = VK_SAMPLE_COUNT_1_BIT;
    l_attachments[i].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    l_attachments[i].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    l_attachments[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    l_attachments[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    
    l_attachments[i].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    l_attachments[i].finalLayout = VK_IMAGE_LAYOUT_GENERAL; // dont care since they are not going to be used for anything else
    
    if( i == 3 )  //if it is the final image stencil attachment
    {
      l_attachments[i].finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;  
    }
    
    if( i == 4 )  //if it is the depth stencil attachment
    {
      l_attachments[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
      l_attachments[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
    }    
  }
  

  // create attachment references 
  // geometry sub pass references
  std::vector <VkAttachmentReference> l_colourAttachmentRefs(4);
  for( unsigned int i = 0; i < 4; ++i)
  {
    l_colourAttachmentRefs[i].attachment = i;
    l_colourAttachmentRefs[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  }
  
  // light pass references
  std::vector <VkAttachmentReference> l_lightInputAttachmentRefs(4);
  for( unsigned int i = 0; i < 3; ++i)
  {
    l_lightInputAttachmentRefs[i].attachment = i;
    l_lightInputAttachmentRefs[i].layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  }  
  l_lightInputAttachmentRefs[3].attachment = 4;
  l_lightInputAttachmentRefs[3].layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  
  VkAttachmentReference l_lightColourAttachment = {};
  l_lightColourAttachment.attachment = 3;
  l_lightColourAttachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  
  
  VkAttachmentReference l_depthAttachmentRef = {};
  l_depthAttachmentRef.attachment = 4;
  l_depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  
  
  
  // create subpasses
  std::vector<VkSubpassDescription> l_subpasses(10);
  // geometry sub pass
  for( unsigned int i = 0; i < 8; ++i )
  {
    l_subpasses[i].inputAttachmentCount = 0;
    l_subpasses[i].colorAttachmentCount = l_colourAttachmentRefs.size();
    l_subpasses[i].pColorAttachments = l_colourAttachmentRefs.data();
    l_subpasses[i].pDepthStencilAttachment = &l_depthAttachmentRef;
  }
  // light subpass
  for( unsigned int i= 8; i < 10; ++i)
  {
    l_subpasses[i].inputAttachmentCount = l_lightInputAttachmentRefs.size();
    l_subpasses[i].pInputAttachments = l_lightInputAttachmentRefs.data();
    l_subpasses[i].colorAttachmentCount = 1;
    l_subpasses[i].pColorAttachments = &l_lightColourAttachment;
    l_subpasses[i].pDepthStencilAttachment = &l_depthAttachmentRef;
  }
  
  // dependencies - two major types of dependencies
  // geometry passes depend on the external
  std::vector< VkSubpassDependency> l_dependencies(3);
  for( unsigned int i = 0; i < 8; ++i )
  {
    l_dependencies[i].srcSubpass = VK_SUBPASS_EXTERNAL;
    l_dependencies[i].dstSubpass = i;
    l_dependencies[i].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    l_dependencies[i].srcAccessMask = 0;
    l_dependencies[i].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    l_dependencies[i].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  }
  // light passes depend on the geometry ones
  for( unsigned int i = 8; i < 10; ++i)
  {
    for( unsigned int j = 0; j < 8; ++j )
    {
      l_dependencies[i].srcSubpass = j;
      l_dependencies[i].dstSubpass = i;
      l_dependencies[i].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      l_dependencies[i].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      l_dependencies[i].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
      l_dependencies[i].dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT; 
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
    std::vector<VkImageView> l_attachmentsImageViews;
    for(unsigned int i = 0; i < m_attachmentImages.size(); ++i)    
      l_attachmentsImageViews.push_back(m_attachmentImages[i]->m_imageView);
    
    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = m_renderPass;
    framebufferInfo.attachmentCount = l_attachmentsImageViews.size();
    framebufferInfo.pAttachments = l_attachmentsImageViews.data();
    framebufferInfo.width = m_resolutionPart.x;
    framebufferInfo.height = m_resolutionPart.y;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(m_logicalDevice->GetDevice(), &framebufferInfo, nullptr, &m_frameBuffer ) != VK_SUCCESS)
      throw std::runtime_error("VKDeferredShadingPass::Init() - failed to create framebuffer!");
}

void RenderControl::VKDeferredShadingPass::CreateCommandBuffers()
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
  m_pipelines[8] = std::make_shared<VKLightPassPipeline>(m_logicalDevice, m_renderPass, CreatePipelineShaderCreateInfo(l_pointVert, l_pointFrag), 7, m_resolutionPart, m_viewPortSetting );
  // directional light pass
  m_pipelines[9] = std::make_shared<VKDirLightPassPipeline>(m_logicalDevice, m_renderPass, CreatePipelineShaderCreateInfo(l_directionalVert, l_directionalFrag), 8, m_resolutionPart, m_viewPortSetting );
  
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
}




















