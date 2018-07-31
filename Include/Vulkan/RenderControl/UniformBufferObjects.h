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
  glm::vec4 UDiffuse;
  glm::vec4 USpecular;
  glm::vec4 UHardness;
  glm::vec4 UEmissive;
};




struct FragDirectionalLight
{
  glm::vec4 m_direction;

  glm::vec4 m_ambient;
  glm::vec4 m_diffuse;
  glm::vec4 m_specular;
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


















