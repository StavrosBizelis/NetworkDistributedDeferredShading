#pragma once

#include "Common/MaterialControl/IShaderProgram.h"

#include "Common/Core/MatrixStack.h"
#include <memory>
namespace RenderControl
{
	/// IRenderable is an interface for objects that need to be rendered in a render pass
	class IRenderable
	{
	private:
		bool m_enabled;
	public:
		IRenderable() : m_enabled(true) {}
    virtual void* GetExtra() {return nullptr;}  ///< used for extra functionlity of classes that inherit this
    
		/// Implement to have this object render itself
		virtual void Render(glutil::MatrixStack& a_matrix = glutil::MatrixStack() ) const = 0;

		virtual std::shared_ptr<IShaderProgram> GetMaterial(const unsigned int& a_index = 0) const = 0;
		virtual void SetMaterial(const std::shared_ptr<IShaderProgram>& a_material, const unsigned int& a_index = 0) = 0;
		

		virtual void SetEnabled(bool a_val) { m_enabled = a_val; }
		virtual bool GetEnabled() const { return m_enabled; }
	};

}