#include "Engine/render_backends/progressive/progressive_render_backend.h"
#include "Engine/constants.h"
#include "Engine/render_backends/progressive/extensions.h"
#include <SDL2/SDL_vulkan.h>
#include <iostream>
#include <sstream>
#include <stdexcept>

// ==== Class Functions ====

ProgressiveRenderBackend::ProgressiveRenderBackend(
	ApplicationConfig* application_config,
	Logger* logger,
	Uint32 sdl_only_window_flags
)
	: RenderBackend(
		application_config,
		logger
	)
{
	// sdl_window_flags must be set in every constructor to include `SDL_WINDOW_VULKAN`
	this->sdl_window_flags = SDL_WINDOW_VULKAN | sdl_only_window_flags;
}

// === Game Loop Hooks ===

void ProgressiveRenderBackend::before_start_window(string window_title, int window_width, int window_height) {
	// This is before SDL is initialized
}

void ProgressiveRenderBackend::before_game_loop() {
	// This is where vulkan is initialized.
	if (!this->initialize_vulkan()) {
		throw std::runtime_error("Failed to initialize vulkan.");
	}
}

void ProgressiveRenderBackend::after_game_loop() {
	// This is where vulkan is de-initialized.
	if (!this->vk_cleanup()) {
		throw std::runtime_error("Failed to clean up vulkan.");
	}
}

void ProgressiveRenderBackend::update_game() {
	// This is where the screen is updated with the vulkan surface.
}

bool ProgressiveRenderBackend::initialize_vulkan() {
	if (!this->vk_create_instance()) {
		throw std::runtime_error("Failed to create vulkan instance.");
	}

	if (!this->vk_setup_debug_messenger()) {
		throw std::runtime_error("Failed to set up debug messenger.");
	}

	if (!this->vk_create_surface()) {
		throw std::runtime_error("Failed to create window surface.");
	}

	if (!this->vk_create_virtual_devices()) {
		throw std::runtime_error("Failed to create virtual devices.");
	}

	if (!this->vk_create_graphics_pipelines()) {
		throw std::runtime_error("Failed to create graphics pipelines.");
	}
}

bool ProgressiveRenderBackend::vk_cleanup() {
	if (vkENABLE_VALIDATION_LAYERS) {
		VK_Extension::destroy_debug_utils_messenger_ext(this->vk_instance, this->vk_debug_messenger, nullptr);
	}
	for (const auto& virtualDevice : this->virtual_devices) {
		virtualDevice->clean_up();
	}
	this->vk_instance.destroySurfaceKHR(this->vk_surface);
	this->vk_instance.destroy();
	return true;
}

bool ProgressiveRenderBackend::vk_create_instance() {
	if (vkENABLE_VALIDATION_LAYERS && !this->vk_check_validation_layer_support()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}

	// Set vulkan ApplicationInfo
	vk::ApplicationInfo vk_ApplicationInfo(
		this->application_config->application_name.c_str(),
		VK_MAKE_VERSION(
			this->application_config->application_version_major,
			this->application_config->application_version_minor,
			this->application_config->application_version_patch
		),
		ENGINE_NAME,
		VK_MAKE_VERSION(ENGINE_VERSION_MAJOR, ENGINE_VERSION_MINOR, ENGINE_VERSION_PATCH),
		VK_API_VERSION_1_0
	);

	// Get extensions
	vector<const char*> extensions = this->vk_get_required_extensions();

	// Construct InstanceCreateInfo

	vk::DebugUtilsMessengerCreateInfoEXT vk_DebugMessengerCreateInfo;
	/// This is a debug messenger taht will let us see validation layer messages
	// for the vulkan instance creation vulkan functions.

	vk::InstanceCreateFlags vk_InstanceCreateFlags{};

	vk::InstanceCreateInfo vk_InstanceCreateInfo;

	if (vkENABLE_VALIDATION_LAYERS) {
		vk_DebugMessengerCreateInfo = this->vk_create_debug_messenger_create_info();
		
		vk_InstanceCreateInfo = vk::InstanceCreateInfo(
			vk_InstanceCreateFlags,
			&vk_ApplicationInfo,
			vkVALIDATION_LAYERS,
			extensions,
			(vk::DebugUtilsMessengerCreateInfoEXT*)&vk_DebugMessengerCreateInfo
		);
	} else {
		vk_InstanceCreateInfo = vk::InstanceCreateInfo(
			vk_InstanceCreateFlags,
			&vk_ApplicationInfo,
			{},
			extensions
		);
	}

	// create the vulkan instance

	if (vk::createInstance(&vk_InstanceCreateInfo, nullptr, &this->vk_instance) != vk::Result::eSuccess) {
		throw std::runtime_error("Failed to create Vulkan instance.");
		return false;
	}

	return true;
}

