#pragma once

#include "Common/Shapes/IMesh.h"
/// Used for screen space quads
/// originally it is full screen but with scaling it can be used for gui elements
class ARect : public IMesh
{
protected:
  std::vector<Vertex> m_vertices;
  std::vector<unsigned int> m_indices;
public:
	ARect();
	~ARect();
	
	virtual void Create();

};

