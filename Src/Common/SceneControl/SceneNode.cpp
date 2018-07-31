#include "Common/SceneControl/SceneNode.h"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/quaternion.hpp>
#include <iostream>
namespace SceneControl
{
	std::map<unsigned int, SceneNode*> SceneNode::s_idRegistry;
  
	SceneNode::SceneNode(SceneNode* a_parent)
		: m_parent(a_parent), m_id(0), m_posRel(0.f), m_orientRel(), m_scaleRel(1.f), m_dirty(true)
	{
		if (a_parent) a_parent->AddChild(this);
		
	}


	SceneNode::~SceneNode()
	{
		for (std::set<SceneNode*>::iterator l_iter = m_children.begin(); l_iter != m_children.end(); ++l_iter)
			if (*l_iter) delete (*l_iter);
    if( m_id != 0 )
      s_idRegistry.erase(m_id);
		// noneed to clear here
		// m_children.clear();
	}

	bool SceneNode::DescendantExists(SceneNode * a_node) const
	{
		// check children
		for (std::set<SceneNode*>::iterator l_iter = m_children.begin(); l_iter != m_children.end(); ++l_iter)
			if (*l_iter == a_node)
				return true;

		// check descedants of children
		for (std::set<SceneNode*>::iterator l_iter = m_children.begin(); l_iter != m_children.end(); ++l_iter)
			if ((*l_iter)->DescendantExists(a_node))
				return true;

		// if we reach that point it means that a_node is not a direct child neither a descendant of any children
		return false;
	}

	void SceneNode::Update(const double & a_deltaTime, bool a_dirty, const glm::mat4& a_parentAbsoluteTrans)
	{
		if (!GetEnabled())
			return;

		bool l_dirty = a_dirty || m_dirty;
		if (l_dirty)
		{
			m_lastAbsoluteTrans = a_parentAbsoluteTrans;
			m_lastAbsoluteTrans = glm::translate(m_lastAbsoluteTrans, m_posRel);
			m_lastAbsoluteTrans *= m_orientRel;
			m_lastAbsoluteTrans = glm::scale(m_lastAbsoluteTrans, m_scaleRel);
			m_dirty = false;
			m_absoluteScale = GetScale();
		}


		for (std::set<SceneNode*>::iterator l_iter = m_children.begin(); l_iter != m_children.end(); ++l_iter)
			(*l_iter)->Update(a_deltaTime, l_dirty, m_lastAbsoluteTrans);
	}

	glm::vec3 SceneNode::GetRelativePos() const
	{
		return m_posRel;
	}

	glm::mat4 SceneNode::GetRelativeRot() const
	{
		return m_orientRel;
	}
  
  glm::vec3 SceneNode::GetRelativeEulerAngles() const
  {
    glm::quat l_quat = glm::toQuat(GetRelativeRot());
    return glm::vec3(glm::pitch(l_quat), glm::yaw(l_quat) ,glm::roll(l_quat)  );
  }

	glm::vec3 SceneNode::GetRelativeScale() const
	{
		return m_scaleRel;
	}



	glm::vec3 SceneNode::GetPos() const
	{
		if (!m_parent)
			return m_posRel;
		
		glm::mat4 l_tmp;
		l_tmp = glm::translate(l_tmp, m_posRel);
		l_tmp =  m_parent->GetRot() * l_tmp;
		glm::vec3 l_actualPosition(
			l_tmp[3][0],
			l_tmp[3][1],
			l_tmp[3][2]
		);;

		return m_parent->GetPos() + l_actualPosition;
	}

	glm::mat4 SceneNode::GetRot() const
	{
		if (!m_parent)
			return m_orientRel;
		return  m_parent->GetRot() * m_orientRel;
	}
  glm::vec3 SceneNode::GetEulerAngles() const
  {
    glm::quat l_quat = glm::toQuat(GetRot());
    return glm::vec3(glm::pitch(l_quat), glm::yaw(l_quat) ,glm::roll(l_quat) );
  }
  

