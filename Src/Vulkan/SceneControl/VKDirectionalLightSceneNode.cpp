#include "Vulkan/SceneControl/VKDirectionalLightSceneNode.h"



namespace SceneControl
{
  VKDirectionalLightSceneNode::VKDirectionalLightSceneNode(SceneNode* a_parent, std::shared_ptr<ARect> m_internalMesh, std::shared_ptr< std::vector<VulkanRenderable*> > a_updateRegistry  )
  : DirectionalLightSceneNode(a_parent, m_internalMesh), VulkanRenderable(a_updateRegistry)
  {
    m_uniformBuffer = nullptr;
    m_descSet = NULL;
  }
  
  VKDirectionalLightSceneNode::~VKDirectionalLightSceneNode()
  {
    if( m_uniformBuffer )
      m_uniformBuffer->Free();
    // in future - free the descSet
  }
  
  void VKDirectionalLightSceneNode::Init(const VkDescriptorSet& a_descSet, const std::shared_ptr<VulkanMemoryChunk>& a_uniformBuffer, const std::shared_ptr<VulkanMemoryChunk>& a_uniformBuffer2)
  {
    m_descSet = a_descSet;
    m_uniformBuffer = a_uniformBuffer;
    m_uniformBuffer2 = a_uniformBuffer2;
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
      
      m_ubo2.m_direction = glm::vec3(l_direction.x, l_direction.y, l_direction.z);

      m_ubo2.m_ambient = GetAmbient();
      m_ubo2.m_diffuse = GetDiffuse();
      m_ubo2.m_specular = GetSpecular();
  
      // register for update
      RegisterForUpdate();
    }
  }
  
  void VKDirectionalLightSceneNode::VulkanUpdate(char* a_mappedUBO)
  {
    // map the model matrix
    memcpy(a_mappedUBO+m_uniformBuffer->GetMemoryOffset(), &m_lastAbsoluteTrans, sizeof(VertexObjectMatrices));
    // map the directional light data
    memcpy(a_mappedUBO+m_uniformBuffer2->GetMemoryOffset(), &m_ubo2, sizeof(FragDirectionalLight));
  }
  
  
}