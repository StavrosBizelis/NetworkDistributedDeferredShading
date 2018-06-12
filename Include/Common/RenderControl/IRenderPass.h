#pragma once
#include <glm/glm.hpp>
#include <vector>

#include "Common/RenderControl/IRenderable.h"
#include "Common/RenderControl/Camera.h"
#include <memory>

#include "Common/MaterialControl/IMaterialManager.h"
namespace RenderControl
{

	/// Interface for RenderPasses that can be used in RenderPassPipeline
	class IRenderPass
	{

	protected:
		std::vector<unsigned int> m_outputTextures;
		std::vector<unsigned int> m_outputSamplers;		///< default samplers in respect to the output textures
		
		glm::vec2 m_resolution;
		unsigned int m_attachmentIndex;
		
		std::shared_ptr<CCamera> m_camera;
		MaterialControl::IMaterialManager* m_materialManager;	///< does not own this 

	public:
		IRenderPass(const glm::vec2& a_resolution): m_resolution(a_resolution), m_camera( new CCamera ), m_attachmentIndex(0), m_materialManager(nullptr) {};
		virtual ~IRenderPass() { };

		
		inline glm::vec2 GetResolution() const { return m_resolution; }

		inline unsigned int GetOutputTextureCount() const { return m_outputTextures.size(); }
		inline int GetOutputTexture(const unsigned int& a_index) const  
		{ 
			if (a_index < m_outputTextures.size()) 
				return m_outputTextures[a_index]; 
			return -1; 
		}

		inline unsigned int GetOutputSamplerCount() const { return m_outputSamplers.size(); }
		inline int GetOutputSampler(const unsigned int& a_index) const
		{
			if (a_index < m_outputSamplers.size())
				return m_outputSamplers[a_index];
			return -1;
		}


		inline std::shared_ptr<CCamera> GetCamera() { return m_camera; }
		inline virtual void SetCamera(const std::shared_ptr<CCamera>& a_cam) { m_camera = a_cam; }

		inline virtual void SetMaterialManager(MaterialControl::IMaterialManager* a_materialManager) { m_materialManager = a_materialManager; }
		inline MaterialControl::IMaterialManager* const GetMaterialManager() const { return m_materialManager; }

		///	Every render pass implementation must initialize itself ( framebuffer setup - output textures - etc)
		virtual bool Init() = 0;
		/// every render pass is responsible on how it will implement its rendering functions
		virtual void Render() = 0;
		/// blit the selected output attachment to the default framebuffer
		virtual void OutputOnScreen() = 0;
		virtual unsigned int GetFBO() = 0;
		/// in case of more than one colour attachments - choose which is to be rendered on screen
		virtual void SetSceenOutputAttachment(const unsigned int& a_attachmentIndex) { m_attachmentIndex = a_attachmentIndex;  }
		

	};

}