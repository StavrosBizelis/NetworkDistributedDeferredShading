/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: GLMaterialManager.cpp
 *   DATE: Mon Jun 11 15:35:05 2018
 *  DESCR: 
 ***********************************************************************/
#include "OpenGL/MaterialControl/GLMaterialManager.h"
#include "OpenGL/MaterialControl/GLShaderProgram.h"
#include "gl/include/glew.h"
#include "gl/gl.h"
/*
 *  Method: MaterialControl::GLMaterialManager::GetMaterial
 *  Params: const std::string &a_programName, const std::string &a_vertShPath, const std::string &a_fragShPath, const std::string &a_geometryShader
 * Returns: std::shared_ptr<IShaderProgram>
 * Effects: 
 */
std::shared_ptr<IShaderProgram>
MaterialControl::GLMaterialManager::GetMaterial(const std::string &a_programName, const std::string &a_vertShPath, const std::string &a_fragShPath, const std::string &a_geometryShader)
{
  		std::map<std::string, std::shared_ptr<IShaderProgram> >::iterator l_iter = m_materials.find(a_programName);
		if (l_iter != m_materials.end())
			return l_iter->second;

		// Load shaders
		GLShader l_vertexShader;
		GLShader l_fragmentShader;
		GLShader l_geometryShader;
		l_vertexShader.LoadShader(a_vertShPath, GL_VERTEX_SHADER);
		l_fragmentShader.LoadShader(a_fragShPath, GL_FRAGMENT_SHADER);
	//	if( !a_geometryShader.empty() )
	//		l_geometryShader.LoadShader(a_geometryShader, GL_GEOMETRY_SHADER);


		// Create the main shader program
		std::shared_ptr<GLShaderProgram> l_program = std::make_shared<GLShaderProgram>();
		l_program->CreateProgram();
		l_program->AddShaderToProgram(&l_vertexShader);
		//if (!a_geometryShader.empty())
		//	l_program->AddShaderToProgram(&l_geometryShader);
		l_program->AddShaderToProgram(&l_fragmentShader);
		l_program->LinkProgram();
		m_materials[a_programName] = l_program;
		return l_program;
}


/*
 *  Method: MaterialControl::GLMaterialManager::GetSingleTextureMaterial
 *  Params: const std::string &a_vertShPath, const std::string &a_fragShPath
 * Returns: std::shared_ptr<IShaderProgram>
 * Effects: 
 */
std::shared_ptr<IShaderProgram>
MaterialControl::GLMaterialManager::GetSingleTextureMaterial(const std::string &a_vertShPath, const std::string &a_fragShPath)
{
  std::shared_ptr<IShaderProgram> l_program = GetMaterial("SingleTexturePass", a_vertShPath, a_fragShPath);
		// set the sampler unit
		l_program->SetOnUse("CreationOnUse",
			[](IShaderProgram* l_me)
			{
				l_me->SetUniform("UTexture", 0);

				glDisable(GL_BLEND);
				glDisable(GL_DEPTH_TEST);
				// glDepthMask(GL_FALSE);

				glCullFace(GL_BACK);
				glEnable(GL_CULL_FACE);
				glDisable(GL_STENCIL_TEST);
			}

		);
		return l_program;
}


/*
 *  Method: MaterialControl::GLMaterialManager::GetAddBlendMaterial
 *  Params: const std::string &a_vertShPath, const std::string &a_fragShPath
 * Returns: std::shared_ptr<IShaderProgram>
 * Effects: 
 */
std::shared_ptr<IShaderProgram>
MaterialControl::GLMaterialManager::GetAddBlendMaterial(const std::string &a_vertShPath, const std::string &a_fragShPath)
{
  std::shared_ptr<IShaderProgram> l_program = GetMaterial("TextureAdditionPass", a_vertShPath, a_fragShPath );
		// set the sampler unit
		l_program->SetOnUse("CreationOnUse",
			[](IShaderProgram* l_me)
		{
			l_me->SetUniform("UTexture", 0);

			glDisable(GL_STENCIL_TEST);

			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			glEnable(GL_BLEND);
			glBlendEquation(GL_MAX);
			glBlendFunc(GL_ONE, GL_ONE);

			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}

		);
		return l_program;
}


/*
 *  Method: MaterialControl::GLMaterialManager::GetGeometryPassMaterial
 *  Params: const std::string &a_vertShPath, const std::string &a_fragShPath
 * Returns: std::shared_ptr<IShaderProgram>
 * Effects: 
 */
