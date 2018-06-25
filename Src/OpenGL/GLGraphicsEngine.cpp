#include "OpenGL/GLGraphicsEngine.h"

#include "OpenGL/RenderControl/GLDeferredShadingPass.h"
#include "OpenGL/MaterialControl/GLMaterialManager.h"

#include "OpenGL/Shapes/GLShapeFactory.h"
#include "OpenGL/Textures/GLTextureFactory.h"

GLGraphicsEngine::GLGraphicsEngine(const glm::vec2& a_dimensions) : AGraphicsEngine(a_dimensions) {}

GLGraphicsEngine::~GLGraphicsEngine()
{
  if( m_deferredShadingPass ) 
    delete m_deferredShadingPass;
  if( m_shapeFactory ) 
    delete m_shapeFactory;
  if( m_textureFactory ) 
    delete m_textureFactory;
}

void GLGraphicsEngine::Init()
{
  AGraphicsEngine::Init();  
  
  MaterialControl::IMaterialManager* l_materialManager = new MaterialControl::GLMaterialManager();
  m_renderPassPipeline->SetMaterialManager(l_materialManager);
  
  m_deferredShadingPass = new RenderControl::GLDeferredShadingPass( m_dimensions );
  m_deferredShadingPass->Init();
	m_deferredShadingPass->SetSceenOutputAttachment(3);
	m_renderPassPipeline->PushBack(m_deferredShadingPass);
  
  m_shapeFactory = new GLShapeFactory();
  m_shapeFactory->Init();
  m_textureFactory = new GLTextureFactory();
}

RenderControl::ADeferredShadingPass* GLGraphicsEngine::GetDeferredRenderPass() const
{
  return m_deferredShadingPass;
}

IShapeFactory* GLGraphicsEngine::GetShapeFactory()  const
{
  return m_shapeFactory;
}

ITextureFactory* GLGraphicsEngine::GetTextureFactory() const
{
  return m_textureFactory;
}