#pragma once

#include "Common/Shapes/IMesh.h"

// Class for generating a unit sphere 
class ASphere : public IMesh
{
protected:
  std::vector<Vertex> m_vertices;
  std::vector<unsigned int> m_indices;
  unsigned int m_numTriangles;
public:
	ASphere();
	~ASphere();
	
	virtual void Create();

};

