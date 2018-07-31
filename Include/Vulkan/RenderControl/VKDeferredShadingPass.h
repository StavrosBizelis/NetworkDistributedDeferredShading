#pragma once
#include "Common/RenderControl/ADeferredShadingPass.h"
#include "Common/MaterialControl/IShaderProgram.h"
#include <Vulkan/Vulkan.hpp>
#include <Vulkan/Core/VulkanMemory.h>
#include <Vulkan/Core/VulkanLogicalDeviceManager.h>
#include <Vulkan/Core/VulkanSwapChainDetails.h>
#include "Vulkan/RenderControl/UniformBufferObjects.h"
#include "Vulkan/RenderControl/Pipelines/VKPipeline.h"

#include "Common/MaterialControl/IMaterialManager.h"
#include "Common/Shapes/ARect.h"
#include "Common/SceneControl/SceneManager.h"
#include "Common/Shapes/IShapeFactory.h"
#include "Common/Textures/ITextureFactory.h"

#include "Vulkan/RenderControl/Pipelines/VulkanPrimaryCommandBuffer.h"
namespace RenderControl
{
  
  
  
	/// A renderpass that implements deferred shading
	class VKDeferredShadingPass :
		public ADeferredShadingPass
	{
  protected:
    std::vector< SceneControl::MeshSceneNode* > m_subpartRects;
    std::vector< SceneControl::LightSceneNode* > m_lights;
    SceneControl::SceneManager* m_scnManager;
  
  
    std::shared_ptr<IShaderProgram> m_material;
    
    // VULKAN VARS
    std::vector< std::shared_ptr<VulkanImageMemoryChunk> > m_attachmentImages;
    // swapchain images and image views
    std::vector<VkImage> m_swapChainImages;
    std::vector<VkImageView> m_swapChainImageViews;
    
    std::shared_ptr<VulkanLogicalDeviceManager> m_logicalDevice;
    VkPhysicalDevice m_physicalDevice;
    VkRenderPass m_renderPass;
    VkFormat m_swapChainImageFormat;
    
    std::shared_ptr<VulkanMemory> m_memory;

    VertexViewProjMatrices m_globalsUbo;
    std::shared_ptr<VulkanMemoryChunk> m_uboMemBuffer;
    
    FragLightGlobalVars m_globalsUbo2;
    std::shared_ptr<VulkanMemoryChunk> m_uboMemBuffer2;
    
    
    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;
    QueueFamilyIndices m_indices;

    std::vector<VkFramebuffer> m_frameBuffers;
    VkCommandPool m_commandPool;
    // semaphores for the rendering process
    std::vector<VkSemaphore> m_imageAvailableSemaphore;
    std::vector<VkSemaphore> m_renderFinishedSemaphore;
    const unsigned int m_maxFramesInFlight = 2;
    unsigned int m_currentFrame;
    
    
    // VulkanFramebuffer m_frameBuffer;
    std::shared_ptr<VulkanPrimaryCommandBuffer> m_primaryCmdBuffer;
    
    void CreateCommandPool();
    void CreateSemaphores();
    void CreateRenderPass();
    void CreateFramebuffer();
    void CreatePipelines();
    void CreateSingleGeometryPassPipeline(const unsigned int& a_index, const unsigned int& a_inputSamplers, const std::string& a_vertPath, const std::string& a_fragPath);
    void CreateSingleLightPassPipeline(const unsigned int& a_index, const std::string& a_vertPath, const std::string& a_fragPath);

    void CreateDescriptorPool();
    void CreateCommandBuffers();
    
    void CreateDescriptorSet(const std::shared_ptr<VKPipeline>& a_pipeline, IRenderable* a_renderable); // descriptor sets will be created and allocated on the fly as objects are introduced to the VKDeferredShadingPass
    void CreateDescriptorSetDirLight(const std::shared_ptr<VKPipeline>& a_pipeline, IRenderable* a_renderable); // descriptor sets will be created and allocated on the fly as objects are introduced to the VKDeferredShadingPass

    
    std::vector< std::shared_ptr<VKPipeline> > m_pipelines;
    VkDescriptorPool m_descriptorPool;
    
    void Clear();
    
    PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR;
    PFN_vkQueuePresentKHR vkQueuePresentKHR;
    
	public:
		VKDeferredShadingPass(const std::shared_ptr<VulkanLogicalDeviceManager>& a_device, const VkPhysicalDevice& a_physicalDevice, const std::shared_ptr<VulkanMemory>& a_memory,
                      const VkQueue& a_graphicsQueue, const VkQueue& a_presentQueue, const QueueFamilyIndices& a_indices,
                      const glm::vec2& a_resolution, SceneControl::SceneManager* a_scnManager, IShapeFactory* a_shapeFactory, ITextureFactory* a_textFactory, const unsigned int& a_subparts );
		virtual ~VKDeferredShadingPass();
    
    
    inline virtual void SetMaterialManager(MaterialControl::IMaterialManager* a_materialManager);
		
    virtual unsigned int GetFBO(){return (unsigned int)(&m_frameBuffers); }
    
		virtual bool Init() override;
		virtual void Render() override;
		virtual void OutputOnScreen() override;
    
    void UpdateViewportSettings(const glm::vec2 &,const glm::vec4 &){}
    bool PackTexture(Network::NetworkMsgPtr &){return true;}
    
    void VulkanUpdate( char* a_mappedBuffer );
	};

}