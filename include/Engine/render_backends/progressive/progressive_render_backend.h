#pragma once

#include "Engine/render_backends/render_backend.h"
#include "Engine/render_backends/progressive/virtual_device.h"
#include "Engine/render_backends/progressive/constants.h"
#include <vulkan/vulkan.hpp>
#include <map>
#include <memory>
#include <unordered_map>

using std::map;
using std::unordered_map;
using std::string;


// --- ProgressiveRenderBackend --- 
// This is the most cutting edge rendering backend in the engine.
// It has the newest features and the worst cross platform support.
// It supports Windows, Linux, and maybe Android.
// --------------------------------

class ProgressiveRenderBackend : public RenderBackend {

public:

/////////////////////
///// FUNCTIONS /////
/////////////////////

// ==== Class Functions ====
// These include things like constructors, destructors and operators.
// ---

	ProgressiveRenderBackend(
		ApplicationConfig* application_config,
		Logger* logger,
		Uint32 sdl_only_window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN
	);
	/// sdl_window_flags must be set in every constructor to include `SDL_WINDOW_VULKAN`

//////////////////////
///// ATTRIBUTES /////
//////////////////////

private:

/////////////////////
///// FUNCTIONS /////
/////////////////////

// === Game Loop Functions ===

	void before_start_window(string window_title, int window_width, int window_height) override;

	void before_game_loop() override;

	void after_game_loop() override;

	void update_game() override;

// ==== Render Functions ====
// Overload these functions depending on the render engine	
// ---


	

	// = Initialization / De-Initialization =
	// ---

	bool initialize_vulkan();

	/// returns false on failure
	bool vk_create_instance();

	vector<const char*> vk_get_required_extensions();

	bool vk_cleanup();

	bool vk_create_surface();

	bool vk_create_virtual_devices();

	// = Validation = 
	// ---

	bool vk_check_validation_layer_support();
	/// checks that all of the validation layers are supported

	vk::DebugUtilsMessengerCreateInfoEXT vk_create_debug_messenger_create_info();

	bool vk_setup_debug_messenger();

	static VKAPI_ATTR vk::Bool32 VKAPI_CALL vk_handle_debug_messages(
		vk::DebugUtilsMessageSeverityFlagBitsEXT message_severity,
		vk::DebugUtilsMessageTypeFlagsEXT message_type,
		const vk::DebugUtilsMessengerCallbackDataEXT* p_callback_data,
		void* logger_void_pointer // The logger instance is passed here.
	);
	/// This is a vulkan callback function for handling validation layer / debug messages

	void vk_bind_to_window(SDL_Window*);

//////////////////////
///// ATTRIBUTES /////
//////////////////////

// === VULKAN ===

	// required vulkan attributes
	vk::Instance vk_instance;
	vk::SurfaceKHR vk_surface;
	// add support for multiple different physical devices.
	// that way later we can support switching devices.
	// the keys of this map are the physical device ids
	map<uint32_t, vk::PhysicalDevice> vk_physical_device_map;

	vector<std::shared_ptr<VirtualDevice>> virtual_devices;
	std::multimap<uint64_t, std::shared_ptr<VirtualDevice>> virtual_device_priority_map;

	std::shared_ptr<VirtualDevice> virtual_device;

	vk::Queue vk_queue;
	vk::DebugUtilsMessengerEXT vk_debug_messenger;

};

