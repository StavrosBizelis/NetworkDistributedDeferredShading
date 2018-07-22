#pragma once
#include "Common/SceneControl/TexturedSceneNode.h"
#include <memory>
#include "Common/Shapes/IMesh.h"


namespace SceneControl
{
	/// Encapsulates a IMesh and renders it
	class MeshSceneNode :
		public TexturedSceneNode
	{
    protected:
		std::shared_ptr<IMesh> m_mesh;

	public:
		MeshSceneNode(SceneNode* a_parent, const std::shared_ptr<IMesh>& a_mesh );
		virtual ~MeshSceneNode();

		virtual void Render(glutil::MatrixStack& a_matrix = glutil::MatrixStack()) const;
	};

}