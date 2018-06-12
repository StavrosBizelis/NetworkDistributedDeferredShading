#pragma once
#include "Common/SceneControl/SceneNode.h"
#include "Common/RenderControl/Camera.h"
namespace SceneControl
{

	enum CameraType
	{
		FPS_CAMERA = 0,
		STATIC_CAMERA
	};

	/// Class that encapsulates a RenderPass camera as any other scene node
	class CameraSceneNode :
		public SceneNode
	{
		std::shared_ptr<CCamera> m_cameraImpl;
		CameraType m_type;

	public:
		CameraSceneNode(SceneNode* a_parent, const std::shared_ptr<CCamera>& a_camera, const CameraType& a_type= FPS_CAMERA);
		virtual ~CameraSceneNode();

		virtual void Update(const double& a_deltaTime, bool a_dirty = false, const glm::mat4& a_parentAbsoluteTrans = glm::mat4());
		
		std::shared_ptr<CCamera> GetCamera() const;
		CameraType GetType() const;
		void SetType(const CameraType& a_type);
	};

}