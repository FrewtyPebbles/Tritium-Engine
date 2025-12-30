#pragma once
#include "Engine/render_backends/progressive/constants.h"
#include "Engine/render_backends/progressive/swap_chain.h"
#include "Engine/state/application_config.h"
#include <vulkan/vulkan.hpp>
#include <optional>
#include <memory>

struct QueueFamilyIndices {

	QueueFamilyIndices(const vk::PhysicalDevice& vk_physical_device, const vk::SurfaceKHR& vk_surface);
	QueueFamilyIndices() = default;

	std::optional<uint32_t> graphics;
	std::optional<uint32_t> present;

	bool has_required() const;
	bool is_complete() const;
};

struct DeviceQueues {
	vk::Queue graphics;
	vk::Queue present;
};

class VirtualDevice {
public:
	VirtualDevice(ApplicationConfig* application_config, SDL_Window* sdl_window, vk::PhysicalDevice vk_physical_device, vk::SurfaceKHR* vk_surface,
		// Make this setting something setable by the user:
		vk::PresentModeKHR prefered_present_mode = vk::PresentModeKHR::eMailbox
	);

	void clean_up();

	vk::Device* get_vulkan_device();
	uint64_t get_suitability() const;
	
	static bool check_physical_device_is_suitable(vk::PhysicalDevice vk_physical_device, const vk::SurfaceKHR& vk_surface);

	
	DeviceQueues queues;

private:

	static bool check_device_extension_support(vk::PhysicalDevice vk_physical_device);

	uint64_t vk_measure_physical_device_suitability();

	void vk_create_logical_device(QueueFamilyIndices queue_family_indices);

	// This is used to ensure we only use the device features we need
	vk::PhysicalDeviceFeatures vk_get_device_features(QueueFamilyIndices queue_family_indices);


	SDL_Window* sdl_window;
	vk::Device vk_device;
	vk::PhysicalDevice vk_physical_device;
	uint64_t suitability;
	vk::SurfaceKHR* vk_surface;
	std::unique_ptr<SwapChain> swapchain;

	ApplicationConfig* application_config;
};