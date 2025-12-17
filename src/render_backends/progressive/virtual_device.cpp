#include "Engine/render_backends/progressive/virtual_device.h"
#include <vector>
#include <stdexcept>
#include <set>

using std::vector;

QueueFamilyIndices::QueueFamilyIndices(const vk::PhysicalDevice& vk_physical_device, const vk::SurfaceKHR& vk_surface) {
	vector<vk::QueueFamilyProperties> queueFamiliesProperties = vk_physical_device.getQueueFamilyProperties();
	// populate struct with the supported queue family indexes
	int i = 0;
	for (const auto& queueFamilyProperties : queueFamiliesProperties) {
		if (queueFamilyProperties.queueFlags & vk::QueueFlags::BitsType::eGraphics) {
			this->graphics = i;
		}

		if (vk_physical_device.getSurfaceSupportKHR(i, vk_surface)) {
			this->present = i;
		}


		if (this->is_complete())
			break;

		i++;
	}


}

bool QueueFamilyIndices::has_required() const {
	return graphics.has_value() && present.has_value();
}

bool QueueFamilyIndices::is_complete() const {
	return graphics.has_value() && present.has_value();
}


VirtualDevice::VirtualDevice(vk::PhysicalDevice vk_physical_device, vk::SurfaceKHR* vk_surface) : vk_physical_device(vk_physical_device), suitability(0), vk_surface(vk_surface) {
	suitability = this->vk_measure_physical_device_suitability();

	// This populates the queue family indices
	this->queue_family_indices = QueueFamilyIndices(this->vk_physical_device, *vk_surface);

	// create the vk::Device
	this->vk_create_logical_device();
}

void VirtualDevice::clean_up() {
	this->vk_device.destroy();
}

void VirtualDevice::vk_create_logical_device() {

	vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

	std::set<uint32_t> uniqueQueueFamilies = {
		this->queue_family_indices.graphics.value(),
		this->queue_family_indices.present.value()
	};

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		vk::DeviceQueueCreateInfo queueCreateInfo = vk::DeviceQueueCreateInfo(
			{},
			queueFamily,
			1,
			&queuePriority
		);
		queueCreateInfos.push_back(queueCreateInfo);
	}
	vk::PhysicalDeviceFeatures deviceFeatures = vk::PhysicalDeviceFeatures();

	vk::DeviceCreateInfo createInfo = vk::DeviceCreateInfo(
		{},
		queueCreateInfos,
		{},
		vkDEVICE_EXTENSIONS
	);

	createInfo.pEnabledFeatures = &deviceFeatures;

	// TODO: later make use of device specific extensions dynamically

	// legacy vulkan support
	if (vkENABLE_VALIDATION_LAYERS) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(vkVALIDATION_LAYERS.size());
		createInfo.ppEnabledLayerNames = vkVALIDATION_LAYERS.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	// Now create the logical device

	if (this->vk_physical_device.createDevice(&createInfo, nullptr, &this->vk_device) != vk::Result::eSuccess) {
		throw std::runtime_error("Failed to create logical device!");
	}

	// Now get the queue handles

	this->vk_device.getQueue(this->queue_family_indices.graphics.value(), 0, &this->queues.graphics);
	this->vk_device.getQueue(this->queue_family_indices.present.value(), 0, &this->queues.present);
	
}

bool VirtualDevice::check_device_extension_support(vk::PhysicalDevice vk_physical_device) {
	vector<vk::ExtensionProperties> availableExtensions =
		vk_physical_device.enumerateDeviceExtensionProperties();
	std::set<std::string> requiredExtensions(vkDEVICE_EXTENSIONS.begin(), vkDEVICE_EXTENSIONS.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}


vk::Device VirtualDevice::get_vulkan_device() {
	return this->vk_device;
}

uint64_t VirtualDevice::vk_measure_physical_device_suitability() {
	uint64_t score = 0;
	vk::PhysicalDeviceProperties physicalDeviceProperties = this->vk_physical_device.getProperties();
	vk::PhysicalDeviceFeatures physicalDeviceFeatures = this->vk_physical_device.getFeatures();

	if (physicalDeviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
		score += 1000;
	}

	if (physicalDeviceProperties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu) {
		score += 500;
	}

	score += physicalDeviceProperties.limits.maxImageDimension1D;
	score += physicalDeviceProperties.limits.maxImageDimension2D;
	score += physicalDeviceProperties.limits.maxImageDimension3D;
	score += physicalDeviceProperties.limits.maxImageDimensionCube;
	score += physicalDeviceProperties.limits.maxComputeSharedMemorySize;
	for (auto s : physicalDeviceProperties.limits.maxComputeWorkGroupCount)
		score += s;
	for (auto s : physicalDeviceProperties.limits.maxComputeWorkGroupSize)
		score += s;
	score += physicalDeviceProperties.limits.maxComputeWorkGroupInvocations;
	score += physicalDeviceProperties.limits.maxFramebufferLayers;
	score += physicalDeviceProperties.limits.maxMemoryAllocationCount;


	return score;
}

bool VirtualDevice::check_physical_device_is_suitable(vk::PhysicalDevice vk_physical_device, const vk::SurfaceKHR& vk_surface) {
	QueueFamilyIndices queue_family_indices = QueueFamilyIndices(vk_physical_device, vk_surface);
	vk::PhysicalDeviceProperties physicalDeviceProperties = vk_physical_device.getProperties();
	vk::PhysicalDeviceFeatures physicalDeviceFeatures = vk_physical_device.getFeatures();

	bool extensionsSupported = VirtualDevice::check_device_extension_support(vk_physical_device);

	// This is where we describe the hardware requirements, later we may set these dynamically
	// based on what features of the engine the game is using such as if the game is using geometry shaders or not.
	return (physicalDeviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu ||
		physicalDeviceProperties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu ||
		physicalDeviceProperties.deviceType == vk::PhysicalDeviceType::eCpu ||
		physicalDeviceProperties.deviceType == vk::PhysicalDeviceType::eVirtualGpu)
		&& (queue_family_indices.has_required()) && extensionsSupported;
}

uint64_t VirtualDevice::get_suitability() const {
	return suitability;
}
