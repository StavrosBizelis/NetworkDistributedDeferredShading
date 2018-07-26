#pragma once

#include <vector>
#include <glm/glm.hpp>
struct Vertex
{
  glm::vec3 m_pos;
  glm::vec2 m_tex;
  glm::vec3 m_normal;
  glm::vec3 m_tangent;

  Vertex() :m_pos(1),m_tex(0), m_normal(0), m_tangent(0) {}
  
  Vertex(const glm::vec3& pos, const glm::vec2& tex, const glm::vec3& normal, const glm::vec3& tan)
  {
    m_pos    = pos;
    m_tex    = tex;
    m_normal = normal;
    m_tangent = tan;
  }
};


/// interface for primitive meshes so they can be used in MeshSceneNode
class IMesh
{
public:
	virtual void Render() = 0;
  virtual void Create() = 0;
	virtual void Release() = 0;
  
  virtual void* GetExtra() {return nullptr;}
};