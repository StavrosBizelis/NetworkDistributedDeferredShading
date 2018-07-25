#pragma once
#include "Common/SceneControl/MeshSceneNode.h"
#include "Vulkan/RenderControl/VulkanRenderable.h"
#include <memory>
#include "Common/Shapes/IMesh.h"
#include "Vulkan/RenderControl/UniformBufferObjects.h"

namespace SceneControl
{
	class VKMeshSceneNode :
		public MeshSceneNode
	{
    VertexObjectMatrices m_ubo;
    FragMaterialData m_ubo2;
    
    VulkanMeshRenderable* m_vulkanRenderOperations;
    
	public:
		VKMeshSceneNode(SceneNode* a_parent, const std::shared_ptr<IMesh>& a_mesh, std::shared_ptr< std::vector<VulkanRenderable*> > a_updateRegistry );
		virtual ~VKMeshSceneNode();
    
    virtual void* GetExtra();
    
    virtual void Render(glutil::MatrixStack& a_matrix = glutil::MatrixStack()) const;
    virtual void Update(const double& a_deltaTime, bool a_dirty = false, const glm::mat4& a_parentAbsoluteTrans = glm::mat4() );
    
  
	};

}