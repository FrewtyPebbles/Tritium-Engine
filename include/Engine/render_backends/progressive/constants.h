#pragma once
#include <vector>

using std::vector;

const vector<const char*> vkVALIDATION_LAYERS = {
	"VK_LAYER_KHRONOS_validation"
};


#ifdef NDEBUG
const bool vkENABLE_VALIDATION_LAYERS = false;
#else
const bool vkENABLE_VALIDATION_LAYERS = true;
#endif