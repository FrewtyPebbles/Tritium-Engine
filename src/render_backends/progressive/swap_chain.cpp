#include "Engine/render_backends/progressive/swap_chain.h"
#include "Engine/render_backends/progressive/constants.h"
#include "Engine/render_backends/progressive/virtual_device.h"
#include <SDL2/SDL_vulkan.h>
#include <cstdint>
#include <limits>
#include <algorithm>
#include <array>

SwapChainSupportDetails::SwapChainSupportDetails(const vk::PhysicalDevice& vk_physical_device, const vk::SurfaceKHR& vk_surface) {
	vk_surface_capabilities = vk_physical_device.getSurfaceCapabilitiesKHR(vk_surface);
	vk_surface_formats = vk_physical_device.getSurfaceFormatsKHR(vk_surface);
	vk_present_modes = vk_physical_device.getSurfacePresentModesKHR(vk_surface);
}

SwapChain::SwapChain(SDL_Window* sdl_window, vk::PhysicalDevice* vk_physical_device, vk::Device* vk_device, vk::SurfaceKHR* vk_surface,
	vk::ImageUsageFlags image_usage_bits,
	// Settings:
	vk::PresentModeKHR setting_prefered_present_mode,
	bool setting_stereoscopic
)
: sdl_window(sdl_window), vk_physical_device(vk_physical_device), vk_device(vk_device), vk_surface(vk_surface) {
	SwapChainSupportDetails support_details = SwapChainSupportDetails(*vk_physical_device, *vk_surface);

	vk::SurfaceFormatKHR vkSurfaceFormat = choose_surface_format(support_details);
	vk::PresentModeKHR vkPresentMode = choose_present_mode(support_details, setting_prefered_present_mode);
	vk::Extent2D vkExtent = choose_swap_extent(support_details);
	
	// get the image count
	uint32_t imageCount = support_details.vk_surface_capabilities.minImageCount + 1;

	if (support_details.vk_surface_capabilities.maxImageCount > 0 &&
		imageCount > support_details.vk_surface_capabilities.maxImageCount) {
		imageCount = support_details.vk_surface_capabilities.maxImageCount;
	}

	// create info
	vk::SwapchainCreateInfoKHR createInfo;

	// set queue family indices

	QueueFamilyIndices queueFamilyIndicesStruct = QueueFamilyIndices(*this->vk_physical_device, *this->vk_surface);
	std::array<uint32_t, 2> queueFamilyIndices = {
		queueFamilyIndicesStruct.graphics.value(),
		queueFamilyIndicesStruct.present.value()
	};

	if (queueFamilyIndicesStruct.graphics != queueFamilyIndicesStruct.present) {
		createInfo = vk::SwapchainCreateInfoKHR(
			{},
			*this->vk_surface,
			imageCount,
			vkSurfaceFormat.format,
			vkSurfaceFormat.colorSpace,
			vkExtent,
			setting_stereoscopic ? 2 : 1,
			image_usage_bits,
			vk::SharingMode::eConcurrent,
			queueFamilyIndices,
			support_details.vk_surface_capabilities.currentTransform, // pretransform (might be useful later if we need to cheaply transform images.)
			vk::CompositeAlphaFlagBitsKHR::eOpaque,
			vkPresentMode,
			vk::True,
			VK_NULL_HANDLE
			// TODO: ^ make ptr to old swap chain incase window is resized or something invalidating or deoptimizing
			// the swapchain and the swapchain needs to be recreated.
		);
	}
	else {
		createInfo = vk::SwapchainCreateInfoKHR(
			{},
			*this->vk_surface,
			imageCount,
			vkSurfaceFormat.format,
			vkSurfaceFormat.colorSpace,
			vkExtent,
			setting_stereoscopic ? 2 : 1,
			image_usage_bits,
			vk::SharingMode::eExclusive,
			nullptr, // queue family indices
			support_details.vk_surface_capabilities.currentTransform, // pretransform (might be useful later if we need to cheaply transform images.)
			vk::CompositeAlphaFlagBitsKHR::eOpaque,
			vkPresentMode,
			vk::True,
			VK_NULL_HANDLE
			// TODO: ^ make ptr to old swap chain incase window is resized or something invalidating or deoptimizing
			// the swapchain and the swapchain needs to be recreated.
		);
	}

	this->vk_swapchain = this->vk_device->createSwapchainKHR(createInfo);
}

vk::SurfaceFormatKHR SwapChain::choose_surface_format(const SwapChainSupportDetails& support_details) {
	for (const auto& availableFormat : support_details.vk_surface_formats) {
		if (availableFormat.format == vk::Format::eB8G8R8A8Srgb
		&& availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
			return availableFormat;
		}
		// Rank scoring might happen here.
	}

	// TODO: rank and pick surface formats based on how "good" they are
	// but for now we will just pick the first one that pops up:
	return support_details.vk_surface_formats[0];
}

vk::PresentModeKHR SwapChain::choose_present_mode(const SwapChainSupportDetails& support_details, vk::PresentModeKHR prefered) {
	for (const auto& availablePresentMode : support_details.vk_present_modes) {
		if (availablePresentMode == prefered) {
			return availablePresentMode;
		}
	}

	// This mode is always available on all hardware so we use it as a default fallback:
	return vk::PresentModeKHR::eFifo;
}

vk::Extent2D SwapChain::choose_swap_extent(const SwapChainSupportDetails& support_details) {
	if (support_details.vk_surface_capabilities.currentExtent.width != std::numeric_limits<uint16_t>::max()) {
		return support_details.vk_surface_capabilities.currentExtent;
	} else {
		int width, height;
		SDL_Vulkan_GetDrawableSize(this->sdl_window, &width, &height);

		vk::Extent2D actualExtent = vk::Extent2D(
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		);

		actualExtent.width = std::clamp(actualExtent.width,
			support_details.vk_surface_capabilities.minImageExtent.width,
			support_details.vk_surface_capabilities.maxImageExtent.width
		);
		actualExtent.height = std::clamp(actualExtent.height,
			support_details.vk_surface_capabilities.minImageExtent.height,
			support_details.vk_surface_capabilities.maxImageExtent.height
		);

		return actualExtent;
	}
}

void SwapChain::clean_up() {
	this->vk_device->destroySwapchainKHR(this->vk_swapchain);
}