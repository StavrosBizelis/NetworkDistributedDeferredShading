#pragma once
#include "Common/RenderControl/ADeferredShadingPass.h"

#include "Common/MaterialControl/IMaterialManager.h"
#include <unordered_map>
namespace RenderControl
{

	/// A renderpass that implements deferred shading
	class GLDeferredShadingPass :
		public ADeferredShadingPass
	{
  protected:
    unsigned int m_fbo;

    void GeometryPass();
    void LightPass();
    
	public:
		GLDeferredShadingPass(const glm::vec2& a_resolution);
		virtual ~GLDeferredShadingPass();

		virtual bool Init() override;
		virtual void Render() override;
		virtual void OutputOnScreen() override;
		virtual unsigned int GetFBO() override { return m_fbo; }

	};

}