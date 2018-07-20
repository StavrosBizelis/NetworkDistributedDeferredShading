#include "Vulkan/Core/VulkanUtilities.h"

VkShaderModule CreateShaderModule(const std::vector<char>& a_code, const VkDevice& a_device)
{
  VkShaderModuleCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = a_code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t*>(a_code.data());

  VkShaderModule shaderModule;
  if (vkCreateShaderModule(a_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    throw std::runtime_error("failed to create shader module!");

  return shaderModule;
}

std::vector<VkPipelineShaderStageCreateInfo> CreatePipelineShaderCreateInfo(VkShaderModule a_vertShaderModule, VkShaderModule a_fragShaderModule)
{
  VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
  vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = a_vertShaderModule;
  vertShaderStageInfo.pName = "main";
  
  VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
  fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = a_fragShaderModule;
  fragShaderStageInfo.pName = "main";
  
  return {vertShaderStageInfo, fragShaderStageInfo};
}




VkDescriptorSetLayoutBinding GetUniformVertexLayoutBinding(const unsigned int& a_bindingNum)
{
  VkDescriptorSetLayoutBinding l_materialBinding = {};
  l_materialBinding.binding = a_bindingNum;
  l_materialBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  l_materialBinding.descriptorCount = 1;
  l_materialBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  l_materialBinding.pImmutableSamplers = NULL;
  return l_materialBinding;
}
VkDescriptorSetLayoutBinding GetUniformFragmentLayoutBinding(const unsigned int& a_bindingNum)
{
  VkDescriptorSetLayoutBinding l_materialBinding = {};
  l_materialBinding.binding = a_bindingNum;
  l_materialBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  l_materialBinding.descriptorCount = 1;
  l_materialBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  l_materialBinding.pImmutableSamplers = NULL;
  return l_materialBinding;
}
VkDescriptorSetLayoutBinding GetSamplerFragmentLayoutBinding(const unsigned int& a_bindingNum)
{
  VkDescriptorSetLayoutBinding l_materialBinding = {};
  l_materialBinding.binding = a_bindingNum;
  l_materialBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  l_materialBinding.descriptorCount = 1;
  l_materialBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  l_materialBinding.pImmutableSamplers = NULL;
  return l_materialBinding;
}

VkDescriptorSetLayoutBinding GetInputAttachmentFragmentLayoutBinding(const unsigned int& a_bindingNum)
{
  VkDescriptorSetLayoutBinding l_materialBinding = {};
  l_materialBinding.binding = a_bindingNum;
  l_materialBinding.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
  l_materialBinding.descriptorCount = 1;
  l_materialBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  l_materialBinding.pImmutableSamplers = NULL;
  return l_materialBinding;
}


VkDescriptorSetLayout CreateDescriptorSetLayout(VkDevice a_device,  const std::vector<VkDescriptorSetLayoutBinding>& a_bindings)
{
  VkDescriptorSetLayout l_toReturn;
  VkDescriptorSetLayoutCreateInfo l_descLayout = {};
  l_descLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  l_descLayout.pNext = NULL;
  l_descLayout.bindingCount = a_bindings.size();
  l_descLayout.pBindings = a_bindings.data();
  
  if ( vkCreateDescriptorSetLayout(a_device, &l_descLayout, NULL, &l_toReturn) != VK_SUCCESS )
    throw std::runtime_error("GetDescriptorSetLayout - failed to create descriptor set layout!");
  
  return l_toReturn;
}