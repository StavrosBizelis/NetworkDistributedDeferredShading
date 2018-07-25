#include "Vulkan/SceneControl/VKDirectionalLightSceneNode.h"



namespace SceneControl
{
  VKDirectionalLightSceneNode::VKDirectionalLightSceneNode(SceneNode* a_parent, std::shared_ptr<ARect> m_internalMesh, std::shared_ptr< std::vector<VulkanRenderable*> > a_updateRegistry  )
  : DirectionalLightSceneNode(a_parent, m_internalMesh)
  {
   
    m_vulkanRenderOperations = new VulkanDirLightRenderable(m_internalMesh, a_updateRegistry);
  }
  
  VKDirectionalLightSceneNode::~VKDirectionalLightSceneNode()
  {
    delete m_vulkanRenderOperations;
  }
  
  void* VKDirectionalLightSceneNode::GetExtra()
  {
    return reinterpret_cast<void*>(m_vulkanRenderOperations);
  }


  
  void VKDirectionalLightSceneNode::Render(glutil::MatrixStack& a_matrix ) const{}
  
  void VKDirectionalLightSceneNode::Update(const double& a_deltaTime, bool a_dirty, const glm::mat4& a_parentAbsoluteTrans  )
  {
    bool l_dirty = m_dirty || a_dirty;
    
    VKDirectionalLightSceneNode::Update(a_deltaTime, a_dirty, a_parentAbsoluteTrans);
    
    if( l_dirty )
    {
      // update the local ubo structure
      glm::vec4 l_direction( 0.0f, -1.f, 0.f, 1.f);
      l_direction = GetRot() * l_direction;
      
      m_vulkanRenderOperations->m_ubo2.m_direction = glm::vec3(l_direction.x, l_direction.y, l_direction.z);

      m_vulkanRenderOperations->m_ubo2.m_ambient = GetAmbient();
      m_vulkanRenderOperations->m_ubo2.m_diffuse = GetDiffuse();
      m_vulkanRenderOperations->m_ubo2.m_specular = GetSpecular();
  
      m_vulkanRenderOperations->m_ubo.modelMatrix = m_lastAbsoluteTrans;
      
      // register for update
      m_vulkanRenderOperations->RegisterForUpdate();
    }
  }

  
}