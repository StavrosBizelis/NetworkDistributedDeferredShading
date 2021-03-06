#include "OpenGL/GLGraphicsEngine.h"

#include "OpenGL/RenderControl/GLDeferredShadingPass.h"
#include "OpenGL/RenderControl/GLCompositionPass.h"
#include "OpenGL/MaterialControl/GLMaterialManager.h"

#include "OpenGL/Shapes/GLShapeFactory.h"
#include "OpenGL/Textures/GLTextureFactory.h"

#include "gl/include/glew.h"
#include "gl/include/wglew.h"
#include <iostream>


GLGraphicsEngine::GLGraphicsEngine(const glm::vec2& a_resolution)
: GLGraphicsEngine(a_resolution, a_resolution, glm::vec4(0,0, a_resolution.x, a_resolution.y) ) {}

GLGraphicsEngine::GLGraphicsEngine(const glm::vec2& a_resolution, const glm::vec2 &a_partialResolution, const glm::vec4& a_viewportSettings ) 
  : AGraphicsEngine(a_resolution, a_partialResolution, a_viewportSettings), m_deferredShadingPass(nullptr), m_compositionPass(nullptr) {}

GLGraphicsEngine::~GLGraphicsEngine()
{
  if( m_shapeFactory ) 
    delete m_shapeFactory;
  if( m_textureFactory ) 
    delete m_textureFactory;
}

void GLGraphicsEngine::Init(bool a_composite, unsigned int a_subparts, unsigned int a_compressResult)
{

  AGraphicsEngine::Init(a_composite, a_subparts);  
  
  MaterialControl::IMaterialManager* l_materialManager = new MaterialControl::GLMaterialManager();
  m_renderPassPipeline->SetMaterialManager(l_materialManager);
  
  m_shapeFactory = new GLShapeFactory();
  m_shapeFactory->Init();
  m_textureFactory = new GLTextureFactory();
  
  
  if(a_composite)
  {
    m_compositionPass = new RenderControl::GLCompositionPass(m_resolution, m_sceneManager, m_shapeFactory, m_textureFactory, a_subparts);
    m_compositionPass->Init();
    m_compositionPass->SetSceenOutputAttachment(0);
    m_renderPassPipeline->PushBack(m_compositionPass);
  }
  else
  {
    m_deferredShadingPass = new RenderControl::GLDeferredShadingPass( m_resolution, m_resolutionPart, m_viewPortSettings, a_compressResult );
    m_deferredShadingPass->Init();
    m_deferredShadingPass->SetSceenOutputAttachment(3);
    m_renderPassPipeline->PushBack(m_deferredShadingPass);    
  }
  

}

RenderControl::ADeferredShadingPass* GLGraphicsEngine::GetDeferredRenderPass() const
{
  return m_deferredShadingPass;
}

RenderControl::ACompositionPass* GLGraphicsEngine::GetCompositionPass() const
{
  return m_compositionPass;
}

IShapeFactory* GLGraphicsEngine::GetShapeFactory()  const
{
  return m_shapeFactory;
}

ITextureFactory* GLGraphicsEngine::GetTextureFactory() const
{
  return m_textureFactory;
}