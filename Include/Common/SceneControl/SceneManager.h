#pragma once
#include "SceneNode.h"
#include "CameraSceneNode.h"
#include "MeshSceneNode.h"


#include "Common//RenderControl/RenderPassPipeline.h"

#include "DirectionalLightSceneNode.h"
#include "PointLightSceneNode.h"
#include "SpotLightSceneNode.h"

namespace SceneControl
{
	/// works like a managing factory of scene nodes and places them in the scene
	class SceneManager
	{
		SceneNode* m_sceneRoot;

		//RenderControl::RenderPassPipeline* m_renderer;		///< does not own this



	public:
		SceneManager();
		virtual ~SceneManager();

		SceneNode* GetRoot();

		inline void UpdateScene(const double& a_deltaTime ) 
		{ 
			if (m_sceneRoot) 
				m_sceneRoot->Update(a_deltaTime); 
		}

		// TODO
		/// a_parent = nullptr, then root is the parent
		/// @return - if null a_parent is not part of the scene
		SceneNode* AddSceneNode(SceneNode* a_parent = nullptr);
		MeshSceneNode* AddMeshSceneNode(const std::shared_ptr<IMesh>& a_mesh, SceneNode* a_parent = nullptr);

		CameraSceneNode* AddCameraSceneNode(const std::shared_ptr<CCamera>& a_camera, const CameraType& a_type = FPS_CAMERA, SceneNode* a_parent = nullptr);
		
		PointLightSceneNode* AddPointLightSceneNode( const std::shared_ptr<ASphere>& a_sphere, SceneNode* a_parent = nullptr);
		SpotLightSceneNode* AddSpotLightSceneNode( const std::shared_ptr<ACone>& a_cone, SceneNode* a_parent = nullptr);
		DirectionalLightSceneNode* AddDirectionalLightSceneNode( const std::shared_ptr<ARect>& a_rect, SceneNode* a_parent = nullptr);
	};

}