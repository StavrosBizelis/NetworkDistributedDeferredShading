#ifndef VULKAN_VALIDATION_LAYER_ENABLER
#define VULKAN_VALIDATION_LAYER_ENABLER


#include <vector>

#ifdef NDEBUG
const bool g_enableValidationLayers = false;
#else
const bool g_enableValidationLayers = true;
#endif

const std::vector<const char*> g_validationLayers = {
  "VK_LAYER_LUNARG_standard_validation"
  };
#endif