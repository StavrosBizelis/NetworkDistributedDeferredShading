#include <Vulkan/Core/VulkanMemory.h>

#include <vulkan/vulkan.hpp>

#include "math.h"

#include <stdexcept>
	
#include <iostream>







VulkanMemorySubBuffer::VulkanMemorySubBuffer()
:m_buffer(0), m_memoryOffset(0), m_memorySize(0)
{}

VulkanMemorySubBuffer::~VulkanMemorySubBuffer()
{}




VulkanMemoryChunk::VulkanMemoryChunk(VulkanMemoryPool* a_allocator, const VkDevice& a_logicalDevice, const VkDeviceMemory& a_memorySpace, const VkDeviceSize& a_offset, const VkDeviceSize& a_size)
: m_available(true), m_allocator(a_allocator), m_logicalDevice (a_logicalDevice), m_memorySpace(a_memorySpace), m_offset(a_offset), m_size(a_size) {}

VulkanMemoryChunk::VulkanMemoryChunk(const VulkanMemoryChunk& a_other)
{
  m_allocator = a_other.m_allocator;
  m_available = a_other.m_available;
  m_logicalDevice = a_other.m_logicalDevice;
  m_memorySpace = a_other.m_memorySpace;
  m_offset = a_other.m_offset;
  m_size = a_other.m_size;
}

VulkanMemoryChunk& VulkanMemoryChunk::operator=(const VulkanMemoryChunk& a_other)
{
  m_allocator = a_other.m_allocator;
  m_available = a_other.m_available;
  m_logicalDevice = a_other.m_logicalDevice;
  m_memorySpace = a_other.m_memorySpace;
  m_offset = a_other.m_offset;
  m_size = a_other.m_size;
  
  return *this;
}

void VulkanMemoryChunk::Free()
{
  if(m_allocator)
    m_allocator->DeallocateMemory(this);
}


void VulkanMemoryPool::RemoveFromTheAvailableChunks(const VkDeviceSize& a_size, const std::shared_ptr< VulkanMemoryChunk>& a_chunk)
{
  std::map<VkDeviceSize, std::list< std::weak_ptr< VulkanMemoryChunk> > >::iterator l_iter = m_availableMemChunks->find(a_size);
  if( l_iter != m_availableMemChunks->end() )
  {
    // for(std::list< std::weak_ptr< VulkanMemoryChunk> >::iterator l_iter2 = l_iter->second.begin(); l_iter2 != l_iter->second.end(); ++l_iter2 )
      // if( l_iter2->lock() == a_chunk )
      // {
        // l_iter->second.erase( l_iter2 );
        // break;
      // }
    l_iter->second.remove_if([a_chunk]( std::weak_ptr< VulkanMemoryChunk> l_tmp) {return l_tmp.lock() == a_chunk; } );
    
    if( l_iter->second.empty() )
      m_availableMemChunks->erase(l_iter);
  }
}

void VulkanMemoryPool::AddToTheAvailableChunks(const VkDeviceSize& a_size, const std::shared_ptr< VulkanMemoryChunk>& a_chunk)
{
  if( m_availableMemChunks->find(a_size) == m_availableMemChunks->end() )
    m_availableMemChunks->insert( 
                                  std::pair<VkDeviceSize, std::list< std::weak_ptr< VulkanMemoryChunk> >> ( a_size, {} ) 
                                  ); 
  m_availableMemChunks->at(a_size).push_back(a_chunk);
}

VulkanMemoryPool::VulkanMemoryPool(const VkPhysicalDevice& a_physicalDevice, const VkDevice& a_logicalDevice, const VkDeviceSize& a_size, const VkBufferUsageFlags& a_usage, const VkMemoryPropertyFlags& a_properties, const int& a_alignment)
: m_physicalDevice(a_physicalDevice), m_logicalDevice(a_logicalDevice), m_size(a_size), m_usage(a_usage), m_properties(a_properties), m_memorySpace(NULL), m_buffer(nullptr), m_alignment(a_alignment)
  m_memChunks(NULL), m_availableMemChunks( std::make_shared< std::map<VkDeviceSize, std::list< std::weak_ptr< VulkanMemoryChunk> > > >() ) {}

VulkanMemoryPool::~VulkanMemoryPool()
{
  if( m_buffer )
    vkDestroyBuffer(m_logicalDevice, m_buffer->m_buffer, nullptr);  
  if( m_memorySpace )
    vkFreeMemory(m_logicalDevice, m_memorySpace, nullptr);
}

void VulkanMemoryPool::Init()
{
  // buffer for memory allocation
  m_buffer = std::make_shared<VulkanMemorySubBuffer>();
  m_buffer->m_memorySize = m_size;
  m_buffer->m_memoryOffset = 0;
  
  
  VkBufferCreateInfo bufferInfo = {};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = m_size;
  bufferInfo.usage = m_usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  
  VkBuffer l_buffer;// = m_buffer->m_buffer;
  if (vkCreateBuffer(m_logicalDevice, &bufferInfo, nullptr, &l_buffer) != VK_SUCCESS) {
    throw std::runtime_error("VulkanMemoryPool::Init() - failed to create dummy buffer for memory pool initialization!");
  }
  
  VkMemoryRequirements l_memRequirements;
  vkGetBufferMemoryRequirements(m_logicalDevice, l_buffer, &l_memRequirements);
  
  m_buffer->m_buffer = l_buffer;
  
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

  uint32_t l_memoryIndex;
  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) 
    if ((l_memRequirements.memoryTypeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & m_properties) == m_properties)
    {
      l_memoryIndex = i;
      break;
    }
  
  if( a_alignment == -1)
    m_alignment = l_memRequirements.alignment;

  VkMemoryAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = m_size;
  allocInfo.memoryTypeIndex = l_memoryIndex;

  if (vkAllocateMemory(m_logicalDevice, &allocInfo, nullptr, &m_memorySpace) != VK_SUCCESS)
  {
    m_memorySpace = NULL;
    throw std::runtime_error("VulkanMemoryPool::Init() - failed to allocate memory!");
  }
  

  // bind this single buffer to the whole memory
  vkBindBufferMemory(m_logicalDevice, m_buffer->m_buffer, m_memorySpace, 0);
  
  // after the memory is allocated create ONE big chunk that fills all the memory and is available to use
  m_memChunks = std::make_shared< VulkanMemoryChunk>( this, m_logicalDevice, m_memorySpace, VkDeviceSize(0) , m_size );
  m_memChunks->m_buffer = m_buffer;
  AddToTheAvailableChunks(m_size, m_memChunks);
  
}

VkMemoryPropertyFlags VulkanMemoryPool::GetPropertyFlags() { return m_properties; }
VkBufferUsageFlags VulkanMemoryPool::GetUsageFlags() { return m_usage; }
VkDeviceSize VulkanMemoryPool::GetMemorySpaceSize() { return m_size; }


