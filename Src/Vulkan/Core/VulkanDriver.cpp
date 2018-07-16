#define ZR_EXPORT 1       // export this class to dll
#include <Vulkan/Core/VulkanDriver.h>
// #include <VulkanDriver/VulkanRenderPassFactory.h>
#include <Common/Core/MyUtilities.h>
#include <Vulkan/Core/VulkanValidationLayerEnabler.h>



VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback) {
    auto func = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pCallback);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
    if (func != nullptr) {
        func(instance, callback, pAllocator);
    }
}


VulkanDriver::VulkanDriver( const std::vector<const char*>& a_requiredInstanceExtensions )
{
  // init required extensions before anything
  m_requiredInstanceExtensions = a_requiredInstanceExtensions;
  IFDBG( m_requiredInstanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME); );
  
  CreateVulkanInstance();
  SetupDebugCallback();
}


VulkanDriver::~VulkanDriver()
{

  Shutdown();
}


void VulkanDriver::Init(const glm::vec2& a_windowSize )
{
  m_windowSize = a_windowSize;

  m_logicalDeviceMan = std::make_shared<VulkanLogicalDeviceManager>();
  
  // // choose physical device
  // std::map<int, VkPhysicalDevice> l_physicalDevices = m_physicalDeviceMan.GetSuitablePhysicalDevices( m_instance, m_surface, m_windowSize.x, m_windowSize.y);
  // if( l_physicalDevices.size() == 0 )
    // throw std::runtime_error("failed to find a suitable GPU!");
  // m_chosenPhysicalDevice = l_physicalDevices.begin()->second;
  
  // // create logical device, swap chain and Image views for this swap chain 
  // SwapChainSupportDetails l_swapChainDetails = m_physicalDeviceMan.GetSwapChainSupportDetails(m_chosenPhysicalDevice, m_surface, m_windowSize.x, m_windowSize.y);
  // VkPhysicalDeviceFeatures l_features;
  // vkGetPhysicalDeviceFeatures(m_chosenPhysicalDevice,&l_features);
  
  // m_logicalDeviceMan->Init(m_chosenPhysicalDevice, m_surface, m_physicalDeviceMan.FindQueueFamilies(m_chosenPhysicalDevice, m_surface) , l_features, m_physicalDeviceMan.GetDeviceExtensions());
  
  // // error in CreateSwapChain
  // m_logicalDeviceMan->CreateSwapChain( l_swapChainDetails);
  // m_logicalDeviceMan->CreateImageViews();
  
  // /// @TODO make the VulkanRenderPassFactory functionality -> programmable
  // /// @TODO maybe add multiple factories - so users can add their own RenderPasses
  // VulkanRenderPassFactory l_factory(m_logicalDeviceMan);
  // m_logicalDeviceMan->SetRenderPass( l_factory.CreateColorRenderPass(true) );
  
  
}

// void VulkanDriver::Update(){ m_logicalDeviceMan->Update(); }



// VulkanPhysicalDeviceSelector& VulkanDriver::GetPhysicalDeviceSelector() { return m_physicalDeviceMan;    }

// VkPhysicalDevice VulkanDriver::GetSelectedPhysicalDevice(){ return m_chosenPhysicalDevice; }

// std::shared_ptr<VulkanLogicalDeviceManager> VulkanDriver::GetLogicalDeviceManager() { return m_logicalDeviceMan; }

void VulkanDriver::Shutdown()
{
  // destroy image views
  
  // destroy swapchain
  
  // destroys logical device
  // m_logicalDeviceMan->Shutdown();
  
  DestroyDebugReportCallbackEXT(m_instance, m_callback, nullptr);
  if( m_surface )
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
  vkDestroyInstance(m_instance, nullptr);
}

void VulkanDriver::CreateVulkanInstance()
{
  m_surface = 0;
  
  VkApplicationInfo l_appInfo;
  l_appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  l_appInfo.pApplicationName = "";
  l_appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  l_appInfo.pEngineName = "";
  l_appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  l_appInfo.apiVersion = VK_API_VERSION_1_0;
  
  
  VkInstanceCreateInfo l_createInfo = {};
  l_createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  l_createInfo.pApplicationInfo = &l_appInfo;
  
  l_createInfo.enabledExtensionCount = static_cast<uint32_t>(m_requiredInstanceExtensions.size());
  l_createInfo.ppEnabledExtensionNames = m_requiredInstanceExtensions.data();
  
  l_createInfo.enabledLayerCount = 0;
  
  IFDBG(
    {
      l_createInfo.enabledLayerCount = static_cast<uint32_t>(g_validationLayers.size());
      l_createInfo.ppEnabledLayerNames = g_validationLayers.data();
    }
  );

  if (vkCreateInstance(&l_createInfo, nullptr, &m_instance) != VK_SUCCESS) {
    throw std::runtime_error("failed to create instance!");
  }
  printf("Created instance\n");
}


void VulkanDriver::SetupDebugCallback() {
  IFDBG(
  {
    VkDebugReportCallbackCreateInfoEXT l_createInfo = {};
    l_createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    l_createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    l_createInfo.pfnCallback = DebugCallback;

    if (CreateDebugReportCallbackEXT(m_instance, &l_createInfo, nullptr, &m_callback) != VK_SUCCESS) {
      throw std::runtime_error("failed to set up debug callback!");
    }
  }
  );
}

