#include <Vulkan/Core/VulkanPhysicalDeviceSelector.h>

#include <set>


bool VulkanPhysicalDeviceSelector::IsSuitable(const VkPhysicalDevice& a_phDev, const VkSurfaceKHR& a_surface, const uint32_t& a_actualWidth, const uint32_t& a_actualHeight )
{
  QueueFamilyIndices indices = FindQueueFamilies(a_phDev, a_surface);

  bool extensionsSupported = CheckDeviceExtensionSupport(a_phDev);
  bool swapChainAdequate = false;
  if (extensionsSupported) 
  {
    SwapChainSupportDetails swapChainSupport = GetSwapChainSupportDetails(a_phDev, a_surface, a_actualWidth, a_actualHeight);
      swapChainAdequate = !swapChainSupport.formats->empty() && !swapChainSupport.presentModes->empty();
  }
  
  return indices.IsComplete() && extensionsSupported && swapChainAdequate;
}


QueueFamilyIndices VulkanPhysicalDeviceSelector::FindQueueFamilies(const VkPhysicalDevice& a_phDev, const VkSurfaceKHR& a_surface)
{
  QueueFamilyIndices indices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(a_phDev, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(a_phDev, &queueFamilyCount, queueFamilies.data());

  int i = 0;
  for (const auto& queueFamily : queueFamilies) 
  {
    if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
    {
      indices.m_graphicsFamily = i;
    }
    VkBool32 presentSupport = false;
    auto vkGetPhysicalDeviceSurfaceSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceSupportKHR>(vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceSurfaceSupportKHR"));    
    vkGetPhysicalDeviceSurfaceSupportKHR(a_phDev, i, a_surface, &presentSupport);

    if (queueFamily.queueCount > 0 && presentSupport)
      indices.m_presentFamily = i;

    if (indices.IsComplete()) 
      break;
    
    i++;
  }

  return indices;
}

bool VulkanPhysicalDeviceSelector::CheckDeviceExtensionSupport(const VkPhysicalDevice& a_phDev)
{
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(a_phDev, nullptr, &extensionCount, nullptr);

  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(a_phDev, nullptr, &extensionCount, availableExtensions.data());

  std::set<std::string> requiredExtensions(m_deviceExtensions->begin(), m_deviceExtensions->end());

  for (const auto& extension : availableExtensions)
    requiredExtensions.erase(extension.extensionName);

  return requiredExtensions.empty();
}



int VulkanPhysicalDeviceSelector::RateSuitability(const VkPhysicalDevice& a_phDev, const VkSurfaceKHR& a_surface, const uint32_t& a_actualWidth, const uint32_t& a_actualHeight )
{
  if( !IsSuitable(a_phDev, a_surface, a_actualWidth, a_actualHeight) )
    return -1;
  
  
  VkPhysicalDeviceProperties l_properties;
  VkPhysicalDeviceFeatures l_features;
  
  vkGetPhysicalDeviceProperties(a_phDev,&l_properties);
  vkGetPhysicalDeviceFeatures(a_phDev,&l_features);
  
  int l_rating = 0;
  // Discrete GPUs have a significant performance advantage
  if( l_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    l_rating += 1000;

  // Maximum possible size of textures affects graphics quality
  l_rating += l_properties.limits.maxImageDimension2D;    
  
  
  return l_rating;
}

VulkanPhysicalDeviceSelector::VulkanPhysicalDeviceSelector()
  : m_deviceExtensions( std::make_shared< std::vector<const char*> >() )
{
  m_deviceExtensions->push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}


VulkanPhysicalDeviceSelector::~VulkanPhysicalDeviceSelector(){}



std::vector<VkPhysicalDevice> VulkanPhysicalDeviceSelector::GetAllPhysicalDevices(const VkInstance& a_instance)
{
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(a_instance, &deviceCount, nullptr);
  std::vector<VkPhysicalDevice> l_toReturn(deviceCount);
  vkEnumeratePhysicalDevices(a_instance, &deviceCount, l_toReturn.data());

  return l_toReturn;
  
}
std::map<int, VkPhysicalDevice> VulkanPhysicalDeviceSelector::GetSuitablePhysicalDevices(const VkInstance& a_instance, const VkSurfaceKHR& a_surface, const uint32_t& a_actualWidth, const uint32_t& a_actualHeight)
{
  m_instance = a_instance;
  std::vector<VkPhysicalDevice> l_physicalDevices ( GetAllPhysicalDevices(a_instance) );
  std::map<int, VkPhysicalDevice> l_suitablePhysicalDevices;
  
  for ( VkPhysicalDevice& phDev : l_physicalDevices)
  { 
    int l_rating{RateSuitability(phDev, a_surface, a_actualWidth, a_actualHeight)};

    if( l_rating > 0 )
      l_suitablePhysicalDevices[l_rating] = phDev;
    
  }
  return l_suitablePhysicalDevices;
}



// // // void VulkanPhysicalDeviceSelector::CreateLogicalDevice(const VkPhysicalDevice& a_phDev)
// // // {
  // // // if ( m_physicalDevice = VK_NULL_HANDLE)
    // // // throw std::runtime_error("Cannot Create logical device before physical device!!!");
  
  // // // QueueFamilyIndices indices = FindQueueFamilies(m_physicalDevice);
  // // // VkPhysicalDeviceFeatures l_features;
  // // // vkGetPhysicalDeviceFeatures(m_physicalDevice,&l_features);
  
  // // // m_logicalDeviceMan->Init(m_physicalDevice, m_surface, indices, l_features, *m_deviceExtensions);
// // // }

std::vector<const char*> VulkanPhysicalDeviceSelector::GetDeviceExtensions(){ return *m_deviceExtensions; }

SwapChainSupportDetails VulkanPhysicalDeviceSelector::GetSwapChainSupportDetails(const VkPhysicalDevice& a_phDev, const VkSurfaceKHR& a_surface, const uint32_t& a_actualWidth, const uint32_t& a_actualHeight) 
{
  SwapChainSupportDetails details(a_actualWidth, a_actualHeight);

  auto vkGetPhysicalDeviceSurfaceCapabilitiesKHR = reinterpret_cast< PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR>(vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR"));
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(a_phDev, a_surface, &details.capabilities);

  uint32_t formatCount;
  auto vkGetPhysicalDeviceSurfaceFormatsKHR = reinterpret_cast< PFN_vkGetPhysicalDeviceSurfaceFormatsKHR>(vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceSurfaceFormatsKHR"));
  vkGetPhysicalDeviceSurfaceFormatsKHR(a_phDev, a_surface, &formatCount, nullptr);

  if (formatCount != 0) {
    details.formats->resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(a_phDev, a_surface, &formatCount, details.formats->data());
  }

  uint32_t presentModeCount;
  auto vkGetPhysicalDeviceSurfacePresentModesKHR = reinterpret_cast< PFN_vkGetPhysicalDeviceSurfacePresentModesKHR>(vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceSurfacePresentModesKHR"));
  vkGetPhysicalDeviceSurfacePresentModesKHR(a_phDev, a_surface, &presentModeCount, nullptr);

  if (presentModeCount != 0) {
    details.presentModes->resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(a_phDev, a_surface, &presentModeCount, details.presentModes->data());
  }
  
  return details;
}

