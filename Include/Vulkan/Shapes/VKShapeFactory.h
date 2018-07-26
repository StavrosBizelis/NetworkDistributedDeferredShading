#pragma once

#include "Vulkan/Core/VulkanMemory.h"
#include "Common/Shapes/IMesh.h"
#include "Common/Shapes/IShapeFactory.h"
#include <memory>
#include <map>
#include <array>

class VKShapeFactory : public IShapeFactory
{
  std::shared_ptr<ACone> m_cone;
  std::shared_ptr<ACube> m_cube;
  std::map<std::string, std::shared_ptr<AOpenAssetImportMesh> > m_meshes;
  std::shared_ptr<ARect> m_rect;
  std::shared_ptr<ASkybox> m_skybox;
  std::shared_ptr<ASphere> m_sphere;
  
  std::shared_ptr<VulkanMemory> m_memory;
public:
  VKShapeFactory( const std::shared_ptr<VulkanMemory>& a_memory );

  virtual void Init();

  virtual std::shared_ptr<ACone> GetCone() const;
  virtual std::shared_ptr<ACube> GetCube() const;
  virtual std::shared_ptr<AOpenAssetImportMesh> GetOpenAssetImportMesh(const std::string& a_filename);
  virtual std::shared_ptr<ARect> GetRectangle() const;
  virtual std::shared_ptr<ASkybox> GetSkybox() const;
  virtual std::shared_ptr<ASphere> GetSphere() const;
  
  
  static VkVertexInputBindingDescription GetBindingDescription() 
  {
    VkVertexInputBindingDescription l_bindingDescription = {};
    l_bindingDescription.binding = 0;
    l_bindingDescription.stride = sizeof( Vertex );
    l_bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return l_bindingDescription;
  }
  
  static std::array<VkVertexInputAttributeDescription, 4> GetAttributeDescriptions() 
  {
    std::array<VkVertexInputAttributeDescription, 4> l_attributeDescriptions = {};
    
    // pos
    l_attributeDescriptions[0].binding = 0;
    l_attributeDescriptions[0].location = 0;
    l_attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    l_attributeDescriptions[0].offset = offsetof(struct Vertex, m_pos);
    // texture coordinates
    l_attributeDescriptions[1].binding = 0;
    l_attributeDescriptions[1].location = 1;
    l_attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
    l_attributeDescriptions[1].offset = offsetof(struct Vertex, m_tex);
    // normals
    l_attributeDescriptions[2].binding = 0;
    l_attributeDescriptions[2].location = 2;
    l_attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
    l_attributeDescriptions[2].offset = offsetof(struct Vertex, m_normal);  
    // tangents
    l_attributeDescriptions[3].binding = 0;
    l_attributeDescriptions[3].location = 3;
    l_attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
    l_attributeDescriptions[3].offset = offsetof(struct Vertex, m_tangent);
    
    return l_attributeDescriptions;
  }
  
};