	glm::vec3 SceneNode::GetScale() const
	{
		if (!m_parent)
			return m_scaleRel;
		glm::vec3 l_vec3 = m_parent->GetScale();
		return glm::vec3(l_vec3.x * m_scaleRel.x, l_vec3.y * m_scaleRel.y, l_vec3.z * m_scaleRel.z);
	}

	void SceneNode::SetRelativePos(const glm::vec3 & a_pos) { m_posRel = a_pos; m_dirty = true; }

	void SceneNode::SetRelativeRot(const glm::mat4 & a_rotation) { m_orientRel = a_rotation;  m_dirty = true; }

	void SceneNode::SetRelativeEulerAngles(const glm::vec3 & a_eulerAngles)
	{
		m_orientRel = glm::rotate(glm::mat4(), a_eulerAngles.x, glm::vec3(1.f, 0.f, 0.f));
		m_orientRel = glm::rotate(m_orientRel, a_eulerAngles.y, glm::vec3(0.f, 1.f, 0.f));
		m_orientRel = glm::rotate(m_orientRel, a_eulerAngles.z, glm::vec3(0.f, 0.f, 1.f));
		m_dirty = true;
	}

	void SceneNode::SetRelativeScale(const glm::vec3 & a_scale) { m_scaleRel = a_scale;  m_dirty = true;  }

	void SceneNode::SetPos(const glm::vec3 & a_pos)
	{
		if (!m_parent)
			SetRelativePos(a_pos);
		else
			SetRelativePos(a_pos - m_parent->GetPos());
	}

	void SceneNode::SetRot(const glm::mat4 & a_rotation)
	{
		if (!m_parent)
			SetRelativeRot(a_rotation);
		else
			SetRelativeRot(a_rotation * glm::inverse(m_parent->GetRot()));

	}

	void SceneNode::SetEulerAngles(const glm::vec3 & a_eulerAngles)
	{
		if (!m_parent)
			SetRelativeEulerAngles(a_eulerAngles);
		else
		{
			SetRelativeRot(glm::inverse(m_parent->GetRot()));
			m_orientRel = glm::rotate(m_orientRel, a_eulerAngles.x, glm::vec3(1.f, 0.f, 0.f));
			m_orientRel = glm::rotate(m_orientRel, a_eulerAngles.y, glm::vec3(0.f, 1.f, 0.f));
			m_orientRel = glm::rotate(m_orientRel, a_eulerAngles.z, glm::vec3(0.f, 0.f, 1.f));
		}
		m_dirty = true;

	}

	void SceneNode::SetScale(const glm::vec3 & a_scale)
	{
		if (!m_parent)
			SetRelativeScale(a_scale);
		else
		{
			glm::vec3 l_v = m_parent->GetScale();
			SetRelativeScale(glm::vec3(a_scale.x / l_v.x, a_scale.y / l_v.y, a_scale.z / l_v.z) );
		}
	}

	void SceneNode::SetEnabled(bool a_val)
	{
		IRenderable::SetEnabled(a_val);
		for (std::set<SceneNode*>::iterator l_iter = m_children.begin(); l_iter != m_children.end(); ++l_iter)
			(*l_iter)->SetEnabled(a_val);
	}

	bool SceneNode::SetPersistentUniform(const unsigned int & a_materialIndex, const std::string & a_name, const int & a_var)
	{
    m_dirty = true;
		if (a_materialIndex < m_materialUniformsInt.size())
		{
			m_materialUniformsInt[a_materialIndex][a_name] = a_var;
			return true;
		}
		return false;
	}

	bool SceneNode::SetPersistentUniform(const unsigned int & a_materialIndex, const std::string & a_name, const float & a_var)
	{
    m_dirty = true;
		if (a_materialIndex < m_materialUniformsFloat.size())
		{
			m_materialUniformsInt[a_materialIndex][a_name] = (int)a_var;
			return true;
		}
		return false;
	}

	bool SceneNode::SetPersistentUniform(const unsigned int & a_materialIndex, const std::string & a_name, const glm::vec2 & a_var)
	{
    m_dirty = true;
		if (a_materialIndex < m_materialUniformsVec2.size())
		{
			m_materialUniformsVec2[a_materialIndex][a_name] = a_var;
			return true;
		}
		return false;
	}

