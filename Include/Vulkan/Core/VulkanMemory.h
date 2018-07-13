#pragma once 

#include <vulkan/vulkan.hpp>
#include <string>
#include <list>
#include <map>
#include <memory> // shared_ptr, unique_ptr

class VulkanMemoryPool;
class VulkanMemoryChunk;
/**
*   structure that holds the buffer handle and a memory handle and cleans them up on destruction
*/
// struct VulkanMemoryBuffer
// {
  // protected:
  // std::shared_ptr<VulkanMemoryPool> m_allocator;  ///< the memory pool in which the memory chunk(m_memory) was allocated - on memory buffer destructor - free it
  
  // public:
  // std::string m_type;
  // VkBuffer m_buffer;
  // std::shared_ptr< VulkanMemoryChunk> m_memory;
  // // VkDeviceMemory m_bufferMemory;
  
  // VulkanMemoryBuffer(); ///< default constructor
  // VulkanMemoryBuffer(const std::shared_ptr<VulkanMemoryPool>& a_memPool, const std::shared_ptr<VulkanMemoryChunk> a_memoryChunk, const std::string& a_type = "");
  
  // VulkanMemoryBuffer(const VulkanMemoryBuffer&) = default; ///< copy constructor
  // VulkanMemoryBuffer& operator=(const VulkanMemoryBuffer&) = default; ///< copy assignment operator
  
  // VulkanMemoryBuffer(VulkanMemoryBuffer&&) = default; ///< move constructor
  // VulkanMemoryBuffer& operator=(VulkanMemoryBuffer&&) = default; ///< move assignment operator
  // virtual ~VulkanMemoryBuffer();
// };











struct VulkanMemorySubBuffer
{
  VkBuffer m_buffer;
  VkDeviceSize m_memoryOffset;  ///< memory offset
  VkDeviceSize m_memorySize;  ///< memory size -  range is [m_offset, m_offset+m_size]
  
  VulkanMemorySubBuffer();
  ~VulkanMemorySubBuffer();
};

struct VulkanMemoryChunk
{
  VulkanMemoryPool* m_allocator;
  
  VkDevice m_logicalDevice;
  VkDeviceMemory m_memorySpace; ///< preallocated memory to use - allocated in VulkanMemoryPool::Init(), controlled solely by VulkanMemoryPool
  
  
  VkDeviceSize m_offset;  ///< the offset of this chunk in memory space, in bytes
  VkDeviceSize m_size;  ///< size of allocation chunk in bytes

  bool m_available;   ///< true if available to use, false if currently in use
  
  std::shared_ptr< VulkanMemorySubBuffer > m_buffer;
  
  std::shared_ptr< VulkanMemoryChunk > m_prev; ///< optional - previous memory chunk
  std::shared_ptr< VulkanMemoryChunk > m_next; ///< optional - next memory chunk
  
  VulkanMemoryChunk(VulkanMemoryPool* a_allocator, const VkDevice& a_logicalDevice, const VkDeviceMemory& a_memorySpace, const VkDeviceSize& a_offset, const VkDeviceSize& a_size );
  
  VulkanMemoryChunk(const VulkanMemoryChunk& a_other);  ///< copy constructor
  VulkanMemoryChunk& operator=(const VulkanMemoryChunk& a_other);  ///< copy assignment operator
  

  VkDeviceSize GetMemoryOffset(){return m_offset;}
  VkDeviceSize GetBufferOffset(){return m_offset - m_buffer->m_memoryOffset;}
  void Free();  ///< frees this memory chunk - object invalidated after Free()
};
  
  
class VulkanMemoryPool
{
  private:
  VkDevice m_logicalDevice;
  VkPhysicalDevice m_physicalDevice;
  
  
  VkDeviceSize m_size;    ///< physically preallocated memory size
  VkDeviceMemory m_memorySpace; ///< preallocated memory to use - allocated in VulkanMemoryPool::Init()
  
  std::shared_ptr< VulkanMemorySubBuffer > m_buffer; ///< buffer for this piece of memory - FUTURE WORK : create more if needed
  
  std::shared_ptr< VulkanMemoryChunk> m_memChunks; ///< first of the memory chunks list, sum of m_memChunks m_size must be equal to VulkanMemoryPool::m_size

  std::shared_ptr< 
      std::map<
            VkDeviceSize, std::list< std::weak_ptr< VulkanMemoryChunk> > 
        > 
    > m_availableMemChunks;   ///< the memory chunks that are available - are sorted by size

  VkMemoryPropertyFlags m_properties;    ///< flags for the type of memory that is allocated in this pool
  VkBufferUsageFlags m_usage;   ///< flags for the type of buffer use
  
  
  // helpers
  void RemoveFromTheAvailableChunks(const VkDeviceSize& a_size, const std::shared_ptr< VulkanMemoryChunk>& a_chunk);
  void AddToTheAvailableChunks(const VkDeviceSize& a_size, const std::shared_ptr< VulkanMemoryChunk>& a_chunk);
  
