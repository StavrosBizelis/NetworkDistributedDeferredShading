#pragma once

#include "Common/Shapes/ACone.h"
#include "OpenGL/Core/VertexBufferObjectIndexed.h"
/// a cone primitive mesh 
/// only vertex positions are set(no normals or texture coordinates)
/// used as a lighting mesh scene node
class GLCone : public ACone
{
private:
  unsigned int m_vao;
	CVertexBufferObjectIndexed m_vbo;
  
public:
	GLCone();
	~GLCone();
	

	virtual void Render();
	virtual void Release();
  virtual void Create();
};

