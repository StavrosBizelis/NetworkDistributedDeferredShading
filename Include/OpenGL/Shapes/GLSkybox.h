#pragma once


#include <vector>
#include "Common/Shapes/ASkybox.h"
#include "OpenGL/Core/VertexBufferObject.h"
/// This is a class for creating and rendering a skybox
/// Modified by Stavros Bizelis - for the Graphics Coursework n City, University of London
/// Stripped down of any texture information and hardcoded paths
class GLSkybox : public ASkybox
{
private:
	unsigned int m_vao;
	CVertexBufferObject m_vbo;

public:
	GLSkybox();
	~GLSkybox();
	

	virtual void Render();
  virtual void Create();
	virtual void Release();
};

