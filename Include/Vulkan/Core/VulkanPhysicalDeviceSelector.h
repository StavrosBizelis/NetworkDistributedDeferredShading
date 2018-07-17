#pragma once

#include <vulkan/vulkan.hpp>


#include <iostream>
#include <stdexcept>
#include <map>

#include <Vulkan/Core/VulkanValidationLayerEnabler.h>
#include <Vulkan/Core/VulkanSwapChainDetails.h>

#include <memory>


/**
*   VulkanPhysicalDeviceSelector represents a class that helps initialize physical devices(graphics cards) for the vulkan api.
*
*/
class VulkanPhysicalDeviceSelector
{
  private:
  VkInstance m_instance;
  std::shared_ptr< std::vector<const char*> > m_deviceExtensions;    ///< this vector holds the mandatory extentions that are set by the user... Always in VK_KHR_SWAPCHAIN_EXTENSION_NAME
  
  bool IsSuitable(const VkPhysicalDevice& a_phDev, const VkSurfaceKHR& a_surface, const uint32_t& a_actualWidth = 0, const uint32_t& a_actualHeight = 0); ///< return true if suitable
  bool CheckDeviceExtensionSupport(const VkPhysicalDevice& a_phDev);
  int RateSuitability(const VkPhysicalDevice& a_phDev, const VkSurfaceKHR& a_surface, const uint32_t& a_actualWidth, const uint32_t& a_actualHeight);   ///< return a score for the suitability of this device ( -1 is not suitable)
  
  
  public:
  VulkanPhysicalDeviceSelector();  ///< default Constructor
  ~VulkanPhysicalDeviceSelector(); ///< Destructor
  
  
  std::vector<VkPhysicalDevice> GetAllPhysicalDevices(const VkInstance& a_instance);    ///< returns all physical devices with vulkan support
  std::map<int, VkPhysicalDevice> GetSuitablePhysicalDevices(const VkInstance& a_instance, const VkSurfaceKHR& a_surface, const uint32_t& a_actualWidth, const uint32_t& a_actualHeight);   ///< Get a map of suitable physical devices for the vulcan api. First is the best choice, last one is the worst choice
  
  QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& a_phDev, const VkSurfaceKHR& a_surface); ///< find queue familes for this specific device
     
  std::vector<const char*> GetDeviceExtensions(); ///< return the mandatory extentions that are set by the user in VulkanPhysicalDeviceSelector
  
  SwapChainSupportDetails GetSwapChainSupportDetails(const VkPhysicalDevice& a_phDev, const VkSurfaceKHR& a_surface, const uint32_t& a_actualWidth = 0, const uint32_t& a_actualHeight = 0); ///< returns the swap chain support details for this physical device
  
  
  
};