bool VulkanMemoryPool::Owns(const std::shared_ptr< VulkanMemoryChunk>& a_chunk)
{
  std::shared_ptr< VulkanMemoryChunk> l_next = m_memChunks;
  while( l_next )
  {
    if( l_next == a_chunk)
      return true;
    l_next = l_next->m_next;
  }
  return false;
}

std::shared_ptr< VulkanMemoryChunk> VulkanMemoryPool::AllocateMemory(const VkDeviceSize& a_size)
{
  // create l_size taking into account the alignement
  VkDeviceSize l_size = a_size + (m_alignment - a_size%m_alignment);
  
  // best fit algorithm
  // using the map of the available chunks find in a linear fashion te best fit instead of searching the whole memory space
  for( std::map<VkDeviceSize, std::list< std::weak_ptr< VulkanMemoryChunk> > >::iterator l_iter = m_availableMemChunks->begin(); l_iter != m_availableMemChunks->end(); ++l_iter )
  {
    if(l_iter->first >= l_size )
    {
      
      std::shared_ptr< VulkanMemoryChunk> l_freeChunk = l_iter->second.begin()->lock();
      
      if( l_freeChunk )
      { 
        // do allocation 
        // if l_size == l_freeChunk->m_size THEN return this chunk and remove it from the available chunks map
        if( l_size == l_freeChunk->m_size)
        {
          l_freeChunk->m_available = false;
          
          RemoveFromTheAvailableChunks(l_iter->first, l_freeChunk);
          return l_freeChunk;
        }
        else
        {
          
          // create a new memory chunk, of the appropriate size at the beginning of the free chunk
          std::shared_ptr< VulkanMemoryChunk> l_memoryChunk = 
                std::make_shared< VulkanMemoryChunk>( this, m_logicalDevice, m_memorySpace, l_freeChunk->m_offset, l_size );
          l_memoryChunk->m_buffer = l_freeChunk->m_buffer;
                
          l_memoryChunk->m_available = false;
          // place the l_memoryChunk after the l_freeChunk->m_prev and before l_freeChunk
          l_memoryChunk->m_prev = l_freeChunk->m_prev;
          if( l_freeChunk->m_prev )
            l_freeChunk->m_prev->m_next = l_memoryChunk;
          else
            this->m_memChunks = l_memoryChunk;
          l_memoryChunk->m_next = l_freeChunk;
          
          
          // update the information of the free chunk to be after the l_memoryChunk and with the new size and offset
          l_freeChunk->m_offset = l_freeChunk->m_offset + l_size;
          l_freeChunk->m_size = l_freeChunk->m_size - l_memoryChunk->m_size;
          l_freeChunk->m_prev = l_memoryChunk;
          // m_next remain the same
          // l_freeChunk->m_next =
          
          RemoveFromTheAvailableChunks(l_iter->first, l_freeChunk);
          
          AddToTheAvailableChunks(l_freeChunk->m_size, l_freeChunk);
          
          return l_memoryChunk;
        }
        
      }
      
      break;
    }
  }
  return NULL;
}

bool VulkanMemoryPool::DeallocateMemory(const std::shared_ptr< VulkanMemoryChunk>& a_chunk )
{
  if( !this->Owns(a_chunk) )
    return false;
  
  a_chunk->m_available = true;
  // if m_prev exists
  std::shared_ptr< VulkanMemoryChunk > l_previousChunk = a_chunk->m_prev;
  if(l_previousChunk)
    // if m_prev is available AND they are used by the same in the same buffer - make the two chunks one
    if(l_previousChunk->m_available && l_previousChunk->m_buffer == a_chunk->m_buffer)
    {
      a_chunk->m_size += l_previousChunk->m_size;
      a_chunk->m_offset = l_previousChunk->m_offset;
      a_chunk->m_prev = l_previousChunk->m_prev;
      
      // if m_prev is the not the root of the list then update the previous chunk to point to this new chunk 
      if( l_previousChunk->m_prev )
        l_previousChunk->m_prev->m_next = a_chunk;
      else // update the root of the list to be this new chunk
        this->m_memChunks = a_chunk;
      
      
      // remove l_previousChunk from the m_availableMemChunks
      RemoveFromTheAvailableChunks(l_previousChunk->m_size, l_previousChunk);
      
    }
  
  
  
  
  // if m_next is also available AND they are used by the same in the same buffer - make the two chunks one
  std::shared_ptr< VulkanMemoryChunk> l_nextChunk = a_chunk->m_next;
  if(l_nextChunk)
    if(l_nextChunk->m_available && l_nextChunk->m_buffer == a_chunk->m_buffer)
    {
      a_chunk->m_size += l_nextChunk->m_size;
      a_chunk->m_next = l_nextChunk->m_next;
      if( a_chunk->m_next )
        a_chunk->m_next->m_prev = a_chunk;
      
      // remove l_nextChunk from the m_availableMemChunks
      RemoveFromTheAvailableChunks(l_nextChunk->m_size, l_nextChunk);
    }
    
  AddToTheAvailableChunks(a_chunk->m_size, a_chunk);

  return true;
}

bool VulkanMemoryPool::DeallocateMemory(VulkanMemoryChunk* a_chunk )
{
  std::shared_ptr< VulkanMemoryChunk> l_next = m_memChunks;
  while( l_next )
  {
    if( l_next.get() == a_chunk)
      return DeallocateMemory(l_next);
    l_next = l_next->m_next;
  }
  return false;
}

std::string VulkanMemoryPool::MemoryChunks()
{
  std::string l_toReturn = "MEMORY CHUNKS - Total memory " + std::to_string(this->m_size) + " Bytes \n";
  
  std::shared_ptr< VulkanMemoryChunk> l_curr = m_memChunks;
  while( l_curr )
  {
    std::string l_currentLine("");
    l_currentLine += "Offset Bytes: " + std::to_string(l_curr->m_offset) + ", ";
    l_currentLine += "Chunk Bytes: " + std::to_string(l_curr->m_size) + ", ";
    if( l_curr->m_available ) l_currentLine += "State: FREE";
    else l_currentLine += "State: ALLOCATED";       
    l_currentLine += "\n";
    
    l_toReturn += l_currentLine;
    l_curr = l_curr->m_next;
  }
  return l_toReturn;
}

std::string VulkanMemoryPool::AvailableMemoryChunks()
{
  std::string l_toReturn = "AVAILABLE MEMORY CHUNKS - Total memory " + std::to_string(this->m_size) + " Bytes \n";
  for( std::map<VkDeviceSize, std::list< std::weak_ptr< VulkanMemoryChunk> > >::iterator l_iter = m_availableMemChunks->begin(); l_iter != m_availableMemChunks->end(); ++l_iter )
  {
    std::string l_currentLine("size: ");
    l_currentLine += std::to_string( l_iter->first );
    l_currentLine += ", count: ";
    l_currentLine += std::to_string( l_iter->second.size() );
    l_currentLine += "\n";
    
    l_toReturn += l_currentLine;
  }
  return l_toReturn;
}





















