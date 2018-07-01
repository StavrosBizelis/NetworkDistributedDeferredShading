#pragma once
#include <set>
#include "Common/Core/MatrixStack.h"

#include "memory"
#include "Common/RenderControl/IRenderable.h"
#include <vector>
#include <unordered_map>
namespace SceneControl
{
		


	/// Class that includes all the required information to place an object in a scene
	class SceneNode : public RenderControl::IRenderable
	{

	protected:
		std::set<SceneNode*> m_children;
		SceneNode* m_parent;		///< does not own this object - is owned by this object

    unsigned int m_id;
    
		glm::vec3 m_posRel;			///< relative position
		glm::mat4 m_orientRel;	///< relative rotation matrix
		glm::vec3 m_scaleRel;	///< relative scale

		bool m_dirty;
		glm::mat4 m_lastAbsoluteTrans;	///< updated only when dirty - useful to have separate Update and Render Functions
		glm::vec3 m_absoluteScale;		///< keep a reference to the last scale in order to be able to quickly check if the scaleX is equal to scaleY and scalaZ and avoid the inversion+transpose of normal matrix

		std::vector< std::shared_ptr<IShaderProgram> > m_material;
		// Persistent uniforms
		///  @brief index material index
		///  @brief key : uniform name
		///  @brief value: the actual uniform
		std::vector< std::unordered_map<std::string, int > > m_materialUniformsInt;
		std::vector< std::unordered_map<std::string, float > > m_materialUniformsFloat;
		std::vector< std::unordered_map<std::string, glm::vec2> > m_materialUniformsVec2;
		std::vector< std::unordered_map<std::string, glm::vec3> > m_materialUniformsVec3;
		std::vector< std::unordered_map<std::string, glm::vec4> > m_materialUniformsVec4;
		std::vector< std::unordered_map<std::string, glm::mat3> > m_materialUniformsMat3;
		std::vector< std::unordered_map<std::string, glm::mat4 > > m_materialUniformsMat4;

    static std::map<unsigned int, SceneNode*> s_idRegistry;
    
	public:
		
		SceneNode(SceneNode* a_parent = nullptr);

		virtual ~SceneNode();
		
    /// optional id to set for quick retrieval of the scene node
    /// an id can be any unsigned int number except 0
    /// use 0 to remove from the id registry
    /// not two objects can have the same id
    inline bool SetID(const unsigned int& a_id)
    { 
      if( a_id == 0 )
      {
        s_idRegistry.erase(a_id);
        return true;
      }
      if( s_idRegistry.find(a_id) == s_idRegistry.end() && a_id != 0)
      {
        m_id = a_id;
        s_idRegistry[m_id] = this;
        return true;
      }
      return false;
    }
    /// an id can be any unsigned int number except 0
    /// not two objects can have the same id
    inline unsigned int GetID(){ return m_id;}
    
    /// @return the SceneNode* with the given id - return nullptr if no scenenode has this id
    static SceneNode* GetByID(const unsigned int& a_id)
    {
      if( a_id == 0)
        return nullptr;        
      std::map<unsigned int, SceneNode*>::iterator l_iter = s_idRegistry.find(a_id);
      if( l_iter == s_idRegistry.end() )
        return nullptr;        
      return l_iter->second;
    }

		inline void AddChild(SceneNode* a_node) 
		{ 
			a_node->m_parent->RemoveChild(a_node);
			a_node->m_parent = this;
			m_children.insert(a_node); 
		}
    
    /// removes child node from the hierarchy - does not delete, caller is the new owner
		inline void RemoveChild(SceneNode* a_node) { m_children.erase(a_node); a_node->m_parent = nullptr; }

    /// removes and deletes node making it unusable if it is direct child of *this
    inline bool DetachChild(SceneNode* a_node) 
    { 
      if( m_children.erase(a_node) ) 
      { 
        delete a_node;
        return true;
      }
      else return false;
    }
    
