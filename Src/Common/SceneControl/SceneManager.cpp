#include "Common/SceneControl/SceneManager.h"

namespace SceneControl
{

	SceneManager::SceneManager()
		: m_sceneRoot( new SceneNode() )
	{
	}


	SceneManager::~SceneManager()
	{
		if(m_sceneRoot)
			delete m_sceneRoot;
	}

	SceneNode * SceneManager::GetRoot()
	{
		return m_sceneRoot;
	}
  
  bool SceneManager::DetachNode(SceneNode* a_node)
  {
    return m_sceneRoot->DetachDescendant(a_node);
  }

	SceneNode* SceneManager::AddSceneNode(SceneNode* a_parent )
	{
		if (a_parent)
		{
			if (m_sceneRoot->DescendantExists(a_parent))
				return new SceneNode(a_parent);
			else
				return nullptr;
		}
		// else
		return new SceneNode(m_sceneRoot);
	}



	MeshSceneNode * SceneManager::AddMeshSceneNode(const std::shared_ptr<IMesh>& a_mesh, SceneNode * a_parent)
	{
		if (a_parent)
		{
			if (m_sceneRoot->DescendantExists(a_parent))
				return new MeshSceneNode(a_parent, a_mesh);
			else
				return nullptr;
		}
		// else
		return new MeshSceneNode(m_sceneRoot, a_mesh);
	}

	

	CameraSceneNode* SceneManager::AddCameraSceneNode(const std::shared_ptr<CCamera>& a_camera, const CameraType& a_type, SceneNode* a_parent)
	{
		if (a_parent)
		{
			if (m_sceneRoot->DescendantExists(a_parent))
				return new CameraSceneNode(a_parent, a_camera, a_type);
			else
				return nullptr;
		}
		// else
		return new CameraSceneNode(m_sceneRoot, a_camera, a_type);

	}

	PointLightSceneNode * SceneManager::AddPointLightSceneNode( const std::shared_ptr<ASphere>& a_sphere, SceneNode* a_parent)
	{
		if (a_parent)
		{
			if (m_sceneRoot->DescendantExists(a_parent))
				return new PointLightSceneNode(a_parent, a_sphere );
			else
				return nullptr;
		}
		// else
		return new PointLightSceneNode(m_sceneRoot, a_sphere );
	}

	SpotLightSceneNode * SceneManager::AddSpotLightSceneNode(const std::shared_ptr<ACone>& a_cone, SceneNode * a_parent)
	{
		if (a_parent)
		{
			if (m_sceneRoot->DescendantExists(a_parent))
				return new SpotLightSceneNode(a_parent , a_cone);
			else
				return nullptr;
		}
		// else
		return new SpotLightSceneNode(m_sceneRoot, a_cone);
	}

	DirectionalLightSceneNode * SceneManager::AddDirectionalLightSceneNode( const std::shared_ptr<ARect>& a_rect, SceneNode * a_parent)
	{
		if (a_parent)
		{
			if (m_sceneRoot->DescendantExists(a_parent))
				return new DirectionalLightSceneNode(a_parent, a_rect);
			else
				return nullptr;
		}
		// else
		return new DirectionalLightSceneNode(m_sceneRoot, a_rect);
	}
}