bool ProgressiveRenderBackend::vk_create_surface() {
	if (!SDL_Vulkan_CreateSurface(this->sdl_window, static_cast<VkInstance>(this->vk_instance), reinterpret_cast<VkSurfaceKHR*>(&vk_surface))) {
		throw std::runtime_error("Failed to create window surface.");
		return false;
	}
	return true;
}

vector<const char*> ProgressiveRenderBackend::vk_get_required_extensions() {
	// Get sdl extensions
	uint32_t sdl_ExtensionCount = 0;

	if (!SDL_Vulkan_GetInstanceExtensions(this->sdl_window, &sdl_ExtensionCount, nullptr)) {
		throw std::runtime_error("Failed to get number of SDL Vulkan instance extensions.");
	}

	vector<const char*> sdl_Extensions(sdl_ExtensionCount);

	if (!SDL_Vulkan_GetInstanceExtensions(this->sdl_window, &sdl_ExtensionCount, sdl_Extensions.data())) {
		throw std::runtime_error("Failed to get SDL Vulkan instance extensions.");
	}

	vector<const char*> extensions(sdl_Extensions);

	if (vkENABLE_VALIDATION_LAYERS) {
		extensions.push_back(vk::EXTDebugUtilsExtensionName);
	}

	return extensions;
}

bool ProgressiveRenderBackend::vk_create_virtual_devices() {
	std::vector<vk::PhysicalDevice> physicalDevices =
		this->vk_instance.enumeratePhysicalDevices();

	if (physicalDevices.size() == 0) {
		throw std::runtime_error("No GPUS with vulkan support.");
		return false;
	}

	// Create a map of all suitable devices:

	for (const auto& physicalDevice : physicalDevices) {
		if (VirtualDevice::check_physical_device_is_suitable(physicalDevice, this->vk_surface)) {
			auto properties = physicalDevice.getProperties();
			this->vk_physical_device_map.insert(std::make_pair(properties.deviceID, physicalDevice));
		}
	}

	if (this->vk_physical_device_map.size() == 0) {
		throw std::runtime_error("Failed to find a supported GPU.");
		return false;
	}

	for (auto const& [physicalDeviceID, physicalDevice] : this->vk_physical_device_map) {
		auto virtualDevice = std::make_shared<VirtualDevice>(this->application_config, this->sdl_window, physicalDevice, &this->vk_surface);
		this->virtual_devices.push_back(virtualDevice);
		this->virtual_device_priority_map.insert(std::make_pair(virtualDevice->get_suitability(), virtualDevice));
	}

	this->virtual_device = this->virtual_device_priority_map.rbegin()->second;
}

