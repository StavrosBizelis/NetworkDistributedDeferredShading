#pragma once

#include <vulkan/vulkan.hpp>


#include <iostream>
#include <stdexcept>


#include <Vulkan/Core/VulkanPhysicalDeviceSelector.h>
#include "Vulkan/Core/VulkanLogicalDeviceManager.h"

#include <glm/glm.hpp>

  
/**
*   VulkanDriver represents a class that helps initialize Vulcan based graphics engine. \n
*   It initialized the vulcan API and contained objects so one can easily check/choose/create the appropriate options for his engine
*
*/
class VulkanDriver
{
  private:
  glm::vec2 m_windowSize;
  std::vector<const char*> m_requiredInstanceExtensions;  ///< The extensions needed to create a vulkan instance with the current windowing system
  
  VkInstance m_instance;
  VkDebugReportCallbackEXT m_callback;
  VkSurfaceKHR m_surface;
  
  
  
  VulkanPhysicalDeviceSelector m_physicalDeviceMan;
  VkPhysicalDevice m_chosenPhysicalDevice;
  
  std::shared_ptr<VulkanLogicalDeviceManager> m_logicalDeviceMan;
  
  void CreateVulkanInstance();
  void SetupDebugCallback();
  
  
  
  static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData) 
  {
      std::cerr << "validation layer: " << msg << std::endl;
      return VK_FALSE;
  }
  
  public:
  /**
  *   @param a_window - is a window created elsewhere(by a window creation API) - use void because it is a pointer of any type
  *   @param a_windowSize - is the window size in terms of pixels
  *   @param a_surfaceCreationFunc - is the surface creation function that will be called internally. const VkInstance& is the Vulkan instance. voi* is the pointer to the window which was provided. 
  */
  VulkanDriver(const std::vector<const char*>& a_requiredInstanceExtensions = {} );  ///< default Constructor
  ~VulkanDriver(); ///< Destructor
  
  
  
  void Init(const glm::vec2& a_windowSize); ///< Initializes the subcomponents before any options are available
  
  //void Update();    ///< call this periodically to draw frames 
  
  
  
  
  
  VulkanPhysicalDeviceSelector& GetPhysicalDeviceSelector();  ///< Return the physical device manager for this instance
  
  VkPhysicalDevice GetSelectedPhysicalDevice();   ///< call only after Init()
  
  std::shared_ptr<VulkanLogicalDeviceManager> GetLogicalDeviceManager();  ///<  return logical device manager
  
  
  void Shutdown();  ///< clears the Vulkan variables 
  
  // getters
  VkInstance *GetVkInstance(){return &m_instance;}
  VkSurfaceKHR *GetVkSurface(){return &m_surface;}

};


