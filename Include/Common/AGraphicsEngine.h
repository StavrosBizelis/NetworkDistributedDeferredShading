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
  
  virtual void Update(const double& a_deltaTime)
  {
    if( m_sceneManager )
      m_sceneManager->UpdateScene(a_deltaTime);
    if( m_renderPassPipeline )
      m_renderPassPipeline->RenderAll();
  }
  
  RenderControl::RenderPassPipeline* GetRenderManager() const { return m_renderPassPipeline; }
  SceneControl::SceneManager* GetSceneManager() const { return m_sceneManager; }
  MaterialControl::IMaterialManager* GetMaterialManager() const { if( m_renderPassPipeline ) return m_renderPassPipeline->GetMaterialManager(); return nullptr; }
  glm::vec2 GetDimensions() const { return m_dimensions; }
  virtual void SetDimensions(const glm::vec2& a_dim) 
  { 
    m_dimensions = a_dim;
    GetDeferredRenderPass()->GetCamera()->SetDimentsions(a_dim);
  }
  
  virtual RenderControl::ADeferredShadingPass* GetDeferredRenderPass() const = 0;
  
  virtual IShapeFactory* GetShapeFactory() const = 0;
  virtual ITextureFactory* GetTextureFactory() const = 0;
};