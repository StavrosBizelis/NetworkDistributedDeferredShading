/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: GLCompositionPass.cpp
 *   DATE: Tue Jun 26 20:46:42 2018
 *  DESCR: 
 ***********************************************************************/
#include "OpenGL/RenderControl/GLCompositionPass.h"
#include <iostream>

/***********************************************************************
 *  Method: RenderControl::GLCompositionPass::GLCompositionPass
 *  Params: const glm::vec2 &a_resolution, SceneControl::SceneManager *a_scnManager, SceneControl::IShapeFactory *a_shapeFactory, const unsigned int &a_subparts
 * Effects: 
 ***********************************************************************/
RenderControl::GLCompositionPass::GLCompositionPass(const glm::vec2 &a_resolution, SceneControl::SceneManager *a_scnManager, IShapeFactory *a_shapeFactory, const unsigned int &a_subparts, std::shared_ptr<ITexture> a_text)
  :ACompositionPass(a_resolution, a_scnManager, a_shapeFactory, a_subparts)
{
  
  for( unsigned int i =0; i < a_subparts; ++i)
    m_subpartRects[i]->SetTexture(0,a_text);
  
  m_outputTextures.resize(1, 0);
  m_outputSamplers.resize(1, 0);
}


/***********************************************************************
 *  Method: RenderControl::GLCompositionPass::~GLCompositionPass
 *  Params: 
 * Effects: 
 ***********************************************************************/
RenderControl::GLCompositionPass::~GLCompositionPass()
{
  Clear();
}


/***********************************************************************
 *  Method: RenderControl::GLCompositionPass::SetMaterialManager
 *  Params: MaterialControl::IMaterialManager *a_materialManager
 * Returns: void
 * Effects: 
 ***********************************************************************/
void
RenderControl::GLCompositionPass::SetMaterialManager(MaterialControl::IMaterialManager *a_materialManager)
{
  ACompositionPass::SetMaterialManager(a_materialManager);
  
  // initiate materials
  m_material =
    a_materialManager->GetSingleTextureMaterial(
      "..\\Assets\\GLSL_shaders\\HUDImage.vert",
      "..\\Assets\\GLSL_shaders\\HUDImage.frag"
    );
}


bool RenderControl::GLCompositionPass::Init()
{
  m_camera->SetOrthographicProjectionMatrix((int)m_resolution.x, (int)m_resolution.y);
  std::cout << m_resolution.x << " " << m_resolution.y << std::endl;
  
  // create framebuffer textures
  glGenTextures(m_outputTextures.size(), &m_outputTextures[0]);
  glGenSamplers(m_outputSamplers.size(), &m_outputSamplers[0]);

  // create default samplers
  glSamplerParameterf(m_outputSamplers[0], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glSamplerParameterf(m_outputSamplers[0], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glSamplerParameterf(m_outputSamplers[0], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glSamplerParameterf(m_outputSamplers[0], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  
  
  glm::vec2 l_res = m_resolution;
  glGenFramebuffers(1, &m_fbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
  
  // output image
  glBindTexture(GL_TEXTURE_2D, m_outputTextures[0]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, (GLsizei)l_res.x, (GLsizei)l_res.y, 0, GL_RGB, GL_FLOAT, NULL);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, (GLuint)m_outputTextures[0], 0);

  GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (Status != GL_FRAMEBUFFER_COMPLETE)
  {
    printf("FB error, status: 0x%x\n", Status);
    return false;
  }

  
  // restore default FBO
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  return true;
}

void RenderControl::GLCompositionPass::Render()
{
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
  glDrawBuffer(GL_COLOR_ATTACHMENT0 );
  glViewport(0, 0, (GLsizei)m_resolution.x, (GLsizei)m_resolution.y);

  glutil::MatrixStack l_matrixStack;
  l_matrixStack.SetIdentity();
  
  glDisable(GL_DEPTH_TEST);
  
  m_material->UseProgram();
  m_material->SetUniform("matrices.projMatrix", m_camera->GetOrthographicProjectionMatrix() );
  
  for( unsigned int i =0; i < m_subpartRects.size(); ++i)
    m_subpartRects[i]->Render(l_matrixStack);
  
  glEnable(GL_DEPTH_TEST);
}

void RenderControl::GLCompositionPass::OutputOnScreen()
{
  unsigned int l_actualAttachment = GL_COLOR_ATTACHMENT0;
  unsigned int l_actualBit = GL_COLOR_BUFFER_BIT;


  /* We are going to blit into the window (default framebuffer)                     */
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glDrawBuffer(GL_BACK);              /* Use backbuffer as color dst.         */
  //glClearColor(0.f, 0.f, 0.f, 0.0f);
  //glClearDepth(1.0f);
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  /* Read from your FBO */
  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
  glReadBuffer(l_actualAttachment); /* Use Color Attachment n as color src. */

                    /* Copy the color and depth buffer from your FBO to the default framebuffer       */
  glBlitFramebuffer(0, 0, (GLint)m_resolution.x, (GLint)m_resolution.y,
    0, 0, (GLint)m_resolution.x, (GLint)m_resolution.y,
    l_actualBit, GL_LINEAR);
  // return to default 
  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}


void RenderControl::GLCompositionPass::Clear()
{
  if (m_fbo != 0)
  {
    glDeleteFramebuffers(1, &m_fbo);
    m_fbo = 0;
  }

  
  if (m_outputTextures[0])
  {
    glDeleteTextures(1, &m_outputTextures[0]);
    m_outputTextures[0] = 0;
  }
  if (m_outputSamplers[0])
  {
    glDeleteSamplers(1, &m_outputSamplers[0]);
    m_outputSamplers[0] = 0;
  }
}


