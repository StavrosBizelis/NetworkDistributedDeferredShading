/***********************************************************************
 * AUTHOR:  <Stavros Bizelis>
 *   FILE: ASphere.cpp
 *   DATE: Mon Jun 11 14:14:54 2018
 *  DESCR: 
 ***********************************************************************/

#define _USE_MATH_DEFINES
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#include "Common/Shapes/ASphere.h"


#include <math.h>
/*
 *  Method: ASphere::ASphere
 *  Params: 
 * Effects: 
 */
ASphere::ASphere()
{
}


/*
 *  Method: ASphere::~ASphere
 *  Params: 
 * Effects: 
 */
ASphere::~ASphere()
{
}


/*
 *  Method: ASphere::Create
 *  Params: 
 * Returns: void
 * Effects: 
 */
void ASphere::Create()
{
  unsigned int l_slicesMax = 10;
  unsigned int l_stacksMax = 10;

  // vertices
  for (int stacks = 0; stacks < l_stacksMax; stacks++) 
  {
    float phi = (stacks / (float)(l_stacksMax - 1)) * (float)M_PI;
    for (int slices = 0; slices <= l_slicesMax; slices++) 
    {
      float theta = (slices / (float)l_slicesMax) * 2 * (float)M_PI;

      glm::vec3 v = glm::vec3(cos(theta) * sin(phi), sin(theta) * sin(phi), cos(phi));
      glm::vec2 t = glm::vec2(slices / (float)l_slicesMax, stacks / (float)l_stacksMax);
      glm::vec3 n = v;

      
      m_vertices.push_back( Vertex( v, t, n, glm::vec3(0) ) );
      
    }  
  }
  
  // indices
  m_numTriangles = 0;
	for (int stacks = 0; stacks < l_stacksMax; stacks++) 
  {
		for (int slices = 0; slices < l_slicesMax; slices++) 
    {
			unsigned int nextSlice = slices + 1;
			unsigned int nextStack = (stacks + 1) % l_stacksMax;

			unsigned int index0 = stacks * (l_slicesMax + 1) + slices;
			unsigned int index1 = nextStack * (l_slicesMax + 1) + slices;
			unsigned int index2 = stacks * (l_slicesMax + 1) + nextSlice;
			unsigned int index3 = nextStack * (l_slicesMax + 1) + nextSlice;

      m_indices.push_back(index0);
      m_indices.push_back(index1);
      m_indices.push_back(index2);
			m_numTriangles++;

      m_indices.push_back(index2);
      m_indices.push_back(index1);
      m_indices.push_back(index3);
			m_numTriangles++;
		}
	}
}


