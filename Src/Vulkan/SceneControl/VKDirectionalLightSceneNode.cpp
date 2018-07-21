#include "Vulkan/SceneControl/VKDirectionalLightSceneNode.h"



namespace SceneControl
{
  VKDirectionalLightSceneNode::VKDirectionalLightSceneNode(SceneNode* a_parent, std::shared_ptr<ASphere> a_sphere, std::vector<VKMeshSceneNode*>& a_updateRegistry  )
  : TexturedSceneNode(a_parent), VulkanRenderable(a_updateRegistry)
  {
    m_mesh = std::dynamic_pointer_cast<VKSphere>(a_sphere);
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
  
  void VKDirectionalLightSceneNode::Render(glutil::MatrixStack& a_matrix = glutil::MatrixStack()) const{}
  
  void VKDirectionalLightSceneNode::Update(const double& a_deltaTime, bool a_dirty = false, const glm::mat4& a_parentAbsoluteTrans = glm::mat4() )
  {
    if( m_dirty || a_dirty )
      RegisterForUpdate();
    
    PointLightSceneNode::Update(a_deltaTime, a_dirty, a_parentAbsoluteTrans);
  }
  
  virtual void VKDirectionalLightSceneNode::VulkanUpdate(char* a_mappedUBO)
  {
    // map the model matrix
    memcpy(a_mappedUBO+m_uniformBuffer->GetMemoryOffset(), &m_lastAbsoluteTrans, sizeof(VertexObjectMatrices));
    // map the directional light data
    memcpy(a_mappedUBO+m_uniformBuffer2->GetMemoryOffset(), &m_ubo2, sizeof(FragDirectionalLight));
  }
  
  
}