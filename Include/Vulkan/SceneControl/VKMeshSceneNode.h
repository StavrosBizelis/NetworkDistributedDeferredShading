#pragma once
#include "Common/SceneControl/MeshSceneNode.h"
#include "Vulkan/RenderControl/VulkanRenderable.h"
#include <memory>
#include "Common/Shapes/IMesh.h"
#include "Vulkan/RenderControl/UniformBufferObjects.h"

namespace SceneControl
{
	class VKMeshSceneNode :
		public MeshSceneNode, public VulkanRenderable
	{
    std::shared_ptr<VulkanMemoryChunk> m_uniformBuffer;
    std::shared_ptr<VulkanMemoryChunk> m_uniformBuffer2;
    VkDescriptorSet m_descSet;
    VertexObjectMatrices m_ubo;
    FragMaterialData m_ubo2;
    
	public:
		VKMeshSceneNode(SceneNode* a_parent, const std::shared_ptr<IMesh>& a_mesh, std::shared_ptr< std::vector<VulkanRenderable*> > a_updateRegistry );
		virtual ~VKMeshSceneNode();
    
    /// @param a_uniformBuffer - use this to update the GPU uniform data
    virtual void Init(const VkDescriptorSet& a_descSet, const std::shared_ptr<VulkanMemoryChunk>& a_uniformBuffer, const std::shared_ptr<VulkanMemoryChunk>& a_uniformBuffer2 );
    
		virtual void Render(glutil::MatrixStack& a_matrix = glutil::MatrixStack()) const;
    virtual void Update(const double& a_deltaTime, bool a_dirty = false, const glm::mat4& a_parentAbsoluteTrans = glm::mat4() );
    
    
    virtual void SetDesciptorSet(const VkDescriptorSet& a_descSet){m_descSet = a_descSet;}
    virtual VkDescriptorSet* GetDesciptorSet(){return &m_descSet;}
    virtual std::shared_ptr<VKShape> GetShape(){ return std::dynamic_pointer_cast<VKShape>(m_mesh);}
    virtual std::shared_ptr<VulkanMemoryChunk> GetUniformBuffer(){return m_uniformBuffer;}
    virtual std::shared_ptr<VKATexture> GetVKTexture(const unsigned int& a_index){ return std::dynamic_pointer_cast<VKATexture>( GetTexture(a_index) ); }
    virtual void VulkanUpdate(char* a_mappedUBO);
	};

}