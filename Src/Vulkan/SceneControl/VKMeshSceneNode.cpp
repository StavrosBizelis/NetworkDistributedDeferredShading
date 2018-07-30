#pragma once
#include "Vulkan/SceneControl/VKMeshSceneNode.h"
#include <iostream>


namespace SceneControl
{
  VKMeshSceneNode::VKMeshSceneNode(SceneNode* a_parent, const std::shared_ptr<IMesh>& a_mesh, std::shared_ptr< std::vector<VulkanRenderable*> > a_updateRegistry  )
  : MeshSceneNode(a_parent, a_mesh)
  {
   
    m_vulkanRenderOperations = new VulkanMeshRenderable(a_mesh, a_updateRegistry);
  }
  
  VKMeshSceneNode::~VKMeshSceneNode()
  {
   
    // in future - free the descSet
    
    delete m_vulkanRenderOperations;
  }
  
  void* VKMeshSceneNode::GetExtra()
  {
    return reinterpret_cast<void*>(m_vulkanRenderOperations);
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
      m_vulkanRenderOperations->m_ubo2.UDiffuse = glm::vec4(1);
      m_vulkanRenderOperations->m_ubo2.USpecular = glm::vec4(1);
      m_vulkanRenderOperations->m_ubo2.UHardness = glm::vec4(1);
      m_vulkanRenderOperations->m_ubo2.UEmissive = glm::vec4(1);
      
      m_vulkanRenderOperations->m_lastAbsoluteTrans = m_lastAbsoluteTrans;
      
      // register for update
      m_vulkanRenderOperations->RegisterForUpdate();
    }
  }
  
  
  
}