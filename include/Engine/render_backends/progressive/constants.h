#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>

using std::vector;

const vector<const char*> vkVALIDATION_LAYERS = {
	"VK_LAYER_KHRONOS_validation"
};

const vector<const char*> vkDEVICE_EXTENSIONS = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
	VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
	VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME
};


#ifdef NDEBUG
const bool vkENABLE_VALIDATION_LAYERS = false;
#else
const bool vkENABLE_VALIDATION_LAYERS = true;
#endif