    /// removes and deletes node making it unusable if it is a descendant of the caller node
    inline bool DetachDescendant(SceneNode* a_node) 
    { 
      std::set<SceneNode*>::iterator l_iter = m_children.find(a_node); 
      if( l_iter!= m_children.end() )
      {
        m_children.erase(l_iter);
        delete a_node; 
        return true;
      }

      for( std::set<SceneNode*>::iterator l_iter2 = m_children.begin(); l_iter2 != m_children.end(); ++l_iter2)
        if( (*l_iter2)->DetachDescendant(a_node) )
          return true;
      return false; // if we get here this node does not exist under the ancestor node
    }
    
    
		inline SceneNode* GetChild(const unsigned int& a_index) const
		{
			if( m_children.size() > a_index)
				return *std::next(m_children.begin(), a_index);
			return nullptr;
		}

		bool DescendantExists(SceneNode* a_node) const;

		inline unsigned int GetChildNum() const { return m_children.size(); }

		SceneNode* GetRoot() 
		{
			if (m_parent)
				return m_parent->GetRoot();
			return this;
		};

		
		// virtual function to override
		/// Function that updates and draws the scene node
		virtual void Update(const double& a_deltaTime, bool a_dirty = false, const glm::mat4& a_parentAbsoluteTrans = glm::mat4() );

		virtual void Render( glutil::MatrixStack& a_matrix = glutil::MatrixStack() ) const {} ///< dont forget to bind persistent uniforms in children
		virtual std::shared_ptr<IShaderProgram> GetMaterial(const unsigned int& a_index = 0) const
		{
			if (!( a_index < m_material.size() ) )
				return nullptr;
			return m_material[a_index];
		}	

		/// virtual because childer nodes may need to update other stuff on material set
		virtual void SetMaterial(const std::shared_ptr<IShaderProgram>& a_material, const unsigned int& a_index = 0) 
		{ 
			if (!(a_index < m_material.size()))
			{
				m_material.resize(a_index + 1);
				m_materialUniformsInt.resize(a_index + 1);
				m_materialUniformsFloat.resize(a_index + 1);
				m_materialUniformsVec2.resize(a_index + 1);
				m_materialUniformsVec3.resize(a_index + 1);
				m_materialUniformsVec4.resize(a_index + 1);
				m_materialUniformsMat3.resize(a_index + 1);
				m_materialUniformsMat4.resize(a_index + 1);
			}
			m_material[a_index] = a_material;
		}



		// transformation matrix operations
		// Getters
		virtual glm::vec3 GetRelativePos() const;
		virtual glm::mat4 GetRelativeRot() const;
		virtual glm::vec3 GetRelativeEulerAngles() const;
		virtual glm::vec3 GetRelativeScale() const;

		virtual glm::vec3 GetPos() const;
		virtual glm::mat4 GetRot() const;
		virtual glm::vec3 GetEulerAngles() const;
		virtual glm::vec3 GetScale() const;
    


		// Setters
		virtual void SetRelativePos(const glm::vec3& a_pos);
		virtual void SetRelativeRot(const glm::mat4& a_rotation);
		virtual void SetRelativeEulerAngles(const glm::vec3& a_eulerAngles);
		virtual void SetRelativeScale(const glm::vec3& a_scale);

		virtual void SetPos(const glm::vec3& a_pos);
		virtual void SetRot(const glm::mat4& a_rotation);
		virtual void SetEulerAngles(const glm::vec3& a_eulerAngles);
		virtual void SetScale(const glm::vec3& a_scale);

		virtual void SetEnabled(bool a_val);

		// Persistent Uniform Setters
		bool SetPersistentUniform(const unsigned int& a_materialIndex, const std::string& a_name, const int& a_var);
		bool SetPersistentUniform(const unsigned int& a_materialIndex, const std::string& a_name, const float& a_var);
		bool SetPersistentUniform(const unsigned int& a_materialIndex, const std::string& a_name, const glm::vec2& a_var);
		bool SetPersistentUniform(const unsigned int& a_materialIndex, const std::string& a_name, const glm::vec3& a_var);
		bool SetPersistentUniform(const unsigned int& a_materialIndex, const std::string& a_name, const glm::vec4& a_var);
		bool SetPersistentUniform(const unsigned int& a_materialIndex, const std::string& a_name, const glm::mat3& a_var);
		bool SetPersistentUniform(const unsigned int& a_materialIndex, const std::string& a_name, const glm::mat4& a_var);
		// Bind Persistent Uniforms
		void BindPersistentUniforms(const unsigned int& a_materialIndex) const;

	};


}
