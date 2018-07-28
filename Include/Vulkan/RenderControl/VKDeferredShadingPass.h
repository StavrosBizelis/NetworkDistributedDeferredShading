#pragma once
#include "Common/RenderControl/ADeferredShadingPass.h"

#include "Common/MaterialControl/IMaterialManager.h"
#include <unordered_map>

#include <Vulkan/Vulkan.hpp>
#include <Common/Textures/ITexture.h>
#include <Vulkan/Core/VulkanMemory.h>
#include <Vulkan/Core/VulkanLogicalDeviceManager.h>
#include <Vulkan/Core/VulkanSwapChainDetails.h>
#include "Vulkan/RenderControl/Pipelines/VKPipeline.h"
#include "Vulkan/RenderControl/VulkanRenderable.h"
#include "Vulkan/RenderControl/Pipelines/VulkanPrimaryCommandBuffer.h"

namespace RenderControl
{

	/// A renderpass that implements deferred shading
	class VKDeferredShadingPass :
		public ADeferredShadingPass
	{
  protected:
    void GeometryPass();
    void LightPass();
    void Clear();
    
    std::vector< std::shared_ptr<VulkanImageMemoryChunk> > m_attachmentImages;
    
    
    
    // VULKAN VARS
    std::shared_ptr<VulkanLogicalDeviceManager> m_logicalDevice;
    VkPhysicalDevice m_physicalDevice;
    VkRenderPass m_renderPass;
    VkFormat m_swapChainImageFormat;
    std::vector<VkImage> m_swapChainImages;
    std::vector<VkImageView> m_swapChainImageViews;
    
    
    std::shared_ptr<VulkanMemory> m_memory;

    FragLightGlobalVars m_globalsUbo1;
    VertexViewProjMatrices m_globalsUbo2;
    std::vector< std::shared_ptr<VulkanMemoryChunk> > m_uboMemBuffers;
    
    
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
    void CreateDescriptorPool();
    void CreateCommandBuffers();
    
    void CreateDescriptorSet(const std::shared_ptr<VKPipeline>& a_pipeline, IRenderable* a_renderable); // descriptor sets will be created and allocated on the fly as objects are introduced to the VKDeferredShadingPass

    
    std::vector< std::shared_ptr<VKPipeline> > m_pipelines;
    VkDescriptorPool m_descriptorPool;
    
    PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR;
    PFN_vkQueuePresentKHR vkQueuePresentKHR;
    
	public:
		VKDeferredShadingPass(const std::shared_ptr<VulkanLogicalDeviceManager>& a_device, const VkPhysicalDevice& a_physicalDevice, const std::shared_ptr<VulkanMemory>& a_memory,
                          const VkQueue& a_graphicsQueue, const VkQueue& a_presentQueue, const QueueFamilyIndices& a_indices,
                          const glm::vec2& a_resolution, const glm::vec2 &a_partialResolution, const glm::vec4& a_viewportSettings);
		virtual ~VKDeferredShadingPass();

		virtual bool Init() override;
		virtual void Render() override;
		virtual void OutputOnScreen() override;
    
    virtual unsigned int GetFBO(){return (unsigned int)(&m_frameBuffers); }
    
    virtual void UpdateViewportSettings(const glm::vec2& a_resolution, const glm::vec4& a_viewportSettings);

    virtual void SetMaterialManager(MaterialControl::IMaterialManager* a_materialManager);
    
    
    virtual bool AddRenderable(IRenderable* a_renderable, const GeometryPassMaterialFlags& a_geometryMaterialFlags = GeometryPassMaterialFlags::SIMPLE_GEOMETRY );
    // virtual void RemoveRenderable(IRenderable* a_renderable);
    virtual void AddLight(IRenderable* a_light, const LightTypeFlags& a_lightType);
		// virtual void RemoveLight(IRenderable* a_light);
    
    virtual bool PackTexture( Network::NetworkMsgPtr& a_outMsg);
    
    void VulkanUpdate( char* a_mappedBuffer );
	};

}