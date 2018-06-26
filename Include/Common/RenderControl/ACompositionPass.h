#pragma once
#include "Common/RenderControl/IRenderPass.h"

#include "Common/MaterialControl/IMaterialManager.h"
#include "Common/Shapes/ARect.h"
#include "Common/SceneControl/SceneManager.h"
#include "Common/Shapes/IShapeFactory.h"

#include <unordered_map>
#include <set>
namespace RenderControl
{
  
  struct CompositionEntity
  {
    glm::vec2 m_resolution;
    glm::vec4 m_viewport;
  };
  
  
	/// A renderpass that implements deferred shading
	class ACompositionPass :
		public IRenderPass
	{
  protected:
    std::vector< CompositionEntity > m_subpartsSettings;
    std::vector< SceneControl::MeshSceneNode* > m_subpartRects;
    SceneControl::SceneManager* m_scnManager;
	public:
		ACompositionPass(const glm::vec2& a_resolution, SceneControl::SceneManager* a_scnManager, IShapeFactory* a_shapeFactory, const unsigned int& a_subparts);
		virtual ~ACompositionPass();
    
    
    const std::vector< CompositionEntity >& GetSubpartsSettings() const;
    void UpdateSubpartsSettings();
    
    
		inline virtual void SetMaterialManager(MaterialControl::IMaterialManager* a_materialManager);
		
//		virtual bool Init() override;
//		virtual void Render() override;
//		virtual void OutputOnScreen() override;
//		virtual unsigned int GetFBO() override { return m_fbo; }

	};

}