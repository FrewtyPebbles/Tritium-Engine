#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>
#include <SDL2/SDL.h>

using std::vector;

struct SwapChainSupportDetails {
	SwapChainSupportDetails(const vk::PhysicalDevice& vk_physical_device, const vk::SurfaceKHR& vk_surface);
	vk::SurfaceCapabilitiesKHR vk_surface_capabilities;
	vector<vk::SurfaceFormatKHR> vk_surface_formats;
	vector<vk::PresentModeKHR> vk_present_modes;
};

class SwapChain {
public:
	SwapChain(SDL_Window* sdl_window, vk::PhysicalDevice* vk_physical_device, vk::Device* vk_device, vk::SurfaceKHR* vk_surface,
		vk::ImageUsageFlags image_usage_bits,
		// This should be optionally user supplied:
		vk::PresentModeKHR setting_prefered_present_mode = vk::PresentModeKHR::eMailbox,
		bool setting_stereoscopic = false
	);

	void clean_up();

private:

	// FUNCTIONS

	vk::SurfaceFormatKHR choose_surface_format(const SwapChainSupportDetails& support_details);

	vk::PresentModeKHR choose_present_mode(const SwapChainSupportDetails& support_details, vk::PresentModeKHR prefered = vk::PresentModeKHR::eMailbox);

	vk::Extent2D choose_swap_extent(const SwapChainSupportDetails& support_details);

	// ATTRIBUTES

	SDL_Window* sdl_window;

	vk::PhysicalDevice* vk_physical_device;
	vk::Device* vk_device;
	vk::SurfaceKHR* vk_surface;

	vk::SwapchainKHR vk_swapchain;
};