std::shared_ptr<IShaderProgram>
MaterialControl::GLMaterialManager::GetGeometryPassMaterial(const std::string &a_vertShPath, const std::string &a_fragShPath)
{
  std::shared_ptr<IShaderProgram> l_program = GetMaterial("GeometryPass", a_vertShPath, a_fragShPath);
  // set the sampler unit
  l_program->SetOnUse("CreationOnUse",
    [](IShaderProgram* l_me)
    {

      glDisable(GL_BLEND);
      glEnable(GL_DEPTH_TEST);
      glDepthMask(GL_TRUE);

      glCullFace(GL_BACK);
      glEnable(GL_CULL_FACE);
      glDisable(GL_STENCIL_TEST);
    }
  );
  return l_program;
}


/*
 *  Method: MaterialControl::GLMaterialManager::GetColourGeometryPassMaterial
 *  Params: const std::string &a_vertShPath, const std::string &a_fragShPath
 * Returns: std::shared_ptr<IShaderProgram>
 * Effects: 
 */
std::shared_ptr<IShaderProgram>
MaterialControl::GLMaterialManager::GetColourGeometryPassMaterial(const std::string &a_vertShPath, const std::string &a_fragShPath)
{
  std::shared_ptr<IShaderProgram> l_program = GetMaterial("ColourGeometryPass", a_vertShPath, a_fragShPath);
  // set the sampler unit
  l_program->SetOnUse("CreationOnUse",
    [](IShaderProgram* l_me)
    {
      l_me->SetUniform("u_diffuseMap", 0);


      glDisable(GL_BLEND);
      glEnable(GL_DEPTH_TEST);
      glDepthMask(GL_TRUE);

      glCullFace(GL_BACK);
      glEnable(GL_CULL_FACE);
      glDisable(GL_STENCIL_TEST);
    }
  );

  return l_program;
}


/*
 *  Method: MaterialControl::GLMaterialManager::GetEmissiveGeometryPassMaterial
 *  Params: const std::string &a_vertShPath, const std::string &a_fragShPath
 * Returns: std::shared_ptr<IShaderProgram>
 * Effects: 
 */
std::shared_ptr<IShaderProgram>
MaterialControl::GLMaterialManager::GetEmissiveGeometryPassMaterial(const std::string &a_vertShPath, const std::string &a_fragShPath)
{
  std::shared_ptr<IShaderProgram> l_program = GetMaterial("EmissiveGeometryPass", a_vertShPath, a_fragShPath);
  // set the sampler unit
  l_program->SetOnUse("CreationOnUse",
    [](IShaderProgram* l_me)
  {
    l_me->SetUniform("u_emissiveMap", 0);


    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glDisable(GL_STENCIL_TEST);
  }
  );

  return l_program;
}


/*
 *  Method: MaterialControl::GLMaterialManager::GetColourNormalGeometryPassMaterial
 *  Params: const std::string &a_vertShPath, const std::string &a_fragShPath
 * Returns: std::shared_ptr<IShaderProgram>
 * Effects: 
 */
std::shared_ptr<IShaderProgram>
MaterialControl::GLMaterialManager::GetColourNormalGeometryPassMaterial(const std::string &a_vertShPath, const std::string &a_fragShPath)
{
  std::shared_ptr<IShaderProgram> l_program = GetMaterial("ColourNormalGeometryPass", a_vertShPath, a_fragShPath);
  // set the sampler units
  
  l_program->SetOnUse("CreationOnUse",
    [](IShaderProgram* l_me)
    {
      l_me->SetUniform("u_diffuseMap", 0);
      l_me->SetUniform("u_normalMap", 1);
      

      glDisable(GL_BLEND);
      glEnable(GL_DEPTH_TEST);
      glDepthMask(GL_TRUE);

      glCullFace(GL_BACK);
      glEnable(GL_CULL_FACE);
      glDisable(GL_STENCIL_TEST);
    }
  );
  
  return l_program;
}


/*
 *  Method: MaterialControl::GLMaterialManager::GetColourNormalSpecGeometryPassMaterial
 *  Params: const std::string &a_vertShPath, const std::string &a_fragShPath
 * Returns: std::shared_ptr<IShaderProgram>
 * Effects: 
 */