VulkanSampler::VulkanSampler( const VkDevice& a_logicalDevice)
:m_logicalDevice(a_logicalDevice), m_sampler(NULL)
{
  m_params[VLK_SMPL_MIN_FLTR] = VK_FILTER_LINEAR;
  m_params[VLK_SMPL_MAG_FLTR] = VK_FILTER_LINEAR;
  m_params[VLK_SMPL_REPEAT] = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  m_params[VLK_SMPL_ANISOTROPY_LVL] = 16;
  
  Create();
}

VulkanSampler::~VulkanSampler()
{
  if(m_sampler)
    vkDestroySampler(m_logicalDevice, m_sampler, nullptr);
}

void VulkanSampler::SetSamplerObjectParameter(const VulkanSamplerOption a_parameter, const float& a_value)
{
  if(m_sampler)
    vkDestroySampler(m_logicalDevice, m_sampler, nullptr);
  m_params[a_parameter] = a_value;
  Create();
}

void VulkanSampler::Create()
{
  VkSamplerCreateInfo samplerInfo = {};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter = (VkFilter)m_params[VLK_SMPL_MAG_FLTR];
  samplerInfo.minFilter = (VkFilter)m_params[VLK_SMPL_MIN_FLTR];
  samplerInfo.addressModeU = (VkSamplerAddressMode)m_params[VLK_SMPL_REPEAT];
  samplerInfo.addressModeV = (VkSamplerAddressMode)m_params[VLK_SMPL_REPEAT];
  samplerInfo.addressModeW = (VkSamplerAddressMode)m_params[VLK_SMPL_REPEAT];
  samplerInfo.anisotropyEnable = m_params[VLK_SMPL_ANISOTROPY_LVL] > 1 ? VK_TRUE : VK_FALSE;
  samplerInfo.maxAnisotropy = m_params[VLK_SMPL_ANISOTROPY_LVL];
  samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

  if (vkCreateSampler(m_logicalDevice, &samplerInfo, nullptr, &m_sampler) != VK_SUCCESS)
    throw std::runtime_error("VulkanSampler::Create() - failed to create texture sampler!");
}


VulkanImageMemoryChunk::VulkanImageMemoryChunk(VulkanImageMemoryPool* a_allocator, const VkDevice& a_logicalDevice, const VkDeviceMemory& a_memorySpace, const VkDeviceSize& a_offset, const VkDeviceSize& a_size, 
                                               const VkDeviceSize& a_width, const VkDeviceSize& a_height, VkFormat a_format, VkImageTiling a_tiling, VkImageViewType a_imageViewType)
: m_available(true), m_allocator(a_allocator), m_logicalDevice (a_logicalDevice), m_memorySpace(a_memorySpace), m_offset(a_offset), m_size(a_size),
  m_width(a_width), m_height(a_height), m_format(a_format), m_tiling(a_tiling), m_layout(VK_IMAGE_LAYOUT_UNDEFINED), m_imageViewType(a_imageViewType), m_image(NULL), m_imageView(NULL)
  {}

void VulkanImageMemoryChunk::Free()
{
  if(m_imageView)
    vkDestroyImageView(m_logicalDevice, m_imageView, nullptr);
  if(m_image)
    vkDestroyImage(m_logicalDevice, m_image, nullptr);
  if(m_allocator)
    m_allocator->DeallocateMemory(this);
}


void VulkanImageMemoryPool::RemoveFromTheAvailableChunks(const VkDeviceSize& a_size, const std::shared_ptr< VulkanImageMemoryChunk>& a_chunk)
{
  std::map<VkDeviceSize, std::list< std::weak_ptr< VulkanImageMemoryChunk> > >::iterator l_iter = m_availableMemChunks->find(a_size);
  if( l_iter != m_availableMemChunks->end() )
  {
    // for(std::list< std::weak_ptr< VulkanImageMemoryChunk> >::iterator l_iter2 = l_iter->second.begin(); l_iter2 != l_iter->second.end(); ++l_iter2 )
      // if( l_iter2->lock() == a_chunk )
      // {
        // l_iter->second.erase( l_iter2 );
        // break;
      // }
    l_iter->second.remove_if([a_chunk]( std::weak_ptr< VulkanImageMemoryChunk> l_tmp) {return l_tmp.lock() == a_chunk; } );
    
    if( l_iter->second.empty() )
      m_availableMemChunks->erase(l_iter);
  }
}

void VulkanImageMemoryPool::AddToTheAvailableChunks(const VkDeviceSize& a_size, const std::shared_ptr< VulkanImageMemoryChunk>& a_chunk)
{
  if( m_availableMemChunks->find(a_size) == m_availableMemChunks->end() )
    m_availableMemChunks->insert( 
                                  std::pair<VkDeviceSize, std::list< std::weak_ptr< VulkanImageMemoryChunk> >> ( a_size, {} ) 
                                  ); 
  m_availableMemChunks->at(a_size).push_back(a_chunk);
}

VkImage VulkanImageMemoryPool::CreateImage(const uint32_t& a_width, const uint32_t& a_height, VkFormat a_format, VkImageTiling a_tiling, VkImageViewType a_imageViewType, VkDeviceSize& a_outSize)
{

  VkImage l_image;
  VkImageCreateInfo imageInfo = {};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width = a_width;
  imageInfo.extent.height = a_height;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.format = a_format;
  imageInfo.tiling = a_tiling;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage = m_usage;
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  if( a_imageViewType == VK_IMAGE_VIEW_TYPE_CUBE )
  {
    imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    imageInfo.arrayLayers = 6;
  }
  
  if (vkCreateImage(m_logicalDevice, &imageInfo, nullptr, &l_image) != VK_SUCCESS) {
      throw std::runtime_error("VulkanImageMemoryPool::CreateImage - failed to create image!");
  }
  
  
  VkMemoryRequirements l_memRequirements;
  vkGetImageMemoryRequirements(m_logicalDevice, l_image, &l_memRequirements);
  
  a_outSize = l_memRequirements.size;
  return l_image;
}


VulkanImageMemoryPool::VulkanImageMemoryPool(const VkPhysicalDevice& a_physicalDevice, const VkDevice& a_logicalDevice, const VkDeviceSize& a_size, const VkImageUsageFlags& a_usage, const VkMemoryPropertyFlags& a_properties)
: m_physicalDevice(a_physicalDevice), m_logicalDevice(a_logicalDevice), m_size(a_size), m_usage(a_usage), m_properties(a_properties), m_memorySpace(NULL),
  m_memChunks(NULL), m_availableMemChunks( std::make_shared< std::map<VkDeviceSize, std::list< std::weak_ptr< VulkanImageMemoryChunk> > > >() ) {}

VulkanImageMemoryPool::~VulkanImageMemoryPool()
{
  if( m_memorySpace )
    vkFreeMemory(m_logicalDevice, m_memorySpace, nullptr);
}