bool ProgressiveRenderBackend::vk_create_graphics_pipelines() {
	// TODO: Create a different graphics pipeline for each scenario
	
	
	// Build the render pipeline:
	// this renders graphics onto the screen.
	this->graphics_pipeline_map.insert(std::make_pair(
		"render",
		GraphicsPipeline::Builder(this->virtual_device->get_vulkan_device())
		.add_stage(
			"shaders/.testing/vert.spv", // for now we are just using .testing since it is gitignored
			"main",
			vk::ShaderStageFlagBits::eVertex
		)->add_stage(
			"shaders/.testing/frag.spv", // for now we are just using .testing since it is gitignored
			"main",
			vk::ShaderStageFlagBits::eFragment
		)->add_dynamic_state(vk::DynamicState::eViewport)
		->add_dynamic_state(vk::DynamicState::eScissor)
		/*->add_dynamic_state(vk::DynamicState::eDepthBias)
		->add_dynamic_state(vk::DynamicState::eBlendConstants)
		->add_dynamic_state(vk::DynamicState::eStencilReference)
		->add_dynamic_state(vk::DynamicState::eStencilCompareMask)
		->add_dynamic_state(vk::DynamicState::eStencilWriteMask)
		->add_dynamic_state(vk::DynamicState::eCullMode)
		->add_dynamic_state(vk::DynamicState::eFrontFace)
		->add_dynamic_state(vk::DynamicState::eDepthTestEnable)
		->add_dynamic_state(vk::DynamicState::eDepthWriteEnable)
		->add_dynamic_state(vk::DynamicState::eDepthCompareOp)*/
		->set_primitive_topology(vk::PrimitiveTopology::eTriangleList)
		->set_primitive_restart(false)
		->build()
	));

	return true;
}


bool ProgressiveRenderBackend::vk_check_validation_layer_support() {
	uint32_t layerCount;
	vk::enumerateInstanceLayerProperties(&layerCount, nullptr);

	vector<vk::LayerProperties> availableLayers(layerCount);

	vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : vkVALIDATION_LAYERS) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}
		if (!layerFound) {
			return false;
		}
	}

	return true;
}

vk::DebugUtilsMessengerCreateInfoEXT ProgressiveRenderBackend::vk_create_debug_messenger_create_info() {
	vk::DebugUtilsMessengerCreateFlagsEXT vk_flags{};

	return vk::DebugUtilsMessengerCreateInfoEXT(
		vk_flags,
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
		vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
		vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
		vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
		this->vk_handle_debug_messages,
		this->logger
	);
}

bool ProgressiveRenderBackend::vk_setup_debug_messenger() {
	if (!vkENABLE_VALIDATION_LAYERS)
		return true;

	auto vk_DebugUtilsMessangerCreateInfoEXT = this->vk_create_debug_messenger_create_info();

	if (VK_Extension::create_debug_utils_messenger_ext(this->vk_instance, &vk_DebugUtilsMessangerCreateInfoEXT, nullptr, &this->vk_debug_messenger) != vk::Result::eSuccess) {
		throw std::runtime_error("failed to set up debug messenger!");
		return false;
	}

	return true;
}

VKAPI_ATTR vk::Bool32 VKAPI_CALL ProgressiveRenderBackend::vk_handle_debug_messages(
	vk::DebugUtilsMessageSeverityFlagBitsEXT message_severity,
	vk::DebugUtilsMessageTypeFlagsEXT message_type,
	const vk::DebugUtilsMessengerCallbackDataEXT* p_callback_data,
	void* logger_void_pointer
) {
	std::stringstream validationLayerMessage;
	validationLayerMessage << "VULKAN VALIDATION LAYER: " << p_callback_data->pMessage;

	if (logger_void_pointer != nullptr) {
		Logger* logger = static_cast<Logger*>(logger_void_pointer);

		Log::Severity logSeverity;

		switch (message_severity) {
		case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
			logSeverity = Log::Severity::INFO;
			break;

		case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
			logSeverity = Log::Severity::WARNING;
			break;

		case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
			logSeverity = Log::Severity::ERROR;
			break;

		case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
			logSeverity = Log::Severity::VERBOSE;
			break;
		}

		logger->log(validationLayerMessage.str(), "rendering", Log::Domain::RENDERING, logSeverity);
		return vk::False;
	}
}