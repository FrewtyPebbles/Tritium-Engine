#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>

using std::vector;

struct SwapChainSupportDetails {
	SwapChainSupportDetails(const vk::PhysicalDevice& vk_physical_device, const vk::SurfaceKHR& vk_surface);
	vk::SurfaceCapabilitiesKHR vk_surface_capabilities;
	vector<vk::SurfaceFormatKHR> vk_surface_formats;
	vector<vk::PresentModeKHR> vk_present_modes;
};

class SwapChain {
public:
	SwapChain(vk::PhysicalDevice* vk_physical_device, vk::SurfaceKHR* vk_surface);

private:

	// FUNCTIONS

	void choose_surface_format();

	// ATTRIBUTES

	vk::PhysicalDevice* vk_physical_device;
	vk::SurfaceKHR* vk_surface;

	vk::SurfaceFormatKHR vk_surface_format;
	vk::SurfaceFormatKHR vk_surface_format;

	SwapChainSupportDetails support_details;
};
