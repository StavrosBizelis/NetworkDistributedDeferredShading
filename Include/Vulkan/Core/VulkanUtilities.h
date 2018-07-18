#include <Vulkan/Vulkan.hpp>


VkShaderModule CreateShaderModule(const std::vector<char>& a_code, const VkDevice& a_device);

std::vector<VkPipelineShaderStageCreateInfo> CreatePipelineShaderCreateInfo(VkShaderModule a_vertShaderModule, VkShaderModule a_fragShaderModule);

VkDescriptorSetLayoutBinding GetUniformVertexLayoutBinding(const unsigned int& a_bindingNum, const unsigned int& a_count);
VkDescriptorSetLayoutBinding GetUniformFragmentLayoutBinding(const unsigned int& a_bindingNum, const unsigned int& a_count);
VkDescriptorSetLayoutBinding GetSamplerFragmentLayoutBinding(const unsigned int& a_bindingNum);
VkDescriptorSetLayoutBinding GetInputAttachmentFragmentLayoutBinding(const unsigned int& a_bindingNum);

VkDescriptorSetLayout CreateDescriptorSetLayout(VkDevice a_device, const std::vector<VkDescriptorSetLayoutBinding>& a_input);