void VulkanImageMemoryPool::Init()
{
  
  // dummy image for memory allocation
  VkImage l_image;
  VkImageCreateInfo imageInfo = {};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width = 128;
  imageInfo.extent.height =128;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
  imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage = m_usage;
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  imageInfo.flags = 0;

  if (vkCreateImage(m_logicalDevice, &imageInfo, nullptr, &l_image) != VK_SUCCESS) {
      throw std::runtime_error("VulkanImageMemoryPool::Init() - failed to create image!");
  }

  VkMemoryRequirements l_memRequirements;
  vkGetImageMemoryRequirements(m_logicalDevice, l_image, &l_memRequirements);
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

  int l_memoryIndex = -1;
  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) 
    if ((l_memRequirements.memoryTypeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & m_properties) == m_properties)
    {
      l_memoryIndex = i;
      break;
    }
  if( l_memoryIndex == -1 )
    throw std::runtime_error("VulkanImageMemoryPool::Init() - failed to find appropriate memory index!");
    
  m_alignment = l_memRequirements.alignment;
  
  
  VkMemoryAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = m_size;
  allocInfo.memoryTypeIndex = l_memoryIndex;

  if (vkAllocateMemory(m_logicalDevice, &allocInfo, nullptr, &m_memorySpace) != VK_SUCCESS)
  {
    m_memorySpace = NULL;
    throw std::runtime_error("VulkanImageMemoryPool::Init() - failed to allocate memory!");
  }
  
  // destroy dummy image
  vkDestroyImage(m_logicalDevice, l_image, nullptr);
  
  // after the memory is allocated create ONE big chunk that fills all the memory and is available to use
  m_memChunks = std::make_shared< VulkanImageMemoryChunk>( this, m_logicalDevice, m_memorySpace, VkDeviceSize(0) , m_size, 0, 0, VK_FORMAT_UNDEFINED, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_VIEW_TYPE_2D );
  AddToTheAvailableChunks(m_size, m_memChunks);
  
}

VkMemoryPropertyFlags VulkanImageMemoryPool::GetPropertyFlags() { return m_properties; }
VkBufferUsageFlags VulkanImageMemoryPool::GetUsageFlags() { return m_usage; }
VkDeviceSize VulkanImageMemoryPool::GetMemorySpaceSize() { return m_size; }


bool VulkanImageMemoryPool::Owns(const std::shared_ptr< VulkanImageMemoryChunk>& a_chunk)
{
  std::shared_ptr< VulkanImageMemoryChunk> l_next = m_memChunks;
  while( l_next )
  {
    if( l_next == a_chunk)
      return true;
    l_next = l_next->m_next;
  }
  return false;
}

std::shared_ptr< VulkanImageMemoryChunk> VulkanImageMemoryPool::AllocateMemory(const uint32_t& a_width, const uint32_t& a_height, VkFormat a_format, VkImageTiling a_tiling, VkImageViewType a_imageViewType)
{
  
  VkDeviceSize l_size;
  
  VkImage l_img = CreateImage(a_width, a_height, a_format, a_tiling, a_imageViewType, l_size);
  l_size = l_size + l_size%m_alignment;

  // best fit algorithm
  // using the map of the available chunks find in a linear fashion te best fit instead of searching the whole memory space
  for( std::map<VkDeviceSize, std::list< std::weak_ptr< VulkanImageMemoryChunk> > >::iterator l_iter = m_availableMemChunks->begin(); l_iter != m_availableMemChunks->end(); ++l_iter )
  {
    if(l_iter->first >= l_size )
    {
      
      std::shared_ptr< VulkanImageMemoryChunk> l_freeChunk = l_iter->second.begin()->lock();
      
      if( l_freeChunk )
      { 
        // do allocation 
        // if l_size == l_freeChunk->m_size THEN return this chunk and remove it from the available chunks map
        if( l_size == l_freeChunk->m_size)
        {
          l_freeChunk->m_available = false;
          
          l_freeChunk->m_image = l_img;
          l_freeChunk->m_width = a_width;
          l_freeChunk->m_height = a_height;
          l_freeChunk->m_format = a_format;
          l_freeChunk->m_tiling = a_tiling;
          l_freeChunk->m_layout = VK_IMAGE_LAYOUT_UNDEFINED;
          l_freeChunk->m_imageViewType = a_imageViewType;
          
          RemoveFromTheAvailableChunks(l_iter->first, l_freeChunk);
          
          vkBindImageMemory(m_logicalDevice, l_freeChunk->m_image, m_memorySpace, l_freeChunk->m_offset);
          return l_freeChunk;
        }
        else
        {
          
          // create a new memory chunk, of the appropriate size at the beginning of the free chunk
          std::shared_ptr< VulkanImageMemoryChunk> l_memoryChunk = 
                std::make_shared< VulkanImageMemoryChunk>( this, m_logicalDevice, m_memorySpace, l_freeChunk->m_offset, l_size, a_width, a_height, a_format, a_tiling, a_imageViewType );
          l_memoryChunk->m_image = l_img;
          vkBindImageMemory(m_logicalDevice, l_memoryChunk->m_image, m_memorySpace, l_memoryChunk->m_offset);
                
          l_memoryChunk->m_available = false;
          // place the l_memoryChunk after the l_freeChunk->m_prev and before l_freeChunk
          l_memoryChunk->m_prev = l_freeChunk->m_prev;
          if( l_freeChunk->m_prev )
            l_freeChunk->m_prev->m_next = l_memoryChunk;
          else
            this->m_memChunks = l_memoryChunk;
          l_memoryChunk->m_next = l_freeChunk;
          
          
          // update the information of the free chunk to be after the l_memoryChunk and with the new size and offset
          l_freeChunk->m_offset = l_freeChunk->m_offset + l_size;
          l_freeChunk->m_size = l_freeChunk->m_size - l_memoryChunk->m_size;
          l_freeChunk->m_prev = l_memoryChunk;
          // m_next remain the same
          // l_freeChunk->m_next =
          
          RemoveFromTheAvailableChunks(l_iter->first, l_freeChunk);
          
          AddToTheAvailableChunks(l_freeChunk->m_size, l_freeChunk);
          
          return l_memoryChunk;
        }
        
      }
      
      break;
    }
  }
  // if we reach that point the allocation failed so delete the image
  vkDestroyImage(m_logicalDevice, l_img, nullptr);
  return NULL;
}

