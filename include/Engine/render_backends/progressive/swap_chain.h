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
	bool GetSurfaceFormats();

private:
	vk::PhysicalDevice* vk_physical_device;

	std::vector<vk::SurfaceFormatKHR> formats;
	vk::SurfaceKHR* vk_surface;


	SwapChainSupportDetails support_details;
};
