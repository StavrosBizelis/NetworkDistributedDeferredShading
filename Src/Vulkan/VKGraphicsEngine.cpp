#include "Vulkan/VKGraphicsEngine.h"

#include "Vulkan/RenderControl/VKDeferredShadingPass.h"
#include "Vulkan/RenderControl/VKCompositionPass.h"
#include "Vulkan/SceneControl/VKSceneManager.h"
#include "Vulkan/RenderControl/VKCompositionPass.h"
#include <Common/Core/MyUtilities.h>
// #include "Vulkan/MaterialControl/VKMaterialManager.h"

#include "Vulkan/Shapes/VKShapeFactory.h"
#include "Vulkan/Textures/VKTextureFactory.h"

#include <iostream>


VKGraphicsEngine::VKGraphicsEngine(const glm::vec2& a_resolution)
: VKGraphicsEngine(a_resolution, a_resolution, glm::vec4(0,0, a_resolution.x, a_resolution.y) ) {}

VKGraphicsEngine::VKGraphicsEngine(const glm::vec2& a_resolution, const glm::vec2 &a_partialResolution, const glm::vec4& a_viewportSettings ) 
  : AGraphicsEngine(a_resolution, a_partialResolution, a_viewportSettings), m_deferredShadingPass(nullptr), m_compositionPass(nullptr)
  {
    
    std::vector<const char*> l_requiredInstanceExtensions = 
    {"VK_KHR_win32_surface", "VK_KHR_surface" };
    
    
    m_driver = std::make_unique<VulkanDriver>(l_requiredInstanceExtensions);
    
  }

VKGraphicsEngine::~VKGraphicsEngine()
{
  if( m_shapeFactory ) 
    delete m_shapeFactory;
  if( m_textureFactory ) 
    delete m_textureFactory;
}

void VKGraphicsEngine::Init(bool a_composite, unsigned int a_subparts, unsigned int a_compressResult)
{
  // AGraphicsEngine::Init(a_composite, a_subparts);  
  
  m_renderPassPipeline = new RenderControl::RenderPassPipeline();
  m_sceneManager = new SceneControl::VKSceneManager();

  
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
  
  m_shapeFactory = new VKShapeFactory(m_driver->GetLogicalDeviceManager()->GetMemoryManager());
  m_shapeFactory->Init();
  m_textureFactory = new VKTextureFactory(m_driver->GetLogicalDeviceManager()->GetMemoryManager());
  
  IFDBG( std::cout << "Driver Initialized \n"; );
  
  if(a_composite)
  {
    IFDBG( std::cout << "next: Create m_compositionPass \n"; );
    
    m_compositionPass = new RenderControl::VKCompositionPass(m_driver->GetLogicalDeviceManager(), m_driver->GetSelectedPhysicalDevice(), m_driver->GetLogicalDeviceManager()->GetMemoryManager(),
                                                             m_driver->GetLogicalDeviceManager()->GetGraphicsQueue(),m_driver->GetLogicalDeviceManager()->GetPresentQueue(), m_driver->GetLogicalDeviceManager()->GetQueueFamilyIndices(),
                                                             m_resolution, m_sceneManager, m_shapeFactory, m_textureFactory, a_subparts);
    IFDBG( std::cout << "m_compositionPass Created \n"; );

    m_compositionPass->Init();
    
    IFDBG( std::cout << "m_compositionPass Initialized \n"; );
    // IFDBG( std::cout << "m_compositionPass Initialized \n"; );
    // m_compositionPass->SetSceenOutputAttachment(0);
    // m_renderPassPipeline->PushBack(m_compositionPass);
  }
  else
  {
    IFDBG( std::cout << "next: Create m_deferredShadingPass \n"; );
    
    m_deferredShadingPass = new RenderControl::VKDeferredShadingPass(m_driver->GetLogicalDeviceManager(), m_driver->GetSelectedPhysicalDevice(), m_driver->GetLogicalDeviceManager()->GetMemoryManager(),
                                                             m_driver->GetLogicalDeviceManager()->GetGraphicsQueue(),m_driver->GetLogicalDeviceManager()->GetPresentQueue(), m_driver->GetLogicalDeviceManager()->GetQueueFamilyIndices(),
                                                             m_resolution, m_resolutionPart, m_viewPortSettings,
                                                             m_sceneManager, m_shapeFactory, m_textureFactory, a_compressResult);
    IFDBG( std::cout << "m_deferredShadingPass Created \n"; );

    try
    {
      m_deferredShadingPass->Init();
    }
    catch(std::runtime_error& e)
    {
      std::cout << e.what();
    }
    IFDBG( std::cout << "m_deferredShadingPass Initialized \n"; );

    
    std::cout << "before new deferred shading pass\n";
    // m_deferredShadingPass = new RenderControl::VKDeferredShadingPass( m_driver->GetLogicalDeviceManager(), m_driver->GetSelectedPhysicalDevice(), m_driver->GetLogicalDeviceManager()->GetMemoryManager(),
                                                                      // m_driver->GetLogicalDeviceManager()->GetGraphicsQueue(),m_driver->GetLogicalDeviceManager()->GetPresentQueue(), m_driver->GetLogicalDeviceManager()->GetQueueFamilyIndices(),
                                                                      // m_resolution, m_resolutionPart, m_viewPortSettings,
                                                                      // m_sceneManager, m_shapeFactory, m_textureFactory  
                                                                    // );
    // std::cout << "before Init() deferred shading pass\n";
    // m_deferredShadingPass->Init();
    // std::cout << "after Init() deferred shading pass\n";
    // // m_deferredShadingPass->SetSceenOutputAttachment(3);
    // // m_renderPassPipeline->PushBack(m_deferredShadingPass);    
  }
  

}

void VKGraphicsEngine::Update(const double& a_deltaTime)
{
  if( m_sceneManager )
  {
    // update scene objects
    m_sceneManager->UpdateScene(a_deltaTime);
    // update vulkan Renderables uniform buffer
    std::shared_ptr< VulkanMemory > l_memory = m_driver->GetLogicalDeviceManager()->GetMemoryManager();
    
    // update all the values in the big uniform buffer
    std::shared_ptr< std::vector<VulkanRenderable*> > l_registry = ((SceneControl::VKSceneManager*)m_sceneManager)->GetUpdateRegistry();
    for( std::vector<VulkanRenderable*>::iterator l_iter = l_registry->begin(); l_iter != l_registry->end(); ++l_iter )
    {
      (*l_iter)->VulkanUpdate( m_DataToUpdate );
    }
    
    // map global data too
    if(m_compositionPass) static_cast<RenderControl::VKCompositionPass*>(m_compositionPass)->VulkanUpdate( m_DataToUpdate );
    else if(m_deferredShadingPass) static_cast<RenderControl::VKDeferredShadingPass*>(m_deferredShadingPass)->VulkanUpdate( m_DataToUpdate );
    
    void* l_data = l_memory->GetMemoryPool(3)->MapMemory();
    memcpy(l_data, &m_DataToUpdate[0], m_DataToUpdate.size() );
    l_memory->GetMemoryPool(3)->UnMapMemory();
    // m_DataToUpdate.resize(0);
    l_registry->clear();
  }
  
  
  // submit queues
  if(m_compositionPass) 
    m_compositionPass->Render();
  else if(m_deferredShadingPass)
    m_deferredShadingPass->Render();
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