bool VulkanImageMemoryPool::DeallocateMemory(const std::shared_ptr< VulkanImageMemoryChunk>& a_chunk )
{
  if( !this->Owns(a_chunk) )
    return false;
  
  a_chunk->m_available = true;
  // if m_prev exists
  std::shared_ptr< VulkanImageMemoryChunk > l_previousChunk = a_chunk->m_prev;
  if(l_previousChunk)
    // if m_prev is available make the two chunks one
    if(l_previousChunk->m_available)
    {
      a_chunk->m_size += l_previousChunk->m_size;
      a_chunk->m_offset = l_previousChunk->m_offset;
      a_chunk->m_prev = l_previousChunk->m_prev;
      
      // if m_prev is the not the root of the list then update the previous chunk to point to this new chunk 
      if( l_previousChunk->m_prev )
        l_previousChunk->m_prev->m_next = a_chunk;
      else // update the root of the list to be this new chunk
        this->m_memChunks = a_chunk;
      
      
      // remove l_previousChunk from the m_availableMemChunks
      RemoveFromTheAvailableChunks(l_previousChunk->m_size, l_previousChunk);
      
    }
  
  
  
  
  // if m_next is also available  make the two chunks one
  std::shared_ptr< VulkanImageMemoryChunk> l_nextChunk = a_chunk->m_next;
  if(l_nextChunk)
    if(l_nextChunk->m_available)
    {
      a_chunk->m_size += l_nextChunk->m_size;
      a_chunk->m_next = l_nextChunk->m_next;
      if( a_chunk->m_next )
        a_chunk->m_next->m_prev = a_chunk;
      
      // remove l_nextChunk from the m_availableMemChunks
      RemoveFromTheAvailableChunks(l_nextChunk->m_size, l_nextChunk);
    }
    
  AddToTheAvailableChunks(a_chunk->m_size, a_chunk);

  return true;
}

bool VulkanImageMemoryPool::DeallocateMemory(VulkanImageMemoryChunk* a_chunk )
{
  std::shared_ptr< VulkanImageMemoryChunk> l_next = m_memChunks;
  while( l_next )
  {
    if( l_next.get() == a_chunk)
      return DeallocateMemory(l_next);
    l_next = l_next->m_next;
  }
  return false;
}

std::string VulkanImageMemoryPool::MemoryChunks()
{
  std::string l_toReturn = "MEMORY CHUNKS - Total memory " + std::to_string(this->m_size) + " Bytes \n";
  
  std::shared_ptr< VulkanImageMemoryChunk> l_curr = m_memChunks;
  while( l_curr )
  {
    std::string l_currentLine("");
    l_currentLine += "Offset Bytes: " + std::to_string(l_curr->m_offset) + ", ";
    l_currentLine += "Chunk Bytes: " + std::to_string(l_curr->m_size) + ", ";
    if( l_curr->m_available ) l_currentLine += "State: FREE";
    else l_currentLine += "State: ALLOCATED";       
    l_currentLine += "\n";
    
    l_toReturn += l_currentLine;
    l_curr = l_curr->m_next;
  }
  return l_toReturn;
}

std::string VulkanImageMemoryPool::AvailableMemoryChunks()
{
  std::string l_toReturn = "AVAILABLE MEMORY CHUNKS - Total memory " + std::to_string(this->m_size) + " Bytes \n";
  for( std::map<VkDeviceSize, std::list< std::weak_ptr< VulkanImageMemoryChunk> > >::iterator l_iter = m_availableMemChunks->begin(); l_iter != m_availableMemChunks->end(); ++l_iter )
  {
    std::string l_currentLine("size: ");
    l_currentLine += std::to_string( l_iter->first );
    l_currentLine += ", count: ";
    l_currentLine += std::to_string( l_iter->second.size() );
    l_currentLine += "\n";
    
    l_toReturn += l_currentLine;
  }
  return l_toReturn;
}

































