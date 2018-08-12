#pragma once

#include "Common/AGraphicsEngine.h"
#include <memory>
#include <vulkan/vulkan.h>
#include <Vulkan/Core/VulkanDriver.h>



class VKGraphicsEngine : public AGraphicsEngine
{
private:
  RenderControl::ADeferredShadingPass* m_deferredShadingPass;
  RenderControl::ACompositionPass* m_compositionPass;

  IShapeFactory* m_shapeFactory;
  ITextureFactory* m_textureFactory;
  
  VkInstance m_instance;
  VkSurfaceKHR m_surface;
  
  std::unique_ptr<VulkanDriver> m_driver;
  
  std::vector<char> m_DataToUpdate;
public:
  VKGraphicsEngine(const glm::vec2& a_resolution = glm::vec2(0) );
  VKGraphicsEngine(const glm::vec2& a_resolution, const glm::vec2 &a_partialResolution, const glm::vec4& a_viewportSettings);
  virtual ~VKGraphicsEngine();
  
  virtual void Init(bool a_composite = false, unsigned int a_subparts = 0, unsigned int a_compressResult = 0);
  virtual void Update(const double& a_deltaTime);

  virtual RenderControl::ADeferredShadingPass* GetDeferredRenderPass()  const;
  virtual RenderControl::ACompositionPass* GetCompositionPass() const;
  
  virtual IShapeFactory* GetShapeFactory()  const;
  virtual ITextureFactory* GetTextureFactory() const;
  

  
  
  VkInstance *GetVkInstance(){ if( m_driver) return m_driver->GetVkInstance(); return nullptr;}
  VkSurfaceKHR *GetVkSurface(){ if( m_driver) return m_driver->GetVkSurface(); return nullptr;}
  
};