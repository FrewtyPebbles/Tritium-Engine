#include "Engine/render_backends/progressive/swap_chain.h"
#include "Engine/render_backends/progressive/constants.h"

SwapChain::SwapChain(vk::PhysicalDevice* vk_physical_device, vk::SurfaceKHR* vk_surface)
: vk_physical_device(vk_physical_device), vk_surface(vk_surface), support_details(*vk_physical_device, *vk_surface) {}


SwapChainSupportDetails::SwapChainSupportDetails(const vk::PhysicalDevice& vk_physical_device, const vk::SurfaceKHR& vk_surface) {
	vk_surface_capabilities = vk_physical_device.getSurfaceCapabilitiesKHR(vk_surface);
	vk_surface_formats = vk_physical_device.getSurfaceFormatsKHR(vk_surface);
	vk_present_modes = vk_physical_device.getSurfacePresentModesKHR(vk_surface);
}

bool SwapChain::GetSurfaceFormats()
{
	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	if (formatCount != 0) {
		formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(
			device,
			surface,
			&formatCount,
			formats.data()
		);
	} else {
		return false;
	}

	return true;
}
