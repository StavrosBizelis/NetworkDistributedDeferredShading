#pragma once
/// Render pass pipeline - holds an internal vector of render passes -  the lower the index in the vector -> the earlier the render pass is going to be done
#include <vector>

#include "Common/RenderControl/IRenderPass.h"
#include "Common/MaterialControl/IMaterialManager.h"

namespace RenderControl
{
	/// a class that manages multiple render passes
	class RenderPassPipeline
	{
		/// owns the material manager
		MaterialControl::IMaterialManager* m_materialManager;
		
		std::vector< IRenderPass* > m_passes;	///< owns these render passes
	public:
		RenderPassPipeline();
		virtual ~RenderPassPipeline();

		/// pushes this pass to the internal vector ( this pass will be render after the previously added render pass)
		/// RenderPassPipeline now owns a_pass
		void PushBack(IRenderPass* a_pass);
		/// remove last render pass
		void PopBack();
		/// @return if nullptr render pass index out of bounds
		IRenderPass* GetRenderPass(const unsigned int& a_index);

		unsigned int GetRenderPassCount();

		void RenderAll();
		void Clear(); ///< clears the pipeline and invalidates the render passes

		void SetMaterialManager(MaterialControl::IMaterialManager* a_materialManager);		///< transfers ownership
		MaterialControl::IMaterialManager* GetMaterialManager() const;		///< valid as long as this RenderControl object is - DO NOT DELETE
	};
}