#pragma once

#include "Common/Shapes/IMesh.h"
/// a cube primitive mesh 
/// only vertex positions are set(no normals or texture coordinates)
/// used as a lighting mesh scene node
class ACube : public IMesh
{
protected: 
  std::vector<Vertex> m_vertices;  
public:
	ACube();
	~ACube();
	
	virtual void Create();

};

