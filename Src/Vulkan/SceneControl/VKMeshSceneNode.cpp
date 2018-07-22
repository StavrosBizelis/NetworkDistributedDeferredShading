#pragma once
#include "Vulkan/SceneControl/VKMeshSceneNode.h"



namespace SceneControl
{
  VKMeshSceneNode::VKMeshSceneNode(SceneNode* a_parent, const std::shared_ptr<IMesh>& a_mesh, std::shared_ptr< std::vector<VulkanRenderable*> > a_updateRegistry  )
  : MeshSceneNode(a_parent, a_mesh), VulkanRenderable(a_updateRegistry)
  {
    m_uniformBuffer = nullptr;
    m_descSet = NULL;
  }
  
  VKMeshSceneNode::~VKMeshSceneNode()
  {
    if( m_uniformBuffer )
      m_uniformBuffer->Free();
    // in future - free the descSet
  }
  
  void VKMeshSceneNode::Init(const VkDescriptorSet& a_descSet, const std::shared_ptr<VulkanMemoryChunk>& a_uniformBuffer, const std::shared_ptr<VulkanMemoryChunk>& a_uniformBuffer2)
  {
    m_descSet = a_descSet;
    m_uniformBuffer = a_uniformBuffer;
    m_uniformBuffer2 = a_uniformBuffer2;
  }
  
  void VKMeshSceneNode::Render(glutil::MatrixStack& a_matrix ) const
  {
  }
  
  void VKMeshSceneNode::Update(const double& a_deltaTime, bool a_dirty, const glm::mat4& a_parentAbsoluteTrans )
  {
    bool l_dirty = m_dirty || a_dirty;
    
    MeshSceneNode::Update(a_deltaTime, a_dirty, a_parentAbsoluteTrans);
    
    if( l_dirty )
    {
      // update the local ubo structure
      m_ubo2.UDiffuse = glm::vec3(1);
      m_ubo2.USpecular = glm::vec3(1);
      m_ubo2.UHardness = 1;
      m_ubo2.UEmissive = glm::vec3(1);;
      
      // register for update
      RegisterForUpdate();
    }
  }
  
  void VKMeshSceneNode::VulkanUpdate(char* a_mappedUBO)
  {
    // map the model matrix
    memcpy(a_mappedUBO+m_uniformBuffer->GetMemoryOffset(), &m_lastAbsoluteTrans, sizeof(VertexObjectMatrices));
    // map the material data
    memcpy(a_mappedUBO+m_uniformBuffer2->GetMemoryOffset(), &m_ubo2, sizeof(FragMaterialData));
  }
  
  
}