#include "OpenGL/GLGraphicsEngine.h"

#include "OpenGL/RenderControl/GLDeferredShadingPass.h"
#include "OpenGL/RenderControl/GLCompositionPass.h"
#include "OpenGL/MaterialControl/GLMaterialManager.h"

#include "OpenGL/Shapes/GLShapeFactory.h"
#include "OpenGL/Textures/GLTextureFactory.h"

GLGraphicsEngine::GLGraphicsEngine(const glm::vec2& a_dimensions ) : AGraphicsEngine(a_dimensions), m_deferredShadingPass(nullptr), m_compositionPass(nullptr) {}

GLGraphicsEngine::~GLGraphicsEngine()
{
  if( m_shapeFactory ) 
    delete m_shapeFactory;
  if( m_textureFactory ) 
    delete m_textureFactory;
}

void GLGraphicsEngine::Init(bool a_composite, unsigned int a_subparts)
{
  AGraphicsEngine::Init(a_composite);  
  
  MaterialControl::IMaterialManager* l_materialManager = new MaterialControl::GLMaterialManager();
  m_renderPassPipeline->SetMaterialManager(l_materialManager);
  
  m_shapeFactory = new GLShapeFactory();
  m_shapeFactory->Init();
  m_textureFactory = new GLTextureFactory();
  
  
  if(a_composite)
  {
    std::shared_ptr<ITexture> l_text = m_textureFactory->GetTexture("..\\Assets\\Models\\Asteroid\\diffuse.jpg");
    m_compositionPass = new RenderControl::GLCompositionPass(m_dimensions, m_sceneManager, m_shapeFactory, a_subparts, l_text);
    m_compositionPass->Init();
    m_compositionPass->SetSceenOutputAttachment(0);
    m_renderPassPipeline->PushBack(m_compositionPass);
  }
  else
  {
    m_deferredShadingPass = new RenderControl::GLDeferredShadingPass( m_dimensions );
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