#ifndef VULKAN_VALIDATION_LAYER_ENABLER
#define VULKAN_VALIDATION_LAYER_ENABLER


#include <vector>

#ifdef DEBUG
const bool g_enableValidationLayers = true;
#else
const bool g_enableValidationLayers = false;
#endif

const std::vector<const char*> g_validationLayers = {
  "VK_LAYER_LUNARG_standard_validation"
  };
#endif