#pragma once
#include "Common/RenderControl/IRenderPass.h"

#include "Common/MaterialControl/IMaterialManager.h"
#include <unordered_map>
#include <set>
namespace RenderControl
{
  
  struct CompositionEntity
  {
    glm::vec2 m_resolution;
    glm::vec4 m_viewport;
  }
  
  
	/// A renderpass that implements deferred shading
	class ACompositionPass :
		public IRenderPass
	{
  protected:
		
	public:
		ACompositionPass(const glm::vec2& a_resolution);
		virtual ~ACompositionPass();
    
    
    
    
		inline virtual void SetMaterialManager(MaterialControl::IMaterialManager* a_materialManager);
		
//		virtual bool Init() override;
//		virtual void Render() override;
//		virtual void OutputOnScreen() override;
//		virtual unsigned int GetFBO() override { return m_fbo; }

	};

}