#pragma once


#include "Common/Shapes/ASphere.h"
#include "OpenGL/Core/VertexBufferObjectIndexed.h"
// Class for generating a unit sphere 
class GLSphere : public ASphere
{
private:
  unsigned int m_vao;
	CVertexBufferObjectIndexed m_vbo;

public:
	GLSphere();
	~GLSphere();
	

	virtual void Render();
	virtual void Release();
  virtual void Create();
};

