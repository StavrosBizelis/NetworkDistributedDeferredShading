#pragma once 

#include <vulkan/vulkan.hpp>
#include <string>
#include <list>
#include <map>
#include <memory> // shared_ptr, unique_ptr

class VulkanMemoryPool;
class VulkanMemoryChunk;


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
  VkDeviceSize m_alignment;
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























enum VulkanSamplerOption
{
  VLK_SMPL_MIN_FLTR = 0,
  VLK_SMPL_MAG_FLTR = 1,
  VLK_SMPL_REPEAT = 2,
  VLK_SMPL_ANISOTROPY_LVL = 3
};

struct VulkanSampler 
{
  VkDevice m_logicalDevice;
  VkSampler m_sampler;
  
  std::map<VulkanSamplerOption, int> m_params;
  
  VulkanSampler( const VkDevice& a_logicalDevice );
  ~VulkanSampler();
  
  void SetSamplerObjectParameter(const VulkanSamplerOption a_parameter, const float& a_value);
  void Create();
};



class VulkanImageMemoryPool;
struct VulkanImageMemoryChunk
{
  VulkanImageMemoryPool* m_allocator;
  
  VkDevice m_logicalDevice;
  VkDeviceMemory m_memorySpace; ///< preallocated memory to use - allocated in VulkanImageMemoryPool::Init(), controlled solely by VulkanImageMemoryPool
  
  
  VkDeviceSize m_offset;  ///< the offset of this chunk in memory space, in bytes
  VkDeviceSize m_size;  ///< size of allocation chunk in bytes

  
  bool m_available;   ///< true if available to use, false if currently in use
  
  // image info
  VkImage m_image;
  VkDeviceSize m_width;
  VkDeviceSize m_height;
  VkFormat m_format;
  VkImageTiling m_tiling;
  VkImageLayout m_layout;
  
  VkImageView  m_imageView;
  VkImageViewType m_imageViewType;
  
  
  
  std::shared_ptr< VulkanImageMemoryChunk > m_prev; ///< optional - previous memory chunk
  std::shared_ptr< VulkanImageMemoryChunk > m_next; ///< optional - next memory chunk
  
  VulkanImageMemoryChunk(VulkanImageMemoryPool* a_allocator, const VkDevice& a_logicalDevice, const VkDeviceMemory& a_memorySpace, const VkDeviceSize& a_offset, const VkDeviceSize& a_size, 
                         const VkDeviceSize& a_width, const VkDeviceSize& a_height, VkFormat a_format, VkImageTiling a_tiling, VkImageViewType a_imageViewType);
  
  VkDeviceSize GetMemoryOffset(){return m_offset;}
  
  /// @brief sets/substitutes the image
  /// @brief if pass null the memory chuck will be deallocated and invalidated
  void SetImage(const VkImage& a_image, const VkImageLayout& a_layout);
  void Free();  ///< frees this memory chunk - object invalidated after Free()
};






class VulkanImageMemoryPool
{
  private:
  VkDevice m_logicalDevice;
  VkPhysicalDevice m_physicalDevice;
  
  
  VkDeviceSize m_size;    ///< physically preallocated memory size
  VkDeviceSize m_alignment;    ///< alignment size 
  VkDeviceMemory m_memorySpace; ///< preallocated memory to use - allocated in VulkanImageMemoryPool::Init()
  
  
  std::shared_ptr< VulkanImageMemoryChunk> m_memChunks; ///< first of the memory chunks list, sum of m_memChunks m_size must be equal to VulkanImageMemoryPool::m_size

  std::shared_ptr< 
      std::map<
            VkDeviceSize, std::list< std::weak_ptr< VulkanImageMemoryChunk> > 
        > 
    > m_availableMemChunks;   ///< the memory chunks that are available - are sorted by size

  VkMemoryPropertyFlags m_properties;    ///< flags for the type of memory that is allocated in this pool
  VkImageUsageFlags m_usage;   ///< flags for the type of buffer use
  
  
  // helpers
  void RemoveFromTheAvailableChunks(const VkDeviceSize& a_size, const std::shared_ptr< VulkanImageMemoryChunk>& a_chunk);
  void AddToTheAvailableChunks(const VkDeviceSize& a_size, const std::shared_ptr< VulkanImageMemoryChunk>& a_chunk);
  
  VkImage CreateImage(const uint32_t& a_width, const uint32_t& a_height, VkFormat a_format, VkImageTiling a_tiling, VkImageViewType a_imageViewType, VkDeviceSize& a_outSize);
  
  public:
  /**
  *   @param a_size - will try to allocate that many bytes in memory - if 0 then allocate as much as possible - IF a_size > available memory THEN a_size = available memory
  *   
  */
  VulkanImageMemoryPool(const VkPhysicalDevice& a_physicalDevice, const VkDevice& a_logicalDevice, const VkDeviceSize& a_size, const VkImageUsageFlags& a_usage, const VkMemoryPropertyFlags& a_properties);
  ~VulkanImageMemoryPool();
  void Init();
  
  
  VkMemoryPropertyFlags GetPropertyFlags();
  VkImageUsageFlags GetUsageFlags();
  VkDeviceSize GetMemorySpaceSize();
  
