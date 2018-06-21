#pragma once

#include "OpenGL/Shapes/GLShapeFactory.h"
#include "OpenGL/Shapes/GLCone.h"
#include "OpenGL/Shapes/GLCube.h"
#include "OpenGL/Shapes/GLOpenAssetImportMesh.h"
#include "OpenGL/Shapes/GLRect.h"
#include "OpenGL/Shapes/GLSkybox.h"
#include "OpenGL/Shapes/GLSphere.h"


void GLShapeFactory::Init()
{
  m_cone = std::make_shared<GLCone>();
  m_cube = std::make_shared<GLCube>();
  m_rect = std::make_shared<GLRect>();
  m_skybox = std::make_shared<GLSkybox>();
  m_sphere = std::make_shared<GLSphere>();
}
std::shared_ptr<IMesh> GLShapeFactory::GetCone()
{
  return m_cone;
}
std::shared_ptr<IMesh> GLShapeFactory::GetCube()
{
  return m_cube;  
}
std::shared_ptr<IMesh> GLShapeFactory::GetOpenAssetImportMesh(const std::string& a_filename)
{
  // if already exists return it
  std::map<std::string, std::shared_ptr<IMesh> >::iterator l_iter = m_meshes.find(a_filename);
  if( l_iter != m_meshes.end() )
    return l_iter->second;
  // otherwise create it
  std::shared_ptr<GLOpenAssetImportMesh> l_mesh = std::make_shared<GLOpenAssetImportMesh>();
  if( l_mesh->Load(a_filename) )
    m_meshes[a_filename] = l_mesh;
  
  return l_mesh;
}

std::shared_ptr<IMesh> GLShapeFactory::GetRectangle()
{
  return m_rect;
}
std::shared_ptr<IMesh> GLShapeFactory::GetSkybox()
{
  return m_skybox;
}
std::shared_ptr<IMesh> GLShapeFactory::GetSphere()
{
  return m_sphere;
}
