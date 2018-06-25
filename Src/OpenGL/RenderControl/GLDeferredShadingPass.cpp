/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: GLDeferredShadingPass.cpp
 *   DATE: Mon Jun 11 16:01:21 2018
 *  DESCR: 
 ***********************************************************************/
#include "OpenGL/RenderControl/GLDeferredShadingPass.h"
#include "gl/include/glew.h"
#include "gl/gl.h"
#include <iostream>
/*
 *  Method: RenderControl::GLDeferredShadingPass::GLDeferredShadingPass
 *  Params: const glm::vec2 &a_resolution
 * Effects: 
 */
RenderControl::GLDeferredShadingPass::GLDeferredShadingPass(const glm::vec2 &a_resolution)
  : ADeferredShadingPass(a_resolution), m_resolutionPart(a_resolution), m_viewPortSetting(0, 0, a_resolution.x, a_resolution.y), m_fbo(0)
{
  m_outputTextures.resize(5, 0);
  m_outputSamplers.resize(5, 0);
}


/*
 *  Method: RenderControl::GLDeferredShadingPass::~GLDeferredShadingPass
 *  Params: 
 * Effects: 
 */
RenderControl::GLDeferredShadingPass::~GLDeferredShadingPass()
{
  Clear();
}


bool RenderControl::GLDeferredShadingPass::Init()
{
  
		// init camera
		std::shared_ptr<CCamera> l_cam = GetCamera();

		l_cam->SetOrthographicProjectionMatrix((int)m_resolution.x, (int)m_resolution.y);
		l_cam->SetPerspectiveProjectionMatrix(45.0f, m_resolution.x / m_resolution.y, 0.5f, 7000.0f);


		// create framebuffer textures
		glGenTextures(m_outputTextures.size(), &m_outputTextures[0]);
		glGenSamplers(m_outputSamplers.size(), &m_outputSamplers[0]);

		// create default samplers
		for (unsigned int i = 0; i<m_outputSamplers.size(); ++i)
		{
			glSamplerParameterf(m_outputSamplers[i], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glSamplerParameterf(m_outputSamplers[i], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glSamplerParameterf(m_outputSamplers[i], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glSamplerParameterf(m_outputSamplers[i], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

	//	glEnable(GL_SCISSOR_TEST);
	//	glScissor(0, 400, 300, 600);

//		glm::vec2 l_res( 128, 648);
		glm::vec2 l_res = m_resolutionPart;
		//l_res.y /= 2.f;
		// setup geometry frame buffer object and its textures
		glGenFramebuffers(1, &m_fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

		// diffuse image 
		glBindTexture(GL_TEXTURE_2D, m_outputTextures[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, (GLsizei)l_res.x, (GLsizei)l_res.y, 0, GL_RGB, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, (GLuint)m_outputTextures[0], 0);

		// normals image
		glBindTexture(GL_TEXTURE_2D, m_outputTextures[1]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, (GLsizei)l_res.x, (GLsizei)l_res.y, 0, GL_RGB, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, (GLuint)m_outputTextures[1], 0);

		// specular
		glBindTexture(GL_TEXTURE_2D, m_outputTextures[2]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, (GLsizei)l_res.x, (GLsizei)l_res.y, 0, GL_RGB, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, (GLuint)m_outputTextures[2], 0);

		// final image
		glBindTexture(GL_TEXTURE_2D, m_outputTextures[3]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, (GLsizei)l_res.x, (GLsizei)l_res.y, 0, GL_RGB, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, (GLuint)m_outputTextures[3], 0);


		// stencil depth
		glBindTexture(GL_TEXTURE_2D, m_outputTextures[4]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, (GLsizei)l_res.x, (GLsizei)l_res.y, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, (GLuint)m_outputTextures[4], 0/*mipmap level*/);

		


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

void RenderControl::GLDeferredShadingPass::Render()
{
  GeometryPass();
		
  LightPass();
}

void RenderControl::GLDeferredShadingPass::OutputOnScreen()
{
  unsigned int l_actualAttachment;
  unsigned int l_actualBit = GL_COLOR_BUFFER_BIT;
  (m_attachmentIndex == 4) ? (l_actualAttachment = GL_DEPTH_STENCIL_ATTACHMENT, l_actualBit = GL_DEPTH_BUFFER_BIT) : l_actualAttachment = GL_COLOR_ATTACHMENT0 + m_attachmentIndex;



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

void RenderControl::GLDeferredShadingPass::UpdateViewportSettings(const glm::vec2& a_resolutionPart, const glm::vec4& a_viewportSettings)
{
  Clear();
  m_resolutionPart = a_resolutionPart;
  m_viewPortSetting = a_viewportSettings;
  Init();
}

void RenderControl::GLDeferredShadingPass::GeometryPass()
{
  glEnable(GL_CULL_FACE);


  // bind and clear the other attachments
  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
  GLenum bufs[4];
  bufs[0] = GL_COLOR_ATTACHMENT0;	// diffuse
  bufs[1] = GL_COLOR_ATTACHMENT1;	// normal
  bufs[2] = GL_COLOR_ATTACHMENT2;	// spec
  bufs[3] = GL_COLOR_ATTACHMENT3;	// emissive - final	
  glDrawBuffers(4, &bufs[0]);



  glm::vec2 l_resolution = GetResolution();
  glViewport( (GLsizei)m_viewPortSetting.x, (GLsizei)m_viewPortSetting.y, 
              (GLsizei)m_viewPortSetting.z, (GLsizei)m_viewPortSetting.w);



  glClearColor(0, 0, 0, 0);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glDepthMask(GL_TRUE);
  glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  
  

  // setup camera matrix
  glutil::MatrixStack modelViewMatrixStack;
  modelViewMatrixStack.SetIdentity();
  modelViewMatrixStack.LookAt(m_camera->GetPosition(), m_camera->GetView(), m_camera->GetUpVector());


  ////////////////////////////////////////////////////////////////////////////////////
  // RENDER OBJECTS HERE /////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////
  typedef std::unordered_map< std::shared_ptr<IShaderProgram>, std::vector<IRenderable*> >::iterator Iter;
  typedef std::vector<IRenderable*>::iterator VectorIter;
  for (Iter l_iter = m_toRender.begin(); l_iter != m_toRender.end(); ++l_iter)
  {
    l_iter->first->UseProgram();
    l_iter->first->SetUniform("matrices.projMatrix", m_camera->GetPerspectiveProjectionMatrix() );
    // used for frustum culling in geometry culling
    l_iter->first->SetUniform("UCamFr.UCamPos", m_camera->GetPosition() );
    l_iter->first->SetUniform("UCamFr.UFOV", m_camera->GetFov() );
    l_iter->first->SetUniform("UCamFr.URatio", m_camera->GetAspectRatio() );
    l_iter->first->SetUniform("UCamFr.UNear", m_camera->GetNearClippingPlane() );
    l_iter->first->SetUniform("UCamFr.UFar", m_camera->GetFarClippingPlane() );
    l_iter->first->SetUniform("UCamFr.UViewDir", m_camera->GetView());
    l_iter->first->SetUniform("UCamFr.UUpVec", m_camera->GetUpVector());
    FrustumData l_temp = m_camera->GetFrustumData();
    l_iter->first->SetUniform("UCamFr.m_x", l_temp.m_x);
    l_iter->first->SetUniform("UCamFr.m_y", l_temp.m_y);
    l_iter->first->SetUniform("UCamFr.m_z", l_temp.m_z);
    l_iter->first->SetUniform("UCamFr.m_tangent", l_temp.m_tangent);
    l_iter->first->SetUniform("UCamFr.m_width", l_temp.m_width);
    l_iter->first->SetUniform("UCamFr.m_height", l_temp.m_height);



    std::vector<IRenderable*>& l_tmp = l_iter->second;
    for (VectorIter l_iter2 = l_tmp.begin(); l_iter2 != l_tmp.end(); ++l_iter2)
      (*l_iter2)->Render(modelViewMatrixStack);
      
  }
}
void RenderControl::GLDeferredShadingPass::LightPass()
{
    // setup camera matrix
  glutil::MatrixStack projModelViewMatrixStack;
  projModelViewMatrixStack.SetIdentity();
  projModelViewMatrixStack.ApplyMatrix(*m_camera->GetPerspectiveProjectionMatrix());
  projModelViewMatrixStack.LookAt(m_camera->GetPosition(), m_camera->GetView(), m_camera->GetUpVector());
  

  // bind the textures
  for (unsigned int i = 0; i < m_outputTextures.size() - 2; ++i)
  {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, m_outputTextures[i]);
    glBindSampler(i, m_outputSamplers[i]);
  }		
  // depth
  glActiveTexture(GL_TEXTURE0 + 3);
  glBindTexture(GL_TEXTURE_2D, m_outputTextures[4]);
  glBindSampler(3, m_outputSamplers[4]);
  

  glEnable(GL_DEPTH_CLAMP);

  ////////////////////////////////////////////////////////////////////////////////////
  // RENDER OBJECTS HERE /////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////
  
  typedef std::set<IRenderable*>::iterator iterator;
  for (iterator l_iter = m_lights.begin(); l_iter != m_lights.end(); ++l_iter)
  {
    std::shared_ptr<IShaderProgram> l_lightMaterial = (*l_iter)->GetMaterial(0);
    std::shared_ptr<IShaderProgram> l_nullMaterial = (*l_iter)->GetMaterial(1);
    glm::mat4 l_invProjViewMat = glm::inverse(projModelViewMatrixStack.Top() );

    if (l_lightMaterial)
    {
      glm::vec2 l_resDiv = glm::vec2(1.0f) / GetResolution();
      glm::vec3 l_camPos = m_camera->GetPosition();
      l_lightMaterial->SetOnUse("LightPassUse",
        [l_resDiv, l_camPos, l_invProjViewMat](IShaderProgram* l_me)
        {
          l_me->SetUniform("UScreenResDiv", l_resDiv);
          l_me->SetUniform("UCamPos", l_camPos);
          l_me->SetUniform("UInverseViewProjectionMatrix", l_invProjViewMat);
        }
      );


    }
    if (l_nullMaterial)
    {
      l_nullMaterial->SetOnUse("LightPassUse",
        [l_invProjViewMat](IShaderProgram* l_me)
        {
          l_me->SetUniform("UInverseViewProjectionMatrix", l_invProjViewMat);
        }
      );


    }

    
    (*l_iter)->Render(projModelViewMatrixStack);
  }
  glDisable(GL_DEPTH_CLAMP);
}

void RenderControl::GLDeferredShadingPass::Clear()
{
  if (m_fbo != 0)
  {
    glDeleteFramebuffers(1, &m_fbo);
    m_fbo = 0;
  }
  for (int i = 0; i < 5; ++i)
  {
    if (m_outputTextures[i])
    {
      glDeleteTextures(1, &m_outputTextures[i]);
      m_outputTextures[i] = 0;
    }
    if (m_outputSamplers[i])
    {
      glDeleteSamplers(1, &m_outputSamplers[i]);
      m_outputSamplers[i] = 0;
    }
  }
}

void RenderControl::GLDeferredShadingPass::SetMaterialManager(MaterialControl::IMaterialManager* a_materialManager)
{
  ADeferredShadingPass::SetMaterialManager(a_materialManager);
  // initiate materials
  std::shared_ptr<IShaderProgram> GeometryMaterial =
    a_materialManager->GetGeometryPassMaterial(
      "..\\Assets\\GLSL_shaders\\GeometryShader.vert",
      "..\\Assets\\GLSL_shaders\\GeometryShader.frag"
    );

  std::shared_ptr<IShaderProgram> GeometryMaterialEmissiveMap =
    GetMaterialManager()->GetEmissiveGeometryPassMaterial(
      "..\\Assets\\GLSL_shaders\\GeometryShader.vert",
      "..\\Assets\\GLSL_shaders\\GeometryEmissiveShader.frag"
    );

  std::shared_ptr<IShaderProgram> GeometryMaterialDiffuseMap =
    a_materialManager->GetColourGeometryPassMaterial(
      "..\\Assets\\GLSL_shaders\\GeometryShader.vert",
      "..\\Assets\\GLSL_shaders\\GeometryColourShader.frag"
    );

  std::shared_ptr<IShaderProgram> GeometryMaterialDiffuseNormalMap =
    a_materialManager->GetColourNormalGeometryPassMaterial(
      "..\\Assets\\GLSL_shaders\\GeometryShader.vert",
      "..\\Assets\\GLSL_shaders\\GeometryColourNormalShader.frag"
    );

  std::shared_ptr<IShaderProgram> GeometryMaterialDiffuseNormalSpecMap =
    a_materialManager->GetColourNormalSpecGeometryPassMaterial(
      "..\\Assets\\GLSL_shaders\\GeometryShader.vert",
      "..\\Assets\\GLSL_shaders\\GeometryColourNormalSpecShader.frag"
    );

  std::shared_ptr<IShaderProgram> GeometryMaterialDiffuseNormalSpecHardnessMap =
    a_materialManager->GetColourNormalSpecHardnessGeometryPassMaterial(
      "..\\Assets\\GLSL_shaders\\GeometryShader.vert",
      "..\\Assets\\GLSL_shaders\\GeometryColourNormalSpecHardnessShader.frag"
    );

  std::shared_ptr<IShaderProgram> GeometryMaterialDiffuseNormalSpecHardnessEmissionMap =
    a_materialManager->GetColourNormalSpecHardnessEmissionGeometryPassMaterial(
      "..\\Assets\\GLSL_shaders\\GeometryShader.vert",
      "..\\Assets\\GLSL_shaders\\GeometryColourNormalSpecHardnessEmissiveShader.frag"
    );
  

  std::shared_ptr<IShaderProgram> GeometrySkybox =
    a_materialManager->GetSkyCubeMaterial(
      "..\\Assets\\GLSL_shaders\\GeometrySkyBoxShader.vert",
      "..\\Assets\\GLSL_shaders\\GeometrySkyBoxShader.frag"
    );

  // create appropriate keys
  m_toRender[GeometryMaterialDiffuseNormalSpecHardnessEmissionMap];
  m_toRender[GeometryMaterialDiffuseNormalSpecHardnessMap];
  m_toRender[GeometryMaterialDiffuseNormalSpecMap];
  m_toRender[GeometryMaterialDiffuseNormalMap];
  m_toRender[GeometryMaterialDiffuseMap];
  m_toRender[GeometryMaterialEmissiveMap];
  m_toRender[GeometryMaterial];
  m_toRender[GeometrySkybox];


  std::shared_ptr<IShaderProgram> l_nullMaterial =
    a_materialManager->GetStencilLightPassMaterial(
      "..\\Assets\\GLSL_shaders\\NullShader.vert",
      "..\\Assets\\GLSL_shaders\\NullShader.frag"
    );

  std::shared_ptr<IShaderProgram> l_pointLightMaterial =
    a_materialManager->GetPointLightPassMaterial(
      "..\\Assets\\GLSL_shaders\\PointLightShader.vert",
      "..\\Assets\\GLSL_shaders\\PointLightShader.frag"
    );

  std::shared_ptr<IShaderProgram> l_spotLightMaterial =
    a_materialManager->GetSpotLightPassMaterial(
      "..\\Assets\\GLSL_shaders\\SpotLightShader.vert",
      "..\\Assets\\GLSL_shaders\\SpotLightShader.frag"
    );

  std::shared_ptr<IShaderProgram> l_directionalLightMaterial =
    a_materialManager->GetFullScreenLightPassMaterial(
      "..\\Assets\\GLSL_shaders\\DirectionalLightShader.vert",
      "..\\Assets\\GLSL_shaders\\DirectionalLightShader.frag"
    );
  
}

