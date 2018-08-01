#include "Vulkan//SceneControl/VKSceneManager.h"


#include "Vulkan//SceneControl/VKMeshSceneNode.h"
// #include "Vulkan//SceneControl/VKCameraSceneNode.h"
#include "Vulkan//SceneControl/VKSpotLightSceneNode.h"
#include "Vulkan//SceneControl/VKPointLightSceneNode.h"
#include "Vulkan//SceneControl/VKDirectionalLightSceneNode.h"



namespace SceneControl
{
	VKSceneManager::VKSceneManager()
    : m_updateRegistry(std::make_shared<std::vector<VulkanRenderable*>>() ){}
    
    
	// SceneNode* SceneManager::AddSceneNode(SceneNode* a_parent )
	// {
		// if (a_parent)
		// {
			// if (m_sceneRoot->DescendantExists(a_parent))
				// return new SceneNode(a_parent);
			// else
				// return nullptr;
		// }
		// // else
		// return new SceneNode(m_sceneRoot);
	// }



	MeshSceneNode * VKSceneManager::AddMeshSceneNode(const std::shared_ptr<IMesh>& a_mesh, SceneNode * a_parent)
	{
		if (a_parent)
		{
			if (m_sceneRoot->DescendantExists(a_parent))
				return new VKMeshSceneNode(a_parent, a_mesh, m_updateRegistry);
			else
				return nullptr;
		}
		// else
		return new VKMeshSceneNode(m_sceneRoot, a_mesh, m_updateRegistry);
	}

	

	// CameraSceneNode* SceneManager::AddCameraSceneNode(const std::shared_ptr<CCamera>& a_camera, const CameraType& a_type, SceneNode* a_parent)
	// {
		// if (a_parent)
		// {
			// if (m_sceneRoot->DescendantExists(a_parent))
				// return new VKCameraSceneNode(a_parent, a_camera, a_type, m_updateRegistry);
			// else
				// return nullptr;
		// }
		// // else
		// return new VKCameraSceneNode(m_sceneRoot, a_camera, a_type, m_updateRegistry);

	// }

	PointLightSceneNode * VKSceneManager::AddPointLightSceneNode( const std::shared_ptr<ASphere>& a_sphere, SceneNode* a_parent)
	{
		if (a_parent)
		{
			if (m_sceneRoot->DescendantExists(a_parent))
				return new VKPointLightSceneNode(a_parent, a_sphere, m_updateRegistry );
			else
				return nullptr;
		}
		// else
		return new VKPointLightSceneNode(m_sceneRoot, a_sphere, m_updateRegistry );
	}

	SpotLightSceneNode * VKSceneManager::AddSpotLightSceneNode(const std::shared_ptr<ACone>& a_cone, SceneNode * a_parent)
	{
		if (a_parent)
		{
			if (m_sceneRoot->DescendantExists(a_parent))
				return new VKSpotLightSceneNode(a_parent, a_cone, m_updateRegistry);
			else
				return nullptr;
		}
		// else
		return new VKSpotLightSceneNode(m_sceneRoot, a_cone, m_updateRegistry);
	}

	DirectionalLightSceneNode * VKSceneManager::AddDirectionalLightSceneNode( const std::shared_ptr<ARect>& a_rect, SceneNode * a_parent)
	{
		if (a_parent)
		{
			if (m_sceneRoot->DescendantExists(a_parent))
				return new VKDirectionalLightSceneNode(a_parent, a_rect, m_updateRegistry);
			else
				return nullptr;
		}
		// else
		return new VKDirectionalLightSceneNode(m_sceneRoot, a_rect, m_updateRegistry);
	}	

}