std::shared_ptr<IShaderProgram>
MaterialControl::GLMaterialManager::GetColourNormalSpecGeometryPassMaterial(const std::string &a_vertShPath, const std::string &a_fragShPath)
{
  std::shared_ptr<IShaderProgram> l_program = GetMaterial("ColourNormalSpecGeometryPass", a_vertShPath, a_fragShPath);
  // set the sampler units
  l_program->SetOnUse("CreationOnUse",
    [](IShaderProgram* l_me)
    {
      l_me->SetUniform("u_diffuseMap", 0);
      l_me->SetUniform("u_normalMap", 1);
      l_me->SetUniform("u_specularMap", 2);


      glDisable(GL_BLEND);
      glEnable(GL_DEPTH_TEST);
      glDepthMask(GL_TRUE);


      glCullFace(GL_BACK);
      glEnable(GL_CULL_FACE);
      glDisable(GL_STENCIL_TEST);
    }
  );
  return l_program;
}


/*
 *  Method: MaterialControl::GLMaterialManager::GetColourNormalSpecHardnessGeometryPassMaterial
 *  Params: const std::string &a_vertShPath, const std::string &a_fragShPath
 * Returns: std::shared_ptr<IShaderProgram>
 * Effects: 
 */
std::shared_ptr<IShaderProgram>
MaterialControl::GLMaterialManager::GetColourNormalSpecHardnessGeometryPassMaterial(const std::string &a_vertShPath, const std::string &a_fragShPath)
{
  std::shared_ptr<IShaderProgram> l_program = GetMaterial("ColourNormalSpecHardGeometryPass", a_vertShPath, a_fragShPath);
  // set the sampler units
  l_program->SetOnUse("CreationOnUse",
    [](IShaderProgram* l_me)
    {
      l_me->SetUniform("u_diffuseMap", 0);
      l_me->SetUniform("u_normalMap", 1);
      l_me->SetUniform("u_specularMap", 2);
      l_me->SetUniform("u_hardnessMap", 3);

      

      glDisable(GL_BLEND);
      glEnable(GL_DEPTH_TEST);
      glDepthMask(GL_TRUE);


      glCullFace(GL_BACK);
      glEnable(GL_CULL_FACE);

      glDisable(GL_STENCIL_TEST);

    }
  );
  return l_program;
}


/*
 *  Method: MaterialControl::GLMaterialManager::GetColourNormalSpecHardnessEmissionGeometryPassMaterial
 *  Params: const std::string &a_vertShPath, const std::string &a_fragShPath
 * Returns: std::shared_ptr<IShaderProgram>
 * Effects: 
 */
std::shared_ptr<IShaderProgram>
MaterialControl::GLMaterialManager::GetColourNormalSpecHardnessEmissionGeometryPassMaterial(const std::string &a_vertShPath, const std::string &a_fragShPath)
{
  std::shared_ptr<IShaderProgram> l_program = GetMaterial("ColourNormalSpecHardEmissionGeometryPass", a_vertShPath, a_fragShPath);
  // set the sampler units
  l_program->SetOnUse("CreationOnUse",
    [](IShaderProgram* l_me)
  {	
    l_me->SetUniform("u_diffuseMap", 0);
    l_me->SetUniform("u_normalMap", 1);
    l_me->SetUniform("u_specularMap", 2);
    l_me->SetUniform("u_hardnessMap", 3);
    l_me->SetUniform("u_emissionMap", 4);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);


    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    glDisable(GL_STENCIL_TEST);

  }
  );
  return l_program;
}


/*
 *  Method: MaterialControl::GLMaterialManager::GetSkyCubeMaterial
 *  Params: const std::string &a_vertShPath, const std::string &a_fragShPath
 * Returns: std::shared_ptr<IShaderProgram>
 * Effects: 
 */
std::shared_ptr<IShaderProgram>
MaterialControl::GLMaterialManager::GetSkyCubeMaterial(const std::string &a_vertShPath, const std::string &a_fragShPath)
{
  std::shared_ptr<IShaderProgram> l_program = GetMaterial("SkyCubeMaterial",a_vertShPath, a_fragShPath);
  // set the sampler units
  l_program->SetOnUse("CreationOnUse",
    [](IShaderProgram* l_me)
  {
    l_me->SetUniform("UCubeMap", 0);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);

    glDepthMask(GL_FALSE);

    glDisable(GL_BLEND);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

  }
  );
  return l_program;
}


/*
 *  Method: MaterialControl::GLMaterialManager::GetStencilLightPassMaterial
 *  Params: const std::string &a_vertShPath, const std::string &a_fragShPath
 * Returns: std::shared_ptr<IShaderProgram>
 * Effects: 
 */
