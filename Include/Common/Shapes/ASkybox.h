#pragma once

#include "Common/Shapes/IMesh.h"

/// This is a class for creating and rendering a skybox
/// Modified by Stavros Bizelis - for the Graphics Coursework n City, University of London
/// Stripped down of any texture information and hardcoded paths
class ASkybox : public IMesh
{
protected:
  std::vector<Vertex> m_vertices;

public:
	ASkybox();
	~ASkybox();
	
	virtual void Create();

};

