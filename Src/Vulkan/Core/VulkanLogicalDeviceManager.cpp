#include <Vulkan/Core/VulkanLogicalDeviceManager.h>

#include <set>


#include <chrono>
#include <Vulkan/Core/VulkanValidationLayerEnabler.h>
#include <glm/gtc/matrix_transform.hpp>
// #include <VulkanDriver/VulkanRenderPassFactory.h>
#include <Common/Core/MyUtilities.h>

VulkanLogicalDeviceManager::VulkanLogicalDeviceManager()
  : m_swapChain(VK_NULL_HANDLE)
{
  m_swapChainImages = std::make_shared< std::vector<VkImage> >();
  m_swapChainImageViews = std::make_shared< std::vector<VkImageView> >();
}




VulkanLogicalDeviceManager::~VulkanLogicalDeviceManager(){}


void VulkanLogicalDeviceManager::Init(const VkPhysicalDevice& a_physicalDevice, const VkSurfaceKHR& a_surface, const QueueFamilyIndices& a_queueFamilyIndexes, 
                                      const VkPhysicalDeviceFeatures& a_physicalDeviceFeatures, const std::vector<const char*>& a_deviceExtensions)
{
  m_chosenPhysicalDevice = a_physicalDevice;
  m_queueFamilyIndexes = a_queueFamilyIndexes;
  m_surface = a_surface;
  
  
  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<int> uniqueQueueFamilies = {a_queueFamilyIndexes.m_graphicsFamily, a_queueFamilyIndexes.m_presentFamily};

  float queuePriority = 1.0f;
  for (int queueFamily : uniqueQueueFamilies) 
  {
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }


  VkDeviceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pQueueCreateInfos = queueCreateInfos.data();

  createInfo.pEnabledFeatures = &a_physicalDeviceFeatures;

  createInfo.enabledExtensionCount = static_cast<uint32_t>(a_deviceExtensions.size());
  createInfo.ppEnabledExtensionNames = a_deviceExtensions.data();
  createInfo.enabledLayerCount = 0;

  IFDBG(    createInfo.enabledLayerCount = static_cast<uint32_t>(g_validationLayers.size()) );
  IFDBG(    createInfo.ppEnabledLayerNames = g_validationLayers.data() );
  
  
  if (vkCreateDevice(a_physicalDevice, &createInfo, nullptr, &m_logicalDevice) != VK_SUCCESS)
    throw std::runtime_error("failed to create logical device!");

  vkGetDeviceQueue(m_logicalDevice, a_queueFamilyIndexes.m_graphicsFamily, 0, &m_graphicsQueue);
  vkGetDeviceQueue(m_logicalDevice, a_queueFamilyIndexes.m_presentFamily, 0, &m_presentQueue);
  
  
  /// @TODO Set as parameter the size of the local and the host memory
  m_memory = std::make_shared<VulkanMemory> ( 
              m_chosenPhysicalDevice, m_logicalDevice,
              this->GetGraphicsQueue(), m_queueFamilyIndexes.m_graphicsFamily  );
            
}


void VulkanLogicalDeviceManager::ReInit()
{
  vkDeviceWaitIdle(m_logicalDevice);
  
  
  CleanUpSwapChain();
  
  CreateSwapChain(m_details);
  CreateImageViews();
  // // // // // // // // // // // // // //  m_renderPass->ReInit();
}



void VulkanLogicalDeviceManager::Update()
{
  // UpdateUniformBuffers();
  
  uint32_t l_imageIndex;
  VkResult result = (VkResult)0;/////////////////////////////////////// vkAcquireNextImageKHR(m_logicalDevice, m_swapChain, std::numeric_limits<uint64_t>::max(), m_renderPass->GetSemaphores().first, VK_NULL_HANDLE, &l_imageIndex);
  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
      ReInit();
      return;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
      throw std::runtime_error("failed to acquire swap chain image!");
  }

  
  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {VkSemaphore()}; /////////////////////////////////////// {m_renderPass->GetSemaphores().first};
  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;


  /////////////////////////////////////// m_renderPass->GetPrimaryCommandBuffer()->Update();
  
  submitInfo.commandBufferCount = 1;
  VkCommandBuffer l_cmdBuffer[] = {VkCommandBuffer()}; ///////////////////////////////////////  { m_renderPass->GetPrimaryCommandBuffer()->GetNextCommandBufferHandle() };
  submitInfo.pCommandBuffers = l_cmdBuffer;

  VkSemaphore signalSemaphores[] = {VkSemaphore()}; /////////////////////////////////////// {m_renderPass->GetSemaphores().second};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
      throw std::runtime_error("failed to submit draw command buffer!");

  VkPresentInfoKHR presentInfo = {};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {m_swapChain};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;

  presentInfo.pImageIndices = &l_imageIndex;

  result = vkQueuePresentKHR(m_presentQueue, &presentInfo);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
      ReInit();
  } else if (result != VK_SUCCESS) {
      throw std::runtime_error("failed to present swap chain image!");
  }
  
  vkQueueWaitIdle(m_presentQueue);
}


