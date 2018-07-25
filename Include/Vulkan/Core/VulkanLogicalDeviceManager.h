#pragma once

#include <vulkan/vulkan.hpp>

#include <iostream>
#include <stdexcept>
// // // // #include <VulkanDriver/VulkanRenderPass.h>
// // // // #include <VulkanDriver/VulkanValidationLayerEnabler.h>
#include <Vulkan/Core/VulkanSwapChainDetails.h>
#include <Vulkan/Core/VulkanMemory.h>

#include <memory>




/**
*   VulkanLogicalDeviceManager represents a class that helps initialize Vulcan logical device. \n
*   @TODO Make multiple render passes(apart from subpasses) possible
*/
class VulkanLogicalDeviceManager
{
  private:
  
  VkInstance m_instance;
  VkPhysicalDevice m_chosenPhysicalDevice;
  VkDevice m_logicalDevice;
  VkSurfaceKHR m_surface;
  
  VkQueue m_graphicsQueue;
  VkQueue m_presentQueue;
  
  
  VkSwapchainKHR m_swapChain;
  std::shared_ptr< std::vector<VkImage> > m_swapChainImages;
  VkFormat m_swapChainImageFormat;
  VkExtent2D m_swapChainExtent;
  std::shared_ptr< std::vector<VkImageView> > m_swapChainImageViews;
  
  
  std::shared_ptr< VulkanMemory > m_memory;
  
  
  // // // // // // // // // // // // // std::shared_ptr<VulkanRenderPass> m_renderPass;
  
  // state vars
  SwapChainSupportDetails m_details;
  QueueFamilyIndices m_queueFamilyIndexes;
  
  // helper that updates uniform buffers
  void UpdateUniformBuffers();
  
  // imported functions
  PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR;
  PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR;
  PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR;
  PFN_vkQueuePresentKHR vkQueuePresentKHR;
  
  
  public:
  VulkanLogicalDeviceManager();  ///< default Constructor
  ~VulkanLogicalDeviceManager(); ///< Destructor
  

  
  
  void Init(const VkPhysicalDevice& a_physicalDevice, const VkSurfaceKHR& a_surface, const QueueFamilyIndices& a_queueFamilyIndexes, 
            const VkPhysicalDeviceFeatures& a_physicalDeviceFeatures, const std::vector<const char*>& a_deviceExtensions); ///< Initializes the subcomponents before any options are available - call last
  
  void ReInit();    ///< reinitialization - does not delete the internal objects - just reinitializes them
  
  
  void Update();    ///< call this to draw frames
  
  void CreateSwapChain(const VkSurfaceFormatKHR& a_surfaceFormat, const VkPresentModeKHR& a_presentMode, const VkExtent2D& a_extent, const QueueFamilyIndices& a_queueFamilyIndexes,  
                        const VkSurfaceTransformFlagBitsKHR& a_pretransform, const uint32_t& a_minImageCount, const uint32_t& a_maxImageCount);

  void CreateSwapChain(const SwapChainSupportDetails& a_details);
                        
  
  void CreateImageViews();
  
  // // // // // void CreateRenderPass();
  /// @TODO Add RenderPasses
  // // // // // void SetRenderPass(const std::shared_ptr<VulkanRenderPass>& a_renderPass);
  
  
  
  
  // getters
  // // // // // std::shared_ptr<VulkanRenderPass> GetRenderPass();
  VkDevice GetDevice();
  VkPhysicalDevice GetPhysicalDevice();
  VkFormat GetSwapChainImageFormat();
  VkExtent2D GetSwapChainExtend();
  VkQueue GetGraphicsQueue();
  VkQueue GetPresentQueue();
  VkSwapchainKHR GetSwapChain();
  std::vector<VkImage> GetSwapChainImages();
  std::vector<VkImageView> GetSwapChainImageViews();
  QueueFamilyIndices GetQueueFamilyIndices();
  std::shared_ptr< VulkanMemory > GetMemoryManager();
  
  void CleanUpSwapChain();
  
  void Shutdown();  ///< clears the Vulkan variables 
};