  public:
  /**
  *   @param a_size - will try to allocate that many bytes in memory - if 0 then allocate as much as possible - IF a_size > available memory THEN a_size = available memory
  *   
  */
  VulkanMemoryPool(const VkPhysicalDevice& a_physicalDevice, const VkDevice& a_logicalDevice, const VkDeviceSize& a_size, const VkBufferUsageFlags& a_usage, const VkMemoryPropertyFlags& a_properties);
  ~VulkanMemoryPool();
  void Init();
  
  
  VkMemoryPropertyFlags GetPropertyFlags();
  VkBufferUsageFlags GetUsageFlags();
  VkDeviceSize GetMemorySpaceSize();
  
  bool Owns(const std::shared_ptr< VulkanMemoryChunk>& a_chunk);  ///< @return true if a_chunk exists inside this pool
  
  std::shared_ptr< VulkanMemoryChunk> AllocateMemory(const VkDeviceSize& a_size);   ///< allocate a chunk of memory, @param a_size must be < GetMemorySpaceSize()
  bool DeallocateMemory(const std::shared_ptr< VulkanMemoryChunk>& a_chunk );  ///< @return true on success, false if chunk does not exists
  bool DeallocateMemory(VulkanMemoryChunk* a_chunk );  ///< @return true on success, false if chunk does not exists - for use in VulkanMemoryChunk::Free()
  
  VkDevice GetLogicalDevice(){ return m_logicalDevice; }
  
  // string output functions
  std::string MemoryChunks();
  std::string AvailableMemoryChunks();
  
};


class VulkanMemory
{
  private:
  
  VkDevice m_logicalDevice;
  VkPhysicalDevice m_physicalDevice;
  
  int m_graphicsFamilyIndex;
  
  VkCommandPool m_commandPool;
  VkQueue m_graphicsQueue;
  
  // helpers
  void CreateCommandPool();
  
  VkCommandBuffer BeginSingleTimeCommands();
  void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
  
  bool HasStencilComponent(VkFormat format);
  
  // memory pools helpers
  void CreateStagingBufferMemPool(const VkDeviceSize& a_sizeInBytes);   ///< Create Memory Pool for data uploading to the gpu (staging) - VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  void CreateVertexIndexBufferMemPool(const VkDeviceSize& a_sizeInBytes);   ///< Create Memory Pool for data local to the gpu (vertex/index data etc) - VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
  void CreateIndexBufferMemPool(const VkDeviceSize& a_sizeInBytes);    ///< Create Memory Pool for data local to the gpu (vertex/index data etc) - VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
  void CreateUniformBufferMemPool(const VkDeviceSize& a_sizeInBytes);    ///< Create Memory Pool for data visible to the cpu (staging buffers, uniform buffers etc) - VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT 
  void CreateMixedBufferMemPool(const VkDeviceSize& a_sizeInBytes);    ///< Create Memory Pool for everything except staging - used for packed data
  
  
  /**
  *   [0] - StagingBufferMemoryPool
  *   [1] - VertexBufferMemoryPool
  *   [2] - IndexBufferMemoryPool
  *   [3] - UniformBufferMemoryPool
  *   [4] - MixBufferMemoryPool
  */
  std::array<std::shared_ptr<VulkanMemoryPool>, 5 > m_memoryPools;
  
  public:
  /**
  *   @param a_hostMemoryPoolSizeInBytes - size of the memory pool for data visible to the CPU
  *   @param a_localMemoryPoolSizeInBytes - size of the memory pool for data local to the GPU
  */
  VulkanMemory(const VkPhysicalDevice& a_physicalDevice, const VkDevice& a_logicalDevice, const VkQueue& a_graphicsQueue, int a_graphicsFamilyIndex); ///< default constructor
  ~VulkanMemory() = default;
  
  
  /**
  *   @param all: if 0 this memory type will not be allocated,
  *   @param a_stagingMemorySize: staging is not covered by mixBufferMemorySize
  */
  bool Init(const VkDeviceSize& a_stagingMemorySize, const VkDeviceSize& a_vertexMemorySize, const VkDeviceSize& a_indexMemorySize, const VkDeviceSize& a_uniformBufferMemorySize, const VkDeviceSize& a_mixBufferMemorySize);
  
  void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
  void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
  void CopyBuffer(std::shared_ptr< VulkanMemoryChunk > a_srcBuffer, std::shared_ptr< VulkanMemoryChunk > a_dstBuffer);
  
  /**
  *   if vertex data are in std:vector<T> a_tmp then a_vertexDataArray == a_tmp.data() and a_sizeInBytes = sizeof(a_tmp) * a_tmp.size()
  *   - also dont use pointers in T
  */
  std::shared_ptr<VulkanMemoryChunk> CreateVertexBuffer(char* a_vertexDataArray, const int& a_sizeInBytes );
  
  /**
  *   if index data are in std:vector<T> a_tmp then a_indexDataArray == a_tmp.data() and a_sizeInBytes = sizeof(a_tmp) * a_tmp.size()
  *   - also dont use pointers in T
  */
  std::shared_ptr<VulkanMemoryChunk> CreateIndexBuffer(char* a_indexDataArray, const int& a_sizeInBytes);
  
  
  std::pair< std::shared_ptr<VulkanMemoryChunk>, unsigned int> CreateUniformBuffer(const int& a_sizeInBytes);    ///< first is vulkan buffer, second is a_sizeInBytes
  
  void CopyBufferToImage();
  
  uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
  
  void TransitionImageLayout(VkCommandBuffer a_cmdBuffer, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);    ///< for use in commands recording
  void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout); ///< for stand alone use
  
  void Clear();
};
