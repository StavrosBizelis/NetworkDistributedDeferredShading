#pragma once

#include <glm/glm.hpp>

// Vertex Uniform Buffer objects
struct VertexViewProjMatrices
{
  glm::mat4 projMatrix;
  glm::mat4 viewMatrix;
};

struct VertexObjectMatrices
{
  glm::mat4 modelMatrix;
};




//  fragment uniform buffer objects

struct FragLightGlobalVars 
{
  glm::mat4 UInverseViewProjectionMatrix;
  glm::vec3 UCamPos;
  glm::vec2 UScreenResDiv;
};



struct FragMaterialData
{
  glm::vec3 UDiffuse;
  glm::vec3 USpecular;
  float UHardness;
  glm::vec3 UEmissive;
};




struct FragDirectionalLight
{
  glm::vec3 m_direction;

  glm::vec3 m_ambient;
  glm::vec3 m_diffuse;
  glm::vec3 m_specular;
};

struct FragPointLight
{
  glm::vec3 m_position;

  //vec3 m_ambient;
  glm::vec3 m_diffuse;
  glm::vec3 m_specular;
  
  // attenuation
  float m_constantAtt;
  float m_linearAtt;
  float m_quadraticAtt;
};

struct FragSpotLight
{
  glm::vec3 m_position;
  glm::vec3 m_direction;
  float m_spotCutoff;
  float m_spotExponent;
  
  //vec3 m_ambient;
  glm::vec3 m_diffuse;
  glm::vec3 m_specular;
  
  // attenuation
  float m_constantAtt;
  float m_linearAtt;
  float m_quadraticAtt;
};


















