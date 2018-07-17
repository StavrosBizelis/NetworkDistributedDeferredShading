#pragma once
#include "Common/RenderControl/ADeferredShadingPass.h"

#include "Common/MaterialControl/IMaterialManager.h"
#include <unordered_map>

#include <Vulkan/Vulkan.hpp>
#include <Vulkan/Core/VulkanMemory.h>
#include <Vulkan/Core/VulkanLogicalDeviceManager.h>
#include <Vulkan/Core/VulkanSwapChainDetails.h>
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
    
    std::shared_ptr<VulkanMemory> m_memory;
    
    
    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;
    QueueFamilyIndices m_indices;

    VkFramebuffer m_frameBuffer;
    VkCommandPool m_commandPool;
    // semaphores for the rendering process
    VkSemaphore m_imageAvailableSemaphore;
    VkSemaphore m_renderFinishedSemaphore;
    
    // VulkanFramebuffer m_frameBuffer;
    // std::shared_ptr<VulkanPrimaryCommandBuffer> m_primaryCmdBuffer;
    
    void CreateCommandPool();
    void CreateSemaphores();
    void CreateRenderPass();
    void CreateFramebuffer();
    void CreateCommandBuffers();
    
    
	public:
		VKDeferredShadingPass(const std::shared_ptr<VulkanLogicalDeviceManager>& a_device, const VkPhysicalDevice& a_physicalDevice, const std::shared_ptr<VulkanMemory>& a_memory,
                          const VkQueue& a_graphicsQueue, const VkQueue& a_presentQueue, const QueueFamilyIndices& a_indices,
                          const glm::vec2& a_resolution, const glm::vec2 &a_partialResolution, const glm::vec4& a_viewportSettings);
		virtual ~VKDeferredShadingPass();

		virtual bool Init() override;
		virtual void Render() override;
		virtual void OutputOnScreen() override;
    
    virtual unsigned int GetFBO(){return (unsigned int)m_frameBuffer; }
    
    virtual void UpdateViewportSettings(const glm::vec2& a_resolution, const glm::vec4& a_viewportSettings);

    virtual void SetMaterialManager(MaterialControl::IMaterialManager* a_materialManager);
    
    virtual bool PackTexture( Network::NetworkMsgPtr& a_outMsg);
	};

}