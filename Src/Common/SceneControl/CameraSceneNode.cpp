#include "Common/SceneControl/CameraSceneNode.h"


namespace SceneControl
{

	CameraSceneNode::CameraSceneNode(SceneNode* a_parent, const std::shared_ptr<CCamera>& a_camera, const CameraType& a_type)
		: SceneNode(a_parent), m_cameraImpl(a_camera), m_type(a_type){}


	CameraSceneNode::~CameraSceneNode()
	{
	}
	void CameraSceneNode::Update(const double & a_deltaTime, bool a_dirty, const glm::mat4& a_parentAbsoluteTrans)
	{

		glm::mat4 l_trans = a_parentAbsoluteTrans;
		bool l_camDirty = true;
				
		if (m_cameraImpl)
		{
			// fps camera isnt affected by the scene
			if (m_type == FPS_CAMERA)
			{	
				m_cameraImpl->Update(a_deltaTime);
				l_trans = glm::translate(l_trans, m_cameraImpl->GetPosition());
			}
			else //STATIC_CAMERA
			{
				l_trans = glm::translate(l_trans, m_cameraImpl->GetPosition());
			}
		}
				SceneNode::Update(a_deltaTime, l_camDirty, l_trans);
		

	}
	std::shared_ptr<CCamera> CameraSceneNode::GetCamera() const { return m_cameraImpl; }

	CameraType CameraSceneNode::GetType() const
	{
		return m_type;
	}

	void CameraSceneNode::SetType(const CameraType & a_type)
	{
		m_type = a_type;
	}

}