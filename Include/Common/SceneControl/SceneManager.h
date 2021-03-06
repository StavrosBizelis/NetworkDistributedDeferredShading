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
    protected:
		SceneNode* m_sceneRoot;

		//RenderControl::RenderPassPipeline* m_renderer;		///< does not own this



	public:
		SceneManager();
		virtual ~SceneManager();

		SceneNode* GetRoot();

		virtual void UpdateScene(const double& a_deltaTime ) 
		{ 
			if (m_sceneRoot) 
				m_sceneRoot->Update(a_deltaTime); 
		}
    
    /// removes and deletes scene node if it exists under this hierarchy
    /// @return - true for success
    virtual bool DetachNode(SceneNode* a_node);

		/// @return - if null, a_parent is not part of the scene
		virtual SceneNode* AddSceneNode(SceneNode* a_parent = nullptr);
		/// @return - if null, a_parent is not part of the scene
		virtual MeshSceneNode* AddMeshSceneNode(const std::shared_ptr<IMesh>& a_mesh, SceneNode* a_parent = nullptr);

		/// @return - if null, a_parent is not part of the scene
		virtual CameraSceneNode* AddCameraSceneNode(const std::shared_ptr<CCamera>& a_camera, const CameraType& a_type = FPS_CAMERA, SceneNode* a_parent = nullptr);
		
		/// @return - if null, a_parent is not part of the scene
		virtual PointLightSceneNode* AddPointLightSceneNode( const std::shared_ptr<ASphere>& a_sphere, SceneNode* a_parent = nullptr);
		/// @return - if null, a_parent is not part of the scene
		virtual SpotLightSceneNode* AddSpotLightSceneNode( const std::shared_ptr<ACone>& a_cone, SceneNode* a_parent = nullptr);
		/// @return - if null, a_parent is not part of the scene
		virtual DirectionalLightSceneNode* AddDirectionalLightSceneNode( const std::shared_ptr<ARect>& a_rect, SceneNode* a_parent = nullptr);
	};

}