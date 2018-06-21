#pragma once

#include "Common/SceneControl/SceneManager.h"
#include "Common/RenderControl/RenderPassPipeline.h"
#include "Common/RenderControl/ADeferredShadingPass.h"
#include "Common/Shapes/IShapeFactory.h"
#include "Common/Textures/ITextureFactory.h"
#include "Common/MaterialControl/IMaterialManager.h"

class AGraphicsEngine
{
protected:
  SceneControl::SceneManager* m_sceneManager;
  RenderControl::RenderPassPipeline* m_renderPassPipeline;
  
  glm::vec2 m_dimensions;
public:

  AGraphicsEngine(const glm::vec2& a_dimensions): m_dimensions(a_dimensions), m_sceneManager(nullptr), m_renderPassPipeline(nullptr){}
  virtual ~AGraphicsEngine()
  {
    if( m_sceneManager ) 
      delete m_sceneManager;
    if( m_renderPassPipeline ) 
      delete m_renderPassPipeline;
  }  
  virtual void Init()
  {
    m_sceneManager = new SceneControl::SceneManager();
    m_renderPassPipeline = new RenderControl::RenderPassPipeline();
  }
  
  RenderControl::RenderPassPipeline* GetRenderManager() const { return m_renderPassPipeline; }
  SceneControl::SceneManager* GetSceneManager() const { return m_sceneManager; }
  MaterialControl::IMaterialManager* GetMaterialManager() const { if( m_renderPassPipeline ) return m_renderPassPipeline->GetMaterialManager(); return nullptr; }
  
  
  virtual RenderControl::ADeferredShadingPass* GetDeferredRenderPass() const = 0;
  
  virtual IShapeFactory* GetShapeFactory() const = 0;
  virtual ITextureFactory* GetTextureFactory() const = 0;
};