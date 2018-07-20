#pragma once

#include <glm/glm.hpp>

// Vertex Uniform Buffer objects
struct VertexSingleMat4 
{
  glm::mat4 projMatrix;
};

struct VertexObjectMatrices
{
  glm::mat4 modelViewMatrix;
  glm::mat3 normalMatrix;
};

struct FragDirLightGlobalVars
{
  glm::mat4 UInverseViewProjectionMatrix;
  glm::vec3 UCamPos;
};

//  frugment uniform buffer objects
struct FragDirectionalLight
{
  glm::vec3 m_direction;

  glm::vec3 m_ambient;
  glm::vec3 m_diffuse;
  glm::vec3 m_specular;
};



struct FragLightGlobalVars 
{
  glm::mat4 UInverseViewProjectionMatrix;
  glm::vec3 UCamPos;
  glm::vec2 UScreenResDiv;
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



struct FragMaterialData
{
  glm::vec3 UDiffuse;
  glm::vec3 USpecular;
  float UHardness;
  glm::vec3 UEmissive;
};


















