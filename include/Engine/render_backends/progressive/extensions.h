#pragma once
#include <vulkan/vulkan.hpp>

namespace VK_Extension {

	vk::Result create_debug_utils_messenger_ext(
		vk::Instance instance,
		const vk::DebugUtilsMessengerCreateInfoEXT* p_create_info,
		const vk::AllocationCallbacks* p_allocator,
		vk::DebugUtilsMessengerEXT* p_debug_messenger
	);

	vk::Result destroy_debug_utils_messenger_ext(
		vk::Instance vk_instance,
		vk::DebugUtilsMessengerEXT vk_debug_messenger,
		const vk::AllocationCallbacks* p_vk_allocator
	);

};
