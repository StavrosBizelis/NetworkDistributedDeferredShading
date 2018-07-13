#pragma once

#include "Vulkan/Shapes/VKShapeFactory.h"
#include "Vulkan/Shapes/VKCone.h"
#include "Vulkan/Shapes/VKCube.h"
#include "Vulkan/Shapes/VKOpenAssetImportMesh.h"
#include "Vulkan/Shapes/VKRect.h"
#include "Vulkan/Shapes/VKSkybox.h"
#include "Vulkan/Shapes/VKSphere.h"


VKShapeFactory::VKShapeFactory(const std::shared_ptr<VulkanMemory>& a_memory )
: m_memory(a_memory) {}

void VKShapeFactory::Init()
{
  m_cone = std::make_shared<VKCone>(m_memory);
  m_cone->Create();
  m_cube = std::make_shared<VKCube>(m_memory);
  m_cube->Create();
  m_rect = std::make_shared<VKRect>(m_memory);
  m_rect->Create();
  m_skybox = std::make_shared<VKSkybox>(m_memory);
  m_skybox->Create();
  m_sphere = std::make_shared<VKSphere>(m_memory);
  m_sphere->Create();
}
std::shared_ptr<ACone> VKShapeFactory::GetCone() const
{
  return m_cone;
}
std::shared_ptr<ACube> VKShapeFactory::GetCube() const
{
  return m_cube;  
}
std::shared_ptr<AOpenAssetImportMesh> VKShapeFactory::GetOpenAssetImportMesh(const std::string& a_filename)
{
  // if already exists return it
  std::map<std::string, std::shared_ptr<AOpenAssetImportMesh> >::iterator l_iter = m_meshes.find(a_filename);
  if( l_iter != m_meshes.end() )
    return l_iter->second;
  // otherwise create it
  std::shared_ptr<VKOpenAssetImportMesh> l_mesh = std::make_shared<VKOpenAssetImportMesh>(m_memory);
  if( l_mesh->Load(a_filename) )
    m_meshes[a_filename] = l_mesh;
  
  return l_mesh;
}

std::shared_ptr<ARect> VKShapeFactory::GetRectangle() const
{
  return m_rect;
}
std::shared_ptr<ASkybox> VKShapeFactory::GetSkybox() const
{
  return m_skybox;
}
std::shared_ptr<ASphere> VKShapeFactory::GetSphere() const
{
  return m_sphere;
}
