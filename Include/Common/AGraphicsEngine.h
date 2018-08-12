#pragma once

#include "Common/SceneControl/SceneManager.h"
#include "Common/RenderControl/RenderPassPipeline.h"
#include "Common/RenderControl/ADeferredShadingPass.h"
#include "Common/RenderControl/ACompositionPass.h"
#include "Common/Shapes/IShapeFactory.h"
#include "Common/Textures/ITextureFactory.h"
#include "Common/MaterialControl/IMaterialManager.h"

class AGraphicsEngine
{
protected:
  SceneControl::SceneManager* m_sceneManager;
  RenderControl::RenderPassPipeline* m_renderPassPipeline;
  
  glm::vec2 m_resolution;
  glm::vec2 m_resolutionPart;
  glm::vec4 m_viewPortSettings;
public:

  AGraphicsEngine(const glm::vec2& a_resolution, const glm::vec2 &a_partialResolution, const glm::vec4& a_viewportSettings)
    : m_resolution(a_resolution), m_resolutionPart(a_partialResolution), m_viewPortSettings(a_viewportSettings), m_sceneManager(nullptr), m_renderPassPipeline(nullptr){}
  virtual ~AGraphicsEngine()
  {
    if( m_renderPassPipeline ) 
      delete m_renderPassPipeline;
    if( m_sceneManager ) 
      delete m_sceneManager;
  }  
  
  /**
  *  @param a_composite : if true, it will work as a compositor, false as a deferred renderer
  */
  virtual void Init(bool a_composite = false, unsigned int a_subpartsCount = 0, unsigned int a_compressResult = 0) 
  {
    m_renderPassPipeline = new RenderControl::RenderPassPipeline();
    m_sceneManager = new SceneControl::SceneManager();
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
  
  glm::vec2 GetResolution() const { return m_resolution; }
  virtual void SetResolution(const glm::vec2& a_dim) 
  { 
    m_resolution = a_dim;
    if(GetCompositionPass())
      GetCompositionPass()->GetCamera()->SetDimentsions(a_dim);
    if(GetDeferredRenderPass())
      GetDeferredRenderPass()->GetCamera()->SetDimentsions(a_dim);
  }
  
  glm::vec2 GetPartRes() { return m_resolutionPart; }
  virtual void SetPartRes(const glm::vec2& a_dim) 
  { 
    m_resolutionPart = a_dim;
  }

  glm::vec4 GetViewportSettings() { return m_viewPortSettings; }
  virtual void SetViewportSettings(const glm::vec4& a_viewport) 
  { 
    m_viewPortSettings = a_viewport;
  }
  
  
  
  
  
  virtual RenderControl::ADeferredShadingPass* GetDeferredRenderPass() const = 0;
  virtual RenderControl::ACompositionPass* GetCompositionPass() const = 0;
  
  virtual IShapeFactory* GetShapeFactory() const = 0;
  virtual ITextureFactory* GetTextureFactory() const = 0;
};