void VulkanMemory::CreateCommandPool() {
  
  VkCommandPoolCreateInfo poolInfo = {};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.queueFamilyIndex = m_graphicsFamilyIndex;

  if (vkCreateCommandPool(m_logicalDevice, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create graphics command pool!");
  }
}
  
VkCommandBuffer VulkanMemory::BeginSingleTimeCommands()
{
  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = m_commandPool;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(m_logicalDevice, &allocInfo, &commandBuffer);

  VkCommandBufferBeginInfo beginInfo = {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(commandBuffer, &beginInfo);

  return commandBuffer;
}


void VulkanMemory::EndSingleTimeCommands(VkCommandBuffer a_commandBuffer)
{
  vkEndCommandBuffer(a_commandBuffer);

  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &a_commandBuffer;

  vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(m_graphicsQueue);

  vkFreeCommandBuffers(m_logicalDevice, m_commandPool, 1, &a_commandBuffer);
}

bool VulkanMemory::HasStencilComponent(VkFormat format) { return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT; }

void VulkanMemory::CreateStagingBufferMemPool(const VkDeviceSize& a_sizeInBytes)
{
  m_memoryPools[0] = std::make_shared<VulkanMemoryPool>( m_physicalDevice, m_logicalDevice, a_sizeInBytes, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  m_memoryPools[0]->Init();
}

void VulkanMemory::CreateVertexIndexBufferMemPool(const VkDeviceSize& a_sizeInBytes)
{
   m_memoryPools[1] = std::make_shared<VulkanMemoryPool>( m_physicalDevice, m_logicalDevice, a_sizeInBytes,VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
   m_memoryPools[1]->Init();
}

void VulkanMemory::CreateIndexBufferMemPool(const VkDeviceSize& a_sizeInBytes)
{
   m_memoryPools[2] = std::make_shared<VulkanMemoryPool>( m_physicalDevice, m_logicalDevice, a_sizeInBytes, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
   m_memoryPools[2]->Init();
}

void VulkanMemory::CreateUniformBufferMemPool(const VkDeviceSize& a_sizeInBytes)
{
   m_memoryPools[3] = std::make_shared<VulkanMemoryPool>( m_physicalDevice, m_logicalDevice, a_sizeInBytes, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );
   m_memoryPools[3]->Init();
}

void VulkanMemory::CreateDynamicUniformBufferMemPool(const VkDeviceSize& a_sizeInBytes)
{
  VkPhysicalDeviceProperties l_props;
  vkGetPhysicalDeviceProperties(m_physicalDevice, &l_props);
  l_props.limits.minUniformBufferOffsetAlignment
  
  VkDeviceSize l_alignedSize = a_sizeInBytes + (l_props.limits.minUniformBufferOffsetAlignment - a_sizeInBytes%l_props.limits.minUniformBufferOffsetAlignment);
   
  m_memoryPools[4] = std::make_shared<VulkanMemoryPool>( m_physicalDevice, m_logicalDevice, l_alignedSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, l_props.limits.minUniformBufferOffsetAlignment );
  m_memoryPools[4]->Init();
}


void VulkanMemory::CreateImageMemPools(const VkDeviceSize& a_shaderImagesSize, const VkDeviceSize& a_colourAttachmentsSize, const VkDeviceSize& a_downloadingColourAttachmentsSize, const VkDeviceSize& a_depthStencilAttachmentsSize  )
{
  m_imageMemoryPools[0] = std::make_shared<VulkanImageMemoryPool>( m_physicalDevice, m_logicalDevice, a_shaderImagesSize, 
   VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  m_imageMemoryPools[0]->Init();

  m_imageMemoryPools[1] = std::make_shared<VulkanImageMemoryPool>( m_physicalDevice, m_logicalDevice, a_colourAttachmentsSize, 
   VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT  | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  m_imageMemoryPools[1]->Init();

  m_imageMemoryPools[2] = std::make_shared<VulkanImageMemoryPool>( m_physicalDevice, m_logicalDevice, a_downloadingColourAttachmentsSize, 
   VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, 
   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  m_imageMemoryPools[2]->Init();  
   
  m_imageMemoryPools[3] = std::make_shared<VulkanImageMemoryPool>( m_physicalDevice, m_logicalDevice, a_depthStencilAttachmentsSize, 
   VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  m_imageMemoryPools[3]->Init();
}

size_t VulkanMemory::SizeOfPixel(VkFormat a_format)
{
  if( a_format >= 9 && a_format <= 15)
    return 1;
  
  if( a_format >= 16 && a_format <= 22)
    return 2;
  if( a_format >= 23 && a_format <= 36)
    return 3;
  if( a_format >= 37 && a_format <= 69)
    return 4;
  if( a_format >= 70 && a_format <= 76)
    return 2;
  if( a_format >= 77 && a_format <= 83)
    return 4;
  if( a_format >= 83 && a_format <= 90)
    return 6;
  if( a_format >= 91 && a_format <= 97)
    return 8;

  if( a_format >= 98 && a_format <= 100)
    return 4;
  if( a_format >= 101 && a_format <= 103)
    return 8;
  if( a_format >= 104 && a_format <= 106)
    return 12;
  if( a_format >= 107 && a_format <= 109)
    return 16;

  else return 3;
}

VulkanMemory::VulkanMemory(const VkPhysicalDevice& a_physicalDevice, const VkDevice& a_logicalDevice, const VkQueue& a_graphicsQueue, int a_graphicsFamilyIndex)
  : m_physicalDevice(a_physicalDevice), m_logicalDevice(a_logicalDevice), m_graphicsFamilyIndex(a_graphicsFamilyIndex), m_graphicsQueue(a_graphicsQueue),
    m_memoryPools( { {NULL, NULL} } )
{
  CreateCommandPool();
}

bool VulkanMemory::Init(const VkDeviceSize& a_stagingMemorySize, const VkDeviceSize& a_vertexMemorySize, const VkDeviceSize& a_indexMemorySize, 
                        const VkDeviceSize& a_uniformBufferMemorySize, const VkDeviceSize& a_mixBufferMemorySize,
                        const VkDeviceSize& a_shaderImagesSize, const VkDeviceSize& a_colourAttachmentsSize, const VkDeviceSize& a_downloadingColourAttachmentsSize,
                        const VkDeviceSize& a_depthStencilAttachmentsSize )
{
  CreateStagingBufferMemPool(a_stagingMemorySize);
  CreateVertexIndexBufferMemPool(a_vertexMemorySize);
  CreateIndexBufferMemPool(a_indexMemorySize);
  CreateUniformBufferMemPool(a_uniformBufferMemorySize);
  CreateDynamicUniformBufferMemPool(a_mixBufferMemorySize);
  
  CreateImageMemPools(a_shaderImagesSize, a_colourAttachmentsSize, a_downloadingColourAttachmentsSize, a_depthStencilAttachmentsSize);
  return true;
}



void VulkanMemory::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
  VkBufferCreateInfo bufferInfo = {};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(m_logicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to create buffer!");
  }

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(m_logicalDevice, buffer, &memRequirements);

  VkMemoryAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

  if (vkAllocateMemory(m_logicalDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate buffer memory!");
  }

  vkBindBufferMemory(m_logicalDevice, buffer, bufferMemory, 0);
}


void VulkanMemory::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
  VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

  VkBufferCopy copyRegion = {};
  copyRegion.size = size;
  vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

  EndSingleTimeCommands(commandBuffer);
}


void VulkanMemory::CopyBuffer(std::shared_ptr< VulkanMemoryChunk > a_srcBuffer, std::shared_ptr< VulkanMemoryChunk > a_dstBuffer)
{
  VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

  VkBufferCopy l_copyRegion = {};
  l_copyRegion.size = a_srcBuffer->m_size;
  l_copyRegion.srcOffset = a_srcBuffer->GetBufferOffset();
  l_copyRegion.dstOffset = a_dstBuffer->GetBufferOffset();
  vkCmdCopyBuffer(commandBuffer, a_srcBuffer->m_buffer->m_buffer, a_dstBuffer->m_buffer->m_buffer, 1, &l_copyRegion);

  EndSingleTimeCommands(commandBuffer);
}

void VulkanMemory::CopyBufferToImage(std::shared_ptr< VulkanMemoryChunk > a_srcBuffer, std::shared_ptr< VulkanImageMemoryChunk > a_dstImage)
{
  std::vector<VkBufferImageCopy> l_bufferCopyRegions;
  unsigned int l_counter = a_dstImage->m_imageViewType == VK_IMAGE_VIEW_TYPE_CUBE ? 6 : 1;
  size_t l_buffOffset = a_srcBuffer->GetBufferOffset();
  size_t l_singleFaceSize = SizeOfPixel(a_dstImage->m_format)*a_dstImage->m_width*a_dstImage->m_height;
  VkCommandBuffer commandBuffer = BeginSingleTimeCommands();
  
  for( unsigned int l_face = 0; l_face < l_counter; ++l_face)
  {
    VkBufferImageCopy region = {};
    region.bufferOffset = l_buffOffset;
    region.bufferRowLength = a_dstImage->m_width * SizeOfPixel(a_dstImage->m_format);
    region.bufferImageHeight = a_dstImage->m_height / l_counter;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = l_face;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
        (uint32_t)a_dstImage->m_width,
        (uint32_t)a_dstImage->m_height/l_counter,
        1
    };
    l_bufferCopyRegions.push_back(region);
    // update buffer offset
    l_buffOffset += l_singleFaceSize;
  }
  TransitionImageLayout(commandBuffer, a_dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

  vkCmdCopyBufferToImage(
    commandBuffer,
    a_srcBuffer->m_buffer->m_buffer,
    a_dstImage->m_image,
    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    l_bufferCopyRegions.size(),
    &l_bufferCopyRegions[0]
  );
  
  EndSingleTimeCommands(commandBuffer);
}

std::shared_ptr<VulkanMemoryChunk> VulkanMemory::CreateVertexBuffer(char* a_vertexDataArray, const int& a_sizeInBytes)
{
  // allocate memory with appropriate buffers
  std::shared_ptr< VulkanMemoryChunk> l_stagingMemory = m_memoryPools[0]->AllocateMemory(a_sizeInBytes);
  std::shared_ptr< VulkanMemoryChunk> l_vertexBuffMemory = m_memoryPools[1]->AllocateMemory(a_sizeInBytes);
  
  if( !l_stagingMemory )
    throw std::runtime_error("VulkanMemory::CreateVertexBuffer - Staging Memory Pool could not allocate memory");
  if(!l_vertexBuffMemory )
    throw std::runtime_error("VulkanMemory::CreateVertexBuffer - Vertex Memory Pool could not allocate memory");
  
  void* data;
  vkMapMemory(m_logicalDevice, l_stagingMemory->m_memorySpace, l_stagingMemory->m_offset, l_stagingMemory->m_size, 0, &data);
      memcpy(data, a_vertexDataArray, (size_t) l_stagingMemory->m_size);
  vkUnmapMemory(m_logicalDevice, l_stagingMemory->m_memorySpace);

  
  // copy staging buffer to index buffer memory
  CopyBuffer(l_stagingMemory, l_vertexBuffMemory);
  // free staging memory chunk
  m_memoryPools[0]->DeallocateMemory(l_stagingMemory);
  // return l_indexBuffMemory

  return l_vertexBuffMemory;
}

std::shared_ptr<VulkanMemoryChunk> VulkanMemory::CreateIndexBuffer(char* a_indexDataArray, const int& a_sizeInBytes)
{
  // allocate memory with appropriate buffers
  std::shared_ptr< VulkanMemoryChunk> l_stagingMemory = m_memoryPools[0]->AllocateMemory(a_sizeInBytes);
  std::shared_ptr< VulkanMemoryChunk> l_indexBuffMemory = m_memoryPools[1]->AllocateMemory(a_sizeInBytes);
  // check is actually allocated
  if( !l_stagingMemory )
    throw std::runtime_error("VulkanMemory::CreateIndexBuffer - Staging Memory Pool could not allocate memory");
  if(!l_indexBuffMemory )
    throw std::runtime_error("VulkanMemory::CreateIndexBuffer - Index Memory Pool could not allocate memory");
  
  // copy data to staging buffer
  void* data;
  vkMapMemory(m_logicalDevice, l_stagingMemory->m_memorySpace, l_stagingMemory->GetMemoryOffset(), l_stagingMemory->m_size, 0, &data);
      memcpy(data, a_indexDataArray, (size_t)a_sizeInBytes);
  vkUnmapMemory(m_logicalDevice, l_stagingMemory->m_memorySpace);
  
  // copy staging buffer to index buffer memory
  CopyBuffer(l_stagingMemory, l_indexBuffMemory);
  // free staging memory chunk
  m_memoryPools[0]->DeallocateMemory(l_stagingMemory);
  // return l_indexBuffMemory
  return l_indexBuffMemory;
}



std::shared_ptr<VulkanMemoryChunk> VulkanMemory::CreateUniformBuffer(const int& a_sizeInBytes)
{
  std::shared_ptr< VulkanMemoryChunk> l_uniformBuffMemory = m_memoryPools[3]->AllocateMemory(a_sizeInBytes);
  if( !l_uniformBuffMemory )
    throw std::runtime_error("VulkanMemory::CreateUniformBuffer - Uniform Memory Pool could not allocate memory");
  
  return l_uniformBuffMemory;
}



std::shared_ptr<VulkanImageMemoryChunk> VulkanMemory::CreateMaterialTexture(char* a_data, const uint32_t& a_width, const uint32_t& a_height, VkFormat a_format )
{
  // allocate image memory first so we can know how much memory we need
  std::shared_ptr< VulkanImageMemoryChunk> l_imageMemory = m_imageMemoryPools[0]->AllocateMemory(a_width, a_height, a_format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_VIEW_TYPE_2D);
  
  if(!l_imageMemory )
    throw std::runtime_error("VulkanMemory::CreateMaterialTexture - Image Memory Pool could not allocate memory");

  
  if( UpdateTextureData(l_imageMemory, a_data, a_width, a_height, a_format) )
  {
    CreateImageView(l_imageMemory);
    return l_imageMemory;
  }
  return nullptr;
}

bool VulkanMemory::UpdateTextureData(std::shared_ptr<VulkanImageMemoryChunk> a_memoryChunk, char* a_data, const uint32_t& a_width, const uint32_t& a_height, VkFormat a_format )
{
  if( !a_memoryChunk )
    return false;
  if( a_memoryChunk->m_width != a_width || a_memoryChunk->m_height != a_height || a_memoryChunk->m_format != a_format )
    return false;
  
  std::shared_ptr< VulkanMemoryChunk> l_stagingMemory = m_memoryPools[0]->AllocateMemory(a_memoryChunk->m_size);
  // check is actually allocated
  if( !l_stagingMemory )
    throw std::runtime_error("VulkanMemory::UpdateTextureData - Staging Memory Pool could not allocate memory");
  
  // copy data to staging buffer
  void* data;
  vkMapMemory(m_logicalDevice, l_stagingMemory->m_memorySpace, l_stagingMemory->GetMemoryOffset(), l_stagingMemory->m_size, 0, &data);
      memcpy(data, a_data, (size_t)( SizeOfPixel(a_format)*a_width*a_height) );
  vkUnmapMemory(m_logicalDevice, l_stagingMemory->m_memorySpace);
  
  
  // copy buffer to image
  CopyBufferToImage(l_stagingMemory, a_memoryChunk);
  // free staging memory chunk
  m_memoryPools[0]->DeallocateMemory(l_stagingMemory);
  TransitionImageLayout(a_memoryChunk, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
  return true;
}

std::shared_ptr<VulkanImageMemoryChunk> VulkanMemory::CreateCubemap(char* a_data1, char* a_data2, char* a_data3, char* a_data4, char* a_data5, char* a_data6,
                                                                    const uint32_t& a_width, const uint32_t& a_height, VkFormat a_format )
{
  // allocate image memory first so we can know how much memory we need
  std::shared_ptr< VulkanImageMemoryChunk> l_imageMemory = m_imageMemoryPools[0]->AllocateMemory(a_width, a_height*6, a_format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_VIEW_TYPE_CUBE);
  if(!l_imageMemory )
    throw std::runtime_error("VulkanMemory::CreateCubemap - Image Memory Pool could not allocate memory");
  
  std::shared_ptr< VulkanMemoryChunk> l_stagingMemory = m_memoryPools[0]->AllocateMemory(l_imageMemory->m_size);
  // check is actually allocated
  if( !l_stagingMemory )
    throw std::runtime_error("VulkanMemory::CreateCubemap - Staging Memory Pool could not allocate memory");
  
  // copy data to staging buffer
  size_t l_sizeOfImage = SizeOfPixel(a_format)*a_width*a_height;
  void* data;
  vkMapMemory(m_logicalDevice, l_stagingMemory->m_memorySpace, l_stagingMemory->GetMemoryOffset(), l_stagingMemory->m_size, 0, &data);
    memcpy(data, a_data1, l_sizeOfImage );  data = ((char *) data) + l_sizeOfImage;
    memcpy(data, a_data2, l_sizeOfImage );  data = ((char *) data) + l_sizeOfImage;
    memcpy(data, a_data3, l_sizeOfImage );  data = ((char *) data) + l_sizeOfImage;
    memcpy(data, a_data4, l_sizeOfImage );  data = ((char *) data) + l_sizeOfImage;
    memcpy(data, a_data5, l_sizeOfImage );  data = ((char *) data) + l_sizeOfImage;
    memcpy(data, a_data6, l_sizeOfImage );  data = ((char *) data) + l_sizeOfImage;
  vkUnmapMemory(m_logicalDevice, l_stagingMemory->m_memorySpace);
  
  // copy buffer to image
  CopyBufferToImage(l_stagingMemory, l_imageMemory);
  // free staging memory chunk
  m_memoryPools[0]->DeallocateMemory(l_stagingMemory);
  CreateImageView(l_imageMemory);  
  TransitionImageLayout(l_imageMemory, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
  return l_imageMemory;
}


std::shared_ptr<VulkanImageMemoryChunk> VulkanMemory::CreateAttachmentTexture(const uint32_t& a_width, const uint32_t& a_height, VkFormat a_format )
{
  std::shared_ptr< VulkanImageMemoryChunk> l_imageMemory = m_imageMemoryPools[1]->AllocateMemory(a_width, a_height, a_format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_VIEW_TYPE_2D);
  if(!l_imageMemory )
    throw std::runtime_error("VulkanMemory::CreateAttachmentTexture - Image Memory Pool could not allocate memory");
  CreateImageView(l_imageMemory);
  TransitionImageLayout(l_imageMemory, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL );
  return l_imageMemory;
}
std::shared_ptr<VulkanImageMemoryChunk> VulkanMemory::CreateAttachmentToDownloadTexture(const uint32_t& a_width, const uint32_t& a_height, VkFormat a_format )
{
  std::shared_ptr< VulkanImageMemoryChunk> l_imageMemory = m_imageMemoryPools[2]->AllocateMemory(a_width, a_height, a_format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_VIEW_TYPE_2D);
  if(!l_imageMemory )
    throw std::runtime_error("VulkanMemory::CreateAttachmentToDownloadTexture - Image Memory Pool could not allocate memory");
  CreateImageView(l_imageMemory);
  
  TransitionImageLayout(l_imageMemory, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL );
  
  return l_imageMemory;
}

std::shared_ptr<VulkanImageMemoryChunk> VulkanMemory::CreateStencilDepthAttachmentTexture(const uint32_t& a_width, const uint32_t& a_height )
{
  VkFormat l_selectedFormat = VK_FORMAT_UNDEFINED;
  VkFormatProperties props;
  vkGetPhysicalDeviceFormatProperties(m_physicalDevice, VK_FORMAT_D32_SFLOAT_S8_UINT, &props);
  
  if( props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT )
    l_selectedFormat = VK_FORMAT_D32_SFLOAT_S8_UINT;
  else
  {
    vkGetPhysicalDeviceFormatProperties(m_physicalDevice, VK_FORMAT_D24_UNORM_S8_UINT, &props);
    if( props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT )
      l_selectedFormat = VK_FORMAT_D24_UNORM_S8_UINT;
    else
      std::runtime_error("VulkanMemory::CreateStencilDepthAttachmentTexture - failed to find supported format for depth stencil texture!");
  }
  
  
  std::shared_ptr< VulkanImageMemoryChunk> l_imageMemory = m_imageMemoryPools[3]->AllocateMemory(a_width, a_height, l_selectedFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_VIEW_TYPE_2D);
  if(!l_imageMemory )
    throw std::runtime_error("VulkanMemory::CreateAttachmentTexture - Image Memory Pool could not allocate memory");
  CreateImageView(l_imageMemory, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT  );
  
  TransitionImageLayout(l_imageMemory, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
  
  return l_imageMemory;
  
}

void VulkanMemory::CreateImageView(std::shared_ptr<VulkanImageMemoryChunk> a_memoryChunk, VkImageAspectFlags aspectFlags)
{
  VkImageViewCreateInfo viewInfo = {};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = a_memoryChunk->m_image;
  viewInfo.viewType = a_memoryChunk->m_imageViewType;
  viewInfo.format = a_memoryChunk->m_format;
  viewInfo.subresourceRange.aspectMask = aspectFlags;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = 1;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  if (vkCreateImageView(m_logicalDevice, &viewInfo, nullptr, &(a_memoryChunk->m_imageView)) != VK_SUCCESS)
    throw std::runtime_error("VulkanMemory::CreateImageView - failed to create texture image view!");
  
}

uint32_t VulkanMemory::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) 
    if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
      return i;
    
  
  throw std::runtime_error("failed to find suitable memory type!");
}

// void VulkanMemory::TransitionImageLayout(VkCommandBuffer a_cmdBuffer, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
void VulkanMemory::TransitionImageLayout(VkCommandBuffer a_cmdBuffer, std::shared_ptr<VulkanImageMemoryChunk> a_imageMemChunk, VkImageLayout newLayout)
{

  if(a_imageMemChunk->m_layout == newLayout )
    return;
  VkImageMemoryBarrier barrier = {};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = a_imageMemChunk->m_layout;
  barrier.newLayout = newLayout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = a_imageMemChunk->m_image;

  if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) 
  {
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

    if (HasStencilComponent(a_imageMemChunk->m_format))
      barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
      
  } 
  else 
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;
  
  VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
  VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
  
  // undefined image to load texture
  if (a_imageMemChunk->m_layout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } 
  // after uploading data we use that to upload it to textures
  else if (a_imageMemChunk->m_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) 
  {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    
    sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } 
  // for texture that was being used in shader to update
  else if (a_imageMemChunk->m_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
  {
    barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    
    sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  }
  // for stencil depth attachment transition
  else if (a_imageMemChunk->m_layout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) 
  {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    
    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  }
  else if (a_imageMemChunk->m_layout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) 
  {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT  | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT ;
    
    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  } 
  else 
  {

      throw std::runtime_error("unsupported layout transition!");
  }
  vkCmdPipelineBarrier(
      a_cmdBuffer,
      sourceStage, destinationStage,
      0,
      0, nullptr,
      0, nullptr,
      1, &barrier
  );
  a_imageMemChunk->m_layout = newLayout;
  
}
void VulkanMemory::TransitionImageLayout(std::shared_ptr<VulkanImageMemoryChunk> a_imageMemChunk, VkImageLayout newLayout) 
{
  VkCommandBuffer commandBuffer = BeginSingleTimeCommands();
  
  TransitionImageLayout(commandBuffer, a_imageMemChunk, newLayout);
  
  EndSingleTimeCommands(commandBuffer);
  
}

void VulkanMemory::Clear(){ vkDestroyCommandPool(m_logicalDevice, m_commandPool, nullptr); }

