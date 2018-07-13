#include <Vulkan/Core/VulkanMemory.h>

void VulkanMemoryPool::RemoveFromTheAvailableChunks(const VkDeviceSize& a_size, const std::shared_ptr< VulkanMemoryChunk>& a_chunk)
{
  std::map<VkDeviceSize, std::list< std::weak_ptr< VulkanMemoryChunk> > >::iterator l_iter = m_availableMemChunks->find(a_size);
  if( l_iter != m_availableMemChunks->end() )
  {

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
: m_physicalDevice(a_physicalDevice), m_logicalDevice(a_logicalDevice), m_size(a_size), m_usage(a_usage), m_properties(a_properties), m_memorySpace(nullptr)
  m_memChunks(NULL), m_availableMemChunks( std::make_shared< std::map<VkDeviceSize, std::list< std::weak_ptr< VulkanMemoryChunk> > > >() ) {}

VulkanMemoryPool::~VulkanMemoryPool()
{
  if( m_memorySpace )
    vkFreeMemory(m_logicalDevice, m_memorySpace, nullptr);
}

void VulkanMemoryPool::Init()
{
  // dummy buffer for creation
  
  VkBufferCreateInfo bufferInfo = {};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = m_size;
  bufferInfo.usage = m_usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VkBuffer l_buffer;
  if (vkCreateBuffer(m_logicalDevice, &bufferInfo, nullptr, &l_buffer) != VK_SUCCESS) {
    throw std::runtime_error("VulkanMemoryPool::Init() - failed to create dummy buffer for memory pool initialization!");
  }

  VkMemoryRequirements l_memRequirements;
  vkGetBufferMemoryRequirements(m_logicalDevice, l_buffer, &l_memRequirements);
  
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
    m_memorySpace = nullptr;
    throw std::runtime_error("VulkanMemoryPool::Init() - failed to allocate memory!");
  }
  
  // destroy dummy buffer
  vkDestroyBuffer(m_logicalDevice, l_buffer, nullptr);  
  
  // after the memory is allocated create ONE big chunk that fills all the memory and is available to use
  m_memChunks = std::make_shared< VulkanMemoryChunk>( std::move(VulkanMemoryChunk(m_logicalDevice, m_memorySpace, VkDeviceSize(0) , m_size ) ) );
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
                std::make_shared< VulkanMemoryChunk>( std::move( VulkanMemoryChunk(m_logicalDevice, m_memorySpace, l_freeChunk->m_offset, a_size ) ) );
                
                
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
    // if m_prev is available - make the two chunks one
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
  
  
  
  
  // if m_next is also available - make the two chunks one
  std::shared_ptr< VulkanMemoryChunk> l_nextChunk = a_chunk->m_next;
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








