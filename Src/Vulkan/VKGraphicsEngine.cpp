#include "Vulkan/VKGraphicsEngine.h"

// #include "Vulkan/RenderControl/VKDeferredShadingPass.h"
// #include "Vulkan/RenderControl/VKCompositionPass.h"
// #include "Vulkan/MaterialControl/VKMaterialManager.h"

// #include "Vulkan/Shapes/VKShapeFactory.h"
// #include "Vulkan/Textures/VKTextureFactory.h"

#include "gl/include/glew.h"
#include "gl/include/wglew.h"
#include <iostream>


VKGraphicsEngine::VKGraphicsEngine(const glm::vec2& a_resolution)
: VKGraphicsEngine(a_resolution, a_resolution, glm::vec4(0,0, a_resolution.x, a_resolution.y) ) {}

VKGraphicsEngine::VKGraphicsEngine(const glm::vec2& a_resolution, const glm::vec2 &a_partialResolution, const glm::vec4& a_viewportSettings ) 
  : AGraphicsEngine(a_resolution, a_partialResolution, a_viewportSettings), m_deferredShadingPass(nullptr), m_compositionPass(nullptr)
  {
    
    std::vector<const char*> l_requiredInstanceExtensions = 
    {"VK_KHR_win32_surface"};
    
    m_driver = std::make_unique<VulkanDriver>(l_requiredInstanceExtensions);
    
  }

VKGraphicsEngine::~VKGraphicsEngine()
{
  // if( m_shapeFactory ) 
    // delete m_shapeFactory;
  // if( m_textureFactory ) 
    // delete m_textureFactory;
}

void VKGraphicsEngine::Init(bool a_composite, unsigned int a_subparts)
{
  AGraphicsEngine::Init(a_composite, a_subparts);  
  // Init vulkan driver
  m_driver->Init(m_resolutionPart);
  
  
  // MaterialControl::IMaterialManager* l_materialManager = new MaterialControl::VKMaterialManager();
  // m_renderPassPipeline->SetMaterialManager(l_materialManager);
  
  // m_shapeFactory = new VKShapeFactory();
  // m_shapeFactory->Init();
  // m_textureFactory = new VKTextureFactory();
  
  
  if(a_composite)
  {
    // m_compositionPass = new RenderControl::VKCompositionPass(m_resolution, m_sceneManager, m_shapeFactory, m_textureFactory, a_subparts);
    // m_compositionPass->Init();
    // m_compositionPass->SetSceenOutputAttachment(0);
    // m_renderPassPipeline->PushBack(m_compositionPass);
  }
  else
  {
    // m_deferredShadingPass = new RenderControl::VKDeferredShadingPass( m_resolution, m_resolutionPart, m_viewPortSettings );
    // m_deferredShadingPass->Init();
    // m_deferredShadingPass->SetSceenOutputAttachment(3);
    // m_renderPassPipeline->PushBack(m_deferredShadingPass);    
  }
  

}

RenderControl::ADeferredShadingPass* VKGraphicsEngine::GetDeferredRenderPass() const
{
  return m_deferredShadingPass;
}

RenderControl::ACompositionPass* VKGraphicsEngine::GetCompositionPass() const
{
  return m_compositionPass;
}

IShapeFactory* VKGraphicsEngine::GetShapeFactory()  const
{
  return m_shapeFactory;
}

ITextureFactory* VKGraphicsEngine::GetTextureFactory() const
{
  return m_textureFactory;
}