void VulkanLogicalDeviceManager::CreateSwapChain(const VkSurfaceFormatKHR& a_surfaceFormat, const VkPresentModeKHR& a_presentMode, const VkExtent2D& a_extent, const QueueFamilyIndices& a_queueFamilyIndexes, 
                      const VkSurfaceTransformFlagBitsKHR& a_pretransform, const uint32_t& a_minImageCount, const uint32_t& a_maxImageCount)
{
  VkSurfaceFormatKHR surfaceFormat = a_surfaceFormat;
  VkPresentModeKHR presentMode = a_presentMode;
  m_swapChainExtent = a_extent;

  uint32_t imageCount = a_minImageCount + 1;
  if (a_maxImageCount > 0 && imageCount > a_maxImageCount)
    imageCount = a_maxImageCount;

  VkSwapchainCreateInfoKHR createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = m_surface;

  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = m_swapChainExtent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  QueueFamilyIndices indices = a_queueFamilyIndexes;
  uint32_t queueFamilyIndices[] = {(uint32_t) indices.m_graphicsFamily, (uint32_t) indices.m_presentFamily};

  if (indices.m_graphicsFamily != indices.m_presentFamily) 
  {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  } 
  else
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

  createInfo.preTransform = a_pretransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;

  createInfo.oldSwapchain = m_swapChain;

  if (vkCreateSwapchainKHR(m_logicalDevice, &createInfo, nullptr, &m_swapChain) != VK_SUCCESS)
    throw std::runtime_error("failed to create swap chain!");

  vkGetSwapchainImagesKHR(m_logicalDevice, m_swapChain, &imageCount, nullptr);
  m_swapChainImages->resize(imageCount);
  vkGetSwapchainImagesKHR(m_logicalDevice, m_swapChain, &imageCount, m_swapChainImages->data());

  m_swapChainImageFormat = surfaceFormat.format;
  
}


void VulkanLogicalDeviceManager::CreateSwapChain(const SwapChainSupportDetails& a_details)
{
  m_details = a_details;
  this->CreateSwapChain( m_details.ChooseSwapSurfaceFormat(), m_details.ChooseSwapPresentMode(), m_details.ChooseSwapExtent(), 
                         m_queueFamilyIndexes, VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR, 
                         m_details.capabilities.minImageCount, m_details.capabilities.maxImageCount);
}


void VulkanLogicalDeviceManager::CreateImageViews()
{
  m_swapChainImageViews->resize(m_swapChainImages->size());

  for (size_t i = 0; i < m_swapChainImages->size(); i++) {
    VkImageViewCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = (*m_swapChainImages)[i];
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = m_swapChainImageFormat;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(m_logicalDevice, &createInfo, nullptr, &( (*m_swapChainImageViews)[i]) ) != VK_SUCCESS)
      throw std::runtime_error("failed to create image views!");
  }
  
}

// // // // // void VulkanLogicalDeviceManager::SetRenderPass(const std::shared_ptr<VulkanRenderPass>& a_renderPass){ m_renderPass = a_renderPass; }

// // // // std::shared_ptr<VulkanRenderPass> VulkanLogicalDeviceManager::GetRenderPass(){  return m_renderPass;  }
VkDevice VulkanLogicalDeviceManager::GetDevice(){  return m_logicalDevice;  }
VkPhysicalDevice VulkanLogicalDeviceManager::GetPhysicalDevice(){  return m_chosenPhysicalDevice;  }
VkFormat VulkanLogicalDeviceManager::GetSwapChainImageFormat(){  return m_swapChainImageFormat;  } 
VkExtent2D VulkanLogicalDeviceManager::GetSwapChainExtend(){  return m_swapChainExtent;  } 
VkQueue VulkanLogicalDeviceManager::GetGraphicsQueue(){ return m_graphicsQueue; }
VkQueue VulkanLogicalDeviceManager::GetPresentQueue(){ return m_presentQueue; }
std::vector<VkImage> VulkanLogicalDeviceManager::GetSwapChainImages(){ return *m_swapChainImages; }
std::vector<VkImageView> VulkanLogicalDeviceManager::GetSwapChainImageViews(){ return *m_swapChainImageViews; }
QueueFamilyIndices VulkanLogicalDeviceManager::GetQueueFamilyIndices(){ return m_queueFamilyIndexes; }
std::shared_ptr< VulkanMemory > VulkanLogicalDeviceManager::GetMemoryManager(){ return m_memory; }


void VulkanLogicalDeviceManager::CleanUpSwapChain()
{
 ///////////////////////////////////////  m_renderPass->CleanUpSwapChain();

  // clear swapchain image views
  for (size_t i = 0; i < m_swapChainImageViews->size(); i++)
    vkDestroyImageView(m_logicalDevice, m_swapChainImageViews->at(i), nullptr);
  m_swapChainImageViews->clear();

  // clear swapchain
  vkDestroySwapchainKHR(m_logicalDevice, m_swapChain, nullptr);
}



void VulkanLogicalDeviceManager::Shutdown()
{
  CleanUpSwapChain();
  vkDestroyDevice(m_logicalDevice, nullptr);
}


void VulkanLogicalDeviceManager::UpdateUniformBuffers()
{
  // static auto startTime = std::chrono::high_resolution_clock::now();

  // auto currentTime = std::chrono::high_resolution_clock::now();
  // float time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count() / 1000.0f;

  // UniformBufferObject ubo = {};
  // ubo.model = glm::rotate(glm::mat4(), time * glm::radians(90.0f) * 8, glm::vec3(0.0f, 0.0f, 1.0f) );
  
  
  // void* data;
  // VkDeviceMemory l_bufferMemory = 
    // GetRenderPass()->GetPrimaryCommandBuffer()->GetPipelines().front()->GetOptions().m_shadersVars->at(0).m_uniformBuffer.first->m_bufferMemory;
  
  // vkMapMemory(GetDevice(), l_bufferMemory, 0, sizeof(ubo), 0, &data);
      // memcpy(data, &ubo, sizeof(ubo));
  // vkUnmapMemory(GetDevice(), l_bufferMemory);
}




