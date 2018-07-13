#include <Vulkan/Core/VulkanMemory.h>

#include <vulkan/vulkan.hpp>

#include "math.h"

#include <stdexcept>
	
#include <iostream>



// VulkanMemoryBuffer::VulkanMemoryBuffer()
  // : m_buffer(NULL), m_memory(NULL), m_type("Invalid")
// {}

// VulkanMemoryBuffer::VulkanMemoryBuffer(const std::shared_ptr<VulkanMemoryPool>& a_memPool, const std::shared_ptr< VulkanMemoryChunk> a_memoryChunk, const std::string& a_type)
  // : m_buffer(NULL), m_memory(a_memoryChunk), m_allocator(a_memPool), m_type( a_type ) 
// {
  // VkMemoryPropertyFlags l_propertyFlags = m_allocator->GetPropertyFlags();
  // VkBufferUsageFlags l_usageFlags = m_allocator->GetUsageFlags();
  // VkDeviceSize l_size = m_memory->m_size;
  // VkDeviceSize l_offset = m_memory->m_offset;
  // VkDeviceMemory l_memorySpace = m_memory->m_memorySpace;
  
  // // create buffer here
  // VkBufferCreateInfo bufferInfo = {};
  // bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  // bufferInfo.size = l_size;
  // bufferInfo.usage = l_usageFlags;
  // bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  // if (vkCreateBuffer(m_logicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
    // throw std::runtime_error("failed to create buffer!");
  // }

  // vkBindBufferMemory(m_logicalDevice, m_buffer, l_memorySpace, l_offset);
// }

// VulkanMemoryBuffer::~VulkanMemoryBuffer()
// {
  // if( m_buffer )
    // vkDestroyBuffer(m_logicalDevice, m_buffer, nullptr);
  // if( m_allocator && m_memory)
    // m_allocator->DeallocateMemory(m_memory);
// }






VulkanMemorySubBuffer::VulkanMemorySubBuffer()
:m_buffer(0), m_memoryOffset(0), m_memorySize(0)
{}

VulkanMemorySubBuffer::~VulkanMemorySubBuffer()
{}




VulkanMemoryChunk::VulkanMemoryChunk(VulkanMemoryPool* a_allocator, const VkDevice& a_logicalDevice, const VkDeviceMemory& a_memorySpace, const VkDeviceSize& a_offset, const VkDeviceSize& a_size)
: m_available(true), m_allocator(m_allocator), m_logicalDevice (a_logicalDevice), m_memorySpace(a_memorySpace), m_offset(a_offset), m_size(a_size) {}

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

