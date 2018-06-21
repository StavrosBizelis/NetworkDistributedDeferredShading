#include "Common/RenderControl/RenderPassPipeline.h"

namespace RenderControl
{

	RenderPassPipeline::RenderPassPipeline() : m_materialManager(nullptr) {}
	
	RenderPassPipeline::~RenderPassPipeline() 
	{
		Clear(); 
		if (m_materialManager) 
			delete m_materialManager;
	}

	
	void RenderControl::RenderPassPipeline::PushBack(IRenderPass * a_pass) 
	{ 
		a_pass->SetMaterialManager(m_materialManager);  
		m_passes.push_back(a_pass); 
	}

	void RenderControl::RenderPassPipeline::PopBack() { m_passes.pop_back(); }

	IRenderPass * RenderControl::RenderPassPipeline::GetRenderPass(const unsigned int & a_index)
	{
		if (m_passes.size() < a_index)
			return nullptr;
		return m_passes[a_index];
	}

	unsigned int RenderControl::RenderPassPipeline::GetRenderPassCount(){ return m_passes.size(); }

	void RenderControl::RenderPassPipeline::RenderAll()
	{

		typedef std::vector< IRenderPass* >::iterator iterator;
		for (iterator l_iter = m_passes.begin(); l_iter != m_passes.end(); ++l_iter)
			(*l_iter)->Render();

		// print the final output on sceen
		if (!m_passes.empty())
		{
			( *(m_passes.end() - 1) )->OutputOnScreen();
		}


	}

	void RenderPassPipeline::Clear()
	{
		typedef std::vector< IRenderPass* >::iterator iterator;
		for (iterator l_iter = m_passes.begin(); l_iter != m_passes.end(); ++l_iter)
			delete (*l_iter);
		m_passes.clear();
	}

  void RenderPassPipeline::SetMaterialManager(MaterialControl::IMaterialManager* a_materialManager)
  {
    m_materialManager = a_materialManager;
  }
  
	MaterialControl::IMaterialManager* RenderPassPipeline::GetMaterialManager() const
	{
		return m_materialManager;
	}

}
