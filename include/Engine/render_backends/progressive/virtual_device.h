#pragma once
#include "Engine/render_backends/progressive/constants.h"
#include <vulkan/vulkan.hpp>
#include <optional>

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
	VirtualDevice(vk::PhysicalDevice vk_physical_device, vk::SurfaceKHR* vk_surface);

	void clean_up();

	vk::Device get_vulkan_device();
	uint64_t get_suitability() const;
	
	static bool check_physical_device_is_suitable(vk::PhysicalDevice vk_physical_device, const vk::SurfaceKHR& vk_surface);

	QueueFamilyIndices queue_family_indices;
	DeviceQueues queues;

private:

	static bool check_device_extension_support(vk::PhysicalDevice vk_physical_device);

	uint64_t vk_measure_physical_device_suitability();
	void vk_create_logical_device();

	


	vk::Device vk_device;
	vk::PhysicalDevice vk_physical_device;
	uint64_t suitability;
	vk::SurfaceKHR* vk_surface;
};