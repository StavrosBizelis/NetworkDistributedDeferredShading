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
  if( g_enableValidationLayers ) m_requiredInstanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
  
  CreateVulkanInstance();
  if( g_enableValidationLayers ) SetupDebugCallback();
  
}


VulkanDriver::~VulkanDriver()
{

  Shutdown();
}


void VulkanDriver::Init(const glm::vec2& a_windowSize )
{
  
  m_windowSize = a_windowSize;

  m_logicalDeviceMan = std::make_shared<VulkanLogicalDeviceManager>();
  
  // choose physical device
  std::map<int, VkPhysicalDevice> l_physicalDevices = m_physicalDeviceMan.GetSuitablePhysicalDevices( m_instance, m_surface, m_windowSize.x, m_windowSize.y);
  if( l_physicalDevices.size() == 0 )
    throw std::runtime_error("failed to find a suitable GPU!");
  m_chosenPhysicalDevice = l_physicalDevices.begin()->second;
  VkPhysicalDeviceProperties l_prop;
  vkGetPhysicalDeviceProperties(m_chosenPhysicalDevice, &l_prop);
  IFDBG( std::cout << "Best Suitable device: " << l_prop.deviceName << std::endl; );
  // create logical device, swap chain and Image views for this swap chain 
  SwapChainSupportDetails l_swapChainDetails = m_physicalDeviceMan.GetSwapChainSupportDetails(m_chosenPhysicalDevice, m_surface, m_windowSize.x, m_windowSize.y);
  VkPhysicalDeviceFeatures l_features;
  vkGetPhysicalDeviceFeatures(m_chosenPhysicalDevice,&l_features);
  
  m_logicalDeviceMan->Init(m_chosenPhysicalDevice, m_surface, m_physicalDeviceMan.FindQueueFamilies(m_chosenPhysicalDevice, m_surface) , l_features, m_physicalDeviceMan.GetDeviceExtensions());
  // setup memory
  VkDeviceSize l_stagingMemorySize = 1048576 * 200; // 200mb
  VkDeviceSize l_vertexMemorySize = 1048576 * 40; // 40mb
  VkDeviceSize l_indexMemorySize = 1048576 * 40; // 40mb
  VkDeviceSize l_uniformBufferMemorySize = 1048576 * 100; // 100mb
  VkDeviceSize l_mixBufferMemorySize = 1048576 * 4; // 4mb
  VkDeviceSize l_shaderImagesSize = 1048576 * 200; // 200mb
  VkDeviceSize l_colourAttachmentsSize = 1048576 * 120; // 120mb
  VkDeviceSize l_downloadingColourAttachmentsSize = 1048576 * 120; // 120mb
  VkDeviceSize l_depthStencilAttachmentsSize = 1048576 * 80; // 80mb
  
  
  m_logicalDeviceMan->GetMemoryManager()->Init(l_stagingMemorySize, l_vertexMemorySize, l_indexMemorySize, l_uniformBufferMemorySize, l_mixBufferMemorySize, 
                                               l_shaderImagesSize, l_colourAttachmentsSize, l_downloadingColourAttachmentsSize, l_depthStencilAttachmentsSize);

  // error in CreateSwapChain
  IFDBG( std::cout << "Initialized memory \n"; );
  m_logicalDeviceMan->CreateSwapChain( l_swapChainDetails );
  IFDBG( std::cout << "Created swapchain\n"; );
  m_logicalDeviceMan->CreateImageViews();
  IFDBG( std::cout << "Created image views\n"; );
  
  // VulkanRenderPassFactory l_factory(m_logicalDeviceMan);
  // m_logicalDeviceMan->SetRenderPass( l_factory.CreateColorRenderPass(true) );
  
  // // // // // // // VkPhysicalDeviceProperties l_props;
  // // // // // // // vkGetPhysicalDeviceProperties(m_chosenPhysicalDevice, &l_props);
  // // // // // // // std::cout << "maxDescriptorSetUniformBuffers" << l_props.limits.maxDescriptorSetUniformBuffers << std::endl;
  // // // // // // // std::cout << "maxBoundDescriptorSets" << l_props.limits.maxBoundDescriptorSets << std::endl;
}

// void VulkanDriver::Update(){ m_logicalDeviceMan->Update(); }



VulkanPhysicalDeviceSelector& VulkanDriver::GetPhysicalDeviceSelector() { return m_physicalDeviceMan;    }

VkPhysicalDevice VulkanDriver::GetSelectedPhysicalDevice(){ return m_chosenPhysicalDevice; }

std::shared_ptr<VulkanLogicalDeviceManager> VulkanDriver::GetLogicalDeviceManager() { return m_logicalDeviceMan; }

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
  
  
  if( g_enableValidationLayers )
  {
    std::cout << "Enable Validation Layers\n";
    l_createInfo.enabledLayerCount = static_cast<uint32_t>(g_validationLayers.size());
    l_createInfo.ppEnabledLayerNames = g_validationLayers.data();
  }
  else l_createInfo.enabledLayerCount = 0;
  
  
  if (vkCreateInstance(&l_createInfo, nullptr, &m_instance) != VK_SUCCESS) {
    throw std::runtime_error("failed to create instance!");
  }
}


void VulkanDriver::SetupDebugCallback() 
{
  VkDebugReportCallbackCreateInfoEXT l_createInfo = {};
  l_createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
  l_createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
  l_createInfo.pfnCallback = DebugCallback;

  if (CreateDebugReportCallbackEXT(m_instance, &l_createInfo, nullptr, &m_callback) != VK_SUCCESS) {
    throw std::runtime_error("failed to set up debug callback!");
  }

}

