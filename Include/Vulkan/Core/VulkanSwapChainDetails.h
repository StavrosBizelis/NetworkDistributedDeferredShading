#ifndef VULKAN_SWAP_CHAIN_DETAILS_H
#define VULKAN_SWAP_CHAIN_DETAILS_H


#include <vector>
#include <vulkan/vulkan.hpp>


#include <memory>



// temporary file reader
std::vector<char> ReadFile(const std::string& filename);


struct SwapChainSupportDetails {

  VkSurfaceCapabilitiesKHR capabilities;
  std::shared_ptr< std::vector<VkSurfaceFormatKHR> > formats; // do not delete
  std::shared_ptr< std::vector<VkPresentModeKHR> > presentModes; // do not delete

  uint32_t m_actualWidth;
  uint32_t m_actualHeight;
  
  SwapChainSupportDetails( const uint32_t& a_actualWidth = 0, const uint32_t& a_actualHeight = 0);
  SwapChainSupportDetails( const SwapChainSupportDetails& a_other);   ///< copy constructor
  SwapChainSupportDetails& operator=(const SwapChainSupportDetails&);    ///< copy assignment operator

  SwapChainSupportDetails(SwapChainSupportDetails&& a_other) = default;   ///< move constructor
  SwapChainSupportDetails& operator=(SwapChainSupportDetails&&) = default;    ///< move assignment operator
  ~SwapChainSupportDetails();
  
  VkSurfaceFormatKHR ChooseSwapSurfaceFormat() const;
  VkPresentModeKHR ChooseSwapPresentMode() const;
  VkExtent2D ChooseSwapExtent() const;
};

struct QueueFamilyIndices 
{  
  int m_graphicsFamily = -1;
  int m_presentFamily = -1;

  bool IsComplete() const;
  QueueFamilyIndices() = default;
  QueueFamilyIndices(const QueueFamilyIndices& a_other) = default;
  QueueFamilyIndices(QueueFamilyIndices&& a_other) = default;   ///< move constructor
  QueueFamilyIndices& operator=(const QueueFamilyIndices&) = default;    ///< copy assignment operator
  QueueFamilyIndices& operator=(QueueFamilyIndices&&) = default;    ///< move assignment operator
};

#endif