VulkanMemoryPool::VulkanMemoryPool(const VkPhysicalDevice& a_physicalDevice, const VkDevice& a_logicalDevice, const VkDeviceSize& a_size, const VkBufferUsageFlags& a_usage, const VkMemoryPropertyFlags& a_properties)
: m_physicalDevice(a_physicalDevice), m_logicalDevice(a_logicalDevice), m_size(a_size), m_usage(a_usage), m_properties(a_properties), m_memorySpace(NULL), m_buffer(nullptr),
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
  
  VkBuffer l_buffer = m_buffer->m_buffer;
  if (vkCreateBuffer(m_logicalDevice, &bufferInfo, nullptr, &l_buffer) != VK_SUCCESS) {
    throw std::runtime_error("VulkanMemoryPool::Init() - failed to create dummy buffer for memory pool initialization!");
  }

  VkMemoryRequirements l_memRequirements;
  vkGetBufferMemoryRequirements(m_logicalDevice, m_buffer->m_buffer, &l_memRequirements);
  
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

  uint32_t l_memoryIndex;
  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) 
    if ((l_memRequirements.memoryTypeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & m_properties) == m_properties)
    {
      l_memoryIndex = i;
      break;
    }
  
  
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
  // best fit algorithm
  // using the map of the available chunks find in a linear fashion te best fit instead of searching the whole memory space
  for( std::map<VkDeviceSize, std::list< std::weak_ptr< VulkanMemoryChunk> > >::iterator l_iter = m_availableMemChunks->begin(); l_iter != m_availableMemChunks->end(); ++l_iter )
  {
    if(l_iter->first >= a_size )
    {
      
      std::shared_ptr< VulkanMemoryChunk> l_freeChunk = l_iter->second.begin()->lock();
      
      if( l_freeChunk )
      { 
        // do allocation 
        // if a_size == l_freeChunk->m_size THEN return this chunk and remove it from the available chunks map
        if( a_size == l_freeChunk->m_size)
        {
          l_freeChunk->m_available = false;
          
          RemoveFromTheAvailableChunks(l_iter->first, l_freeChunk);
          return l_freeChunk;
        }
        else
        {
          
          // create a new memory chunk, of the appropriate size at the beginning of the free chunk
          std::shared_ptr< VulkanMemoryChunk> l_memoryChunk = 
                std::make_shared< VulkanMemoryChunk>( this, m_logicalDevice, m_memorySpace, l_freeChunk->m_offset, a_size );
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
          l_freeChunk->m_offset = l_freeChunk->m_offset + a_size;
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

void VulkanMemory::CreateMixedBufferMemPool(const VkDeviceSize& a_sizeInBytes)
{
   m_memoryPools[4] = std::make_shared<VulkanMemoryPool>( m_physicalDevice, m_logicalDevice, a_sizeInBytes, 
   VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
   m_memoryPools[4]->Init();
}




VulkanMemory::VulkanMemory(const VkPhysicalDevice& a_physicalDevice, const VkDevice& a_logicalDevice, const VkQueue& a_graphicsQueue, int a_graphicsFamilyIndex)
  : m_physicalDevice(a_physicalDevice), m_logicalDevice(a_logicalDevice), m_graphicsFamilyIndex(a_graphicsFamilyIndex), m_graphicsQueue(a_graphicsQueue),
    m_memoryPools( { {NULL, NULL} } )
{
  CreateCommandPool();
}

bool VulkanMemory::Init(const VkDeviceSize& a_stagingMemorySize, const VkDeviceSize& a_vertexMemorySize, const VkDeviceSize& a_indexMemorySize, const VkDeviceSize& a_uniformBufferMemorySize, const VkDeviceSize& a_mixBufferMemorySize )
{
  CreateStagingBufferMemPool(a_stagingMemorySize);
  CreateVertexIndexBufferMemPool(a_vertexMemorySize);
  CreateIndexBufferMemPool(a_indexMemorySize);
  CreateUniformBufferMemPool(a_uniformBufferMemorySize);
  try
  {
    CreateMixedBufferMemPool(a_mixBufferMemorySize);
  }
  catch(std::runtime_error&  a_e )
  {
    return false;
  }
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

void VulkanMemory::CopyBufferToImage(){}

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



std::pair< std::shared_ptr<VulkanMemoryChunk>, unsigned int> VulkanMemory::CreateUniformBuffer(const int& a_sizeInBytes)
{
  std::shared_ptr< VulkanMemoryChunk> l_uniformBuffMemory = m_memoryPools[3]->AllocateMemory(a_sizeInBytes);
  if( !l_uniformBuffMemory )
    throw std::runtime_error("VulkanMemory::CreateUniformBuffer - Uniform Memory Pool could not allocate memory");
  
  return std::pair< std::shared_ptr<VulkanMemoryChunk>, unsigned int>( l_uniformBuffMemory, a_sizeInBytes );
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

void VulkanMemory::TransitionImageLayout(VkCommandBuffer a_cmdBuffer, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
  VkImageMemoryBarrier barrier = {};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = oldLayout;
  barrier.newLayout = newLayout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = image;

  if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
      barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

      if (HasStencilComponent(format)) {
          barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
      }
  } else {
      barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  }

  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;

  if (oldLayout == VK_IMAGE_LAYOUT_PREINITIALIZED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
      barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
      barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
      barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
  } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
      barrier.srcAccessMask = 0;
      barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  } else {
      throw std::invalid_argument("unsupported layout transition!");
  }

  vkCmdPipelineBarrier(
      a_cmdBuffer,
      VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
      0,
      0, nullptr,
      0, nullptr,
      1, &barrier
  );
  
}
void VulkanMemory::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) 
{
  VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

  TransitionImageLayout( commandBuffer, image, format, oldLayout, newLayout);

  EndSingleTimeCommands(commandBuffer);
}

void VulkanMemory::Clear(){ vkDestroyCommandPool(m_logicalDevice, m_commandPool, nullptr); }

