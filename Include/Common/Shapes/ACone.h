#pragma once

#include "Common/Shapes/IMesh.h"
/// a cone primitive mesh 
/// only vertex positions are set(no normals or texture coordinates)
/// used as a lighting mesh scene node
class ACone : public IMesh
{
protected:
  std::vector<Vertex> m_vertices;
  std::vector<unsigned int> m_indices;
  int m_numTriangles;
public:
	ACone();
	~ACone();
	
	virtual void Create();

};

