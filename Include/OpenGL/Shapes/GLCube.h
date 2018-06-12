#pragma once

#include "Common/Shapes/ACube.h"
#include "OpenGL/Core/VertexBufferObject.h"
/// a cube primitive mesh 
/// only vertex positions are set(no normals or texture coordinates)
/// used as a lighting mesh scene node
class GLCube : public ACube
{
private:
  unsigned int m_vao;
  CVertexBufferObject m_vbo;
public:
	GLCube();
	~GLCube();
	
	virtual void Render();
	virtual void Release();
  virtual void Create();
};

