#pragma once
#include "Engine/render_backends/progressive/constants.h"
#include <vulkan/vulkan.hpp>
#include <optional>

struct QueueFamilyIndices {

	QueueFamilyIndices(const vk::PhysicalDevice& vk_physical_device);
	QueueFamilyIndices() = default;

	std::optional<uint32_t> graphics_family;

	bool has_required() const;
	bool is_complete() const;
};

class VirtualDevice {
public:
	VirtualDevice(vk::PhysicalDevice vk_physical_device);

	void clean_up();

	vk::Device get_vulkan_device();
	uint64_t get_suitability() const;
	
	static bool check_physical_device_is_suitable(vk::PhysicalDevice physical_device);

	QueueFamilyIndices queue_family_indices;

private:

	uint64_t vk_measure_physical_device_suitability();
	void vk_create_logical_device();


	vk::Device vk_device;
	vk::PhysicalDevice vk_physical_device;
	uint64_t suitability;
};