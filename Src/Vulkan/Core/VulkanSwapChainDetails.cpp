#include <Vulkan/Core/VulkanSwapChainDetails.h>

#include <set>

#include <fstream>      // std::ifstream


std::vector<char> ReadFile(const std::string& filename)
{
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("failed to open file!");
  }

  size_t fileSize = (size_t) file.tellg();
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);

  file.close();

  return buffer;
}


SwapChainSupportDetails::SwapChainSupportDetails(const uint32_t& a_actualWidth, const uint32_t& a_actualHeight)
  : m_actualWidth(a_actualWidth), m_actualHeight(a_actualHeight), formats( std::make_shared< std::vector<VkSurfaceFormatKHR> >() ), presentModes( std::make_shared< std::vector<VkPresentModeKHR> >() )
{}

SwapChainSupportDetails::SwapChainSupportDetails( const SwapChainSupportDetails& a_other)
{
  m_actualWidth = a_other.m_actualWidth;
  m_actualHeight = a_other.m_actualHeight;
  formats = std::make_shared< std::vector<VkSurfaceFormatKHR> >( std::vector<VkSurfaceFormatKHR>(*(a_other.formats) ) );
  presentModes = std::make_shared< std::vector<VkPresentModeKHR> >( std::vector<VkPresentModeKHR>( *(a_other.presentModes) ) );

  
  capabilities = a_other.capabilities;
}

SwapChainSupportDetails& SwapChainSupportDetails::operator=(const SwapChainSupportDetails& a_other)
{
  m_actualWidth = a_other.m_actualWidth;
  m_actualHeight = a_other.m_actualHeight;
  formats = std::make_shared< std::vector<VkSurfaceFormatKHR> >( std::vector<VkSurfaceFormatKHR>(*(a_other.formats) ) );
  presentModes = std::make_shared< std::vector<VkPresentModeKHR> >( std::vector<VkPresentModeKHR>( *(a_other.presentModes) ) );
  
  capabilities = a_other.capabilities;
  
  return *this;
}

SwapChainSupportDetails::~SwapChainSupportDetails(){}


VkSurfaceFormatKHR SwapChainSupportDetails::ChooseSwapSurfaceFormat() const
{
  if (formats->size() == 1 && (*formats)[0].format == VK_FORMAT_UNDEFINED) 
    return {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};

  for (const auto& availableFormat : (*formats)) 
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
      return availableFormat;
    
  

  return (*formats)[0];
}
VkPresentModeKHR SwapChainSupportDetails::ChooseSwapPresentMode() const
{
  VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

  for (const auto& availablePresentMode : (*presentModes) ) 
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
      return availablePresentMode;
    else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
      bestMode = availablePresentMode;

  return bestMode;
}

VkExtent2D SwapChainSupportDetails::ChooseSwapExtent() const
{
  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    return capabilities.currentExtent;

  VkExtent2D actualExtent = {m_actualWidth, m_actualHeight};

  actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
  actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
  
  return actualExtent;
}


bool QueueFamilyIndices::IsComplete()  const
{
  return m_graphicsFamily >= 0 && m_presentFamily >= 0;
}