std::shared_ptr<IShaderProgram>
MaterialControl::GLMaterialManager::GetStencilLightPassMaterial(const std::string &a_vertShPath, const std::string &a_fragShPath)
{
  std::shared_ptr<IShaderProgram> l_program = GetMaterial("StencilLightPass", a_vertShPath, a_fragShPath);
  // set the sampler units
  l_program->SetOnUse("CreationOnUse",
    [](IShaderProgram* l_me)
    {

      glDrawBuffer(GL_NONE);
      // no face culling
      glDisable(GL_CULL_FACE);
      // stencil clear
      glStencilMask(GL_TRUE);
      glClearStencil(0);
      glClear(GL_STENCIL_BUFFER_BIT);
      // stencil test set
      glEnable(GL_STENCIL_TEST);
      glStencilFunc(GL_ALWAYS, 0, 0);
      glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
      glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
      // depth test set
      glEnable(GL_DEPTH_TEST);
      glDepthMask(GL_FALSE);

    }
  );
  return l_program;
}


/*
 *  Method: MaterialControl::GLMaterialManager::GetPointLightPassMaterial
 *  Params: const std::string &a_vertShPath, const std::string &a_fragShPath
 * Returns: std::shared_ptr<IShaderProgram>
 * Effects: 
 */
std::shared_ptr<IShaderProgram>
MaterialControl::GLMaterialManager::GetPointLightPassMaterial(const std::string &a_vertShPath, const std::string &a_fragShPath)
{
  std::shared_ptr<IShaderProgram> l_program = GetMaterial("PointLightPass", a_vertShPath, a_fragShPath);
  // set the sampler units
  l_program->SetOnUse("CreationOnUse",
    [](IShaderProgram* l_me)
    {
      l_me->SetUniform("UColor", 0);
      l_me->SetUniform("UNormal", 1);
      l_me->SetUniform("USpecularIntensityPower", 2);
      l_me->SetUniform("UDepth", 3);

      
      glDrawBuffer(GL_COLOR_ATTACHMENT3);
      // front face culling
      glEnable(GL_CULL_FACE);
      glCullFace(GL_FRONT);
      // stencil test set
      // glEnable(GL_STENCIL_TEST);
      glDisable(GL_STENCIL_TEST);
      glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
      // depth test set
      glDisable(GL_DEPTH_TEST);
      glDepthMask(GL_FALSE);

      // light blending
      glEnable(GL_BLEND);
      glBlendEquation(GL_FUNC_ADD);
      glBlendFunc(GL_ONE, GL_ONE);
      

    }
  );
  return l_program;
}


/*
 *  Method: MaterialControl::GLMaterialManager::GetSpotLightPassMaterial
 *  Params: const std::string &a_vertShPath, const std::string &a_fragShPath
 * Returns: std::shared_ptr<IShaderProgram>
 * Effects: 
 */
std::shared_ptr<IShaderProgram>
MaterialControl::GLMaterialManager::GetSpotLightPassMaterial(const std::string &a_vertShPath, const std::string &a_fragShPath)
{
  std::shared_ptr<IShaderProgram> l_program = GetMaterial("SpotLightPass", a_vertShPath, a_fragShPath);
  // set the sampler units
  l_program->SetOnUse("CreationOnUse",
    [](IShaderProgram* l_me)
  {
    l_me->SetUniform("UColor", 0);
    l_me->SetUniform("UNormal", 1);
    l_me->SetUniform("USpecularIntensityPower", 2);
    l_me->SetUniform("UDepth", 3);


    glDrawBuffer(GL_COLOR_ATTACHMENT3);
    // front face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    // stencil test set
    glDisable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
    // depth test set
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    // light blending
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);


  }
  );
  return l_program;
}


/*
 *  Method: MaterialControl::GLMaterialManager::GetFullScreenLightPassMaterial
 *  Params: const std::string &a_vertShPath, const std::string &a_fragShPath
 * Returns: std::shared_ptr<IShaderProgram>
 * Effects: 
 */
std::shared_ptr<IShaderProgram>
MaterialControl::GLMaterialManager::GetFullScreenLightPassMaterial(const std::string &a_vertShPath, const std::string &a_fragShPath)
{
  std::shared_ptr<IShaderProgram> l_program = GetMaterial("FullScreenLightPass", a_vertShPath, a_fragShPath);
  // set the sampler units
  l_program->SetOnUse("CreationOnUse",
    [](IShaderProgram* l_me)
  {

    l_me->SetUniform("UColor", 0);
    l_me->SetUniform("UNormal", 1);
    l_me->SetUniform("USpecularIntensityPower", 2);
    l_me->SetUniform("UDepth", 3);

    glDrawBuffer(GL_COLOR_ATTACHMENT3);

    glDisable(GL_STENCIL_TEST);

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

  }
  );
  return l_program;
}


