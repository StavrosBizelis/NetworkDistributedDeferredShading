#pragma once


#include <vector>
#include "Common/Shapes/ARect.h"
#include "OpenGL/Core/VertexBufferObjectIndexed.h"
/// Used for screen space quads
/// originally it is full screen but with scaling it can be used for gui elements
class GLRect : public ARect
{

  unsigned int m_vao;
	CVertexBufferObjectIndexed m_vbo;
public:
	GLRect();
	~GLRect();
	

	virtual void Render();
	virtual void Release();  
  virtual void Create();
};

