#pragma once
#include "Common/SceneControl/SceneNode.h"
#include "Common/Textures/ITexture.h"
namespace SceneControl
{
	/// Abstract class for scene nodes that have a texture. Dont use as is if you want to see something on the screen
	class TexturedSceneNode :
		public SceneNode
	{

	protected:
		std::map<unsigned int, std::shared_ptr<ITexture> > m_textures;

	public:
		TexturedSceneNode(SceneNode* a_parent = nullptr) :SceneNode(a_parent) {}
		virtual ~TexturedSceneNode() {}

		void SetTexture(const unsigned int& a_layer, const std::shared_ptr<ITexture>& a_texture) 
		{
			if (a_texture)
				m_textures[a_layer] = a_texture;
			else
				m_textures.erase(a_layer);
		}
		
		/// @return the texture on that layer, null if that texture doesnt exist
		std::shared_ptr<ITexture> GetTexture(const unsigned int& a_layer) const
		{
			auto l_text = m_textures.find(a_layer);
			if ( l_text != m_textures.end())
				return l_text->second;
			return nullptr;
		}

		void BindTextures() const
		{
			for (std::map<unsigned int, std::shared_ptr<ITexture> >::const_iterator l_iter = m_textures.cbegin(); l_iter != m_textures.cend(); ++l_iter)
				l_iter->second->Bind(l_iter->first);
		}
	};

}