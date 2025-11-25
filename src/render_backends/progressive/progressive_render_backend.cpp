#include "Engine/render_backends/progressive/progressive_render_backend.h"
#include "Engine/constants.h"
#include <SDL2/SDL_vulkan.h>
#include <iostream>
#include <stdexcept>

// ==== Class Functions ====

ProgressiveRenderBackend::ProgressiveRenderBackend(
	string application_name, string application_description,
	vector<string> application_authors, int application_version_major,
	int application_version_minor, int application_version_patch,
	string application_version_identifier, Uint32 sdl_only_window_flags
)
	: RenderBackend(
		application_name, application_description, application_authors,
		application_version_major, application_version_minor,
		application_version_patch, application_version_identifier
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
	this->initialize_vulkan();
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

void ProgressiveRenderBackend::initialize_vulkan() {
	if (!this->vk_create_instance()) {
		throw std::runtime_error("Failed to create vulkan instance.");
	}
}

bool ProgressiveRenderBackend::vk_cleanup() {
	this->vk_instance.destroy();
	return true;
}

bool ProgressiveRenderBackend::vk_create_instance() {
	// Set vulkan ApplicationInfo
	vk::ApplicationInfo vk_ApplicationInfo(
		this->application_name.c_str(),
		VK_MAKE_VERSION(
			this->application_version_major, 
			this->application_version_minor, 
			this->application_version_patch
		),
		ENGINE_NAME,
		VK_MAKE_VERSION(ENGINE_VERSION_MAJOR, ENGINE_VERSION_MINOR, ENGINE_VERSION_PATCH),
		VK_API_VERSION_1_0
	);

	// Get sdl extensions
	uint32_t sdl_ExtensionCount = 0;

	if (!SDL_Vulkan_GetInstanceExtensions(this->sdl_window, &sdl_ExtensionCount, nullptr)) {
		throw std::runtime_error("Failed to get number of SDL Vulkan instance extensions.");
		return false;
	}
	
	std::vector<const char*> sdl_Extensions(sdl_ExtensionCount);

	if (!SDL_Vulkan_GetInstanceExtensions(this->sdl_window, &sdl_ExtensionCount, sdl_Extensions.data())) {
		throw std::runtime_error("Failed to get SDL Vulkan instance extensions.");
		return false;
	}

	// Construct InstanceCreateInfo

	vk::InstanceCreateFlags vk_InstanceCreateFlags{};

	vk::InstanceCreateInfo vk_InstanceCreateInfo(
		vk_InstanceCreateFlags,
		&vk_ApplicationInfo,
		{},
		sdl_Extensions
	);

	// create the vulkan instance

	if (vk::createInstance(&vk_InstanceCreateInfo, nullptr, &this->vk_instance) != vk::Result::eSuccess) {
		throw std::runtime_error("Failed to create Vulkan instance.");
		return false;
	}

	//find a physical device for vulkan;

	uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(this->vk_instance, &deviceCount, nullptr);

    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    vkEnumeratePhysicalDevices(this->vk_instance, &deviceCount, physicalDevices.data());

    // Iterate through physicalDevices to find a suitable one
    this->vk_physical_device = physicalDevices[0];

	return true;
}