  bool Owns(const std::shared_ptr< VulkanImageMemoryChunk>& a_chunk);  ///< @return true if a_chunk exists inside this pool
  
  std::shared_ptr< VulkanImageMemoryChunk> AllocateMemory(const uint32_t& a_width, const uint32_t& a_height, VkFormat a_format, VkImageTiling a_tiling, VkImageViewType a_imageViewType);   ///< allocate a chunk of memory, @param a_size must be < GetMemorySpaceSize()
  bool DeallocateMemory(const std::shared_ptr< VulkanImageMemoryChunk>& a_chunk );  ///< @return true on success, false if chunk does not exists
  bool DeallocateMemory(VulkanImageMemoryChunk* a_chunk );  ///< @return true on success, false if chunk does not exists - for use in VulkanImageMemoryChunk::Free()
  
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
  
  /// create three image memory pools for 
  void CreateImageMemPools(const VkDeviceSize& a_shaderImagesSize, const VkDeviceSize& a_colourAttachmentsSize, const VkDeviceSize& a_downloadingColourAttachmentsSize, const VkDeviceSize& a_depthStencilAttachmentsSize );      
  
  /**
  *   [0] - StagingBufferMemoryPool
  *   [1] - VertexBufferMemoryPool
  *   [2] - IndexBufferMemoryPool
  *   [3] - UniformBufferMemoryPool
  *   [4] - MixBufferMemoryPool
  */
  std::array<std::shared_ptr<VulkanMemoryPool>, 5 > m_memoryPools;
  /**
  *   [0] - for use in shaders ( usage : VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT  )
  *   [1] - for framebuffer attachments which may be used through 
  *     ( usage : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT  | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT )
  *   [2] - for framebuffer attachments which be downloaded 
  *     ( usage : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT )
  *   [3] - for framebuffer depth stencil attachments 
  *     ( usage : VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT   | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT )
  */
  std::array<std::shared_ptr<VulkanImageMemoryPool>, 4 > m_imageMemoryPools;
  
  // utility helpers
  size_t SizeOfPixel(VkFormat a_format);
  
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
  bool Init(const VkDeviceSize& a_stagingMemorySize, const VkDeviceSize& a_vertexMemorySize, const VkDeviceSize& a_indexMemorySize, const VkDeviceSize& a_uniformBufferMemorySize, const VkDeviceSize& a_mixBufferMemorySize,
            const VkDeviceSize& a_shaderImagesSize, const VkDeviceSize& a_colourAttachmentsSize, const VkDeviceSize& a_downloadingColourAttachmentsSize, const VkDeviceSize& a_depthStencilAttachmentsSize );
  
  void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
  void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
  void CopyBuffer(std::shared_ptr< VulkanMemoryChunk > a_srcBuffer, std::shared_ptr< VulkanMemoryChunk > a_dstBuffer);
  void CopyBufferToImage(std::shared_ptr< VulkanMemoryChunk > a_srcBuffer, std::shared_ptr< VulkanImageMemoryChunk > a_dstImage);
  
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
  
  /// Create texture for use in shaders
  std::shared_ptr<VulkanImageMemoryChunk> CreateMaterialTexture(char* a_data, const uint32_t& a_width, const uint32_t& a_height, VkFormat a_format );
  bool UpdateTextureData(std::shared_ptr<VulkanImageMemoryChunk> a_memoryChunk, char* a_data, const uint32_t& a_width, const uint32_t& a_height, VkFormat a_format );
  /// all cubemap textures must be of the same size
  std::shared_ptr<VulkanImageMemoryChunk> CreateCubemap(char* a_data1, char* a_data2, char* a_data3, char* a_data4, char* a_data5, char* a_data6,
                                                        const uint32_t& a_width, const uint32_t& a_height, VkFormat a_format );
  
  /// @TODO IMPLEMENT
  /// @brief do not need "data" - just allocate and create VkImage and VkImageView for it
  std::shared_ptr<VulkanImageMemoryChunk> CreateAttachmentTexture(const uint32_t& a_width, const uint32_t& a_height, VkFormat a_format );
  std::shared_ptr<VulkanImageMemoryChunk> CreateAttachmentToDownloadTexture(const uint32_t& a_width, const uint32_t& a_height, VkFormat a_format );
  /// @TODO IMPLEMENT
  /// @brief do not need "data" - just allocate and create VkImage and VkImageView for it
  std::shared_ptr<VulkanImageMemoryChunk> CreateStencilDepthAttachmentTexture(const uint32_t& a_width, const uint32_t& a_height );
  
  
  void CreateImageView(std::shared_ptr<VulkanImageMemoryChunk> a_memoryChunk, VkImageAspectFlags a_aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);
  
  
  uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
  
  void TransitionImageLayout(VkCommandBuffer a_cmdBuffer, std::shared_ptr<VulkanImageMemoryChunk> a_imageMemChunk, VkImageLayout newLayout);    ///< for use in commands recording
  void TransitionImageLayout(std::shared_ptr<VulkanImageMemoryChunk> a_imageMemChunk, VkImageLayout newLayout); ///< for stand alone use
  
  void Clear();
};
