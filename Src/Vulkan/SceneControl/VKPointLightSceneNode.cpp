#include "Vulkan/SceneControl/VKPointLightSceneNode.h"



namespace SceneControl
{
  VKPointLightSceneNode::VKPointLightSceneNode(SceneNode* a_parent, std::shared_ptr<ASphere> a_sphere, std::vector<VKMeshSceneNode*>& a_updateRegistry  )
  : TexturedSceneNode(a_parent), VulkanRenderable(a_updateRegistry)
  {
    m_mesh = std::dynamic_pointer_cast<VKSphere>(a_sphere);
    m_uniformBuffer = nullptr;
    m_descSet = NULL;
  }
  
  VKPointLightSceneNode::~VKPointLightSceneNode()
  {
    if( m_uniformBuffer )
      m_uniformBuffer->Free();
    // in future - free the descSet
  }
  
  void VKPointLightSceneNode::Init(const VkDescriptorSet& a_descSet, const std::shared_ptr<VulkanMemoryChunk>& a_uniformBuffer, const std::shared_ptr<VulkanMemoryChunk>& a_uniformBuffer2)
  {
    m_descSet = a_descSet;
    m_uniformBuffer = a_uniformBuffer;
    m_uniformBuffer2 = a_uniformBuffer2;
  }
  
  void VKPointLightSceneNode::Render(glutil::MatrixStack& a_matrix = glutil::MatrixStack()) const{}
  
  void VKPointLightSceneNode::Update(const double& a_deltaTime, bool a_dirty = false, const glm::mat4& a_parentAbsoluteTrans = glm::mat4() )
  {
    if( m_dirty || a_dirty )
      RegisterForUpdate();
    
    PointLightSceneNode::Update(a_deltaTime, a_dirty, a_parentAbsoluteTrans);
  }
  
  virtual void VKPointLightSceneNode::VulkanUpdate(char* a_mappedUBO)
  {
    // map the model matrix
    memcpy(a_mappedUBO+m_uniformBuffer->GetMemoryOffset(), &m_lastAbsoluteTrans, sizeof(VertexObjectMatrices));
    // map the point light data
    memcpy(a_mappedUBO+m_uniformBuffer2->GetMemoryOffset(), &m_ubo2, sizeof(FragPointLight) );
  }
  
  
}