	bool SceneNode::SetPersistentUniform(const unsigned int & a_materialIndex, const std::string & a_name, const glm::vec3 & a_var)
	{
    m_dirty = true;
		if (a_materialIndex < m_materialUniformsVec3.size())
		{
			m_materialUniformsVec3[a_materialIndex][a_name] = a_var;
			return true;
		}
		return false;
	}

	bool SceneNode::SetPersistentUniform(const unsigned int & a_materialIndex, const std::string & a_name, const glm::vec4 & a_var)
	{
    m_dirty = true;
		if (a_materialIndex < m_materialUniformsVec4.size())
		{
			m_materialUniformsVec4[a_materialIndex][a_name] = a_var;
			return true;
		}
		return false;
	}

	bool SceneNode::SetPersistentUniform(const unsigned int & a_materialIndex, const std::string & a_name, const glm::mat3 & a_var)
	{
    m_dirty = true;
		if (a_materialIndex < m_materialUniformsMat3.size())
		{
			m_materialUniformsMat3[a_materialIndex][a_name] = a_var;
			return true;
		}
		return false;
	}

	bool SceneNode::SetPersistentUniform(const unsigned int & a_materialIndex, const std::string & a_name, const glm::mat4 & a_var)
	{
    m_dirty = true;
		if (a_materialIndex < m_materialUniformsMat4.size() )
		{
			m_materialUniformsMat4[a_materialIndex][a_name] = a_var;
			return true;
		}
		return false;
	}

	void SceneNode::BindPersistentUniforms(const unsigned int& a_materialIndex) const
	{

		for (std::unordered_map<std::string, int>::const_iterator l_iter = m_materialUniformsInt[a_materialIndex].cbegin(); l_iter != m_materialUniformsInt[a_materialIndex].cend(); ++l_iter)
			m_material[a_materialIndex]->SetUniform(l_iter->first, l_iter->second);
		for (std::unordered_map<std::string, float>::const_iterator l_iter = m_materialUniformsFloat[a_materialIndex].cbegin(); l_iter != m_materialUniformsFloat[a_materialIndex].cend(); ++l_iter)
			m_material[a_materialIndex]->SetUniform(l_iter->first, l_iter->second);
		for (std::unordered_map<std::string, glm::vec2>::const_iterator l_iter = m_materialUniformsVec2[a_materialIndex].cbegin(); l_iter != m_materialUniformsVec2[a_materialIndex].cend(); ++l_iter)
			m_material[a_materialIndex]->SetUniform(l_iter->first, l_iter->second);
		for (std::unordered_map<std::string, glm::vec3>::const_iterator l_iter = m_materialUniformsVec3[a_materialIndex].cbegin(); l_iter != m_materialUniformsVec3[a_materialIndex].cend(); ++l_iter)
			m_material[a_materialIndex]->SetUniform(l_iter->first, l_iter->second);
		for (std::unordered_map<std::string, glm::vec4>::const_iterator l_iter = m_materialUniformsVec4[a_materialIndex].cbegin(); l_iter != m_materialUniformsVec4[a_materialIndex].cend(); ++l_iter)
			m_material[a_materialIndex]->SetUniform(l_iter->first, l_iter->second);
		for (std::unordered_map<std::string, glm::mat3>::const_iterator l_iter = m_materialUniformsMat3[a_materialIndex].cbegin(); l_iter != m_materialUniformsMat3[a_materialIndex].cend(); ++l_iter)
			m_material[a_materialIndex]->SetUniform(l_iter->first, l_iter->second);
		for (std::unordered_map<std::string, glm::mat4>::const_iterator l_iter = m_materialUniformsMat4[a_materialIndex].cbegin(); l_iter != m_materialUniformsMat4[a_materialIndex].cend(); ++l_iter)
			m_material[a_materialIndex]->SetUniform(l_iter->first, l_iter->second);
	}


}