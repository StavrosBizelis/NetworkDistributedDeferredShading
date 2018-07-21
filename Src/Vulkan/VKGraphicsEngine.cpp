#include "Vulkan/VKGraphicsEngine.h"

#include "Vulkan/RenderControl/VKDeferredShadingPass.h"
// #include "Vulkan/RenderControl/VKCompositionPass.h"
// #include "Vulkan/MaterialControl/VKMaterialManager.h"

// #include "Vulkan/Shapes/VKShapeFactory.h"
// #include "Vulkan/Textures/VKTextureFactory.h"

#include <iostream>


VKGraphicsEngine::VKGraphicsEngine(const glm::vec2& a_resolution)
: VKGraphicsEngine(a_resolution, a_resolution, glm::vec4(0,0, a_resolution.x, a_resolution.y) ) {}

VKGraphicsEngine::VKGraphicsEngine(const glm::vec2& a_resolution, const glm::vec2 &a_partialResolution, const glm::vec4& a_viewportSettings ) 
  : AGraphicsEngine(a_resolution, a_partialResolution, a_viewportSettings), m_deferredShadingPass(nullptr), m_compositionPass(nullptr), m_updateRegistry(std::make_shared<std::vector<VulkanRenderable*> >() )
  {
    
    std::vector<const char*> l_requiredInstanceExtensions = 
    {"VK_KHR_win32_surface", "VK_KHR_surface"};
    
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
  try{ 
    m_driver->Init(m_resolutionPart);
  }
  catch(std::runtime_error& e)
  {
    std::cout << e.what();
    return;
  }
  
  // MaterialControl::IMaterialManager* l_materialManager = new MaterialControl::VKMaterialManager();
  // m_renderPassPipeline->SetMaterialManager(l_materialManager);
  
  // m_shapeFactory = new VKShapeFactory();
  // m_shapeFactory->Init();
  // m_textureFactory = new VKTextureFactory();
  
  
  // if(a_composite)
  // {
    // m_compositionPass = new RenderControl::VKCompositionPass(m_resolution, m_sceneManager, m_shapeFactory, m_textureFactory, a_subparts);
    // m_compositionPass->Init();
    // m_compositionPass->SetSceenOutputAttachment(0);
    // m_renderPassPipeline->PushBack(m_compositionPass);
  // }
  // else
  // {
    std::cout << "before new deferred shading pass\n";
    m_deferredShadingPass = new RenderControl::VKDeferredShadingPass( m_driver->GetLogicalDeviceManager(), m_driver->GetSelectedPhysicalDevice(), m_driver->GetLogicalDeviceManager()->GetMemoryManager(),
                                                                      m_driver->GetLogicalDeviceManager()->GetGraphicsQueue(),m_driver->GetLogicalDeviceManager()->GetPresentQueue(), m_driver->GetLogicalDeviceManager()->GetQueueFamilyIndices(),
                                                                      m_resolution, m_resolutionPart, m_viewPortSettings );
    std::cout << "before Init() deferred shading pass\n";
    m_deferredShadingPass->Init();
    std::cout << "after Init() deferred shading pass\n";
    // m_deferredShadingPass->SetSceenOutputAttachment(3);
    // m_renderPassPipeline->PushBack(m_deferredShadingPass);    
  // }
  

}

virtual void VKGraphicsEngine::Update(const double& a_deltaTime)
{
  if( m_sceneManager )
  {
    // update scene objects
    m_sceneManager->UpdateScene(a_deltaTime);
  
  // update vulkan Renderables uniform buffer
  char* l_mappedBuffer = nullptr;
  std::shared_ptr< VulkanMemory > l_memory = m_driver->GetLogicalDeviceManager()->GetMemoryManager();
  
  // update all the values in the big uniform buffer
  void* l_data = l_memory->GetMemoryPool(3)->MapMemory();
  std::shared_ptr< std::vector<VulkanRenderable*> > l_registry = m_sceneManager->GetUpdateRegistry();
  for( std::vector<VulkanRenderable*>::iterator l_iter = l_registry->begin(); l_iter != l_registry->end(); ++l_iter )
  {
    (*l_iter)->VulkanUpdate(l_data);
  }
  l_memory->GetMemoryPool(3)->UnMapMemory();
  
  
  // render pass -> re-record if needed -> render 
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