#pragma once
#include "Common/RenderControl/ACompositionPass.h"
#include "Common/MaterialControl/IShaderProgram.h"
#include <Vulkan/Vulkan.hpp>
#include <Vulkan/Core/VulkanMemory.h>
#include <Vulkan/Core/VulkanLogicalDeviceManager.h>
#include <Vulkan/Core/VulkanSwapChainDetails.h>
#include "Vulkan/RenderControl/UniformBufferObjects.h"
#include "Vulkan/RenderControl/Pipelines/VKPipeline.h"
#include "Vulkan/RenderControl/VulkanRenderable.h"
#include "Vulkan/RenderControl/Pipelines/VulkanPrimaryCommandBuffer.h"
namespace RenderControl
{
  
  
  
	/// A renderpass that implements deferred shading
	class VKCompositionPass :
		public ACompositionPass
	{
  protected:
    std::shared_ptr<IShaderProgram> m_material;
    
    // VULKAN VARS
    // std::vector< std::shared_ptr<VulkanImageMemoryChunk> > m_attachmentImages;
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
    
    
    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;
    QueueFamilyIndices m_indices;

    VkFramebuffer m_frameBuffer;
    VkCommandPool m_commandPool;
    // semaphores for the rendering process
    VkSemaphore m_imageAvailableSemaphore;
    VkSemaphore m_renderFinishedSemaphore;
    
    // VulkanFramebuffer m_frameBuffer;
    std::shared_ptr<VulkanPrimaryCommandBuffer> m_primaryCmdBuffer;
    
    void CreateCommandPool();
    void CreateSemaphores();
    void CreateRenderPass();
    void CreateFramebuffer();
    void CreatePipelines();
    void CreateDescriptorPool();
    void CreateCommandBuffers();
    
    void CreateDescriptorSet(const std::shared_ptr<VKPipeline>& a_pipeline, VulkanRenderable* a_renderable); // descriptor sets will be created and allocated on the fly as objects are introduced to the VKDeferredShadingPass

    
    std::vector< std::shared_ptr<VKPipeline> > m_pipelines;
    VkDescriptorPool m_descriptorPool;
    
    void Clear();
    
	public:
		VKCompositionPass(const std::shared_ptr<VulkanLogicalDeviceManager>& a_device, const VkPhysicalDevice& a_physicalDevice, const std::shared_ptr<VulkanMemory>& a_memory,
                      const VkQueue& a_graphicsQueue, const VkQueue& a_presentQueue, const QueueFamilyIndices& a_indices,
                      const glm::vec2& a_resolution, SceneControl::SceneManager* a_scnManager, IShapeFactory* a_shapeFactory, ITextureFactory* a_textFactory, const unsigned int& a_subparts );
		virtual ~VKCompositionPass();
    
    
    inline virtual void SetMaterialManager(MaterialControl::IMaterialManager* a_materialManager);
		
    virtual unsigned int GetFBO(){return (unsigned int)m_frameBuffer; }
    
		virtual bool Init() override;
		virtual void Render() override;
		virtual void OutputOnScreen() override;
    
    void VulkanUpdate( char* a_mappedBuffer );
	};

}