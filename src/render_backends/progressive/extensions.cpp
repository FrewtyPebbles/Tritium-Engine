#include "Engine/render_backends/progressive/extensions.h"

namespace VK_Extension {

	vk::Result create_debug_utils_messenger_ext(
		vk::Instance vk_instance,
		const vk::DebugUtilsMessengerCreateInfoEXT* p_vk_create_info,
		const vk::AllocationCallbacks* p_vk_allocator,
		vk::DebugUtilsMessengerEXT* p_vk_debug_messenger
	) {
		auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(vk_instance,
			"vkCreateDebugUtilsMessengerEXT"
		));

		if (!func) {
			return vk::Result::eErrorExtensionNotPresent;
		}


		VkDebugUtilsMessengerEXT vk_RawMessenger{};

		VkResult result = func(
			static_cast<VkInstance>(vk_instance),
			reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(p_vk_create_info),
			reinterpret_cast<const VkAllocationCallbacks*>(p_vk_allocator),
			&vk_RawMessenger
		);

		*p_vk_debug_messenger = vk::DebugUtilsMessengerEXT(vk_RawMessenger);

		return static_cast<vk::Result>(result);
	}

	vk::Result destroy_debug_utils_messenger_ext(
		vk::Instance vk_instance,
		vk::DebugUtilsMessengerEXT vk_debug_messenger,
		const vk::AllocationCallbacks* p_vk_allocator
	) {
		// Load the raw function pointer
		auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(static_cast<VkInstance>(vk_instance),
			"vkDestroyDebugUtilsMessengerEXT"
		));

		if (!func) {
			return vk::Result::eErrorExtensionNotPresent;
		}

		// Convert to raw Vulkan types and call
		func(
			static_cast<VkInstance>(vk_instance),
			static_cast<VkDebugUtilsMessengerEXT>(vk_debug_messenger),
			reinterpret_cast<const VkAllocationCallbacks*>(p_vk_allocator)
		);

		return vk::Result::eSuccess;
	}

}
