#pragma once
#include "Engine/state/application_config.h"
#include <vulkan/vulkan.hpp>
#include <vector>
#include <SDL2/SDL.h>
#include <unordered_map>
#include <string>

using std::vector;
using std::unordered_map;
using std::string;

struct SwapChainSupportDetails {
	SwapChainSupportDetails(const vk::PhysicalDevice& vk_physical_device, const vk::SurfaceKHR& vk_surface);
	vk::SurfaceCapabilitiesKHR vk_surface_capabilities;
	vector<vk::SurfaceFormatKHR> vk_surface_formats;
	vector<vk::PresentModeKHR> vk_present_modes;
};

class SwapChain {
public:
	SwapChain(ApplicationConfig* application_config, SDL_Window* sdl_window, vk::PhysicalDevice* vk_physical_device, vk::Device* vk_device, vk::SurfaceKHR* vk_surface,
		vk::ImageUsageFlags image_usage_bits,
		// This should be optionally user supplied:
		vk::PresentModeKHR setting_prefered_present_mode = vk::PresentModeKHR::eMailbox,
		bool setting_stereoscopic = false
	);

	void clean_up();

	// Creates an image view dynamically for a sampler2D or sampler2DArray or something.
	vk::ImageView create_image_view(string label, vk::ImageViewType type, size_t image_index);
	vk::ImageView& get_image_view(string label);

private:

	// FUNCTIONS

	vk::SurfaceFormatKHR choose_surface_format(const SwapChainSupportDetails& support_details);

	vk::PresentModeKHR choose_present_mode(const SwapChainSupportDetails& support_details, vk::PresentModeKHR prefered = vk::PresentModeKHR::eMailbox);

	vk::Extent2D choose_swap_extent(const SwapChainSupportDetails& support_details);

	void create_display_image_views();

	vk::ImageView create_image_view(vk::ImageViewType type, size_t image_index);


	// ATTRIBUTES

	SDL_Window* sdl_window;

	vk::PhysicalDevice* vk_physical_device;
	vk::Device* vk_device;
	vk::SurfaceKHR* vk_surface;

	vk::SwapchainKHR vk_swapchain;

	unordered_map<string, vk::ImageView> vk_image_view_map;
	vector<vk::ImageView> vk_display_image_views;
	vector<vk::Image> vk_images;
	vk::Format vk_image_format;
	vk::Extent2D vk_extent;
	
	ApplicationConfig* application_config;
};
